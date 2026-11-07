# Ptyxis with colored tabs

A fork of [Ptyxis](https://gitlab.gnome.org/chergert/ptyxis), GNOME's container-oriented terminal, that colors each tab by the program running in it.

![An orange Claude Code tab, a blue ssh tab and a red sudo tab](docs/colored-tabs.png)

For everything else about the terminal, see the [Ptyxis README](https://gitlab.gnome.org/chergert/ptyxis/-/blob/main/README.md).

## What's different

Ptyxis tints the window header when the active tab runs sudo or ssh. This fork adds:

- **Colored tabs:** every tab shows its own color, including inactive tabs.
- **Custom rules:** any program can have a color. Claude Code (`claude`) is orange by default.

sudo and ssh always take priority, so a rule can't hide that a tab is running as root or remote. Their colors come from your palette.

## Configuration

The `process-tints` setting maps program names to colors, and changes apply immediately:

```sh
gsettings set org.gnome.Ptyxis process-tints "{'claude': '#d97757', 'vim': '#2e8b57'}"
```

This replaces the whole list, so include `claude` if you want to keep it. Turning off Ptyxis's own `visual-process-leader` setting also turns off all tinting.

## Install

On Fedora 45 (x86_64), from [COPR](https://copr.fedorainfracloud.org/coprs/erik88/ptyxis-colored-tabs/):

```sh
sudo dnf copr enable erik88/ptyxis-colored-tabs
sudo dnf config-manager setopt 'copr:copr.fedorainfracloud.org:erik88:ptyxis-colored-tabs.priority=50'
sudo dnf upgrade --refresh ptyxis
```

This replaces Fedora's `ptyxis` package. The priority line makes dnf prefer this repository even when Fedora ships a newer Ptyxis, which would otherwise replace this build and remove the tab colors. The new version starts once every Ptyxis window is closed.

To go back to Fedora's build:

```sh
sudo dnf copr remove erik88/ptyxis-colored-tabs
sudo dnf distro-sync ptyxis
```

### Build from source

```sh
sudo dnf install meson gcc gtk4-devel libadwaita-devel vte291-gtk4-devel json-glib-devel libportal-gtk4-devel desktop-file-utils gettext systemd-devel
meson setup build
ninja -C build
sudo ninja -C build install
```

By default this installs to `/usr/local`, alongside any packaged Ptyxis. It uses the same app ID and settings.

## Limitations

- Rules match the name a program was started with, so `python foo.py` only matches as `python`.
- Detection looks at the first program in a pipeline, so `cat prompt.txt | claude` gets no color, and `echo pw | sudo -S cmd` doesn't get the sudo color. A program whose path contains a space won't match a rule either.
- A background tab's color can lag until the tab produces output.
- libadwaita has no API for styling a single tab, so this relies on its private tab widgets. A libadwaita update could break the tab colors.

## License

GPL-3.0-or-later, like Ptyxis. See [COPYING](COPYING).

**Modification notice:** this is a modified version of Ptyxis 50.1. Erik Lissel changed it on 2026-11-06 to add per-tab process colors and the `process-tints` setting, and replaced this README. The full list of changes is in this repository's commit history since the `50.1` release.
