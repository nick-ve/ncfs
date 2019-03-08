#ifndef IceAOM_h
#define IceAOM_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceAOM.h 5 2010-03-19 10:10:02Z nickve $

#include "IceGOM.h"

class IceAOM : public IceGOM
{
 public:
  IceAOM();                                          // Default constructor
  virtual ~IceAOM();                                 // Default destructor
  IceAOM(const IceAOM& m);                           // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer

 ClassDef(IceAOM,1) // Signal (Hit) handling of a generic Amanda Optical Module (AOM).
};
#endif
