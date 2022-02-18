#!/usr/bin/env python

######################################################################
# Python script to convert a 1D hdf dataset into a FITS table column.
#
# To perform the conversion, issue the command ($ = prompt)
#
# $ python hdf2fits-column.py datasetname
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
# files via the hdf2fits-column.sub HTCondor batch script.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC February 18, 2022  16:27
######################################################################

import argparse
import h5py
import numpy as np
from astropy.table import Table
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

print(" *** hdf2fits-column ***")
print(" Input filename  : ",ifname)
print(" DataSet         : ",dsname)
print(" Output filename : ",ofname)

h5f=h5py.File(ifname,"r")

### Create an empty Table
t=Table()

### Add the dataset as a Table column
print(" Adding DataSet  : ",dsname)
datax=h5f[dsname][:]
t[dsname]=datax

### Write the Table data into a FITS file
t.write(ofname,format="fits",overwrite=True)

print(" *** Conversion finished. ***")
