#ifndef NcVeto_h
#define NcVeto_h
// Copyright(c) 2016 NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "NcDevice.h"
#include "NcEvent.h"

/** NcDevice derived class to serve as a generic interface for event vetoing procedures. **/

class NcVeto : public NcDevice
{
 public :
  NcVeto();                                          // Constructor
  virtual ~NcVeto();                                 // Destructor
  NcVeto(const NcVeto& v);                           // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer
  void StoreVetoLevel(NcEvent* evt,Double_t level);  // Store the veto level in the event structure

 ClassDef(NcVeto,2) // NcDevice derived class to serve as a generic interface for event vetoing procedures
};
#endif
