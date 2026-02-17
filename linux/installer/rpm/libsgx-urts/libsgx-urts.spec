#
# Copyright(c) 2011-2026 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause
#

%define _license_file COPYING

Name:           libsgx-urts
Version:        @version@
Release:        1%{?dist}
Summary:        Intel(R) Software Guard Extensions uRTS
Group:          Development/Libraries
Requires:       libsgx-enclave-common >= %{version}-%{release}

License:        BSD License
URL:            https://github.com/intel/linux-sgx
Source0:        %{name}-%{version}.tar.gz

%description
Intel(R) Software Guard Extensions uRTS

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

%debug_package

%changelog
* @date@ Intel Confidential Computing Team <confidential.computing@intel.com> - @version@-1
- Release v2.28
  See https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.28 for full release notes.

- Key changes:
  1. Removed support for the previously-deprecated Launch Enclave-based launch mechanism (via out-of-tree driver).

  2. The API signature of sgx_create_enclave() remains intact, but former *launch_token and *launch_token_updated
     parameters are now RESERVED and ignored by the implementation.
     Implementers may choose to pass a nullptr in their place.

* Thu Dec 18 2025 Intel Confidential Computing Team <confidential.computing@intel.com> - 2.27.100.1-1
- Release v2.27
  See release notes at https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.27 for more details and historical changelog
