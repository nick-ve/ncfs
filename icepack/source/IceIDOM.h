#ifndef IceIDOM_h
#define IceIDOM_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

#include "IceDOM.h"

/** Signal (Hit) handling of a generic IceCube In-ice Digital Optical Module (IDOM). **/

class IceIDOM : public IceDOM
{
 public:
  IceIDOM();                                         // Default constructor
  virtual ~IceIDOM();                                // Default destructor
  IceIDOM(const IceIDOM& m);                         // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer

 ClassDef(IceIDOM,1) // Signal (Hit) handling of a generic IceCube In-ice Digital Optical Module (IDOM).
};
#endif
