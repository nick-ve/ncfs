#ifndef IceDB2Root_h
#define IceDB2Root_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

#include "TFile.h"
#include "TString.h"
#include "TSQLServer.h"
#include "TSQLStatement.h"
#include "TDatabasePDG.h"

#include "NcJob.h"
#include "NcObjMatrix.h"

#include "IceAOM.h"
#include "IceDOM.h"

/** Job for extracting calibration data from database and storing them into an NcObjMatrix OM dbase. **/

class IceDB2Root : public NcJob
{
 public :
  IceDB2Root(const char* name="IceDB2Root",const char* title="");            // Constructor
  virtual ~IceDB2Root();                                                     // Destructor
  void SetDatabase(TString name, TString user, TString password="");         // Set name of database, user and password
  void SetOutputFile(TString name);                                          // Set output file for the ROOT data structures           
  void SetUT(Int_t y, Int_t m, Int_t d, Int_t hh=0, Int_t mm=0, Int_t ss=0); // Set time for which calibration is done
  NcTimestamp GetTime();                                                     // Provide time for which calibration is done
  TDatabasePDG* GetPDG();                                                    // Provide pointer to the PDG database
  NcObjMatrix* GetOMdbase(TString name="MuDaq");                             // Provide pointer to the requested OM database
  virtual void Exec(Option_t* opt);                                          // Perform the format conversion

 protected :
  TString fDBName;          // Name of the database
  TString fUser;            // User name for access to the DB
  TString fPassword;        // Password for access to the DB
  TString fRootFileName;    // Name of the ROOT output file
  TFile* fOutfile;          // The ROOT output file
  NcTimestamp fTime;        // Time for which calibration is done

  TDatabasePDG* fPdg;       // Database with PDG information
  NcObjMatrix* fMuDaqdb;    // Database of all OM devices with their MuDaq geometry, calib. etc... data
  NcObjMatrix* fTWRDaqdb;   // Database of all OM devices with their TWRDaq geometry, calib. etc... data
  NcObjMatrix* fJEBTDaqdb;  // Database of all OM devices with their JEB TWRDaq geometry, calib. etc... data
  NcObjMatrix* fJEBADaqdb;  // Database of all OM devices with their JEB ATWDDaq geometry, calib. etc... data

  void GetMuDaqData();      // Fill MuDaq geometry, calibration and Xtalk parameters of all devices
  void GetTWRDaqData();     // Fill TWRDaq geometry and calibration parameters of all devices
  void GetJEBTDaqData();    // Fill JEB TWRDaq geometry and calibration parameters of all devices
  void GetJEBADaqData();    // Fill JEB ATWDDaq geometry and calibration parameters of all devices

 ClassDef(IceDB2Root,1) // Job for extracting calibration data from database and storing them into an NcObjMatrix OM dbase
};
#endif
