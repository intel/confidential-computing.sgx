#!/usr/bin/env bash
#
# Copyright(c) 2011-2026 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause
#

set -e

SCRIPT_DIR=$(dirname "$0")
ROOT_DIR="${SCRIPT_DIR}/../../../../"
LINUX_INSTALLER_DIR="${ROOT_DIR}/linux/installer"
LINUX_INSTALLER_COMMON_DIR="${LINUX_INSTALLER_DIR}/common"

INSTALL_PATH=${SCRIPT_DIR}/output

# Cleanup
rm -fr ${INSTALL_PATH}

# Get the configuration for this package
source ${SCRIPT_DIR}/installConfig

# Fetch the gen_source script
cp ${LINUX_INSTALLER_COMMON_DIR}/gen_source/gen_source.py ${SCRIPT_DIR}

# Copy the files according to the BOM
python ${SCRIPT_DIR}/gen_source.py --bom=BOMs/libsgx-ae-pce.txt --cleanup=false --installdir=pkgroot/libsgx-ae-pce
python ${SCRIPT_DIR}/gen_source.py --bom=BOMs/libsgx-aesm-ecdsa-plugin.txt --cleanup=false --installdir=pkgroot/libsgx-aesm-ecdsa-plugin
python ${SCRIPT_DIR}/gen_source.py --bom=BOMs/libsgx-aesm-pce-plugin.txt --cleanup=false --installdir=pkgroot/libsgx-aesm-pce-plugin
python ${SCRIPT_DIR}/gen_source.py --bom=BOMs/libsgx-aesm-quote-ex-plugin.txt --cleanup=false --installdir=pkgroot/libsgx-aesm-quote-ex-plugin
python ${SCRIPT_DIR}/gen_source.py --bom=BOMs/sgx-aesm-service.txt --cleanup=false --installdir=pkgroot/sgx-aesm-service
python ${SCRIPT_DIR}/gen_source.py --bom=BOMs/sgx-aesm-service-package.txt --cleanup=false
python ${SCRIPT_DIR}/gen_source.py --bom=../licenses/BOM_license.txt --cleanup=false

# Create the tarball
PCE_VERSION=$(awk '/PCE_VERSION/ {print $3}' ${ROOT_DIR}/common/inc/internal/se_version.h|sed 's/^\"\(.*\)\"$/\1/')
URTS_VERSION=$(awk '/URTS_VERSION/ {print $3}' ${ROOT_DIR}/common/inc/internal/se_version.h|sed 's/^\"\(.*\)\"$/\1/')
QE3_VERSION=$(awk '/QE3_VERSION/ {print $3}' ${ROOT_DIR}/external/dcap_source/QuoteGeneration/common/inc/internal/se_version.h|sed 's/^\"\(.*\)\"$/\1/')
pushd ${INSTALL_PATH} &> /dev/null
sed -i "s/PCE_VER=.*/PCE_VER=${PCE_VERSION}/" Makefile
sed -i "s/URTS_VER=.*/URTS_VER=${URTS_VERSION}/" Makefile
sed -i "s/QE3_VER=.*/QE3_VER=${QE3_VERSION}/" Makefile
tar -zcvf ${TARBALL_NAME} *
popd &> /dev/null
