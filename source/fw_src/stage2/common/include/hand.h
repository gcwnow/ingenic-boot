#ifndef __HAND_H__
#define __HAND_H__

typedef struct {
	/* CPU ID */
	unsigned int  cpu_id;
	/* PLL args */
	unsigned char ext_clk;
	unsigned char cpu_speed;
	unsigned char phm_div;
	unsigned char use_uart;
	unsigned int  boudrate;

	/* SDRAM args */
	unsigned char bus_width;
	unsigned char bank_num;
	unsigned char row_addr;
	unsigned char col_addr;
	unsigned char is_mobile;
	unsigned char is_busshare;

	/* debug args */
	unsigned char debug_ops;
	unsigned char pin_num;
	unsigned int  start;
	unsigned int  size;
	
	/* for align */
//	unsigned char align1;
//	unsigned char align2;
}fw_args_t;

typedef struct {
	/* nand flash info */
//	int nand_start;
	int pt;                 //cpu type: jz4740/jz4750 .....
	int nand_bw;
	int nand_rc;
	int nand_ps;
	int nand_ppb;
	int nand_force_erase;
	int nand_pn;
	int nand_os;
	int nand_eccpos;        //ECC position
	int nand_bbpage;        //bad block position
	int nand_bbpos;         //bad block position
	int nand_plane;
	int nand_bchbit;
	int nand_wppin;
	int nand_bpc;
	int nand_bchstyle;		//device os : linux or minios

	fw_args_t fw_args;
} hand_t;

#endif /* __HAND_H__ */
