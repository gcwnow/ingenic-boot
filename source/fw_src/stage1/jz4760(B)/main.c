/*
 * main.c
 *
 * Main routine of the firmware.
 *
 * Copyright (C) 2008 Ingenic Semiconductor Inc.
 *
 */
#include "jz4760.h"
#include "configs.h"

fw_args_t * fw_args;
volatile u32 CPU_ID;
volatile u32 UART_BASE;
volatile u32 CONFIG_BAUDRATE;
volatile u8 SDRAM_BW16;
volatile u8 SDRAM_BANK4;
volatile u8 SDRAM_ROW;
volatile u8 SDRAM_COL;
volatile u8 CONFIG_MOBILE_SDRAM;
volatile u32 CFG_CPU_SPEED;
volatile u32 CFG_EXTAL;
volatile u8 PHM_DIV;
volatile u8 IS_SHARE;
extern int pllout2;
volatile u32 i;

#if 1
void test_load_args(void)
{
	CPU_ID = 0x4760 ;
	CFG_EXTAL = 12000000;
	CFG_CPU_SPEED = 144000000;
	PHM_DIV = 2;
	UART_BASE = UART0_BASE + 0x1000;
	CONFIG_BAUDRATE = 115200;
	SDRAM_BW16 = 0;
	SDRAM_BANK4 = 1;
	SDRAM_ROW = 13;
	SDRAM_COL = 9;
	CONFIG_MOBILE_SDRAM = 0;
	IS_SHARE = 1;
}
#endif

void load_args(void)
{
	fw_args = (fw_args_t *)0x80002008;       //get the fw args from memory
	CPU_ID = fw_args->cpu_id ;
#if 0
	CFG_EXTAL = (u32)fw_args->ext_clk * 1000000;
	CFG_CPU_SPEED = (u32)fw_args->cpu_speed * CFG_EXTAL ;
	if (CFG_EXTAL == 19000000) 
	{
		CFG_EXTAL = 19200000;
		CFG_CPU_SPEED = 192000000;
	}
#endif
	CFG_EXTAL = 33333000;
	CFG_CPU_SPEED = CFG_EXTAL;
	PHM_DIV = fw_args->phm_div;
	if ( fw_args->use_uart > 3 ) fw_args->use_uart = 0;
	UART_BASE = UART0_BASE + fw_args->use_uart * 0x1000;
	CONFIG_BAUDRATE = fw_args->boudrate;
	SDRAM_BW16 = fw_args->bus_width;
	SDRAM_BANK4 = fw_args->bank_num;
	SDRAM_ROW = fw_args->row_addr;
	SDRAM_COL = fw_args->col_addr;
	CONFIG_MOBILE_SDRAM = fw_args->is_mobile;
	IS_SHARE = fw_args->is_busshare;
}

void c_main(void)
{
	unsigned int cpu_id;
	cpu_id = *((volatile u32*) (0XBFC0132C));

#if 0
	load_args();

	if (fw_args->debug_ops > 0)
	{
		do_debug();
		return ;
	}
#endif
	test_load_args();
	gpio_init_4760();
	
	if(cpu_id == 0x352920e4)
	{
		// for jz4760B
		__cpm_start_uart1();
		REG_MDMAC_DMACKES = 0x3;
	}

	__cpm_start_ddr();
	__cpm_start_mdma();
	__cpm_start_dmac();
	__cpm_start_uart1();
	__cpm_start_bch();
	__cpm_start_nemc();
	
	serial_init();
	if(cpu_id == 0x352920e4)
	{
		serial_puts("Setup JZ4760B fw args finish!\n");
	}
	else
	{
		serial_puts("Setup JZ4760 fw args finish!\n");
	}
	pll_init_4760();
	sdram_init_4760(cpu_id);

	serial_puts("Setup fw args finish ??\n");
	__asm__ (
		"li  $31, 0xbfc012e0 \n\t"
		"jr  $31 \n\t "
		);
}


