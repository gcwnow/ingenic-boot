/*
 * Copyright(C) 2012 Ingenic Semiconductor Inc.
 * Authors: Duke Fong <duke@dukelec.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <usb.h>
#include "../basic_cmd_lib/basic_cmd_lib.h"
#include "stage2.h"

struct ingenic_dev *ingenic_dev;

static int usb_ingenic_configration(struct ingenic_dev *ingenic_dev, int ops)
{
	int status;
	status = usb_control_msg(ingenic_dev->usb_handle,
	  /* bmRequestType */ USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
	  /* bRequest      */ VR_CONFIGRATION,
	  /* wValue        */ ops,
	  /* wIndex        */ 0,
	  /* Data          */ 0,
	  /* wLength       */ 0,
			      USB_TIMEOUT);

	if (status != 0) {
		fprintf(stderr, "Error - "
			"can't init Ingenic configration: %i\n", status);
		return -1;
	}

	return 0;
}

int usb_ingenic_nand_ops(struct ingenic_dev *ingenic_dev, int ops)
{
	int status;
	status = usb_control_msg(ingenic_dev->usb_handle,
	  /* bmRequestType */ USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
	  /* bRequest      */ VR_NAND_OPS,
	  /* wValue        */ ops & 0xffff,
	  /* wIndex        */ 0,
	  /* Data          */ 0,
	  /* wLength       */ 0,
			      USB_TIMEOUT);

	if (status != 0) {
		fprintf(stderr, "Error - "
			"can't set Ingenic device nand ops: %i\n", status);
		return -1;
	}

	return 0;
}

static int usb_ingenic_sdram_ops(struct ingenic_dev *ingenic_dev, int ops)
{
	int status;
	status = usb_control_msg(ingenic_dev->usb_handle,
	  /* bmRequestType */ USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
	  /* bRequest      */ VR_SDRAM_OPS,
	  /* wValue        */ ops,
	  /* wIndex        */ 0,
	  /* Data          */ 0,
	  /* wLength       */ 0,
			      USB_TIMEOUT);

	if (status != 0) {
		fprintf(stderr, "Error - "
			"Device can't load file to sdram: %i\n", status);
		return -1;
	}

	return 0;
}

static int usb_ingenic_reset(struct ingenic_dev *ingenic_dev, int ops)
{
	int status;
	status = usb_control_msg(ingenic_dev->usb_handle,
	  /* bmRequestType */ USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
	  /* bRequest      */ VR_RESET,
	  /* wValue        */ ops,
	  /* wIndex        */ 0,
	  /* Data          */ 0,
	  /* wLength       */ 0,
			      USB_TIMEOUT);

	if (status != 0) {
		fprintf(stderr, "Error - "
			"reset XBurst device: %i\n", status);
		return -1;
	}

	return 0;
}

/* after download stage2. must init device */
static int init_config(struct ingenic_dev *ingenic_dev, char *buf,
		       unsigned int len)
{
	unsigned char ret[8];
	//ingenic_dev->hand.fw_args.cpu_id = ingenic_dev->cpu_id;

	/* send data first */
	if (usb_send_data_to_ingenic(ingenic_dev, buf, len))
		goto xout;

	usleep(2000);
	if (usb_ingenic_configration(ingenic_dev, DS_hand))
		goto xout;

	if (usb_read_data_from_ingenic(ingenic_dev, ret, 8))
		goto xout;

	printf(" Configuring XBurst CPU succeeded.\n");
	return 0;
xout:
	printf(" Configuring XBurst CPU failed.\n");
	return -1;

}

static void dev_init()
{
	const char *bus_filter = getenv("IBBUS");
	const char *dev_filter = getenv("IBDEV");

	ingenic_dev = malloc(sizeof (struct ingenic_dev));
	memset(ingenic_dev, 0, sizeof(struct ingenic_dev));
	if (usb_ingenic_init(ingenic_dev, bus_filter, dev_filter))
		exit (-1);
}

int main (int argc, char *argv[])
{
	if (argc != 2)
		goto helpmsg;

	/* /\* prepare the realpath *\/ */
	/* readlink("/proc/self/exe", ingenic_dev->tool_cfg.realpath, PATH_MAX); */
	/* dirname(ingenic_dev->tool_cfg.realpath); */
	/* strcat(ingenic_dev->tool_cfg.realpath, "/"); */

	if (strncmp(argv[1], "config=", 7) == 0) {

		struct stat fstat;
		unsigned int len;
		char *buf;

		argv[1] += 7;

		int status = stat(argv[1], &fstat);

		if (status < 0) {
			fprintf(stderr, "Error - can't get file size from '%s': %s\n",
				argv[1], strerror(errno));
			return -1;
		}

		len = fstat.st_size;
		buf = malloc(fstat.st_size);

		int fd = open(argv[1], O_RDONLY);

		if (fd < 0) {
			fprintf(stderr, "Error - can't open file '%s': %s\n",
				argv[1], strerror(errno));
			return -1;
		}

		status = read(fd, buf, len);
		close(fd);

		if (status < len) {
			fprintf(stderr, "Error - can't read file '%s': %s\n",
				argv[1], strerror(errno));
		}

		dev_init();
		if (init_config(ingenic_dev, buf, len))
			return -1;
		return 0;
	}

	if (strcmp(argv[1], "reboot") == 0) {
		dev_init();
		if (usb_ingenic_sdram_ops(ingenic_dev, DEVICE_RESET))
			return -1;
		return 0;
	}

	if (strcmp(argv[1], "sdinit") == 0) {
		dev_init();
		if (sd_card_init(ingenic_dev))
			return -1;
		return 0;
	}

	if (strncmp(argv[1], "sdif=", 3) == 0) {
		/* filename@addr#check */

		argv[1] += 5;
		char *filename = strtok(argv[1], "@#");
		/* TODO: check strtok() != NULL */
		int addr = strtoul(strtok(NULL, "@#"), NULL, 0);
		int check = strtoul(strtok(NULL, "@#"), NULL, 0);

		if (check != 1)
			check = 0;

		printf("filename=%s, addr=0x%08x, check=%d\n", filename, addr, check);

		dev_init();

		return sd_card_program(ingenic_dev, addr, filename, check);
	}

helpmsg:
	printf("Command of this tool:\n"
	       "	config=hand.bin	: configure stage2 function\n"
	       "	reboot		: reboot device\n"
	       "	sdinit		: init sd function\n"
	       "	sdif=filename@addr#check\n"
	       "			: download file to specify addr of sd card\n"
	       "			  check=0 or 1\n");

	exit (0);
}
