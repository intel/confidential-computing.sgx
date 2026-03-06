#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../../buildenv.mk

CPPFLAGS += -I$(COMMON_DIR)/inc/           \
            -I$(COMMON_DIR)/inc/internal/

CFLAGS += -D_GNU_SOURCE -fpic -Werror=format-security
LDLIBS += -ldl

ifdef DEBUG
    CFLAGS += -DSE_DEBUG=1
endif

OBJ1 := se_memory.o se_trace.o
OBJ2 := se_ptrace.o ptrace_version.o

OBJS := $(OBJ1) $(OBJ2)


GDB_PLUGINS_FILES := gdb-sgx-plugin/sgx_emmt.py         \
                     gdb-sgx-plugin/gdb_sgx_cmd         \
                     gdb-sgx-plugin/gdb_sgx_plugin.py   \
                     gdb-sgx-plugin/load_symbol_cmd.py  \
                     gdb-sgx-plugin/readelf.py          \
                     gdb-sgx-plugin/sgx-gdb             \
                     gdb-sgx-plugin/printers.py

GDB_OUT_DIR := $(BUILD_DIR)/gdb-sgx-plugin
TARGET      := libsgx_ptrace.so


.PHONY: all
all: $(TARGET) install

.PHONY:install
install: $(TARGET) | $(GDB_OUT_DIR) $(GDB_PLUGINS_FILES)
	$(CP) $(GDB_PLUGINS_FILES) $(GDB_OUT_DIR)
	$(CP) $(TARGET) $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDLIBS) $(COMMON_LDFLAGS) -Wl,--gc-sections

$(OBJ1): %.o: $(COMMON_DIR)/src/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(OBJ2): %.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(GDB_OUT_DIR):
	@$(MKDIR) $@

$(BUILD_DIR):
	@$(MKDIR) $@

.PHONY: clean
clean:
	@$(RM) $(TARGET) $(OBJS) $(BUILD_DIR)/$(TARGET)
	@$(RM) -r $(GDB_OUT_DIR)
