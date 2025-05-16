#!/bin/sh
### Shell script to create a ROOT loadable GCC shared lib out of .cxx source code
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
### Nick van Eijndhoven, IIHE-VUB, Brussel, May 3, 2021  10:01 UTC
# 
### Name of the produced shared libraries
lib1=rnopack.so
lib2=rnopack.dylib
#
### Some MAC specific settings
export MACOSX_DEPLOYMENT_TARGET=10.3
unset LD_PREBIND
# 
### The option string for GCC shared lib compilation and linking ***
### For the GCC ROOT loadable shared lib the strict requirements are ***
### dropped to avoid many warnings from the rootcint generated code ***
gccroot="-c -g0 -ansi -pedantic -Wall -Wno-long-long -I$NCFS/ncfspack/source -I$ROOTSYS/include"
#
echo "lib1 = " $lib1
echo "lib2 = " $lib2
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS
echo "gccroot = " $gccroot 
#
### Go to the directory with the source files
cd $NCFS/rnopack/source
#
### Create the dictionary files
rootcint -f rnopackdict.cxx -c -I$NCFS/ncfspack/source RNOHeaders.h RNOLinkDef.h
# 
### Compile and create the ROOT loadable shared library
#
# Compilation phase
g++ $gccroot *.cxx   
#
# Creating ralice.so library 
g++ -v -bundle -undefined dynamic_lookup -o $lib1 *.o
#
# Creating ralice.dylib library 
g++ -v -dynamiclib -undefined dynamic_lookup -single_module -o $lib2 *.o
### On some systems the following extra "-read_only_relocs" flag might be needed
# g++ -v -dynamiclib -undefined dynamic_lookup -single_module -read_only_relocs -o $lib2 *.o
#
rm -f *.o
# 
### Move the created lib to the corresponding ROOT subdirectory
mv $lib1 $NCFS/libs
mv $lib2 $NCFS/libs
mv rnopackdict* $NCFS/libs

### Return to the scripts directory
cd ../scripts
#
echo ' ' 
echo '*** macgcclib.sh done. Results in ' $lib1 ' and ' $lib2
