#
# Copyright(c) 2011-2026 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause
#

%define _license_file COPYING

Name:           libsgx-headers
Version:        @version@
Release:        1%{?dist}
Summary:        Intel(R) Software Guard Extensions Basic Headers
Group:          Development

License:        BSD License
URL:            https://github.com/intel/confidential-computing.sgx
Source0:        %{name}-%{version}.tar.gz

%description
Intel(R) Software Guard Extensions Basic Headers

%prep
%setup -qc

%install
make DESTDIR=%{?buildroot} install
install -d %{?buildroot}%{_docdir}/%{name}
find %{?_sourcedir}/package/licenses/ -type f -print0 | xargs -0 -n1 cat >> %{?buildroot}%{_docdir}/%{name}/%{_license_file}
rm -f %{_specdir}/list-%{name}
for f in $(find %{?buildroot} -type f -o -type l); do
    echo $f | sed -e "s#%{?buildroot}##" >> %{_specdir}/list-%{name}
done

%files -f %{_specdir}/list-%{name}

%changelog
* @date@ Intel Confidential Computing Team <confidential.computing@intel.com> - @version@-1
- Remove EPID (Enhanced Privacy ID) attestation support:
  - Removed EPID-based attestation functionality, including remote attestation
  - Removed deprecated header files: sgx_uae_epid.h, sgx_key_exchange.h
  - Removed support for Quote versions 1 and 2 (EPID-based quotes)
  - Users should migrate to ECDSA-based attestation (Quote version 3+)
- Remove deprecated Launch Enclave (LE) mechanism:
  - Removed reference Launch Enclave implementation and related tools
  - Removed Launch Enclave service bundle from AESM (Architectural Enclave Service Manager)
  - Updated build system to remove LE-related compilation flags and targets
  - Removed LE-related packages from installer scripts

* Thu Dec 18 2025 Intel Confidential Computing Team <confidential.computing@intel.com> - 2.27.100.1-1
- Release v2.27.100.1
  See release notes at https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.27 for more details and historical changelog 
