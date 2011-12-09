//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755UART_H__
#define __JZ4755UART_H__

//--------------------------------------------------------------------------
// Universal Asynchronous Receiver&Transmitter(UART) Address Definition
//--------------------------------------------------------------------------
#define UART_PHYS_ADDR			( 0x10030000 )
#define UART_BASE_U_VIRTUAL		( 0xB0030000 )

// UART0
#define UART0_PHYS_ADDR			( 0x10030000 )
#define UART0_BASE_U_VIRTUAL	( 0xB0030000 )
// UART1
#define UART1_PHYS_ADDR			( 0x10031000 )
#define UART1_BASE_U_VIRTUAL	( 0xB0031000 )
// UART2
#define UART2_PHYS_ADDR			( 0x10032000 )
#define UART2_BASE_U_VIRTUAL	( 0xB0032000 )
// UART3
#define UART3_PHYS_ADDR			( 0x10033000 )
#define UART3_BASE_U_VIRTUAL	( 0xB0033000 )

#define	UART_NUM				4

//--------------------------------------------------------------------------
// UART Registers Offset Definition
//--------------------------------------------------------------------------
#define	UART_URBR_OFFSET		( 0x00 )	//  R, 8, 0x??
#define	UART_UTHR_OFFSET		( 0x00 )	//  W, 8, 0x??
#define	UART_UDLLR_OFFSET		( 0x00 )	// RW, 8, 0x00
#define	UART_UDLHR_OFFSET		( 0x04 )	// RW, 8, 0x00
#define	UART_UIER_OFFSET		( 0x04 )	// RW, 8, 0x00
#define	UART_UIIR_OFFSET		( 0x08 )	//  R, 8, 0x01
#define	UART_UFCR_OFFSET		( 0x08 )	//  W, 8, 0x00
#define	UART_ULCR_OFFSET		( 0x0C )	// RW, 8, 0x00
#define	UART_UMCR_OFFSET		( 0x10 )	// RW, 8, 0x00
#define	UART_ULSR_OFFSET		( 0x14 )	//  R, 8, 0x00
#define	UART_UMSR_OFFSET		( 0x18 )	//  R, 8, 0x00
#define	UART_USPR_OFFSET		( 0x1C )	// RW, 8, 0x00
#define	UART_UISR_OFFSET		( 0x20 )	// RW, 8, 0x00

//--------------------------------------------------------------------------
// UART Registers Address Definition
//--------------------------------------------------------------------------
 
#define	A_UART_URBR		( UART_BASE_U_VIRTUAL + UART_URBR_OFFSET  )	
#define	A_UART_UTHR		( UART_BASE_U_VIRTUAL + UART_UTHR_OFFSET  )	
#define	A_UART_UDLLR	( UART_BASE_U_VIRTUAL + UART_UDLLR_OFFSET )	
#define	A_UART_UDLHR	( UART_BASE_U_VIRTUAL + UART_UDLHR_OFFSET )	
#define	A_UART_UIER		( UART_BASE_U_VIRTUAL + UART_UIER_OFFSET  )	
#define	A_UART_UIIR		( UART_BASE_U_VIRTUAL + UART_UIIR_OFFSET  )	
#define	A_UART_UFCR		( UART_BASE_U_VIRTUAL + UART_UFCR_OFFSET  )	
#define	A_UART_ULCR		( UART_BASE_U_VIRTUAL + UART_ULCR_OFFSET  )	
#define	A_UART_UMCR		( UART_BASE_U_VIRTUAL + UART_UMCR_OFFSET  )	
#define	A_UART_ULSR		( UART_BASE_U_VIRTUAL + UART_ULSR_OFFSET  )	
#define	A_UART_UMSR		( UART_BASE_U_VIRTUAL + UART_UMSR_OFFSET  )	
#define	A_UART_USPR		( UART_BASE_U_VIRTUAL + UART_USPR_OFFSET  )	
#define	A_UART_UISR		( UART_BASE_U_VIRTUAL + UART_UISR_OFFSET  )	
 
//--------------------------------------------------------------------------
// UART Common Define
//--------------------------------------------------------------------------
// UART Interrupt Enable Register(UIER) Common Define
#define UIER_RTOIE			( 1 << 4 )
#define UIER_MSIE			( 1 << 3 )
#define UIER_RLSIE			( 1 << 2 )
#define UIER_TDRIE			( 1 << 1 )
#define UIER_RDRIE			( 1 << 0 )

// UART Interrupt Identification Register(UIIR) Common Define
#define UIIR_INPEND			( 1 << 0 )

#define SHIFT_FFMSEL		6
#define SHIFT_INID			1

#define	UIIR_FFMSEL_MASK	( 0x3 << SHIFT_FFMSEL )
#define	UIIR_INID_MASK		( 0x7 << SHIFT_INID )

#define UIIR_INID_MS    (0<<1)
#define UIIR_INID_TB    (1<<1)
#define UIIR_INID_RD    (2<<1)
#define UIIR_INID_LS    (3<<1)
#define UIIR_INID_TO    (6<<1)

// UART FIFO Control Register(UFCR) Common Define
#define	UFCR_RDTR_1			( 0x3 << 6 )
#define	UFCR_RDTR_4			( 0x2 << 6 )
#define	UFCR_RDTR_8			( 0x1 << 6 )
#define	UFCR_RDTR_15		( 0x0 << 6 )
#define UFCR_UME			( 0x1 << 4 )
#define	UFCR_DME			( 0x1 << 3 )
#define	UFCR_TFRT			( 0x1 << 2 )
#define	UFCR_RFRT			( 0x1 << 1 )
#define	UFCR_FME			( 0x1 << 0 )

// UART Line Control Register(ULCR) Common Define
#define ULCR_DLAB			( 0x1 << 7 )
#define ULCR_SBK			( 0x1 << 6 )
#define ULCR_STPAR			( 0x1 << 5 )
#define ULCR_PARM_EVEN		( 0x1 << 4 )
#define ULCR_PARM_ODD		( 0x0 << 4 )
#define ULCR_PARE			( 0x1 << 3 )
#define ULCR_PARE_MASK		( 0x3 << 3 )
#define ULCR_SBLS_1BIT		( 0x0 << 2 )
#define ULCR_SBLS_2BITS		( 0x1 << 2 )
//#define ULCR_SBLS_MASK		( 0x3 << 2 )


#define ULCR_WLS_MASK		( 0x3 << 0 )
#define ULCR_WLS_5BITS		( 0x0 << 0 )
#define ULCR_WLS_6BITS		( 0x1 << 0 )
#define ULCR_WLS_7BITS		( 0x2 << 0 )
#define ULCR_WLS_8BITS		( 0x3 << 0 )

// UART Modem Control Register(UMCR) Common Define
#define	UMCR_MDCE			( 0x1 << 7 )
#define	UMCR_LOOP			( 0x1 << 4 )
#define	UMCR_RTS			( 0x1 << 1 )

// UART Line Status Register(ULSR) Common Define
#define	ULSR_FIFOE			( 0x1 << 7 )
#define	ULSR_TEMP			( 0x1 << 6 )
#define	ULSR_TDRQ			( 0x1 << 5 )
#define	ULSR_BI				( 0x1 << 4 )
#define	ULSR_FMER			( 0x1 << 3 )
#define	ULSR_PARER			( 0x1 << 2 )
#define	ULSR_OVER			( 0x1 << 1 )
#define	ULSR_DRY			( 0x1 << 0 )

//	JZ47 not define UMSR_DDSR UMSR_DRI UMSR_DDCD  UMSR_DSR UMSR_RI UMSR_DCD
#define UMSR_CCTS		(1 << 0)
#define UMSR_DDSR		(1 << 1)
#define UMSR_DRI		(1 << 2)
#define UMSR_DDCD		(1 << 3)
#define UMSR_CTS		(1 << 4)
#define UMSR_DSR		(1 << 5)
#define UMSR_RI			(1 << 6)
#define UMSR_DCD		(1 << 7)

// Infrared Selection Register
#define UISR_RDPL		(1 << 4)
#define UISR_TDPL		(1 << 3)
#define UISR_XMODE		(1 << 2)
#define UISR_RCVEIR		(1 << 1)
#define UISR_XMITIR		(1 << 0)

//--------------------------------------------------------------------------
//	Universal Asynchronous Receiver&Transmitter(UART) Init Macro
#define UART_LINECTRL		( ULCR_WLS_8BITS | ULCR_SBLS_1BIT )
#define UART_FIFOCTRL		( UFCR_FME | UFCR_RFRT | UFCR_TFRT | UFCR_UME | UFCR_RDTR_15 )

// UART GPIO DEFINE
#define UART_GPIO_U_BASE		( GPIO_PORT_D_BASE_U_VIRTUAL )

//#define	UART0_TXD		GPIO_25
//#define	UART0_RXD		GPIO_26

//#define	UART1_TXD		GPIO_31
//#define	UART1_RXD		GPIO_30

#ifndef __MIPS_ASSEMBLER

#define LCR_INIT_UART(x)	OUTREG8( (PVOID)((x) + UART_ULCR_OFFSET), UART_LINECTRL )
#define FCR_INIT_UART(x)	OUTREG8( (PVOID)((x) + UART_UFCR_OFFSET), UART_FIFOCTRL )

#define SET_BAUDRATE_UART(x)													\
	SETREG8( (PVOID)((x) + UART_ULCR_OFFSET), ULCR_DLAB );							\
	OUTREG8( (PVOID)((x) + UART_UDLLR_OFFSET), UART_DIVISOR_LATCH & 0xFF );			\
	OUTREG8( (PVOID)((x) + UART_UDLHR_OFFSET), (UART_DIVISOR_LATCH >> 8) & 0xFF );	\
	CLRREG8( (PVOID)((x) + UART_ULCR_OFFSET), ULCR_DLAB )

#define UART0_GPIO_INIT()							\
do													\
{													\
	OUTREG32(A_GPIO_PXFUNS(3), 0x30000000);			\
	OUTREG32(A_GPIO_PXSELC(3), 0x30000000);			\
	OUTREG32(A_GPIO_PXTRGC(3) ,0x30000000);			\
} while (0)

#define UART1_GPIO_INIT()							\
do													\
{													\
	OUTREG32(A_GPIO_PXFUNS(4), 0x02800000);			\
	OUTREG32(A_GPIO_PXSELS(4), 0x02800000);			\
	OUTREG32(A_GPIO_PXTRGC(4) ,0x02800000);			\
} while (0)

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755UART_H__
