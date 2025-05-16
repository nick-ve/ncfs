#!/bin/sh
### Shell script to create all ROOT loadable GCC shared libs out of .cxx source code
###
### Note : Make sure that the environment variables ROOTSYS, NCFS and CFITSIO are correctly set.
###        See the file ncfs.sh in the top directory for examples.
###
### Nick van Eijndhoven, IIHE-VUB, Brussels, March 26, 2025.
# 
echo ' '
echo '************************************************************'
echo '*** Going to create the shared libraries of all packages ***'
echo '************************************************************'
# 
### Create the Pythia6 library
source $NCFS/Pythia6/amdgcclib.sh
# 
### Create the ncfspack library
source $NCFS/ncfspack/scripts/amdgcclib.sh
#
### Create the icepack library
source $NCFS/icepack/scripts/amdgcclib.sh
#
### Create the iceconvert library
source $NCFS/iceconvert/scripts/amdgcclib.sh
#
### Create the rnopack library
source $NCFS/rnopack/scripts/amdgcclib.sh
#
### Return to this NCFS top directory
cd $NCFS
# 
echo ' ' 
echo '**********************************************************'
echo '*** Shared libraries of all packages have been created ***'
echo '**********************************************************'
