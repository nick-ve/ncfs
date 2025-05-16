# /bin/sh
### Shell script to create the ROOT loadable Pythia6 shared lib.
### This script is based on the script makePythia6.linuxx8664
###
### Note : An environment variable NCFS has to be set, pointing to the
###        location where the topdirectory of the NCFS software is residing.  
###
###
### Nick van Eijndhoven, IIHE-VUB, Brussel, May 13, 2025  11:32Z
# 
### Name of the produced shared library
lib=libPythia6.so

#
echo ' '
echo '*** Creation of the Pythia6 shared library for NcCollider ***' 
echo "lib = " $lib
echo "ROOTSYS = " $ROOTSYS
echo "NCFS = " $NCFS

### Go to the Pythia6 folder
cd $NCFS/Pythia6
#
### Perform the compilation 
gfortran -c -m64 -fPIC pythia*.f
gfortran -c -m64 -fPIC  -fno-second-underscore tpythia6_called_from_cc.F
echo 'void MAIN__() {}' >main.c
gcc -c -m64 -fPIC main.c
gcc -c -m64 -fPIC pythia6_common_address.c
#
### Create the shared library
gfortran -m64 -shared -o $lib *.o
#
echo '*** amdgcclib done. Result in ' $lib 
