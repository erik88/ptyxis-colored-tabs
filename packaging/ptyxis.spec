# Based on Fedora's ptyxis.spec (50.1-6), building this repository's
# process-tabs branch instead of the upstream release tarball.

%global glib2_version 2.80
%global gtk4_version 4.14
%global vte291_version 0.77
%global json_glib_version 1.6
%global libadwaita_version 1.6
%global libportal_gtk4_version 0.7.1

Name:		ptyxis
Version:	50.1
Release:	6.2.coloredtabs%{?dist}
Summary:	A container oriented terminal for GNOME, with tabs colored by process

License:	GPL-2.0-or-later AND GPL-3.0-or-later AND LGPL-3.0-or-later AND LGPL-2.0-or-later AND CC0-1.0
URL:		https://github.com/erik88/ptyxis-colored-tabs
# Created by .copr/Makefile with git archive.
Source0:	%{name}-%{version}.tar.gz
Source1:	org.gnome.Ptyxis.fedora.gschema.override

BuildRequires:	pkgconfig(gio-unix-2.0) >= %{glib2_version}
BuildRequires:	pkgconfig(gtk4) >= %{gtk4_version}
BuildRequires:	pkgconfig(vte-2.91-gtk4) >= %{vte291_version}
BuildRequires:	pkgconfig(libadwaita-1) >= %{libadwaita_version}
BuildRequires:	pkgconfig(libportal-gtk4) >= %{libportal_gtk4_version}
BuildRequires:	pkgconfig(json-glib-1.0) >= %{json_glib_version}
BuildRequires:	desktop-file-utils
BuildRequires:	gcc
BuildRequires:	itstool
BuildRequires:	meson
BuildRequires:	/usr/bin/appstream-util

Requires:	hicolor-icon-theme

# https://fedoraproject.org/wiki/Changes/EncourageI686LeafRemoval
ExcludeArch:	%{ix86}

%description
Ptyxis is a container oriented terminal that provides transparent support for
container systems like Podman, Distrobox, and Toolbx. It also has robust
support for user profiles.

This is an unofficial modified build that colors each tab by the program
running in it, including inactive tabs, with configurable colors per program.


%prep
%autosetup -p1 -n %{name}-%{version}


%build
%meson -Dgeneric=terminal
%meson_build


%install
%meson_install
install -p %{SOURCE1} %{buildroot}%{_datadir}/glib-2.0/schemas
%find_lang %{name} --with-gnome


%check
appstream-util validate-relax --nonet %{buildroot}%{_metainfodir}/org.gnome.Ptyxis.metainfo.xml
desktop-file-validate %{buildroot}%{_datadir}/applications/org.gnome.Ptyxis.desktop


%files -f %{name}.lang
%doc README.md NEWS
%license COPYING
%{_bindir}/ptyxis
%{_libexecdir}/ptyxis-agent
%{_metainfodir}/org.gnome.Ptyxis.metainfo.xml
%{_datadir}/applications/org.gnome.Ptyxis.desktop
%dir %{_datadir}/dbus-1
%dir %{_datadir}/dbus-1/services
%{_datadir}/dbus-1/services/org.gnome.Ptyxis.service
%{_datadir}/glib-2.0/schemas/org.gnome.Ptyxis.gschema.xml
%{_datadir}/glib-2.0/schemas/org.gnome.Ptyxis.fedora.gschema.override
%{_datadir}/icons/hicolor/*/*/*.svg
%{_mandir}/man1/ptyxis.1*

%changelog
* Sun Nov 08 2026 Erik Lissel <erik@lissel.net> - 50.1-6.2.coloredtabs
- Keep the tab bar neutral and join the active colored tab to the header
- Improve text contrast on the active colored tab

* Sat Nov 07 2026 Erik Lissel <erik@lissel.net> - 50.1-6.1.coloredtabs
- Color tabs and the window header by foreground process
- Add the process-tints setting
- Build from the process-tabs branch of erik88/ptyxis-colored-tabs
