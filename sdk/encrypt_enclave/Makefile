#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../buildenv.mk
ifndef VERBOSE
	PCLVERBOSE := @
else
	PCLVERBOSE := 
endif

INC_DIR	:= -I$(COMMON_DIR)/inc -I$(COMMON_DIR)/inc/internal -I.
CXXFLAGS += $(INC_DIR) -Wno-attributes -g -mrdrnd -fpie -Werror=format-security
LDFLAGS := -pie $(COMMON_LDFLAGS)

LINK_FLAGS := -lcrypto -L$(BUILD_DIR) -lsgx_tservice
CPP_FILES := encryptip.cpp
OBJS := $(CPP_FILES:.cpp=.o)
TOOL_NAME := sgx_encrypt

.PHONY: all

all: $(TOOL_NAME) | $(BUILD_DIR)
	$(PCLVERBOSE)$(CP) $< $|

$(BUILD_DIR):
	@$(MKDIR) $@

$(OBJS): $(CPP_FILES)
	$(PCLVERBOSE)$(CXX) -c $< -o $@ $(CXXFLAGS)
	@echo "CXX  =>  $@"

$(TOOL_NAME): $(OBJS)
	$(PCLVERBOSE)$(CXX) $^ -o $@ $(LINK_FLAGS) $(LDFLAGS)
	@echo "LINK =>  $@"

.PHONY: clean
clean:
	$(PCLVERBOSE)$(RM) $(TOOL_NAME) $(OBJS) $(BUILD_DIR)/$(TOOL_NAME)

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) all

