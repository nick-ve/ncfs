#ifndef IceGOM_h
#define IceGOM_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceGOM.h 5 2010-03-19 10:10:02Z nickve $

#include "NcDevice.h"

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

 ClassDef(IceGOM,4) // Signal (Hit) handling of a generic IceCube Optical Module (GOM).
};
#endif
