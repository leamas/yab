/****************************************************************************
 ** options.h ***************************************************************
 ****************************************************************************
 *
 * options.h - global options access.
 *
 */
#include "ciniparser.h"

#define DEFAULT_REPEAT_MAX       "600"    // FIXME: duplicate from ir_remotes_types.h

/* Program name as used in user messages. */
extern char* progname;

/* Environment variable overriding default config file location. */
extern const char* const LIRC_OPTIONS_VAR;

/* Default userspace drivers directory. */
extern const char* const PLUGINDIR;

/*  Environment variable providing default for plugindir. */
extern const char* const PLUGINDIR_VAR;

/* Global options instance with all option values. */
extern dictionary* lirc_options;

/*
 *   Parse global option file and command line. Exits om errors and
 *   simple actions
 *   Arguments:
 *      argc, argv; As handled to main()
 *
 */
void lirc_options_init(int argc, char **argv);
