#
# Copyright (C) 2006 Ingenic Semiconductor Inc.
#
# Makefile for the JZ4740 internal bootrom code.
#

CROSS	:= mipsel-linux-

CFLAGS	:= -O2 -mips32 -fno-pic -mno-abicalls
LDFLAGS	:= -nostdlib -EL -T target.ld 
VPATH += ./jz4755 ./jz4725b ./jz4750
FWDIR := .
#SOURCES += $(wildcard $(FWDIR)/jz4755/*.c) 

SOURCES += $(wildcard $(FWDIR)/jz4755/*.c)
SOURCES += $(wildcard $(FWDIR)/jz4725b/*.c)
SOURCES += $(wildcard $(FWDIR)/jz4750/*.c)
SOURCES += $(wildcard $(FWDIR)/memory.c)

#SOURCES := ./jz4755/jz4755.c  ./jz4755serialport.c 


#OBJS	= head.o main.o jz4755serialport.o jz4755.o jz4725bserialport.o jz4725b.o
OBJS	= head.o main.o jz4755serialport.o jz4755.o jz4725b.o board_4750.o memory.o

all: fw.bin

fw.bin: fw.elf
	$(CROSS)objcopy -O binary $< $@
	$(CROSS)objdump -D $< > fw.dump
	$(CROSS)objdump -h $< > fw.map
	$(CROSS)nm -n $< > System.map
#	cp fw.bin ../../../../tools

fw.elf: $(OBJS)
	$(CROSS)ld $(LDFLAGS) $(OBJS) -o $@
.c.o:
	$(CROSS)gcc $(CFLAGS) -c $< -o $@
.S.o:
	$(CROSS)gcc $(CFLAGS) -c $< -o $@
clean:
	rm -fr *.o *.elf *.bin *.dump *.map
