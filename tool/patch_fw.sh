#!/bin/bash

if [ -n "$TOPPATH" ]; then
    cd $TOPPATH/fw
else
    cd $TOOLPATH/../fw
fi

../bin/fw_cfg_tool || exit

dd if=hand.fw_args.bin of=fw_ddr2.bin conv=notrunc bs=1 seek=8
dd if=hand.fw_args.bin of=usb_boot.bin conv=notrunc bs=1 seek=8
