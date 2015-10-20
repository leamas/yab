all: README.html



README.html: rpmdev-patchbuild.1
	man2html $? > $@

pylint: .phony
	-python3-pylint --rcfile pylint.conf rpmdev-patchbuild

.phony:

