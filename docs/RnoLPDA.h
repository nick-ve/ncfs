#ifndef RnoLPDA_h
#define RnoLPDA_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "RnoSANT.h"

class RnoLPDA : public RnoSANT
{
 public:
  RnoLPDA(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoLPDA();                                // Default destructor
  RnoLPDA(const RnoLPDA& q);                         // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer

 ClassDef(RnoLPDA,1) // Signal (Hit) handling of an RNO-G surface LogPeriodic Dipole Antenna (LPDA).
};
#endif
