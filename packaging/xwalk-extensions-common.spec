Name:       xwalk-extensions-common
Summary:    Common modules and tools for Crosswalk Extensions
Version:    0.0.1
Release:    1
Group:      Development/Libraries
License:    Apache-2.0 and BSD-3-Clause
URL:        https://www.tizen.org
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest

BuildRequires: cmake
BuildRequires: pkgconfig(jsoncpp)

%description
Common modules and tools for Crosswalk Extensions

%package devel
Summary:  Development package of xwalk-extensions-common
Group:    Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: python
Requires: sed
Requires: cpp
Provides: gyp_xwext

%description devel
Development package of xwalk-extensions-common

%prep
%setup -q
cp %{SOURCE1} .

%build

mkdir -p cmake_build_tmp
cd cmake_build_tmp
cmake .. \
        -DCMAKE_INSTALL_PREFIX=%{_prefix} \
        -DLIB_INSTALL_DIR=%{_libdir}

make %{?jobs:-j%jobs}


%install
rm -rf %{buildroot}

mkdir -p %{buildroot}%{_datadir}/license
cp LICENSE %{buildroot}%{_datadir}/license/%{name}
cat LICENSE.BSD >> %{buildroot}%{_datadir}/license/%{name}

mkdir -p %{buildroot}%{_bindir}
ln -s /usr/share/xwalk/tools/build/gyp_xwext \
      %{buildroot}%{_bindir}/gyp_xwext

cd cmake_build_tmp
%make_install

%clean
rm -rf %{buildroot}

%files
%manifest %{name}.manifest
%{_libdir}/lib%{name}.so.*
%{_datadir}/license/%{name}

%files devel
%{_includedir}/xwalk/
%{_libdir}/lib%{name}.so
%{_libdir}/pkgconfig/%{name}.pc
%{_bindir}/gyp_xwext
/usr/share/xwalk/tools/
%attr(0755,root,root) /usr/share/xwalk/tools/gyp/gyp
%attr(0755,root,root) /usr/share/xwalk/tools/build/gyp_xwext
