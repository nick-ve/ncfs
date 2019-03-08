#ifndef IcePandel_h
#define IcePandel_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IcePandel.h 72 2013-01-21 13:13:40Z nickve $

#include "TFitter.h"
#include "TMath.h"
#include "Math/SpecFunc.h"

#include "IceRecoBase.h"

#include "NcSample.h"

class IcePandel : public IceRecoBase
{
 public :
  IcePandel(const char* name="IcePandel",const char* title="Gauss convoluted Pandel fitting"); // Constructor
  virtual ~IcePandel();                                // Destructor
  virtual void Exec(Option_t* opt);                    // Perform the fitting procedure
  void SetPrintLevel(Int_t level);                     // Set the fitter (Minuit) printlevel
  void SelectHits(Int_t mode=1);                       // Specify which hits to be used
  void SetPenalty(Float_t val);  // Set psi penalty value in dB for extreme distance and/or time values
  void FitFCN(Int_t&,Double_t*,Double_t&,Double_t*,Int_t); // The minimisation FCN

 protected :
  Int_t fPrint;         // Flag to denote the fitter (Minuit) printlevel
  Int_t fSelhits;       // Flag to denote which hits to be used
  TObjArray* fHits;     // The various hits to be used in the fitting process 
  TFitter* fFitter;     // Pointer to the minimisation processor
  Float_t fPenalty;     // User defined psi penalty value (dB) for extreme distance and/or time values
  NcTrack* fTkfit;      // Pointer to the produced fitted track
  NcSignal* fFitstats;  // The fit details of the produced fitted track
  NcSample fPsistats;   // Statistics of the Bayesian psi value for the best fitted track

 ClassDef(IcePandel,9) // IceRecoBase derived TTask processor to perform Convoluted Pandel fitting
};
#endif
