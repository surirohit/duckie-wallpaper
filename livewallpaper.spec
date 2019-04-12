%define lw_version 0.5.0
%define branch_owner aurelien-riv
%define branch_name  lw-ui-vala-port

Name:           livewallpaper
Version:        %{lw_version}.%{_revno}
Release:        1%{?dist}
Summary:        Animated wallpaper
License:        GPLv3
URL:            https://launchpad.net/livewallpaper
Source0:        http://bazaar.launchpad.net/~%{branch_owner}/livewallpaper/%{branch_name}/tarball/%{_revno}
BuildRequires: cmake >= 2.8, gettext, intltool, gtk-doc, xcftools, libappstream-glib
BuildRequires: pkgconfig(gobject-introspection-1.0), pkgconfig(libpeas-1.0), pkgconfig(glew), pkgconfig(upower-glib)
Requires:      python-pillow, PyOpenGL

%description
    Livewallpaper provides animated wallpaper for your desktop

%package devel
Requires:      %{name} = %{version}-%{release}
Summary:       LiveWallpaper development files
Provides:      livewallpaper-devel
%description devel
    Header files for Livewallpaper, required to develop new plugins or to
    build existing plugins.
    Developer documentation for the LiveWallpaper API

%package ui
BuildRequires: pkgconfig(gtk+-3.0), vala
Requires:      %{name} = %{version}-%{release}
Summary:       User interfaces to configure LiveWallpaper
Provides:      livewallpaper-config, livewallpaper-indicator
%description ui
    This package contains an application to set up Livewallpaper, and an indicator
    to access quickly to basic options of LiveWallpaper and that embeds a shortcut
    to the configuration program.

%prep
    rm -rf $RPM_BUILD_DIR/livewallpaper
    tar -xzf $RPM_SOURCE_DIR/%{_revno} -C $RPM_BUILD_DIR
    mv $RPM_BUILD_DIR/~%{branch_owner}/livewallpaper/%{branch_name} $RPM_BUILD_DIR/livewallpaper
    rm -rf $RPM_BUILD_DIR/~%{branch_owner}

%build
    mkdir -p livewallpaper/build
    cd livewallpaper/build
    %cmake .. -DENABLE_OPTIMIZATION=ON -DENABLE_DOC=ON
    make %{?_smp_mflags}

%check
    appstream-util validate-relax --nonet %{buildroot}/usr/share/appdata/*.appdata.xml

%install
    rm -rf %{buildroot}
    cd livewallpaper/build
    %make_install DESTDIR=%{buildroot}
    ln -fs /usr/lib/liblivewallpaper-core.so.%{lw_version} %{buildroot}/usr/lib/liblivewallpaper-core.so
    %find_lang %{name}

%preun
    if [ $1 -eq 0 ] ; then
        update-alternatives --remove lw-desktop-icons %{_bindir}/lw-desktop-icons.default.sh
    fi

%post
    /sbin/ldconfig
    touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :
    update-alternatives --install %{_bindir}/lw-desktop-icons lw-desktop-icons %{_bindir}/lw-desktop-icons.default.sh 10

%postun
    /sbin/ldconfig
    if [ $1 -eq 0 ] ; then
        touch --no-create %{_datadir}/icons/hicolor &>/dev/null
        gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null     || :
        glib-compile-schemas  %{_datadir}/glib-2.0/schemas &> /dev/null || :
    fi

%posttrans
    glib-compile-schemas  %{_datadir}/glib-2.0/schemas &> /dev/null || :
    gtk-update-icon-cache %{_datadir}/icons/hicolor    &>/dev/null  || :

%files -f livewallpaper/build/%{name}.lang
    %defattr(-,root,root,-)
    /etc/xdg/autostart/livewallpaper-autostart.desktop
    /usr/bin/livewallpaper
    /usr/bin/livewallpaper-autostart
    /usr/bin/lw-desktop-icons.default.sh
    /usr/lib/livewallpaper/*
    /usr/lib/liblivewallpaper-core.so.%{lw_version}
    /usr/share/applications/livewallpaper.desktop
    /usr/share/glib-2.0/schemas/*
    /usr/share/icons/*/scalable/apps/*
    /usr/share/livewallpaper/*
%exclude
    /usr/share/icons/*/scalable/apps/livewallpaper-config.svg

%files devel
    /usr/include/livewallpaper/*
    /usr/lib/liblivewallpaper-core.so
    /usr/lib/pkgconfig/livewallpaper.pc
    /usr/share/cmake/Modules/*
    /usr/bin/lw-generate-schema
    /usr/share/gtk-doc/html/

%files ui
    /etc/xdg/autostart/livewallpaper-indicator.desktop
    /usr/share/appdata/livewallpaper-config.appdata.xml
    /usr/bin/livewallpaper-config
    /usr/bin/livewallpaper-indicator
    /usr/share/applications/livewallpaper-config.desktop
    /usr/share/icons/*/scalable/apps/livewallpaper-config.svg
    /usr/share/icons/*/scalable/status/livewallpaper-indicator.svg

%changelog
    * Sat Dec 20 2014 Aurélien Rivière <aurelien.riv@gmail.com> 0.5.0-1
    - Maybe Livewallpaper will join COPR soon ? :)
