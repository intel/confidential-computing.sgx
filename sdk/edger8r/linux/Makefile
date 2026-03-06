#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../../buildenv.mk

.PHONY: all
all: build | $(BUILD_DIR)
	@$(CP) $(CUR_DIR)/_build/Edger8r.native $(BUILD_DIR)/sgx_edger8r

.PHONY: build
build:
	ocamlbuild -cflags -ccopt,-fpie,-ccopt,-fcf-protection -lflags -runtime-variant,_pic,-ccopt,-pie,-ccopt -lflag "-Wl,-z,relro,-z,now,-z,noexecstack" -no-links -libs str,unix Edger8r.native

$(BUILD_DIR):
	@$(MKDIR) $@

.PHONY:
clean:
	@ocamlbuild Edger8r.native -clean
	@$(RM) $(BUILD_DIR)/sgx_edger8r
