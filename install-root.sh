#!/bin/sh
### This is a shell script to install ROOT on the IIHE computer cluster.
### On other systems, just specify the corresponding paths where the
### ROOT source and Pythia files reside.
###
### Nick van Eijndhoven, IIHE-VUB, Brussel, Belgium.
#
### Go to the directory where the ROOT source resides
cd /software/icecube/root_v5.34.38
source configure  --enable-pythia6 --with-pythia6-libdir=/software/icecube/pythia-6.4.28 --enable-gsl-shared --enable-builtin-ftgl --enable-mathmore --enable-fftw3 --enable-fitsio --enable-opengl
#
make -j4
 
