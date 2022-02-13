#!/usr/bin/env python

#########################################################################
# Python script to convert all datasets of an hdf group into a FITS file.
#
# To perform the conversion, issue the command ($ = prompt)
#
# $ python hdf2fits-group.py
# 
# The input and output data files, as well as the group name may be
# specified as input arguments to this script.
#
# Input arguments for this script :
# -i : (optional) Input  file name (default is "data.hdf")
# -o : (optional) Output file name (default is "data.fits")
# -g : (optional) The HDF5 Group name (default is "/")
#
# Keeping the default file names allows the user to use this
# python script "as is" and provide/rename the corresponding
# files via the hdf2fits-group.sub HTCondor batch processing script.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels, UTC February 13, 2022  23:15
#########################################################################

import argparse
import h5py
import numpy as np
from astropy.io import fits

### Declaring the input arguments
parser=argparse.ArgumentParser()
parser.add_argument("-i","--i",default="data.hdf",help="Input filename (Default=data.hdf)")
parser.add_argument("-o","--o",default="data.fits",help="Output filename (Default=data.fits)")
parser.add_argument("-g","--g",default="/",help="Group name (Default=/)")
args=parser.parse_args()

### The HDF input filename
ifname=args.i

### The FITS output filename
ofname=args.o

### The requested Group be converted
gname=args.g

print(" *** hdf2fits-group ***")
print(" Input filename  : ",ifname)
print(" Group name      : ",gname)
print(" Output filename : ",ofname)

h5f=h5py.File(ifname,"r")

group=h5f[gname]

### Create the output file with the default Primary HDU
hdu0=fits.PrimaryHDU()
hdu0.writeto(ofname,overwrite=True)

### Store the requested DataSet in a temporary FITS file
for dsname in group.keys():
 print(" Converting DataSet : ",dsname)
 data=h5f[dsname][:]
 hdu0.writeto("temp.fits",overwrite=True)
 fits.append("temp.fits",data)

 ### Open the temp file to enter the DataSet name into the last added HDU
 hduL=fits.open("temp.fits")
 n=len(hduL)
 hdux=hduL[n-1]
 hdr=hdux.header
 hdr["EXTNAME"]=dsname
 print(" --- FITS extension : ",hdr[0])

 ### Reverse the row numbering for IMAGE data to comply with the FITS (row,col) convention
 if (hdr[0] == "IMAGE"):
  datx=hdux.data
  data=np.flip(datx,axis=0)

 ### Append the updated data to the final FITS output file
 fits.append(ofname,data,header=hdr)

print(" *** Conversion finished. ***")
