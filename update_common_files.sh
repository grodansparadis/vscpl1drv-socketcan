#!/bin/bash

# Warning!
# Do not run this script if you don't know what you do!

# General common files
cd _vscp_common_general
rm -f *
cp  ${VSCP_PATH}/src/common/dllist.c .
cp  ${VSCP_PATH}/src/common/dllist.h .
cp  ${VSCP_PATH}/src/common/crc.h .
cp  ${VSCP_PATH}/src/common/crc.c .
cd ..
# VSCP Common files
cd _vscp_common
rm -f *
cp  ${VSCP_PATH}/src/vscp/common/version.h .
cp  ${VSCP_PATH}/src/vscp/common/canal.h .
cp  ${VSCP_PATH}/src/vscp/common/canal_macro.h .
cp  ${VSCP_PATH}/src/vscp/common/vscp.h .
cp  ${VSCP_PATH}/src/vscp/common/vscp_class.h .
cp  ${VSCP_PATH}/src/vscp/common/vscp_type.h .
cp  ${VSCP_PATH}/src/vscp/common/guid.cpp .
cp  ${VSCP_PATH}/src/vscp/common/guid.h .
cd ..
