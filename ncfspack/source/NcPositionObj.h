#ifndef NcPositionObj_h
#define NcPositionObj_h
// Copyright(c) 1999 NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "TObject.h"

#include "NcPosition.h"

/** Handling of positions in various reference frames. **/
 
class NcPositionObj : public TObject,public NcPosition
{
 public:
  NcPositionObj();                                  // Default constructor
  NcPositionObj(NcPosition& p);                     // Constructor
  virtual ~NcPositionObj();                         // Destructor
  NcPositionObj(const NcPositionObj& p);            // Copy constructor
  NcPositionObj& operator=(const NcPositionObj& p); // Set all attributes equal to those of NcPositionObj p

 ClassDef(NcPositionObj,2) // Handling of positions in various reference frames.
};
#endif
