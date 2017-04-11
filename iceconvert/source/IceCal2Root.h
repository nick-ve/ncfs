#ifndef IceCal2Root_h
#define IceCal2Root_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceCal2Root.h 5 2010-03-19 10:10:02Z nickve $

#include "TFile.h"
#include "TString.h"
#include "TDatabasePDG.h"

#include "NcJob.h"
#include "NcObjMatrix.h"

#include "IceAOM.h"

#include "Riostream.h"

class IceCal2Root : public NcJob
{
 public :
  IceCal2Root(const char* name="IceCal2Root",const char* title=""); // Constructor
  virtual ~IceCal2Root();                                           // Destructor
  void SetAmacalibFile(TString name); // Set name of the Amacalib MuDaq input file
  void SetTWRDaqFile(TString name);   // Set name of the TWRDaq calibration input file
  void SetOutputFile(TString name);   // Set output file for the ROOT data structures           
  TDatabasePDG* GetPDG();             // Provide pointer to the PDG database
  NcObjMatrix* GetOMdbase(TString name="MuDaq"); // Provide pointer to the requested OM database
  virtual void Exec(Option_t* opt);   // Perform the format conversion

 protected :
  ifstream fInput;         // Input stream for generic use of reading data

  TString fAmacalFileName; // Name of the Amacalib MuDaq input file
  TString fTWRDaqFileName; // Name of the TWRDaq calibration input file
  TString fRootFileName;   // Name of the ROOT output file
  TFile* fOutfile;         // The ROOT output file

  TDatabasePDG* fPdg;      // Database with PDG information
  NcObjMatrix* fMuDaqdb;   // Database of all OM devices with their MuDaq geometry, calib. etc... data
  NcObjMatrix* fTWRDaqdb;  // Database of all OM devices with their TWRDaq geometry, calib. etc... data

  void GetMuDaqData();     // Fill MuDaq geometry, calibration and Xtalk parameters of all devices
  void GetTWRDaqData();    // Fill TWRDaq geometry and calibration parameters of all devices

 ClassDef(IceCal2Root,2) // Job for conversion of (ascii) calibration data into an NcObjMatrix OM dbase
};
#endif
