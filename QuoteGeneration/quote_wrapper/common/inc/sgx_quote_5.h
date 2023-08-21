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
 * File: sgx_quote_5.h
 * Description: Definition for quote structure.
 *
 * Quote structure and all relative structure will be defined in this file.
 */

#ifndef _SGX_QUOTE_5_H_
#define _SGX_QUOTE_5_H_

#include "sgx_quote_4.h"


#pragma pack(push, 1)

#define QE_QUOTE_VERSION_V5 5
#define TD_INFO_RESERVED_BYTES_V1_5 64
typedef struct _tee_info_v1_5_t                 /* 512 bytes */
{
    tee_attributes_t     attributes;          /* (  0) TD's attributes */
    tee_attributes_t     xfam;                /* (  8) TD's XFAM */
    tee_measurement_t    mr_td;               /* ( 16) Measurement of the initial contents of the TD */
    tee_measurement_t    mr_config_id;        /* ( 64) Software defined ID for non-owner-defined configuration on the guest TD. e.g., runtime or OS configuration */
    tee_measurement_t    mr_owner;            /* (112) Software defined ID for the guest TD's owner */
    tee_measurement_t    mr_owner_config;     /* (160) Software defined ID for owner-defined configuration of the guest TD, e.g., specific to the workload rather than the runtime or OS */
    tee_measurement_t    rt_mr[4];            /* (208) Array of 4(TDX1: NUM_RTMRS is 4) runtime extendable measurement registers */
    tee_measurement_t    mr_servicetd;        /* (400) If is one or more bound or pre-bound service TDs, SERVTD_HASH is the SHA384 hash of the TDINFO_STRUCTs of those service TDs bound.
                                                       Else, SERVTD_HASH is 0. */
    uint8_t reserved[TD_INFO_RESERVED_BYTES_V1_5]; /* (448) Reserved, must be zero */
} tee_info_v1_5_t;


#define TD_TEE_TCB_INFO_RESERVED_BYTES_V1_5 95
typedef struct _tee_tcb_info_v1_5_t
{
    uint8_t           valid[8];                                   /* (  0) Indicates TEE_TCB_INFO fields which are valid */
                                                                  /*       - 1 in the i-th significant bit reflects that the field starting at byte offset(8*i) */
                                                                  /*       - 0 in the i-th significant bit reflects that either no field start by byte offset(8*i) or that */
                                                                  /*           field is not populated and is set to zero. */
                                                                  /*       the accepted value of a TDX 1.5 tee_tcb_info_v2 is 0x013ff. (Note: Set to 0x301FF if */
                                                                  /*       SEAMDB_ENABLED == ‘1, otherwise set to 0x1FF. (SEAMDB_ENABLED is introduced for TDX1.4 TD Preserving)*/
    tee_tcb_svn_t     tee_tcb_svn;                                /* (  8) TEE_TCB_SVN Array */
    tee_measurement_t mr_seam;                                    /* ( 24) Measurement of the SEAM module */
    tee_measurement_t mr_seam_signer;                             /* ( 72) Measurement of SEAM module signer. (Not populated for Intel SEAM modules) */
    tee_attributes_t  attributes;                                 /* (120) Additional configuration attributes.(Not populated for Intel SEAM modules) */
    tee_tcb_svn_t     tee_tcb_svn2;                               /* (128) Array of TEE TCB SVNs (for TD preserving). */
    uint8_t           reserved[TD_TEE_TCB_INFO_RESERVED_BYTES_V1_5];/* (144) Reserved, must be zero */
} tee_tcb_info_v1_5_t;

/** The quote header.  It is designed to compatible with earlier versions of the quote. */
typedef sgx_quote4_header_t  sgx_quote5_header_t;

/** SGX Report2 body for quote v5 */
typedef struct _sgx_report2_body_v1_5_t {
    tee_tcb_svn_t       tee_tcb_svn;          ///<  0:  TEE_TCB_SVN Array
    tee_measurement_t   mr_seam;              ///< 16:  Measurement of the SEAM module
    tee_measurement_t   mrsigner_seam;        ///< 64:  Measurement of a 3rd party SEAM module’s signer (SHA384 hash).
                                              ///       The value is 0’ed for Intel SEAM module
    tee_attributes_t    seam_attributes;      ///< 112: MBZ: TDX 1.0
    tee_attributes_t    td_attributes;        ///< 120: TD's attributes
    tee_attributes_t    xfam;                 ///< 128: TD's XFAM
    tee_measurement_t   mr_td;                ///< 136: Measurement of the initial contents of the TD
    tee_measurement_t   mr_config_id;         ///< 184: Software defined ID for non-owner-defined configuration on the guest TD. e.g., runtime or OS configuration
    tee_measurement_t   mr_owner;             ///< 232: Software defined ID for the guest TD's owner
    tee_measurement_t   mr_owner_config;      ///< 280: Software defined ID for owner-defined configuration of the guest TD, e.g., specific to the workload rather than the runtime or OS
    tee_measurement_t   rt_mr[4];             ///< 328: Array of 4(TDX1: NUM_RTMRS is 4) runtime extendable measurement registers
    tee_report_data_t   report_data;          ///< 520: Additional report data
    tee_tcb_svn_t       tee_tcb_svn2;         ///< 584: Array of TEE TCB SVNs (for TD preserving).
    tee_measurement_t   mr_servicetd;         ///< 600: If is one or more bound or pre-bound service TDs, SERVTD_HASH is the SHA384 hash of the TDINFO_STRUCTs of those service TDs bound.
                                              ///       Else, SERVTD_HASH is 0..
}sgx_report2_body_v1_5_t;

/** The generic TD quote data structure.  This is the common part of the quote.  The signature_data[] contains the signature and supporting
 *  information of the key used to sign the quote and the contents depend on the sgx_quote_sign_type_t value. */
typedef struct _sgx_quote5_t {
    sgx_quote5_header_t header;               ///< 0:   The quote header.
    uint16_t type;                            ///< 48: Determines type of Quote body (TEE report)
                                              ///      Architecturally supported values:
                                              ///      1 (SGX Enclave Report)
                                              ///      2 (TD Report for TDX 1.0)
                                              ///      3 (TD Report for TDX 1.5)
    uint32_t size;                            ///< 50: Size of Quote Body field.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4200)
#endif
    uint8_t body[]; ///< 54: Data conveyed as Quote Body. Its content depends on the value of Quote Body Type
                    ///     1 Byte array that contains SGX Enclave Report.
                    ///       sgx_report_body_t + (uint32_t)signature_data_len + signature
                    ///     2 Byte array that contains TD Report for TDX 1.0.
                    ///       sgx_report2_body_t + (uint32_t)signature_data_len + signature
                    ///     3 Byte array that contains TD Report for TDX 1.5.
                    ///       sgx_report2_body_v1_5_t + (uint32_t)signature_data_len + signature
#ifdef _MSC_VER
#pragma warning(pop)
#endif
} sgx_quote5_t;

#pragma pack(pop)

#endif //_SGX_QUOTE_5_H_
