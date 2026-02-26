#
# Copyright(c) 2011-2026 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause
#

include installConfig

PACKAGE_ROOT_FOLDER=pkgroot
PACKAGES=$(notdir $(wildcard $(PACKAGE_ROOT_FOLDER)/*))

VAR_OPT_PATH=/var/opt/aesmd
USR_LIB_PATH=/usr/$(notdir $(shell gcc -print-multi-os-directory))/$(shell dpkg-architecture -qDEB_HOST_MULTIARCH 2> /dev/null)

AESMD_CONF_NAME=$(if $(wildcard /run/systemd/system/.*),aesmd.service,$(if $(wildcard /etc/init/.*),aesmd.conf,))
AESMD_CONF_DEL=$(if $(wildcard /run/systemd/system/.*),aesmd.conf,$(if $(wildcard /etc/init/.*),aesmd.service,))
AESMD_CONF_PATH=$(if $(wildcard /run/systemd/system/.*),$(if $(wildcard /lib/systemd/system/.*),/lib/systemd/system,/usr/lib/systemd/system),$(if $(wildcard /etc/init/.*),/etc/init/))

ifeq ($(AESMD_CONF_NAME),)
ifeq ($(BUILD_PLATFORM),docker)
AESMD_CONF_NAME=aesmd.service
AESMD_CONF_DEL=aesmd.conf
AESMD_CONF_PATH=/lib/systemd/system
$(warning "You may need to start aesmd manually after it's installed!")
else
$(error "Unsupported platform - neither systemctl nor initctl is found!")
endif
endif

QE_VER=1.0.0
PCE_VER=1.0.0
QE3_VER=1.0.0
URTS_VER:=2.0.0
SPLIT_VERSION=$(word $2,$(subst ., ,$1))

default:

install: $(PACKAGES)
	install -d $(shell readlink -m $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(VAR_OPT_PATH))
	install -d $(shell readlink -m $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESMD_CONF_PATH))
	sed -e "s:@aesm_folder@:$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm:" \
		$(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/$(AESMD_CONF_NAME) \
		> $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESMD_CONF_PATH)/$(AESMD_CONF_NAME)
	rm -f $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/$(AESMD_CONF_NAME)
	rm -f $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/$(AESMD_CONF_DEL)
	$(if $(shell grep '^ID=clear-linux-os' /usr/lib/os-release 2> /dev/null), \
		$(if $(wildcard $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/conf/aesmd.conf), \
			mv $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/conf/aesmd.conf \
				$(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/conf/aesmd.conf.sample), \
		$(if $(wildcard $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/conf/.*), \
			install -d $(shell readlink -m $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(ETC_DIR)) && \
			cp -fr $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/conf/* \
				$(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(ETC_DIR) && \
			rm -fr $(DESTDIR)/$(AESM_SERVICE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/conf))
	cd $(shell readlink -m $(DESTDIR)/$(AE_PCE_PACKAGE_NAME)/$(USR_LIB_PATH)) && \
	mv libsgx_pce.signed.so libsgx_pce.signed.so.$(PCE_VER) && \
	ln -fs libsgx_pce.signed.so.$(PCE_VER) libsgx_pce.signed.so.$(call SPLIT_VERSION,$(PCE_VER),1) && \
	ln -fs libsgx_pce.signed.so.$(call SPLIT_VERSION,$(PCE_VER),1) libsgx_pce.signed.so
	cd $(shell readlink -m $(DESTDIR)/$(AESM_ECDSA_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm) && \
	ln -fs $(USR_LIB_PATH)/libsgx_qe3.signed.so.$(call SPLIT_VERSION,$(QE3_VER),1) && \
	ln -fs $(USR_LIB_PATH)/libsgx_qe3.signed.so && \
	cd $(shell readlink -m $(DESTDIR)/$(AESM_PCE_PACKAGE_NAME)/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm) && \
	ln -fs $(USR_LIB_PATH)/libsgx_pce.signed.so.$(call SPLIT_VERSION,$(PCE_VER),1) && \
	ln -fs $(USR_LIB_PATH)/libsgx_pce.signed.so && \
	ln -fs liburts_internal.so libsgx_urts.so.$(call SPLIT_VERSION,$(URTS_VER),1)
	
$(PACKAGES):
	install -d $(shell readlink -m $(DESTDIR)/$@)
	$(if $(wildcard $(PACKAGE_ROOT_FOLDER)/$@/$(LIB_DIR)/.*), \
		install -d $(shell readlink -m $(DESTDIR)/$@/$(USR_LIB_PATH)) && \
		install $(PACKAGE_ROOT_FOLDER)/$@/$(LIB_DIR)/* $(DESTDIR)/$@/$(USR_LIB_PATH), \
		install -d $(shell readlink -m $(DESTDIR)/$@/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)) && \
		cp -r $(PACKAGE_ROOT_FOLDER)/$@/* $(DESTDIR)/$@/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)) 
	$(if $(wildcard $(PACKAGE_ROOT_FOLDER)/$@/$(ETC_DIR)/.*), \
		install -d $(shell readlink -m $(DESTDIR)/$@/$(ETC_DIR)) && \
		cp -fr $(PACKAGE_ROOT_FOLDER)/$@/$(ETC_DIR)/* $(DESTDIR)/$@/$(ETC_DIR))
	$(if $(wildcard $(PACKAGE_ROOT_FOLDER)/$@/aesm/data/.*), \
		install -d $(shell readlink -m $(DESTDIR)/$@/$(VAR_OPT_PATH)) && \
		cp -fr $(DESTDIR)/$@/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/data \
			$(DESTDIR)/$@/$(VAR_OPT_PATH) && \
		rm -fr $(DESTDIR)/$@/$(AESM_SERVICE_PACKAGE_PATH)/$(AESM_SERVICE_PACKAGE_NAME)/aesm/data)
