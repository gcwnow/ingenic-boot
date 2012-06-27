#
# ingenic_boot configuration file
#
# Utility to respond to the Ingenic XBurst USB boot protocol, provide
# initial boot stages and ability to access NAND on device.
#
# Authors: Duke Fong <duke@dukelec.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

LDFLAGS := -lconfuse

all : ../../bin/fw_cfg_tool

../../bin/fw_cfg_tool : fw_cfg_tool.o
	gcc -o ../../bin/fw_cfg_tool fw_cfg_tool.o $(LDFLAGS)

fw_cfg_tool.o : fw_cfg_tool.c
	gcc -c fw_cfg_tool.c

clean :
	rm -rf fw_cfg_tool.o ../../bin/fw_cfg_tool
