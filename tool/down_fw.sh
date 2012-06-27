#!/bin/bash

echo
echo "probe 1th"
$TOPPATH/bin/basic_cmd_tool probe || exit

echo
echo "addr set 0x80002000"
$TOPPATH/bin/basic_cmd_tool addr=0x80002000 || exit

echo
echo "download fw_ddr2.bin"
$TOPPATH/bin/basic_cmd_tool if="$TOPPATH/fw/fw_ddr2.bin" || exit

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
echo "download usb_boot.bin"
$TOPPATH/bin/basic_cmd_tool if="$TOPPATH/fw/usb_boot.bin" || exit

echo
echo "flush cache"
$TOPPATH/bin/basic_cmd_tool flush || exit

echo
echo "start2@0x80002000"
$TOPPATH/bin/basic_cmd_tool start=2@0x80002000 || exit

echo
echo "probe 3th"
$TOPPATH/bin/basic_cmd_tool probe || exit
