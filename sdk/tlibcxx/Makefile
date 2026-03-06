#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../buildenv.mk

CFLAGS   += $(ENCLAVE_CFLAGS) -Werror=format-security
CXXFLAGS += $(ENCLAVE_CXXFLAGS) -Werror=format-security

CPPFLAGS += -I./include \
            -I$(COMMON_DIR)/inc       \
            -I$(COMMON_DIR)/inc/stdc++ \
            -I$(COMMON_DIR)/inc/tlibc \
            -I$(COMMON_DIR)/inc/stdc++ \
            -I$(COMMON_DIR)/inc/internal \

CPPFLAGS += -I$(LINUX_SDK_DIR)/trts       \
            -I$(LINUX_SDK_DIR)/trts/linux

ifeq ($(BUILD),ANDROID)
CPPFLAGS += -I$(ROOT_DIR)/android/ndk-toolchain/sysroot/usr/include
endif   #ANDROID

CPP_SRCS := $(wildcard src/*.cpp)
CPP_OBJS := $(sort $(CPP_SRCS:.cpp=.o))

OBJS     := $(CPP_OBJS)

LIBCXX_NAME = libcxx.a

.PHONY:all
all: $(LIBCXX_NAME)

$(LIBCXX_NAME): $(OBJS)
	$(AR) rcs $(LIBCXX_NAME) $^

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@$(RM) $(LIBCXX_NAME) $(OBJS)
