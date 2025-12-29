/*
 * Copyright(c) 2011-2025 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SE_VERSION_H_
#define _SE_VERSION_H_

#define STRFILEVER    "2.27.100.1"
#define SGX_MAJOR_VERSION       2
#define SGX_MINOR_VERSION       27
#define SGX_REVISION_VERSION    100
#define MAKE_VERSION_UINT(major,minor,rev)  (((uint64_t)major)<<32 | ((uint64_t)minor) << 16 | rev)
#define VERSION_UINT        MAKE_VERSION_UINT(SGX_MAJOR_VERSION, SGX_MINOR_VERSION, SGX_REVISION_VERSION)

#define COPYRIGHT      "Copyright (C) 2025 Intel Corporation"

#define UAE_SERVICE_VERSION       "2.3.225.0"
#define URTS_VERSION              "2.0.109.0"
#define ENCLAVE_COMMON_VERSION    "1.2.109.0"
#define LAUNCH_VERSION            "1.0.127.0"
#define EPID_VERSION              "1.0.127.0"
#define QUOTE_EX_VERSION          "1.1.127.0"

#define PCE_VERSION               "1.25.100.1"
#define LE_VERSION                "1.25.100.1"
#define QE_VERSION                "1.25.100.1"
#define PVE_VERSION               "1.25.100.1"

#endif
