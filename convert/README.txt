//////////////////////////////////////////////////////////////////////////
//                Various format conversion facilities                  //
//////////////////////////////////////////////////////////////////////////

This folder contains various (Python) scripts and ROOT macros to convert
common data formats (e.g. Numpy, FITS, HDF5,...) into ROOT Trees.
For the conversion of Numpy and HDF5 data, the FITS format is used as an
intermediate stage, since the NcFITSIO facility provides an easy way to
convert FITS data into ROOT Trees.

The scripts and macros in this folder are of a rather generic character,
which implies that they may not provide the most optimal format and performance
for specific experimental data.
Scripts and macros that are more tailored for specific experiments (e.g. IceCube
and RNO-G) may be found in corresponding sub-folders (if available).

To facilitate the conversion of large data sets consisting of many individual files,
also (templates for) corresponding HTCondor scripts (i.e. the .sub files) are provided.
