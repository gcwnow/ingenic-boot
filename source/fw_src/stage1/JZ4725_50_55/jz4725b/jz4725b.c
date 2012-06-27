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
 *  Author:   <hyhang@ingenic.cn>
 *
 *  Create:   2009-06-27, by hyhang
 *
 *******************************************************************************
 */

#include "jz4725b.h"
#include "../configs.h"

void gpio_as_uart_4725b()
{
	OUTREG32(A_GPIO_PXTRGC(2), 0x00003000);
	OUTREG32(A_GPIO_PXFUNS(2), 0x00003000);
	OUTREG32(A_GPIO_PXSELS(2), 0x00003000);
	OUTREG32(A_GPIO_PXPES(2), 0x00003000);
}

/*
 * D0 ~ D15, A0 ~ A14, DCS0#, RAS#, CAS#, 
 * RDWE#, WE0#, WE1#, WE2#, WE3#, CKO#, CKE#
 */
 
void gpio_as_sdram_16bit_jz4725b()
{
	OUTREG32(A_GPIO_PXFUNS(0), 0x0000FFFF);
	OUTREG32(A_GPIO_PXSELC(0), 0x0000FFFF);
	OUTREG32(A_GPIO_PXPES(0), 0x0000FFFF);

	OUTREG32(A_GPIO_PXFUNS(1), 0x033F7FFF);
	OUTREG32(A_GPIO_PXSELC(1), 0x033F7FFF);
	OUTREG32(A_GPIO_PXPES(1), 0x033F7FFF);
}



void gpio_as_sdram_32bit_jz4725b()
{
	OUTREG32(A_GPIO_PXFUNS(0), 0xffffffff);
	OUTREG32(A_GPIO_PXSELC(0), 0xffffffff);
	OUTREG32(A_GPIO_PXPES(0), 0xffffffff);

	OUTREG32(A_GPIO_PXFUNS(1), 0x03ff7fff);
	OUTREG32(A_GPIO_PXSELC(1), 0x03ff7fff);
	OUTREG32(A_GPIO_PXPES(1), 0x03ff7fff);
}
/*
 * D0 ~ D7, CS1#, CLE, ALE, FRE#, FWE#, FRB#, RDWE#/BUFD#
 */
void gpio_as_nand_jz4725b()
{
	OUTREG32(A_GPIO_PXFUNS(0), 0x000000ff);
	OUTREG32(A_GPIO_PXSELC(0), 0x000000ff);
	OUTREG32(A_GPIO_PXPES(0), 0x000000ff);

	OUTREG32(A_GPIO_PXFUNS(2), 0x37F00300);
	OUTREG32(A_GPIO_PXSELC(2), 0x37F00300);
	OUTREG32(A_GPIO_PXPES(2), 0x37F00300);
}

void jz4725b_gpio_init(void)
{
	gpio_as_sdram_16bit_jz4725b();
	
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_UART0);
	gpio_as_uart_4725b();
	gpio_as_nand_jz4725b();
}
