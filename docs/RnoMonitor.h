#ifndef RnoMonitor_h
#define RnoMonitor_h

// Copyright(c) 2022, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "NcJob.h"
#include "NcAstrolab.h"
#include "NcBlocks.h"

#include "RnoEvent.h"

class RnoMonitor : public TTask
{
 public :
  RnoMonitor(const char* name="RnoMonitor",const char* title="RNO-G data monitoring"); // Constructor
  virtual ~RnoMonitor();                              // Destructor
  virtual void Exec(Option_t* opt);                   // The monitoring procedures
  void SetDevices(TString devclass,Int_t ista=-1,Int_t ichan=-1); // Specify the devices to be monitored
  void SetDeviceSample(Int_t j);                      // Specify the sample to be used from the stored device samples
  void SetSampleVariable(Int_t i,TString f="-");      // Specify the (function of the) sample variable to be used 
  void SetSampleVariable(TString name,TString f="-"); // Specify the (function of the) sample variable to be used 
  void DefineStatistic(TString mode);                 // Select the statistic for monitoring
  void SetBaselineMode(Int_t mode,Int_t n=128,Double_t nrms=1.2,Double_t fpr=0.1); // Select the baseline (correction) mode
  void SetNbins24(Int_t n);                           // Set the number of bins for the 24 hour monitoring histograms
  void ListHistograms() const;                        // Provide a list of all the stored histograms
  void WriteHistograms(TString filename);             // Write all stored histograms to a ROOT output file

 protected :
  RnoEvent* fEvt;     // Pointer to the current event structure
  Double_t fOffset;   // The RNO-G location time offset in fractional hours w.r.t. Greenwich
  TString fDevClass;  // The classname of the devices to be analysed
  Int_t fSta;         // The station number to be analysed (<0 means all stations)
  Int_t fChan;        // The channel number to be analysed (<0 means all channels)
  Int_t fDevSample;   // Identifier of the sample to be used from the stored device samples
  Int_t fVarIndex;    // Index of the sample variable to be used
  TString fVarName;   // Name of the sample variable to be used
  TF1* fVarFunc;      // The function to be used for the sample variable
  Int_t fNbins24;     // The number of bins for the 24 hour monitoring histograms 
  TObjArray fHistos;  // Storage array for the produced histograms
  NcSample fValues;   // NcSample object to obtain statisics of function values
  TString fAvMode;    // The sampling averaging mode
  Int_t fBasemode;    // The baseline (correction) mode
  Int_t fBlocksize;   // Number of samples to form the fixed baseline blocksize
  NcBlocks fBB;       // The Bayesian Block facility
  Double_t fNrms;     // Error scale factor to tailor the fluctuation sensitivity for the Bayesian Block analysis
  Double_t fFPR;      // False Positive Rate for the Bayesian Block analysis 
  TGraph fGin;        // The input graph of the time trace for Bayesian Block analysis
  TH1F fHblock;       // The produced histogram with the Bayesian Blocks of the time trace
  TGraph fGout;       // The output graph representing the Bayesian Block subtracted time trace
  Bool_t fFirst;      // Flag to indicate first pass through the processor

 ClassDef(RnoMonitor,3) // TTask derived class to monitor RNO-G data over certain time periods.
};
#endif
