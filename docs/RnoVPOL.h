#ifndef RnoVPOL_h
#define RnoVPOL_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "RnoDANT.h"

class RnoVPOL : public RnoDANT
{
 public:
  RnoVPOL(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoVPOL();                                // Default destructor
  RnoVPOL(const RnoVPOL& q);                         // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer

 ClassDef(RnoVPOL,1) // Signal (Hit) handling of an RNO-G deep Vertical Polarisation antenna (VPOL).
};
#endif
