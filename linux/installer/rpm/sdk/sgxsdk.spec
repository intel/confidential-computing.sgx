#
# Copyright (C) 2011-2019 Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#   * Neither the name of Intel Corporation nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

%define _unpackaged_files_terminate_build 0
%define _install_path @install_path@
%define _helper_command @helper_command@
%define _license_file COPYING

Name:           sgxsdk
Version:        @version@
Release:        1%{?dist}
Summary:        Intel(R) SGX SDK
Group:          Development/Libraries

License:        BSD License
URL:            https://github.com/intel/linux-sgx
Source0:        %{name}-%{version}.tar.gz

%description
Intel(R) SGX SDK

%prep
%setup -qc

%install
make DESTDIR=%{?buildroot} install
install -d %{?buildroot}%{_docdir}/%{name}
find %{?_sourcedir}/package/licenses/ -type f -print0 | xargs -0 -n1 cat >> %{?buildroot}%{_docdir}/%{name}/%{_license_file}
echo "%{_install_path}" > %{_specdir}/listfiles
find %{?buildroot} | sort | \
awk '$0 !~ last "/" {print last} {last=$0} END {print last}' | \
sed -e "s#^%{?buildroot}##" | \
grep -v "^%{_install_path}" >> %{_specdir}/listfiles || :
%{_helper_command}

%files -f %{_specdir}/listfiles

%changelog


* @date@ Intel Confidential Computing Team <confidential.computing@intel.com> - @version@-1
- Release v2.28
  See https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.28 for full release notes.

- Notable changes:
  1. [BREAKING] Removed deprecated functionality based on EPID (Enhanced Privacy ID):
     - Following headers and supporting libraries have been removed: 
         sgx_uae_epid.h (+ libsgx_epid.so/libsgx_epid_sim.so)
         sgx_key_exchange.h, sgx_ukey_exchange.h, sgx_tkey_exchange.edl, sgx_tkey_exchange.h (+sgx_ukey_exchange.a, sgx_tkey_exchange.a),
     - Deprecated `sgx_quote_t` (v1, EPID-based) and related structures. ECDSA-based Quote version 3+ continue to be supported.

  2. [BREAKING] Removed code supporting the deprecated Launch Enclave, whitelist management and the supporting "out-of-tree" Linux SGX driver.
     Recommended launch mechanism continues to be the Flexible Launch Control via the in-kernel SGX driver.
     - The supporting library stub libsgx_launch.so and libsgx_launch_sim.so have been removed
     - The sgx_uae_launch.h has been deprecated and all the LE-specific launch definitions:
         get_launch_token()
         sgx_get_whitelist()
         sgx_get_whitelist_size()
         sgx_register_wl_cert_chain()
       now return SGX_ERROR_FEATURE_NOT_SUPPORTED.

  3. EPID-specific RemoteAttestation sample has been removed. Refer to `SampleAttestedTLS` or samples in the DCAP repository
     (i.e. https://github.com/intel/confidential-computing.tee.dcap/tree/main/SampleCode/QuoteVerificationSample)
     for ECDSA attestation examples.     


* Thu Dec 18 2025 Intel Confidential Computing Team <confidential.computing@intel.com> - 2.27.100.1-1
- Release v2.27
  See release notes at https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.27 for more details and historical changelog
