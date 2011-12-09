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
 *  Author:   <zzhang@ingenic.cn> <xyzhang@ingenic.cn> 
 *
 *  Create:   2008-09-18, by zzhang
 *
 *  Maintain: 2008-09-24, by xyzhang
 *
 *******************************************************************************
 */

#ifndef __JZ4755EMC_H__
#define __JZ4755EMC_H__

/*
				Physical Address Space Map

			-----------------------------------------	<--- 0xFFFFFFFF
			|										|
			|										|
			|	Reserved Space (512MB)				|
			|										|
			|										|
			 ---------------------------------------	<--- 0xE0000000
			|										|
			|			SDRAM						|
			|	Changeable Base Address				|
			|	Changeable Size(2944MB Max)			|
			|										|
			|										|
			|										|
			|										|
			|										|
			 ---------------------------------------
			|										|
		|->	|	Default SDRAM Bank (128MB)			|
		|	|										|
		|	 ---------------------------------------	<--- 0x20000000
		|	|										|
		|	|	Default Static Memory Bank 0 (64MB)	|
		|	 ---------------------------------------	<--- 0x1C000000
		|	|///////////////////////////////////////|
		|	 ---------------------------------------
		|	|	Default Static Memory Bank 1 (8MB)	|
		|	 ---------------------------------------	<--- 0x18000000
		|	|///////////////////////////////////////|
		|	 ---------------------------------------
		|	|	Default Static Memory Bank 2 (8MB)	|
		|	 ---------------------------------------	<--- 0x14000000
		|	|										|
		|	|	Internal I/O Space (64MB)			|
		|	 ---------------------------------------	<--- 0x10000000
		|	|///////////////////////////////////////|
		|	 ---------------------------------------
		|	|	Default Static Memory Bank 3 (8MB)	|
		|	 ---------------------------------------	<--- 0x0C000000
		|	|///////////////////////////////////////|
		|	 ---------------------------------------
		|	|	Default Static Memory Bank 4 (8MB)	|
		|	 ---------------------------------------	<--- 0x08000000
		|	|										|
		---	|	SDRAM Space (128MB)					|
			|										|
			-----------------------------------------	<--- 0x00000000
*/

//--------------------------------------------------------------------------
// External Memory Controller Module(EMC) Address Definition
//--------------------------------------------------------------------------
#define EMC_PHYS_ADDR			( 0x13010000 )
#define EMC_BASE_U_VIRTUAL		( 0xB3010000 )


//--------------------------------------------------------------------------
// EMC Registers Offset Definition
//--------------------------------------------------------------------------
#define EMC_BCR_OFFSET			( 0x00 )	// RW, 32, 0x?0000001, Bus Control Register
#define EMC_SMCR1_OFFSET			( 0x14 )	// RW, 32, 0x0FFF7700, Static memory control register 1
#define EMC_SMCR2_OFFSET			( 0x18 )	// RW, 32, 0x0FFF7700, Static memory control register 2
#define EMC_SMCR3_OFFSET			( 0x1C )	// RW, 32, 0x0FFF7700, Static memory control register 3
#define EMC_SMCR4_OFFSET			( 0x20 )	// RW, 32, 0x0FFF7700, Static memory control register 4
#define EMC_SACR1_OFFSET		( 0x34 )	// RW, 32, 0x000018FC, Static memory bank 1 address configuration register
#define EMC_SACR2_OFFSET		( 0x38 )	// RW, 32, 0x000016FE, Static memory bank 2 address configuration register
#define EMC_SACR3_OFFSET		( 0x3C )	// RW, 32, 0x000014FE, Static memory bank 3 address configuration register
#define EMC_SACR4_OFFSET		( 0x40 )	// RW, 32, 0x00000CFC, Static memory bank 4 address configuration register
#define	EMC_NFCSR_OFFSET			( 0x50 )	// RW, 32, 0x00000000
#define	EMC_DMCR_OFFSET				( 0x80 )	// RW, 32, 0x00000000
#define	EMC_RTCSR_OFFSET			( 0x84 )	// RW, 16, 0x0000
#define	EMC_RTCNT_OFFSET			( 0x88 )	// RW, 16, 0x0000
#define	EMC_RTCOR_OFFSET			( 0x8C )	// RW, 16, 0x0000
#define	EMC_DMAR1_OFFSET				( 0x90 )	// RW, 32, 0x0000
#define	EMC_DMAR2_OFFSET				( 0x94 )	// RW, 32, 0x0000
#define	EMC_SDMR_OFFSET				( 0x8000 )	//  W,  8, ---

//--------------------------------------------------------------------------
// EMC Registers Address Definition
//--------------------------------------------------------------------------
#define A_EMC_BCR				( EMC_BASE_U_VIRTUAL + EMC_BCR_OFFSET )
#define A_EMC_SMCR1				( EMC_BASE_U_VIRTUAL + EMC_SMCR1_OFFSET )
#define A_EMC_SMCR2				( EMC_BASE_U_VIRTUAL + EMC_SMCR2_OFFSET )
#define A_EMC_SMCR3				( EMC_BASE_U_VIRTUAL + EMC_SMCR3_OFFSET )
#define A_EMC_SMCR4				( EMC_BASE_U_VIRTUAL + EMC_SMCR4_OFFSET )
#define A_EMC_SACR1				( EMC_BASE_U_VIRTUAL + EMC_SACR1_OFFSET )
#define A_EMC_SACR2				( EMC_BASE_U_VIRTUAL + EMC_SACR2_OFFSET )
#define A_EMC_SACR3				( EMC_BASE_U_VIRTUAL + EMC_SACR3_OFFSET )
#define A_EMC_SACR4				( EMC_BASE_U_VIRTUAL + EMC_SACR4_OFFSET )
#define A_EMC_NFCSR				( EMC_BASE_U_VIRTUAL + EMC_NFCSR_OFFSET )
#define A_EMC_DMCR				( EMC_BASE_U_VIRTUAL + EMC_DMCR_OFFSET )
#define A_EMC_RTCSR				( EMC_BASE_U_VIRTUAL + EMC_RTCSR_OFFSET )
#define A_EMC_RTCNT				( EMC_BASE_U_VIRTUAL + EMC_RTCNT_OFFSET )
#define A_EMC_RTCOR				( EMC_BASE_U_VIRTUAL + EMC_RTCOR_OFFSET )
#define A_EMC_DMAR1				( EMC_BASE_U_VIRTUAL + EMC_DMAR1_OFFSET )
#define A_EMC_DMAR2				( EMC_BASE_U_VIRTUAL + EMC_DMAR2_OFFSET )
#define A_EMC_SDMR				( EMC_BASE_U_VIRTUAL + EMC_SDMR_OFFSET )

//--------------------------------------------------------------------
/* Bus Control Register */
//--------------------------------------------------------------------
#define EMC_BCR_BT_SEL_BIT      	30
#define EMC_BCR_BT_SEL_MASK     	(0x3 << EMC_BCR_BT_SEL_BIT)
#define EMC_BCR_PK_SEL          		(1 << 24)
#define EMC_BCR_BSR_MASK          	(1 << 2)  /* Nand and SDRAM Bus Share Select: 0, share; 1, unshare */
#define EMC_BCR_BSR_SHARE      	(0 << 2)
#define EMC_BCR_BSR_UNSHARE    	(1 << 2)
#define EMC_BCR_BRE             		(1 << 1)
#define EMC_BCR_ENDIAN         		(1 << 0)

//--------------------------------------------------------------------------
// DRAM Control Register(DMCR) Common Define
//--------------------------------------------------------------------------
#define DMCR_BW_BIT			31
#define DMCR_CA_BIT			26		//	BIT[26:28]
#define DMCR_RMODE_BIT		25		//	BIT[25]
#define DMCR_RFSH_BIT		24		//	BIT[24]
#define DMCR_MRSET_BIT		23		//	BIT[23]
#define DMCR_RA_BIT			20		//	BIT[20:21]
#define DMCR_BA_BIT			19		//	BIT[19]
#define DMCR_PDM_BIT		18		//	BIT[18]
#define DMCR_EPIN_BIT		17		//	BIT[17]
#define DMCR_MBSEL_BIT		16		//	BIT[16]
#define DMCR_TRAS_BIT		13		//	BIT[13:15]
#define DMCR_RCD_BIT		11		//	BIT[11:12]
#define DMCR_TPC_BIT		8		//	BIT[8:10]
#define DMCR_TRWL_BIT		5		//	BIT[5:6]
#define DMCR_TRC_BIT		2		//	BIT[2:4]
#define DMCR_TCL_BIT		0		//	BIT[0:1]

#define DMCR_MBSL_BANK(x)		((x)<<DMCR_MBSEL_BIT)			// NOTE: The x = {0, 1};
#define	DMCR_BUS_WIDTH(x)		((2 - (x)/16)<<DMCR_BW_BIT)		// NOTE: The x = {16, 32};
#define	DMCR_COL_WIDTH(x)		(((x)-8)<<DMCR_CA_BIT)			// NOTE: The x = {8, 9, 10, 11, 12};
#define	DMCR_AUTO_REFRESH		(0<<DMCR_RMODE_BIT)				//
#define	DMCR_SELF_REFRESH		(1<<DMCR_RMODE_BIT)				//
#define	DMCR_REFRESH			(1<<DMCR_RFSH_BIT)				//
#define	DMCR_MRSET				(1<<DMCR_MRSET_BIT)				//

#define	DMCR_ROW_WIDTH(x)		(((x)-11)<<DMCR_RA_BIT)			// NOTE: The x = {11, 12, 13};
#define	DMCR_BANKS(x)			(((x)/2-1)<<DMCR_BA_BIT)		// NOTE: The x = {2, 4};
#define	DMCR_POWER_DOWN			(1<<DMCR_PDM_BIT)				// 
#define	DMCR_CKE_ASSERT			(1<<DMCR_EPIN_BIT)				// 

#define	DMCR_T_RAS_ASSERT(x)		(((x)-4)<<DMCR_TRAS_BIT)		// NOTE: The x = {4, 5, 6, 7, 8, 9, 10, 11};
#define	DMCR_T_RAS_TO_CAS(x)		(((x)-1)<<DMCR_RCD_BIT)			// NOTE: The x = {1, 2, 3, 4};
#define	DMCR_T_RAS_PRECHARGE(x)		(((x)-1)<<DMCR_TPC_BIT)			// NOTE: The x = {1, 2, 3, 4, 5, 6, 7, 8};
#define	DMCR_T_WRITE_PRECHARGE(x)	(((x)-1)<<DMCR_TRWL_BIT)		// NOTE: The x = {1, 2, 3, 4};
#define	DMCR_T_RAS_CYCLE(x)			((((x)-1)/2)<<DMCR_TRC_BIT)		// NOTE: The x = {1, 3, 5, 7, 9, 11, 13, 15};
#define	DMCR_T_CAS_LATENCY(x)		(((x)-1)<<DMCR_TCL_BIT)			// NOTE: The x = {2, 3};

//--------------------------------------------------------------------------
// SMCR Common Define
//--------------------------------------------------------------------------
#define	EMC_SMCR_STRV_BIT	24
#define	EMC_SMCR_TAW_BIT	20
#define	EMC_SMCR_TBP_BIT	16
#define	EMC_SMCR_TAH_BIT	12
#define	EMC_SMCR_TAS_BIT	8

#define	EMC_SMCR_DEFAULT_VALUE		0x0FFF7700

//--------------------------------------------------------------------------
// DMAR Common Define
//--------------------------------------------------------------------------
#define EMC_DMAR_BASE_BIT	8
#define EMC_DMAR_MASK_BIT	0

#define EMC_DMAR_BASE_MASK	(0xff << EMC_DMAR_BASE_BIT)
#define EMC_DMAR_MASK_MASK	(0xff << EMC_DMAR_MASK_BIT)

#define EMC_DMAR1_BASE		(0x20 << EMC_DMAR_BASE_BIT)
#define EMC_DMAR1_BASE_64M	(0x24 << EMC_DMAR_BASE_BIT)
#define EMC_DMAR1_BASE_128M	(0x28 << EMC_DMAR_BASE_BIT)

#define EMC_DMAR_MASK_64_64		(0xfc << EMC_DMAR_MASK_BIT)
#define EMC_DMAR_MASK_128_128	(0xf8 << EMC_DMAR_MASK_BIT)


//--------------------------------------------------------------------------
// Refresh Timer Control/Status Register(RTCSR) Common Define
//--------------------------------------------------------------------------
#define RTCSR_SRF			( 1 << 8 )
#define RTCSR_CMF			( 1 << 7 )
#define RTCSR_CKO_DIV_16		2
#define RTCSR_CKO_DIV_64		3
#define RTCSR_CKO_DIV_256		4
#define RTCSR_CKO_DIV_1024		5
#define RTCSR_CKO_DIV_2048		6
#define RTCSR_CKO_DIV_4096		7

//--------------------------------------------------------------------------
// NAND Flash Control/Status(NFCSR) Common Define
//--------------------------------------------------------------------------
#define	NFCSR_NFCE4			( 1 << 7 )
#define	NFCSR_NFE4			( 1 << 6 )
#define	NFCSR_NFCE3			( 1 << 5 )
#define	NFCSR_NFE3			( 1 << 4 )
#define	NFCSR_NFCE2			( 1 << 3 )
#define	NFCSR_NFE2			( 1 << 2 )
#define	NFCSR_NFCE1			( 1 << 1 )
#define	NFCSR_NFE1			( 1 << 0 )


//--------------------------------------------------------------------------
// SDMR Common Define
//--------------------------------------------------------------------------
#define SDMR_BM		(1 << 9) /* Write Burst Mode */

#define SDMR_OM_BIT		7        /* Operating Mode */
#define SDMR_OM_MASK	(3 << SDMR_OM_BIT)
#define SDMR_OM_NORMAL	(0 << SDMR_OM_BIT)

#define SDMR_CAS_BIT	4        /* CAS Latency */
#define SDMR_CAS_MASK	(7 << SDMR_CAS_BIT)
  #define SDMR_CAS_1	(1 << SDMR_CAS_BIT)
  #define SDMR_CAS_2	(2 << SDMR_CAS_BIT)
  #define SDMR_CAS_3	(3 << SDMR_CAS_BIT)

#define SDMR_BT_BIT		3        /* Burst Type */
#define SDMR_BT_MASK	(1 << SDMR_BT_BIT)
  #define SDMR_BT_SEQ	(0 << SDMR_BT_BIT) /* Sequential */
  #define SDMR_BT_INT	(1 << SDMR_BT_BIT) /* Interleave */

#define SDMR_BL_BIT		0        /* Burst Length */
#define SDMR_BL_MASK	(7 << SDMR_BL_BIT)
  #define SDMR_BL_1		(0 << SDMR_BL_BIT)
  #define SDMR_BL_2		(1 << SDMR_BL_BIT)
  #define SDMR_BL_4		(2 << SDMR_BL_BIT)
  #define SDMR_BL_8		(3 << SDMR_BL_BIT)


#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int	BCR;			//	0x00
	unsigned int	EMCRSV00[4];	//	0x04, 0x08, 0x0C, 0x10
	unsigned int	SMCR1;			//	0x14
	unsigned int	SMCR2;			//	0x18
	unsigned int	SMCR3;			//	0x1C
	unsigned int	SMCR4;			//	0x20
	unsigned int	EMCRSV01[4];	//	0x24, 0x28, 0x2C, 0x30
	unsigned int	SACR1;			//	0x34
	unsigned int	SACR2;			//	0x38
	unsigned int	SACR3;			//	0x3C
	unsigned int	SACR4;			//	0x40
	unsigned int	EMCRSV02[3];	//	0x44, 0x48, 0x4C
	unsigned int	NFCSR;			//	0x50

	unsigned int	EMCRSV03[4];	//	0x54, 0x58, 0x5C, 0x60
	unsigned int	EMCRSV04[4];	//	0x64, 0x68, 0x6C, 0x70
	unsigned int	EMCRSV05[3];	//	0x74, 0x78, 0x7C

	unsigned int	DMCR;			//	0x80
	unsigned int	RTCSR;			//	0x84
	unsigned int	RTCNT;			//	0x88
	unsigned int	RTCOR;			//	0x8C
	unsigned int	DMAR1;			//	0x90
	unsigned int	DMAR2;			//	0x94

} JZ4755_EMC, *PJZ4755_EMC;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755EMC_H__
