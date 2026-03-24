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
/**
 * File: sgx_dcap_qae_tvl.h
 *
 * Description: Trusted library for app enclave to verify QaE Report and Identity
 *
 */
#ifndef __QAE_VERIFICATION_INPUT_T
#define __QAE_VERIFICATION_INPUT_T

#include "sgx_dcap_qal.h"

#ifndef _MSC_VER

typedef enum _tvl_mode_t
{
    APPRAISAL = 1,    //should be used along with QVL API `tee_appraise_verification_token`
    AUTH_POLICY,    //should be used along with QVL API`tee_authenticate_appraisal_result_ex`
    AUTH_OWNER    //should be used along with QVL API`tee_authenticate_policy_owner`
} tvl_mode_t;

typedef struct _qae_verification_input_t
{
    tvl_mode_t mode;
    union 
    {
        struct
        {
            char* p_appraisal_jwt;   //Pointer to the final appraisal JWT
            char* p_qvl_jwt;    //Pointer to the QvE output JWT
            time_t appraisal_check_date;    //The date for appraisal check
            uint8_t** p_policies;    //Pointer to an array of pointers to individual policies
            uint8_t policy_count;    //Count of individual policies provided
        } appraisal;  // APPRAISAL mode
        struct
        {
            char* p_appraisal_jwt;    //Pointer to the final appraisal JWT
            tee_policy_bundle_t* p_policy_bundle;    //Pointer to the policy bundle structure
            const uint8_t* p_td_identity;    //This parameter should currently be set to NULL; functionality to be implemented in a future release
            const uint8_t* p_td_tcb_mapping_table;    //This parameter should currently be set to NULL; functionality to be implemented in a future release
            tee_policy_auth_result_t* p_result;    //Pointer to the result of policy authentication
            uint8_t *p_quote;    //Optional. Pointer to the quote data
            uint32_t quote_size;   //quote size, it should be 0 if p_quote is NULL
        } auth_policy;  // AUTH POLICY mode
        struct
        {
            char* p_appraisal_jwt;   //Pointer to the final appraisal JWT
            uint8_t** p_policy_key_list;    //Points to an array of pointers, with each pointer pointing to a buffer holding a policy signing key
            uint8_t key_list_count;    //Count of individual policy keys provided
            const uint8_t* p_td_identity;    //This parameter should currently be set to NULL; functionality to be implemented in a future release
            const uint8_t* p_td_tcb_mapping_table;    //This parameter should currently be set to NULL; functionality to be implemented in a future release
            tee_policy_auth_result_t* p_result;    //Pointer to the result of policy authentication
            uint8_t * p_quote;    //Optional. Pointer to the quote data
            uint32_t quote_size;    //quote size, it should be 0 if p_quote is NULL
        } auth_owner;  // AUTH OWNER mode
    }input;
} qae_verification_input_t;
#endif
#endif