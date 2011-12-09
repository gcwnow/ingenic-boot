/*
 * Jz4770 ddr routines
 *
 *  Copyright (c) 2006
 *  Ingenic Semiconductor, <jlwei@ingenic.cn>
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

#include "jz4770.h"
#include "configs.h"
#include "board_4770.h"
#include "sdram.h"

//#define DEBUG
#undef DEBUG

static long int initdram(int board_type)
{
	u32 dmcr;
	u32 rows, cols, dw, banks;
	unsigned long size;

	dmcr = REG_EMC_DMCR;
	rows = 11 + ((dmcr & EMC_DMCR_RA_MASK) >> EMC_DMCR_RA_BIT);
	cols = 8 + ((dmcr & EMC_DMCR_CA_MASK) >> EMC_DMCR_CA_BIT);
	dw = (dmcr & EMC_DMCR_BW) ? 2 : 4;
	banks = (dmcr & EMC_DMCR_BA) ? 4 : 2;

	size = (1 << (rows + cols)) * dw * banks;
	size *= CONFIG_NR_DRAM_BANKS;

	return size;
}

#ifdef DEBUG

#define MEM_BASE  0xa0000000		/*un-cached*/

static void dump_jz_dma_channel(unsigned int dmanr)
{

	if (dmanr > MAX_DMA_NUM)
		return;

	dprintf("DMA%d Registers:\n", dmanr);
	dprintf("  DMACR  = 0x%08x\n", REG_DMAC_DMACR(dmanr/HALF_DMA_NUM));
	dprintf("  DSAR   = 0x%08x\n", REG_DMAC_DSAR(dmanr));
	dprintf("  DTAR   = 0x%08x\n", REG_DMAC_DTAR(dmanr));
	dprintf("  DTCR   = 0x%08x\n", REG_DMAC_DTCR(dmanr));
	dprintf("  DRSR   = 0x%08x\n", REG_DMAC_DRSR(dmanr));
	dprintf("  DCCSR  = 0x%08x\n", REG_DMAC_DCCSR(dmanr));
	dprintf("  DCMD  = 0x%08x\n", REG_DMAC_DCMD(dmanr));
	dprintf("  DDA  = 0x%08x\n", REG_DMAC_DDA(dmanr));
	dprintf("  DMADBR = 0x%08x\n", REG_DMAC_DMADBR(dmanr/HALF_DMA_NUM));
}

static void sdmr_regs_print(void)
{
	dprintf("\nSDMR REGS:-----------------------------\n");
	dprintf("REG_EMC_BCR \t\t= 0x%08x\n", REG_EMC_BCR);
	dprintf("REG_EMC_DMCR \t\t= 0x%08x\n", REG_EMC_DMCR);
	dprintf("REG_EMC_RTCSR \t\t= 0x%08x\n", REG_EMC_RTCSR);
	dprintf("REG_EMC_RTCNT \t\t= 0x%08x\n", REG_EMC_RTCNT);
	dprintf("REG_EMC_RTCOR \t\t= 0x%08x\n", REG_EMC_RTCOR);
	dprintf("REG_EMC_DMAR0 \t\t= 0x%08x\n", REG_EMC_DMAR0);
	dprintf("REG_EMC_DMAR1 \t\t= 0x%08x\n", REG_EMC_DMAR1);
	dprintf("---------------------------------------\n");
}

static void jzmemset(void *dest,int ch,int len)
{
	unsigned int *d = (unsigned int *)dest;
	int i;
	int wd;

	wd = (ch << 24) | (ch << 16) | (ch << 8) | ch;

	for(i = 0;i < len / 32;i++)
	{
		*d++ = wd;
		*d++ = wd;
		*d++ = wd;
		*d++ = wd;
		*d++ = wd;
		*d++ = wd;
		*d++ = wd;
		*d++ = wd;
	}
}

static int sdram_dma_test(int print_flag) {
	int i, err = 0, banks;
	int times;
	unsigned int addr, DDR_DMA0_SRC, DDR_DMA0_DST, DDR_DMA1_SRC, DDR_DMA1_DST;
	volatile unsigned int tmp;
	register unsigned int cpu_clk;
	long int memsize, banksize, testsize;

	banks = (SDRAM_BANK4 ? 4 : 2) *(CONFIG_NR_DRAM_BANKS);

	memsize = initdram(0);
	//dprintf("memsize = 0x%08x\n", memsize);
	banksize = memsize/banks;
	testsize = 4096;

	for(times = 0; times < banks; times++) {
		DDR_DMA0_SRC = DDR_DMA_BASE + banksize*times;
		DDR_DMA0_DST = DDR_DMA_BASE + banksize*(times+1) - testsize;

		cpu_clk = CFG_CPU_SPEED;

		addr = DDR_DMA0_SRC;
		//dprintf("DDR_DMA0_SRC =  0x%08x, DDR_DMA0_DST = 0x%08x\n", DDR_DMA0_SRC, DDR_DMA0_DST);
		for (i = 0; i < testsize; i += 4) {
//			serial_put_hex(addr+i);
//			serial_put_hex(i);
			*(volatile unsigned int *)(addr + i) = gen_verify_data(i);

		}

		//dprintf("\nWrite finish\n");
		REG_MDMAC_DMACR = 0;

		/* Init target buffer */
		jzmemset((void *)DDR_DMA0_DST, 0, testsize);
		dma_nodesc_test(0, DDR_DMA0_SRC, DDR_DMA0_DST, testsize);

		REG_MDMAC_DMACR = DMAC_DMACR_DMAE; /* global DMA enable bit */

		while(REG_MDMAC_DTCR(0));

		tmp = (cpu_clk / 1000000) * 1;
		while (tmp--);
/*
		serial_puts("DDR_DMA0_SRC = ");
		serial_put_hex(DDR_DMA0_SRC);
		serial_puts("DDR_DMA0_DST = ");
		serial_put_hex(DDR_DMA0_DST);
		serial_puts("testsize = ");
		serial_put_hex(testsize);
*/
		err = dma_check_result((void *)DDR_DMA0_SRC, (void *)DDR_DMA0_DST, testsize,print_flag);
		
		REG_MDMAC_DCCSR(0) &= ~DMAC_DCCSR_EN;  /* disable DMA */

		if(err == 0) {
//			serial_puts("pass\n");
//			serial_put_hex(times);
		}
		else {
//			serial_puts("failed\n");
//			serial_put_hex(times);
		}

		if (err != 0) {
			return err;
		}
	}
	return err;
}

static void mem_test(void)
{
	volatile unsigned int *ptr;

	long int memsize;
	memsize = initdram(0);
	/*write data to bank0~3*/
	dprintf("Write data to SDRAM\n");
	for (ptr = (volatile unsigned int *)(MEM_BASE); (unsigned int)ptr < MEM_BASE + memsize; ptr++) {
		*ptr = (unsigned int)ptr;
		if (*ptr != (unsigned int)ptr) {
			dprintf("\nERROR: ");
			dprintf("--0x%08x\t", (unsigned int)ptr);
			dprintf("--0x%08x\n", *ptr);
		}
	}
	dprintf("0x%08x\n", (unsigned int)ptr);
	for (ptr = (volatile unsigned int *)(MEM_BASE); (unsigned int)ptr < MEM_BASE + memsize; ptr++) {
		if (*ptr != (unsigned int)ptr) {
			dprintf("\n SDRAM ERROR\n");
			dprintf("0x%08x\t", (unsigned int)ptr);
			dprintf("0x%08x\n", *ptr);
		}
	}
	dprintf("0x%08x\n", (unsigned int)ptr);
	dprintf("Read and compare finish\n");
	/*mobile test finish*/
}

static void sdram_add_test(int new_freq)
{
	register unsigned int dmcr, sdmode, tmp, cpu_clk, mem_clk, ns;

	unsigned int cas_latency_sdmr[2] = {
		EMC_SDMR_CAS_2,
		EMC_SDMR_CAS_3,
	};

	unsigned int cas_latency_dmcr[2] = {
		1 << EMC_DMCR_TCL_BIT,	/* CAS latency is 2 */
		2 << EMC_DMCR_TCL_BIT	/* CAS latency is 3 */
	};

	int div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	cpu_clk = new_freq;
	mem_clk = cpu_clk * div[__cpm_get_cdiv()] / div[__cpm_get_mdiv()];

	REG_EMC_RTCSR = EMC_RTCSR_CKS_DISABLE;
	REG_EMC_RTCOR = 0;
	REG_EMC_RTCNT = 0;

	/* Basic DMCR register value. */
	dmcr = ((SDRAM_ROW-11)<<EMC_DMCR_RA_BIT) |
		((SDRAM_COL-8)<<EMC_DMCR_CA_BIT) |
		(SDRAM_BANK4<<EMC_DMCR_BA_BIT) |
		(SDRAM_BW16<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* SDRAM timimg parameters */
	ns = 1000000000 / mem_clk;

#if 0
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
#else
	dmcr |= 0xfffc;
#endif

	/* First, precharge phase */
	REG_EMC_DMCR = dmcr;

	/* Set refresh registers */
	tmp = SDRAM_TREF/ns;
	tmp = tmp/64 + 1;
	if (tmp > 0xff) tmp = 0xff;

	REG_EMC_RTCOR = tmp;
	REG_EMC_RTCSR = EMC_RTCSR_CKS_64;	/* Divisor is 64, CKO/64 */

	/* SDRAM mode values */
	sdmode = EMC_SDMR_BT_SEQ |
		 EMC_SDMR_OM_NORMAL |
		 EMC_SDMR_BL_4 |
		 cas_latency_sdmr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* precharge all chip-selects */
	REG8(EMC_SDMR0|sdmode) = 0;

	/* wait for precharge, > 200us */
	tmp = (cpu_clk / 1000000) * 300;
	while (tmp--);

	/* enable refresh and set SDRAM mode */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

	/* write sdram mode register for each chip-select */
	REG8(EMC_SDMR0|sdmode) = 0;

	/* everything is ok now */
}

#endif /* DEBUG */

void sdram_init_4770(void)
{
	register unsigned int dmcr, sdmode, tmp, cpu_clk, mem_clk, ns;

#ifdef CONFIG_MOBILE_SDRAM
	register unsigned int sdemode; /*SDRAM Extended Mode*/
#endif
	unsigned int cas_latency_sdmr[2] = {
		EMC_SDMR_CAS_2,
		EMC_SDMR_CAS_3,
	};

	unsigned int cas_latency_dmcr[2] = {
		1 << EMC_DMCR_TCL_BIT,	/* CAS latency is 2 */
		2 << EMC_DMCR_TCL_BIT	/* CAS latency is 3 */
	};

#ifndef CONFIG_FPGA
	int div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
#endif

	REG_DDRC_CFG = 0x80000000;

#ifdef DEBUG
	sdmr_regs_print();
#endif
	cpu_clk = CFG_CPU_SPEED;

#if defined(CONFIG_FPGA)
	mem_clk = CFG_EXTAL / CFG_DIV;
#else
	mem_clk = cpu_clk * div[__cpm_get_cdiv()] / div[__cpm_get_mdiv()];
#endif

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

	/* Precharge Bank1 SDRAM */
#if CONFIG_NR_DRAM_BANKS == 2
	REG_EMC_DMCR = dmcr | EMC_DMCR_MBSEL_B1;
	REG8(EMC_SDMR0|sdmode) = 0;
#endif

#ifdef CONFIG_MOBILE_SDRAM
	/* Mobile SDRAM Extended Mode Register */
	sdemode = EMC_SDMR_SET_BA1 | EMC_SDMR_DS_FULL | EMC_SDMR_PRSR_ALL;
#endif

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

#ifdef CONFIG_MOBILE_SDRAM
	REG8(EMC_SDMR0|sdemode) = 0;   	/* Set Mobile SDRAM Extended Mode Register */
#endif

#if CONFIG_NR_DRAM_BANKS == 2
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET | EMC_DMCR_MBSEL_B1;
	REG8(EMC_SDMR0|sdmode) = 0;	/* Set Bank1 SDRAM Register */


#ifdef CONFIG_MOBILE_SDRAM
	REG8(EMC_SDMR0|sdemode) = 0;	/* Set Mobile SDRAM Extended Mode Register */
#endif

#endif   /*CONFIG_NR_DRAM_BANKS == 2*/

	/* Set back to basic DMCR value */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

	/* bank_size: 32M 64M 128M ... */
	unsigned int bank_size = initdram(0)/ CONFIG_NR_DRAM_BANKS;
	unsigned int mem_base0, mem_base1, mem_mask;

	mem_base0 = EMC_MEM_PHY_BASE >> EMC_MEM_PHY_BASE_SHIFT;
	mem_base1 = ((EMC_MEM_PHY_BASE + bank_size) >> EMC_MEM_PHY_BASE_SHIFT);
	mem_mask = EMC_DMAR_MASK_MASK &
		(~(((bank_size) >> EMC_MEM_PHY_BASE_SHIFT)-1)&EMC_DMAR_MASK_MASK);

	REG_EMC_DMAR0 = (mem_base0 << EMC_DMAR_BASE_BIT) | mem_mask;
	REG_EMC_DMAR1 = (mem_base1 << EMC_DMAR_BASE_BIT) | mem_mask;

	/* everything is ok now */
#ifdef DEBUG
	sdmr_regs_print();
	sdram_dma_test(1);
	mem_test();
#endif
}
