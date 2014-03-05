ACLOCAL_AMFLAGS             = -I m4
lib_LTLIBRARIES		    = liblirc.la
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

include_HEADERS             = lirc.h

lirc_includedir             = /usr/include/lirc
lirc_include_HEADERS        = config_file.h \
			      ciniparser.h \
			      dictionary.h \
                              dump_config.h \
                              hardware.h \
                              hw-types.h \
	                      input_map.h \
                              ir_remote.h \
                              ir_remote_types.h \
			      lirc_log.h \
			      lirc_base.h \
			      lircd_base.h \
			      lirc_options.h \
			      release.h \
                              receive.h \
                              serial.h \
                              transmit.h

input_map.inc:
	$(srcdir)/input_map.sh >$@

DISTCLEANFILES = input_map.inc
EXTRA_DIST = input_map.inc input_map.sh

