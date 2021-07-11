#ifndef RnoEvent_h
#define RnoEvent_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "NcEvent.h"

#include "RnoDetector.h"

class RnoEvent : public NcEvent
{
 public:
  RnoEvent();                                        // Default constructor
  virtual ~RnoEvent();                               // Default destructor
  RnoEvent(const RnoEvent& evt);                     // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer
  virtual void Reset();                              // Reset all values
  TCanvas* DisplayWaveform(Int_t ista,Int_t ich,Int_t j=1); // Display the waveform of a station channel for the j-th sampled observable

 protected:

 ClassDef(RnoEvent,2) // Handling of RNO-G event data.
};
#endif
