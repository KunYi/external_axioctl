/*==========================================================================
 * Module Name : console_debug.c
 * Purpose     : 
 * Author      : 
 * Date        : 
 * Notes       :
 * $Log$
 *==========================================================================
 */
 
/* INCLUDE FILE DECLARATIONS */
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <stdlib.h>
#include "axioctl.h"

/* STATIC VARIABLE DECLARATIONS */
#define AX88179_IOCTL_VERSION		"AX88179/AX88178A IOCTL version 1.0.0"

/* LOCAL SUBPROGRAM DECLARATIONS */
static unsigned long STR_TO_U32(const char *cp,char **endp,unsigned int base);


/* LOCAL SUBPROGRAM BODIES */
static void show_usage(void)
{
	int i;
	printf ("\n%s\n",AX88179_IOCTL_VERSION);
	printf ("Usage:\n");
	for (i = 0; command_list[i].cmd != NULL; i++)
		printf ("%s\n", command_list[i].help_ins);
}

static unsigned long STR_TO_U32(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;

	return result;
}

void help_func (struct ax_command_info *info)
{
	int i;

	if (info->argv[2] == NULL) {
		for(i=0; command_list[i].cmd != NULL; i++) {
			printf ("%s%s\n", command_list[i].help_ins, command_list[i].help_desc);
		}
	}

	for (i = 0; command_list[i].cmd != NULL; i++)
	{
		if (strncmp(info->argv[1], command_list[i].cmd, strlen(command_list[i].cmd)) == 0 ) {
			printf ("%s%s\n", command_list[i].help_ins, command_list[i].help_desc);
			return;
		}
	}

}

int compare_file(struct ax_command_info *info)
{
	struct ifreq *ifr = (struct ifreq *)info->ifr;
	unsigned short *rout_buf;
	unsigned short *ori_buf;
	AX_IOCTL_COMMAND *ioctl_cmd = (AX_IOCTL_COMMAND *)(ifr->ifr_data);
	int i;

	rout_buf = malloc(sizeof(unsigned short) * ioctl_cmd->size);

	ori_buf = ioctl_cmd->buf;	

	ioctl_cmd->ioctl_cmd = AX_READ_EEPROM;
	ioctl_cmd->buf = rout_buf;

	if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
		perror("axioctl");
		return -1;
	}

	for (i = 0; i < ioctl_cmd->size; i++) {
		if (*(ioctl_cmd->buf + i) != *(ori_buf + i)) {
			ioctl_cmd->buf = ori_buf;
			free(rout_buf);
			return -1;
		}
	}

	ioctl_cmd->buf = ori_buf;
	free(rout_buf);
	return 0;
}

void readeeprom_func(struct ax_command_info *info)
{
	struct ifreq *ifr = (struct ifreq *)info->ifr;
	AX_IOCTL_COMMAND ioctl_cmd;
	unsigned short *buf;
	unsigned short wLen;
	char str_buf[5];
	unsigned char type;
	FILE *pFile;
	int i;	

	type = STR_TO_U32(info->argv[2], NULL, 0);
	wLen = STR_TO_U32(info->argv[4], NULL, 0) / 2;

	if ((info->argc != 5) || (type > 1) || ((type==0) && (wLen > 256)) || ((type==1) && (wLen > 32))) {
		for(i=0; command_list[i].cmd != NULL; i++) {
			if (strncmp(info->argv[1], command_list[i].cmd, 
					strlen(command_list[i].cmd)) == 0 ) {
				printf ("%s%s\n", command_list[i].help_ins, 
						command_list[i].help_desc);
				return;
			}
		}
	}

	pFile = fopen(info->argv[3],"w");
	buf = (unsigned short *)malloc(sizeof(unsigned short) * wLen);

	ioctl_cmd.ioctl_cmd = info->ioctl_cmd;
	ioctl_cmd.size = wLen;
	ioctl_cmd.buf = buf;
	ioctl_cmd.type = type;

	ifr->ifr_data = (caddr_t)&ioctl_cmd;

	if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
		perror("axioctl");
		free(buf);
		fclose(pFile);
		return;
	}		

	for (i = 0; i < wLen; i++) {
		sprintf(str_buf, "%04x\n", *(buf + i));
		fputs(str_buf, pFile);
	}
	free(buf);
	fclose(pFile);
	printf("read completely\n");
	return;
}

void writeeeprom_func(struct ax_command_info *info)
{
	struct ifreq *ifr = (struct ifreq *)info->ifr;
	AX_IOCTL_COMMAND ioctl_cmd;
	int i;
	unsigned short *buf;
	unsigned long tmp;
	unsigned short wLen;
	char str_buf[5] = {'\0'}, c[2] = {'\0'};
	FILE *pFile;
	unsigned char retried = 0;
	unsigned char type;


	type = STR_TO_U32(info->argv[2], NULL, 0);
	wLen = STR_TO_U32(info->argv[4], NULL, 0) / 2;

	if ((info->argc != 5) || (type > 1) || ((type==0) && (wLen > 256)) || ((type==0) && (wLen < 6)) ||
		((type==1) && (wLen > 32))) {
		for(i=0; command_list[i].cmd != NULL; i++) {
			if (strncmp(info->argv[1], command_list[i].cmd,
					strlen(command_list[i].cmd)) == 0) {
				printf ("%s%s\n", command_list[i].help_ins,
						command_list[i].help_desc);
				return;
			}
		}
	}

	pFile = fopen(info->argv[3], "r");



	buf = (unsigned short *)malloc(sizeof(unsigned short) * wLen);

	for (i = 0; i < wLen; i++) {
			fgets(str_buf, 5, pFile);
			sscanf(str_buf, "%08X",(unsigned int *)&tmp);
			buf[i] = tmp;
			fgets(str_buf, 2, pFile);
			//sscanf(str_buf, "%s\r\n", c);
	} 

	ioctl_cmd.ioctl_cmd = info->ioctl_cmd;
	ioctl_cmd.size = wLen;
	ioctl_cmd.buf = buf;

	//check efuse empty or not.
	if (type) {
		ioctl_cmd.type = 2;
		ifr->ifr_data = (caddr_t)&ioctl_cmd;
		if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
			free(buf);
			fclose(pFile);
			perror("axioctl");
			return;
		}
		else {
			if(ioctl_cmd.type) {
				printf("EFuse has been programed.\n");
				return;
			}		
		}
	}

	ioctl_cmd.type = type;
io:	
	ifr->ifr_data = (caddr_t)&ioctl_cmd;

  	if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
		free(buf);
		fclose(pFile);
		perror("axioctl");
	}
	else {
		if (compare_file(info) && retried < 3) { 
			ioctl_cmd.ioctl_cmd = info->ioctl_cmd;
			retried++;
			goto io;
		}
		if (retried == 3) {
			printf("Failure to write\n");
			free(buf);
			fclose(pFile);
			return;
		}		
	}
	printf("Write completely\n");
	free(buf);
	fclose(pFile);	
	return;
}

/* EXPORTED SUBPROGRAM BODIES */
int main(int argc, char **argv)
{
	int inet_sock;
	struct ifreq ifr;
	char buf[0xff];
	struct ax_command_info info;
	unsigned char i;
	char	input;
	unsigned char count = 0;
	int fd, console;
	const unsigned char length = sizeof(char);
	AX_IOCTL_COMMAND ioctl_cmd;

	if (argc < 2) {
		show_usage();
		return 0;
	}

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

	/*  */
	for (i = 0; i < 255; i++) {

		memset (&ioctl_cmd, 0, sizeof (AX_IOCTL_COMMAND));
		ioctl_cmd.ioctl_cmd = AX_SIGNATURE;

		sprintf (ifr.ifr_name, "eth%d", i);
		ifr.ifr_data = (caddr_t)&ioctl_cmd;

		if (ioctl (inet_sock, AX_PRIVATE, &ifr) < 0) {
			//printf("i=%d\n",i);
				continue;
		}

		if (strncmp (ioctl_cmd.sig, AX8817XX_DRV_NAME, 8/*strlen(AX8817XX_DRV_NAME)*/) == 0 ) {
			break;
		}

	}

	if (i == 255) {
		printf ("No %s found\n", AX8817XX_SIGNATURE);
		return 0;
	}

	for(i=0; command_list[i].cmd != NULL; i++)
	{
		if (strncmp(argv[1], command_list[i].cmd, strlen(argv[1])) == 0 ) {
			//printf("z\n",i);

			info.help_ins = command_list[i].help_ins;
			info.help_desc = command_list[i].help_desc;
			info.ifr = &ifr;
			info.argc = argc;
			info.argv = argv;
			info.inet_sock = inet_sock;
			info.ioctl_cmd = command_list[i].ioctl_cmd;
			(command_list[i].OptFunc)(&info);
			//printf("success return\n",i);
			return 0;
		}
	}
	printf ("Wrong command\n");

	return 0;
}

