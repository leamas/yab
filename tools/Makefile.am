## $Id: Makefile.am,v 5.20 2009/12/27 19:04:10 lirc Exp $

## Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS             = -I m4

AM_CPPFLAGS = -I$(top_srcdir) -I../lib @X_CFLAGS@

EXTRA_PROGRAMS = smode2 xmode2 irxevent
bin_PROGRAMS = irw irpty irexec ircat mode2 irsend \
	lircrcd \
	@vga_progs@ @x_progs@

## simple programs
irw_SOURCES = irw.c
irpty_SOURCES = irpty.c
irpty_DEPENDENCIES = ../lib/liblirc_client.la
irpty_LDADD = -L../lib -llirc_client @forkpty@ -llirc
irexec_SOURCES = irexec.c
irexec_DEPENDENCIES = ../lib/liblirc_client.la
irexec_LDADD = -L../lib -llirc_client @daemon@ -llirc
ircat_SOURCES = ircat.c
ircat_DEPENDENCIES = ../lib/liblirc_client.la
ircat_LDADD = -L../lib -llirc_client -llirc
mode2_SOURCES = mode2.c
irsend_SOURCES = irsend.c

mode2_LDADD = ../daemons/libhw_module.a @hw_module_libs@ -L../lib -llirc

## vga programs
smode2_SOURCES = smode2.c
smode2_LDADD = -lvga -lvgagl -L../lib -llirc

## x programs
irxevent_SOURCES = irxevent.c
irxevent_LDADD = @X_LIBS@ @X_PRE_LIBS@ -lX11 @X_EXTRA_LIBS@ -L../lib -llirc_client -llirc
irxevent_DEPENDENCIES = ../lib/liblirc_client.la
xmode2_SOURCES = xmode2.c
xmode2_LDADD = ../daemons/libhw_module.a @hw_module_libs@ \
	@X_LIBS@ @X_PRE_LIBS@ -lX11 @X_EXTRA_LIBS@ -L../lib -llirc

## lircrcd
lircrcd_SOURCES = lircrcd.c
lircrcd_DEPENDENCIES = ../lib/liblirc_client.la
lircrcd_LDADD = -L../lib -llirc_client -llirc

## libraries
lircincludedir = $(includedir)/lirc


if HAVE_PYTHON
bin_SCRIPTS = pronto2lirc

pronto2lirc: $(srcdir)/pronto2lirc.py
	echo "#! ${PYTHON}" >$@
	cat $< >>$@
	chmod +x $@
endif

EXTRA_DIST = pronto2lirc.py

CLEANFILES = *~ pronto2lirc
