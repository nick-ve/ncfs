#ifndef NcCalmodule_h
#define NcCalmodule_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcCalmodule.h 5 2010-03-19 10:10:02Z nickve $

#include "NcSignal.h"
 
class NcCalmodule : public NcSignal
{
 public:
  NcCalmodule();                                           // Default constructor
  virtual ~NcCalmodule();                                  // Default destructor
  NcCalmodule(const NcCalmodule& m);                       // Copy constructor
  NcCalmodule(Int_t row,Int_t col,Double_t sig);           // Create a module and initialise data
  virtual TObject* Clone(const char* name="") const;       // Make a deep copy and provide its pointer
  using NcSignal::SetSignal;
  virtual void SetSignal(Double_t sig,Int_t j=1);          // Set or change data for certain module
  using NcSignal::AddSignal;
  virtual void AddSignal(Double_t sig,Int_t j=1);          // Add signal to a certain module
  void SetRow(Int_t i);                                    // Set the row number of the module
  void SetColumn(Int_t i);                                 // Set the column number of the module
  Int_t GetRow() const;                                    // Return the row number of the module
  Int_t GetColumn() const;                                 // Return the column number of the module
  void SetClusteredSignal(Double_t val);                   // Set the signal of the module after clustering
  Float_t GetClusteredSignal() const;                      // Return module signal after clustering
 
 protected:
  Int_t fRow;        // The current row number
  Int_t fCol;        // The current column number
  Float_t fSigc;     // The signal after clustering
 
 ClassDef(NcCalmodule,1) // Description of a module in a calorimeter system.
};
#endif
