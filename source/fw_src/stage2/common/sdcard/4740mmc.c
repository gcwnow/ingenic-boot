#include "jz4740.h"
#include <mmc.h>
#include <usb_boot.h>
#include "mmc_com.c"
#include "open_card.c"

static unsigned int mmc_power_pin;
static u8 power_off_level;

// ls
static  file_header_t * get_file_header(char* buffer, int buffer_size, char* name)
{
	file_header_t * header = NULL;
	int i;
	//find first empty header & last full header
	for(i=0; i<(HEADER_SECTION_SIZE/sizeof(file_header_t)); i++)
	{
		file_header_t * header = (file_header_t *)(buffer + i * sizeof(file_header_t));
		if (strcmp(header->name, name) == 0)
		{
			serial_put_hex(header->offset); serial_put_hex(header->size);
			return header;
		}
	}
	return NULL;	
}

static void mmc_init_gpio(void)
{
	serial_puts("mmc_init_gpio\n");
	REG_CPM_MSCCDR = 13;
	REG_CPM_CPCCR |= CPM_CPCCR_CE;
	REG_MSC_RDTO = 0x0000ffff;

	__gpio_as_msc();
}

static void sd_skymedi_on_off_power(void)
{
	serial_puts("JZ4740 not  provides card boot\n");
}

/*test power pin setting error or not */
static void sd_skymedi_off_power(void)
{
	serial_puts("JZ4740 not  provides card boot\n");
}

int mmc_init_4740(unsigned int msc_clkrt_val)
{
	return mmc_init(msc_clkrt_val);
}

int mmc_block_readm_4740(u32 src, u32 num, u8 *dst)
{
	return mmc_block_readm(src, num, dst);
}

int mmc_block_writem_4740(u32 src, u32 num, u8 *dst)
{
	return mmc_block_writem(src, num, dst);
}

void mmc_set_power_pin_parameter_4740(int power_pin,u8 off_level)
{
	serial_puts("JZ4740 not  provides card boot\n");	
}

int skymedi_open_card_4740(void)
{
	serial_puts("JZ4740 not  provides card boot\n");
	return NOT_SUPPORT_CARD_BOOT;
}

u32 card_query_flash_id_4740(u8 *bulk_in_buf)
{
	serial_puts("JZ4740 not  provides card boot\n");
	return NOT_SUPPORT_CARD_BOOT;
}

