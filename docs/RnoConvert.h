#ifndef RnoConvert_h
#define RnoConvert_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "TFile.h"
#include "TChain.h"
#include "TLeaf.h"

#include "NcJob.h"

#include "RnoDetector.h"
#include "RnoEvent.h"

class RnoConvert : public NcJob
{
 public :
  RnoConvert(const char* name="RnoConvert",const char* title=""); // Constructor
  virtual ~RnoConvert();                                          // Destructor
  void SetMaxEvents(Int_t n);            // Set maximum number of events to be processed
  void SetPrintFreq(Int_t f);            // Set printfrequency to provide info every f events
  void SetSplitLevel(Int_t split);       // Set split level for the produced RnoEvent data file
  void SetBufferSize(Int_t bsize);       // Set buffersize for the produced RnoEvent data file
  void AddInputFile(TString file,TString tree); // Add an RNO-G data input file to the list
  void SetOutputFile(TFile* ofile);      // Set output file for the RnoEvent data structures       
  void SetOutputFile(TString name);      // Create output file for the RnoEvent data structures
  TFile* GetOutputFile();                // Provide pointer to the RnoEvent output file
  virtual void Exec(Option_t* opt);      // Perform the format conversion

 protected :
  Int_t fSplit;           // The split level of the produced RnoEvent data file
  Int_t fBsize;           // The buffersize of the produced RnoEvent data file
  Int_t fMaxevt;          // The maximum number of events to be processed
  Int_t fPrintfreq;       // The event info printing frequency
  TFile* fOutfile;        // The RnoEvent data output file
  TChain* fData;          // TChain with RNO-G input data

 ClassDef(RnoConvert,2) // Job for conversion of RNO-G Root data into RnoEvent data structures.
};
#endif
