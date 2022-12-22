#ifndef NcTaggingSelector_h
#define NcTaggingSelector_h
// Copyright(c) 2022, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <iostream>
#include <fstream>

#include "NcTaggingBase.h"
#include "NcJob.h"

class NcTaggingSelector : public NcTaggingBase
{
 public:
  NcTaggingSelector(const char* name="NcTaggingSelector",const char* title="Event selection"); // Default constructor
  virtual ~NcTaggingSelector();     // Default destructor
  virtual void Exec(Option_t* opt); // Perform the analysis
 
 ClassDef(NcTaggingSelector,1) // Perform event selection based on event tag settings
};
#endif
