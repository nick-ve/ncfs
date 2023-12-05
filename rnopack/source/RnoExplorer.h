#ifndef RnoExplorer_h
#define RnoExplorer_h
// Copyright(c) 2023, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <math.h>

#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"

#include "TApplication.h"
#include "TGFrame.h"
#include "TGComboBox.h"
#include "TGNumberEntry.h"
#include "TGButtonGroup.h"

#include "NcVersion.h"
#include "RnoEvent.h"
#include "NcBlocks.h"
#include "NcDSP.h"

class RnoExplorer : public TChain
{
 public:
  RnoExplorer(const char* name="T",const char* title="RnoExplorer"); // Constructor
  virtual ~RnoExplorer();            // Destructor
  void ShowPanel();                  // Show the GUI panel
  void FileAdd();                    // Add the specified input file to the input chain
  void LoadEntry();                  // Load the specified entry
  void ExpChannel(const char* text); // Update the channel selection
  void ExpMode(Int_t i);             // Set the sampling mode
  void ExpFmV(const char* text);     // Update the function for the ADC->mV conversion
  void ExpOpsMode(Int_t i);          // Set the mode for the operations processing
  void ExpOpsPar1(const char* text); // Update the operations parameter1
  void ExpOpsPar2(const char* text); // Update the operations parameter2
  void ExpOpsPar3(const char* text); // Update the operations parameter3
  void ExpExecMode(Int_t i);         // Select the execution mode for the selected operation(s) for the requested channel(s)
  void ExpExecute();                 // Execute the selected operation(s) for the requested channel(s)
  void ExpDevName(const char* text); // Update the device (class) name selection
  void ListDevices();                // List the various devices
  void ListDAQ();                    // List the event DAQ status
  void ListTriggers();               // List the event trigger data
  void ListTags();                   // List the event tag/filter data
  void ExpClose();                   // Close the explorer panel window
  void ExpExit();                    // Exit the current ROOT session

 protected:
  Int_t fEnt;               // The current TChain entry number
  RnoEvent* fEvt;           // Pointer to the RnoEvent object
  Double_t fSample;         // The DAQ sampling rate
  Int_t fStation;           // The current station number
  Int_t fChannel;           // The selected channel number (-1=all)
  TString fMode;            // The sampling mode
  TString fFmV;             // Description of the ADC->mV conversion
  TString fOpsMode;         // The mode for the operations processing
  TString fOpsPars[3];      // The parameter values for the operations
  TString fExecMode;        // The parameter for the execution mode
  TH1F fHistos[3][24];      // Histograms of the various Stores/channels for analysis
  TString fYtitles[3][24];  // Y-axis title of the time domain input of the Fourier transform for the various Stores/channels
  TArrayD fReArr[3][24];    // Real Fourier coefficients for the various Stores/channels
  TArrayD fImArr[3][24];    // Imaginary Fourier coefficients for the various Stores/channels
  TArrayD fFilterBands[24]; // Filter band fequencies for the various channels

  TString fDevName;         // The device (class) name for listing

  TCanvas* fCanvas[3];      // The canvases for the storage and analysis displays

  NcBlocks fBB;             // (Bayesian) Block facilities
  NcDSP fDSP;               // DSP facilities

  TGMainFrame* fExpPanel;   // The panel mainframe
  TGTextEntry* fFiles;      // The text entry for the input filenames
  TGTextEntry* fNentries;   // The text window to show the number of entries
  Bool_t fConnected;        // Flag to indicate that the input branch is connected
  TGTextEntry* fEntry;      // The text entry to select a specific entry
  TGTextEntry* fHead[4];    // The text windows to show the event header data
  TGTextEntry* fParams[3];  // The text windows for the various operations parameters

  // Internal functions for the composition of the various sub-panels of the ExplorerPanel GUI
  virtual void ExplorePanel();                           // Main routine for creation of the GUI panel
  virtual void FilesPanel(TGCompositeFrame* frame);      // Compose the input file and entries panel 
  virtual void HeaderPanel(TGCompositeFrame* frame);     // Compose the entry selection and event header info panel 
  virtual void SamplingsPanel(TGCompositeFrame* frame);  // Compose the samplings selection panel 
  virtual void CommandPanel(TGCompositeFrame* frame);    // Compose the command panel 
  void FilesEnter();    // Internal memberfunction to enter the selected input file(s) into the input data chain
  void HeadEnter();     // Internal memberfunction to enter the event header data into the information windows
  void GetHistograms(Int_t jstore); // Internal memberfunction to obtain the requested output histograms
  void DrawResult(Int_t jstore); // Draw the result of the selected operation(s) for the requested channel(s)

 ClassDef(RnoExplorer,1) // Graphical User Interface (GUI) to explore RNO-G event data.
};
#endif

