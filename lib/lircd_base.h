/*      $Id: lircd.h,v 5.19 2010/03/20 16:18:30 lirc Exp $      */

/****************************************************************************
 ** lircd.h *****************************************************************
 ****************************************************************************
 *
 */

#ifndef _LIRCD_BASE_H
#define _LIRCD_BASE_H

#include <syslog.h>
#include <sys/time.h>

#include "ir_remote.h"

extern int debug;

int waitfordata(long maxusec);

#endif /* _LIRCD_H */
