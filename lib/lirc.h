/*
 * lirc.h - linux infrared remote control header file
 * last modified 2010/06/03 by Jarod Wilson
 */

#ifndef _LINUX_LIRC_H
#define _LINUX_LIRC_H

#include "ciniparser.h"
#include "lirc_base.h"
#include "lirc_log.h"
#include "lircd_base.h"
#include "lircd_options.h"
#include "config_file.h"
#include "dump_config.h"
#include "input_map.h"
#include "hardware.h"
#include "ir_remote_types.h"
#include "hw-types.h"
#include "ir_remote.h"
#include "ir_remote_types.h"
#include "receive.h"
#include "release.h"
#include "serial.h"
#include "transmit.h"

int waitfordata(long maxusec);	// from lircd.h, avoiding complete header.
#endif
