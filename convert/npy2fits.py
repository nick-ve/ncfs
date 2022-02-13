#!/usr/bin/env python

#########################################################
# Python script to convert a numpy file into a FITS file.
#
# To perform the conversion, issue the command ($ = prompt)
#
# $ python npy2fits.py
# 
# The input and output data files may be specified as
# input arguments to this script.
#
# Input arguments for this script :
# -i : (optional) Input  file name (default is "data.npy")
# -o : (optional) Output file name (default is "data.fits")
#
# Keeping the default file names allows the user to use this
# python script "as is" and provide/rename the corresponding
# files via the npy2fits.sub HTCondor batch processing script.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC February 13, 2022  23:17
###############################################################

import argparse
import numpy as np
from astropy.io import fits

### Declaring the input arguments
parser=argparse.ArgumentParser()
parser.add_argument("-i","--i",default="data.npy",help="Input filename (Default=data.npy)")
parser.add_argument("-o","--o",default="data.fits",help="Output filename (Default=data.fits)")
args=parser.parse_args()

### The numpy input filename
ifname=args.i

### The FITS output filename
ofname=args.o

print(" *** npy2fits ***")
print(" Input filename  : ",ifname)
print(" Output filename : ",ofname)

### Load the data from the numpy input file
data=np.load(ifname)

### Store a PrimaryHDU in the output file
### This ensures to have the data always starting with the extension [1]
hdu0=fits.PrimaryHDU()
hdu0.writeto(ofname,overwrite=True)

### Append the numpy data as the corresponding FITS structures
fits.append(ofname,data)

print(" *** Conversion finished ***")
