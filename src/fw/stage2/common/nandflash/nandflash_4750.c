#include "jz4750.h"
#include "nandflash.h"
#include "usb_boot.h"
#include "hand.h"

#define dprintf(n...) serial_puts(n)
#if 1
void gpio_as_nand_8bit(int n)
{		              	
	/* 32/16-bit data bus */
	REG_GPIO_PXFUNS(0) = 0x000000ff; /* D0~D7 */			
	REG_GPIO_PXSELC(0) = 0x000000ff;				
	REG_GPIO_PXPES(0) = 0x000000ff;					
	REG_GPIO_PXFUNS(1) = 0x00008000; /* CLE(A15) */			
	REG_GPIO_PXSELC(1) = 0x00008000;				
	REG_GPIO_PXPES(1) = 0x00008000;					
	REG_GPIO_PXFUNS(2) = 0x00010000; /* ALE(A16) */			
	REG_GPIO_PXSELC(2) = 0x00010000;				
	REG_GPIO_PXPES(2) = 0x00010000;					
									
	REG_GPIO_PXFUNS(2) = 0x00200000 << ((n)-1); /* CSn */		
	REG_GPIO_PXSELC(2) = 0x00200000 << ((n)-1);			
	REG_GPIO_PXPES(2) = 0x00200000 << ((n)-1);			
									
        REG_GPIO_PXFUNS(1) = 0x00080000; /* RDWE#/BUFD# */		
        REG_GPIO_PXSELC(1) = 0x00080000;				
	REG_GPIO_PXPES(1) = 0x00080000;					
	REG_GPIO_PXFUNS(2) = 0x30000000; /* FRE#, FWE# */		
	REG_GPIO_PXSELC(2) = 0x30000000;				
	REG_GPIO_PXPES(2) = 0x30000000;					
	REG_GPIO_PXFUNC(2) = 0x08000000; /* FRB#(input) */		
	REG_GPIO_PXSELC(2) = 0x08000000;				
	REG_GPIO_PXDIRC(2) = 0x08000000;				
	REG_GPIO_PXPES(2) = 0x08000000;				
	REG_GPIO_PXTRGC(1) = 0xffffffff;	
	REG_GPIO_PXTRGC(2) = 0xffffffff;
}
#endif
#define PROID_4750 0x1ed0024f

#define read_32bit_cp0_processorid()                            \
({ int __res;                                                   \
        __asm__ __volatile__(                                   \
        "mfc0\t%0,$15\n\t"                                      \
        : "=r" (__res));                                        \
        __res;})

#define __nand_enable()		(REG_EMC_NFCSR |= EMC_NFCSR_NFE1 | EMC_NFCSR_NFCE1)
#define __nand_disable()	(REG_EMC_NFCSR &= ~(EMC_NFCSR_NFCE1))
#define __nand_ready()		((REG_GPIO_PXPIN(2) & 0x08000000) ? 1 : 0)
#define __nand_cmd(n)		(REG8(cmdport) = (n))
#define __nand_addr(n)		(REG8(addrport) = (n))
#define __nand_data8()		REG8(dataport)
#define __nand_data16()		REG16(dataport)

#define CMD_READA	0x00
#define CMD_READB	0x01
#define CMD_READC	0x50
#define CMD_ERASE_SETUP	0x60
#define CMD_ERASE	0xD0
#define CMD_READ_STATUS 0x70
#define CMD_CONFIRM	0x30
#define CMD_SEQIN	0x80
#define CMD_PGPROG	0x10
#define CMD_READID	0x90

#define ECC_BLOCK	512

static volatile unsigned char *gpio_base = (volatile unsigned char *)0xb0010000;
static volatile unsigned char *emc_base = (volatile unsigned char *)0xb3010000;
static volatile unsigned char *addrport = (volatile unsigned char *)0xb8010000;
static volatile unsigned char *dataport = (volatile unsigned char *)0xb8000000;
static volatile unsigned char *cmdport = (volatile unsigned char *)0xb8008000;

static int bus = 8, row = 2, pagesize = 512, oobsize = 16, ppb = 32;
static int eccpos = 3, bchbit = 8, par_size = 13, forceerase = 1, wp_pin;
static int eccpos_sav, bchbit_sav, par_size_sav;
static int oobfs = 0;            /* 1:store file system information in oob, 0:don't store */
static int oobecc = 0;           /* Whether the oob data of the binary contains ECC data? */
static int bad_block_page = 127; /* Specify the page number of badblock flag inside a block */
static u32 bad_block_pos = 0;    /* Specify the badblock flag offset inside the oob */
static u8 oob_buf[256] = {0};
extern hand_t Hand;
extern u16 handshake_PKT[4];

/*global variable for oobdata bch*/
static int oob_len = 16;	/*oob valid info data length, by byte*/
static int par_len = 7;		/*parity data length, by byte*/

static inline void __nand_sync(void)
{
	unsigned int timeout = 10000;
	while ((REG_GPIO_PXPIN(2) & 0x08000000) && timeout--);
	while (!(REG_GPIO_PXPIN(2) & 0x08000000));
}

static int read_oob(void *buf, u32 size, u32 pg);
static int nand_data_write8(char *buf, int count);
static int nand_data_write16(char *buf, int count);
static int nand_data_read8(char *buf, int count);
static int nand_data_read16(char *buf, int count);

static int (*write_proc)(char *, int) = NULL;
static int (*read_proc)(char *, int) = NULL;

inline void nand_enable_4750(unsigned int csn)
{
	unsigned int processor_id = read_32bit_cp0_processorid();
	//modify this fun to a specifical borad
	//this fun to enable the chip select pin csn
	//the choosn chip can work after this fun
	//dprintf("\n Enable chip select :%d",csn);
	__nand_enable();
	if (processor_id == PROID_4750) {
		if (bus == 8)
			__gpio_as_nand_8bit();
		else
			__gpio_as_nand_16bit();
	} else
		gpio_as_nand_8bit(1);
}

inline void nand_disable_4750(unsigned int csn)
{
	//modify this fun to a specifical borad
	//this fun to enable the chip select pin csn
	//the choosn chip can not work after this fun
	//dprintf("\n Disable chip select :%d",csn);
	__nand_disable();
}

unsigned int nand_query_4750(u8 *id)
{
	__nand_sync();
	__nand_cmd(CMD_READID);
	__nand_addr(0);

	id[0] = __nand_data8();      //VID
	id[1] = __nand_data8();      //PID
	id[2] = __nand_data8();      //CHIP ID
	id[3] = __nand_data8();      //PAGE ID
	id[4] = __nand_data8();      //PLANE ID

	return 0;
}

int nand_init_4750(int bus_width, int row_cycle, int page_size, int page_per_block,
		   int bch_bit, int ecc_pos, int bad_pos, int bad_page, int force)
{
	bus = bus_width;
	row = row_cycle;
	pagesize = page_size;
	oobsize = pagesize / 32;
	ppb = page_per_block;
	bchbit = bchbit_sav = bch_bit;
	forceerase = force;
	eccpos = eccpos_sav = ecc_pos;
	bad_block_pos = bad_pos;
	bad_block_page = bad_page;
	wp_pin = Hand.nand_wppin;

	if (bchbit == 8)
		par_size = par_size_sav = 13;
	else
		par_size = par_size_sav = 7;


#if 0
	gpio_base = (u8 *)gbase;
	emc_base = (u8 *)ebase;
	addrport = (u8 *)aport;
	dataport = (u8 *)dport;
	cmdport = (u8 *)cport;
#endif
	if(is_share_mode()) {
		serial_puts("nand share mode\n");
		addrport = (volatile unsigned char *)0xb8010000;
		cmdport = (volatile unsigned char *)0xb8008000;
	} else {
		serial_puts("nand unshare mode\n");
		addrport = (volatile unsigned char *)0xb8000010;
		cmdport = (volatile unsigned char *)0xb8000008;
	}

	/* Initialize NAND Flash Pins */
	if (bus == 8) {
		serial_puts("8bit bus width\n");
		//REG_EMC_SMCR1 = 0x0fff7700;      //slow speed
		REG_EMC_SMCR1 = 0x04444400;      //normal speed
		//REG_EMC_SMCR1 = 0x0d221200;      //fast speed

		__gpio_as_nand_8bit();
	} else {
		serial_puts("16bit bus width\n");
		REG_EMC_SMCR1 = 0x0fff7740;      //slow speed
		//REG_EMC_SMCR1 = 0x04444440;      //normal speed
		//REG_EMC_SMCR1 = 0x0d221240;      //fast speed

		__gpio_as_nand_16bit();
	}

	if (wp_pin)
	{
		__gpio_as_output(wp_pin);
		__gpio_disable_pull(wp_pin);
	}
	__nand_enable();

       /* If ECCPOS isn't configured in config file, the initial value is 0 */
 	if (eccpos == 0) {
		eccpos = 3;
	}

	if (bus == 8) {
		write_proc = nand_data_write8;
		read_proc = nand_data_read8;
	} else {
		write_proc = nand_data_write16;
		read_proc = nand_data_read16;
	}

	serial_puts("Send Micron NAND Reset.\n"); 

	__nand_cmd(0xFF);
	__nand_sync();

	return 0;
}

int nand_fini_4750(void)
{
	__nand_disable();
	return 0;
}

/*
 * Read oob <pagenum> pages from <startpage> page.
 * Don't skip bad block.
 * Do use HW ECC.
 */
u32 nand_read_oob_4750(void *buf, u32 startpage, u32 pagenum)
{
	u32 cnt, cur_page;
	u8 *tmpbuf;
	
	tmpbuf = (u8 *)buf;

	cur_page = startpage;
	cnt = 0;

	while (cnt < pagenum) {
		read_oob((void *)tmpbuf, oobsize, cur_page);
		tmpbuf += oobsize;
		cur_page++;
		cnt++;
	}

	return cur_page;
}

static int nand_check_block(u32 block)
{
	u32 pg,i;
	
	if ( bad_block_page >= ppb )    //do absolute bad block detect!
	{
		pg = block * ppb + 0;
		read_oob(oob_buf, oobsize, pg);
		if ( oob_buf[0] != 0xff || oob_buf[1] != 0xff )
		{
			serial_puts("Absolute skip a bad block\n");
			serial_put_hex(block);
			return 1;
		}

		pg = block * ppb + 1;
		read_oob(oob_buf, oobsize, pg);
		if ( oob_buf[0] != 0xff || oob_buf[1] != 0xff )
		{
			serial_puts("Absolute skip a bad block\n");
			serial_put_hex(block);
			return 1;
		}

		pg = block * ppb + ppb - 2 ;
		read_oob(oob_buf, oobsize, pg);
		if ( oob_buf[0] != 0xff || oob_buf[1] != 0xff )
		{
			serial_puts("Absolute skip a bad block\n");
			serial_put_hex(block);
			return 1;
		}

		pg = block * ppb + ppb - 1 ;
		read_oob(oob_buf, oobsize, pg);
		if ( oob_buf[0] != 0xff || oob_buf[1] != 0xff )
		{
			serial_puts("Absolute skip a bad block\n");
			serial_put_hex(block);
			return 1;
		}

	}
	else
	{
        if ((pagesize == 512) && (block == 0))
            return 0;
        
		pg = block * ppb + bad_block_page;
		read_oob(oob_buf, oobsize, pg);
		if (oob_buf[bad_block_pos] != 0xff)
		{
			serial_puts("Skip a bad block at");
			serial_put_hex(block);
			return 1;
		}

	}
	return 0;
}


/*
 * Read data <pagenum> pages from <startpage> page.
 * Don't skip bad block.
 * Don't use HW ECC.
 */
u32 nand_read_raw_4750(void *buf, u32 startpage, u32 pagenum, int option)
{
	u32 cnt, j;
	u32 cur_page, rowaddr;
	u8 *tmpbuf;
	
	tmpbuf = (u8 *)buf;

	cur_page = startpage;
	cnt = 0;
	while (cnt < pagenum) {
		if ((cur_page % ppb) == 0) {
			if (nand_check_block(cur_page / ppb)) {
				cur_page += ppb;   // Bad block, set to next block 
				continue;
			}
		}

		__nand_cmd(CMD_READA);
		__nand_addr(0);
		if (pagesize != 512)
			__nand_addr(0);

		rowaddr = cur_page;
		for (j = 0; j < row; j++) {
			__nand_addr(rowaddr & 0xff);
			rowaddr >>= 8;
		}

		if (pagesize != 512)
			__nand_cmd(CMD_CONFIRM);

		__nand_sync();
		read_proc(tmpbuf, pagesize);

		tmpbuf += pagesize;
		if (option != NO_OOB)
		{
			read_oob(tmpbuf, oobsize, cur_page);
			tmpbuf += oobsize;
		}

		cur_page++;
		cnt++;
	}

	return cur_page;
}


u32 nand_erase_4750(int blk_num, int sblk, int force)
{
	int i, j;
	u32 cur, rowaddr;

	if (wp_pin)
		__gpio_set_pin(wp_pin);
	cur = sblk * ppb;
	for (i = 0; i < blk_num; i++) {
		rowaddr = cur;

		if (!force)	/* if set, erase anything */
			if (nand_check_block(cur/ppb))
			{
				cur += ppb;
				blk_num += Hand.nand_plane;
				continue;
			}
		
		__nand_cmd(CMD_ERASE_SETUP);

		for (j = 0; j < row; j++) {
			__nand_addr(rowaddr & 0xff);
			rowaddr >>= 8;
		}
		__nand_cmd(CMD_ERASE);
		__nand_sync();
		__nand_cmd(CMD_READ_STATUS);

		if (__nand_data8() & 0x01) 
		{
			serial_puts("Erase fail at ");
			serial_put_hex(cur / ppb);
			nand_mark_bad_4750(cur / ppb);
			cur += ppb;
			blk_num += Hand.nand_plane;
			continue;
		}
		cur += ppb;
	}

	if (wp_pin)
		__gpio_clear_pin(wp_pin);
	return cur;
}

static void bch_correct(unsigned char *dat, int idx)
{
	int i, bit;  // the 'bit' of i byte is error 
	i = (idx - 1) >> 3;
	bit = (idx - 1) & 0x7;
	dat[i] ^= (1 << bit);
}

static int check_ff_4750(u8 *buf, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		if (buf[i] != 0xff)
		{
			//serial_puts("\n %s: Not 0xFF! %d=0x%x \n", __FUNCTION__, i, buf[i]);
			return -1;
		}
	}
	//serial_puts("\n %s: All 0xFF! \n", __FUNCTION__);
	return 0;
}

int hw_reset_4750(void)
{
	serial_puts("1,hw_reset.\n");
	__wdt_select_extalclk();
	__wdt_select_clk_div1024();
	__wdt_set_data(3);
	__wdt_set_count(0);
	__wdt_start();
	/*
	OUTREG16(REG_WDT_TCSR, WDT_TCSR_EXT_EN);
	SETREG16(REG_WDT_TCSR, WDT_TCSR_PRESCALE1024);
	OUTREG16(REG_WDT_TDR, 3);
	OUTREG16(REG_WDT_TCNT, 0);
	SETREG8(REG_WDT_TCER, WDT_TCER_TCEN);
	*/
	while (1) serial_puts("We should NOT come here, please check the rtc\n");
	return 0;
}

/*oob data 4-bit BCH decoding*/
u32 nand_oobdata_bch_dec_4750(void *buf)
{
	u8 *oobbuf = (u8 *)buf;
	u8 *parbuf = oobbuf + 16;
	int i, stat;
	
	if (!check_ff_4750(oobbuf, oob_len))
		return 0;		/*oobdata all 0xff, no ecc*/
	
	REG_BCH_INTS = 0xffffffff;
	__ecc_decoding_4bit();
	__ecc_cnt_dec(oob_len + par_len);
	
	for (i = 0; i < oob_len; i++) {
		REG_BCH_DR = oobbuf[i];
	}
	for (i = 0; i < par_len; i++) {
		REG_BCH_DR = parbuf[i];
	}
	
	__ecc_decode_sync();
	__ecc_disable();
	
	/* Check decoding */
	stat = REG_BCH_INTS;
	if (stat & BCH_INTS_ERR)
	{
		if (stat & BCH_INTS_UNCOR)
		{
			dprintf("Uncorrectable ECC error occurred\n");
			handshake_PKT[3] = 1;
                }
		else
		{
                        handshake_PKT[3] = 0;
                        unsigned int errcnt = (stat & BCH_INTS_ERRC_MASK) >> BCH_INTS_ERRC_BIT;
                        switch (errcnt)
                        {
			case 4:
                            dprintf("correct 4th error\n");
                            bch_correct(oobbuf, (REG_BCH_ERR1 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
                        case 3:
                            dprintf("correct 3th error\n");
                            bch_correct(oobbuf, (REG_BCH_ERR1 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
                        case 2:
                            dprintf("correct 2th error\n");
                            bch_correct(oobbuf, (REG_BCH_ERR0 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
                        case 1:
                            dprintf("correct 1th error\n");
                            bch_correct(oobbuf, (REG_BCH_ERR0 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
                            break;
                        default:
                            dprintf("no error\n");
                            break;
                        }
		}
	}
	return 0;
}

static int read_oob(void *buf, u32 size, u32 pg)
{
	u32 i, coladdr, rowaddr;
	
	if (pagesize == 512)
		coladdr = 0;
	else {
		if (bus == 8)
			coladdr = pagesize;
		else
			coladdr = pagesize / 2;
	}
	if (pagesize == 512)
		/* Send READOOB command */
		__nand_cmd(CMD_READC);
	else
		/* Send READ0 command */
		__nand_cmd(CMD_READA);

	/* Send column address */
	__nand_addr(coladdr & 0xff);
	if (pagesize != 512)
		__nand_addr(coladdr >> 8);

	/* Send page address */
	rowaddr = pg;
	for (i = 0; i < row; i++) {
		__nand_addr(rowaddr & 0xff);
		rowaddr >>= 8;
	}

	/* Send READSTART command for 2048 or 4096 ps NAND */
	if (pagesize != 512)
		__nand_cmd(CMD_CONFIRM);

	/* Wait for device ready */
	__nand_sync();

	/* Read oob data */
	read_proc(buf, size);

	if (pagesize == 512)
		__nand_sync();
	
	nand_oobdata_bch_dec_4750(buf);

	return 0;
}

 /*
 * Read data <pagenum> pages from <startpage> page.
 * Skip bad block if detected.
 * HW ECC is used.
 */
u32 nand_read_4750(void *buf, u32 startpage, u32 pagenum, int option)
{
	u32 j, k;
	u32 cur_page, cur_blk, cnt, rowaddr, ecccnt;
	u8 *tmpbuf, *p, flag = 0;
	u32 oob_per_eccsize;
	int spl_size = 16 * 1024 / pagesize;

	ecccnt = pagesize / ECC_BLOCK;
	oob_per_eccsize = eccpos / ecccnt;

	cur_page = startpage;
	cnt = 0;

	tmpbuf = buf;

	while (cnt < pagenum) {
		if (cur_page < spl_size) {
			bchbit = 8;
			eccpos = 3;
			par_size = 13;
		} else if (cur_page >= spl_size) {
			bchbit = bchbit_sav;
			eccpos = eccpos_sav;
			par_size = par_size_sav;
		}

		/* If this is the first page of the block, check for bad. */
		if ((cur_page % ppb) == 0) {
			cur_blk = cur_page / ppb;
			if (nand_check_block(cur_blk)) {
				cur_page += ppb;   // Bad block, set to next block 
				continue;
			}
		}
		__nand_cmd(CMD_READA);

		__nand_addr(0);
		if (pagesize != 512)
			__nand_addr(0);

		rowaddr = cur_page;
		for (j = 0; j < row; j++) {
			__nand_addr(rowaddr & 0xff);
			rowaddr >>= 8;
		}

		if (pagesize != 512)
			__nand_cmd(CMD_CONFIRM);

		__nand_sync();

		/* Read data */
		read_proc((char *)tmpbuf, pagesize);

		/* read oob first */
		read_proc((char *)oob_buf, oobsize);

        /*
         * Do ecc correction. Don't do it for OOB_ECC case,
         * since we don't know how to use its own ecc.
         */
        if (option != OOB_ECC) {
            for (j = 0; j < ecccnt; j++) {
                u32 stat;
                flag = 0;
                REG_BCH_INTS = 0xffffffff;

                if (cur_page >= 16384 / pagesize)
                {
                    if (bchbit == 8)
                    {
                        __ecc_decoding_8bit();
                        par_size = 13;
                    }
                    else
                    {
                        __ecc_decoding_4bit();
                        par_size = 7;
                    }
                }
                else
                {
                    __ecc_decoding_8bit();
                    par_size = 13;
                }

                if (option != NO_OOB) 
                    __ecc_cnt_dec(ECC_BLOCK + oob_per_eccsize + par_size);
                else
                    __ecc_cnt_dec(ECC_BLOCK + par_size);
                
                for (k = 0; k < ECC_BLOCK; k++) {
                    REG_BCH_DR = tmpbuf[k];
                }
                
                if (option != NO_OOB) {
                    for (k = 0; k < oob_per_eccsize; k++) {
                        REG_BCH_DR = oob_buf[oob_per_eccsize * j + k];
                    }
                }

                for (k = 0; k < par_size; k++) {
                    REG_BCH_DR = oob_buf[eccpos + j*par_size + k];
                    if (oob_buf[eccpos + j*par_size + k] != 0xff)
                        flag = 1;
                }

                /* Wait for completion */
                __ecc_decode_sync();
                __ecc_disable();
                /* Check decoding */
                stat = REG_BCH_INTS;
		
                if (stat & BCH_INTS_ERR) {
                    if (stat & BCH_INTS_UNCOR) {
                        if (flag)
                        {
                            dprintf("Uncorrectable ECC error occurred\n");
                            handshake_PKT[3] = 1;
                        }
                    }
                    else {
                        handshake_PKT[3] = 0;
                        unsigned int errcnt = (stat & BCH_INTS_ERRC_MASK) >> BCH_INTS_ERRC_BIT;
                        switch (errcnt) {
                        case 8:
                            dprintf("correct 8th error\n");
                            bch_correct(tmpbuf, (REG_BCH_ERR3 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
                        case 7:
                            dprintf("correct 7th error\n");
                            bch_correct(tmpbuf, (REG_BCH_ERR3 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
                        case 6:
                            dprintf("correct 6th error\n");
                            bch_correct(tmpbuf, (REG_BCH_ERR2 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
                        case 5:
                            dprintf("correct 5th error\n");
                            bch_correct(tmpbuf, (REG_BCH_ERR2 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
                        case 4:
                            dprintf("correct 4th error\n");
                            bch_correct(tmpbuf, (REG_BCH_ERR1 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
                        case 3:
                            dprintf("correct 3th error\n");
                            bch_correct(tmpbuf, (REG_BCH_ERR1 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
                        case 2:
                            dprintf("correct 2th error\n");
                            bch_correct(tmpbuf, (REG_BCH_ERR0 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
                        case 1:
                            dprintf("correct 1th error\n");
                            bch_correct(tmpbuf, (REG_BCH_ERR0 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
                            break;
                        default:
                            dprintf("no error\n");
                            break;
                        }
                    }
                }
                /* increment pointer */
                tmpbuf += ECC_BLOCK;
            }
	} else
            tmpbuf += pagesize;
        
		switch (option)
		{
		case	OOB_ECC:
			for (j = 0; j < oobsize; j++)
				tmpbuf[j] = oob_buf[j];
			tmpbuf += oobsize;
			break;
		case	OOB_NO_ECC:
			for (j = 0; j < par_size * ecccnt; j++)
				oob_buf[eccpos + j] = 0xff;
			for (j = 0; j < oobsize; j++)
				tmpbuf[j] = oob_buf[j];
			tmpbuf += oobsize;
			break;
		case	NO_OOB:
			break;
		}

		cur_page++;
		cnt++;
	}
	return cur_page;
}

/*oob data 4-bit BCH ecc encoding*/
u32 nand_oobdata_bch_enc_4750(void *buf)
{
	u8 *oobbuf = (u8 *)buf;
	u8 *parbuf = oobbuf + 16;
	volatile u8 *paraddr = (volatile u8 *)BCH_PAR0;
	int i;
	
	REG_BCH_INTS = 0xffffffff;
	__ecc_encoding_4bit();
	__ecc_cnt_enc(oob_len);
	
	for (i = 0; i < oob_len; i++) {
		REG_BCH_DR = oobbuf[i];
	}
	
	__ecc_encode_sync();
	__ecc_disable();
	
	/* Read PAR values */
	for (i = 0; i < par_len; i++) {
		parbuf[i] = *paraddr++;
	}
	
	return 0;
}

u32 nand_program_4750(void *context, int spage, int pages, int option)
{
	u32 i, j, cur_page, cur_blk, rowaddr;
	u8 *tmpbuf;
	u32 ecccnt;
	u8 ecc_buf[256];
	u32 oob_per_eccsize;
	int spl_size = 16 * 1024 / pagesize;

	for(i = 0; i < oobsize; i++)
		oob_buf[i] = 0xff;
    
	if (wp_pin)
		__gpio_set_pin(wp_pin);
restart:
	tmpbuf = (u8 *)context;
	ecccnt = pagesize / ECC_BLOCK;
	oob_per_eccsize = eccpos / ecccnt;

	i = 0;
	cur_page = spage;

	while (i < pages) {
		if (cur_page < spl_size) {
			bchbit = 8;
			eccpos = 3;
			par_size = 13;
		} else if (cur_page >= spl_size) {
			bchbit = bchbit_sav;
			eccpos = eccpos_sav;
			par_size = par_size_sav;
		}

		if ((cur_page % ppb) == 0) { /* First page of block, test BAD. */
			if (nand_check_block(cur_page / ppb)) {
				cur_page += ppb;   /* Bad block, set to next block */
				continue;
			}
		}

		if ( option != NO_OOB )      //if NO_OOB do not perform vaild check!
		{
			for ( j = 0 ; j < pagesize + oobsize; j ++)
			{
				if (tmpbuf[j] != 0xff)
					break;
			}
			if ( j == oobsize + pagesize ) 
			{
				tmpbuf += ( pagesize + oobsize ) ;
				i ++;
				cur_page ++;
				continue;
			}
		}

		if (pagesize == 512)
			__nand_cmd(CMD_READA);

		__nand_cmd(CMD_SEQIN);
		__nand_addr(0);

		if (pagesize != 512)
			__nand_addr(0);

		rowaddr = cur_page;
		for (j = 0; j < row; j++) {
			__nand_addr(rowaddr & 0xff);
			rowaddr >>= 8;
		}
		
		if (option == OOB_NO_ECC)	/*oobdata do ecc in -o mode*/
			nand_oobdata_bch_enc_4750(&tmpbuf[pagesize]);
		
		/* write out data */
		for (j = 0; j < ecccnt; j++) {
			volatile u8 *paraddr;
			int k;

			paraddr = (volatile u8 *)BCH_PAR0;

			REG_BCH_INTS = 0xffffffff;

			if (bchbit == 8)
				__ecc_encoding_8bit();
			else
				__ecc_encoding_4bit();
			
			/* Set BCHCNT.DEC_COUNT to data block size in bytes */
			if (option != NO_OOB)
				__ecc_cnt_enc(ECC_BLOCK + oob_per_eccsize);
			else
				__ecc_cnt_enc(ECC_BLOCK);
			
			/* Write data in data area to BCH */
			for (k = 0; k < ECC_BLOCK; k++) {
				REG_BCH_DR = tmpbuf[ECC_BLOCK * j + k];
			}
			
			/* Write file system information in oob area to BCH */
			if (option != NO_OOB)
			{
				for (k = 0; k < oob_per_eccsize; k++) {
					REG_BCH_DR = tmpbuf[pagesize + oob_per_eccsize * j + k];
				}
			}
			
			__ecc_encode_sync();
			__ecc_disable();
			
			/* Read PAR values */
			for (k = 0; k < par_size; k++) {
				ecc_buf[j * par_size + k] = *paraddr++;
			}
			
			write_proc((char *)&tmpbuf[ECC_BLOCK * j], ECC_BLOCK);
		}

		switch (option)
		{
		case OOB_ECC:
			for (j = 0; j < oobsize; j++) {
				oob_buf[j] = tmpbuf[pagesize + j];
			}
			tmpbuf += pagesize + oobsize;
			break;
		case OOB_NO_ECC:          
			for (j = 0; j < eccpos; j++) {
				oob_buf[j] = tmpbuf[pagesize + j];
			}
			for (j = 0; j < ecccnt * par_size; j++) {
				oob_buf[eccpos + j] = ecc_buf[j];
			}
			tmpbuf += pagesize + oobsize;
			break;
		case NO_OOB:              //bin image
			for (j = 0; j < ecccnt * par_size; j++) {
				oob_buf[eccpos + j] = ecc_buf[j];
			}
			tmpbuf += pagesize;
			break;
		}

		/* write out oob buffer */
		write_proc((u8 *)oob_buf, oobsize);

		/* send program confirm command */
		__nand_cmd(CMD_PGPROG);
		__nand_sync();

		__nand_cmd(CMD_READ_STATUS);

		if (__nand_data8() & 0x01)  /* page program error */
		{
			serial_puts("Skip a write fail block\n");
			nand_erase_4750( 1, cur_page/ppb, 1);  //force erase before
			nand_mark_bad_4750(cur_page/ppb);
			spage += ppb;
			goto restart;
		}

		i++;
		cur_page++;
	}

	if (wp_pin)
		__gpio_clear_pin(wp_pin);
	bchbit = bchbit_sav;
	eccpos = eccpos_sav;
	par_size = par_size_sav;

	return cur_page;
}

static u32 nand_mark_bad_page(u32 page)
{
	u8 badbuf[4096 + 128];
	u32 i;

	if (wp_pin)
		__gpio_set_pin(wp_pin);
	for (i = 0; i < pagesize + oobsize; i++)
		badbuf[i] = 0x00;
	//all set to 0x00

	__nand_cmd(CMD_READA);
	__nand_cmd(CMD_SEQIN);

	__nand_addr(0);
	if (pagesize != 512)
		__nand_addr(0);
	for (i = 0; i < row; i++) {
		__nand_addr(page & 0xff);
		page >>= 8;
	}

	write_proc((char *)badbuf, pagesize + oobsize);
	__nand_cmd(CMD_PGPROG);
	__nand_sync();

	if (wp_pin)
		__gpio_clear_pin(wp_pin);
	return page;
}

u32 nand_mark_bad_4750(int block)
{
	u32 rowaddr;

	if ( bad_block_page >= ppb )    //absolute bad block mark!
	{                               //mark four page!
		rowaddr = block * ppb + 0;
		nand_mark_bad_page(rowaddr);

		rowaddr = block * ppb + 1;
		nand_mark_bad_page(rowaddr);

		rowaddr = block * ppb + ppb - 2;
		nand_mark_bad_page(rowaddr);

		rowaddr = block * ppb + ppb - 1;
		nand_mark_bad_page(rowaddr);
	}
	else                            //mark one page only
	{
		rowaddr = block * ppb + bad_block_page;
		nand_mark_bad_page(rowaddr);
	}

	return rowaddr;
}

static int nand_data_write8(char *buf, int count)
{
	int i;
	u8 *p = (u8 *)buf;
	for (i=0;i<count;i++)
		__nand_data8() = *p++;
	return 0;
}

static int nand_data_write16(char *buf, int count)
{
	int i;
	u16 *p = (u16 *)buf;
	for (i=0;i<count/2;i++)
		__nand_data16() = *p++;
	return 0;
}

static int nand_data_read8(char *buf, int count)
{
	int i;
	u8 *p = (u8 *)buf;
	for (i=0;i<count;i++)
		*p++ = __nand_data8();
	return 0;
}

static int nand_data_read16(char *buf, int count)
{
	int i;
	u16 *p = (u16 *)buf;
	for (i=0;i<count/2;i++)
		*p++ = __nand_data16();
	return 0;
}
