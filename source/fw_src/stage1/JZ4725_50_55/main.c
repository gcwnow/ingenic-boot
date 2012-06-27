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

#define FOREC_TO_4750 1
#define FW_DEBUG	0

#include "configs.h"

volatile unsigned int CPU_ID;
volatile unsigned int UART_BASE;
volatile unsigned int CONFIG_BAUDRATE;
volatile unsigned char SDRAM_BW16;
volatile unsigned char  SDRAM_BANK4;
volatile unsigned char  SDRAM_ROW;
volatile unsigned char  SDRAM_COL;
volatile unsigned char  CONFIG_MOBILE_SDRAM;
volatile unsigned int CFG_CPU_SPEED;
volatile unsigned int CFG_EXTAL;
volatile unsigned char  PHM_DIV;
volatile unsigned char  IS_SHARE;
volatile unsigned char cpu_type = 0;

#if FW_DEBUG
unsigned int check_sdram(unsigned int saddr, unsigned int size)
{
	unsigned int addr,err = 0;

	serial_puts("\nCheck SDRAM ... \n");
	saddr += 0xa0000000;
	size += saddr;
	serial_put_hex(saddr);
	serial_put_hex(size);
	
	saddr &= 0xfffffffc;      //must word align
	for (addr = saddr; addr < size; addr += 4)
	{
		*(volatile unsigned int *)addr = addr;
		if (*(volatile unsigned int *)addr != addr)
		{
			serial_put_hex(addr);
			err = addr;
		}
	}
	
	if (err)
		serial_puts("Check SDRAM fail!\n");
	else
		serial_puts("Check SDRAM pass!\n");
	
	return err;
}
#endif

void load_args(void)
{
	fw_args = (fw_args_t *)0x80002008;       //get the fw args from memory
	
	CPU_ID = fw_args->cpu_id ;
	CFG_EXTAL = (unsigned int)fw_args->ext_clk * 1000000;
	CFG_CPU_SPEED = (unsigned int)fw_args->cpu_speed * CFG_EXTAL ;
	if (CFG_EXTAL == 19000000) 
	{
		CFG_EXTAL = 19200000;
		CFG_CPU_SPEED = 192000000;
	}
	
	PHM_DIV = fw_args->phm_div;
	if (JZ4725B == cpu_type)
	{
		fw_args->use_uart = 0;
	}
	else if (fw_args->use_uart > 3)
	{
		fw_args->use_uart = 0;
	}
	
	UART_BASE = UART_BASE_U_VIRTUAL + fw_args->use_uart * 0x1000;
	CONFIG_BAUDRATE = fw_args->boudrate;
	
	SDRAM_BW16 = fw_args->bus_width;
	SDRAM_BANK4 = fw_args->bank_num;
	SDRAM_ROW = fw_args->row_addr;
	SDRAM_COL = fw_args->col_addr;
	CONFIG_MOBILE_SDRAM = fw_args->is_mobile;
	IS_SHARE = fw_args->is_busshare;
}

static void inline gain_cpu_type(void)
{
	int cpu_id = 0;
	int jz4725_pre_check = 0;
	
	jz4725_pre_check = (unsigned int) ( * (volatile unsigned int *) 0xB00F0000);
	cpu_id = read_32bit_cp0_processorid();

	if (jz4725_pre_check&0x80000000)
	{
		cpu_type = JZ4725B;
	}
	else if (cpu_id == PROCESSOR_ID_4755)
	{
		cpu_type = JZ4755;
	}
	else if(cpu_id == PROCESSOR_ID_4750)
	{
		cpu_type = JZ4750;
	}
	else
	{
		;
	}
}

void c_main(void)
{
	int i = 0, cpu_id = 0;
	int jz4725_pre_check = 0;
	
	gain_cpu_type();
	load_args();

	switch(cpu_type)
	{
		case JZ4725B:
			jz4725b_gpio_init();
			jz4725b_pll_init();
			jz4725b_serial_init();
			jz4725b_sdram_init();
			serial_puts("Welcome, CPU is 4725B................\n");
			break;
		case JZ4755:
			jz4755_gpio_init();
			jz4755_pll_init();
			jz4755_serial_init();
			jz4755_sdram_init();
			serial_puts("Welcome, CPU is 4755................\n");
			break;
		case JZ4750:
			gpio_init_4750();
			jz4750_pll_init();
			jz4750_serial_init();
			jz4750_sdram_init();
			serial_puts("Welcome, CPU is 4750................\n");
			break;
	}

#if FW_DEBUG
	serial_puts("\ncpu_id is: \n");
	serial_put_hex(cpu_id);

	
	serial_puts("\ntesting..............\n");
	
	
	serial_puts("\ngpio dump..............\n");
	serial_puts("\nA_GPIO_PXFUN 0 1 2 4..............\n");
	serial_put_hex(INREG32(A_GPIO_PXFUN(0)));
	serial_put_hex(INREG32(A_GPIO_PXFUN(1)));
	serial_put_hex(INREG32(A_GPIO_PXFUN(2)));
	serial_put_hex(INREG32(A_GPIO_PXFUN(4)));

	serial_puts("\nA_GPIO_PXSEL 0 1 2 4..............\n");
	serial_put_hex(INREG32(A_GPIO_PXSEL(0)));
	serial_put_hex(INREG32(A_GPIO_PXSEL(1)));
	serial_put_hex(INREG32(A_GPIO_PXSEL(2)));
	serial_put_hex(INREG32(A_GPIO_PXSEL(4)));

	serial_puts("\nA_GPIO_PXPE 0 1 2 4..............\n");
	serial_put_hex(INREG32(A_GPIO_PXPE(0)));
	serial_put_hex(INREG32(A_GPIO_PXPE(1)));
	serial_put_hex(INREG32(A_GPIO_PXPE(2)));
	serial_put_hex(INREG32(A_GPIO_PXPE(4)));

	serial_puts("\nA_GPIO_PXTRG 0 1 2 4..............\n");
	serial_put_hex(INREG32(A_GPIO_PXTRG(4)));

	serial_puts("UART_BASE\n");
	serial_put_hex(UART_BASE);
	serial_put_hex(UART_BASE_U_VIRTUAL);
	serial_put_hex(fw_args->use_uart);
	serial_puts("pll_init_4755\n");
	serial_puts("A_CPM_CPCCR\n");
	serial_put_hex(INREG32(A_CPM_CPCCR));
	serial_puts("A_CPM_CPPCR\n");
	serial_put_hex(INREG32(A_CPM_CPPCR));
	serial_puts("jz4755_sdram_init\n");
	serial_puts("A_EMC_DMCR\n");
	serial_put_hex(INREG32(A_EMC_DMCR));
	serial_puts("A_EMC_RTCOR\n");
	serial_put_hex(INREG16(A_EMC_RTCOR));
	serial_puts("A_EMC_RTCNT\n");
	serial_put_hex(INREG16(A_EMC_RTCNT));
	serial_puts("A_EMC_RTCSR\n");
	serial_put_hex(INREG16(A_EMC_RTCSR));
	check_sdram(0, 0x10);
#endif
	
	serial_puts("\n Starting SDRAM test ......\n");
	if(memory_post_test(1) != 0)
	{
		serial_puts(" SDRAM test result: fail! Please check sdram configurate parament.\n");
		while(1);
	}
	serial_puts(" SDRAM test result: pass!\n");

#if FW_DEBUG	
	serial_put_hex(cpu_id);
	serial_put_hex(fw_args->cpu_id);
	serial_put_hex(cpu_type);
	serial_put_hex(CFG_EXTAL);
	serial_put_hex(CFG_CPU_SPEED);
	serial_put_hex(PHM_DIV);
	serial_put_hex(CONFIG_BAUDRATE);
	serial_put_hex(SDRAM_BW16);
	serial_put_hex(SDRAM_BANK4);
	serial_put_hex(SDRAM_ROW);
	serial_put_hex(SDRAM_COL);
#endif
	serial_puts("\n Flash and sdram data bus config mode: ");
	if(IS_SHARE)
		serial_puts("share.\n");
	else
		serial_puts("unshare.\n");
		
	//serial_puts("Fw run finish !\n");
	//while(1);
	
	return;
}

