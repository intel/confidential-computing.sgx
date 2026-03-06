#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../buildenv.mk

CFLAGS   += $(ENCLAVE_CFLAGS) -Werror=format-security
CPPFLAGS += $(ADDED_INC) -Werror=format-security		\
            -I$(COMMON_DIR)/inc/tlibc/

SRCS := $(wildcard *.c)
OBJS := $(sort $(SRCS:.c=.o))

TARGET  := libcompiler-rt.a

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcsD $@ $^

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	@$(RM) $(OBJS) $(TARGET)
