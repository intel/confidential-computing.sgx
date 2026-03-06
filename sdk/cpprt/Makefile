#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../buildenv.mk

LINUXDIR       := linux
OBJDIR         := obj
CPPRT          := libcpprt.a
STAMP          := .stamp
LOCKFILE       := .lockfile
LIBCXXRT_LIB   := $(LINUX_EXTERNAL_DIR)/libcxxrt/libcxxrt.a
LIBCXXRT_STAMP := $(OBJDIR)/.stamp

CFLAGS   += $(ENCLAVE_CFLAGS) -Werror=format-security
CXXFLAGS += $(ENCLAVE_CXXFLAGS) -Werror=format-security

CPPFLAGS += -I$(COMMON_DIR)/inc           \
            -I$(COMMON_DIR)/inc/tlibc     \
            -I$(COMMON_DIR)/inc/stdc++    \
            -I$(COMMON_DIR)/inc/internal  \

CPPFLAGS += -I$(LINUX_SDK_DIR)/trts       \
            -I$(LINUX_SDK_DIR)/trts/linux \

# DLIBCXXRT_NO_DEFAULT_TERMINATE_DIAGNOSTICS: diagnostics are written using printf but printf is not available in sgx_tcxx
# LIBCXXRT_NO_EMERGENCY_MALLOC: if enabled sgx_tcxx will depend on sgx_pthread
CFLAGS   += -DLIBCXXRT_NO_DEFAULT_TERMINATE_DIAGNOSTICS -DLIBCXXRT_NO_EMERGENCY_MALLOC
CXXFLAGS += -DLIBCXXRT_NO_DEFAULT_TERMINATE_DIAGNOSTICS -DLIBCXXRT_NO_EMERGENCY_MALLOC

CPP_SRCS := $(wildcard memory_manage/new*.cpp) \
            $(wildcard memory_manage/delete*.cpp)
CPP_SRCS += tcxx_version.cpp

CC_SRCS  := $(wildcard $(LINUXDIR)/*.cc)
C_SRCS   := $(wildcard $(LINUXDIR)/*.c)

CPP_OBJS := $(CPP_SRCS:.cpp=.o)
CC_OBJS  := $(CC_SRCS:.cc=.o)
C_OBJS   := $(C_SRCS:.c=.o)

OBJS := $(sort $(CPP_OBJS) $(C_OBJS) $(CC_OBJS))

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

$(LIBCXXRT_STAMP): $(OBJDIR) $(LIBCXXRT_LIB)
	$(RM) $(OBJDIR)/*.o
	cd $(OBJDIR) && $(AR) x $(LIBCXXRT_LIB)
	touch $(STAMP)

$(C_OBJS): %.o: %.c
	$(CC) -c  $(CFLAGS)   $(CPPFLAGS) $< -o $@

$(CC_OBJS): %.o: %.cc
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

$(CPP_OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@


.PHONY: all
all: $(CPPRT)

.PHONY: prepare-libunwind
prepare-libunwind:
	$(CP) linux/pthread_compat.h $(LIBUNWIND_DIR)/include

$(CPPRT): $(OBJS) $(LIBCXXRT_STAMP) prepare-libunwind libunwind
	$(CP) $(LIBUNWIND_DIR)/src/.libs/libunwind.a $(CPPRT)
	@LIBCXXRT_OBJ="$(wildcard $(OBJDIR)/*.o)"; \
	$(AR) rs $@ $(OBJS) $$LIBCXXRT_OBJ

.PHONY: libunwind
libunwind:
	cd $(LIBUNWIND_DIR)/ && \
		( test -f Makefile || CFLAGS="$(CFLAGS)" ./autogen.sh ) && \
		$(MAKE) -j$(shell nproc)

.PHONY: clean
clean:
	@$(RM) $(OBJS) $(CPPRT)
ifeq ("$(shell test -f $(LIBUNWIND_DIR)/Makefile && echo Makefile exists)", "Makefile exists")
	@$(MAKE) -C $(LIBUNWIND_DIR) clean
endif
	@$(RM) -r $(LIBUNWIND_DIR)/INSTALL             \
              $(LIBUNWIND_DIR)/Makefile.in         \
              $(LIBUNWIND_DIR)/aclocal.m4          \
              $(LIBUNWIND_DIR)/autom4te.cache/     \
              $(LIBUNWIND_DIR)/config/             \
              $(LIBUNWIND_DIR)/configure           \
              $(LIBUNWIND_DIR)/doc/Makefile.in     \
              $(LIBUNWIND_DIR)/include/pthread_compat.h \
              $(LIBUNWIND_DIR)/src/Makefile.in   \
              $(LIBUNWIND_DIR)/Makefile          \
              $(LIBUNWIND_DIR)/m4                \
              $(OBJDIR)

