#include "jz4750.h"
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

static void set_gpio_as_msc(void)
{
	unsigned int processor_id = 0, jz4725_pre_check;
	jz4725_pre_check = (unsigned int) ( * (volatile unsigned int *) 0xB00F0000 );
	processor_id = read_32bit_cp0_processorid();
	if ( jz4725_pre_check & 0x80000000 )
	{
		serial_puts("\n processor is 4725b \n");
		REG_GPIO_PXFUNS(2) = 0x38400300;	
		REG_GPIO_PXTRGC(2) = 0x38400300;	
		REG_GPIO_PXSELS(2) = 0x30400300;	
		REG_GPIO_PXSELC(2) = 0x08000000;	
		REG_GPIO_PXPES(2)  = 0x38400300;
	}
	else
	{
		if (processor_id == PROID_4750){
			serial_puts("this is 4750 board\n");
			__gpio_as_msc0_8bit();
		}else if (processor_id == PROID_4755){
			serial_puts("this is 4755 board\n");
			REG_GPIO_PXFUNS(1) = 0x00008000;		//写1打开功能	
			REG_GPIO_PXTRGS(1) = 0x00008000; 		//写1设置TRIG	
			REG_GPIO_PXSELC(1) = 0x00008000;		//写1清0	
			REG_GPIO_PXPES(1)  = 0x00008000;		//写1打开上拉	
			REG_GPIO_PXFUNS(2) = 0x38030000;	
			REG_GPIO_PXTRGS(2) = 0x00010000;	
			REG_GPIO_PXTRGC(2) = 0x38020000;	
			REG_GPIO_PXSELC(2) = 0x08010000;	
			REG_GPIO_PXSELS(2) = 0x30020000;	
			REG_GPIO_PXPES(2)  = 0x38030000;
		}
		else
		{
			serial_puts("MMC/SD GPIO init Fail  !!!!\n");
		}
	}
}

static void mmc_init_gpio(void)
{
	serial_puts("mmc_init_gpio\n");

	REG_CPM_MSCCDR(0) = 13;
	REG_CPM_CPCCR |= CPM_CPCCR_CE;
	REG_MSC_RDTO = 0x0fffffff;
	
	set_gpio_as_msc();
}

static void mmc_power_off()
{
	unsigned int processor_id = 0, jz4725_pre_check;
	jz4725_pre_check = (unsigned int) ( * (volatile unsigned int *) 0xB00F0000 );
	processor_id = read_32bit_cp0_processorid();
	if ( jz4725_pre_check & 0x80000000 )
	{
		serial_puts("\n power_off_mmc_io is 4725b \n");
		REG_GPIO_PXFUNC(2) = 0x38400300;	
		REG_GPIO_PXSELC(2) = 0x38400300;	
		REG_GPIO_PXDIRS(2) = 0x38400300;		
		
		//将mmc功能io全部拉低	
		REG_GPIO_PXDATC(2) = 0x38400300;
	}
	else
	{
		if (processor_id == PROID_4750){
			serial_puts("power_off_mmc_io is 4750 board\n");
			REG_GPIO_PXFUNC(5) = 0x000003FF;	
			REG_GPIO_PXSELC(5) = 0x000003FF;	
			REG_GPIO_PXDIRS(5) = 0x000003FF;	
			
			//将mmc功能io全部拉低	
			REG_GPIO_PXDATC(5) = 0x000003FF;
		}else if (processor_id == PROID_4755){
			serial_puts("power_off_mmc_io is 4755 board\n");
			//设置为gpio后全部拉低	
			REG_GPIO_PXFUNC(1) = 0x00008000;	
			REG_GPIO_PXSELC(1) = 0x00008000; 		
			REG_GPIO_PXDIRS(1) = 0x00008000;	
			// set CLK to low	            	
			REG_GPIO_PXDATC(1) = 0x00008000;	
			REG_GPIO_PXFUNC(2) = 0x38030000;	
			REG_GPIO_PXSELC(2) = 0x38030000;	
			REG_GPIO_PXDIRS(2) = 0x38030000;			
			//将mmc功能io全部拉低	
			REG_GPIO_PXDATC(2) = 0x38030000;
		}
		else
		{
			serial_puts("power_off_mmc_io Fail  !!!!\n");
		}
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
	serial_puts("open card off  power test\n");
	unsigned int processor_id = 0, jz4725_pre_check;
	jz4725_pre_check = (unsigned int) ( * (volatile unsigned int *) 0xB00F0000 );
	processor_id = read_32bit_cp0_processorid();
	if ( jz4725_pre_check & 0x80000000 )
	{
		serial_puts("\n power_off_mmc_io is 4725b \n");
		REG_GPIO_PXFUNC(2) = 0x38400000;	
		REG_GPIO_PXSELC(2) = 0x38400000;	
		REG_GPIO_PXDIRS(2) = 0x38400000;		
		
		//将mmc功能io全部拉低	
		REG_GPIO_PXDATC(2) = 0x38400000;
	}
	else
	{
		if (processor_id == PROID_4750){
			serial_puts("power_off_mmc_io is 4750 board\n");
			REG_GPIO_PXFUNC(5) = 0x000000FF;	
			REG_GPIO_PXSELC(5) = 0x000000FF;	
			REG_GPIO_PXDIRS(5) = 0x000000FF;	
			
			//将mmc功能io全部拉低	
			REG_GPIO_PXDATC(5) = 0x000000FF;
		}else if (processor_id == PROID_4755){
			serial_puts("power_off_mmc_io is 4755 board\n");
			//设置为gpio后全部拉低	
	
			REG_GPIO_PXFUNC(2) = 0x38020000;	
			REG_GPIO_PXSELC(2) = 0x38020000;	
			REG_GPIO_PXDIRS(2) = 0x38020000;			
			//将mmc功能io全部拉低	
			REG_GPIO_PXDATC(2) = 0x38020000;
		}
		else
		{
			serial_puts("power_off_mmc_io Fail  !!!!\n");
		}
	}
	__gpio_as_output(mmc_power_pin);
	//set power pin level
	if(power_off_level > 0)		
		__gpio_set_pin(mmc_power_pin);  	
	else		
		__gpio_clear_pin(mmc_power_pin);
	sd_mdelay(20);
}

int mmc_init_4750(unsigned int msc_clkrt_val)
{
	return mmc_init(msc_clkrt_val);
}

int mmc_block_readm_4750(u32 src, u32 num, u8 *dst)
{
	return mmc_block_readm(src, num, dst);
}

int mmc_block_writem_4750(u32 src, u32 num, u8 *dst)
{
	return mmc_block_writem(src, num, dst);
}

void mmc_set_power_pin_parameter_4750(int power_pin,u8 off_level)
{
	mmc_power_pin = power_pin;
	serial_puts("\n power_pin==0X");  serial_put_hex(mmc_power_pin);
	power_off_level = off_level;
	serial_puts("\n power_pin_level==0X");  serial_put_hex(power_off_level);
}

int skymedi_open_card_4750(void)
{
	return skymedi_open_card();
}

u32 card_query_flash_id_4750(u8 *bulk_in_buf)
{
	return card_query_flash_id(bulk_in_buf);
}

