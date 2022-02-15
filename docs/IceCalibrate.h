#ifndef IceCalibrate_h
#define IceCalibrate_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

#include "TSystem.h"
#include "TROOT.h"
#include "TTask.h"
#include "TString.h"
#include "TFile.h"

#include "NcJob.h"
#include "IceEvent.h"
#include "IceGOM.h"

class IceCalibrate : public TTask
{
 public :
  IceCalibrate(const char* name="",const char* title="");    // Constructor
  virtual ~IceCalibrate();                                   // Destructor
  virtual void Exec(Option_t* opt);                          // Perform the calibrations
  void SetOMdbase(NcObjMatrix* omdb, TString name="MuDaq");  // Set the OM dbase object
  void SetCalibFile(TString name);                           // Set ROOT calibration input file

 protected :
  TFile* fCalfile;          // The (optional) calibration input file in ROOT format
  NcObjMatrix* fMuDaqDB;    // The MuDaq OM database object
  NcObjMatrix* fTWRDaqDB;   // The TWRDaq OM database object
  NcObjMatrix* fJEBTDaqDB;  // The JEBTDaq OM database object
  NcObjMatrix* fJEBADaqDB;  // The JEBADaq OM database object

 ClassDef(IceCalibrate,5) // TTask derived class to perform the various calibrations
};
#endif
