#!/bin/sh

####################################################
# Shell script to process the merging of all
# datasets of an HDF group into a FITS Table.
#
# This shell script is launched by the
# hdf2fits-merge.sub HTCondor job description script.
# See the file hdf2fits-group.sub for further details.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC February 14, 2022  20:20
####################################################

export JOBNAME=$1
export HOMEDIR=$2
export HOSTDIR=$3
export OUTPUTDIR=$4
export INPUTFILE=$5
export GROUP=$6

export SLAVEDIR=$_CONDOR_SCRATCH_DIR

### Determine the basename of the input filename
fname=$INPUTFILE
nodir=$(basename $fname)
export FILENAMEBASE=${nodir%.*}

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
echo " Group=$GROUP"
echo " Slavedir=$SLAVEDIR"
echo " "

cd $SLAVEDIR

### Import the necessary input file(s)
cp $HOSTDIR/hdf2fits-merge.py script.py
cp $INPUTFILE data.hdf

### Load the Python environment
source /ice3/software/iihe/python.sh

### Execute the Python script
echo " Converting the numpy data ..."
python script.py -g $GROUP

### Rename and transfer the produced output file
rm -f $OUTPUTFILE
mv -f data.fits $OUTPUTFILE

echo " "
echo "*** Job $JOBNAME completed ***"
