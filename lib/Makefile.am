ACLOCAL_AMFLAGS             = -I m4

BUILT_SOURCES               = lirc/input_map.inc

lib_LTLIBRARIES		    = liblirc.la liblirc_client.la

liblirc_client_la_SOURCES   = lirc_client.c lirc_client.h
liblirc_client_la_LDFLAGS   = -version-info 2:1:2

liblirc_la_SOURCES          = config_file.c \
			      ciniparser.c \
			      dictionary.c \
                              dump_config.c \
                              hw-types.c \
	                      input_map.c \
                              ir_remote.c \
			      lirc_log.c \
			      lircd_base.c \
			      lirc_options.c \
                              receive.c  \
                              release.c \
                              serial.c \
                              transmit.c

dist_include_HEADERS        = lirc.h lirc_client.h

lircincludedir              = $(includedir)/lirc
dist_lircinclude_HEADERS    = lirc_client.h \
			      lirc/config_file.h \
			      lirc/ciniparser.h \
			      lirc/dictionary.h \
                              lirc/dump_config.h \
                              lirc/hardware.h \
                              lirc/hw-types.h \
	                      lirc/input_map.h \
                              lirc/ir_remote.h \
                              lirc/ir_remote_types.h \
			      lirc/lirc_log.h \
			      lirc/lirc_base.h \
			      lirc/lircd_base.h \
			      lirc/lirc_options.h \
			      lirc/release.h \
                              lirc/receive.h \
                              lirc/serial.h \
                              lirc/transmit.h

lirc/input_map.inc:
	$(srcdir)/input_map.sh >$@

DISTCLEANFILES = input_map.inc
EXTRA_DIST = lirc/input_map.inc input_map.sh

