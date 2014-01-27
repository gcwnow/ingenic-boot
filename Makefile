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

INSTALL ?= install
PREFIX ?= /usr/local

all: lib/libcmd.so bin/basic_cmd_tool bin/fw_cfg_tool bin/stage2_cmd_tool

lib/libcmd.so : force_look
	@cd source/basic_cmd_lib; make
	@echo

bin/basic_cmd_tool : force_look
	@cd source/basic_cmd_tool; make
	@echo

bin/fw_cfg_tool : force_look
	@cd source/fw_cfg_tool; make
	@echo

bin/stage2_cmd_tool : force_look
	@cd source/stage2_cmd_tool; make
	@echo

clean:
	@cd source/basic_cmd_lib; make clean;
	@echo
	@cd source/basic_cmd_tool; make clean;
	@echo
	@cd source/fw_cfg_tool; make clean;
	@echo
	@cd source/stage2_cmd_tool; make clean;
	@echo

force_look :
	@true

install-lib:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(PREFIX)/lib/ingenic-boot/lib/
	cp -a lib/libcmd.so $(DESTDIR)$(PREFIX)/lib/ingenic-boot/lib/libcmd.so

install: $(if $(STATIC),,install-lib)
	$(INSTALL) -s -D -m 0755 bin/basic_cmd_tool \
		$(DESTDIR)$(PREFIX)/lib/ingenic-boot/bin/basic_cmd_tool
	$(INSTALL) -s -D -m 0755 bin/fw_cfg_tool \
		$(DESTDIR)$(PREFIX)/lib/ingenic-boot/bin/fw_cfg_tool
	$(INSTALL) -s -D -m 0755 bin/stage2_cmd_tool \
		$(DESTDIR)$(PREFIX)/lib/ingenic-boot/bin/stage2_cmd_tool
	$(INSTALL) -d -m 0755 $(DESTDIR)$(PREFIX)/lib/ingenic-boot/fw/ $(DESTDIR)$(PREFIX)/lib/ingenic-boot/tool/
	cp -a fw/* $(DESTDIR)$(PREFIX)/lib/ingenic-boot/fw/
	cp -a tool/* $(DESTDIR)$(PREFIX)/lib/ingenic-boot/tool/
	$(INSTALL) -D -m 0644 README $(DESTDIR)$(PREFIX)/lib/ingenic-boot/
	$(INSTALL) -D -m 0755 ingenic-boot $(DESTDIR)$(PREFIX)/bin/ingenic-boot
