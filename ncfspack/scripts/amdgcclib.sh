#!/bin/sh
### Shell script to create a ROOT loadable GCC shared lib out of .cxx source code
###
### Note : Make sure that the environment variables ROOTSYS, NCFS and CFITSIO are correctly set.
###        See the file ncfs.sh in the top directory for examples.
###
### NvE 23-may-2000 UU-SAP Utrecht
# 
### Name of the produced shared library
lib=ncfspack.so
# 
### The option string for GCC shared lib compilation and linking ***
### For the GCC ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
gccroot="-fPIC -shared -g0 -ansi -pedantic -Wunused -Wno-long-long -Woverloaded-virtual -I$ROOTSYS/include -I$CFITSIO -o $lib"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "gccroot = " $gccroot 
#
### Go to the directory with the source files
cd $NCFS/ncfspack/source
#
### Create the dictionary files
rootcint -f zzzncfspackdict.cxx -c -I$CFITSIO NCFSHeaders.h NCFSLinkDef.h
# 
### Compile and create the ROOT loadable shared library
g++ $gccroot *.cxx   
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
echo '*** gcclib done. Result in ' $lib 
