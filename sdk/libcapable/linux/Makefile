#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include ../../../buildenv.mk

ifndef DEBUG
CXXFLAGS += -DDISABLE_TRACE
CFLAGS += -DDISABLE_TRACE
endif

CXXFLAGS += -fPIC -Werror $(CET_FLAGS)
CFLAGS += -fPIC -Werror $(CET_FLAGS)
LDFLAGS += $(COMMON_LDFLAGS)

INC += -I$(COMMON_DIR)/inc                \
       -I$(COMMON_DIR)/inc/internal       \
       -I$(COMMON_DIR)/inc/internal/linux 

LIB += -L$(BUILD_DIR)

LIBCAPABLE := libsgx_capable.so
LIBCAPABLE_STATIC := libsgx_capable.a

OBJ := sgx_capable.o sgx_capable_version.o

.PHONY: all
all: $(LIBCAPABLE) $(LIBCAPABLE_STATIC) | $(BUILD_DIR)
	@$(CP) $(LIBCAPABLE)          $|
	@$(CP) $(LIBCAPABLE_STATIC)   $|

$(LIBCAPABLE): $(OBJ) 
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname=$@ $(LIB) -o $@  $(OBJ) $(LDFLAGS)

$(LIBCAPABLE_STATIC): $(OBJ) 
	$(AR) r $@ $?

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@

$(BUILD_DIR):
	@$(MKDIR) $@

.PHONY: clean
clean::
	@$(RM) *.o $(LIBCAPABLE) $(BUILD_DIR)/$(LIBCAPABLE)
	@$(RM) $(LIBCAPABLE_STATIC) $(BUILD_DIR)/$(LIBCAPABLE_STATIC)
