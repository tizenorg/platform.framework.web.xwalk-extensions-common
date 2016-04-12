%define crosswalk_extensions tizen-extensions-crosswalk
%define crosswalk_extensions_path %{_libdir}/%{crosswalk_extensions}

Name:       echo
Summary:    Echo sample for xwalk-extensions-common
Version:    0.0.1
Release:    1
Group:      Development/Libraries
License:    Apache-2.0 and BSD-3-Clause
URL:        https://www.tizen.org
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest

BuildRequires: pkgconfig(xwalk-extensions-common)
BuildRequires: xwalk-gyp

%description
Echo sample extension

%prep
%setup -q
cp %{SOURCE1} .

%build
xwalk-gyp build.gyp
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{crosswalk_extensions_path}
install -p -m 644 out/Default/lib.target/*.so %{buildroot}%{crosswalk_extensions_path}
install -p -m 644 tizen_echo.json %{buildroot}%{crosswalk_extensions_path}

%clean
rm -rf %{buildroot}

%files
%manifest %{name}.manifest
%{crosswalk_extensions_path}/*.so
%{crosswalk_extensions_path}/*.json
