#!/bin/sh
### Shell script to create a ROOT loadable ALPHA-CXX shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
### Nick van Eijndhoven, IIHE-VUB, Brussel, May 3, 2021  10:01 UTC
#
### Name of the produced shared library
lib=rnopack.so
#
### The option string for ALPHA-CXX shared lib compilation and linking ***
### For the ALPHA-CXX ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
alpharoot="-x cxx -g0 -shared -w1 -I$ROOTSYS/include -I$NCFS/ncfspack/source -o $lib"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "alpharoot = " $alpharoot 
#
### Go to the directory with the source files
cd $NCFS/rnopack/source
#
### Create the dictionary files
rootcint -f zzzrnopackdict.cxx -c -I$NCFS/ncfspack/source RNOHeaders.h RNOLinkDef.h
# 
### Compile and create the ROOT loadable shared library
cxx $alpharoot *.cxx   
#
rm zzzicepackdict.*
rm *.o
rm so_locations
# 
### Move the created lib to the corresponding ROOT subdirectory
mv $lib $ROOTSYS/lib

### Return to the scripts directory
cd ../scripts
#
echo ' '
echo '*** alphacxxlib done. Result in ' $lib
