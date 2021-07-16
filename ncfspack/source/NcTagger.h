#ifndef NcTagger_h
#define NcTagger_h

// Copyright(c) 1997-2021, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "NcDevice.h"

class NcTagger : public NcDevice
{
 public:
  NcTagger(const char* name="NcTagger",const char* title="Event tag handling"); // Default constructor
  virtual ~NcTagger();                                   // Default destructor
  NcTagger(const NcTagger& q);                           // Copy constructor
  virtual TObject* Clone(const char* name="") const;  // Make a deep copy and provide its pointer
  void SetPass(TString name,Bool_t flag);  // Set the pass flag of the tag with the specified name
  void SetWrite(TString name,Bool_t flag); // Set the write flag of the tag with the specified name
  Bool_t GetPass(TString name);            // Provide the pass flag of the tag with the specified name
  Bool_t GetWrite(TString name);           // Provide the write flag of the tag with the specified name

 protected:
  void SetTag(TString hitname,TString slotname,Float_t value); // Record the tag data

 ClassDef(NcTagger,1) // Handling of generic event classification tags.
};
#endif
