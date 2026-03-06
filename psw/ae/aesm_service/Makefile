#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

TOP_DIR  = ../../..
include $(TOP_DIR)/buildenv.mk

CPPMICROSERVICES_DIR := $(LINUX_EXTERNAL_DIR)/CppMicroServices
CPPMICROSERVICES_INSTALL := $(CPPMICROSERVICES_DIR)/local-install
AESM_CONFIG := -DBUILD_SHARED_LIBS=ON -DUS_BUILD_SHARED_LIBS=ON
CPPMICROSERVICES_CONFIG := -DCMAKE_INSTALL_PREFIX=$(CPPMICROSERVICES_INSTALL) -DCMAKE_BUILD_TYPE=RelWithDebInfo -DFORTIFY_SOURCE_VAL=$(FORTIFY_SOURCE_VAL)
ifdef DEBUG
    AESM_CONFIG += -DCMAKE_BUILD_TYPE=Debug
else
     AESM_CONFIG += -DCMAKE_BUILD_TYPE=Release
endif

ifeq ($(ARCH), x86)
    SQLITECFLAGS += -m32
else
    SQLITECFLAGS += -m64
endif

ifdef PROFILE
    CXXFLAGS += -D_PROFILE_
    CFLAGS += -D_PROFILE_
endif

CXXFLAGS += -fpie
CFLAGS += -fpie
CFLAGS := $(filter-out -Werror, $(CFLAGS))

AESM_CONFIG += -DCMAKE_C_FLAGS="$(CFLAGS)" -DCMAKE_CXX_FLAGS="$(CXXFLAGS)" 	\
				-DCMAKE_EXE_LINKER_FLAGS="$(COMMON_LDFLAGS) -pie"			\
               	-DCMAKE_SHARED_LINKER_FLAGS="$(COMMON_LDFLAGS)"

APPNAME         := source/build/bin/aesm_service
CPPMICROSERVICES:= $(CPPMICROSERVICES_DIR)/build/lib/libCppMicroServices.so.4.0.0
RDRAND_LIBDIR :=$(LINUX_EXTERNAL_DIR)/rdrand/src
RDRAND_MAKEFILE := $(RDRAND_LIBDIR)/Makefile

.PHONY: all
all: $(APPNAME) $(APPNAME_DEBUG) | $(BUILD_DIR)
	$(CP) -r source/build/bin/* $|

$(APPNAME): $(CPPMICROSERVICES) source/build/CMakeCache.txt urts RDRAND
	$(MAKE) -C source/build
	$(CP) $(CPPMICROSERVICES) source/build/bin/

CMAKE_VERSION_MAJOR=$(shell cmake --version | head -n  1 | cut -d \  -f 3 | cut -d . -f 1)
CMAKE_VERSION_MINOR=$(shell cmake --version | head -n  1 | cut -d \  -f 3 | cut -d . -f 2 | xargs printf '%02d')
CMAKE_VERSION=$(CMAKE_VERSION_MAJOR)$(CMAKE_VERSION_MINOR)
CMAKE := $(HOME)/cache/bin/cmake

.PHONY: CPPMICROSERVICES
$(CPPMICROSERVICES):
	mkdir -p $(CPPMICROSERVICES_DIR)/build
ifeq ($(shell test $(CMAKE_VERSION) -lt 302 && echo 1), 1)
	$(CPPMICROSERVICES_DIR)/install_cmake.sh
	cd $(CPPMICROSERVICES_DIR)/build && $(CMAKE) -DCMAKE_COMMAND=$(CMAKE) $(CPPMICROSERVICES_CONFIG) ../ && $(MAKE) && $(MAKE) install
else
	cd $(CPPMICROSERVICES_DIR)/build && cmake $(CPPMICROSERVICES_CONFIG) ../ && $(MAKE) && $(MAKE) install
endif

source/build/CMakeCache.txt: $(CPPMICROSERVICES)
ifeq ($(shell test $(CMAKE_VERSION) -lt 300 && echo 1), 1)
	mkdir -p source/build && cd source/build && $(CMAKE) -DCMAKE_COMMAND=$(CMAKE) $(AESM_CONFIG) ../
else
	mkdir -p source/build && cd source/build && cmake $(AESM_CONFIG) ../
endif

RDRAND: $(RDRAND_MAKEFILE)
	$(MAKE) -C $(RDRAND_LIBDIR)

$(RDRAND_MAKEFILE):
ifeq ($(ARCH), x86)
	@cd $(RDRAND_LIBDIR);./configure
else
	@cd $(RDRAND_LIBDIR);./configure CFLAGS=-fPIC
endif

.PHONY: urts
urts:
	$(MAKE) -C $(LINUX_PSW_DIR)/urts/linux

$(BUILD_DIR):
	@$(MKDIR) $@

.PHONY: clean
clean:
	@$(RM) -r source/build
	@$(RM) -r $(CPPMICROSERVICES_DIR)/build
	@$(RM) -r $(CPPMICROSERVICES_INSTALL)
	@$(RM) -r data
ifeq ($(RDRAND_MAKEFILE), $(wildcard $(RDRAND_MAKEFILE)))
	@$(MAKE) distclean -C $(RDRAND_LIBDIR)
endif

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) all

