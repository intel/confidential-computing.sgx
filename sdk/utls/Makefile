#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

TOP_DIR  = ../..
include $(TOP_DIR)/buildenv.mk

CERT_UTIL_DIR   := ../ttls
SGX_QV_PATH ?= ../../external/dcap_source/QuoteVerification
SGX_QVE_PATH ?= ../../external/dcap_source/ae/QvE
PREBUILD_OPENSSL_PATH ?= $(SGX_QV_PATH)/../prebuilt/openssl
CBOR_PATH ?= $(LINUX_EXTERNAL_DIR)/cbor
LIBCBOR_PATH := $(CBOR_PATH)/untrusted/lib
CBOR_INCLUDE := $(CBOR_PATH)/untrusted/include

INCLUDE += -I.
INCLUDE += -I$(COMMON_DIR)              \
           -I$(COMMON_DIR)/inc          \
           -I$(COMMON_DIR)/inc/internal \
           -I$(PREBUILD_OPENSSL_PATH)/inc \
           -I$(CBOR_PATH)/untrusted/include \
           -I$(CERT_UTIL_DIR) \
           -I$(SGX_QVE_PATH)/Include \
           -I$(SGX_QV_PATH)/dcap_quoteverify/inc \
           -I$(SGX_QV_PATH)/../QuoteGeneration/quote_wrapper/common/inc \
           -I$(SGX_QV_PATH)/../QuoteGeneration/quote_wrapper/ql/inc


CXXFLAGS += $(COMMON_LDFLAGS) -fPIC -Werror -Wno-unused-parameter -g


vpath %.cpp .. $(COMMON_DIR)/src $(CERT_UTIL_DIR)

SRC :=  utls.cpp

LIBCBOR := libcbor.a

HOST_CERT_SRC := $(CERT_UTIL_DIR)/cert_verifier.cpp

OBJ := $(SRC:.cpp=.o)

CERT_OBJ := $(HOST_CERT_SRC:.cpp=.o)

LIBNAME  = libsgx_utls.a

CBOR_HEADER := cbor.h

.PHONY: all cbor_untrusted
all: $(LIBNAME) | $(BUILD_DIR)
	$(CP) $(LIBNAME) $|

$(LIBNAME): $(OBJ) $(CERT_OBJ)
	$(AR) rs $@ $(OBJ) $(CERT_OBJ)
	$(MKDIR) $(BUILD_DIR)/.libcbor
	$(RM) $(BUILD_DIR)/.libcbor/* && cd $(BUILD_DIR)/.libcbor && $(AR) x $(LIBCBOR_PATH)/$(LIBCBOR)
	$(AR) rs $@ $(BUILD_DIR)/.libcbor/*.o
	@$(RM) -r $(BUILD_DIR)/.libcbor

$(OBJ): cbor_untrusted $(SRC)
	$(CXX) $(CXXFLAGS) -Wno-deprecated-declarations $(INCLUDE) -c $(SRC) -o $@

$(CERT_OBJ): $(HOST_CERT_SRC)
	$(CXX) $(CXXFLAGS) -Wno-deprecated-declarations $(INCLUDE) -c $< -o $@

$(HOST_CERT_SRC): cbor_untrusted

cbor_untrusted: 
	# before build utls, check cbor untrusted  
	@if [ ! -f $(CBOR_INCLUDE)/$(CBOR_HEADER) ] || [ ! -f $(LIBCBOR_PATH)/$(LIBCBOR) ]; then \
		$(MAKE) -C $(CBOR_PATH) cbor_untrusted; \
	fi

.PHONY: clean
clean:
	@$(RM) $(OBJ) $(CERT_OBJ)
	@$(RM) $(LIBNAME) $(BUILD_DIR)/$(LIBNAME)
	$(MAKE) -C $(CBOR_PATH) clean

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) all
