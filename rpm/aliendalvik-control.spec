%define theme sailfish-default
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}

Name:       aliendalvik-control
Summary:    Aliendalvik control
Version:    9.9.9
Release:    1
Group:      Qt/Qt
License:    WTFPL
URL:        https://github.com/CODeRUS/aliendalvik-control
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfish-version >= 3
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   aliendalvik < 8
Conflicts:  android-shareui
Obsoletes:  android-shareui
Requires:   nemo-transferengine-qt5 >= 0.3.1
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  desktop-file-utils
BuildRequires:  pkgconfig(nemotransferengine-qt5)
BuildRequires:  sailfish-svg2png >= 0.1.5
BuildRequires:  pkgconfig(contentaction5)
BuildRequires:  pkgconfig(libgbinder)

%description
D-Bus daemon for sending commands to aliendalvik

%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%pre
systemctl stop aliendalvik-control ||:
if /sbin/pidof aliendalvik-control > /dev/null; then
killall -9 aliendalvik-control ||:
fi
if /sbin/pidof aliendalvik-control-proxy > /dev/null; then
killall -9 aliendalvik-control-proxy ||:
fi
/usr/bin/aliendalvik-control restore ||:
/usr/bin/update-desktop-database ||:

%post
systemctl daemon-reload
systemctl restart aliendalvik-control ||:

%preun
systemctl stop aliendalvik-control ||:
if /sbin/pidof aliendalvik-control > /dev/null; then
killall -9 aliendalvik-control ||:
fi
if /sbin/pidof aliendalvik-control-proxy > /dev/null; then
killall -9 aliendalvik-control-proxy ||:
fi
/usr/bin/aliendalvik-control restore ||:
umount /home/.android/data/media/0/sdcard_external ||:
/usr/bin/update-desktop-database ||:
apkd-uninstall /home/.android/data/app/aliendalvik-control.apk ||:

%files
%{_bindir}/aliendalvik-control
%{_bindir}/aliendalvik-control-proxy
%{_bindir}/aliendalvik-control-share
%{_bindir}/aliendalvik-control-selector

%{_libdir}/libaliendalvikcontrolplugin-chroot.so

%{_datadir}/dbus-1/system-services/org.coderus.aliendalvikcontrol.service
%{_sysconfdir}/dbus-1/system.d/org.coderus.aliendalvikcontrol.conf
/lib/systemd/system/aliendalvik-control.service

%{_datadir}/dbus-1/services/org.coderus.aliendalvikcontrol.service
%{_datadir}/dbus-1/services/org.coderus.aliendalvikshare.service
%{_datadir}/dbus-1/services/org.coderus.aliendalvikselector.service

%{_datadir}/applications/android-open-url.desktop
%{_datadir}/applications/android-open-url-selector.desktop

%{_datadir}/jolla-settings/entries/aliendalvikcontrol.json

%{_datadir}/jolla-settings/pages/aliendalvikcontrol/main.qml
%{_datadir}/jolla-settings/pages/aliendalvikcontrol/NavbarToggle.qml

%{_libdir}/nemo-transferengine/plugins/libaliendalvikshareplugin.so
%{_datadir}/nemo-transferengine/plugins/AliendalvikShare.qml

%{_datadir}/aliendalvik-control-share/qml/aliendalvik-control-share.qml
%{_datadir}/aliendalvik-control-share/qml/aliendalvik-control-share-cover.qml

%{_datadir}/aliendalvik-control-selector/qml/aliendalvik-control-selector.qml

%{_datadir}/aliendalvik-control/apk/app-release.apk

%{_datadir}/themes/%{theme}/meegotouch/z1.0/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z1.25/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z1.5/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z1.5-large/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z1.75/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z2.0/icons/*.png
