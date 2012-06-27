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

all : ../../bin/basic_cmd_tool

../../bin/basic_cmd_tool : basic_cmd_tool.o
	gcc -o ../../bin/basic_cmd_tool basic_cmd_tool.o $(LDFLAGS)

basic_cmd_tool.o : basic_cmd_tool.c
	gcc -c basic_cmd_tool.c

clean :
	rm -rf basic_cmd_tool.o ../../bin/basic_cmd_tool
