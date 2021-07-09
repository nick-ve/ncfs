#ifndef RnoSANT_h
#define RnoSANT_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "RnoGANT.h"

class RnoSANT : public RnoGANT
{
 public:
  RnoSANT(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoSANT();                                // Default destructor
  RnoSANT(const RnoSANT& q);                         // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer

 ClassDef(RnoSANT,1) // Signal (Hit) handling of an RNO-G Surface Antenna (SANT).
};
#endif
