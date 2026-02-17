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
LINUX_INSTALLER_URTS_COMMON_DIR="${LINUX_INSTALLER_COMMON_DIR}/libsgx-urts"

source ${LINUX_INSTALLER_URTS_COMMON_DIR}/installConfig

SGX_VERSION=$(awk '/STRFILEVER/ {print $3}' ${ROOT_DIR}/common/inc/internal/se_version.h|sed 's/^\"\(.*\)\"$/\1/')
RPM_BUILD_FOLDER=${URTS_PACKAGE_NAME}-${SGX_VERSION}

main() {
    pre_build
    update_version
    create_upstream_tarball
    build_rpm_package
    post_build
}

pre_build() {
    rm -fR ${SCRIPT_DIR}/${RPM_BUILD_FOLDER}
    mkdir -p ${SCRIPT_DIR}/${RPM_BUILD_FOLDER}/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
    cp -f ${SCRIPT_DIR}/${URTS_PACKAGE_NAME}.spec ${SCRIPT_DIR}/${RPM_BUILD_FOLDER}/SPECS
}

post_build() {
    for FILE in $(find ${SCRIPT_DIR}/${RPM_BUILD_FOLDER} -name "*.rpm" 2> /dev/null); do
        cp "${FILE}" ${SCRIPT_DIR}
    done
    rm -fR ${SCRIPT_DIR}/${RPM_BUILD_FOLDER}
}

update_version() {
    pushd ${SCRIPT_DIR}/${RPM_BUILD_FOLDER}

    # Replace placeholders in spec
    sed -i "s/@version@/${SGX_VERSION}/" SPECS/${URTS_PACKAGE_NAME}.spec

    # RPM %changelog requires English format "Day Mon DD YYYY"; use LC_ALL=C to force English locale
    BUILD_DATE=$(LC_ALL=C date +"%a %b %d %Y")
    sed -i "s/@date@/${BUILD_DATE}/" SPECS/${URTS_PACKAGE_NAME}.spec

    popd
}

create_upstream_tarball() {
    ${LINUX_INSTALLER_URTS_COMMON_DIR}/createTarball.sh
    tar -xvf ${LINUX_INSTALLER_URTS_COMMON_DIR}/output/${TARBALL_NAME} -C ${SCRIPT_DIR}/${RPM_BUILD_FOLDER}/SOURCES
    pushd ${SCRIPT_DIR}/${RPM_BUILD_FOLDER}/SOURCES
    tar -zcvf ${RPM_BUILD_FOLDER}$(echo ${TARBALL_NAME}|awk -F'.' '{print "."$(NF-1)"."$(NF)}') *
    popd
}

build_rpm_package() {
    pushd ${SCRIPT_DIR}/${RPM_BUILD_FOLDER}
    rpmbuild --define="_topdir `pwd`" --define='_debugsource_template %{nil}' -ba SPECS/${URTS_PACKAGE_NAME}.spec
    popd
}

main $@
