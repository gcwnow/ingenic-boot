#!/bin/sh

if [ -z "$1" ]; then
    echo "Usage: patch_fw.sh <config>"
    exit 2
fi
CONFIG="$1"

if [ -n "$TOPPATH" ]; then
    cd $TOPPATH/fw
else
    cd $TOOLPATH/../fw
fi

CONFIG_FILE=${CONFIG}.cfg
if [ ! -e ${CONFIG_FILE} ]; then
    echo "No matching config file: fw/${CONFIG_FILE}"
    exit 1
fi
RAMTYPE=$(sed -ne 's/^\s*RAMTYPE\s*=\s*\([^ \t#]*\).*$/\1/p' ${CONFIG_FILE})
echo " SDRAM type: ${RAMTYPE}"

../bin/fw_cfg_tool ${CONFIG} || exit

FW_PATCHED=fw-cfg-${CONFIG}.bin
cp fw_${RAMTYPE}.bin ${FW_PATCHED}
dd if=hand-fw-${CONFIG}.bin of=${FW_PATCHED} conv=notrunc bs=1 seek=8

USB_PATCHED=usb_boot-cfg-${CONFIG}.bin
cp usb_boot.bin ${USB_PATCHED}
dd if=hand-fw-${CONFIG}.bin of=${USB_PATCHED} conv=notrunc bs=1 seek=8
