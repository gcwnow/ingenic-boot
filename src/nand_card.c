/*
 * Authors: Duke Fong <duke@dukelec.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ingenic_boot.h"

/* 512K per transmiter 0x400 * 512 = 512KB */
#define SD_BLOCK_SIZE 512
#define SD_MAX_BLOCK_NUM 0x400

/* no handshake */
int sd_card_read(struct ingenic_dev *ingenic_dev,
		 unsigned int block_addr, unsigned int block_num, char *buf)
{
	int retval;
	retval = usb_send_data_address_to_ingenic(ingenic_dev, block_addr);
	if (retval != 1)
		return -1;
	retval = usb_send_data_length_to_ingenic(ingenic_dev, block_num);
	if (retval != 1)
		return -1;
	
	retval = usb_ingenic_nand_ops(ingenic_dev, CARD_READ);
	if (retval != 1)
		return -1;
	
	retval = usb_read_data_from_ingenic(ingenic_dev, buf,
					    block_num * SD_BLOCK_SIZE);
	if (retval != 1)
		return -1;
	return 1;
}

/* have handshake */
static int sd_card_write(struct ingenic_dev *ingenic_dev,
			 unsigned int block_addr,
			 unsigned int block_num, char *buf)
{
	int retval;
	char hex_data[8];
	retval = usb_send_data_address_to_ingenic(ingenic_dev, block_addr);
	if (retval != 1)
		return -1;
	retval = usb_send_data_length_to_ingenic(ingenic_dev, block_num);
	if (retval != 1)
		return -1;
	
	/* do wirte first */
	retval = usb_send_data_to_ingenic(ingenic_dev, buf,
					  block_num * SD_BLOCK_SIZE);
	if (retval != 1)
		return -1;
	
	/* then send request */
	retval = usb_ingenic_nand_ops(ingenic_dev, CARD_PROGRAM);
	if (retval != 1)
		return -1;

	
	retval = usb_read_data_from_ingenic(ingenic_dev, hex_data, 8);
	if (retval != 1)
		return -1;
	
	/* printf("Whandshake: %02x %02x %02x %02x.\n",hex_data[0],hex_data[1], */
	/*        hex_data[2],hex_data[3]); */
	
	return 1;
}

/* have handshake */
int sd_card_init(struct ingenic_dev *ingenic_dev)
{
	int retval;
	char hex_data[9];
	printf("\n#SD init\n");
	retval = usb_ingenic_nand_ops(ingenic_dev, CARD_INIT);
	if (retval != 1)
		return -1;
	
	retval = usb_read_data_from_ingenic(ingenic_dev, hex_data, 8);
	if (retval != 1)
		return -1;
	/* for (i = 0; i < 8; i++) { */
	/* 	printf("%x ", *(hex_data + i)); */
	/* } */
	return 1;
}

int sd_card_program(struct ingenic_dev *ingenic_dev, unsigned int addr,
		    const char *file_path, int check)
{
	unsigned int block_addr = addr / SD_BLOCK_SIZE;
	struct stat fstat;
	int fd;
	unsigned int download_times, block_nums;
	unsigned int last_download_block_num, last_block_len;
	unsigned int i;
	char *origin_data, *readback_data;
	int retval;

	retval = stat(file_path, &fstat);

	if (retval < 0) {
		fprintf(stderr, "Error - can't get file size from '%s': %s\n",
			file_path, strerror(errno));
		goto out0;
	}

	/* < 512 */
	last_block_len = fstat.st_size % SD_BLOCK_SIZE;
	printf(" last_block_len %d ", last_block_len);
	if (last_block_len)
		block_nums = fstat.st_size / SD_BLOCK_SIZE + 1;
	else
		block_nums = fstat.st_size / SD_BLOCK_SIZE;
	printf("\n block_nums %d ", block_nums);

	/* < 0x400 */
	last_download_block_num = block_nums % SD_MAX_BLOCK_NUM;
	printf("\n last_download_block_num %d ", last_download_block_num);
	if (last_download_block_num)
		download_times = block_nums / SD_MAX_BLOCK_NUM + 1;
	else
		download_times = block_nums / SD_MAX_BLOCK_NUM;
	printf("\n download_times %d : ", download_times);
	
	fd = open(file_path, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error - can't open file '%s': %s\n",
			file_path, strerror(errno));
		goto out0;
	}

	/* malloc buffer */
	origin_data = malloc(SD_BLOCK_SIZE * SD_MAX_BLOCK_NUM);
	if (!origin_data)
		goto out0;
	if (check) {
		readback_data = malloc(SD_BLOCK_SIZE * SD_MAX_BLOCK_NUM);
		if (!readback_data)
			goto out1;
	}

	/* start the work */
	for (i = 0; i < download_times; i++) {

		/* read file */
		if (i + 1 == download_times) {
			if (last_block_len != 0) {
				retval = read(fd, origin_data,
					      (last_download_block_num - 1)
					      * SD_BLOCK_SIZE + last_block_len);
				if (retval < (last_download_block_num - 1)
				    * SD_BLOCK_SIZE + last_block_len) {
					fprintf(stderr,
						"Error - "
						"can't read file '%s': %s\n",
						file_path, strerror(errno));
					break;
				}
				/* pad 0 */
				memset (origin_data
					+ (last_download_block_num - 1)
					* SD_BLOCK_SIZE + last_block_len,
					'\0', SD_BLOCK_SIZE - last_block_len);
			} else {
				retval = read(fd, origin_data,
					      last_download_block_num
					      * SD_BLOCK_SIZE);
				if (retval < last_download_block_num
				    * SD_BLOCK_SIZE) {
					fprintf(stderr,
						"Error - "
						"can't read file '%s': %s\n",
						file_path, strerror(errno));
					break;
				}
			}
		} else {
			retval = read(fd, origin_data,
				      SD_MAX_BLOCK_NUM * SD_BLOCK_SIZE);
			if (retval < SD_MAX_BLOCK_NUM * SD_BLOCK_SIZE) {
				fprintf(stderr,
					"Error - can't read file '%s': %s\n",
					file_path, strerror(errno));
				break;
			}
		}

		/* download */
		if (i + 1 == download_times)
			retval = sd_card_write(ingenic_dev,
					       block_addr + SD_MAX_BLOCK_NUM
					       * i,
					       last_download_block_num,
					       origin_data);
		else
			retval = sd_card_write(ingenic_dev,
					       block_addr + SD_MAX_BLOCK_NUM
					       * i,
					       SD_MAX_BLOCK_NUM,
					       origin_data);
		if (retval != 1)
			break;

		/* check */
		if (check) {
			if (i + 1 == download_times)
				retval = sd_card_read(ingenic_dev,
						      block_addr
						      + SD_MAX_BLOCK_NUM * i,
						      last_download_block_num,
						      readback_data);
			else
				retval = sd_card_read(ingenic_dev,
						      block_addr
						      + SD_MAX_BLOCK_NUM * i,
						      SD_MAX_BLOCK_NUM,
						      readback_data);
			if (retval != 1)
				return;
			/* int p; */
			/* printf("\n origin  : "); */
			/* for (p = 0; p < 30; p++) */
			/* 	printf("%02x ", */
			/* 	       (unsigned char)*(origin_data + p)); */
			/* printf("\n readback: "); */
			/* for (p = 0; p < 30; p++) */
			/* 	printf("%02x ", */
			/* 	       (unsigned char)*(readback_data + p)); */
			/* printf("\n"); */
			
			if (i + 1 == download_times)
				retval = memcmp(origin_data, readback_data,
						last_download_block_num
						* SD_BLOCK_SIZE);
			else
				retval = memcmp(origin_data, readback_data,
						SD_MAX_BLOCK_NUM
						* SD_BLOCK_SIZE);
			if (retval != 0) {
				printf("Check error\n");
				return -1;
			}
		}
		
		printf("#");
		fflush(stdout);
	}
	printf("\n");

	if (i != download_times) {
		fprintf(stderr, "Error - sd program exit.\n");
		goto out2;
	}

	free(origin_data);
	if (check)
		free(readback_data);
	return 1;

out2:
	if (check)
		free(readback_data);
out1:
	free(origin_data);
out0:
	return -1;
}
