/*
 * Copyright (C) 2009 Ingenic Semiconductor Inc.
 * Author: lltang <lltang@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef NULL
#define NULL  (0)
#endif
#ifndef False
#define False 0
#define True !False
#endif

enum OPEN_CARD_TYPE
{
	ITE_HW=6,
	SK_6633SD = 10,
	SK_6633MMC,
	SK_6617SD,
	SK_6811MMC
};

typedef struct file_header_t
{
	char name[32];
	int offset;
	int size;
} file_header_t;

typedef struct card_prameter_t
{
	u8 flash_id[15];		//前3组 FLASH ID
	u8 card_cid[15];		//CID
	u8 hw_ver[2];           //HW_Version
	u8 fw_ver[2];           //FW_Version
	u8 card_type;
}card_prameter_t;

typedef struct file_desc_t
{
	unsigned int addr;
	char name[32];
} file_desc_t;

static file_desc_t erase_file_list[] =
{
	{0x80000001, "erase00"}, //Erase0.bin
	{0x81000001, "erase01"}, //Erase1.bin
	{0x82000001, "erase02"}, //Erase2.bin
	{0x83000001, "erase03"}, //Erase3.bin
	{0x84000001, "erase04"}, //Erase4.bin
	{0x85000001, "erase05"}, //Erase5.bin
	{0x86000001, "erase06"}, //Erase6.bin
	{0x87000001, "erase07"}, //Erase7.bin
	{0x88000001, "erase08"}, //Erase8.bin
	{0x89000001, "erase09"}, //Erase9.bin
	{0x8A000001, "erase10"}, //Erase10.bin
	{0x8B000001, "erase11"}, //Erase11.bin
	{0x8C000001, "erase12"}, //Erase12.bin
	{0x8D000001, "erase13"}, //Erase13.bin
	{0x8E000001, "erase14"}, //Erase14.bin
	{0x8F000001, "erase15"}, //Erase15.bin
};

static file_desc_t llf1_file_list[] =
{
	{0x80000001, "llf1_00"}, //LLF1_00.bin
	{0x81000001, "llf1_01"}, //LLF1_01.bin
	{0x82000001, "llf1_02"}, //LLF1_02.bin
	{0x83000001, "llf1_03"}, //LLF1_03.bin
	{0x84000001, "llf1_04"}, //LLF1_04.bin
	{0x85000001, "llf1_05"}, //LLF1_05.bin
	{0x86000001, "llf1_06"}, //LLF1_06.bin
	{0x87000001, "llf1_07"}, //LLF1_07.bin
	{0x88000001, "llf1_08"}, //LLF1_08.bin
	{0x89000001, "llf1_09"}, //LLF1_09.bin
	{0x8A000001, "llf1_10"}, //LLF1_10.bin
	{0x8B000001, "llf1_11"}, //LLF1_11.bin
	{0x8C000001, "llf1_12"}, //LLF1_12.bin
	{0x8D000001, "llf1_13"}, //LLF1_13.bin
	{0x8E000001, "llf1_14"}, //LLF1_14.bin
	{0x8F000001, "llf1_15"}, //LLF1_15.bin
};

static file_desc_t llf2_file_list[] =
{
	{0x80000001, "llf2_00"}, //LLF2_00.bin
	{0x81000001, "llf2_01"}, //LLF2_01.bin
	{0x82000001, "llf2_02"}, //LLF2_02.bin
	{0x83000001, "llf2_03"}, //LLF2_03.bin
	{0x84000001, "llf2_04"}, //LLF2_04.bin
	{0x85000001, "llf2_05"}, //LLF2_05.bin
	{0x86000001, "llf2_06"}, //LLF2_06.bin
	{0x87000001, "llf2_07"}, //LLF2_07.bin
	{0x88000001, "llf2_08"}, //LLF2_08.bin
	{0x89000001, "llf2_09"}, //LLF2_09.bin
	{0x8A000001, "llf2_10"}, //LLF2_10.bin
	{0x8B000001, "llf2_11"}, //LLF2_11.bin
	{0x8C000001, "llf2_12"}, //LLF2_12.bin
	{0x8D000001, "llf2_13"}, //LLF2_13.bin
	{0x8E000001, "llf2_14"}, //LLF2_14.bin
	{0x8F000001, "llf2_15"}, //LLF2_15.bin
};

#define HEADER_SECTION_SIZE    (1024*4)
extern u32 Bulk_out_buf[BULK_OUT_BUF_SIZE];
extern u32 Bulk_in_buf[BULK_OUT_BUF_SIZE];

static unsigned int sk_hw_style = 0;
static unsigned int fdm_size = 0;
static card_prameter_t card_prameter; 
static int bus_width = MSC_CMDAT_BUS_WIDTH_1BIT;
static int card_status = 0;
static int rca;
static int ocr = 0;

static void sd_skymedi_on_off_power(void);
static void sd_skymedi_off_power(void);
static  file_header_t * get_file_header(char* buffer, int buffer_size, char* name);
static void mmc_init_gpio(void);

static int strcmp(char *s1, char *s2)
{
  while (*s1 != '\0' && *s1 == *s2)
    {
      s1++;
      s2++;
    }

  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

static int sd_skymedi_set_func(unsigned int arg)
{
	u8 *resp;
	
	resp = mmc_cmd(60, arg, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	serial_puts("mmc_cmd 60\n");

	return 0;
}

static int sd_skymedi_open_card_init(void)
{
	serial_puts("sd_skymedi_open_card_init\n"); 
	int retries = 100, wait,i;
	u8 *resp;	
	unsigned int stat;
	unsigned int cardaddr;
	unsigned int card_status;

	__msc_reset();
	
	REG_MSC_IMASK = 0xffff;	
	REG_MSC_IREG = 0xffff;	
	REG_MSC_CLKRT = 6;    //200K
	bus_width = MSC_CMDAT_BUS_WIDTH_1BIT;

	serial_puts("mmc_cmd 0\n");
	sd_mdelay(2);
	resp = mmc_cmd(0, 0, MSC_CMDAT_INIT, 0);
	sd_mdelay(2);
	
	resp = mmc_cmd(8, 0x000001AA, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	
	resp = mmc_cmd(55, 0x00000000,  MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	serial_dump_data(resp, 15);
	
	if(55 == resp[5])
	{
		sd_mdelay(10);
		resp = mmc_cmd(41, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
		if(resp[5] == 0x3f)
		{
			serial_puts("sd init start");
			do {
				sd_mdelay(50);
				resp = mmc_cmd(55, 0x00000000,  MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
				sd_mdelay(10);
				resp = mmc_cmd(41, 0x40FF8000,  MSC_CMDAT_RESPONSE_R3 | MSC_CMDAT_BUSY, MSC_CMDAT_RESPONSE_R3);
				ocr = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
				serial_puts("  ocr = "); serial_put_hex(ocr);
				if(0 == ocr) 	return OPEN_CARD_OCR_POWER_INIT_ERROR;
			} 
			while(retries-- && resp && !(resp[4] & 0x80)); //

			if(retries <= 0)  return OPEN_CARD_OCR_POWER_INIT_ERROR;
				
			if (resp[4] & 0x80) //If bit-30 = 1, this is a MMC4.3 High
			{
				serial_puts("this is a SD2.0 High\n");
			} else {
				serial_puts("this is not a SD2.0 High\n");
			}
			
			card_prameter.card_type = SD_CARD;
			goto finish_power_init;
		}
	}

	//mmc power init
	serial_puts("mmc init start");
	do {
		resp = mmc_cmd(1, 0x40FF8000, 0x3, MSC_CMDAT_RESPONSE_R3);
		ocr = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
		serial_puts("  ocr = "); serial_put_hex(ocr);
		if(0 == ocr) 	return OPEN_CARD_OCR_POWER_INIT_ERROR;
		sd_mdelay(50);
	}
	while (retries-- && resp && !(resp[4] & 0x80));

	if(retries <= 0)  return OPEN_CARD_OCR_POWER_INIT_ERROR;
	
	if (resp[4] & 0x80) //If bit-30 = 1, this is a MMC4.3 High
	{
		serial_puts("this is a MMC High\n");
	} else {
		serial_puts("this is not a MMC High\n");
	}
	card_prameter.card_type = MMC_CARD;
	
finish_power_init:
	resp = mmc_cmd(2, 0x00000000, MSC_CMDAT_RESPONSE_R2, MSC_CMDAT_RESPONSE_R2);	
	serial_puts("  CID CSD = ");  serial_dump_data(resp, 15);
	//保存CID到结构用于检测是否支持在线开卡
	for(i=0; i<15; i++)
	{
		card_prameter.card_cid[i] = *resp++;
	}

	if(0x7F == card_prameter.card_cid[14]) return ITE_HW_TYPE;
	//else if(resp[14] == 0x55) ;//skymedi ID
	//else return CANNOT_OPEN_CARD;

	resp = mmc_cmd(3, 0x00000000, MSC_CMDAT_RESPONSE_R6, MSC_CMDAT_RESPONSE_R6);
	if(card_prameter.card_type == MMC_CARD)
	{
		rca = 0x10;
	}
	else
	{
		cardaddr = (resp[4] << 8) | resp[3]; 
		rca = cardaddr << 16;
	}
	serial_puts("mmc_cmd 3\n");
	serial_puts("  rca="); serial_put_hex(rca);

	resp = mmc_cmd(7, rca, MSC_CMDAT_RESPONSE_R1 | MSC_CMDAT_BUSY, MSC_CMDAT_RESPONSE_R1);	
	serial_puts("mmc_cmd 7\n");
	
	resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
	serial_puts("mmc_cmd 13\n");
	
	if((card_status & 0x900) != 0x900) //ready && tran
	{
		return OPEN_CARD_INIT_CHECK_STATUS_ERROR;
	}	
	return 0;
}

static int compare_chech_out_data(char* name)
{
	serial_puts("compare write data");  serial_puts(name); serial_puts("   -----\n");
	unsigned char * write_buffer = (unsigned char *)Bulk_out_buf;
	unsigned char * check_buffer = (unsigned char *)Bulk_in_buf;
	file_header_t* header = get_file_header(write_buffer, BULK_OUT_BUF_SIZE, name);
	if (header == NULL)
	{
		serial_puts("  can't find file ");  serial_puts(name); serial_puts("\n");
		return -1;
	}
	u32 data_positoin = 0;
	u32 num = header->size;
	u8 *dst = write_buffer + header->offset;
  while (data_positoin < num && *dst == *check_buffer)
  {
  	data_positoin ++;
    dst++;
    check_buffer++;
  }

  if(data_positoin == num)
  {
  	serial_puts(name);  serial_puts("compare_chech_out_data ----> OK\n");
  	return 0;
  }
  else
  {
	serial_puts("compare_chech_out_data ERROR at");  serial_put_hex(data_positoin); serial_puts("\n");
	//serial_dump_data((check_buffer-2), 16);
	//serial_dump_data((dst-2), 16);
	return -1;
  }
}

static int sd_skymedi_set_bus_width(u8 card_type)
{
	u8 *resp;
	u32 timeout;

	if(card_type == SD_CARD)
	{
		resp = mmc_cmd(55, 0x00020000,MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
		resp = mmc_cmd(6, 0x0000002, MSC_CMDAT_BUSY | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
		serial_puts("SD cmd 6\n");
	}
	else 
	{
		serial_puts("MMC cmd 6\n");
		resp = mmc_cmd(6, 0x03B70100, MSC_CMDAT_BUSY | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	}
	sd_mdelay(50);
	resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
	serial_puts("  card_status = "); serial_put_hex(card_status);
	
	if ((card_status & 0x900) != 0x900 ) //ready & trans status
	{
		return -1;
	}
	
	return 0;
}

static int sd_skymedi_perform_erase(u32 timeout)
{
	serial_puts("perform_erase\n");
	u8 *resp;
	
	resp = mmc_cmd(38, 0x00000002, MSC_CMDAT_BUSY | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	card_status = 0;
	
	while ((timeout>0) && ((card_status & 0x900) != 0x900 ))
	{
		sd_mdelay(200);
		timeout -= 200;
		resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
		card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
	}
	
	if (timeout <=0) //ready status
	{
		serial_puts("  TIMEOUT\n");
		return OPEN_CARD_CHECK_CARD_STATUS_TIME_OUT;
	}

	sd_skymedi_set_func(0x00000057);
	
	serial_puts("perform_erase end!!!\n");
	
	return 0;
}

static int sd_skymedi_read(u32 src, u32 num, u8 *dst, int timeout)
{
	serial_puts("sd_skymedi_read\n");

	u8 *resp;
	u32 stat, data, cnt, wait, nob;
	
	REG_MSC_BLKLEN = 512;
	REG_MSC_NOB = num / 512;
	
	resp = mmc_cmd(8, src, bus_width | MSC_CMDAT_DATA_EN | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);  

	nob  = num / 512;

	for (nob; nob >= 1; nob--) 
	{
		timeout = 0x7ffffff;
		while (timeout) 
		{
			timeout--;
			stat = REG_MSC_STAT;

			if (stat & MSC_STAT_TIME_OUT_READ) {
				serial_puts("\n MSC_STAT_TIME_OUT_READ\n\n");
				return OPEN_CARD_MSC_STAT_TIME_OUT_READ;
			}
			else if (stat & MSC_STAT_CRC_READ_ERROR) {
				serial_puts("\n MSC_STAT_CRC_READ_ERROR\n\n");
				return OPEN_CARD_MSC_STAT_CRC_READ_ERROR;
			}
			else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)) {
				/* Ready to read data */
				break;
			}
			wait = 336;
			while (wait--) ; // TODO ?aà??aê2?′òaμè°?
		}
		
		if (!timeout) {
			serial_puts("\n mmc/sd read timeout\n");
			return OPEN_CARD_READ_TIME_OUT;
		}

		/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
		cnt = 128;
		while (cnt) 
		{
			while (cnt && (REG_MSC_STAT & MSC_STAT_DATA_FIFO_EMPTY)) ;
			cnt --;
			data = REG_MSC_RXFIFO;
			{
				*dst++ = (u8)(data >> 0);
				*dst++ = (u8)(data >> 8);
				*dst++ = (u8)(data >> 16);
				*dst++ = (u8)(data >> 24);
			}
		}
	}

	while (!(REG_MSC_IREG & MSC_IREG_DATA_TRAN_DONE)) ;	
	REG_MSC_IREG = MSC_IREG_DATA_TRAN_DONE;	/* clear status */

	resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
	serial_puts("  card_status = "); serial_put_hex(card_status);

	while ((timeout>0) && ((card_status & 0x900) != 0x900 ))
	{
		unsigned int card_status1=0;
	
		sd_mdelay(10);
		timeout -= 10;
		resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
		card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
		
		if (card_status1 != card_status)
		{
			serial_puts("  card_status = "); serial_put_hex(card_status);
			card_status1 = card_status;
		}
	}
	
	if (timeout <=0)
	{
		serial_puts("  TIMEOUT\n");
		return OPEN_CARD_CHECK_CARD_STATUS_TIME_OUT;
	}
	
	jz_mmc_stop_clock();

	return 0;
}

static int sd_skymedi_read_fw_version(u32 src, u32 num, u8 *dst, int timeout)
{
	serial_puts("sd_skymedi_read\n");

	u8 *resp;
	u32 stat, data, cnt, wait, nob;

	REG_MSC_BLKLEN = 512;
	REG_MSC_NOB = num / 512;

	resp = mmc_cmd(17, src, bus_width | MSC_CMDAT_DATA_EN | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);  
	
	resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];

	serial_puts("  card_status = "); serial_put_hex(card_status);

	while ((timeout>0) && ((card_status & 0x900) != 0x900 ))
	{
		unsigned int card_status1=0;
	
		sd_mdelay(10);
		timeout -= 10;
		resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
		card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
		
		if (card_status1 != card_status)
		{
			serial_puts("  card_status = "); serial_put_hex(card_status);
			card_status1 = card_status;
		}
	}
	
	if (timeout <=0)
	{
		serial_puts("  TIMEOUT\n");
		return OPEN_CARD_CHECK_CARD_STATUS_TIME_OUT;
	}


	nob  = num / 512;

	for (nob; nob >= 1; nob--) 
	{
		timeout = 0x3ffffff;
		while (timeout) 
		{
			timeout--;
			stat = REG_MSC_STAT;

			if (stat & MSC_STAT_TIME_OUT_READ) {
				serial_puts("\n MSC_STAT_TIME_OUT_READ\n\n");
				return OPEN_CARD_MSC_STAT_TIME_OUT_READ;
			}
			else if (stat & MSC_STAT_CRC_READ_ERROR) {
				serial_puts("\n MSC_STAT_CRC_READ_ERROR\n\n");
				return OPEN_CARD_MSC_STAT_CRC_READ_ERROR;
			}
			else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)) {
				/* Ready to read data */
				break;
			}
			wait = 336;
			while (wait--) ; // TODO ?aà??aê2?′òaμè°?
		}
		
		if (!timeout) {
			serial_puts("\n mmc/sd read timeout\n");
			return -1;
		}

		/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
		cnt = 128;
		while (cnt) 
		{
			while (cnt && (REG_MSC_STAT & MSC_STAT_DATA_FIFO_EMPTY)) ;
			cnt --;
			data = REG_MSC_RXFIFO;
			{
				*dst++ = (u8)(data >> 0);
				*dst++ = (u8)(data >> 8);
				*dst++ = (u8)(data >> 16);
				*dst++ = (u8)(data >> 24);
			}
		}
	}

	sd_mdelay(30);	
	while (!(REG_MSC_IREG & MSC_STAT_DATA_TRAN_DONE)) ;
	REG_MSC_IREG = MSC_STAT_DATA_TRAN_DONE;	/* clear status */

	jz_mmc_stop_clock();

	return 0;
}
static int sd_skymedi_read_fmd(u32 src, u32 num, u8 *dst, int timeout)
{
	serial_puts("sd_skymedi_read\n");

	u8 *resp;
	u32 stat, data, cnt, wait, nob;

	REG_MSC_BLKLEN = 512;
	REG_MSC_NOB = num / 512;

	resp = mmc_cmd(18, src, bus_width | MSC_CMDAT_DATA_EN | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1); 

	nob  = num / 512;
	
	for (nob; nob >= 1; nob--) 
	{
		timeout = 0x3ffffff;
		while (timeout) 
		{
			timeout--;
			stat = REG_MSC_STAT;

			if (stat & MSC_STAT_TIME_OUT_READ) {
				serial_puts("\n MSC_STAT_TIME_OUT_READ\n\n");
				return OPEN_CARD_MSC_STAT_TIME_OUT_READ;
			}
			else if (stat & MSC_STAT_CRC_READ_ERROR) {
				serial_puts("\n MSC_STAT_CRC_READ_ERROR\n\n");
				return OPEN_CARD_MSC_STAT_CRC_READ_ERROR;
			}
			else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)) {
				/* Ready to read data */
				break;
			}
			wait = 336;
			while (wait--) ; // TODO ?aà??aê2?′òaμè°?
		}
		
		if (!timeout) {
			serial_puts("\n mmc/sd read timeout\n");
			return -1;
		}

		/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
		cnt = 128;
		while (cnt) 
		{
			while (cnt && (REG_MSC_STAT & MSC_STAT_DATA_FIFO_EMPTY)) ;
			cnt --;
			data = REG_MSC_RXFIFO;
			{
				*dst++ = (u8)(data >> 0);
				*dst++ = (u8)(data >> 8);
				*dst++ = (u8)(data >> 16);
				*dst++ = (u8)(data >> 24);
			}
		}
	}
	
	while (!(REG_MSC_IREG & MSC_IREG_DATA_TRAN_DONE)) ;
	REG_MSC_IREG = MSC_IREG_DATA_TRAN_DONE;	/* clear status */
	
	resp = mmc_cmd(12, 0x00000000,  MSC_CMDAT_BUSY | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
	
	resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);

	card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
	serial_puts("  card_status = "); serial_put_hex(card_status);
	while ((timeout>0) && ((card_status & 0x900) != 0x900 ))
	{	
		sd_mdelay(10);
		timeout -= 10;
		resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
		card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
	}
	
	if (timeout <=0)
	{
		serial_puts("  TIMEOUT\n");
		return OPEN_CARD_CHECK_CARD_STATUS_TIME_OUT;
	}
	
	jz_mmc_stop_clock();

	return 0;
}

static int sd_skymedi_write_file(char* name, u32 src, int timeout)
{
	serial_puts("write_file ");  serial_puts(name); serial_puts("\n");
	
	unsigned char * buffer = (unsigned char *)Bulk_out_buf;
	file_header_t* header = get_file_header(buffer, BULK_OUT_BUF_SIZE, name);
	if (header == NULL)
	{
		serial_puts("  can't find file ");  serial_puts(name); serial_puts("\n");
		return -1;
	}

	u32 msc_timeout;
	u32 num = header->size;
	u8 *dst = buffer + header->offset;

	serial_dump_data(dst, 16);
	
	u8 *resp;
	u32 stat, data, cnt, wait, nob, i, j;
	u32 *wbuf = (u32 *)dst;
	
	REG_MSC_BLKLEN = 512;
	REG_MSC_NOB = num / 512;

	resp = mmc_cmd(23, src, bus_width | MSC_CMDAT_WRITE | MSC_CMDAT_DATA_EN | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);  
	nob  = num / 512;
	
	for (i = 0; i < nob; i++) {
		msc_timeout = 0x3FFFFFF;
		while (msc_timeout) {
			
			msc_timeout--;
			stat = REG_MSC_STAT;

			if (stat & (MSC_STAT_CRC_WRITE_ERROR | MSC_STAT_CRC_WRITE_ERROR_NOSTS))
				return -1;
			else if (!(stat & MSC_STAT_DATA_FIFO_FULL)) {
				/* Ready to write data */
				break;
			}
//			udelay(1)
			wait = 336;
			while (wait--)
				;
		}
		
		if (!msc_timeout)
		{
			serial_puts("---timeout--- \n");
			return -1;
		}

		/* Write data to TXFIFO */
		cnt = 128;
		while (cnt) {
			while(!(REG_MSC_IREG & MSC_IREG_TXFIFO_WR_REQ));
			//while (REG_MSC_STAT & MSC_STAT_DATA_FIFO_FULL);

			for (j=0; j<8; j++)
			{
				REG_MSC_TXFIFO = *wbuf++;
				cnt--;
			}
		}
	}
	
	while (!(REG_MSC_IREG & MSC_IREG_DATA_TRAN_DONE));
	REG_MSC_IREG = MSC_IREG_DATA_TRAN_DONE;	/* clear status */
	/* waite _--_ */
	sd_mdelay(10);

	if(timeout == 0)  goto out_file_write;
	//waite for (ready)
	serial_puts("waite for (ready)");
	do{
		sd_mdelay(10);
		timeout -= 10;
		resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
		card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
		//serial_puts("  card_status this_time= "); serial_put_hex(card_status);
	}
	while ((timeout>0) && ((card_status & 0x900) != 0x900 ));
	
	if (timeout <=0)
	{
		serial_puts("  TIMEOUT\n");
		return OPEN_CARD_CHECK_CARD_STATUS_TIME_OUT;
	}
	
out_file_write:	
	jz_mmc_stop_clock();
	return 0;
}


static int sd_skymedi_write_file_fmd(char* name, u32 src, int timeout)
{
	serial_puts("write_file_fmd ");  serial_puts(name); serial_puts("\n");
	
	unsigned char* buffer = (unsigned char *)Bulk_out_buf;
	file_header_t* header = get_file_header(buffer, BULK_OUT_BUF_SIZE, name);
	if (header == NULL)
	{
		serial_puts("  can't find file ");  serial_puts(name); serial_puts("\n");
		return -1;
	}
	
	u32 num = header->size;
	u8 *dst = buffer + header->offset;

	fdm_size = num;
	serial_dump_data(dst, 16);
	
	u8 *resp;
	u32 stat, data, cnt, wait, nob, i, j;
	u32 *wbuf = (u32 *)dst;
	
	REG_MSC_BLKLEN = 512;
	REG_MSC_NOB = num / 512;

	resp = mmc_cmd(25, src, bus_width | MSC_CMDAT_WRITE | MSC_CMDAT_DATA_EN | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);  
	
	nob  = num / 512;
	timeout = 0x3ffffff;

	for (i = 0; i < nob; i++) {
		
		timeout = 0x3FFFFFF;
		while (timeout) {
			timeout--;
			stat = REG_MSC_STAT;

			if (stat & (MSC_STAT_CRC_WRITE_ERROR | MSC_STAT_CRC_WRITE_ERROR_NOSTS))
				return -1;
			else if (!(stat & MSC_STAT_DATA_FIFO_FULL)) {
				/* Ready to write data */
				break;
			}
			
			wait = 336;
			while (wait--) ;
		}
		if (!timeout)
			return -1;

		/* Write data to TXFIFO */
		cnt = 128;
		while (cnt) {
			while(!(REG_MSC_IREG & MSC_IREG_TXFIFO_WR_REQ));
			//while (REG_MSC_STAT & MSC_STAT_DATA_FIFO_FULL);
			for (j=0; j<8; j++)
			{	
				REG_MSC_TXFIFO = *wbuf++;
				cnt--;
			}
		}
	}
	
	while (!(REG_MSC_IREG & MSC_IREG_DATA_TRAN_DONE)) ;
	REG_MSC_IREG = MSC_IREG_DATA_TRAN_DONE;	/* clear status */
	sd_mdelay(10);
	resp = mmc_cmd(12, 0x00000000,  MSC_CMDAT_BUSY | MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);  
	do{
		sd_mdelay(10);
		timeout -= 10;
		resp = mmc_cmd(13, rca, MSC_CMDAT_RESPONSE_R1, MSC_CMDAT_RESPONSE_R1);
		card_status = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
		
	}
	while ((timeout>0) && ((card_status & 0x900) != 0x900 ));
	
	if (timeout <=0)
	{
		serial_puts("  TIMEOUT\n");
		return OPEN_CARD_CHECK_CARD_STATUS_TIME_OUT;
	}

	jz_mmc_stop_clock();
	return 0;
}

//write user CID infro
static int sd_skymedi_make_CID(void)
{
	serial_puts("skymedi_make_CID\n");
	
	unsigned char * buffer = (unsigned char *)Bulk_out_buf;
	file_header_t * header;
	int ret;
	int i;
    
	bus_width = MSC_CMDAT_BUS_WIDTH_1BIT;
			
	sd_skymedi_set_func(0x00000055);
	sd_skymedi_set_func(0x00000057);
	if((sk_hw_style == SK_6633SD) || (sk_hw_style == SK_6633MMC))
	{
		for(i=0; i<(sizeof(erase_file_list)/sizeof(erase_file_list[0])); i++)
		{
			ret = sd_skymedi_write_file(erase_file_list[i].name, erase_file_list[i].addr, 3*1000); 
			if (ret != 0) return ret;
		}  
	}
    else if(sk_hw_style == SK_6617SD)
    {
		ret = sd_skymedi_write_file("erase99", 0x80000014, 3*1000); 
		if (ret != 0) return ret;
    }
    else if(sk_hw_style == SK_6811MMC)
    {
		ret = sd_skymedi_write_file("erase99", 0x80000020, 3*1000); 
		if (ret != 0) return ret;
    }
	
	sd_skymedi_set_func(0x00000056);
	if((sk_hw_style == SK_6811MMC) || (sk_hw_style == SK_6633MMC))
	{
		mmc_cmd(35,0x00000000, 0, 0);   
	}
	else if((sk_hw_style == SK_6617SD)||(sk_hw_style == SK_6633SD))
	{
		mmc_cmd(32,0x00000001, 0, 0);   
	}
	
	//Write new CID.bin
	ret = sd_skymedi_write_file("CID", 0x80000001, 3*1000); 
	if (ret != 0) return ret;
		
	sd_skymedi_on_off_power();    	
	    
  return 0;
}

static int skymedi_open_card(void)
{
	serial_puts("=================\n");
	serial_puts("skymedi_open_card\n");
	
	unsigned char * buffer = (unsigned char *)Bulk_out_buf;
	unsigned char * sector_buff = (unsigned char *)Bulk_in_buf;
	unsigned int HWvesion;
	unsigned int FWvesion;
	
	file_header_t * header;
	int ret;
	int i;

	//2.1 Card Initial: 
	sd_skymedi_on_off_power();
	serial_puts("\n-------------\n");
	serial_puts("1. Card Initial\n");
	ret = sd_skymedi_open_card_init();
	if (ret != 0) return ret;

	//2.2 Validation Process
	serial_puts("\n-------------\n");
	serial_puts("2. Validation Process\n");
	sd_skymedi_set_func(0x00000055);


	ret = sd_skymedi_read(0x00000068, 512, sector_buff, 3*1000); //read HW_Version
	if (ret != 0) return ret;
	serial_puts("sector_buff[0]===");serial_put_hex(sector_buff[0]);serial_put_hex(sector_buff[1]);
	if((sector_buff[0] == 0x02)&&(sector_buff[1] == 0x08))
	{
		if(card_prameter.card_type == MMC_CARD) sk_hw_style = SK_6633MMC;
		else	sk_hw_style = SK_6633SD;
	}
	else if((sector_buff[0] == 0x00)&&(sector_buff[1] == 0x0D))
	{
		sk_hw_style = SK_6811MMC;
	}
	else if((sector_buff[0] == 0x01)&&(sector_buff[1] == 0x07))
	{
		sk_hw_style = SK_6617SD;
	}
	else
	{
		serial_puts("skymedi hw type error !\n");
	}

	serial_puts(" hw type===="); serial_put_hex(sk_hw_style);

	ret = sd_skymedi_read(0x00000067, 512, sector_buff, 3*1000); //read FW_Version
	if (ret != 0) return ret;
	
	ret = sd_skymedi_read(0x00000065, 512, sector_buff, 3*1000); //read Flash_ID  //TODO Dèíêé?
	if (ret != 0) return ret;
	serial_puts("  Flash_ID = "); serial_dump_data(sector_buff, 16);
#if 1
	sd_skymedi_set_func(0x000000AA);
	
	serial_puts("set bus width 4\n");
	bus_width = MSC_CMDAT_BUS_WIDTH_4BIT;
	ret = sd_skymedi_set_bus_width(card_prameter.card_type);
	if (ret != 0) return ret;

	sd_skymedi_set_func(0x00000055);
	ret = sd_skymedi_read(0x00000065, 512, sector_buff, 3*1000); //read Flash_ID
	if (ret != 0) return ret;
	serial_puts("  Flash_ID = "); serial_dump_data(sector_buff, 16);
#endif
	//2.3 Low Level Process
	serial_puts("\n-------------\n");
	serial_puts("3. Low Level Process\n");

	sd_skymedi_on_off_power();
	
	ret = sd_skymedi_open_card_init();
	if (ret != 0) return ret;
	
	bus_width = MSC_CMDAT_BUS_WIDTH_1BIT;
			
	sd_skymedi_set_func(0x00000055);
	sd_skymedi_set_func(0x00000057);
	sd_mdelay(2);
	
	if((sk_hw_style == SK_6633SD) || (sk_hw_style == SK_6633MMC))
	{
		for(i=0; i<(sizeof(erase_file_list)/sizeof(erase_file_list[0])); i++)
		{
			ret = sd_skymedi_write_file(erase_file_list[i].name, erase_file_list[i].addr, 3*1000); 
			if (ret != 0) return ret;
		}
	}
	else if(sk_hw_style == SK_6617SD)
	{
		ret = sd_skymedi_write_file("erase99", 0x80000014, 3*1000); 
		if (ret != 0) return ret;
	}
	else if(sk_hw_style == SK_6811MMC)
	{
		ret = sd_skymedi_write_file("erase99", 0x80000020, 3*1000); 
		if (ret != 0) return ret;
	}
	
	sd_skymedi_set_func(0x00000056);

	sd_skymedi_write_file("llf_parameter", 0xE0000001, 3*1000); //LLF_Parameter.bin

	sd_skymedi_perform_erase(100*1000);

	serial_puts("3. Low Level Process  On Off  power\n");
	
	sd_skymedi_on_off_power();
	ret = sd_skymedi_open_card_init();
	if (ret != 0) return ret;
	
	sd_skymedi_set_func(0x00000055);
	sd_skymedi_set_func(0x00000057);
	sd_mdelay(2);
	if((sk_hw_style == SK_6633SD) || (sk_hw_style == SK_6633MMC))
	{
		for(i=0; i<(sizeof(llf1_file_list)/sizeof(llf1_file_list[0])); i++)
		{
			ret = sd_skymedi_write_file(llf1_file_list[i].name, llf1_file_list[i].addr, 3*1000); 
			if (ret != 0) return ret;		
		}
	}
    else if(sk_hw_style == SK_6617SD)
    {
		ret = sd_skymedi_write_file("llf1_99", 0x80000014, 3*1000); 
		if (ret != 0) return ret;	
    }
	else if(sk_hw_style == SK_6811MMC)
    {
		ret = sd_skymedi_write_file("llf1_99", 0x80000020, 3*1000); 
		if (ret != 0) return ret;	
    }
	
	sd_skymedi_set_func(0x00000056);
	sd_mdelay(2);
	if(sk_hw_style == SK_6811MMC)
	{
		ret = sd_skymedi_write_file("vt_table_99", 0x90000004, 3*1000); //VT_TABLE.BIN
		if (ret != 0) return ret;
	}
	ret = sd_skymedi_write_file("llf_parameter", 0xE0000001, 300*1000); //LLF_Parameter.bin
	if (ret != 0) return ret;

	ret = sd_skymedi_read(0x00000072, 512, sector_buff, 3*1000); //Temp.bin = LLF_Status.bin
	if (ret != 0) return ret;

	if (sector_buff[0] != 0x65 || sector_buff[1] != 0x67)
	{
		serial_puts("LLF STATUS ERROR= "); serial_put_hex(sector_buff[0]);serial_put_hex(sector_buff[1]);
		return ret;
	}
	serial_puts("LLF STATUS ==== "); serial_put_hex(sector_buff[0]);serial_put_hex(sector_buff[1]);
	if(sk_hw_style == SK_6811MMC)  goto write_fdm_dirct;
	
	sd_skymedi_set_func(0x00000057);
	if((sk_hw_style == SK_6633SD) || (sk_hw_style == SK_6633MMC))
	{
		for(i=0; i<(sizeof(llf2_file_list)/sizeof(llf2_file_list[0])); i++)
		{
			ret = sd_skymedi_write_file(llf2_file_list[i].name, llf2_file_list[i].addr, 3*1000);
			if (ret != 0) return ret;		
		}	
	}
	else if(sk_hw_style == SK_6617SD)
	{
		ret = sd_skymedi_write_file("llf2_99", 0x80000014, 3*1000);
		if (ret != 0) return ret;
	}
	sd_skymedi_set_func(0x00000056);
	
	sd_skymedi_write_file("llf_parameter", 0xE0000001, 300*1000); //LLF_Parameter.bin
	if (ret != 0) return ret;

	ret = sd_skymedi_read(0x00000072, 512, sector_buff, 3*1000); //Temp.bin = LLF_Status.bin
	if (ret != 0) return ret;

	if (sector_buff[0] != 0x65 || sector_buff[1] != 0x67)
	{
		serial_puts("LLF STATUS ERROR= "); serial_put_hex(sector_buff[0]);serial_put_hex(sector_buff[1]);
		return ret;
	}
	
	sd_skymedi_set_func(0x00000057);
	if(sk_hw_style == SK_6633SD)
	{
		for(i=0; i<(sizeof(erase_file_list)/sizeof(erase_file_list[0])); i++)
		{
			ret = sd_skymedi_write_file(erase_file_list[i].name, erase_file_list[i].addr, 3*1000);
			if (ret != 0) return ret;
		}
	}
	else if(sk_hw_style == SK_6617SD)
	{
		ret = sd_skymedi_write_file("erase99", 0x80000014, 3*1000);
		if (ret != 0) return ret;
	}
	sd_skymedi_set_func(0x00000056);
	 
	if(sk_hw_style == SK_6633SD) mmc_cmd(61, 0x600E4224, 0, 0);  

write_fdm_dirct:
	ret = sd_skymedi_write_file_fmd("fdm", 0xFFFFFF10, 3*1000); //FMD.bin 64K 80K
	if (ret != 0) return ret;

	ret = sd_skymedi_read_fmd(0xFFFFFF10 , fdm_size, sector_buff, 3*1000); //Temp1.bin = FDM.bin
	if (ret != 0) return ret;
	
	ret = compare_chech_out_data("fdm");	
	if (ret != 0) return ret;

	ret = sd_skymedi_write_file_fmd("fdm", 0xFFFFFF20, 3*1000); //FMD.bin 64K 80K
	if (ret != 0) return ret;

	ret = sd_skymedi_read_fmd(0xFFFFFF20 , fdm_size, sector_buff, 3*1000); //Temp2.bin = FDM.bin
	if (ret != 0) return ret;	

	ret = compare_chech_out_data("fdm");	
	if (ret != 0) return ret;
	
#if 0
	//2.4 Verify LLF result & FW Patch Version
	serial_puts("\n-------------\n");
	serial_puts("4. Verify LLF result & FW Patch Version\n");
	
	sd_skymedi_on_off_power();
	ret = sd_skymedi_open_card_init();
	if (ret != 0) return ret;	
	
	sd_skymedi_set_func(0x00000055);
	sd_skymedi_set_func(0x00000057);

	for(i=0; i<(sizeof(erase_file_list)/sizeof(erase_file_list[0])); i++)
	{
		ret = sd_skymedi_write_file(erase_file_list[i].name, erase_file_list[i].addr, 3*1000);
		if (ret != 0) return ret;
	}
	
	sd_skymedi_set_func(0x00000056);
	
	//check version
	ret = sd_skymedi_read_fw_version(0xFFFFFF47, 512, sector_buff, 3*1000); //FW Patch Version.bin
	if (ret != 0) return ret;

	//check sectors data from address 0 at least 10
	sd_skymedi_on_off_power();

	ret = sd_skymedi_open_card_init();
	if (ret != 0) return ret;

	serial_puts("\n-------------\n");
	serial_puts("5. Program CID\n");
#endif
	sd_skymedi_on_off_power();
	
	ret = sd_skymedi_open_card_init();
	if (ret != 0) return ret;

	ret = sd_skymedi_make_CID();
	if (ret != 0) return ret;

	return 0;
}

static u32 card_query_flash_id(u8 *bulk_in_buf)
{
	int ret;
	u8 *resp;

	mmc_init_gpio();
	ret = sd_skymedi_open_card_init();
	if(ret > 0){
		if(ret == ITE_HW_TYPE){
			memcpy(bulk_in_buf, ((u8 *)(&card_prameter)), sizeof(card_prameter));
			return 0;
		}
		else  return ret; 
	}
		
	sd_skymedi_set_func(0x00000055);
	
	ret = sd_skymedi_read(0x00000068, 512, bulk_in_buf, 3*1000); //read HW_Version
	if (ret != 0) return ret;

	memcpy(card_prameter.hw_ver, bulk_in_buf, sizeof(card_prameter.hw_ver));
	serial_puts("  hw_ver = "); serial_dump_data(bulk_in_buf, 4);	
	ret = sd_skymedi_read(0x00000067, 512, bulk_in_buf, 3*1000); //read FW_Version
	if (ret != 0) return ret;

	memcpy(card_prameter.fw_ver, bulk_in_buf, sizeof(card_prameter.fw_ver));
	serial_puts("  fw_ver = "); serial_dump_data(bulk_in_buf, 4);						
	ret = sd_skymedi_read(0x00000065, 512, bulk_in_buf, 3*1000); //read Flash_ID  
	if (ret != 0) return ret;
	serial_puts("  Flash_ID = "); serial_dump_data(bulk_in_buf, 16);	

	memcpy(card_prameter.flash_id, bulk_in_buf, sizeof(card_prameter.flash_id));

	memcpy(bulk_in_buf, ((u8 *)(&card_prameter)), sizeof(card_prameter));

	/*power pin setting error may cause open card fail !!*/
	sd_skymedi_off_power();
	serial_puts(" sky power pin test start !!");
	ret = sd_skymedi_open_card_init();
	if(ret != OPEN_CARD_OCR_POWER_INIT_ERROR){
		serial_puts("  power pin setting error !!");
		return OPEN_CARD_POWER_PIN_ERROR;
	}

	sd_skymedi_on_off_power();
	return 0;
}

