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

#include <stdio.h>
#include "../basic_cmd_lib/basic_cmd_lib.h"

struct ingenic_dev *ingenic_dev;

static void dev_init()
{
	ingenic_dev = malloc(sizeof (struct ingenic_dev));
	memset(ingenic_dev, 0, sizeof(struct ingenic_dev));
	if (usb_ingenic_init(ingenic_dev))
		exit (-1);
}

int main (int argc, char *argv[])
{
	if (argc != 2)
		goto helpmsg;

	if (strcmp(argv[1], "probe") == 0) {
		dev_init();
		if (usb_get_ingenic_cpu(ingenic_dev))
			return -1;
		return 0;
	}

	if (strncmp(argv[1], "addr=", 5) == 0) {
		/* size@addr */
		unsigned long addr;

		argv[1] += 5;
		addr = strtoul(argv[1], &argv[1], 0);

		printf(" addr=0x%lx\n", addr);

		dev_init();

		if (usb_send_data_address_to_ingenic(ingenic_dev, addr))
			return -1;

		return 0;
	}

	if (strcmp(argv[1], "flush") == 0) {
		dev_init();
		return usb_ingenic_flush_cache(ingenic_dev);
	}

	if (strncmp(argv[1], "if=", 3) == 0) {
		/* filename */

		struct stat fstat;
		unsigned int len;
		char *buf;

		argv[1] += 3;

		int status = stat(argv[1], &fstat);

		if (status < 0) {
			fprintf(stderr, "Error - can't get file size from '%s': %s\n",
				argv[1], strerror(errno));
			return -1;
		}

		len = fstat.st_size;
		buf = malloc(fstat.st_size);

		int fd = open(argv[1], O_RDONLY);

		if (fd < 0) {
			fprintf(stderr, "Error - can't open file '%s': %s\n",
				argv[1], strerror(errno));
			return -1;
		}

		status = read(fd, buf, len);
		close(fd);

		if (status < len) {
			fprintf(stderr, "Error - can't read file '%s': %s\n",
				argv[1], strerror(errno));
		}

		printf(" download len=%d\n", len);

		dev_init();
		return usb_send_data_to_ingenic(ingenic_dev, buf, len);
	}

	if (strncmp(argv[1], "of=", 3) == 0) {
		/* size@filename */
		unsigned long size;
		char *buf;

		argv[1] += 3;
		size = strtoul(argv[1], &argv[1], 0);
		if (*argv[1] != '@')
			return -1;
		argv[1]++;

		buf = malloc(size);

		int fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC);

		if (fd < 0) {
			fprintf(stderr, "Error - can't create file '%s': %s\n",
				argv[1], strerror(errno));
			return -1;
		}

		printf(" size=0x%lx, file=%s\n", size, argv[1]);

		dev_init();
		if (usb_read_data_from_ingenic(ingenic_dev, buf, size))
			return -1;

		write(fd, buf, size);
		close(fd);

		return 0;
	}

	if (strncmp(argv[1], "start=", 6) == 0) {
		/* 1,2@addr */
		unsigned long choice, addr;

		argv[1] += 6;
		choice = strtoul(argv[1], &argv[1], 0);

		if ((choice != 1) && (choice != 2))
			return -1;

		if (*argv[1] == '@')
			addr = strtoul(++argv[1], &argv[1], 0);
		else
			return -1;

		printf(" choice=%ld, addr=0x%lx\n", choice, addr);

		dev_init();
		if (usb_ingenic_start(ingenic_dev, choice, addr))
			return -1;

		return 0;
	}

helpmsg:
	printf("Command of this tool:\n"
	       "	probe		: get cpu info\n"
	       "	addr=addr	: set addr\n"
	       "	flush		: flush fifo\n"
	       "	if=filename 	: download file to device memory\n"
	       "			  at addr previously set\n"
	       "	start=method@addr\n"
	       "			: execute fireware, method=1 or 2\n\n"
		);

	exit (0);
}
