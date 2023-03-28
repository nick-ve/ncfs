#!/bin/sh

##################################################################
# Shell script for simulation of an (obscured) astrophysical Jet.
# Both the p+p interactions of the jet with the dust
# and the p+gamma interactions within the jet are simulated.
#
# This shell script is launched by the jet-gen.sub
# HTCondor job description script.
# See the file jet-gen.sub for further details.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC March 28, 2023  07:19
#############################################################

export JOBNAME=$1
export HOMEDIR=$2
export HOSTDIR=$3
export OUTPUTDIR=$4
export FILENAME=$5

export SLAVEDIR=$_CONDOR_SCRATCH_DIR

### Determine the basename of the input filename
fname=$FILENAME
nodir=$(basename $fname)
export FILENAMEBASE=${nodir%.*}

export OUTPUTFILE=$OUTPUTDIR/$FILENAMEBASE.root

echo "*** Starting job $JOBNAME ***"
echo " "
echo " Jobname=$JOBNAME"
echo " Homedir=$HOMEDIR"
echo " Hostdir=$HOSTDIR"
echo " Outputdir=$OUTPUTDIR"
echo " Outputfile=$OUTPUTFILE"
echo " Slavedir=$SLAVEDIR"
echo " "

### Create the output directory, if not already present
mkdir -p $OUTPUTDIR

cd $SLAVEDIR

### Import the necessary input file(s)
cp $HOSTDIR/jet-gen.cc .

### Load the NCFS environment
source /software/icecube/iihe/ncfs.sh

### Execute the ROOT macro
echo " "
root -b -q jet-gen.cc

### Transfer the produced ROOT file
mv -f jet-output.root $OUTPUTFILE

echo " "
echo "*** Job $JOBNAME completed ***"
