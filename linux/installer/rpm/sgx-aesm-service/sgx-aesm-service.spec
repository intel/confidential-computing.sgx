#
# Copyright(c) 2011-2025 Intel Corporation 
#
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
URL:            https://github.com/intel/linux-sgx
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
* Mon Jul 29 2019 SGX Team
- Initial Release
