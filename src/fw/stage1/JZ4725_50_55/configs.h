/********************** BEGIN LICENSE BLOCK ************************************
 *
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
 *
 * This file, and the files included with this file, is distributed and made
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * http://www.ingenic.cn
 *
 ********************** END LICENSE BLOCK **************************************
 *
 *  Author:   <hyhang@ingenic.cn>
 *
 *  Create:   2009-06-27, by hyhang
 *
 *******************************************************************************
 */
 
#ifndef _CONFIGS_H
#define _CONFIGS_H
#include "jz4755/jz4755.h"
#include "jz4725b/jz4725b.h"
#include "jz4750/jz4750.h"
//Here are these common definitions
//Once your system configration change,just modify the file

#define CONFIG_NR_DRAM_BANKS	1  /* SDRAM BANK Number: 1, 2*/
#define SDRAM_CASL		3	/* CAS latency: 2 or 3 */
// SDRAM Timings, unit: ns
#define SDRAM_TRAS		45	/* RAS# Active Time */
#define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#define SDRAM_TPC		20	/* RAS# Precharge Time */
#define SDRAM_TRWL		7	/* Write Latency Time */
#define SDRAM_TREF	    15625	/* Refresh period: 4096 refresh cycles/64ms */

#define read_32bit_cp0_processorid()                            \
({ int __res;                                                   \
        __asm__ __volatile__(                                   \
        "mfc0\t%0,$15\n\t"                                      \
        : "=r" (__res));                                        \
        __res;})

extern volatile unsigned int CPU_ID;
extern volatile unsigned char SDRAM_BW16;
extern volatile unsigned char SDRAM_BANK4;
extern volatile unsigned char SDRAM_ROW;
extern volatile unsigned char SDRAM_COL;
extern volatile unsigned char CONFIG_MOBILE_SDRAM;
extern volatile unsigned int CFG_CPU_SPEED;
extern volatile unsigned char PHM_DIV;
extern volatile unsigned int CFG_EXTAL;
extern volatile unsigned int CONFIG_BAUDRATE;
extern volatile unsigned int UART_BASE;
extern volatile unsigned char CONFIG_MOBILE_SDRAM;
extern volatile unsigned char IS_SHARE;
extern volatile unsigned char UARTNUM;

typedef struct {
	/* CPU ID */
	unsigned int  cpu_id;
	/* PLL args */
	unsigned char ext_clk;
	unsigned char cpu_speed;
	unsigned char phm_div;
	unsigned char use_uart;
	unsigned int  boudrate;

	/* SDRAM args */
	unsigned char bus_width;
	unsigned char bank_num;
	unsigned char row_addr;
	unsigned char col_addr;
	unsigned char is_mobile;
	unsigned char is_busshare;

	/* debug args */
	unsigned char debug_ops;
	unsigned char pin_num;
	unsigned int  start;
	unsigned int  size;
}fw_args_t;

enum cpu_type
{
	UNKNOW_TYPE = 0,
	JZ4725B,
	JZ4755,
	JZ4750
};

fw_args_t * fw_args;

extern void jz4755_gpio_init();
extern void jz4755_serial_init();
extern void jz4755_sdram_init();
extern void jz4755_pll_init();

extern void jz4725b_gpio_init();

extern void serial_puts(const char *s);

extern void gpio_init_4750();
extern void pll_init_4750();
extern void sdram_init_4750();

typedef void (*FW_INIT)();
#define INIT_NUM	4
FW_INIT fw_init[INIT_NUM];

#endif
