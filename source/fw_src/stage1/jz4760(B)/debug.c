#include "jz4760.h"
#include "configs.h"

extern fw_args_t * fw_args;

unsigned int check_sdram(unsigned int saddr, unsigned int size)
{
#if 0
	 unsigned int addr,err = 0;

	serial_puts("\nCheck SDRAM ... \n");
	saddr += 0xa0000000;
	size += saddr;
	serial_put_hex(saddr);
	serial_put_hex(size);
	saddr &= 0xfffffffc;      //must word align
	for (addr = saddr; addr < size; addr += 4)
	{
		*(volatile unsigned int *)addr = addr;
		if (*(volatile unsigned int *)addr != addr)
		{
			serial_put_hex(addr);
			err = addr;
		}
	}
	if (err)
		serial_puts("Check SDRAM fail!\n");
	else
		serial_puts("Check SDRAM pass!\n");

	return err;
#endif
}

void gpio_test(unsigned char ops, unsigned char pin)
{
	__gpio_as_output(pin);
	if (ops)
	{
//		serial_puts("\nGPIO set ");
//		serial_put_hex(pin);
		__gpio_set_pin(pin);
	}
	else
	{
//		serial_puts("\nGPIO clear ");
//		serial_put_hex(pin);
		__gpio_clear_pin(pin);
	}
//	__gpio_as_input(pin);
}

void do_debug()
{
#if 0
	switch (fw_args->debug_ops)
	{
	case 1:      //sdram check
		gpio_init_4760();
		serial_init();
		sdram_init_4760();
		REG8(USB_REG_INDEX) = 1;
		REG32(USB_FIFO_EP1) = check_sdram(fw_args->start, fw_args->size);
		REG32(USB_FIFO_EP1) = 0x0;
		REG8(USB_REG_INCSR) |= USB_INCSR_INPKTRDY;
		break;
	case 2:      //set gpio
		gpio_test(1, fw_args->pin_num);
		break;
	case 3:      //clear gpio
		gpio_test(0, fw_args->pin_num);
		break;
	}
#endif
}
