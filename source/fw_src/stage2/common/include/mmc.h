#ifndef _MMC_H_
#define _MMC_H_

static void udelay(unsigned int usec)
{
    //unsigned int i = usec * (336000000 / 2000000);
	unsigned int i = usec  << 7;
    __asm__ __volatile__ (
        "\t.set noreorder\n"
        "1:\n\t"
        "bne\t%0, $0, 1b\n\t"
        "addi\t%0, %0, -1\n\t"
        ".set reorder\n"
        : "=r" (i)
        : "0" (i)
    );
}

static void sd_mdelay(int sdelay)
{
    udelay(sdelay * 1000);	
}

int mmc_init_4770(unsigned int);
int mmc_block_readm_4770(u32 src, u32 num, u8 *dst);
int mmc_block_writem_4770(u32 src, u32 num, u8 *dst);
void mmc_set_power_pin_parameter_4770(int power_pin,u8 off_level);
int skymedi_open_card_4770(void);
u32 card_query_flash_id_4770(u8 *bulk_in_buf);

int mmc_init_4760(unsigned int);
int mmc_block_readm_4760(u32 src, u32 num, u8 *dst);
int mmc_block_writem_4760(u32 src, u32 num, u8 *dst);
void mmc_set_power_pin_parameter_4760(int power_pin,u8 off_level);
int skymedi_open_card_4760(void);
u32 card_query_flash_id_4760(u8 *bulk_in_buf);

int mmc_init_4750(unsigned int);
int mmc_block_readm_4750(u32 src, u32 num, u8 *dst);
int mmc_block_writem_4750(u32 src, u32 num, u8 *dst);
void mmc_set_power_pin_parameter_4750(int power_pin,u8 off_level);
int skymedi_open_card_4750(void);
u32 card_query_flash_id_4750(u8 *bulk_in_buf);

int mmc_init_4740(unsigned int);
int mmc_block_readm_4740(u32 src, u32 num, u8 *dst);
int mmc_block_writem_4740(u32 src, u32 num, u8 *dst);
void mmc_set_power_pin_parameter_4740(int power_pin,u8 off_level);
int skymedi_open_card_4740(void);
u32 card_query_flash_id_4740(u8 *bulk_in_buf);

#endif

