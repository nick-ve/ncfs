#!/bin/sh
### Shell script to create a ROOT loadable GCC shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
### NvE 23-may-2000 UU-SAP Utrecht
# 
### Name of the produced shared library
lib=iceconvert.so
#
### The option string for GCC compilation of the .c code
### For the GCC ROOT loadable shared lib the strict requirements are
### dropped to avoid many warnings from the rootcint generated code
gcccomp="-fPIC -c -g0 -Wall -Wno-long-long -I$NCFS/ncfspack/source -I$NCFS/icepack/source -I$ROOTSYS/include"
#
### The option string for GCC shared lib compilation and linking ***
### For the GCC ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
gccroot="-fPIC -shared -g0 -ansi -pedantic -Wall -Wno-long-long -I$NCFS/ncfspack/source -I$NCFS/icepack/source -I$ROOTSYS/include -o $lib"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "gcccomp = " $gcccomp
echo "gccroot = " $gccroot
#
### Go to the directory with the source files
cd $NCFS/iceconvert/source
#
### Create the dictionary files
rootcint -f iceconvertdict.cxx -c -p -I$NCFS/ncfspack/source -I$NCFS/icepack/source ICEConvHeaders.h ICEConvLinkDef.h
# 
### Compile and create the ROOT loadable shared library
gcc $gcccomp *.c   
g++ $gccroot *.cxx *.o   
# 
rm -f *.o
# 
### Move the created lib to the corresponding ROOT subdirectory
mv -f $lib $NCFS/libs
mv -f iceconvertdict* $NCFS/libs

### Return to the scripts directory
cd ../scripts
#
echo ' ' 
echo '*** gcclib.sh done. Result in ' $lib 
