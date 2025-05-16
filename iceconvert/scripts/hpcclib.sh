#!/bin/sh
### Shell script to create a ROOT loadable HP-CC shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
### NvE 28-jun-1999 UU-SAP Utrecht
# 
### Name of the produced shared library
lib=iceconvert.sl
#
### The option strings for HP-CC shared lib compilation and linking ***
### For the HP-CC ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
hpcomp="-c -s -z +z +a1 +w +DAportable -I$NCFS/ncfspack/source -I$NCFS/icepack/source -I$ROOTSYS/include"
hproot="-c -s -z +z +a1 +DAportable -I$NCFS/ncfspack/source -I$NCFS/icepack/source -I$ROOTSYS/include"
hplink="-L$ROOTSYS/lib/ -l*.sl -lm"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "hpcomp = " $hpcomp 
echo "hproot = " $hproot 
#
### Go to the directory with the source files
cd $NCFS/iceconvert/source
#
### Create the dictionary files
rootcint -f iceconvertdict.cxx -c -p -I$NCFS/ncfspack/source -I$NCFS/icepack/source ICEConvHeaders.h ICEConvLinkDef.h
# 
### Compile and create the ROOT loadable shared library
CC $hpcomp *.c   
CC $hproot *.cxx   
CC -b -o iceconvert.sl *.o
#
rm -f *.o
#
### Move the created lib to the corresponding ROOT subdirectory
mv -f $lib $NCFS/libs
mv -f iceconvertdict* $NCFS/libs
#
### Return to the scripts directory
cd ../scripts
#
echo ' ' 
echo '*** hpcclib.sh done. Result in ' $lib 
