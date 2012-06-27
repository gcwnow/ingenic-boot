#ifndef __ERROR_H__
#define __ERROR_H__

/*
 * All of the return codes
 */
#define ERR_OK				0
#define ERR_TIMOUT			1
#define ERR_NOT_ERASED			2
#define ERR_PROTECTED			4
#define ERR_INVAL			8
#define ERR_OPS_NOTSUPPORT              9
#define ERR_ALIGN			16
#define ERR_UNKNOWN_FLASH_VENDOR	32
#define ERR_UNKNOWN_FLASH_TYPE		64
#define ERR_PROG_ERROR			128
#define ERR_ERASE_ERROR			256
#define ERR_WRITE_VERIFY		512
#define ERR_NOT_SUPPORT			1024  /* operation not supported */

#endif /* __ERROR_H__ */
