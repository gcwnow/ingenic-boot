//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755CPM_H__
#define __JZ4755CPM_H__

//--------------------------------------------------------------------------
// Clock Reset and Power Controller Module(CPM) Address Definition
//--------------------------------------------------------------------------
#define CPM_PHYS_ADDR			( 0x10000000 )
#define CPM_BASE_U_VIRTUAL		( 0xB0000000 )

//--------------------------------------------------------------------------
// CPM Registers Offset Definition
//--------------------------------------------------------------------------
#define CPM_CPCCR_OFFSET		( 0x00 )	// RW, 32, 0x42040000, Clock Control Register
#define CPM_LCR_OFFSET			( 0x04 )	// RW, 32, 0x000000F8, Low Power Control Register
#define CPM_RSR_OFFSET			( 0x08 )	// RW, 32, 0x????????, Reset Status Register
#define CPM_CPPCR_OFFSET		( 0x10 )	// RW, 32, 0x28080011, PLL Control Register
#define CPM_CPPSR_OFFSET		( 0x14 )	// RW, 32, 0x80000000, PLL Switch and Status Register
#define CPM_CLKGR_OFFSET		( 0x20 )	// RW, 32, 0x00000000, Clock Gate Register
#define CPM_OPCR_OFFSET			( 0x24 )	// RW, 32, 0x00001592, Oscillator and Power Control Register
#define CPM_I2SCDR_OFFSET		( 0x60 )	// RW, 32, 0x00000004, I2S device clock divider Register
#define CPM_LPCDR_OFFSET		( 0x64 )	// RW, 32, 0x00000004, LCD pix clock divider Register
#define CPM_MSCCDR_OFFSET		( 0x68 )	// RW, 32, 0x00000004, MSC device clock divider Register
#define CPM_UHCCDR_OFFSET		( 0x6C )	// RW, 32, 0x00000004, UHC 48M clock divider Register
#define CPM_UHCTST_OFFSET		( 0x70 )	// RW, 32, 0x00000000, UHC PHY test point register
#define CPM_SSICDR_OFFSET		( 0x74 )	// RW, 32, 0x00000000, SSI clock divider Register
//#define CPM_MSC1CDR_OFFSET	( 0x78 )	// RW, 32, 0x28080011, MSC1 deviceclock divider Register
//#define CPM_PCMCDR_OFFSET		( 0x7c )	// RW, 32, 0x28080011, PCM device clock divider Register
#define CPM_CIMCDR_OFFSET		( 0x7c )	// RW, 32, 0x00000004,, CIM device clock divider Register

//--------------------------------------------------------------------------
// CPM Registers Address Definition
//--------------------------------------------------------------------------
#define A_CPM_CPCCR			( CPM_BASE_U_VIRTUAL + CPM_CPCCR_OFFSET )
#define A_CPM_LCR			( CPM_BASE_U_VIRTUAL + CPM_LCR_OFFSET )
#define A_CPM_RSR			( CPM_BASE_U_VIRTUAL + CPM_RSR_OFFSET )
#define A_CPM_CPPCR			( CPM_BASE_U_VIRTUAL + CPM_CPPCR_OFFSET )
#define A_CPM_CPPSR			( CPM_BASE_U_VIRTUAL + CPM_CPPSR_OFFSET )
#define A_CPM_CLKGR			( CPM_BASE_U_VIRTUAL + CPM_CLKGR_OFFSET )
#define A_CPM_OPCR			( CPM_BASE_U_VIRTUAL + CPM_OPCR_OFFSET )
#define A_CPM_I2SCDR		( CPM_BASE_U_VIRTUAL + CPM_I2SCDR_OFFSET )
#define A_CPM_LPCDR			( CPM_BASE_U_VIRTUAL + CPM_LPCDR_OFFSET )
#define A_CPM_MSCCDR		( CPM_BASE_U_VIRTUAL + CPM_MSCCDR_OFFSET )
#define A_CPM_UHCCDR		( CPM_BASE_U_VIRTUAL + CPM_UHCCDR_OFFSET )
#define A_CPM_UHCTST		( CPM_BASE_U_VIRTUAL + CPM_UHCTST_OFFSET )
#define A_CPM_SSICDR		( CPM_BASE_U_VIRTUAL + CPM_SSICDR_OFFSET )
//#define A_CPM_MSC1CDR		( CPM_BASE_U_VIRTUAL + CPM_MSC1CDR_OFFSET )
//#define A_CPM_PCMCDR		( CPM_BASE_U_VIRTUAL + CPM_PCMCDR_OFFSET )
#define A_CPM_CIMCDR		( CPM_BASE_U_VIRTUAL + CPM_CIMCDR_OFFSET )

//--------------------------------------------------------------------------
// Clock Control Register(CPCCR) Common Define
//--------------------------------------------------------------------------
#define CPCCR_I2S_EXTCLK	( 0 << 31 )
#define CPCCR_I2S_PLLCLK	( 1 << 31 )
#define CPCCR_ECS_EXCLK		( 0 << 30 )
#define CPCCR_ECS_EXCLK_DIV2	( 1 << 30 )
#define CPCCR_USB_EXTCLK	( 0 << 29 )
#define CPCCR_USB_PLLCLK	( 1 << 29 )
#define CPCCR_CHANGE_EN		( 1 << 22 )
#define CPCCR_PLLCLK_DIV2	( 1 << 21 )		// The clock is supplied to MSC, I2S, LCD and USB

#define CPCCR_UDIV_BIT		( 23 )
#define CPCCR_H1DIV_BIT		( 16 )
#define CPCCR_MDIV_BIT		( 12 )
#define CPCCR_PDIV_BIT		( 8 )
#define CPCCR_H0DIV_BIT		( 4 )
#define CPCCR_CDIV_BIT		( 0 )

#define CPCCR_UDIV_MASK		( 0x3F << CPCCR_UDIV_BIT )
#define CPCCR_H1DIV_MASK		( 0xF << CPCCR_H1DIV_BIT )
#define CPCCR_MDIV_MASK		( 0xF << CPCCR_MDIV_BIT )
#define CPCCR_PDIV_MASK		( 0xF << CPCCR_PDIV_BIT )
#define CPCCR_H0DIV_MASK		( 0xF << CPCCR_H0DIV_BIT )
#define CPCCR_CDIV_MASK		( 0xF << CPCCR_CDIV_BIT )

#define	XDIV_1				( 0x00 )	// XDIV is used for MDIV, PDIV, HDIV and CDIV
#define	XDIV_2				( 0x01 )
#define	XDIV_3				( 0x02 )
#define	XDIV_4				( 0x03 )
#define	XDIV_6				( 0x04 )
#define	XDIV_8				( 0x05 )
#define	XDIV_12				( 0x06 )
#define	XDIV_16				( 0x07 )
#define	XDIV_24				( 0x08 )
#define	XDIV_32				( 0x09 )

//--------------------------------------------------------------------------
// I2S device clock divider(I2SCDR) Common Define
//--------------------------------------------------------------------------
#define I2SCDR_I2SCDR_MASK		0xff << 0


//--------------------------------------------------------------------------
// PLL Control Register(CPPSR) Common Define
//--------------------------------------------------------------------------
#define	CPPSR_PLLOFF		( 0X1 << 31 )
#define	CPPSR_PLLBP			( 0X1 << 30 )
#define	CPPSR_PLLON			( 0X1 << 29 )
#define	CPPSR_PS			( 0X1 << 28 )
#define	CPPSR_FS			( 0X1 << 27 )
#define	CPPSR_CS			( 0X1 << 26 )
#define	CPPSR_SM			( 0X1 << 2 )
#define	CPPSR_PM			( 0X1 << 1 )
#define	CPPSR_FM			( 0X1 << 0 )



//--------------------------------------------------------------------------
// LCD Common Define
//--------------------------------------------------------------------------
#define LCR_LPM_MASK			( 0x3 << 0 )
#define	LCR_IDLE_MODE			( 0x0 << 0 )
#define	LCR_SLEEP_MODE			( 0x1 << 0 )

//--------------------------------------------------------------------------
// LCD pix clock divider Register(LPCDR) Common Define
//--------------------------------------------------------------------------
#define LPCDR_LSCS				(1 << 31)
#define LPCDR_LTCS				(1 << 30)

#define LPCDR_PIXDIV_BIT		(0)
#define LPCDR_PIXDIV_MASK		(0x7ff << LPCDR_PIXDIV_BIT)

//--------------------------------------------------------------------------
// PLL Control Register(CPPCR) Common Define
//--------------------------------------------------------------------------
#define CPPCR_PLLS			( 1 << 10 )
#define CPPCR_PLLBP			( 1 << 9 )
#define CPPCR_PLLEN			( 1 << 8 )

#define CPPCR_PLLM_BIT		( 23 )
#define CPPCR_PLLN_BIT		( 18 )
#define CPPCR_PLLOD_BIT		( 16 )
#define CPPCR_PLLST_BIT		( 0 )

#define CPPCR_PLLM_MASK			( 0x1FF << CPPCR_PLLM_BIT )
#define CPPCR_PLLN_MASK			( 0x1F << CPPCR_PLLN_BIT )
#define CPPCR_PLLOD_MASK		( 0x3 << CPPCR_PLLOD_BIT )
#define CPPCR_PLLST_MASK		( 0xFF )

// Clock Disable Register(CDR) Common Define
#define CLKGR_STOP_AUX_CPU		( 1 << 24 )
#define CLKGR_STOP_AHB1			( 1 << 23 )
#define CLKGR_STOP_IDCT			( 1 << 22 )
#define CLKGR_STOP_DB			( 1 << 21 )
#define CLKGR_STOP_ME			( 1 << 20 )
#define CLKGR_STOP_MC			( 1 << 19 )
#define CLKGR_STOP_TVE			( 1 << 18 )
#define CLKGR_STOP_TSSI			( 1 << 17 )
#define CLKGR_STOP_MSC1			( 1 << 16 )
#define CLKGR_STOP_UART2		( 1 << 15 )
#define CLKGR_STOP_UART1		( 1 << 14 )
#define CLKGR_STOP_IPU			( 1 << 13 )
#define CLKGR_STOP_DMAC			( 1 << 12 )
#define CLKGR_STOP_BCH			( 1 << 11 )
#define CLKGR_STOP_UDC			( 1 << 10 )
#define CLKGR_STOP_LCD			( 1 << 9 )
#define CLKGR_STOP_CIM			( 1 << 8 )
#define CLKGR_STOP_SADC			( 1 << 7 )
#define CLKGR_STOP_MSC0			( 1 << 6 )
#define CLKGR_STOP_AIC_PCLK		( 1 << 5 )
#define CLKGR_STOP_SSI1			( 1 << 4 )
#define CLKGR_STOP_I2C			( 1 << 3 )
#define CLKGR_STOP_RTC			( 1 << 2 )
#define CLKGR_STOP_TCU			( 1 << 1 )
#define CLKGR_STOP_UART0		( 1 << 0 )

//--------------------------------------------------------------------------
// OPCR Control Register(SCR) Common Define
//--------------------------------------------------------------------------

#define SCR_NO_SUSPEND_UDC		(1 << 6)
#define SCR_NO_SUSPEND_BPM		(1 << 5)
#define SCR_NO_SUSPEND_OSC		(1 << 4)
#define SCR_NO_SUSPEND_ERCS		(1 << 2)

#define __cpm_start_dmac()	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_DMAC);

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int	CPCCR;			//	0x00
	unsigned int	LCR;			//	0x04
	unsigned int	RSR;			//	0x08
	unsigned int	CPMRSV00[1];	//	0x0C
	unsigned int	CPPCR;			//	0x10
	unsigned int	CPPSR;			//	0x14
	unsigned int	CPMRSV01[2];	//	0x18, 0x1C
	unsigned int	CLKGR;			//	0x20
	unsigned int	OPCR;			//	0x24
	unsigned int	CPMRSV02[14];	//	0x28-0x5C
	unsigned int	I2SCDR;			//	0x60
	unsigned int	LPCDR;			//	0x64
	unsigned int	MSC0CDR;		//	0x68
	unsigned int	UHCCDR;			//	0x6C
	unsigned int	UHCTST;			//	0x70
	unsigned int	SSICDR;			//	0x74
	unsigned int	MSC1CDR;		//	0x78
	unsigned int	PCMCDR;			//	0x7C

} JZ4740_CPM, *PJZ4740_CPM;

typedef enum __CLOCK_DIV__
{
	CPM_CDIV = 0x00,
	CPM_H0DIV = 0x04,
	CPM_PDIV = 0x08,
	CPM_MDIV = 0x0C,
	CPM_H1DIV = 0x10
} CLKDIV, *PCLKDIV;

typedef struct
{
	unsigned int	BootPLL;
	unsigned int	BootPLLM;

	unsigned int	PLL;
	unsigned int	CPU;
	unsigned int	SYS;
	unsigned int	MEM;
	unsigned int	PER;
	unsigned int	LCD;
	unsigned int	USB;

	unsigned int	PLLM;
	unsigned int	PLLN;
	unsigned int	PLLOD;
	unsigned int	CDIV;
	unsigned int	HDIV;
	unsigned int	PDIV;
	unsigned int	MDIV;
	unsigned int	LDIV;
	unsigned int	UDIV;
} CLOCK_INFO, *PCLOCK_INFO;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4740CPM_H__
