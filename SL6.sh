#!/bin/sh
##################################################################
# Script to provide a Scientific Linux 6 environment via CVMFS.  #
# The settings here are tailored for the IIHE computer cluster.  #
# On other systems, just modify the corresponding mounts         #
# to match the configuration of the system, such that all        #
# needed libraries are accessible.                               #
#                                                                #
# An interactive environment can be created via the command :    #
# ($ is the command prompt)                                      #
#                                                                #
# $ source SL6.sh bash                                           #
#                                                                #
# after which all interactive commands will be executed in the   #
# Scientific Linux 6 environment.                                #
#                                                                #
# To execute (batch job) commands, the "bash" above has to be    #
# replaced by the desired command sequence.                      #
# For instance, to execute a ROOT macro test.cc in batch mode    #
# with the output redirected to a file test.log the format is :  # 
#                                                                #
# $ source SL6.sh root -b -q test.cc >test.log                   #
#                                                                #
# Nick van Eijndhoven, IIHE-VUB, Brussel, March 27, 2025  12:05Z #
##################################################################
#
### Setting the image
export SINGULARITY_IMAGE=/cvmfs/singularity.opensciencegrid.org/opensciencegrid/osgvo-el6:latest
export PATH=/bin:$PATH
#
### Fixing perl warning
export LC_ALL="en_US.UTF-8"
export LANG="en_US.UTF-8"
export LANGUAGE="en_US.UTF-8"
#
##############################
### Create the environment ###
##############################
#
### Setting env path inside container
#export SINGULARITYENV_PATH=$PATH
export APPTAINERENV_SING_USER_DEFINED_PATH=$PATH
#export SINGULARITYENV_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
export APPTAINERENV_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
#
### Defining mounts
SINGULARITY_MOUNTS="-B /cvmfs -B /pnfs -B /user -B /software"
if [ -d /scratch ];
then
    SINGULARITY_MOUNTS="$SINGULARITY_MOUNTS -B /scratch"
fi
#
### Running the command
singularity exec ${SINGULARITY_MOUNTS} ${SINGULARITY_IMAGE} $*
