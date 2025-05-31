#ifndef NcCollider_h
#define NcCollider_h
// Copyright(c) 2002 NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "TSystem.h"
#include "TPythia6.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TArrayI.h" 

#include "NcEvent.h"
#include "NcRandom.h"
#include "NcBoost.h"
#include "NcTreeMaker.h"

/** Pythia based universal (astro)physics event generator. **/
 
class NcCollider : public TPythia6
{
 public:
  NcCollider();                                         // Default constructor
  virtual ~NcCollider();                                // Default destructor
  NcTreeMaker* SetOutputFile(TString fname,Int_t mode=0); // Initialise the output data file(s)
  void SetVertexMode(Int_t mode);                       // Select mode for (sec.) vertex structure creation
  Int_t GetVertexMode() const;                          // Provide vertex structure creation mode
  void SetResolution(Double_t res);                     // Set resolution (in meter) for resolving (sec.) vertices
  Double_t GetResolution() const;                       // Provide (sec.) vertex resolving resolution (in meter)
  void SetRunNumber(Int_t run);                         // Set user defined run number
  Int_t GetRunNumber() const;                           // Provide the user defined run number
  void SetPrintFreq(Int_t n);                           // Set print frequency for every 'n' events
  Int_t GetPrintFreq() const;                           // Provide the print frequency
  Int_t Init(TString frame,TString beam,TString target,Float_t win,Nc3Vector* pbeam=0,Nc3Vector* ptarget=0,Int_t wxsec=1,Double_t fact=0); // Standard Pythia initialisation
  Int_t Init(TString frame,Int_t zp,Int_t ap,Int_t zt,Int_t at,Float_t win,Nc3Vector* pbeam=0,Nc3Vector* ptarget=0,Int_t wxsec=1); // Nucleus-Nucleus initialisation
  void SetMomentum(Nc3Vector& p,Int_t mode);            // Set the beam or target particle 3-momentum
  void SetStable(Int_t id,Int_t mode=1,Int_t cls=0);    // Declare a certain particle (class) as stable or not
  void SelectEvent(Int_t id);                           // Select only events containing specified particles
  Int_t GetSelectionFlag() const;                       // Return the selection flag for this event
  Int_t MakeEvent(Int_t npt=0,Int_t mlist=-1,Int_t medit=1);// Generate a single event with npt participant nucleons
  void EndRun();                                        // Properly close all buffers and output file
  NcEvent* GetEvent(Int_t select=0) const;              // Provide pointer to the generated event structure
  void SetSpectatorPmin(Float_t pmin);                  // Set minimal momentum for spectator track to be stored
  Float_t GetSpectatorPmin() const;                     // Provide the minimal momentum for spectator tracks
  void SetUserControl(Int_t flag);                      // Selection of full user control w.r.t. MC parameters. 
  Int_t GetUserControl() const;                         // Provide the value of the user control flag.
  void SetElastic(Int_t flag);                          // Selection flag for elastic and diffractive processes.
  Int_t GetElastic() const;                             // Provide the value of the elastic selection flag.
  void SetMultiple(Int_t flag);                         // Selection flag for multiple interactions.
  Int_t GetMultiple() const;                            // Provide the value of the multiple selection flag.
  void SetEcmsMin(Double_t ecms);                       // Set minimal CMS energy (in GeV) for events to get generated
  Double_t GetEcmsMin() const;                          // Provide the minimal CMS energy (in GeV) for events to get generated
  void SetRandomSeed(Int_t iseed);                      // Initialise a specific random number sequence.
  Int_t GetRandomSeed();                                // Provide the current random number sequence seed.
  Float_t GetWin() const;                               // Provide the Pythia energy indicator (in GeV) used for initialisation

  // Facilities for the p+p and p+gamma modeling for the jet of (obscured) astrophysical objects
  void SetJetProtonSpectrum(Double_t pmin,Double_t pmax=-1,TF1* fspec=0,TH1* hspec=0,Int_t mode=0); // Set the proton (beam) spectrum of the Jet
  void SetJetGammaSpectrum(Double_t pmin,Double_t pmax=-1,TF1* fspec=0,TH1* hspec=0,Int_t mode=0);  // Set the gamma (target) spectrum of the Jet
  TH1* GetJetProtonSpectrum(Double_t* pmin=0,Double_t* pmax=0); // Get the proton (beam) spectrum of the Jet
  TH1* GetJetGammaSpectrum(Double_t* pmin=0,Double_t* pmax=0);  // Get the gamma (target) spectrum of the Jet
  void ProcessJet(Double_t np,Double_t gfrac,TString flux,Double_t dthmax=0,Int_t nlist=1,Int_t ntrymax=1000,Int_t wxsec=0,Double_t finit=0,Int_t full=0); // Perform the astrophysical Jet simulation

 protected:
  Int_t fVertexmode;    // The vertex structure creation mode
  Double_t fResolution; // The resolution (in meter) for resolving (sec.) vertices 
  Int_t fRunnum;        // The user defined run number
  Int_t fEventnum;      // The automatically updated event number
  Int_t fPrintfreq;     // The user selected print frequency
  TString fFrame;       // The Pythia frame indicator
  Float_t fWin;         // The Pythia energy indicator
  NcTrack fBeam;        // Track parameters for the beam particle in the selected experimental frame
  NcTrack fTarget;      // Track parameters for the target particle in the selected experimental frame
  Int_t fNucl;          // Flag to denote nucleus-nucleus (1) or standard Pythia (0) running
  Int_t fZproj;         // Z of the projectile particle
  Int_t fAproj;         // A of the projectile particle
  Int_t fZtarg;         // Z of the target particle
  Int_t fAtarg;         // A of the target particle
  Float_t fFracpp;      // Fraction of p+p collisions
  Float_t fFracnp;      // Fraction of n+p collisions
  Float_t fFracpn;      // Fraction of p+n collisions
  Float_t fFracnn;      // Fraction of n+n collisions
  NcRandom fRan;        // Random number generator
  NcEvent* fEvent;      // The produced event structure
  Float_t fSpecpmin;    // The minimal momentum for spectator tracks to be stored
  Int_t fUserctrl;      // Flag to denote the user control selection w.r.t. MC parameters
  Int_t fElastic;       // Flag to denote inclusion of elastic and difractive processes.
  Int_t fMultiple;      // Flag to denote inclusion of multiple interactions.
  Double_t fEcmsmin;    // Minimal CMS energy (in GeV) for events to get generated
  Int_t fWxsec;         // Flag to denote event weighting by cross section for variable energy runs
  NcBoost fLorbo;       // The Lorentz boost for the case of forced CMS processing

  TFile* fOutFile;      // The user defined output file for the NcEvent data structures 
  TTree* fOutTree;      // The ROOT output tree with the NcEvent data structures

  TArrayI* fSelections; // The particle KC codes for event selection
  Int_t fSelect;        // Flag to indicate whether the total event is selected (1) or not (0)

  NcTreeMaker* fMktree; // The TreeMaker task processor
  NcJob* fJob;          // The NcJob to perform all (sub)tasks
  NcSignal fEvtuser;    // Storage for event user data

  // Parameters for the astrophysical Jet modeling
  Double_t fJetPpmin; // The Jet minimum proton (beam) momentum
  Double_t fJetPpmax; // The Jet maximum proton (beam) momentum
  Double_t fJetGpmin; // The Jet minimum gamma (target) momentum
  Double_t fJetGpmax; // The Jet maximum gamma (target) momentum
  TH1* fJetPspectrum; // The Jet proton (beam) spectrum
  Int_t fJetPscale;   // Linear, Log10 or Ln scale indicator for fJetPspectrum X-axis
  TH1* fJetGspectrum; // The Jet gamma (target) specrrum
  Int_t fJetGscale;   // Linear, Log10 or Ln scale indicator for fJetGspectrum X-axis
  NcAstrolab fLab;    // To provide facilities for the astrophysical Jet modeling

  Int_t IsSelected();   // Check whether (sub)event passed the selection criteria
  void GetFractions(Float_t zp,Float_t ap,Float_t zt,Float_t at); // Determine various N-N collision fractions
  TString GetPyname(Int_t kf); // Provide the correctly truncated Pythia particle name for PDG code kf  

 ClassDef(NcCollider,10) // Pythia based universal (astro)physics event generator
};
#endif
