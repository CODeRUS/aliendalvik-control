Name:       powermenu2

# >> macros
# << macros

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary:    PowerMenu 2
Version:    0.9.0
Release:    1
Group:      Qt/Qt
License:    WTFPL
URL:        https://openrepos.net/content/coderus/powermenu2
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   sailfish-version >= 2.0.0
Conflicts:  powermenu
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils

Summary: fancy menu and configuration for power key actions

%description
Powermenu - fancy menu and configuration for power key actions


%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
# << build pre

%qtc_qmake5  \
    VERSION=%{version}

%qtc_make %{?_smp_mflags}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake5_install

# >> install post
mkdir -p %{buildroot}/usr/lib/systemd/user/post-user-session.target.wants
ln -s ../powermenu.service %{buildroot}/usr/lib/systemd/user/post-user-session.target.wants/powermenu.service
# << install post

%pre
# >> pre
systemctl-user stop powermenu.service

if /sbin/pidof powermenu2-daemon > /dev/null; then
killall powermenu2-daemon || true
fi

if /sbin/pidof powermenu2-gui > /dev/null; then
killall powermenu2-gui || true
fi
# << pre

%preun
# >> preun
systemctl-user stop powermenu.service

if /sbin/pidof powermenu2-daemon > /dev/null; then
killall powermenu2-daemon || true
fi

if /sbin/pidof powermenu2-gui > /dev/null; then
killall powermenu2-gui || true
fi
# << preun

%post
# >> post
systemctl-user restart powermenu.service
# << post

%files
%defattr(-,root,root,-)
%{_bindir}/powermenu2-daemon
%{_bindir}/powermenu2-gui
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/86x86/apps/%{name}.png
%{_datadir}/powermenu2
/usr/share/dbus-1/services/org.coderus.powermenu.service
/usr/lib/systemd/user/*.service
/usr/lib/systemd/user/post-user-session.target.wants/*.service
/usr/lib/qt5/qml/org/coderus/desktopfilemodel
# >> files
# << files
