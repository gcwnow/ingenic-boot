/*
 * Copyright (C) 2009 Ingenic Semiconductor Inc.
 * Author: Taylor <cwjia@ingenic.cn>
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

/*
  BUS_WIDTH 0  --> 1 BIT 
  BUS_WIDTH 2  --> 4 BIT
*/

#define BUS_WIDTH          2
#define PROID_4750       0x1ed0024f
#define PROID_4755	     0x2ed0024f
/*
 * External routines
 */
extern void flush_cache_all(void);
extern int serial_init(void);
extern void serial_puts(const char *s);
extern void sdram_init(void);
static int highcap = 0;
static void mmc_init_gpio(void);
static int rca;

#define read_32bit_cp0_processorid()                            \
({ int __res;                                                   \
        __asm__ __volatile__(                                   \
        "mfc0\t%0,$15\n\t"                                      \
        : "=r" (__res));                                        \
        __res;})
	
/* Stop the MMC clock and wait while it happens */
static  int jz_mmc_stop_clock(void)
{
	int timeout = 1000;
	int wait = 336; /* 1 us */ 

	REG_MSC_STRPCL = MSC_STRPCL_CLOCK_CONTROL_STOP;
	while (timeout && (REG_MSC_STAT & MSC_STAT_CLK_EN)) {
		timeout--;
		if (timeout == 0) {
			return -1;
		}
		wait = 336;
		while (wait--)
			;
	}
	return 0;
}

/* Start the MMC clock and operation */
static int jz_mmc_start_clock(void)
{
	REG_MSC_STRPCL = MSC_STRPCL_CLOCK_CONTROL_START | MSC_STRPCL_START_OP;
	return 0;
}

static u8 resp[20];
static void clear_resp()
{
	memset(resp, 0, sizeof(resp));
}

static u8 * mmc_cmd(u16 cmd, unsigned int arg, unsigned int cmdat, u16 rtype)
{
	u32 timeout = 0x7fffff;
	int words, i;

	clear_resp(); 
	jz_mmc_stop_clock();
	
	REG_MSC_CMD   = cmd;
	REG_MSC_ARG   = arg;
	REG_MSC_CMDAT = cmdat;

	REG_MSC_IMASK = ~MSC_IMASK_END_CMD_RES;
	jz_mmc_start_clock();

	while (timeout-- && !(REG_MSC_STAT & MSC_STAT_END_CMD_RES))
		;
	
	REG_MSC_IREG = MSC_IREG_END_CMD_RES;

	switch (rtype) {
         	case MSC_CMDAT_RESPONSE_R1:
		case MSC_CMDAT_RESPONSE_R3:
	        case MSC_CMDAT_RESPONSE_R6:		
			words = 3;
			break;
		case MSC_CMDAT_RESPONSE_R2:
			words = 8;
			break;
		default:
			return 0;
	}
	for (i = words-1; i >= 0; i--) {
		u16 res_fifo = REG_MSC_RES;
		int offset = i << 1;

		resp[offset] = ((u8 *)&res_fifo)[0];
		resp[offset+1] = ((u8 *)&res_fifo)[1];

	}
	return resp;
}

static int mmc_erase_sd(void)
{
		
}

static int mmc_erase_mmc(void)
{

}

static int mmc_block_readm(u32 src, u32 num, u8 *dst)
{
	u8 *resp;
	u32 stat, timeout, data, cnt, wait, nob;

	resp = mmc_cmd(16, 0x200, 0x401, MSC_CMDAT_RESPONSE_R1);
	REG_MSC_BLKLEN = 0x200;
	REG_MSC_NOB = num / 512;

	if (highcap) 
		resp = mmc_cmd(18, src,  0x10409, MSC_CMDAT_RESPONSE_R1); // for sdhc card
	else
		resp = mmc_cmd(18, src * 512, 0x10409, MSC_CMDAT_RESPONSE_R1);
	nob  = num / 512;
	//serial_puts("nob ==r===");serial_put_hex(nob);
	//serial_puts("src ==r===");serial_put_hex(src);
	for (nob; nob >= 1; nob--) {
		timeout = 0x7ffffff;
		while (timeout) {
			timeout--;
			stat = REG_MSC_STAT;

			if (stat & MSC_STAT_TIME_OUT_READ) {
				serial_puts("\n MSC_STAT_TIME_OUT_READ\n\n");
				return -1;
			}
			else if (stat & MSC_STAT_CRC_READ_ERROR) {
				serial_puts("\n MSC_STAT_CRC_READ_ERROR\n\n");
				return -1;
			}
			else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)) {
				/* Ready to read data */
				break;
			}
			wait = 336;
			while (wait--)
				;
		}
		if (!timeout) {
			serial_puts("\n mmc/sd read timeout\n");
			return -1;
		}

		/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
		cnt = 128;
		while (cnt) {
			while (cnt && (REG_MSC_STAT & MSC_STAT_DATA_FIFO_EMPTY))
				;
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
#if defined(MSC_STAT_AUTO_CMD_DONE)
	while(!(REG_MSC_STAT & MSC_STAT_AUTO_CMD_DONE));
#else
	resp = mmc_cmd(12, 0, 0x41, MSC_CMDAT_RESPONSE_R1);
	while (!(REG_MSC_STAT & MSC_STAT_DATA_TRAN_DONE));
#endif	
	jz_mmc_stop_clock();
	return 0;
}

static int mmc_block_writem(u32 src, u32 num, u8 *dst)
{
	u8 *resp;
	u32 stat, timeout, data, cnt, wait, nob, i, j;
	u32 *wbuf = (u32 *)dst;

	resp = mmc_cmd(16, 0x200, 0x1, MSC_CMDAT_RESPONSE_R1);
	REG_MSC_BLKLEN = 0x200;
	REG_MSC_NOB = num / 512;

	if (highcap)
		resp = mmc_cmd(25, src, 0x19 | (BUS_WIDTH << 9), MSC_CMDAT_RESPONSE_R1); // for sdhc card
	else
		resp = mmc_cmd(25, src * 512, 0x19 | (BUS_WIDTH << 9), MSC_CMDAT_RESPONSE_R1);
	nob  = num / 512;
	timeout = 0x3ffffff;
	//serial_puts("nob ==w===");serial_put_hex(nob);
	//serial_dump_data(dst, 16);
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
			while (wait--)
				;
		}
		if (!timeout)
			return -1;

		/* Write data to TXFIFO */
		cnt = 128;
		while (cnt) {
			//stat = REG_MSC_STAT;
			//serial_puts("stat ==write===");serial_put_hex(stat);
			while(!(REG_MSC_IREG & MSC_IREG_TXFIFO_WR_REQ))
				;
			for (j=0; j<16; j++)
			{
				//serial_put_hex(*wbuf);
				REG_MSC_TXFIFO = *wbuf++;
				cnt--;
			}
		}
	}
#if 0	
	while (!(REG_MSC_IREG & MSC_IREG_DATA_TRAN_DONE)) ;
	REG_MSC_IREG = MSC_IREG_DATA_TRAN_DONE;	/* clear status */
	
	resp = mmc_cmd(12, 0, 0x441, MSC_CMDAT_RESPONSE_R1);
	while (!(REG_MSC_IREG & MSC_IREG_PRG_DONE)) ;
	REG_MSC_IREG = MSC_IREG_PRG_DONE;	/* clear status */

#else
	while (!(REG_MSC_IREG & MSC_IREG_DATA_TRAN_DONE)) ;
	REG_MSC_IREG = MSC_IREG_DATA_TRAN_DONE;	/* clear status */

	while (!(REG_MSC_STAT & MSC_STAT_PRG_DONE)) ;
	REG_MSC_IREG = MSC_IREG_PRG_DONE;	/* clear status */

	resp = mmc_cmd(12, 0, 0x441, MSC_CMDAT_RESPONSE_R1);
	do{
		resp = mmc_cmd(13, rca, 0x1, MSC_CMDAT_RESPONSE_R1); // for sdhc card
	}while(!(resp[2] & 0x1));   //wait the card is ready for data

#endif	
	jz_mmc_stop_clock();
	//return src+nob;
	return 0;
}

static int mmc_found(unsigned int msc_clkrt_val)
{
	int retries;
	u8 *resp;
	int ocr;
	
	serial_puts("MMC card found!\n");
	resp = mmc_cmd(1, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
	retries = 1000;
	while (retries-- && resp && !(resp[4] & 0x80)) {
		resp = mmc_cmd(1, 0x40300000, 0x3, MSC_CMDAT_RESPONSE_R3);
		ocr = (resp[4] << 24) | (resp[3] << 16) | (resp[2] << 8) | resp[1];
		serial_puts("  ocr = "); serial_put_hex(ocr);
		sd_mdelay(10);
	}
		
	sd_mdelay(10);

	if ((resp[4] & 0x80 )== 0x80) 
		serial_puts("MMC init ok\n");
	else{ 
		serial_puts("MMC init fail\n");
		return -1;
	}
	if((resp[4] & 0x60 ) == 0x40)
		highcap = 1;
	else
		highcap =0;
	/* try to get card id */
	resp = mmc_cmd(2, 0, 0x2, MSC_CMDAT_RESPONSE_R2);
	serial_puts("CID="); serial_dump_data(resp, 15);
	
	resp = mmc_cmd(3, 0x10, 0x1, MSC_CMDAT_RESPONSE_R1);

	REG_MSC_CLKRT = msc_clkrt_val;	/* 16/1 MHz */
	resp = mmc_cmd(7, 0x10, 0x1, MSC_CMDAT_RESPONSE_R1);

	if(BUS_WIDTH == 2){
		resp = mmc_cmd(6, 0x3b70101, 0x441, MSC_CMDAT_RESPONSE_R1);
	}
	else if(BUS_WIDTH == 0){
		resp = mmc_cmd(6, 0x3b30001, 0x41, MSC_CMDAT_RESPONSE_R1); 
	}
	return 0;
}
static int sd_init(unsigned int msc_clkrt_val)
{
	int retries, wait;
//	int rca;
	u8 *resp;
	unsigned int cardaddr;
	serial_puts("SD card found!\n");
	resp = mmc_cmd(55, 0, 0x1, MSC_CMDAT_RESPONSE_R1);
	resp = mmc_cmd(41, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
	retries = 500;
	while (retries-- && resp && !(resp[4] & 0x80)) {
		resp = mmc_cmd(55, 0, 0x1, MSC_CMDAT_RESPONSE_R1);
		resp = mmc_cmd(41, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
		
		sd_mdelay(10);
	}

	if ((resp[4] & 0x80) == 0x80) 
		serial_puts("SD init ok\n");
	else{ 
		serial_puts("SD init fail\n");
		return -1;
	}
	/* try to get card id */
	resp = mmc_cmd(2, 0, 0x2, MSC_CMDAT_RESPONSE_R2);
	serial_puts("CID="); serial_dump_data(resp, 15);
	resp = mmc_cmd(3, 0, 0x6, MSC_CMDAT_RESPONSE_R1);
	cardaddr = (resp[4] << 8) | resp[3]; 
	rca = cardaddr << 16;

	resp = mmc_cmd(9, rca, 0x2, MSC_CMDAT_RESPONSE_R2);
	highcap = (resp[14] & 0xc0) >> 6;
	REG_MSC_CLKRT = msc_clkrt_val;
	resp = mmc_cmd(7, rca, 0x41, MSC_CMDAT_RESPONSE_R1);
	resp = mmc_cmd(55, rca, 0x1, MSC_CMDAT_RESPONSE_R1);
	resp = mmc_cmd(6, BUS_WIDTH, 0x1 | (BUS_WIDTH << 9), MSC_CMDAT_RESPONSE_R1);
	return 0;
}

/* init mmc/sd card we assume that the card is in the slot */
static int  mmc_init(unsigned int msc_clkrt_val)
{
	int retries, ret;
	u8 *resp;
	//unsigned int msc_clk_div = (__cpm_get_pllout())/24000000;
	__msc_reset();
	mmc_init_gpio();
	REG_MSC_IMASK = 0xffff;	
	REG_MSC_IREG = 0xffff;			
	REG_MSC_CLKRT = 6;    //200k
	
	/* just for reading and writing, suddenly it was reset, and the power of sd card was not broken off */
	resp = mmc_cmd(12, 0, 0x41, MSC_CMDAT_RESPONSE_R1);

	/* reset */
	resp = mmc_cmd(0, 0, 0x80, 0);
	resp = mmc_cmd(8, 0x1aa, 0x1, MSC_CMDAT_RESPONSE_R1);
	resp = mmc_cmd(55, 0, 0x1, MSC_CMDAT_RESPONSE_R1);
	if(resp[5] == 0x37){
		resp = mmc_cmd(41, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
		if(resp[5] == 0x3f)
		ret = sd_init(msc_clkrt_val);
		return ret;
	}
	ret = mmc_found(msc_clkrt_val);
	return ret;
}

