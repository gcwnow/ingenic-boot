#
# Copyright (C) 2006 Ingenic Semiconductor Inc.
#
# Makefile for the JZ4770 internal bootrom code.
#

CROSS	:= mipsel-linux-

CFLAGS	:= -O2 -mips32 -fno-pic -mno-abicalls -EL
LDFLAGS	:= -nostdlib -EL -T target.ld

OBJS1	= head.o main.o common.o board_4770.o jz4770_ddr1.o
OBJS2	= head.o main.o common.o board_4770.o jz4770_ddr2.o jz_ddr2_init.o
OBJS3	= head.o main.o common.o board_4770.o jz4770_mddr.o jz_mddr_init.o

all: fw_ddr1.bin fw_ddr2.bin fw_mddr.bin

fw_ddr1.bin: fw_ddr1.elf
	$(CROSS)objcopy -O binary $< $@
	$(CROSS)objdump -D $< > fw.dump
	$(CROSS)objdump -h $< > fw.map
	$(CROSS)nm -n $< > System.map
fw_ddr1.elf: $(OBJS1)
	$(CROSS)ld $(LDFLAGS) $(OBJS1) -o $@

fw_ddr2.bin: fw_ddr2.elf
	$(CROSS)objcopy -O binary $< $@
	$(CROSS)objdump -D $< > fw.dump
	$(CROSS)objdump -h $< > fw.map
	$(CROSS)nm -n $< > System.map
fw_ddr2.elf: $(OBJS2)
	$(CROSS)ld $(LDFLAGS) $(OBJS2) -o $@

fw_mddr.bin: fw_mddr.elf
	$(CROSS)objcopy -O binary $< $@
	$(CROSS)objdump -D $< > fw.dump
	$(CROSS)objdump -h $< > fw.map
	$(CROSS)nm -n $< > System.map
fw_mddr.elf: $(OBJS3)
	$(CROSS)ld $(LDFLAGS) $(OBJS3) -o $@

.c.o:
	$(CROSS)gcc $(CFLAGS) -c $< -o $@
.S.o:
	$(CROSS)gcc $(CFLAGS) -c $< -o $@
clean:
	rm -fr *.o *.elf *.bin *.dump *.map
