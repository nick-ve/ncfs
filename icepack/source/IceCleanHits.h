#ifndef IceCleanHits_h
#define IceCleanHits_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceCleanHits.h 57 2012-02-07 10:05:54Z nickve $

#include "TROOT.h"
#include "TTask.h"
#include "TString.h"

#include "NcJob.h"
#include "IceEvent.h"
#include "IceAOM.h"
#include "IceTDOM.h"
#include "IceICDOM.h"
#include "IceDCDOM.h"

class IceCleanHits : public TTask
{
 public :
  IceCleanHits(const char* name="IceCleanHits",const char* title="Hit cleaning"); // Constructor
  virtual ~IceCleanHits();                                     // Destructor
  virtual void Exec(Option_t* opt);                            // Hit cleaning
  void SetAdcRange(Float_t min,Float_t max,TString s);         // Set ADC range (in PE)
  void SetTotRange(Float_t min,Float_t max,TString s);         // Set TOT range (in ns)
  void SetIsolation(Float_t rmax,Float_t dtmax,TString s);     // Set isolation radius (in m) and dt (in ns)
  void SetTwindow(Float_t dtmax,TString s);                    // Set maximal time window around the trigger time(s)
  void SetTtime(Float_t t,TString s,Int_t slc=0,Int_t mult=0); // Set trigger time(s)
  void SetTname(TString name,TString s);                       // Set trigger name

 protected :
  IceEvent* fEvt;     // Pointer to the current event structure
  Float_t fAdcminAM;  // Minimum Amanda MuDaq ADC value in PE
  Float_t fAdcmaxAM;  // Maximum Amanda MuDaq ADC value in PE
  Float_t fAdcminAT;  // Minimum Amanda TWRDaq ADC value in PE
  Float_t fAdcmaxAT;  // Maximum Amanda TWRDaq ADC value in PE
  Float_t fAdcminT;   // Minimum IceTop ADC value in PE
  Float_t fAdcmaxT;   // Maximum IceTop ADC value in PE
  Float_t fAdcminIC;  // Minimum standard IceCube InIce ADC value in PE
  Float_t fAdcmaxIC;  // Maximum standard IceCube InIce ADC value in PE
  Float_t fAdcminDC;  // Minimum DeepCore ADC value in PE
  Float_t fAdcmaxDC;  // Maximum DeepCore ADC value in PE
  Float_t fTotminAEM; // Minimum Amanda MuDaq electrical TOT value in ns
  Float_t fTotmaxAEM; // Maximum Amanda MuDaq electrical TOT value in ns
  Float_t fTotminAOM; // Minimum Amanda MuDaq optical TOT value in ns
  Float_t fTotmaxAOM; // Maximum Amanda MuDaq optical TOT value in ns
  Float_t fTotminAET; // Minimum Amanda TWRDaq electrical TOT value in ns
  Float_t fTotmaxAET; // Maximum Amanda TWRDaq electrical TOT value in ns
  Float_t fTotminAOT; // Minimum Amanda TWRDaq optical TOT value in ns
  Float_t fTotmaxAOT; // Maximum Amanda TWRDaq optical TOT value in ns
  Float_t fTotminT;   // Minimum IceTop TOT value in ns
  Float_t fTotmaxT;   // Maximum IceTop TOT value in ns
  Float_t fTotminIC;  // Minimum standard IceCube InIce TOT value in ns
  Float_t fTotmaxIC;  // Maximum standard IceCube InIce TOT value in ns
  Float_t fTotminDC;  // Minimum DeepCore TOT value in ns
  Float_t fTotmaxDC;  // Maximum DeepCore TOT value in ns
  Float_t fRmaxA;     // Maximum Amanda hit isolation radius in m
  Float_t fDtmaxA;    // Maximum Amanda hit isolation dt in ns
  Float_t fRmaxT;     // Maximum IceTop hit isolation radius in m
  Float_t fDtmaxT;    // Maximum IceTop hit isolation dt in ns
  Float_t fRmaxIC;    // Maximum standard IceCube InIce hit isolation radius in m
  Float_t fDtmaxIC;   // Maximum standard IceCube InIce hit isolation dt in ns
  Float_t fRmaxDC;    // Maximum DeepCore hit isolation radius in m
  Float_t fDtmaxDC;   // Maximum DeepCore hit isolation dt in ns
  TString fTnamAM;    // The Amanda MuDaq trigger name
  Float_t fTtimAM;    // The Amanda MuDaq trigger time in TDC counts
  TString fTnamAT;    // The Amanda TWRDaq trigger name
  Float_t fTtimAT;    // The Amanda TWRDaq trigger time in ns
  TString fTnamT;     // The IceCube trigger name for IceTop cleaning
  Int_t fSlcT;        // Usage flag for SLC hits of IceTop DOMs to determine the trigger time
  Int_t fMultT;       // Flag to allow multiple trigger times from IceTop hits 
  Float_t fTtimT;     // The (median) trigger time in ns for IceTop DOM hits
  TString fTnamIC;    // The IceCube trigger name for standard InIce DOM cleaning
  Int_t fSlcIC;       // Usage flag for SLC hits of standard InIce DOMs to determine the trigger time
  Int_t fMultIC;      // Flag to allow multiple trigger times from standard InIce hits 
  Float_t fTtimIC;    // The (median) trigger time in ns for standard InIce DOM hits
  TString fTnamDC;    // The IceCube trigger name for DeepCore DOM cleaning
  Int_t fSlcDC;       // Usage flag for SLC hits of DeepCore DOMs to determine the trigger time
  Int_t fMultDC;      // Flag to allow multiple trigger times from DeepCore hits 
  Float_t fTtimDC;    // The (median) trigger time in ns for DeepCore DOM hits
  Float_t fTwinAM;    // Maximum Amanda MuDaq hit time difference from the trigger time in TDC counts
  Float_t fTwinAT;    // Maximum Amanda TWRDaq hit time difference from the trigger time in ns
  Float_t fTwinT;     // Maximum IceTop hit time difference from the trigger time in ns
  Float_t fTwinIC;    // Maximum standard InIce hit time difference from the trigger time in ns
  Float_t fTwinDC;    // Maximum DeepCore hit time difference from the trigger time in ns
  void Amanda();      // Cleaning of Amanda modules
  void MuDaq();       // Cleaning of Amanda modules for MuDaq data
  void TWRDaq();      // Cleaning of Amanda modules for TWRDaq data
  void IceTop();      // Cleaning of IceTop DOMs
  void ICdoms();      // Cleaning of the standard IceCube InIce DOMs
  void DCdoms();      // Cleaning of the DeepCore DOMs
  void Amplitude(TObjArray& hits,Float_t adcmin,Float_t adcmax,Float_t totmin,Float_t totmax) const; // ADC and TOT cleaning
  void Isolation(TObjArray& hits,Float_t dtmax,Float_t rmax) const;                                  // Isolated hit cleaning
  void TimeWindow(TObjArray& hits,Float_t ttrig,Float_t twin,TArrayF* peaks=0) const;                // Time window cleaning

 ClassDef(IceCleanHits,7) // TTask derived class to perform hit cleaning
};
#endif
