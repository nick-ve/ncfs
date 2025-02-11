#ifndef NcBlocks_h
#define NcBlocks_h

// Copyright(c) 2021, NCFS/IIHE, All Rights Reserved.
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
  Double_t GetBlocks(NcSample s,TString name,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for the named variable of an NcSample
  Double_t GetBlocks(Int_t n,Double_t* arr,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for the given data array
  Double_t GetBlocks(Int_t n,Float_t* arr,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for the given data array
  Double_t GetBlocks(TGraphErrors gr,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for measurements of an observable
  Double_t GetBlocks(TGraph gr,TF1 f,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for measurements of an observable
  Double_t GetBlocks(TGraph gr,TString f,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for measurements of an observable
  Double_t GetBlocks(TGraph gr,Double_t nrms,Double_t fpr,TH1* hout,Int_t ntrig=0); // Get Bayesian Block partitions for measurements of an observable
  Int_t GetBlocks(TH1* hin,TH1* hout,Int_t n,Int_t mode=0); // Get block partitions of "n" consecutive bins of binned data c.q. a histogram
  Int_t GetBlocks(NcSample s,Int_t i,TH1* hout,Int_t n,Int_t mode=0); // Get block partitions of "n" consecutive samplings for the i-th variable of an NcSample
  Int_t GetBlocks(NcSample s,TString name,TH1* hout,Int_t n,Int_t mode=0); // Get block partitions of "n" consecutive samplings for the named variable of an NcSample
  Int_t GetBlocks(Int_t nr,Double_t* arr,TH1* hout,Int_t n,Int_t mode=0); // Get block partitions of "n" consecutive recrodings for the given data array
  Int_t GetBlocks(Int_t nr,Float_t* arr,TH1* hout,Int_t n,Int_t mode=0); // Get block partitions of "n" consecutive recrodings for the given data array
  Int_t GetBlocks(TGraph* gr,TH1* hout,Int_t n,Int_t mode=0); // Get block partitions of "n" consecutive samples of measurements of an observable 
  Int_t Add(TH1* h1,TH1* h2,TH1* hout,Bool_t scale,Double_t c,Double_t d=0); // Provide the histogram hout=h1+c*h2+d
  Int_t Add(TGraph* gr,TH1* h,TGraph* gout,Double_t c,Double_t d=0);         // Provide the graph gout=gr+c*h+d
  Int_t Divide(TH1* h1,TH1* h2,TH1* hout,Bool_t scale,Double_t c,Double_t d=0); // Provide the histogram hout=d+h1/(c*h2)
  Int_t Divide(TGraph* gr,TH1* h,TGraph* gout,Double_t c,Double_t d=0);         // Provide the graph gout=d+gr/(c*h)
  Int_t Rebin(TH1* hin,TH1* hout,Bool_t scale,Int_t nbins=0,Double_t xmin=0,Double_t xmax=-1); // Provide the histogram hout as an equally binned version of hin

 protected:
  Int_t fMode; // The Data Mode
  Double_t GetPrior(Int_t n,Double_t fpr);           // Provide the prior fitness value for each block
  Double_t GetBlockFitness(Double_t n,Double_t len); // Provide the block fitness for Data Modes 1 and 2 

 ClassDef(NcBlocks,10) // (Bayesian) Block treatment of sequential data
};
#endif
