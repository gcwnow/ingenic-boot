/*
 * Copyright(C) 2006 Ingenic Semiconductor Inc.
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

#ifndef __BASIC_CMD_LIB_H__
#define __BASIC_CMD_LIB_H__

#include <errno.h>
#include <unistd.h>   
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <limits.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/*-------------------------------------------------------------------------*/

enum BOOT_STAGE {
	DISCONNECT = 0,
	UNBOOT,
	BOOT,
};

struct ingenic_dev {
	struct usb_device	*usb_dev;
	struct usb_dev_handle	*usb_handle;
	uint8_t			interface;
	unsigned int		cpu_id;
	enum BOOT_STAGE		boot_stage;
};

struct vid_pid {
	uint16_t		vid;
	uint16_t		pid;
};

/*-------------------------------------------------------------------------*/

#define USB_TIMEOUT	5000
#define INGENIC_OUT_ENDPOINT	0x01
#define INGENIC_IN_ENDPOINT	0x81
#define CODE_SIZE  ( 4 * 1024 * 1024 )
#define STAGE_ADDR_MSB(addr) ((addr) >> 16)
#define STAGE_ADDR_LSB(addr) ((addr) & 0xffff)

enum UDC_STATE
{
	IDLE,
	BULK_IN,
	BULK_OUT
};

enum USB_JZ4740_REQUEST_BASIC
{
	VR_GET_CPU_INFO = 0,
	VR_SET_DATA_ADDRESS,
	VR_SET_DATA_LENGTH,
	VR_FLUSH_CACHES,
	VR_PROGRAM_START1,
	VR_PROGRAM_START2
};

/*-------------------------------------------------------------------------*/

int usb_ingenic_init(struct ingenic_dev *ingenic_dev,
		     const char *bus_filter,
		     const char *dev_filter);
void usb_ingenic_cleanup(struct ingenic_dev *ingenic_dev);
int usb_get_ingenic_cpu(struct ingenic_dev *ingenic_dev);
int usb_ingenic_flush_cache(struct ingenic_dev *ingenic_dev);
int usb_send_data_length_to_ingenic(struct ingenic_dev *ingenic_dev, int len);
int usb_send_data_address_to_ingenic(struct ingenic_dev *ingenic_dev, 
				     unsigned int stage_addr);
int usb_send_data_to_ingenic(struct ingenic_dev *ingenic_dev,
			     unsigned char *buff, unsigned int len);
int usb_read_data_from_ingenic(struct ingenic_dev *ingenic_dev,
			       unsigned char *buff, unsigned int len);
int usb_ingenic_start(struct ingenic_dev *ingenic_dev, int rqst, int stage_addr);

/*-------------------------------------------------------------------------*/

#endif	/*__BASIC_CMD_LIB_H__ */
