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

#include <dlfcn.h>
#include "sgx_uae_quote_ex.h"
#include "arch.h"
#include <cstdlib>

/* used to eliminate `unused variable' warning */
#ifndef UNUSED
#define UNUSED(val) (void)(val)
#endif

template<class T>
class Singleton
{
public:
    static T& instance()
    {
        if (0 == _instance)
        {
            _instance = new T();
            atexit(destroy);
        }
        return *_instance;
    }

    virtual ~Singleton()
    {
        _instance = NULL;
    }

protected:
    Singleton(){}
private:
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
    static void destroy()
    {
        if ( _instance != 0 )
        {
            delete _instance;
            _instance = 0;
        }
    }
    static T * volatile _instance;
};


template<class T>
T * volatile Singleton<T>::_instance = 0;

template<class T>
class SharedLibProxy: public Singleton<T>
{
private:
    void* handle;

protected:	
    virtual const char* GetLibraryPath(void) const = 0;

public:
    SharedLibProxy(void):handle(NULL){};
	virtual ~SharedLibProxy(void)
	{
		unload();
	};

    void load()
	{
		handle = dlopen(GetLibraryPath(), RTLD_LAZY);
		if (!isLoaded())
			dlerror();
	};
    void unload()
	{
		if (isLoaded())
		{
			dlclose(handle);
			handle = 0;
		}
	};
    bool isLoaded() const { return handle != 0; };
	bool findSymbol(const char* name, void** function)
	{
		if (!isLoaded())
		{
			load();
		}
		if (isLoaded())
		{
			*function = dlsym(handle, name);
			return *function != NULL;
		}
		return false;
	};
};

#define _CONCAT(x, y) x/**/y

#define MAJOR_VER "1"

const static char QUOTE_EX_LIB[] = _CONCAT("libsgx_quote_ex.so.", MAJOR_VER);

class QuoteExLib:public SharedLibProxy<QuoteExLib>
{
protected:	
    const char* GetLibraryPath(void) const
	{
		return QUOTE_EX_LIB;
	};
};

extern "C" {

sgx_status_t get_launch_token(
    const enclave_css_t*        signature,
    const sgx_attributes_t*     attribute,
    sgx_reserved_field_1024t*   reserved)
{
    UNUSED(signature);
    UNUSED(attribute);
    UNUSED(reserved);
    return SGX_ERROR_FEATURE_NOT_SUPPORTED; // Token-based launch control is deprecated
}


sgx_status_t sgx_get_whitelist_size(
    uint32_t* p_whitelist_size)
{
    UNUSED(p_whitelist_size);
    return SGX_ERROR_FEATURE_NOT_SUPPORTED; // Token-based launch control is deprecated
}


sgx_status_t sgx_get_whitelist(
    uint8_t* p_whitelist,
    uint32_t whitelist_size)
{
    UNUSED(p_whitelist);
    UNUSED(whitelist_size);
	return SGX_ERROR_FEATURE_NOT_SUPPORTED; // Token-based launch control is deprecated
}

sgx_status_t sgx_register_wl_cert_chain(uint8_t* p_wl_cert_chain, uint32_t wl_cert_chain_size)
{
    UNUSED(p_wl_cert_chain);
    UNUSED(wl_cert_chain_size);
	return SGX_ERROR_FEATURE_NOT_SUPPORTED; // Token-based launch control is deprecated
}

sgx_status_t sgx_select_att_key_id(const uint8_t *p_att_key_id_list, uint32_t att_key_id_list_size,
                                   sgx_att_key_id_t *p_selected_key_id)
{
	sgx_status_t (*p_sgx_select_att_key_id)(const uint8_t *p_att_key_id_list, uint32_t att_key_id_list_size,
                                   sgx_att_key_id_t *p_selected_key_id) = NULL;
    if (QuoteExLib::instance().findSymbol(__FUNCTION__, (void**)&p_sgx_select_att_key_id))
    {
        return p_sgx_select_att_key_id(p_att_key_id_list, att_key_id_list_size, p_selected_key_id);
    }
    return SGX_ERROR_SERVICE_UNAVAILABLE;
 }

sgx_status_t sgx_init_quote_ex(const sgx_att_key_id_t* p_att_key_id,
                                            sgx_target_info_t *p_qe_target_info,
                                            size_t* p_pub_key_id_size,
                                            uint8_t* p_pub_key_id)
{
	sgx_status_t (*p_sgx_init_quote_ex)(const sgx_att_key_id_t* p_att_key_id,
                                            sgx_target_info_t *p_qe_target_info,
                                            size_t* p_pub_key_id_size,
                                            uint8_t* p_pub_key_id) = NULL;
    if (QuoteExLib::instance().findSymbol(__FUNCTION__, (void**)&p_sgx_init_quote_ex))
    {
        return p_sgx_init_quote_ex(p_att_key_id, p_qe_target_info, p_pub_key_id_size, p_pub_key_id);
    }
    return SGX_ERROR_SERVICE_UNAVAILABLE;
}

sgx_status_t SGXAPI sgx_get_quote_size_ex(const sgx_att_key_id_t *p_att_key_id,
                                                uint32_t* p_quote_size)
{
	sgx_status_t (*p_sgx_get_quote_size_ex)(const sgx_att_key_id_t *p_att_key_id,
                                                uint32_t* p_quote_size) = NULL;
    if (QuoteExLib::instance().findSymbol(__FUNCTION__, (void**)&p_sgx_get_quote_size_ex))
    {
        return p_sgx_get_quote_size_ex(p_att_key_id, p_quote_size);
    }
    return SGX_ERROR_SERVICE_UNAVAILABLE;
}

sgx_status_t SGXAPI  sgx_get_quote_ex(const sgx_report_t *p_app_report,
                                           const sgx_att_key_id_t *p_att_key_id,
                                           sgx_qe_report_info_t *p_qe_report_info,
                                           uint8_t *p_quote,
                                           uint32_t quote_size)
{
	sgx_status_t (*p_sgx_get_quote_ex)(const sgx_report_t *p_app_report,
                                           const sgx_att_key_id_t *p_att_key_id,
                                           sgx_qe_report_info_t *p_qe_report_info,
                                           uint8_t *p_quote,
                                           uint32_t quote_size) = NULL;
    if (QuoteExLib::instance().findSymbol(__FUNCTION__, (void**)&p_sgx_get_quote_ex))
    {
        return p_sgx_get_quote_ex(p_app_report, p_att_key_id, p_qe_report_info, p_quote, quote_size);
    }
    return SGX_ERROR_SERVICE_UNAVAILABLE;
}

sgx_status_t SGXAPI sgx_get_supported_att_key_id_num(uint32_t *p_att_key_id_num)
{
	sgx_status_t (*p_sgx_get_supported_att_key_id_num)(uint32_t *p_att_key_id_num) = NULL;
    if (QuoteExLib::instance().findSymbol(__FUNCTION__, (void**)&p_sgx_get_supported_att_key_id_num))
    {
		return p_sgx_get_supported_att_key_id_num(p_att_key_id_num);
    }
    return SGX_ERROR_SERVICE_UNAVAILABLE;
}

sgx_status_t SGXAPI sgx_get_supported_att_key_ids(sgx_att_key_id_ext_t *p_att_key_id_list,
												   uint32_t att_key_id_num)
{
    sgx_status_t (*p_sgx_get_supported_att_key_ids)(sgx_att_key_id_ext_t *p_att_key_id_list,
												   uint32_t att_key_id_num) = NULL;
    if (QuoteExLib::instance().findSymbol(__FUNCTION__, (void**)&p_sgx_get_supported_att_key_ids))
    {
		return p_sgx_get_supported_att_key_ids(p_att_key_id_list, att_key_id_num);
    }
    return SGX_ERROR_SERVICE_UNAVAILABLE;
}

} /* extern "C" */
