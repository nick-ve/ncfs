#ifndef NcDataStreamStats_h
#define NcDataStreamStats_h
// Copyright(c) 1997-2022, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <iostream>
#include <fstream>

#include "TSystem.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"

#include "NcTaggingBase.h"

class NcDataStreamStats : public NcTaggingBase
{
 public:
  NcDataStreamStats(const char* name="NcDataStreamStats",const char* title="Event tag statistics"); // Default constructor
  virtual ~NcDataStreamStats();                                     // Default destructor
  NcDataStreamStats(const NcDataStreamStats& q);                    // Copy constructor
  void AddInputFile(TString file,TString tree,TString branch);      // Add an input file
  void SetMaxEvt(Int_t n);                                          // Set the maximum number of events to be processed (0 : no limitation)
  void SetPrintFrequency(Int_t m);                                  // Set print frequency to provide a progress output line every "m" events
  void ListInputFiles(Option_t* opt="") const;                      // Provide an overview of the various input files
  virtual void Exec(Option_t* opt);                                 // Perform the analysis
  virtual TObject* Clone(const char* name="") const;                // Make a deep copy and provide its pointer
 
 protected:
  TChain* fData;       // The master data Chain used for the analysis
  TString fBranchname; // The name of the Branch containing the NcEvent (or derived) data structures
  Int_t fMaxevt;       // Maximum number of events to be processed
  Int_t fPfreq;        // The print frequency for producing a progress output line
 
 ClassDef(NcDataStreamStats,5) // Provide event classification tag statistics.
};
#endif
