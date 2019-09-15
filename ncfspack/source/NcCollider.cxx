/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.                     *
 *                                                                             *
 * Authors: The Netherlands Center for Fundamental Studies (NCFS).             *
 *          The Inter-university Institute for High Energies (IIHE).           *                 
 *                    Website : http://www.iihe.ac.be                          *
 *            Contact : Nick van Eijndhoven (nickve.nl@gmail.com)              *
 *                                                                             *
 * Contributors are mentioned in the code where appropriate.                   *
 *                                                                             * 
 * No part of this software may be used, copied, modified or distributed       *
 * by any means nor transmitted or translated into machine language for        *
 * commercial purposes without written permission by the IIHE representative.  *
 * Permission to use the software strictly for non-commercial purposes         *
 * is hereby granted without fee, provided that the above copyright notice     *
 * appears in all copies and that both the copyright notice and this           *
 * permission notice appear in the supporting documentation.                   *
 * This software is provided "as is" without express or implied warranty.      *
 * The authors make no claims that this software is free of error, or is       *
 * consistent with any particular standard of merchantability, or that it      *
 * will meet your requirements for any particular application, other than      *
 * indicated in the corresponding documentation.                               *
 * This software should not be relied on for solving a problem whose           *
 * incorrect solution could result in injury to a person or loss of property.  *
 * If you do use this software in such a manner, it is at your own risk.       *
 * The authors disclaim all liability for direct or consequential damage       *
 * resulting from your use of this software.                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// $Id: NcCollider.cxx 126 2016-05-26 13:01:32Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcCollider
// Pythia based universal (astro)physics event generator.
// This class is derived from TPythia6 and has some extensions to
// support also generation of nucleus-nucleus interactions and to allow
// investigation of the effect of detector resolving power.
// Furthermore, the produced event information is provided in a format
// using the NcEvent structure.
// For the produced NcTrack objects, the particle ID code is set to the
// Pythia KF value, which is compatible with the PDG identifier.
// This will allow a direct analysis of the produced data using the
// NCFSPack physics analysis tools.
//
// For further details concerning the produced output structure,
// see the docs of the memberfunctions SetVertexMode and SetResolution.
//
// Example job of minimum biased Pb+Pb interactions :
// --------------------------------------------------
// {
//  gSystem->Load("libEG");
//  gSystem->Load("libEGPythia6");
//  gSystem->Load("ncfspack");
//
//  NcCollider* gen=new NcCollider();
//
//  gen->SetOutputFile("test.root");
//  gen->SetVertexMode(3);    
//  gen->SetResolution(1e-6); // 1 micron vertex resolution
//
//  gen->SetRunNumber(1);
//
//  Int_t zp=82;
//  Int_t ap=208;
//  Int_t zt=82;
//  Int_t at=208;
//
//  gen->Init("fixt",zp,ap,zt,at,158);
//
//  gen->SetTitle("SPS Pb-Pb collision at 158A GeV/c beam energy");
//
//  Int_t nevents=5;
//
//  NcRandom rndm;
//  Float_t* rans=new Float_t[nevents];
//  rndm.Uniform(rans,nevents,2,ap+at);
//  Int_t npart;
//  for (Int_t i=0; i<nevents; i++)
//  {
//   npart=rans[i];
//   gen->MakeEvent(npart);
//
//   NcEvent* evt=gen->GetEvent();
//  
//   evt->List();
//  }
//
//  gen->EndRun();
// }
//
//
// Example job of a cosmic nu+p atmospheric interaction.
// -----------------------------------------------------
// {
//  gSystem->Load("libEG");
//  gSystem->Load("libEGPythia6");
//  gSystem->Load("ncfspack");
//
//  NcCollider* gen=new NcCollider();
//
//  gen->SetOutputFile("test.root");
//
//  gen->SetRunNumber(1);
//
//  gen->Init("fixt","nu_mu","p",1e11);
//
//  gen->SetTitle("Atmospheric nu_mu-p interaction at 1e20 eV");
//
//  Int_t nevents=10;
//
//  for (Int_t i=0; i<nevents; i++)
//  {
//   gen->MakeEvent(0,1);
//
//   NcEvent* evt=gen->GetEvent();
//  
//   evt->Data();
//  }
//
//  gen->EndRun();
// }
//
//
//--- Author: Nick van Eijndhoven 22-nov-2002 Utrecht University
//- Modified: NvE $Date: 2016-05-26 15:01:32 +0200 (Thu, 26 May 2016) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "NcCollider.h"
#include "Riostream.h"
 
ClassImp(NcCollider) // Class implementation to enable ROOT I/O
 
NcCollider::NcCollider() : TPythia6()
{
// Default constructor.
// All variables initialised to default values.
//
// Some Pythia default MC parameters are automatically modified to provide
// more suitable running conditions for soft processes in view of
// nucleus-nucleus interactions and astrophysical processes.
// The user may initialise the generator with all the default Pythia
// parameters and obtain full user control to modify the settings by means
// of the SetUserControl memberfunction.
//
// Refer to the SetElastic memberfunction for the inclusion of elastic
// and diffractive processes.
// By default these processes are not included.

 fVertexmode=0;    // No vertex structure creation
 fResolution=1e-7; // Standard resolution is 0.1 micron
 fRunnum=0;
 fEventnum=0;
 fPrintfreq=1;
 fUserctrl=0;  // Automatic optimisation of some MC parameters 
 fElastic=0;   // No elastic and diffractive processes
 fMultiple=1;  // Include multiple interactions
 fEcmsmin=2.7; // Minimal CMS energy (in GeV) for events to get generated

 fEvent=0;

 fSpecpmin=0;

 fFrame="none";
 fWin=-1;
 fWxsec=0;

 fNucl=0;
 fZproj=0;
 fAproj=0;
 fZtarg=0;
 fAtarg=0;
 fFracpp=0;
 fFracnp=0;
 fFracpn=0;
 fFracnn=0;

 fOutFile=0;
 fOutTree=0;

 fSelections=0;
 fSelect=0;

 TString s=GetName();
 s+=" (NcCollider)";
 SetName(s.Data());
 SetTitle("");
}
///////////////////////////////////////////////////////////////////////////
NcCollider::~NcCollider()
{
// Default destructor

 if (fEvent)
 {
  delete fEvent;
  fEvent=0;
 }

 if (fSelections)
 {
  delete fSelections;
  fSelections=0;
 }

 // In case of output to an output file the Tree is deleted
 // at the closing of the output file.
 if (fOutTree && !fOutFile)
 {
  delete fOutTree;
  fOutTree=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetOutputFile(TString s)
{
// Create the output file containing all the data in ROOT output format.

 // Flush and close the current existing output file (if any)
 // This will also delete the existing output tree connected to this file
 if (fOutFile)
 {
  if (fOutFile->IsOpen())
  {
   fOutFile->Write();
   fOutFile->Close();
  }
 }

 // Create a new output file
 fOutFile=new TFile(s.Data(),"RECREATE","NcCollider data");

 // Create a new NcEvent structure 
 if (fEvent)
 {
  delete fEvent;
  fEvent=0;
 }
 fEvent=new NcEvent();
 fEvent->SetOwner();
 fEvent->SetName(GetName());
 fEvent->SetTitle(GetTitle());

 // Create a new output Tree
 fOutTree=new TTree("T","NcCollider event data");
 Int_t bsize=32000;
 Int_t split=0;
 fOutTree->Branch("Events","NcEvent",&fEvent,bsize,split);

 cout << " *NcCollider::SetOutputFile* Event data will be written to output file: " << fOutFile->GetName() << endl;
 cout << endl;
 cout << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetVertexMode(Int_t mode)
{
// Set the mode of the vertex structure creation.
//
// By default all generated tracks will only appear in the NcEvent
// structure without any primary (and secondary) vertex structure.
// The user can build the vertex structure if he/she wants by means
// of the beginpoint location of each NcTrack.
//
// However, one can also let NcCollider automatically create
// the primary (and secondary) vertex structure(s).
// In this case the primary vertex is given Id=1 and all sec. vertices
// are given Id's 2,3,4,....
// All vertices are created as standalone entities in the NcEvent structure
// without any linking between the various vertices.
// For this automated process, the user-selected resolution
// (see SetResolution) is used to decide whether or not certain vertex
// locations can be resolved.
// In case no vertex creation is selected (i.e. the default mode=0),
// the value of the resolution is totally irrelevant.
//
// The user can also let NcCollider automatically connect the sec. vertices
// to the primary vertex (i.e. mode=3). This process will also automatically
// generate the tracks connecting the vertices.
// Note that the result of the mode=3 operation may be very sensitive to
// the resolution parameter. Therefore, no attempt is made to distinguish
// between secondary, tertiary etc... vertices. All sec. vertices are
// linked to the primary one.
//  
// Irrespective of the selected mode, all generated tracks can be obtained
// directly from the NcEvent structure.
// In case (sec.) vertex creation is selected, all generated vertices can
// also be obtained directly from the NcEvent structure. 
// These (sec.) vertices contain only the corresponding pointers to the various
// tracks which are stored in the NcEvent structure.
//
// Overview of vertex creation modes :
// -----------------------------------
// mode = 0 ==> No vertex structure will be created
//        1 ==> Only primary vertex structure will be created
//        2 ==> Unconnected primary and secondary vertices will be created
//        3 ==> Primary and secondary vertices will be created where all the
//              sec. vertices will be connected to the primary vertex.
//              Also the vertex connecting tracks will be automatically
//              generated. 
//
 if (mode<0 || mode >3)
 {
  cout << " *NcCollider::SetVertexMode* Invalid argument mode : " << mode << endl;
  fVertexmode=0;
 }
 else
 {
  fVertexmode=mode;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::GetVertexMode() const
{
// Provide the current mode for vertex structure creation.
 return fVertexmode;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetResolution(Double_t res)
{
// Set the resolution (in meter) for resolving (sec.) vertices.
// By default this resolution is set to 0.1 micron.
// Note : In case no vertex creation has been selected, the value of
//        the resolution is totally irrelevant.
 fResolution=fabs(res);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcCollider::GetResolution() const
{
// Provide the current resolution (in meter) for resolving (sec.) vertices.
 return fResolution;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetRunNumber(Int_t run)
{
// Set the user defined run number.
// By default the run number is set to 0.
 fRunnum=run;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::GetRunNumber() const
{
// Provide the user defined run number.
 return fRunnum;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetPrintFreq(Int_t n)
{
// Set the print frequency for every 'n' events.
// By default the printfrequency is set to 1 (i.e. every event).
 fPrintfreq=n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::GetPrintFreq() const
{
// Provide the user selected print frequency.
 return fPrintfreq;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetUserControl(Int_t flag)
{
// Set the user control flag w.r.t. disabling automatic optimisation
// of some Pythia default MC parameters for soft interactions in view of
// nucleus-nucleus collisions and astrophysical processes.
// Flag = 0 : Limited user control (automatic optimisation enabled)
//        1 : Full user control (automatic optimisation disabled)
// By default the user control is set to 0 (i.e. automatic optimisation).
// See the Init() memberfunctions for further details w.r.t. the optimisations.
 fUserctrl=flag;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::GetUserControl() const
{
// Provide the value of the user control flag.
 return fUserctrl;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetElastic(Int_t flag)
{
// Set the flag w.r.t. inclusion of elastic and diffractive processes.
// By default these processes are not included.
// Flag = 0 : Do not include elastic and diffractive processes
//        1 : Elastic and diffractive processes will be included
 fElastic=flag;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::GetElastic() const
{
// Provide the value of the control flag for elastic and diffractive processes.
 return fElastic;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetMultiple(Int_t flag)
{
// Set the flag w.r.t. inclusion of multiple interactions.
// By default these processes are included.
// Flag = 0 : Do not include multiple interactions
//        1 : Multiple interactions will be included
 fMultiple=flag;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::GetMultiple() const
{
// Provide the value of the control flag for multiple interactions.
 return fMultiple;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetEcmsMin(Double_t ecms)
{
// Set the minimal CMS energy (in GeV) for events to get generated.
// Notes :
// -------
// 1) This memberfunction may be invoked at will before the initialisation
//    or before the generation of any event. 
// 2) By default a minimal CMS energy of 2.7 GeV is required for event generation.
//    At lower values the underlying Pythia event generation gets rather slow
//    because of the limited available phase-space.
 fEcmsmin=ecms;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcCollider::GetEcmsMin() const
{
// Provide the minimal CMS energy (in GeV) for events to get generated.
 return fEcmsmin;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetRandomSeed(Int_t iseed)
{
// Initialise the random number generator with a specific sequence.
// Regular allowed values are 0<=iseed<=900000000.
// However, also the specification iseed<0 is allowed to obtain an
// automatic sequence selection as explained below.
//
// Notes :
// -------
// 1) In case the user does not invoke this memberfunction, the default
//    Pythia seed will be used. 
// 2) This memberfunction may be invoked at will before the initialisation
//    or before the generation of any event. 
// 3) A convenient way to automatically start each NcCollider run with a
//    different seed is to construct the seed from e.g. the date and time.
//    An automatic (quasi unique) seed generation based on date and time information
//    (with second precision) is obtained by providing iseed<0 as input argument.

 if (iseed>900000000) return;

 if (iseed<0)
 {
  NcTimestamp ts;
  Int_t mjd,sec,ns;
  ts.GetMJD(mjd,sec,ns);
  iseed=10000*sec+(mjd%10000);
 }
 SetMRPY(1,iseed);
 SetMRPY(2,0);
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::GetRandomSeed()
{
// Provide the value of the current random number sequence seed.
 Int_t iseed=GetMRPY(1);
 return iseed;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcCollider::GetWin() const
{
// Provide the value of the Pythia energy indicator (in GeV) used for initialisation.
// For regular frame="free" processing the value is set to -1, whereas a positive value
// indicates forced CMS processing. See Init() for further details.
 return fWin;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::Init(TString frame,TString beam,TString target,Float_t win,Nc3Vector* pbeam,Nc3Vector* ptarget,Int_t wxsec,Double_t fact)
{
// Initialisation of the underlying Pythia generator package.
// The event number is reset to 0.
//
// Input arguments :
// -----------------
// frame   : "cms"  --> Colliding beam experiment in the CM frame (beam=+z, target=-z direction)
//           "fixt" --> Fixed target experiment with beam in the +z direction
//           "free" --> System defined by the user via "pbeam" and "ptarget"
// beam    : Beam particle specification (see below).
// target  : Target particle specification (see below).
// win     : Energy of the system depending on the frame specification.
//           frame="cms"  --> win is the total cms energy in GeV
//           frame="fixt" --> win is the beam particle momentum in GeV/c
//           frame="free" --> win>=0 forces event generation in the CMS followed by a Lorentz boost (using NcBoost)
//                            according to the user defined conditions via "pbeam" and "ptarget" (see below).
//                            For win>0 the CM energy (Ecms) for cross section initialisations is set to the value of "win".
//                            Consequently, the use of the parameter "fact" (see below) is de-activated.
//                            For win=0 the Ecms for cross section initialisations is determined on basis of the specified
//                            "pbeam" and "ptarget" and the setting of "fact" is taken into account.
//                            Note that events will always be generated according to the actual settings of "pbeam" and "ptarget".
//                            For very large boosts (e.g. Cosmic Ray physics above 10 PeV/c) the Pythia boost facility yields
//                            incorrect results due to accuracy issues. The more accurate NcBoost facility is used instead.
//                            This implies that for win>=0 the Pythia event data will reflect the situation in the CM frame
//                            and that the event data in the user frame (defined via "pbeam" and "ptarget") is only available
//                            from the produced NcEvent structure (see memberfunction GetEvent).
//                            Due to internal Pythia accuracy issues it turns out that forcing the event generation
//                            in the CMS may also result in faster processing.  
//                            In case win<0 it is a dummy parameter of which the value is irrelevant and the cross section
//                            initialisations are based on the settings of "pbeam" and "ptarget" with the setting of "fact"
//                            taken into account.
// pbeam   : Beam particle 3-momentum in GeV/c   (only to be provided if frame="free").
// ptarget : Target particle 3-momentum in GeV/c (only to be provided if frame="free").
// wxsec   : Cross section weighting (1) or not (0) in the case frame="free".
//           Weighting by cross section is necessary to obtain correct particle spectra when running
//           at different energies within the same generation run via frame="free" and SetMomentum().
//           The most accurate weighting is obtained by invoking this Init() function at the interaction
//           energy with the highest cross section. In general this is just the highest foreseen energy
//           (e.g. hadron-hadron interactions) or at some resonance (e.g. e+e- interactions).
//           To use a certain interaction energy for cross section initialisations without changing the actual
//           beam and/or target 3-momenta, please refer to the "fact" input argument outlined hereafter. 
//           Note that with weighting sometimes no event is generated after invoking MakeEvent()
//           to preserve the correct relative number of events for different energies. 
//           The information whether an event has been generated or not is indicated via the
//           return argument of MakeEvent().
// fact    : Multiplication factor for the beam or target 3-momentum to be used for initialisation purposes
//           (e.g. selection of processes and cross sections) only.
//           Initialisation at a different interaction energy than the one corresponding to the specified
//           beam and target 3-momenta might be needed as explained above, or to ensure activation of the
//           relevant processes in case interactions are to be studied around threshold energies.
//           The latter may save considerable cpu time at the event generation, since most of the phase space
//           has been scanned already at the initialisation step.
//           Note that also disabling multiple interactions via SetMultiple() may speed up the event generation
//           significantly around threshold energies.
//           The value of "fact" is only relevant when frame="free" and win<=0 (see above) are specified.
//           fact=0 --> The provided beam and target 3-momenta are used for intialisation.
//           fact>0 --> The beam 3-momentum is multiplied by the value of "fact" for intialisation.
//           fact<0 --> The target 3-momentum is multiplied by the absolute value of "fact" for initialisation.
//
// By default pbeam=0, ptarget=0, wxsec=1 and fact=0.
//
// Return value : 
// --------------
// In case an error occurred, a value 1 will be returned. Otherwise the return value is 0.
//
// For the beam and target particle specifications the following naming conventions are used :
// 
// Leptons :
// ---------
// "e-", "e+", "mu-", "mu+", "tau-", "tau+", "nue", "numu", "nutau"
//
// Mesons :
// --------
// "pi+", "pi-", "pi0", "K+", "K-", "KS0", "KL0"
//
// Baryons :
// ---------
// "p", "n", "Lambda0", "Sigma+", "Sigma-", "Sigma0", "Xi-", "Xi0", "Omega-"
//
// (Virtual) Photons :
// -------------------
// "gamma"      : Real photon
// "gamma/e-"   : Virtual photon in an "e-" beam;  "win" refers to the "e-" energy;   not possible with frame="free"
// "gamma/e+"   : Virtual photon in an "e+" beam;  "win" refers to the "e+" energy;   not possible with frame="free"
// "gamma/mu-"  : Virtual photon in a "mu-" beam;  "win" refers to the "mu-" energy;  not possible with frame="free" 
// "gamma/mu+"  : Virtual photon in a "mu+" beam;  "win" refers to the "mu+" energy;  not possible with frame="free"
// "gamma/tau-" : Virtual photon in a "tau-" beam; "win" refers to the "tau-" energy; not possible with frame="free"
// "gamma/tau+" : Virtual photon in a "tau+" beam; "win" refers to the "tau+" energy; not possible with frame="free"
//
// Note : Anti-particles are specified by denoting "bar" at the end of the name.
//        So, "pbar" for an anti-proton, "nuebar" for an electron anti-neutrino etc.
//
// Some Pythia default MC parameters are automatically modified to provide
// more suitable running conditions for soft processes in view of
// astrophysical processes.
//
// The optimisations consist of : 
// ------------------------------
// * Usage of real photons for photon beams or targets
// * No minimum CMS energy is required for initialisation (see SetEcmsMin).
// * Activation of the default K factor values
//   with separate settings for ordinary and color annihilation graphs.
// * Setting of the measured mass for the Higgs.
//
// The user may initialise the generator with all the default Pythia
// parameters and obtain full user control to modify the settings by means
// of invoking the SetUserControl memberfunction before this initialisation.
// Note that the inclusion of elastic and diffractive processes is controlled
// by invokation of the SetElastic() memberfunction before this initialisation,
// irrespective of the UserControl selection. The same is true for the disabling
// of multiple interactions via the memberfunction SetMultiple().

 Int_t ier=0;
 if (frame!="cms" && frame!="fixt" && frame!="free") ier=1;
 if (frame!="free" && win<=0) ier=1;
 if (frame=="free" && (!pbeam || !ptarget)) ier=1;
 if (frame=="free" && (beam.Contains("gamma/") || target.Contains("gamma/"))) ier=1;
 if (ier)
 {
  cout << " *NcCollider::Init* Standard Pythia initialisation ==> Inconsistent input." << endl;
  return ier;
 }

 if (!fUserctrl) // Optimisation of some MC parameters
 {
  if (beam=="gamma" || target=="gamma")
  {
   SetMSTP(14,10);      // Real photons for photon beams or targets
  }
  SetPARP(2,0);         // No minimum CMS energy required at initialisation
  SetMSTP(33,2);        // Activate K factor. Separate for ordinary and color annih. graphs
  SetPMAS(25,1,125.09); // Setting the Higgs mass
 }

 fWxsec=0;
 if (frame=="free")
 {
  SetMSTP(171,1);  // Enable variation of beam and/or target momenta
  SetMSTP(82,1);   // Select abrupt pt_min cut-off (see Pythia parameter PARP(81)) for multiple interactions 
  if (wxsec)
  {
   SetMSTP(172,2); // Weight event production by cross section
   fWxsec=1;
  }
  else
  {
   SetMSTP(172,1); // Just always generate an event at the requested energy
  }
 }
 else
 {
  SetMSTP(171,0); // Disable variation of beam and/or target momenta
 }

 if (fElastic) SetMSEL(2); // Include low-Pt, elastic and diffractive events

 if (!fMultiple) // Disable multiple interactions
 {
  SetMSTP(81,0);
  SetMSTP(82,1);
 }

 fEventnum=0;
 fNucl=0;
 fFrame=frame;
 fWin=-1;
 if (win>0) fWin=win;
 Double_t s;
 Double_t ecms;
 fBeam.SetNameTitle(beam.Data(),"Beam");
 fTarget.SetNameTitle(target.Data(),"Target");

 // Set initial beam and target specifications for user defined system via "pbeam" and "ptarget"
 if (fFrame=="free")
 {
  Double_t bmass=0;
  Double_t tmass=0;

  if (beam=="e-" || beam=="e+") bmass=0.510998928e-3;
  if (beam=="mu-" || beam=="mu+") bmass=105.6583715e-3;
  if (beam=="tau-" || beam=="tau+") bmass=1.77686;
  if (beam=="pi+" || beam=="pi-") bmass=139.57018e-3;
  if (beam=="pi0") bmass=134.9766e-3;
  if (beam=="K+" || beam=="K-") bmass=493.677e-3;
  if (beam=="KS0" || beam=="KL0") bmass=497.611e-3;
  if (beam=="p") bmass=938.272046e-3;
  if (beam=="n") bmass=939.565379e-3;
  if (beam=="Lambda0") bmass=1.115683;
  if (beam=="Sigma+") bmass=1.18937;
  if (beam=="Sigma0") bmass=1.192642;
  if (beam=="Sigma-") bmass=1.197449;
  if (beam=="Xi-") bmass=1.32171;
  if (beam=="Xi0") bmass=1.31486;
  if (beam=="Omega-") bmass=1.67245;

  if (target=="e-" || target=="e+") tmass=0.510998928e-3;
  if (target=="mu-" || target=="mu+") tmass=105.6583715e-3;
  if (target=="tau-" || target=="tau+") tmass=1.77686;
  if (target=="pi+" || target=="pi-") tmass=139.57018e-3;
  if (target=="pi0") tmass=134.9766e-3;
  if (target=="K+" || target=="K-") tmass=493.677e-3;
  if (target=="KS0" || target=="KL0") tmass=497.611e-3;
  if (target=="p") tmass=938.272046e-3;
  if (target=="n") tmass=939.565379e-3;
  if (target=="Lambda0") tmass=1.115683;
  if (target=="Sigma+") tmass=1.18937;
  if (target=="Sigma0") tmass=1.192642;
  if (target=="Sigma-") tmass=1.197449;
  if (target=="Xi-") tmass=1.32171;
  if (target=="Xi0") tmass=1.31486;
  if (target=="Omega-") tmass=1.67245;

  fBeam.SetMass(bmass);
  fTarget.SetMass(tmass);

  Nc3Vector vinit;
  if (fact>0 && fWin<0) // Modify beam 3-momentum for initialisation only
  {
   vinit.Load(*pbeam);
   vinit*=fact;
   SetMomentum(vinit,1);
   SetMomentum(*ptarget,2);
   cout << endl;
   cout << " ************************************* NcCollider::Init ************************************" << endl;
   cout << " *** Beam momentum artificially increased for intialisation only. Increase factor : " << fact << endl;
   cout << " *******************************************************************************************" << endl;
   cout << endl;
  }
  else if (fact<0 && fWin<0) // Modify target 3-momentum for initialisation only
  {
   vinit.Load(*ptarget);
   vinit*=fabs(fact);
   SetMomentum(*pbeam,1);
   SetMomentum(vinit,2);
   cout << endl;
   cout << " ************************************** NcCollider::Init *************************************" << endl;
   cout << " *** Target momentum artificially increased for intialisation only. Increase factor : " << fabs(fact) << endl;
   cout << " *********************************************************************************************" << endl;
   cout << endl;
  }
  else // Use the provided beam and target 3-momenta for initialisation
  {
   SetMomentum(*pbeam,1);
   SetMomentum(*ptarget,2);
  }
  frame="3mom"; // Use the Pythia convention for the frame name
  if (win>=0)   // Forced event generation in the CMS
  {
   frame="cms";
   if (!win) // Set CM energy according to "pbeam", "ptarget" and "fact"
   {
    s=fBeam.GetInvariant()+fTarget.GetInvariant()+2.*fBeam.Dot(fTarget);
    ecms=sqrt(s);
    fWin=ecms;
   }
  }
 }

 // Prevent title overwriting by Initialize()
 TString title=GetTitle();

 Initialize(frame.Data(),beam.Data(),target.Data(),fWin);

 SetTitle(title.Data());

 // Use the Pythia beam and target specifications for consistency
 fBeam.SetMass(GetP(1,5));
 fTarget.SetMass(GetP(2,5));

 if (fFrame=="cms")
 {
  fBeam.Set3Vector(GetP(1,1),GetP(1,2),GetP(1,3),"car");
  fTarget.Set3Vector(GetP(2,1),GetP(2,2),GetP(2,3),"car");
 }
 if (fFrame=="fixt")
 {
  fBeam.Set3Vector(0,0,win,"car");
  fTarget.Set3Vector(0,0,0,"car");
 }
 if (fFrame=="free")
 {
  SetMomentum(*pbeam,1);
  SetMomentum(*ptarget,2);
 }

 TString sweight="Yes";
 if (!wxsec) sweight="No";

 s=fBeam.GetInvariant()+fTarget.GetInvariant()+2.*fBeam.Dot(fTarget);
 ecms=sqrt(s);

 cout << endl;
 cout << endl;
 cout << " ********************************************************" << endl;
 cout << " *** NcCollider::Init  Standard Pythia initialisation ***" << endl;
 cout << " ********************************************************" << endl;
 cout << " *** Beam particle: " << beam.Data() << " Target particle: " << target.Data() << " Frame: " << fFrame.Data() << endl;
 if (fFrame=="cms") cout << " *** Total CMS energy: " << win << " GeV" << endl;
 if (fFrame=="fixt") cout << " *** Beam particle momentum: " << win << " GeV/c" << endl;
 if (fFrame=="free") cout << " *** Event weighting by cross section: " << sweight.Data() << endl;
 cout << " *** Beam   particle 3-momentum (GeV/c): px=" << fBeam.GetX(1,"car") << " py=" << fBeam.GetX(2,"car") << " pz=" << fBeam.GetX(3,"car") << endl; 
 cout << " *** Target particle 3-momentum (GeV/c): px=" << fTarget.GetX(1,"car") << " py=" << fTarget.GetX(2,"car") << " pz=" << fTarget.GetX(3,"car") << endl; 
 if (fFrame!="cms") cout << " *** Total CMS energy: " << ecms << " GeV" << endl;
 if (fFrame=="free" && fWin>0) cout << " *** Forced CMS processing. Cross sections initialised for a CMS energy of " << fWin << " GeV" << endl;
 if (fOutFile) cout << " *** Event data will be written to output file: " << fOutFile->GetName() << endl;
 cout << endl;
 cout << endl;

 return ier;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::Init(TString frame,Int_t zp,Int_t ap,Int_t zt,Int_t at,Float_t win,Nc3Vector* pbeam,Nc3Vector* ptarget,Int_t wxsec)
{
// Initialisation of the underlying Pythia generator package for the generation
// of nucleus-nucleus interactions.
// The event number is reset to 0.
//
// Input arguments :
// -----------------
// frame   : "cms"  --> Colliding beam experiment in the nucleon-nucleon CM frame (beam=+z, target=-z direction)
//           "fixt" --> Fixed target experiment with beam in the +z direction
//           "free" --> System defined by the user via "pbeam" and "ptarget"
// zp      : Z value for the (Z,A) projectile nucleus
// ap      : A value for the (Z,A) projectile nucleus
// zt      : Z value for the (Z,A) target nucleus
// at      : A value for the (Z,A) target nucleus
// win     : Energy of the system depending on the frame specification.
//           frame="cms"  --> win is the total cms energy (in GeV) per nucleon-nucleon collision
//           frame="fixt" --> win is the momentum (in GeV/c) per nucleon of the beam 
//           frame="free" --> win>=0 forces event generation in the nucleon-nucleon CMS followed by a Lorentz boost (using NcBoost)
//                            according to the user defined conditions via "pbeam" and "ptarget" (see below).
//                            For win>0 the nucleon-nucleon CM energy (Ecms) for cross section initialisations is set to the value of "win".
//                            For win=0 the Ecms for cross section initialisations is determined on basis of the specified
//                            "pbeam" and "ptarget".
//                            Note that events will always be generated according to the actual settings of "pbeam" and "ptarget".
//                            For very large boosts (e.g. Cosmic Ray physics above 10 PeV/c) the Pythia boost facility yields
//                            incorrect results due to accuracy issues. The more accurate NcBoost facility is used instead.
//                            This implies that for win>=0 the Pythia event data will reflect the situation in the nucleon-nucleon CM frame
//                            and that the event data in the user frame (defined via "pbeam" and "ptarget") is only available
//                            from the produced NcEvent structure (see memberfunction GetEvent).
//                            Due to internal Pythia accuracy issues it turns out that forcing the event generation
//                            in the CMS may also result in faster processing.  
//                            In case win<0 it is a dummy parameter of which the value is irrelevant and the cross section
//                            initialisations are based on the settings of "pbeam" and "ptarget".
// pbeam   : Beam 3-momentum in GeV/c per nucleon   (only to be provided if frame="free").
// ptarget : Target 3-momentum in GeV/c per nucleon (only to be provided if frame="free").
// wxsec   : Cross section weighting (1) or not (0) in the case frame="free".
//           Weighting by cross section is necessary to obtain correct particle spectra when running
//           at different energies within the same generation run via frame="free" and SetMomentum().
//           The most accurate weighting is obtained by invoking this Init() function at the interaction
//           energy with the highest cross section. In general this is just the highest foreseen energy
//           (e.g. hadron-hadron interactions) or at some resonance (e.g. e+e- interactions). 
//           Note that with weighting sometimes no event is generated after invoking MakeEvent()
//           to preserve the correct relative number of events for different energies. 
//           The information whether an event has been generated or not is indicated via the
//           return argument of MakeEvent().
//
// By default pbeam=0, ptarget=0 and wxsec=1.
//
// Return value : 
// --------------
// In case an error occurred, a value 1 will be returned. Otherwise the return value is 0.
//
// Some Pythia default MC parameters are automatically modified to provide
// more suitable running conditions for soft processes in view of
// nucleus-nucleus interactions and astrophysical processes.
//
// The optimisations consist of : 
// ------------------------------
// * No minimum CMS energy is required for initialisation (see SetEcmsMin).
// * Activation of the default K factor values
//   with separate settings for ordinary and color annihilation graphs.
// * Setting of the measured mass for the Higgs.
//
// The user may initialise the generator with all the default Pythia
// parameters and obtain full user control to modify the settings by means
// of invoking the SetUserControl memberfunction before this initialisation.
// Note that the inclusion of elastic and diffractive processes is controlled
// by invokation of the SetElastic memberfunction before this initialisation,
// irrespective of the UserControl selection. The same is true for the disabling
// of multiple interactions via the memberfunction SetMultiple.

 Int_t ier=0;
 if (frame!="cms" && frame!="fixt" && frame!="free") ier=1;
 if (frame!="free" && win<=0) ier=1;
 if (frame=="free" && (!pbeam || !ptarget)) ier=1;
 if (ier)
 {
  cout << " *NcCollider::Init* Nucleus-Nucleus generator initialisation ==> Inconsistent input." << endl;
  return ier;
 }

 if (!fUserctrl) // Optimisation of some MC parameters
 {
  SetPARP(2,0);         // No minimum CMS energy required at initialisation
  SetMSTP(33,2);        // Activate K factor. Separate for ordinary and color annih. graphs
  SetPMAS(25,1,125.09); // Setting the Higgs mass
 }

 fWxsec=0;
 if (frame=="free")
 {
  SetMSTP(171,1);  // Enable variation of beam and/or target momenta
  SetMSTP(82,1);   // Select abrupt pt_min cut-off (see Pythia parameter PARP(81)) for multiple interactions 
  if (wxsec)
  {
   SetMSTP(172,2); // Weight event production by cross section
   fWxsec=1;
  }
  else
  {
   SetMSTP(172,1); // Just always generate an event at the requested energy
  }
 }
 else
 {
  SetMSTP(171,0); // Disable variation of beam and/or target momenta
 }

 if (fElastic) SetMSEL(2); // Include low-Pt, elastic and diffractive events

 if (!fMultiple) // Disable multiple interactions
 {
  SetMSTP(81,0);
  SetMSTP(82,1);
 }

 fEventnum=0;
 fNucl=1;
 fFrame=frame;
 fWin=-1;
 if (win>0) fWin=win;
 fZproj=0;
 fAproj=0;
 fZtarg=0;
 fAtarg=0;
 fFracpp=0;
 fFracnp=0;
 fFracpn=0;
 fFracnn=0;

 if (ap<1 || at<1 || zp>ap || zt>at)
 {
  cout << endl;
  cout << " *NcCollider::Init* Invalid input value(s). Zproj = " << zp
       << " Aproj = " << ap << " Ztarg = " << zt << " Atarg = " << at << endl;
  return 1;
 }

 fZproj=zp;
 fAproj=ap;
 fZtarg=zt;
 fAtarg=at;

 TString beam="(Z=";
 beam+=zp;
 beam+=",A=";
 beam+=ap;
 beam+=")";
 fBeam.SetNameTitle(beam.Data(),"Beam");

 TString target="(Z=";
 target+=zt;
 target+=",A=";
 target+=at;
 target+=")";
 fTarget.SetNameTitle(target.Data(),"Target");

 Float_t mnuc=(0.9382720+0.9395654)/2.;
 fBeam.SetMass(mnuc);
 fTarget.SetMass(mnuc);

 if (fFrame=="cms")
 {
  Float_t pcms=sqrt((win*win/4.)-mnuc*mnuc);
  fBeam.Set3Vector(0,0,pcms,"car");
  fTarget.Set3Vector(0,0,-pcms,"car");
 }
 if (fFrame=="fixt")
 {
  fBeam.Set3Vector(0,0,win,"car");
  fTarget.Set3Vector(0,0,0,"car");
 }
 if (fFrame=="free")
 {
  SetMomentum(*pbeam,1);
  SetMomentum(*ptarget,2);
 }

 TString sweight="Yes";
 if (!wxsec) sweight="No";

 Double_t s=fBeam.GetInvariant()+fTarget.GetInvariant()+2.*fBeam.Dot(fTarget);
 Double_t ecms=sqrt(s);

 // Set CM energy according to "pbeam" and "ptarget" for forced CMS event generation
 if (fFrame=="free" && !win) fWin=ecms;

 cout << endl;
 cout << endl;
 cout << " ******************************************************************" << endl;
 cout << " *** NcCollider::Init  Nucleus-Nucleus generator initialisation ***" << endl;
 cout << " ******************************************************************" << endl;
 cout << " *** Beam nucleus: " << fBeam.GetName() << " Target nucleus: " << fTarget.GetName() << " Frame: " << fFrame.Data() << endl;
 if (fFrame=="cms") cout << " *** Total CMS energy per nucleon-nucleon collision: " << win << " GeV" << endl;
 if (fFrame=="fixt") cout << " *** Beam momentum: " << win << " GeV/c per nucleon" << endl;
 if (fFrame=="free") cout << " *** Event weighting by cross section: " << sweight.Data() << endl;
 cout << " *** Beam   3-momentum in GeV/c per nucleon: px=" << fBeam.GetX(1,"car") << " py=" << fBeam.GetX(2,"car") << " pz=" << fBeam.GetX(3,"car") << endl; 
 cout << " *** Target 3-momentum in GeV/c per nucleon: px=" << fTarget.GetX(1,"car") << " py=" << fTarget.GetX(2,"car") << " pz=" << fTarget.GetX(3,"car") << endl; 
 if (fFrame!="cms") cout << " *** Total CMS energy per nucleon-nucleon collision: " << ecms << " GeV" << endl;
 if (fFrame=="free" && fWin>0) cout << " *** Forced CMS processing. Cross sections initialised for a nucleon-nucleon CMS energy of " << fWin << " GeV" << endl;
 if (fOutFile) cout << " *** Event data will be written to output file: " << fOutFile->GetName() << endl;
 cout << endl;
 cout << endl;

 return ier;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::GetFractions(Float_t zp,Float_t ap,Float_t zt,Float_t at)
{
// Determine the fractions for the various N-N collision processes.
// The various processes are : p+p, n+p, p+n and n+n.
 if (zp<0) zp=0;
 if (zt<0) zt=0;

 fFracpp=0;
 fFracnp=0;
 fFracpn=0;
 fFracnn=0;

 if (ap>0 && at>0)
 {
  fFracpp=(zp/ap)*(zt/at);
  fFracnp=(1.-zp/ap)*(zt/at);
  fFracpn=(zp/ap)*(1.-zt/at);
  fFracnn=(1.-zp/ap)*(1.-zt/at);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetMomentum(Nc3Vector& p,Int_t mode)
{
// Set c.q. modify the beam or target particle 3-momentum in the case frame="free".
// Invokation of this function only has effect after the initialisation via Init(). 
//
// Input argument :
// ----------------
// p : Momentum 3-vector in GeV/c
// mode : 1 ==> Modify the beam particle 3-momentum
//        2 ==> Modify the target particle 3-momentum

 if (mode!=1 && mode!=2)
 {
  cout << " *NcCollider::SetMomentum* Invalid input value. mode = " << mode << endl;
  return;  
 }

 if (fFrame!="free")
 {
  cout << " *NcCollider::SetMomentum* Not valid for frame = " << fFrame.Data() << endl;
  return;  
 }

 if (mode==1) fBeam.Set3Momentum(p);
 if (mode==2) fTarget.Set3Momentum(p);


 if (fWin<0) // Update the internal Pythia beam c.q. target momentum
 {
  SetP(mode,1,p.GetX(1,"car"));
  SetP(mode,2,p.GetX(2,"car"));
  SetP(mode,3,p.GetX(3,"car"));
 }
 else // Update the corresponding Ecms scaling factor in case of forced CMS processing
 {
  Double_t s=fBeam.GetInvariant()+fTarget.GetInvariant()+2.*fBeam.Dot(fTarget);
  Double_t ecms=sqrt(s);
  SetPARP(171,ecms/fWin);
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::MakeEvent(Int_t npt,Int_t mlist,Int_t medit)
{
// Generate one event.
// In case of a nucleus-nucleus interaction, the argument 'npt' denotes
// the total number of participant nucleons.
// So, the range of 'npt' is given by [2,(Abeam+Atarget)].
// Normally also the spectator tracks will be stored into the event structure.
// The spectator tracks have a negative user Id to distinguish them from the
// ordinary generated tracks.
// In case the user has selected the creation of vertex structures, the spectator
// tracks will be linked to the primary vertex.
// However, specification of npt<0 will suppress the storage of spectator tracks.
// In the latter case abs(npt) will be taken as the number of participants.  
// In case of a standard Pythia run for 'elementary' particle interactions,
// the value of npt is totally irrelevant.
//
// The argument 'mlist' denotes the list mode used for Pylist().
// Note : mlist<0 suppresses the invokation of Pylist().
// By default, no listing is produced (i.e. mlist=-1).
//
// The argument 'medit' denotes the edit mode used for Pyedit().
// Note : medit<0 suppresses the invokation of Pyedit().
// By default, only 'stable' final particles are kept (i.e. medit=1). 
//
// In the case of a standard Pythia run concerning 'elementary' particle
// interactions, the projectile and target particle ID's for the created
// event structure are set to the corresponding Pythia KF codes.
// All the A and Z values are in that case set to zero.
// In case of a nucleus-nucleus interaction, the proper A and Z values for 
// the projectile and target particles are set in the event structure.
// However, in this case both particle ID's are set to zero.
//
// Note : Only in case an event passed the selection criteria as specified
//        via SelectEvent(), the event will appear on the output file.
//
// The return argument will indicate that an event has been generated (1) or not (0)
// or that an error occurred (-1).

 fEventnum++; 

 Int_t specmode=1;
 if (npt<0)
 {
  specmode=0;
  npt=abs(npt);
 }

 // Counters for the various (proj,targ) combinations : p+p, n+p, p+n and n+n
 Int_t ncols[4]={0,0,0,0};

 Int_t zp=0;
 Int_t ap=0;
 Int_t zt=0;
 Int_t at=0;

 Int_t ncol=1;

 if (fNucl)
 {
  if (npt<2 || npt>(fAproj+fAtarg))
  {
   cout << " *NcCollider::MakeEvent* Invalid input value. npt = " << npt
        << " Aproj = " << fAproj << " Atarg = " << fAtarg << endl;
   return -1;
  }

  // Determine the number of nucleon-nucleon collisions
  ncol=npt/2;
  if (npt%2 && fRan.Uniform()>0.5) ncol+=1;

  // Determine the number of the various types of N+N interactions
  zp=fZproj;
  ap=fAproj;
  zt=fZtarg;
  at=fAtarg;
  Int_t maxa=2; // Indicator whether proj (1) or target (2) has maximal A left
  if (ap>at) maxa=1;
  Float_t* rans=new Float_t[ncol];
  fRan.Uniform(rans,ncol);
  Float_t rndm=0;
  for (Int_t i=0; i<ncol; i++)
  {
   GetFractions(zp,ap,zt,at);
   rndm=rans[i];
   if (rndm<=fFracpp) // p+p interaction
   {
    ncols[0]++;
    if (maxa==2)
    {
     at--;
     zt--;
    } 
    else
    {
     ap--;
     zp--;
    }
   }
   if (rndm>fFracpp && rndm<=(fFracpp+fFracnp)) // n+p interaction
   {
    ncols[1]++;
    if (maxa==2)
    {
     at--;
     zt--;
    } 
    else
    {
     ap--;
    }
   }
   if (rndm>(fFracpp+fFracnp) && rndm<=(fFracpp+fFracnp+fFracpn)) // p+n interaction
   {
    ncols[2]++;
    if (maxa==2)
    {
     at--;
    } 
    else
    {
     ap--;
     zp--;
    }
   }
   if (rndm>(fFracpp+fFracnp+fFracpn)) // n+n interaction
   {
    ncols[3]++; 
    if (maxa==2)
    {
     at--;
    } 
    else
    {
     ap--;
    }
   }
  }
  delete [] rans;
 }

 TString sweight="Yes";
 if (!fWxsec) sweight="No";

 Double_t s=fBeam.GetInvariant()+fTarget.GetInvariant()+2.*fBeam.Dot(fTarget);
 Double_t ecms=sqrt(s);

 if (!(fEventnum%fPrintfreq))
 {
  cout << " *NcCollider::MakeEvent* Run : " << fRunnum << " Event : " << fEventnum << endl;
  if (fFrame=="free") cout << "  Event weighting by cross section: " << sweight.Data() << endl;

  if (fNucl)
  {
   cout << "  Beam nucleus: " << fBeam.GetName() << " Target nucleus: " << fTarget.GetName() << " Frame: " << fFrame.Data() << endl;
   cout << "  Beam   3-momentum in GeV/c per nucleon: px=" << fBeam.GetX(1,"car") << " py=" << fBeam.GetX(2,"car") << " pz=" << fBeam.GetX(3,"car") << endl; 
   cout << "  Target 3-momentum in GeV/c per nucleon: px=" << fTarget.GetX(1,"car") << " py=" << fTarget.GetX(2,"car") << " pz=" << fTarget.GetX(3,"car") << endl; 
   cout << "  Total CMS energy per nucleon-nucleon collision: " << ecms << " GeV" << endl;
   cout << "  # participants and collisions: npart=" << npt << " ncol=" << ncol 
        << " ncolpp=" << ncols[0] << " ncolnp=" << ncols[1] << " ncolpn=" << ncols[2] << " ncolnn=" << ncols[3] << endl;
  }
  else
  {
   cout << "  Beam particle: " << fBeam.GetName() << " Target particle: " << fTarget.GetName() << " Frame: " << fFrame.Data() << endl;
   cout << "  Beam   particle 3-momentum (GeV/c): px=" << fBeam.GetX(1,"car") << " py=" << fBeam.GetX(2,"car") << " pz=" << fBeam.GetX(3,"car") << endl; 
   cout << "  Target particle 3-momentum (GeV/c): px=" << fTarget.GetX(1,"car") << " py=" << fTarget.GetX(2,"car") << " pz=" << fTarget.GetX(3,"car") << endl; 
   cout << "  Total CMS energy: " << ecms << " GeV" << endl;
  }

  if (ecms<fEcmsmin) cout << "  *** No event generated. Ecms is below the minimal requirement of : " << fEcmsmin << " GeV." << endl;
  cout << endl;
 }

 if (ecms<fEcmsmin) return 0;

 if (!fEvent)
 {
  fEvent=new NcEvent();
  fEvent->SetOwner();
  fEvent->SetName(GetName());
  fEvent->SetTitle(GetTitle());
 }
 else
 {
  fEvent->Reset();
 }
 fEvent->SetRunNumber(fRunnum);
 fEvent->SetEventNumber(fEventnum);

 // Set event title text if not provided by the user
 TString title=GetTitle();
 if (title=="")
 {
  title=fBeam.GetName();
  title+=" on ";
  title+=fTarget.GetName();
  title+=" collision";
  fEvent->SetTitle(title.Data());
 }

 NcTrack t;
 Nc3Vector p;
 NcPosition r,rx;
 Float_t v[3];
 NcVertex vert;
 Nc3Vector pproj,ptarg;
 Nc3Vector v3;
 Nc4Vector v4;
 Double_t ct=0;

 // The Lorentz boost for the case of forced CMS processing
 Nc4Vector ptot;
 ptot.SetInvariant(s);
 p=fBeam.Get3Momentum();
 v3=fTarget.Get3Momentum();
 p+=v3;
 ptot.Set3Vector(p);
 fLorbo.Set4Momentum(ptot);

 if (fVertexmode)
 {
  // Make sure the primary vertex gets correct location and Id=1
  v[0]=0;
  v[1]=0;
  v[2]=0;
  r.SetPosition(v,"car");
  v[0]=fResolution;
  v[1]=fResolution;
  v[2]=fResolution;
  r.SetPositionErrors(v,"car");

  vert.SetId(1);
  vert.SetTrackCopy(0);
  vert.SetVertexCopy(0);
  vert.SetPosition(r);
  fEvent->AddVertex(vert,0);
 }

 Int_t kf=0;
 Float_t charge=0,mass=0;
 TString name;

 Int_t ntypes=4;

 // Singular settings for a normal Pythia elementary particle interation 
 if (!fNucl)
 {
  ntypes=1;
  ncols[0]=1;
 }

 // Generate all the various collisions
 fSelect=0;      // Flag to indicate whether the total event is selected or not 
 Int_t select=0; // Flag to indicate whether the sub-event is selected or not 
 Int_t first=1;  // Flag to indicate the first collision process
 TString frame=fFrame;
 if (fFrame=="free")
 {
  frame="3mom";            // Use the Pythia convention for the frame name
  if (fWin>0) frame="cms"; // Forced event generation in the CMS
 }
 pproj=fBeam.Get3Momentum();
 ptarg=fTarget.Get3Momentum();
 Int_t npart=0,ntk=0;
 Double_t dist=0;
 for (Int_t itype=0; itype<ntypes; itype++)
 {
  if (fNucl)
  {
   if (fFrame=="free")
   {
    SetMomentum(pproj,1);
    SetMomentum(ptarg,2);
   }

   if (itype==0 && ncols[itype]) Initialize(frame.Data(),"p","p",fWin);
   if (itype==1 && ncols[itype]) Initialize(frame.Data(),"n","p",fWin);
   if (itype==2 && ncols[itype]) Initialize(frame.Data(),"p","n",fWin);
   if (itype==3 && ncols[itype]) Initialize(frame.Data(),"n","n",fWin);

   if (fFrame=="free")
   {
    SetMomentum(pproj,1);
    SetMomentum(ptarg,2);
   }
  }

  for (Int_t jcol=0; jcol<ncols[itype]; jcol++)
  {

   GenerateEvent();

   select=IsSelected();
   if (select) fSelect=1;

   if (first) // Store generator parameter information in the event structure
   {
    // Enter generator parameters as a device in the event

    NcDevice params;
    params.SetNameTitle("NcCollider","NcCollider generator parameters");
    params.SetSlotName("Medit",1);
    params.SetSlotName("Vertexmode",2);
    params.SetSlotName("Resolution",3);
    params.SetSlotName("Userctrl",4);
    params.SetSlotName("Elastic",5);
    params.SetSlotName("Multiple",6);
    params.SetSlotName("Wxsec",7);
    params.SetSlotName("Ecms",8);

    params.SetSignal(medit,1);
    params.SetSignal(fVertexmode,2);
    params.SetSignal(fResolution,3);
    params.SetSignal(fUserctrl,4);
    params.SetSignal(fElastic,5);
    params.SetSignal(fMultiple,6);
    params.SetSignal(fWxsec,7);
    params.SetSignal(ecms,8);

    // Store projectile and target information in the event structure
    if (fNucl)
    {
     fEvent->SetProjectile(fAproj,fZproj,pproj);
     fEvent->SetTarget(fAtarg,fZtarg,ptarg);

     params.AddNamedSlot("specmode");
     params.AddNamedSlot("Specpmin");
     params.AddNamedSlot("npart");
     params.AddNamedSlot("ncolpp");
     params.AddNamedSlot("ncolnp");
     params.AddNamedSlot("ncolpn");
     params.AddNamedSlot("ncolnn");

     params.SetSignal(specmode,"specmode");
     params.SetSignal(fSpecpmin,"Specpmin");
     params.SetSignal(npt,"npart");
     params.SetSignal(ncols[0],"ncolpp");
     params.SetSignal(ncols[1],"ncolnp");
     params.SetSignal(ncols[2],"ncolpn");
     params.SetSignal(ncols[3],"ncolnn");
    }
    else
    {
     kf=GetK(1,2);
     fEvent->SetProjectile(0,0,pproj,kf);
     kf=GetK(2,2);
     fEvent->SetTarget(0,0,ptarg,kf);
    }

    fEvent->AddDevice(params);

    first=0;
   }

   if (medit >= 0) Pyedit(medit); // Define which particles are to be kept

   if (mlist>=0 && select)
   {
    Pylist(mlist);
    cout << endl;
   }

   npart=GetN();
   for (Int_t jpart=1; jpart<=npart; jpart++)
   {
    kf=GetK(jpart,2);
    charge=Pychge(kf)/3.;
    mass=GetP(jpart,5);
    name=GetPyname(kf);

    // 3-momentum in GeV/c
    v[0]=GetP(jpart,1);
    v[1]=GetP(jpart,2);
    v[2]=GetP(jpart,3);
    p.SetVector(v,"car");

    // Production location in meter.
    v[0]=GetV(jpart,1)/1000.;
    v[1]=GetV(jpart,2)/1000.;
    v[2]=GetV(jpart,3)/1000.;
    r.SetPosition(v,"car");

    ct=GetV(jpart,4)/1000.;

    // Boost the track momentum and vertex location into the user defined frame in case of forced CMS processing
    if (fFrame=="free" && fWin>0)
    {
     // Momentum boost
     v4.SetInvariant(mass*mass);
     v4.Set3Vector(p);
     v4=fLorbo.Inverse(v4);
     p=v4.Get3Vector();

     // Vertex location boost
     v4.SetInvariant(pow(ct,2)-r.Dot(r));
     v4.Set3Vector(r);
     v4=fLorbo.Inverse(v4);
     v3=v4.Get3Vector();
     r.SetPosition(v3);
    }    

    ntk++;

    t.Reset();
    t.SetId(ntk);
    t.SetParticleCode(kf);
    t.SetName(name.Data());
    t.SetCharge(charge);
    t.SetMass(mass);
    t.Set3Momentum(p);
    t.SetBeginPoint(r);

    fEvent->AddTrack(t);

    // Build the vertex structures if requested
    if (fVertexmode)
    {
     // Check if track belongs within the resolution to an existing vertex
     Int_t add=0;  
     for (Int_t jv=1; jv<=fEvent->GetNvertices(); jv++)
     {
      NcVertex* vx=fEvent->GetVertex(jv);
      if (vx)
      {
       rx=vx->GetPosition();
       dist=rx.GetDistance(r);
       if (dist < fResolution)
       {
        NcTrack* tx=fEvent->GetIdTrack(ntk);
        if (tx)
        { 
         vx->AddTrack(tx);
         add=1;
        }
       }
      }
      if (add) break; // No need to look further for vertex candidates
     }

     // If track was not close enough to an existing vertex
     // a new secondary vertex is created      
     if (!add && fVertexmode>1)
     {
      NcTrack* tx=fEvent->GetIdTrack(ntk);
      if (tx)
      {
       v[0]=fResolution;
       v[1]=fResolution;
       v[2]=fResolution;
       r.SetPositionErrors(v,"car");
       vert.Reset();
       vert.SetTrackCopy(0);
       vert.SetVertexCopy(0);
       vert.SetId((fEvent->GetNvertices())+1);
       vert.SetPosition(r);
       vert.AddTrack(tx);
       fEvent->AddVertex(vert,0);
      } 
     }
    }
   } // End of loop over the produced particles for each collision
  } // End of loop over number of collisions for each type
 } // End of loop over collision types

 // Link sec. vertices to primary if requested
 // Note that also the connecting tracks are automatically created
 if (fVertexmode>2)
 {
  NcVertex* vp=fEvent->GetIdVertex(1); // Primary vertex
  if (vp)
  {
   for (Int_t i=2; i<=fEvent->GetNvertices(); i++)
   {
    NcVertex* vx=fEvent->GetVertex(i);
    if (vx)
    {
     if (vx->GetId() != 1) vp->AddVertex(vx);
    }
   }
  }
 }

 // Include the spectator tracks in the event structure.
 if (fNucl && specmode)
 {
  v[0]=0;
  v[1]=0;
  v[2]=0;
  r.SetPosition(v,"car");

  zp=fZproj-(ncols[0]+ncols[2]);
  if (zp<0) zp=0;
  ap=fAproj-(ncols[0]+ncols[1]+ncols[2]+ncols[3]);
  if (ap<0) ap=0;
  zt=fZtarg-(ncols[0]+ncols[1]);
  if (zt<0) zt=0;
  at=fAtarg-(ncols[0]+ncols[1]+ncols[2]+ncols[3]);
  if (at<0) at=0;

  Int_t nspec=0;

  if (pproj.GetNorm() > fSpecpmin)
  {
   kf=2212; // Projectile spectator protons
   charge=Pychge(kf)/3.;
   mass=GetPMAS(Pycomp(kf),1);
   name=GetPyname(kf);
   for (Int_t iprojp=1; iprojp<=zp; iprojp++)
   {
    nspec++;
    t.Reset();
    t.SetId(-nspec);
    t.SetParticleCode(kf);
    t.SetName(name.Data());
    t.SetTitle("Projectile spectator proton");
    t.SetCharge(charge);
    t.SetMass(mass);
    t.Set3Momentum(pproj);
    t.SetBeginPoint(r);

    fEvent->AddTrack(t);
   }

   kf=2112; // Projectile spectator neutrons
   charge=Pychge(kf)/3.;
   mass=GetPMAS(Pycomp(kf),1);
   name=GetPyname(kf);
   for (Int_t iprojn=1; iprojn<=(ap-zp); iprojn++)
   {
    nspec++;
    t.Reset();
    t.SetId(-nspec);
    t.SetParticleCode(kf);
    t.SetName(name.Data());
    t.SetTitle("Projectile spectator neutron");
    t.SetCharge(charge);
    t.SetMass(mass);
    t.Set3Momentum(pproj);
    t.SetBeginPoint(r);

    fEvent->AddTrack(t);
   }
  }

  if (ptarg.GetNorm() > fSpecpmin)
  {
   kf=2212; // Target spectator protons
   charge=Pychge(kf)/3.;
   mass=GetPMAS(Pycomp(kf),1);
   name=GetPyname(kf);
   for (Int_t itargp=1; itargp<=zt; itargp++)
   {
    nspec++;
    t.Reset();
    t.SetId(-nspec);
    t.SetParticleCode(kf);
    t.SetName(name.Data());
    t.SetTitle("Target spectator proton");
    t.SetCharge(charge);
    t.SetMass(mass);
    t.Set3Momentum(ptarg);
    t.SetBeginPoint(r);

    fEvent->AddTrack(t);
   }

   kf=2112; // Target spectator neutrons
   charge=Pychge(kf)/3.;
   mass=GetPMAS(Pycomp(kf),1);
   name=GetPyname(kf);
   for (Int_t itargn=1; itargn<=(at-zt); itargn++)
   {
    nspec++;
    t.Reset();
    t.SetId(-nspec);
    t.SetParticleCode(kf);
    t.SetName(name.Data());
    t.SetTitle("Target spectator neutron");
    t.SetCharge(charge);
    t.SetMass(mass);
    t.Set3Momentum(ptarg);
    t.SetBeginPoint(r);

    fEvent->AddTrack(t);
   }
  }

  // Link the spectator tracks to the primary vertex.
  if (fVertexmode)
  {
   NcVertex* vp=fEvent->GetIdVertex(1);
   if (vp)
   {
    for (Int_t ispec=1; ispec<=nspec; ispec++)
    {
     NcTrack* tx=fEvent->GetIdTrack(-ispec);
     if (tx) vp->AddTrack(tx);
    }
   }
  }
 }

 if (!(fEventnum%fPrintfreq) && (mlist || fEvent))
 {
  if (fEvent)
  {
   cout << " Number of tracks in the event structure : "
        << fEvent->GetNtracks() << endl;
  }
  cout << endl; // Create empty output line after the event
 }

 if (fOutTree && fSelect) fOutTree->Fill();

 return fSelect;
}
///////////////////////////////////////////////////////////////////////////
NcEvent* NcCollider::GetEvent(Int_t select) const
{
// Provide pointer to the generated event structure.
//
// select = 0 : Always return the pointer to the generated event.
//          1 : Only return the pointer to the generated event in case
//              the event passed the selection criteria as specified via
//              SelectEvent(). Otherwise the value 0 will be returned.
//
// By invoking GetEvent() the default of select=0 will be used.

 if (!select || fSelect)
 {
  return fEvent;
 }
 else
 {
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::EndRun()
{
// Properly flush last data to the output file and close it (if needed).
 // This will also delete the existing output tree connected to this file
 if (fOutFile)
 {
  if (fOutFile->IsOpen())
  {
   fOutFile->Write();
   fOutFile->Close();
  }
  TString name=fOutFile->GetName();
  cout << " *NcCollider::EndRun* Output file " << name << " correctly written and closed." << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetStable(Int_t id,Int_t mode,Int_t cls)
{
// Declare whether a particle (class) must be regarded as stable or not.
//
// Input arguments :
// -----------------
// id    = The Pythia KF particle code, which basically is the PDG particle identifier code.
//       = 0 ==> Will invoke the setting for a whole particle class (see below).  
// mode  = 1 : The specified particle (class) will be regarded as stable.
//         0 : The specified particle (class) will be able to decay.
// cls   = 1 : Quarks
//         2 : Leptons
//         3 : Gauge Bosons (including photons and the Higgs)
//         4 : Mesons
//         5 : Baryons
//
// In case the user does NOT explicitly invoke this function, the standard
// Pythia settings for the decay tables are used.
//
// Default values are mode=1 and cls=0.
//
// Notes :
// -------
// 1) This function should be invoked after the initialisation call
//    to NcCollider::Init.
// 2) Due to the internals of Pythia, there is no need to specify particles
//    and their corresponding anti-particles separately as (un)stable.
//    Once a particle has been declared (un)stable, the corresponding 
//    anti-particle will be treated in the same way.
// 3) After declaring a whole particle class stable/unstable, specific particles
//    can be (re)declared unstable/stable by invoking this function again
//    for the specific particle(s).

 if (mode==0 || mode==1)
 {
  Int_t kc=0;
  Int_t decay=1-mode;
  
  // Introduce additional c.q. missing decay channels
  Int_t idc=7000;   // Start entry point in the decay table for the new decay processes
  SetPARJ(64,5e-4); // Reduce the allowed minimal mass difference in decays

  // Specification of neutron decay data
  Double_t ctau=2.6391e14; // Average ctau in mm
  kc=Pycomp(2112);         // kc code for (anti)neutrons
  SetPMAS(kc,4,ctau);      // Set average lifetime via ctau in mm
  SetMDCY(kc,2,idc);       // Set the idc entry point for this particle decay
  SetMDCY(kc,3,1);         // Number of decay modes
  SetMDME(idc,1,1);        // Activate this decay channel
  SetMDME(idc,2,0);        // Use a normal decay matrix element for this decay channel
  SetBRAT(idc,1);          // Set branching ratio for this decay channel
  SetKFDP(idc,1,2212);     // The "p" decay product
  SetKFDP(idc,2,11);       // The "e-" decay product
  SetKFDP(idc,3,-12);      // The "nue_bar" decay product

  if (!id) // Settings for a class of particles
  {
   if (cls>0 && cls<6)
   {
    for (Int_t i=1; i<1000; i++)
    {
     kc=Pycomp(i);
     if (kc>0)
     {
      if (cls==1 && i<10) SetMDCY(kc,1,decay);
      if (cls==2 && i>10 && i<20) SetMDCY(kc,1,decay);
      if (cls==3 && i>20 && i<26) SetMDCY(kc,1,decay);
      if (cls==4 && i>100 && i<1000) SetMDCY(kc,1,decay);
      if (cls==5 && i>1000) SetMDCY(kc,1,decay);
     }
    }
    // Special entries for Psi' and Ypsilon'
    if (cls==4)
    {
     kc=Pycomp(100443); 
     if (kc>0) SetMDCY(kc,1,decay);
     kc=Pycomp(100553); 
     if (kc>0) SetMDCY(kc,1,decay);
    }
   }
   else
   {
    cout << " *NcCollider::SetStable* Invalid parameter. cls = " << cls << endl;
   }
  }
  else // Settings for individual particles
  {
   kc=Pycomp(id);
   if (kc>0)
   {
    SetMDCY(kc,1,decay);
   }
   else 
   {
    cout << " *NcCollider::SetStable* Unknown particle code. id = " << id << endl;
   }
  }
 } 
 else
 {
  cout << " *NcCollider::SetStable* Invalid parameter. mode = " << mode << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SelectEvent(Int_t id)
{
// Add a particle to the event selection list.
// The parameter "id" indicates the Pythia KF particle code, which
// basically is the PDG particle identifier code.
// In case the user has built a selection list via this procedure, only the
// events in which one of the particles specified in the list was generated
// will be kept. 
// The investigation of the generated particles takes place when the complete
// event is in memory, including all (shortlived) mother particles and resonances.
// So, the settings of the various particle decay modes have no influence on
// the event selection described here.
//
// If no list has been specified, all events will be accepted.  
//
// Note : id=0 will delete the selection list.
//
// Be aware of the fact that severe selection criteria (i.e. selecting only
// rare events) may result in long runtimes before an event sample has been
// obtained.
//
 if (!id)
 {
  if (fSelections)
  {
   delete fSelections;
   fSelections=0;
  }
 }
 else
 {
  Int_t kc=Pycomp(id);
  if (!fSelections)
  {
   fSelections=new TArrayI(1);
   fSelections->AddAt(kc,0);
  }
  else
  {
   Int_t exist=0;
   Int_t size=fSelections->GetSize();
   for (Int_t i=0; i<size; i++)
   {
    if (kc==fSelections->At(i))
    {
     exist=1;
     break;
    }
   }
  
   if (!exist)
   {
    fSelections->Set(size+1);
    fSelections->AddAt(kc,size);
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::GetSelectionFlag() const
{
// Return the value of the selection flag for the total event.
// When the event passed the selection criteria as specified via
// SelectEvent() the value 1 is returned, otherwise the value 0 is returned.
 return fSelect;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCollider::IsSelected()
{
// Check whether the generated (sub)event contains one of the particles
// specified in the selection list via SelectEvent().
// If this is the case or when no selection list is present, the value 1
// will be returned, indicating the event is selected to be kept.
// Otherwise the value 0 will be returned.
//
// Note : In case no event was generated (due to possible kinematical constraints),
//        also a value 0 will be returned.

 if (GetMSTI(61)) return 0;

 if (!fSelections) return 1; 

 Int_t nsel=fSelections->GetSize();
 Int_t npart=GetN();
 Int_t kf,kc;

 Int_t select=0;
 for (Int_t jpart=1; jpart<=npart; jpart++)
 {
  kf=GetK(jpart,2);
  kc=Pycomp(kf);
  for (Int_t i=0; i<nsel; i++)
  {
   if (kc==fSelections->At(i))
   {
    select=1;
    break;
   }
  }
  if (select) break;
 }
 return select;
}
///////////////////////////////////////////////////////////////////////////
void NcCollider::SetSpectatorPmin(Float_t pmin)
{
// Set minimal momentum in GeV/c for spectator tracks to be stored.
// Spectator tracks with a momentum below this threshold will not be stored
// in the (output) event structure.
// This facility allows to minimise the output file size.
// Note that when the user wants to boost the event into another reference
// frame these spectator tracks might have got momenta above the threshold.
// However, when the spectator tracks were not stored in the event structure
// in the original frame, there is no way to retreive them anymore. 
 fSpecpmin=pmin;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcCollider::GetSpectatorPmin() const
{
// Provide the minimal spectator momentum in GeV/c.
 return fSpecpmin;
}
///////////////////////////////////////////////////////////////////////////
TString NcCollider::GetPyname(Int_t kf)
{
// Provide the correctly truncated Pythia particle name for PGD code kf
//
// The TPythia6::Pyname returned name is copied into a TString and truncated
// at the first blank to prevent funny trailing characters due to incorrect
// stripping of empty characters in TPythia6::Pyname.
// The truncation at the first blank is allowed due to the Pythia convention
// that particle names never contain blanks.
 char name[16];
 TString sname;
 Pyname(kf,name);
 sname=name[0];
 for (Int_t i=1; i<16; i++)
 {
  if (name[i]==' ') break;
  sname=sname+name[i];
 }
 return sname;
}
///////////////////////////////////////////////////////////////////////////
