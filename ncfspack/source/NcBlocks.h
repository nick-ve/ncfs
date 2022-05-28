#ifndef NcBlocks_h
#define NcBlocks_h

// Copyright(c) 1997-2021, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "TGraphErrors.h"
#include "TMath.h"
#include "TList.h"
#include "TLegend.h"

#include "NcSample.h"

class NcBlocks
{
 public:
  NcBlocks();                  // Default constructor
  virtual ~NcBlocks();         // Default destructor
  NcBlocks(const NcBlocks& q); // Copy constructor
  Double_t GetBlocks(TH1* hin,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for binned data c.q. a histogram
  Double_t GetBlocks(NcSample s,Int_t i,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for the i-th variable of an NcSample
  Double_t GetBlocks(Int_t n,Double_t* arr,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for the given data array
  Double_t GetBlocks(Int_t n,Float_t* arr,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for the given data array
  Double_t GetBlocks(TGraphErrors gr,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for measurements of an observable
  Double_t GetBlocks(TGraph gr,TF1 f,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for measurements of an observable
  Double_t GetBlocks(TGraph gr,TString f,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for measurements of an observable
  Double_t GetBlocks(TGraph gr,Double_t nrms,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for measurements of an observable

 protected:
  Int_t fMode; // The Data Mode
  Double_t GetPrior(Int_t n,Double_t fpr);           // Provide the prior fitness value for each block
  Double_t GetBlockFitness(Double_t n,Double_t len); // Provide the block fitness for Data Modes 1 and 2 

 ClassDef(NcBlocks,3) // Bayesian Block treatment of sequential data
};
#endif
