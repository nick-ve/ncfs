#ifndef RnoConvert_h
#define RnoConvert_h

// Copyright(c) 2021, RNO-G Experiment at Summit Station, All rights reserved.
// See cxx source for full Copyright notice.

#include "TSystem.h"
#include "TFile.h"
#include "TChain.h"
#include "TLeaf.h"
#include "TMath.h"
#include "TIterator.h"

#include "NcJob.h"
#include "NcTagger.h"

#include "RnoDetector.h"
#include "RnoEvent.h"

/** Job for conversion of RNO-G Root data into RnoEvent data structures. **/

class RnoConvert : public NcJob
{
 public :
  RnoConvert(const char* name="RnoConvert",const char* title=""); // Constructor
  virtual ~RnoConvert();                                          // Destructor
  void SetMaxEvents(Int_t n);                // Set maximum number of events to be processed
  void SetPrintFreq(Int_t m,Int_t level=0);  // Set printfrequency to provide info every f events
  void SetSplitLevel(Int_t split);           // Set split level for the produced RnoEvent data file
  void SetBufferSize(Int_t bsize);           // Set buffersize for the produced RnoEvent data file
  void AddInputFile(TString file,TString type); // Add an RNO-G data input file to the list
  void SetOutputFile(TFile* ofile);          // Set output file for the RnoEvent data structures       
  void SetOutputFile(TString name);          // Create output file for the RnoEvent data structures
  void SetSelectLevels(Int_t min,Int_t max); // Set required event select level interval for events to be written out
  Int_t GetMinSelectLevel() const;           // Provide minimum required event select level for events to be written out
  Int_t GetMaxSelectLevel() const;           // Provide maximum required event select level for events to be written out
  void ListInput(TString type="*",Option_t* opt=""); // Provide an overview listing of the selected input data
  TFile* GetOutputFile();                    // Provide pointer to the RnoEvent output file
  virtual void Exec(Option_t* opt);          // Perform the format conversion

 protected :
  Int_t fSplit;           // The split level of the produced RnoEvent data file
  Int_t fBsize;           // The buffersize of the produced RnoEvent data file
  Int_t fMaxevt;          // The maximum number of events to be processed
  Int_t fPrintfreq;       // The event info printing frequency
  Int_t fPrintlevel;      // The event info printing level
  TFile* fOutfile;        // The RnoEvent data output file
  TChain* fHdr;           // TChain with RNO-G event header data
  TChain* fDs;            // TChain with RNO-G event DAQ status data
  TChain* fWf;            // TChain with RNO-G event waveform data
  TChain* fComb;          // TChain with RNO-G combined event data
  TChain* fPed;           // TChain with RNO-G pedestal data for all channels
  TChain* fData;          // TChain containing all of the above RNO-G input data
  Int_t fMinSelectLevel;  // Minimum event select level for events to be written out
  Int_t fMaxSelectLevel;  // Maximum event select level for events to be written out

  void CreateMainChain();

 ClassDef(RnoConvert,9) // Job for conversion of RNO-G Root data into RnoEvent data structures.
};
#endif
