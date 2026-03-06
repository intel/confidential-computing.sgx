#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../buildenv.mk

CFLAGS   += $(ENCLAVE_CFLAGS) -Werror=format-security -D_TLIBC_GNU_
ASFLAGS  := $(CFLAGS)
CFLAGS   += -std=c99
CXXFLAGS += $(ENCLAVE_CXXFLAGS) -fno-exceptions -fno-rtti -Werror=format-security

CPPFLAGS += -I.                          \
            -I$(COMMON_DIR)/inc          \
            -I$(COMMON_DIR)/inc/tlibc    \
            -I$(COMMON_DIR)/inc/internal \
            -I$(LINUX_SDK_DIR)/trts      \
            -I./string/asm               \
            -I./gdtoa

LIBM_PATH   := $(LINUX_EXTERNAL_DIR)/libm/Libm_ct_Linux
LIBIRC_PATH := $(LINUX_EXTERNAL_DIR)/libirc/efi2
SVML_PATH   := $(LINUX_EXTERNAL_DIR)/libm/SVML_Intel64_Linux

ifeq ($(USE_STRING_LIB), 0)
CFLAGS += -D_TLIBC_USE_INTEL_FAST_STRING_
CXXFLAGS += -D_TLIBC_USE_INTEL_FAST_STRING_
endif

ifeq ($(USE_MATH_LIB), 0)
C_SRCS := $(wildcard gen/*.c gdtoa/*.c locale/*.c math/opt/*.c stdlib/*.c string/*.c stdio/*.c time/*.c)
LIBC_C_SRCS := $(filter-out $(wildcard gen/fpclassify*.c gen/isfinite*.c gen/isinf*.c gen/isnan*.c gen/isnormal*.c gen/signbit*.c), $(C_SRCS))
LIBC_CPP_SRCS := $(wildcard gen/*.cpp) tstdc_version.cpp
LIBC_ASM_SRCS := $(wildcard gen/*.S string/asm/*.S)
else
LIBC_C_SRCS := $(wildcard gen/*.c gdtoa/*.c locale/*.c math/*.c stdlib/*.c string/*.c stdio/*.c time/*.c) 
LIBC_CPP_SRCS := $(wildcard gen/*.cpp) tstdc_version.cpp
LIBC_ASM_SRCS := $(wildcard gen/*.S math/*.S string/asm/*.S)
endif

LIBC_OBJS := $(LIBC_C_SRCS:.c=.o)
LIBC_OBJS += $(LIBC_CPP_SRCS:.cpp=.o)
LIBC_OBJS += $(LIBC_ASM_SRCS:.S=.o)
LIBC_OBJS := $(sort $(LIBC_OBJS))

LIBC_NAME    := libtlibc.a

.PHONY: all
all: $(LIBC_NAME) | $(BUILD_DIR) 

$(LIBC_NAME): $(LIBC_OBJS)
	$(AR) rcs $@ $^
ifeq ($(USE_STRING_LIB), 0)
	$(MKDIR) $(BUILD_DIR)/.libirc
	$(RM) -f $(BUILD_DIR)/.libirc/* && cd $(BUILD_DIR)/.libirc && $(AR) x $(LIBIRC_PATH)/libirc.a
	$(AR) qD $@ $(BUILD_DIR)/.libirc/*.o
	@$(RM) -rf $(BUILD_DIR)/.libirc
endif
ifeq ($(USE_MATH_LIB), 0)
	$(MKDIR) $(BUILD_DIR)/.libimf  $(BUILD_DIR)/.libsvml
	$(RM) -f $(BUILD_DIR)/.libimf/* && cd $(BUILD_DIR)/.libimf && $(AR) x $(LIBM_PATH)/libimf.a
	$(RM) -f $(BUILD_DIR)/.libsvml/* && cd $(BUILD_DIR)/.libsvml && $(AR) x $(SVML_PATH)/libsvml.a
	$(AR) qD $@ $(BUILD_DIR)/.libimf/*.o
	$(AR) qD $@ $(BUILD_DIR)/.libsvml/*.o
	@$(RM) -rf $(BUILD_DIR)/.libimf $(BUILD_DIR)/.libsvml
endif

string/consttime_memequal.o: string/consttime_memequal.c
	$(CC)  $(filter-out -O2,$(CFLAGS)) -O1 $(CPPFLAGS) -c $< -o $@
	
%.o: %.c
	$(CC)  $(CFLAGS)   $(CPPFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.S
	$(CC)  $(ASFLAGS)  $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR):
	$(MKDIR) $@

.PHONY: clean
clean:
	@$(RM) $(LIBC_NAME) $(LIBC_OBJS) math/opt/*.o
