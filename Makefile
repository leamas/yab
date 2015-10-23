all: README.md



README.md: README.header rpmdev-patchbuild.1 Makefile
	cp README.header $@
	man2html -H linux.die.net -M /man rpmdev-patchbuild.1 | \
	    sed -e '/Content-type/d'  -e '/HREF=/s|\?1+|/1/|'>> $@

check: .phony
	-python3-pep8 --config pep8.conf rpmdev-patchbuild
	-python3-pylint --rcfile pylint.conf rpmdev-patchbuild

archive: rpmdev-patchbuild.tar.gz
rpmdev-patchbuild.tar.gz:  rpmdev-patchbuild rpmdev-patchbuild.1
	git archive --prefix rpmdev-patchbuild/ -o $@ HEAD

.phony:

