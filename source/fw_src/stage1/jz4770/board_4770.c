/*
 * board.c
 *
 * Board init routines.
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 *
 */
#include "jz4770.h"
#include "configs.h"
#include "board_4770.h"

/*
 * SD0 ~ SD7, SA0 ~ SA5, CS2#, RD#, WR#, WAIT#	
 */
//#define __gpio_as_nor()						\
//do {								        \
	/* SD0 ~ SD7, RD#, WR#, CS2#, WAIT# */				\
//	REG_GPIO_PXFUNS(0) = 0x084300ff;				\
//	REG_GPIO_PXTRGC(0) = 0x084300ff;				\
//	REG_GPIO_PXSELC(0) = 0x084300ff;				\
//	REG_GPIO_PXPES(0) = 0x084300ff;					\
	/* SA0 ~ SA5 */							\
//	REG_GPIO_PXFUNS(1) = 0x0000003f;				\
//	REG_GPIO_PXTRGC(1) = 0x0000003f;				\
//	REG_GPIO_PXSELC(1) = 0x0000003f;				\
//	REG_GPIO_PXPES(1) = 0x0000003f;					\
//} while (0)

void gpio_init_4770()
{
	__gpio_as_uart0();
	__cpm_start_uart0();
	__gpio_as_uart1();
	__cpm_start_uart1();
	__gpio_as_uart2();
	__cpm_start_uart2();
	__gpio_as_uart3();
	__cpm_start_uart3();

#if 0
#ifdef CONFIG_FPGA // if the delay isn't added on FPGA, the first line that uart to print will not be normal. 
	__gpio_as_nor();
	{
		volatile int i=1000;
		while(i--);
	}
#endif
	__gpio_as_nand_8bit(1);
#endif
}

#define MHZ (1000 * 1000)
#define PLL_OUT_MAX 1200		/* 1200MHz. */
static inline unsigned int pll_calc_m_n_od(unsigned int speed, unsigned int xtal)
{
	unsigned int CFG_EXTAL_CLK;
	unsigned int CFG_PLL_OUT;
	unsigned int PLL_NO;
	unsigned int NR;
	unsigned int NO;
	unsigned int PLL_MO;
	unsigned int NF;
	unsigned int FOUT;
	unsigned int PLL_BS;
	unsigned int PLL_OD;
	unsigned int CPCCR_M_N_OD;


	CFG_EXTAL_CLK = xtal/1000000;
	CFG_PLL_OUT = speed/1000000;

	if (CFG_PLL_OUT > PLL_OUT_MAX)
		serail_puts("PLL output can NOT more than 1200MHZ");
	else if (CFG_PLL_OUT > 600) {
		PLL_BS	=	1; 
		PLL_OD	=	0; 
	}
	else if (CFG_PLL_OUT > 300) {
		PLL_BS	 =      0; 
		PLL_OD	 =      0; 
	}
	else if (CFG_PLL_OUT > 155) {
		PLL_BS	 =      0;
		PLL_OD	 =      1;
	}
	else if (CFG_PLL_OUT > 76) {
		PLL_BS	 =      0; 
		PLL_OD	 =      2; 
	}
	else if (CFG_PLL_OUT > 47) {
		PLL_BS	 =      0; 
		PLL_OD	 =      3; 
	}
	else {
		serail_puts("PLL ouptput can NOT less than 48");
		goto __ERROR_PLL;
	}
	PLL_NO = 0;
	NR  = (PLL_NO + 1);
	NO  = (0x1 << PLL_OD);
	PLL_MO =	(((CFG_PLL_OUT / CFG_EXTAL_CLK) * NR * NO) - 1);
	NF = (PLL_MO + 1);
	FOUT = (CFG_EXTAL_CLK * NF / NR / NO);
	PLL_BS = 1;

	if ((CFG_EXTAL_CLK / NR > 50) || (CFG_EXTAL_CLK / NR < 10)) {
		serial_puts("Can NOT set the value to PLL_N: ");
		serial_put_hex(NR);
		goto __ERROR_PLL;
	}
	
	if ((PLL_MO > 127) || (PLL_MO < 1)) {
		serial_puts("Can NOT set the value to PLL_M:");
		serial_put_hex(PLL_MO);
		goto __ERROR_PLL;
	}

	if ((FOUT * NO) < 500)
		PLL_BS = 0;
	else if ((FOUT * NO) > 600)
		PLL_BS == 1;
	if ((FOUT * NO) > PLL_OUT_MAX || ((FOUT * NO) < 300)) {
		serial_puts("The PLL is outof range");
		goto __ERROR_PLL;
	}
	CPCCR_M_N_OD =	((PLL_MO << 24) | (PLL_NO << 18) | (PLL_OD << 16) | (PLL_BS << 31));
		 
	return CPCCR_M_N_OD;
__ERROR_PLL:
	while(1);
}

#define DIV_1 0
#define DIV_2 1
#define DIV_3 2
#define DIV_4 3
#define DIV_6 4
#define DIV_8 5
#define DIV_12 6
#define DIV_16 7
#define DIV_24 8
#define DIV_32 9

#define DIV(a,b,c,d,e,f)					\
({								\
	unsigned int retval;					\
	retval = (DIV_##a << CPM_CPCCR_CDIV_BIT)   |		\
		 (DIV_##b << CPM_CPCCR_H0DIV_BIT)  |		\
		 (DIV_##c << CPM_CPCCR_PDIV_BIT)   |		\
		 (DIV_##d << CPM_CPCCR_C1DIV_BIT)  |		\
		 (DIV_##e << CPM_CPCCR_H2DIV_BIT)  |		\
		 (DIV_##f << CPM_CPCCR_H1DIV_BIT);		\
	retval;							\
})

/* TODO: pll_init() need modification. */
void pll_init_4770()
{
	register unsigned int cfcr, plcr1;
	int n2FR[9] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5
	};

	/** divisors, 
	 *  for jz4770 ,C:H0:P:C1:H2:H1.
	 *  DIV should be one of [1, 2, 3, 4, 6, 8, 12, 16, 24, 32]
	 */

//	unsigned int div = DIV(1,6,6,2,6,3);
	unsigned int div;
	int pllout2;

	div = DIV(1,4,8,2,4,4);
#if 0
	if (CFG_CPU_SPEED < 288000000)
		div = DIV(1,2,2,2,2,2);
	else if (CFG_CPU_SPEED < 528000000)
		div = DIV(1,2,2,2,2,2);
	else
		div = DIV(1,4,8,2,4,4);
#endif
	
//	cfcr = DIV(1,2,4,2,4,4);

	cfcr = 	div;

	// write REG_DDRC_CTRL 8 times to clear ddr fifo
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;

	/* set CPM_CPCCR_MEM only for ddr1 or ddr2 */
#if (defined(CONFIG_SDRAM_DDR1) || defined(CONFIG_SDRAM_DDR2))
	cfcr |= CPM_CPCCR_MEM;
#else	/* mddr or sdram */
	cfcr &= ~CPM_CPCCR_MEM;
#endif
	cfcr |= CPM_CPCCR_CE;

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? (CFG_CPU_SPEED/2) : CFG_CPU_SPEED;

	plcr1 = pll_calc_m_n_od(CFG_CPU_SPEED, CFG_EXTAL);
	plcr1 |= (0x20 << CPM_CPPCR_PLLST_BIT)	/* PLL stable time */
		 | CPM_CPPCR_PLLEN;             /* enable PLL */

	serial_puts("CFG_CPU_SPEED = ");
	serial_put_hex(CFG_CPU_SPEED);
	serial_puts("CFG_EXTAL = ");
	serial_put_hex(CFG_EXTAL);


	/* init PLL */
	serial_puts("cfcr = ");
	serial_put_hex(cfcr);
	serial_puts("plcr1 = ");
	serial_put_hex(plcr1);

	REG_CPM_CPCCR = cfcr;
	REG_CPM_CPPCR = plcr1;
	
	while(!(REG_CPM_CPPSR & (1 << 29))); 
	
	/*wait for pll output stable ...*/
	while (!(REG_CPM_CPPCR & CPM_CPPCR_PLLS));

	serial_puts("REG_CPM_CPCCR = ");
	serial_put_hex(REG_CPM_CPCCR);
	serial_puts("REG_CPM_CPPCR = ");
	serial_put_hex(REG_CPM_CPPCR);
}

void serial_setbrg_4770(void)
{
	volatile u8 *uart_lcr = (volatile u8 *)(UART_BASE + OFF_LCR);
	volatile u8 *uart_dlhr = (volatile u8 *)(UART_BASE + OFF_DLHR);
	volatile u8 *uart_dllr = (volatile u8 *)(UART_BASE + OFF_DLLR);
	u32 baud_div, tmp;

	baud_div = (CFG_EXTAL / 16 / 57600);
	//baud_div = (CFG_EXTAL / 16 / CONFIG_BAUDRATE);

	tmp = *uart_lcr;
	tmp |= UART_LCR_DLAB;
	*uart_lcr = tmp;

	*uart_dlhr = (baud_div >> 8) & 0xff;
	*uart_dllr = baud_div & 0xff;

	tmp &= ~UART_LCR_DLAB;
	*uart_lcr = tmp;
}
