#ifndef RnoStation_h
#define RnoStation_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "TROOT.h"

#include "NcDetectorUnit.h"
#include "NcDSP.h"

#include "RnoString.h"
#include "RnoULPDA.h"
#include "RnoDLPDA.h"

/** Handling of RNO-G event data. **/

class RnoStation : public NcDetectorUnit
{
 public:
  RnoStation(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoStation();                                // Default destructor
  RnoStation(const RnoStation& q);                      // Copy constructor
  virtual TObject* Clone(const char* name="") const;    // Make a deep copy and provide its pointer
  TGraph* DisplaySampling(Int_t ich,Int_t j=1);         // Display the sampling of a channel for the j-th sampled observable
  TCanvas* DisplaySamplings(Int_t j=1);                 // Display the samplings of all channels for the j-th sampled observable
  TGraph GetSamplingGraph(Int_t ich,Int_t j=1);         // Provide the sampling graph of a channel for the j-th sampled observable
  TH1F GetSamplingDFT(Int_t ich,TString sel="AMP Hz",Int_t j=1); // Provide the sampling DFT of a channel for the j-th sampled observable

 protected:
  TCanvas* fCanvas; //! Pointer to the temp. canvas for displays

 ClassDef(RnoStation,5) // Handling of RNO-G event data.
};
#endif
