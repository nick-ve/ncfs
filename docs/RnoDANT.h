#ifndef RnoDANT_h
#define RnoDANT_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "RnoGANT.h"

class RnoDANT : public RnoGANT
{
 public:
  RnoDANT(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoDANT();                                // Default destructor
  RnoDANT(const RnoDANT& q);                         // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer

 ClassDef(RnoDANT,1) // Signal (Hit) handling of an RNO-G Deep Antenna (DANT).
};
#endif
