Summary: A graphical file integrity checksum tool
Name: checkitgui
Version: 0.1.1
Release: 1
License: GPLv2
Group: System
Source: checkitgui-0.1.1.tar.gz
URL: http://dennisk.customer.netspace.net.au/checkit.html
Distribution: Fedora
Vendor: Dekay Software
Packager: Dennis Katsonis <dennisk@netspace.net.au>
%global debug_package %{nil}

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
%{_datadir}/applications/*
%{_docdir}/*
%{_datadir}/icons/hicolor/64x64/apps/*
%{_bindir}/checkitgui


%defattr(-,root,root,-)

%post
/bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :

%postun
if [ $1 -eq 0 ] ; then
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
fi

%posttrans
/usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :


%clean
rm -rf $RPM_BUILD_ROOT

