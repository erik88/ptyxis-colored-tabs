# Agent notes

Fork of [Ptyxis](https://gitlab.gnome.org/chergert/ptyxis) that colors tabs by foreground process. Work on branch `process-tabs`. The `upstream` remote is GNOME's Ptyxis.

## Build and test

Build inside a toolbox with the dependencies from the README, for example `meson setup build -Ddevelopment=true && ninja -C build`. A development build uses its own app ID and settings, so it can run next to the installed Ptyxis.

## Releases

- Packaging is in `packaging/ptyxis.spec`. `.copr/Makefile` builds the SRPM for COPR.
- `Version` matches the upstream Ptyxis release. `Release` is our own counter with a `.coloredtabs` suffix, such as `50.2-1.coloredtabs`. Don't raise it to outnumber Fedora: users set a dnf repository priority instead, as the README describes.
- Pushing doesn't rebuild. Run `copr-cli build-package ptyxis-colored-tabs --name ptyxis`.
