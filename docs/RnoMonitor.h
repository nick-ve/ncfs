#ifndef RnoMonitor_h
#define RnoMonitor_h

// Copyright(c) 2022, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "NcJob.h"
#include "NcAstrolab.h"

#include "RnoEvent.h"

class RnoMonitor : public TTask
{
 public :
  RnoMonitor(const char* name="RnoMonitor",const char* title="RNO-G data monitoring"); // Constructor
  virtual ~RnoMonitor();                              // Destructor
  virtual void Exec(Option_t* opt);                   // The monitoring procedures
  void SetDeviceClass(TString devclass);              // Specify the class of devices to be monitored
  void SetDeviceSample(Int_t j);                      // Specify the sample to be used from the stored device samples
  void SetSampleVariable(Int_t i,TString f="-");      // Specify the (function of the) sample variable to be used 
  void SetSampleVariable(TString name,TString f="-"); // Specify the (function of the) sample variable to be used 
  void DefineStatistic(TString mode);                 // Select the statistic for monitoring
  void SetNbins24(Int_t n);                           // Set the number of bins for the 24 hour monitoring histograms
  void ListHistograms() const;                        // Provide a list of all the stored histograms
  void WriteHistograms(TString filename);             // Write all stored histograms to a ROOT output file

 protected :
  RnoEvent* fEvt;     // Pointer to the current event structure
  Double_t fOffset;   // The RNO-G location time offset in fractional hours w.r.t. Greenwich
  TString fDevClass;  // The classname of the devices to be analysed
  Int_t fDevSample;   // Identifier of the sample to be used from the stored device samples
  Int_t fVarIndex;    // Index of the sample variable to be used
  TString fVarName;   // Name of the sample variable to be used
  TF1* fVarFunc;      // The function to be used for the sample variable
  Int_t fNbins24;     // The number of bins for the 24 hour monitoring histograms 
  TObjArray fHistos;  // Storage array for the produced histograms
  NcSample fValues;   // NcSample object to obtain statisics of function values
  TString fAvMode;    // The sampling averaging mode
  Bool_t fFirst;      // Flag to indicate first pass through the processor

 ClassDef(RnoMonitor,2) // TTask derived class to monitor RNO-G data over certain time periods.
};
#endif
