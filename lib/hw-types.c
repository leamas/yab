#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(SIM_REC) || defined (SIM_SEND)
#undef HW_DEFAULT
#undef LIRC_DRIVER_ANY
#endif

#include "lirc/hardware.h"
#include "lirc/hw-types.h"
#include "lirc/lirc_options.h"


struct hardware hw_null = {
	"/dev/null",		/* default device */
	-1,			/* fd */
	0,			/* features */
	0,			/* send_mode */
	0,			/* rec_mode */
	0,			/* code_length */
	NULL,			/* init_func */
	NULL,			/* deinit_func */
	NULL,			/* send_func */
	NULL,			/* rec_func */
	NULL,			/* decode_func */
	NULL,			/* ioctl_func */
	NULL,			/* readdata */
	"null",			/* name */
};

struct hardware *hw_list[] = {
        &hw_null,
	NULL
};  // FIXME

struct hardware hw_default = {
	"/dev/null",		/* default device */
	-1,			/* fd */
	0,			/* features */
	0,			/* send_mode */
	0,			/* rec_mode */
	0,			/* code_length */
	NULL,			/* init_func */
	NULL,			/* deinit_func */
	NULL,			/* send_func */
	NULL,			/* rec_func */
	NULL,			/* decode_func */
	NULL,			/* ioctl_func */
	NULL,			/* readdata */
	"null",			/* name */
};  //FIXME


struct hardware hw;

// which one is HW_DEFAULT could be selected with autoconf in a similar
// way as it is now done upstream

int hw_choose_driver(char *name)
{
	int i;

	if (name == NULL) {
		hw = HW_DEFAULT;
		return 0;
	}
	if (strcasecmp(name, "dev/input") == 0) {
		/* backwards compatibility */
		name = "devinput";
	}
	for (i = 0; hw_list[i]; i++)
		if (!strcasecmp(hw_list[i]->name, name))
			break;
	if (!hw_list[i])
		return -1;
	hw = *hw_list[i];

	return 0;
}

void hw_print_drivers(FILE * file)
{
	int i;
	fprintf(file, "Supported drivers:\n");
	for (i = 0; hw_list[i]; i++)
		fprintf(file, "\t%s\n", hw_list[i]->name);
}
