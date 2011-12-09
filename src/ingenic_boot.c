/*
 * Copyright(C) 2006 Ingenic Semiconductor Inc.
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

#include <getopt.h>
#include "ingenic_boot.h"

struct ingenic_dev *ingenic_dev;
int boot_flag = 0;
int kernel_flag = 0;
int filesys_flag = 0;
int check_flag = 0;
int probe_flag = 0;

int do_work(void)
{
	int retval;
	
	printf("\n#usb_ingenic_init\n");
	retval = usb_ingenic_init(ingenic_dev);
	if (retval != 1)
		return -1;
	
	printf("\n#usb_get_ingenic_cpu\n");
	retval = usb_get_ingenic_cpu(ingenic_dev);
	if (retval != 1)
		return -1;

	/* probe device only */
	if (probe_flag)
		return 1;

	printf("\n#parse_configure\n");
	char cfg_file_path[PATH_MAX] = {0};
	strcat(cfg_file_path, ingenic_dev->tool_cfg.realpath);
	strcat(cfg_file_path, "fw/current.cfg");
	retval = parse_configure(ingenic_dev, cfg_file_path);
	if (retval != 1)
		return -1;

	printf("\n#download stage1 bin\n");
	if (usb_download_stage1_stage2(ingenic_dev,
				       ingenic_dev->tool_cfg.fw_stage1_path, 1) < 1)
		return -1;
	
	printf("\n#download stage2 bin\n");
	if (usb_download_stage1_stage2(ingenic_dev,
				       ingenic_dev->tool_cfg.fw_stage2_path, 2) < 1)
		return -1;

	printf("\n#init_cfg\n");
	retval = init_cfg(ingenic_dev);
	if (retval == -1)
		return -1;


	retval = sd_card_init(ingenic_dev);
	if (retval != 1)
		return -1;

	/* xboot */
	if (boot_flag) {
		printf("\n#SD program bootloader...\n");
		retval = sd_card_program(ingenic_dev,
					 ingenic_dev->tool_cfg.img_bootloader_addr,
					 ingenic_dev->tool_cfg.img_bootloader_path,
					 check_flag);
		if (retval != 1)
			return -1;
	}

	/* kernel */
	if (kernel_flag) {
		printf("\n#SD program kernel...\n");
		retval = sd_card_program(ingenic_dev,
					 ingenic_dev->tool_cfg.img_kernel_addr,
					 ingenic_dev->tool_cfg.img_kernel_path,
					 check_flag);
		if (retval != 1)
			return -1;
	}

	/* system */
	if (filesys_flag) {
		printf("\n#SD program filesys...\n");
		retval = sd_card_program(ingenic_dev,
					 ingenic_dev->tool_cfg.img_filesys_addr,
					 ingenic_dev->tool_cfg.img_filesys_path,
					 check_flag);
		if (retval != 1)
			return -1;
	}

	/* reboot */
	printf("\n#Reboot\n");
	retval = usb_ingenic_sdram_ops(ingenic_dev, DEVICE_RESET);
	if (retval != 1)
		return -1;

	printf("\n#Done.\n\n");
	return 1;
}


static struct option long_options[] =
{
	/* These options set a flag. */
	{"boot",	optional_argument, &boot_flag, 1},
	{"kernel",	optional_argument, &kernel_flag, 1},
	{"filesys",	optional_argument, &filesys_flag, 1},
	{"check",	no_argument, &check_flag, 1},
	{"probe",	no_argument, &probe_flag, 1},
	/* These options don't set a flag.
	   We distinguish them by their indices. */
	{"help",	no_argument, 0, 'h'},
	{"version",	no_argument, 0, 'v'},
	{0, 0, 0, 0}
};
     
int main (int argc, char **argv)
{
	int c;
	ingenic_dev = malloc(sizeof (struct ingenic_dev));
	memset(ingenic_dev, 0, sizeof(struct ingenic_dev));

	if (argc < 2)
		goto helpmsg;

	/* prepare the realpath */
	readlink("/proc/self/exe", ingenic_dev->tool_cfg.realpath, PATH_MAX);
	dirname(ingenic_dev->tool_cfg.realpath);
	strcat(ingenic_dev->tool_cfg.realpath, "/");
	
	while (1)
	{
		/* getopt_long stores the option index here. */
		int option_index = 0;
     
		c = getopt_long (argc, argv, "hv",
				 long_options, &option_index);
     
		/* Detect the end of the options. */
		if (c == -1)
			break;
     
		switch (c) {
			
		case 0:
			/* printf ("option %s", long_options[option_index].name); */
			/* printf (" with arg %s", optarg); */
			/* printf ("\n"); */

			if (optarg != NULL) {
				switch (option_index) {

				case 0:
					strcat(ingenic_dev->tool_cfg.img_bootloader_path,
					       optarg);
					break;
				case 1:
					strcat(ingenic_dev->tool_cfg.img_kernel_path,
					       optarg);
					break;
				case 2:
					strcat(ingenic_dev->tool_cfg.img_filesys_path,
					       optarg);
					break;
				}
			}
			break;
     
		case '?':
		case 'h':
helpmsg:
			printf("\n\t--help or -h\t\t: this help screen\n"
			       "\t--version or -v\t\t: version num\n"
			       "\t--probe\t\t\t: only get device CPU infomation\n"
			       "\t--boot=<filename>\t: download the boot image to device\n"
			       "\t--kernel=<filename>\t: download the kernel image to device\n"
			       "\t--filesys=<filename>\t: download the filesys image to device\n"
			       "\t--check\t\t\t: read data back and check\n"
			       "\n\t\"=<filename>\" are optional, default setting"
			       " in fw/current.cfg\n\n"
				);
			abort ();
		case 'v':
			printf("\n\tversion: 1.1\n\n");
     
		default:
			abort ();
		}
	}
     
	/* Print any remaining command line arguments (not options). */
	if (optind < argc)
	{
		printf ("\tNon-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		printf ("\n\n");
		abort ();
	}

	do_work();
	
	exit (0);
}
