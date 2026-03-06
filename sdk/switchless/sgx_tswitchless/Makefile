#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../../buildenv.mk

# SL = Switch-Less
SL_DIR := $(LINUX_SDK_DIR)/switchless
SL_DIR_COMMON := $(SL_DIR)/common
SL_DIR_SRCS := $(SL_DIR)/sgx_tswitchless

vpath %_common.c $(SL_DIR_COMMON)

EDGER8R_DIR = $(LINUX_SDK_DIR)/edger8r/linux
EDGER8R = $(EDGER8R_DIR)/_build/Edger8r.native


# Files that do NOT match *_untrusted.c or *_untrusted.S are trusted components
C_SRCS := $(wildcard $(SL_DIR_SRCS)/*.c)
C_OBJS := $(C_SRCS:.c=.o)

ALL_OBJS := $(C_OBJS)

# Compiler flags
SL_CFLAGS := $(CFLAGS)
SL_CFLAGS += $(ENCLAVE_CFLAGS)
SL_CFLAGS += -DSL_INSIDE_ENCLAVE
SL_CFLAGS += -std=c11
SL_CFLAGS += -Werror -Wno-redundant-decls
SL_CFLAGS += -I$(COMMON_DIR)/inc \
             -I$(COMMON_DIR)/inc/internal \
             -I$(COMMON_DIR)/inc/tlibc \
             -I$(LINUX_SDK_DIR)/trts \
             -I$(SL_DIR)/inc

SL_CXXFLAGS := $(CXXFLAGS)
SL_CXXFLAGS += $(ENCLAVE_CXXFLAGS)
SL_CXXFLAGS += -DSL_INSIDE_ENCLAVE
SL_CXXFLAGS += -I$(COMMON_DIR)/inc \
               -I$(COMMON_DIR)/inc/internal \
               -I$(COMMON_DIR)/inc/tlibc \
               -I$(LINUX_SDK_DIR)/trts \
               -I$(SL_DIR)/inc

ifdef DEBUG
	SL_CFLAGS += -DSL_DEBUG
	SL_CXXFLAGS += -DSL_DEBUG
endif


# Primary build targets
OUT_LIB := libsgx_tswitchless.a

GREEN :=\033[1;32m
NC :=\033[0m

.PHONY: all build_src clean
all: build_src $(BUILD_DIR)/$(OUT_LIB)

build_src: $(OUT_LIB)

$(OUT_LIB): $(ALL_OBJS)
	@$(AR) rcs $@ $^
	@echo "$(GREEN)LINK => $@$(NC)"

$(BUILD_DIR)/$(OUT_LIB): $(OUT_LIB) | $(BUILD_DIR)
	$(CP) $(OUT_LIB) $(BUILD_DIR)/

$(C_OBJS): %.o: %.c sgx_tswitchless_t.h
	@$(CXX) $(SL_CXXFLAGS) -c $< -o $@
	@echo "$(CXX) <= $@"


sgx_tswitchless_t.h: $(EDGER8R)
	$(EDGER8R) --header-only --trusted $(COMMON_DIR)/inc/sgx_tswitchless.edl --search-path $(COMMON_DIR)/inc

$(EDGER8R):
	$(MAKE) -C $(EDGER8R_DIR)


clean:
	@$(RM) -f $(OUT_LIB) $(ALL_OBJS) $(BUILD_DIR)/$(OUT_LIB) sgx_tswitchless_t.h


