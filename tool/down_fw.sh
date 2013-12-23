#!/bin/sh

if [ -z "$1" ]; then
    echo "Usage: down_fw.sh <config>"
    exit 2
fi
config="$1"

FW_CFG=fw-cfg-${config}.bin
USB_CFG=usb_boot-cfg-${config}.bin

echo
echo "probe 1th"
$TOPPATH/bin/basic_cmd_tool probe || exit

echo
echo "addr set 0x80002000"
$TOPPATH/bin/basic_cmd_tool addr=0x80002000 || exit

echo
echo "download $FW_CFG"
$TOPPATH/bin/basic_cmd_tool if="$TOPPATH/fw/$FW_CFG" || exit

echo
echo "start1@0x80002000"
$TOPPATH/bin/basic_cmd_tool start=1@0x80002000 || exit

echo
echo "probe 2th"
$TOPPATH/bin/basic_cmd_tool probe || exit

echo
echo "addr set 0x80002000"
$TOPPATH/bin/basic_cmd_tool addr=0x80002000 || exit

echo
echo "download $USB_CFG"
$TOPPATH/bin/basic_cmd_tool if="$TOPPATH/fw/$USB_CFG" || exit

echo
echo "flush cache"
$TOPPATH/bin/basic_cmd_tool flush || exit

echo
echo "start2@0x80002000"
$TOPPATH/bin/basic_cmd_tool start=2@0x80002000 || exit

echo
echo "probe 3th"
$TOPPATH/bin/basic_cmd_tool probe || exit
