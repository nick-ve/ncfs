#ifndef RnoStation_h
#define RnoStation_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "NcDetectorUnit.h"

#include "RnoString.h"
#include "RnoULPDA.h"
#include "RnoDLPDA.h"

class RnoStation : public NcDetectorUnit
{
 public:
  RnoStation(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoStation();                                // Default destructor
  RnoStation(const RnoStation& q);                      // Copy constructor
  virtual TObject* Clone(const char* name="") const;    // Make a deep copy and provide its pointer
  TCanvas* DisplayWaveform(Int_t ich,Int_t j=1);        // Display the waveform of a channel for the j-th sampled observable

 protected:
  TCanvas* fCanvas; //! Pointer to the temp. canvas for displays

 ClassDef(RnoStation,2) // Handling of RNO-G event data.
};
#endif
