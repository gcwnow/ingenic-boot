#!/bin/sh
git pull
make clean
make

rm -rf ingenic-boot-bin
mkdir ingenic-boot-bin
cp -r bin/ fw/ lib/ tool/ ingenic-boot README ingenic-boot-bin/
find ingenic-boot-bin/ -name .gitignore -exec rm {} \;
rm -f ingenic-boot-bin/fw/fw-cfg-*.bin
rm -f ingenic-boot-bin/fw/hand-*.bin
rm -f ingenic-boot-bin/fw/hand-fw-*.bin
rm -f ingenic-boot-bin/fw/usb_boot-cfg-*.bin

PACKAGE_FILE=ingenic-boot-bin.`date +'%Y-%m-%d'`.tar.gz
rm -f $PACKAGE_FILE
tar zcf $PACKAGE_FILE ingenic-boot-bin/
echo "Created package: $PACKAGE_FILE"
