/****************************************************************************
 ** lirc_options.c **********************************************************
 ****************************************************************************
 *
 * options.c - global options access.
 *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>

#include "lirc/ciniparser.h"
#include "lirc/lircd_base.h"
#include "lirc/lirc_options.h"

char* progname = "lircd";

/* Default options file. */
static const char* const LIRC_OPTIONS_PATH = "/etc/lirc/lirc_options.conf";

/* Environment variable overriding default config file location. */
const char* const LIRC_OPTIONS_VAR  = "LIRC_OPTIONS_PATH";

/* Default userspace drivers directory. */
const char* const PLUGINDIR = "/usr/lib/lirc/plugins";

/*  Environment variable providing default for plugindir. */
const char* const PLUGINDIR_VAR = "LIRC_PLUGINDIR";

// FIXME: these two should go to configure.ac/config.h.

static const char* const default_permissions = "666";

dictionary* lirc_options = NULL;

static void parse_options(int argc, char** argv);  /* forward. */

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{"nodaemon", no_argument, NULL, 'n'},
	{"options-file", required_argument, NULL, 'O'},
	{"permission", required_argument, NULL, 'p'},
	{"driver", required_argument, NULL, 'H'},
	{"device", required_argument, NULL, 'd'},
	{"listen", optional_argument, NULL, 'l'},
	{"connect", required_argument, NULL, 'c'},
	{"output", required_argument, NULL, 'o'},
	{"pidfile", required_argument, NULL, 'P'},
	{"plugindir", required_argument, NULL, 'U'},
#       ifndef USE_SYSLOG
	{"logfile", required_argument, NULL, 'L'},
#       endif
#       ifdef DEBUG
	{"debug", optional_argument, NULL, 'D'},
#       endif
	{"release", optional_argument, NULL, 'r'},
	{"allow-simulate", no_argument, NULL, 'a'},
#        if defined(__linux__)
	{"uinput", no_argument, NULL, 'u'},
#        endif
	{"repeat-max", required_argument, NULL, 'R'},
	{0, 0, 0, 0}
};

static void help(void)
{
	printf("Usage: %s [options] <config-file>\n", progname);
	printf("\t -h --help\t\t\tdisplay this message\n");
	printf("\t -v --version\t\t\tdisplay version\n");
	printf("\t -O --options-file\t\toptions file\n");
	printf("\t -n --nodaemon\t\t\tdon't fork to background\n");
	printf("\t -p --permission=mode\t\tfile permissions for " LIRCD "\n");
	printf("\t -H --driver=driver\t\tuse given driver (-H help lists drivers)\n");
	printf("\t -d --device=device\t\tread from given device\n");
	printf("\t -l --listen[=[address:]port]\tlisten for network connections\n");
	printf("\t -c --connect=host[:port]\tconnect to remote lircd server\n");
	printf("\t -o --output=socket\t\toutput socket filename\n");
	printf("\t -P --pidfile=file\t\tdaemon pid file\n");
	printf("\t -U --plugindir=directory\tdriver directory\n");
#       ifndef USE_SYSLOG
	printf("\t -L --logfile=file\t\tdaemon log file\n");
#       endif
#       ifdef DEBUG
	printf("\t -D[debug_level] --debug[=debug_level]\n");
#       endif
	printf("\t -r --release[=suffix]\t\tauto-generate release events\n");
	printf("\t -a --allow-simulate\t\taccept SIMULATE command\n");
#       if defined(__linux__)
	printf("\t -u --uinput\t\t\tgenerate Linux input events\n");
#       endif
	printf("\t -R --repeat-max=limit\t\tallow at most this many repeats\n");
}

static void set_option(char* key, char* value)
{
	if (dictionary_set(lirc_options, key, value) != 0)
		fprintf(stderr, "Cannot set option %s to %s\n", key, value);
}


static void add_defaults(void)
{
	const char* const defaults[] = {
		"lircd:nodaemon", 	"False",
		"lircd:permission", 	default_permissions,
		"lircd:driver", 	"default",
		"lircd:device", 	"/dev/lirc0",
		"lircd:listen", 	NULL ,
                "lircd:connect", 	NULL,
                "lircd:output", 	LIRCD,
                "lircd:pidfile", 	PIDFILE,
                "lircd:logfile", 	LOGFILE,
                "lircd:plugindir", 	PLUGINDIR,
		"lircd:debug", 		"False",
		"lircd:release", 	NULL,
		"lircd:allow_simulate", "False",
		"lircd:uinput", 	"False",
		"lircd:repeat-max", 	DEFAULT_REPEAT_MAX,
                (const char*)NULL, 	(const char*)NULL
	};
	int i;
	const char* key;
	const char* value;

	for(i = 0; defaults[i] != NULL; i += 2){
		key = defaults[i];
		value = defaults[i + 1];
		if (ciniparser_getstring(lirc_options, key, NULL) == NULL)
			set_option((char*)key, (char*)value);
	}
}


static void load_config(int argc, char** argv, char* path)
{
	lirc_options = ciniparser_load(path);
	if (lirc_options == NULL) {
		fprintf(stderr,
			"Warning: cannot load options file %s\n", path);
		lirc_options = dictionary_new(0);
	}
	add_defaults();
	optind = 1;
	parse_options(argc, argv);
}



static void parse_options(int argc, char** argv)
{
	int c;
	const char* optstring =  "hvnp:H:d:o:P:U:l::c:r::aR:"
#       if defined(__linux__)
		"u"
#       endif
#       ifndef USE_SYSLOG
		"L:"
#       endif
#       ifdef DEBUG
		"D::"
#       endif
	;

	while ((c = getopt_long(argc, argv, optstring, long_options, NULL))
		!= -1 )
	{
		switch (c) {
		case 'h':
			help();
			exit(EXIT_SUCCESS);
		case 'v':
			printf("%s %s\n", progname, VERSION);
			exit(EXIT_SUCCESS);
		case 'O':
			load_config(argc, argv, optarg);
			return;
		case 'n':
			set_option("lircd:nodaemon", "True");
			break;
		case 'p':
			set_option("lircd:permission", optarg);
			break;
		case 'H':
			set_option("lircd:driver", optarg);
			break;
		case 'd':
			set_option("lircd:device", optarg);
			break;
		case 'P':
			set_option("lircd:pidfile", optarg);
			break;
		case 'U':
			set_option("lircd:plugindir", optarg);
			break;
#               ifndef USE_SYSLOG
		case 'L':
			set_option("lircd:logfile", optarg);
			break;
#               endif
		case 'o':
			set_option("lircd:lircdfile", optarg);
			break;
		case 'l':
			set_option("lircd:listen", "True");
			set_option("lircd:listen_hostport", optarg);
			break;
		case 'c':
			set_option("lircd:connect", optarg);
			break;
#               ifdef DEBUG
		case 'D':
			set_option("lircd:debug", optarg ? optarg : "1");
			debug = 1;
			break;
#               endif
		case 'a':
			set_option("lircd:allow-simulate", "True");
			break;
		case 'r':
			set_option("lircd:release", "True");
			set_option("lircd:release_suffix",
                                   optarg ? optarg : LIRC_RELEASE_SUFFIX);
			break;
#               if defined(__linux__)
		case 'u':
			set_option("lircd:uinput", "True");
			break;
#               endif
		case 'R':
			set_option("lircd:repeat-max", optarg);
			break;
		default:
			printf("Usage: %s [options] [config-file]\n", progname);
			exit(EXIT_FAILURE);
		}
	}
	if (optind == argc - 1) {
		set_option("configfile", argv[optind]);
	} else if (optind != argc) {
		fprintf(stderr, "%s: invalid argument count\n", progname);
		exit(EXIT_FAILURE);
	}
}


void lirc_options_init(int argc, char **argv)
{
	const char* options_path;

        options_path = getenv( LIRC_OPTIONS_VAR );
        if (options_path == NULL)
		options_path = LIRC_OPTIONS_PATH;
	load_config(argc, argv, (char*) options_path);
#	ifdef DEBUG
	if (debug && lirc_options != NULL ) {
		fprintf(stderr, "Dumping parsed option values:\n" );
		ciniparser_dump(lirc_options, stdout);
	}
#	endif
}
