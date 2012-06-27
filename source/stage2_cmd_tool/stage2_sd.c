/*
 * Copyright(C) 2012 Ingenic Semiconductor Inc.
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

#include "stdio.h"
#include "../basic_cmd_lib/basic_cmd_lib.h"
#include "stage2.h"

/* 512K max per transmiter 1024 * 512 = 512KB */
#define SD_BLOCK_SIZE 512
#define SD_MAX_BLOCK_NUM 1024

/* no handshake */
static int sd_card_read(struct ingenic_dev *ingenic_dev,
			unsigned int block_addr, unsigned int block_num,
			char *buf)
{
	if (usb_send_data_address_to_ingenic(ingenic_dev, block_addr))
		return -1;

	if (usb_send_data_length_to_ingenic(ingenic_dev, block_num))
		return -1;

	if (usb_ingenic_nand_ops(ingenic_dev, CARD_READ))
		return -1;

	if (usb_read_data_from_ingenic(ingenic_dev, buf,
				       block_num * SD_BLOCK_SIZE))
		return -1;

	return 0;
}

/* have handshake */
static int sd_card_write(struct ingenic_dev *ingenic_dev,
			 unsigned int block_addr,
			 unsigned int block_num, char *buf)
{
	char hex_data[8];

	if (usb_send_data_address_to_ingenic(ingenic_dev, block_addr))
		return -1;
	if (usb_send_data_length_to_ingenic(ingenic_dev, block_num))
		return -1;

	/* do wirte first */
	if (usb_send_data_to_ingenic(ingenic_dev, buf,
				     block_num * SD_BLOCK_SIZE))
		return -1;

	/* then send request */
	if (usb_ingenic_nand_ops(ingenic_dev, CARD_PROGRAM))
		return -1;


	if (usb_read_data_from_ingenic(ingenic_dev, hex_data, 8))
		return -1;

	/* printf("Whandshake: %02x %02x %02x %02x %02x %02x %02x %02x.\n", */
	/*        hex_data[0],hex_data[1],hex_data[2],hex_data[3], */
	/*        hex_data[4],hex_data[5],hex_data[6],hex_data[7]); */

	return 0;
}

/* have handshake */
int sd_card_init(struct ingenic_dev *ingenic_dev)
{
	char hex_data[9];
	printf("\n#SD init\n");
	if (usb_ingenic_nand_ops(ingenic_dev, CARD_INIT))
		return -1;

	if (usb_read_data_from_ingenic(ingenic_dev, hex_data, 8))
		return -1;

	/* int i; */
	/* for (i = 0; i < 8; i++) { */
	/* 	printf("%x ", *(hex_data + i)); */
	/* } */

	return 0;
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

	/* <= 512 */
	last_block_len = fstat.st_size % SD_BLOCK_SIZE;
	if (last_block_len)
		block_nums = fstat.st_size / SD_BLOCK_SIZE + 1;
	else {
		last_block_len = SD_BLOCK_SIZE;		/* == 512 */
		block_nums = fstat.st_size / SD_BLOCK_SIZE;
	}
	printf(" last_block_len %d\n", last_block_len);
	printf(" block_nums %d\n", block_nums);
	/* <= 1024 */
	last_download_block_num = block_nums % SD_MAX_BLOCK_NUM;
	if (last_download_block_num)
		download_times = block_nums / SD_MAX_BLOCK_NUM + 1;
	else {
		last_download_block_num = SD_MAX_BLOCK_NUM;
		download_times = block_nums / SD_MAX_BLOCK_NUM;
	}
	printf(" last_download_block_num %d\n", last_download_block_num);
	printf(" download_times %d : ", download_times);
	fflush(stdout);

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
		if ((i + 1 == download_times) &&
		    (last_download_block_num != SD_MAX_BLOCK_NUM)) {

			if (last_block_len != SD_BLOCK_SIZE) {
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
		if (retval)
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
			if (retval)
				break;
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
			if (retval) {
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
	return 0;

out2:
	if (check)
		free(readback_data);
out1:
	free(origin_data);
out0:
	return -1;
}
