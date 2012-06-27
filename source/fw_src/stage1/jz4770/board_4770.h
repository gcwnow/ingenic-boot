/*
 * jz4770_board.h
 *
 * JZ4770 board definitions.
 *
 * Copyright (c) 2005-2008 Ingenic Semiconductor Inc.
 *
 */
#ifndef __BOARD_JZ4770_H__
#define __BOARD_JZ4770_H__


//#define CONFIG_FPGA
//#define DEBUG
//#define CONFIG_LOAD_UBOOT /* if not defined, load zImage */

/*-------------------------------------------------------------------
 * Frequency of the external OSC in Hz.
 */
#define CFG_EXTAL		12000000
#define CFG_DIV                 2             /* for FPGA */
/*-------------------------------------------------------------------
 * CPU speed.
 */
//#define CFG_CPU_SPEED		144000000	/* CPU clock */
#define CFG_CPU_SPEED		288000000	/* CPU clock */
//#define CFG_CPU_SPEED		996000000	/* CPU clock */
//#define CFG_CPU_SPEED		600000000	/* CPU clock */

/*-------------------------------------------------------------------
 * Serial console.
 */
#define UART_BASE		UART2_BASE
//CONFIG_BAUDRATE = 	115200

/*-----------------------------------------------------------------------
 * NAND FLASH configuration
 */
#define CFG_NAND_BW8		1               /* Data bus width: 0-16bit, 1-8bit */
#define CFG_NAND_PAGE_SIZE      2048
#define CFG_NAND_ROW_CYCLE	3     
#define CFG_NAND_BLOCK_SIZE	(256 << 10)	/* NAND chip block size		*/
#define CFG_NAND_BADBLOCK_PAGE	127		/* NAND bad block was marked at this page in a block, starting from 0 */
#define CFG_NAND_BCH_BIT        4               /* Specify the hardware BCH algorithm for 4770 (4|8) */
#define CFG_NAND_ECC_POS        24              /* Ecc offset position in oob area, its default value is 3 if it isn't defined. */
#define CFG_NAND_BASE           0xBA000000
#define CFG_NAND_SMCR1          0x0D555500      /* 0x0fff7700 is slowest */

#define CFG_NAND_USE_PN         1               /* Use PN in jz4770 for TLC NAND */

#ifdef CONFIG_LOAD_UBOOT
#define CFG_NAND_U_BOOT_OFFS	(CFG_NAND_BLOCK_SIZE*2)	/* Offset to RAM U-Boot image	*/
#define CFG_NAND_U_BOOT_SIZE	(512 << 10)	/* Size of RAM U-Boot image	*/
#define CFG_NAND_U_BOOT_DST	0x80100000	/* Load NUB to this addr	*/
#define CFG_NAND_U_BOOT_START	CFG_NAND_U_BOOT_DST /* Start NUB from this addr	*/
#else // load zImage
#define PARAM_BASE		0x80004000
#define CFG_KERNEL_OFFS		(CFG_NAND_BLOCK_SIZE*2) /* NAND offset of kernel image being loaded */
#define CFG_KERNEL_SIZE		(2 << 20)	/* Size of kernel image */
#define CFG_KERNEL_DST		0x80100000	/* Load kernel to this addr */
#define CFG_KERNEL_START	CFG_KERNEL_DST	/* Start kernel from this addr	*/
#endif


#endif /* __BOARD_JZ4770_H__ */
