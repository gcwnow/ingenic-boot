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

LDFLAGS := -L../../lib -lcmd -lusb

all : ../../bin/stage2_cmd_tool

../../bin/stage2_cmd_tool : stage2_main.o stage2_sd.o
	gcc -o ../../bin/stage2_cmd_tool stage2_main.o stage2_sd.o $(LDFLAGS)

stage2_main.o : stage2_main.c
	gcc -c stage2_main.c

stage2_sd.o : stage2_sd.c
	gcc -c stage2_sd.c

clean :
	rm -rf stage2_main.o stage2_sd.o ../../bin/stage2_cmd_tool
