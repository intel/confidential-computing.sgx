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
#ifndef _I_AESM_LOGIC_H
#define _I_AESM_LOGIC_H

#include <stdint.h>
#include <aeerror.h>
#include <aesm_error.h>

class IAESMLogic {
    public:
        virtual aesm_error_t select_att_key_id(uint32_t att_key_id_list_size,
                const uint8_t *att_key_id_list,
                uint32_t *select_att_key_id_size,
                uint8_t **select_att_key_id) = 0;

        virtual aesm_error_t init_quote_ex(
                uint32_t att_key_id_size, const uint8_t *att_key_id,
                uint8_t **target_info, uint32_t *target_info_size,
                bool b_pub_key_id, size_t *pub_key_id_size, uint8_t **pub_key_id) = 0;

        virtual aesm_error_t get_quote_size_ex(
                uint32_t att_key_id_size, const uint8_t *att_key_id,
                uint32_t *quote_size) = 0;

        virtual aesm_error_t get_quote_ex(
                uint32_t report_size, const uint8_t *report,
                uint32_t att_key_id_size, const uint8_t *att_key_id,
                uint32_t qe_report_info_size, uint8_t *qe_report_info,
                uint32_t quote_size, uint8_t **quote) = 0;

        virtual aesm_error_t get_supported_att_key_id_num(uint32_t *att_key_id_num) = 0;
        virtual aesm_error_t get_supported_att_key_ids(uint8_t **att_key_ids, uint32_t att_key_ids_size) = 0;

        virtual void service_stop() = 0;
        virtual ~IAESMLogic(){};
};

#endif
