/*
 * Copyright(c) 2011-2026 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sgx_uae_launch.h"
#include "util.h"
#include "uae_service_internal.h"

sgx_status_t get_launch_token(
    const enclave_css_t *p_signature,
    const sgx_attributes_t *p_attribute,
    sgx_launch_token_t *p_launch_token)
{
    UNUSED(p_signature);
    UNUSED(p_attribute);
    UNUSED(p_launch_token);
    return SGX_ERROR_FEATURE_NOT_SUPPORTED; // Token-based launch control is deprecated
}

sgx_status_t SGXAPI sgx_get_whitelist_size(uint32_t* p_whitelist_size)
{
    UNUSED(p_whitelist_size);
    return SGX_ERROR_FEATURE_NOT_SUPPORTED; // Token-based launch control is deprecated
}

sgx_status_t SGXAPI sgx_get_whitelist(uint8_t* p_whitelist, uint32_t whitelist_size)
{
    UNUSED(p_whitelist);
    UNUSED(whitelist_size);
    return SGX_ERROR_FEATURE_NOT_SUPPORTED; // Token-based launch control is deprecated
}

sgx_status_t SGXAPI sgx_register_wl_cert_chain(uint8_t* p_wl_cert_chain, uint32_t wl_cert_chain_size)
{
    UNUSED(p_wl_cert_chain);
    UNUSED(wl_cert_chain_size);
    return SGX_ERROR_FEATURE_NOT_SUPPORTED; // Token-based launch control is deprecated
}
