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

#include "jz4770.h"
#include "configs.h"
#include "board_4770.h"
#include "ddr2.h"

//#define DEBUG
#undef DEBUG

#define dprintf(x...)

void sdram_init(void);

extern void ddr_mem_init(int msel, int hl, int tsel, int arg);

#ifdef DEBUG
static void ddrc_regs_print(void)
{
	serial_puts("\nDDRC REGS:\n");
	serial_puts("REG_DDRC_ST \t\t= ");
	serial_put_hex(REG_DDRC_ST);
	serial_puts("REG_DDRC_CFG \t\t= ");
	serial_put_hex(REG_DDRC_CFG);
	serial_puts("REG_DDRC_CTRL \t\t= ");
	serial_put_hex(REG_DDRC_CTRL);
	serial_puts("REG_DDRC_LMR \t\t= ");
	serial_put_hex(REG_DDRC_LMR);
	serial_puts("REG_DDRC_TIMING1 \t= ");
	serial_put_hex(REG_DDRC_TIMING1);
	serial_puts("REG_DDRC_TIMING2 \t= ");
	serial_put_hex(REG_DDRC_TIMING2);
	serial_puts("REG_DDRC_REFCNT \t\t= ");
	serial_put_hex(REG_DDRC_REFCNT);
	serial_puts("REG_DDRC_DQS \t\t= ");
	serial_put_hex(REG_DDRC_DQS);
	serial_puts("REG_DDRC_DQS_ADJ \t= ");
	serial_put_hex(REG_DDRC_DQS_ADJ);
	serial_puts("REG_DDRC_MMAP0 \t\t= ");
	serial_put_hex(REG_DDRC_MMAP0);
	serial_puts("REG_DDRC_MMAP1 \t\t= ");
	serial_put_hex(REG_DDRC_MMAP1);
	serial_puts("REG_DDRC_MDELAY \t\t= ");
	serial_put_hex(REG_DDRC_MDELAY);
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

long int initdram(int board_type)
{
	u32 ddr_cfg;
	u32 rows, cols, dw, banks;
	ulong size;
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
		if ((dsrc != data_expect)
		    || (ddst != data_expect)) {
#if 0
			serial_puts("wrong data at:");
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

	return err;
}

static void dma_nodesc_test(int dma_chan, int dma_src_addr, int dma_dst_addr, int size)
{
	int dma_src_phys_addr, dma_dst_phys_addr;

	/* Allocate DMA buffers */
	dma_src_phys_addr = dma_src_addr & ~0xa0000000;
	dma_dst_phys_addr = dma_dst_addr & ~0xa0000000;

	/* Init DMA module */
	REG_DMAC_DCCSR(dma_chan) = 0;
	REG_DMAC_DRSR(dma_chan) = DMAC_DRSR_RS_AUTO;
	REG_DMAC_DSAR(dma_chan) = dma_src_phys_addr;
	REG_DMAC_DTAR(dma_chan) = dma_dst_phys_addr;
	REG_DMAC_DTCR(dma_chan) = size / 32;
	REG_DMAC_DCMD(dma_chan) = DMAC_DCMD_SAI | DMAC_DCMD_DAI | DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 | DMAC_DCMD_DS_32BYTE | DMAC_DCMD_TIE;
	REG_DMAC_DCCSR(dma_chan) = DMAC_DCCSR_NDES | DMAC_DCCSR_EN;
}

#define MAX_TSEL_VALUE 4
#define MAX_DELAY_VALUES 16 /* quars (2) * hls (2) * msels (4) */
typedef struct ddrc_common_regs {
	unsigned int ctrl;
	unsigned int timing1;
	unsigned int timing2; 
	unsigned int refcnt;
	unsigned int dqs;
	unsigned int dqs_adj;
} ddrc_common_regs_t;

typedef struct delay_sel {
	int tsel;
	int msel;
	int hl;
	int quar;
} delay_sel_t;

typedef struct mdelay_array {
	int tsel;
	int num;
	int index[MAX_DELAY_VALUES];
} mdelay_array_t;

#define DDR_DMA_BASE  (0xa0000000)		/*un-cached*/

static int ddr_dma_test(int print_flag)
{
	int i, err = 0, banks, blocks;
	int times;
	unsigned int addr, DDR_DMA0_SRC, DDR_DMA0_DST;
	volatile unsigned int tmp;
	register unsigned int cpu_clk;
	long int memsize, banksize, testsize;

	banks = (DDR_BANK8 ? 8 : 4) *(DDR_CS0EN + DDR_CS1EN);
	memsize = initdram(0);
	if (memsize > EMC_LOW_SDRAM_SPACE_SIZE)
		memsize = EMC_LOW_SDRAM_SPACE_SIZE;
	//dprintf("memsize = 0x%08x\n", memsize);
	banksize = memsize/banks;
	testsize = 4096;
	blocks = memsize / testsize;
	cpu_clk = CFG_CPU_SPEED;
	//for(times = 0; times < blocks; times++) {
	for(times = 0; times < banks; times++) {
#if 0
		DDR_DMA0_SRC = DDR_DMA_BASE + banksize * 0;
		DDR_DMA0_DST = DDR_DMA_BASE + banksize * (banks - 2) + testsize;
#else
		DDR_DMA0_SRC = DDR_DMA_BASE + banksize * times;
		DDR_DMA0_DST = DDR_DMA_BASE + banksize * (times + 1) - testsize;
#endif
		addr = DDR_DMA0_SRC;

		for (i = 0; i < testsize; i += 4) {
			*(volatile unsigned int *)(addr + i) = gen_verify_data(i);
		}

		REG_DMAC_DMACR(0) = 0;

		/* Init target buffer */
		jzmemset((void *)DDR_DMA0_DST, 0, testsize);
		dma_nodesc_test(0, DDR_DMA0_SRC, DDR_DMA0_DST, testsize);

		REG_DMAC_DMACR(0) = DMAC_DMACR_DMAE; /* global DMA enable bit */

		while(REG_DMAC_DTCR(0));

		tmp = (cpu_clk / 1000000) * 1;
		while (tmp--);

		err = dma_check_result((void *)DDR_DMA0_SRC, (void *)DDR_DMA0_DST, testsize,print_flag);

		//REG_DMAC_DCCSR(0) &= ~DMAC_DCCSR_EN;  /* disable DMA */
		REG_DMAC_DMACR(0) = 0;
		REG_DMAC_DCCSR(0) = 0;
		REG_DMAC_DCMD(0) = 0;
		REG_DMAC_DRSR(0) = 0;

		if (err != 0) {
			return err;
		}
	}
	return err;
}

void ddr_controller_init(ddrc_common_regs_t *ddrc_common_regs, int msel, int hl, int tsel, int arg)
{
	volatile unsigned int tmp_cnt;
	register unsigned int cpu_clk, us;
	register unsigned int memsize, ddrc_mmap0_reg, ddrc_mmap1_reg, mem_base0, mem_base1, mem_mask0, mem_mask1;
	cpu_clk = CFG_CPU_SPEED;
	us = cpu_clk / 1000000;

	/* reset ddrc_controller */
	REG_DDRC_CTRL = DDRC_CTRL_RESET;
	
	/* Wait for precharge, > 200us */
	tmp_cnt = us * 300;
	while (tmp_cnt--);

	REG_DDRC_CTRL = 0x0;

#if defined(CONFIG_SDRAM_DDR2)
	REG_DDRC_PMEMCTRL0 =  0xaaaa; /* FIXME: ODT registers not configed */

#if defined(CONFIG_DDR2_DIFFERENTIAL)
	REG_DDRC_PMEMCTRL2 = 0xaaaaa;
#else
	REG_DDRC_PMEMCTRL2 = 0x0;
#endif
	REG_DDRC_PMEMCTRL3 &= ~(1 << 16);
	REG_DDRC_PMEMCTRL3 |= (1 << 17);
#if defined(CONFIG_DDR2_DIFFERENTIAL)
	REG_DDRC_PMEMCTRL3 &= ~(1 << 15);
#endif
#elif defined(CONFIG_SDRAM_MDDR)
	REG_DDRC_PMEMCTRL3 |= (1 << 16);
#endif
	ddrc_common_regs->timing2 &= ~DDRC_TIMING2_RWCOV_MASK;
	ddrc_common_regs->timing2 |= ((tsel == 0) ? 0 : (tsel-1)) << DDRC_TIMING2_RWCOV_BIT;
	
	REG_DDRC_TIMING1 = ddrc_common_regs->timing1;
	REG_DDRC_TIMING2 = ddrc_common_regs->timing2;
	REG_DDRC_DQS_ADJ = ddrc_common_regs->dqs_adj;

	ddr_mem_init(msel, hl, tsel, arg);

	memsize = initdram(0);
	mem_base0 = DDR_MEM_PHY_BASE >> 24;
	mem_base1 = (DDR_MEM_PHY_BASE + memsize / (DDR_CS1EN + DDR_CS0EN)) >> 24;
	mem_mask1 = mem_mask0 = 0xff &
		~(((memsize / (DDR_CS1EN + DDR_CS0EN) >> 24) - 1) &
		  DDRC_MMAP_MASK_MASK);

	ddrc_mmap0_reg = mem_base0 << DDRC_MMAP_BASE_BIT | mem_mask0;
	ddrc_mmap1_reg = mem_base1 << DDRC_MMAP_BASE_BIT | mem_mask1;

	REG_DDRC_MMAP0 = ddrc_mmap0_reg;
	REG_DDRC_MMAP1 = ddrc_mmap1_reg;
#ifdef DEBUG
	ddrc_regs_print();
#endif
	REG_DDRC_REFCNT = ddrc_common_regs->refcnt;

	/* Enable DLL Detect */
	REG_DDRC_DQS    = ddrc_common_regs->dqs;
	
	/* Set CKE High */
	REG_DDRC_CTRL = ddrc_common_regs->ctrl;
	
	/* Wait for number of auto-refresh cycles */
	tmp_cnt = us * 10;
	while (tmp_cnt--);
	
	/* Auto Refresh */
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;
	
	/* Wait for number of auto-refresh cycles */
	tmp_cnt = us * 10;
	while (tmp_cnt--);

}

/* DDR sdram init */
void sdram_init_4770(void)
{
	int i, num = 0, tsel = 0, msel, hl;
	int j = 0, k = 0, index = 0, quar = 0, sum = 0, tsel_index=0;
	register unsigned int tmp, cpu_clk, mem_clk, ddr_twr, ns_int;
	register unsigned long ps;
	register unsigned int dqs_adj = 0x2325;
	ddrc_common_regs_t ddrc_common_regs;

#ifdef DEBUG
	ddrc_regs_print();
#endif

	cpu_clk = __cpm_get_cclk();;

	mem_clk = __cpm_get_mclk();
	ps = 1000000000 / (mem_clk / 1000); /* ns per tck ns <= real value */
	//ns = 1000000000 / mem_clk; /* ns per tck ns <= real value */
#if 1//def DEBUG
	serial_puts("cpu_clk = ");
	serial_put_hex(cpu_clk);
	serial_puts("mem_clk = ");
	serial_put_hex(mem_clk);
	serial_puts("ps = ");
	serial_put_hex(ps);
#endif
	/* ACTIVE to PRECHARGE command period */
	tmp = DDR_GET_VALUE(DDR_tRAS, ps);
	//tmp = (DDR_tRAS % ns == 0) ? (DDR_tRAS / ps) : (DDR_tRAS / ps + 1);
	if (tmp < 1) tmp = 1;
	if (tmp > 31) tmp = 31;
	ddrc_common_regs.timing1 = (((tmp) / 2) << DDRC_TIMING1_TRAS_BIT);

	/* READ to PRECHARGE command period. */
	tmp = DDR_GET_VALUE(DDR_tRTP, ps);
	//tmp = (DDR_tRTP % ns == 0) ? (DDR_tRTP / ns) : (DDR_tRTP / ns + 1);
	if (tmp < 1) tmp = 1;
	if (tmp > 4) tmp = 4;
	ddrc_common_regs.timing1 |= ((tmp - 1) << DDRC_TIMING1_TRTP_BIT);

	/* PRECHARGE command period. */
        tmp = DDR_GET_VALUE(DDR_tRP, ps);
	//tmp = (DDR_tRP % ns == 0) ? DDR_tRP / ns : (DDR_tRP / ns + 1);
	if (tmp < 1) tmp = 1;
	if (tmp > 8) tmp = 8;
	ddrc_common_regs.timing1 |= ((tmp - 1) << DDRC_TIMING1_TRP_BIT);

	/* ACTIVE to READ or WRITE command period. */
	tmp = DDR_GET_VALUE(DDR_tRCD, ps);
	//tmp = (DDR_tRCD % ns == 0) ? DDR_tRCD / ns : (DDR_tRCD / ns + 1);
	if (tmp < 1) tmp = 1;
	if (tmp > 8) tmp = 8;
	ddrc_common_regs.timing1 |= ((tmp - 1) << DDRC_TIMING1_TRCD_BIT);

	/* ACTIVE to ACTIVE command period. */
	tmp = DDR_GET_VALUE(DDR_tRC, ps);
	//tmp = (DDR_tRC % ns == 0) ? DDR_tRC / ns : (DDR_tRC / ns + 1);
	if (tmp < 3) tmp = 3;
	if (tmp > 31) tmp = 31;
	ddrc_common_regs.timing1 |= ((tmp / 2) << DDRC_TIMING1_TRC_BIT);

	/* ACTIVE bank A to ACTIVE bank B command period. */
	tmp = DDR_GET_VALUE(DDR_tRRD, ps);
	//tmp = (DDR_tRRD % ns == 0) ? DDR_tRRD / ns : (DDR_tRRD / ns + 1);
	if (tmp < 2) tmp = 2;
	if (tmp > 4) tmp = 4;
	ddrc_common_regs.timing1 |= ((tmp - 1) << DDRC_TIMING1_TRRD_BIT);

	/* WRITE Recovery Time defined by register MR of DDR2 memory */
	tmp = DDR_GET_VALUE(DDR_tWR, ps);
	//tmp = (DDR_tWR % ns == 0) ? DDR_tWR / ns : (DDR_tWR / ns + 1);
	tmp = (tmp < 1) ? 1 : tmp;
	tmp = (tmp < 2) ? 2 : tmp;
	tmp = (tmp > 6) ? 6 : tmp;
	ddrc_common_regs.timing1 |= ((tmp - 1) << DDRC_TIMING1_TWR_BIT);
	ddr_twr = tmp;

	// Unit is ns
	if(DDR_tWTR > 5) {
		/* WRITE to READ command delay. */
		tmp = DDR_GET_VALUE(DDR_tWTR, ps);
		//tmp = (DDR_tWTR % ns == 0) ? DDR_tWTR / ns : (DDR_tWTR / ns + 1);
		if (tmp > 4) tmp = 4;
		ddrc_common_regs.timing1 |= ((tmp - 1) << DDRC_TIMING1_TWTR_BIT);
	// Unit is tCK
	} else {
		/* WRITE to READ command delay. */
		tmp = DDR_tWTR;
		if (tmp > 4) tmp = 4;
		ddrc_common_regs.timing1 |= ((tmp - 1) << DDRC_TIMING1_TWTR_BIT);
	}

	/* AUTO-REFRESH command period. */
	tmp = DDR_GET_VALUE(DDR_tRFC, ps);
	//tmp = (DDR_tRFC % ns == 0) ? DDR_tRFC / ns : (DDR_tRFC / ns + 1);
	if (tmp > 31) tmp = 31;
	ddrc_common_regs.timing2 = ((tmp / 2) << DDRC_TIMING2_TRFC_BIT);

	/* Minimum Self-Refresh / Deep-Power-Down time */
	tmp = DDR_tMINSR;
	if (tmp < 9) tmp = 9;
	if (tmp > 129) tmp = 129;
	tmp = ((tmp - 1)%8 == 0) ? ((tmp - 1)/8-1) : ((tmp - 1)/8);
	ddrc_common_regs.timing2 |= (tmp << DDRC_TIMING2_TMINSR_BIT);
	ddrc_common_regs.timing2 |= (DDR_tXP - 1) << 4 | (DDR_tMRD - 1);

	ddrc_common_regs.refcnt = DDR_CLK_DIV << 1 | DDRC_REFCNT_REF_EN;

	ns_int = (1000000000 % mem_clk == 0) ?
		(1000000000 / mem_clk) : (1000000000 / mem_clk + 1);
	tmp = DDR_tREFI/ns_int;
	tmp = tmp / (16 * (1 << DDR_CLK_DIV)) - 1;
	if (tmp > 0xfff)
		tmp = 0xfff;
	if (tmp < 1)
		tmp = 1;

	ddrc_common_regs.refcnt |= tmp << DDRC_REFCNT_CON_BIT;
	ddrc_common_regs.dqs = DDRC_DQS_AUTO | DDRC_DQS_DET | DDRC_DQS_SRDET;

	/* precharge power down, disable power down */
	/* precharge power down, if set active power down, |= DDRC_CTRL_ACTPD */
	ddrc_common_regs.ctrl = DDRC_CTRL_PDT_DIS | DDRC_CTRL_PRET_8 | DDRC_CTRL_UNALIGN | DDRC_CTRL_CKE;
	ddrc_common_regs.dqs_adj = dqs_adj;
#if 0
	if (mem_clk > 60000000)
		ddrc_common_regs.ctrl |= DDRC_CTRL_RDC;
#endif

	/* Add Jz4760 chip here. Jz4760 chip have no cvt */


	mdelay_array_t mdelay_pass_array[MAX_TSEL_VALUE];
	jzmemset(mdelay_pass_array, 0, sizeof(mdelay_pass_array));

	for (i = 0; i < MAX_TSEL_VALUE; i ++) {
		tsel = i;
		num =0;
		for (j = 0; j < MAX_DELAY_VALUES; j++) {
			msel = j / 4;
			hl = ((j / 2) & 1) ^ 1;
			quar = j & 1;

			ddr_controller_init(&ddrc_common_regs, msel, hl, tsel, quar);
			
			if(ddr_dma_test(0) != 0) {
				if (num > 0) {
					mdelay_pass_array[k].tsel = tsel;
					mdelay_pass_array[k].num = num;
					k++;
					break;
				} 
				else
					continue;
			}
			else {
				mdelay_pass_array[k].index[num] = j;
				num++;
			}
		}
		if (k > 0 && num == 0)
			break;
	}
	if (k == 0) {
		serial_puts("\n\nDDR INIT ERROR: can't find a suitable mask delay.\n");
		while(1);
	}
#if 1
	serial_put_hex(k);
	serial_puts("{\n");
	for (i = 0; i < k; i++) {
		serial_put_hex(mdelay_pass_array[i].tsel);
		serial_put_hex(mdelay_pass_array[i].num);
		for (j = 0; j < mdelay_pass_array[i].num; j++) {
			serial_put_hex(mdelay_pass_array[i].index[j]);
		}
		serial_puts("\n");
			
	}
	serial_puts("}\n");
#endif
	tsel_index = (k-1)/2;
	tsel = mdelay_pass_array[tsel_index].tsel;
	num = mdelay_pass_array[tsel_index].num;
	index = (mdelay_pass_array[tsel_index].index[0] + mdelay_pass_array[tsel_index].index[num-1])/2;
	serial_puts("X");
	serial_put_hex(index);
	serial_put_hex(tsel);
	msel = index/4;
	hl = ((index / 2) & 1) ^ 1;
	quar = index & 1;

	int max_adj = (REG_DDRC_DQS & DDRC_DQS_RDQS_MASK) >> DDRC_DQS_RDQS_BIT;
	max_adj = (REG_DDRC_DQS_ADJ & DDRC_DQS_ADJRSIGN)
		? (max_adj + (REG_DDRC_DQS_ADJ & DDRC_DQS_ADJRDQS_MASK) >> DDRC_DQS_ADJRDQS_BIT)
		: (max_adj - (REG_DDRC_DQS_ADJ & DDRC_DQS_ADJRDQS_MASK) >> DDRC_DQS_ADJRDQS_BIT);
	if (max_adj > DDRC_DQS_ADJRDQS_MASK >> DDRC_DQS_ADJRDQS_BIT)
		max_adj = DDRC_DQS_ADJRDQS_MASK >> DDRC_DQS_ADJRDQS_BIT;

	int index_min=-max_adj, index_max = max_adj;
	num = 0;
	for (i = -max_adj; i < max_adj; i++) {
		dqs_adj = (i < 0) ? (0x2320 | -i) : (0x2300 | i);
		ddrc_common_regs.dqs_adj = dqs_adj;
		ddr_controller_init(&ddrc_common_regs, msel, hl, tsel, quar);
		if (ddr_dma_test(0) != 0) {
			if (num > 0) {
				index_max = i;
				break;
			}
		}
		else {
			if (num == 0)
				index_min = i;
			num++;
		}
	}
	index = (index_min + index_max - 1) / 2;

	dqs_adj = (index < 0) ? (0x2320 | (-index)) : (0x2300 | index);
#if 1
	if (index_min < 0)
		serial_puts("-");
	serial_put_hex((index_min < 0) ? -index_min : index_min);
	if (index_max < 0)
		serial_puts("-");
	serial_put_hex((index_max < 0) ? -index_max : index_max);
	serial_put_hex(dqs_adj);
#endif

	ddrc_common_regs. dqs_adj = dqs_adj;
	ddr_controller_init(&ddrc_common_regs, msel, hl, tsel, quar);

			
//	ddr_dma_test(1);
}
