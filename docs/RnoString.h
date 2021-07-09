#ifndef RnoString_h
#define RnoString_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "NcDetectorUnit.h"

#include "RnoVPOL.h"
#include "RnoHPOL.h"

class RnoString : public NcDetectorUnit
{
 public:
  RnoString(const char* name="",const char* title=""); // Default constructor
  virtual ~RnoString();                                // Default destructor
  RnoString(const RnoString& q);                       // Copy constructor
  virtual TObject* Clone(const char* name="") const;   // Make a deep copy and provide its pointer

 protected:

 ClassDef(RnoString,1) // Data handling of an RNO-G string.
};
#endif
