#ifndef RnoHPOL_h
#define RnoHPOL_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "RnoDANT.h"

/** Signal (Hit) handling of an RNO-G deep Horizontal Polarisation antenna (HPOL). **/

class RnoHPOL : public RnoDANT
{
 public:
  RnoHPOL(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoHPOL();                                // Default destructor
  RnoHPOL(const RnoHPOL& q);                         // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer

 ClassDef(RnoHPOL,1) // Signal (Hit) handling of an RNO-G deep Horizontal Polarisation antenna (HPOL).
};
#endif
