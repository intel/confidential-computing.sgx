#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../buildenv.mk

CFLAGS   += $(ENCLAVE_CFLAGS) -D _LIBSAFECRT_SGX_CONFIG -Werror=format-security
CFLAGS   += -std=c99
CXXFLAGS += $(ENCLAVE_CXXFLAGS) -Werror=format-security

CPPFLAGS += -I.                          \
            -I$(COMMON_DIR)/inc          \
            -I$(COMMON_DIR)/inc/tlibc    \
            -I$(COMMON_DIR)/inc/internal \
            -I$(LINUX_SDK_DIR)/tsafecrt/pal/inc      \


ifneq ($(ARCH), x86)
CFLAGS += -D _WIN64
endif

LIB_NAME = libsgx_tsafecrt.a
LIBC_SRCS := $(wildcard pal/src/safecrt/*.c)
LIBCPP_SRCS := $(wildcard pal/src/*.cpp)

LIB_OBJS := $(LIBC_SRCS:.c=.o)
LIB_OBJS += $(LIBCPP_SRCS:.cpp=.o)
LIB_OBJS := $(sort $(LIB_OBJS))


.PHONY: all
all: $(LIB_NAME)

$(LIB_NAME): $(LIB_OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC)  $(CFLAGS)   $(CPPFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
   
.PHONY: clean
clean:
	@$(RM) $(LIB_NAME) $(LIB_OBJS)
