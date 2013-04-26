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
	// Setup stack.
	la	$sp, 0x80004000

	// The Boot ROM code expects v1 and a0 to be preserved.
	// If they aren't, depending on their value, it's possible the system will
	// drop out of USB Boot Mode and continue with the normal boot process.
	addiu	$sp,$sp,-8
	sw	$v1,0($sp)
	sw	$a0,4($sp)

	// Call C code.
	jal	c_main
	nop

	// Restore regs and jump back into the Boot ROM code.
	lw	$a0,4($sp)
	lw	$v1,0($sp)
	la	$ra, 0xbfc0130c
	jr	$ra
	nop

	.set reorder
