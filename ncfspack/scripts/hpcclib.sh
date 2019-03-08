#!/bin/sh
### Shell script to create a ROOT loadable HP-CC shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
### NvE 28-jun-1999 UU-SAP Utrecht
#
### Name of the produced shared library
lib=ncfspack.sl
#
### The option strings for HP-CC shared lib compilation and linking ***
### For the HP-CC ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
hpcomp="-c -s -z +z +a1 +w +DAportable -I$ROOTSYS/include"
hproot="-c -s -z +z +a1 +DAportable -I$ROOTSYS/include"
hplink="-L$ROOTSYS/lib/ -l*.sl -lm"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "hproot = " $hproot 
#
### Go to the directory with the source files
cd $NCFS/ncfspack/source
#
### Create the dictionary files
rootcint -f zzzncfspackdict.cxx -c NCFSHeaders.h NCFSLinkDef.h
# 
### Compile and create the ROOT loadable shared library
CC $hproot *.cxx   
CC -b -o ncfspack.sl *.o
# 
rm zzzncfspackdict.*
rm *.o
# 
### Move the created lib to the corresponding ROOT subdirectory
mv $lib $ROOTSYS/lib

### Return to the scripts directory
cd ../scripts
#
echo ' ' 
echo '*** hpcclib done. Result in ' $lib 
