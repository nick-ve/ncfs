#ifndef NcFITSIO_h
#define NcFITSIO_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <stdlib.h>

#include "fitsio.h"

#include "TSystem.h"
#include "TNamed.h"
#include "TArrayI.h"
#include "TArrayD.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TASImage.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TObjArray.h"
#include "TString.h"
#include "TObjString.h"

class NcFITSIO : public TNamed
{
 public:
  // Markers of the various HDU types
  enum eHDUTypes{kImageHDU,kTableHDU};
   
  // Markers of the various column data types
  enum eColumnTypes{kString,kStringArray,kRealNumber,kRealArray,kComplexNumber,kComplexArray};

  NcFITSIO(const char* name="NcFITSIO",const char* title="FITS data I/O interface"); // Default constructor
  virtual ~NcFITSIO();                                       // Default destructor
  NcFITSIO(const NcFITSIO &q);                               // Copy constructor
  virtual TObject* Clone(const char* name="") const;         // Make a deep copy and provide its pointer

  // Input file handling
  Bool_t OpenInputFile(TString specs);
  Bool_t SelectHDU(TString extname="[0]");
  Bool_t SelectHDU(Int_t extnumber);
  void ListHDUHeader() const;
  void ListFileHeader(Int_t mode=1) const;
  TString GetKeywordValue(TString keyname,Int_t mode=0);

  // Table access methods
  Int_t GetTableNrows() const;
  Int_t GetTableNcolumns() const;
  Int_t GetColumnNumber(TString colname,Int_t mode=0) const;
  TString GetColumnName(Int_t colnum) const;
  Int_t GetTableCell(Double_t &val,Int_t row,Int_t col,Int_t layer=1);
  Int_t GetTableCell(Double_t &val,Int_t row,TString colname,Int_t layer=1,Int_t mode=0);
  Int_t GetTableCell(TArrayD &arr,Int_t row,Int_t col);
  Int_t GetTableCell(TArrayD &arr,Int_t row,TString colname,Int_t mode=0);
  Int_t GetTableCell(TString &str,Int_t row,Int_t col,Int_t layer=1);
  Int_t GetTableCell(TString &str,Int_t row,TString colname,Int_t layer=1,Int_t mode=0);
  Int_t GetTableCell(TString* &arr,Int_t row,Int_t col);
  Int_t GetTableCell(TString* &arr,Int_t row,TString colname,Int_t mode=0);
  Int_t GetTableCell(TObjArray &arr,Int_t row,Int_t col);
  Int_t GetTableCell(TObjArray &arr,Int_t row,TString colname,Int_t mode=0);
  Int_t GetTableColumn(TArrayD &arr,Int_t col,Int_t rstart=1,Int_t rend=0,Int_t layer=1);
  Int_t GetTableColumn(TArrayD &arr,TString colname,Int_t rstart=1,Int_t rend=0,Int_t layer=1,Int_t mode=0);
  Int_t GetTableColumn(TString* &arr,Int_t col,Int_t rstart=1,Int_t rend=0,Int_t layer=1);
  Int_t GetTableColumn(TString* &arr,TString colname,Int_t rstart=1,Int_t rend=0,Int_t layer=1,Int_t mode=0);
  Int_t GetTableColumn(TObjArray &arr,Int_t col,Int_t rstart=1,Int_t rend=0,Int_t layer=1);
  Int_t GetTableColumn(TObjArray &arr,TString colname,Int_t rstart=1,Int_t rend=0,Int_t layer=1,Int_t mode=0);
  void ListTable(Int_t width=-10,Int_t rstart=1,Int_t rend=0,Int_t cstart=1,Int_t cend=0);

  // Image access methods
  Int_t GetImageDimension(Int_t i=0) const;
  Int_t GetImageLayer(TASImage &im,Int_t layer=1,Double_t* thres=0,Double_t max=-1);
  Int_t GetImageLayer(TMatrixD &m,Int_t layer=1,Double_t* thres=0,Double_t max=-1);
  Int_t GetImageLayer(TH2D &his,Int_t layer=1,Double_t* thres=0,Double_t max=-1);
  UInt_t GetImageArray(TArrayD &arr,TArrayI ifirst,TArrayI ilast,TArrayI incr);
  UInt_t GetImageArray(TArrayD &arr,TArrayI ifirst,UInt_t npix);
 
 protected:
  TString             fFilename;         // The (full path) name of the FITS file on the computer system
  TString             fFilenameFilter;   // The FITS filename with the HDU selection filter
  fitsfile*           fInput;            // Pointer to the FITS input file
  fitsfile*           fOutput;           // Pointer to the FITS output file
  eHDUTypes           fType;             // The HDU type
  TString             fExtensionName;    // The HDU extension Name
  Int_t               fExtensionNumber;  // The HDU extension number (0=PRIMARY)
  Int_t               fNkeys;            // The number of HDU keywords
  TString*            fKeyNames;         // The HDU key names
  TString*            fKeyValues;        // The HDU key values
  TString*            fComments;         // The HDU (key) comments
  Int_t               fNrows;            // The number of table rows
  Int_t               fNcolumns;         // The number of table columns
  TString*            fColumnNames;      // The names of the table columns
  eColumnTypes*       fColumnTypes;      // The types of the table columns
  Int_t*              fColumnLayers;     // The number of layers of the table column
  TArrayI*            fSizes;            // Image sizes in each dimension

  void Initialize();
  void Reset();
  TString StripFilter(TString filename) const;
  Bool_t LoadHeaderInfo();
  Int_t LoadLayer(TArrayD &arr,Int_t layer);
  void ApplyPixelThreshold(TArrayD &arr,Double_t thres);
  void RescalePixels(TArrayD &arr,Double_t max);

 ClassDef(NcFITSIO,0) // I/O interface for FITS files
};
#endif
