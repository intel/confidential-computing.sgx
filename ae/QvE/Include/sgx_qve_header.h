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

#ifndef _SGX_QVE_HEADER_H_
#define _SGX_QVE_HEADER_H_

#include "sgx_key.h"
#include "time.h"

#ifndef TEE_QV_MK_ERROR
#define TEE_QV_MK_ERROR(x)              (0x0000A000|(x))
#endif //TEE_QV_MK_ERROR
/** Contains the possible values of the quote verification result. */
typedef enum _sgx_ql_qv_result_t
{
    // Quote verification passed and is at the latest TCB level
    SGX_QL_QV_RESULT_OK = 0x0000,   TEE_QV_RESULT_OK = 0x0000,

    SGX_QL_QV_RESULT_MIN = TEE_QV_MK_ERROR(0x0001),   TEE_QV_RESULT_MIN = TEE_QV_MK_ERROR(0x0001),

    // The Quote verification passed, but further actions are required:
    SGX_QL_QV_RESULT_CONFIG_NEEDED = TEE_QV_MK_ERROR(0x0001),    TEE_QV_RESULT_CONFIG_NEEDED = TEE_QV_MK_ERROR(0x0001), // Additional configuration of the platform needed
    SGX_QL_QV_RESULT_OUT_OF_DATE = TEE_QV_MK_ERROR(0x0002),  TEE_QV_RESULT_OUT_OF_DATE = TEE_QV_MK_ERROR(0x0002),   // TCB level out of date, platform patching required
    SGX_QL_QV_RESULT_OUT_OF_DATE_CONFIG_NEEDED = TEE_QV_MK_ERROR(0x0003),    TEE_QV_RESULT_OUT_OF_DATE_CONFIG_NEEDED = TEE_QV_MK_ERROR(0x0003), // Both patching and additional configuration needed

    // Errors
    SGX_QL_QV_RESULT_INVALID_SIGNATURE = TEE_QV_MK_ERROR(0x0004),   TEE_QV_RESULT_INVALID_SIGNATURE = TEE_QV_MK_ERROR(0x0004),
    SGX_QL_QV_RESULT_REVOKED = TEE_QV_MK_ERROR(0x0005), TEE_QV_RESULT_REVOKED = TEE_QV_MK_ERROR(0x0005),
    SGX_QL_QV_RESULT_UNSPECIFIED = TEE_QV_MK_ERROR(0x0006), TEE_QV_RESULT_UNSPECIFIED = TEE_QV_MK_ERROR(0x0006),

    // Requires Software or Configuration Hardening
    SGX_QL_QV_RESULT_SW_HARDENING_NEEDED = TEE_QV_MK_ERROR(0x0007), TEE_QV_RESULT_SW_HARDENING_NEEDED = TEE_QV_MK_ERROR(0x0007),    // TCB level is up to date, but SGX SW Hardening is needed
    SGX_QL_QV_RESULT_CONFIG_AND_SW_HARDENING_NEEDED = TEE_QV_MK_ERROR(0x0008),  TEE_QV_RESULT_CONFIG_AND_SW_HARDENING_NEEDED = TEE_QV_MK_ERROR(0x0008), //TCB level is up to date, but both SW Hardening and additional configuration are needed

    // TDX specific results
    SGX_QL_QV_RESULT_TD_RELAUNCH_ADVISED = TEE_QV_MK_ERROR(0x0009), TEE_QV_RESULT_TD_RELAUNCH_ADVISED = TEE_QV_MK_ERROR(0x0009),    // All components in the TD’s TCB are latest, including the TD preserving loaded TDX, but the TD was launched
                                                                                                                                    // and ran for some time with out-of-date TDX Module. Relaunching or re-provisioning your TD is advised
    SGX_QL_QV_RESULT_TD_RELAUNCH_ADVISED_CONFIG_NEEDED = TEE_QV_MK_ERROR(0x000A), TEE_QV_RESULT_TD_RELAUNCH_ADVISED_CONFIG_NEEDED = TEE_QV_MK_ERROR(0x000A),    // Same as above, relaunching or re-provisioning your TD is advised. In the meantime,
                                                                                                                                                                // additional configuration of the platform is needed

    // Maximum result value
    SGX_QL_QV_RESULT_MAX = TEE_QV_MK_ERROR(0x00FF),  TEE_QV_RESULT_MAX = TEE_QV_MK_ERROR(0x00FF),

} sgx_ql_qv_result_t, tee_qv_result_t;

typedef enum _pck_cert_flag_enum_t {
    PCK_FLAG_FALSE = 0,
    PCK_FLAG_TRUE,
    PCK_FLAG_UNDEFINED
} pck_cert_flag_enum_t;


#define ROOT_KEY_ID_SIZE    48
#define PLATFORM_INSTANCE_ID_SIZE   16

// Each Intel Advisory size is ~16 bytes
// Assume each TCB level has 20 advisoryIDs at the very most
#define MAX_SA_SIZE     20
#define MAX_SA_NUMBER_PER_TCB   20
#define MAX_SA_LIST_SIZE    450

// Nameless struct generates C4201 warning in MS compiler, but it is allowed in c++ 11 standard
// Should remove the pragma after Microsoft fixes this issue
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

/** Contains data that will allow an alternative quote verification policy. */
typedef struct _sgx_ql_qv_supplemental_t
{
    union {
        uint32_t version;                       ///< 'version' is the backward compatible legacy representation
        struct {
            uint16_t major_version;             ///< If this major version doesn't change, the size of the structure may change and new fields appended to the end but old minor version structure can still be 'cast'
                                                ///< If this major version does change, then the structure has been modified in a way that makes the older definitions non-backwards compatible. i.e. You cannot 'cast' older definitions
            uint16_t minor_version;             ///< If this version changes, new fields have been appended to the end of the previous minor version definition of the structure
                                                ///< Set to 1 to support SA_List.  Set to 0 to support everything except the SA List
        };
    };
    time_t earliest_issue_date;           ///< Earliest issue date of all the collateral (UTC)
    time_t latest_issue_date;             ///< Latest issue date of all the collateral (UTC)
    time_t earliest_expiration_date;      ///< Earliest expiration date of all the collateral (UTC)
    time_t tcb_level_date_tag;            ///< The SGX TCB of the platform that generated the quote is not vulnerable
                                          ///< to any Security Advisory with an SGX TCB impact released on or before this date.
                                          ///< See Intel Security Center Advisories
    uint32_t pck_crl_num;                 ///< CRL Num from PCK Cert CRL
    uint32_t root_ca_crl_num;             ///< CRL Num from Root CA CRL
    uint32_t tcb_eval_ref_num;            ///< Lower number of the TCBInfo and QEIdentity
    uint8_t root_key_id[ROOT_KEY_ID_SIZE];              ///< ID of the collateral's root signer (hash of Root CA's public key SHA-384)
    sgx_key_128bit_t pck_ppid;            ///< PPID from remote platform.  Can be used for platform ownership checks
    sgx_cpu_svn_t tcb_cpusvn;             ///< CPUSVN of the remote platform's PCK Cert
    sgx_isv_svn_t tcb_pce_isvsvn;         ///< PCE_ISVNSVN of the remote platform's PCK Cert
    uint16_t pce_id;                      ///< PCE_ID of the remote platform
    uint32_t tee_type;                    ///< 0x00000000: SGX or 0x00000081: TDX
    uint8_t sgx_type;                     ///< Indicate the type of memory protection available on the platform, it should be one of
                                          ///< Standard (0), Scalable (1) and Scalable with Integrity (2)

    // Multi-Package PCK cert related flags, they are only relevant to PCK Certificates issued by PCK Platform CA
    uint8_t platform_instance_id[PLATFORM_INSTANCE_ID_SIZE];           ///< Value of Platform Instance ID, 16 bytes
    pck_cert_flag_enum_t dynamic_platform;      ///< Indicate whether a platform can be extended with additional packages - via Package Add calls to SGX Registration Backend
    pck_cert_flag_enum_t cached_keys;           ///< Indicate whether platform root keys are cached by SGX Registration Backend
    pck_cert_flag_enum_t smt_enabled;           ///< Indicate whether a plat form has SMT (simultaneous multithreading) enabled

    char sa_list[MAX_SA_LIST_SIZE];             ///< String of comma separated list of Security Advisory IDs
    time_t qe_iden_earliest_issue_date;           ///< Earliest issue date of QEIdentity (UTC)
    time_t qe_iden_latest_issue_date;             ///< Latest issue date of QEIdentity (UTC)
    time_t qe_iden_earliest_expiration_date;      ///< Earliest expiration date of QEIdentity (UTC)
    time_t qe_iden_tcb_level_date_tag;            ///< The SGX TCB of the platform that generated the quote is not vulnerable
    uint32_t qe_iden_tcb_eval_ref_num;            ///< Lower number of the QEIdentity
    sgx_ql_qv_result_t qe_iden_status;            /// QEIdentity status
} sgx_ql_qv_supplemental_t;

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/** Descriptor of the supplemental data requestor structure. Used when requesting supplemental data from the DCAP quote verification API */
typedef struct _tee_supp_data_descriptor_t
{
    uint16_t major_version;             ///< Input. Major version of supplemental data
                                        ///< If == 0, then return latest version of the sgx_ql_qv_supplemental_t structure
                                        ///< If <= latest supported, return the latest minor version associated with that major version
                                        ///< > latest supported, return an error (SGX_QL_SUPPLEMENTAL_DATA_VERSION_NOT_SUPPORTED)

    uint32_t data_size;                 ///< Input. Supplemental data size of `p_data`, which returned by API `tee_get_supplemental_data_version_and_size()`
    uint8_t *p_data;                    ///< Output. Pointer to supplemental data
}tee_supp_data_descriptor_t;


#endif //_QVE_HEADER_H_
