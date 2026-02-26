#
# Copyright(c) 2011-2025 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

# part of SDK
# include buildenv in linux trunk
include ../../../../buildenv.mk

TVL_CPP_SRCS := $(wildcard *.cpp)
TVL_OBJS := $(TVL_CPP_SRCS:.cpp=.o)

COMMON_INCLUDE	:= -I$(COMMON_DIR)/inc -I$(COMMON_DIR)/inc/tlibc -I$(LINUX_SDK_DIR)/tlibcxx/include
TVL_INC  += $(COMMON_INCLUDE) -I../../ae/QvE/Include -I$(DCAP_DIR)/QuoteGeneration/quote_wrapper/common/inc -I$(DCAP_DIR)/QuoteGeneration/pce_wrapper/inc -I$(DCAP_DIR)/QuoteVerification/appraisal/qal


TVL_LIB  := sgx_dcap_tvl
TVL_NAME := lib$(TVL_LIB).a


ifeq ($(MITIGATION-CVE-2020-0551), LOAD)
	BUILD_DIR := $(DCAP_DIR)/QuoteGeneration/build/linuxLOAD
else ifeq ($(MITIGATION-CVE-2020-0551), CF)
	BUILD_DIR := $(DCAP_DIR)/QuoteGeneration/build/linuxCF
else
	BUILD_DIR := $(DCAP_DIR)/QuoteGeneration/build/linux
endif


ifneq ($(DEBUG), 1)
    COMMON_FLAGS += -ffunction-sections -fdata-sections
endif

ENCLAVE_CXXFLAGS += $(SGX_COMMON_CXXFLAGS) $(COMMON_FLAGS) -fPIC

.PHONY: all

all: install_lib $(TVL_NAME)

$(BUILD_DIR):
	@$(MKDIR) $@

install_lib: $(TVL_NAME) | $(BUILD_DIR)
	@$(CP) $(TVL_NAME) $|

$(TVL_OBJS): %.o: %.cpp
	$(CXX) $(ENCLAVE_CXXFLAGS) $(TVL_INC) -c $< -o $@
	@echo "CXX  <=  $<"

$(TVL_NAME): $(TVL_OBJS)
	$(AR) rsD $(TVL_NAME) $(TVL_OBJS)


.PHONY: clean

clean:
	@echo "Cleaning objects"
	@rm -f $(TVL_OBJS) $(TVL_NAME)
	@rm -f $(BUILD_DIR)/$(TVL_NAME)
