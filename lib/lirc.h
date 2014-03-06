/*
 * lirc.h - linux infrared remote control header file
 * last modified 2010/06/03 by Jarod Wilson
 */

#ifndef _LINUX_LIRC_H
#define _LINUX_LIRC_H

#include "lirc/ciniparser.h"
#include "lirc/lirc_base.h"
#include "lirc/lirc_log.h"
#include "lirc/lircd_base.h"
#include "lirc/lirc_options.h"
#include "lirc/config_file.h"
#include "lirc/dump_config.h"
#include "lirc/input_map.h"
#include "lirc/hardware.h"
#include "lirc/ir_remote_types.h"
#include "lirc/hw-types.h"
#include "lirc/ir_remote.h"
#include "lirc/ir_remote_types.h"
#include "lirc/receive.h"
#include "lirc/release.h"
#include "lirc/serial.h"
#include "lirc/transmit.h"

int waitfordata(long maxusec);	// from lircd.h, avoiding complete header.
#endif
