#!/usr/bin/env bash
#
# Copyright(c) 2011-2026 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause
#

top_dir=`dirname $0`
out_dir=$top_dir
optlib_name=optimized_libs_2.28.tar.gz
ae_file_name=prebuilt_ae_2.28.tar.gz
binutils_file_name=as.ld.objdump.r4.tar.gz
checksum_file=SHA256SUM_prebuilt_2.28.cfg
server_url_path=https://download.01.org/intel-sgx/sgx-linux/2.28
server_optlib_url=$server_url_path/$optlib_name
server_ae_url=$server_url_path/$ae_file_name
server_binutils_url=$server_url_path/$binutils_file_name
server_checksum_url=$server_url_path/$checksum_file

rm -f $out_dir/$optlib_name
wget $server_optlib_url -P $out_dir
if [ $? -ne 0 ]; then
    echo "Fail to download file $server_optlib_url"
    exit -1
fi

rm -f $out_dir/$ae_file_name
wget $server_ae_url -P $out_dir
if [ $? -ne 0 ]; then
    echo "Fail to download file $server_ae_url"
    exit -1
fi

rm -f $out_dir/$binutils_file_name
wget $server_binutils_url -P $out_dir
if [ $? -ne 0 ]; then
    echo "Fail to download file $server_binutils_url"
    exit -1
fi

rm -f $out_dir/$checksum_file
wget $server_checksum_url -P $out_dir
if [ $? -ne 0 ]; then
    echo "Fail to download file $server_checksum_url"
    exit -1
fi


pushd $out_dir

sha256sum -c $checksum_file
if [ $? -ne 0 ]; then
    echo "Checksum verification failure"
    exit -1
fi
tar -zxf $optlib_name
tar -zxf $ae_file_name
tar -zxf $binutils_file_name
rm -f $optlib_name
rm -f $ae_file_name
rm -f $checksum_file
rm -f $binutils_file_name

popd
