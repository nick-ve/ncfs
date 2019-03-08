#!/bin/sh
### Shell script to create a ROOT loadable ALPHA-CXX shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
### NvE 05-jul-2000 UU-SAP Utrecht
#
### Name of the produced shared library
lib=iceconvert.so
#
### The option string for ALPHA-CXX compilation of the .c code ***
### For the ALPHA-CXX ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
alphacomp="-c -g0 -w1 -I$ROOTSYS/include -I$NCFS/ncfspack/source -I$NCFS/icepack/source"
#
### The option string for ALPHA-CXX shared lib compilation and linking ***
### For the ALPHA-CXX ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
alpharoot="-x cxx -g0 -shared -w1 -I$ROOTSYS/include -I$NCFS/ncfspack/source -I$NCFS/icepack/source -o $lib"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "alphacomp = " $alphacomp 
echo "alpharoot = " $alpharoot 
#
### Go to the directory with the source files
cd $NCFS/iceconvert/source
#
### Create the dictionary files
rootcint -f zzziceconvertdict.cxx -c -p -I$NCFS/ncfspack/source -I$NCFS/icepack/source ICEConvHeaders.h ICEConvLinkDef.h
# 
### Compile and create the ROOT loadable shared library
cxx $alphacomp *.c   
cxx $alpharoot *.cxx *.o   
#
rm zzziceconvertdict.*
rm *.o
rm so_locations
#
### Move the created lib to the corresponding ROOT subdirectory
mv $lib $ROOTSYS/lib
#
### Return to the scripts directory
cd ../scripts
#
echo ' '
echo '*** alphacxxlib.sh done. Result in ' $lib
