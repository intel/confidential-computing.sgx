#
# Copyright(c) 2011-2026 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause
#

%define _license_file COPYING

Name:           libsgx-uae-service
Version:        @version@
Release:        1%{?dist}
Summary:        Intel(R) Software Guard Extensions Untrusted AE Service
Group:          Development/Libraries
Requires:       libsgx-quote-ex >= %{version}-%{release}

License:        BSD License
URL:            https://github.com/intel/confidential-computing.sgx
Source0:        %{name}-%{version}.tar.gz

%description
Intel(R) Software Guard Extensions Untrusted AE Service

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

- Breaking changes:
  1. Removed deprecated functionality based on EPID (Enhanced Privacy ID):
     - Removed code supporting EPID-based attestation, including remote attestation.
       Note ECDSA-based attestation and universal quoting APIs (i.e. sgx_get_quote_ex())
       continue to be supported.
     - The following definitions have been removed:
         sgx_calc_quote_size()
         sgx_check_update_status()
         sgx_get_extended_epid_group_id()
         sgx_get_quote()                     - note: sgx_get_quote_ex() remains supported
         sgx_get_quote_size()                - note: sgx_get_quote_size_ex() remains supported
         sgx_init_quote()                    - note: sgx_init_quote_ex() remains supported
         sgx_report_attestation_status()
     - The following dev header has been removed: sgx_uae_epid.h

  2. Removed code supporting the deprecated Launch Enclave, whitelist management and supporting "out-of-tree" driver.
     Recommended launch mechanism continues to be the Flexible Launch Control via the in-kernel SGX driver.
     - The following launch-related UAE APIs were deprecated and will now return `SGX_ERROR_FEATURE_NOT_SUPPORTED`:
         get_launch_token()
         sgx_get_whitelist()
         sgx_get_whitelist_size()
         sgx_register_wl_cert_chain()
     - The following dev header has been deprecated: sgx_uae_launch.h

  3. The corresponding libraries (libsgx_epid.so, libsgx_launch.so, ...) are removed, including their _SIM counterparts in the SGX SDK.

* Thu Dec 18 2025 Intel Confidential Computing Team <confidential.computing@intel.com> - 2.27.100.1-1
- Release v2.27
  See release notes at https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.27 for more details and historical changelog
