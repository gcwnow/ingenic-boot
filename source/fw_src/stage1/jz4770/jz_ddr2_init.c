/* along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "configs.h"
#include "ddr2.h"

void ddr_mem_init(int msel, int hl, int tsel, int arg);

void ddr_mem_init(int msel, int hl, int tsel, int arg)
{
	volatile int tmp_cnt;
	register unsigned int cpu_clk, ddr_twr;
	register unsigned int ddrc_cfg_reg = 0, init_ddrc_mdelay = 0;

	cpu_clk = CFG_CPU_SPEED;

#if defined(CONFIG_FPGA)

	ddrc_cfg_reg = arg << 30 | DDRC_CFG_TYPE_DDR2 | (DDR_ROW - 12) << 10
		| (DDR_COL-8) << 8 | DDR_CS1EN << 7 | DDR_CS0EN << 6
		| ((DDR_CL-1) | 0x8)<<2 | DDR_BANK8 << 1 | DDR_DW32;

#else /* if defined(CONFIG_FPGA) */

	ddrc_cfg_reg = DDRC_CFG_TYPE_DDR2 | (DDR_ROW - 12) << 10
		| (DDR_COL - 8) << 8 | DDR_CS1EN << 7 | DDR_CS0EN << 6
		| ((DDR_CL - 1) | 0x8) << 2 | DDR_BANK8 << 1 | DDR_DW32;

#endif /* if defined(CONFIG_FPGA) */

	ddrc_cfg_reg |= DDRC_CFG_MPRT;

#if defined(CONFIG_FPGA)
	init_ddrc_mdelay = tsel << 18 | msel << 16 | hl << 15;
#else
	init_ddrc_mdelay = tsel << 18 | msel << 16 | hl << 15 | arg << 14;
#endif
	ddr_twr = ((REG_DDRC_TIMING1 & DDRC_TIMING1_TWR_MASK) >> DDRC_TIMING1_TWR_BIT) + 1;
	REG_DDRC_CFG = ddrc_cfg_reg;
	REG_DDRC_MDELAY = init_ddrc_mdelay | DDRC_MDELAY_MAUTO;

	/***** init ddrc registers & ddr memory regs ****/
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
#if defined(CONFIG_DDR2_DIFFERENTIAL)
	REG_DDRC_LMR = ((DDR_EMRS1_DIC_HALF) << 16) | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
#else
	REG_DDRC_LMR = ((DDR_EMRS1_DIC_HALF | DDR_EMRS1_DQS_DIS) << 16) | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
#endif

	/* wait DDR_tMRD */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);

	/* MR - DLL Reset A1A0 burst 2 */
	REG_DDRC_LMR = ((ddr_twr - 1) << 9 | DDR2_MRS_DLL_RST | DDR_CL << 4 | DDR_MRS_BL_4) << 16
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
#if defined(CONFIG_DDR2_DIFFERENTIAL)
	REG_DDRC_LMR = (DDR_EMRS1_DIC_HALF | DDR_EMRS1_OCD_DFLT) << 16 | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
#else
	REG_DDRC_LMR = (DDR_EMRS1_DIC_HALF | DDR_EMRS1_DQS_DIS | DDR_EMRS1_OCD_DFLT) << 16
		| DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
#endif

	/* EMR1 - OCD Exit */
#if defined(CONFIG_DDR2_DIFFERENTIAL)
	REG_DDRC_LMR = ((DDR_EMRS1_DIC_HALF) << 16) | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
#else
	REG_DDRC_LMR = ((DDR_EMRS1_DIC_HALF | DDR_EMRS1_DQS_DIS) << 16) | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
#endif

	/* wait DDR_tMRD */
	tmp_cnt = (cpu_clk / 1000000) * 1;
	while (tmp_cnt--);
}
