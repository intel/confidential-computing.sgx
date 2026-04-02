#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../buildenv.mk

CPPFLAGS += -I$(COMMON_DIR)/inc          \
            -I$(COMMON_DIR)/inc/internal \
            -I$(COMMON_DIR)/inc/tlibc    \
            -I$(LINUX_SDK_DIR)/tlibcxx/include \
            -I$(LINUX_SDK_DIR)/pthread

CXXFLAGS += $(ENCLAVE_CXXFLAGS) \
            -Werror         \
            -fno-exceptions \
            -fno-rtti

TCXXFLAGS := $(patsubst -fstack-protector%,-fno-stack-protector,$(CXXFLAGS))

OBJS1 := init_enclave.o  \
        trts.o           \
        trts_ecall.o     \
        trts_ocall.o     \
        ema_init.o     \
        trts_util.o      \
        trts_veh.o       \
        trts_xsave.o     \
        init_optimized_lib.o \
        trts_version.o \
        trts_add_trim.o

OBJS2 := trts_nsp.o

OBJS3 := ema_rt.o

OBJS4 := ctd.o # The CTD requires AVX2

OBJS := $(OBJS1) $(OBJS2) $(OBJ3) $(OBJS4)

all: $(OBJS) elf_parser

$(OBJS1): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -fPIC $< -o $@
	objcopy $@ --set-section-flags .niprod='alloc','readonly'

$(OBJS2): %.o: %.cpp
	$(CXX) -c $(TCXXFLAGS) $(CPPFLAGS) -fPIC $< -o $@

$(OBJS3): %.o: %.c
	$(CC) -c $(TCFLAGS) $(CFLAGS) -fPIC $< -o $@

$(OBJS4): %.o: %.c
	$(CC) -mavx2 -O3 -masm=intel $(filter-out -O2,$(CFLAGS)) $(TCFLAGS) \
		-fPIC $(CPPFLAGS) -c $< -o $@

.PHONY: elf_parser
elf_parser: $(OBJS)
	$(MAKE) -C linux

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.bak *~
	$(MAKE) -C linux clean
