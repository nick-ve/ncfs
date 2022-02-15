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

///////////////////////////////////////////////////////////////////////////
// Class NcTreeMaker
// TTask based processor to convert NCFS generic event structures into a plain
// ROOT tree structure.
//
// The selection of the specific observables to appear in the output tree
// is performed via the Select memberfunction.
// The user may define a restricted set of tracks to be processed via the
// memberfunction UseTracks.  
// The user may define a restricted set of devices to be processed via the
// memberfunction UseDevices.
//
// By making use of setting the so called "user data words" of tracks and events
// the user can have full flexibility on the data to appear in the output tree.
// The Select memberfunction allows to specify also which "user data words" one wants
// to obtain in the output tree for tracks and/or events.  
//
// Note : Tracks without vector information will be skipped.
// 
// An NcAstrolab object may be specified via the memberfunction SetAstrolab.
// The available (astrophysical) reference objects will lead to so called "astrotracks".
// The astrotrack directions correspond to hypothetical tracks of messengers arriving
// from the (astrophysical) reference object.
// These directions (in radians) are provided in the local lab coordinate system from the
// location where the (astrophysical) phenomenon would be at the time of the current event. 
// The timestamps of the astrotracks correspond to the timestamps as stored for the
// reference objects in the NcAstrolab, which in general is the actual timestamp
// of the astrophysical observation.
// The latter allows investigation of a possible time difference between the observation
// of an (astrophysical) phenomenon and the regular detected tracks/signals in the detector. 
// Also the ICRS longitude (l) and latitude (b) of the (astrophysical) reference objects
// are provided in radians.
//
// Example :
// ---------
// gSystem->Load("ncfspack");
//
// NcTreeMaker mktree;
// mktree.SetOutputFile("gentest.root","NcCollider data in plain ROOT tree format");
//
// mktree.UseTracks("e-");
// mktree.UseTracks("e+");
// mktree.UseTracks("pi-");
// mktree.UseTracks("pi+");
// mktree.UseTracks("K-");
// mktree.UseTracks("K+");
//
// mktree.Select("track","p");
// mktree.Select("track","e");
// mktree.Select("track","m");
// mktree.Select("track","q");
// mktree.Select("track","user","length");
//
// mktree.Select("event","*");
//
// Int_t nevents=100; // Number of events to be generated
// Int_t jrun=1;      // The run number of this batch of generated events
//
// NcCollider gen;
// gen.SetRunNumber(jrun);
// gen.SetPrintFreq(10);
// gen.Init("cms","p","pbar",2000);
//
// NcJob q;
// q.Add(&mktree);
//
// NcEvent* evt=0;
// for (Int_t i=0; i<nevents; i++)
// {
//  gen.MakeEvent();
//  evt=gen.GetEvent();
//
//  q.ProcessObject(evt);
// }
//
// mktree.CloseTree();
//
//--- Author: Nick van Eijndhoven 03-apr-2008 Utrecht University
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, February 15, 2022  14:33Z
///////////////////////////////////////////////////////////////////////////
 
#include "NcTreeMaker.h"
#include "Riostream.h"

ClassImp(NcTreeMaker) // Class implementation to enable ROOT I/O

NcTreeMaker::NcTreeMaker(const char* name,const char* title) : TTask(name,title)
{
// Default constructor.

 fFirst=1;
 fOutfile=0;
 fOuttree=0;
 fTrees=0;
 fNtkmax=1000;
 fEvt=0;
 fAstrolab=0;
 fTrackflag=0;
 fEventflag=0;
 fDeviceflag=0;
 fUseTracks=0;
 fUseNtk=0;
 fUseTkMode=0;
 fTrackMomenta=0;
 fTrackEnergies=0;
 fTrackMasses=0;
 fTrackCharges=0;
 fTrackMjds=0;
 fTrackSecs=0;
 fTrackIds=0;
 fTrackNsigs=0;
 fTrackUnames=0;
 fTn=0;
 fTp=0;
 fTthetap=0;
 fTphip=0;
 fTe=0;
 fTm=0;
 fTq=0;
 fTmjd=0;
 fTsec=0;
 fTid=0;
 fTnsig=0;
 fTuser=0;
 fEventMomenta=0;
 fEventEnergies=0;
 fEventMasses=0;
 fEventCharges=0;
 fEventJruns=0;
 fEventJevts=0;
 fEventWeights=0;
 fEventMjds=0;
 fEventSecs=0;
 fEventUnames=0;
 fEjrun=0;
 fEjevt=0;
 fEmjd=0;
 fEsec=0;
 fEp=0;
 fEthetap=0;
 fEphip=0;
 fEe=0;
 fEm=0;
 fEq=0;
 fEuser=0;
 fUseDevices=0;
 fDeviceNdevs=0;
 fDeviceNhits=0;
 fDndev=0;
 fDnhit=0;
}
///////////////////////////////////////////////////////////////////////////
NcTreeMaker::~NcTreeMaker()
{
// Default destructor.

 if (fOutfile)
 {
  delete fOutfile;
  fOutfile=0;
 }

 if (fOuttree)
 {
  delete fOuttree;
  fOuttree=0;
 }

 if (fTrees)
 {
  delete fTrees;
  fTrees=0;
 }

 if (fAstrolab)
 {
  delete fAstrolab;
  fAstrolab=0;
 }

 if (fUseTracks)
 {
  delete fUseTracks;
  fUseTracks=0;
 }
 if (fUseNtk)
 {
  delete fUseNtk;
  fUseNtk=0;
 }
 if (fUseTkMode)
 {
  delete fUseTkMode;
  fUseTkMode=0;
 }

 if (fTp)
 {
  delete[] fTp;
  fTp=0;
 }
 if (fTthetap)
 {
  delete[] fTthetap;
  fTthetap=0;
 }
 if (fTphip)
 {
  delete[] fTphip;
  fTphip=0;
 }
 if (fTe)
 {
  delete[] fTe;
  fTe=0;
 }
 if (fTm)
 {
  delete[] fTm;
  fTm=0;
 }
 if (fTq)
 {
  delete[] fTq;
  fTq=0;
 }
 if (fTmjd)
 {
  delete[] fTmjd;
  fTmjd=0;
 }
 if (fTsec)
 {
  delete[] fTsec;
  fTsec=0;
 }
 if (fTid)
 {
  delete[] fTid;
  fTid=0;
 }
 if (fTnsig)
 {
  delete[] fTnsig;
  fTnsig=0;
 }
 if (fTuser)
 {
  for (Int_t i=0; i<fTrackUnames->GetEntries(); i++)
  {
   delete[] fTuser[i];
  }
  delete[] fTuser;
  fTuser=0;
 }
 if (fEuser)
 {
  delete[] fEuser;
  fEuser=0;
 }

 if (fTrackUnames)
 {
  delete fTrackUnames;
  fTrackUnames=0;
 }
 if (fEventUnames)
 {
  delete fEventUnames;
  fEventUnames=0;
 }

 if (fUseDevices)
 {
  delete fUseDevices;
  fUseDevices=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcTreeMaker::NcTreeMaker(const NcTreeMaker& q) : TTask(q)
{
// Copy constructor

 fFirst=1;
 fOutfile=0;
 fOuttree=0;
 fTrees=0;
 fEvt=0;

 fAstrolab=0;
 if (q.fAstrolab) SetAstrolab(q.fAstrolab);

 fUseTracks=0;
 fUseNtk=0;
 fUseTkMode=0;
 Int_t n=0;
 Int_t mode=0;
 if (q.fUseTracks)
 {
  for (Int_t i=0; i<q.fUseTracks->GetSize(); i++)
  {
   TObjString* s=(TObjString*)q.fUseTracks->At(i);
   if (s)
   {
    n=q.fUseNtk->At(i);
    mode=q.fUseTkMode->At(i);
    UseTracks(s->GetString(),n,mode);
   }
  }
 }

 fUseDevices=0;
 if (q.fUseDevices)
 {
  for (Int_t j=0; j<q.fUseDevices->GetSize(); j++)
  {
   TObjString* s=(TObjString*)q.fUseDevices->At(j);
   if (s) UseDevices(s->GetString());
  }
 }

 fNtkmax=q.fNtkmax;
 fTrackflag=q.fTrackflag;
 fEventflag=q.fEventflag;
 fDeviceflag=q.fDeviceflag;
 fTrackMomenta=q.fTrackMomenta;
 fTrackEnergies=q.fTrackEnergies;
 fTrackMasses=q.fTrackMasses;
 fTrackCharges=q.fTrackCharges;
 fTrackMjds=q.fTrackMjds;
 fTrackSecs=q.fTrackSecs;
 fTrackIds=q.fTrackIds;
 fTrackNsigs=q.fTrackNsigs;
 fTrackUnames=0;
 if (q.fTrackUnames)
 {
  for (Int_t k=0; k<q.fTrackUnames->GetSize(); k++)
  {
   TObjString* s=(TObjString*)q.fTrackUnames->At(k);
   if (s) Select("track","user",s->GetString());
  }
 }
 fTn=0;
 fTp=0;
 fTthetap=0;
 fTphip=0;
 fTe=0;
 fTm=0;
 fTq=0;
 fTmjd=0;
 fTsec=0;
 fTid=0;
 fTnsig=0;
 fTuser=0;
 fEventMomenta=q.fEventMomenta;
 fEventEnergies=q.fEventEnergies;
 fEventMasses=q.fEventMasses;
 fEventCharges=q.fEventCharges;
 fEventJruns=q.fEventJruns;
 fEventJevts=q.fEventJevts;
 fEventWeights=q.fEventWeights;
 fEventMjds=q.fEventMjds;
 fEventSecs=q.fEventSecs;
 fEventUnames=0;
 if (q.fEventUnames)
 {
  for (Int_t l=0; l<q.fEventUnames->GetSize(); l++)
  {
   TObjString* s=(TObjString*)q.fEventUnames->At(l);
   if (s) Select("event","user",s->GetString());
  }
 }
 fEjrun=0;
 fEjevt=0;
 fEweight=0;
 fEmjd=0;
 fEsec=0;
 fEp=0;
 fEthetap=0;
 fEphip=0;
 fEe=0;
 fEm=0;
 fEq=0;
 fEuser=0;
 fDeviceNdevs=q.fDeviceNdevs;
 fDeviceNhits=q.fDeviceNhits;
 fDndev=0;
 fDnhit=0;
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::SetOutputFile(TString fname,TString ftitle,TString tname,TString ttitle)
{
// Create the output file for the ROOT data tree.
// Also the overall output tree is created.
//
// fname  : Name of the output file to be created.
// ftitle : Title of the output file to be created.
// tname  : Name of the overall output tree to be created.
// ttitle : Title of the overall output tree to be created.
//
// Environment variables may be used as $(...) in the filenname for convenience.
// For example "$(HOME)/my-data/sample.root".
//
// By default tname="Data" and ttitle="All event data".

 // Expand the path name of the specified output file
 fname=gSystem->ExpandPathName(fname.Data());

 if (fOutfile) delete fOutfile;
 fOutfile=new TFile(fname.Data(),"RECREATE",ftitle.Data());
 if (fOuttree) delete fOuttree;
 fOuttree=new TTree(tname.Data(),ttitle.Data());
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::CloseTree()
{
// Proper writing and closing of the output tree structure(s).

 fOutfile->cd();

 Int_t ntrees=0;
 if (fTrees) ntrees=fTrees->GetEntries();
 TTree* tree=0;
 for (Int_t i=0; i<ntrees; i++)
 {
  tree=(TTree*)fTrees->At(i);
  if (tree) tree->Write();
 }
 fOuttree->Write();
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::SetTrackMaximum(Int_t n)
{
// Set maximum number of tracks per name in the output tree.
// In the constructor n is set to a default of 1000, which implies that each
// trackname can occur at maximum 1000 times in the output tree storage.
// For instance this means that the momenta etc... of "pi+" can be stored
// up to a maximum of 1000 "pi+" tracks in case "pi+" was selected for the
// output tree via the invokation of UseTracks("pi+").
//
// Note : In case all tracks are selected via invokation of UseTracks("*")
//        then the generic name "trk" will be given to each track in the
//        produced output tree. 
//        This implies that the argument "n" (or the default 1000) defines
//        the total number of tracks per event that can occur in the produced
//        output tree. 

 fNtkmax=n;
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::UseTracks(TString name,Int_t n,Int_t mode)
{
// Specification of the track names to be used for the investigation
// of individual track observables.
//
// name : Specifies the track name (e.g. "IceDwalk")
//        In case name="*" all track names will be accepted.
//
// n : Specifies the max. number of these tracks to be used
//
// mode = 0 : The provided name should exactly match the trackname
//        1 : The provided name should be part of the trackname 
//
// Notes :
// -------
// 1) n<0 will use all the existing tracks of the specified name.
//    See the remarks in the docs of memberfunction SetTrackMaximum
//    concerning the maximum number of tracks that can appear in the
//    produced output tree. 
//
// 2) The provided name (specifically with mode=1) should be unique among
//    all the provided names, since these names will define the various
//    variable names in the output tree.  
//
// The defaults are n=-1 and mode=0.
//
// Consecutive invokations of this memberfunction with different names
// will result in an incremental effect.
//
// Example :
// ---------
// UseTracks("IceDwalk",5);
// UseTracks("IceLinefit",2);
// UseTracks("Pythia");
//
// This will use the first 5 IceDwalk, the first 2 IceLinefit and all the
// Pythia tracks which are encountered in the event structure.

 if (!fUseTracks)
 {
  fUseTracks=new TObjArray();
  fUseTracks->SetOwner();
 }
 
 if (!fUseNtk) fUseNtk=new TArrayI();
 
 if (!fUseTkMode) fUseTkMode=new TArrayI();

 // Check if this classname has already been specified before 
 TString s;
 Int_t nen=fUseTracks->GetEntries();
 for (Int_t i=0; i<nen; i++)
 {
  TObjString* sx=(TObjString*)fUseTracks->At(i);
  if (!sx) continue;
  s=sx->GetString();
  if (s==name) return;
 }

 // New name to be added into the storage
 if (nen >= fUseTracks->GetSize()) fUseTracks->Expand(nen+1);
 if (nen >= fUseNtk->GetSize()) fUseNtk->Set(nen+1);
 if (nen >= fUseTkMode->GetSize()) fUseTkMode->Set(nen+1);
 
 TObjString* namex=new TObjString();
 namex->SetString(name);
 fUseTracks->Add(namex);
 fUseNtk->AddAt(n,nen);
 fUseTkMode->AddAt(mode,nen);
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::UseDevices(TString classname)
{
// Specification of the device classes to be used for the various observables.
//
// classname : Specifies the device class (e.g. "IceGOM").
//
// All devices of (or derived from) the specified classname will be taken
// into account as well as all the hits which belong to them. 
//
// Note : Specification of classname="TObject" will result in using
//        all existing devices (and recorded hits).
//
// Consecutive invokations of this memberfunction with different names
// will result in an incremental effect.
//
// Example :
// ---------
// UseDevices("IceAOM);
// UseDevices("IceIDOM);
// UseDevices("IceTDOM");
//
// This will take into account all devices (derived from) IceAOM, IceIDOM
// and IceTDOM as well as all the hits of these devices.

 if (!fUseDevices)
 {
  fUseDevices=new TObjArray();
  fUseDevices->SetOwner();
 }
 
 // Check if this classname has already been specified before 
 TString s;
 Int_t nen=fUseDevices->GetEntries();
 for (Int_t i=0; i<nen; i++)
 {
  TObjString* sx=(TObjString*)fUseDevices->At(i);
  if (!sx) continue;
  s=sx->GetString();
  if (s==classname) return;
 }

 // New classname to be added into the storage
 if (nen >= fUseDevices->GetSize()) fUseDevices->Expand(nen+1);
 
 TObjString* namex=new TObjString();
 namex->SetString(classname);
 fUseDevices->Add(namex);
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::Select(TString type,TString obs,TString uname)
{
// Select the observable to appear in the output tree.
//
// type  : Selection type specifier (e.g. "track", "event" or "device").
// obs   : Observable specification.
// uname : Name of the user data word to be stored (only for "track" or "event")
//
// The various observables that are available for selection are :
//
// obs : "p"      ==> Momentum in spherical coordinates (p,theta,phi) in GeV/c and radians
//       "e"      ==> Energy value in GeV
//       "m"      ==> (Invariant) mass in GeV/c^2
//       "q"      ==> Charge (electron charge is defined as -1)
//       "mjd"    ==> (Fractional) Modified Julian Date
//       "sec"    ==> (Fractional) Number of elapsed seconds in the MJD
//       "id"     ==> Track identifier (for tracks only)
//       "nsig"   ==> Number of associated signals (for tracks only)
//                    Note : Only signals of selected devices (see UseDevices) will be considered 
//       "jrun"   ==> Run number (for event only)
//       "jevt"   ==> Event number (for event only)
//       "weight" ==> Event weight (for event only)
//       "ndev"   ==> Number of fired devices (for device only)
//       "nhit"   ==> Number of hits (for device only)
//       "user"   ==> User data words (for tracks and events only)
//       "*"      ==> All possible observables (except user data words) for the specified selection type
//
// Note :
// ------
// A track or event can have various user data words.
// These user data words can be specified here one by one through subsequent invokations
// of this member function with the observable name "user" and the required user data word name "uname".
//
// In the default constructor all observables are de-activated for selection.

 TString s;
 Int_t nen;

 if (type=="track") // Track observables
 {
  fTrackflag=1;
  if (obs=="p" || obs=="*") fTrackMomenta=1;
  if (obs=="e" || obs=="*") fTrackEnergies=1;
  if (obs=="m" || obs=="*") fTrackMasses=1;
  if (obs=="q" || obs=="*") fTrackCharges=1;
  if (obs=="mjd" || obs=="*") fTrackMjds=1;
  if (obs=="sec" || obs=="*") fTrackSecs=1;
  if (obs=="id" || obs=="*") fTrackIds=1;
  if (obs=="nsig" || obs=="*") fTrackNsigs=1;
  if (obs=="user" && uname != "") // Selection of names of user data words
  {
   if (!fTrackUnames)
   {
    fTrackUnames=new TObjArray();
    fTrackUnames->SetOwner();
   }
   // Check if this name has already been specified before 
   nen=fTrackUnames->GetEntries();
   for (Int_t i=0; i<nen; i++)
   {
    TObjString* sx=(TObjString*)fTrackUnames->At(i);
    if (!sx) continue;
    s=sx->GetString();
    if (s==uname) return;
   }
   // New name to be added into the storage
   if (nen >= fTrackUnames->GetSize()) fTrackUnames->Expand(nen+1);
   TObjString* namex=new TObjString();
   namex->SetString(uname);
   fTrackUnames->Add(namex);
  }
 }
 
 if (type=="event") // Total event observables
 {
  fEventflag=1;
  if (obs=="p" || obs=="*") fEventMomenta=1;
  if (obs=="e" || obs=="*") fEventEnergies=1;
  if (obs=="m" || obs=="*") fEventMasses=1;
  if (obs=="q" || obs=="*") fEventCharges=1;
  if (obs=="jrun" || obs=="*") fEventJruns=1;
  if (obs=="jevt" || obs=="*") fEventJevts=1;
  if (obs=="weight" || obs=="*") fEventWeights=1;
  if (obs=="mjd" || obs=="*") fEventMjds=1;
  if (obs=="sec" || obs=="*") fEventSecs=1;
  if (obs=="user" && uname != "") // Selection of names of user data words
  {
   if (!fEventUnames)
   {
    fEventUnames=new TObjArray();
    fEventUnames->SetOwner();
   }
   // Check if this name has already been specified before 
   nen=fEventUnames->GetEntries();
   for (Int_t j=0; j<nen; j++)
   {
    TObjString* sx=(TObjString*)fEventUnames->At(j);
    if (!sx) continue;
    s=sx->GetString();
    if (s==uname) return;
   }
   // New name to be added into the storage
   if (nen >= fEventUnames->GetSize()) fEventUnames->Expand(nen+1);
   TObjString* namex=new TObjString();
   namex->SetString(uname);
   fEventUnames->Add(namex);
  }
 }

 if (type=="device") // Device observables
 {
  fDeviceflag=1;
  if (obs=="ndev" || obs=="*") fDeviceNdevs=1;
  if (obs=="nhit" || obs=="*") fDeviceNhits=1;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::SetAstrolab(NcAstrolab* lab)
{
// Specify the astrolab to be used.
// This will lead to the appearance of so called "astrotracks" from
// (astrophysical) reference objects in the produced output tree.
//
// In case lab=0 no "astrotracks" are produced.
//
// Note :
// ------
// An internal copy of the specified Astrolab will be made.
// In case one wants to modify the Astrolab, this should be done via
// the corresponding pointer which is provided via GetAstrolab.

 if (fAstrolab)
 {
  delete fAstrolab;
  fAstrolab=0;
 }

 if (lab) fAstrolab=new NcAstrolab(*lab);
}
///////////////////////////////////////////////////////////////////////////
NcAstrolab* NcTreeMaker::GetAstrolab() const
{
// Provide pointer to the astrolab.
 return fAstrolab;
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::Exec(Option_t* opt)
{
// Implementation of the ouput tree creation and filling.

 TString name=opt;
 NcJob* parent=(NcJob*)(gROOT->GetListOfTasks()->FindObject(name.Data()));

 if (!parent) return;

 fEvt=(NcEvent*)parent->GetObject("NcEvent");
 if (!fEvt) return;

 // Only process accepted events
 NcDevice* seldev=(NcDevice*)fEvt->GetDevice("NcEventSelector");
 if (seldev)
 {
  if (seldev->GetSignal("Select") < 0.1) return;
 }

 Int_t ntkmax=0;   // Max. number of tracks for a certain name
 Int_t ntnames=0;  // Number of track names to be processed
 Int_t ntunames=0; // Number of track user data names to be processed
 Int_t neunames=0; // Number of event user data names to be processed
 if (fUseTracks) ntnames=fUseTracks->GetEntries();
 Int_t ndnames=0; // Number of device classes to be processed
 if (fUseDevices) ndnames=fUseDevices->GetEntries();
 if (fTrackUnames) ntunames=fTrackUnames->GetEntries();
 if (fEventUnames) neunames=fEventUnames->GetEntries();
 TObjString* strx=0;
 TString str,str2;

 TTree* tree=0;

 if (fFirst)
 {
  // Setup of the output tree storage arrays
  if (fTp) delete[] fTp;
  fTp=new Float_t[fNtkmax];
  if (fTthetap) delete[] fTthetap;
  fTthetap=new Float_t[fNtkmax];
  if (fTphip) delete[] fTphip;
  fTphip=new Float_t[fNtkmax];
  if (fTe) delete[] fTe;
  fTe=new Float_t[fNtkmax];
  if (fTm) delete[] fTm;
  fTm=new Float_t[fNtkmax];
  if (fTq) delete[] fTq;
  fTq=new Float_t[fNtkmax];
  if (fTmjd) delete[] fTmjd;
  fTmjd=new Double_t[fNtkmax];
  if (fTsec) delete[] fTsec;
  fTsec=new Double_t[fNtkmax];
  if (fTid) delete[] fTid;
  fTid=new Int_t[fNtkmax];
  if (fTnsig) delete[] fTnsig;
  fTnsig=new Int_t[fNtkmax];

  if (fTuser) delete[] fTuser;
  fTuser=0;
  if (ntunames)
  {
   fTuser=new Float_t*[ntunames];
   for (Int_t i=0; i<ntunames; i++)
   {
    fTuser[i]=new Float_t[fNtkmax];
   }
  }

  if (fEuser) delete[] fEuser;
  fEuser=0;
  if (neunames) fEuser=new Float_t[neunames];

  if (fTrees) delete fTrees;
  fTrees=new TObjArray();
  fTrees->SetOwner();

  cout << " *NcTreeMaker* Selection parameters." << endl;
  cout << " Selection types in use :";
  if (fTrackflag) cout << " track";
  if (fEventflag) cout << " event";
  if (fDeviceflag) cout << " device";
  if (!fTrackflag && !fEventflag && !fDeviceflag) cout << " none";
  cout << endl;

  if (ntnames) cout << " Track name selections to be processed (-1=all)." << endl;
  for (Int_t i=0; i<ntnames; i++)
  {
   strx=(TObjString*)fUseTracks->At(i);
   if (!strx) continue;
   str=strx->GetString();
   if (str=="*") str="trk"; // Generic track name "trk" in output tree for "*" selection
   ntkmax=fUseNtk->At(i);
   cout << " Maximally " << ntkmax << " track(s) per event of name : " << str.Data() << endl;

   // Creation of the corresponding track tree and branch structure(s)
   if (fTrackflag)
   {
    tree=(TTree*)fOutfile->Get(str.Data());
    if (!tree)
    {
     // String replacements to prevent problems with TreeViewer arithmetic
     str.ReplaceAll("-","m");
     str.ReplaceAll("+","p");
     fOutfile->cd();
     tree=new TTree(str.Data(),"track data");
     if (fOuttree) fOuttree->AddFriend(tree->GetName());
     fTrees->Add(tree);
    }
    if (!(tree->GetBranch("n"))) tree->Branch("n",&fTn,"n/I");
    if (fTrackMomenta)
    {
     if (!(tree->GetBranch("p"))) tree->Branch("p",fTp,"p[n]/F");
     if (!(tree->GetBranch("thetap"))) tree->Branch("thetap",fTthetap,"thetap[n]/F");
     if (!(tree->GetBranch("phip"))) tree->Branch("phip",fTphip,"phip[n]/F");
    }
    if (fTrackEnergies && !(tree->GetBranch("E"))) tree->Branch("E",fTe,"E[n]/F");
    if (fTrackMasses && !(tree->GetBranch("m"))) tree->Branch("m",fTm,"m[n]/F");
    if (fTrackCharges && !(tree->GetBranch("q"))) tree->Branch("q",fTq,"q[n]/F");
    if (fTrackMjds && !(tree->GetBranch("mjd"))) tree->Branch("mjd",fTmjd,"mjd[n]/D");
    if (fTrackSecs && !(tree->GetBranch("sec"))) tree->Branch("sec",fTsec,"sec[n]/D");
    if (fTrackIds && !(tree->GetBranch("id"))) tree->Branch("id",fTid,"id[n]/I");
    if (fTrackNsigs && !(tree->GetBranch("nsig"))) tree->Branch("nsig",fTnsig,"nsig[n]/I");
    if (fTrackUnames)
    {
     for (Int_t ituname=0; ituname<ntunames; ituname++)
     {
      strx=(TObjString*)fTrackUnames->At(ituname);
      if (!strx) continue;
      str=strx->GetString();
      str2=str;
      str2+="[n]/F";
      if (!(tree->GetBranch(str.Data()))) tree->Branch(str.Data(),fTuser[ituname],str2.Data());
     }
    }
   }
  }

  // Creation of the track tree and branch structure for astrophysical reference objects
  if (fAstrolab)
  {
   cout << " Number of reference objects in Astrolab : " << fAstrolab->GetNRefSignals() << endl;

   tree=(TTree*)fOutfile->Get("astrotrack");
   if (!tree)
   {
    fOutfile->cd();
    tree=new TTree("astrotrack","astro track data");
    if (fOuttree) fOuttree->AddFriend(tree->GetName());
    fTrees->Add(tree);
   }
   if (!(tree->GetBranch("n"))) tree->Branch("n",&fTn,"n/I");
   if (!(tree->GetBranch("thetap"))) tree->Branch("thetap",fTthetap,"thetap[n]/F");
   if (!(tree->GetBranch("phip"))) tree->Branch("phip",fTphip,"phip[n]/F");
   if (!(tree->GetBranch("icrsl"))) tree->Branch("icrsl",fTe,"icrsl[n]/F");
   if (!(tree->GetBranch("icrsb"))) tree->Branch("icrsb",fTm,"icrsb[n]/F");
   if (!(tree->GetBranch("mjd"))) tree->Branch("mjd",fTmjd,"mjd[n]/D");
   if (fTrackSecs && !(tree->GetBranch("sec"))) tree->Branch("sec",fTsec,"sec[n]/D");
  }

  if ((fTrackflag && ntnames) || fAstrolab) cout << " --- Max. number of output tracks per event per name : " << fNtkmax << endl; 

  // Creation of the event tree and branch structure(s)
  if (fEventflag)
  {
   tree=(TTree*)fOutfile->Get("evt");
   if (!tree)
   {
    fOutfile->cd();
    tree=new TTree("evt","event data");
    if (fOuttree) fOuttree->AddFriend(tree->GetName());
    fTrees->Add(tree);
   }
   if (fEventJruns && !(tree->GetBranch("jrun"))) tree->Branch("jrun",&fEjrun,"jrun/I");
   if (fEventJevts && !(tree->GetBranch("jevt"))) tree->Branch("jevt",&fEjevt,"jevt/I");
   if (fEventWeights && !(tree->GetBranch("weight"))) tree->Branch("weight",&fEweight,"weight/D");
   if (fEventMjds && !(tree->GetBranch("mjd"))) tree->Branch("mjd",&fEmjd,"mjd/D");
   if (fEventSecs && !(tree->GetBranch("sec"))) tree->Branch("sec",&fEsec,"sec/D");
   if (fEventMomenta)
   {
    if (!(tree->GetBranch("p"))) tree->Branch("p",&fEp,"p/F");
    if (!(tree->GetBranch("thetap"))) tree->Branch("thetap",&fEthetap,"thetap/F");
    if (!(tree->GetBranch("phip"))) tree->Branch("phip",&fEphip,"phip/F");
   }
   if (fEventEnergies && !(tree->GetBranch("E"))) tree->Branch("E",&fEe,"E/F");
   if (fEventMasses && !(tree->GetBranch("m"))) tree->Branch("m",&fEm,"m/F");
   if (fEventCharges && !(tree->GetBranch("q"))) tree->Branch("q",&fEq,"q/F");
   if (fEventUnames)
   {
    for (Int_t ieuname=0; ieuname<neunames; ieuname++)
    {
     strx=(TObjString*)fEventUnames->At(ieuname);
     if (!strx) continue;
     str=strx->GetString();
     str2=str;
     str2+="/F";
     if (!(tree->GetBranch(str.Data()))) tree->Branch(str.Data(),&fEuser[ieuname],str2.Data());
    }
   }
  }

  // Creation of the device tree and branch structure(s)
  if (ndnames) cout << " Device classname selections to be processed." << endl;
  for (Int_t j=0; j<ndnames; j++)
  {
   strx=(TObjString*)fUseDevices->At(j);
   if (!strx) continue;
   str=strx->GetString();
   cout << " Devices (derived from) : " << str.Data() << endl;
   if (fDeviceflag)
   {
    tree=(TTree*)fOutfile->Get(str.Data());
    if (!tree)
    {
     fOutfile->cd();
     tree=new TTree(str.Data(),"device data");
     if (fOuttree) fOuttree->AddFriend(tree->GetName());
     fTrees->Add(tree);
    }
    if (fDeviceNdevs && !(tree->GetBranch("ndev"))) tree->Branch("ndev",&fDndev,"ndev/I");
    if (fDeviceNhits && !(tree->GetBranch("nhit"))) tree->Branch("nhit",&fDnhit,"nhit/I");
   }
  }

  cout << endl;

  fFirst=0;
 }

 if (fEventflag) Event();     // Obtain total event observables
 if (fTrackflag) Track();     // Obtain individual track observables
 if (fDeviceflag) Device();   // Obtain device observables
 if (fAstrolab) AstroTrack(); // Create tracks from (astrophysical) reference objects

 if (fOuttree && (fEventflag || fTrackflag || fDeviceflag)) fOuttree->Fill(); 
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::Track()
{
// Obtain individual track observables.
// Note : Tracks without vector information will be skipped.

 if (!fUseTracks) return;

 Int_t nnames=fUseTracks->GetEntries(); // Number of track names to be processed
 Int_t ntkmax=0; // Max. number of tracks for a certain name
 Int_t tkmode=0; // Mode for track retrieval from the event based on the track name (pattern)
 TObjString* strx=0;
 TString str,str2;
 Int_t ntk;

 Int_t ntunames=0; // Number of track user data names to be processed
 if (fTrackUnames) ntunames=fTrackUnames->GetEntries();

 Int_t ndnames=0; // Number of device classnames to be processed
 if (fUseDevices) ndnames=fUseDevices->GetEntries();

 TTree* tree=0;

 NcTrack* track=0;
 TObjArray tracks;
 Int_t mjd,s,ns,ps;
 for (Int_t i=0; i<nnames; i++) // Loop over selected track names
 {
  strx=(TObjString*)fUseTracks->At(i);
  if (!strx) continue;
  str=strx->GetString();
  ntkmax=fUseNtk->At(i);
  tkmode=fUseTkMode->At(i);
  if (str=="*")
  {
   fEvt->GetTracks(0,2,0,&tracks);
   str="trk";
  }
  else
  {
   fEvt->GetTracks(str,tkmode,&tracks);
  }
  ntk=tracks.GetEntries();
  if (ntkmax>0 && ntk>ntkmax) ntk=ntkmax;

  if (ntk>fNtkmax) ntk=fNtkmax; // Max. track number for output tree

  // Enter selected observables for this track name into the output tree
  fTn=0;
  for (Int_t jtk=0; jtk<ntk; jtk++) // Loop over tracks of a certain name
  {
   track=(NcTrack*)tracks.At(jtk);
   if (!track) continue;

   if (track->HasVector()==0) continue;

   fTp[fTn]=track->GetMomentum(1);
   fTthetap[fTn]=track->GetX(2,"sph","rad");
   fTphip[fTn]=track->GetX(3,"sph","rad");
   fTe[fTn]=track->GetEnergy(1);
   fTm[fTn]=track->GetMass(1);
   fTq[fTn]=track->GetCharge();
   fTmjd[fTn]=0;
   fTsec[fTn]=0;
   NcTimestamp* ts=track->GetTimestamp();
   if (ts)
   {
    fTmjd[fTn]=ts->GetMJD();
    ts->GetMJD(mjd,s,ns);
    ps=fEvt->GetPs();
    fTsec[fTn]=double(s)+double(ns)*1.e-9+double(ps)*1.e-12;
   }
   fTid[fTn]=track->GetId();

   fTnsig[fTn]=0;
   for (Int_t id=0; id<ndnames; id++) // Loop over selected device classnames
   {
    strx=(TObjString*)fUseDevices->At(id);
    if (!strx) continue;
    str2=strx->GetString();
    fTnsig[fTn]+=track->GetNsignals(str2.Data(),2); // Sample number of associated signals
   } // End of loop over selected device classnames 

   NcSignal* sx=track->GetUserData();
   for (Int_t ituname=0; ituname<ntunames; ituname++) // Loop over selected user data word names
   {
    fTuser[ituname][fTn]=0;
    strx=(TObjString*)fTrackUnames->At(ituname);
    if (!strx) continue;
    str2=strx->GetString();
    if (!sx) continue;
    fTuser[ituname][fTn]=sx->GetSignal(str2); // Get value of the requested user data word
   } // End of loop over selected user data word names

   fTn++;
  } // End of loop over tracks of a certain name

  // String replacements to prevent problems with TreeViewer arithmetic
  str.ReplaceAll("-","m");
  str.ReplaceAll("+","p");
  tree=(TTree*)fOutfile->Get(str.Data());
  if (tree) tree->Fill();

 } // End of loop over selected track names 
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::Event()
{
// Obtain total event observables.

 TTree* tree=(TTree*)fOutfile->Get("evt");
 if (!tree) return;

 Int_t neunames=0; // Number of event user data names to be processed
 if (fEventUnames) neunames=fEventUnames->GetEntries();

 Nc3Vector p;
 p=fEvt->Get3Momentum();
 Int_t mjd,s,ns,ps;
 fEvt->GetMJD(mjd,s,ns);
 ps=fEvt->GetPs();

 fEjrun=fEvt->GetRunNumber();
 fEjevt=fEvt->GetEventNumber();
 fEweight=fEvt->GetWeight();
 fEmjd=fEvt->GetMJD();
 fEsec=double(s)+double(ns)*1.e-9+double(ps)*1.e-12;
 fEp=fEvt->GetMomentum(1);
 fEthetap=p.GetX(2,"sph","rad");
 fEphip=p.GetX(3,"sph","rad");
 fEe=fEvt->GetEnergy(1);
 fEm=fEvt->GetInvmass(1);
 fEq=fEvt->GetCharge();

 TObjString* strx=0;
 TString str;
 NcSignal* sx=fEvt->GetUserData();
 for (Int_t ieuname=0; ieuname<neunames; ieuname++) // Loop over selected user data word names
 {
  fEuser[ieuname]=0;
  strx=(TObjString*)fEventUnames->At(ieuname);
  if (!strx) continue;
  str=strx->GetString();
  if (!sx) continue;
  fEuser[ieuname]=sx->GetSignal(str); // Get value of the requested user data word
 } // End of loop over selected user data word names

 tree->Fill();
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::Device()
{
// Obtain device observables.

 if (!fUseDevices) return;

 Int_t nnames=fUseDevices->GetEntries(); // Number of device classnames to be processed
 TObjString* strx=0;
 TString str;

 TTree* tree=0;

 for (Int_t i=0; i<nnames; i++) // Loop over selected device classnames
 {
  strx=(TObjString*)fUseDevices->At(i);
  if (!strx) continue;
  str=strx->GetString();

  // Enter selected observables for this device class into the output tree
  fDndev=fEvt->GetNdevices(str.Data());
  fDnhit=fEvt->GetNhits(str.Data());

  tree=(TTree*)fOutfile->Get(str.Data());
  if (tree) tree->Fill();

 } // End of loop over selected device classnames 
}
///////////////////////////////////////////////////////////////////////////
void NcTreeMaker::AstroTrack()
{
// Create individual track observables from (astrophysical) reference objects.
// The astrotrack directions correspond to hypothetical tracks of messengers arriving
// from the (astrophysical) reference object.
// These directions (in radians) are provided in the local lab coordinate system from the
// location where the (astrophysical) phenomenon would be at the time of the current event. 
// The timestamps of the astrotracks correspond to the timestamps as stored for the
// reference objects in the NcAstrolab, which in general is the actual timestamp
// of the astrophysical observation.
// The latter allows investigation of a possible time difference between the observation
// of an (astrophysical) phenomenon and the regular detected tracks/signals in the detector. 
// Also the ICRS longitude (l) and latitude (b) of the (astrophysical) reference objects
// are provided in radians.

 if (!fAstrolab) return;

 Int_t ntk=fAstrolab->GetNRefSignals(1);
 if (ntk>fNtkmax) ntk=fNtkmax; // Max. track number for output tree

 // Enter observables for each astrotrack into the output tree
 Double_t d,a,b;
 Double_t vec[3];
 Nc3Vector p;
 Int_t mjd,s,ns,ps;
 fTn=0;
 for (Int_t jtk=1; jtk<=ntk; jtk++)
 {
  NcSignal* sx=fAstrolab->GetSignal(d,a,"rad",b,"rad","loc",(NcTimestamp*)fEvt,jtk);
  if (!sx) continue;

  vec[0]=d;
  vec[1]=a;
  vec[2]=b;
  p.SetVector(vec,"sph","rad");

  p*=-1.; // Track direction as arriving from the (astrophysical) object

  sx=fAstrolab->GetSignal(d,a,"rad",b,"rad","icr",(NcTimestamp*)fEvt,jtk);

  fTthetap[fTn]=p.GetX(2,"sph","rad");
  fTphip[fTn]=p.GetX(3,"sph","rad");
  fTm[fTn]=b; // ICRS latitude in radians
  fTe[fTn]=a; // ICRS longitude in radians
  fTmjd[fTn]=0;
  fTsec[fTn]=0;
  NcTimestamp* ts=sx->GetTimestamp();
  if (ts)
  {
   fTmjd[fTn]=ts->GetMJD();
   ts->GetMJD(mjd,s,ns);
   ps=fEvt->GetPs();
   fTsec[fTn]=double(s)+double(ns)*1.e-9+double(ps)*1.e-12;
  }
  fTn++;
 } // End of loop over astrotracks

 TTree* tree=(TTree*)fOutfile->Get("astrotrack");
 if (tree) tree->Fill();
}
///////////////////////////////////////////////////////////////////////////
TObject* NcTreeMaker::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers when adding objects in case the container owns the objects.

 NcTreeMaker* sel=new NcTreeMaker(*this);
 if (name)
 {
  if (strlen(name)) sel->SetName(name);
 }
 return sel;
}
///////////////////////////////////////////////////////////////////////////
