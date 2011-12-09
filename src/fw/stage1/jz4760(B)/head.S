/*
 * head.S
 *
 * Entry point of the firmware.
 * The firmware code are executed in the ICache.
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 *
 */

	.text

	.extern c_main

	.globl _start
	.set noreorder
_start:
	b	real_start
	nop
	/* reserve 8 words for args */
	.word	0x0                     //it's address is 0x80000008
	.word	0x0
	.word	0x0
	.word	0x0
	.word	0x0
	.word	0x0
	.word	0x0
	.word	0x0
real_start:		
	//----------------------------------------------------
	// setup stack, jump to C code
	//----------------------------------------------------
	la	$29, 0x80004000		// sp
	j	c_main
	nop

	.set reorder
