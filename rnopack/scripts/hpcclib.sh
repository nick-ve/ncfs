#!/bin/sh
### Shell script to create a ROOT loadable HP-CC shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
### Nick van Eijndhoven, IIHE-VUB, Brussel, May 3, 2021  10:01 UTC
# 
### Name of the produced shared library
lib=rnopack.sl
#
### The option strings for HP-CC shared lib compilation and linking ***
### For the HP-CC ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
hpcomp="-c -s -z +z +a1 +w +DAportable -I$NCFS/ncfspack/source -I$ROOTSYS/include"
hproot="-c -s -z +z +a1 +DAportable -I$NCFS/ncfspack/source -I$ROOTSYS/include"
hplink="-L$ROOTSYS/lib/ -l*.sl -lm"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "hproot = " $hproot 
#
### Go to the directory with the source files
cd $NCFS/rnopack/source
#
### Create the dictionary files
rootcint -f rnopackdict.cxx -c -I$NCFS/ncfspack/source RNOHeaders.h RNOLinkDef.h
# 
### Compile and create the ROOT loadable shared library
CC $hproot *.cxx   
#
CC -b -o rnopack.sl *.o
#
rm -f *.o
# 
### Move the created lib to the corresponding ROOT subdirectory
mv -f $lib $NCFS/libs
mv -f rnopackdict* $NCFS/libs

### Return to the scripts directory
cd ../scripts
#
echo '*** hpcclib.sh done. Result in ' $lib
