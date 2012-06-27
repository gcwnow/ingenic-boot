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

#include "jz4755.h"
#include "..//configs.h"
int pllout2 = 0;

#define is_share_mode() ((INREG32(A_EMC_BCR) & EMC_BCR_BSR_MASK) == EMC_BCR_BSR_SHARE)
#define is_normal_order() (!(INREG32(A_EMC_BCR) & EMC_BCR_PK_SEL))

void gpio_as_uart1_4755()
{
	OUTREG32(A_GPIO_PXTRGC(4), 0x02800000);
	OUTREG32(A_GPIO_PXFUNS(4), 0x02800000);
	OUTREG32(A_GPIO_PXSELS(4), 0x02800000);
	OUTREG32(A_GPIO_PXPES(4), 0x02800000);
}

void gpio_as_uart0_4755()
{
	OUTREG32(A_GPIO_PXTRGC(3), 0x30000000);
	OUTREG32(A_GPIO_PXFUNS(3), 0x30000000);
	OUTREG32(A_GPIO_PXSELS(3), 0x30000000);
	OUTREG32(A_GPIO_PXPES(3), 0x30000000);
}

void gpio_as_sdram_32bit()
{
	OUTREG32(A_GPIO_PXFUNS(0), 0xffffffff);
	OUTREG32(A_GPIO_PXSELC(0), 0xffffffff);
	OUTREG32(A_GPIO_PXPES(0), 0xffffffff);

	OUTREG32(A_GPIO_PXFUNS(1), 0x03ff7fff);
	OUTREG32(A_GPIO_PXSELC(1), 0x03ff7fff);
	OUTREG32(A_GPIO_PXPES(1), 0x03ff7fff);
}

/*
 * D0 ~ D7, CS1#, CLE, ALE, FRE#, FWE#, FRB#, RDWE#/BUFD#
 */
 /*
void gpio_as_nand_8bit()
{
	OUTREG32(A_GPIO_PXFUNS(0), 0x000000ff);
	OUTREG32(A_GPIO_PXSELC(0), 0x000000ff);
	OUTREG32(A_GPIO_PXPES(0), 0x000000ff);

	OUTREG32(A_GPIO_PXFUNS(1), 0x00088000);
	OUTREG32(A_GPIO_PXSELC(1), 0x00088000);
	OUTREG32(A_GPIO_PXPES(1), 0x00088000);

	OUTREG32(A_GPIO_PXFUNS(2), 0x30210000);
	OUTREG32(A_GPIO_PXSELC(2), 0x30210000);
	OUTREG32(A_GPIO_PXPES(2), 0x30210000);

	OUTREG32(A_GPIO_PXFUNS(2), 0x08000000);
	OUTREG32(A_GPIO_PXSELC(2), 0x08000000);
	OUTREG32(A_GPIO_PXDIRC(2), 0x08000000);
	OUTREG32(A_GPIO_PXPES(2), 0x08000000);
}
*/
#define gpio_as_nand_8bit()						\
do {		              						\
	if (!is_share_mode()) {						\
		/* unshare mode */					\
		OUTREG32(A_GPIO_PXFUNS(2), 0x000000ff); /* SD0~SD7 */		\
		OUTREG32(A_GPIO_PXSELC(2), 0x000000ff);			\
		OUTREG32(A_GPIO_PXPES(2), 0x000000ff);				\
		OUTREG32(A_GPIO_PXFUNS(1), 0x00008000); /* CLE(SA3) */		\
		OUTREG32(A_GPIO_PXSELS(1), 0x00008000);			\
		OUTREG32(A_GPIO_PXPES(1), 0x00008000);				\
		OUTREG32(A_GPIO_PXFUNS(2), 0x00010000); /* ALE(SA4) */		\
		OUTREG32(A_GPIO_PXSELS(2), 0x00010000);			\
		OUTREG32(A_GPIO_PXPES(2), 0x00010000);				\
	} else {							\
		/* share mode */					\
		if (is_normal_order()) {	              		\
			/* 32/16-bit data normal order */		\
			OUTREG32(A_GPIO_PXFUNS(0), 0x000000ff); /* D0~D7 */	\
			OUTREG32(A_GPIO_PXSELC(0), 0x000000ff);		\
			OUTREG32(A_GPIO_PXPES(0), 0x000000ff);			\
		} else {						\
			/* 16-bit data special order */			\
			OUTREG32(A_GPIO_PXFUNS(0), 0x0000ff00); /* D0~D7 */	\
			OUTREG32(A_GPIO_PXSELC(0), 0x0000ff00);		\
			OUTREG32(A_GPIO_PXPES(0), 0x0000ff00);			\
		}							\
		OUTREG32(A_GPIO_PXFUNS(1), 0x00008000); /* CLE(A15) */		\
		OUTREG32(A_GPIO_PXSELC(1), 0x00008000);			\
		OUTREG32(A_GPIO_PXPES(1), 0x00008000);				\
		OUTREG32(A_GPIO_PXFUNS(2), 0x00010000); /* ALE(A16) */		\
		OUTREG32(A_GPIO_PXSELC(2), 0x00010000);			\
		OUTREG32(A_GPIO_PXPES(2), 0x00010000);				\
	}								\
									\
	OUTREG32(A_GPIO_PXFUNS(2), 0x00200000); /* CS1 */			\
        OUTREG32(A_GPIO_PXFUNS(1), 0x00080000); /* RDWE#/BUFD# */		\
        OUTREG32(A_GPIO_PXSELC(1), 0x00080000);				\
	OUTREG32(A_GPIO_PXPES(1) , 0x00080000);					\
	OUTREG32(A_GPIO_PXFUNS(2), 0x30000000); /* FRE#, FWE# */		\
	OUTREG32(A_GPIO_PXSELC(2), 0x30000000);				\
	OUTREG32(A_GPIO_PXPES(2), 0x30000000);					\
	OUTREG32(A_GPIO_PXFUNC(2), 0x08000000); /* FRB#(input) */		\
	OUTREG32(A_GPIO_PXSELC(2), 0x08000000);				\
	OUTREG32(A_GPIO_PXDIRC(2), 0x08000000);				\
	OUTREG32(A_GPIO_PXPES(2), 0x08000000);					\
} while (0)

void jz4755_gpio_init(void)
{
	if(IS_SHARE) {
		//REG_EMC_BCR &= ~EMC_BCR_BSR_UNSHARE;
		CLRREG32(A_EMC_BCR, EMC_BCR_BSR_UNSHARE);
	} else {
		//REG_EMC_BCR |= EMC_BCR_BSR_UNSHARE;
		SETREG32(A_EMC_BCR, EMC_BCR_BSR_UNSHARE);
	}

	gpio_as_sdram_32bit();
	
	OUTREG32(A_CPM_CLKGR, 0);
	if (fw_args->use_uart == 1)
	{
		gpio_as_uart1_4755();
	}
	else
	{
		gpio_as_uart0_4755();
	}
	
	gpio_as_nand_8bit();
}

int get_sdram_configure(unsigned int pllin, unsigned int *sdram_freq)
{
	register unsigned int ps, dmcr = 0, tmp;
 
	ps = 1000000/(pllin / 1000000);
	tmp = (SDRAM_TRAS * 1000)/ps;
	if (tmp < 4) tmp = 4;
	if (tmp > 11) tmp = 11;
	dmcr |= ((tmp-4) << DMCR_TRAS_BIT);

	tmp = (SDRAM_RCD * 1000)/ps;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << DMCR_RCD_BIT);

	tmp = (SDRAM_TPC * 1000)/ps;
	if (tmp > 7) tmp = 7;
	dmcr |= (tmp << DMCR_TPC_BIT);

	tmp = (SDRAM_TRWL * 1000)/ps;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << DMCR_TRWL_BIT);

	tmp = ((SDRAM_TRAS + SDRAM_TPC) * 1000)/ps;
	if (tmp > 14) tmp = 14;
	dmcr |= (((tmp + 1) >> 1) << DMCR_TRC_BIT);

	/* Set refresh registers */
	tmp = (SDRAM_TREF * 1000)/ps;
	tmp = tmp/64 + 1;
	if (tmp > 0xff) tmp = 0xff;
        *sdram_freq = tmp; 

	return dmcr;
}

void jz4755_pll_init()
{
	int nf;
	unsigned int sdramclock = 0;
	register unsigned int cfcr, plcr1,tmp;
	int n2FR[33] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5, 0, 0, 0, 6, 0, 0, 0,
		7, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
		9
	};
	int div[5] = {1, 3, 3, 3, 3}; /* divisors of I:S:P:L:M */

	cfcr = 	CPCCR_PLLCLK_DIV2 |
		(n2FR[1] 			<< CPCCR_CDIV_BIT) 		|
		(n2FR[PHM_DIV] 	<< CPCCR_H0DIV_BIT) 		|
		(n2FR[PHM_DIV] 	<< CPCCR_PDIV_BIT) 			|
		(n2FR[PHM_DIV] 	<< CPCCR_MDIV_BIT) 			|
		(n2FR[PHM_DIV] 	<< CPCCR_H1DIV_BIT);

	if (CFG_EXTAL > 16000000)
		cfcr |= CPCCR_ECS_EXCLK_DIV2;

	pllout2 = (cfcr & CPCCR_PLLCLK_DIV2) ? CFG_CPU_SPEED : (CFG_CPU_SPEED / 2);

	nf = CFG_CPU_SPEED * 2 / CFG_EXTAL;
	plcr1 = ((nf - 2) << CPPCR_PLLM_BIT) 	| /* FD */
			(0 << CPPCR_PLLN_BIT) 			|	/* RD=0, NR=2 */
			(0 << CPPCR_PLLOD_BIT) 			|    /* OD=0, NO=1 */
			(0x20 << CPPCR_PLLST_BIT) 	| /* PLL stable time */
			CPPCR_PLLEN;                /* enable PLL */

	cfcr |= CPCCR_USB_PLLCLK;           /* set PLL as UDC PHY*/
	tmp = pllout2 / 1000000 / 12 - 1;
	cfcr |= (tmp << CPCCR_UDIV_BIT);    /* set UDC DIV*/

	/* init PLL */
	OUTREG32(A_CPM_CPCCR, cfcr);
	OUTREG32(A_CPM_CPPCR, plcr1);

	get_sdram_configure(pllout2, &sdramclock);
	if (sdramclock > 0)
	{
		OUTREG16(A_EMC_RTCOR, sdramclock);
		OUTREG16(A_EMC_RTCNT, sdramclock);

  		// Config SDRAM to auto Power Down Mode
		//SETREG32(A_EMC_DMCR, DMCR_POWER_DOWN);	
	}
	else
	{
		serial_puts("\nInvalid sdramclock.........\n");
		while(1);
	}
}

//----------------------------------------------------------------------------
unsigned int CalcutatePLLClock(unsigned int cppcr,int  bCheckPCSBit)
{
	unsigned int  m, n, od, pll, pcs;

	if ((cppcr & CPPCR_PLLEN) && !(cppcr & CPPCR_PLLBP))
	{
		m = ((cppcr & CPPCR_PLLM_MASK) >> CPPCR_PLLM_BIT) + 2;
		n = ((cppcr & CPPCR_PLLN_MASK) >> CPPCR_PLLN_BIT) + 2;
		od = ((cppcr & CPPCR_PLLOD_MASK) >> CPPCR_PLLOD_BIT) + 1;
		od = (od == 3) ? (od - 1) : od;
		pll = CFG_EXTAL * m / (n * od);
	}
	else
		pll = CFG_EXTAL;

	// Get PCS Bit for MSC, I2S, LCD and USB if need.
	if (bCheckPCSBit)
	{
		pcs = INREG32(A_CPM_CPCCR) & CPCCR_PLLCLK_DIV2;
		if (pcs == 0)
			pll >>= 1;
	}

	return (pll);
}

unsigned int GetCurrentPLLClock(int bCheckPCSBit)
{
	return CalcutatePLLClock(INREG32(A_CPM_CPPCR), bCheckPCSBit);
}

//----------------------------------------------------------------------------
static unsigned int div_matrix[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

unsigned int GetCommonClock(CLKDIV	clk)
{
	unsigned int div, clock;
		div = (INREG32(A_CPM_CPCCR) >> (unsigned int)clk) & 0x0000000F;
		clock = GetCurrentPLLClock(0) / div_matrix[div];


	return (clock);
}

void jz4755_sdram_init(void)
{
	volatile unsigned int timeout;
	unsigned int sdram_freq;
	register unsigned int dmcr, sdmode;

	OUTREG32(A_EMC_DMAR1, EMC_DMAR1_BASE | EMC_DMAR_MASK_64_64);
	//OUTREG32(A_EMC_BCR, 0);
	CLRREG32(A_EMC_BCR, EMC_BCR_BRE);	/* Disable bus release */
	OUTREG16(A_EMC_RTCSR, 0);

	// Basic DMCR value
	dmcr = DMCR_ROW_WIDTH(SDRAM_ROW)
		 | DMCR_COL_WIDTH(SDRAM_COL)
		 |(SDRAM_BANK4 << DMCR_BA_BIT) | (SDRAM_BW16	<< DMCR_BW_BIT)
		 | DMCR_CKE_ASSERT | DMCR_T_CAS_LATENCY(SDRAM_CASL);

	dmcr |= get_sdram_configure(GetCommonClock(CPM_MDIV), &sdram_freq);

	sdmode = SDMR_BT_SEQ | SDMR_OM_NORMAL
			| SDMR_BL_4 | (SDRAM_CASL << SDMR_CAS_BIT);

	OUTREG32(A_EMC_DMCR, dmcr);
	OUTREG8(A_EMC_SDMR + sdmode, 0);

	// Wait for precharge, > 200us
	timeout = (CFG_CPU_SPEED / 1000000) * 1000;
	while (timeout--);

	SETREG32(A_EMC_DMCR, DMCR_REFRESH);

	sdram_freq = 0x0b;	
	OUTREG16(A_EMC_RTCOR, sdram_freq);
	OUTREG16(A_EMC_RTCNT, sdram_freq);//this value is effect of SDRAM work some SDRAM can not work because this value is to much.It also effec the power,the value more small the power performace is more lager.when download it can be set to 0x0b

	OUTREG16(A_EMC_RTCSR, RTCSR_CKO_DIV_64);

	// Wait for precharge, > 200us
	timeout = (CFG_CPU_SPEED / 1000000) * 1000;
	while (timeout--);

	OUTREG32(A_EMC_DMCR, dmcr | DMCR_REFRESH | DMCR_MRSET);
	OUTREG8(A_EMC_SDMR + sdmode, 0);

	OUTREG32(A_EMC_DMCR, dmcr | DMCR_REFRESH | DMCR_MRSET);
}

void serial_setbrg_4755(void)
{
	volatile unsigned char *uart_lcr = (volatile unsigned char *)(UART_BASE + UART_ULCR_OFFSET);
	volatile unsigned char *uart_dlhr = (volatile unsigned char *)(UART_BASE + UART_UDLHR_OFFSET);
	volatile unsigned char *uart_dllr = (volatile unsigned char *)(UART_BASE + UART_UDLLR_OFFSET);
	unsigned int baud_div, tmp;

	baud_div = (INREG32(A_CPM_CPCCR) & CPCCR_ECS_EXCLK_DIV2) ?
		(CFG_EXTAL / 32 / CONFIG_BAUDRATE) : (CFG_EXTAL / 16 / CONFIG_BAUDRATE);
	tmp = *uart_lcr;
	tmp |= ULCR_DLAB;
	*uart_lcr = tmp;

	*uart_dlhr = (baud_div >> 8) & 0xff;
	*uart_dllr = baud_div & 0xff;

	tmp &= ~ULCR_DLAB;
	*uart_lcr = tmp;
}
