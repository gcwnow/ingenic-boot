#!/bin/bash

export TOOLPATH=$(cd "$(dirname "$0")"; pwd)

chown root $TOOLPATH/../bin/basic_cmd_tool
chown root $TOOLPATH/../bin/stage2_cmd_tool
chmod +s $TOOLPATH/../bin/basic_cmd_tool
chmod +s $TOOLPATH/../bin/stage2_cmd_tool
ln -s $TOOLPATH/../lib/libcmd.so /usr/lib/libcmd.so
