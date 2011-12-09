/*
 * Jz4760 ddr routines
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
#include "board_4760.h"
#include "jz4760.h"
//#include "configs.h"
//#include "board_4760.h"
#include "ddr2.h"

//#define DEBUG
#undef DEBUG

#define dprintf

#ifdef DEBUG
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
	dprintf("  DCMD   = 0x%08x\n", REG_DMAC_DCMD(dmanr));
	dprintf("  DDA    = 0x%08x\n", REG_DMAC_DDA(dmanr));
	dprintf("  DMADBR = 0x%08x\n", REG_DMAC_DMADBR(dmanr/HALF_DMA_NUM));
}

static void ddrc_regs_print(void)
{
	dprintf("\nDDRC REGS:\n");
	dprintf("REG_DDRC_ST \t\t= 0x%08x\n", REG_DDRC_ST);
	dprintf("REG_DDRC_CFG \t\t= 0x%08x\n", REG_DDRC_CFG);
	dprintf("REG_DDRC_CTRL \t\t= 0x%08x\n", REG_DDRC_CTRL);
	dprintf("REG_DDRC_LMR \t\t= 0x%08x\n", REG_DDRC_LMR);
	dprintf("REG_DDRC_TIMING1 \t= 0x%08x\n", REG_DDRC_TIMING1);
	dprintf("REG_DDRC_TIMING2 \t= 0x%08x\n", REG_DDRC_TIMING2);
	dprintf("REG_DDRC_REFCNT \t\t= 0x%08x\n", REG_DDRC_REFCNT);
	dprintf("REG_DDRC_DQS \t\t= 0x%08x\n", REG_DDRC_DQS);
	dprintf("REG_DDRC_DQS_ADJ \t= 0x%08x\n", REG_DDRC_DQS_ADJ);
	dprintf("REG_DDRC_MMAP0 \t\t= 0x%08x\n", REG_DDRC_MMAP0);
	dprintf("REG_DDRC_MMAP1 \t\t= 0x%08x\n", REG_DDRC_MMAP1);
	dprintf("REG_DDRC_MDELAY \t\t= 0x%08x\n", REG_DDRC_MDELAY);
}

#define DDR_16M (16 * 1024 * 1024)
static void map_ddr_memory(unsigned long vbase, unsigned long pbase, unsigned long meg) {
	int i, entrys, pfn0, pfn1, vadd, padd;
	unsigned long entrylo0, entrylo1, entryhi, pagemask;

	entrys = meg / 16;
	pagemask = PM_16M;

	for (i = 0; i < entrys; i+=2) {
		vadd = vbase + i * DDR_16M;
		padd = pbase + i * DDR_16M;
		entryhi = vadd;
		pfn0 = (padd >> 6) | (2 << 3);
		pfn1 = (padd + DDR_16M) >> 6 | (2 << 3);
		entrylo0 = (pfn0 | 0x6) & 0x3ffffff;
		entrylo1 = (pfn1 | 0x6) & 0x3ffffff;
		add_wired_entry(entrylo0, entrylo1, entryhi, pagemask);
	}
}
#endif /* DEBUG */

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

static int initdram(int board_type)
{
	u32 ddr_cfg;
	u32 rows, cols, dw, banks;
	unsigned long size;
	ddr_cfg = REG_DDRC_CFG;
	rows = 12 + ((ddr_cfg & DDRC_CFG_ROW_MASK) >> DDRC_CFG_ROW_BIT);
	cols = 8 + ((ddr_cfg & DDRC_CFG_COL_MASK) >> DDRC_CFG_COL_BIT);
	
	dw = (ddr_cfg & DDRC_CFG_DW) ? 4 : 2;
	banks = (ddr_cfg & DDRC_CFG_BA) ? 8 : 4;
	
	size = (1 << (rows + cols)) * dw * banks;
	size *= (DDR_CS1EN + DDR_CS0EN);

	return size;
}

static unsigned int gen_verify_data(unsigned int i)
{
	int data = i/4;
	if (data & 0x1)
	i = data*0x5a5a5a5a;
	else
	i = data*0xa5a5a5a5;
	return i;
} 

static int dma_check_result(void *src, void *dst, int size,int print_flag)
{
	unsigned int addr1, addr2, i, err = 0;
	unsigned int data_expect,dsrc,ddst; 
	
	addr1 = (unsigned int)src;
	addr2 = (unsigned int)dst;

	for (i = 0; i < size; i += 4) {
		data_expect = gen_verify_data(i);
		dsrc = REG32(addr1);
		ddst = REG32(addr2);
		if ((dsrc != data_expect) || (ddst != data_expect)) {
#if 1
			//serial_puts("wrong data at34:");
			serial_put_hex(addr2);
			serial_puts("data:");
			serial_put_hex(data_expect);
			serial_puts("src");
			serial_put_hex(dsrc);
			serial_puts("dst");
			serial_put_hex(ddst);
#endif
			err = 1;
			if(!print_flag)
				return 1;
		}

		addr1 += 4;
		addr2 += 4;
	}
//	serial_puts("check passed!");
	return err;
}

static void dma_nodesc_test(int dma_chan, int dma_src_addr, int dma_dst_addr, int size)
{
	int dma_src_phys_addr, dma_dst_phys_addr;

	/* Allocate DMA buffers */
	dma_src_phys_addr = dma_src_addr & (~0xa0000000);
	dma_dst_phys_addr = dma_dst_addr & (~0xa0000000);

	/* Init DMA module */
	REG_DMAC_DCCSR(dma_chan) = 0;
	REG_DMAC_DRSR(dma_chan) = DMAC_DRSR_RS_AUTO;
	REG_DMAC_DSAR(dma_chan) = dma_src_phys_addr;
	REG_DMAC_DTAR(dma_chan) = dma_dst_phys_addr;
	REG_DMAC_DTCR(dma_chan) = size / 32;
	REG_DMAC_DCMD(dma_chan) = DMAC_DCMD_SAI | DMAC_DCMD_DAI | DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 | DMAC_DCMD_DS_32BYTE;
	REG_DMAC_DCCSR(dma_chan) = DMAC_DCCSR_NDES | DMAC_DCCSR_EN;
}

static void testallmem()
{
	unsigned int i,d;
	unsigned int *dat;
	dat = (unsigned int *)0xa0000000;
	for(i = 0; i < 64*1024*1024;i+=4)
	{
		*dat = i;
		dat++;
	}
	
	dat = (unsigned int *)0xa0000000;
	for(i = 0; i < 64*1024*1024;i+=4)
	{
		d = *dat;
		if(d != i)
		{
			serial_puts("errdata:\n");
			serial_puts("expect:\n");
			serial_put_hex(i);
			serial_put_hex(d);
		}
		dat++;
	}
}

#define DDR_DMA_BASE  (0xa0000000)		/*un-cached*/
#define DMA_CHANNEL0_EN
//#define DMA_CHANNEL1_EN
static int ddr_dma_test(int print_flag) 
{
	int i, err = 0, banks;
	int times;
	unsigned int addr, DDR_DMA0_SRC, DDR_DMA0_DST, DDR_DMA1_SRC, DDR_DMA1_DST;
	volatile unsigned int tmp;
	register unsigned int cpu_clk;
	long int memsize, banksize, testsize;
	REG_DMAC_DMACKE(0) = 0x3f;
	REG_DMAC_DMACKE(1) = 0x3f;

	banks = (DDR_BANK8 ? 8 : 4) *(DDR_CS0EN + DDR_CS1EN);
	memsize = initdram(0);
	banksize = memsize/banks;
	testsize = 4096;

	for(times = 0; times < banks; times++) {
		DDR_DMA0_SRC = DDR_DMA_BASE + banksize*times;
		DDR_DMA0_DST = DDR_DMA_BASE + banksize*(times+1) - testsize;
		DDR_DMA1_SRC = DDR_DMA_BASE + banksize*(banks - 1) + testsize*2;
		DDR_DMA1_DST = DDR_DMA_BASE + banksize*(banks - 1) + testsize*3;

		cpu_clk = CFG_CPU_SPEED;

#ifdef DMA_CHANNEL0_EN
		addr = DDR_DMA0_SRC;
		for (i = 0; i < testsize; i += 4) {
			*(volatile unsigned int *)(addr + i) = gen_verify_data(i);		
		}
#endif
#ifdef DMA_CHANNEL1_EN
		addr = DDR_DMA1_SRC;
		for (i = 0; i < testsize; i += 4) {
			*(volatile unsigned int *)addr = gen_verify_data(i);
			addr += 4;
		}
#endif

		REG_DMAC_DMACR(0) = 0;
//		REG_DMAC_DMACR(1) = 0;
	/* Init target buffer */
#ifdef DMA_CHANNEL0_EN
		jzmemset((void *)DDR_DMA0_DST, 0, testsize);
		dma_nodesc_test(0, DDR_DMA0_SRC, DDR_DMA0_DST, testsize);
#endif
#ifdef DMA_CHANNEL1_EN
//		jzmemset((void *)DDR_DMA1_DST, 0, testsize);
//		dma_nodesc_test(1, DDR_DMA1_SRC, DDR_DMA1_DST, testsize);
#endif
		REG_DMAC_DMACR(0) = DMAC_DMACR_DMAE; /* global DMA enable bit */
//		REG_DMAC_DMACR(1) = DMAC_DMACR_DMAE; /* global DMA enable bit */
//		while(REG_DMAC_DTCR(0) || REG_DMAC_DTCR(1));
		while(REG_DMAC_DTCR(0));
		tmp = (cpu_clk / 1000000) * 1;
		while (tmp--);

#ifdef DMA_CHANNEL0_EN
		err = dma_check_result((void *)DDR_DMA0_SRC, (void *)DDR_DMA0_DST, testsize,print_flag);

		REG_DMAC_DCCSR(0) &= ~DMAC_DCCSR_EN;  /* disable DMA */

		if(err == 0) {
//			serial_puts("pass\n");
			serial_put_hex(times);
		}
		else {
//			serial_puts("failed\n");
			serial_put_hex(times);
		}

		if (err != 0) {
#ifdef DMA_CHANNEL1_EN
			REG_DMAC_DCCSR(1) &= ~DMAC_DCCSR_EN;  /* disable DMA */
#endif
			return err;
		}
#endif

#ifdef DMA_CHANNEL1_EN
//		err += dma_check_result((void *)DDR_DMA1_SRC, (void *)DDR_DMA1_DST, testsize);
//		REG_DMAC_DCCSR(1) &= ~DMAC_DCCSR_EN;  /* disable DMA */
#endif
	}
	return err;
}

static void ddr_mem_init(int msel, int hl, int tsel, int arg)
{
	volatile int tmp_cnt;
	register unsigned int cpu_clk, ddr_twr;
	register unsigned int ddrc_cfg_reg=0, init_ddrc_mdelay=0;

	cpu_clk = CFG_CPU_SPEED;

	serial_puts("\nddr2-\n");
	ddrc_cfg_reg = DDRC_CFG_TYPE_DDR2 | (DDR_ROW-12)<<10
		| (DDR_COL-8)<<8 | DDR_CS1EN<<7 | DDR_CS0EN<<6
		| ((DDR_CL-1) | 0x8)<<2 | DDR_BANK8<<1 | DDR_DW32;

	ddrc_cfg_reg |= DDRC_CFG_MPRT;

#if defined(CONFIG_FPGA)
	init_ddrc_mdelay= tsel<<18 | msel<<16 | hl<<15;
#else
	init_ddrc_mdelay= tsel<<18 | msel<<16 | hl<<15 | arg << 14;
#endif
	ddr_twr = ((REG_DDRC_TIMING1 & DDRC_TIMING1_TWR_MASK) >> DDRC_TIMING1_TWR_BIT) + 1;
	REG_DDRC_CFG     = ddrc_cfg_reg;

	REG_DDRC_MDELAY = init_ddrc_mdelay  | 1 << 6;
	/***** init ddrc registers & ddr memory regs ****/
	/* AR: auto refresh */
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;
	/* Wait for DDR_tRP */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* Wait for number of auto-refresh cycles */
	tmp_cnt = (cpu_clk / 1000000) * 10;
	while (tmp_cnt--);

	/* Set CKE High */
	REG_DDRC_CTRL = DDRC_CTRL_CKE; // ?

	/* Wait for number of auto-refresh cycles */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* PREA */
	REG_DDRC_LMR =  DDRC_LMR_CMD_PREC | DDRC_LMR_START; //0x1;

	/* Wait for DDR_tRP */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* EMR2: extend mode register2 */
	REG_DDRC_LMR = DDRC_LMR_BA_EMRS2 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;//0x221;

	/* EMR3: extend mode register3 */
	REG_DDRC_LMR = DDRC_LMR_BA_EMRS3 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;//0x321;

	/* EMR1: extend mode register1 */
	REG_DDRC_LMR = (DDR_EMRS1_DQS_DIS << 16) | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* wait DDR_tMRD */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* MR - DLL Reset A1A0 burst 2 */
	REG_DDRC_LMR = ((ddr_twr-1)<<9 | DDR2_MRS_DLL_RST | DDR_CL<<4 | DDR_MRS_BL_4)<< 16
		| DDRC_LMR_BA_MRS | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* wait DDR_tMRD */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* PREA */
	REG_DDRC_LMR =  DDRC_LMR_CMD_PREC | DDRC_LMR_START; //0x1;

	/* Wait for DDR_tRP */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* AR: auto refresh */
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;
	/* Wait for DDR_tRP */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;

	/* Wait for DDR_tRP */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* MR - DLL Reset End */
	REG_DDRC_LMR = ((ddr_twr-1)<<9 | DDR_CL<<4 | DDR_MRS_BL_4)<< 16
		| DDRC_LMR_BA_MRS | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* wait 200 tCK */
	tmp_cnt = (cpu_clk / 1000000) * 2;
	while (tmp_cnt--);

	/* EMR1 - OCD Default */
	REG_DDRC_LMR = (DDR_EMRS1_DQS_DIS | DDR_EMRS1_OCD_DFLT) << 16
		| DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* EMR1 - OCD Exit */
	REG_DDRC_LMR = (DDR_EMRS1_DQS_DIS << 16) | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* wait DDR_tMRD */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);
}

#define DEF_DDR_CVT 0
#define DDR_USE_FIRST_ARGS 0
unsigned int testall = 0;
/* DDR sdram init */

void sdram_init_4760(unsigned int cpu_id)
{
	serial_puts("REG_CPM_CPCCR = ");
	serial_put_hex(REG_CPM_CPCCR);
	serial_puts("REG_CPM_CPPCR = ");
	serial_put_hex(REG_CPM_CPPCR);
	//add driver power
	if(cpu_id == 0x352920e4)
	{
		serial_puts("sdram_init_4760  -b- ");
		REG_DMAC_DMACKES(0) = 0x3f;
		REG_DMAC_DMACKES(1) = 0x3f;
	}
	else
	{
		serial_puts("sdram_init_4760");
		REG_EMC_PMEMPS2 |= (3 << 18);
		REG_DMAC_DMACKE(0) = 0x3f;
		REG_DMAC_DMACKE(1) = 0x3f;
	}
	int i, num = 0, tsel = 0, msel, hl;
#if defined(CONFIG_FPGA)
	int cvt = DEF_DDR_CVT, cvt_cnt0 = 0, cvt_cnt1 = 1, max = 0, max0 = 0, max1 = 0, min0 = 0, min1 = 0, tsel0 = 0, tsel1 = 0;
	struct ddr_delay_sel_t {
	int msel;
	int hl;
};
#endif
	volatile unsigned int tmp_cnt;
	register unsigned int tmp, cpu_clk, mem_clk, ddr_twr, ns, ns_int;
	register unsigned int ddrc_timing1_reg=0, ddrc_timing2_reg=0, init_ddrc_refcnt=0, init_ddrc_dqs=0, init_ddrc_ctrl=0;
#if defined(CONFIG_FPGA)
	struct ddr_delay_sel_t ddr_delay_sel[] = {
		{0, 1}, {0, 0},	{1, 1}, {1, 0},
		{2, 1}, {2, 0},	{3, 1}, {3, 0}
	};
#endif
	register unsigned int memsize, ddrc_mmap0_reg, ddrc_mmap1_reg, mem_base0, mem_base1, mem_mask0, mem_mask1;
	register unsigned int pll_out,div_mem;
	testall = 0; 
#ifdef DEBUG
	ddrc_regs_print();
#endif

	cpu_clk = CFG_CPU_SPEED;
	serial_puts("cpu_clk = ");
	serial_put_hex(cpu_clk);

#if defined(CONFIG_FPGA)
	mem_clk = CFG_EXTAL / CFG_DIV;
#else
	mem_clk = __cpm_get_mclk();
#endif

	serial_puts("mem_clk = ");
	serial_put_hex(mem_clk);
#if defined(CONFIG_FPGA)
	ns = 7;
#else
	ns = 1000000000 / mem_clk; /* ns per tck ns <= real value */
#endif

	/* ACTIVE to PRECHARGE command period */
	tmp = (DDR_tRAS%ns == 0) ? (DDR_tRAS/ns) : (DDR_tRAS/ns+1);
	if (tmp < 1) tmp = 1;
	if (tmp > 31) tmp = 31;
	ddrc_timing1_reg = ((tmp/2) << DDRC_TIMING1_TRAS_BIT);

	/* READ to PRECHARGE command period. */
	tmp = (DDR_tRTP%ns == 0) ? (DDR_tRTP/ns) : (DDR_tRTP/ns+1);
	if (tmp < 1) tmp = 1;
	if (tmp > 4) tmp = 4;
	ddrc_timing1_reg |= ((tmp-1) << DDRC_TIMING1_TRTP_BIT);
	
	/* PRECHARGE command period. */
	tmp = (DDR_tRP%ns == 0) ? DDR_tRP/ns : (DDR_tRP/ns+1);
	if (tmp < 1) tmp = 1;
	if (tmp > 8) tmp = 8;
	ddrc_timing1_reg |= ((tmp-1) << DDRC_TIMING1_TRP_BIT);

	/* ACTIVE to READ or WRITE command period. */
	tmp = (DDR_tRCD%ns == 0) ? DDR_tRCD/ns : (DDR_tRCD/ns+1);
	if (tmp < 1) tmp = 1;
	if (tmp > 8) tmp = 8;
	ddrc_timing1_reg |= ((tmp-1) << DDRC_TIMING1_TRCD_BIT);

	/* ACTIVE to ACTIVE command period. */
	tmp = (DDR_tRC%ns == 0) ? DDR_tRC/ns : (DDR_tRC/ns+1);
	if (tmp < 3) tmp = 3;
	if (tmp > 31) tmp = 31;
	ddrc_timing1_reg |= ((tmp/2) << DDRC_TIMING1_TRC_BIT);

	/* ACTIVE bank A to ACTIVE bank B command period. */
	tmp = (DDR_tRRD%ns == 0) ? DDR_tRRD/ns : (DDR_tRRD/ns+1);
	if (tmp < 2) tmp = 2;
	if (tmp > 4) tmp = 4;
	ddrc_timing1_reg |= ((tmp-1) << DDRC_TIMING1_TRRD_BIT);


	/* WRITE Recovery Time defined by register MR of DDR2 memory */
	tmp = (DDR_tWR%ns == 0) ? DDR_tWR/ns : (DDR_tWR/ns+1);
	tmp = (tmp < 1) ? 1 : tmp;
	tmp = (tmp < 2) ? 2 : tmp;
	tmp = (tmp > 6) ? 6 : tmp;
	ddrc_timing1_reg |= ((tmp-1) << DDRC_TIMING1_TWR_BIT);
	ddr_twr = tmp; 

	/* WRITE to READ command delay. */ 
	tmp = (DDR_tWTR%ns == 0) ? DDR_tWTR/ns : (DDR_tWTR/ns+1);
	if (tmp > 4) tmp = 4;
	ddrc_timing1_reg |= ((tmp-1) << DDRC_TIMING1_TWTR_BIT);


	/* WRITE to READ command delay. */ 
	tmp = DDR_tWTR/ns;
	if (tmp < 1) tmp = 1;
	if (tmp > 4) tmp = 4;
	ddrc_timing1_reg |= ((tmp-1) << DDRC_TIMING1_TWTR_BIT);

	/* AUTO-REFRESH command period. */
	tmp = (DDR_tRFC%ns == 0) ? DDR_tRFC/ns : (DDR_tRFC/ns+1);
	if (tmp > 31) tmp = 31;

	if(cpu_id == 0x352920e4)
	{
		ddrc_timing2_reg = ((tmp/2) << DDRC_TIMING2_TRFC_BIT_60b);
	}
	else
	{
		ddrc_timing2_reg = ((tmp/2) << DDRC_TIMING2_TRFC_BIT_60);
	}

	/* Minimum Self-Refresh / Deep-Power-Down time */
	tmp = DDR_tMINSR/ns;
	if (tmp < 9) tmp = 9;
	if (tmp > 129) tmp = 129;
	ddrc_timing2_reg |= (((tmp-1)/8-1) << DDRC_TIMING2_TMINSR_BIT);
	ddrc_timing2_reg |= (DDR_tXP-1)<<4 | (DDR_tMRD-1);

	init_ddrc_refcnt = DDR_CLK_DIV << 1 | DDRC_REFCNT_REF_EN;

	ns_int = (1000000000%mem_clk == 0) ?
		(1000000000/mem_clk) : (1000000000/mem_clk+1);
	tmp = DDR_tREFI/ns_int;
	tmp = tmp / (16 * (1 << DDR_CLK_DIV)) - 1;
	if (tmp > 0xfff)
		tmp = 0xfff;
	if (tmp < 1)
		tmp = 1;

	init_ddrc_refcnt |= tmp << DDRC_REFCNT_CON_BIT;
	init_ddrc_dqs = DDRC_DQS_AUTO | DDRC_DQS_DET;

	/* precharge power down, disable power down */
	/* precharge power down, if set active power down, |= DDRC_CTRL_ACTPD */
	init_ddrc_ctrl = DDRC_CTRL_PDT_DIS | DDRC_CTRL_PRET_8 | DDRC_CTRL_UNALIGN | DDRC_CTRL_CKE;
	/* Add Jz4760 chip here. Jz4760 chip have no cvt */
#define MAX_TSEL_VALUE 4
#define MAX_DELAY_VALUES 16 /* quars (2) * hls (2) * msels (4) */
	int j, index, quar;
	int mem_index[MAX_DELAY_VALUES];
#if 1 // probe
	jzmemset(mem_index, 0, MAX_DELAY_VALUES);
	for (i = 1; i < MAX_TSEL_VALUE; i ++) {
		tsel = i;
		for (j = 0; j < MAX_DELAY_VALUES; j++) {
			msel = j/4;
			hl = ((j/2)&1)^1;
			quar = j&1;

			/* reset ddrc_controller */
			REG_DDRC_CTRL = DDRC_CTRL_RESET;

			/* Wait for precharge, > 200us */
			tmp_cnt = (cpu_clk / 1000000) * 200;
			while (tmp_cnt--);
			
			REG_DDRC_CTRL = 0x0;
			if(cpu_id == 0x352920e4)
			{
				REG_DDRC_PMEMPS0 = 0xff00; 
				REG_DDRC_PMEMPS1 = 0xff015555; 
				REG_DDRC_PMEMPS2 = 0x55555; 
			}
			REG_DDRC_TIMING1 = ddrc_timing1_reg;
			REG_DDRC_TIMING2 = ddrc_timing2_reg;
			
			ddr_mem_init(msel, hl, tsel, quar);

			memsize = initdram(0);
			mem_base0 = DDR_MEM_PHY_BASE >> 24;
			mem_base1 = (DDR_MEM_PHY_BASE + memsize / (DDR_CS1EN + DDR_CS0EN)) >> 24;
			mem_mask1 = mem_mask0 = 0xff &
				~(((memsize/(DDR_CS1EN+DDR_CS0EN) >> 24)
				   - 1) & DDRC_MMAP_MASK_MASK);
			
			ddrc_mmap0_reg = mem_base0 << DDRC_MMAP_BASE_BIT | mem_mask0;
			ddrc_mmap1_reg = mem_base1 << DDRC_MMAP_BASE_BIT | mem_mask1;

			REG_DDRC_MMAP0 = ddrc_mmap0_reg;
			if(cpu_id == 0x352920e4)
			{
				REG_DDRC_MMAP1 = 0x0000ff00;
			}
			else
			{
				REG_DDRC_MMAP1 = ddrc_mmap1_reg;
			}

			REG_DDRC_REFCNT = init_ddrc_refcnt;

			/* Enable DLL Detect */
			REG_DDRC_DQS    = init_ddrc_dqs;

			while(!(REG_DDRC_DQS &( DDRC_DQS_ERROR | DDRC_DQS_READY)));
			/* Set CKE High */
			REG_DDRC_CTRL = init_ddrc_ctrl;

			/* Wait for number of auto-refresh cycles */
			tmp_cnt = (cpu_clk / 1000000) * 10;
			while (tmp_cnt--);

			/* Auto Refresh */
			REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;

			/* Wait for number of auto-refresh cycles */
			tmp_cnt = (cpu_clk / 1000000) * 10;
			while (tmp_cnt--);

			tmp_cnt = (cpu_clk / 1000000) * 10;
			while (tmp_cnt--);

			int result = 0;
			serial_puts("ddr test:");
			result = ddr_dma_test(0);
			if(result != 0)
				serial_puts("ddr test fail\n");
			else
				serial_puts("ddr test ok\n");
#if 0
			serial_puts("result:");
			serial_put_hex(result);
			serial_put_hex(num);
#endif
			if(result != 0) {
				if (num > 0)
					break;
				else
					continue;
			} else { /* test pass */
				mem_index[num] = j;
				num++;
			}
		}
		
		if (num > 0)
			break;
	}
	if (tsel == 3 && num == 0) 
		serial_puts("\n\nDDR INIT ERROR: can't find a suitable mask delay.\n");
	index = 0;
	for (i = 0; i < num; i++) {
		index += mem_index[i];
		serial_put_hex(mem_index[i]);
	}

	serial_puts("index-1:");
	serial_put_hex(index);
	
	if (num)
		index /= num;
#endif

	msel = index/4;
	hl = ((index/2)&1)^1;
	quar = index&1;

	serial_puts("tsel");
	serial_put_hex(tsel);
	serial_puts("num");
	serial_put_hex(num);
	serial_puts("index-2:");
	serial_put_hex(index);

	/* reset ddrc_controller */
	REG_DDRC_CTRL = DDRC_CTRL_RESET;
	
	/* Wait for precharge, > 200us */
	tmp_cnt = (cpu_clk / 1000000) * 200;
	while (tmp_cnt--);
	
	REG_DDRC_CTRL = 0x0;
	if(cpu_id == 0x352920e4)
	{
		REG_DDRC_PMEMPS0 = 0xff00; 
		REG_DDRC_PMEMPS1 = 0xff015555; 
		REG_DDRC_PMEMPS2 = 0x55555; 
	}
	REG_DDRC_TIMING1 = ddrc_timing1_reg;
	REG_DDRC_TIMING2 = ddrc_timing2_reg;

	ddr_mem_init(msel, hl, tsel, quar);

	memsize = initdram(0);
	mem_base0 = DDR_MEM_PHY_BASE >> 24;
	mem_base1 = (DDR_MEM_PHY_BASE + memsize / (DDR_CS1EN + DDR_CS0EN)) >> 24;
	mem_mask1 = mem_mask0 = 0xff &
		~(((memsize/(DDR_CS1EN+DDR_CS0EN) >> 24)
		   - 1) & DDRC_MMAP_MASK_MASK);
	
	ddrc_mmap0_reg = mem_base0 << DDRC_MMAP_BASE_BIT | mem_mask0;
	ddrc_mmap1_reg = mem_base1 << DDRC_MMAP_BASE_BIT | mem_mask1;

	REG_DDRC_MMAP0 = ddrc_mmap0_reg;
	if(cpu_id == 0x352920e4)
	{
		REG_DDRC_MMAP1 = 0x0000ff00;
	}
	else
	{
		REG_DDRC_MMAP1 = ddrc_mmap1_reg;
	}
	REG_DDRC_REFCNT = init_ddrc_refcnt;

	/* Enable DLL Detect */
	REG_DDRC_DQS    = init_ddrc_dqs;
			
	/* Set CKE High */
	REG_DDRC_CTRL = init_ddrc_ctrl;

	/* Wait for number of auto-refresh cycles */
	tmp_cnt = (cpu_clk / 1000000) * 10;
	while (tmp_cnt--);
	
	/* Auto Refresh */
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;
	
	/* Wait for number of auto-refresh cycles */
	tmp_cnt = (cpu_clk / 1000000) * 10;
	while (tmp_cnt--);
	ddr_dma_test(0);
	if(testall)
		testallmem();
}
