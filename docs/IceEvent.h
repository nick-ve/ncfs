#ifndef IceEvent_h
#define IceEvent_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceEvent.h 130 2016-08-24 11:11:33Z nickve $

#include "TSpectrum.h"
#include "TError.h"

#include "NcEvent.h"
#include "NcSample.h"
#include "IceGOM.h"

class IceEvent : public NcEvent
{
 public:
  IceEvent();                                        // Default constructor
  virtual ~IceEvent();                               // Default destructor
  IceEvent(const IceEvent& evt);                     // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer
  virtual void Reset();                              // Reset all values
  Int_t GetNstrings(TString classname);              // Number of fired strings for good (D)OMs of the specified class for this event 
  Int_t GetNstrings(NcTrack& t,TString classname);   // Number of fired strings for good (D)OMs of the specified class associated to this track
  Int_t GetNstrings(NcJet& j,TString classname);     // Number of fired strings for good (D)OMs of the specified class associated to this jet
  Int_t GetNmodules(NcTrack& t,TString classname);   // Number of fired (D)OMs of the specified class associated to this track
  Int_t GetNmodules(NcJet& j,TString classname);     // Number of fired (D)OMs of the specified class associated to this jet
  Int_t GetStringMax(TString classname,Int_t* id=0,Float_t* x=0,Float_t* y=0); // Max. number of fired good (D)OMs of the specified class at a single string
  Float_t GetTriggerTime(TString trigname,TObjArray* arr=0,Int_t slc=0,TArrayF* peaks=0) const; // Provide selected trigger time

 protected:
  TArrayI* fStrings; //! Temp. array to hold the string ids of fired modules

 ClassDef(IceEvent,8) // Handling of IceCube event data.
};
#endif
