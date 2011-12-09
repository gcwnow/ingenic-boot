/*
 * board.c
 *
 * Board init routines.
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 *
 */

#include "jz4750.h"
#include "../configs.h"

#define is_share_mode() ((INREG32(A_EMC_BCR) & EMC_BCR_BSR_MASK) == EMC_BCR_BSR_SHARE)
#define is_normal_order() (!(INREG32(A_EMC_BCR) & EMC_BCR_PK_SEL))

#define __gpio_as_sdram_32bit_4750()			\
do {						\
	OUTREG32(A_GPIO_PXFUNS(0),0xffffffff);	\
	OUTREG32(A_GPIO_PXSELC(0),0xffffffff);	\
	OUTREG32(A_GPIO_PXPES(0), 0xffffffff);		\
	OUTREG32(A_GPIO_PXFUNS(1), 0x03ff7fff);	\
	OUTREG32(A_GPIO_PXSELC(1), 0x03ff7fff);	\
	OUTREG32(A_GPIO_PXPES(1), 0x03ff7fff);		\
} while (0)
/*
 * UART1_TxD, UART1_RxD
 */
#define __gpio_as_uart1_4750()			\
do {						\
	OUTREG32(A_GPIO_PXFUNS(4), 0x00030000);	\
	OUTREG32(A_GPIO_PXSELC(4), 0x00030000);	\
	OUTREG32(A_GPIO_PXPES(4),  0x00030000);		\
} while (0)

/*
 * UART0_TxD, UART_RxD0
 */
#define __gpio_as_uart0_4750()			\
do {						\
	OUTREG32(A_GPIO_PXFUNS(5), 0x00000030);	\
	OUTREG32(A_GPIO_PXSELC(5), 0x00000030);	\
	OUTREG32(A_GPIO_PXPES(5),  0x00000030);		\
} while (0)


#define __gpio_as_uart2_4750()			\
do {						\
	OUTREG32( A_GPIO_PXFUNS(4) , 0x0c000000);	\
	OUTREG32( A_GPIO_PXSELC(4) , 0x0c000000);	\
	OUTREG32( A_GPIO_PXPES(4) , 0x0c000000);		\
} while (0)

/*
 * UART3_TxD, UART3_RxD
 */
#define __gpio_as_uart3_4750()			\
do {						\
	OUTREG32( A_GPIO_PXFUNS(5) , 0x00030000);	\
	OUTREG32( A_GPIO_PXSELC(5) , 0x00030000);	\
	OUTREG32( A_GPIO_PXPES(5) , 0x00030000);		\
} while (0)

/*
 * D0 ~ D7, CS1#, CLE, ALE, FRE#, FWE#, FRB#, RDWE#/BUFD#
 */
#define __gpio_as_nand_8bit_4750()					\
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

void gpio_init_4750(void)
{
	if(IS_SHARE) {
		CLRREG32(A_EMC_BCR, EMC_BCR_BSR_UNSHARE);
	} else {
		SETREG32(A_EMC_BCR, EMC_BCR_BSR_UNSHARE);
	}
	__gpio_as_sdram_32bit_4750();
	__gpio_as_uart1_4750();
	__gpio_as_uart0_4750();
	__gpio_as_uart2_4750();
	__gpio_as_uart3_4750();
	__gpio_as_nand_8bit_4750();
}

#if 0
void ccpll_init_4750(void)
{
	register unsigned int cfcr, plcr1;
	int n2FR[33] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5, 0, 0, 0, 6, 0, 0, 0,
		7, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
		9
	};
//	int div[5] = {1, 4, 4, 4, 4}; /* divisors of I:S:P:L:M */
	int nf, pllout2;

	cfcr = ~CPM_CPCCR_ECS &
		(n2FR[1] << CPM_CPCCR_CDIV_BIT) | 
		(n2FR[PHM_DIV] << CPM_CPCCR_HDIV_BIT) | 
		(n2FR[PHM_DIV] << CPM_CPCCR_PDIV_BIT) |
		(n2FR[PHM_DIV] << CPM_CPCCR_MDIV_BIT) |
		(n2FR[PHM_DIV] << CPM_CPCCR_LDIV_BIT);

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? CFG_CPU_SPEED : (CFG_CPU_SPEED / 2);

	nf = CFG_CPU_SPEED * 2 / CFG_EXTAL;
	plcr1 = ((nf - 2) << CPM_CPPCR_PLLM_BIT) | /* FD */
		(0 << CPM_CPPCR_PLLN_BIT) |	/* RD=0, NR=2 */
		(0 << CPM_CPPCR_PLLOD_BIT) |    /* OD=0, NO=1 */
		(0x20 << CPM_CPPCR_PLLST_BIT) | /* PLL stable time */
		CPM_CPPCR_PLLEN;                /* enable PLL */          

	/* init PLL */
	REG_CPM_CPCCR = cfcr;
	REG_CPM_CPPCR = plcr1;
} 


int nf, pllout2;

void pll_init_4750(void)
{
	register unsigned int cfcr, plcr1,tmp;
	int n2FR[33] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5, 0, 0, 0, 6, 0, 0, 0,
		7, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
		9
	};
	int div[5] = {1, 3, 3, 3, 3}; /* divisors of I:S:P:L:M */

	cfcr = 	CPM_CPCCR_PCS |
		(n2FR[1] << CPM_CPCCR_CDIV_BIT) | 
		(n2FR[PHM_DIV] << CPM_CPCCR_HDIV_BIT) | 
		(n2FR[PHM_DIV] << CPM_CPCCR_PDIV_BIT) |
		(n2FR[PHM_DIV] << CPM_CPCCR_MDIV_BIT) |
		(n2FR[PHM_DIV] << CPM_CPCCR_LDIV_BIT);

	if (CFG_EXTAL > 16000000)
		cfcr |= CPM_CPCCR_ECS;

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? CFG_CPU_SPEED : (CFG_CPU_SPEED / 2);

	/* Init USB Host clock, pllout2 must be n*48MHz */
//	REG_CPM_UHCCDR = pllout2 / 48000000 - 1;

	nf = CFG_CPU_SPEED * 2 / CFG_EXTAL;
	plcr1 = ((nf - 2) << CPM_CPPCR_PLLM_BIT) | /* FD */
		(0 << CPM_CPPCR_PLLN_BIT) |	/* RD=0, NR=2 */
		(0 << CPM_CPPCR_PLLOD_BIT) |    /* OD=0, NO=1 */
		(0x20 << CPM_CPPCR_PLLST_BIT) | /* PLL stable time */
		CPM_CPPCR_PLLEN;                /* enable PLL */          

	cfcr |= CPM_CPCCR_UCS;           /* set PLL as UDC PHY*/
	tmp = pllout2 / 1000000 / 12 - 1;
	cfcr |= (tmp << CPM_CPCCR_UDIV_BIT);    /* set UDC DIV*/

	/* init PLL */
	REG_CPM_CPCCR = cfcr;
	REG_CPM_CPPCR = plcr1;
}

void sdram_init_4750(void)
{
	register unsigned int dmcr, sdmode, tmp, cpu_clk, mem_clk, ns;
	register unsigned int sdemode; /*SDRAM Extended Mode*/

	unsigned int cas_latency_sdmr[2] = {
		EMC_SDMR_CAS_2,
		EMC_SDMR_CAS_3,
	};

	unsigned int cas_latency_dmcr[2] = {
		1 << EMC_DMCR_TCL_BIT,	/* CAS latency is 2 */
		2 << EMC_DMCR_TCL_BIT	/* CAS latency is 3 */
	};

	int div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	cpu_clk = CFG_CPU_SPEED;
	mem_clk = cpu_clk * div[__cpm_get_cdiv()] / div[__cpm_get_mdiv()];

	/* set REG_EMC_DMAR0 for supporting 128MB sdram on DCS0 */
	REG_EMC_DMAR0 = EMC_DMAR0_BASE | EMC_DMAR_MASK_128_128;

	REG_EMC_BCR = 0;	/* Disable bus release */
	REG_EMC_RTCSR = 0;	/* Disable clock for counting */

	/* Basic DMCR value */
	dmcr = ((SDRAM_ROW-11)<<EMC_DMCR_RA_BIT) |
		((SDRAM_COL-8)<<EMC_DMCR_CA_BIT) |
		(SDRAM_BANK4<<EMC_DMCR_BA_BIT) |
		(SDRAM_BW16<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* SDRAM timimg */
	ns = 1000000000 / mem_clk;
	tmp = SDRAM_TRAS/ns;
	if (tmp < 4) tmp = 4;
	if (tmp > 11) tmp = 11;
	dmcr |= ((tmp-4) << EMC_DMCR_TRAS_BIT);
	tmp = SDRAM_RCD/ns;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << EMC_DMCR_RCD_BIT);
	tmp = SDRAM_TPC/ns;
	if (tmp > 7) tmp = 7;
	dmcr |= (tmp << EMC_DMCR_TPC_BIT);
	tmp = SDRAM_TRWL/ns;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << EMC_DMCR_TRWL_BIT);
	tmp = (SDRAM_TRAS + SDRAM_TPC)/ns;
	if (tmp > 14) tmp = 14;
	dmcr |= (((tmp + 1) >> 1) << EMC_DMCR_TRC_BIT);

	/* SDRAM mode value */
	sdmode = EMC_SDMR_BT_SEQ | 
		 EMC_SDMR_OM_NORMAL |
		 EMC_SDMR_BL_4 | 
		 cas_latency_sdmr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* Stage 1. Precharge all banks by writing SDMR with DMCR.MRSET=0 */
	REG_EMC_DMCR = dmcr;
	REG8(EMC_SDMR0|sdmode) = 0;

	if (CONFIG_MOBILE_SDRAM == 1)
		/* Mobile SDRAM Extended Mode Register */
		sdemode = EMC_SDMR_SET_BA1 | EMC_SDMR_DS_FULL | EMC_SDMR_PRSR_ALL;

	/* Wait for precharge, > 200us */
	tmp = (cpu_clk / 1000000) * 1000;
	while (tmp--);

	/* Stage 2. Enable auto-refresh */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH;

	tmp = SDRAM_TREF/ns;
	tmp = tmp/64 + 1;
	if (tmp > 0xff) tmp = 0xff;
	REG_EMC_RTCOR = tmp;
	REG_EMC_RTCNT = 0;
	REG_EMC_RTCSR = EMC_RTCSR_CKS_64;	/* Divisor is 64, CKO/64 */

	/* Wait for number of auto-refresh cycles */
	tmp = (cpu_clk / 1000000) * 1000;
	while (tmp--);

 	/* Stage 3. Mode Register Set */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET | EMC_DMCR_MBSEL_B0;
	REG8(EMC_SDMR0|sdmode) = 0;

	if (CONFIG_MOBILE_SDRAM == 1)
		REG8(EMC_SDMR0|sdemode) = 0;   	/* Set Mobile SDRAM Extended Mode Register */

	/* Set back to basic DMCR value */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

	/* everything is ok now */
}

void serial_setbrg_4750(void)
{
	volatile u8 *uart_lcr = (volatile u8 *)(UART_BASE + OFF_LCR);
	volatile u8 *uart_dlhr = (volatile u8 *)(UART_BASE + OFF_DLHR);
	volatile u8 *uart_dllr = (volatile u8 *)(UART_BASE + OFF_DLLR);
	u32 baud_div, tmp;

	baud_div = (REG_CPM_CPCCR & CPM_CPCCR_ECS) ?
		(CFG_EXTAL / 32 / CONFIG_BAUDRATE) : (CFG_EXTAL / 16 / CONFIG_BAUDRATE);
	tmp = *uart_lcr;
	tmp |= UART_LCR_DLAB;
	*uart_lcr = tmp;

	*uart_dlhr = (baud_div >> 8) & 0xff;
	*uart_dllr = baud_div & 0xff;

	tmp &= ~UART_LCR_DLAB;
	*uart_lcr = tmp;
}
#endif
