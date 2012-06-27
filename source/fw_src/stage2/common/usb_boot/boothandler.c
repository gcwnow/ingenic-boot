/* USB_BOOT Handle routines*/

#include <config.h>
#include "usb.h" 
#include "error.h"
#include "usb_boot.h"
#include "hand.h"
#include "nandflash.h"
#include "udc.h"
#include "mmc.h"

#define VERIFY_CRC32

#define dprintf(x) serial_puts(x)

unsigned int (*nand_query)(u8 *);
int 	(*nand_init)(int bus_width, int row_cycle, int page_size, int page_per_block,
		 	int,int,int,int);
int 	(*nand_fini)(void);
int 	(*hw_reset) (void);
u32 	(*nand_program)(void *context, int spage, int pages,int option);
u32 	(*nand_erase)(int blk_num, int sblk, int force);
u32 	(*nand_mark_erase)(void);
u32 	(*nand_read)(void *buf, u32 startpage, u32 pagenum,int option);
u32 	(*nand_read_oob)(void *buf, u32 startpage, u32 pagenum);
u32 	(*nand_read_raw)(void *buf, u32 startpage, u32 pagenum,int);
u32 	(*nand_mark_bad) (int bad);
void 	(*nand_enable) (unsigned int csn);
void 	(*nand_disable) (unsigned int csn);
int 	(*mmc_init)(unsigned int);
int 	(*mmc_block_readm)(u32 , u32 , u8 *);
int 	(*mmc_block_writem)(u32 , u32 , u8 *);
void	(*mmc_set_power_pin_parameter)(int, u8);
u32 	(*card_query_flash_id)(u8 *);
int 	(*skymedi_open_card)(void);


hand_t Hand,*Hand_p;
extern u32 Bulk_out_buf[BULK_OUT_BUF_SIZE];
extern u32 Bulk_in_buf[BULK_IN_BUF_SIZE];
extern u16 handshake_PKT[4];
extern udc_state;
extern void *memset(void *, int , size_t);
extern void *memcpy(void *, const void *, size_t);

u32 ret_dat;
u32 start_addr;  //program operation start address or sector
u32 ops_length;  //number of operation unit ,in byte or sector
u32 ram_addr;

#ifdef VERIFY_CRC32
unsigned int crc32_tab[256]={
	 0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 
	 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005, 
	 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 
	 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 
	 0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 
	 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75, 
	 0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 
	 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd, 
	 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 
	 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 
	 0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 
	 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d, 
	 0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 
	 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95, 
	 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 
	 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 
	 0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 
	 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072, 
	 0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 
	 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 
	 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 
	 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 
	 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 
	 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba, 
	 0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 
	 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692, 
	 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 
	 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 
	 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 
	 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 
	 0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 
	 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a, 
	 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 
	 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 
	 0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 
	 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53, 
	 0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 
	 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b, 
	 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 
	 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 
	 0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 
	 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b, 
	 0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 
	 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3, 
	 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 
	 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 
	 0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 
	 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3, 
	 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 
	 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 
	 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 
	 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 
	 0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 
	 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec, 
	 0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 
	 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 
	 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 
	 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 
	 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 
	 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 
	 0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 
	 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c, 
	 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 
	 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4, };

unsigned int do_crc32 (unsigned char *buf, int len)
{
	unsigned int crc32 = 0;
	int i, counter = 0;
	
	for (i = 0; i < len; i++)
	{
		crc32 = ((crc32 << 8) | buf[i]) ^ (crc32_tab[(crc32 >> 24) & 0xff]);
	}
	
	return crc32;
}
#endif /* #ifdef VERIFY_CRC32 */

void config_flash_info()
{
}
#define read_32bit_cp0_processorid()                            \
({ int __res;                                                   \
        __asm__ __volatile__(                                   \
        "mfc0\t%0,$15\n\t"                                      \
        : "=r" (__res));                                        \
        __res;})
        
/*
void dump_data(unsigned int *p, int size)
{
	int i;
	for(i = 0; i < size; i ++)
		serial_put_hex(*p++);
}
*/

void config_hand()
{
	hand_t *hand_p;
	hand_p=( hand_t *)Bulk_out_buf;
	memcpy(&Hand, (unsigned char *)Bulk_out_buf, sizeof(hand_t));

	serial_puts("\n config_hand data:\n");
//	dump_data(Bulk_out_buf, sizeof(hand_t) / 4);
//serial_puts("config_hand data end.\n");

#if 0
	Hand.nand_bw=hand_p->nand_bw;
	Hand.nand_rc=hand_p->nand_rc;
	Hand.nand_ps=hand_p->nand_ps;
	Hand.nand_ppb=hand_p->nand_ppb;
	Hand.nand_force_erase=hand_p->nand_force_erase;
	Hand.nand_pn=hand_p->nand_pn;
	Hand.nand_os=hand_p->nand_os;

	Hand.nand_eccpos=hand_p->nand_eccpos;
	Hand.nand_bbpos=hand_p->nand_bbpos;
	Hand.nand_bbpage=hand_p->nand_bbpage;
//	memcpy( &Hand.fw_args, (unsigned char *)(start_addr + 0x8), 32 );

//	serial_putc(Hand.nand_eccpos + 48);
//	serial_putc(Hand.nand_bbpos + 48);
//	serial_putc(Hand.nand_bbpage + 48);
//	dprintf("\n Hand : bw %d rc %d ps %d ppb %d erase %d pn %d os %d",
//		Hand.nand_bw,Hand.nand_rc,Hand.nand_ps,Hand.nand_ppb,Hand.nand_force_erase,Hand.nand_pn,Hand.nand_os);
	serial_put_hex(Hand.fw_args.cpu_id);
	serial_put_hex(Hand.fw_args.ext_clk);
#endif
}

int GET_CUP_INFO_Handle()
{
	char temp1[8]="Boot4740",temp2[8]="Boot4750",temp3[8]="Boot4760",temp4[8]="Boot4770";
	
	dprintf("\n GET_CPU_INFO!");
	if ( Hand.fw_args.cpu_id == 0x4740 )
		HW_SendPKT(0,temp1,8);
	else if ( Hand.fw_args.cpu_id == 0x4750 )
		HW_SendPKT(0,temp2,8);
	else if ( Hand.fw_args.cpu_id == 0x4760 )
		HW_SendPKT(0,temp3,8);
	else
		HW_SendPKT(0,temp4,8);
	udc_state = IDLE;
	return ERR_OK; 
}
	       
int SET_DATA_ADDERSS_Handle(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	start_addr=(((u32)dreq->wValue)<<16)+(u32)dreq->wIndex;
	
//	dprintf("\n SET ADDRESS:");
//	serial_put_hex(start_addr);
	
	return ERR_OK;
}
		
int SET_DATA_LENGTH_Handle(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	ops_length=(((u32)dreq->wValue)<<16)+(u32)dreq->wIndex;
	
//	dprintf("\n DATA_LENGTH :");
//	serial_put_hex(ops_length);

	return ERR_OK;
}

int FLUSH_CACHES_Handle()
{
//	dprintf("\n FLUSH_CACHES_Handle :");
	return ERR_OK;
}
    
int PROGRAM_START1_Handle(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	ram_addr=(((u32)dreq->wValue)<<16)+(u32)dreq->wIndex;
//	dprintf("\n RAM ADDRESS :%x", ram_addr);
//	dprintf("\n PROGRAM_START1_Handle :");
//	serial_put_hex(ram_addr);

	return ERR_OK;
}

int PROGRAM_START2_Handle(u8 *buf)
{
	void (*f)(void);
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	f=(void *) ((((u32)dreq->wValue)<<16)+(u32)dreq->wIndex);
	__dcache_writeback_all();
	
//	dprintf("\n PROGRAM_START2_Handle  1111:");
	//stop udc connet before execute program!
	jz_writeb(USB_REG_POWER, 0x0);   //High speed
	
//	dprintf("\n Execute program at %x",(u32)f);
//	dprintf("\n PROGRAM_START2_Handle :");
//	serial_put_hex((u32)f);

	f();
	return ERR_OK;
}
	      
int NOR_OPS_Handle(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	return ERR_OK;
}


static RTC_setting(unsigned int second)
{
#if defined(CAN_SUPPORT_RTC)
/* The divider is decided by the RTC clock frequency. */
#define RTC_FREQ_DIVIDER	(32768 - 1)

	/* set 32K clock */
//	SETREG32(CPM_OPCR, OPCR_ERCS);
	REG_CPM_OPCR |= CPM_OPCR_ERCS;

	/* Set 32768 rtc clocks per seconds */
	rtc_write_reg(RTC_RGR, RTC_FREQ_DIVIDER);

	/* Set minimum wakeup_n pin low-level assertion time for wakeup: 100ms */
//	rtc_write_reg(RTC_HWFCR, HWFCR_WAIT_TIME(100));
//	rtc_write_reg(RTC_HRCR, HRCR_WAIT_TIME(60));

	/* start RTC only and clear all flags */
	rtc_write_reg(RTC_RCR, RTC_RCR_RTCE);	
	rtc_write_reg(RTC_HSPR, HSPR_RTCV);

	/* wite rtc second */
	rtc_write_reg(RTC_RSR, second);

	serial_puts("RTC time has set to:");
	serial_put_hex(rtc_read_reg(RTC_RSR));
#endif
}

int NAND_OPS_Handle(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	u32 temp;
	int option;
	u8 CSn;

	CSn = (dreq->wValue>>8) & 0xf;
	option = (dreq->wValue>>12) & 0xf;

	switch ((dreq->wValue)&0xff)
	{
	case NAND_QUERY:
		dprintf("\n 111Request : NAND_QUERY!");
		nand_query((u8 *)Bulk_in_buf);
		HW_SendPKT(1, Bulk_in_buf, 8);
		handshake_PKT[3]=(u16)ERR_OK;
		udc_state = BULK_IN;
		break;
	case NAND_INIT:
		dprintf("\n Request : NAND_INIT!");
		break;
	case NAND_MARK_BAD:
		dprintf("\n Request : NAND_MARK_BAD!");
		ret_dat = nand_mark_bad(start_addr);
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;		
		break;
	case NAND_READ_OOB:
		dprintf("\n Request : NAND_READ_OOB!");
		memset(Bulk_in_buf,0,ops_length*Hand.nand_ps);
		ret_dat = nand_read_oob(Bulk_in_buf,start_addr,ops_length);
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,(u8 *)Bulk_in_buf,ops_length*Hand.nand_ps);
		udc_state = BULK_IN;		
		break;
	case NAND_READ_RAW:
		dprintf("\n Request : NAND_READ_RAW!");
		switch (option)
		{
		case OOB_ECC:
			nand_read_raw(Bulk_in_buf,start_addr,ops_length,option);
			HW_SendPKT(1,(u8 *)Bulk_in_buf,ops_length*(Hand.nand_ps + Hand.nand_os));
			handshake_PKT[0] = (u16) ret_dat;
			handshake_PKT[1] = (u16) (ret_dat>>16);
			udc_state = BULK_IN;
			break;
		default:
			nand_read_raw(Bulk_in_buf,start_addr,ops_length,option);
			HW_SendPKT(1,(u8 *)Bulk_in_buf,ops_length*Hand.nand_ps);
			handshake_PKT[0] = (u16) ret_dat;
			handshake_PKT[1] = (u16) (ret_dat>>16);
			udc_state = BULK_IN;
			break;
		}
		break;
	case NAND_ERASE:
		dprintf("\n Request : NAND_ERASE!");
		ret_dat = nand_erase(ops_length,start_addr,
			   Hand.nand_force_erase);
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		dprintf("\n Request : NAND_ERASE_FINISH!");
		break;
	case NAND_FORCE_ERASE:
		dprintf("\n Request : NAND_FORCE_ERASE!");
		ret_dat = nand_mark_erase();
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		break;		
	case NAND_READ:
		dprintf("\n Request : NAND_READ!");
		switch (option)
		{
		case 	OOB_ECC:
			ret_dat = nand_read(Bulk_in_buf,start_addr,ops_length,OOB_ECC);
			handshake_PKT[0] = (u16) ret_dat;
			handshake_PKT[1] = (u16) (ret_dat>>16);
			HW_SendPKT(1,(u8 *)Bulk_in_buf,ops_length*(Hand.nand_ps + Hand.nand_os ));
			udc_state = BULK_IN;
			break;
		case 	OOB_NO_ECC:
			ret_dat = nand_read(Bulk_in_buf,start_addr,ops_length,OOB_NO_ECC);
			handshake_PKT[0] = (u16) ret_dat;
			handshake_PKT[1] = (u16) (ret_dat>>16);
			HW_SendPKT(1,(u8 *)Bulk_in_buf,ops_length*(Hand.nand_ps + Hand.nand_os));
			udc_state = BULK_IN;
			break;
		case 	NO_OOB:
			ret_dat = nand_read(Bulk_in_buf,start_addr,ops_length,NO_OOB);
			handshake_PKT[0] = (u16) ret_dat;
			handshake_PKT[1] = (u16) (ret_dat>>16);
			HW_SendPKT(1,(u8 *)Bulk_in_buf,ops_length*Hand.nand_ps);
			udc_state = BULK_IN;
			break;
		}
		dprintf("\n Request : NAND_READ_FUNISH!");
		break;
	case NAND_PROGRAM:
		dprintf("\n Request : NAND_PROGRAM!");		
#ifdef VERIFY_CRC32
		u32 file_crc32, check_crc32, check_len;
		ops_length -= 1;
		if(option == NO_OOB)
			check_len = ops_length * Hand.nand_ps;
		else
			check_len = ops_length * (Hand.nand_ps + Hand.nand_os);
		file_crc32 = (unsigned long)Bulk_out_buf[check_len / 4];
		check_crc32 = do_crc32(Bulk_out_buf, check_len);
                if(file_crc32 != check_crc32){
			serial_puts("\nfile_crc32  = ");serial_put_hex(file_crc32);
			serial_puts("\ncheck_crc32 = ");serial_put_hex(check_crc32);
			handshake_PKT[0] = 0xffff;
			handshake_PKT[1] = 0xffff;
			HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
			udc_state = IDLE;
			break;
		}
#endif
		ret_dat = nand_program((void *)Bulk_out_buf,
			     start_addr,ops_length,option);
		dprintf("\n NAND_PROGRAM finish!");
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		break;
	case NAND_READ_TO_RAM:
		dprintf("\n Request : NAND_READNAND!");
		nand_read((u8 *)ram_addr,start_addr,ops_length,NO_OOB);
		__dcache_writeback_all();
		handshake_PKT[3]=(u16)ERR_OK;
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		break;
	case CARD_PROGRAM:
		dprintf("\n Request : SD_PROG!");
		ret_dat = mmc_block_writem(start_addr, ops_length * 512, (u8 *)Bulk_out_buf);
		dprintf("\n Request : SD_PROG finish!");
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		break;
	case CARD_READ:
		dprintf("\n Request : SD_READ!");
		ret_dat = mmc_block_readm(start_addr, ops_length * 512, (u8 *)Bulk_in_buf);
		dprintf("\n Request : SD_READ finish!");
		//handshake_PKT[0] = (u16) ret_dat;
		//handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,(u8 *)Bulk_in_buf, ops_length * 512);
		//udc_state = BULK_IN;
		udc_state = IDLE;
		break;
	case CARD_QUERY_HW:		
		dprintf("\n Request : SD_MMC_QUERY! \n");				
		mmc_set_power_pin_parameter(start_addr&0xfff,(start_addr >> 12)&0xf);		
		ret_dat = card_query_flash_id((u8 *)Bulk_in_buf);		
		handshake_PKT[0] = (u16) ret_dat;		
		handshake_PKT[1] = (u16) (ret_dat>>16);		
		HW_SendPKT(1,(u8 *)Bulk_in_buf,512);		
		udc_state = BULK_IN;		
		break;
	case CARD_OPEN_CARD:
		dprintf("\n Request : SD_MMC_OPENCARD! \n");		
		//serial_dump_data(((u8 *)Bulk_out_buf),512);	
		ret_dat = skymedi_open_card();		
		handshake_PKT[0] = (u16) ret_dat;		
		handshake_PKT[1] = (u16) (ret_dat>>16);		
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));		
		udc_state = IDLE;
		break;
	case CARD_INIT:
		dprintf("\n Request : SD_INIT!");
		ret_dat = mmc_init(option);	
		dprintf("\n Request : SD_INIT finish!");
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		break;
	case SET_RTC:
		serial_puts("setting RTC time \n");
		serial_put_hex(start_addr);
		RTC_setting(start_addr);
		handshake_PKT[3]=(u16)ERR_OK;
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		break;
	default:
		nand_disable(CSn);
		dprintf("\n Request : ERR_OPS_NOTSUPPORT! \n");
		return ERR_OPS_NOTSUPPORT;
	}

	return ERR_OK;
}

int SDRAM_OPS_Handle(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	u32 temp,i;
	u8 *obj;

	switch ((dreq->wValue)&0xf)
	{
	case 	SDRAM_LOAD:
//		dprintf("\n Request : SDRAM_LOAD!");
		ret_dat = (u32)memcpy((u8 *)start_addr,Bulk_out_buf,ops_length);
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		break;
		
	case	DEVICE_RESET:
		dprintf("\n Request : DEVICE_RESET!");
		ret_dat = hw_reset();
		handshake_PKT[0] = (u16) ret_dat;
		handshake_PKT[1] = (u16) (ret_dat>>16);
		HW_SendPKT(1,handshake_PKT,sizeof(handshake_PKT));
		udc_state = IDLE;
		break;
	}
	return ERR_OK;
}

void Borad_Init()
{
	dprintf("\n Borad_init! ");
	serial_put_hex(Hand.fw_args.cpu_id);
	switch (Hand.fw_args.cpu_id)
	{
	case 0x4740:
		//Init nand flash
		nand_init_4740(Hand.nand_bw,Hand.nand_rc,Hand.nand_ps,Hand.nand_ppb,
		       Hand.nand_bbpage,Hand.nand_bbpos,Hand.nand_force_erase,Hand.nand_eccpos);
	
		nand_program=nand_program_4740;
		nand_erase  =nand_erase_4740;
		nand_read   =nand_read_4740;
		nand_read_oob=nand_read_oob_4740;
		nand_read_raw=nand_read_raw_4740;
		nand_query  = nand_query_4740;
		nand_enable = nand_enable_4740;
		nand_disable= nand_disable_4740;
		nand_mark_bad = nand_mark_bad_4740;
		mmc_init = mmc_init_4740;
		mmc_block_readm = mmc_block_readm_4740;
		mmc_block_writem = mmc_block_writem_4740;
		mmc_set_power_pin_parameter = mmc_set_power_pin_parameter_4740;
		card_query_flash_id = card_query_flash_id_4740;
		skymedi_open_card = skymedi_open_card_4740;
	break;
	case 0x4750:
		//Init nand flash
		nand_init_4750(Hand.nand_bw, Hand.nand_rc, Hand.nand_ps,
			       Hand.nand_ppb, Hand.nand_bchbit, Hand.nand_eccpos,
			       Hand.nand_bbpos, Hand.nand_bbpage, Hand.nand_force_erase);

		nand_program=nand_program_4750;
		nand_erase  =nand_erase_4750;
		nand_read   =nand_read_4750;
		nand_read_oob=nand_read_oob_4750;
		nand_read_raw=nand_read_raw_4750;
		nand_query  = nand_query_4750;
		nand_enable = nand_enable_4750;
		nand_disable= nand_disable_4750;
		nand_mark_bad = nand_mark_bad_4750;
		hw_reset = hw_reset_4750;
		mmc_init = mmc_init_4750;
		mmc_block_readm = mmc_block_readm_4750;
		mmc_block_writem = mmc_block_writem_4750;
		mmc_set_power_pin_parameter = mmc_set_power_pin_parameter_4750;
		card_query_flash_id = card_query_flash_id_4750;
		skymedi_open_card = skymedi_open_card_4750;
	break;
	case 0x4760:
		//Init nand flash
		nand_init_4760(Hand.nand_bw, Hand.nand_rc, Hand.nand_ps,
			       Hand.nand_ppb, Hand.nand_bchbit, Hand.nand_eccpos,
			       Hand.nand_bbpos, Hand.nand_bbpage, Hand.nand_force_erase, Hand.nand_os, Hand.nand_bchstyle);

		nand_program=nand_program_4760;
		nand_erase  =nand_erase_4760;
		nand_read   =nand_read_4760;
		nand_read_oob=nand_read_oob_4760;
		nand_read_raw=nand_read_raw_4760;
		nand_query  = nand_query_4760;
		nand_enable = nand_enable_4760;
		nand_disable= nand_disable_4760;
		nand_mark_bad = nand_mark_bad_4760;
		nand_mark_erase = nand_mark_erase_4760;		
		hw_reset = hw_reset_4760;
		mmc_init = mmc_init_4760;
		mmc_block_readm = mmc_block_readm_4760;
		mmc_block_writem = mmc_block_writem_4760;
		mmc_set_power_pin_parameter = mmc_set_power_pin_parameter_4760;
		card_query_flash_id = card_query_flash_id_4760;
		skymedi_open_card = skymedi_open_card_4760;
	break;
	case 0x4770:
		//Init nand flash
		nand_init_4770(Hand.nand_bw, Hand.nand_rc, Hand.nand_ps,
			       Hand.nand_ppb, Hand.nand_bchbit, Hand.nand_eccpos,
			       Hand.nand_bbpos, Hand.nand_bbpage, Hand.nand_force_erase, Hand.nand_os, Hand.nand_bchstyle);

		nand_program=nand_program_4770;
		nand_erase  =nand_erase_4770;
		nand_read   =nand_read_4770;
		nand_read_oob=nand_read_oob_4770;
		nand_read_raw=nand_read_raw_4770;
		nand_query  = nand_query_4770;
		nand_enable = nand_enable_4770;
		nand_disable= nand_disable_4770;
		nand_mark_bad = nand_mark_bad_4770;
		nand_mark_erase = nand_mark_erase_4770;		
		hw_reset = hw_reset_4770;
		mmc_init = mmc_init_4770;
		mmc_block_readm = mmc_block_readm_4770;
		mmc_block_writem = mmc_block_writem_4770;
		mmc_set_power_pin_parameter = mmc_set_power_pin_parameter_4770;
		card_query_flash_id = card_query_flash_id_4770;
		skymedi_open_card = skymedi_open_card_4770;
	break;
	default:
		serial_puts("Not support CPU ID!");
	}
	//enable plan 0
	nand_enable(0);
}

int CONFIGRATION_Handle(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	switch ((dreq->wValue)&0xf)
	{
	case DS_flash_info:
		dprintf("\n configration :DS_flash_info_t!");
		config_flash_info();
		break;

	case DS_hand:
		dprintf("\n configration :DS_hand_t!");
		config_hand();
		break;
	default:;
		
	}
	Borad_Init();
	return ERR_OK;
}


