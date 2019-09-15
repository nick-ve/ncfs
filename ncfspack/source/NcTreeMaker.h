#ifndef NcTreeMaker_h
#define NcTreeMaker_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcTreeMaker.h 67 2012-07-18 14:51:01Z nickve $

#include "TFile.h"
#include "TTree.h"

#include "NcJob.h"
#include "NcEvent.h"
#include "NcAstrolab.h"

class NcTreeMaker : public TTask
{
 public :
  NcTreeMaker(const char* name="NcTreeMaker",const char* title="ROOT tree data generation"); // Constructor
  virtual ~NcTreeMaker();                                                                    // Destructor
  NcTreeMaker(const NcTreeMaker& q);                         // Copy constructor
  virtual TObject* Clone(const char* name="") const;         // Make a deep copy and provide its pointer
  virtual void Exec(Option_t* opt);                          // Event selection
  void CloseTree();                                          // Proper writing and closing of the output tree
  void SetOutputFile(TString fname,TString ftitle,TString tname="Data",TString ttitle="All event data"); // Create output file for the ROOT data tree
  void SetTrackMaximum(Int_t n);                             // Set maximum number of tracks per name in the output tree
  void UseTracks(TString name,Int_t n=-1,Int_t mode=0);      // Specify track names to be used
  void UseDevices(TString classname);                        // Specify device classes to be used
  void Select(TString type,TString obs,TString uname="");    // Select observables for the output tree
  void SetAstrolab(NcAstrolab* lab);                         // Specify the astrolab to be used
  NcAstrolab* GetAstrolab() const;                           // Provide pointer to the astrolab

 protected :
  Int_t fFirst;                 // Flag to indicate first invokation
  TFile* fOutfile;              // The output file
  TTree* fOuttree;              // The output tree
  TObjArray* fTrees;            // Array with pointers of all created trees with observables
  Int_t fNtkmax;                // The maximum number of tracks per name in the output tree
  NcEvent* fEvt;                // Pointer to the current event structure
  NcAstrolab* fAstrolab;        // Pointer to the astrolab
  Int_t fTrackflag;             // Flag to indicate usage of individual track selection criteria
  Int_t fEventflag;             // Flag to indicate usage of total event selection criteria
  Int_t fDeviceflag;            // Flag to indicate usage of device selection criteria
  TObjArray* fUseTracks;        // The track names to be used 
  TArrayI* fUseNtk;             // The max. numbers of the various track names to be used
  TArrayI* fUseTkMode;          // The modes for the retrieval of the various tracks from the event based on the name (pattern)
  Int_t fTrackMomenta;          // Track momentum selection flag
  Int_t fTrackEnergies;         // Track energy selection flag 
  Int_t fTrackCharges;          // Track charge selection flag
  Int_t fTrackMasses;           // Track mass selection flag
  Int_t fTrackMjds;             // Track MJD selection flag
  Int_t fTrackSecs;             // Track elapsed seconds selection flag
  Int_t fTrackIds;              // Track Ids selection flag
  Int_t fTrackNsigs;            // Track number of associated signals selection flag
  TObjArray* fTrackUnames;      // The names of the track user words to be selected 
  Int_t fTn;                    //! Number of tracks in the output tree storage array
  Float_t* fTp;                 //! Array for track data storage in output tree
  Float_t* fTthetap;            //! Array for track data storage in output tree
  Float_t* fTphip;              //! Array for track data storage in output tree
  Float_t* fTe;                 //! Array for track data storage in output tree
  Float_t* fTq;                 //! Array for track data storage in output tree
  Float_t* fTm;                 //! Array for track data storage in output tree
  Double_t* fTmjd;              //! Array for track data storage in output tree
  Double_t* fTsec;              //! Array for track data storage in output tree
  Int_t* fTid;                  //! Array for track data storage in output tree
  Int_t* fTnsig;                //! Array for track data storage in output tree
  Float_t** fTuser;             //! Matrix array for track data storage in output tree
  Int_t fEventJruns;            // Event run number selection flag
  Int_t fEventJevts;            // Event event number selection flag
  Int_t fEventMjds;             // Event MJD selection flag
  Int_t fEventSecs;             // Event elapsed seconds selection flag
  Int_t fEventMomenta;          // Event momentum selection flag
  Int_t fEventEnergies;         // Event energy selection flag 
  Int_t fEventCharges;          // Event charge selection flag
  Int_t fEventMasses;           // Event invariant mass selection flag
  Int_t fEventWeights;          // Event weight selection flag
  TObjArray* fEventUnames;      // The names of the event user words to be selected 
  Int_t fEjrun;                 //! Variable for event data storage in output tree
  Int_t fEjevt;                 //! Variable for event data storage in output tree
  Double_t fEweight;            //! Variable for event data storage in output tree
  Double_t fEmjd;               //! Variable for event data storage in output tree
  Double_t fEsec;               //! Variable for event data storage in output tree
  Float_t fEp;                  //! Variable for event data storage in output tree
  Float_t fEthetap;             //! Variable for event data storage in output tree
  Float_t fEphip;               //! Variable for event data storage in output tree
  Float_t fEe;                  //! Variable for event data storage in output tree
  Float_t fEq;                  //! Variable for event data storage in output tree
  Float_t fEm;                  //! Variable for event data storage in output tree
  Float_t* fEuser;              //! Variable for event data storage in output tree
  TObjArray* fUseDevices;       // The device classes to be used 
  Int_t fDeviceNdevs;           // Device class number of fired devices selection flag
  Int_t fDeviceNhits;           // Device class number of hits selection flag
  Int_t fDndev;                 //! Variable for device data storage in output tree
  Int_t fDnhit;                 //! Variable for device data storage in output tree
  void Track();                 // Obtain individual track observables
  void Event();                 // Obtain total event observables
  void Device();                // Obtain device observables
  void AstroTrack();            // Create individual track observables from astrophysical reference objects

 ClassDef(NcTreeMaker,3) // TTask derived class to generate a plain ROOT tree from NCFS generic event structures
};
#endif
