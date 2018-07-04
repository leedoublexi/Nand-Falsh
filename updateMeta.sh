#!/bin/bash

#set -x
#set -e

CUR_DIR="`pwd`"

# update the meta.inf file automatically with the version and build date
echo '+++ Updating Meta.inf...'
#SZ rm Scripts/meta.new if any
rm -f meta.new
SCRIPT_VER="`cat meta.inf | grep ^Version= | cut -d. -f3-`"
BUILD_DATE="`date +%m/%d/%Y`"
sed '/^Version/d' meta.inf > meta.new
sed '/^BuildDate/d' meta.new > meta.inf
echo "BuildDate=\"$BUILD_DATE\"" >> meta.inf
echo "Version=\"$META_VER$SCRIPT_VER" >> meta.inf
rm -f meta.new
