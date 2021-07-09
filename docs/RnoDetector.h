#ifndef RnoDetector_h
#define RnoDetector_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "NcDetector.h"

#include "RnoStation.h"

class RnoDetector : public NcDetector
{
 public:
  RnoDetector(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoDetector();                                // Default destructor
  RnoDetector(const RnoDetector& d);                     // Copy constructor
  virtual TObject* Clone(const char* name="") const;     // Make a deep copy and provide its pointer
  RnoStation* GetStation(Int_t id,Bool_t create);        // Provide access to an RNO-G station

 protected:
 void CreateStation(Int_t id); // Create an RNO-G station

 ClassDef(RnoDetector,1) // Handling of RNO-G detector data.
};
#endif
