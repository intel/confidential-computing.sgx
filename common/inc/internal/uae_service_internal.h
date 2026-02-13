/*
 * Copyright(c) 2011-2026 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UAE_SERVICE_INTERNAL_H_
#define _UAE_SERVICE_INTERNAL_H_

#ifdef __DEPRECATED
#warning This header is deprecated. Support for Launch Enclave (whitelist-based) launch control has been removed. To disable this warning use -Wno-deprecated.
#endif

#include <stdint.h>
#include "arch.h"
#include "sgx_urts.h"

#ifdef  __cplusplus
extern "C" {
#endif
/**
 * Function to get launch token of an enclave - Not supported since v2.28
 * 
 * @deprecated Support for whitelist-based launch control (via Launch Enclave) has been removed.
 *             Flexible Launch Control mechanism does not require use of this API.
 *
 * @param signature[in] Unused (was: signature of enclave to be launched).
 * @param attribute[in] Unused (was: attribute of enclave to be launched).
 * @param launch_token[out] Unused (was: contains launch token).
 * @return SGX_ERROR_FEATURE_NOT_SUPPORTED
 */
sgx_status_t SGXAPI get_launch_token(const enclave_css_t* signature, const sgx_attributes_t* attribute, sgx_launch_token_t* launch_token);

typedef sgx_status_t (*func_get_launch_token_t)(const enclave_css_t*,
                                                const sgx_attributes_t*,
                                                sgx_launch_token_t*);

#ifdef  __cplusplus
}
#endif

#endif