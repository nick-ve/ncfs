#!/usr/bin/env python

#########################################################
# Python script to convert a numpy file into a FITS file.
#
# To perform the conversion issue the command ($ = prompt)
#
# $ python npy2fits.py
# 
# The input and output data files are specified in this script.
# In this template the numpy input data file is named
# "data.npy" and the FITS output file is "data.fits".
#
# Keeping these generic names allows the user to use this
# python script "as is" and provide/rename the corresponding
# files via the npy2fits.sub HTCondor batch processing script.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC February 3, 2022  11:35
###############################################################

import numpy as np
from astropy.io import fits
#
### Specification of the input and output files
input_file="data.npy"
output_file="data.fits"
#
### Load the data from the numpy input file
data=np.load(input_file)
#
### Write the numpy data as the corresponding FITS structures
hdu=fits.ImageHDU()
fits.writeto(output_file,data,overwrite=True)
