#!/bin/sh

####################################################
# Shell script to process the conversion of
# a numpy data file into a FITS file.
#
# This shell script is launched by the npy2fits.sub
# HTCondor job description script.
# See the file npy2fits.sub for further details.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC February 7, 2022  08:56
####################################################

export JOBNAME=$1
export HOMEDIR=$2
export HOSTDIR=$3
export OUTPUTDIR=$4
export INPUTFILE=$5

export SLAVEDIR=$_CONDOR_SCRATCH_DIR

export FILENAMEBASE=`basename -s .npy $INPUTFILE`

export OUTPUTFILE=$OUTPUTDIR/$FILENAMEBASE.fits

echo "*** Starting job $JOBNAME ***"
echo " "
echo " Jobname=$JOBNAME"
echo " Homedir=$HOMEDIR"
echo " Hostdir=$HOSTDIR"
echo " Outputdir=$OUTPUTDIR"
echo " Inputfile=$INPUTFILE"
echo " FilenameBase=$FILENAMEBASE"
echo " Outputfile=$OUTPUTFILE"
echo " Slavedir=$SLAVEDIR"
echo " "

cd $SLAVEDIR

### Import the necessary input file(s)
cp $HOSTDIR/npy2fits.py script.py
cp $INPUTFILE data.npy

### Load the Python environment
source /ice3/software/iihe/python.sh

### Execute the Python script
echo " Converting the numpy data ..."
python script.py

### Rename and transfer the produced output file
rm -f $OUTPUTFILE
mv -f data.fits $OUTPUTFILE

echo " "
echo "*** Job $JOBNAME completed ***"
