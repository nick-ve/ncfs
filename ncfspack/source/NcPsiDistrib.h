#ifndef NcPsiDistrib_h
#define NcPsiDistrib_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcPsiDistrib.h 5 2010-03-19 10:10:02Z nickve $

#include "TCanvas.h"
#include "TH1D.h"

#include "NcMath.h"
#include "NcRandom.h"
#include "NcSample.h"

class NcPsiDistrib 
{
 public :
  NcPsiDistrib();                                         // Constructor
  virtual ~NcPsiDistrib();                                // Destructor

  void SetNtrials(Long_t n);                              // Set number of trials
  void SetNevents(Int_t n);                               // Set number of events to simulate
  void SetNoutcomes(Int_t n, Double_t* p=0);              // Set number of possible outcomes
  void SetProbabilities(Double_t* p);                     // Set probabilities of the possible outcomes
  void SetSignal(Int_t* s);                               // Set signal counts for each outcome
  void SetPsiRange(Int_t nb, Float_t low, Float_t high);  // Set lower edge of psi histogram
  void SetReferencePsi(Double_t ref);                     // Set reference psi
  void SetPrintFreq(Int_t freq);                          // Set print frequency

  void Distribute(Int_t storemode=0);                     // Make psi distribution

  TH1D* GetPsiHisto();                // Get the histogram with the psi distribution
  NcSample* GetPsiSample();           // Get the sample containing information on the psi distribution
  Double_t GetFracBelow();            // Get fraction of trials in which a psi smaller than the reference psi is observed
  Double_t GetFracAbove();            // Get fraction of trials in which a psi larger than the reference psi is observed

 protected :
  Long_t fNtrials;
  Int_t fNevents;
  Int_t fNoutcomes;
  Double_t* fProbs;
  Int_t* fSignal;
  Bool_t fRangeSet;
  Double_t fRefPsi;
  Long_t fNbelow;
  Long_t fNabove;
  TH1D* fPsiHisto;
  NcSample* fSample;
  Int_t fPrintFreq;

  Float_t FindMaxPsi();

 ClassDef(NcPsiDistrib,0) // Simple class to simulate Bayesian psi distributions
};
#endif
