//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755GPIO_H__
#define __JZ4755GPIO_H__

//--------------------------------------------------------------------------
// General Purpose IO(GPIO) Address Definition
//--------------------------------------------------------------------------
#define GPIO_PHYS_ADDR			( 0x10010000 )
#define GPIO_BASE_U_VIRTUAL		( 0xB0010000 )

#define GPIO_PORT_A_PHYS_ADDR			( GPIO_PHYS_ADDR )
#define GPIO_PORT_A_BASE_U_VIRTUAL		( GPIO_BASE_U_VIRTUAL )

#define GPIO_PORT_B_PHYS_ADDR			( 0x10010100 )
#define GPIO_PORT_B_BASE_U_VIRTUAL		( 0xB0010100 )

#define GPIO_PORT_C_PHYS_ADDR			( 0x10010200 )
#define GPIO_PORT_C_BASE_U_VIRTUAL		( 0xB0010200 )

#define GPIO_PORT_D_PHYS_ADDR			( 0x10010300 )
#define GPIO_PORT_D_BASE_U_VIRTUAL		( 0xB0010300 )

#define GPIO_PORT_E_PHYS_ADDR			( 0x10010400 )
#define GPIO_PORT_E_BASE_U_VIRTUAL		( 0xB0010400 )

#define GPIO_PORT_F_PHYS_ADDR			( 0x10010500 )
#define GPIO_PORT_F_BASE_U_VIRTUAL		( 0xB0010500 )

//--------------------------------------------------------------------------
// GPIO Registers Offset Definition
//--------------------------------------------------------------------------
#define GPIO_PXPIN_OFFSET		( 0x00 )	//  R, 32, 0x00000000
#define GPIO_PXDAT_OFFSET		( 0x10 )	//  R, 32, 0x00000000
#define GPIO_PXDATS_OFFSET		( 0x14 )	//  W, 32, 0x????????
#define GPIO_PXDATC_OFFSET		( 0x18 )	//  W, 32, 0x????????
#define GPIO_PXIM_OFFSET		( 0x20 )	//  R, 32, 0xFFFFFFFF
#define GPIO_PXIMS_OFFSET		( 0x24 )	//  W, 32, 0x????????
#define GPIO_PXIMC_OFFSET		( 0x28 )	//  W, 32, 0x????????
#define GPIO_PXPE_OFFSET		( 0x30 )	//  R, 32, 0x00000000
#define GPIO_PXPES_OFFSET		( 0x34 )	//  W, 32, 0x????????
#define GPIO_PXPEC_OFFSET		( 0x38 )	//  W, 32, 0x????????
#define GPIO_PXFUN_OFFSET		( 0x40 )	//  R, 32, 0x00000000
#define GPIO_PXFUNS_OFFSET		( 0x44 )	//  W, 32, 0x????????
#define GPIO_PXFUNC_OFFSET		( 0x48 )	//  W, 32, 0x????????
#define GPIO_PXSEL_OFFSET		( 0x50 )	//  R, 32, 0x00000000
#define GPIO_PXSELS_OFFSET		( 0x54 )	//  W, 32, 0x????????
#define GPIO_PXSELC_OFFSET		( 0x58 )	//  W, 32, 0x????????
#define GPIO_PXDIR_OFFSET		( 0x60 )	//  R, 32, 0x00000000
#define GPIO_PXDIRS_OFFSET		( 0x64 )	//  W, 32, 0x????????
#define GPIO_PXDIRC_OFFSET		( 0x68 )	//  W, 32, 0x????????
#define GPIO_PXTRG_OFFSET		( 0x70 )	//  R, 32, 0x00000000
#define GPIO_PXTRGS_OFFSET		( 0x74 )	//  W, 32, 0x????????
#define GPIO_PXTRGC_OFFSET		( 0x78 )	//  W, 32, 0x????????
#define GPIO_PXFLG_OFFSET		( 0x80 )	//  R, 32, 0x00000000
#define GPIO_PXFLGC_OFFSET		( GPIO_PXDATS_OFFSET )	//  W, 32, 0x????????
//#define GPIO_PXFLGC_OFFSET		( GPIO_PXDATC_OFFSET )	//  W, 32, 0x????????

//--------------------------------------------------------------------------
// GPIO Registers Address Definition
//--------------------------------------------------------------------------
#define A_GPIO_PXPIN(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXPIN_OFFSET )
#define A_GPIO_PXDAT(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXDAT_OFFSET )
#define A_GPIO_PXDATS(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXDATS_OFFSET )
#define A_GPIO_PXDATC(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXDATC_OFFSET )
#define A_GPIO_PXIM(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXIM_OFFSET )
#define A_GPIO_PXIMS(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXIMS_OFFSET )
#define A_GPIO_PXIMC(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXIMC_OFFSET )
#define A_GPIO_PXPE(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXPE_OFFSET )
#define A_GPIO_PXPES(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXPES_OFFSET )
#define A_GPIO_PXPEC(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXPEC_OFFSET )
#define A_GPIO_PXFUN(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXFUN_OFFSET )
#define A_GPIO_PXFUNS(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXFUNS_OFFSET )
#define A_GPIO_PXFUNC(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXFUNC_OFFSET )
#define A_GPIO_PXSEL(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXSEL_OFFSET )
#define A_GPIO_PXSELS(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXSELS_OFFSET )
#define A_GPIO_PXSELC(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXSELC_OFFSET )
#define A_GPIO_PXDIR(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXDIR_OFFSET )
#define A_GPIO_PXDIRS(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXDIRS_OFFSET )
#define A_GPIO_PXDIRC(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXDIRC_OFFSET )
#define A_GPIO_PXTRG(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXTRG_OFFSET )
#define A_GPIO_PXTRGS(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXTRGS_OFFSET )
#define A_GPIO_PXTRGC(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXTRGC_OFFSET )
#define A_GPIO_PXFLG(x)			( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXFLG_OFFSET )
#define A_GPIO_PXFLGC(x)		( GPIO_BASE_U_VIRTUAL + (x) * 0x100 + GPIO_PXFLGC_OFFSET )

// GPIO
#define GPIO_0			0x00000001
#define GPIO_1			0x00000002
#define GPIO_2			0x00000004
#define GPIO_3			0x00000008
#define GPIO_4			0x00000010
#define GPIO_5			0x00000020
#define GPIO_6			0x00000040
#define GPIO_7			0x00000080
#define GPIO_8			0x00000100
#define GPIO_9			0x00000200
#define GPIO_10			0x00000400
#define GPIO_11			0x00000800
#define GPIO_12			0x00001000
#define GPIO_13			0x00002000
#define GPIO_14			0x00004000
#define GPIO_15			0x00008000
#define GPIO_16			0x00010000
#define GPIO_17			0x00020000
#define GPIO_18			0x00040000
#define GPIO_19			0x00080000
#define GPIO_20			0x00100000
#define GPIO_21			0x00200000
#define GPIO_22			0x00400000
#define GPIO_23			0x00800000
#define GPIO_24			0x01000000
#define GPIO_25			0x02000000
#define GPIO_26			0x04000000
#define GPIO_27			0x08000000
#define GPIO_28			0x10000000
#define GPIO_29			0x20000000
#define GPIO_30			0x40000000
#define GPIO_31			0x80000000

#define UNUSED_GPIO_PIN		0xFFFFFFFF
#define GPIO_GROUP_NUM		0x6

#define	GPIO_GROUP_A	0x00
#define	GPIO_GROUP_B	0x20
#define	GPIO_GROUP_C	0x40
#define	GPIO_GROUP_D	0x60
#define	GPIO_GROUP_E	0x80
#define	GPIO_GROUP_F	0xA0

#define NUM_GPIO (GPIO_GROUP_NUM * 32)

#define GPIO_HIGH_LEVEL		1
#define GPIO_LOW_LEVEL		0

//
// Power Pin define, do NOT change!
//
#define POWER_OFF_PIN		(GPIO_GROUP_E + 30)

#define CS2_PIN					(GPIO_GROUP_C + 17)
#define CS2_GROUP				2
#define CS2_BIT					GPIO_17

#ifndef __MIPS_ASSEMBLER

typedef enum
{
	GITT_L_LEVEL = 0x00,
	GITT_H_LEVEL = 0x01,
	GITT_F_EDGE = 0x02,
	GITT_R_EDGE = 0x03,
	GITT_MAX = 0x04,		// Just a value of the boundary
}GPIO_INTR_TRIGGER_TYPE;

typedef volatile struct
{
	// Group A
	unsigned int	PAPIN;			//	0x00
	unsigned int	PARSV00[3];		//	0x04, 0x08, 0x0C

	unsigned int	PADAT;				//	0x10
	unsigned int	PADATS;				//	0x14
	unsigned int	PADATC;				//	0x18
	unsigned int	PARSV01[1];			//	0x1C

	unsigned int	PAIM;			//	0x20
	unsigned int	PAIMS;			//	0x24
	unsigned int	PAIMC;			//	0x28
	unsigned int	PARSV02[1];		//	0x2C

	unsigned int	PAPE;				//	0x30
	unsigned int	PAPES;				//	0x34
	unsigned int	PAPEC;				//	0x38
	unsigned int	PARSV03[1];			//	0x3C

	unsigned int	PAFUN;			//	0x40
	unsigned int	PAFUNS;			//	0x44
	unsigned int	PAFUNC;			//	0x48
	unsigned int	PARSV04[1];		//	0x4C

	unsigned int	PASEL;				//	0x50
	unsigned int	PASELS;				//	0x54
	unsigned int	PASELC;				//	0x58
	unsigned int	PARSV05[1];			//	0x5C

	unsigned int	PADIR;			//	0x60
	unsigned int	PADIRS;			//	0x64
	unsigned int	PADIRC;			//	0x68
	unsigned int	PARSV06[1];		//	0x6C

	unsigned int	PATRG;				//	0x70
	unsigned int	PATRGS;				//	0x74
	unsigned int	PATRGC;				//	0x78
	unsigned int	PARSV07[1];			//	0x7C

	unsigned int	PAFLG;			//	0x80
	unsigned int	PAFLGC;			//	0x84
	unsigned int	PARSV08[2];		//	0x88, 0x8C
	unsigned int	PARSV09[4];		//	0x90, 0x94, 0x98, 0x9C

	unsigned int	PARSV10[24];	//	0xAX, 0xBX, 0xCX, 0xDX, 0xEX, 0xFX

	// Group B
	unsigned int	PBPIN;			//	0x00
	unsigned int	PBRSV00[3];		//	0x04, 0x08, 0x0C

	unsigned int	PBDAT;				//	0x10
	unsigned int	PBDATS;				//	0x14
	unsigned int	PBDATC;				//	0x18
	unsigned int	PBRSV01[1];			//	0x1C

	unsigned int	PBIM;			//	0x20
	unsigned int	PBIMS;			//	0x24
	unsigned int	PBIMC;			//	0x28
	unsigned int	PBRSV02[1];		//	0x2C

	unsigned int	PBPE;				//	0x30
	unsigned int	PBPES;				//	0x34
	unsigned int	PBPEC;				//	0x38
	unsigned int	PBRSV03[1];			//	0x3C

	unsigned int	PBFUN;			//	0x40
	unsigned int	PBFUNS;			//	0x44
	unsigned int	PBFUNC;			//	0x48
	unsigned int	PBRSV04[1];		//	0x4C

	unsigned int	PBSEL;				//	0x50
	unsigned int	PBSELS;				//	0x54
	unsigned int	PBSELC;				//	0x58
	unsigned int	PBRSV05[1];			//	0x5C

	unsigned int	PBDIR;			//	0x60
	unsigned int	PBDIRS;			//	0x64
	unsigned int	PBDIRC;			//	0x68
	unsigned int	PBRSV06[1];		//	0x6C

	unsigned int	PBTRG;				//	0x70
	unsigned int	PBTRGS;				//	0x74
	unsigned int	PBTRGC;				//	0x78
	unsigned int	PBRSV07[1];			//	0x7C

	unsigned int	PBFLG;			//	0x80
	unsigned int	PBFLGC;			//	0x84
	unsigned int	PBRSV08[2];		//	0x88, 0x8C
	unsigned int	PBRSV09[4];		//	0x90, 0x94, 0x98, 0x9C

	unsigned int	PBRSV10[24];	//	0xAX, 0xBX, 0xCX, 0xDX, 0xEX, 0xFX

	// Group C
	unsigned int	PCPIN;			//	0x00
	unsigned int	PCRSV00[3];		//	0x04, 0x08, 0x0C

	unsigned int	PCDAT;				//	0x10
	unsigned int	PCDATS;				//	0x14
	unsigned int	PCDATC;				//	0x18
	unsigned int	PCRSV01[1];			//	0x1C

	unsigned int	PCIM;			//	0x20
	unsigned int	PCIMS;			//	0x24
	unsigned int	PCIMC;			//	0x28
	unsigned int	PCRSV02[1];		//	0x2C

	unsigned int	PCPE;				//	0x30
	unsigned int	PCPES;				//	0x34
	unsigned int	PCPEC;				//	0x38
	unsigned int	PCRSV03[1];			//	0x3C

	unsigned int	PCFUN;			//	0x40
	unsigned int	PCFUNS;			//	0x44
	unsigned int	PCFUNC;			//	0x48
	unsigned int	PCRSV04[1];		//	0x4C

	unsigned int	PCSEL;				//	0x50
	unsigned int	PCSELS;				//	0x54
	unsigned int	PCSELC;				//	0x58
	unsigned int	PCRSV05[1];			//	0x5C

	unsigned int	PCDIR;			//	0x60
	unsigned int	PCDIRS;			//	0x64
	unsigned int	PCDIRC;			//	0x68
	unsigned int	PCRSV06[1];		//	0x6C

	unsigned int	PCTRG;				//	0x70
	unsigned int	PCTRGS;				//	0x74
	unsigned int	PCTRGC;				//	0x78
	unsigned int	PCRSV07[1];			//	0x7C

	unsigned int	PCFLG;			//	0x80
	unsigned int	PCFLGC;			//	0x84
	unsigned int	PCRSV08[2];		//	0x88, 0x8C
	unsigned int	PCRSV09[4];		//	0x90, 0x94, 0x98, 0x9C

	unsigned int	PCRSV10[24];	//	0xAX, 0xBX, 0xCX, 0xDX, 0xEX, 0xFX

	// Group D
	unsigned int	PDPIN;			//	0x00
	unsigned int	PDRSV00[3];		//	0x04, 0x08, 0x0C

	unsigned int	PDDAT;				//	0x10
	unsigned int	PDDATS;				//	0x14
	unsigned int	PDDATC;				//	0x18
	unsigned int	PDRSV01[1];			//	0x1C

	unsigned int	PDIM;			//	0x20
	unsigned int	PDIMS;			//	0x24
	unsigned int	PDIMC;			//	0x28
	unsigned int	PDRSV02[1];		//	0x2C

	unsigned int	PDPE;				//	0x30
	unsigned int	PDPES;				//	0x34
	unsigned int	PDPEC;				//	0x38
	unsigned int	PDRSV03[1];			//	0x3C

	unsigned int	PDFUN;			//	0x40
	unsigned int	PDFUNS;			//	0x44
	unsigned int	PDFUNC;			//	0x48
	unsigned int	PDRSV04[1];		//	0x4C

	unsigned int	PDSEL;				//	0x50
	unsigned int	PDSELS;				//	0x54
	unsigned int	PDSELC;				//	0x58
	unsigned int	PDRSV05[1];			//	0x5C

	unsigned int	PDDIR;			//	0x60
	unsigned int	PDDIRS;			//	0x64
	unsigned int	PDDIRC;			//	0x68
	unsigned int	PDRSV06[1];		//	0x6C

	unsigned int	PDTRG;				//	0x70
	unsigned int	PDTRGS;				//	0x74
	unsigned int	PDTRGC;				//	0x78
	unsigned int	PDRSV07[1];			//	0x7C

	unsigned int	PDFLG;			//	0x80
	unsigned int	PDFLGC;			//	0x84
	unsigned int	PDRSV08[2];		//	0x88, 0x8C
	unsigned int	PDRSV09[4];		//	0x90, 0x94, 0x98, 0x9C

	unsigned int	PDRSV10[24];	//	0xAX, 0xBX, 0xCX, 0xDX, 0xEX, 0xFX
	
	unsigned int	PEPIN;			//	0x00
	unsigned int	PERSV00[3];		//	0x04, 0x08, 0x0C
    //  Group E               
	unsigned int	PEDAT;				//	0x10
	unsigned int	PEDATS;				//	0x14
	unsigned int	PEDATC;				//	0x18
	unsigned int	PERSV01[1];			//	0x1C
                   
	unsigned int	PEIM;			//	0x20
	unsigned int	PEIMS;			//	0x24
	unsigned int	PEIMC;			//	0x28
	unsigned int	PERSV02[1];		//	0x2C
                     
	unsigned int	PEPE;				//	0x30
	unsigned int	PEPES;				//	0x34
	unsigned int	PEPEC;				//	0x38
	unsigned int	PERSV03[1];			//	0x3C
                     
	unsigned int	PEFUN;			//	0x40
	unsigned int	PEFUNS;			//	0x44
	unsigned int	PEFUNC;			//	0x48
	unsigned int	PERSV04[1];		//	0x4C
                     
	unsigned int	PESEL;				//	0x50
	unsigned int	PESELS;				//	0x54
	unsigned int	PESELC;				//	0x58
	unsigned int	PERSV05[1];			//	0x5C
                     
	unsigned int	PEDIR;			//	0x60
	unsigned int	PEDIRS;			//	0x64
	unsigned int	PEDIRC;			//	0x68
	unsigned int	PERSV06[1];		//	0x6C
                     
	unsigned int	PETRG;				//	0x70
	unsigned int	PETRGS;				//	0x74
	unsigned int	PETRGC;				//	0x78
	unsigned int	PERSV07[1];			//	0x7C
                     
	unsigned int	PEFLG;			//	0x80
	unsigned int	PEFLGC;			//	0x84
	unsigned int	PERSV08[2];		//	0x88, 0x8C
	unsigned int	PERSV09[4];		//	0x90, 0x94, 0x98, 0x9C
                     
	unsigned int	PERSV10[24];	//	0xAX, 0xBX, 0xCX, 0xDX, 0xEX, 0xFX
	//	Group F
	unsigned int	PFPIN;			//	0x00
	unsigned int	PFRSV00[3];		//	0x04, 0x08, 0x0C
                     
	unsigned int	PFDAT;				//	0x10
	unsigned int	PFDATS;				//	0x14
	unsigned int	PFDATC;				//	0x18
	unsigned int	PFRSV01[1];			//	0x1C
                     
	unsigned int	PFIM;			//	0x20
	unsigned int	PFIMS;			//	0x24
	unsigned int	PFIMC;			//	0x28
	unsigned int	PFRSV02[1];		//	0x2C
                     
	unsigned int	PFPE;				//	0x30
	unsigned int	PFPES;				//	0x34
	unsigned int	PFPEC;				//	0x38
	unsigned int	PFRSV03[1];			//	0x3C
                     
	unsigned int	PFFUN;			//	0x40
	unsigned int	PFFUNS;			//	0x44
	unsigned int	PFFUNC;			//	0x48
	unsigned int	PFRSV04[1];		//	0x4C
                     
	unsigned int	PFSEL;				//	0x50
	unsigned int	PFSELS;				//	0x54
	unsigned int	PFSELC;				//	0x58
	unsigned int	PFRSV05[1];			//	0x5C
                     
	unsigned int	PFDIR;			//	0x60
	unsigned int	PFDIRS;			//	0x64
	unsigned int	PFDIRC;			//	0x68
	unsigned int	PFRSV06[1];		//	0x6C
                     
	unsigned int	PFTRG;				//	0x70
	unsigned int	PFTRGS;				//	0x74
	unsigned int	PFTRGC;				//	0x78
	unsigned int	PFRSV07[1];			//	0x7C
                     
	unsigned int	PFFLG;			//	0x80
	unsigned int	PFFLGC;			//	0x84
	unsigned int	PFRSV08[2];		//	0x88, 0x8C
	unsigned int	PFRSV09[4];		//	0x90, 0x94, 0x98, 0x9C
                     
	unsigned int	PFRSV10[24];	//	0xAX, 0xBX, 0xCX, 0xDX, 0xEX, 0xFX

} JZ4755_GPIO, *PJZ4755_GPIO;

typedef volatile struct
{
	struct gpio_grp_type
	{
		unsigned int	PIN;			//	0x00
		unsigned int	RSV00[3];		//	0x04, 0x08, 0x0C

		unsigned int	DAT;				//	0x10
		unsigned int	DATS;				//	0x14
		unsigned int	DATC;				//	0x18
		unsigned int	RSV01[1];			//	0x1C

		unsigned int	IM;			//	0x20
		unsigned int	IMS;			//	0x24
		unsigned int	IMC;			//	0x28
		unsigned int	RSV02[1];		//	0x2C

		unsigned int	PE;				//	0x30
		unsigned int	PES;				//	0x34
		unsigned int	PEC;				//	0x38
		unsigned int	RSV03[1];			//	0x3C

		unsigned int	FUN;			//	0x40
		unsigned int	FUNS;			//	0x44
		unsigned int	FUNC;			//	0x48
		unsigned int	RSV04[1];		//	0x4C

		unsigned int	SEL;				//	0x50
		unsigned int	SELS;				//	0x54
		unsigned int	SELC;				//	0x58
		unsigned int	RSV05[1];			//	0x5C

		unsigned int	DIR;			//	0x60
		unsigned int	DIRS;			//	0x64
		unsigned int	DIRC;			//	0x68
		unsigned int	RSV06[1];		//	0x6C

		unsigned int	TRG;				//	0x70
		unsigned int	TRGS;				//	0x74
		unsigned int	TRGC;				//	0x78
		unsigned int	RSV07[1];			//	0x7C

		unsigned int	FLG;			//	0x80
		unsigned int	FLGC;			//	0x84
		unsigned int	RSV08[2];		//	0x88, 0x8C
		unsigned int	RSV09[4];		//	0x90, 0x94, 0x98, 0x9C

		unsigned int	RSV10[24];	//	0xAX, 0xBX, 0xCX, 0xDX, 0xEX, 0xFX
	} group[GPIO_GROUP_NUM];
} JZ4755_GPIO2, *PJZ4755_GPIO2;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755GPIO_H__
