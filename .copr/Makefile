# Used by COPR's make_srpm method: make -f .copr/Makefile srpm outdir=... spec=...
top := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
spec ?= $(top)/packaging/ptyxis.spec
outdir ?= $(top)/_srpm
version := $(shell sed -n 's/^Version:[[:space:]]*//p' $(top)/packaging/ptyxis.spec)
work := $(outdir)/.srpm-sources

srpm:
	command -v git >/dev/null || dnf -y install git
	command -v rpmbuild >/dev/null || dnf -y install rpm-build
	git config --global --add safe.directory $(top)
	mkdir -p $(work)
	git -C $(top) archive --format=tar.gz --prefix=ptyxis-$(version)/ -o $(work)/ptyxis-$(version).tar.gz HEAD
	cp $(top)/packaging/org.gnome.Ptyxis.fedora.gschema.override $(work)/
	rpmbuild -bs $(top)/packaging/ptyxis.spec \
		--define "_sourcedir $(work)" \
		--define "_srcrpmdir $(outdir)"

.PHONY: srpm
