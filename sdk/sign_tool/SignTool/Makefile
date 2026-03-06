#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../../buildenv.mk

FLAGS = -fpie -Werror $(CET_FLAGS)

ifdef DEBUG
FLAGS += -DSE_DEBUG_LEVEL=SE_TRACE_DEBUG
else
FLAGS += -DSE_DEBUG_LEVEL=SE_TRACE_ERROR
endif
CFLAGS += $(FLAGS)
CXXFLAGS += $(FLAGS)
LDFLAGS := -pie $(COMMON_LDFLAGS) -Wno-odr

PREBUILT_OPENSSL_DIR := $(LINUX_EXTERNAL_DIR)/dcap_source/prebuilt/openssl
CRYPTO_LIB := -L$(PREBUILT_OPENSSL_DIR)/lib/linux64 -lcrypto

INC += $(ADDED_INC)
INC +=  -I$(COMMON_DIR)/inc                         \
        -I$(COMMON_DIR)/inc/internal                \
        -I$(COMMON_DIR)/inc/internal/linux/         \
        -I$(LINUX_PSW_DIR)/enclave_common           \
        -I$(LINUX_PSW_DIR)/urts                     \
        -I$(LINUX_PSW_DIR)/urts/linux               \
        -I$(LINUX_EXTERNAL_DIR)/tinyxml2            \
        -I$(LINUX_PSW_DIR)/urts/parser              \
        -I$(PREBUILT_OPENSSL_DIR)/inc

DIR1 := $(LINUX_EXTERNAL_DIR)/tinyxml2/
DIR2 := $(COMMON_DIR)/src/
DIR3 := $(LINUX_PSW_DIR)/urts/
DIR4 := $(LINUX_PSW_DIR)/urts/linux
DIR5 := $(LINUX_PSW_DIR)/urts/parser
DIR6 := $(LINUX_PSW_DIR)/psw/urts/parser/linux

OBJ1 := manage_metadata.o      \
        enclave_creator_sign.o \
        sign_tool.o            \
        parse_key_file.o       \
        util_st.o

OBJ2 := sgx_memset_s.o crypto_evp_digest.o crypto_rsa_key.o crypto_rsa3072.o

OBJ3 := tinyxml2.o

OBJ4 := loader.o     \
        se_detect.o  \
        shared_object_parser.o

CPP_OBJS := $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)
C_OBJS   := se_trace.o se_map.o
OBJS     := $(CPP_OBJS) $(C_OBJS)

vpath %.cpp $(DIR1):$(DIR2):$(DIR3):$(DIR4):$(DIR5):$(DIR6)

.PHONY: all
all: sgx_sign | $(BUILD_DIR)
	@$(CP) $< $|

$(BUILD_DIR):
	@$(MKDIR) $@

sgx_sign: PRIVATE_LDLIBS  := -lpthread -lenclaveparser $(CRYPTO_LIB) -ldl
sgx_sign: PRIVATE_LDFLAGS := -L$(LINUX_PSW_DIR)/urts/parser $(LDFLAGS)

sgx_sign: $(OBJS) enclaveparser
	$(CXX) $(CXXFLAGS) $(OBJS) $(PRIVATE_LDFLAGS) $(PRIVATE_LDLIBS) -o $@

$(CPP_OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(C_OBJS):   %.o: $(DIR2)%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

.PHONY: clean
clean:
	$(RM) *~ *.o sgx_sign $(BUILD_DIR)/sgx_sign
	$(MAKE) -C $(LINUX_PSW_DIR)/urts/parser/ clean

.PHONY: enclaveparser
enclaveparser:
	$(MAKE) -C $(LINUX_PSW_DIR)/urts/parser/ SIGN_TOOL=1
