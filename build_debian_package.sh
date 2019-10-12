#!/bin/sh

# Package version
MAJOR_VERSION=`head -n4  VERSION.m4 |  grep major_version | tr -d "m4_define[major_version], ()"`
MINOR_VERSION=`head -n4  VERSION.m4 |  grep minor_version | tr -d "m4_define[minor_version], ()"`
RELEASE_VERSION=`head -n4  VERSION.m4 |  grep release_version | tr -d "m4_define[release_version], ()"`
BUILD_VERSION=`head -n4  VERSION.m4 |  grep build_version | tr -d "m4_define[build_version], ()"`

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
#ls
echo "---Now do 'dpkg-buildpackage -us -uc' or 'dpkg-buildpackage -b'"

#cd $NAME_PLUS_VER
#debuild clean
debuild -us -uc

echo "If all is alright check /tmp/__BUILD__/ for Debian package "
