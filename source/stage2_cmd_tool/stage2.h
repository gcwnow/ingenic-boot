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

#ifndef __STAGE2_CMD_TOOL_H__
#define __STAGE2_CMD_TOOL_H__

#include <errno.h>
#include <confuse.h>
#include <unistd.h>   
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <limits.h>

enum USB_JZ4740_REQUEST_STAGE2
{
	VR_NOR_OPS = 6,
	VR_NAND_OPS,
	VR_SDRAM_OPS,
	VR_CONFIGRATION,
	VR_RESET
};

enum NOR_OPS_TYPE
{
	NOR_INIT = 0,
	NOR_QUERY,
	NOR_WRITE,
	NOR_ERASE_CHIP,
	NOR_ERASE_SECTOR
};

enum NOR_FLASH_TYPE
{
	NOR_AM29 = 0,
	NOR_SST28,
	NOR_SST39x16,
	NOR_SST39x8
};

enum NAND_OPS_TYPE
{
	NAND_QUERY = 0,
	NAND_INIT,
	NAND_MARK_BAD,
	NAND_READ_RAW,
	NAND_ERASE,
	NAND_READ,
	NAND_PROGRAM,
	SET_RTC,
	NAND_READ_EXT,	//9
	NAND_FORCE_ERASE,
	NAND_FORMAT,
	CARD_INIT,
	CARD_PROGRAM,
	CARD_READ,
	CARD_ERASE,
	CARD_OPEN_CARD,
	CARD_QUERY_HW,
	NAND_READ_TO_RAM,
	NAND_READ_OOB
};

enum SDRAM_OPS_TYPE
{
	SDRAM_LOAD,
	DEVICE_RESET
};

enum DATA_STRUCTURE_OB
{
	DS_flash_info,
	DS_hand
};

enum SD_MMC_TYPE
{
	SD_CARD,
	MMC_CARD
};

enum USB_BOOT_ERROR_STATUS 
{
	USB_NO_ERR =0 ,
	GET_CPU_INFO_ERR,
	SET_DATA_ADDRESS_ERR,
	SET_DATA_LENGTH_ERR,
	FLUSH_CAHCES_ERR,
	PROGRAM_START1_ERR,
	PROGRAM_START2_ERR,
	NOR_OPS_ERR,
	NAND_OPS_ERR,
	NOR_FLASHTYPE_ERR,
	OPS_NOTSUPPORT_ERR
};

enum OPEN_CARD_ERROR_TYPE
{
	OPEN_CARD_ERROR = 1,
	OPEN_CARD_OCR_POWER_INIT_ERROR,
	OPEN_CARD_INIT_CHECK_STATUS_ERROR,
	OPEN_CARD_POWER_PIN_ERROR,
	OPEN_CARD_TYPE_ERROR,
	OPEN_CARD_READ_TIME_OUT,
	OPEN_CARD_MSC_STAT_TIME_OUT_READ,
	OPEN_CARD_MSC_STAT_CRC_READ_ERROR,
	OPEN_CARD_CHECK_CARD_STATUS_TIME_OUT,
	NOT_SUPPORT_CARD_BOOT,
	CANNOT_OPEN_CARD,
	OPEN_CARD_WARNING_BEGAIN = 100,
	ITE_HW_TYPE
};

enum OPTION
{
	OOB_ECC,
	OOB_NO_ECC,
	NO_OOB
};

/*-------------------------------------------------------------------------*/

/* int hand_init_def(struct hand *hand); */
/* int check_dump_cfg(struct hand *hand); */
/* int parse_configure(struct hand *hand, char * file_path); */

/*-------------------------------------------------------------------------*/

#endif	/*__STAGE2_CMD_TOOL_H__ */
