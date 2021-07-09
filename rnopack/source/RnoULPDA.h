#ifndef RnoULPDA_h
#define RnoULPDA_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "RnoLPDA.h"

class RnoULPDA : public RnoLPDA
{
 public:
  RnoULPDA(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoULPDA();                                // Default destructor
  RnoULPDA(const RnoULPDA& q);                        // Copy constructor
  virtual TObject* Clone(const char* name="") const;  // Make a deep copy and provide its pointer

 ClassDef(RnoULPDA,1) // Signal (Hit) handling of an RNO-G surface Upward pointing LogPeriodic Dipole Antenna (ULPDA).
};
#endif
