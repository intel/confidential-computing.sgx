{
global:
    get_launch_token;           /* DEPRECATED, hollow definition kept for ABI compat - returns SGX_ERROR_FEATURE_NOT_SUPPORTED */
    sgx_get_whitelist_size;     /* DEPRECATED, hollow definition kept for ABI compat - returns SGX_ERROR_FEATURE_NOT_SUPPORTED */
    sgx_get_whitelist;          /* DEPRECATED, hollow definition kept for ABI compat - returns SGX_ERROR_FEATURE_NOT_SUPPORTED */
    sgx_register_wl_cert_chain; /* DEPRECATED, hollow definition kept for ABI compat - returns SGX_ERROR_FEATURE_NOT_SUPPORTED */
    sgx_select_att_key_id;
    sgx_init_quote_ex;
    sgx_get_quote_size_ex;
    sgx_get_quote_ex;
    sgx_get_supported_att_key_id_num;
    sgx_get_supported_att_key_ids;
    sgx_trace_logger_callback;
    sgx_trace_loglevel;
local:
    *;
};
