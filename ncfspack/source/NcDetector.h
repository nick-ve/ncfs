#ifndef NcDetector_h
#define NcDetector_h

// Copyright(c) 2021 NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "NcDetectorUnit.h"

/** Creation and investigation of an NCFS generic detector structure. **/

class NcDetector : public NcDetectorUnit
{
 public:
  NcDetector(const char* name="",const char* title="");  // Default constructor
  virtual ~NcDetector();                                 // Default destructor
  NcDetector(const NcDetector& q);                       // Copy constructor
  virtual TObject* Clone(const char* name="") const;     // Make a deep copy and provide its pointer

 protected:

 ClassDef(NcDetector,1) // Creation and investigation of an NCFS generic detector structure.
};
#endif
