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

There are no packages yet, so you need to build it from source. On Fedora:

```sh
sudo dnf install meson gcc gtk4-devel libadwaita-devel vte291-gtk4-devel json-glib-devel libportal-gtk4-devel desktop-file-utils gettext systemd-devel
meson setup build
ninja -C build
sudo ninja -C build install
```

By default this installs to `/usr/local`, alongside any packaged Ptyxis. It uses the same app ID and settings.

## Limitations

- Rules match the name a program was started with, so `python foo.py` only matches as `python`.
- A background tab's color can lag until the tab produces output.
- libadwaita has no API for styling a single tab, so this relies on its private tab widgets. A libadwaita update could break the tab colors.

## License

GPL-3.0-or-later, like Ptyxis. See [COPYING](COPYING). This fork is based on Ptyxis 50.1, and its changes are in this repository's history since that release.
