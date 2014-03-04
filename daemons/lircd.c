/*      $Id: lircd.c,v 5.96 2010/07/09 16:54:48 lirc Exp $      */

/****************************************************************************
 ** lircd.c *****************************************************************
 ****************************************************************************
 *
 * lircd - LIRC Decoder Daemon
 *
 * Copyright (C) 1996,97 Ralph Metzler <rjkm@thp.uni-koeln.de>
 * Copyright (C) 1998,99 Christoph Bartelmus <lirc@bartelmus.de>
 *
 *  =======
 *  HISTORY
 *  =======
 *
 * 0.1:  03/27/96  decode SONY infra-red signals
 *                 create mousesystems mouse signals on pipe /dev/lircm
 *       04/07/96  send ir-codes to clients via socket (see irpty)
 *       05/16/96  now using ir_remotes for decoding
 *                 much easier now to describe new remotes
 *
 * 0.5:  09/02/98 finished (nearly) complete rewrite (Christoph)
 *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* disable daemonise if maintainer mode SIM_REC / SIM_SEND defined */
#if defined(SIM_REC) || defined (SIM_SEND)
# undef DAEMONIZE
#endif

#define _GNU_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/file.h>

#if defined(__linux__)
#include <linux/input.h>
#include <linux/uinput.h>
#include "lirc/input_map.h"
#endif

#ifdef HAVE_SYSTEMD
#include "systemd/sd-daemon.h"
#endif

#if defined __APPLE__  || defined __FreeBSD__
#include <sys/ioctl.h>
#endif

#ifndef timersub
#define timersub(a, b, result)                                            \
  do {                                                                    \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                         \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                      \
    if ((result)->tv_usec < 0) {                                          \
      --(result)->tv_sec;                                                 \
      (result)->tv_usec += 1000000;                                       \
    }                                                                     \
  } while (0)
#endif

#include "lirc.h"


#define WHITE_SPACE " \t"

struct protocol_directive {
	char *name;
	int (*function) (int fd, char *message, char *arguments);
};

void config(void);
void remove_client(int fd);
void broadcast_message(const char *message);

extern struct ir_remote *remotes;
extern struct ir_remote *free_remotes;

extern struct ir_remote *decoding;
extern struct ir_remote *last_remote;
extern struct ir_remote *repeat_remote;
extern struct ir_ncode *repeat_code;

static __u32 repeat_max = REPEAT_MAX_DEFAULT;

extern struct hardware hw;

extern char *pidfile;
extern char *lircdfile;

/* substract one for lirc, sockfd, sockinet, logfile, pidfile, uinput */
#define MAX_PEERS	((FD_SETSIZE-6)/2)
#define MAX_CLIENTS     ((FD_SETSIZE-6)/2)

/* Default options file. */
#define LIRC_OPTIONS_PATH   "/etc/lirc/lirc_options.conf"

struct peer_connection {
	char *host;
	unsigned short port;
	struct timeval reconnect;
	int connection_failure;
	int socket;
};


extern int listen_tcpip;
extern unsigned short int port;
extern struct in_addr address;
extern int peern;
extern  int allow_simulate;
extern  int userelease;
extern  int useuinput;

#ifdef DEBUG
extern int debug;
#endif


/* cut'n'paste from fileutils-3.16: */

#define isodigit(c) ((c) >= '0' && (c) <= '7')

// Private entries in lircd_base

/* A safer write(), since sockets might not write all but only some of the
   bytes requested */

int write_socket(int fd, const char *buf, int len);

int write_socket_len(int fd, const char *buf);

int read_timeout(int fd, char *buf, int len, int timeout);

void sigterm(int sig);

void dosigterm(int sig);

void sighup(int sig);

void dosighup(int sig);

int setup_uinputfd(const char *name);

void config(void);

void nolinger(int sock);

void remove_client(int fd);

void add_client(int sock);
int add_peer_connection(char *server);

void connect_to_peers();

int get_peer_message(struct peer_connection *peer);

void start_server(mode_t permission, int nodaemon);

#ifdef DAEMONIZE
void daemonize(void);
#endif /* DAEMONIZE */

void sigalrm(int sig);

void dosigalrm(int sig);

int parse_rc(int fd, char *message, char *arguments, struct ir_remote **remote,
	     struct ir_ncode **code, int *reps, int n, int *err);


int send_remote_list(int fd, char *message);

int send_remote(int fd, char *message, struct ir_remote *remote);

int send_name(int fd, char *message, struct ir_ncode *code);

int get_command(int fd);

void free_old_remotes();

void input_message(const char *message, const char *remote_name,
	  	   const char *button_name, int reps, int release);

int waitfordata(long maxusec);


/* Return a positive integer containing the value of the ASCII
 *    octal number S.  If S is not an octal number, return -1.  */

static int oatoi(s)
char *s;
{
        register int i;

        if (s == NULL || *s == 0)
                return -1;
        for (i = 0; isodigit(*s); ++s)
                i = i * 8 + *s - '0';
        if (*s)
                return -1;
        return i;
}



void loop()
{
	char *message;

	logprintf(LOG_NOTICE, "lircd(%s) ready, using %s", hw.name, lircdfile);
	while (1) {
		(void)waitfordata(0);
		if (!hw.rec_func)
			continue;
		message = hw.rec_func(remotes);

		if (message != NULL) {
			const char *remote_name;
			const char *button_name;
			int reps;

			if (hw.ioctl_func && (hw.features & LIRC_CAN_NOTIFY_DECODE)) {
				hw.ioctl_func(LIRC_NOTIFY_DECODE, NULL);
			}

			get_release_data(&remote_name, &button_name, &reps);

			input_message(message, remote_name, button_name, reps, 0);
		}
	}
}


static int opt2host_port(const char* optarg,
	  		 struct in_addr* address,
			 unsigned short* port,
		         char* errmsg)
{
	long p;
	char *endptr;
	char *sep = strchr(optarg, ':');
	const char *port_str = sep ? sep + 1 : optarg;

	p = strtol(port_str, &endptr, 10);
	if (!*optarg || *endptr || p < 1 || p > USHRT_MAX) {
		sprintf(errmsg,
			"%s: bad port number \"%s\"\n", progname, port_str);
		return -1;
	}
	*port = (unsigned short int)p;
	if (sep) {
		*sep = 0;
		if (!inet_aton(optarg, address)) {
			sprintf(errmsg,
                                "%s: bad address \"%s\"\n", progname, optarg);
			return -1;
		}
	}
	return 0;
}


int main(int argc, char **argv)
{
	struct sigaction act;
	int nodaemon = 0;
	mode_t permission;
	char *device = NULL;
	char errmsg[128];
	char* opt;

	address.s_addr = htonl(INADDR_ANY);
	hw_choose_driver(NULL);
	lirc_options_init(argc, argv);

	nodaemon = ciniparser_getboolean(lirc_options, "lircd:nodaemon", 0);
	opt = ciniparser_getstring(lirc_options, "lircd:permission", 0);
	if (oatoi(opt) == -1) {
		fprintf(stderr, "%s: Invalid mode %s\n", progname, opt);
		return(EXIT_FAILURE);
	}
	permission = oatoi(opt);
	opt = ciniparser_getstring(lirc_options, "lircd:driver", NULL);
	if (strcmp(opt, "help") == 0 || strcmp(opt, "?") == 0){
		hw_print_drivers(stdout);
		return(EXIT_SUCCESS);
	}
        else if (hw_choose_driver(opt) != 0) {
		fprintf(stderr, "Driver `%s' not supported.\n", optarg);
		hw_print_drivers(stderr);
		return(EXIT_FAILURE);
	}
	opt = ciniparser_getstring(lirc_options, "lircd:device", NULL);
	if (opt != NULL)
		device = opt;
	pidfile = ciniparser_getstring(lirc_options, "lircd:pidfile", NULL);
#       ifndef USE_SYSLOG
		opt = ciniparser_getstring(lirc_options, "lircd:logfile", NULL);
		if (opt != NULL)
			lirc_set_logfile(optarg);
#       endif
	lircdfile = ciniparser_getstring(lirc_options, "lircd:output", NULL);
	if (ciniparser_getstring(lirc_options, "lircd:listen", NULL) != NULL){
		listen_tcpip = 1;
		opt = ciniparser_getstring(lirc_options,
					   "lircd:listen_hostport", NULL);
		if (opt){
			if (opt2host_port(opt, &address, &port, errmsg) != 0){
				fprintf(stderr, errmsg);
				return(EXIT_FAILURE);
			}
		} else
			port =  LIRC_INET_PORT;
	}
	opt = ciniparser_getstring(lirc_options, "lircd:connect", NULL);
	if (opt != NULL) {
		if (!add_peer_connection(optarg))
			return(EXIT_FAILURE);
	}
#       ifdef DEBUG
	debug = ciniparser_getboolean(lirc_options, "lircd:debug", 0);
#       endif
	userelease = ciniparser_getboolean(lirc_options, "lircd:release", 0);
	set_release_suffix(ciniparser_getstring(lirc_options,
						"lircd:release_suffix",
						LIRC_RELEASE_SUFFIX));
	allow_simulate = ciniparser_getboolean(lirc_options,
					       "lircd:allow_simulate", 0);
#       if defined(__linux__)
	useuinput = ciniparser_getboolean(lirc_options, "lircd:uinput", 0);
#       endif
	repeat_max = ciniparser_getint(lirc_options, "lircd:repeat-max", 0);

	if (device != NULL) {
		hw.device = device;
	}
	if (strcmp(hw.name, "null") == 0 && peern == 0) {
		fprintf(stderr, "%s: there's no hardware I can use and no peers are specified\n", progname);
		return (EXIT_FAILURE);
	}
	if (hw.device != NULL && strcmp(hw.device, lircdfile) == 0) {
		fprintf(stderr, "%s: refusing to connect to myself\n", progname);
		fprintf(stderr, "%s: device and output must not be the same file: %s\n", progname, lircdfile);
		return (EXIT_FAILURE);
	}

	signal(SIGPIPE, SIG_IGN);

	start_server(permission, nodaemon);

	act.sa_handler = sigterm;
	sigfillset(&act.sa_mask);
	act.sa_flags = SA_RESTART;	/* don't fiddle with EINTR */
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT, &act, NULL);

	act.sa_handler = sigalrm;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;	/* don't fiddle with EINTR */
	sigaction(SIGALRM, &act, NULL);

	remotes = NULL;
	config();		/* read config file */

	act.sa_handler = sighup;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;	/* don't fiddle with EINTR */
	sigaction(SIGHUP, &act, NULL);

#ifdef DAEMONIZE
	/* ready to accept connections */
	if (!nodaemon)
		daemonize();
#endif

#if defined(SIM_SEND) && !defined(DAEMONIZE)
	{
		struct ir_remote *r;
		struct ir_ncode *c;

		if (hw.init_func) {
			if (!hw.init_func())
				dosigterm(SIGTERM);
		}

		printf("space 1000000\n");
		r = remotes;
		while (r != NULL) {
			c = r->codes;
			while (c->name != NULL) {
				repeat_remote = NULL;
				repeat_code = NULL;
				c->transmit_state = NULL;
				send_ir_ncode(r, c);
				repeat_remote = r;
				repeat_code = c;
				send_ir_ncode(r, c);
				send_ir_ncode(r, c);
				send_ir_ncode(r, c);
				send_ir_ncode(r, c);
				c++;
			}
			r = r->next;
		}
		fflush(stdout);
		if (hw.deinit_func)
			hw.deinit_func();
	}
	fprintf(stderr, "Ready.\n");
	dosigterm(SIGTERM);
#endif
	loop();

	/* never reached */
	return (EXIT_SUCCESS);
}
