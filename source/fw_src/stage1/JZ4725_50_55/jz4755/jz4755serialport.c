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
 
#include "jz4755.h"
#include "..//configs.h"

void serial_putc (const char c)
{
	volatile unsigned char *uart_lsr = (volatile unsigned char *)(UART_BASE + UART_ULSR_OFFSET);
	volatile unsigned char *uart_tdr = (volatile unsigned char *)(UART_BASE + UART_URBR_OFFSET);

	if (c == '\n') serial_putc ('\r');

	/* Wait for fifo to shift out some bytes */
	while ( !((*uart_lsr & (ULSR_TDRQ | ULSR_TEMP)) == 0x60) );

	*uart_tdr = (unsigned char)c;
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

int serial_getc (void)
{
	volatile unsigned char *uart_rdr = (volatile unsigned char *)(UART_BASE + UART_URBR_OFFSET);

	while (!serial_tstc());

	return *uart_rdr;
}

int serial_tstc (void)
{
	volatile unsigned char *uart_lsr = (volatile unsigned char *)(UART_BASE+ UART_ULSR_OFFSET);

	if (*uart_lsr & ULSR_DRY) {
		/* Data in rfifo */
		return (1);
	}
	return 0;
}
extern char cpu_type;
void jz4755_serial_init()
{
	volatile unsigned char *uart_fcr = (volatile unsigned char *)(UART_BASE + UART_UFCR_OFFSET);
	volatile unsigned char *uart_lcr = (volatile unsigned char *)(UART_BASE + UART_ULCR_OFFSET);
	volatile unsigned char *uart_ier = (volatile unsigned char *)(UART_BASE + UART_UIER_OFFSET);
	volatile unsigned char *uart_sircr = (volatile unsigned char *)(UART_BASE + UART_UISR_OFFSET);

	/* Disable port interrupts while changing hardware */
	*uart_ier = 0;

	/* Disable UART unit function */
	*uart_fcr = ~UFCR_UME;

	/* Set both receiver and transmitter in UART mode (not SIR) */
	*uart_sircr = ~(UISR_RCVEIR | UISR_XMITIR);

	/* Set databits, stopbits and parity. (8-bit data, 1 stopbit, no parity) */
	*uart_lcr = ULCR_WLS_8BITS | ULCR_SBLS_1BIT;

	/* Set baud rate */
	//if ( fw_args->cpu_id == JZ4755 )
	//{
	serial_setbrg_4755();

	//}

	/* Enable UART unit, enable and clear FIFO */
	*uart_fcr = UFCR_UME | UFCR_FME | UFCR_TFRT | UFCR_RFRT;
}

void serial_put_hex(unsigned int  d)
{

	unsigned char c[12];
	char i;
	for(i = 0; i < 8;i++)
	{
		c[i] = (d >> ((7 - i) * 4)) & 0xf;
		if(c[i] < 10)
			c[i] += 0x30;
		else
			c[i] += (0x41 - 10);
	}
	c[8] = '\n';
	c[9] = 0;
	serial_puts(c);

}
