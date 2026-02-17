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
LINUX_INSTALLER_COMMON_AESM_SERVICE_DIR="${LINUX_INSTALLER_COMMON_DIR}/sgx-aesm-service"
LINUX_OS_ID=$(grep "^ID=" /usr/lib/os-release 2> /dev/null | awk -F'=' '{print $2}')

source ${LINUX_INSTALLER_COMMON_AESM_SERVICE_DIR}/installConfig
if [ "${LINUX_OS_ID}" = "clear-linux-os" ]; then
    AESM_SERVICE_PACKAGE_PATH=/usr/share
fi

SGX_VERSION=$(awk '/STRFILEVER/ {print $3}' ${ROOT_DIR}/common/inc/internal/se_version.h|sed 's/^\"\(.*\)\"$/\1/')
PACKAGE_NAMES[0]=${AESM_SERVICE_PACKAGE_NAME}
PACKAGE_NAMES[1]=${AE_PCE_PACKAGE_NAME}
PACKAGE_NAMES[2]=${AESM_ECDSA_PACKAGE_NAME}
PACKAGE_NAMES[3]=${AESM_PCE_PACKAGE_NAME}
PACKAGE_NAMES[4]=${AESM_QUOTE_EX_PACKAGE_NAME}

main() {
    pre_build
    update_spec
    create_upstream_tarball
    build_rpm_package
    post_build
}

pre_build() {
    for PACKAGE_NAME in "${PACKAGE_NAMES[@]}"; do
        rm -fR ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION}
        mkdir -p ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION}/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
        cp -f ${SCRIPT_DIR}/${PACKAGE_NAME}.spec ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION}/SPECS
    done
}

post_build() {
    for PACKAGE_NAME in "${PACKAGE_NAMES[@]}"; do
        RPMS=$(find ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION} -name "*.rpm" 2> /dev/null)
        [ -z "${RPMS}" ] || cp ${RPMS} ${SCRIPT_DIR}
        rm -fR ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION}
    done
}

update_spec() {
    min_version="4.12"
    rpm_version=$(rpmbuild --version 2> /dev/null | awk '{print $NF}')
    cur_version=$(echo -e "${rpm_version}\n${min_version}" | sort -V | head -n 1)
    
    # RPM %changelog requires English format "Day Mon DD YYYY"; use LC_ALL=C to force English locale
    changelog_date=$(LC_ALL=C date "+%a %b %d %Y")

    for PACKAGE_NAME in "${PACKAGE_NAMES[@]}"; do
        pushd ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION}
        sed -i "s#@version@#${SGX_VERSION}#" SPECS/${PACKAGE_NAME}.spec
        sed -i "s#@date@#${changelog_date}#" SPECS/${PACKAGE_NAME}.spec
        sed -i "s#@install_path@#${AESM_SERVICE_PACKAGE_PATH}/${AESM_SERVICE_PACKAGE_NAME}#" SPECS/${PACKAGE_NAME}.spec
        if [ "${min_version}" != "${cur_version}" ]; then
            sed -i "s#^Recommends:#Requires:  #" SPECS/${PACKAGE_NAME}.spec
        fi
        popd
    done
}

create_upstream_tarball() {
    ${LINUX_INSTALLER_COMMON_AESM_SERVICE_DIR}/createTarball.sh

    for PACKAGE_NAME in "${PACKAGE_NAMES[@]}"; do
        tar -xvf ${LINUX_INSTALLER_COMMON_AESM_SERVICE_DIR}/output/${TARBALL_NAME} -C ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION}/SOURCES
        pushd ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION}/SOURCES
        # change the install path to /usr/share instead of /opt/intel if the OS is clear linux
        sed -i "s#\(AESM_SERVICE_PACKAGE_PATH=\).*#\1${AESM_SERVICE_PACKAGE_PATH}#" installConfig
        tar -zcvf ${PACKAGE_NAME}-${SGX_VERSION}$(echo ${TARBALL_NAME}|awk -F'.' '{print "."$(NF-1)"."$(NF)}') *
        popd
    done
}

build_rpm_package() {
    for PACKAGE_NAME in "${PACKAGE_NAMES[@]}"; do
        pushd ${SCRIPT_DIR}/${PACKAGE_NAME}-${SGX_VERSION}
        rpmbuild --define="_topdir `pwd`" --define='_debugsource_template %{nil}' -ba SPECS/${PACKAGE_NAME}.spec
        popd
    done
}

main $@
