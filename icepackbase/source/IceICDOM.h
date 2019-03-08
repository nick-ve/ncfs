#ifndef IceICDOM_h
#define IceICDOM_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceICDOM.h 5 2010-03-19 10:10:02Z nickve $

#include "IceIDOM.h"

class IceICDOM : public IceIDOM
{
 public:
  IceICDOM();                                         // Default constructor
  virtual ~IceICDOM();                                // Default destructor
  IceICDOM(const IceICDOM& m);                        // Copy constructor
  virtual TObject* Clone(const char* name="") const;  // Make a deep copy and provide its pointer

 ClassDef(IceICDOM,1) // Signal (Hit) handling of an In-Ice main IceCube Digital Optical Module (ICDOM).
};
#endif
