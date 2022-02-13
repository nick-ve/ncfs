#!/usr/bin/env python

#########################################################################
# Python script to merge all datasets of an hdf group into a FITS Table.
#
# To perform the conversion, issue the command ($ = prompt)
#
# $ python hdf2fits-merge.py
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
# files via the hdf2fits-merge.sub HTCondor batch processing script.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels, UTC February 13, 2022  23:16
#########################################################################

import argparse
import h5py
import numpy as np
from astropy.table import Table
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

### The requested Group to be converted
gname=args.g

print(" *** hdf2fits-merge ***")
print(" Input filename  : ",ifname)
print(" Group name      : ",gname)
print(" Output filename : ",ofname)

h5f=h5py.File(ifname,"r")

### The group to be converted ("/"= the whole file)
group=h5f[gname]

### Create an empty Table
t=Table()

### Loop over all the datasets and add each of them as a Table column
for key in group.keys():
 print(" Adding DataSet : ",key)
 datax=h5f[key][:]
 t[key]=datax

### Write the Table data into a FITS file
t.write(ofname,format="fits",overwrite=True)

print(" *** Conversion finished. ***")
