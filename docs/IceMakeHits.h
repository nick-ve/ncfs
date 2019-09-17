#ifndef IceMakeHits_h
#define IceMakeHits_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceMakeHits.h 14 2010-07-20 11:42:33Z nickve $

#include "TROOT.h"
#include "TTask.h"
#include "TString.h"
#include "TSpectrum.h"
#include "TMath.h"
#include "TError.h"

#include "NcJob.h"
#include "NcSample.h"

#include "IceEvent.h"
#include "IceAOM.h"
#include "IceIDOM.h"
#include "IceTDOM.h"

class IceMakeHits : public TTask
{
 public :
  IceMakeHits(const char* name="IceMakeHits",const char* title="Hit extraction"); // Constructor
  virtual ~IceMakeHits();                                // Destructor
  virtual void Exec(Option_t* opt);                      // Hit extraction
  void SetBasefracA(Float_t val);                        // Set the Amanda fract. baseline update value
  void SetSigmaA(Float_t val);                           // Set the Amanda clipping window width
  void SetMaxPeaksA(Int_t val);                          // Set the Amanda maximum number of peaks in a waveform
  void SetMinPulseHeightA(Float_t val);                  // Set the Amanda threshold for narrow pulses
  void SetThresholdA(Float_t val);                       // Set the Amanda threshold for narrow pulses
  void SetBasefracI(Float_t val);                        // Set the IceCube fractional baseline update value
  void SetSigmaI(Float_t val);                           // Set the IceCube clipping window width
  void SetMaxPeaksI(Int_t val);                          // Set the IceCube maximum number of peaks in a waveform
  void SetPeakAcceptanceLevelI(Float_t val);             // Set the IceCube minimum height of a peak above the baseline
  void SetMinPulseHeightI(Float_t val);                  // Set the IceCube threshold for narrow pulses
  void SetThresholdI(Float_t val);                       // Set the IceCube threshold for narrow pulses
  void SetWaveformNameI(const char* name);               // Set a single specific IceCube waveform name to be analysed
  void UseWaveformNameI(TString name);                   // Specify the names of the IceCube waveforms to be analysed
  void PrintWaveformNamesI() const;                      // Print the IceCube waveform name(s) to be analysed

 protected :
  IceEvent* fEvt;                // Pointer to the current event structure
  Float_t fBasefracA;            // The fractional baseline update for Amanda TWR extraction 
  Float_t fSigmaA;               // The width of the clipping window to be used by TSpectrum::Search() in Amanda TWR extraction
  Int_t fMaxPeaksA;              // The maximum number of peaks in a waveform in Amanda TWR extraction
  Float_t fMinPulseHeightA;      // The minimum pulse height for narrow pulses in Amanda TWR extraction
  Float_t fThresholdA;           // The threshold to be used in analysis of narrow pulses in Amanda TWR extraction
  Float_t fBasefracI;            // The fractional baseline update for IceCube waveform hit extraction 
  Float_t fSigmaI;               // The width of the clipping window to be used by TSpectrum::Search() in IceCube hit extraction
  Int_t fMaxPeaksI;              // The maximum number of peaks in a waveform in IceCube hit extraction
  Float_t fPeakAcceptanceLevelI; // The minimum height of a peak above the baseline, in terms of baseline spread, in IceCube hit extraction
  Float_t fMinPulseHeightI;      // The minimum pulse height for narrow pulses in IceCube hit extraction
  Float_t fThresholdI;           // The threshold to be used in analysis of narrow pulses in IceCube hit extraction
  TObjArray* fUseNamesI;         // The IceCube waveform names to be analysed
  void Amanda();                 // Hit extraction from Amanda TWR data
  void IceCube();                // Hit extraction from IceCube waveform data

 ClassDef(IceMakeHits,8) // TTask derived class to perform hit extraction from waveforms
};
#endif
