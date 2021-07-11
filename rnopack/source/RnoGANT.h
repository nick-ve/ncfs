#ifndef RnoGANT_h
#define RnoGANT_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "NcEvent.h"

class RnoGANT : public NcDevice
{
 public:
  RnoGANT(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoGANT();                                // Default destructor
  RnoGANT(const RnoGANT& q);                         // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer
  Int_t GetStation(Int_t id=0) const;                // Provide the corresponding station number
  Int_t GetString(Int_t id=0) const;                 // Provide the corresponding string number
  Int_t GetNumber(Int_t id=0) const;                 // Provide the corresponding antenna number
  Int_t GetANTId(Int_t station,Int_t string,Int_t level) const; // Provide unique ID based on station, string and number indicators
  Double_t GetTimeResidual(NcEvent* evt,NcTrack* t,NcSignal* s,TString name="LE",Int_t mode=7,Int_t vgroup=1) const; // Provide time residual of a signal w.r.t. a track
  Double_t GetTimeResidual(NcEvent* evt,NcTrack* t,Int_t j,TString name="LE",Int_t mode=7,Int_t vgroup=1) const;     // Provide time residual of a signal w.r.t. a track

 ClassDef(RnoGANT,2) // Signal (Hit) handling of an RNO-G Generic Antenna (GANT).
};
#endif
