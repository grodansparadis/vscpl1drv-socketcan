#!/bin/sh

MAJOR_VERSION=`sed '35!d' ../vscp/src/vscp/common/version.h  | cut -b 33-`
MINOR_VERSION=`sed '36!d' ../vscp/src/vscp/common/version.h  | cut -b 33-`
RELEASE_VERSION=`sed '37!d' ../vscp/src/vscp/common/version.h  | cut -b 33-`
BUILD_VERSION=`sed '38!d' ../vscp/src/vscp/common/version.h  | cut -b 33-`
NAME_PLUS_VER=vscpl1drv-socketcan-$MAJOR_VERSION.$MINOR_VERSION.$RELEASE_VERSION
BUILD_FOLDER=/tmp/__build__/`date +vscp_build_%y%m%d_%H%M%S`

echo ---$NAME_PLUS_VER

# Create the build folder
echo "---Creating build folder:"$BUILD_FOLDER
mkdir -p $BUILD_FOLDER

# Clean project
make clean
rm dist/*
./clean_for_dist

echo "---Copying Debian_orig to destination folder"
cp -r debian_orig $BUILD_FOLDER

echo "---making tar"
tar -zcf $BUILD_FOLDER/$NAME_PLUS_VER.tar.gz *
echo $NAME_PLUS_VER.tgz created.
cd $BUILD_FOLDER
mkdir $NAME_PLUS_VER/
cd $NAME_PLUS_VER/
mkdir debian
tar -zxvf ../$NAME_PLUS_VER.tar.gz
dh_make --single --defaultless -f ../$NAME_PLUS_VER.tar.gz -a -s -c mit -y
cp -r ../debian_orig/* debian/
echo "---Now do 'dpkg-buildpackage -us -uc' or 'dpkg-buildpackage -b'"

cd $NAME_PLUS_VER
#debuild clean
debuild -us -uc

echo "If all is alright check /tmp/__BUILD__/ for Debian package "

#cp -r vscpl1drv-socketcan /tmp/__build__/vscpl1drv-socketcan-${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}
#cd /tmp/__build__
#tar czvf vscpl1drv-socketcan_${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}.tar.gz vscpl1drv-socketcan_${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}
#rm -rf vscpl1drv-socketcan_${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}vscpl1drv-socketcan_${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}
#tar xzvf vscpl1drv-socketcan_${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}.tar.gz
#cd vscpl1drv-socketcan-${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}
#dh_make -f ../vscpl1drv-socketcan_${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}.tar.gz
#cp -r debian_orig/* debian
#debuild -us -uc
