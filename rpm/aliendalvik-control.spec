%define theme sailfish-default
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}

Name:       aliendalvik-control
Summary:    Aliendalvik control
Version:    8.1.9
Release:    1
Group:      Qt/Qt
License:    WTFPL
URL:        https://github.com/CODeRUS/aliendalvik-control
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   aliendalvik >= 8.1.0.46.17
Conflicts:  android-shareui
Obsoletes:  android-shareui
Requires:   nemo-transferengine-qt5 >= 0.3.1
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils
BuildRequires:  pkgconfig(nemotransferengine-qt5)
BuildRequires:  sailfish-svg2png >= 0.1.5
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
apkd-install /usr/share/aliendalvik-control/apk/app-release.apk ||:
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
apkd-uninstall org.coderus.aliendalvikcontrol ||:

%files
%{_bindir}/aliendalvik-control
%{_bindir}/aliendalvik-control-proxy

%{_datadir}/dbus-1/system-services/org.coderus.aliendalvikcontrol.service
%{_sysconfdir}/dbus-1/system.d/org.coderus.aliendalvikcontrol.conf
/lib/systemd/system/aliendalvik-control.service

%{_datadir}/dbus-1/services/org.coderus.aliendalvikcontrol.service

%{_datadir}/applications/android-open-url.desktop
%{_datadir}/applications/android-open-url-selector.desktop

%{_datadir}/jolla-settings/entries/aliendalvikcontrol.json

%{_datadir}/jolla-settings/pages/aliendalvikcontrol/main.qml
%{_datadir}/jolla-settings/pages/aliendalvikcontrol/NavbarToggle.qml
%{_datadir}/jolla-settings/pages/aliendalvikcontrol/icon-m-aliendalvik-back.png

%{_libdir}/nemo-transferengine/plugins/libaliendalvikshareplugin.so
%{_datadir}/nemo-transferengine/plugins/AliendalvikShare.qml

%{_datadir}/aliendalvik-control/apk/app-release.apk

%{_datadir}/themes/%{theme}/meegotouch/z1.0/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z1.25/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z1.5/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z1.5-large/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z1.75/icons/*.png
%{_datadir}/themes/%{theme}/meegotouch/z2.0/icons/*.png
