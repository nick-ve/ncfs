#ifndef NcPositionObj_h
#define NcPositionObj_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcPositionObj.h 5 2010-03-19 10:10:02Z nickve $

#include "TObject.h"

#include "NcPosition.h"
 
class NcPositionObj : public TObject,public NcPosition
{
 public:
  NcPositionObj();                        // Default constructor
  NcPositionObj(NcPosition& p);           // Constructor
  virtual ~NcPositionObj();               // Destructor
  NcPositionObj(const NcPositionObj& p);  // Copy constructor

 ClassDef(NcPositionObj,1) // Handling of positions in various reference frames.
};
#endif
