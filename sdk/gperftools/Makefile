#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../buildenv.mk

TCMALLOC := libsgx_tcmalloc.a
SRCDIR := gperftools-2.7

ifdef DEBUG
    DEBUG_FLAG := DEBUG
else
    DEBUG_FLAG := NDEBUG
endif

CFLAGS += -Werror=format-security
CXXFLAGS += -Werror=format-security

.PHONY: all
all: $(TCMALLOC) | $(BUILD_DIR)
	$(CP) $(TCMALLOC) $|

.PHONY: libtcmalloc
libtcmalloc:
	cd $(SRCDIR) && \
		( test -f Makefile || \
			CFLAGS="$(CFLAGS)" \
			CXXFLAGS="$(CXXFLAGS)" \
			ENCLAVE_CFLAGS="$(ENCLAVE_CFLAGS)" \
			ENCLAVE_CXXFLAGS="$(ENCLAVE_CXXFLAGS)" ./autogen.sh $(DEBUG_FLAG) ) \
		&& $(MAKE)

$(TCMALLOC): libtcmalloc
	$(CP) $(SRCDIR)/.libs/libtcmalloc_minimal.a $(TCMALLOC)

$(BUILD_DIR):
	@$(MKDIR) $@

.PHONY: clean
clean:
	@test ! -f $(SRCDIR)/Makefile || $(MAKE) -C $(SRCDIR) clean
	@$(RM) -r $(SRCDIR)/Makefile $(TCMALLOC) $(BUILD_DIR)/$(TCMALLOC)
