#include <jz4760.h>
#include <mmc.h>
#include <usb_boot.h>
#include "mmc_com.c"
#include "open_card.c"

static unsigned int mmc_power_pin;
static u8 power_off_level;

// ls
static file_header_t * get_file_header(char* buffer, int buffer_size, char* name)
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
	REG_CPM_MSCCDR(0) = 13;
	REG_CPM_CPCCR |= CPM_CPCCR_CE;
	REG_MSC_RDTO = 0x000fffff;
	
	__gpio_as_msc0_a();
}

static void mmc_power_off()
{
	int i;
	for(i=0;i<6;i++){
		__gpio_as_output(18+i);
		__gpio_clear_pin(18+i);
	}
	//set power pin level
	if(power_off_level > 0)		
		__gpio_set_pin(mmc_power_pin);  	
	else		
		__gpio_clear_pin(mmc_power_pin);
}

static void mmc_power_on()
{
	if(power_off_level > 0)		
		__gpio_clear_pin(mmc_power_pin);	
	else		
		__gpio_set_pin(mmc_power_pin);

	mmc_init_gpio();
} 

static void sd_skymedi_on_off_power(void)
{
	serial_puts("open card off on power\n");
	__gpio_as_output(mmc_power_pin);
	mmc_power_off();
	sd_mdelay(200);
	mmc_power_on();
	sd_mdelay(800);
}

/*test power pin setting error or not */
static void sd_skymedi_off_power(void)
{
	int i;
	serial_puts("open card off  power test\n");
	__gpio_as_output(mmc_power_pin);
	//set power pin level
	if(power_off_level > 0)		
		__gpio_set_pin(mmc_power_pin);  	
	else		
		__gpio_clear_pin(mmc_power_pin);

	//去掉GPIO漏电对电影控制引脚检查的影响
	for(i=0;i<6;i++){
		__gpio_as_output(18+i);
		__gpio_clear_pin(18+i);
	}
	sd_mdelay(20);
}

int mmc_init_4760(unsigned int msc_clkrt_val)
{
	serial_puts("\n msc_clkrt_val==0X");  serial_put_hex(msc_clkrt_val);
	return mmc_init(msc_clkrt_val);
}

int mmc_block_readm_4760(u32 src, u32 num, u8 *dst)
{
	return mmc_block_readm(src, num, dst);
}

int mmc_block_writem_4760(u32 src, u32 num, u8 *dst)
{
	return mmc_block_writem(src, num, dst);
}

void mmc_set_power_pin_parameter_4760(int power_pin,u8 off_level)
{
	mmc_power_pin = power_pin;
	serial_puts("\n power_pin==0X");  serial_put_hex(mmc_power_pin);
	power_off_level = off_level;
	serial_puts("\n power_pin_level==0X");  serial_put_hex(power_off_level);
}

int skymedi_open_card_4760(void)
{
	return skymedi_open_card();
}

u32 card_query_flash_id_4760(u8 *bulk_in_buf)
{
	return card_query_flash_id(bulk_in_buf);
}

