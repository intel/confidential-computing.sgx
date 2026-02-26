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
 * App enclave can link this library to verify QaE report and identity
 */


#include "sgx_dcap_tvl.h"
#include "encode_helper.h"
#include "sgx_utils.h"
#include "sgx_tcrypto.h"
#include "sgx_trts.h"
#include "sgx_dcap_constant_val.h"


#define SGX_ERR_BREAK(x) {if (x != SGX_SUCCESS) break;}

#define SAFE_FREE(x) if(x!=NULL){free(x);x=NULL;}
#define SAFE_FREE_CONST(x)  if (*(x) != NULL) { free((void*)*(x)); *(x) = NULL;} 
static void tmp_mem_free(qae_verification_input_t *input)
{
    switch(input->mode){
        case APPRAISAL:
            SAFE_FREE(input->input.appraisal.p_appraisal_jwt);
            SAFE_FREE(input->input.appraisal.p_qvl_jwt);
            for(int i=0; i<input->input.appraisal.policy_count; i++){
                SAFE_FREE(input->input.appraisal.p_policies[i]);
            }
            SAFE_FREE(input->input.appraisal.p_policies);
            break;
        case AUTH_POLICY:
            SAFE_FREE(input->input.auth_policy.p_quote);
            SAFE_FREE(input->input.auth_policy.p_appraisal_jwt);
            SAFE_FREE_CONST(&input->input.auth_policy.p_policy_bundle->p_tenant_identity_policy);
            SAFE_FREE_CONST(&input->input.auth_policy.p_policy_bundle->tdqe_policy.p_policy);
            SAFE_FREE_CONST(&input->input.auth_policy.p_policy_bundle->platform_policy.p_policy);
            SAFE_FREE(input->input.auth_policy.p_policy_bundle);
            SAFE_FREE(input->input.auth_policy.p_result);
            break;
        case AUTH_OWNER:
            SAFE_FREE(input->input.auth_owner.p_quote);
            SAFE_FREE(input->input.auth_owner.p_appraisal_jwt);
            SAFE_FREE(input->input.auth_owner.p_result);
            for(int i=0; i<input->input.auth_owner.key_list_count; i++){
                SAFE_FREE(input->input.auth_owner.p_policy_key_list[i]);
            }
            SAFE_FREE(input->input.auth_owner.p_policy_key_list);
            break;
        default:
            break;
    }
}
#define TEE_ERROR_RETURN(x,y) if(x!=TEE_SUCCESS){tmp_mem_free(y); return x;} 
//copy all memory into enclave before using
static quote3_error_t deep_copy (char *in, char **out)
{
    if(in != NULL)
    {
        if(sgx_is_within_enclave(in, sizeof(in))){
            *out = in;
        }
        else {
            *out = (char *)malloc(strlen(in)+1);
            if(*out == NULL)
            {
                return TEE_ERROR_OUT_OF_MEMORY;
            }
            memset(*out, 0, strlen(in)+1);
            memcpy(*out, in, strlen(in));
        }
    }
    else{
        return TEE_ERROR_INVALID_PARAMETER;
    }
    return TEE_SUCCESS;
}

/*
            * Mode APPRAISAL: QAE Appraisal - should be used along with API `tee_appraise_verification_token`
            *         - p_policies, appraisal_check_date, and p_qvl_jwt must be provided.
            *         - p_policy_bundle and p_result must be NULL.
            *         - All other optional parameters should be NULL except p_quote.
            * report_data = SHA384(nonce in p_ae_report_info || QVL output JSON || policy array || appraisal output JWT ||  appraisal_check_date) || 00'
*/
static quote3_error_t tee_verify_appraisal_result(
    qae_verification_input_t *input,
    sgx_ql_qe_report_info_t qae_report_info)
{   
    qae_verification_input_t tmp_input;
    memset(&tmp_input, 0, sizeof(qae_verification_input_t));
    quote3_error_t dcap_ret = TEE_ERROR_UNEXPECTED;
    sgx_status_t sgx_ret = SGX_ERROR_UNEXPECTED;
    sgx_report_data_t report_data = { 0 };
    sgx_sha_state_handle_t sha_handle = NULL;
    
    if(input->input.appraisal.p_appraisal_jwt == NULL || input->input.appraisal.p_qvl_jwt == NULL || 
                input->input.appraisal.p_policies == NULL || input->input.appraisal.policy_count == 0)
    {
        return TEE_ERROR_INVALID_PARAMETER;
    }

    for (uint8_t i = 0; i < input->input.appraisal.policy_count; i++)
    {
        if (input->input.appraisal.p_policies[i] == NULL)
        {
            return TEE_ERROR_INVALID_PARAMETER;
        }
    }
    //memory deep copy into enclave
    tmp_input.input.appraisal.appraisal_check_date = input->input.appraisal.appraisal_check_date;
    tmp_input.input.appraisal.policy_count = input->input.appraisal.policy_count;
    tmp_input.input.appraisal.p_policies = (uint8_t **)malloc((input->input.appraisal.policy_count) * sizeof(uint8_t *));
    if (tmp_input.input.appraisal.p_policies == NULL){
        return TEE_ERROR_OUT_OF_MEMORY;
    }
    memset(tmp_input.input.appraisal.p_policies, 0, input->input.appraisal.policy_count * sizeof(uint8_t *));

    for(int i = 0; i < tmp_input.input.appraisal.policy_count; i++)
    {
        dcap_ret = deep_copy((char *)input->input.appraisal.p_policies[i], (char **)&tmp_input.input.appraisal.p_policies[i]);
        TEE_ERROR_RETURN(dcap_ret, &tmp_input);
    }

    dcap_ret = deep_copy(input->input.appraisal.p_appraisal_jwt, &tmp_input.input.appraisal.p_appraisal_jwt);
    TEE_ERROR_RETURN(dcap_ret, &tmp_input);
    dcap_ret = deep_copy(input->input.appraisal.p_qvl_jwt, &tmp_input.input.appraisal.p_qvl_jwt);
    TEE_ERROR_RETURN(dcap_ret, &tmp_input);

    do {
        //verify QaE report data
        //MODE #APPRAISAL
        //report_data = SHA384(nonce in p_ae_report_info || QVL output JSON || policy array || appraisal output JWT ||  appraisal_check_date) || 00'
        dcap_ret = TEE_ERROR_UNEXPECTED;
        sgx_ret = sgx_sha384_init(&sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(&qae_report_info.nonce), sizeof(qae_report_info.nonce), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(tmp_input.input.appraisal.p_qvl_jwt), (uint32_t)strlen(tmp_input.input.appraisal.p_qvl_jwt), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        for (uint8_t i = 0; i < tmp_input.input.appraisal.policy_count; i++)
        {
            if ((sgx_ret = sgx_sha384_update(tmp_input.input.appraisal.p_policies[i], (uint32_t)strlen(reinterpret_cast<const char *>(tmp_input.input.appraisal.p_policies[i])), sha_handle)) != SGX_SUCCESS)
            {
                sgx_sha384_close(sha_handle);
                TEE_ERROR_RETURN(dcap_ret, &tmp_input);
            }
        }

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(tmp_input.input.appraisal.p_appraisal_jwt), (uint32_t)strlen(tmp_input.input.appraisal.p_appraisal_jwt), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(&tmp_input.input.appraisal.appraisal_check_date), sizeof(time_t), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_get_hash(sha_handle, reinterpret_cast<sgx_sha384_hash_t *>(&report_data));
        SGX_ERR_BREAK(sgx_ret);

        if (memcmp(&qae_report_info.qe_report.body.report_data, &report_data, sizeof(report_data)) != 0) {
            dcap_ret = TEE_RESULT_REPORT_DATA_MISMATCH;
            break;
        }
        dcap_ret = TEE_SUCCESS;
    } while(0);
    if (sgx_ret != SGX_SUCCESS)
    {
        dcap_ret = (sgx_ret == SGX_ERROR_OUT_OF_MEMORY) ? TEE_ERROR_OUT_OF_MEMORY : TEE_ERROR_UNEXPECTED;
    }
    if (sha_handle){
        sgx_sha384_close(sha_handle);
    }
    tmp_mem_free(&tmp_input);
    return dcap_ret;
}

/*
            * Mode AUTH_POLICY: Strict Policy Authentication - should be used along with API`tee_authenticate_appraisal_result_ex`
            *         - p_policy_bundle and p_result must be provided.
            *         - p_policies, appraisal_check_date, and p_qvl_jwt must be NULL.
            *         - All other optional parameters should be NULL except p_quote.
            * report_data = SHA384 (nonce in p_ae_report_info || appraisal output JWT || policy bundle || audit result || quote (optional)) || 00'
*/
static quote3_error_t tee_verify_auth_policy_result(
    qae_verification_input_t *input,
    sgx_ql_qe_report_info_t qae_report_info)
{   
    qae_verification_input_t tmp_input;
    memset(&tmp_input, 0, sizeof(qae_verification_input_t));
    quote3_error_t dcap_ret = TEE_ERROR_UNEXPECTED;
    sgx_status_t sgx_ret = SGX_ERROR_UNEXPECTED;
    sgx_report_data_t report_data = { 0 };
    sgx_sha_state_handle_t sha_handle = NULL;

    if(input->input.auth_policy.p_appraisal_jwt == NULL || input->input.auth_policy.p_policy_bundle == NULL ||
        (input->input.auth_policy.p_policy_bundle->platform_policy.pt == CUSTOMIZED && input->input.auth_policy.p_policy_bundle->platform_policy.p_policy == NULL) ||
        (input->input.auth_policy.p_policy_bundle->platform_policy.pt == DEFAULT_STRICT && input->input.auth_policy.p_policy_bundle->platform_policy.p_policy != NULL) ||
        (input->input.auth_policy.p_policy_bundle->tdqe_policy.pt == CUSTOMIZED && input->input.auth_policy.p_policy_bundle->tdqe_policy.p_policy == NULL) ||
        (input->input.auth_policy.p_policy_bundle->tdqe_policy.pt == DEFAULT_STRICT && input->input.auth_policy.p_policy_bundle->tdqe_policy.p_policy != NULL) ||
        input->input.auth_policy.p_td_identity != NULL || input->input.auth_policy.p_td_tcb_mapping_table != NULL ||
        input->input.auth_policy.p_result == NULL)
    {
        return TEE_ERROR_INVALID_PARAMETER;
    }
    if(input->input.auth_policy.p_quote != NULL){
        if(input->input.auth_policy.quote_size != 0){
            tmp_input.input.auth_policy.quote_size = input->input.auth_policy.quote_size;
            tmp_input.input.auth_policy.p_quote = (uint8_t *)malloc(tmp_input.input.auth_policy.quote_size);
            if(tmp_input.input.auth_policy.p_quote == NULL){
                return TEE_ERROR_OUT_OF_MEMORY;
            }
            memcpy(tmp_input.input.auth_policy.p_quote, input->input.auth_policy.p_quote, tmp_input.input.auth_policy.quote_size);
        }
        else {
            return TEE_ERROR_INVALID_PARAMETER;
        }
    }
    dcap_ret = deep_copy(input->input.auth_policy.p_appraisal_jwt, &tmp_input.input.auth_policy.p_appraisal_jwt);
    TEE_ERROR_RETURN(dcap_ret, &tmp_input);
    tmp_input.input.auth_policy.p_policy_bundle = (tee_policy_bundle_t *)malloc(sizeof(tee_policy_bundle_t));
    if(input->input.auth_policy.p_policy_bundle == NULL){
        TEE_ERROR_RETURN(TEE_ERROR_OUT_OF_MEMORY, &tmp_input);
    }
    memset(tmp_input.input.auth_policy.p_policy_bundle, 0, sizeof(tmp_input.input.auth_policy.p_policy_bundle));
    // Copy policies to enclave before operation
    if (input->input.auth_policy.p_policy_bundle->p_tenant_identity_policy)
    {
        uint8_t *tmp_p = NULL;
        dcap_ret = deep_copy((char *)input->input.auth_policy.p_policy_bundle->p_tenant_identity_policy, (char **)&tmp_p);
        TEE_ERROR_RETURN(dcap_ret, &tmp_input);
        tmp_input.input.auth_policy.p_policy_bundle->p_tenant_identity_policy = tmp_p;
    }
    if (input->input.auth_policy.p_policy_bundle->platform_policy.p_policy)
    {
        uint8_t *tmp_p = NULL;
        dcap_ret = deep_copy((char *)input->input.auth_policy.p_policy_bundle->platform_policy.p_policy, (char **)&tmp_p);
        TEE_ERROR_RETURN(dcap_ret, &tmp_input);
        tmp_input.input.auth_policy.p_policy_bundle->platform_policy.p_policy = tmp_p;
    }
    tmp_input.input.auth_policy.p_policy_bundle->platform_policy.pt = input->input.auth_policy.p_policy_bundle->platform_policy.pt;
    if (input->input.auth_policy.p_policy_bundle->tdqe_policy.p_policy)
    {
        uint8_t *tmp_p = NULL;
        dcap_ret = deep_copy((char *)input->input.auth_policy.p_policy_bundle->tdqe_policy.p_policy, (char **)&tmp_p);
        TEE_ERROR_RETURN(dcap_ret, &tmp_input);
        tmp_input.input.auth_policy.p_policy_bundle->tdqe_policy.p_policy = tmp_p;
    }
    tmp_input.input.auth_policy.p_policy_bundle->tdqe_policy.pt = input->input.auth_policy.p_policy_bundle->tdqe_policy.pt;
    tmp_input.input.auth_policy.p_result = (tee_policy_auth_result_t *)malloc(sizeof(tee_policy_auth_result_t));
    if(tmp_input.input.auth_policy.p_result == NULL){
        TEE_ERROR_RETURN(TEE_ERROR_OUT_OF_MEMORY, &tmp_input);
    }
    memcpy(tmp_input.input.auth_policy.p_result, input->input.auth_policy.p_result, sizeof(tee_policy_auth_result_t));

    do {
        dcap_ret = TEE_ERROR_UNEXPECTED;
        //MODE #AUTH_POLICY
        //report_data = SHA384 (nonce in p_ae_report_info || appraisal output JWT || policy bundle || audit result || quote (optional)) || 00'
        sgx_ret = sgx_sha384_init(&sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(&qae_report_info.nonce), sizeof(qae_report_info.nonce), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(tmp_input.input.auth_policy.p_appraisal_jwt), (uint32_t)strlen(tmp_input.input.auth_policy.p_appraisal_jwt), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        if (tmp_input.input.auth_policy.p_policy_bundle->p_tenant_identity_policy)
        {
            sgx_ret = sgx_sha384_update(tmp_input.input.auth_policy.p_policy_bundle->p_tenant_identity_policy, (uint32_t)strlen((const char *)tmp_input.input.auth_policy.p_policy_bundle->p_tenant_identity_policy), sha_handle);
            SGX_ERR_BREAK(sgx_ret);
        }

        if (tmp_input.input.auth_policy.p_policy_bundle->platform_policy.p_policy)
        {
            sgx_ret = sgx_sha384_update(tmp_input.input.auth_policy.p_policy_bundle->platform_policy.p_policy, (uint32_t)strlen((const char *)(tmp_input.input.auth_policy.p_policy_bundle->platform_policy.p_policy)), sha_handle);
            SGX_ERR_BREAK(sgx_ret);
        }
        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(&tmp_input.input.auth_policy.p_policy_bundle->platform_policy.pt), sizeof(tmp_input.input.auth_policy.p_policy_bundle->platform_policy.pt), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        if (tmp_input.input.auth_policy.p_policy_bundle->tdqe_policy.p_policy)
        {
            sgx_ret = sgx_sha384_update(tmp_input.input.auth_policy.p_policy_bundle->tdqe_policy.p_policy, (uint32_t)strlen((const char *)(tmp_input.input.auth_policy.p_policy_bundle->tdqe_policy.p_policy)), sha_handle);
            SGX_ERR_BREAK(sgx_ret);
        }
        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(&tmp_input.input.auth_policy.p_policy_bundle->tdqe_policy.pt), sizeof(tmp_input.input.auth_policy.p_policy_bundle->tdqe_policy.pt), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(tmp_input.input.auth_policy.p_result), sizeof(tee_policy_auth_result_t ), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        if(tmp_input.input.auth_policy.p_quote != NULL)
        {
            sgx_ret = sgx_sha384_update(tmp_input.input.auth_policy.p_quote, tmp_input.input.auth_policy.quote_size, sha_handle);
            SGX_ERR_BREAK(sgx_ret);
        }

        sgx_ret = sgx_sha384_get_hash(sha_handle, reinterpret_cast<sgx_sha384_hash_t *>(&report_data));
        SGX_ERR_BREAK(sgx_ret);

        if (memcmp(&qae_report_info.qe_report.body.report_data, &report_data, sizeof(report_data)) != 0) {
            dcap_ret = TEE_RESULT_REPORT_DATA_MISMATCH;
            break;
        }
        dcap_ret = TEE_SUCCESS;
    } while(0);

    if (sgx_ret != SGX_SUCCESS)
    {
        dcap_ret = (sgx_ret == SGX_ERROR_OUT_OF_MEMORY) ? TEE_ERROR_OUT_OF_MEMORY : TEE_ERROR_UNEXPECTED;
    }
    if (sha_handle){
        sgx_sha384_close(sha_handle);
    }
    tmp_mem_free(&tmp_input);
    return dcap_ret;
}

/*
            * Mode 3: Policy Owner Authentication - should be used along with API`tee_authenticate_policy_owner`
            *         - p_policy_key_list and p_result must be provided.
            *         - p_policy_bundle, appraisal_check_date, and p_qvl_jwt must be NULL.
            *         - All other optional parameters should be NULL except p_quote.
            * report_data = SHA384 (nonce in p_ae_report_info || appraisal output JWT || policy key list array || audit result || quote (optional)) || 00'
*/
static quote3_error_t tee_verify_auth_audit_result(
    qae_verification_input_t *input,
    sgx_ql_qe_report_info_t qae_report_info)
{   
    qae_verification_input_t tmp_input;
    memset(&tmp_input, 0, sizeof(qae_verification_input_t));
    quote3_error_t dcap_ret = TEE_ERROR_UNEXPECTED;
    sgx_status_t sgx_ret = SGX_ERROR_UNEXPECTED;
    sgx_report_data_t report_data = { 0 };
    sgx_sha_state_handle_t sha_handle = NULL;

     if(input->input.auth_owner.p_appraisal_jwt == NULL || input->input.auth_owner.p_policy_key_list == NULL ||
            input->input.auth_owner.key_list_count == 0 || input->input.auth_owner.p_result == NULL ||
            input->input.auth_owner.p_td_identity != NULL || input->input.auth_owner.p_td_tcb_mapping_table != NULL)
    {
        return TEE_ERROR_INVALID_PARAMETER;
    }
    if(input->input.auth_owner.p_quote != NULL){
        if(input->input.auth_owner.quote_size != 0){
            tmp_input.input.auth_owner.quote_size = input->input.auth_owner.quote_size;
            tmp_input.input.auth_owner.p_quote = (uint8_t *)malloc(tmp_input.input.auth_owner.quote_size);
            if(tmp_input.input.auth_owner.p_quote == NULL){
                return TEE_ERROR_OUT_OF_MEMORY;
            }
            memcpy(tmp_input.input.auth_owner.p_quote, input->input.auth_owner.p_quote, tmp_input.input.auth_owner.quote_size);
        }
        else {
            return TEE_ERROR_INVALID_PARAMETER;
        }
    }
    for (uint8_t i = 0; i < input->input.auth_owner.key_list_count; i++)
    {
        if (input->input.auth_owner.p_policy_key_list[i] == NULL)
        {
            TEE_ERROR_RETURN(TEE_ERROR_INVALID_PARAMETER, &tmp_input);
        }
    }
        //deep copy
        tmp_input.input.auth_owner.key_list_count = input->input.auth_owner.key_list_count;
        tmp_input.input.auth_owner.p_policy_key_list = (uint8_t **)malloc((input->input.auth_owner.key_list_count) * sizeof(uint8_t *));
        if (tmp_input.input.auth_owner.p_policy_key_list == NULL){
            TEE_ERROR_RETURN(TEE_ERROR_OUT_OF_MEMORY, &tmp_input);
        }
        memset(tmp_input.input.auth_owner.p_policy_key_list, 0, input->input.auth_owner.key_list_count * sizeof(uint8_t *));

        for(int i = 0; i < tmp_input.input.auth_owner.key_list_count; i++)
        {
            dcap_ret = deep_copy((char *)input->input.auth_owner.p_policy_key_list[i], (char **)&tmp_input.input.auth_owner.p_policy_key_list[i]);
            TEE_ERROR_RETURN(dcap_ret, &tmp_input);
        }

        dcap_ret = deep_copy((char *)input->input.auth_owner.p_appraisal_jwt, &tmp_input.input.auth_owner.p_appraisal_jwt);
        TEE_ERROR_RETURN(dcap_ret, &tmp_input);
        tmp_input.input.auth_owner.p_result = (tee_policy_auth_result_t *)malloc(sizeof(tee_policy_auth_result_t));
        if(tmp_input.input.auth_owner.p_result == NULL){
            TEE_ERROR_RETURN(TEE_ERROR_OUT_OF_MEMORY, &tmp_input);
        }
        memcpy(tmp_input.input.auth_owner.p_result, input->input.auth_owner.p_result, sizeof(tee_policy_auth_result_t));

    do {
        //MODE #AUTH_OWNER
        //report_data = SHA384 (nonce in p_ae_report_info || appraisal output JWT || policy key list array || audit result || quote (optional)) || 00'
        dcap_ret = TEE_ERROR_UNEXPECTED;
        sgx_ret = sgx_sha384_init(&sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(&qae_report_info.nonce), sizeof(qae_report_info.nonce), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(tmp_input.input.auth_owner.p_appraisal_jwt), (uint32_t)strlen(tmp_input.input.auth_owner.p_appraisal_jwt), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        for (uint8_t i = 0; i < tmp_input.input.auth_owner.key_list_count; i++)
        {
            if ((sgx_ret = sgx_sha384_update(tmp_input.input.auth_owner.p_policy_key_list[i], (uint32_t)strlen(reinterpret_cast<const char *>(tmp_input.input.auth_owner.p_policy_key_list[i])), sha_handle)) != SGX_SUCCESS)
            {
                sgx_sha384_close(sha_handle);
                TEE_ERROR_RETURN(dcap_ret, &tmp_input);
            }
        }

        sgx_ret = sgx_sha384_update(reinterpret_cast<const uint8_t *>(tmp_input.input.auth_owner.p_result), sizeof(tee_policy_auth_result_t), sha_handle);
        SGX_ERR_BREAK(sgx_ret);

        if(tmp_input.input.auth_owner.p_quote != NULL)
        {
            sgx_ret = sgx_sha384_update(tmp_input.input.auth_owner.p_quote, tmp_input.input.auth_owner.quote_size, sha_handle);
            SGX_ERR_BREAK(sgx_ret);
        }

        sgx_ret = sgx_sha384_get_hash(sha_handle, reinterpret_cast<sgx_sha384_hash_t *>(&report_data));
        SGX_ERR_BREAK(sgx_ret);

        if (memcmp(&qae_report_info.qe_report.body.report_data, &report_data, sizeof(report_data)) != 0) {
            dcap_ret = TEE_RESULT_REPORT_DATA_MISMATCH;
            break;
        }
        dcap_ret = TEE_SUCCESS;
    } while(0);
    if (sgx_ret != SGX_SUCCESS)
    {
        dcap_ret = (sgx_ret == SGX_ERROR_OUT_OF_MEMORY) ? TEE_ERROR_OUT_OF_MEMORY : TEE_ERROR_UNEXPECTED;
    }
    if (sha_handle){
        sgx_sha384_close(sha_handle);
    }

    tmp_mem_free(&tmp_input);
    return dcap_ret;
}

quote3_error_t tee_verify_qae_report_and_identity(
        qae_verification_input_t *input,
        sgx_ql_qe_report_info_t qae_report_info,
        sgx_isv_svn_t qae_isvsvn_threshold)
{
    sgx_status_t sgx_ret = SGX_ERROR_UNEXPECTED;
    quote3_error_t ret = TEE_ERROR_UNEXPECTED;

    if(input == NULL || !sgx_is_within_enclave(input, sizeof(input)))
    {
        return TEE_ERROR_INVALID_PARAMETER;
    }

    if(input->mode != APPRAISAL && input->mode != AUTH_POLICY && input->mode != AUTH_OWNER)
    {
        return TEE_ERROR_INVALID_PARAMETER;
    }

    const sgx_report_t *p_qae_report = &(qae_report_info.qe_report);
    do {

        //verify QaE report
        sgx_ret = sgx_verify_report(p_qae_report);
        if (sgx_ret != SGX_SUCCESS) {
            ret = TEE_ERROR_REPORT;
            break;
        }

        switch(input->mode){

            //Mode APPRAISAL: QAE Appraisal - should be used along with API `tee_appraise_verification_token`
            case APPRAISAL:
                ret = tee_verify_appraisal_result(input, qae_report_info);
                break;
            
            //Mode AUTH_POLICY: Strict Policy Authentication - should be used along with API`tee_authenticate_appraisal_result_ex`
            case AUTH_POLICY:
                ret = tee_verify_auth_policy_result(input, qae_report_info);
                break;

            //Mode 3: Policy Owner Authentication - should be used along with API`tee_authenticate_policy_owner`
            case AUTH_OWNER:
                ret = tee_verify_auth_audit_result(input, qae_report_info);
                break;
            default:
                break;
        }
        if(ret != TEE_SUCCESS)
        {
            break;
        }

        //Check QaE Identity
        //
        ret = enclave_identity_verify(
            QAE_PRODID,
            LEAST_QAE_ISVSVN,
            p_qae_report,
            qae_isvsvn_threshold,
            true);
    } while (0);

    return ret;
}
