#ifndef NcTaggingStats_h
#define NcTaggingStats_h
// Copyright(c) 1997-2022, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <iostream>
#include <fstream>

#include "NcTaggingBase.h"
#include "NcJob.h"

class NcTaggingStats : public NcTaggingBase
{
 public:
  NcTaggingStats(const char* name="NcTaggingStats",const char* title="Event tag statistics"); // Default constructor
  virtual ~NcTaggingStats();        // Default destructor
  void ShowStatistics();            // Provide an overview of the statistics
  virtual void Exec(Option_t* opt); // Perform the analysis
 
 ClassDef(NcTaggingStats,2) // Provide event classification tag statistics.
};
#endif
