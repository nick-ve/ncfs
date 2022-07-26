#ifndef NcTaggingStats_h
#define NcTaggingStats_h
// Copyright(c) 1997-2022, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <iostream>
#include <fstream>

#include "TArrayI.h"
#include "TMatrixF.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "NcJob.h"
#include "NcEvent.h"

class NcTaggingStats : public TTask
{
 public:
  NcTaggingStats(const char* name="NcTaggingStats",const char* title="Event tag statistics"); // Default constructor
  virtual ~NcTaggingStats();                                        // Default destructor
  void SetMaxTag(Int_t n);                                          // Set the maximum number of tags to be processed
  void SetDeviceNames(TString dev,TString pass,TString write);      // Set the name of the NcDevice and the names of the tag indicators
  void SetDevice(TString dev,Bool_t pass=kTRUE,Bool_t write=kTRUE); // Set the name of the NcTagger (derived) device and the check mode of the tag indicators
  void ActivateTag(TString name);                                   // Activate a certain tag for user selected event sample studies
  void DeactivateTag(TString name);                                 // Deactivate a certain tag for background reduction studies
  void ShowStatistics();                                            // Provide an overview of the statistics
  virtual void Exec(Option_t* opt);                                 // Perform the analysis
 
 protected:
  NcEvent* fEvt;       // The current event
  NcEvent* fEvtFirst;  // The event that occurred first in time
  NcEvent* fEvtLast;   // The event that occurred last in time
  Int_t fNevt;         // The number of events that were tagged
  Int_t fNevtComb;     // Number of user reduced events via selected re-tagging
  NcSignal fPass;      // Passing statistics for the encountered tags
  TArrayI fApass;      // Passing flags per event for the encountered tags
  TMatrixF fMpass;     // The tagging matrix with the passing flags statistics
  NcSignal fWrites;    // Writing statistics for the encountered tags
  TArrayI fAwrites;    // Writing flags per event for the encountered tags
  TMatrixF fMwrites;   // The tagging matrix with the writing flags statistics
  NcSignal fCombis;    // (User reduced) combined passing and writing statistics for the encountered tags
  TArrayI fAcombis;    // (User reduced) combined passing and writing flags per event for the encountered tags
  TMatrixF fMcombis;   // The tagging matrix with the (User reduced) combined passing and writing flags statistics
  Bool_t fFirst;       // Flag to indicate first invokation of Exec()

  Int_t fMaxtag;       // Maximum number of tags to be processed
  TString fDevname;    // The name of the NcDevice that contains the various event tags
  TString fPassname;   // Name of the tag passing indicator
  TString fWritename;  // Name of the tag writing indicator
  TObjArray* fAct;     // Array with activated tags
  TObjArray* fDeact;   // Array with de-activated tags
 
 ClassDef(NcTaggingStats,1) // Provide event classification tag statistics.
};
#endif
