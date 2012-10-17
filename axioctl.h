#ifndef axioctl_h
#define axioctl_h

/* INCLUDE FILE DECLARATIONS */
#include "command.h"

/* NAMING CONSTANT DECLARATIONS */

struct ax_command_info { 
	int inet_sock;
	struct ifreq *ifr;
	int argc;
	char **argv;
	unsigned short ioctl_cmd;
	const char *help_ins;
	const char *help_desc;
};

const char help_str1[] =
"./axioctl help [command]\n"
"    -- command description\n";
const char help_str2[] =
"        [command] - Display usage of specified command\n";

const char readeeprom_str1[] =
"./axioctl reeprom [type] [file] [size]\n"
"    -- AX88179_178A EEPROM/eFuse read tool\n";
const char readeeprom_str2[] =
"        [type]    - 0: EEPROM,  1: eFuse\n"
"        [file]    - Output file\n"
"        [size]    - EEPROM/eFuse SIZE (bytes). EEPROM maximum 512 bytes, eFuse maximum 64 bytes.\n";

const char writeeeprom_str1[] =
"./axioctl weeprom [type] [file] [size]\n"
"    -- AX88179_178A EEPROM/eFuse write tool\n";
const char writeeeprom_str2[] =
"        [type]    - 0: EEPROM,  1: eFuse\n"
"        [file]    - Input file\n"
"        [size]    - EEPROM/eFuse SIZE (bytes). EEPROM size 12-512 bytes, eFuse maximum 64 bytes.\n";

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void help_func (struct ax_command_info *info);
void readeeprom_func(struct ax_command_info *info);
void writeeeprom_func(struct ax_command_info *info);

/* TYPE DECLARATIONS */

typedef void (*MENU_FUNC)(struct ax_command_info *info);

struct {
	char *cmd;
	unsigned short ioctl_cmd;
	MENU_FUNC OptFunc;
	const char *help_ins;
	const char *help_desc;
} command_list[] = {
	{"help",	AX_SIGNATURE,		help_func,		help_str1,		help_str2},
	{"reeprom",	AX_READ_EEPROM,		readeeprom_func,	readeeprom_str1,	readeeprom_str2},
	{"weeprom", 	AX_WRITE_EEPROM, 	writeeeprom_func,	writeeeprom_str1,	writeeeprom_str2},
	{NULL},
};

#endif /* End of console_debug_h */
