/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.                     *
 *                                                                             *
 * Authors: The Netherlands Center for Fundamental Studies (NCFS).             *
 *          The Inter-university Institute for High Energies (IIHE).           *                 
 *                    Website : http://www.iihe.ac.be                          *
 *            Contact : Nick van Eijndhoven (nickve.nl@gmail.com)              *
 *                                                                             *
 * Contributors are mentioned in the code where appropriate.                   *
 *                                                                             * 
 * No part of this software may be used, copied, modified or distributed       *
 * by any means nor transmitted or translated into machine language for        *
 * commercial purposes without written permission by the IIHE representative.  *
 * Permission to use the software strictly for non-commercial purposes         *
 * is hereby granted without fee, provided that the above copyright notice     *
 * appears in all copies and that both the copyright notice and this           *
 * permission notice appear in the supporting documentation.                   *
 * This software is provided "as is" without express or implied warranty.      *
 * The authors make no claims that this software is free of error, or is       *
 * consistent with any particular standard of merchantability, or that it      *
 * will meet your requirements for any particular application, other than      *
 * indicated in the corresponding documentation.                               *
 * This software should not be relied on for solving a problem whose           *
 * incorrect solution could result in injury to a person or loss of property.  *
 * If you do use this software in such a manner, it is at your own risk.       *
 * The authors disclaim all liability for direct or consequential damage       *
 * resulting from your use of this software.                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///////////////////////////////////////////////////////////////////////////
// Class NcFITSIO
// Class to provide an I/O facility for FITS data files.
//
// This class requires the CFITSIO library to be installed on your system.
// It is maintained by NASA and can be downloaded from :
//
//       https://heasarc.gsfc.nasa.gov/docs/software/fitsio
//
// Note that on some operating systems (e.g. Ubuntu) facilities exist
// to automatically import the compiled library from server machines. 
//
// The FITS reference manual is available via the link :
//
//  https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/cfitsio.html
//
// Note : 
// ------
// This class has been inspired by the code released on 19-jul-2010
// by Claudi Martinez via the files TFITS.h and TFITS.cxx.
//
// NcFITSIO is an interface for the treatment of Flexible Image Transport System
// (FITS) files, which were initially used in astronomy. This file format
// was introduced in the late 70's and today it is widely used among scientists.
// FITS files can contain a large variety of (multi-dimensional) data like
// for instance spectra, data tables, histograms and multi-layered images.
// Furthermore, FITS data files are self explanatory since they contain
// human-readable information to allow the interpretation of the data within
// the FITS file. For example, a FITS file could contain a 3D data cube, 
// but an additional description could indicate that it represents, for 
// example, a 3-layer image.
// As such, it is very well suited to make experimental data available
// to the general community. 
//
// The basic data entity is a so called Header-Data Unit (HDU) and the
// NcFITSIO facility provides several methods to access the data of
// the various HDUs that may be present in a FITS file.
// An HDU is a set of data with a header containing several tokens as
// "keyword = value". The header describes the data structure within
// the HDU. An HDU can be of two types: an "Image HDU" or a "Table HDU".
// The former can be any kind of multidimensional set of numerical values.
// So, an "Image" should be regarded as an N-dimensional data object,
// which may indeed contain a (multi-layered) image, but also the data
// of a matrix, a 1D, 2D or 3D histogram or even space-time detector signals.
// On the other hand, a Table is a set of several rows and columns
// (columns are also called fields) which contain generic data, such as strings,
// integer, real or even complex numbers.
// In a Table, a (row,column) element may also contain an array of data,
// which may even have a variable number of array elements for different rows.
// In this NCFITSIO2 facility, a Table is regarded as a 3D data cube, where
// a specific single data item is addressed as (row,column,layer).
// The convention is that all row, column and layer numbering starts at 1.
//
// Current limitations :
// - No support yet for complex values within data tables
// - No support yet for grouping
// - No interfaces have yet been implemented for writing FITS files
//
// Examples :
// ==========
//
// Example 1 :
// -----------
// The file contains only the primary HDU which consists of
// an image of NGC7662.
// We retrieve the first layer of the image array as a TASImage object
// and as a 2-dimensional histogram and display both on a canvas.
// For illustrative purposes we also retrieve the data as a matrix.
//
// gSystem->Load("ncfspack");
//
// TString filename="$(ROOTSYS)/tutorials/fitsio/sample1.fits";
//
// NcFITSIO q;
//
// if (!q.OpenInputFile(filename)) return;
//
// q.ListFileHeader();
//  
// Double_t thres=1000; // Threshold for pixel value
// Double_t max=1;      // Rescale pixel values to [0,max]
//
// TASImage im;
// q.GetImageLayer(im,1,&thres,max); // Image with threshold and rescaling
//
// TH2D hist;
// q.GetImageLayer(hist,1,0,max); // Histogram with only rescaling 
//
// TMatrixD m;
// q.GetImageLayer(m,1); // Matrix without threshold and rescaling
//
// TCanvas* c1=new TCanvas("c1","NcFITSIO Example 1: Image from layer");
// im.Draw();
//
// TCanvas* c2=new TCanvas("c2","NcFITSIO Example 1: Histogram from layer");
// hist.Draw("COLZ");
//
// Example 2 :
// -----------
// The primary HDU is an array of 2 rows by 2040 columns,
// which represent a radiation spectrum.
// The first row contains the flux data, whereas the second row the wavelengths.
// We will extract the data and show the flux vs. wavelength graph on a canvas.
// Data copyright: NASA.
//
// gSystem->Load("ncfspack");
//
// TString filename="$(ROOTSYS)/tutorials/fitsio/sample2.fits";
//
// NcFITSIO q;
//
// if (!q.OpenInputFile(filename)) return;
//
// q.ListFileHeader(0);
// q.ListHDUHeader();
//
// Int_t dim=q.GetImageDimension();    // The dimension "N" of the N-dimensional data object
//
// Int_t nmeas=q.GetImageDimension(1); // The number of measurements (i.e. columns)
//
// TArrayI ifirst(dim); // Array to indicate the starting point for reading in each dimension
//
// TArrayD a1;        // Array to hold the data of row 1
// ifirst.SetAt(1,0); // Start index 1 of dimension 1 (i.e. columnn 1)
// ifirst.SetAt(1,1); // Start at index 1 of dimension 2 (i.e. row 1)
// q.GetImageArray(a1,ifirst,nmeas);
//
// TArrayD a2;        // Array to hold the data of row 2
// ifirst.SetAt(1,0); // Start at index 1 of dimension 1 (i.e. column 1)
// ifirst.SetAt(2,1); // Start at index 2 of dimension 2 (i.e. row 2)
// q.GetImageArray(a2,ifirst,nmeas);
//
// TGraph* gr=new TGraph(nmeas,a2.GetArray(),a1.GetArray());
// TCanvas* c1=new TCanvas("c1","NcFITSIO Example 2: Spectrum from Image data");
// gr->Draw("AP");
//
// Example 3 :
// -----------
// The primary HDU contains an array representing a flux vs. wavelength spectrum.
// The extension #1 contains a data table with 2 rows.
// We will read from the table row(s) the column named DATAMAX.
// Data copyright: NASA.
//
// gSystem->Load("ncfspack");
//
// TString filename="$(ROOTSYS)/tutorials/fitsio/sample2.fits";
//
// NcFITSIO q;
//
// if (!q.OpenInputFile(filename)) return;
//
// q.ListFileHeader(0);
//  
// // Open the extension #1 which contains the data table
// if (!q.SelectHDU(1)) return;
//
// q.ListTable();   // List the table column description
// q.ListTable(10); // List the table contents
//
// // Retrieve the actual values of the column named DATAMAX.
// TString colname="DATAMAX";
// TArrayD darr; // Array to hold the actual data
// q.GetTableColumn(darr,colname);
//
// // List the contents of the retrieved data array
// cout << endl;
// cout << " Data contents of the column " << colname << endl;
// for (Int_t i=1; i<=darr.GetSize(); i++)
// {
//  cout << " Row:" << i << " value:" << darr.At(i-1) << endl;
// }
//
// Example 4 :
// -----------
// The primary HDU contains an array representing a flux vs. wavelength spectrum.
// The extension #1 contains a data table with 2 rows.
// We will read only the row(s) that have a column named DATAMAX with a
// value greater than 2e-15.
// Data copyright: NASA.
//
// gSystem->Load("ncfspack");
//
// TString filename="$(ROOTSYS)/tutorials/fitsio/sample2.fits";
//
// NcFITSIO q;
//
// if (!q.OpenInputFile(filename)) return;
//
// q.ListFileHeader(0);
//  
// // Open the extension #1 which contains the data table with a selection filter
// if (!q.SelectHDU("[1][DATAMAX>2e-15]")) return;
//
// q.ListTable();   // List the table column description
// q.ListTable(10); // List the table contents
//
// // Retrieve the actual values of the column named DATAMAX.
// TString colname="DATAMAX";
// TArrayD darr; // Array to hold the actual data
// q.GetTableColumn(darr,colname);
//
// // List the contents of the retrieved data array
// cout << endl;
// cout << " Data contents of the column " << colname << endl;
// for (Int_t i=1; i<=darr.GetSize(); i++)
// {
//  cout << " Row:" << i << " value:" << darr.At(i-1) << endl;
// }
//
// Example 5 :
// -----------
// The primary HDU contains an array representing a flux vs. wavelength spectrum.
// The extension #1 contains a data table with 9 rows and 8 columns.
// Column 4 has the name "mag" and contains a vector of 6 numeric components.
// We will read the "mag" values of the first 5 rows.
// The "mag" values of rows 1 and 2 are :
// Row1: (99.0, 24.768, 23.215, 21.68, 21.076, 20.857)
// Row2: (99.0, 21.689, 20.206, 18.86, 18.32 , 18.128)
//
// gSystem->Load("ncfspack");
//
// TString filename="$(ROOTSYS)/tutorials/fitsio/sample4.fits";
//
// NcFITSIO q;
//
// if (!q.OpenInputFile(filename)) return;
//
// q.ListFileHeader();
//  
// // Open the extension #1 which contains the data table
// if(!q.SelectHDU(1)) return;
//
// q.ListTable();   // List the table column description
// q.ListTable(10); // List the table contents
//
// // Read the "mag" values of rows 1 to 5 (incl.)
// TArrayD arr;
// for (Int_t jrow=1; jrow<=5; jrow++)
// { 
//  q.GetTableCell(arr,jrow,"mag");
//
//  // List the contents of the data array
//  cout << endl;
//  cout << " \"mag\" data contents of Row:" << jrow << endl;
//  for (Int_t i=1; i<=arr.GetSize(); i++)
//  {
//   cout <<  " " << arr.At(i-1);
//  }
//  cout << endl;
// }
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel March 13, 2019  03:40
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel February 8, 2022  17:02Z
///////////////////////////////////////////////////////////////////////////

#include "NcFITSIO.h"
#include "Riostream.h"
 
ClassImp(NcFITSIO) // Class implementation to enable ROOT I/O

NcFITSIO::NcFITSIO(const char* name,const char* title) : TNamed(name,title)
{
// Default constructor

 Initialize();
}
/////////////////////////////////////////////////////////////////////////// 
NcFITSIO::~NcFITSIO()
{
// Default destructor.

 Reset();
}
///////////////////////////////////////////////////////////////////////////
NcFITSIO::NcFITSIO(const NcFITSIO& q) : TNamed(q)
{
// Copy constructor

 Initialize();
 fFilename=q.fFilename;
 fFilenameFilter=q.fFilenameFilter;

 if (!LoadHeaderInfo())
 {
  cout << endl;
  cout << " *" << ClassName() << "::NcFITSIO* Failure in copy constructor." << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcFITSIO::Initialize()
{
// Initialization of all parameters 

 fFilename="";           // The (full path) name of the FITS file on the computer system
 fFilenameFilter="";     // The FITS filename with the HDU selection filter
 fInput=0;               // Pointer to the FITS input file
 fOutput=0;              // Pointer to the FITS output file
 fType=kImageHDU;        // The HDU type
 fExtensionName="";      // Extension name of the HDU
 fExtensionNumber=0;     // Extension number of the HDU (0=PRIMARY)
 fNkeys=0;               // The number of HDU keywords
 fKeyNames=0;            // The HDU key names
 fKeyValues=0;           // The HDU key values
 fComments=0;            // The HDU (key) comments
 fNrows=0;               // The number of table rows
 fNcolumns=0;            // The number of table columns
 fColumnNames=0;         // The names of the table columns
 fColumnTypes=0;         // The types of the table columns
 fColumnLayers=0;        // The number of layers of the table column
 fSizes=0;               // Image sizes in each dimension (when fType == kImageHDU)
}
///////////////////////////////////////////////////////////////////////////
void NcFITSIO::Reset()
{
// Reset all allocated memory and parameters

 if (fKeyNames)
 {
  delete[] fKeyNames;
  fKeyNames=0;
 }
 if (fKeyValues)
 {
  delete[] fKeyValues;
  fKeyValues=0;
 }
 if (fComments)
 {
  delete[] fComments;
  fComments=0;
 }
 if (fColumnNames)
 {
  delete[] fColumnNames;
  fColumnNames=0;
 }
 if (fColumnTypes)
 {
  delete[] fColumnTypes;
  fColumnTypes=0;
 }
 if (fColumnLayers)
 {
  delete[] fColumnLayers;
  fColumnLayers=0;
 }
 if (fSizes)
 {
  delete fSizes;
  fSizes=0;
 }

 // Reset all parameters
 Initialize();
}
/////////////////////////////////////////////////////////////////////////// 
Bool_t NcFITSIO::OpenInputFile(TString specs)
{
// Open a FITS input file and indicate success (kTRUE) or failure (kFALSE)
// via the boolean return value.
//
// The input argument "specs" indicates the FITS input filename
// with an optional Header Data Unit (HDU) selection filter.
//
// Examples of "specs" format:
// ---------------------------
// "myfile.fits"                  // Open the PRIMARY HDU of file "myfile.fits"
// "myfile.fits[1]"               // Open the HDU of extension #1 in file "myfile.fits"
// "myfile.fits[SPECTRUM]"        // Open the HDU called "SPECTRUM" in file "myfile.fits"
// "myfile.fits[HUBBLE][#row<10]" // Open the HDU called "HUBBLE" and select only row numbers <10
// "myfile.fits[HUBBLE][RA>0.5]"  // Open the HDU called "HUBBLE" and select only those rows
//                                   that have in the column named "RA" values >0.5
//
// For further details on HDU selection filters please refer to:
//
//    https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/cfitsio.html
//
// Notes :
// -------
// 1) Environment variables may be used as $(...) in the filenname for convenience.
//    For example "$(ROOTSYS)/tutorials/fitsio/sample1.fits".
// 2) A specifiec HDU can also be selected by invoking the memberfunction SelectHDU()
//    after a FITS input file has been opened.

 TString file=gSystem->ExpandPathName(specs.Data());
   
 Initialize();
   
 // Store both the filename with the optional HDU selection filter appended and the plain filename
 fFilenameFilter=file;
 fFilename=StripFilter(fFilenameFilter);

 Bool_t good=LoadHeaderInfo();

 if (good)
 {
  cout << endl;
  cout << " *" << ClassName() << "::OpenInputFile* FITS file specs: " << fFilenameFilter << endl;
 }
 else
 {
  cout << endl;
  cout << " *" << ClassName() << "::OpenInputFile* Could not open " << fFilenameFilter << endl;
  Reset();
 }

 return good;  
}
///////////////////////////////////////////////////////////////////////////
TString NcFITSIO::StripFilter(TString filename) const
{
// Strip the (optional) HDU filter selection string from the filename.

 TString s=filename;
 Int_t idx=s.Index("[",1,0,TString::kExact);
 if (idx>=0) s.Resize(idx);

 return s;
}
///////////////////////////////////////////////////////////////////////////
Bool_t NcFITSIO::LoadHeaderInfo()
{
// Load the header records of the current HDU and indicate success (kTRUE) or failure (kFALSE)
// via the boolean return value.

 Bool_t good=kTRUE;
 fInput=0;
 int status=0;

 if (fKeyNames) delete[] fKeyNames;
 if (fKeyValues) delete[] fKeyValues;
 if (fComments) delete[] fComments;

 // Open the FITS file as specified via OpenInputFile() or SelectHDU()
 fits_open_file(&fInput,fFilenameFilter.Data(),READONLY,&status);
 
 if (status)
 {
  cout << endl;
  cout << " *" << ClassName() << "::LoadHeaderInfo* Could not open " << fFilenameFilter << endl;
  Reset();
  good=kFALSE;
  return good;
 }

 // Read the number of this HDU
 int hdunum;
 fits_get_hdu_num(fInput,&hdunum);
 fExtensionNumber=Int_t(hdunum)-1;

 // Read the type of this HDU
 int hdutype;
 fits_get_hdu_type(fInput,&hdutype,&status);
 
 if (status)
 {
  cout << endl;
  cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve the HDU type." << endl;
  fits_close_file(fInput,&status);
  Reset();
  good=kFALSE;
  return good;
 }

 fType=kTableHDU;
 if (hdutype==IMAGE_HDU) fType=kImageHDU;

 // Read the HDU header records
 int nkeys,morekeys;
 fits_get_hdrspace(fInput,&nkeys,&morekeys,&status);
 
 if (status)
 {
  cout << endl;
  cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve the HDU header space." << endl;
  fits_close_file(fInput,&status);
  Reset();
  good=kFALSE;
  return good;
 }

 // Store the HDU header information
 fKeyNames=new TString[nkeys];
 fKeyValues=new TString[nkeys];
 fComments=new TString[nkeys];

 char keyname[FLEN_KEYWORD+1];
 char keyvalue[FLEN_VALUE+1];
 char comment[FLEN_COMMENT+1];
 fNkeys=0;
 for (Int_t i=1; i<=nkeys; i++)
 {
  fits_read_keyn(fInput,i,keyname,keyvalue,comment,&status);

  if (status)
  {
   cout << endl;
   cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve info of HDU key record "<< i << endl;
   fits_close_file(fInput,&status);
   Reset(); 
   good=kFALSE;
   return good;
  }

  fKeyNames[i-1]=keyname;
  fKeyValues[i-1]=keyvalue;
  fComments[i-1]=comment;

  fNkeys++;

  // Obtain the extension name
  fExtensionName="[";
   if (fKeyNames[i-1]=="EXTNAME")
  {
   fExtensionName+=fKeyValues[i-1];
   fExtensionName.ReplaceAll("'",""); // Remove the single quotes
  }
  else
  {
   fExtensionName+=fExtensionNumber;
  }
  fExtensionName+="]";
 }

 // Obtain row and column information in case of Table data
 if (fType==kTableHDU)
 {
  long nrows;
  fits_get_num_rows(fInput,&nrows,&status);

  if (status)
  {
   cout << endl;
   cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve number of table rows." << endl;
   fits_close_file(fInput,&status);
   Reset(); 
   good=kFALSE;
   return good;
  }
      
  fNrows=Int_t(nrows);
      
  int ncols;
  fits_get_num_cols(fInput,&ncols,&status);

  if (status)
  {
   cout << endl;
   cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve number of table columns." << endl;
   fits_close_file(fInput,&status);
   Reset(); 
   good=kFALSE;
   return good;
  }

  // Store the Table column information
  fNcolumns=Int_t(ncols);
  fColumnNames=new TString[ncols];
  fColumnTypes=new eColumnTypes[ncols];
  fColumnLayers=new Int_t[ncols];
            
  // Read the column names
  status=0;
  char colname[80];     
  int jcol;
  fits_get_colname(fInput,CASEINSEN,(char*)"*",colname,&jcol,&status);

  if (status==COL_NOT_FOUND)
  {
   cout << endl;
   cout << " *" << ClassName() << "::LoadHeaderInfo* Could not find any table column." << endl;
   fits_close_file(fInput,&status);
   Reset(); 
   good=kFALSE;
   return good;
  }

  if (jcol>0 && jcol<=ncols) fColumnNames[jcol-1]=colname;

  while (status==COL_NOT_UNIQUE)
  {
   fits_get_colname(fInput,CASEINSEN,(char*)"*",colname,&jcol,&status);
   if (jcol>0 && jcol<=ncols) fColumnNames[jcol-1]=colname;
  }
  if (status && status!=COL_NOT_FOUND)
  {
   cout << endl;
   cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve name of table column " << jcol << endl;
   fits_close_file(fInput,&status);
   Reset(); 
   good=kFALSE;
   return good;
  }

  // Read the column data types
  status=0;
  int typecode;
  long repeat,width;
  Int_t dim=1; // Dimension of the stored column elements
  for (jcol=1; jcol<=fNcolumns; jcol++)
  {
   fits_get_coltype(fInput,jcol,&typecode,&repeat,&width,&status);

   if (status)
   {
    cout << endl;
    cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve data type of table column " << jcol << endl;
    fits_close_file(fInput,&status);
    Reset(); 
    good=kFALSE;
    return good;
   }

   // Determine the dimension of the stored column elements
   dim=Int_t(repeat);
   if (typecode==TSTRING) dim=Int_t(repeat/width);
   if (dim<=0) dim=1;

   fColumnLayers[jcol-1]=dim;

   if (typecode==TSTRING)
   {
    fColumnTypes[jcol-1]=kString;
    if (dim>1) fColumnTypes[jcol-1]=kStringArray;
   }
   else if (typecode==TLOGICAL)
   {
    fColumnTypes[jcol-1]=kLogical;
    if (dim>1) fColumnTypes[jcol-1]=kLogicalArray;
   }
   else if (typecode==TCOMPLEX || typecode==TDBLCOMPLEX)
   {
    fColumnTypes[jcol-1]=kComplexNumber;
    if (dim>1) fColumnTypes[jcol-1]=kComplexArray;
   }
   else
   {
    fColumnTypes[jcol-1]=kRealNumber;
    if (dim>1) fColumnTypes[jcol-1]=kRealArray;
   }
  }
 } // End of Table info

 // Obtain dimension and size information in case of Image data
 if (fType==kImageHDU)
 {
  // The number of dimensions of the N-dimensional data 
  status=0;
  int ndims=0;
  fits_get_img_dim(fInput,&ndims,&status);

  if (status)
  {
   cout << endl;
   cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve the number of dimensions of the Image." << endl;
   fits_close_file(fInput,&status);
   Reset(); 
   good=kFALSE;
   return good;
  }

  // Empty Image
  if (ndims<=0)
  {
   fSizes=new TArrayI();
   return good;
  }

  // The size of each dimension
  long* dimsizes=new long[ndims];
  fits_get_img_size(fInput,ndims,dimsizes,&status);

  if (status)
  {
   cout << endl;
   cout << " *" << ClassName() << "::LoadHeaderInfo* Could not retrieve the sizes of the dimensions of the Image." << endl;
   fits_close_file(fInput,&status);
   Reset(); 
   good=kFALSE;
   delete[] dimsizes;
   return good;
  }

  // Store the size of each dimension
  fSizes=new TArrayI(ndims);
  for (Int_t i= 0; i<ndims; i++)
  {
   fSizes->SetAt(dimsizes[i],i);
  }

  delete [] dimsizes;
 } // End of Image info

 return good;
}
///////////////////////////////////////////////////////////////////////////
Bool_t NcFITSIO::SelectHDU(TString extname)
{
// Select the HDU with the specified extension name and indicate success (kTRUE)
// or failure (kFALSE) via the boolean return value.
//
// Examples for extension name specifications :
// --------------------------------------------
// "[HUBBLE]" will select the HDU stored as [HUBBLE].
// "[HUBBLE][#row<10]" will select the HDU stored as [HUBBLE] and select only row numbers <10
// "[HUBBLE][RA>0.5]" will select the HDU stored as [HUBBLE] and select only
//                    those rows that have in column "RA" values >0.5
//
// For further details on HDU selection filters please refer to:
//
//    https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/cfitsio.html
//
// The default value is extname="[0]", which is the primary HDU.

 fFilenameFilter=fFilename;
 fFilenameFilter+=extname;

 Bool_t good=LoadHeaderInfo();

 if (good)
 {
  cout << endl;
  cout << " *" << ClassName() << "::SelectHDU* Current selection: " << fFilenameFilter << endl;
 }
 else
 {
  cout << endl;
  cout << " *" << ClassName() << "::SelectHDU* Could not select " << fFilenameFilter << endl;
  Reset();
 }
 
 return good;
}
///////////////////////////////////////////////////////////////////////////
Bool_t NcFITSIO::SelectHDU(Int_t extnumber)
{
// Select the HDU with the specified extension number and indicate success (kTRUE)
// or failure (kFALSE) via the boolean return value.
//
// Example : extnumber=3 will select the HDU stored as [3].
//
// The primary HDU is stored as [0].
//
// For a more elaborate HDU selection, please refer to the SelectHDU()
// memberfunction that selects the HDU via a name specification.

 TString extname="[";
 extname+=extnumber;
 extname+="]";

 Bool_t good=SelectHDU(extname);

 return good;
}
///////////////////////////////////////////////////////////////////////////
TString NcFITSIO::GetKeywordValue(TString keyname,Int_t mode)
{
// Provide a TString with the value of the HDU keyword with the specified name.
// 
// If no match is found, an empty string is returned.
//
// mode = 0 --> The HDU keyword name should match exactly the provided "keyname"
//        1 --> The HDU keyword name only has to contain the provided "keyname" pattern.
//
// The default value is mode=0.
//
// Notes :
// -------
// 1) Leading and trailing blanks and single quotes will be removed from the value string.
//    For example, the HDU keyword value 'Zenith  ' will be provided as TString="Zenith".
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().


 TString value="";

 for (Int_t i=0; i<fNkeys; i++)
 {
  if (fKeyNames[i]==keyname || (mode && fKeyNames[i].Contains(keyname.Data())))
  {
   value=fKeyValues[i];
   break;
  } 
 }

 value.ReplaceAll("'","");          // Remove the single quotes
 value=value.Strip(value.kBoth,' '); // Remove leading and trailing blanks

 return value;
}
///////////////////////////////////////////////////////////////////////////
Bool_t NcFITSIO::IsTable() const
{
// Indicate whether the current HDU is a Table (kTRUE) or not (kFALSE).

 Bool_t flag=kFALSE;
 if (fType==kTableHDU) flag=kTRUE;

 return flag; 
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableNrows() const
{
// Provide the number of rows in the table

 return fNrows;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableNcolumns() const
{
// Provide the number of columns in the table

 return fNcolumns;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetColumnNumber(TString colname,Int_t mode) const
{
// Provide the number of the first table column that matches the provided name pattern.
// Column numbers start at 1.
// 
// If no match is found, the value 0 is returned.
//
// mode = 0 --> The column name should match exactly the provided "colname"
//        1 --> The column name only has to contain the provided "colname" pattern.
//
// The default value is mode=0.
   
 Int_t colnum=0;
 Int_t match=0;
 for (Int_t i=1; i<=fNcolumns; i++)
 {
  if (!mode && fColumnNames[i-1]==colname) match=1;
  if (mode && fColumnNames[i-1].Contains(colname)) match=1;
  if (match)
  {
   colnum=i;
   break;
  }
 }
 return colnum;
}
///////////////////////////////////////////////////////////////////////////
TString NcFITSIO::GetColumnName(Int_t colnum) const
{
// Provide the name of the table column with number "colnum".
// Column numbers start at 1.
//
// In case of inconsistent data an empty string is returned.

 TString name="";

 if (fType!=kTableHDU || colnum<1 || colnum>fNcolumns) return name;

 name=fColumnNames[colnum-1];
 return name;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(Double_t &val,Int_t row,Int_t col,Int_t layer)
{
// Provide the real number value of the cell (row,col,layer) in a table.
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
// The dimension of the full array is reflected by the return argument,
// which in case of a single data element is 1.
//
// The row, column and layer counting starts at 1.
//
// The default value is layer=1.
//
// In case of inconsistent data the value 0 is provided with 0 as return argument.

 TArrayD arr;
 Int_t ndim=GetTableCell(arr,row,col);

 if (!ndim || layer<1 || layer>ndim)
 {
  val=0;
  return 0;
 }

 val=arr[layer-1];
 return ndim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(Double_t &val,Int_t row,TString colname,Int_t layer,Int_t mode)
{
// Provide the real number value of the cell matching the row number "row",
// column name (pattern) "colname" and layer number "layer" in a table.
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
// The dimension of the full array is reflected by the return argument,
// which in case of a single data element is 1.
//
// mode = 0 --> The column name should match exactly the provided "colname"
//        1 --> The column name only has to contain the provided "colname" pattern.
//
// The row, column and layer counting starts at 1.
//
// The default values are layer=1 and mode=0.
//
// In case of inconsistent data the value 0 is provided with 0 as return argument.

 Int_t col=GetColumnNumber(colname,mode);

 Int_t ndim=GetTableCell(val,row,col);

 return ndim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(TArrayD &arr,Int_t row,Int_t col)
{
// Provide via array "arr" the real number value(s) of the cell (row,col) in a table.
// The row and column counting starts at 1.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an empty array "arr" is provided with 0 as return argument.

 arr.Set(0);

 if (row<=0 || row>fNrows || col<=0 || col>fNcolumns) return 0;

 if (fType!=kTableHDU || fColumnTypes[col-1]==kString || fColumnTypes[col-1]==kStringArray) return 0;

 // Obtain the number of (array) elements stored in this (row,col) cell.
 Int_t dim=0;
 int colnum=col;
 long long rownum=row;
 long repeat=0;
 long offset=0;
 int status=0;
 fits_read_descript(fInput,colnum,rownum,&repeat,&offset,&status);

 if (!status) // The column was created to hold a variable number of elements
 {
  dim=repeat;
 }
 else // The column was created to hold a fixed number of elements
 {
  dim=fColumnLayers[col-1];
 }
 
 if (dim<=0) return 0;

 // Read the cell contents into an array
 double nulval=0;
 int anynul=0;
 double* array=new double[dim];
 status=0;
 if (fColumnTypes[col-1]==kLogical || fColumnTypes[col-1]==kLogicalArray)
 {
  bool* barray=new bool[dim];
  bool bnulval=0;
  fits_read_col(fInput,TLOGICAL,col,row,1,dim,&bnulval,barray,&anynul,&status);
  for (Int_t i=0; i<dim; i++)
  {
   array[i]=double(barray[i]);
  }
  delete [] barray;
 }
 else
 {
  fits_read_col(fInput,TDOUBLE,col,row,1,dim,&nulval,array,&anynul,&status);
 }

 if (status)
 {
  cout << endl;
  cout << " *" << ClassName() << "::GetTableCell* Could not retrieve data type of table cell [" 
               << row << "," << col << "]." << endl;
  fits_close_file(fInput,&status);
  Reset();
  delete [] array; 
  return 0;
 }

 // Copy the cell values into the output array
 arr.Set(dim);
 for (Int_t i=0; i<dim; i++)
 {
  arr.SetAt(array[i],i);
 }

 delete [] array;
 return dim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(TArrayD &arr,Int_t row,TString colname,Int_t mode)
{
// Provide via array "arr" the real number value(s) of the cell matching the row number "row"
// and column name (pattern) "colname" in a table.
// The row and column counting starts at 1.
//
// mode = 0 --> The column name should match exactly the provided "colname"
//        1 --> The column name only has to contain the provided "colname" pattern.
//
// The default value is mode=0.
//
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an empty array "arr" is provided with 0 as return argument.

 Int_t col=GetColumnNumber(colname,mode);

 Int_t ndim=GetTableCell(arr,row,col);

 return ndim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(TString &str,Int_t row,Int_t col,Int_t layer)
{
// Provide the data item from the cell (row,col,layer) in a table as a TString.
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
// The dimension of the full array is reflected by the return argument,
// which in case of a single data element is 1.
//
// The row, column and layer counting starts at 1.
//
// The default value is layer=1.
//
// In case of inconsistent data an empty string is provided with 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().

 TString* arr=0;
 Int_t ndim=GetTableCell(arr,row,col);
 if (!ndim || layer<1 || layer>ndim)
 {
  str="";
  ndim=0;
 }
 else
 {
  str=arr[layer-1];
 }
 
 delete[] arr;
 return ndim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(TString &str,Int_t row,TString colname,Int_t layer,Int_t mode)
{
// Provide the data item of the cell matching the row number "row",
// column name (pattern) "colname" and layer number "layer" in a table as a TString.
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
// The dimension of the full array is reflected by the return argument,
// which in case of a single data element is 1.
//
// mode = 0 --> The column name should match exactly the provided "colname"
//        1 --> The column name only has to contain the provided "colname" pattern.
//
// The row, column and layer counting starts at 1.
//
// The default values are layer=1 and mode=0.
//
// In case of inconsistent data an empty string is provided with 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().

 Int_t col=GetColumnNumber(colname,mode);

 Int_t ndim=GetTableCell(str,row,col,layer);

 return ndim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(TString* &arr,Int_t row,Int_t col)
{
// Provide via the TString array "arr" the content(s) of the cell (row,col) in a table.
// The row and column counting starts at 1.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an array with 1 empty string is provided and 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().
// 3) This memberfunction produces a newly created TString array, so to prevent a memory leak,
//    the user should invoke the "delete[]" operator when the array is not needed anymore.
// 4) To obtain an array with automatic memory management and size indication, the user is
//    referred to the corresponding GetTableCell(TObjArray,...) memberfunction.

 if (!arr) arr=new TString[1];
 arr[0]="";

 if (row<=0 || row>fNrows || col<=0 || col>fNcolumns) return 0;

 if (fType!=kTableHDU) return 0;

 // Obtain the number of (array) elements stored in this (row,col) cell.
 Int_t dim=0;
 int colnum=col;
 long long rownum=row;
 long repeat=0;
 long offset=0;
 int status=0;
 fits_read_descript(fInput,colnum,rownum,&repeat,&offset,&status);

 if (!status) // The column was created to hold a variable number of elements
 {
  dim=repeat;
 }
 else // The column was created to hold a fixed number of elements
 {
  dim=fColumnLayers[col-1];
 }
 
 if (dim<=0) return 0;

 // Retrieve the character width for this column
 int dispwidth=0;
 status=0;
 fits_get_col_display_width(fInput,col,&dispwidth,&status);

 if (status)
 {
  cout << endl;
  cout << " *" << ClassName() << "::GetTableCell* Could not retrieve string width of table cell [" 
               << row << "," << col << "]." << endl;
  fits_close_file(fInput,&status);
  Reset();
  return 0;
 }

 if (dispwidth<=0) dispwidth=1;

 // Read the cell contents into an array
 char* nulval=(char*)"";
 int anynul=0;
 char** array=new char*[dim];
 for (Int_t i=0; i<dim; i++)
 {
  array[i]=new char[dispwidth+1];
 }
 fits_read_col(fInput,TSTRING,col,row,1,dim,nulval,array,&anynul,&status);

 if (status)
 {
  cout << endl;
  cout << " *" << ClassName() << "::GetTableCell* Could not retrieve string of table cell [" 
               << row << "," << col << "]." << endl;
  fits_close_file(fInput,&status);
  Reset();
  delete [] array; 
  return 0;
 }

 // Copy the data into the output array
 delete[] arr;
 arr=new TString[dim];
 for (Int_t j=0; j<dim; j++)
 {
  arr[j]=array[j];
 }

 delete[] array;
 return dim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(TString* &arr,Int_t row,TString colname,Int_t mode)
{
// Provide via the TString array "arr" the content(s) of the cell matching the row number "row"
// and column name (pattern) "colname" in a table.
// The row and column counting starts at 1.
//
// mode = 0 --> The column name should match exactly the provided "colname"
//        1 --> The column name only has to contain the provided "colname" pattern.
//
// The default value is mode=0.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an array with 1 empty string is provided and 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().
// 3) This memberfunction produces a newly created TString array, so to prevent a memory leak,
//    the user should invoke the "delete[]" operator when the array is not needed anymore.
// 4) To obtain an array with automatic memory management and size indication, the user is
//    referred to the corresponding GetTableCell(TObjArray,...) memberfunction.

 Int_t col=GetColumnNumber(colname,mode);

 Int_t ndim=GetTableCell(arr,row,col);

 return ndim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(TObjArray &arr,Int_t row,Int_t col)
{
// Provide via an array of TObjString objects the content(s) of the cell (row,col) in a table.
// The row and column counting starts at 1.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an empty array is provided and 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().

 TString* temp=0;
 Int_t ndim=GetTableCell(temp,row,col);

 arr.Clear();
 arr.SetOwner();

 TObjString* sobj;
 for (Int_t i=0; i<ndim; i++)
 {
  sobj=new TObjString();
  sobj->SetString(temp[i]);
  arr.Add(sobj);
 }

 delete[] temp;

 return ndim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableCell(TObjArray &arr,Int_t row,TString colname,Int_t mode)
{
// Provide via an array of TObjString objects the content(s) of the cell matching the row number "row"
// and column name (pattern) "colname" in a table.
// The row and column counting starts at 1.
//
// mode = 0 --> The column name should match exactly the provided "colname"
//        1 --> The column name only has to contain the provided "colname" pattern.
//
// The default value is mode=0.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an empty array provided and 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().

 Int_t col=GetColumnNumber(colname,mode);

 Int_t ndim=GetTableCell(arr,row,col);

 return ndim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableColumn(TArrayD &arr,Int_t col,Int_t rstart,Int_t rend,Int_t layer)
{
// Provide via array "arr" the real number values for row=[rstart,rend], column=col
// and layer number "layer".
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
//
// Input arguments :
// -----------------
// arr    : The array to contain the retrieved real numbers
// col    : The requested column number (first column is col=1)
// rstart : Row number to start with (first row is rstart=1)
// rend   : Row number to end with (rend=0 will process until the last row)
// layer  : The layer number
//
// The default values are rstart=1, rend=0 and layer=1.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an empty array "arr" is provided with 0 as return argument.

 if (!rend) rend=fNrows;

 arr.Set(0);

 if (rstart<=0 || rstart>fNrows || rend<=0 || rend>fNrows || col<=0 || col>fNcolumns || layer<1) return 0;

 if (fType!=kTableHDU || fColumnTypes[col-1]==kString || fColumnTypes[col-1]==kStringArray) return 0;

 Int_t ndim=(rend-rstart)+1;
 arr.Set(ndim);

 Double_t val=0;
 Int_t dim=0;
 Int_t n=0;
 for (Int_t irow=rstart; irow<=rend; irow++)
 {
  dim=GetTableCell(val,irow,col);

  if (!dim || layer>dim)
  {
   arr.Set(0);
   return 0;
  }

  arr.AddAt(val,n);
  n++;
 }

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableColumn(TArrayD &arr,TString colname,Int_t rstart,Int_t rend,Int_t layer,Int_t mode)
{
// Provide via array "arr" the real number values for row=[rstart,rend], the column
// for which the name matches the name (pattern) "colname" and the layer number "layer".
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
//
// Input arguments :
// -----------------
// arr     : The array to contain the retrieved real numbers
// colname : The requested column name (pattern)
// rstart  : Row number to start with (first row is rstart=1)
// rend    : Row number to end with (rend=0 will process until the last row)
// layer   : The layer number
// mode    : 0 --> The column name should match exactly the provided "colname"
//           1 --> The column name only has to contain the provided "colname" pattern.
//
// The default values are rstart=1, rend=0, layer=1 and mode=0.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an empty array "arr" is provided with 0 as return argument.

 Int_t col=GetColumnNumber(colname,mode);

 Int_t n=GetTableColumn(arr,col,rstart,rend,layer);

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableColumn(TString* &arr,Int_t col,Int_t rstart,Int_t rend,Int_t layer)
{
// Provide via the TString array "arr" the contents for row=[rstart,rend], column=col
// and layer number "layer".
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
//
// Input arguments :
// -----------------
// arr    : The array to contain the retrieved strings
// col    : The requested column number (first column is col=1)
// rstart : Row number to start with (first row is rstart=1)
// rend   : Row number to end with (rend=0 will process until the last row)
// layer  : The layer number
//
// The default values are rstart=1, rend=0 and layer=1.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an array with 1 empty string is provided and 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().
// 3) This memberfunction produces a newly created TString array, so to prevent a memory leak,
//    the user should invoke the "delete[]" operator when the array is not needed anymore.
// 4) To obtain an array with automatic memory management and size indication, the user is
//    referred to the corresponding GetTableCell(TObjArray,...) memberfunction.

 if (!rend) rend=fNrows;

 if (!arr) arr=new TString[1];
 arr[0]="";

 if (rstart<=0 || rstart>fNrows || rend<=0 || rend>fNrows || col<=0 || col>fNcolumns || layer<1) return 0;

 if (fType!=kTableHDU) return 0;

 Int_t ndim=(rend-rstart)+1;

 delete[] arr;
 arr=new TString[ndim];

 TString str;
 Int_t dim=0;
 Int_t n=0;
 for (Int_t irow=rstart; irow<=rend; irow++)
 {
  dim=GetTableCell(str,irow,col,layer);

  if (!dim || layer>dim)
  {
   delete[] arr;
   arr=new TString[1];
   arr[0]="";
   return 0;
  }

  arr[n]=str;
  n++;
 }

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableColumn(TString* &arr,TString colname,Int_t rstart,Int_t rend,Int_t layer,Int_t mode)
{
// Provide via the TString array "arr" the contents for row=[rstart,rend], the column
// for which the name matches the name (pattern) "colname" and layer number "layer".
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
//
// Input arguments :
// -----------------
// arr     : The array to contain the retrieved strings
// colname : The requested column name (pattern)
// rstart  : Row number to start with (first row is rstart=1)
// rend    : Row number to end with (rend=0 will process until the last row)
// layer   : The layer number
// mode    : 0 --> The column name should match exactly the provided "colname"
//           1 --> The column name only has to contain the provided "colname" pattern.
//
// The default values are rstart=1, rend=0, layer=1 and mode=0.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an array with 1 empty string is provided and 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().
// 3) This memberfunction produces a newly created TString array, so to prevent a memory leak,
//    the user should invoke the "delete[]" operator when the array is not needed anymore.
// 4) To obtain an array with automatic memory management and size indication, the user is
//    referred to the corresponding GetTableCell(TObjArray,...) memberfunction.

 Int_t col=GetColumnNumber(colname,mode);

 Int_t n=GetTableColumn(arr,col,rstart,rend,layer);

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableColumn(TObjArray &arr,Int_t col,Int_t rstart,Int_t rend,Int_t layer)
{
// Provide via an array of TObjString objects the contents for row=[rstart,rend], column=col
// and layer number "layer".
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
//
// Input arguments :
// -----------------
// arr    : The array to contain the retrieved TObjString objects
// col    : The requested column number (first column is col=1)
// rstart : Row number to start with (first row is rstart=1)
// rend   : Row number to end with (rend=0 will process until the last row)
// layer  : The layer number (first layer is layer=1)
//
// The default values are rstart=1, rend=0 and layer=1.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an empty array is provided and 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().

 if (!rend) rend=fNrows;

 arr.Clear();
 arr.SetOwner();

 if (rstart<=0 || rstart>fNrows || rend<=0 || rend>fNrows || col<=0 || col>fNcolumns || layer<1) return 0;

 if (fType!=kTableHDU) return 0;

 TString str;
 Int_t dim=0;
 TObjString* sx;
 Int_t n=0;
 for (Int_t irow=rstart; irow<=rend; irow++)
 {
  dim=GetTableCell(str,irow,col,layer);

  if (!dim || layer>dim)
  {
   arr.Clear();
   return 0;
  }

  sx=new TObjString();
  sx->SetString(str.Data());
  arr.Add(sx);
  n++;
 }

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetTableColumn(TObjArray &arr,TString colname,Int_t rstart,Int_t rend,Int_t layer,Int_t mode)
{
// Provide via an array of TObjString objects the contents for row=[rstart,rend], the column
// for which the name matches the name (pattern) "colname" and layer number "layer".
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
//
// Input arguments :
// -----------------
// arr     : The array to contain the retrieved TObjString objects
// colname : The requested column name (pattern)
// rstart  : Row number to start with (first row is rstart=1)
// rend    : Row number to end with (rend=0 will process until the last row)
// layer   : The layer number (first layer is layer=1)
// mode    : 0 --> The column name should match exactly the provided "colname"
//           1 --> The column name only has to contain the provided "colname" pattern
//
// The default values are rstart=1, rend=0, layer=1 and mode=0.
//
// The integer return argument represents the number of array elements.
//
// In case of inconsistent data an empty array is provided and 0 as return argument.
//
// Notes :
// -------
// 1) Any table data item can be obtained as a TString.
// 2) Conversion from a TString to a numerical value can easily be obtained
//    via the TString memberfunctions Atof(), Atoi() and Atoll().

 Int_t col=GetColumnNumber(colname,mode);

 Int_t n=GetTableColumn(arr,col,rstart,rend,layer);

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetImageDimension(Int_t i) const
{
// Provide the dimension of an N-dimensional Image data unit.
//
// Input argument :
// ----------------
// i : The axis (1,...N) for which the dimension is provided.
//   : 0 --> Provide the total Image dimension "N".
//
// In case of inconsistent data the value 0 is returned.
//
// The default value is i=0.

 if (!fSizes) return 0;

 Int_t dim=fSizes->GetSize();

 if (!i) return dim;

 if (i<1 || i>dim) return 0;

 dim=fSizes->At(i-1);

 return dim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetImageLayer(TASImage &im,Int_t layer,Double_t* thres,Double_t max)
{
// Provide the specified layer as a displayable image.
// The return value indicates the number of pixels in the image.
// 
// Input arguments :
// -----------------
// im    : The TASImage object to contain the image.
// layer : The layer number (1=first layer) of the data cube.
// thres : (Pointer to) Threshold for pixel value. Pixel values below the threshold will be set to 0.
//         No threshold will be applied when thres=0.
// max   : Rescale pixel values to [0,max], which may be used to match a specific colour or grey scale scheme.
//         For instance max=255 would match a 256 colour scheme.
//         No rescaling will be performed when max<=0.
//
// Notes :
// -------
// 1) The (optional) rescaling is performed after the threshold correction.
// 2) By default a standard colour palette is used, but by setting a specific colour palette
//    (e.g. via the canvas Palette Editor) one may tailor the color scheme.
//
// In case of inconsistent data, an empty image and a return value 0 is provided.
//
// The default values are layer=1, thres=0 and max=-1.

 // Set the image empty
 im.SetImage(0,0);

 TArrayD arr;
 if (!LoadLayer(arr,layer)) return 0;

 Int_t ndim1=fSizes->At(0);
 Int_t npix=arr.GetSize();

 if (npix)
 {
  if (thres) ApplyPixelThreshold(arr,*thres);
  if (max>0) RescalePixels(arr,max);
  im.SetImage(arr,ndim1);
 }

 return npix;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetImageLayer(TMatrixD &m,Int_t layer,Double_t* thres,Double_t max)
{
// Provide the specified layer as a 2-dimensional matrix.
// The return value indicates the number of matrix elements.
// 
// Input arguments :
// -----------------
// m     : The TMatrixD object to contain the layer data.
// layer : The layer number (1=first layer) of the data cube.
// thres : (Pointer to) Threshold for pixel value. Pixel values below the threshold will be set to 0.
//         No threshold will be applied when thres=0.
// max   : Rescale values of the matrix elements to [0,max].
//         No rescaling will be performed when max<=0.
//
// Notes :
// -------
// 1) The size (nrow,ncol) of the matrix is automatically set according to the layer dimensions.
//    The correspondence with the FITS parameters is ncol=NAXIS1 (=width) and nrow=NAXIS2 (=height).
// 2) The FITS image data start at the lower left corner and end at the right upper corner.
//    This convention is maintained for the produced matrix to provide a consistent "view" of the
//    contents of a matrix, image and 2D histogram.
//    Note that the TMatrixD row and column numbering start at 0.
//    This implies that for the matrix contents the row numbering should be interpreted as inverted,
//    such that (row,col)=(0,0) indicates the lower left corner instead of the (usual) upper left corner.
//    With this convention, the upper left corner is (nrow-1,0), the lower left corner is (0,ncol-1)
//    and the upper right corner is (nrow-1,ncol-1).     
// 3) The (optional) rescaling is performed after the threshold correction.
//
// In case of inconsistent data, an empty matrix and a return value 0 is provided.
//
// The default values are layer=1, thres=0 and max=-1.

 // Clear the matrix elements
 m.Clear();

 TArrayD arr;
 if (!LoadLayer(arr,layer)) return 0;

 Int_t ndim1=fSizes->At(0);
 Int_t ndim2=fSizes->At(1);
 Int_t npix=arr.GetSize();

 if (npix)
 {
  if (thres) ApplyPixelThreshold(arr,*thres);
  if (max>0) RescalePixels(arr,max);
  m.ResizeTo(ndim2,ndim1);
  Int_t i=0;
  for (Int_t jrow=0; jrow<ndim2; jrow++)
  {
   for (Int_t jcol=0; jcol<ndim1; jcol++)
   {
    m(jrow,jcol)=arr.At(i);
    i++;
   }
  }
 }

 return npix;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::GetImageLayer(TH2D &his,Int_t layer,Double_t* thres,Double_t max)
{
// Provide the specified layer as a 2-dimensional histogram.
// The return value indicates the number of histogram entries.
// 
// Input arguments :
// -----------------
// his   : The 2-D histogram to contain the layer data.
// layer : The layer number (1=first layer) of the data cube.
// thres : (Pointer to) Threshold for pixel value. Pixel values below the threshold will be set to 0.
//         No threshold will be applied when thres=0.
// max   : Rescale values of the matrix elements to [0,max].
//         No rescaling will be performed when max<=0.
//
// Note :
// ------
// The binning and range of the histogram axes is automatically set according to the layer dimensions.
// The correspondence with the FITS parameters is nbinsx=NAXIS1 (=width) and nbinsy=NAXIS2 (=height). 
//
// In case of inconsistent data, an empty histogram and a return value 0 is provided.
//
// The default values are layer=1, thres=0 and max=-1.

 // Clear the histogram
 his.Reset();
 TString title="Histogram of layer ";
 title+=layer;
 his.SetTitle(title.Data());

 TMatrixD m;
 Int_t npix=GetImageLayer(m,layer,thres,max);

 if (!npix) return 0;

 Int_t nrows=m.GetNrows();
 Int_t ncols=m.GetNcols();

 // Set the binning and range of the histogram axes
 his.SetBins(ncols+1,0,ncols,nrows+1,0,nrows);

 // Fill the histogram cells with the corresponding matrix contents
 Double_t val=0;
 for (Int_t icol=0; icol<ncols; icol++)
 {
  for (Int_t irow=0; irow<nrows; irow++)
  {
   val=m(irow,icol);
   his.SetBinContent(icol+1,irow+1,val);
  }
 }

 return npix;
}
///////////////////////////////////////////////////////////////////////////
UInt_t NcFITSIO::GetImageArray(TArrayD &arr,TArrayI ifirst,TArrayI ilast,TArrayI incr)
{
// Copy (a subset of) the pixel contents of an N-dimensional Image data unit into a linear data array.
// The return argument is the number of stored pixels.
//
// Input arguments :
// -----------------
// arr    : The array to hold the pixel values
// ifirst : Array with the starting pixel numbers (1=first) for each dimension
// ilast  : Array with the (inclusive) ending pixel numbers for each dimension
// incr   : Array with the pixel sampling increment for each dimension
//
// For a (nrow,ncol) matrix, the FITS parameters correspond to ncol=NAXIS1 (=width) and nrow=NAXIS2 (=height). 
// The FITS image data start at the lower left corner and end at the right upper corner and follow a
// pixel array storage with the following sequence convention for pixel coordinates (ix,iy) :
// (1,1),(2,1),(3,1),...(NAXIS1,1),(1,2),(2,2),(3,2),...(NAXIS1,2),...(1,NAXIS2),(2,NAXIS2),...(NAXIS1,NAXIS2).
// This implies that for a matrix interpretation the row numbering should be considered as inverted,
// such that (row,col)=(1,1) indicates the lower left corner instead of the (usual) upper left corner.
// With this convention, the upper left corner is (nrow,1), the lower left corner is (1,ncol)
// and the upper right corner is (nrow,ncol).     
//
// Example : The full 2nd row of the 3rd layer of a 3-dimensional data cube is obtained via :
//           ifirst[0]=1, ilast[0]=NAXIS1, ifirst[1]=2, ilast[1]=2, ifirst[2]=3, ilast[2]=3
//           and all incr[i] values set to 1.
//
// Note : The incr[i] values must always be larger than 0.
//
// In case of inconsistent data, an empty array and a return value 0 is provided.

 arr.Set(0);

 if (fType!=kImageHDU || !fSizes) return 0;

 Int_t ndims=fSizes->GetSize();

 if (ndims<1 || ifirst.GetSize()<ndims || ilast.GetSize()<ndims || incr.GetSize()<ndims) return 0;

 // Read the pixels of the specified (sub)set
 long* fpixel=new long[ndims];
 long* lpixel=new long[ndims];
 long* inc=new long[ndims];
 long long npixels=1;
 int status=0;

 Int_t istart=0;
 Int_t iend=0;
 Int_t istep=0;
 for (Int_t i=0; i<ndims; i++)
 {
  istart=ifirst.At(i);
  iend=ilast.At(i);
  istep=incr.At(i);

  if (istart<1 || iend<1 || istep<1 || iend<istart)
  {
   cout << endl;
   cout << " *" << ClassName() << "::GetImageArray* Inconsistent ifirst, ilast or incr input array(s)." << endl;
   fits_close_file(fInput,&status);
   Reset();
   delete[] fpixel;
   delete[] lpixel;
   delete[] inc;
   return 0;
  }

  fpixel[i]=istart;
  lpixel[i]=iend;
  inc[i]=istep;
  if (iend>istart) npixels*=1+(iend-istart)/istep;
 }

 if (!npixels) npixels=1;
 double* pixels=new double[npixels];

 double nulval=0;
 int anynul;
 status=0;
 fits_read_subset(fInput,TDOUBLE,fpixel,lpixel,inc,(void*)&nulval,(void*)pixels,&anynul,&status);

 if (status)
 {
  cout << endl;
  cout << " *" << ClassName() << "::GetImageArray* Could not read pixel data." << endl;
  fits_close_file(fInput,&status);
  Reset();
  delete[] fpixel;
  delete[] lpixel;
  delete[] inc;
  delete[] pixels;
  return 0;
 }

 // Store the pixel data in the output array
 arr.Set(npixels);
 Double_t val=0;
 for (long long i=0; i<npixels; i++)
 {
  val=pixels[i];
  arr.SetAt(val,i);
 }

 UInt_t npix=npixels;
 delete[] pixels;
 return npix;
}
///////////////////////////////////////////////////////////////////////////
UInt_t NcFITSIO::GetImageArray(TArrayD &arr,TArrayI ifirst,UInt_t npix)
{
// Copy (a subset of) the pixel contents of an N-dimensional Image data unit into a linear data array.
// The return argument is the number of stored pixels.
//
// Input arguments :
// -----------------
// arr    : The array to hold the pixel values
// ifirst : Array with the starting pixel numbers (1=first) for each dimension
// npix   : The number of pixels to be read
//
// For a (nrow,ncol) matrix, the FITS parameters correspond to ncol=NAXIS1 (=width) and nrow=NAXIS2 (=height). 
// The FITS image data start at the lower left corner and end at the right upper corner and follow a
// pixel array storage with the following sequence convention for pixel coordinates (ix,iy) :
// (1,1),(2,1),(3,1),...(NAXIS1,1),(1,2),(2,2),(3,2),...(NAXIS1,2),...(1,NAXIS2),(2,NAXIS2),...(NAXIS1,NAXIS2).
// This implies that for a matrix interpretation the row numbering should be considered as inverted,
// such that (row,col)=(1,1) indicates the lower left corner instead of the (usual) upper left corner.
// With this convention, the upper left corner is (nrow,1), the lower left corner is (1,ncol)
// and the upper right corner is (nrow,ncol).     
//
// Example : The full 2nd row of the 3rd layer of a 3-dimensional data cube is obtained via :
//           ifirst[0]=1, ifirst[1]=2, ifirst[2]=3 and npix=NAXIS1.
//
// In case of inconsistent data, an empty array and a return value 0 is provided.

 arr.Set(0);

 if (fType!=kImageHDU || !fSizes || npix<1) return 0;

 Int_t ndims=GetImageDimension();

 if (ndims<1 || ifirst.GetSize()<ndims) return 0;

 ULong_t nmax=1;
 for (Int_t i=1; i<=ndims; i++)
 {
  nmax*=GetImageDimension(i);
 } 

 if (npix>nmax) return 0;

 // Read the pixels of the specified (sub)set
 long* fpixel=new long[ndims];
 int status=0;

 Int_t istart=0;
 for (Int_t i=0; i<ndims; i++)
 {
  istart=ifirst.At(i);

  if (istart<1)
  {
   cout << endl;
   cout << " *" << ClassName() << "::GetImageArray* Inconsistent ifirst input array." << endl;
   fits_close_file(fInput,&status);
   Reset();
   delete[] fpixel;
   return 0;
  }

  fpixel[i]=istart;
 }

 long long npixels=npix;
 double* pixels=new double[npixels];
 double nulval=0;
 int anynul;
 status=0;
 fits_read_pix(fInput,TDOUBLE,fpixel,npixels,(void*)&nulval,(void*)pixels,&anynul,&status);

 if (status)
 {
  cout << endl;
  cout << " *" << ClassName() << "::GetImageArray* Could not read pixel data." << endl;
  fits_close_file(fInput,&status);
  Reset();
  delete[] fpixel;
  delete[] pixels;
  return 0;
 }

 // Store the pixel data in the output array
 arr.Set(npixels);
 Double_t val=0;
 for (long long i=0; i<npixels; i++)
 {
  val=pixels[i];
  arr.SetAt(val,i);
 }

 UInt_t nread=npixels;
 delete[] pixels;
 return nread;
}
///////////////////////////////////////////////////////////////////////////
void NcFITSIO::ListTable(Int_t width,Int_t rstart,Int_t rend,Int_t cstart,Int_t cend)
{
// List table information for row=[rstart,rend], column=[cstart,cend] and layer=1.
//
// A table element is identified via the usual (row,column) indication.
// If the table element contains an array of data, the array components are
// considered as layers, where layer=n represents the n-th component of the array.
// Since this listing may involve various columns with a different number of layers,
// only the data of the first layer (which is always present) can be displayed.
//
// Input arguments :
// -----------------
// width  : column width (in character count) for the listing output
//          <0 --> List only the description (width=|width|) of the table columns [cstart,cend]
// rstart  : Row number to start with (first row is rstart=1)
// rend    : Row number to end with (rend=0 will list until the last row)
// cstart  : Column number to start with (first column is cstart=1)
// cend    : Column number to end with (rend=0 will list until the last row)
//
// Note : In general a column width of 10 is sufficient.
//
// The default values are width=-10, rstart=1, rend=0, cstart=1 and cend=0.
   
 if (fType != kTableHDU)
 {
  cout << " *" << ClassName() <<"::ListTable* This is not a table HDU." << endl;
  return;
 }
   
 if (!width)
 {
  cout << " *" << ClassName() <<"::ListTable* Invalid argument : width=" << width << endl;
  return;
 }

 if (!rend) rend=fNrows;

 if (!cend) cend=fNcolumns;

 if (rstart<=0 || rstart>fNrows || rend<=0 || rend>fNrows ||
     cstart<=0 || cstart>fNcolumns || cend<=0 || cend>fNcolumns)
 {
  cout << " *" << ClassName() <<"::ListTable* Invalid input rstart=" << rstart << " rend=" << rend
       << " cstart=" << cstart << " cend=" << cend << endl;
  return;
 }

 TString type;
 TString name;
 if (width<0) // List description of all table columns
 {  
  width=-width;

  cout << endl;
  cout << " *" << ClassName() << "::ListTable* Table column description for col=["
       << cstart << "," << cend << "] (name width is " << width << " characters)."<< endl;
  cout << endl;

  cout.setf(ios::left); // Left adjusted printout
  for (Int_t i=cstart; i<=cend; i++)
  {
   if (fColumnTypes[i-1]==kString) type="STRING";
   if (fColumnTypes[i-1]==kStringArray) type="STRING ARRAY";
   if (fColumnTypes[i-1]==kRealNumber) type="REAL NUMBER";
   if (fColumnTypes[i-1]==kRealArray) type="REAL ARRAY";
   if (fColumnTypes[i-1]==kComplexNumber) type="COMPLEX NUMBER";
   if (fColumnTypes[i-1]==kComplexArray) type="COMPLEX ARRAY";
   if (fColumnTypes[i-1]==kLogical) type="LOGICAL";
   if (fColumnTypes[i-1]==kLogicalArray) type="LOGICAL ARRAY";

   name=fColumnNames[i-1];
   name=name.Strip(name.kBoth,' ');
   name.Resize(width);

   cout << " " << name << " : " << type;
   if (type.Contains("ARRAY")) cout << "[" << fColumnLayers[i-1] << "]";
   cout << endl;
  }
  return;
 }

 if (width>0) // List the (selected) table contents
 {

  cout << endl;
  cout << " *" << ClassName() << "::ListTable* Table contents for row=[" << rstart << "," << rend << "] and"
       << " col=[" << cstart << "," << cend << "] (column width is " << width << " characters)."<< endl;
  cout << endl;

  // List the header with the (selected) column names
  if (width<7) width=7;               // Minimal width needed for scientific notation
  Int_t nold=cout.precision(width-7); // Adjust precision and save the original value
  cout.setf(ios::left);               // Print values left adjusted
  TString str;
  Int_t nchars=0;
  for (Int_t col=cstart; col<=cend; col++)
  {
   str=fColumnNames[col-1];
   str=str.Strip(str.kBoth,' ');
   str.Resize(width);
   if (col==cstart) cout << " ";
   cout << str << "| ";
   nchars+=(width+2);
  }
  cout << endl;
  cout << " ";
  while(nchars--)
  {
   cout << "-";
  }
  cout << endl;
   
  // List the (selected) row content
  Double_t val=0;
  Int_t ndim=0;
  for (Int_t row=rstart; row<=rend; row++)
  {
   for (Int_t col=cstart; col<=cend; col++)
   {
    if (col==cstart) cout << " ";
    if (fColumnTypes[col-1]==kString || fColumnTypes[col-1]==kStringArray || fColumnTypes[col-1]==kLogical || fColumnTypes[col-1]==kLogicalArray)
    {
     ndim=GetTableCell(str,row,col);
     if (!ndim) str="---";
     str=str.Strip(str.kBoth,' ');
     str.Resize(width);
     cout << str;
    }
    else // Numerical value
    {
     str="---";
     str.Resize(width);
     ndim=GetTableCell(val,row,col);
     if (ndim)
     {
      cout << setw(width) << val;
     }
     else
     {
      cout << str;
     }
    }
    cout << "| ";
   } // end of column loop
   cout << endl;
  } // end of row loop

  // Reset precision to original value
  cout.precision(nold);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcFITSIO::ListHDUHeader() const
{
// List the header information (also called Metadata) of the current HDU record.

 cout << endl;
 cout << " *" << ClassName() << "::ListHDUHeader* The current HDU header record " << fExtensionName  << endl;
 cout << endl;

 cout.setf(ios::left);
 for (Int_t i=0; i<fNkeys; i++)
 {
  cout << " " << setw(8) << fKeyNames[i] << " = " << fKeyValues[i];
  if (fComments[i].Length()>0) cout << " / " << fComments[i];
  cout << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcFITSIO::ListFileHeader(Int_t mode) const
{
// List the header information (also called Metadata) of the FITS file.
//
// Input argument :
// ----------------
// mode : 0 --> Only the extension number, type and name of the available HDUs is provided
//        1 --> For each HDU also the header information is provided
//
// The default value is mode=1.

 // Open the FITS file without any filters and start at the primary HDU
 fitsfile* fp=0;
 int status=0;
 fits_open_file(&fp,fFilename.Data(),READONLY,&status);

 if (status)
 {
  cout << " *" << ClassName() << "::ListFileHeader* Could not open file : " << fFilename << endl;
  fits_close_file(fp,&status);
  return;
 }

 cout << endl;
 if (!mode)
 {
  cout << " *" << ClassName() << "::ListFileHeader* Short summary of the FITS file header information" << endl;
 }
 else
 {
  cout << " *" << ClassName() << "::ListFileHeader* Full FITS file header information" << endl;
 }
 cout << endl;

 // Obtain the number of HDUs present in the FITS file
 int nhdus=0;
 fits_get_num_hdus(fp,&nhdus,&status);

 if (status)
 {
  cout << " *" << ClassName() << "::ListFileHeader* Could not read the number of HDUs" << endl;
  fits_close_file(fp,&status);
  return;
 }

 cout << " Total number of HDUs : " << nhdus << endl;

 // Loop over all available HDUs
 int hdutype=0;
 int nkeys=0;
 int morekeys=0;
 char keyname[FLEN_KEYWORD+1];
 char keyvalue[FLEN_VALUE+1];
 char comment[FLEN_COMMENT+1];
 TString exttype;
 TString extname;
 TString* keynames=0;
 TString* keyvalues=0;
 TString* comments=0;
 for (Int_t jhdu=1; jhdu<=nhdus; jhdu++)
 {
  // Read the HDU type
  fits_get_hdu_type(fp,&hdutype,&status);

  if (status)
  {
   cout << " *" << ClassName() << "::ListFileHeader* Could not read the type of HDU ["<< (jhdu-1) << "]" << endl;
   fits_close_file(fp,&status);
   return;
  }

  exttype="unknown";
  if (hdutype==IMAGE_HDU) exttype="IMAGE";
  if (hdutype==ASCII_TBL) exttype="ASCII-TABLE";
  if (hdutype==BINARY_TBL) exttype="BINARY-TABLE";

  //Read the HDU header space information
  fits_get_hdrspace(fp,&nkeys,&morekeys,&status);

  if (status)
  {
   cout << " *" << ClassName() << "::ListFileHeader* Could not read the header space of HDU ["<< (jhdu-1) << "]" << endl;
   fits_close_file(fp,&status);
   return;
  }

  keynames=new TString[nkeys];
  keyvalues=new TString[nkeys];
  comments=new TString[nkeys];

  // Read the description of the HDU keys
  extname="";
  for (Int_t i=1; i<=nkeys; i++)
  {
   fits_read_keyn(fp,i,keyname,keyvalue,comment,&status);

   if (status)
   {
    cout << " *" << ClassName() << "::ListFileHeader* Could not read key number " << i << " of HDU ["<< (jhdu-1) << "]" << endl;
    fits_close_file(fp,&status);
    if (keynames) delete[] keynames;
    if (keyvalues) delete[] keyvalues;
    if (comments) delete[] comments;
    return;
   }

   keynames[i-1]=keyname;
   keyvalues[i-1]=keyvalue;
   comments[i-1]=comment;

   // Obtain the extension name if specified in the HDU
   if (keynames[i-1]=="EXTNAME")
   {
    extname="[";
    extname+=keyvalues[i-1];
    extname+="]";
    extname.ReplaceAll("'",""); // Remove the single quotes
   }
  }

  // List the HDU extension number, type and (optional) name
  cout << " [" << (jhdu-1) << "] " << exttype << " " << extname << endl;

  // List the contents of the HDU header record
  if (mode)
  {
   cout.setf(ios::left);
   for (Int_t i=0; i<nkeys; i++)
   {
    cout << " " << setw(8) << keynames[i] << " = " << keyvalues[i];
    if (comments[i].Length()>0) cout << " / " << comments[i];
    cout << endl;
   }
   cout << endl;
  }

  if (keynames) delete[] keynames;
  if (keyvalues) delete[] keyvalues;
  if (comments) delete[] comments;

  // Move to the next HDU
  if (jhdu<nhdus)
  {
   fits_movrel_hdu(fp,1,&hdutype,&status);

   if (status)
   {
    cout << " *" << ClassName() << "::ListFileHeader* Could not move to HDU [" << jhdu << "]" << endl;
    fits_close_file(fp,&status);
    return;
   }
  }
 }

 // Close file
 fits_close_file(fp,&status);
 return;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcFITSIO::LoadLayer(TArrayD &arr,Int_t layer)
{
// Internal memberfunction to load the pixels of an image layer.
// The layer counting starts at 1.
// The return argument is the number of stored pixels.

 arr.Set(0);

 if (fType!=kImageHDU || !fSizes || layer<1) return 0;

 Int_t ndims=GetImageDimension();

 // Check whether the data dimensions are consistent with a (layered) image. 
 if (ndims<2) return 0;

 Int_t ndim1=GetImageDimension(1);
 Int_t ndim2=GetImageDimension(2);
 Int_t ndim3=GetImageDimension(3);

 // Check whether the layer number is within bounds
 if ((ndims==2 && layer>1) || (ndims>2 && layer>ndim3)) return 0;

 // Read the pixels of the specified layer
 TArrayI ifirst(ndims);
 TArrayI ilast(ndims);
 TArrayI incr(ndims);
 for (Int_t i=0; i<ndims; i++)
 {
  ifirst.SetAt(1,i);
  ilast.SetAt(1,i);
  incr.SetAt(1,i);
 }

 // Read a full layer content
 ilast.SetAt(ndim1,0);
 ilast.SetAt(ndim2,1);

 // The selected layer
 if (ndim3)
 {
  ifirst.SetAt(layer,2);
  ilast.SetAt(layer,2);
 }

 // Read the layer into the data storage
 // For a (nrow,ncol) matrix, the FITS parameters correspond to ncol=NAXIS1 (=width) and nrow=NAXIS2 (=height). 
 // The FITS image data start at the lower left corner and end at the right upper corner and follow a
 // pixel array storage with the following sequence convention for pixel coordinates (ix,iy) :
 // (1,1),(2,1),(3,1),...(NAXIS1,1),(1,2),(2,2),(3,2),...(NAXIS1,2),...(1,NAXIS2),(2,NAXIS2),...(NAXIS1,NAXIS2).
 // This implies that for a matrix interpretation the row numbering should be considered as inverted,
 // such that (row,col)=(1,1) indicates the lower left corner instead of the (usual) upper left corner.
 // With this convention, the upper left corner is (nrow,1), the lower left corner is (1,ncol)
 // and the upper right corner is (nrow,ncol).     
 GetImageArray(arr,ifirst,ilast,incr);

 Int_t npix=arr.GetSize();
 return npix;
}
///////////////////////////////////////////////////////////////////////////
void NcFITSIO::ApplyPixelThreshold(TArrayD &arr,Double_t thres)
{
// Internal memberfunction to apply a threshold to the Image pixel values.
// All pixels with a value below "thres" will get the value 0.

 UInt_t npix=arr.GetSize();

 for (UInt_t i=0; i<npix; i++)
 {
  if (arr.At(i) < thres) arr.SetAt(0,i); 
 }
}
///////////////////////////////////////////////////////////////////////////
void NcFITSIO::RescalePixels(TArrayD &arr,Double_t max)
{
// Internal memberfunction to rescale the Image pixel values to [0,max].

 UInt_t npix=arr.GetSize();

 if (!npix) return;

 Double_t minval=0;
 Double_t maxval=0;
 Double_t val=0;
 for (UInt_t i=0; i<npix; i++)
 {
  val=arr.At(i);

  if (!i)
  {
   minval=val;
   maxval=val;
   continue;
  }

  if (val<minval) minval=val; 
  if (val>maxval) maxval=val; 
 }

 Double_t range=maxval-minval;
 Double_t fact=-1;
 if (range>0) fact=max/range;

 for (UInt_t i=0; i<npix; i++)
 {
  if (fact<0)
  {
   arr.SetAt(max,i);
  }
  else
  {
   val=arr.At(i);
   val=fact*(val-minval);
   arr.SetAt(val,i);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* NcFITSIO::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers when adding objects in case the container owns the objects.

 NcFITSIO* q=new NcFITSIO(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
