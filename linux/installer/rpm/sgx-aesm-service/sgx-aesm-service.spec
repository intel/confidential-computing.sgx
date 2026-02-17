#
# Copyright(c) 2011-2026 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause
#

%define _install_path @install_path@
%define _license_file COPYING

Name:           sgx-aesm-service
Version:        @version@
Release:        1%{?dist}
Summary:        Intel(R) Software Guard Extensions AESM Service
Group:          Development/System

License:        BSD License
URL:            https://github.com/intel/confidential-computing.sgx
Source0:        %{name}-%{version}.tar.gz

%description
Intel(R) Software Guard Extensions AESM Service

%prep
%setup -qc

%install
make DESTDIR=%{?buildroot} install
OLDDIR=$(pwd)
cd %{?buildroot}
rm -fr $(ls | grep -xv "%{name}")
install -d %{name}%{_docdir}/%{name}
find %{?_sourcedir}/package/licenses/ -type f -print0 | xargs -0 -n1 cat >> %{name}%{_docdir}/%{name}/%{_license_file}
cd "$OLDDIR"
echo "%{_install_path}" > %{_specdir}/list-%{name}
find %{?buildroot}/%{name} | sort | \
awk '$0 !~ last "/" {print last} {last=$0} END {print last}' | \
sed -e "s#^%{?buildroot}/%{name}##" | \
grep -v "^%{_install_path}" >> %{_specdir}/list-%{name} || :
cp -r %{?buildroot}/%{name}/* %{?buildroot}/
rm -fr %{?buildroot}/%{name}
sed -i 's#^/etc/aesmd.conf#%config &#' %{_specdir}/list-%{name}

%files -f %{_specdir}/list-%{name}

%posttrans
if [ -x %{_install_path}/startup.sh ]; then %{_install_path}/startup.sh; fi

%preun
if [ -x %{_install_path}/cleanup.sh ]; then %{_install_path}/cleanup.sh; fi

%debug_package

%changelog
* @date@ Intel Confidential Computing Team <confidential.computing@intel.com> - @version@-1
- Release v2.28
  See https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.28 for full release notes.

- Breaking changes:
  1. Removed deprecated functionality based on EPID (Enhanced Privacy ID):
       Removed code supporting EPID-based attestation, including remote attestation.
       The `libsgx-aesm-epid-plugin` as well as the `epid_quote_service_bundle` are removed.
       Note ECDSA-based attestation and universal quoting APIs (i.e. sgx_get_quote_ex())
       continue to be supported.

       Deprecated `sgx_quote_t` (v1, EPID-based). ECDSA-based Quote version 3+ remains supported.

  2. Removed code supporting the deprecated Launch Enclave, whitelist management and the supporting "out-of-tree" Linux SGX driver.
     Recommended launch mechanism continues to be the Flexible Launch Control via the in-kernel SGX driver.
        The `libsgx-aesm-launch-plugin` as well as the `le_launch_service_bundle` are removed.

  3. Supporting architectural enclaves: Launch Enclave (LE), EPID-based Provisioning Enclave (PVE), EPID-based Quoting Enclave (QE)
     are no longer distributed. Launch Whitelist files (white_list_cert*.bin) and signature files (le_prod_css.bin) are removed as well.

* Thu Dec 18 2025 Intel Confidential Computing Team <confidential.computing@intel.com> - 2.27.100.1-1
- Release v2.27
  See release notes at https://github.com/intel/confidential-computing.sgx/releases/tag/sgx_2.27 for more details and historical changelog

