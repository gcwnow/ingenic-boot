#!/bin/sh

export TOOLPATH=$(cd "$(dirname "$0")"; pwd)
export LD_LIBRARY_PATH=$TOOLPATH/../lib

helpmsg()
{
    echo "download file to memory:"
    echo "	hardware config specified by --config=XXX"
    echo "	file name specified by --bin=XXX"
    echo "	download addr specified by --downto=XXX"
    echo "	execute addr specified by --runat=XXX"
    echo
}

ARGS=`getopt -a -o h -l config:,bin:,downto:,runat:,help -- "$@"`
[ $? -ne 0 ] && helpmsg && exit
#set -- "${ARGS}"
eval set -- "${ARGS}"

while true
do
	case "$1" in
	--config)
		config="$2"
		;;
	--bin)
		bin="$2"
		;;
	--downto)
		downto="$2"
		;;
	--runat)
		runat="$2"
		;;
	-h|--help)
		cat helpmsg && exit
		;;
	--)
		shift
		break
		;;
	esac
shift
done

[ ! -n "$config" ] && helpmsg && exit
[ ! -n "$bin" ] && helpmsg && exit
[ ! -n "$downto" ] && helpmsg && exit
[ ! -n "$runat" ] && helpmsg && exit

$TOOLPATH/patch_fw.sh "$config" || exit

echo
echo "probe 1th"
$TOOLPATH/../bin/basic_cmd_tool probe || exit

echo
echo "addr set 0x80002000"
$TOOLPATH/../bin/basic_cmd_tool addr=0x80002000 || exit

echo
echo "download fw-cfg-${config}.bin"
$TOOLPATH/../bin/basic_cmd_tool if="$TOOLPATH/../fw/fw-cfg-${config}.bin" || exit

echo
echo "start1@0x80002000"
$TOOLPATH/../bin/basic_cmd_tool start=1@0x80002000 || exit

echo
echo "probe 2th"
$TOOLPATH/../bin/basic_cmd_tool probe || exit
sleep 0.1

echo
echo "addr set $downto"
$TOOLPATH/../bin/basic_cmd_tool addr="$downto" || exit
sleep 0.1

echo
echo "download $bin"
$TOOLPATH/../bin/basic_cmd_tool if="$bin" || exit
sleep 0.5

echo
echo "flush cache"
$TOOLPATH/../bin/basic_cmd_tool flush || exit
sleep 0.1

echo
echo "start2@$runat"
$TOOLPATH/../bin/basic_cmd_tool start=2@"$runat" || exit
sleep 0.1

echo
echo "running..."
