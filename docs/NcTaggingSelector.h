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
  void SetLogic(TString logic,TString type); // Set the logic for combination of tag selections

 protected:
  TString fLogicA; // Logic for tags that are requested to be set "passed" (aka Active tags)
  TString fLogicD; // Logic for tags that are requested to be set "not passed" (aka Deactive tags)
 
 ClassDef(NcTaggingSelector,2) // Perform event selection based on event tag settings
};
#endif
