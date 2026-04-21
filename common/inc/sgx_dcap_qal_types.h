/**
* Copyright (c) 2011-2026, Intel Corporation
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*    * Neither the name of Intel Corporation nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE

* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _SGX_DCAP_QAL_TYPES_H_
#define _SGX_DCAP_QAL_TYPES_H_

#include <stdint.h>

typedef enum _tee_platform_policy_type_t
{
    DEFAULT_STRICT = 0,
    CUSTOMIZED
} tee_platform_policy_type_t;

typedef struct _tee_platform_policy_t
{
    tee_platform_policy_type_t pt;
    const uint8_t* p_policy;
} tee_platform_policy_t;

typedef struct _tee_policy_bundle_t
{
    const uint8_t *p_tenant_identity_policy;
    tee_platform_policy_t platform_policy;

    tee_platform_policy_t tdqe_policy;  /* For tdqe. Only for TDX and only need to be set when user uses a separate tdqe_policy
                                         * instead of an integrated platform_policy including both TDX platform policy and TDQE. */

    tee_platform_policy_t reserved[2];  /* Reserved for future usage */
} tee_policy_bundle_t;

typedef enum _tee_policy_auth_result_t
{
    TEE_AUTH_INCOMPLET = -1,    /* Only part of the policies are provided and authenticated successfully. For example, you only input
                                 * SGX platform policy for an SGX appraisal token, and the platform policy is authenticated successfully */
    TEE_AUTH_SUCCESS = 0,       /* All the policies are authenticated successfully. For SGX, both SGX platform policies are provided and successfully */
    TEE_AUTH_FAILURE = 1,       /* At least one of the input policies are authenticated failed */
} tee_policy_auth_result_t;

#endif