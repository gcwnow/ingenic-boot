//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZIO_H__
#define __JZIO_H__

#ifndef __MIPS_ASSEMBLER

//------------------------------------------------------------------------------
//
//  Macros:  INREGx/OUTREGx/SETREGx/CLRREGx
//
//  This macros encapsulates basic I/O operations. Depending on OAL_DDK_NOMACRO
//  definition they will expand to direct memory operation or function call.
//  Memory address space operation is used on all platforms.
//

#define BIT0  	0x00000001
#define BIT1  	0x00000002
#define BIT2  	0x00000004		
#define BIT3  	0x00000008		
#define BIT4  	0x00000010		
#define BIT5  	0x00000020		
#define BIT6  	0x00000040		
#define BIT7  	0x00000080		
#define BIT8  	0x00000100		
#define BIT9  	0x00000200		
#define BIT10 	0x00000400		
#define BIT11 	0x00000800			
#define BIT12 	0x00001000			
#define BIT13 	0x00002000			
#define BIT14 	0x00004000			
#define BIT15 	0x00008000			
#define BIT16 	0x00010000			
#define BIT17 	0x00020000			
#define BIT18 	0x00040000			
#define BIT19 	0x00080000			
#define BIT20 	0x00100000			
#define BIT21 	0x00200000			
#define BIT22 	0x00400000			
#define BIT23 	0x00800000			
#define BIT24 	0x01000000			
#define BIT25 	0x02000000			
#define BIT26 	0x04000000			
#define BIT27 	0x08000000			
#define BIT28 	0x10000000			
#define BIT29 	0x20000000			
#define BIT30 	0x40000000			
#define BIT31 	0x80000000			


#define INREG8(x)           ( (unsigned char)(*(volatile unsigned char * const)(x)) )
#define OUTREG8(x, y)       *(volatile unsigned char * const)(x) = (y)
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))

#define INREG16(x)           ( (unsigned short)(*(volatile unsigned short * const)(x)) )
#define OUTREG16(x, y)       *(volatile unsigned short * const)(x) = (y)
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))

#define INREG32(x)           ( (unsigned int)(*(volatile unsigned int* const)(x)) )
#define OUTREG32(x, y)       *(volatile unsigned int * const)(x) = (y)
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))


#endif	// __MIPS_ASSEMBLER


#endif // __JZIO_H__
