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
# Headers moved from libsgx-dcap-quote-verify-devel into this package in 1.26.
# Obsoletes: replaces the old package on upgrade; Conflicts: prevents co-installation.
# Safe because all files from the old package are now owned here.
Obsoletes:      libsgx-dcap-quote-verify-devel < 1.26
Conflicts:      libsgx-dcap-quote-verify-devel < 1.26

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
- Release v2.29
  See https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.29 for full release notes.

- Key changes:
  1. MOVED `sgx_qve_header.h` file from libsgx-dcap-quote-verify-devel to this package
  2. ADDED `sgx_dcap_qal_types.h` header containing common QAL types.
     Origin: extracted from `sgx_dcap_qal.h` (part of libsgx-dcap-quote-verify-devel package)

* Tue Mar 03 2026 Intel Confidential Computing Team <confidential.computing@intel.com> - 2-28.100.1-1
- Release v2.28
  See https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.28 for full release notes.

- Key changes:
  1. Deprecated EPID (Enhanced Privacy ID) and Launch Enclave (LE) related error codes

* Thu Dec 18 2025 Intel Confidential Computing Team <confidential.computing@intel.com> - 2.27.100.1-1
- Release v2.27
  See release notes at https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.27 for more details and historical changelog 
