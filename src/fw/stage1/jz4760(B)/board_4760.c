/*
 * board.c
 *
 * Board init routines.
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 *
 */
#include "jz4760.h"
#include "configs.h"
#include "board_4760.h"

/*
 * SD0 ~ SD7, SA0 ~ SA5, CS2#, RD#, WR#, WAIT#	
 */
#define __gpio_as_nor()							\
do {								        \
	/* SD0 ~ SD7, RD#, WR#, CS2#, WAIT# */				\
	REG_GPIO_PXFUNS(0) = 0x084300ff;				\
	REG_GPIO_PXTRGC(0) = 0x084300ff;				\
	REG_GPIO_PXSELC(0) = 0x084300ff;				\
	REG_GPIO_PXPES(0) = 0x084300ff;					\
	/* SA0 ~ SA5 */							\
	REG_GPIO_PXFUNS(1) = 0x0000003f;				\
	REG_GPIO_PXTRGC(1) = 0x0000003f;				\
	REG_GPIO_PXSELC(1) = 0x0000003f;				\
	REG_GPIO_PXPES(1) = 0x0000003f;					\
} while (0)

void gpio_init_4760()
{
	//__gpio_as_uart0();
	__gpio_as_uart1();
	//__gpio_as_uart2();
	//__gpio_as_uart3();
	
#ifdef CONFIG_FPGA // if the delay isn't added on FPGA, the first line that uart to print will not be normal. 
	__gpio_as_nor();
	{
		volatile int i=1000;
		while(i--);
	}
#endif
	__gpio_as_nand_8bit(1);
}

#define MHZ (1000 * 1000)
static inline unsigned int pll_calc_m_n_od(unsigned int speed, unsigned int xtal)
{
	const int pll_m_max = 0x7f, pll_m_min = 4;
	const int pll_n_max = 0x0f, pll_n_min = 2;

	int od[] = {1, 2, 4, 8};
	int min_od = 0;

	unsigned int plcr_m_n_od = 0;
	unsigned int distance;
	unsigned int tmp, raw;

	int i, j, k;
	int m, n;

	distance = 0xFFFFFFFF;

	for (i = 0; i < sizeof (od) / sizeof(int); i++) {
		/* Limit: 500MHZ <= CLK_OUT * OD <= 1500MHZ */
//		if (min_od != 0)
//			break;
		if ((speed * od[i]) < 500 * MHZ || (speed * od[i]) > 1500 * MHZ)
			continue;
		for (k = pll_n_min; k <= pll_n_max; k++) {
			n = k;
			
			/* Limit: 1MHZ <= XIN/N <= 50MHZ */
			if ((xtal / n) < (1 * MHZ))
				break;
			if ((xtal / n) > (15 * MHZ))
				continue;

			for (j = pll_m_min; j <= pll_m_max; j++) {
				m = j*2;

				raw = xtal * m / n;
				tmp = raw / od[i];

				tmp = (tmp > speed) ? (tmp - speed) : (speed - tmp);

				if (tmp < distance) {
					distance = tmp;
					
					plcr_m_n_od = (j << CPM_CPPCR_PLLM_BIT) 
						| (k << CPM_CPPCR_PLLN_BIT)
						| (i << CPM_CPPCR_PLLOD_BIT);

					if (!distance) {	/* Match. */
//						serial_puts("right value");
						return plcr_m_n_od;
					}
				}
			}
			min_od = od[i];
		}
	}
	return plcr_m_n_od;
}

/* TODO: pll_init() need modification. */
void pll_init_4760(void)
{
/*
 * Init PLL.
 *
 * PLL output clock = EXTAL * NF / (NR * NO)
 *
 * NF = FD + 2, NR = RD + 2
 * NO = 1 (if OD = 0), NO = 2 (if OD = 1 or 2), NO = 4 (if OD = 3)
 */
 
	register unsigned int cfcr, plcr1, plcr2;
	int n2FR[9] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5
	};

	/** divisors, 
	 *  for jz4760 ,I:H:H2:P:M:S.
	 *  DIV should be one of [1, 2, 3, 4, 6, 8]
         */
	//int div[6] = {1, 2, 4, 4, 4, 4};
	int div[6] = {1, 2, 2, 2, 2, 2};
	int pllout2;

	cfcr = 	CPM_CPCCR_PCS |
		(n2FR[div[0]] << CPM_CPCCR_CDIV_BIT) | 
		(n2FR[div[1]] << CPM_CPCCR_HDIV_BIT) | 
		(n2FR[div[2]] << CPM_CPCCR_H2DIV_BIT) |
		(n2FR[div[3]] << CPM_CPCCR_PDIV_BIT) |
		(n2FR[div[4]] << CPM_CPCCR_MDIV_BIT) |
		(n2FR[div[5]] << CPM_CPCCR_SDIV_BIT);

	// write REG_DDRC_CTRL 8 times to clear ddr fifo
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;
	REG_DDRC_CTRL = 0;

	if (CFG_EXTAL > 16000000)
		cfcr |= CPM_CPCCR_ECS;
	else
		cfcr &= ~CPM_CPCCR_ECS;

	cfcr |= CPM_CPCCR_MEM;

	//cfcr &= ~CPM_CPCCR_MEM;   //mddr sdram

	cfcr |= CPM_CPCCR_CE;

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? CFG_CPU_SPEED : (CFG_CPU_SPEED / 2);

	plcr1 = CPCCR_M_N_OD;
	plcr1 |= (0x20 << CPM_CPPCR_PLLST_BIT)	/* PLL stable time */
		 | CPM_CPPCR_PLLEN;             /* enable PLL */

	/*
	 * Init USB Host clock, pllout2 must be n*48MHz
	 * For JZ4760 UHC - River.
	 */
	REG_CPM_UHCCDR = pllout2 / 48000000 - 1;
	/* init PLL */
	REG_CPM_CPCCR = cfcr;
	REG_CPM_CPPCR = plcr1;

	/*wait for pll output stable ...*/
	while (!(REG_CPM_CPPCR & CPM_CPPCR_PLLS));

    /* set CPM_CPCCR_MEM only for ddr1 or ddr2 */
    plcr2 = CPCCR1_M_N_OD | CPM_CPPCR1_PLL1EN;

    /* init PLL_1 , source clock is extal clock */
    REG_CPM_CPPCR1 = plcr2;

    __cpm_enable_pll_change();

	/*wait for pll_1 output stable ...*/
    while (!(REG_CPM_CPPCR1 & CPM_CPPCR1_PLL1S));
/*
	serial_puts("REG_CPM_CPCCR = ");
	serial_put_hex(REG_CPM_CPCCR);
	serial_puts("REG_CPM_CPPCR = ");
	serial_put_hex(REG_CPM_CPPCR);
*/
}

void serial_setbrg_4760(void)
{
	volatile u8 *uart_lcr = (volatile u8 *)(UART_BASE + OFF_LCR);
	volatile u8 *uart_dlhr = (volatile u8 *)(UART_BASE + OFF_DLHR);
	volatile u8 *uart_dllr = (volatile u8 *)(UART_BASE + OFF_DLLR);
	u32 baud_div, tmp;

//	baud_div = (REG_CPM_CPCCR & CPM_CPCCR_ECS) ?
//		(CFG_EXTAL / 32 / CONFIG_BAUDRATE) : (CFG_EXTAL / 16 / CONFIG_BAUDRATE);
	
	baud_div = (CFG_EXTAL / 16 / 57600);

	tmp = *uart_lcr;
	tmp |= UART_LCR_DLAB;
	*uart_lcr = tmp;

	*uart_dlhr = (baud_div >> 8) & 0xff;
	*uart_dllr = baud_div & 0xff;

	tmp &= ~UART_LCR_DLAB;
	*uart_lcr = tmp;
}
