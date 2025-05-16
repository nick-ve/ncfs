#!/bin/sh
### Shell script to create a ROOT loadable ALPHA-CXX shared lib out of .cxx source code
###
### Note : Make sure that the environment variables ROOTSYS, NCFS and CFITSIO are correctly set.
###        See the file ncfs.sh in the top directory for examples.
###
### NvE 05-jul-2000 UU-SAP Utrecht
#
### Name of the produced shared library
lib=ncfspack.so
#
### The option string for ALPHA-CXX shared lib compilation and linking ***
### For the ALPHA-CXX ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
alpharoot="-x cxx -g0 -shared -w1 -I$CFITSIO -I$ROOTSYS/include -o $lib"
#
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "alpharoot = " $alpharoot 
#
### Go to the directory with the source files
cd $NCFS/ncfspack/source
#
### Create the dictionary files
rootcint -f ncfspackdict.cxx -c -I$CFITSIO NCFSHeaders.h NCFSLinkDef.h
# 
### Compile and create the ROOT loadable shared library
cxx $alpharoot *.cxx   
# 
rm -f *.o
rm -f so_locations
# 
### Move the created lib to the corresponding ROOT subdirectory
mv -f $lib $NCFS/libs
mv -f ncfspackdict* $NCFS/libs

### Return to the scripts directory
cd ../scripts
#
echo ' '
echo '*** alphacxxlib done. Result in ' $lib 
