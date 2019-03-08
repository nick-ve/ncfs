#ifndef IceGOM_h
#define IceGOM_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceGOM.h 63 2012-07-13 14:17:43Z nickve $

#include "NcEvent.h"

class IceGOM : public NcDevice
{
 public:
  IceGOM();                                          // Default constructor
  virtual ~IceGOM();                                 // Default destructor
  IceGOM(const IceGOM& m);                           // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer
  Int_t GetString(Int_t id=0) const;                 // Provide the corresponding string number
  Int_t GetLevel(Int_t id=0) const;                  // Provide the corresponding level on the string
  Int_t GetOMId(Int_t string,Int_t level) const;     // Provide ID based on string and level indicators
  Double_t GetTimeResidual(NcEvent* evt,NcTrack* t,NcSignal* s,TString name="LE",Int_t mode=7,Int_t vgroup=1) const; // Provide time residual of a signal w.r.t. a track
  Double_t GetTimeResidual(NcEvent* evt,NcTrack* t,Int_t j,TString name="LE",Int_t mode=7,Int_t vgroup=1) const;     // Provide time residual of a signal w.r.t. a track

 ClassDef(IceGOM,5) // Signal (Hit) handling of a generic IceCube Optical Module (GOM).
};
#endif
