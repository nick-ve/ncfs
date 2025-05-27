/*******************************************************************************
~~~
 * Copyright(c) 2003, IceCube Experiment at the South Pole. All rights reserved.
 *
 * Author: The IceCube NCFS-based Offline Project.
 * Contributors are mentioned in the code where appropriate.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation strictly for non-commercial purposes is hereby granted
 * without fee, provided that the above copyright notice appears in all
 * copies and that both the copyright notice and this permission notice
 * appear in the supporting documentation.
 * The authors make no claims about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
~~~
 *******************************************************************************/

///////////////////////////////////////////////////////////////////////////
/** @class IcePandel
~~~
// Class IcePandel
// IceRecoBase derived TTask processor to perform track fitting via minimisation
// of a Gauss convoluted Pandel pdf.
//
// In case an event has been rejected by an NcEventSelector (based) processor,
// this task (and its sub-tasks) is not executed.
//
// The code in this processor is based on the algorithms as developed
// by Oladipo Fadiran, George Japaridze (Clark Atlanta University, USA)
// and Nick van Eijndhoven (Utrecht University, The Netherlands).
// For details please refer to Astroparticle Physics 28 (2007) 456-462.
//
// For the minimisation process the TFitter facility, which is basically Minuit,
// is used. Minimisation is performed by invokation of the SIMPLEX method,
// followed by an invokation of HESSE to determine the uncertainties on the results. 
// The statistics of the TFitter result are stored as an NcSignal object
// in the track, which can be obtained via the GetFitDetails memberfunction.
// In the minimisation procedure an overall plausibility for the fitted track
// is determined based on a Gauss convoluted Pandel pdf value for each used hit.
// This track plausibility is expressed in terms of a Bayesian psi value
// w.r.t. a Gauss Convoluted Pandel PDF.
// The Baysian psi value is defined as -loglikelihood in a decibel scale.
// This implies psi=-10*log10(L) where L=p(D|HI) being the likelihood of
// the data D under the hypothesis H and prior information I.
// Since all (associated) hits contribute independently to the Bayesian psi
// value, this psi value is built up by summation of the various hit contributions.
// As such, the FitDetails entries contain the statistics of all the different
// hit contributions, like PsiMedian, PsiMean, and PsiSigma.
// The Bayesian psi value is available in the fit details under the name "PsiSum".
// In addition the standard Minuit results like IERFIT, FCN, EDM etc... are
// also available from the FitDetails.
//
// The Gauss convoluted Pandel value is evaluated in various areas in the distance-time
// space as described in Astropart. Phys. 28 (2007) 456-462 by N. van Eijndhoven,
// O. Fadiran and G. Japaridze
// In case the distance-time point of a certain hit falls outside the
// validity rectangle, the point is moved onto the corresponding side location
// of the rectangle. For this new location the Pandel value is evaluated for
// this hit and an extra penalty is added to the corresponding psi value
// for this hit. 
// The penalty value can be specified via the memberfunction SetPenalty().
// This allows investigation/tuning of the sensitivity to hits with
// extreme distance and/or time residual values.
//
// As can be seen in the publication mentioned above, the pdf depends on
// the PMT time jitter, the photon scattering length and the photon absorption length.
// These parameters can be specified via the memberfunctions SetTimeJitter(),
// SetScatteringLength() and SetAbsorptionLength(), respectively. 
//
// A separate treatment of the phase and group velocities is introduced
// which will provide more accurate time residuals due to the different
// velocities of the Cerenkov wave front (v_phase) and the actually detected
// photons (v_group).
// This distinction between v_phase and v_group can be (de)activated via the
// memberfunction SetVgroupUsage().
//
// Use the UseTracks memberfunction to specify the first guess tracks
// to be processed by the minimiser (e.g. IceDwalkIC, IceDwalkI, IceDwalkDC).
//
// Use the SelectHits() memberfunction to specify the hits to be used.
//
// Information about the actual parameter settings can be found in the event
// structure itself via the device named "IcePandel".
//
// The fit processor printlevel can be selected via the memberfunction SetPrintLevel().
// By default all printout is suppressed (i.e. level=-2).
// 
// An example of how to invoke this processor after Xtalk, hit cleaning
// and a direct walk first guess estimate can be found in the ROOT example
// macro icepandel.cc which resides in the /macros subdirectory.
// 
// The minimisation results are stored in the IceEvent structure as
// tracks with as default the basename "IcePandel".
// This track basename identifier can be modified by the user via the
// SetTrackName() memberfunction. This will allow unique identification
// of tracks which are produced when re-processing existing data with
// different criteria.
// A suffix "4..." will be added to the tracks basename automatically according
// to the input track class that was used for the fit.
// For instance, the produced track name is IcePandel4DwalkIC when the input track
// was an IceDwalkIC track.
//
// The mass and charge of the track are left 0, since no distinction can
// be made between positive or negative tracks and also no mass can be determined.
// However, the user can define the track charge by invokation of the
// memberfunction SetCharge().
// This facility may be used to distinguish tracks produced by the
// various reconstruction algorithms in a (3D) colour display
// (see the class NcHelix for further details).  
//
// The produced track will be linked as a Track Hypothesis to the
// (first guess) track which was used as input, the so called parent track.
// As such, a pointer to the parent track is available via the GetParentTrack facility
// of the produced "IcePandel" tracks.
// Furthermore, all the hits that were used in the minisation are available
// via the GetSignal facility of the produced "IcePandel" tracks.
//
// An example of how the various data can be accessed is given below,
// where "evt" indicates the pointer to the IceEvent structure.
//
// Example for accessing data :
// ----------------------------
// TObjArray* tracks=evt->GetTracks("IcePandel4DwalkIC");
// if (!tracks) return;
// NcPosition* r0=0;
// Float_t fcn=0;
// for (Int_t jtk=0; jtk<tracks->GetEntries(); jtk++)
// {
//  NcTrack* tx=(NcTrack*)tracks->At(jtk);
//  if (!tx) continue;
//  tx->Data("sph");
//  r0=tx->GetReferencePoint();
//  if (r0) r0->Data();
//  sx=(NcSignal*)tx->GetFitDetails();
//  if (sx) fcn=sx->GetSignal("FCN");
//  NcTrack* parent=tx->GetParentTrack();
//  if (!parent) continue;
//  parent->Data("sph");
//  r0=parent->GetReferencePoint();
//  if (r0) r0->Data();
// }
//
// Notes :
// -------
// 1) This processor only works properly on data which are calibrated
//    and contain tracks from first guess algorithms like e.g. IceDwalk.
// 2) In view of the usage of TFitter/Minuit minimisation, a global pointer
//    to the instance of this class (gIcePandel) and a global static
//    wrapper function (IcePandelFCN) have been introduced, to allow the
//    actual minimisation to be performed via the memberfunction FitFCN.
//    This implies that in a certain processing job only 1 instance of
//    this IcePandel class may occur.
//
//--- Author: Nick van Eijndhoven 09-feb-2006 Utrecht University
//- Modified: NvE $Date: 2013-02-27 23:43:36 +0100 (Wed, 27 Feb 2013) $ IIHE-VUB, Brussels
~~~
**/
///////////////////////////////////////////////////////////////////////////
 
#include "IcePandel.h"
#include "Riostream.h"

// Global pointer to the instance of this object
 IcePandel* gIcePandel=0;

// TFitter/Minuit interface to IcePandel::FitFCN
 void IcePandelFCN(Int_t& npar,Double_t* gin,Double_t& f,Double_t* u,Int_t flag)
 {
  if (gIcePandel) gIcePandel->FitFCN(npar,gin,f,u,flag);
 }

ClassImp(IcePandel) // Class implementation to enable ROOT I/O

IcePandel::IcePandel(const char* name,const char* title) : IceRecoBase(name,title)
{
/**
~~~
// Default constructor.
~~~
**/

 fPrint=-2;
 fHits=0;
 fFitter=0;
 fTkfit=0;
 fFitstats=0;

 SetCleaned(1,"A");
 SetCleaned(0,"I");
 SetCleaned(0,"IC");
 SetCleaned(0,"DC");

 SetMinAhits(0,"A");
 SetMinAhits(0,"I");
 SetMinAhits(0,"IC");
 SetMinAhits(0,"DC");

 SetMinAmods(7,"A");
 SetMinAmods(7,"I");
 SetMinAmods(7,"IC");
 SetMinAmods(7,"DC");

 SetScatteringLength(33.3,"A");
 SetScatteringLength(30,"UD");
 SetScatteringLength(5,"DL");
 SetScatteringLength(40,"LD");

 SetAbsorptionLength(50,"A");
 SetAbsorptionLength(100,"UD");
 SetAbsorptionLength(10,"DL");
 SetAbsorptionLength(150,"LD");

 SetTimeJitter(10,"A");
 SetTimeJitter(5,"IC");
 SetTimeJitter(5,"DC");

 SetVgroupUsage(1,"A");
 SetVgroupUsage(1,"IC");
 SetVgroupUsage(1,"I");
 SetVgroupUsage(1,"DC");

 SelectHits(1);
 SetPenalty(0);

 // Set the global pointer to this instance
 gIcePandel=this;
}
///////////////////////////////////////////////////////////////////////////
IcePandel::~IcePandel()
{
/**
~~~
// Default destructor.
~~~
**/

 if (fHits)
 {
  delete fHits;
  fHits=0;
 }
 if (fFitter)
 {
  delete fFitter;
  fFitter=0;
 }
 if (fTkfit)
 {
  delete fTkfit;
  fTkfit=0;
 }
 if (fFitstats)
 {
  delete fFitstats;
  fFitstats=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void IcePandel::SetPrintLevel(Int_t level)
{
/**
~~~
// Set the fitter (Minuit) print level.
// See the TFitter and TMinuit docs for details.
//
// Note : level=-2 suppresses also all fit processor warnings.
//        
// The default in the constructor is level=-2. 
~~~
**/

 fPrint=level;
}
///////////////////////////////////////////////////////////////////////////
void IcePandel::SelectHits(Int_t mode)
{
/**
~~~
// Specification of the hits to be used in the minimisation.
//
// mode = 0 : All hits (except IceTop) of the complete event are used.
//        1 : Only the associated hits are used for each first guess track.
//        2 : All hits are used of those detector systems (e.g. Amanda, InIce)
//            that were used to construct the first guess track.
//
// Note : The user can select to use only hits which survived a hit cleaning processor
//        via the SetClean() memberfunction.
//
// For the default value please refer to the default constructor of this class.
~~~
**/

 if (mode>=0 && mode<=2)
 {
  fSelhits=mode;
  fParams.AddNamedSlot("Selhits");
  fParams.SetSignal(fSelhits,"Selhits");
 }
}
///////////////////////////////////////////////////////////////////////////
void IcePandel::SetPenalty(Float_t val)
{
/**
~~~
// Set user defined psi penalty value (in dB) in the minimiser for
// distance-time points that fall outside the validity rectangle.
// This allows investigation/tuning of the sensitivity to hits with
// extreme distance and/or time residual values.
// For the default value please refer to the default constructor of this class.
~~~
**/

 fPenalty=val;
 fParams.AddNamedSlot("Penalty");
 fParams.SetSignal(fPenalty,"Penalty");
}
///////////////////////////////////////////////////////////////////////////
void IcePandel::Exec(Option_t* opt)
{
/**
~~~
// Implementation of the Convoluted Pandel hit fitting procedure.
~~~
**/

 // Obtain a pointer to the parent NcJob of this reconstruction task
 TString name=opt;
 NcJob* parent=(NcJob*)(gROOT->GetListOfTasks()->FindObject(name.Data()));

 if (!parent) return;

 // Obtain a pointer to the IceCube event data structure
 fEvt=(IceEvent*)parent->GetObject("IceEvent");
 if (!fEvt) return;

 // Only process accepted events
 NcDevice* seldev=(NcDevice*)fEvt->GetDevice("NcEventSelector");
 if (seldev)
 {
  if (seldev->GetSignal("Select") < 0.1) return;
 }

 // Provide a name for the fParams device in the event
 fParams.SetNameTitle("IcePandel","IcePandel processor parameters");

 // Add the fParams device to the IceEvent structure
 fEvt->AddDevice(fParams);

 Int_t nclasses=fUseNames->GetEntries(); // Number of first guess classes to be processed
 Int_t ntkmax=0; // Max. number of tracks for a certain class
 TObjString* strx=0;
 TString str;

 // Printout information on used tracks (if any) at first startup of the processor task
 if (fFirst)
 {
  if (!fUseNames)
  {
   cout << " *IcePandel* No input tracks have been specified." << endl;
   cout << " *** No IcePandel processing will be performed ***." << endl;
   fFirst=0;
   return;
  }
  cout << " *IcePandel* First guess selections to be processed (-1=all)." << endl;
  for (Int_t i=0; i<nclasses; i++)
  {
   strx=(TObjString*)fUseNames->At(i);
   if (!strx) continue;
   str=strx->GetString();
   ntkmax=fUseNtk->At(i);
   cout << " Maximally " << ntkmax << " track(s) per event for procedure : " << str.Data() << endl;
  }
  cout << " *IcePandel* Hit selection mode : " << fSelhits << endl;
  cout << " *IcePandel* Penalty value for minimiser : " << fPenalty << " dB." << endl;
  cout << endl;

  fPsistats.SetStoreMode(1);

  fFirst=0;
 }

 const Double_t pi=acos(-1.);
 const Double_t e=exp(1.);

 // Initialisation of the minimisation processor
 Double_t arglist[100];
 if (!fFitter) fFitter=new TFitter();

 // The number of reconstructed tracks already present in the event
 Int_t ntkreco=fEvt->GetNtracks(1);

 if (!fHits)
 {
  fHits=new TObjArray();
 }
 else
 {
  fHits->Clear();
 }

 // Initialise the basis for the fitted track(s)
 if (!fTkfit) fTkfit=new NcTrack();
 if (fTrackname=="") fTrackname=ClassName();
 TString tracktitle;

 // If selected, use all the good quality hits (except IceTop) of the complete event
 if (fSelhits==0)
 {
  TObjArray* hits=fEvt->GetHits("IceGOM");
  if (!hits) return;
  tracktitle=" Pandel fit result using all event hits";
  for (Int_t ih=0; ih<hits->GetEntries(); ih++)
  {
   NcSignal* sx=(NcSignal*)hits->At(ih);
   if (!sx) continue;
   NcDevice* om=sx->GetDevice();
   if (!om) continue;
   if (om->InheritsFrom("IceTDOM")) continue;
   if (fCleanI || (om->InheritsFrom("IceAOM") && fCleanA) || (om->InheritsFrom("IceICDOM") && fCleanIC) ||
      (om->InheritsFrom("IceDCDOM") && fCleanDC))
   {
    if (sx->GetDeadValue("ADC") || sx->GetDeadValue("LE") || sx->GetDeadValue("TOT")) continue;
   }
   fHits->Add(sx);
  }
 }

 // Track by track processing of the selected first guess classes
 Float_t vec[3];
 Float_t err[3];
 Nc3Vector p;
 NcPosition pos;
 if (!fFitstats)
 {
  fFitstats=new NcSignal();
  fFitstats->SetNameTitle("Fitstats","TFitter stats for Pandel fit");
  fFitstats->SetSlotName("IERFIT",1);
  fFitstats->SetSlotName("FCN",2);
  fFitstats->SetSlotName("EDM",3);
  fFitstats->SetSlotName("NVARS",4);
  fFitstats->SetSlotName("IERERR",5);
  fFitstats->SetSlotName("PsiSum",6);
  fFitstats->SetSlotName("PsiMedian",7);
  fFitstats->SetSlotName("PsiSpread",8);
  fFitstats->SetSlotName("PsiMean",9);
  fFitstats->SetSlotName("PsiSigma",10);
 }
 Float_t x,y,z,theta,phi,t0;
 Double_t amin,edm,errdef; // Minimisation stats
 Int_t ierfit,iererr,nvpar,nparx;    // Minimisation stats
 Int_t ntk=0;
 Int_t nsig=0;
 NcTrack* track=0;
 Int_t nam=0;
 Int_t nah=0;
 Int_t amanda,inice,icecube,deepcore; // Detector system indicators of associated hits
 TString trackname;
 TString newname;
 TObjArray mytracks; // Temp. storage for the extracted tracks per class
 for (Int_t iclass=0; iclass<nclasses; iclass++) // Loop over first guess classes
 {
  strx=(TObjString*)fUseNames->At(iclass);
  if (!strx) continue;
  str=strx->GetString();
  ntkmax=fUseNtk->At(iclass);
  TObjArray* tracks=fEvt->GetTracks(str);
  ntk=0;
  if (tracks) ntk=tracks->GetEntries();
  if (ntkmax>0 && ntk>ntkmax) ntk=ntkmax;

  // Store track pointers in private array to prevent overwriting of TObjArray
  mytracks.Clear();
  for (Int_t i=0; i<ntk; i++)
  {
   mytracks.Add(tracks->At(i));
  }

  for (Int_t jtk=0; jtk<ntk; jtk++) // Loop over tracks of a certain class
  {
   track=(NcTrack*)mytracks.At(jtk);
   if (!track) continue;

   // The name of the first guess track without the intial "Ice"
   trackname=track->GetName();
   trackname.ReplaceAll("Ice","");

   amanda=0;
   inice=0;
   icecube=0;
   deepcore=0;

   if (fSelhits==1) tracktitle=" Pandel fit result for assoc.";
   if (fSelhits==2) tracktitle=" Pandel fit result for full";

   NcPosition* r0=track->GetReferencePoint();
   if (!r0) continue;

   NcTimestamp* tt0=r0->GetTimestamp();

   // Selection of hits to be used in the fitting procedure
   if (fSelhits==1 || fSelhits==2)
   {
    fHits->Clear();
    nsig=track->GetNsignals();
    for (Int_t is=1; is<=nsig; is++)
    {
     NcSignal* sx=track->GetSignal(is);
     if (!sx) continue;
     NcDevice* om=sx->GetDevice();
     if (!om->InheritsFrom("IceGOM")) continue;
     if (fCleanI && (sx->GetDeadValue("ADC") || sx->GetDeadValue("LE") || sx->GetDeadValue("TOT"))) continue;
     if (om->InheritsFrom("IceAOM"))
     {
      if (fCleanA && (sx->GetDeadValue("ADC") || sx->GetDeadValue("LE") || sx->GetDeadValue("TOT"))) continue;
      amanda=1;
     }
     if (om->InheritsFrom("IceICDOM"))
     {
      if (fCleanIC && (sx->GetDeadValue("ADC") || sx->GetDeadValue("LE") || sx->GetDeadValue("TOT"))) continue;
      icecube=1;
     }
     if (om->InheritsFrom("IceDCDOM"))
     {
      if (fCleanDC && (sx->GetDeadValue("ADC") || sx->GetDeadValue("LE") || sx->GetDeadValue("TOT"))) continue;
      deepcore=1;
     }
     // Only use the associated hits
     if (fSelhits==1) fHits->Add(sx);
    }

    inice=icecube*deepcore;

    // Determine the corresponding track title suffix and hits to be used in case of fSelhits==2
    TObjArray* hits=0;
    if (amanda && !icecube && !deepcore) // Only Amanda hit usage
    {
     hits=fEvt->GetHits("IceAOM");
     tracktitle+=" Amanda hits";
    }
    if (inice) // InIce hit usage
    {
     hits=fEvt->GetHits("IceIDOM");
     tracktitle+=" InIce hits";
    }
    if (icecube && !deepcore) // Only standard IceCube hit usage
    {
     hits=fEvt->GetHits("IceICDOM");
     tracktitle+=" standard IceCube hits";
    }
    if (deepcore && !icecube) // Only DeepCore hit usage
    {
     hits=fEvt->GetHits("IceDCDOM");
     tracktitle+=" DeepCore hits";
    }

    if (fSelhits==2)
    {
     if (!hits) return;
     for (Int_t is2=0; is2<hits->GetEntries(); is2++)
     {
      NcSignal* sx=(NcSignal*)hits->At(is2);
      if (!sx) continue;
      NcDevice* om=sx->GetDevice();
      if (!om) continue;
      if (om->InheritsFrom("IceTDOM")) continue;
      if (fCleanI || (om->InheritsFrom("IceAOM") && fCleanA) || (om->InheritsFrom("IceICDOM") && fCleanIC) ||
         (om->InheritsFrom("IceDCDOM") && fCleanDC))
      {
       if (sx->GetDeadValue("ADC") || sx->GetDeadValue("LE") || sx->GetDeadValue("TOT")) continue;
      }
      fHits->Add(sx);
     }
    }
   }
   
   // Require sufficient hits to produce a resulting track from the fitting procedure
   nah=fHits->GetEntries();
   nam=fEvt->GetNdevices("IceGOM",fHits);
   if (!amanda && !icecube && !deepcore) // Complete event hit usage
   {
    if (nah<fMinahitsA || nah<fMinahitsI || nah<fMinahitsIC || nah<fMinahitsDC) continue;
    if (nam<fMinamodsA || nam<fMinamodsI || nam<fMinamodsIC || nam<fMinamodsDC) continue;
   }
   if (amanda && !icecube && !deepcore) // Only Amanda hit usage
   {
    if (nah<fMinahitsA) continue;
    if (nam<fMinamodsA) continue;
   }
   if (inice) // InIce hit usage
   {
    if (nah<fMinahitsI) continue;
    if (nam<fMinamodsI) continue;
   }
   if (icecube && !deepcore) // Only standard IceCube hit usage
   {
    if (nah<fMinahitsIC) continue;
    if (nam<fMinamodsIC) continue;
   }
   if (deepcore && !icecube) // Only DeepCore hit usage
   {
    if (nah<fMinahitsDC) continue;
    if (nam<fMinamodsDC) continue;
   }

   r0->GetVector(vec,"car");
   r0->GetErrors(err,"car");

   x=vec[0];
   y=vec[1];
   z=vec[2];

   p=track->Get3Momentum();
   p.GetVector(vec,"sph");

   theta=vec[1];
   phi=vec[2];

   t0=fEvt->GetDifference(tt0,"ns");
   
   // Process this first guess track with its associated hits
   fFitter->Clear();

   // Set user selected TFitter printout level
   arglist[0]=fPrint;
   if (fPrint==-2) arglist[0]=-1;
   fFitter->ExecuteCommand("SET PRINT",arglist,1);
   if (fPrint==-2) fFitter->ExecuteCommand("SET NOWARNINGS",arglist,0);

   fFitter->SetFitMethod("loglikelihood");

   // Define errors to represent 1 sigma for this likelihood scale
   arglist[0]=5.*log10(e);
   fFitter->ExecuteCommand("SET ERRORDEF",arglist,1);

   fFitter->SetParameter(0,"r0x",x,0.1,0,0);
   fFitter->SetParameter(1,"r0y",y,0.1,0,0);
   fFitter->SetParameter(2,"r0z",z,0.1,0,0);
   fFitter->SetParameter(3,"theta",theta,0.001,0,pi);
   fFitter->SetParameter(4,"phi",phi,0.001,0,2.*pi);
   fFitter->SetParameter(5,"t0",t0,1.,0,32000);

   fFitter->SetFCN(IcePandelFCN);

   fTkfit->Reset();

   arglist[0]=0;
   ierfit=fFitter->ExecuteCommand("SIMPLEX",arglist,0);

   fFitter->GetStats(amin,edm,errdef,nvpar,nparx);

   fFitstats->Reset();
   fFitstats->SetSignal(ierfit,1);
   fFitstats->SetSignal(amin,2);
   fFitstats->SetSignal(edm,3);
   fFitstats->SetSignal(nvpar,4);

   fFitstats->SetSignal(fPsistats.GetSum(1),6);
   fFitstats->SetSignal(fPsistats.GetMedian(1),7);
   fFitstats->SetSignal(fPsistats.GetSpread(1),8);
   fFitstats->SetSignal(fPsistats.GetMean(1),9);
   fFitstats->SetSignal(fPsistats.GetSigma(1),10);

   iererr=fFitter->ExecuteCommand("HESSE",arglist,0);
   fFitstats->SetSignal(iererr,5);

   // Resulting parameters after minimisation and error calculation
   vec[0]=fFitter->GetParameter(0);
   vec[1]=fFitter->GetParameter(1);
   vec[2]=fFitter->GetParameter(2);
   err[0]=fFitter->GetParError(0);
   err[1]=fFitter->GetParError(1);
   err[2]=fFitter->GetParError(2);
   pos.SetPosition(vec,"car");
   pos.SetPositionErrors(err,"car");

   vec[0]=1.;
   vec[1]=fFitter->GetParameter(3);
   vec[2]=fFitter->GetParameter(4);
   err[0]=0.;
   err[1]=fFitter->GetParError(3);
   err[2]=fFitter->GetParError(4);
   p.SetVector(vec,"sph");
   p.SetErrors(err,"sph");

   t0=fFitter->GetParameter(5);
   NcTimestamp t0fit((NcTimestamp)(*fEvt));
   t0fit.Add(0,0,int(t0));

   // Enter the fit result as a track in the event structure
   ntkreco++;
   fTkfit->SetId(ntkreco);

   fEvt->AddTrack(fTkfit);
   NcTrack* trk=fEvt->GetIdTrack(ntkreco);
   if (!trk) continue;

   trk->SetCharge(fCharge);
   pos.SetTimestamp(t0fit);
   trk->SetTimestamp(t0fit);
   trk->SetReferencePoint(pos);
   trk->Set3Momentum(p);
   trk->SetFitDetails(fFitstats);
   // Link all the hits used for the fit to the newly created track (and vice versa)
   for (Int_t ihit=0; ihit<fHits->GetEntries(); ihit++)
   {
    NcSignal* sx=(NcSignal*)fHits->At(ihit);
    if (sx) trk->AddSignal(*sx,1);
   }
   // Give the newly created track the proper name
   newname=fTrackname;
   if (newname=="") newname=ClassName();
   newname+="4";
   newname+=trackname;
   trk->SetNameTitle(newname.Data(),tracktitle.Data());
   // Link this newly created track as a hypothesis to the parent first guess track
   track->SetHypCopy(0);
   track->AddTrackHypothesis(*trk);
  } // End loop over tracks
 } // End loop over first guess classes

}
///////////////////////////////////////////////////////////////////////////
void IcePandel::FitFCN(Int_t&,Double_t*,Double_t& f,Double_t* x,Int_t)
{
/**
~~~
// Minimisation of the Bayesian psi value for a track w.r.t. a Convoluted Pandel PDF.
// The Baysian psi value is defined as -loglikelihood in a decibel scale.
// This implies psi=-10*log10(L) where L=p(D|HI) being the likelihood of
// the data D under the hypothesis H and prior information I.
~~~
**/

 const Float_t c=0.299792458;        // Light speed in vacuum in meters per ns
 const Float_t npice=1.31768387;     // Phase refractive index (c/v_phase) of ice
 const Float_t ngice=1.35075806;     // Group refractive index (c/v_group) of ice
 const Float_t thetac=acos(1./npice);// Cherenkov angle (in radians)
 const Float_t cice=c/ngice;         // Light speed in ice in meters per ns
 const Float_t tau=557;
 const Double_t pi=acos(-1.);

 f=0;

 // The new r0 and p vectors and t0 from the minimisation
 Float_t vec[3];
 
 NcPosition r0;
 vec[0]=x[0];
 vec[1]=x[1];
 vec[2]=x[2];
 r0.SetPosition(vec,"car");

 Nc3Vector p;
 vec[0]=1;
 vec[1]=x[3];
 vec[2]=x[4];
 p.SetVector(vec,"sph");

 Float_t t0=x[5];

 // Construct a track with the new values from the minimisation
 fTkfit->SetReferencePoint(r0);
 fTkfit->Set3Momentum(p);

 Int_t nhits=fHits->GetEntries();

 NcPosition rhit;
 Nc3Vector r12;
 Float_t d,dist,thit,tgeo,proj;
 Float_t alphac;
 Float_t zhit,lambda,labs;
 Double_t sigma ; // PMT time jitter in ns (set per (D)OM)
 Double_t tres,ksi,rho,eta,pandel;
 Double_t cpandel1,cpandel2,cpandel3,cpandel;
 Double_t z,k,alpha,beta,phi,n1,n2,n3,u; // Function parameters for regions 3 and 4
 Int_t ier;
 Double_t psihit=0;
 fPsistats.Reset();
 for (Int_t i=0; i<nhits; i++)
 {
  NcSignal* sx=(NcSignal*)fHits->At(i);
  if (!sx) continue;
  IceGOM* omx=(IceGOM*)sx->GetDevice();
  if (!omx) continue;

  // Angular reduction of complement of thetac due to v_phase and v_group difference
  alphac=0;
  if (omx->InheritsFrom("IceAOM"))
  {
   if (fVgroupA) alphac=atan((1.-npice/ngice)/sqrt(npice*npice-1.));
   sigma=fTsigmaA;
  }
  if (omx->InheritsFrom("IceICDOM"))
  {
   if (fVgroupIC || fVgroupI) alphac=atan((1.-npice/ngice)/sqrt(npice*npice-1.));
   sigma=fTsigmaIC;
  }
  if (omx->InheritsFrom("IceDCDOM"))
  {
   if (fVgroupDC || fVgroupI) alphac=atan((1.-npice/ngice)/sqrt(npice*npice-1.));
   sigma=fTsigmaDC;
  }

  // The hit position dependent scattering and absorption length
  if (omx->InheritsFrom("IceAOM")) // Amanda reconstruction
  {
   lambda=fLambdaA;
   labs=fLabsA;
  }
  else // IceCube reconstruction
  {
   lambda=fLambdaDL; // The ice at the Dust Layer
   labs=fLabsDL;
   zhit=omx->GetX(3,"car");
   if (zhit>-50) // The Ice in the Upper Detector above the dustlayer
   {
    lambda=fLambdaUD;
    labs=fLabsUD;
   }
   if (zhit<-150) // Clearest Ice in the Lower Detector under the dustlayer
   {
    lambda=fLambdaLD;
    labs=fLabsLD;
   }
  }

  rhit=omx->GetPosition();
  d=fTkfit->GetDistance(rhit);
  ksi=d/(sin(thetac)*lambda);
  r12=rhit-r0;
  proj=p.Dot(r12);
  dist=fabs(proj)+d/tan(pi/2.-thetac-alphac);
  if (proj<0) dist=-dist;
  tgeo=t0+dist/c;
  thit=sx->GetSignal("LE",7);
  tres=thit-tgeo;

  // The Convoluted Pandel function evaluation
  // For definitions of functions and validity regions, see the
  // CPandel writeup of O. Fadiran, G. Japaridze and N. van Eijndhoven

  // Move points which are outside the validity rectangle in the (tres,ksi) space
  // to the edge of the validity rectangle and signal the use of the penalty
  ier=0;
  if (tres<-25.*sigma)
  {
   tres=-25.*sigma;
   ier=1;
  }
  if (tres>3500)
  {
   tres=3500;
   ier=1;
  }
  if (ksi>50)
  {
   ksi=50;
   ier=1;
  }

  rho=((1./tau)+(cice/labs));
  eta=(rho*sigma)-(tres/sigma);

  if (ksi<=0) // The zero distance (ksi=0) axis
  {
   cpandel=exp(-tres*tres/(2.*sigma*sigma))/(sigma*sqrt(2.*pi));
  }
  else if (ksi<=5 && tres>=-5.*sigma && tres<=30.*sigma) // The exact expression in region 1
  {
   cpandel1=pow(rho,ksi)*pow(sigma,ksi-1.)*exp(-tres*tres/(2.*sigma*sigma))/pow(2.,0.5*(1.+ksi));
   cpandel2=ROOT::Math::conf_hyperg(ksi/2.,0.5,eta*eta/2.)/TMath::Gamma((ksi+1.)/2.);
   cpandel3=sqrt(2.)*eta*ROOT::Math::conf_hyperg((ksi+1.)/2.,1.5,eta*eta/2.)/TMath::Gamma(ksi/2.);

   cpandel=cpandel1*(cpandel2-cpandel3);
  }
  else if (ksi<=1 && tres>30.*sigma && tres<=3500) // Approximation in region 2
  {
   pandel=pow(rho,ksi)*pow(tres,(ksi-1.))*exp(-rho*tres)/TMath::Gamma(ksi);

   cpandel=exp(rho*rho*sigma*sigma/2.)*pandel;
  }
  else if (ksi<=1 && tres<-5.*sigma && tres>=-25.*sigma) // Approximation in region 5
  {
   cpandel=pow(rho*sigma,ksi)*pow(eta,-ksi)*exp(-tres*tres/(2.*sigma*sigma))/(sigma*sqrt(2.*pi));
  }
  else if (ksi<=50 && eta<=0 && tres<=3500) // Approximation in region 3
  {
   z=-eta/sqrt(4.*ksi-2.);
   k=0.5*(z*sqrt(1.+z*z)+log(z+sqrt(1.+z*z)));
   alpha=-tres*tres/(2.*sigma*sigma)+eta*eta/4.-ksi/2.+0.25+k*(2.*ksi-1.);
   alpha+=-log(1.+z*z)/4.-ksi*log(2.)/2.+(ksi-1.)*log(2.*ksi-1.)/2.+ksi*log(rho)+(ksi-1.)*log(sigma);
   beta=0.5*(z/sqrt(1.+z*z)-1.);
   n1=beta*(20.*beta*beta+30.*beta+9.)/12.;
   n2=pow(beta,2.)*(6160.*pow(beta,4.)+18480.*pow(beta,3.)+19404.*pow(beta,2.)+8028.*beta+945.)/288.;
   n3=27227200.*pow(beta,6.)+122522400.*pow(beta,5.)+220540320.*pow(beta,4.);
   n3+=200166120.*pow(beta,3.)+94064328.*pow(beta,2.)+20546550.*beta+1403325.;
   n3*=pow(beta,3.)/51840.;
   phi=1.-n1/(2.*ksi-1.)+n2/pow(2.*ksi-1.,2.)-n3/pow(2.*ksi-1.,3.);
   cpandel=exp(alpha)*phi/TMath::Gamma(ksi);
  }
  else if (ksi<=50 && eta>=0 && tres>=-25.*sigma) // Approximation in region 4
  {
   z=eta/sqrt(4.*ksi-2.);
   k=0.5*(z*sqrt(1.+z*z)+log(z+sqrt(1.+z*z)));
   u=exp(ksi/2.-0.25)*pow(2.*ksi-1.,-ksi/2.)*pow(2.,(ksi-1.)/2.);
   beta=0.5*(z/sqrt(1.+z*z)-1.);
   n1=beta*(20.*beta*beta+30.*beta+9.)/12.;
   n2=pow(beta,2.)*(6160.*pow(beta,4.)+18480.*pow(beta,3.)+19404.*pow(beta,2.)+8028.*beta+945.)/288.;
   n3=27227200.*pow(beta,6.)+122522400.*pow(beta,5.)+220540320.*pow(beta,4.);
   n3+=200166120.*pow(beta,3.)+94064328.*pow(beta,2.)+20546550.*beta+1403325.;
   n3*=pow(beta,3.)/51840.;
   phi=1.+n1/(2.*ksi-1.)+n2/pow(2.*ksi-1.,2.)+n3/pow(2.*ksi-1.,3.);
   cpandel=pow(rho,ksi)*pow(sigma,ksi-1.)*exp(-pow(tres,2.)/(2.*pow(sigma,2.))+pow(eta,2.)/4.)/sqrt(2.*pi);
   cpandel*=u*phi*exp(-k*(2.*ksi-1.))*pow(1.+z*z,-0.25);
  }
  else // (tres,ksi) outside validity rectangle
  {
   ier=1;
   cout << " *IcePandel::FitFCN* Outside rectangle. We should never get here." << endl;
  }

  // Use 10*log10 expression to obtain intuitive dB scale
  // Omit (small) negative values which are possible due to computer accuracy
  psihit=0;
  if (cpandel>0) psihit=-10.*log10(cpandel);

  // Penalty in dB for (tres,ksi) points outside the validity rectangle
  if (ier) psihit+=fPenalty; 

  // Update the psi statistics for this hit
  fPsistats.Enter(float(psihit));
  f+=psihit;
 }
}
///////////////////////////////////////////////////////////////////////////
