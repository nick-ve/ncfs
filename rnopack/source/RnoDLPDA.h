#ifndef RnoDLPDA_h
#define RnoDLPDA_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "RnoLPDA.h"

class RnoDLPDA : public RnoLPDA
{
 public:
  RnoDLPDA(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoDLPDA();                                // Default destructor
  RnoDLPDA(const RnoDLPDA& q);                        // Copy constructor
  virtual TObject* Clone(const char* name="") const;  // Make a deep copy and provide its pointer

 ClassDef(RnoDLPDA,1) // Signal (Hit) handling of an RNO-G surface Downward pointing LogPeriodic Dipole Antenna (DLPDA).
};
#endif
