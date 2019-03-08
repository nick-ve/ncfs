#!/bin/sh
### Shell script to create a ROOT loadable GCC shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
### NvE 23-may-2000 UU-SAP Utrecht
# 
### Name of the produced shared libraries
lib1=ncfspack.so
lib2=ncfspack.dylib
#
### Some MAC specific settings
export MACOSX_DEPLOYMENT_TARGET=10.3
unset LD_PREBIND
# 
### The option string for GCC shared lib compilation and linking ***
### For the GCC ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
gccroot="-c -g0 -ansi -pedantic -Wall -Wno-long-long -I$ROOTSYS/include"
#
echo "lib1 = " $lib1
echo "lib2 = " $lib2
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "gccroot = " $gccroot 
#
### Go to the directory with the source files
cd $NCFS/ncfspack/source
#
### Create the dictionary files
rootcint -f zzzncfspackdict.cxx -c NCFSHeaders.h NCFSLinkDef.h
# 
# 
### Compile and create the ROOT loadable shared library
#
# Compilation phase
g++ $gccroot *.cxx   
#
# Creating ncfspack.so library 
g++ -v -bundle -undefined dynamic_lookup -o $lib1 *.o
#
# Creating ncfspack.dylib library 
g++ -v -dynamiclib -undefined dynamic_lookup -single_module -o $lib2 *.o
### On some systems the following extra "-read_only_relocs" flag might be needed
# g++ -v -dynamiclib -undefined dynamic_lookup -single_module -read_only_relocs -o $lib2 *.o
# 
rm zzzncfspackdict.*
rm *.o
# 
### Move the created libs to the corresponding ROOT subdirectory
mv $lib1 $ROOTSYS/lib
mv $lib2 $ROOTSYS/lib

### Return to the scripts directory
cd ../scripts
#
#
echo ' ' 
echo '*** macgcclib done. Results in ' $lib1 ' and ' $lib2 
