/*
* Copyright(c) 2026 Intel Corporation
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _SGX_SGX_ENCLAVE_IDENTITY_HELPER_H_
#define _SGX_SGX_ENCLAVE_IDENTITY_HELPER_H_

quote3_error_t enclave_identity_verify(
    int16_t prodid,
    uint16_t isv_svn,
    const sgx_report_t *p_report,
    sgx_isv_svn_t isvsvn_threshold,
    bool qae_mode
);

#endif //_SGX_SGX_ENCLAVE_IDENTITY_HELPER_H_
