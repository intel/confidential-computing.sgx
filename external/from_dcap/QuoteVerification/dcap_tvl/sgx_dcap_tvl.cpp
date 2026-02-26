/*
 * Copyright (C) 2011-2021 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

 /*
 * SGX DCAP trusted verification library (sgx_dcap_tvl)
 * App enclave can link this library to verify QvE report and identity
 */


#include "sgx_dcap_tvl.h"
#include "encode_helper.h"
#include "sgx_utils.h"
#include "sgx_tcrypto.h"
#include "sgx_trts.h"
#include "sgx_quote_4.h"
#include "sgx_dcap_constant_val.h"

#ifndef QE_QUOTE_VERSION
#define QE_QUOTE_VERSION        4  ///< Version of the quote structure that supports ECDSA (and EPID).  It is a generic form of the Quote.
#endif

#define SGX_ERR_BREAK(x) {if (x != SGX_SUCCESS) break;}

quote3_error_t enclave_identity_verify(
    int16_t prodid,
    uint16_t isv_svn,
    const sgx_report_t *p_report,
    sgx_isv_svn_t isvsvn_threshold,
    bool qae_mode
)
{
    quote3_error_t ret = TEE_ERROR_UNEXPECTED;
    do {
        try {
            //Convert hardcode MiscSelect, MiscSelect Mask to uint32
            //
            const auto miscselectMask = BytesToUint32(hexStringToBytes(QAE_QVE_MISC_SELECT_MASK));
            const auto miscselect = BytesToUint32(hexStringToBytes(QAE_QVE_MISC_SELECT));

            //Check MiscSelect in QaE/QvE report, QaE/QvE share the same misselect info
            //
            if ((p_report->body.misc_select & miscselectMask) != miscselect) {
                 ret = (qae_mode ? TEE_QAEIDENTITY_MISMATCH : TEE_QVEIDENTITY_MISMATCH);
                 break;
            }


            //Convert hardcode Attribute, Attribute Mask to hex array
            //
            const auto attribute_mask = hexStringToBytes(QAE_QVE_ATTRIBUTE_MASK);
            const auto attribute = hexStringToBytes(QAE_QVE_ATTRIBUTE);

            //Check Attribute in QaE/QvE report
            //
            Bytes attributeReport;

            std::copy((uint8_t*) const_cast<sgx_attributes_t*> (&p_report->body.attributes),
                (uint8_t*) const_cast<sgx_attributes_t*> (&p_report->body.attributes) + sizeof(sgx_attributes_t),
                std::back_inserter(attributeReport));

            if (applyMask(attributeReport, attribute_mask) != attribute) {
                 ret = (qae_mode ? TEE_QAEIDENTITY_MISMATCH : TEE_QVEIDENTITY_MISMATCH);
                 break;
            }

            //Convert MrSigner to hex array
            //
            const auto mrsigner = hexStringToBytes(QAE_QVE_MRSIGNER);

            //Check MrSigner in QaE/QvE report, QaE/QvE share the same mrsigner
            //
            Bytes mrsignerReport;
            std::copy(std::begin(p_report->body.mr_signer.m),
                    std::end(p_report->body.mr_signer.m),
                    std::back_inserter(mrsignerReport));

            if (mrsigner.empty() || mrsignerReport.empty()) {
                 break;
            }

            if (mrsigner != mrsignerReport) {
                 ret = (qae_mode ? TEE_QAEIDENTITY_MISMATCH : TEE_QVEIDENTITY_MISMATCH);
                 break;
            }

            //Check Prod ID in QaE/QvE report
            //
            if (p_report->body.isv_prod_id != prodid) {
                 ret = (qae_mode ? TEE_QAEIDENTITY_MISMATCH : TEE_QVEIDENTITY_MISMATCH);
                 break;
            }

            //Check QaE/QvE ISV SVN in QaE/QvE report meets the minimum requires SVN when the TVL was built.
            //
            if (p_report->body.isv_svn < isv_svn) {
                 ret = (qae_mode ? TEE_QAE_OUT_OF_DATE : TEE_QVE_OUT_OF_DATE);
                 break;
            }

            //Check if there has been a TCB Recovery on the QaE/QvE used to verify the report.
            //Warning: The function may return erroneous result if QaE/QvE ISV SVN has been modified maliciously.
            //
            if (p_report->body.isv_svn < isvsvn_threshold) {
                 ret = (qae_mode ? TEE_QAE_OUT_OF_DATE : TEE_QVE_OUT_OF_DATE);
                 break;
            }

            ret = TEE_SUCCESS;
        }

        catch (...) {
            break;
        }
    } while(0);

    return ret;
}

quote3_error_t sgx_tvl_verify_qve_report_and_identity(
        const uint8_t *p_quote,
        uint32_t quote_size,
        const sgx_ql_qe_report_info_t *p_qve_report_info,
        time_t expiration_check_date,
        uint32_t collateral_expiration_status,
        sgx_ql_qv_result_t quote_verification_result,
        const uint8_t *p_supplemental_data,
        uint32_t supplemental_data_size,
        sgx_isv_svn_t qve_isvsvn_threshold)
{
    sgx_status_t sgx_ret = SGX_ERROR_UNEXPECTED;
    quote3_error_t ret = SGX_QL_ERROR_UNEXPECTED;
    sgx_sha_state_handle_t sha_handle = NULL;
    sgx_report_data_t report_data = { 0 };


    if (p_quote == NULL ||
            p_qve_report_info == NULL ||
            sizeof(*p_qve_report_info) != sizeof(sgx_ql_qe_report_info_t) ||
            !sgx_is_within_enclave(p_quote, quote_size) ||
            !sgx_is_within_enclave(p_qve_report_info, sizeof(sgx_ql_qe_report_info_t)) ||
            (p_supplemental_data == NULL && supplemental_data_size != 0) ||
            (p_supplemental_data != NULL && supplemental_data_size == 0))
        return SGX_QL_ERROR_INVALID_PARAMETER;

    if (p_supplemental_data && supplemental_data_size > 0) {
        if (!sgx_is_within_enclave(p_supplemental_data, supplemental_data_size)) {
            return SGX_QL_ERROR_INVALID_PARAMETER;
        }
    }

    const sgx_report_t *p_qve_report = &(p_qve_report_info->qe_report);

    do {

        //verify QvE report
        sgx_ret = sgx_verify_report(p_qve_report);
        if (sgx_ret != SGX_SUCCESS) {
            ret = SGX_QL_ERROR_REPORT;
            break;
        }

        //verify QvE report data
        //report_data = SHA256([nonce || quote || expiration_check_date || expiration_status || verification_result || supplemental_data]) || 32 - 0x00
        //
        sgx_ret = sgx_sha256_init(&sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        //nonce
        //
        sgx_ret = sgx_sha256_update(reinterpret_cast<const uint8_t *>(&(p_qve_report_info->nonce)), sizeof(p_qve_report_info->nonce), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        //quote
        //
        sgx_ret = sgx_sha256_update(p_quote, quote_size, sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        //expiration_check_date
        //
        sgx_ret = sgx_sha256_update(reinterpret_cast<const uint8_t *>(&expiration_check_date), sizeof(expiration_check_date), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        //collateral_expiration_status
        //
        sgx_ret = sgx_sha256_update(reinterpret_cast<const uint8_t *>(&collateral_expiration_status), sizeof(collateral_expiration_status), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        //quote_verification_result
        //
        sgx_ret = sgx_sha256_update(reinterpret_cast<const uint8_t *>(&quote_verification_result), sizeof(quote_verification_result), sha_handle);
        SGX_ERR_BREAK(sgx_ret);


        //p_supplemental_data
        //
        if (p_supplemental_data) {
            sgx_ret = sgx_sha256_update(p_supplemental_data, supplemental_data_size, sha_handle);
            SGX_ERR_BREAK(sgx_ret);
        }

        //get the hashed report_data
        //
        sgx_ret = sgx_sha256_get_hash(sha_handle, reinterpret_cast<sgx_sha256_hash_t *>(&report_data));
        SGX_ERR_BREAK(sgx_ret);


        if (memcmp(&p_qve_report->body.report_data, &report_data, sizeof(report_data)) != 0) {
            ret = SGX_QL_ERROR_REPORT;
            break;
        }


        //Check QvE Identity
        //
        ret = enclave_identity_verify(
            QVE_PRODID,
            LEAST_QVE_ISVSVN,
            p_qve_report,
            qve_isvsvn_threshold,
            false);

    } while (0);
    if (sgx_ret != SGX_SUCCESS)
    {
        ret = (sgx_ret == SGX_ERROR_OUT_OF_MEMORY) ? TEE_ERROR_OUT_OF_MEMORY : TEE_ERROR_UNEXPECTED;
    }
    if (sha_handle)
        sgx_sha256_close(sha_handle);

    return ret;
}
