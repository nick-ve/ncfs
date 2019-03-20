#ifndef NcFITSIO_h
#define NcFITSIO_h
// Copyright(c) 1997-2019, NCFS, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <stdlib.h>

#include "fitsio.h"

#include "TSystem.h"
#include "TNamed.h"
#include "TImage.h"
#include "TArrayI.h"
#include "TArrayD.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TImage.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"

class TImagePalette;

class NcFITSIO : public TNamed
{
 public:
  // Markers of the various HDU types
  enum EHDUTypes{kImageHDU,kTableHDU};
   
  // Markers of the various column data types
  enum EColumnTypes{kRealNumber,kString,kRealVector};
   
  // The FITS HDU record
  struct HDURecord
  {
   TString fKeyword;
   TString fValue;
   TString fComment;
  };
   
  // Information of a table column
  struct Column
  {
   TString            fName; // The column name
   enum EColumnTypes  fType; // The column data type
   Int_t              fDim;  // Vector dimension (1=scalar) in case a cell contains a vector of real numbers
  };
   
  // Contents of a table cell
  union Cell
  {
   Char_t*   fString;
   Double_t  fRealNumber;
   Double_t* fRealVector;
  };

  NcFITSIO(const char* name="NcFITSIO",const char* title="FITS I/O interface"); // Default constructor
  virtual ~NcFITSIO();                                     // Default destructor
  NcFITSIO(const NcFITSIO& q);                             // Copy constructor
  Bool_t OpenInputFile(TString specs);                     // Open a FITS input file with the provided specs
  Bool_t SelectHDU(TString extname="[0]");                 // Select a certain HDU name
  Bool_t SelectHDU(Int_t extnumber);                       // Select a certain HDU number
  Double_t GetTabRealValueCell(Int_t row,Int_t col);       // Retrieve the value of a table cell
  Double_t GetTabRealValueCell(Int_t row,TString colname); // Retrieve the value of a table cell
  void ListTable(Int_t width=-10,Int_t rstart=-1,Int_t rend=-1,Int_t cstart=-1,Int_t cend=-1); // List table information
  void ListHDUHeader() const;                              // List the header info from the current HDU record
  void ListFileHeader(Int_t mode=1) const;                 // List the header info of the FITS file
  virtual TObject* Clone(const char* name="") const;       // Make a deep copy and provide its pointer

   //Metadata access methods
   Int_t              GetRecordNumber() const { return fNRecords; }
   struct HDURecord*  GetRecord(const char *keyword);
   TString&           GetKeywordValue(const char *keyword);

   //Image readers
   TH1*               ReadAsHistogram();
   TImage*            ReadAsImage(Int_t layer=0,TImagePalette* pal=0);
   TMatrixD*          ReadAsMatrix(Int_t layer=0,Option_t* opt="");
   TVectorD*          GetArrayRow(UInt_t row);
   TVectorD*          GetArrayColumn(UInt_t col);
   
   //Table readers
   Int_t              GetTabNColumns() const { return fNColumns; }
   Int_t              GetTabNRows()    const { return fNRows; }
   Int_t              GetColumnNumber(const char* colname);
   const TString&     GetColumnName(Int_t colnum);
   TObjArray*         GetTabStringColumn(Int_t colnum);
   TObjArray*         GetTabStringColumn(const char* colname);
   TVectorD*          GetTabRealVectorColumn(Int_t colnum);
   TVectorD*          GetTabRealVectorColumn(const char* colname);
   TVectorD*          GetTabRealVectorCell(Int_t rownum,Int_t colnum);
   TVectorD*          GetTabRealVectorCell(Int_t rownum, const char* colname);
   TObjArray*         GetTabRealVectorCells(Int_t colnum);
   TObjArray*         GetTabRealVectorCells(const char* colname);
   
   //Misc
   void               Draw(Option_t* opt="");
 
 protected:
  TString             fFilename;         // The (full path) name of the FITS file on the computer system
  TString             fFilenameFilter;   // The FITS filename with the HDU selection filter
  fitsfile*           fInput;            // Pointer to the FITS input file
  fitsfile*           fOutput;           // Pointer to the FITS output file
  struct HDURecord*   fRecords;          // HDU metadata records
  Int_t               fNRecords;         // Number of records
  enum EHDUTypes      fType;             // HDU type
  TString             fExtensionName;    // Extension Name
  Int_t               fNumber;           // HDU number (1=PRIMARY)
  TArrayI*            fSizes;            // Image sizes in each dimension (when fType == kImageHDU)
  TArrayD*            fPixels;           // Image pixels (when fType == kImageHDU)
  struct Column*      fColumnsInfo;      // Information about columns (when fType == kTableHDU)
  Int_t               fNColumns;         // Number of columns (when fType == kTableHDU)
  Int_t               fNRows;            // Number of rows (when fType == kTableHDU)
  union Cell*         fCells;            // Table cells (when fType == kTableHDU). Cells are ordered in the following way:
                                         // fCells[0..fNRows-1] -> cells of column 0
                                         // fCells[fNRows..2*fNRows-1] -> cells of column 1
                                         // fCells[2*fNRows..3*fNRows-1] -> cells of column 2
                                         // fCells[(fNColumns-1)*fNRows..fNColumns*fNRows-1] -> cells of column fNColumns-1
   
   
 TString StripFilter(TString filename) const;
 Bool_t  LoadHDU(TString& filepath_filter);

 private:
  void Initialize();
  void Reset();
 
 ClassDef(NcFITSIO,0) // I/O interface for FITS files
};
#endif
