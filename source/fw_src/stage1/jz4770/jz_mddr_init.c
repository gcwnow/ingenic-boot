/* along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "configs.h"
#include "mddr.h"

void ddr_mem_init(int msel, int hl, int tsel, int arg);

void ddr_mem_init(int msel, int hl, int tsel, int arg)
{
	volatile int tmp_cnt;
	register unsigned int cpu_clk, ddr_twr;
	register unsigned int ddrc_cfg_reg=0, init_ddrc_mdelay=0;

	cpu_clk = CFG_CPU_SPEED;

#if defined(CONFIG_FPGA)

	ddrc_cfg_reg = arg << 30 | DDRC_CFG_BTRUN | DDRC_CFG_TYPE_MDDR
		| (DDR_ROW - 12) << 10 | (DDR_COL - 8) << 8 | DDR_CS1EN << 7 | DDR_CS0EN << 6
		| ((DDR_CL - 1) | 0x8) << 2 | DDR_BANK8 << 1 | DDR_DW32;

#else /* CONFIG_FPGA */

	ddrc_cfg_reg = DDRC_CFG_BTRUN | DDRC_CFG_TYPE_MDDR
		| (DDR_ROW - 12) << 10 | (DDR_COL - 8) << 8 | DDR_CS1EN << 7 | DDR_CS0EN << 6
		| ((DDR_CL - 1) | 0x8) << 2 | DDR_BANK8 << 1 | DDR_DW32;

#endif /* CONFIG_FPGA */

	ddrc_cfg_reg |= DDRC_CFG_MPRT;
#if defined(CONFIG_FPGA)
	init_ddrc_mdelay = tsel << 18 | msel << 16 | hl << 15;
#else
	init_ddrc_mdelay = tsel << 18 | msel << 16 | hl << 15 | arg << 14;
#endif
	ddr_twr = ((REG_DDRC_TIMING1 & DDRC_TIMING1_TWR_MASK) >> DDRC_TIMING1_TWR_BIT) + 1;
	REG_DDRC_CFG     = ddrc_cfg_reg;
	REG_DDRC_MDELAY = init_ddrc_mdelay | DDRC_MDELAY_MAUTO;
	/***** init ddrc registers & ddr memory regs ****/

	/* Wait for number of auto-refresh cycles */
	tmp_cnt = (cpu_clk / 1000000) * 10;
	while (tmp_cnt--);

	REG_DDRC_CTRL = DDRC_CTRL_CKE; // ?

	/* Wait for number of auto-refresh cycles */
	tmp_cnt = (cpu_clk / 1000000) * 20;
	while (tmp_cnt--);

	/* PREA */
	REG_DDRC_LMR =  DDRC_LMR_CMD_PREC | DDRC_LMR_START; //0x1;

	/* Wait for DDR_tRP */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* AR: auto refresh */
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;

	/* wait DDR_tRFC */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* AR: auto refresh */
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;
	/* wait DDR_tRFC */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* MR */
	REG_DDRC_LMR = (DDR_CL<<4 | DDR_MRS_BL_4)<< 16
		| DDRC_LMR_BA_M_MRS | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* wait DDR_tMRD */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* EMR: extend mode register */
	REG_DDRC_LMR = (DDR_EMRS_DS_FULL | DDR_EMRS_PRSR_ALL) << 16
		| DDRC_LMR_BA_M_EMRS | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* wait DDR_tMRD */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);
}
