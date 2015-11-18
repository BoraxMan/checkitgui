Summary: A graphical file integrity checksum tool
Name: checkitgui
Version: 0.1.0
Release: 1
License: GPLv2
Group: System
Source: http://dennisk.customer.netspace.net.au/checkit/checkitgui-0.1.0.tar.gz
URL: http://dennisk.customer.netspace.net.au/checkit.html
Distribution: Fedora
Vendor: Dekay Software
Packager: Dennis Katsonis <dennisk@netspace.net.au>

%description
Checksum adds additions data assurance capabilities to filesystems which support extended attributes.  Checkit allows you to detect any otherwise undetected data integrity issues or file changes to any file.
By storing a checksum as an extended attribute, checkit provides an easy way to detect any silent data corruption, bit rot or otherwise undetected error.

This is the GUI version of checkit.

%prep
%setup

%build
qmake-qt4 CONFIG+=release PREFIX=$RPM_BUILD_ROOT
make
strip checkitgui

%install
make install



%files
%{_bindir}/checkitgui
/usr/share/applications/checkitgui.desktop
/usr/share/icons/hicolor/64x64/apps/checkitgui.png
/usr/share/checkitgui/doc/README
/usr/share/checkitgui/doc/LICENSE


%defattr(-,root,root,-)


%clean
rm -rf $RPM_BUILD_ROOT

