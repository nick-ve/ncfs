#!/bin/sh
### Shell script to create a ROOT loadable GCC shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
###
### NvE 23-may-2000 UU-SAP Utrecht
# 
### Name of the produced shared library
lib=icepack.so
#
### The option string for GCC shared lib compilation and linking ***
### For the GCC ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
gccroot="-fPIC -shared -g0 -ansi -pedantic -Wall -Wno-long-long -Woverloaded-virtual -I$ROOTSYS/include -I$NCFS/ncfspack/source -o $lib"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "gccroot = " $gccroot 
#
### Go to the directory with the source files
cd $NCFS/icepack/source
#
### Create the dictionary files
rootcint -f zzzicepackdict.cxx -c -I$NCFS/ncfspack/source ICEHeaders.h ICELinkDef.h
# 
### Compile and create the ROOT loadable shared library
g++ $gccroot *.cxx   
# 
rm zzzicepackdict.*
rm *.o
# 
### Move the created lib to the corresponding ROOT subdirectory
mv $lib $ROOTSYS/lib

### Return to the scripts directory
cd ../scripts
#
echo ' ' 
echo '*** gcclib done. Result in ' $lib 
