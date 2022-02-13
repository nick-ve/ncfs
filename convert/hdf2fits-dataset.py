#!/usr/bin/env python

###############################################################
# Python script to convert an hdf dataset into FITS format.
#
# To perform the conversion, issue the command ($ = prompt)
#
# $ python hdf2fits-dataset.py datasetname
# 
# The input and output data files, as well as the dataset name
# are specified as input arguments to this script.
#
# Input arguments for this script :
# ds : (obligatory) The HDF5 DataSet (incl. the group if needed)
# -i : (optional) Input  file name (default is "data.hdf")
# -o : (optional) Output file name (default is "data.fits")
#
# Keeping the default file names allows the user to use this
# python script "as is" and provide/rename the corresponding
# files via the hdf2fits-dataset.sub HTCondor batch script.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC February 13, 2022  23:15
###############################################################

import argparse
import h5py
import numpy as np
from astropy.io import fits

### Declaring the input arguments
parser=argparse.ArgumentParser()
parser.add_argument("ds",help="DataSet name (include group if needed)")
parser.add_argument("-i","--i",default="data.hdf",help="Input filename (Default=data.hdf)")
parser.add_argument("-o","--o",default="data.fits",help="Output filename (Default=data.fits)")
args=parser.parse_args()

### The HDF input filename
ifname=args.i

### The FITS output filename
ofname=args.o

### The requested DataSet be converted (specify also the group if needed)
dsname=args.ds

print(" *** hdf2fits-dataset ***")
print(" Input filename  : ",ifname)
print(" DataSet         : ",dsname)
print(" Output filename : ",ofname)

h5f=h5py.File(ifname,"r")

### Store a PrimaryHDU in the output file
### This ensures to have the data always in the extension [1]
hdu0=fits.PrimaryHDU()
hdu0.writeto(ofname,overwrite=True)

### Store the requested DataSet in the corresponding FITS format
print(" Converting DataSet : ",dsname)
data=h5f[dsname][:]
fits.append(ofname,data)

### Re-open the file to enter the DataSet name into the corresponding HDU
hduL=fits.open(ofname)
n=len(hduL)
hdux=hduL[n-1]
hdr=hdux.header
hdr["EXTNAME"]=dsname
print(" --- FITS extension : ",hdr[0])

### Reverse the row numbering for IMAGE data to comply with the FITS (row,col) convention
if (hdr[0] == "IMAGE"):
 dat=hdux.data
 dat=np.flip(dat,axis=0)
 hdux.data=dat

### Overwrite the FITS file with the updated data
hduL.writeto(ofname,overwrite=True)

print(" *** Conversion finished. ***")
