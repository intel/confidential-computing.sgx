/*
 * Copyright(c) 2011-2026 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "sgx_error.h"

// Note: These symbols are exported in `libsgx_uae_service_deploy.so` (part of SGX SDK)

sgx_status_t sgx_get_whitelist_size()
{
    printf("Support for the legacy (whitelist-based) Launch Enclave and the legacy out-of-tree Intel(R) SGX driver has been removed in SGX SDK v2.28. There's no need to call this API anymore on systems with Flexible Launch Control. Please update your code accordingly.\n");
    return SGX_ERROR_FEATURE_NOT_SUPPORTED;    
}

sgx_status_t sgx_get_whitelist()
{
    printf("Support for the legacy (whitelist-based) Launch Enclave and the legacy out-of-tree Intel(R) SGX driver has been removed in SGX SDK v2.28. There's no need to call this API anymore on systems with Flexible Launch Control. Please update your code accordingly.\n");
    return SGX_ERROR_FEATURE_NOT_SUPPORTED;
}

sgx_status_t get_launch_token()
{
    printf("Support for the legacy (whitelist-based) Launch Enclave and the legacy out-of-tree Intel(R) SGX driver has been removed in SGX SDK v2.28. There's no need to call this API anymore on systems with Flexible Launch Control. Please update your code accordingly.\n");
    return SGX_ERROR_FEATURE_NOT_SUPPORTED;
}

sgx_status_t sgx_register_wl_cert_chain()
{
    printf("Support for the legacy (whitelist-based) Launch Enclave and the legacy out-of-tree Intel(R) SGX driver has been removed in SGX SDK v2.28. There's no need to call this API anymore on systems with Flexible Launch Control. Please update your code accordingly.\n");
    return SGX_ERROR_FEATURE_NOT_SUPPORTED;
}
