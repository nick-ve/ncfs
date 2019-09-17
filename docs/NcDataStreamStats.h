#ifndef NcDataStreamStats_h
#define NcDataStreamStats_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <iostream>
#include <fstream>

#include "TTask.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TArrayI.h"
#include "TMatrixF.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "NcEvent.h"

class NcDataStreamStats : public TTask
{
 public:
  NcDataStreamStats(const char* name="NcDataStreamStats",const char* title="Event tag statistics"); // Default constructor
  virtual ~NcDataStreamStats();                                 // Default destructor
  NcDataStreamStats(const NcDataStreamStats& q);                // Copy constructor
  void AddInputFile(TString file,TString tree,TString branch);  // Add an input file
  void SetMaxEvt(Int_t n);                                      // Set the maximum number of events to be processed (0 : no limitation)
  void SetMaxTag(Int_t n);                                      // Set the maximum number of tags to be processed
  void SetPrintFrequency(Int_t m);                              // Set print frequency to provide a progress output line every "m" events
  void SetDeviceNames(TString dev,TString pass,TString write);  // Set the name of the NcDevice and the names of the tag passing criteria
  void ActivateTag(TString name);                               // Activate a certain tag for user selected event sample studies
  void DeactivateTag(TString name);                             // Deactivate a certain tag for background reduction studies
  void ListInputFiles(Option_t* opt="") const;                  // Provide an overview of the various input files
  virtual void Exec(Option_t* opt);                             // Perform the analysis
  virtual TObject* Clone(const char* name="") const;            // Make a deep copy and provide its pointer
 
 protected:
  TChain* fData;       // The master data Chain used for the analysis
  TString fBranchname; // The name of the Branch containing the NcEvent (or derived) data structures
  Int_t fMaxevt;       // Maximum number of events to be processed
  Int_t fMaxtag;       // Maximum number of tags to be processed
  Int_t fPfreq;        // The print frequency for producing a progress output line
  TString fDevname;    // The name of the NcDevice that contains the various event tags
  TString fPassname;   // Name of the tag passing flag
  TString fWritename;  // Name of the tag writing flag
  TObjArray* fAct;     // Array with activated tags
  TObjArray* fDeact;   // Array with de-activated tags
 
 ClassDef(NcDataStreamStats,2) // Provide event classification tag statistics.
};
#endif
