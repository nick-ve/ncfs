/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997, NCFS/IIHE, All Rights Reserved.                          *
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
// Class NcAstrolab
// Virtual lab to provide (astro)physical parameters, treat data and relate observations with astrophysical phenomena
//
// This class is derived from TTask, but the only reason for this
// is to enable this class to serve as a base class for other TTask
// derived classes (e.g. NcEventSelector) without the need for
// multiple virtual inheritance.
// So, this NcAstrolab class itself does not provide any TTask
// related functionality.
//
// The lab can be given a terrestrial location via SetLabPosition()
// using the usual longitude and latitude specifications.
// Since this class is derived from NcTimestamp, a lab can also be
// given a specific timestamp. Together with the terrestrial location
// this provides access to local (sidereal) times etc...
// In addition to the usual astronomical reference frames, a local
// lab reference frame can also be specified via SetLocalFrame().
// Together with the lab's timestamp this uniquely defines all the
// coordinate transformations between the various reference frames.
// These lab characteristics allow space and time correlations of lab
// observations with external (astrophysical) phenomena.
//
// By default, the lab is located at the North Pole using UTC
// and the right handed local reference frame has the following convention :
// X-axis pointing South (=Greenwich)
// Y-axis pointing East
// Z-axis pointing towards Zenith
//
// The location and local reference frame convention of several well
// known experimental sites can be automatically obtained via
// the memberfunction SetExperiment().
//
// Observations are entered as generic signals containing a position,
// reference frame specification and a timestamp (see SetSignal).
// These observations can then be analysed in various reference frames
// via the available GET functions.
//
// Various external (astrophysical) phenomena may be entered as
// so-called reference signals.
// This NcAstrolab class provides facilities (e.g. MatchSignals) to check
// correlations of the stored measurements with these reference signals.
// The memberfunction SetTimeScramble provides a facility to scramble
// the timestamp of an observation or time differences with reference signals.
// The memberfunction SetPositionScramble provides a facility to scramble
// the location of an observation or angular differences with reference signals.
// These facilities will enable a background c.q. blind analysis in studying
// correlations with external (astrophysical) phenomena.
//
// The member function sequence InitDataNames(), SetDataNames() and LoadInputData()
// provides a generic facility to load both observations and reference signals
// from ROOT data files in Tree format.
// This provides a convenient way to load e.g. experimental observations or (GCN) catalog data.
//
// This NcAstrolab class also provides a facility to model c.q. perform the
// analysis of transient phenomena by means of (stacked) time series observations.
// It reflects the observation of neutrinos based on gamma/GW burst alerts, but the
// procedure may of course be used for any kind of messenger particles.
// For a worked out example, please refer to the macro "burst-model.cc"
// in the folder "/ncfs/physics/macros".
//
// By default different random sequences are ensured for different NcAstrolab instances,
// even with identical stored measurements and reference signals.  
// In case the user wants to control the random sequence (e.g. to obtain reproducible results),
// the randomiser has to be explicitly initialised via the member function SetRandomiser().
// 
// Also graphical facilities (e.g. DisplaySignals) are available to
// provide skymaps in various projections.
// A Graphical User Interface (GUI) is available via the memberfunction SkyMapPanel(),
// which facilitates several functionalities in a user friendly graphical environment.
// 
// Coding example :
// ----------------
// gSystem->Load("ncfspack");
//
// NcAstrolab lab("IceCube","The South Pole Neutrino Observatory");
// lab.SetLabPosition(0,-90,"deg"); // South Pole
//
// // Right handed Amanda/IceCube local frame has Y-North, X-East and Z-Zenith
// lab.SetLocalFrame(90,90,90,180,0,0);
//
// // Note : The above lab initialisation could also have been performed as follows :
// // NcAstrolab lab;
// // lab.SetExperiment("IceCube"); // Set more precise IceCube location and orientation
//
// // Print laboratory parameters
// lab.Data(1,"dms");
//
// // Enter some measurements (e.g. observed events) to be investigated
// NcTimestamp ts;
// ts.SetUT("30-07-1989","08:14:23.74",0);
// lab.SetSignal(1,-18.2,"deg",87.53,"deg","loc",&ts,-1,"T","Event9081",1);
// ts.Add(0,1,87326);
// lab.SetSignal(1,23.8,"deg",208.65,"deg","loc",&ts,-1,"T","Event10372",1);
//
// // Enter some reference signals
// Float_t alpha=194818.0;
// Float_t delta=84400.;
// lab.SetSignal(1,alpha,"hms",delta,"dms","equ",0,-1,"B","Altair");
// alpha=124900.0;
// delta=272400.;
// lab.SetSignal(1,alpha,"hms",delta,"dms","equ",0,-1,"B","NGP");
// alpha=64508.917;
// delta=-164258.02;
// lab.SetSignal(1,alpha,"hms",delta,"dms","equ",0,-1,"J","Sirius");
// alpha=23149.08;
// delta=891550.8;
// lab.SetSignal(1,alpha,"hms",delta,"dms","equ",0,-1,"J","Polaris");
// alpha=43600.;
// delta=163100.;
// lab.SetSignal(1,alpha,"hms",delta,"dms","equ",0,-1,"J","Aldebaran");
// lab.SetSignal(1,182948.53,"hms",380228.45,"dms","equ",0,-1,"J","LyrA");
// lab.SetSignal(1,0,"deg",-90,"deg","gal",0,-1,"M","SGP");
// // Mimic a GRB 5 seconds before the occurrence of Event10372
// ts.Add(0,-5,0);
// lab.SetSignal(1,327.531,"deg",-35.8903,"deg","gal",&ts,-1,"T","GRB890730");
// ts.Add(0,5,0); // Restore the time stamp
//
// // List all stored objects
// cout << endl;
// lab.ListSignals("equ","J",5,"M",-1);
//
// // Search for space and time matches between the measurements and the reference signals
// // within a cone of 20 degrees and a time window of [-60,60] seconds
// NcDevice matches;
// lab.MatchSignals(matches,20,"deg",60,"s");
// matches.Data();
// 
// // Get the minimal encountered angular and time difference
// // (for a more detailed example please refer to the docs of MatchSignals)
// Double_t da=0; // Angular difference
// Double_t dt=0; // Time difference
// Int_t ia=0;    // "Hit" index of the pair with the minimal angular difference 
// Int_t it=0;    // "Hit" index of the pair with the minimal time difference 
// da=matches.GetSignal(1);
// dt=matches.GetSignal(2);
// ia=matches.GetSignal(3);
// it=matches.GetSignal(4);
//
// cout << " Minimal differences damin (deg) : " << da << " dtmin (s) : " << dt << endl;
//
// // The full information for the pair with the minimal time difference
// NcSignal* sx=matches.GetHit(it);
// if (sx) sx->Data();
//
// // Display all stored objects in a skymap (Hammer projection)
// lab.DisplaySignals("equ","J",0,"ham",1);
//
//--- Author: Nick van Eijndhoven 15-mar-2007 Utrecht University
//- Modified: Nick van Eijndhoven, IIHE-VUB Brussel, December 19, 2024  11:58Z
///////////////////////////////////////////////////////////////////////////

#include "NcAstrolab.h"
#include "Riostream.h"
 
ClassImp(NcAstrolab) // Class implementation to enable ROOT I/O
 
NcAstrolab::NcAstrolab(const char* name,const char* title) : TTask(name,title),NcTimestamp()
{
// Default constructor

 fExperiment="User";
 fLabId=0;
 fToffset=0;
 fRefs=0;
 fSigs=0;
 fNen[0]=0;
 fNen[1]=0;
 fBias=0;
 fGal=0;
 fIndices=0;
 fUsMeridian=0;
 fMeridian=0;
 fProj="none";
 fCanvas=0;
 fHist[0]=0;
 fHist[1]=0;
 fMarkers=0;
 fMarkerSize[0]=1.5; 
 fMarkerSize[1]=1;
 fMarkerSize[2]=1.5;
 fMarkerSize[3]=0.3;
 fMarkerStyle[0]=29;
 fMarkerStyle[1]=8;
 fMarkerStyle[2]=34;
 fMarkerStyle[3]=8;
 fMarkerColor[0]=kRed;
 fMarkerColor[1]=kBlue;
 fMarkerColor[2]=kBlack;
 fMarkerColor[3]=kBlack;
 fSkyMapPanel=0;
 fTscmode=0;
 fTscmin=0;
 fTscmax=0;
 fTscfunc=0;
 fRscmode=0;
 fDscmin=0;
 fDscmax=0;
 fDscfunc=0;
 fThetascmin=0;
 fThetascmax=0;
 fThetascfunc=0;
 fPhiscmin=0;
 fPhiscmax=0;
 fPhiscfunc=0;
 fRan=0;
 fMaxDt=-1;
 fSolUpdate=0;

 // Standard values (Particle Data Group 2018) for some (astro)physical parameters
 fSpeedC=299792458;
 fQe=1.602176565e-19;
 fMe=0.510998928;
 fMmu=105.6583715;
 fMtau=1776.82;
 fAmu=931.494061;
 fMp=1.007276466812*fAmu;
 fMn=1.00866491600*fAmu;
 fMW=80.385;
 fGammaW=2.085;
 fMZ=91.1876;
 fGammaZ=2.4952;
 fAlphaEM=1./137.035999074;
 fFermi=1.1663787e-5;
 fPlanck=6.62606957e-34;
 fBoltz=1.3806488e-23;
 fNewton=6.67384e-11;
 fGn=9.80665;
 fAu=1.49597870700e11;
 fPc=3.08567758149e16;
 // Cosmological parameters from the final Planck 2018 results (arXiv:1807.06209)
 fHubble=67.4;
 fOmegaM=0.315;
 fOmegaR=5.38e-5;
 fOmegaL=0.685;
 fOmegaB=0.0492;
 fOmegaC=0.264;

 // Some derived (astro)physical parameters c.q. conversion constants
 fHbar=6.58211928e-22;
 fHbarc=197.3269718;
 fHbarc2=3.89379338e-4;

 // Function to parametrize the Neutrino-Lepton kinematic opening angle
 fNuAngle=0;

 // Specifications of the data from a ROOT input Tree  
 fDataDir=0;
 fDataFrame="undefined";
 fDataMode="undefined";

 // Storage for transient burst investigations
 fBurstParameters=0;

 // Initialize the default values for the burst parameters
 SetBurstParameter("*",0);

 // Load the UTC parameters
 LoadUTCparameterFiles();

 // Set the default local frame convention
 // X-axis pointing South (=Greenwich)
 // Y-axis pointing East
 // Z-axis pointing towards Zenith
 SetLocalFrame(90,0,90,90,0,0);

 // First initialization of the various SkyMapPanel GUI parameters.
 // In case SkyMapPanel() is invoked, the current Lab settings will be imported.
 fSkyMapPanel=0;
 fMapTS.LoadUTCparameterFiles();
 for (Int_t i=0; i<3; i++)
 {
  fMapLabLBI[i]=0;
 }
 fMapLabU=0;
 fMapLabE=0;
 fMapLabLocL=0;
 fMapLabLocB=0;
 fMapLabLocU="deg";
 fMapLabExpName="User";
 fMapLabId=0;
 fMapTSdatetime=0;
 fMapTStimetype=0;
 fMapDate="";
 fMapTime="";
 fMapTimeType="-";
 fMapDateTime="";
 fMapLabTS=kTRUE;
 for (Int_t i=0; i<6; i++)
 {
  fMapLabLframe[i]=0;
 }
 fMapCinfo="Lab";
 fMapTinfo=-1;
 fMapUinfo="deg";
 fMapIname="";
 fMapEa=0;
 fMapEua="deg";
 fMapEb=0;
 fMapEub="deg";
 fMapEtype=1;
 fMapEcoord="-";
 fMapEmode="-";
 fMapEname="";
 fMapDcoord="-";
 fMapProj="-";
 fMapDmode="-";
 for (Int_t i=0; i<5; i++)
 {
  fMapDoptions[i]=kFALSE;
 }
 fMapNmax=-1;
 fMapNdigs=1;
 fMapDname="";
 for (Int_t i=0; i<10; i++)
 {
  fMapSolar[i]=kFALSE;
 }
 fMapMerMode=0;
 fMapMerC=0;
 fMapMerUc="deg";
 fMapMarkSize=1;
 fMapMarkStyle=23;
 fMapMarkColor=kRed;
 fMapMarkType=0;
}
///////////////////////////////////////////////////////////////////////////
NcAstrolab::~NcAstrolab()
{
// Destructor to delete all allocated memory.

 if (fRefs)
 {
  delete fRefs;
  fRefs=0;
 }
 if (fSigs)
 {
  delete fSigs;
  fSigs=0;
 }
 if (fIndices)
 {
  delete fIndices;
  fIndices=0;
 }
 for (Int_t i=0; i<2; i++)
 {
  if (fHist[i])
  {
   delete fHist[i];
   fHist[i]=0;
  }
 }
 if (fMarkers)
 {
  delete fMarkers;
  fMarkers=0;
 }
 if (fCanvas)
 {
  if (gROOT->GetListOfCanvases()->FindObject("NcAstrolab")) delete fCanvas;
  fCanvas=0;
 }
 if (fTscfunc)
 {
  delete fTscfunc;
  fTscfunc=0;
 }
 if (fDscfunc)
 {
  delete fDscfunc;
  fDscfunc=0;
 }
 if (fThetascfunc)
 {
  delete fThetascfunc;
  fThetascfunc=0;
 }
 if (fPhiscfunc)
 {
  delete fPhiscfunc;
  fPhiscfunc=0;
 }
 if (fRan)
 {
  delete fRan;
  fRan=0;
 }

 if (fNuAngle)
 {
  delete fNuAngle;
  fNuAngle=0;
 }

 if (fBurstParameters)
 {
  delete fBurstParameters;
  fBurstParameters=0;
 }

 // Remove the subtasks from the internal TTask list without deleting them
 if (fTasks) fTasks->Clear();
}
///////////////////////////////////////////////////////////////////////////
NcAstrolab::NcAstrolab(const NcAstrolab& t) : TTask(t),NcTimestamp(t)
{
// Copy constructor

 fToffset=t.fToffset;
 fLabPos=t.fLabPos;
 fLabId=t.fLabId;
 fL=t.fL;
 Int_t size=0;
 fRefs=0;
 if (t.fRefs)
 {
  size=t.fRefs->GetSize();
  fRefs=new TObjArray(size);
  for (Int_t i=0; i<size; i++)
  {
   NcSignal* sx=(NcSignal*)t.fRefs->At(i);
   if (sx) fRefs->AddAt(sx->Clone(),i);
  }
 }
 fSigs=0;
 if (t.fSigs)
 {
  size=t.fSigs->GetSize();
  fSigs=new TObjArray(size);
  for (Int_t i=0; i<size; i++)
  {
   NcSignal* sx=(NcSignal*)t.fSigs->At(i);
   if (sx) fSigs->AddAt(sx->Clone(),i);
  }
 }
 fNen[0]=t.fNen[0];
 fNen[1]=t.fNen[1];
 fBias=0;
 fGal=0;
 fIndices=0;
 fMeridian=-999;
 fProj="none";
 fCanvas=0;
 for (Int_t ih=0; ih<2; ih++)
 {
  fHist[ih]=0;
 }
 fMarkers=0;
 for (Int_t i=0; i<4; i++)
 {
  fMarkerSize[i]=t.fMarkerSize[i];
  fMarkerStyle[i]=t.fMarkerStyle[i];
  fMarkerColor[i]=t.fMarkerColor[i];
 }

 fTscmode=0;
 fTscmin=0;
 fTscmax=0;
 fTscfunc=0;
 SetTimeScramble(t.fTscmode,t.fTscmin,t.fTscmax,t.fTscfunc);

 fRscmode=0;
 fDscmin=0;
 fDscmax=0;
 fDscfunc=0;
 fThetascmin=0;
 fThetascmax=0;
 fThetascfunc=0;
 fPhiscmin=0;
 fPhiscmax=0;
 fPhiscfunc=0;
 SetPositionScramble(t.fRscmode,t.fDscmin,t.fDscmax,t.fDscfunc,t.fThetascmin,t.fThetascmax,t.fThetascfunc,t.fPhiscmin,t.fPhiscmax,t.fPhiscfunc);

 fRan=0;
 NcRandom* ran=t.fRan;
 if (ran) fRan=new NcRandom(*ran);

 fMaxDt=t.fMaxDt;

 TF1* fx=t.fNuAngle;
 if (fx) fNuAngle=(TF1*)fx->Clone();

 NcDevice* dx=t.fBurstParameters;
 if (dx) fBurstParameters=(NcDevice*)dx->Clone();
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::Data(Int_t mode,TString u,Bool_t utc)
{
// Provide lab information.
//
// "mode" indicates the mode of the timestamp info (see NcTimestamp::Date).
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//     "dms" : angles provided in ddd:mm:ss.sss
//     "hms" : angles provided in hh:mm:ss.sss
//
// The boolean argument "utc" allows to add UTC and TAI related date/time
// information in case this is available.
// utc = kTRUE  --> Add the UTC and TAI related information
//       kFALSE --> Do not add the UTC and TAI related information
//
// The defaults are mode=1, u="deg" and utc=kTRUE.
 
 TString name=GetName();
 TString title=GetTitle();
 printf(" *%-s::Data*",ClassName());
 if (name!="")  printf(" Name : %-s",name.Data());
 if (title!="") printf(" Title : %-s",title.Data());
 printf("\n");

 Double_t l,b;
 GetLabPosition(l,b,"deg");
 printf(" Position longitude : "); PrintAngle(l,"deg",u,3);
 printf(" latitude : "); PrintAngle(b,"deg",u,3);
 printf(" for detector ID : %-i \n",fLabId);
 printf(" Local user reference frame orientation w.r.t X0=South, Y0=East and Z0=Zenith : \n");
 printf(" (Note : At the Poles (e.g. IceCube) South means the Greenwich meridian) \n");
 TString saxes[3]={"Local X-axis","Local Y-axis","Local Z-axis"};
 for (Int_t i=0; i<5; i+=2)
 {
  printf(" %-s : zenith=",saxes[i/2].Data()); PrintAngle(fAxes[i],"deg",u,3,kTRUE);
  printf(" | phi="); PrintAngle(fAxes[i+1],"deg",u,3,kTRUE);
  printf("\n");
 }
 printf(" Lab time offset w.r.t. UT : "); PrintTime(fToffset,12);
 printf("\n");

 // UT and Local time info
 Date(mode,fToffset);

 // Add the UTC and TAI related date/time information if requested
 if (utc && mode!=4) Date(4);

 if (fTscmode)
 {
  printf(" ------------------ Time scrambling ----------------- \n");
  if (fTscmode<0)
  {
   printf(" *** Scrambling is applied only for off-source patch (background) data generation in MatchBurstData() *** \n");
   printf(" *** The actual stored source and measurement data are not modified *** \n");
   printf("\n");
  }
  if (abs(fTscmode)==1)
  {
   printf(" *** Each obtained time difference will be scrambled (mode %-i) *** \n",fTscmode);
   printf(" *** The angular differences are not affected *** \n");
   printf(" --> Tailored for scrambling entries in a specific time window without affecting \n");
   printf("     the event selection based on angular separation w.r.t. a source. \n");
  }
  if (abs(fTscmode)==2)
  {
   if (fTscmode>0)
   {
    printf(" *** Each measurement is stored with a scrambled timestamp (mode %-i) *** \n",fTscmode);
    printf(" *** The data of the sources are not modified *** \n");
    printf(" *** Off-source (background) data : At each source matching, the measurement is given a new scrambled fake timestamp *** \n");
    printf(" --> Tailored for blind analyses without access to the unblinded measurement data \n");
   }
   else
   {
    printf(" *** At each source matching, the measurement is given a new scrambled fake timestamp (mode %-i) *** \n",fTscmode);
   }
   printf(" --> Possible patterns in the distribution of sources on the sky will stay intact \n");
  }
  if (abs(fTscmode)==3)
  {
   printf(" *** At each measurement matching, the source is retrieved from the storage with a scrambled fake timestamp (mode %-i) *** \n",fTscmode);
   printf(" *** The measurements are not affected --> Detection efficiency is unaltered *** \n"); 
   printf(" --> Both time and angular differences will be scrambled \n"); 
   printf(" --> Possible patterns in the distribution of sources on the sky will be washed out \n");
  }
  TString sx="offsets";
  if (abs(fTscmode)==1) sx="differences";
  printf(" Time %-s are randomly drawn from the interval [%-g,%-g] sec. \n",sx.Data(),fTscmin,fTscmax);
  if (fTscfunc)
  {
   printf(" Randomising TF1 function %-s is used. \n",fTscfunc->GetName());
  }
  else
  {
   printf(" Uniform randomisation is used. \n");
  }
 }

 if (fRscmode)
 {
  printf(" ------------------ Position scrambling ------------------ \n");
  if (fRscmode<0)
  {
   printf(" *** Scrambling is applied only for off-source patch (background) data generation in MatchBurstData() *** \n");
   printf(" *** The actual stored source and measurement data are not modified *** \n");
  }
  else
  {
   printf(" *** The data of the sources and the measurement timestamps are not modified *** \n");
  }
  printf("\n");

  if (abs(fRscmode==1))
  {
   printf(" *** Each obtained angular difference will be scrambled (mode %-i) *** \n",fRscmode);
   printf(" *** The time differences are not affected *** \n");
   printf(" ---> Tailored for scrambling entries within a specific angular range w.r.t. a source,");
   printf("      without affecting the event selection based on the time difference w.r.t. a transient source. \n");
   printf(" Angular differences are randomly drawn from the interval [%-g,%-g] degrees. \n",fDscmin,fDscmax);
   if (fDscfunc)
   {
    printf(" Randomising TF1 function %-s is used. \n",fDscfunc->GetName());
   }
   else
   {
    printf(" Homogeneous solid angle randomisation is used. \n");
   }
  }

  if (fRscmode==2)
  {
    printf(" *** Each measurement is stored with a scrambled fake local position (mode %-i) *** \n",fRscmode);
    printf(" *** Off-source (background) data : At each source matching, the measurement is given a new scrambled fake local position *** \n");
    printf(" --> Tailored for blind analyses without access to the unblinded measurement data \n");
  }

  if (abs(fRscmode)==3 || fRscmode==-2)
  {
   printf(" *** At each source matching, the measurement is given a new scrambled fake local position (mode %-i) *** \n",fRscmode);
  }

  if (abs(fRscmode)>1)
  {
   printf(" The local coordinates (r,theta,phi) of the measurement are modified to (r+dr,theta+dtheta,phi+dphi) with : \n");

   printf(" dr is randomly drawn from the interval [%-g,%-g] \n",fDscmin,fDscmax);
   if (fDscfunc)
   {
    printf(" Randomising TF1 function %-s is used. \n",fDscfunc->GetName());
   }
   else
   {
    printf(" Uniform randomisation is used. \n");
   }

   printf(" dtheta is randomly drawn from the interval [%-g,%-g] degrees. \n",fThetascmin,fThetascmax);
   if (fThetascfunc)
   {
    printf(" Randomising TF1 function %-s is used. \n",fThetascfunc->GetName());
   }
   else
   {
    printf(" Uniform cos(theta) randomisation is used. \n");
   }

   printf(" dphi is randomly drawn from the interval [%-g,%-g] degrees. \n",fPhiscmin,fPhiscmax);
   if (fPhiscfunc)
   {
    printf(" Randomising TF1 function %-s is used. \n",fPhiscfunc->GetName());
   }
   else
   {
    printf(" Uniform phi randomisation is used. \n");
   }
  }
 }

 printf(" ------------------ \n");
 if (fRan)
 {
  Int_t iseed,cnt1,cnt2;
  GetRandomiser(iseed,cnt1,cnt2);
  cout << " *** Current settings of the internal NcRandom randomiser : iseed=" << iseed << " cnt1=" << cnt1 << " cnt2=" << cnt2 << endl;
 }
 else
 {
  cout << " *** The internal NcRandom randomiser is currently not intialised ***" << endl;
  cout << " Automatic initialisation will be performed with the actual timestamp at the first random number request." << endl;
  cout << " This will ensure different random sequences for different NcAstrolab instances." << endl;
  cout << " To obtain reproducible scrambled results, please invoke SetRandomiser() before the first SetSignal() invokation." << endl;
 }
 printf(" ------------------ \n");
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLabPosition(Nc3Vector& p)
{
// Set the lab position in the terrestrial coordinates and its corresponding
// time offset w.r.t. UT.
//
// In the default constructor, the lab has been located at the North Pole using UTC
// and the right handed local reference frame was given the following convention :
// X-axis pointing South (=Greenwich)
// Y-axis pointing East
// Z-axis pointing towards Zenith
//
// In other words : 
// The right handed position reference frame was defined such that the North Pole
// corresponds to a polar angle theta=0 and the Greenwich meridian corresponds
// to an azimuth angle phi=0, with phi increasing eastwards.
//
// See the memberfunction SetExperiment() for an automatic setting of the
// characteristics of several well known experimental sites.

 fLabPos.SetPosition(p);

 // Determine local time offset in fractional hours w.r.t. UT.
 Double_t vec[3];
 p.GetVector(vec,"sph","deg");
 Double_t l=vec[2];
 fToffset=l/15.;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLabPosition(Double_t l,Double_t b,TString u)
{
// Set the lab position in the terrestrial longitude (l) and latitude (b)
// and its corresponding time offset w.r.t. UT.
// Positions north of the equator have b>0, whereas b<0 indicates
// locations south of the equator.
// Positions east of the Greenwich reference meridian have l>0,
// whereas l<0 indicates locations west of the Greenwich reference meridian.
//
// In the default constructor, the lab has been located at the North Pole using UTC
// and the right handed local reference frame was given the following convention :
// X-axis pointing South (=Greenwich)
// Y-axis pointing East
// Z-axis pointing towards Zenith
//
// In other words : 
// The right handed position reference frame was defined such that the North Pole
// corresponds to a polar angle theta=0 and the Greenwich meridian corresponds
// to an azimuth angle phi=0, with phi increasing eastwards.
//
// See the memberfunction SetExperiment() for an automatic setting of the
// characteristics of several well known experimental sites.
//
// The string argument "u" allows to choose between different angular units
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//     "dms" : angles provided in dddmmss.sss
//     "hms" : angles provided in hhmmss.sss
//
// The default is u="deg".

 Double_t r=1,theta=0,phi=0;

 l=ConvertAngle(l,u,"deg");
 b=ConvertAngle(b,u,"deg");

 Double_t offset=90.;

 theta=offset-b;
 phi=l;

 Double_t p[3]={r,theta,phi};
 fLabPos.SetPosition(p,"sph","deg");

 // Local time offset in fractional hours w.r.t. UT.
 fToffset=l/15.;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetExperiment(TString name,Int_t id)
{
// Set position, local frame definition and time offset w.r.t. UT
// for a known experiment as specified via the arguments "name" and "id".
// The (optional) input argument "id" allows to specify a particular
// element of a larger experimental setup, like for instance a specific
// detector station of the RNO-G radio array.
// The value id=0 (which is the default) represents a global location,
// like for instance the center of the experiment (like for IceCube)
// or some generic location (like the Big House for RNO-G).
//
// Currently the supported experiment names are :
//
// User      : Virtual Lab for general use
// Greenwich : The Royal Observatory in the UK
// Amanda    : Antarctic Muon And Neutrino Detector Array at the South Pole
// IceCube   : The IceCube neutrino observatory at the South Pole
// WSRT      : The Westerbork Synthesis Radio Telescope in the Netherlands
// Astron    : The Netherlands Institute for Radio Astronomy in Dwingeloo
// ARA       : The Askaryan Radio Array at the South Pole
// RNO-G     : The Greenland Radio Neutrino Observatory at Summit Station
// ARCA      : The KM3NeT ARCA detector next to Sicily
//
// Notes :
// -------
// 1) The name and title for the NcAstrolab object are updated automatically
//    according to the specified experiment name at invokation of this memberfunction.
//    In case a different name and/or title is required, please use the usual
//    SetNameTitle facility after invokation of this memberfunction.
//
// 2) In case no detector identifiers have been defined for the specified experiment,
//    the identifier "id" will be set to 0.
//    For the experiment "User" the provided detector identifier (id) will always be set.
//    This allows full flexibility for a generic "User" lab.
//
// The default value is id=0.

 fExperiment="User";
 fLabId=0;

 Double_t l=0; // Longitude
 Double_t b=0; // Lattitude

 if (name=="User")
 {
  SetNameTitle("User","Virtual Lab for general use");
  SetLabPosition(0,90,"deg"); // North Pole
  // Right handed local grid frame has X-South (to Greenwich), Y-East and Z-Zenith
  // which is the same as the Master Reference Frame (MRF)
  SetLocalFrame(90,0,90,90,0,0);
  fExperiment=name;
  fLabId=id;
  return;
 }

 if (name=="Greenwich")
 {
  // Exact location : 51d 28' 36.6672" (N) and 0d 0' 1.8000" (W)
  SetNameTitle("Greenwich","The Royal Observatory in the UK");
  l=-0.000500;
  b=51.476852;
  SetLabPosition(l,b,"deg"); // South Pole
  // Right handed Greenwich local grid frame has X-South, Y-East and Z-Zenith
  // which is the same as the Master Reference Frame (MRF)
  SetLocalFrame(90,0,90,90,0,0);
  fExperiment=name;
  return;
 }

 if (name=="Amanda")
 {
  SetNameTitle("Amanda","Antarctic Muon And Neutrino Detector Array");
  SetLabPosition(0,-90,"deg"); // South Pole
  // Right handed Amanda local grid frame has Y-North (to Greenwich), X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
  fExperiment=name;
  return;
 }

 if (name=="IceCube")
 {
  // Exact location : 89d 59' 23.977" (S) and 63d 37' 21.432" (W)
  SetNameTitle("IceCube","The South Pole Neutrino Observatory");
  l=-63.453056;
  b=-89.99;
  SetLabPosition(l,b,"deg"); // South Pole
  // Right handed IceCube local grid frame has Y-North (to Greenwich), X-East and Z-Zenith
  SetLocalFrame(90,90.+l,90,180.+l,0,0);
  fExperiment=name;
  return;
 }

 if (name=="WSRT")
 {
  SetNameTitle("WSRT","The Westerbork Synthesis Radio Telescope");
  SetLabPosition(63612.74,525454.33,"dms");
  // Right handed local grid frame has X-South, Y-East and Z-Zenith
  // which is the same as the Master Reference Frame (MRF)
  SetLocalFrame(90,0,90,90,0,0);
  fExperiment=name;
  return;
 }

 if (name=="Astron")
 {
  SetNameTitle("Astron","The Netherlands Institute for Radio Astronomy");
  SetLabPosition(62346.23,524843.99,"dms");
  // Right handed local grid frame has X-South, Y-East and Z-Zenith
  // which is the same as the Master Reference Frame (MRF)
  SetLocalFrame(90,0,90,90,0,0);
  fExperiment=name;
  return;
 }

 if (name=="ARA")
 {
  SetNameTitle("ARA","The Askaryan Radio Array at the South Pole");
  SetLabPosition(0,-90,"deg"); // South Pole
  // Right handed ARA local grid frame has Y-North (to Greenwich), X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
  fExperiment=name;
  return;
 }

 if (name=="RNO-G")
 {
  SetNameTitle("RNO-G","The Greenland Radio Neutrino Observatory at Summit Station");
  // Use the location of the Big House as global location
  l=-38.4604;
  b=72.57889;

  // The locations of the various stations
  Int_t ids[35]={11,12,13,14,15,16,17,21,22,23,24,25,26,27,33,34,35,36,37,44,45,46,47,54,55,56,57,64,65,66,67,74,75,76,77};
  Float_t ls[35]={-38.5023,-38.4962,-38.4901,-38.4841,-38.4780,-38.4719,-38.4657,-38.4660,-38.4599,-38.4538,-38.4477,-38.4416,-38.4355,-38.4293, /*last is 27*/
                  -38.4175,-38.4114,-38.4053,-38.3991,-38.3930,-38.3751,-38.3689,-38.3627,-38.3566,-38.3388,-38.3326,-38.3264,-38.3202,-38.3025, /*last is 64*/
                  -38.2963,-38.2900,-38.2838,-38.2662,-38.2599,-38.2537,-38.2474};
  Float_t bs[35]={72.58923,72.60009,72.61095,72.62181,72.63267,72.64353,72.65439,72.58741,72.59827,72.60912,72.61998,72.63084,72.64170,72.65256, /*last is 27*/  
                  72.60729,72.61815,72.62901,72.63987,72.65073,72.61631,72.62717,72.63803,72.64889,72.61447,72.62533,72.63618,72.64704,72.61262, /*last is 64*/
                  72.62347,72.63433,72.64518,72.61076,72.62161,72.63247,72.64332};

  // Select a specific station, if requested
  if (id)
  {
   for (Int_t i=0; i<35; i++)
   {
    if (id==ids[i])
    {
     l=ls[i];
     b=bs[i];
     fLabId=id;
     break;
    }
   }
  }

  // Set the selected location
  SetLabPosition(l,b,"deg"); // Summit Station
  // Right handed RNO-G local grid frame has Y-North, X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
  fExperiment=name;
  return;
 }

 if (name=="ARCA")
 {
  SetNameTitle("ARCA","The KM3NeT/ARCA Neutrino detector near Sicily");
  SetLabPosition(155842.25,361748.34,"dms");
  // Right handed ARCA local grid frame has Y-North, X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
  fExperiment=name;
  return;
 }

 cout << " *" << ClassName() << "::SetExperiment* Unsupported experiment name : " << name.Data() << endl;
 printf(" Experiment is set to %-s with detector identifier %-i \n",fExperiment.Data(),fLabId);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLabTimeOffset(Double_t dt)
{
// Set the lab time offset (dt) w.r.t. UT in (fractional) hours.
// Normally one should not use this function, but set the time offset automatically 
// by invokation of the memberfunctions SetPosition() or SetExperiment().
// This memberfunction is meant to investigate (systematic) effects in the case
// of possible timing problems. 

 fToffset=dt;
}
///////////////////////////////////////////////////////////////////////////
NcPosition NcAstrolab::GetLabPosition() const
{
// Provide the lab position in the terrestrial coordinates.
// The right handed position reference frame is defined such that the North Pole
// corresponds to a polar angle theta=0 and the Greenwich meridian corresponds
// to an azimuth angle phi=0, with phi increasing eastwards.

 return fLabPos;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GetLabPosition(Double_t& l,Double_t& b,TString u) const
{
// Provide the lab position in the terrestrial longitude (l) and latitude (b).
// Positions north of the equator have b>0, whereas b<0 indicates
// locations south of the equator.
// Positions east of Greenwich have l>0, whereas l<0 indicates
// locations west of Greenwich.
//
// The string argument "u" allows to choose between different angular units
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="deg".

 Double_t pi=acos(-1.);

 Double_t offset=90.;
 if (u=="rad") offset=pi/2.;

 Double_t p[3];
 fLabPos.GetPosition(p,"sph",u);
 b=offset-p[1];
 l=p[2];
}
///////////////////////////////////////////////////////////////////////////
TString NcAstrolab::GetExperiment() const
{
// Provide the name of the experiment site.

 return fExperiment;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::GetLabDetectorId() const
{
// Provide the detector identifier within the lab.

 return fLabId;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetLabTimeOffset() const
{
// Provide the lab time offset (dt) w.r.t. UT in (fractional) hours.
// This time offset is automatically set by invokation of the memberfunctions
// SetPosition() or SetExperiment().

 return fToffset;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetRandomiser(Int_t iseed,Int_t cnt1,Int_t cnt2,NcTimestamp* ts)
{
// (Re)initialise the internal NcRandom randomisation facility.
//
// Note :
// ------
// This member function provides the user a handle to (re)initialise the internal randomisation
// facility in order to obtain reproducible scrambled results or to define unique random sequences
// for various NcAstrolab instances.
// If not initialised by the user, the randomisation facility is internally automatically "date/time driven"
// initialised by the actual timestamp of the moment the first need for a randomisation arises.
// This will ensure different random sequences for different NcAstrolab instances (if created at least 0.01 sec. apart),
// even in the case of identical stored measurements and reference signals.
// For details about the atomatically generated seed value please refer to the NcRandom documentation.
//
// So, in case there is no need for reproducible scrambled results among NcAstrolab instances that are created
// at time intervals of more than 0.01 second, the user is advised NOT to invoke this member function.
//
// Input arguments :
// -----------------
// iseed >= 0 --> Use this value as seed for the internal NcRandom object and start the
//                random sequence at the point defined by the counters "cnt1" and "cnt2".
//                In this case the timestamp "ts" is irrelevant.
//                For allowed seed values and info on the parameters "cnt1" and "cnt2"
//                please refer to the docs of NcRandom.
//       < 0  --> Use the provided NcTimestamp "ts" to generate a seed for the internal NcRandom object.
//                If ts=0 the current timestamp of this NcAstrolab instance is used.
//                The values of "cnt1" and "cnt2" are irrelevant, since the random sequence will always
//                be started from scratch.
//
// Note :
// ------
// Reproducible scrambled results among different NcAstrolab instances is only possible if they are
// all initialised with the same parameters for the internal randomiser.
// This implies that when initialisation is performed via a timestamp, this timestamp should be the
// same for all the corresponding NcAstrolab instances. This can be obtained via an explicit SetUT()
// invokation of the provided timestamp or NcAstrolab instance.
// By providing a specific iseed>=0 (and optionally "cnt1" and "cnt2") for all NcAstrolab instances,
// reproducibility is automatically obtained if the stored signals are identical.
//
// The default values are cnt1=0, cnt2=0 and ts=0.

 if (!ts) ts=(NcTimestamp*)this;

 if (fRan) delete fRan;
 
 fRan=new NcRandom(iseed,cnt1,cnt2,ts);
}
///////////////////////////////////////////////////////////////////////////
NcRandom* NcAstrolab::GetRandomiser(Int_t& iseed,Int_t& cnt1,Int_t& cnt2) const
{
// Provide the current "iseed", "cnt1" and "cnt2" parameters of the internal NcRandom randomiser.
// For detailed information about these parameters please refer to the docs of NcRandom. 
// The return argument is the pointer to the internal NcRandom object.
// In case no internal randomiser was defined, a pointer value 0 will be returned and
// the parameters will be set to iseed=-1, cnt1=-1 and cnt2=-1.

 iseed=-1;
 cnt1=-1;
 cnt2=-1;

 if (!fRan) return 0;

 iseed=fRan->GetSeed();
 cnt1=fRan->GetCnt1();
 cnt2=fRan->GetCnt2();

 return fRan;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetMaxDt(Double_t s)
{
// Set the maximum time difference (in sec) for returning a timestamp related
// (reference) signal via GetSignal.
//
// Note : In case s<0 no check on the timestamps will be performed and a signal
//        is always returned.
//        
// In the default constructor s=-1 is set. 

 fMaxDt=s;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetMaxDt() const
{
// Provide the maximum time difference (in sec) for returning a timestamp related
// (reference) signal via GetSignal.
//
// Note : In case s<0 no check on the timestamps will be performed and a signal
//        is always returned.

 return fMaxDt;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetLT()
{
// Provide the Lab's local time in fractional hours.
// A mean solar day lasts 24h (i.e. 86400s).
//
// In case a hh:mm:ss format is needed, please use the Convert() facility. 
 
 Double_t h=GetLT(fToffset);
 return h;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetLMST()
{
// Provide the Lab's Local Mean Sidereal Time (LMST) in fractional hours.
// A sidereal day corresponds to 23h 56m 04.09s (i.e. 86164.09s) mean solar time.
// The definition of GMST is such that a sidereal clock corresponds with
// 24 sidereal hours per revolution of the Earth.
// As such, local time offsets w.r.t. UT and GMST can be treated similarly. 
//
// In case a hh:mm:ss format is needed, please use the Convert() facility. 

 Double_t h=GetLMST(fToffset);
 return h;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetLAST()
{
// Provide the Lab's Local Apparent Sidereal Time (LAST) in fractional hours.
// A sidereal day corresponds to 23h 56m 04.09s (i.e. 86164.09s) mean solar time.
// The definition of GMST and GAST is such that a sidereal clock corresponds with
// 24 sidereal hours per revolution of the Earth.
// As such, local time offsets w.r.t. UT, GMST and GAST can be treated similarly. 
//
// In case a hh:mm:ss format is needed, please use the Convert() facility. 

 Double_t h=GetLAST(fToffset);
 return h;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::PrintAngle(Double_t a,TString in,TString out,Int_t ndig,Bool_t align) const
{
// Printing of angles in various formats.
//
// The input argument "a" denotes the angle to be printed. 
// The string arguments "in" and "out" specify the angular I/O formats.
//
// in = "rad" : input angle provided in radians
//      "deg" : input angle provided in degrees
//      "dms" : input angle provided in dddmmss.sss
//      "hms" : input angle provided in hhmmss.sss
//
// out = "rad" : output angle provided in radians
//       "deg" : output angle provided in degrees
//       "dms" : output angle provided in dddmmss.sss
//       "hms" : output angle provided in hhmmss.sss
//
// The argument "ndig" specifies the number of digits for the fractional
// part (e.g. ndig=6 for "dms" corresponds to micro-arcsecond precision).
// Rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.5 on the output, but with ndig=2 it will show 03.47.
// Due to computer accuracy, precision on the pico-arcsecond level may get lost.
//
// The argument "align", when set to kTRUE, ensures that for a certain selected
// output format, all provided output strings will have the same length.
// This will allow the user to provide data in an aligned tabular format.
// When "align" is set to kFALSE, the produced output will be in the most
// compact format.
//
// The defaults are ndig=1 and align=kFALSE.
//
// Note : The angle info is printed without additional spaces or "endline".
//        This allows the print to be included in various composite output formats.

 Double_t b=ConvertAngle(a,in,out);

 if (out=="deg" || out=="rad")
 {
  if (align)
  {
   printf("%*.*f %-s",5+ndig,ndig,b,out.Data());
  }
  else
  {
   printf("%-.*f %-s",ndig,b,out.Data());
  }
  return; 
 }

 Double_t epsilon=1.e-12; // Accuracy in (arc)seconds
 Int_t word=0,ddd=0,hh=0,mm=0,ss=0;
 Double_t s;

 if (out=="dms")
 {
  word=Int_t(b);
  word=abs(word);
  ddd=word/10000;
  word=word%10000;
  mm=word/100;
  ss=word%100;
  s=fabs(b)-Double_t(ddd*10000+mm*100+ss);
  if (s>(1.-epsilon))
  {
   s=0.;
   ss++;
  }
  while (ss>=60)
  {
   ss-=60;
   mm++;
  }
  while (mm>=60)
  {
   mm-=60;
   ddd++;
  }
  while (ddd>=360)
  {
   ddd-=360;
  }
  if (b<0) ddd=-ddd;
  s+=double(ss);
  if (align)
  {
   printf("%4dd %02d' %0*.*f\"",ddd,mm,3+ndig,ndig,s);
  }
  else
  {
   printf("%-dd %-d' %-.*f\"",ddd,mm,ndig,s);
  }
  return;
 }

 if (out=="hms")
 {
  word=Int_t(b);
  word=abs(word);
  hh=word/10000;
  word=word%10000;
  mm=word/100;
  ss=word%100;
  s=fabs(b)-Double_t(hh*10000+mm*100+ss);
  if (s>(1.-epsilon))
  {
   s=0.;
   ss++;
  }
  while (ss>=60)
  {
   ss-=60;
   mm++;
  }
  while (mm>=60)
  {
   mm-=60;
   hh++;
  }
  while (hh>=24)
  {
   hh-=24;
  }
  if (b<0) hh=-hh;
  s+=double(ss);
  if (align)
  {
   printf("%3dh %02dm %0*.*fs",hh,mm,3+ndig,ndig,s);
  }
  else
  {
   printf("%-dh %-dm %-.*fs",hh,mm,ndig,s);
  }
  return;
 }
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::SetSignal(Nc3Vector* r,TString frame,TString mode,NcTimestamp* ts,Int_t jref,TString name,Int_t type)
{
// Internal memberfunction for generic storage of a signal as specified by the position r and the timestamp ts.
// The pointer to the stored signal is returned (0 in case of incompatible data).
// The position is stored in International Celestial Reference System (ICRS) coordinates.
// The ICRS is a fixed, time independent frame and as such provides a unique reference
// frame without the need of specifying any epoch etc...
// The ICRS coordinate definitions match within 20 mas with the mean ones of the J2000.0
// equatorial system. Nevertheless, to obtain the highest accuracy, the slight
// coordinate correction between J2000 and ICRS is performed here via the
// so-called frame bias matrix.
// For further details see the U.S. Naval Observatory (USNO) circular 179 (2005),
// which is available on http://aa.usno.navy.mil/publications/docs/Circular_179.pdf.
//
// The input parameter "frame" allows the user to specify the frame to which
// the components of r refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (d),
//                    where the "sph" components of r correspond to theta=(pi/2)-d and phi=a.
//          "gal" ==> Galactic coordinates with longitude (l) and latitude (b).
//                    where the "sph" components of r correspond to theta=(pi/2)-b and phi=l.
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (l) and latitude (b),
//                    where the "sph" components of r correspond to theta=(pi/2)-b and phi=l.
//          "hor" ==> Horizontal coordinates at the NcAstrolab location, where the "sph"
//                    components of r correspond to theta=zenith angle and phi=pi-azimuth.
//          "icr" ==> ICRS coordinates with longitude (l) and latitude (b),
//                    where the "sph" components of r correspond to theta=(pi/2)-b and phi=l.
//          "loc" ==> Local coordinates at the NcAstrolab location, where the "sph"
//                    components of r correspond to the usual theta and phi angles.
//
// In case the coordinates are the equatorial right ascension and declination (a,d),
// they can represent so-called "mean", "true" and reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,d) are only corrected for precession and not for nutation
// true values  : (a,d) are corrected for both precession and nutation
// epoch values : (a,d) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to specifiy either "mean", "true" or "epoch"
// values for the input in case of equatorial (a,d) coordinates.
//
// mode = "M" --> Input coordinates are the mean values 
//        "T" --> Input coordinates are the true values 
//        "B" --> Besselian (B1950) reference epoch coordinates 
//        "J" --> Julian (J2000) reference epoch coordinates 
//
// The input parameters "jref" and "type" allow the user to store measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Storage of a signal at the j-th position (j=1 is first)
//      < 0 --> Add a signal at the next available position
//
// type = 0 --> The entered data is a reference signal
//        1 --> The entered data is a measured signal
//
// Note : For backward compatibility jref=0 invokes the storage of a single measurement.
//        When invoked with jref=0 all other stored measurements will be lost.
//        However, the stored reference signals are not affected.
//        The user is advised not to use this obsolete jref=0 facility anymore. 
//
// Via the input argument "name" the user can give the stored signal also a name.
// If no name is provided, a name will be generated based on the type and the storage index,
// like for instance Meas8# or Ref5#.
//
// Notes :
// -------
// 1) In case ts=0 the current timestamp of the lab will be taken.
// 2) In case scrambling has been activated, the data will be scrambled accordingly.

 // Cope with the (obsolete) jref=0 specification
 if (!jref)
 {
  type=1;
  jref=1;
  delete fSigs;
  fSigs=0;
 }
 
 if (!r) return 0;

 if (!r->HasVector()) return 0;

 if (frame!="equ" && frame!="gal" && frame!="ecl" && frame!="hor" && frame!="icr" && frame!="loc") return 0;

 if (frame=="equ" && mode!="M" && mode!="m" && mode!="T" && mode!="t" && mode!="B" && mode!="b" && mode!="J" && mode!="j") return 0;

 NcSignal* sx=0;
 
 if (!ts) ts=(NcTimestamp*)this;

 // Make a local copy of the timestamp to make sure that the newly stored
 // signal will never contain the Tree with the UTCparameter data.
 NcTimestamp ts2;
 Int_t mjd=0;
 Int_t isec=0;
 Int_t ins=0;
 Int_t ips=0;
 ts->GetMJD(mjd,isec,ins);
 ips=ts->GetPs();
 ts2.SetMJD(mjd,isec,ins,ips);

 Double_t vec[3];
 vec[0]=r->GetX(1,"sph","rad");
 vec[1]=r->GetX(2,"sph","rad");
 vec[2]=r->GetX(3,"sph","rad");
 Nc3Vector q;
 q.SetVector(vec,"sph","rad"); 

 // Recursive invokation in case of local coordinates
 if (frame=="loc")
 {
  // Convert to horizontal coordinates
  q=q.GetUnprimed(&fL);

  // Store the signal
  sx=SetSignal(&q,"hor",mode,&ts2,jref,name,type);
  return sx;
 }

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // with identical measurements and reference signals without explicit initialisation
 // of the randomiser by the user at the start of the analysis.
 if (!fRan && type && (fTscmode==2 || fRscmode==2)) fRan=new NcRandom(-1);

 // Local timestamp copy to allow time scrambling
 NcTimestamp tx(ts2);

 // Perform time scrambling (measurements only) if requested
 if (type && fTscmode==2)
 {
  Double_t dt=0;

  // Allow for specific offset studies
  if (fTscmin==fTscmax) dt=fTscmin;

  // Go for randomly scrambled values
  if (fTscfunc)
  {
   if (fTscmax>fTscmin)
   {
    dt=fTscfunc->GetRandom(fTscmin,fTscmax);
   }
  }
  else
  {
   if (fTscmax>fTscmin) dt=fRan->Uniform(fTscmin,fTscmax);
  }
  tx.AddSec(dt);
 }

 // Construct the corresponding ICRS position vector to be stored
 if (frame=="equ")
 {
  // Convert to "mean" values at specified epoch
  if (mode=="T" || mode=="t")
  {
   SetNmatrix(&tx);
   q=q.GetUnprimed(&fN);
  }

  // Convert to "mean" values at J2000
  if (mode=="T" || mode=="t" || mode=="M" || mode=="m")
  {
   SetPmatrix(&tx);
  }
  else
  {
   NcTimestamp te;
   if (mode=="B" || mode=="b") te.SetEpoch(1950,"B");
   if (mode=="J" || mode=="j") te.SetEpoch(2000,"J");
   SetPmatrix(&te);
  }
  q=q.GetUnprimed(&fP);

  // Convert to ICRS values
  if (!fBias) SetBmatrix(); 
  q=q.GetUnprimed(&fB);
 }

 if (frame=="gal")
 {
  // Convert to J2000 equatorial mean coordinates
  if (fGal != 2) SetGmatrix("J");
  q=q.GetUnprimed(&fG);

  // Convert to ICRS values
  if (!fBias) SetBmatrix(); 
  q=q.GetUnprimed(&fB);
 }

 if (frame=="ecl")
 {
  // Convert to mean equatorial values at specified epoch
  SetEmatrix(&tx);
  q=q.GetUnprimed(&fE);

  // Convert to "mean" values at J2000
  SetPmatrix(&tx);
  q=q.GetUnprimed(&fP);

  // Convert to ICRS values
  if (!fBias) SetBmatrix(); 
  q=q.GetUnprimed(&fB);
 }

 if (frame=="hor")
 {
  // Convert to "true" equatorial values at the specified timestamp
  SetHmatrix(&tx);
  q=q.GetUnprimed(&fH);

  // Convert to "mean" values at specified timestamp
  SetNmatrix(&tx);
  q=q.GetUnprimed(&fN);

  // Convert to "mean" values at J2000
  SetPmatrix(&tx);
  q=q.GetUnprimed(&fP);

  // Convert to ICRS values
  if (!fBias) SetBmatrix(); 
  q=q.GetUnprimed(&fB);
 }
 
 // Store the signal in ICRS coordinates
 Int_t size=0;
 Int_t jmax=0;
 Int_t jlast=0;
 if (type) // Storage of a measurement
 {
  NcSignal* sxsig=0;
  if (!fSigs) 
  {
   fSigs=new TObjArray();
   fSigs->SetOwner();
  }
  // Expand array size if needed
  size=fSigs->GetSize();
  jmax=size-1;
  jlast=fSigs->GetLast();
  if (jref>0)
  {
   if (jref>size) fSigs->Expand(jref);
  }
  else
  {
   if (jlast==jmax) fSigs->Expand(size+1);
  }
  sxsig=GetSignal(jref,type);
  if (!sxsig)
  {
   sxsig=new NcSignal();
  }
  else
  {
   sxsig->Reset(1);
  }
  if (name=="") // Generate a corresponding measurement name
  {
   fNen[1]++;
   name="Meas";
   name+=fNen[1];
   name+="#";
  }
  sxsig->SetName(name);
  sxsig->SetTitle("Observed event in ICRS coordinates");
  sxsig->SetTimestamp(tx);
  sxsig->SetPosition(q);
  if (jref<0)
  {
   fSigs->Add(sxsig);
  }
  else
  {
   fSigs->AddAt(sxsig,jref-1);
  }
  sx=sxsig;
 }
 else // Storage of a reference signal
 {
  NcSignal* sxref=0;
  if (!fRefs) 
  {
   fRefs=new TObjArray();
   fRefs->SetOwner();
  }
  // Expand array size if needed
  size=fRefs->GetSize();
  jmax=size-1;
  jlast=fRefs->GetLast();
  if (jref>0)
  {
   if (jref>size) fRefs->Expand(jref);
  }
  else
  {
   if (jlast==jmax) fRefs->Expand(size+1);
  }
  sxref=GetSignal(jref,type);
  if (!sxref)
  {
   sxref=new NcSignal();
  }
  else
  {
   sxref->Reset(1);
  }
  if (name=="") // Generate a corresponding reference name
  {
   fNen[0]++;
   name="Ref";
   name+=fNen[0];
   name+="#";
  }
  sxref->SetName(name);
  sxref->SetTitle("Reference event in ICRS coordinates");
  sxref->SetTimestamp(tx);
  sxref->SetPosition(q);
  if (jref<0)
  {
   fRefs->Add(sxref);
  }
  else
  {
   fRefs->AddAt(sxref,jref-1);
  }
  sx=sxref;
 }

 if (fRscmode !=2 || !type) return sx;

 //////////////////////////////////////////////////////////////////
 // Perform position scrambling (measurements only) if requested //
 //////////////////////////////////////////////////////////////////

 // Get the measurement in local coordinates
 Int_t index=fSigs->IndexOf(sx);
 index++; // First storage is at index=1 and not index=0
 GetSignal(q,"loc",mode,&tx,index,type);

 q.GetVector(vec,"sph","deg"); 

 Double_t dd=0;
 Double_t dtheta=0;
 Double_t dphi=0;

 // Allow specific offset studies
 if (fDscmin==fDscmax) dd=fDscmin;
 if (fThetascmin==fThetascmax) dtheta=fThetascmin;
 if (fPhiscmin==fPhiscmax) dphi=fPhiscmin;

 // Go for randomly scrambled values
 if (fDscfunc)
 {
  if (fDscmax>fDscmin)
  {
   dd=fDscfunc->GetRandom(fDscmin,fDscmax);
  }
 }
 else 
 {
  if (fDscmax>fDscmin) dd=fRan->Uniform(fDscmin,fDscmax);
 }

 if (fThetascfunc)
 {
  if (fThetascmax>fThetascmin)
  {
   dtheta=fThetascfunc->GetRandom(fThetascmin,fThetascmax);
  }
 }
 else if (fThetascmax>fThetascmin)
 {
  Double_t pi=acos(-1.);
  Float_t cosmin=cos(fThetascmin*pi/180.);
  Float_t cosmax=cos(fThetascmax*pi/180.);
  if (cosmin>cosmax)
  {
   Float_t temp=cosmin;
   cosmin=cosmax;
   cosmax=temp;
  }
  Double_t cosang=fRan->Uniform(cosmin,cosmax);
  dtheta=acos(cosang)*180./pi;
 }

 if (fPhiscfunc)
 {
  if (fPhiscmax>fPhiscmin)
  {
   dphi=fPhiscfunc->GetRandom(fPhiscmin,fPhiscmax);
  }
 }
 else
 {
  if (fPhiscmax>fPhiscmin) dphi=fRan->Uniform(fPhiscmin,fPhiscmax);
 }

 vec[0]+=dd;
 if (vec[0]<=0) vec[0]=1e-20; // Keep a physical situation
 vec[1]+=dtheta;
 vec[2]+=dphi;
 q.SetVector(vec,"sph","deg");

 ///////////////////////////////////////////////////////////////////
 // Construct the corresponding ICRS position vector to be stored //
 ///////////////////////////////////////////////////////////////////

 // Convert to horizontal coordinates
 q=q.GetUnprimed(&fL);

 // Convert to "true" equatorial values at the specified timestamp
 SetHmatrix(&tx);
 q=q.GetUnprimed(&fH);

 // Convert to "mean" values at specified timestamp
 SetNmatrix(&tx);
 q=q.GetUnprimed(&fN);

 // Convert to "mean" values at J2000
 SetPmatrix(&tx);
 q=q.GetUnprimed(&fP);

 // Convert to ICRS values
 if (!fBias) SetBmatrix(); 
 q=q.GetUnprimed(&fB);

 // Store the measurement position
 sx->SetPosition(q);

 return sx;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::SetSolarSystem(TString name,NcTimestamp* ts,Int_t type)
{
// Internal memberfunction to set c.q. update coordinates for solar system objects
// according to timestamp "ts".
// Due to the movement of the Earth around the Sun, the apparent coordinates
// of solar system objects change more rapidly than those of more distant objects.
// All locations are evaluated in geocentric ecliptic coordinates.   
// For further details see the U.S. Naval Observatory (USNO) circular 179 (2005),
// which is available on http://aa.usno.navy.mil/publications/docs/Circular_179.pdf.
//
// name : Name of the solar system object.
// ts   : Timestamp used to evaluate the position of the specified object.
// type : 0 --> The object data will be searched c.q. entered among the reference signals
//        1 --> The object data will be searched c.q. entered among the measured signals
//
// The return argument indicates whether a solar sytem object has been set/updated (1) or not (0).
//
// All geocentric name specifications as indicated in the docs of NcTimestamp::Almanac()
// are supported here.
//
// Note : In case ts=0 the current timestamp of this NcAstrolab instance will be taken.
//
// The default value is type=0 for backward compatibility.

 // Only geocentric positions are allowed
 if (name.Contains("*")) return 0;

 if (!ts) ts=(NcTimestamp*)this;

 Double_t lx=0; // Geocentric ecliptic longitude of the object in degrees
 Double_t bx=0; // Geocentric ecliptic latitude of the object in degrees
 Double_t rx=0; // Distance (AU for planets, km for the Moon) between the object and the Earth.

 Int_t set=0; // Flag to indicate that a location has been set

 ts->Almanac(0,0,0,0,name,&lx,&bx,&rx);

 if (rx>0) set=1;

 // Replace c.q. store the object data as a reference or measured signal according to "type".
 // In case the object wasn't stored yet, jref=-1 and the object will be 
 // added to the list of stored signals of "type".
 Int_t jref=GetSignalIndex(name,type);
 if (set && jref) SetSignal(rx,lx,"deg",bx,"deg","ecl",ts,jref,"M",name,type);

 return set;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::SetSignal(Double_t d,Double_t a,TString au,Double_t b,TString bu,TString frame,NcTimestamp* ts,Int_t jref,TString mode,TString name,Int_t type)
{
// Store a signal as specified by the distance d, angular position (a,b) and the timestamp ts.
// The pointer to the stored signal is returned (0 in case of incompatible data).
// For angular celestial positions it is quite common to use unit vectors, i.e. d=1.
// However, via the input argument "d" an actual distance, redshift, Lorentz factor or any other
// positive scalar observable related to the observation can be specified here and retrieved later.
// The position is stored in International Celestial Reference System (ICRS) coordinates.
// The ICRS is a fixed, time independent frame and as such provides a unique reference
// frame without the need of specifying any epoch etc...
// The ICRS coordinate definitions match within 20 mas with the mean ones of the J2000.0
// equatorial system. Nevertheless, to obtain the highest accuracy, the slight
// coordinate correction between J2000 and ICRS is performed here via the
// so-called frame bias matrix.
// For further details see the U.S. Naval Observatory (USNO) circular 179 (2005),
// which is available on http://aa.usno.navy.mil/publications/docs/Circular_179.pdf.
//
// The string input arguments "au" and "bu" allow the user to specifiy the units in which
// the angular coordinates "a" and "b" are provided. The convention is the following :
//
// string = "rad" : input angle provided in radians
//          "deg" : input angle provided in degrees
//          "dms" : input angle provided in dddmmss.sss
//          "hms" : input angle provided in hhmmss.sss
//          "hrs" : input angle provided in fractional hours
//
// The input parameter "frame" allows the user to specify the frame to which
// the angular coordinates (a,b) refer. Available options are :
//
//  frame = "equ"  ==> Equatorial coordinates with right ascension (a) and declination (b).
//          "gal"  ==> Galactic coordinates with longitude (a) and latitude (b).
//          "ecl"  ==> Geocentric ecliptic coordinates with longitude (a) and latitude (b).
//          "hor"  ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle (a) and zenith angle (b).
//          "icr"  ==> ICRS coordinates with longitude (a) and latitude (b).
//          "loc"  ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta (a) and phi (b).
//          "pdir" ==> Particle momentum direction at the NcAstrolab location in local coordinates theta (a) and phi (b).
//
// In case the coordinates are the equatorial right ascension and declination (a,b),
// they can represent so-called "mean", "true" or reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,b) are only corrected for precession and not for nutation
// true values  : (a,b) are corrected for both precession and nutation
// epoch values : (a,b) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to specifiy either "mean", "true" or "epoch"
// values for the input in case of equatorial (a,b) coordinates.
//
// mode = "M" --> Input coordinates are the mean values 
//        "T" --> Input coordinates are the true values
//        "B" --> Besselian (B1950) reference epoch coordinates 
//        "J" --> Julian (J2000) reference epoch coordinates 
//
// The input parameters "jref" and "type" allow the user to store measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Storage of a signal at the j-th position (j=1 is first)
//      < 0 --> Add a signal at the next available position
//
// type = 0 --> The entered data is a reference signal
//        1 --> The entered data is a measured signal
//
// Note : For backward compatibility jref=0 invokes the storage of a single measurement.
//        When invoked with jref=0 all other stored measurements will be lost.
//        However, the stored reference signals are not affected.
//        The user is advised not to use this obsolete jref=0 facility anymore. 
//
// Via the input argument "name" the user can give the stored signal also a name.
//
// The default values are mode="T", name="" and type=0.
//
// Notes :
// -------
// 1) In case ts=0 the current timestamp of the lab will be taken.
// 2) In case scrambling has been activated, the data will be scrambled accordingly.

 // Assure physical value for the norm of the location vector 
 if (d<=0) d=1;

 // Convert angular coordinates to fractional degrees.
 a=ConvertAngle(a,au,"deg");
 b=ConvertAngle(b,bu,"deg");

 Nc3Vector r;
 Double_t vec[3]={0,0,0};
 vec[0]=d;

 // Equatorial coordinates
 if (frame=="equ")
 {
  if (mode!="M" && mode!="m" && mode!="T" && mode!="t" && mode!="B" && mode!="b" && mode!="J" && mode!="j") return 0;
  vec[1]=90.-b;
  vec[2]=a;
 }

 // Galactic coordinates
 if (frame=="gal")
 {
  vec[1]=90.-b;
  vec[2]=a;
 }

 // Geocentric ecliptic coordinates
 if (frame=="ecl")
 {
  vec[1]=90.-b;
  vec[2]=a;
 }

 // Horizontal coordinates
 if (frame=="hor")
 {
  vec[1]=b;
  vec[2]=180.-a;
 }

 // ICRS coordinates
 if (frame=="icr")
 {
  vec[1]=90.-b;
  vec[2]=a;
 }

 // Local coordinates
 if (frame=="loc" || frame=="pdir")
 {
  vec[1]=a;
  vec[2]=b;
 }

 if (!ts) ts=(NcTimestamp*)this;

 r.SetVector(vec,"sph","deg");

 // Revert momentum direction in order to get the source direction 
 if (frame=="pdir")
 {
  r*=-1.;
  frame="loc";
 }

 NcSignal* sx=SetSignal(&r,frame,mode,ts,jref,name,type);
 return sx;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::SetSignal(Double_t d,Double_t a,TString au,Double_t b,TString bu,TString frame,TString s,Double_t e,Int_t jref,TString mode,TString name,Int_t type)
{
// Store a signal as specified by the distance d, angular position (a,b) and the specified epoch.
// The pointer to the stored signal is returned (0 in case of incompatible data).
// For angular celestial positions it is quite common to use unit vectors, i.e. d=1.
// However, via the input argument "d" an actual distance, redshift, Lorentz factor or any other
// positive scalar observable related to the observation can be specified here and retrieved later.
// The position is stored in International Celestial Reference System (ICRS) coordinates.
// The ICRS is a fixed, time independent frame and as such provides a unique reference
// frame without the need of specifying any epoch etc...
// The ICRS coordinate definitions match within 20 mas with the mean ones of the J2000.0
// equatorial system. Nevertheless, to obtain the highest accuracy, the slight
// coordinate correction between J2000 and ICRS is performed here via the
// so-called frame bias matrix.
// For further details see the U.S. Naval Observatory (USNO) circular 179 (2005),
// which is available on http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.
//
// The string input arguments "au" and "bu" allow the user to specifiy the units in which
// the angular coordinates "a" and "b" are provided. The convention is the following :
//
// string = "rad" : input angle provided in radians
//          "deg" : input angle provided in degrees
//          "dms" : input angle provided in dddmmss.sss
//          "hms" : input angle provided in hhmmss.sss
//          "hrs" : input angle provided in fractional hours
//
// The input parameter "frame" allows the user to specify the frame to which
// the angular coordinates (a,b) refer. Available options are :
//
//  frame = "equ"  ==> Equatorial coordinates with right ascension (a) and declination (b).
//          "gal"  ==> Galactic coordinates with longitude (a) and latitude (b).
//          "ecl"  ==> Geocentric ecliptic coordinates with longitude (a) and latitude (b).
//          "hor"  ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle (a) and zenith angle (b).
//          "icr"  ==> ICRS coordinates with longitude (a) and latitude (b).
//          "loc"  ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta (a) and phi (b).
//          "pdir" ==> Particle momentum direction at the NcAstrolab location in local coordinates theta (a) and phi (b).
//
// The time of the observation is specified by the following epoch input arguments :
//
//  s = "B" --> Besselian reference epoch.
//      "J" --> Julian reference epoch.
//  e : (Fractional) epoch to specify the timestamp of the observation (e.g. 1952.004353 or 2003.80953216)
//
// In case the coordinates are the equatorial right ascension and declination (a,b),
// they can represent so-called "mean", "true" or reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,b) are only corrected for precession and not for nutation
// true values  : (a,b) are corrected for both precession and nutation
// epoch values : (a,b) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to specifiy either "mean", "true" or "epoch"
// values for the input in case of equatorial (a,b) coordinates.
//
// mode = "M" --> Input coordinates are the mean values 
//        "T" --> Input coordinates are the true values 
//        "B" --> Besselian (B1950) reference epoch coordinates 
//        "J" --> Julian (J2000) reference epoch coordinates 
//
// The input parameters "jref" and "type" allow the user to store measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Storage of a signal at the j-th position (j=1 is first)
//      < 0 --> Add a signal at the next available position
//
// type = 0 --> The entered data is a reference signal
//        1 --> The entered data is a measured signal
//
// Note : For backward compatibility jref=0 invokes the storage of a single measurement.
//        When invoked with jref=0 all other stored measurements will be lost.
//        However, the stored reference signals are not affected.
//        The user is advised not to use this obsolete jref=0 facility anymore. 
//
// Via the input argument "name" the user can give the stored signal also a name.
//
// The default values are name="" and type=0.
//
// Note : In case scrambling has been activated, the data will be scrambled accordingly.

 NcTimestamp tx;
 tx.SetEpoch(e,s);
 
 NcSignal* sx=SetSignal(d,a,au,b,bu,frame,&tx,jref,mode,name,type);
 return sx;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::GetNRefSignals(Int_t mode) const
{
// ************************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility. ***
// *** The user is advised to use the memberfunction GetNsignals() instead.         ***
// ************************************************************************************
//
// Provide the (max. index) number of stored reference signals.
//
// Note : It might be that a user may have removed a reference signal
//        from the storage without compressing the storage array
//        (e.g. for maintaining the original reference signal indices).
//        In such a case the actual storage array may contain reference
//        signals at index values which are larger than the number of
//        actually stored reference signals.
//        The input argument "mode" may be used to obtain this max. index
//        value instead of the number actually stored reference signals.
//
// mode : 0 --> Return the number of actually stored reference signals.
//        1 --> Return the max. index value for stored reference signals
//              (which actually is just the size of the storage array). 
//
// The default value is mode=0.

 Int_t n=GetNsignals(0,mode);
 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::GetNsignals(Int_t type,Int_t mode) const
{
// Provide the (max. index) number of stored signals of type "type".
//
// type = 0 --> The requested number corresponds to the stored reference signals
//        1 --> The requested number corresponds to the stored  measured signals
//
// Note : It might be that a user may have removed a reference signal
//        from the storage without compressing the storage array
//        (e.g. for maintaining the original reference signal indices).
//        In such a case the actual storage array may contain reference
//        signals at index values which are larger than the number of
//        actually stored reference signals.
//        The input argument "mode" may be used to obtain this max. index
//        value instead of the number actually stored reference signals.
//
// mode : 0 --> Return the number of actually stored signals of type "type".
//        1 --> Return the max. index value for stored signals of type "type"
//              (which actually is just the size of the storage array). 
//
// The default value is mode=0.
 
 TObjArray* arr=fRefs;
 if (type) arr=fSigs;

 if (!arr) return 0;

 Int_t n=0;
 if (!mode)
 {
  n=arr->GetEntries();
 }
 else
 {
  n=arr->GetSize();
 }
 return n;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::GetSignal(Nc3Vector& r,TString frame,TString mode,NcTimestamp* ts,Int_t jref,Int_t type)
{
// Internal memberfunction to provide the frame specific coordinates of a signal at the specified timestamp ts.
// The coordinates are returned via the vector argument "r".
// In addition also a pointer to the stored signal object is provided.
// In case no stored signal was available or one of the input arguments was
// invalid, the returned pointer will be 0.
//
// Note : In case the time difference between the stored signal and the specified
//        timestamp is larger than the maximum allowed (see memberfunction SetMaxDt),
//        the vector "r" is set to 0 and also the returned pointer will be 0.
//
// The input parameter "frame" specifies the frame to which the components of r refer.
// The various conventions are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (d),
//                    where the "sph" components of r correspond to theta=(pi/2)-d and phi=a.
//          "gal" ==> Galactic coordinates with longitude (l) and latitude (b).
//                    where the "sph" components of r correspond to theta=(pi/2)-b and phi=l.
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (l) and latitude (b),
//                    where the "sph" components of r correspond to theta=(pi/2)-b and phi=l.
//          "hor" ==> Horizontal coordinates at the NcAstrolab location, where the "sph"
//                    components of r correspond to theta=zenith angle and phi=pi-azimuth.
//          "icr" ==> ICRS coordinates with longitude (l) and latitude (b),
//                    where the "sph" components of r correspond to theta=(pi/2)-b and phi=l.
//          "loc" ==> Local coordinates at the NcAstrolab location, where the "sph"
//                    components of r correspond to the usual theta and phi angles.
//
// In case the coordinates are the equatorial right ascension and declination (a,d),
// they can represent so-called "mean", "true" or reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,d) are only corrected for precession and not for nutation
// true values  : (a,d) are corrected for both precession and nutation
// epoch values : (a,d) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" specifies either "mean", "true" or "epoch" values for the equatorial (a,d) coordinates.
//
// mode = "M" --> Provided coordinates are the mean values 
//        "T" --> Provided coordinates are the true values 
//        "B" --> Provides Besselian (B1950) reference epoch coordinates 
//        "J" --> Provides Julian (J2000) reference epoch coordinates 
//
// The input parameters "jref" and "type" allow to access measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Access to a signal at the j-th position (j=1 is first)
//        0 --> Access to the first measurement (only kept for backward compatibility)
//
// type = 0 --> The requested data is a reference signal
//        1 --> The requested data is a measured signal
//
// The user is advised not to use the obsolete "jref=0" functionality anymore.
//
// Note : In case ts=0 the current timestamp of the lab will be taken.

 r.SetZero();

 if (frame!="equ" && frame!="gal" && frame!="ecl" && frame!="hor" && frame!="icr" && frame!="loc") return 0;

 if (frame=="equ" && mode!="M" && mode!="m" && mode!="T" && mode!="t" && mode!="B" && mode!="b" && mode!="J" && mode!="j") return 0;

 // For backward compatibility
 if (!jref)
 {
  jref=1;
  type=1;
 }

 NcSignal* sx=GetSignal(jref,type);

 if (!sx) return 0;

 if (!ts) ts=(NcTimestamp*)this;

 // Check on maximum time difference
 if (fMaxDt>0)
 {
  NcTimestamp* tx=sx->GetTimestamp();
  if (!tx) return 0;
  Double_t dt=tx->GetDifference(ts,"s",1);
  if (fabs(dt)>fMaxDt) return 0;
 }

 // Update coordinates for Solar system objects
 TString name=sx->GetName();
 SetSolarSystem(name,ts,type);

 Double_t vec[3];
 sx->GetPosition(vec,"sph","rad");
 Nc3Vector q;
 q.SetVector(vec,"sph","rad");

 if (frame=="icr")
 {
  r.Load(q);
  return sx;
 }

 // Convert from ICRS to equatorial J2000 coordinates
 if (!fBias) SetBmatrix();
 q=q.GetPrimed(&fB);

 if (frame=="equ" && mode!="J" && mode!="j")
 {
  // Precess to specified timestamp
  NcTimestamp ts1;
  ts1.SetEpoch(2000,"J");
  if (mode!="B" && mode!="b")
  {
   Precess(q,&ts1,ts);
  }
  else
  {
   NcTimestamp ts2;
   ts2.SetEpoch(1950,"B");
   Precess(q,&ts1,&ts2);
  }

  // Nutation correction if requested
  if (mode=="T" || mode=="t") Nutate(q,ts);
 }

 if (frame=="gal")
 {
  // Convert from equatorial J2000 to galactic
  if (fGal != 2) SetGmatrix("J");
  q=q.GetPrimed(&fG);
 }

 if (frame=="ecl")
 {
  // Precess to specified timestamp
  NcTimestamp ts1;
  ts1.SetEpoch(2000,"J");
  Precess(q,&ts1,ts);

  // Convert from equatorial to ecliptic coordinates
  SetEmatrix(ts);
  q=q.GetPrimed(&fE);
 }

 if (frame=="hor")
 {
  // Precess to specified timestamp
  NcTimestamp ts1;
  ts1.SetEpoch(2000,"J");
  Precess(q,&ts1,ts);

  // Nutation correction
  Nutate(q,ts);

  // Convert from equatorial to horizontal coordinates
  SetHmatrix(ts);
  q=q.GetPrimed(&fH);
 }

 if (frame=="loc")
 {
  // Get the signal in horizontal coordinates
  GetSignal(q,"hor",mode,ts,jref,type);

  // Convert from horizontal to local-frame coordinates
  q=q.GetPrimed(&fL);
 }

 r.Load(q);
 return sx;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GeoToHeliocentric(Double_t& R,Double_t& B,Double_t& L,NcTimestamp* ts,TString Bu,TString Lu)
{
// Convert Geocentric ecliptic coordinates into Heliocentric ones at the provided timestamp.
// The algorithm used here is the one outlined in Ch. 33 of the book by Jean Meeus,
// "Astronomical Algorithms" (2nd edition of August 2009).
// In view of the accuracy of the current algorithm, the effects of light-time
// and aberration are not taken into account here.
//
// Input arguments :
// -----------------
// R  : Geocentric distance of the object in Astronomical Units (AU)
// B  : Geocentric ecliptic latitude
// L  : Geocentric ecliptic longitude
// ts : Timestamp of the (R,B,L) observation (0=The current NcAstrolab timestamp)
// Bu : Angular units of B (see below)
// Lu : Angular units of L (see below)
//
// Angular units indicator :
// -------------------------
// "rad" : input angle provided in radians
// "deg" : input angle provided in degrees
// "dms" : input angle provided in dddmmss.sss
// "hms" : input angle provided in hhmmss.sss
// "hrs" : input angle provided in fractional hours
//
// After the conversion, R, B and L will be the Heliocentric values.
//
// Note : For objects far outside the Solar System, the Geocentric and Heliocentric
//        coordinates are basically identical.
//
// The default values are Bu="deg" and Lu="deg".

 if (!ts) ts=(NcTimestamp*)this;

 // Convert the angles into radians
 B=ConvertAngle(B,Bu,"rad");
 L=ConvertAngle(L,Lu,"rad");

 // The Carthesian Geocentric ecliptic coordinates of the object
 Double_t xg=R*cos(B)*cos(L);
 Double_t yg=R*cos(B)*sin(L);
 Double_t zg=R*sin(B);
 
 // Get the Heliocentric ecliptic coordinates of the Earth
 Double_t r0,b0,l0;
 ts->Almanac(0,0,0,0,"Earth*",&l0,&b0,&r0);

 l0=ConvertAngle(l0,"deg","rad");
 b0=ConvertAngle(b0,"deg","rad");

 Double_t x0=r0*cos(b0)*cos(l0);
 Double_t y0=r0*cos(b0)*sin(l0);
 Double_t z0=r0*sin(b0);

 // The Heliocentric Carthesian ecliptic coordinates of the object
 Double_t xh=xg+x0;
 Double_t yh=yg+y0;
 Double_t zh=zg+z0;

 // Convert to Heliocentric distance, latitude and longitude
 Double_t Rh=sqrt(xh*xh+yh*yh+zh*zh);
 Double_t Bh=atan2(zh,sqrt(xh*xh+yh*yh));
 Double_t Lh=atan2(yh,xh);
 
 Double_t twopi=2.*acos(-1.);
 while (Lh<0) { Lh+=twopi; }
 while (Lh>twopi) { Lh-=twopi; }

 // Return the Heliocentric values in the original units
 R=Rh;
 B=ConvertAngle(Bh,"rad",Bu);
 L=ConvertAngle(Lh,"rad",Lu);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::HelioToGeocentric(Double_t& R,Double_t& B,Double_t& L,NcTimestamp* ts,TString Bu,TString Lu)
{
// Convert Heliocentric ecliptic coordinates into Geocentric ones at the provided timestamp.
// The algorithm used here is the one outlined in Ch. 33 of the book by Jean Meeus,
// "Astronomical Algorithms" (2nd edition of August 2009).
// In view of the accuracy of the current algorithm, the effects of light-time
// and aberration are not taken into account here.
//
// Input arguments :
// -----------------
// R  : Heliocentric distance of the object in Astronomical Units (AU)
// B  : Heliocentric ecliptic latitude
// L  : Heliocentric ecliptic longitude
// ts : Timestamp of the (R,B,L) observation (0=The current NcAstrolab timestamp)
// Bu : Angular units of B (see below)
// Lu : Angular units of L (see below)
//
// Angular units indicator :
// -------------------------
// "rad" : input angle provided in radians
// "deg" : input angle provided in degrees
// "dms" : input angle provided in dddmmss.sss
// "hms" : input angle provided in hhmmss.sss
// "hrs" : input angle provided in fractional hours
//
// After the conversion, R, B and L will be the Geocentric values.
//
// Note : For objects far outside the Solar System, the Geocentric and Heliocentric
//        coordinates are basically identical.
//
// The default values are Bu="deg" and Lu="deg".

 if (!ts) ts=(NcTimestamp*)this;

 // Convert the angles into radians
 B=ConvertAngle(B,Bu,"rad");
 L=ConvertAngle(L,Lu,"rad");

 // The Carthesian Heliocentric ecliptic coordinates of the object
 Double_t xh=R*cos(B)*cos(L);
 Double_t yh=R*cos(B)*sin(L);
 Double_t zh=R*sin(B);
 
 // Get the Heliocentric ecliptic coordinates of the Earth
 Double_t r0,b0,l0;
 ts->Almanac(0,0,0,0,"Earth*",&l0,&b0,&r0);

 l0=ConvertAngle(l0,"deg","rad");
 b0=ConvertAngle(b0,"deg","rad");

 Double_t x0=r0*cos(b0)*cos(l0);
 Double_t y0=r0*cos(b0)*sin(l0);
 Double_t z0=r0*sin(b0);

 // The Geocentric Carthesian ecliptic coordinates of the object
 Double_t xg=xh-x0;
 Double_t yg=yh-y0;
 Double_t zg=zh-z0;

 // Convert to Geocentric distance, latitude and longitude
 Double_t Rg=sqrt(xg*xg+yg*yg+zg*zg);
 Double_t Bg=atan2(zg,sqrt(xg*xg+yg*yg));
 Double_t Lg=atan2(yg,xg);
 
 Double_t twopi=2.*acos(-1.);
 while (Lg<0) { Lg+=twopi; }
 while (Lg>twopi) { Lg-=twopi; }

 // Return the Geocentric values in the original units
 R=Rg;
 B=ConvertAngle(Bg,"rad",Bu);
 L=ConvertAngle(Lg,"rad",Lu);
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::GetSignal(Double_t& d,Double_t& a,TString au,Double_t& b,TString bu,TString frame,NcTimestamp* ts,Int_t jref,TString mode,Int_t type)
{
// Provide celestial position data (d,a,b) of the signal specified by "jref" and "type" at the specific timestamp ts.
// For angular celestial positions it is quite common to use unit vectors, i.e. d=1.
// However, via the coordinate "d" an actual distance, redshift, Lorentz factor or any other
// positive scalar observable related to the observation may have been specified and retrieved here.
// In addition also a pointer to the stored signal object is provided.
// In case no stored signal was available or one of the input arguments was
// invalid, the returned pointer will be 0.
//
// Note : In case the time difference between the stored signal and the specified
//        timestamp is larger than the maximum allowed (see memberfunction SetMaxDt),
//        the d, a and b coordinates are set to 0 and also the returned pointer will be 0.
//
// The input parameters "jref" and "type" allow to access measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Access to a signal at the j-th position (j=1 is first)
//        0 --> Access to the first measurement (only kept for backward compatibility)
//
// type = 0 --> The requested data is a reference signal
//        1 --> The requested data is a measured signal
//
// The user is advised not to use the obsolete "jref=0" functionality anymore.
//
// The input parameter "frame" allows the user to specify the frame to which
// the angular coordinates (a,b) refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (b).
//          "gal" ==> Galactic coordinates with longitude (a) and latitude (b).
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (a) and latitude (b).
//          "hor" ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle (a) and zenith angle (b).
//          "icr" ==> ICRS coordinates with longitude (a) and latitude (b).
//          "loc" ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta (a) and phi (b).
//
// The string input arguments "au" and "bu" allow the user to specifiy the units in which
// the angular coordinates "a" and "b" are provided. The convention is the following :
//
// string = "rad" : angle provided in radians
//          "deg" : angle provided in degrees
//          "dms" : angle provided in dddmmss.sss
//          "hms" : angle provided in hhmmss.sss
//          "hrs" : angle provided in fractional hours
//
// In case the coordinates are the equatorial right ascension and declination (a,b),
// they can represent so-called "mean", "true" and reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,b) are only corrected for precession and not for nutation
// true values  : (a,b) are corrected for both precession and nutation
// epoch values : (a,b) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to select either "mean", "true" or "epoch"
// values in the case of equatorial (a,b) coordinates.
//
// mode = "M" --> Provided coordinates are the mean values 
//        "T" --> Provided coordinates are the true values 
//        "B" --> Provides Besselian (B1950) reference epoch coordinates 
//        "J" --> Provides Julian (J2000) reference epoch coordinates 
//
// The correction methods used are the new IAU 2000 ones as described in the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.
//
// Default values are mode="T" and type=0.
//
// Note : In case ts=0 the current timestamp of the lab will be taken.

 d=0;
 a=0;
 b=0;

 Nc3Vector r;
 NcSignal* sx=GetSignal(r,frame,mode,ts,jref,type);

 if (!sx) return 0;

 // Retrieve the requested (a,d) values in the correct format
 Double_t vec[3];
 r.GetVector(vec,"sph","deg");

 d=vec[0];
 if (d<=0) d=1;
 b=vec[1];
 a=vec[2];

 if (frame=="equ" || frame=="gal" || frame=="ecl" || frame=="icr")
 {
  b=90.-vec[1];
  while (b<-90.)
  {
   b+=90.;
  }
  while (b>90.)
  {
   b-=90.;
  }
 }

 if (frame=="hor")
 {
  a=180.-vec[2];
 }

 while (a<-360.)
 {
  a+=360.;
 }
 while (a>360.)
 {
  a-=360.;
 }

 // Interchange a and b to represent theta and phi, respectively, for local coordinates
 if (frame=="loc")
 {
  Double_t temp=a;
  a=b;
  b=temp;
 }

 // Convert coordinates to appropriate format
 a=ConvertAngle(a,"deg",au);
 b=ConvertAngle(b,"deg",bu);

 return sx;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::GetSignal(Double_t& d,Double_t& a,TString au,Double_t& b,TString bu,TString frame,NcTimestamp* ts,TString name,TString mode,Int_t type)
{
// Provide celestial position data (d,a,b) of the signal specified by "name" and "type" at the specific timestamp ts.
// For angular celestial positions it is quite common to use unit vectors, i.e. d=1.
// However, via the coordinate "d" an actual distance, redshift, Lorentz factor or any other
// positive scalar observable related to the observation may have been specified and retrieved here.
// In addition also a pointer to the stored signal object is provided.
// In case no stored signal was available or one of the input arguments was
// invalid, the returned pointer will be 0.
//
// The input parameter "type" allows to access measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// type = 0 --> The requested data is a reference signal
//        1 --> The requested data is a measured signal
//
// Notes :
// -------
// 1) In case the name specifies a solar system object which was not yet stored according to "type",
//    the corresponding signal will be created and stored with the specified timestamp.
//    All geocentric name specifications for solar system objects as indicated in the
//    docs of NcTimestamp::Almanac() are supported.
// 2) In case the time difference between the stored signal and the specified
//    timestamp is larger than the maximum allowed (see memberfunction SetMaxDt),
//    the d, a and b coordinates are set to 0 and also the returned pointer will be 0.
//
// The input parameter "frame" allows the user to specify the frame to which
// the angular coordinates (a,b) refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (b).
//          "gal" ==> Galactic coordinates with longitude (a) and latitude (b).
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (a) and latitude (b).
//          "hor" ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle (a) and zenith angle (b).
//          "icr" ==> ICRS coordinates with longitude (a) and latitude (b).
//          "loc" ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta (a) and phi (b).
//
// The string input arguments "au" and "bu" allow the user to specifiy the units in which
// the angular coordinates "a" and "b" are provided. The convention is the following :
//
// string = "rad" : angle provided in radians
//          "deg" : angle provided in degrees
//          "dms" : angle provided in dddmmss.sss
//          "hms" : angle provided in hhmmss.sss
//          "hrs" : angle provided in fractional hours
//
// In case the coordinates are the equatorial right ascension and declination (a,b),
// they can represent so-called "mean", "true" and reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,b) are only corrected for precession and not for nutation
// true values  : (a,b) are corrected for both precession and nutation
// epoch values : (a,b) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to select either "mean", "true" or "epoch"
// values in the case of equatorial (a,b) coordinates.
//
// mode = "M" --> Provided coordinates are the mean values 
//        "T" --> Provided coordinates are the true values 
//        "B" --> Provides Besselian (B1950) reference epoch coordinates 
//        "J" --> Provides Julian (J2000) reference epoch coordinates 
//
// The correction methods used are the new IAU 2000 ones as described in the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.
//
// Default values are mode="T" and type=0.
//
// Note : In case ts=0 the current timestamp of the lab will be taken.

 // Set c.q. update coordinates for Solar system objects
 SetSolarSystem(name,ts,type);

 NcSignal* sx=0;
 Int_t j=GetSignalIndex(name,type);
 if (j>=0) sx=GetSignal(d,a,au,b,bu,frame,ts,j,mode,type);
 return sx;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::GetSignal(Double_t& d,Double_t& a,TString au,Double_t& b,TString bu,TString frame,TString s,Double_t e,Int_t jref,TString mode,Int_t type)
{
// Provide celestial position data (d,a,b) of the signal specified by "jref" and "type" at the specified epoch.
// For angular celestial positions it is quite common to use unit vectors, i.e. d=1.
// However, via the coordinate "d" an actual distance, redshift, Lorentz factor or any other
// positive scalar observable related to the observation may have been specified and retrieved here.
// In addition also a pointer to the stored signal object is provided.
// In case no stored signal was available or one of the input arguments was
// invalid, the returned pointer will be 0.
//
// Note : In case the time difference between the stored signal and the specified
//        timestamp is larger than the maximum allowed (see memberfunction SetMaxDt),
//        the d, a and b coordinates are set to 0 and also the returned pointer will be 0.
//
// The input parameters "jref" and "type" allow to access measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Access to a signal at the j-th position (j=1 is first)
//        0 --> Access to the first measurement (only kept for backward compatibility)
//
// type = 0 --> The requested data is a reference signal
//        1 --> The requested data is a measured signal
//
// The user is advised not to use the obsolete "jref=0" functionality anymore.
//
// Default values are jref=0 and type=0.
//
// The input parameter "frame" allows the user to specify the frame to which
// the angular coordinates (a,b) refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (b).
//          "gal" ==> Galactic coordinates with longitude (a) and latitude (b).
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (a) and latitude (b).
//          "hor" ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle (a) and zenith angle (b).
//          "icr" ==> ICRS coordinates with longitude (a) and latitude (b).
//          "loc" ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta (a) and phi (b).
//
// The string input arguments "au" and "bu" allow the user to specifiy the units in which
// the angular coordinates "a" and "b" are provided. The convention is the following :
//
// string = "rad" : angle provided in radians
//          "deg" : angle provided in degrees
//          "dms" : angle provided in dddmmss.sss
//          "hms" : angle provided in hhmmss.sss
//          "hrs" : angle provided in fractional hours
//
// The time of the observation is specified by the following epoch input arguments :
//
//  s = "B" --> Besselian reference epoch.
//      "J" --> Julian reference epoch.
//  e : (Fractional) epoch to specify the timestamp of the observation (e.g. 1952.004353 or 2003.80953216)
//
// In case the coordinates are the equatorial right ascension and declination (a,b),
// they can represent so-called "mean", "true" and "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,b) are only corrected for precession and not for nutation
// true values  : (a,b) are corrected for both precession and nutation
// epoch values : (a,b) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to select either "mean", "true" or "epoch"
// values in the case of equatorial (a,b) coordinates.
//
// mode = "M" --> Provided coordinates are the mean values 
//        "T" --> Provided coordinates are the true values 
//        "B" --> Provides Besselian (B1950) reference epoch coordinates 
//        "J" --> Provides Julian (J2000) reference epoch coordinates 
//
// The correction methods used are the new IAU 2000 ones as described in the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.

 d=0;
 a=0;
 b=0;

 if (s!="B" && s!="b" && s!="J" && s!="j") return 0;

 NcTimestamp tx;
 tx.SetEpoch(e,s);

 NcSignal* sx=GetSignal(d,a,au,b,bu,frame,&tx,jref,mode,type);
 return sx;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::GetSignal(Double_t& d,Double_t& a,TString au,Double_t& b,TString bu,TString frame,TString s,Double_t e,TString name,TString mode,Int_t type)
{
// Provide celestial position data (d,a,b) of the signal specified by "name" and "type" at the specified epoch.
// For angular celestial positions it is quite common to use unit vectors, i.e. d=1.
// However, via the coordinate "d" an actual distance, redshift, Lorentz factor or any other
// positive scalar observable related to the observation may have been specified and retrieved here.
// In addition also a pointer to the stored signal object is provided.
// In case no stored signal was available or one of the input arguments was
// invalid, the returned pointer will be 0.
//
// The input parameter "type" allows to access measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// type = 0 --> The requested data is a reference signal
//        1 --> The requested data is a measured signal
//
// Default value is type=0.
//
// Notes :
// -------
// 1) In case the name specifies a solar system object which was not yet stored according to "type",
//    the corresponding signal will be created and stored with the specified epoch as timestamp.
//    All geocentric name specifications for solar system objects as indicated in the
//    docs of NcTimestamp::Almanac() are supported.
// 2) In case the time difference between the stored signal and the specified
//    timestamp is larger than the maximum allowed (see memberfunction SetMaxDt),
//    the d, a and b coordinates are set to 0 and also the returned pointer will be 0.
//
// The input parameter "frame" allows the user to specify the frame to which
// the angular coordinates (a,b) refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (b).
//          "gal" ==> Galactic coordinates with longitude (a) and latitude (b).
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (a) and latitude (b).
//          "hor" ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle (a) and zenith angle (b).
//          "icr" ==> ICRS coordinates with longitude (a) and latitude (b).
//          "loc" ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta (a) and phi (b).
//
// The string input arguments "au" and "bu" allow the user to specifiy the units in which
// the angular coordinates "a" and "b" are provided. The convention is the following :
//
// string = "rad" : angle provided in radians
//          "deg" : angle provided in degrees
//          "dms" : angle provided in dddmmss.sss
//          "hms" : angle provided in hhmmss.sss
//          "hrs" : angle provided in fractional hours
//
// The time of the observation is specified by the following epoch input arguments :
//
//  s = "B" --> Besselian reference epoch.
//      "J" --> Julian reference epoch.
//  e : (Fractional) epoch to specify the timestamp of the observation (e.g. 1952.004353 or 2003.80953216)
//
// In case the coordinates are the equatorial right ascension and declination (a,b),
// they can represent so-called "mean", "true" and reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,b) are only corrected for precession and not for nutation
// true values  : (a,b) are corrected for both precession and nutation
// epoch values : (a,b) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to select either "mean", "true" or "epoch"
// values in the case of equatorial (a,b) coordinates.
//
// mode = "M" --> Provided coordinates are the mean values 
//        "T" --> Provided coordinates are the true values 
//        "B" --> Provides Besselian (B1950) reference epoch coordinates 
//        "J" --> Provides Julian (J2000) reference epoch coordinates 
//
// The correction methods used are the new IAU 2000 ones as described in the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.

 // Set c.q. update coordinates for Solar system objects
 NcTimestamp tx;
 tx.SetEpoch(e,s);
 SetSolarSystem(name,&tx,type);

 NcSignal* sx=0;
 Int_t j=GetSignalIndex(name,type);
 if (j>=0) sx=GetSignal(d,a,au,b,bu,frame,s,e,j,mode,type);
 return sx;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::GetSignal(Int_t jref,Int_t type)
{
// Provide the pointer to a stored signal object.
//
// The input parameters "jref" and "type" allow to access either measured of so-called "reference" signals.
// The reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Access to the signal at the j-th position (j=1 is first)
//
// type = 0 --> Provide the corresponding reference signal
//        1 --> Provide the corresponding measurement
//
// Note : jref=0 is an obsolete way to access the first measurement.
//        The user is advised not to use this obsolete "jref=0" method anymore,
//        but it is still supported for backward compatibility.
//
// Default values are jref=0 and type=0 for backward compatibility.

 if (jref<0) return 0;
 
 if (!jref) // For backward compatibility
 {
  jref=1;
  type=1;
 }
 
 if (!type && !fRefs) return 0;
 if (type && !fSigs) return 0;

 NcSignal* sx=0;
 if (type)
 {
  if (jref<=fSigs->GetSize()) sx=(NcSignal*)fSigs->At(jref-1);
 }
 else
 {
  if (jref<=fRefs->GetSize()) sx=(NcSignal*)fRefs->At(jref-1);
 }
 return sx;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcAstrolab::GetSignal(TString name,Int_t type,NcTimestamp* ts)
{
// Provide the pointer to the stored signal object with the specified name and type.
//
// type = 0 --> Provide the corresponding reference signal
//        1 --> Provide the corresponding measurement
//
//
// Note : In case the name specifies a solar system object which was not yet stored,
//        the corresponding signal will be created according to "type" and the provided
//        timestamp "ts".
//        In case ts=0, the current timestamp of this NcAstrolab instance will be used.
//        All geocentric name specifications for solar system objects as indicated in the
//        docs of NcTimestamp::Almanac() are supported.
//
// The default values are type=0 and ts=0 for backward compatibility.

 NcSignal* sx=0;
 Int_t j=GetSignalIndex(name,type);

 if (j==-1) // Set and store info for the requested Solar system object if not already stored
 {
  SetSolarSystem(name,ts,type);
  j=GetSignalIndex(name,type);
 }

 if (j>=0) sx=GetSignal(j,type);
 return sx;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::RemoveRefSignal(Int_t j,Int_t compress)
{
// ***********************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility ***
// *** The user is advised to use the memberfunction RemoveSignal() instead        ***
// ***********************************************************************************
//
// Remove the reference signal which was stored at the j-th position (j=1 is first).
// The return argument is the number of signals that were removed.
// Note : j=0 means that all stored reference signals will be removed.
//        j<0 allows array compression (see below) without removing any signals. 
//
// The "compress" parameter allows compression of the ref. signal storage array.
//
// compress = 1 --> Array will be compressed
//            0 --> Array will not be compressed
//
// Note : Compression of the storage array means that the indices of the
//        reference signals in the storage array will change.

 Int_t nrem=0;

 if (!fRefs) return 0;

 // Clearing of the complete storage
 if (!j)
 {
  nrem=fRefs->GetEntries();
  delete fRefs;
  fRefs=0;
  return nrem;
 }

 // Removing a specific reference signal
 if (j>0 && j<=fRefs->GetSize())
 {
  TObject* obj=fRefs->RemoveAt(j-1);
  if (obj)
  {
   delete obj;
   nrem++;
  }
 }

 // Compression of the storage array
 if (compress) fRefs->Compress();

 return nrem;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::RemoveRefSignal(TString name,Int_t compress)
{
// ***********************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility ***
// *** The user is advised to use the memberfunction RemoveSignal() instead        ***
// ***********************************************************************************
//
// Remove the reference signal with the specified name.
// The return argument is the number of signals that were removed.
//
// The "compress" parameter allows compression of the ref. signal storage array.
//
// compress = 1 --> Array will be compressed
//            0 --> Array will not be compressed
//
// Note : Compression of the storage array means that the indices of the
//        reference signals in the storage array will change.

 Int_t nrem=0;

 Int_t j=GetSignalIndex(name);
 if (j>0) nrem=RemoveRefSignal(j,compress);

 return nrem;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::RemoveSignal(Int_t j,Int_t type,Int_t compress)
{
// Remove the signal of type "type" which was stored at the j-th position (j=1 is first).
// The return argument is the number of signals that were removed.
// Note : j=0 means that all stored signals of type "type" will be removed with compression.
//        j<0 allows array compression (see below) without removing any signals. 
//
// type = 0 --> Remove a reference signal
//      = 1 --> Remove a measured signal
//
// The "compress" parameter allows compression of the signal storage array of type "type".
//
// compress = 1 --> Array will be compressed
//            0 --> Array will not be compressed
//
// Note : Compression of the storage array means that the indices of the
//        signals in that storage array will change.

 Int_t nrem=0;
 
 TObjArray* arr=fRefs;
 if (type) arr=fSigs;

 if (!arr) return nrem;

 // Clearing of the complete "type" storage
 if (!j)
 {
  nrem=arr->GetEntries();
  delete arr;
  if (type)
  {
   fSigs=0;
   fNen[1]=0;
  }
  else
  {
   fRefs=0;
   fNen[0]=0;
  }
  return nrem;
 }

 // Removing the specified signal
 if (j>0 && j<=arr->GetSize())
 {
  TObject* obj=arr->RemoveAt(j-1);
  if (obj)
  {
   delete obj;
   nrem++;
  }
 }

 // Compression of the storage array
 if (compress)
 {
  arr->Compress();
  Int_t n=arr->GetEntries();
  arr->Expand(n);
 }

 return nrem;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::RemoveSignal(TString name,Int_t type,Int_t compress)
{
// Remove the signal with the specified name and type.
// The return argument is the number of signals that were removed.
//
// Note : name="*" will remove all the entries of the specified type with compression.
//
// type = 0 --> Remove a reference signal
//      = 1 --> Remove a measured signal
//
// The "compress" parameter allows compression of the signal storage array of type "type".
//
// compress = 1 --> Array will be compressed
//            0 --> Array will not be compressed
//
// Note : Compression of the storage array means that the indices of the
//        signals in that storage array will change.
//
// In case name="" or no matching signal with the specified name and type is found,
// no action will be taken.

 Int_t nrem=0;

 if (name=="") return nrem;

 if (name=="*")
 {
  nrem=RemoveSignal(0,type,compress);
 }
 else
 {
  Int_t j=GetSignalIndex(name,type);
  if (j>0) nrem=RemoveSignal(j,type,compress);
 }

 return nrem;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::RemoveSignals(TString name,Int_t type,Int_t compress)
{
// Remove all signals that match the provided name pattern and type.
// The return argument is the number of signals that were removed.
//
// Note : name="*" will remove all the entries of the specified type with compression.
//
// type = 0 --> Remove a reference signal
//      = 1 --> Remove a measured signal
//
// The "compress" parameter allows compression of the signal storage array of type "type".
//
// compress = 1 --> Array will be compressed
//            0 --> Array will not be compressed
//
// Note : Compression of the storage array means that the indices of the
//        signals in that storage array will change.
//
// In case name="" or no matching signal with the specified name pattern and type is found,
// no action will be taken.

 Int_t nrem=0;

 if (name=="") return 0;

 if (name=="*")
 {
  nrem=RemoveSignal(0,type,compress);
 }
 else
 {
  TObjArray* arr=fRefs;
  if (type) arr=fSigs;

  if (!arr) return 0;

  NcSignal* sx=0;
  TString namex="";
  Int_t nremx=0;
  for (Int_t j=1; j<=arr->GetSize(); j++)
  {
   sx=GetSignal(j,type);
   if (!sx) continue;

   namex=sx->GetName();
   if (!namex.Contains(name)) continue;

   nremx=RemoveSignal(j,type,0);
   if (nremx) nrem++;
  }

  // Compression of the storage array
  if (compress)
  {
   arr->Compress();
   Int_t n=arr->GetEntries();
   arr->Expand(n);
  }
 }

 return nrem;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::GetSignalIndex(TString name,Int_t type)
{
// Provide storage index of the signal with the specified name.
//
// type = 0 --> Provide the index of a reference signal
//      = 1 --> Provide the index of a measured signal
//
// In case name="" or no signal with the specified name was found, the value -1 is returned.
//
// The default value is type=0 for backward compatibility.

 if (name=="") return -1;

 Int_t index=-1;
 
 TObjArray* arr=fRefs;
 if (type) arr=fSigs;
 
 if (!arr) return -1;

 for (Int_t i=0; i<arr->GetSize(); i++)
 {
  NcSignal* sx=(NcSignal*)arr->At(i);
  if (!sx) continue;

  if (name==sx->GetName())
  {
   index=i+1;
   break;
  }
 }

 return index;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::GetSignalIndex(NcSignal* s,Int_t type)
{
// Provide storage index of the specified signal.
//
// type = 0 --> Provide the index of a reference signal
//      = 1 --> Provide the index of a measured signal
//
// In case no matching signal is found, the value -1 is returned.

 if (!s) return -1;

 Int_t index=-1;
 
 TObjArray* arr=fRefs;
 if (type) arr=fSigs;
 
 if (!arr) return -1;

 for (Int_t i=0; i<arr->GetSize(); i++)
 {
  NcSignal* sx=(NcSignal*)arr->At(i);
  if (!sx) continue;

  if (sx==s)
  {
   index=i+1;
   break;
  }
 }

 return index;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::PrintSignal(TString frame,TString mode,NcTimestamp* ts,Int_t ndig,Int_t jref,TString emode,Int_t type,Bool_t align)
{
// Print data of a stored signal in user specified coordinates at the specific timestamp ts.
// In case ts=0 the actual timestamp of the stored signal will be taken.
// In case no stored signal was available or one of the input arguments was
// invalid, no printout is produced.
//
// The argument "ndig" specifies the number of digits for the fractional
// part (e.g. ndig=6 for "dms" corresponds to micro-arcsecond precision).
// Rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.5 on the output, but with ndig=2 it will show 03.47.
// Due to computer accuracy, precision on the pico-arcsecond level may get lost.
//
// The argument "align", when set to kTRUE, ensures that all coordinate
// output strings will have the same length.
// This will allow the user to provide data in an aligned tabular format.
// When "align" is set to kFALSE, the coordinate output will be in the most
// compact format.
//
// Note : The angle info is printed without additional spaces or "endline".
//        This allows the print to be included in various composite output formats.
//
// The input parameter "frame" allows the user to specify the frame to which
// the coordinates refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (d).
//
//          "gal" ==> Galactic coordinates with longitude (l) and latitude (b).
//
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (l) and latitude (b).
//
//          "hor" ==> Horizontal azimuth and altitude coordinates at the NcAstrolab location.
//
//          "icr" ==> ICRS coordinates with longitude (l) and latitude (b).
//
//          "loc" ==> Local spherical angles theta and phi at the NcAstrolab location.
//
// In case the coordinates are the equatorial right ascension and declination (a,d),
// they can represent so-called "mean", "true" and reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,d) are only corrected for precession and not for nutation
// true values  : (a,d) are corrected for both precession and nutation
// epoch values : (a,d) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to specifiy either "mean", "true" or "epoch"
// values for the printout in case of equatorial (a,d) coordinates.
//
// mode = "M" --> Printed coordinates are the mean values 
//        "T" --> Printed coordinates are the true values 
//        "B" --> Prints Besselian (B1950) reference epoch coordinates 
//        "J" --> Prints Julian (J2000) reference epoch coordinates 
//
// The input parameter "mode" also determines which type of time and
// local hour angle will appear in the printout.
//
// mode = "M" --> Mean Sidereal Time (MST) and Local Mean Hour Angle (LMHA)
//        "T" --> Apparent Sidereal Time (AST) and Local Apparent Hour Angle (LAHA)
//        "B" --> Time and local hour angle type determined by input argument "emode" ("M" or "T")
//        "J" --> Time and local hour angle type determined by input argument "emode" ("M" or "T")
//
// The input parameters "jref" and "type" allow printing of a measurement or a so-called "reference" signal.
// These reference signals may serve to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Printing of the j-th stored signal
//
// type = 0 --> Print data of a stored reference signal
//        1 --> Print data of a stored measurement
//
// Note :
// ------
// For jref=0 always the first stored measurement will be printed for backward compatibility,
// but the user is advised not to use this obsolete method anymore.
//
// Default values are jref=0, emode="T", type=0 and align=kFALSE.

 NcSignal* sx=GetSignal(jref,type);

 if (!sx) return;

 if (!ts) ts=sx->GetTimestamp();

 Nc3Vector r;
 GetSignal(r,frame,mode,ts,jref,type);

 // Local Hour Angle of the signal
 Double_t lha=GetHourAngle("A",ts,jref,type);
 TString slha="LAHA";
 if (mode=="M" || mode=="m")
 {
  lha=GetHourAngle("M",ts,jref,type);
  slha="LMHA";
 }
 else if ((mode=="B" || mode=="b" || mode=="J" || mode=="j") && emode=="M")
 {
  lha=GetHourAngle("M",ts,jref,type);
  slha="LMHA";
 }

 if (frame=="equ")
 {
  Double_t a,d;
  d=90.-r.GetX(2,"sph","deg");
  a=r.GetX(3,"sph","rad");
  if (mode=="B" || mode=="b") mode="B1950";
  if (mode=="J" || mode=="j") mode="J2000";
  cout << "Equatorial (" << mode.Data() <<") | a :";
  if (!align) {cout << " ";} PrintAngle(a,"rad","hms",ndig,align);
  if (!align) {cout << " ";} PrintAngle(a,"rad","deg",ndig,align);
  cout << " | b :";
  if (!align) {cout << " ";} PrintAngle(d,"deg","dms",ndig,align);
  if (!align) {cout << " ";} PrintAngle(d,"deg","deg",ndig,align);
  cout << " | " << slha.Data() << " : ";
  PrintAngle(lha,"deg","hms",ndig,align);
  cout << " "; PrintAngle(lha,"deg","deg",ndig,align);
 }

 if (frame=="gal")
 {
  Double_t l,b;
  b=90.-r.GetX(2,"sph","deg");
  l=r.GetX(3,"sph","deg");
  cout << "Galactic | l :";
  if (!align) {cout << " ";} PrintAngle(l,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(l,"deg","dms",ndig,align);
  cout << " | b :";
  if (!align) {cout << " ";} PrintAngle(b,"deg","deg",ndig,align); 
  if (!align) {cout << " ";} PrintAngle(b,"deg","dms",ndig,align);
  cout << " | " << slha.Data() << " : ";
  PrintAngle(lha,"deg","hms",ndig,align);
  cout << " "; PrintAngle(lha,"deg","deg",ndig,align);
 }

 if (frame=="icr")
 {
  Double_t l,b;
  b=90.-r.GetX(2,"sph","deg");
  l=r.GetX(3,"sph","deg");
  cout << "ICRS | l :";
  if (!align) {cout << " ";} PrintAngle(l,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(l,"deg","dms",ndig,align);
  cout << " | b :";
  if (!align) {cout << " ";} PrintAngle(b,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(b,"deg","dms",ndig,align);
  cout << " | " << slha.Data() << " : ";
  PrintAngle(lha,"deg","hms",ndig,align);
  cout << " "; PrintAngle(lha,"deg","deg",ndig,align);
 }

 if (frame=="ecl")
 {
  Double_t l,b;
  b=90.-r.GetX(2,"sph","deg");
  l=r.GetX(3,"sph","deg");
  cout << "Geocentric ecliptic | l :";
  if (!align) {cout << " ";} PrintAngle(l,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(l,"deg","dms",ndig,align);
  cout << " | b :";
  if (!align) {cout << " ";} PrintAngle(b,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(b,"deg","dms",ndig,align);
  cout << " | " << slha.Data() << " : ";
  PrintAngle(lha,"deg","hms",ndig,align);
  cout << " "; PrintAngle(lha,"deg","deg",ndig,align);
 }

 if (frame=="hor")
 {
  Double_t alt=90.-r.GetX(2,"sph","deg");
  Double_t azi=180.-r.GetX(3,"sph","deg");
  while (azi>360)
  {
   azi-=360.;
  }
  while (azi<0)
  {
   azi+=360.;
  }
  cout << "Horizontal | azi :";
  if (!align) {cout << " ";} PrintAngle(azi,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(azi,"deg","dms",ndig,align);
  cout << " | alt :";
  if (!align) {cout << " ";} PrintAngle(alt,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(alt,"deg","dms",ndig,align);
  cout << " | " << slha.Data() << " : ";
  PrintAngle(lha,"deg","hms",ndig,align);
  cout << " "; PrintAngle(lha,"deg","deg",ndig,align);
 }

 if (frame=="loc")
 {
  Double_t theta=r.GetX(2,"sph","deg");
  Double_t phi=r.GetX(3,"sph","deg");
  cout << "Local-frame | phi :";
  if (!align) {cout << " ";} PrintAngle(phi,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(phi,"deg","dms",ndig,align);
  cout << " | theta :";
  if (!align) {cout << " ";} PrintAngle(theta,"deg","deg",ndig,align);
  if (!align) {cout << " ";} PrintAngle(theta,"deg","dms",ndig,align);
  cout << " | " << slha.Data() << " : ";
  PrintAngle(lha,"deg","hms",ndig,align);
  cout << " "; PrintAngle(lha,"deg","deg",ndig,align);
 }

 cout << " |";

 TString name=sx->GetName();
 if (name != "") cout << " " << name.Data();
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::PrintSignal(TString frame,TString mode,NcTimestamp* ts,Int_t ndig,TString name,TString emode,Int_t type,Bool_t align)
{
// Print data of the stored signal with the specified name and type in user specified coordinates
// at the specific timestamp ts.
// In case ts=0 the actual timestamp of the stored signal will be taken.
// In case no such stored signal was available or one of the input arguments was
// invalid, no printout is produced.
//
// type = 0 --> Print data of a stored reference signal
//        1 --> Print data of a stored measurement
//
// Note : In case the name specifies a solar system object which was not yet stored according to "type",
//        the corresponding signal will be created and stored with the specified timestamp.
//        In case ts=0 the solar system object will be stored with the current timestamp of
//        this NcAstrolab instance.
//        All geocentric name specifications for solar system objects as indicated in the
//        docs of NcTimestamp::Almanac() are supported.
//
// The argument "ndig" specifies the number of digits for the fractional
// part (e.g. ndig=6 for "dms" corresponds to micro-arcsecond precision).
// Rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.5 on the output, but with ndig=2 it will show 03.47.
// Due to computer accuracy, precision on the pico-arcsecond level may get lost.
//
// The argument "align", when set to kTRUE, ensures that all coordinate
// output strings will have the same length.
// This will allow the user to provide data in an aligned tabular format.
// When "align" is set to kFALSE, the coordinate output will be in the most
// compact format.
//
// Note : The angle info is printed without additional spaces or "endline".
//        This allows the print to be included in various composite output formats.
//
// The input parameter "frame" allows the user to specify the frame to which
// the coordinates refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (d).
//
//          "gal" ==> Galactic coordinates with longitude (l) and latitude (b).
//
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (l) and latitude (b).
//
//          "hor" ==> Horizontal azimuth and altitude coordinates at the NcAstrolab location.
//
//          "icr" ==> ICRS coordinates with longitude (l) and latitude (b).
//
//          "loc" ==> Local spherical angles theta and phi at the NcAstrolab location.
//
// In case the coordinates are the equatorial right ascension and declination (a,d),
// they can represent so-called "mean", "true" or reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,d) are only corrected for precession and not for nutation
// true values  : (a,d) are corrected for both precession and nutation
// epoch values : (a,d) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to specifiy either "mean", "true" or "epoch"
// values for the printout in case of equatorial (a,d) coordinates.
//
// mode = "M" --> Printed coordinates are the mean values 
//        "T" --> Printed coordinates are the true values 
//        "B" --> Prints Besselian (B1950) reference epoch coordinates 
//        "J" --> Prints Julian (J2000) reference epoch coordinates 
//
// The input parameter "mode" also determines which type of time and
// local hour angle will appear in the printout.
//
// mode = "M" --> Mean Sidereal Time (MST) and Local Mean Hour Angle (LMHA)
//        "T" --> Apparent Sidereal Time (AST) and Local Apparent Hour Angle (LAHA)
//        "B" --> Time and local hour angle type determined by input argument "emode" ("M" or "T")
//        "J" --> Time and local hour angle type determined by input argument "emode" ("M" or "T")
//
// The default values are emode="T", type=0 and align=kFALSE

 // Set c.q. update coordinates for Solar system objects
 SetSolarSystem(name,ts,type);

 Int_t j=GetSignalIndex(name,type);
 if (j>=0) PrintSignal(frame,mode,ts,ndig,j,emode,type,align);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ListSignals(TString frame,TString mode,Int_t ndig,TString emode,Int_t nmax,Int_t j,Int_t type,NcTimestamp* ts,TString name)
{
// List stored measurements and/or reference signals in user specified coordinates
// at a specific timestamp if their name matches the provided name pattern "name", where name="*" means all.
// Measurements are listed according to the timestamp of their actual stored recording, which may be
// a scrambled timestamp according to the invokation of the memberfunction SetTimeScramble().
// For reference signals the actual timestamp of the j-th (j=1 is first) stored measurement is used.
// In case j=0 or no (timestamp of the) j-th measurement is available, the provided timestamp will be taken,
// where a provided ts=0 (which is the default) refers to the current timestamp of the lab.
// For j<0 the timestamp of the actual recording of the reference signal will be used.
// In case no corresponding stored signal is available or one of the input arguments is
// invalid, no printout is produced.
//
// type = 0 --> List the data of stored reference signals
//        1 --> List the data of stored measured signals
//      < 0 --> List the data of both measured and reference signals  
//
// The argument "nmax" specifies the maximum number of measured c.q. reference signals
// that will be listed. When nmax<0 all stored measured c.q. reference signals are listed.
//
// The argument "ndig" specifies the number of digits for the fractional
// part (e.g. ndig=6 for "dms" corresponds to micro-arcsecond precision).
// Rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.5 on the output, but with ndig=2 it will show 03.47.
// Due to computer accuracy, precision on the pico-arcsecond level may get lost.
//
// Note : The angle info is printed without additional spaces or "endline".
//        This allows the print to be included in various composite output formats.
//
// The input parameter "frame" allows the user to specify the frame to which
// the coordinates refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (d).
//
//          "gal" ==> Galactic coordinates with longitude (l) and latitude (b).
//
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (l) and latitude (b).
//
//          "hor" ==> Horizontal azimuth and altitude coordinates at the NcAstrolab location.
//
//          "icr" ==> ICRS coordinates with longitude (l) and latitude (b).
//
//          "loc" ==> Local spherical angles theta and phi at the NcAstrolab location.
//
// In case the coordinates are the equatorial right ascension and declination (a,d),
// they can represent so-called "mean", "true" and reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,d) are only corrected for precession and not for nutation
// true values  : (a,d) are corrected for both precession and nutation
// epoch values : (a,d) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" allows the user to specifiy either "mean", "true" or "epoch"
// values for the listing in case of equatorial (a,d) coordinates.
//
// mode = "M" --> Listed coordinates are the mean values 
//        "T" --> Listed coordinates are the true values 
//        "B" --> Lists Besselian (B1950) reference epoch coordinates 
//        "J" --> Lists Julian (J2000) reference epoch coordinates 
//
// The input parameter "mode" also determines which type of time and
// local hour angle will appear in the listing.
//
// mode = "M" --> Mean Sidereal Time (MST) and Local Mean Hour Angle (LMHA)
//        "T" --> Apparent Sidereal Time (AST) and Local Apparent Hour Angle (LAHA)
//        "B" --> Time and local hour angle type determined by input argument "emode" ("M" or "T")
//        "J" --> Time and local hour angle type determined by input argument "emode" ("M" or "T")
//
// The default values are ndig=1, emode="T", nmax=10, j=-1, type=-1, ts=0 and name="*".

 Int_t iprint=0;
 Int_t width=0; // Width for printing of the index
 if (nmax>0) width=log10(nmax);
 if (nmax<0)
 {
  Int_t maxref=0;
  if (fRefs) maxref=fRefs->GetSize();
  Int_t maxsig=0;
  if (fSigs) maxsig=fSigs->GetSize();
  Int_t maxj=maxref;
  if (maxsig>maxj) maxj=maxsig;
  if (maxj>0) width=log10(maxj);
 }
 width++;

 NcSignal* sx=0;
 NcTimestamp* tx=0;

 Int_t dform=1;
 if (mode=="T" || mode=="t") dform=-1;
 if ((mode=="B" || mode=="b" || mode=="J" || mode=="j") && emode=="T") dform=-1;

 if (j>0) sx=GetSignal(j,1);
 if (sx)
 {
  tx=sx->GetTimestamp();
  if (!tx) tx=ts;
  if (!tx) tx=(NcTimestamp*)this;
  printf(" *%-s::ListSignals* Name : %-s Title : %-s \n",ClassName(),GetName(),GetTitle());
  if (fTscmode!=2)
  {
   cout << " Timestamp of the measurement stored at index=" << j;
  }
  else
  {
   cout << " *Scrambled* timestamp of the measurement stored at index=" << j;
  }
  cout << " (Lab time offset w.r.t. UT : "; PrintTime(fToffset,12); cout << ")" << endl;
  tx->Date(dform,fToffset);
  tx->Date(4);
  cout << " Corresponding location of this measurement" << endl;
  cout << " "; PrintSignal(frame,mode,tx,ndig,j,emode,1); cout << endl;
  iprint=1;
 }

 TObjArray* arr=0;
 Int_t nstored=0;
 Int_t jlist=0;
 Int_t test=type;
 while (test<2)
 {
  if (test==0)
  {
   type=0;
   arr=fRefs;
   test=999;
  }
  if (test==1)
  {
   type=1;
   arr=fSigs;
   test=999;
  }
  if (test<0)
  {
   type=0;
   arr=fRefs;
   test=1;
  }
  
  if (!arr) continue;

  nstored=arr->GetEntries();
  jlist=0;
  TString namex="";
  for (Int_t i=1; i<=arr->GetSize(); i++)
  {
   sx=GetSignal(i,type);
   if (!sx) continue;

   jlist++;
   if (nmax>=0 && jlist>nmax) break;

   // Check for the name pattern
   namex=sx->GetName();
   if (name!="*" && !namex.Contains(name)) continue;

   if (!iprint)
   {
    printf(" *%-s::ListSignals* Name : %-s Title : %-s \n",ClassName(),GetName(),GetTitle());
    if (j==0) tx=ts;
    if (tx)
    {
     cout << " User provided timestamp (Lab time offset w.r.t. UT : "; PrintTime(fToffset,12); cout << ")";
     cout << endl;
     tx->Date(dform,fToffset);
     tx->Date(4);
    }
    else
    {
     tx=(NcTimestamp*)this;
     if (j>=0)
     {
      cout << " Current timestamp of the laboratory (Lab time offset w.r.t. UT : "; PrintTime(fToffset,12); cout << ")";
      cout << endl;
      tx->Date(dform,fToffset);
      tx->Date(4);
     }
    }
    iprint=1;
   }
   if (iprint==1)
   {
    if (nmax<0 || nmax>=nstored)
    {
     if (!type)
     {
      if (j>=0)
      {
       cout << " === All stored reference signals according to the above timestamp ===" << endl;
      }
      else
      {
       cout << " === All stored reference signals according to their actual recorded timestamp ===" << endl;
      }
     }
     else
     {
      if (fTscmode!=2)
      {
       cout << " === All stored measurements according to their actual observation timestamp ===" << endl;
      }
      else
      {
       cout << " === All stored measurements according to their *scrambled* observation timestamp ===" << endl;
       cout << " === Time scrambling was performed by adding dt from the interval [dtmin,dtmax] to their actual timestamp" << endl;
       cout << " === dtmin : " << fTscmin << " dtmax : " << fTscmax << " sec.";
       if (fTscfunc)
       {
        cout << " Randomising TF1 function " << fTscfunc->GetName() << " was used." << endl;
       }
       else
       {
        cout << " Uniform randomisation was used." << endl;
       }
      }
     } 
    }
    else
    {
     if (!type)
     {
      if (j>=0)
      {
       cout << " === The first " << nmax << " stored reference signals according to the above timestamp ===" << endl;
      }
      else
      {
       cout << " === The first " << nmax << " stored reference signals according to their actual recorded timestamp ===" << endl;
      }
     }
     else
     {
      if (fTscmode!=2)
      {
       cout << " === The first " << nmax << " stored measurements according to their actual observation timestamp ===" << endl;
      }
      else
      {
       cout << " === The first " << nmax << " stored measurements according to their *scrambled* observation timestamp ===" << endl;
       cout << " === Time scrambling was performed by adding dt from the interval [dtmin,dtmax] to their actual timestamp" << endl;
       cout << " === dtmin : " << fTscmin << " dtmax : " << fTscmax << " sec.";
       if (fTscfunc)
       {
        cout << " Randomising TF1 function " << fTscfunc->GetName() << " was used." << endl;
       }
       else
       {
        cout << " Uniform randomisation was used." << endl;
       }
      }
     }
    }
    iprint=2;
   }
   if (type==1 || (!type && j<0)) tx=0;
   printf(" Index : %*d ",width,i); PrintSignal(frame,mode,tx,ndig,i,emode,type,kTRUE); printf("\n");
  }
  iprint=1;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::Precess(Nc3Vector& r,NcTimestamp* ts1,NcTimestamp* ts2)
{
// Correct mean right ascension and declination (as given for timestamp ts1)
// for the earth's precession, corresponding to the specified timestamp ts2.
// The results are the so-called "mean" (i.e. precession corrected) values,
// corresponding to the specified timestamp ts2.
// The method used is the new IAU 2000 one as described in the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.
// Since the standard reference epoch is J2000, this implies that all
// input (a,d) coordinates will be first internally converted to the
// corresponding J2000 values before the precession correction w.r.t. the
// specified lab timestamp will be applied.
//
// r   : Input vector containing the right ascension and declination information
//       in the form of standard Nc3Vector coordinates.
//       In spherical coordinates the phi corresponds to the right ascension,
//       whereas the declination corresponds to (pi/2)-theta.
// ts1 : Timestamp corresponding to the input coordinate values.
// ts2 : Timestamp corresponding to the requested corrected coordinate values.
//
// Note : In case ts2=0 the current timestamp of the lab will be taken.

 // Convert back to J2000 values
 Nc3Vector r0;
 SetPmatrix(ts1);
 r0=r.GetUnprimed(&fP);

 // Precess to the specified timestamp
 if (!ts2) ts2=(NcTimestamp*)this;
 SetPmatrix(ts2);
 r=r0.GetPrimed(&fP);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::Nutate(Nc3Vector& r,NcTimestamp* ts)
{
// Correct mean right ascension and declination for the earth's nutation
// corresponding to the specified timestamp.
// The results are the so-called "true" (i.e. nutation corrected) values,
// corresponding to the specified timestamp.
// The method used is the new IAU 2000 one as described in the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.
//
// r  : Input vector containing the right ascension and declination information
//      in the form of standard Nc3Vector coordinates.
//      In spherical coordinates the phi corresponds to the right ascension,
//      whereas the declination corresponds to (pi/2)-theta.
// ts : Timestamp for which the corrected coordinate values are requested.
//
// Note : In case ts=0 the current timestamp of the lab will be taken.

 // Nutation correction for the specified timestamp
 if (!ts) ts=(NcTimestamp*)this;
 SetNmatrix(ts);
 r=r.GetPrimed(&fN);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetBmatrix()
{
// Set the frame bias matrix elements.
// The formulas and numerical constants used are the ones from the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.

 Double_t pi=acos(-1.);
 
 // Parameters in mas
 Double_t a=-14.6;
 Double_t x=-16.6170;
 Double_t e=-6.8192;

 // Convert to radians
 a*=pi/(180.*3600.*1000.);
 x*=pi/(180.*3600.*1000.);
 e*=pi/(180.*3600.*1000.);

 Double_t mat[9];
 mat[0]=1.-0.5*(a*a+x*x);
 mat[1]=a;
 mat[2]=-x;
 mat[3]=-a-e*x;
 mat[4]=1.-0.5*(a*a+e*e);
 mat[5]=-e;
 mat[6]=x-e*a;
 mat[7]=e+x*a;
 mat[8]=1.-0.5*(e*e+x*x);

 fB.SetMatrix(mat);
 fBias=1;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetPmatrix(NcTimestamp* ts)
{
// Set precession matrix elements for Julian date jd w.r.t. J2000.
// The formulas and numerical constants used are the ones from the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.
// All numerical constants refer to the standard reference epoch J2000.

 Double_t mat[9]={0,0,0,0,0,0,0,0,0};
 if (!ts)
 {
  fP.SetMatrix(mat);
  return;
 }

 Double_t pi=acos(-1.);

 Double_t t=(ts->GetJD()-2451545.0)/36525.; // Julian centuries since J2000.0

 // Parameters for the precession matrix in arcseconds
 Double_t eps0=84381.406; // Mean ecliptic obliquity at J2000.0
 Double_t psi=5038.481507*t-1.0790069*pow(t,2)-0.00114045*pow(t,3)+0.000132851*pow(t,4)
                -0.0000000951*pow(t,4);
 Double_t om=eps0-0.025754*t+0.0512623*pow(t,2)-0.00772503*pow(t,3)-0.000000467*pow(t,4)
                 +0.0000003337*pow(t,5);
 Double_t chi=10.556403*t-2.3814292*pow(t,2)-0.00121197*pow(t,3)+0.000170663*pow(t,4)
              -0.0000000560*pow(t,5);

 // Convert to radians
 eps0*=pi/(180.*3600.);
 psi*=pi/(180.*3600.);
 om*=pi/(180.*3600.);
 chi*=pi/(180.*3600.);

 Double_t s1=sin(eps0);
 Double_t s2=sin(-psi);
 Double_t s3=sin(-om);
 Double_t s4=sin(chi);
 Double_t c1=cos(eps0);
 Double_t c2=cos(-psi);
 Double_t c3=cos(-om);
 Double_t c4=cos(chi);

 mat[0]=c4*c2-s2*s4*c3;
 mat[1]=c4*s2*c1+s4*c3*c2*c1-s1*s4*s3;
 mat[2]=c4*s2*s1+s4*c3*c2*s1+c1*s4*s3;
 mat[3]=-s4*c2-s2*c4*c3;
 mat[4]=-s4*s2*c1+c4*c3*c2*c1-s1*c4*s3;
 mat[5]=-s4*s2*s1+c4*c3*c2*s1+c1*c4*s3;
 mat[6]=s2*s3;
 mat[7]=-s3*c2*c1-s1*c3;
 mat[8]=-s3*c2*s1+c3*c1;

 fP.SetMatrix(mat);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetNmatrix(NcTimestamp* ts)
{
// Set nutation matrix elements for the specified Julian date jd.
// The formulas and numerical constants used are the ones from the 
// U.S. Naval Observatory (USNO) circular 179 (2005), which is available on
// http://aa.usno,navy.mil/publications/docs/Circular_179.pdf.

 Double_t mat[9]={0,0,0,0,0,0,0,0,0};
 if (!ts)
 {
  fN.SetMatrix(mat);
  return;
 }

 Double_t pi=acos(-1.);
 
 Double_t dpsi,deps,eps;
 ts->Almanac(&dpsi,&deps,&eps);

 // Convert to radians
 dpsi*=pi/(180.*3600.);
 deps*=pi/(180.*3600.);
 eps*=pi/(180.*3600.);

 Double_t s1=sin(eps);
 Double_t s2=sin(-dpsi);
 Double_t s3=sin(-(eps+deps));
 Double_t c1=cos(eps);
 Double_t c2=cos(-dpsi);
 Double_t c3=cos(-(eps+deps));

 mat[0]=c2;
 mat[1]=s2*c1;
 mat[2]=s2*s1;
 mat[3]=-s2*c3;
 mat[4]=c3*c2*c1-s1*s3;
 mat[5]=c3*c2*s1+c1*s3;
 mat[6]=s2*s3;
 mat[7]=-s3*c2*c1-s1*c3;
 mat[8]=-s3*c2*s1+c3*c1;

 fN.SetMatrix(mat);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetGmatrix(TString mode)
{
// Set the mean equatorial to galactic coordinate conversion matrix.
// The B1950 parameters were taken from section 22.3 of the book
// "An Introduction to Modern Astrophysics" by Carrol and Ostlie (1996).
// The J2000 parameters are obtained by precession of the B1950 values.
//
// Via the input argument "mode" the required epoch can be selected
// mode = "B" ==> B1950
//        "J" ==> J2000

 Nc3Vector x; // The Galactic x-axis in the equatorial frame
 Nc3Vector y; // The Galactic y-axis in the equatorial frame
 Nc3Vector z; // The Galactic z-axis in the equatorial frame

 Double_t a,d;
 Double_t vec[3]={1,0,0};

 fGal=1; // Set flag to indicate B1950 matrix values

 // B1950 equatorial coordinates of the North Galactic Pole (NGP)
 a=124900.;
 d=272400.;
 a=ConvertAngle(a,"hms","deg");
 d=ConvertAngle(d,"dms","deg");
 vec[1]=90.-d;
 vec[2]=a;
 z.SetVector(vec,"sph","deg");

 // B1950 equatorial coordinates of the Galactic l=b=0 point
 a=174224.;
 d=-285500.;
 a=ConvertAngle(a,"hms","deg");
 d=ConvertAngle(d,"dms","deg");
 vec[1]=90.-d;
 vec[2]=a;
 x.SetVector(vec,"sph","deg");

 // Precess to the corresponding J2000 values if requested
 if (mode=="J")
 {
  fGal=2; // Set flag to indicate J2000 matrix values
  NcTimestamp t1;
  t1.SetEpoch(1950,"B");
  NcTimestamp t2;
  t2.SetEpoch(2000,"J");
  Precess(z,&t1,&t2);
  Precess(x,&t1,&t2);
 }

 // The Galactic y-axis is determined for the right handed frame
 y=z.Cross(x);

 fG.SetAngles(x.GetX(2,"sph","deg"),x.GetX(3,"sph","deg"),
              y.GetX(2,"sph","deg"),y.GetX(3,"sph","deg"),
              z.GetX(2,"sph","deg"),z.GetX(3,"sph","deg"));
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetEmatrix(NcTimestamp* ts)
{
// Set the mean equatorial to ecliptic coordinate conversion matrix
// for the specified timestamp.
// A nice sketch and explanation of the two frames can be found
// in chapter 3 of the book "Astronomy Methods" by Hale Bradt (2004).

 Double_t dpsi,deps,eps;
 ts->Almanac(&dpsi,&deps,&eps);

 // Convert to degrees
 eps/=3600.;

 // Positions of the ecliptic axes w.r.t. the equatorial ones
 // at the moment of the specified timestamp 
 Double_t theta1=90; // Ecliptic x-axis
 Double_t phi1=0;
 Double_t theta2=90.-eps; //Ecliptic y-axis
 Double_t phi2=90;
 Double_t theta3=eps; // Ecliptic z-axis
 Double_t phi3=270;

 fE.SetAngles(theta1,phi1,theta2,phi2,theta3,phi3);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetHmatrix(NcTimestamp* ts)
{
// Set the mean equatorial to horizontal coordinate conversion matrix
// for the specified timestamp.
// A nice sketch and explanation of the two frames can be found
// in chapter 3 of the book "Astronomy Methods" by Hale Bradt (2004).
//
// Note : In order to simplify the calculations, we use here a
//        right-handed horizontal frame.

 Nc3Vector x; // The (South pointing) horizontal x-axis in the equatorial frame
 Nc3Vector y; // The (East pointing) horizontal y-axis in the equatorial frame
 Nc3Vector z; // The (Zenith pointing) horizontal z-axis in the equatorial frame

 Double_t l,b;
 GetLabPosition(l,b,"deg");

 Double_t a;
 Double_t vec[3]={1,0,0};

 // Equatorial coordinates of the horizontal z-axis
 // at the moment of the specified timestamp 
 a=ts->GetLAST(fToffset);
 a*=15.; // Convert fractional hours to degrees 
 vec[1]=90.-b;
 vec[2]=a;
 z.SetVector(vec,"sph","deg");

 // Equatorial coordinates of the horizontal x-axis
 // at the moment of the specified timestamp 
 vec[1]=180.-b;
 vec[2]=a;
 x.SetVector(vec,"sph","deg");

 // The horizontal y-axis is determined for the right handed frame
 y=z.Cross(x);

 fH.SetAngles(x.GetX(2,"sph","deg"),x.GetX(3,"sph","deg"),
              y.GetX(2,"sph","deg"),y.GetX(3,"sph","deg"),
              z.GetX(2,"sph","deg"),z.GetX(3,"sph","deg"));
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLocalFrame(Double_t t1,Double_t p1,Double_t t2,Double_t p2,Double_t t3,Double_t p3)
{
// Specification of the orientations of the local reference frame axes.
// The input arguments represent the angles (in degrees) of the local-frame axes
// w.r.t. a so called Master Reference Frame (MRF).
//
// The right handed Master Reference Frame (MRF) is defined as follows :
//  Z-axis : Points to the local Zenith
//  X-axis : Makes an angle of 90 degrees with the Z-axis and points South
//  Y-axis : Makes an angle of 90 degrees with the Z-axis and points East
//
// The various input arguments are defined as follows :
// t1 : Theta (=Zenith) angle of the local X-axis in the MRF
// p1 : Phi angle of the local X-axis in the MRF
// t2 : Theta (=Zenith) angle of the local Y-axis in the MRF
// p2 : Phi angle of the local Y-axis in the MRF
// t3 : Theta (=Zenith) angle of the local Z-axis in the MRF
// p3 : Phi angle of the local Z-axis in the MRF
//
// Example :
// ---------
// The local frame is identical to the MRF when (t1,p1,t2,p2,t3,p3)=(90,0,90,90,0,0).
//
// Once the user has specified the local reference frame, any observed event
// can be related to astronomical space-time locations via the SetSignal
// and GetSignal memberfunctions.

 // Set the matrix for the conversion of our reference frame coordinates
 // into the local-frame ones.

 fL.SetAngles(t1,p1,t2,p2,t3,p3);

 // Store the local user frame axes orientations w.r.t. the standard local Horizon (zen,azi) frame
 fAxes[0]=t1;
 fAxes[1]=p1;
 fAxes[2]=t2;
 fAxes[3]=p2;
 fAxes[4]=t3;
 fAxes[5]=p3;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GetLocalFrame(Float_t* arr)
{
// Provide the orientations of the local reference frame axes w.r.t.
// the Master Reference Frame (MRF).
// For details please refer to the memberfunction SetLocalFrame().
//
// Note : The provided array must have a minimum length of 6.

 arr[0]=fAxes[0];
 arr[1]=fAxes[1];
 arr[2]=fAxes[2];
 arr[3]=fAxes[3];
 arr[4]=fAxes[4];
 arr[5]=fAxes[5];
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::ConvertAngle(Double_t a,TString in,TString out) const
{
// Conversion of various angular formats.
//
// The input argument "a" denotes the angle to be converted. 
// The string arguments "in" and "out" specify the angular I/O formats.
//
// in = "rad" : input angle provided in radians
//      "deg" : input angle provided in degrees
//      "dms" : input angle provided in dddmmss.sss
//      "hms" : input angle provided in hhmmss.sss
//      "hrs" : input angle provided in fractional hours
//
// out = "rad" : output angle provided in radians
//       "deg" : output angle provided in degrees
//       "dms" : output angle provided in dddmmss.sss
//       "hms" : output angle provided in hhmmss.sss
//       "hrs" : output angle provided in fractional hours
 
 if (in==out) return a;

 // Convert input to its absolute value in (fractional) degrees. 
 Double_t pi=acos(-1.);
 Double_t epsilon=1.e-12; // Accuracy in (arc)seconds
 Int_t word=0,ddd=0,hh=0,mm=0,ss=0;
 Double_t s=0;

 Double_t b=fabs(a);

 if (in=="rad") b*=180./pi;

 if (in=="hrs") b*=15.;

 if (in=="dms")
 {
  word=Int_t(b);
  ddd=word/10000;
  word=word%10000;
  mm=word/100;
  ss=word%100;
  s=b-Double_t(ddd*10000+mm*100+ss);
  b=Double_t(ddd)+Double_t(mm)/60.+(Double_t(ss)+s)/3600.;
 }

 if (in=="hms")
 {
  word=Int_t(b);
  hh=word/10000;
  word=word%10000;
  mm=word/100;
  ss=word%100;
  s=b-Double_t(hh*10000+mm*100+ss);
  b=15.*(Double_t(hh)+Double_t(mm)/60.+(Double_t(ss)+s)/3600.);
 }

 while (b>360)
 {
  b-=360.;
 }

 if (out=="rad") b*=pi/180.;

 if (out=="hrs") b/=15.;

 if (out=="dms")
 {
  ddd=Int_t(b);
  b=b-Double_t(ddd);
  b*=60.;
  mm=Int_t(b);
  b=b-Double_t(mm);
  b*=60.;
  ss=Int_t(b);
  s=b-Double_t(ss);
  if (s>(1.-epsilon))
  {
   s=0.;
   ss++;
  }
  while (ss>=60)
  {
   ss-=60;
   mm++;
  }
  while (mm>=60)
  {
   mm-=60;
   ddd++;
  }
  while (ddd>=360)
  {
   ddd-=360;
  }
  b=Double_t(10000*ddd+100*mm+ss)+s;
 }

 if (out=="hms")
 {
  b/=15.;
  hh=Int_t(b);
  b=b-Double_t(hh);
  b*=60.;
  mm=Int_t(b);
  b=b-Double_t(mm);
  b*=60.;
  ss=Int_t(b);
  s=b-Double_t(ss);
  if (s>(1.-epsilon))
  {
   s=0.;
   ss++;
  }
  while (ss>=60)
  {
   ss-=60;
   mm++;
  }
  while (mm>=60)
  {
   mm-=60;
   hh++;
  }
  while (hh>=24)
  {
   hh-=24;
  }
  b=Double_t(10000*hh+100*mm+ss)+s;
 }

 if (a<0) b=-b;

 return b;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetSolidAngle(Double_t thetamin,Double_t thetamax,TString tu,Double_t phimin,Double_t phimax,TString pu) const
{
// Provide the solid angle (in sr) between the specified boundaries.
//
// Input arguments :
// -----------------
// thetamin : The minimum value of the polar angle theta
// thetamax : The maximum value of the polar angle theta
// tu       : The units in which theta is provided (see memberfunction ConvertAngle)
// phimin   : The minimum value of the azimuthal angle phi
// phimax   : The maximum of the azimuthal angle phi
// pu       : The units in which phi is provided (see memberfunction ConvertAngle)

 Double_t omega=0;

 Double_t th1=ConvertAngle(thetamin,tu,"rad");
 Double_t th2=ConvertAngle(thetamax,tu,"rad");
 Double_t ph1=ConvertAngle(phimin,pu,"rad");
 Double_t ph2=ConvertAngle(phimax,pu,"rad");

 omega=(ph2-ph1)*(cos(th1)-cos(th2));
 if (omega<0) omega=-omega;

 return omega;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetHourAngle(TString mode,NcTimestamp* ts,Int_t jref,Int_t type)
{
// Provide the Local Hour Angle (in fractional degrees) of a stored signal
// object at the specified timestamp.
// The hour angle is provided within the interval [-180,180], where
// negative (positive) values indicate positions East (West) of the observers meridian.
//
// The input parameter "mode" allows the user to select either the
// "mean" or "apparent" value for the returned Hour Angle.
//
// mode = "M" --> Output is the Mean Hour Angle
//        "A" --> Output is the Apparent Hour Angle
// ts   : Timestamp at which the hour angle is requested.
//
// The input parameters "jref" and "type" allow to specify measured or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with the
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// jref = j --> Use the signal at the j-th position (j=1 is first)
//
// type = 0 --> Select a stored reference signal
//        1 --> Select a stored measurement
//
// Note :
// ------
// For jref=0 always the first stored measurement will be selected for backward compatibility,
// but the user is advised not to use this obsolete method anymore.
//
// Default values are jref=0 and type=0.
//
// Note : In case ts=0 the current timestamp of the lab will be taken.

 if (!ts) ts=(NcTimestamp*)this;

 // Get corrected right ascension and declination for the specified timestamp.
 Double_t d,a,b;
 if (mode=="M" || mode=="m") GetSignal(d,a,"deg",b,"deg","equ",ts,jref,"M",type);
 if (mode=="A" || mode=="a") GetSignal(d,a,"deg",b,"deg","equ",ts,jref,"T",type);

 a/=15.; // Convert a to fractional hours
 Double_t ha=0;
 if (mode=="M" || mode=="m") ha=ts->GetLMST(fToffset)-a;
 if (mode=="A" || mode=="a") ha=ts->GetLAST(fToffset)-a;
 ha*=15.; // Convert to (fractional) degrees

 // Project to the interval [-180,180]
 while (ha<-180)
 {
  ha+=360.;
 }
 while (ha>180)
 {
  ha-=360.;
 }

 return ha;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLT(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns,Int_t ps)
{
// Set the NcTimestamp parameters corresponding to the (mean) local date and time (LT)
// in the Gregorian calendar as specified by the input arguments.
// This facility is valid for all AD dates in the Gregorian calendar.
//
// Note : One should NOT use Daylight Saving Time (DST) 
//
// The input arguments represent the following :
// y  : year in LT (e.g. 1952, 2003 etc...)
// m  : month in LT (1=jan  2=feb etc...)
// d  : day in LT (1-31)
// hh : elapsed hours in LT (0-23) 
// mm : elapsed minutes in LT (0-59)
// ss : elapsed seconds in LT (0-59)
// ns : remaining fractional elapsed second of LT in nanosecond
// ps : remaining fractional elapsed nanosecond of LT in picosecond
//
// Note : ns=0 and ps=0 are the default values.
//

 SetLT(fToffset,y,m,d,hh,mm,ss,ns,ps);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLT(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Double_t s)
{
// Set the NcTimestamp parameters corresponding to the (mean) local date and time (LT)
// in the Gregorian calendar as specified by the input arguments.
// This facility is valid for all AD dates in the Gregorian calendar.
//
// Note : One should NOT use Daylight Saving Time (DST) 
//
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetLT() memberfunctions.
//
// The input arguments represent the following :
// y  : year in LT (e.g. 1952, 2003 etc...)
// m  : month in LT (1=jan  2=feb etc...)
// d  : day in LT (1-31)
// hh : elapsed hours in LT (0-23) 
// mm : elapsed minutes in LT (0-59)
// s  : elapsed (fractional) seconds in LT (0-59.999...)

 SetLT(fToffset,y,m,d,hh,mm,s);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLT(Int_t y,Int_t m,Int_t d,TString time)
{
// Set the NcTimestamp parameters corresponding to the (mean) local date and time (LT)
// in the Gregorian calendar as specified by the input arguments.
// This facility is valid for all AD dates in the Gregorian calendar.
//
// Note : One should NOT use Daylight Saving Time (DST) 
//
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetLT() memberfunctions.
//
// The input arguments represent the following :
// y    : year in LT (e.g. 1952, 2003 etc...)
// m    : month in LT (1=jan  2=feb etc...)
// d    : day in LT (1-31)
// time : The LT time in the format hh:mm:ss.sss

 SetLT(fToffset,y,m,d,time);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLT(TString date,TString time,Int_t mode)
{
// Set the NcTimestamp parameters corresponding to the (mean) local date and time (LT)
// in the Gregorian calendar as specified by the input arguments.
// This facility is valid for all AD dates in the Gregorian calendar.
//
// Note : One should NOT use Daylight Saving Time (DST) 
//
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetLT() memberfunctions.
//
// The input arguments represent the following :
// date : The LT date in the format dd-mm-yyyy or dd/mm/yyyy or ddmmyyyy (mode=0)
// date : The LT date in the format yyyy-mm-dd or yyyy/mm/dd or yyyymmdd (mode=1)
// date : The LT date in the format mm-dd-yyyy or mm/dd/yyyy or mmddyyyy (mode=2)
// date : The LT date in the format yyyy-dd-mm or yyyy/dd/mm or yyyyddmm (mode=3)
// time : The LT time in the format hh:mm:ss.sss
// mode : Date format specifier (see above)

 SetLT(fToffset,date,time,mode);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLT(Int_t y,Int_t d,Int_t s,Int_t ns,Int_t ps)
{
// Set the NcTimestamp parameters corresponding to the specified elapsed
// timespan since the beginning of the new LT year.
//
// The LT year and elapsed time span is entered via the following input arguments :
//
// y  : year in LT (e.g. 1952, 2003 etc...)
// d  : elapsed number of days 
// s  : (remaining) elapsed number of seconds
// ns : (remaining) elapsed number of nanoseconds
// ps : (remaining) elapsed number of picoseconds
//
// The specified d, s, ns and ps values will be used in an additive
// way to determine the elapsed timespan.
// So, specification of d=1, s=100, ns=0, ps=0 will result in the
// same elapsed time span as d=0, s=24*3600+100, ns=0, ps=0.
// However, by making use of the latter the user should take care
// of possible integer overflow problems in the input arguments,
// which obviously will provide incorrect results. 
//
// Note : ns=0 and ps=0 are the default values.

 SetLT(fToffset,y,d,s,ns,ps);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetDifference(Int_t j,TString au,Double_t& dt,TString tu,Int_t mode,Int_t* ia,Int_t* it)
{
// ************************************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility.             ***
// *** The user is advised to use the memberfunction GetSeparation() or MatchSignals() instead. ***
// ************************************************************************************************
//
// Provide space and time difference between the j-th reference signal
// (j=1 indicates first) and the single stored measurement at storage index 1.
// 
// The return value of this memberfunction provides the positional angular
// difference, whereas the output argument "dt" provides the time difference.
//
// The units of the angular difference can be specified via the the "au"
// input argument, where
//
// au = "rad" --> Angular difference in (fractional) radians
//      "deg" --> Angular difference in (fractional) degrees
//
// The units of the time difference can be specified via the "tu" and "mode"
// input arguments. For details please refer to NcTimestamp::GetDifference().
// Also here mode=1 is the default value.
//
// For the time difference the reference signal is used as the standard.
// This means that in case of a positive time difference, the stored
// measurement occurred later than the reference signal.
//
// In case j=0, the stored measurement will be compared with each
// reference signal and the returned angular and time differences are
// the minimal differences which were encountered.
// In this case the user may obtain the indices of the two stored reference signals
// which had the minimal angular and minimal time difference via the output
// arguments "ia" and "it" as follows :
//
// ia = Index of the stored reference signal with minimial angular difference
// it = Index of the stored reference signal with minimial time difference
//
// In case these indices are the same, there obviously was 1 single reference signal
// which showed both the minimal angular and time difference.
//
// The default values are mode=1, ia=0 and it=0;
//
// Note : In case scrambling has been activated, the results will be scrambled accordingly.

 Double_t da=999;
 dt=1.e30;

 if (ia) *ia=0;
 if (it) *it=0;

 if (j<0) return da;

 NcDevice matches;
 Int_t nhits=0;
 NcSignal* sx=0;
 if (j) // Space and time difference w.r.t. a specific reference signal
 {
  MatchSignals(matches,da,au,dt,tu,mode,j,j,0,1,1,1);
  nhits=matches.GetNhits();
  if (nhits)
  {
   da=matches.GetSignal(1);
   dt=matches.GetSignal(2);
   if (ia) *ia=j;
   if (it) *it=j;
  }
 }
 else // Minimal space and time difference encountered over all reference signals
 {
  MatchSignals(matches,da,au,dt,tu,mode,1,0,0,1,1,1);
  nhits=matches.GetNhits();
  if (nhits)
  {
   da=matches.GetSignal(1);
   dt=matches.GetSignal(2);
   if (ia)
   {
    Int_t ipsi=matches.GetSignal("ipsi");
    sx=matches.GetHit(ipsi);
    if (sx) *ia=sx->GetSignal("index1");
   }
   if (it)
   {
    Int_t idt=matches.GetSignal("idt");
    sx=matches.GetHit(idt);
    if (sx) *it=sx->GetSignal("index1");
   }
  }
 }
 return da;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetSeparation(TString name1,TString name2,TString au,Double_t* dt,TString tu,Int_t mode,Double_t* diftheta,Double_t* difphi)
{
// Provide space (and optional time) separation between the signals stored with "name1" and "name2".
//
// Note :
// ------
// In case both signals were stored as a measurement (or reference signal),
// then the signal indicated via "name1" will be regarded as a reference signal,
// and the signal indicated via "name2" will be regarded as a measurement.
//
// In case a name specifies a solar system object supported by SetSolarSystem(),
// which was not yet stored, the corresponding reference signal will be created and stored.
// In case the solar system object was already stored as a reference object, its position and timestamp
// will be updated according to the timestamp of the measurement to which it will be compared.
// In case the solar system object was stored as a measurement, its position and timestamp are not modfified.
// All geocentric name specifications for solar system objects as indicated in the
// docs of NcTimestamp::Almanac() are supported.
// 
// The return value of this memberfunction provides the positional angular
// separation, whereas the optional output argument "dt" provides the time difference.
// The optional output arguments "diftheta" and "difphi" provide the difference
// in theta and phi, respectively, between the signals, in which the reference signal is used as the origin.  
// The units of "diftheta" and "difphi" are determined by the "au" input argument (see below).
//
// In case of inconsistent input, the return value and the optional "diftheta" and "difphi"
// will have the unphysical value of -999, whereas "dt" will be set to 1e30.
//
// The units of the angular separation can be specified via the "au" input argument, where
//
// au = "rad" --> Angular separation in (fractional) radians
//      "deg" --> Angular separation in (fractional) degrees
//
// The units of the time difference can be specified via the "tu" and "mode"
// input arguments. For details please refer to NcTimestamp::GetDifference().
//
// For the time difference the reference signal is used as the standard.
// This means that in case of a positive time difference, the other signal
// occurred later than the reference one.
//
// Note : In case scrambling has been activated, the results will be scrambled accordingly,
//        but only in case of positive values of the time c.q. position scrambling modes.
//        For details see the docs of SetTimeScramble() and SetPositionScramble().
//
// The default values are dt=0, tu="s", mode=1, diftheta=0 and difphi=0.

 // Obtain the storage indices for the requested signals
 Int_t i=GetSignalIndex(name1,1); // Search for "name1" among the measurement signals
 Int_t j=GetSignalIndex(name2,1); // Search for "name2" among the measurement signals

 if (i>0) // Set to negative value to indicate measurement entry
 {
  i=-i;
 }
 else // Search for "name1" among the reference signals
 {
  // In case a Solar system body was specified, store c.q. update it as a reference object.
  SetSolarSystem(name1,0,0);
  i=GetSignalIndex(name1,0);
  if (i<0) i=0;
 }

 if (j>0) // Set to negative value to indicate measurement entry
 {
  j=-j;
 }
 else // Search for "name2" among the reference signals
 {
  // In case a Solar system body was specified, store c.q. update it as a reference object.
  SetSolarSystem(name2,0,0);
  j=GetSignalIndex(name2,0);
  if (j<0) j=0;
 }

 // Retrieve the requested data via the internal GetSeparation() memberfunction
 Double_t dtx;
 Double_t da=GetSeparation(i,j,au,dtx,tu,mode,0,diftheta,difphi);

Double_t dthetax,dphix;
da=GetSeparation(i,j,au,dtx,tu,mode,0,&dthetax,&dphix);
printf(" i=%-i j=%-i da=%-g dt=%-g %-s dtheta=%-g dphi=%-g %-s \n",i,j,da,dtx,tu.Data(),dthetax,dphix,au.Data());

 if (dt) *dt=dtx;
 return da;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetSeparation(Int_t i,Int_t j,TString au,Double_t& dt,TString tu,Int_t mode,Int_t bkgpatch,Double_t* diftheta,Double_t* difphi)
{
// Internal memberfunction to provide space and time separation between the i-th and j-th stored signal.
// This memberfunction serves the public GetSeparation(), MatchSignals() and MatchBurstData() facilities.
//
// i > 0 : Indicates a reference signal stored at index i (i=1 is the first)
// i < 0 : Indicates a measurement stored at index abs(i) (i=-1 is the first)
// j > 0 : Indicates a reference signal stored at index j (j=1 is the first)
// j < 0 : Indicates a measurement stored at index abs(j) (j=-1 is the first)
//
// In case both signals were stored as a measurement (or reference signal),
// then the signal indicated via "i" will be regarded as a reference signal,
// and the signal indicated via "j" will be regarded as a measurement.
// 
// The return value of this memberfunction provides the positional angular
// separation, whereas the output argument "dt" provides the time difference.
// The optional output arguments "diftheta" and "difphi" provide the difference
// in theta and phi, respectively, between the i-th and the j-th stored signal,
// in which the reference signal is used as the origin.
// These optional values may be used for local zenith or azimuth band investigations.
// The units of "diftheta" and "difphi" are determined by the "au" input argument (see below).
//
// In case of inconsistent input, the return value and the optional "diftheta" and "difphi"
// will have the unphysical value of -999, whereas "dt" will be set to 1e30.
//
// The units of the angular separation can be specified via the "au" input argument, where
//
// au = "rad" --> Angular separation in (fractional) radians
//      "deg" --> Angular separation in (fractional) degrees
//
// The units of the time difference can be specified via the "tu" and "mode"
// input arguments. For details please refer to NcTimestamp::GetDifference().
//
// For the time difference the reference signal is used as the standard.
// This means that in case of a positive time difference, the other signal
// occurred later than the reference one.
//
// In case scrambling has been activated, the results will be scrambled accordingly.
// The argument "bkgpatch" indicates the background patch number for the creation of
// the corresponding off-source (background) data, where bkgpatch=0 means on-source data.
// Negative values for the time and position scrambling parameters are taken into account
// only for the creation of off-source (background) data, see MatchBurstData().
// For details see the docs of SetTimeScramble() and SetPositionScramble().
//
// The default values are diftheta=0 and difphi=0.

 Double_t dang=-999;
 dt=1.e30;
 if (diftheta) *diftheta=-999;
 if (difphi) *difphi=-999;

 if (!i || !j) return dang;
 if ((i>0 || j>0) && !fRefs) return dang;
 if ((i<0 || j<0) && !fSigs) return dang;

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // with identical measurements and reference signals without explicit initialisation
 // of the randomiser by the user at the start of the analysis.
 if (!fRan && (fRscmode>0 || fTscmode>0)) fRan=new NcRandom(-1);

 Int_t itype=0;
 if (i<0)
 {
  itype=1;
  i=abs(i);
 }

 Int_t jtype=0;
 if (j<0)
 {
  jtype=1;
  j=abs(j);
 }

 Nc3Vector ri; // Position of the i-th signal
 Nc3Vector rj; // Position of the j-th signal
 NcSignal* si=0; // Link to the stored i-th signal
 NcSignal* sj=0; // Link to the stored j-th signal
 NcTimestamp* ti=0; // Link to the timestamp of the i-th signal
 NcTimestamp* tj=0; // Link to the timestamp of the j-th signal

 si=GetSignal(i,itype);
 sj=GetSignal(j,jtype);
 if (!si || !sj) return dang;

 ti=si->GetTimestamp();
 tj=sj->GetTimestamp();
 if (!ti || !tj) return dang;

 Int_t reftype=0;
 Int_t evttype=0;
 Int_t idxref=0;
 Int_t idxevt=0;
 NcSignal* sxref=0; // Link to the stored reference signal
 NcTimestamp* txref=0; // Link to the timestamp of the reference signal
 NcTimestamp* txevt=0; // Link to the timestamp of the measurement signal
 Nc3Vector rxref; // Position of the reference signal
 Nc3Vector rxevt; // Position of the measurement signal

 if (itype==jtype) // Self correlations
 {
  reftype=itype;
  idxref=i;
  sxref=si;
  txref=ti;
  evttype=jtype;
  idxevt=j;
  txevt=tj;
 }
 else // Correlations between sources and measurements
 {
  if (itype)
  {
   evttype=itype;
   idxevt=i;
   txevt=ti;
   reftype=jtype;
   idxref=j;
   sxref=sj;
   txref=tj;
  }
  else
  {
   reftype=itype;
   idxref=i;
   sxref=si;
   txref=ti;
   evttype=jtype;
   idxevt=j;
   txevt=tj;
  }
 }

 // Update the location and timestamp in case of a Solar system reference object
 TString name=sxref->GetName();
 if (SetSolarSystem(name,txevt,reftype)) txref=sxref->GetTimestamp();

 // Local variables to act on requested scrambling
 Int_t Tscmode=fTscmode;
 Int_t Rscmode=fRscmode;

 // Create scrambled background patch data even when also the on-source data are scrambled 
 if (bkgpatch)
 {
  Tscmode=-abs(fTscmode);
  Rscmode=-abs(fRscmode);
 }

 // Apply time scrambling of the date/time of the observation if requested
 NcTimestamp txtmp(*txevt);
 Double_t trndm=0;
 if (Tscmode==1 || Tscmode==3 || (Tscmode<0 && bkgpatch))
 {
  if (!fTscfunc)
  {
   trndm=fRan->Uniform(fTscmin,fTscmax);
  }
  else
  {
   trndm=fTscfunc->GetRandom(fTscmin,fTscmax);
  }
  // Creating a fake scrambled timestamp
  if (Tscmode==3 || (abs(Tscmode)>1 && bkgpatch)) txtmp.AddSec(trndm);
 }

 // Obtain the (scrambled) time difference
 if (Tscmode==1 || (Tscmode==-1 && bkgpatch)) // Only provide a random time difference
 {
  if (tu=="d")
  {
   trndm/=double(24*3600);
  }
  if (tu=="ns")
  {
   trndm*=1.e9;
   trndm*=1.e9;
  }
  if (tu=="ps")
  {
   trndm*=1.e12;
   trndm*=1.e12;
  }
  dt=trndm;
 }
 else // Determine the time difference w.r.t. the fake timestamp of the measurement
 {
  dt=txref->GetDifference(&txtmp,tu,mode);
 }

 // Update the location of Solar system objects if needed
 name=sxref->GetName();
 SetSolarSystem(name,&txtmp,reftype);

 // Get the original measurement position and (time modified) reference position in local coordinates
 GetSignal(rxevt,"loc","T",txevt,idxevt,evttype);
 GetSignal(rxref,"loc","T",&txtmp,idxref,reftype);

 // Apply position scrambling of the measurement if requested
 Double_t pi=acos(-1.);
 Float_t cosmin=0;
 Float_t cosmax=0;
 Double_t cosang=0;
 Double_t vec[3];
 Double_t dd=0;
 Double_t dtheta=0;
 Double_t dphi=0;

 if (Rscmode==1 || (Rscmode==-1 && bkgpatch)) // Only provide a random angular separation
 {
  if (!fDscfunc)
  {
   cosmin=cos(fDscmin*pi/180.);
   cosmax=cos(fDscmax*pi/180.);
   if (cosmin>cosmax)
   {
    Float_t temp=cosmin;
    cosmin=cosmax;
    cosmax=temp;
   }
   cosang=fRan->Uniform(cosmin,cosmax);
   dang=acos(cosang);
   if (au=="deg") dang*=180./pi;
  }
  else
  {
   dang=fDscfunc->GetRandom(fDscmin,fDscmax);
   if (au=="rad") dang*=pi/180.;
  }
 }
 else // Create a fake local position for the measurement
 {
  if (Rscmode==3 || (abs(Rscmode)>1 && bkgpatch))
  {
   rxevt.GetVector(vec,"sph","deg"); 

   // Allow specific offset studies
   if (fDscmin==fDscmax) dd=fDscmin;
   if (fThetascmin==fThetascmax) dtheta=fThetascmin;
   if (fPhiscmin==fPhiscmax) dphi=fPhiscmin;

   // Go for randomly scrambled values
   if (fDscfunc)
   {
    if (fDscmax>fDscmin)
    {
     dd=fDscfunc->GetRandom(fDscmin,fDscmax);
    }
   }
   else 
   {
    if (fDscmax>fDscmin) dd=fRan->Uniform(fDscmin,fDscmax);
   }

   if (fThetascfunc)
   {
    if (fThetascmax>fThetascmin)
    {
     dtheta=fThetascfunc->GetRandom(fThetascmin,fThetascmax);
    }
   }
   else if (fThetascmax>fThetascmin)
   {
    cosmin=cos(fThetascmin*pi/180.);
    cosmax=cos(fThetascmax*pi/180.);
    if (cosmin>cosmax)
    {
     Float_t temp=cosmin;
     cosmin=cosmax;
     cosmax=temp;
    }
    cosang=fRan->Uniform(cosmin,cosmax);
    dtheta=acos(cosang)*180./pi;
   }

   if (fPhiscfunc)
   {
    if (fPhiscmax>fPhiscmin)
    {
     dphi=fPhiscfunc->GetRandom(fPhiscmin,fPhiscmax);
    }
   }
   else
   {
    if (fPhiscmax>fPhiscmin) dphi=fRan->Uniform(fPhiscmin,fPhiscmax);
   }

   vec[0]+=dd;
   if (vec[0]<=0) vec[0]=1e-20; // Keep a physical situation
   vec[1]+=dtheta;
   vec[2]+=dphi;
   rxevt.SetVector(vec,"sph","deg");
  }
  
  dang=rxref.GetOpeningAngle(rxevt,au);
 }

 // Determine the difference in local theta and phi
 Double_t refvec[3];
 rxevt.GetVector(vec,"sph",au);
 rxref.GetVector(refvec,"sph",au);
 dtheta=vec[1]-refvec[1];
 dphi=vec[2]-refvec[2];
 if (diftheta) *diftheta=dtheta;
 if (difphi) *difphi=dphi;

 return dang;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetDifference(TString name,TString au,Double_t& dt,TString tu,Int_t mode)
{
// ************************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility. ***
// *** The user is advised to use the memberfunction GetSeparation() instead.       ***
// ************************************************************************************
//
// Provide space and time difference between the stored reference signal
// with the specified name and the single stored measurement at storage index 1.
//
// Note : In case the name specifies a solar system object supported by SetSolarSystem()
//        which was not yet stored as a reference signal, the corresponding reference signal
//        will be created and stored with the current timestamp of this NcAstrolab instance. 
// 
// The return value of this memberfunction provides the positional angular
// difference, whereas the output argument "dt" provides the time difference.
//
// The units of the angular difference can be specified via the the "au"
// input argument, where
//
// au = "rad" --> Angular difference in (fractional) radians
//      "deg" --> Angular difference in (fractional) degrees
//
// The units of the time difference can be specified via the "tu" and "mode"
// input arguments. For details please refer to NcTimestamp::GetDifference().
// Also here mode=1 is the default value.
//
// For the time difference the reference signal is used as the standard.
// This means that in case of a positive time difference, the stored
// measurement occurred later than the reference signal.
//
// Note : In case scrambling has been activated, the results will be scrambled accordingly.

 Double_t dang=999;
 dt=1.e30;

 Int_t j=GetSignalIndex(name);

 if (j==-1) // Set and store info for the requested Solar system object if not already stored
 {
  SetSolarSystem(name,0);
  j=GetSignalIndex(name);
 }

 if (j>0) dang=GetDifference(j,au,dt,tu,mode);
 return dang;
}
///////////////////////////////////////////////////////////////////////////
TArrayI* NcAstrolab::MatchRefSignal(Double_t da,TString au,Double_t dt,TString tu,Int_t mode)
{
// ***********************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility ***
// *** The user is advised to use the memberfunction MatchSignals() instead        ***
// ***********************************************************************************
//
// Provide the storage indices of the reference signals which match in space
// and time with the single stored measurement at storage index 1.
// The indices are returned via a pointer to a TArrayI object.
// In case no matches were found, the null pointer is returned.
// A reference signal is regarded as matching with the stored measurement
// if the positional angular difference doesn't exceed "da" and the absolute
// value of the time difference doesn't exceed "dt".
//
// The units of the angular difference "da" can be specified via the the "au"
// input argument, where
//
// au = "rad" --> Angular difference in (fractional) radians
//      "deg" --> Angular difference in (fractional) degrees
//
// The units of the time difference "dt" can be specified via the "tu" and "mode"
// input arguments. For details please refer to NcTimestamp::GetDifference().
// Also here mode=1 is the default value.
//
// Note : In case scrambling has been activated, the results reflect the scrambled measurement.

 if (!fSigs || !fRefs) return 0;

 NcDevice matches;
 MatchSignals(matches,da,au,dt,tu,mode,1,0,0,1,1,1); // Perform the obsolete MatchRefSignal() action

 Int_t nhits=matches.GetNhits();
 if (!nhits) return 0;

 if (fIndices) delete fIndices;
 fIndices=new TArrayI(nhits);

 Int_t index=0;
 NcSignal* sx=0;
 Int_t jfill=0;
 for (Int_t i=1; i<=nhits; i++)
 {
  sx=matches.GetHit(i);

  if(!sx) continue;

  index=sx->GetSignal("index1");
  fIndices->AddAt(index,jfill);
  jfill++;
 }

 fIndices->Set(jfill);

 if (!jfill) return 0; // No match found

 return fIndices;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MatchSignals(NcDevice& matches,Double_t da,TString au,Double_t dt,TString tu,Int_t mode,Int_t i1,Int_t i2,Int_t itype,Int_t j1,Int_t j2,Int_t jtype)
{
// Provide information about the matching in space and time of the stored
// reference signal(s) and/or measurement(s).
// Signals are regarded as matching if the positional angular difference
// doesn't exceed "da" and the absolute value of the time difference doesn't exceed "dt".
//
// Notes :
// -------
// In case da<0 the signals will always be regarded as matching w.r.t. the positional angular difference. 
// In case dt<0 the signals will always be regarded as matching w.r.t. the time difference. 
//
// So, specifying da>0 and dt<0 will only check on matching of the locations of the signals, irrespective
// of the time of the various recordings. This is very convenient in the study of steady sources over
// extended time periods. 
//
// The information (see below) for the observed matches is provided via the NcDevice argument "matches",
// where each matching pair is recorded as a "hit" with the corresponding NcSignal data.
// Consequently, the number of matches can be obtained as the number of "hits" in the NcDevice.
//
// Note : This memberfunction will completely reset and re-define the NcDevice "matches".
//
// The units of the angular difference "da" can be specified via the the "au"
// input argument, where
//
// au = "rad" --> Angular difference in (fractional) radians
//      "deg" --> Angular difference in (fractional) degrees
//
// The units of the time difference "dt" can be specified via the "tu" and "mode"
// input arguments. For details please refer to NcTimestamp::GetDifference().
// Also here mode=1 is the default value.
//
// It is possible to investigate only a sub-set of the stored reference signals c.q. measurements
// via the input arguments "i1", "i2", "itype", "j1" and "j2", "jtype" as follows :
//
// i1    : Start index of the stored signals to be investigated (1=first stored signal)
// i2    : End index of the stored signals to be investigated (0=last stored signal)
// itype : 0 ==> [i1,i2] refer to stored reference signals
//         1 ==> [i1,i2] refer to stored measurements
// j1    : Start index of the stored signals to be investigated (1=first stored signal)
// j2    : End index of the stored signals to be investigated (0=last stored signal)
// jtype : 0 ==> [j1,j2] refer to stored reference signals
//         1 ==> [j1,j2] refer to stored measurements
//
// Note : Using itype=jtype allows investigation of (self)correlations within a sample. 
//        In this case the matching of a signal with itself will be skipped.
//
// The default values are i1=1, i2=0, itype=0, j1=1, j2=0 and jtype=1.
//
// The data for each matching signal pair provided as a "hit" of the NcDevice object is the following :
// Name   : Indicating the name (if any) of object1
// Title  : Indicating the name (if any) of object2
// Slot 1 : "type1"  --> Type of object1 (0=reference signal, 1=measurement) 
// Slot 2 : "index1" --> Storage index of object1 (see memberfunction GetSignal)
// Slot 3 : "type2"  --> Type of object2 (0=reference signal, 1=measurement) 
// Slot 4 : "index2" --> Storage index of object2 (see memberfunction GetSignal)
// Slot 5 : "psi"    --> Opening angle between object2 and object1 in units as specified via "au"
// Slot 6 : "t2-t1"  --> Time offset between object2 and object1 in units as specified via "tu" and "mode"
//                       (t2-t1)<0 in case object2 was recorded earlier than object1.
//
// The NcDevice object itself provides the data of the best encountered matchings as follows :
//
// Slot 1 : "psimin" --> Minimal encountered opening angle in units as specified via "au"
// Slot 2 : "dtmin"  --> Minimal encountered time offset |t2-t1| in units as specified via "tu" and "mode"
// Slot 3 : "ipsi"   --> Index of the hit that corresponds to the minimal opening angle
// Slot 4 : "idt"    --> Index of the hit that corresponds to the minimal time offset
//
// A full overview of all matchings can be obtained via NcDevice::Data().
//
// Note : In case scrambling has been activated, the results will reflect scrambled data,
//        but only in case of positive values of the time c.q. position scrambling modes.
//        For details see the docs of SetTimeScramble() and SetPositionScramble().
//
// Usage example :
// ---------------
// NcAstrolab lab;
// // Enter various measurements and reference signals into "lab"
// lab.SetSignal(...);
// lab.SetSignal(...);
// lab.SetSignal(...);
// ...
// ...
// ...
// // Look for matches between the measurements and reference signals
// // within 10 degrees opening angle and 600 seconds time offset.
// NcDevice matches;
// lab.MatchSignals(matches,10,"deg",600,"s");
//
// // List all the encountered matching details
// matches.Data();
//
// // Retrieve the minimal encountered opening angle
// Float_t angmin=matches.GetSignal(1);
//
// // Access the data of the matching pair (i.e. "hit") that yielded the minimal time offset
// Int_t ih=matches.GetSignal("idt");
// NcSignal* sx=matches.GetHit(ih);
// sx->Data();
//
// // Access the originally recorded signal of object1 that yielded the minimal opening angle
// // and retrieve its timestamp and the corresponding MJD. 
// ih=matches.GetSignal("ipsi");
// sx=matches.GetHit(ih);
// Int_t itype=sx->GetSignal("type1");
// Int_t i=sx->GetSignal("index1");
// sx=lab.GetSignal(i,itype);
// NcTimestamp* tx=sx->GetTimestamp();
// tx->Date();
// Double_t mjd1=tx->GetMJD();

 // Initialize the Device/Hit structure to contain the correlation info 
 matches.Reset(1);
 matches.SetHitCopy(1);

 TString name="Matches";
 TString title="Space and time matchings of NcAstrolab stored signals";
 matches.SetNameTitle(name.Data(),title.Data());
 TString tux=tu;
 if (tu=="d") tux="days";
 if (tu=="s") tux="sec";
 TString namedamin="psimin in "; 
 namedamin+=au;
 TString namedtmin="dtmin in ";
 namedtmin+=tux;
 matches.AddNamedSlot(namedamin);
 matches.AddNamedSlot(namedtmin);
 matches.AddNamedSlot("ipsi");
 matches.AddNamedSlot("idt");

 NcSignal data;
 TString nameda="psi in "; 
 nameda+=au;
 TString namedt="t2-t1 in ";
 namedt+=tux;
 data.AddNamedSlot("type1");
 data.AddNamedSlot("index1");
 data.AddNamedSlot("type2");
 data.AddNamedSlot("index2");
 data.AddNamedSlot(nameda);
 data.AddNamedSlot(namedt);

 if ((!itype || !jtype) && !fRefs)
 {
  printf(" *%-s::MatchSignals* Error: itype=%-i jtype=%-i but no reference signals are present. \n",ClassName(),itype,jtype); 
  return;
 }

 if ((itype || jtype) && !fSigs)
 {
  printf(" *%-s::MatchSignals* Error: itype=%-i jtype=%-i but no measurements are present. \n",ClassName(),itype,jtype); 
  return;
 }

 Int_t nrefs=0;
 if (fRefs) nrefs=fRefs->GetSize();
 Int_t nsigs=0;
 if (fSigs) nsigs=fSigs->GetSize();


 // Make input data consistent with conventions
 if (itype) itype=1;
 if (jtype) jtype=1;
 if (!itype)
 {
  if (i2<1 || i2>nrefs) i2=nrefs;
 }
 else
 {
  if (i2<1 || i2>nsigs) i2=nsigs;
 }
 if (!jtype)
 {
  if (j2<1 || j2>nrefs) j2=nrefs;
 }
 else
 {
  if (j2<1 || j2>nsigs) j2=nsigs;
 }

 if (i1<1 || j1<1 || i1>i2 || j1>j2)
 {
  printf(" *%-s::MatchSignals* Inconsistent parameters: i1=%-i i2=%-i itype=%-i j1=%-i j2=%-i jtype=%-i. \n",ClassName(),i1,i2,itype,j1,j2,jtype); 
  return;
 }

 Double_t dang,dtime;
 Int_t ix=0;
 Int_t jx=0;
 NcSignal* sx=0;
 Int_t id=0;
 Double_t dangmin=0;
 Double_t dtmin=0;
 Int_t idamin=0;
 Int_t idtmin=0;
 Bool_t first=kTRUE;
 for (Int_t i=i1; i<=i2; i++)
 {
  ix=i;
  if (itype) ix=-i;

  for (Int_t j=j1; j<=j2; j++)
  {
   // Skip matching a signal with itself
   if (itype==jtype && i==j) continue;

   jx=j;
   if (jtype) jx=-j;

   dang=GetSeparation(ix,jx,au,dtime,tu,mode,0);

   if ((fabs(dang)<=da || da<0) && (fabs(dtime)<=dt || dt<0)) 
   {
    data.Reset();
    name="Object1=";
    sx=GetSignal(i,itype);
    if (!sx) continue;
    name+=sx->GetName();
    title="Object2=";
    sx=GetSignal(j,jtype);
    if (!sx) continue;
    title+=sx->GetName();
    id++;
    data.SetNameTitle(name.Data(),title.Data());
    data.SetUniqueID(id);
    data.SetSignal(itype,"type1");
    data.SetSignal(i,"index1");
    data.SetSignal(jtype,"type2");
    data.SetSignal(j,"index2");
    data.SetSignal(dtime,namedt);
    data.SetSignal(dang,nameda);
    matches.AddHit(data);

    // Record the data for the minimal encountered opening angle
    if (first || fabs(dang)<dangmin)
    {
     dangmin=fabs(dang);
     idamin=id;
    }

    // Record the data for the minimal encountered time difference
    if (first || fabs(dtime)<fabs(dtmin))
    {
     dtmin=dtime;
     idtmin=id;
    }
    
    first=kFALSE;
   }
  }
 }

 // Store the data for the minimal encountered opening angle and time difference
 matches.SetSignal(dangmin,namedamin);
 matches.SetSignal(dtmin,namedtmin);
 matches.SetSignal(idamin,"ipsi");
 matches.SetSignal(idtmin,"idt");
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MatchSignals(NcDevice& matches,TString name,Double_t da,TString au,Double_t dt,TString tu,Int_t mode,Int_t itype,Int_t j1,Int_t j2,Int_t jtype)
{
// Provide information about the matching in space and time of the stored
// reference signal(s) and/or measurement(s).
// Signals are regarded as matching if the positional angular difference
// doesn't exceed "da" and the absolute value of the time difference doesn't exceed "dt".
//
// Notes :
// -------
// In case da<0 the signals will always be regarded as matching w.r.t. the positional angular difference. 
// In case dt<0 the signals will always be regarded as matching w.r.t. the time difference. 
//
// So, specifying da>0 and dt<0 will only check on matching of the locations of the signals, irrespective
// of the time of the various recordings. This is very convenient in the study of steady sources over
// extended time periods. 
//
// name  : The name of the object to be used for the matching with the stored signals 
// itype : 0 ==> The object "name" resides among the stored reference signals
//         1 ==> The object "name" resides among the stored measurements
// j1    : Start index of the stored signals to be investigated (1=first stored signal)
// j2    : End index of the stored signals to be investigated (0=last stored signal)
// jtype : 0 ==> [j1,j2] refer to stored reference signals
//         1 ==> [j1,j2] refer to stored measurements
//
// The default values are itype=0, j1=1, j2=0 and jtype=1.
//
// Note : In case the name specifies a solar system object which was not yet stored,
//        the corresponding signal will be created according to "itype" and initially
//        stored with the current timestamp of this NcAstrolab instance.
//        In the matching process the location of the solar system object will be updated
//        according to the timestamp of the other signal. 
//        All geocentric name specifications for solar system objects as indicated in the
//        docs of NcTimestamp::Almanac() are supported.
//
// The information (see below) for the observed matches is provided via the NcDevice argument "matches",
// where each matching pair is recorded as a "hit" with the corresponding NcSignal data.
// Consequently, the number of matches can be obtained as the number of "hits" in the NcDevice.
//
// Note : This memberfunction will completely reset and re-define the NcDevice "matches".
//
// The units of the angular difference "da" can be specified via the the "au"
// input argument, where
//
// au = "rad" --> Angular difference in (fractional) radians
//      "deg" --> Angular difference in (fractional) degrees
//
// The units of the time difference "dt" can be specified via the "tu" and "mode"
// input arguments. For details please refer to NcTimestamp::GetDifference().
// Also here mode=1 is the default value.
//
// The data for each matching signal pair provided as a "hit" of the NcDevice object is the following :
// Name   : Indicating the name (if any) of object1
// Title  : Indicating the name (if any) of object2
// Slot 1 : "type1"  --> Type of object1 (0=reference signal, 1=measurement) 
// Slot 2 : "index1" --> Storage index of object1 (see memberfunction GetSignal)
// Slot 3 : "type2"  --> Type of object2 (0=reference signal, 1=measurement) 
// Slot 4 : "index2" --> Storage index of object2 (see memberfunction GetSignal)
// Slot 5 : "psi"    --> Opening angle between object2 and object1 in units as specified via "au"
// Slot 6 : "t2-t1"  --> Time offset between object2 and object1 in units as specified via "tu" and "mode"
//
// The NcDevice object itself provides the data of the best encountered matchings as follows :
//
// Slot 1 : "psimin" --> Minimal encountered opening angle in units as specified via "au"
// Slot 2 : "dtmin"  --> Minimal encountered time offset in units as specified via "tu" and "mode"
// Slot 3 : "ipsi"   --> Index of the hit that corresponds to the minimal opening angle
// Slot 4 : "idt"    --> Index of the hit that corresponds to the minimal time offset
//
// A full overview of all matchings can be obtained via NcDevice::Data().
//
// Note : In case scrambling has been activated, the results reflect the scrambled measurement(s).
//
// Usage example :
// ---------------
// NcAstrolab lab;
// // Enter various measurements and reference signals into "lab"
// lab.SetSignal(...);
// lab.SetSignal(...);
// lab.SetSignal(...);
// ...
// ...
// ...
// // Look for matches between GRB140611A and the recorded measurements
// // within 10 degrees opening angle and 600 seconds time offset.
// NcDevice matches;
// lab.MatchSignals(matches,10,"deg",600,"s","GRB140611A");
//
// // List all the encountered matching details
// matches.Data();
//
// // Retrieve the minimal encountered opening angle
// Float_t angmin=matches.GetSignal(1);
//
// // Access the data of the matching pair (i.e. "hit") that yielded the minimal time offset
// Int_t ih=matches.GetSignal("idt");
// NcSignal* sx=matches.GetHit(ih);
// sx->Data();
//
// // Access the originally recorded signal of object2 that yielded the minimal opening angle
// // w.r.t. GRB140611A and retrieve its timestamp and the corresponding MJD. 
// ih=matches.GetSignal("ipsi");
// sx=matches.GetHit(ih);
// Int_t jtype=sx->GetSignal("type2");
// Int_t j=sx->GetSignal("index2");
// sx=lab.GetSignal(j,jtype);
// NcTimestamp* tx=sx->GetTimestamp();
// tx->Date();
// Double_t mjd2=tx->GetMJD();

 Int_t i=GetSignalIndex(name,itype);

 if (i==-1) // Add the info for the requested Solar system object if not already stored
 {
  SetSolarSystem(name,0,itype);
  i=GetSignalIndex(name,itype);
  if (i>0) fSolUpdate=1;
 }

 if (i<1)
 {
  printf(" *%-s::MatchSignals* Object %-s not found for itype=%-i. \n",ClassName(),name.Data(),itype); 
 }
 else
 {
  MatchSignals(matches,da,au,dt,tu,mode,i,i,itype,j1,j2,jtype);
 }

 fSolUpdate=0;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetTimeScramble(Int_t mode,Double_t tmin,Double_t tmax,TF1* frndm)
{
// Set parameters for time scrambling of the measurements.
// This scrambling facility is very useful for studying (accidental) background
// correlations and performing blind analyses.
//
// mode  : Mode for performing the time scrambling (see below)
// tmin  : Minimal value (in seconds) for time scrambling
// tmax  : Maximal value (in seconds) for time scrambling
// frndm : Randomising function to perform the scrambling
//
// Conventions for the mode parameter :
// ------------------------------------
// mode : 0 ==> No time scrambling is performed.
//        1 ==> Scrambling is only performed after having obtained the actual time difference.
//              So, the actual (stored) timestamps, retrieved coordinates c.q. angular differences
//              and the NcTimestamp::GetDifference() are not affected by the scrambling.
//              This implies that setting mode=0 will reproduce again the unscrambled values.
//              The provided time difference is randomly selected from the interval [tmin,tmax].
//              As such, this scrambling mode is very useful for scrambling entries in
//              specific time windows (see e.g. NvE, Astroparticle Physics 28 (2008) 540),
//              without modification of the event selection based on angular separation w.r.t. a source.
//              This time difference scrambling will only affect the outcome of the memberfunctions
//              GetDifference(), MatchRefSignal(), MatchSignals() and MatchBurstData() of this class NcAstrolab.
//              Note : Every time a time difference is requested, a new random value is produced.
//                     As such, scrambled values are not reproducible on subsequent invokations.
//        2 ==> Time scrambling is performed before storing the measurement.
//              A value is randomly selected from the interval [tmin,tmax] and added to the 
//              actual timestamp of the measurement.
//              The measurement will then be stored with the modified timestamp. 
//              As such, this scrambling is irreversible but will always provide reproducible results.
//              Note : In case the measurement is entered in NcAstrolab specific coordinates
//                     (i.e. local or horizontal position coordinates) then this mode=2 scrambling
//                     will also result in a corresponding scrambling of e.g. the right ascension.
//              Possible patterns in the distribution of sources on the sky will stay intact.
//        3 ==> Time scrambling is performed by using fake timestamps for the reference objects c.q. sources.
//              To create a fake timestamp, a value is randomly selected from the interval [tmin,tmax] and
//              added to the actual timestamp of the measurement.
//              This fake timestamp is then used to retrieve the local coordinates of the reference object,
//              whereas the local coordinates of the measurement are retrieved with the actual unmodified timestamp.
//              This implies that the time and local c.q. horizontal position coordinates of the measurement
//              will remain unchanged, but those of the reference object get modified.
//              These new local coordinates and time will then be used to obtain the and angular and time differences.
//              As such, this scrambling mode is very well suited to scramble both time and angular differences
//              w.r.t. sources, but keeping the local position (and consequently the reconstruction efficiency)
//              of the measurement unchanged.
//              So, the actual stored timestamps and coordinates are not affected by the scrambling.
//              This implies that setting mode=0 will reproduce again the unscrambled values.
//              This time scrambling will only affect the outcome of the memberfunctions
//              GetDifference(), MatchRefSignal(), MatchSignals() and MatchBurstData() of this class NcAstrolab.
//              Note : Every time a time difference is requested, a new random value is produced.
//                     As such, scrambled values are not reproducible on subsequent invokations.
//              Possible patterns in the distribution of sources on the sky will be washed out.
//       <0 ==> Similar as the corresponding mode>0, but the scrambling is only performed to obtain
//              the off-source background data in the memberfunction MatchBurstData().
//              So, the actual on-source recordings are not affected.  
//              Furthermore, for mode=-2 the scrambled timestamps are only used after retrieving the measurement
//              data from the storage, so the actual stored measurement data are not modified.   
//
// Notes :
// -------
// In case frndm=0 the scrambling will be performed via a uniform distribution.
//
// The default is frndm=0.
//
// In the NcAstrolab constructor the time scrambling is switched off
// by setting mode=0 explicitly.

 fTscmode=mode;
 fTscmin=tmin;
 fTscmax=tmax;
 if (fTscfunc)
 {
  delete fTscfunc;
  fTscfunc=0;
 }
 if (frndm)
 {
  fTscfunc=new TF1(*frndm);
  if (tmax>tmin) fTscfunc->SetRange(tmin,tmax);
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::GetTimeScramble(Double_t* tmin,Double_t* tmax,TF1* frndm)
{
// Provide parameters which are used for time scrambling of the measurements.
//
// The standard returned value is the "time scrambling mode". 
// The memberfunction arguments provide the possibility of retrieving
// various optional values. The corresponding parameters are :
//
// tmin  : Minimal value (in seconds) for time scrambling
// tmax  : Maximal value (in seconds) for time scrambling
// frndm : Randomising function to perform the scrambling
//
// For a detailed description of the various time scrambling parameters
// please refer to the docs of the memberfunction SetTimeScramble.
//
// Note : In case an argument is entered as zero, the corresponding parameter
//        value will not be provided.
//
// Default argument values are tmin=0, tmax=0 and frndm=0.  
//
// Invokation example :
// --------------------
// NcAstrolab lab;
// Int_t mode;
// Double_t tlow,tup;
// TF1 func;
// mode=lab.GetTimeScramble(&tlow,&tup,&func);

 if (tmin) *tmin=fTscmin; 
 if (tmax) *tmax=fTscmax;
 if (frndm) *frndm=*fTscfunc;

 return fTscmode; 
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetPositionScramble(Int_t mode,Double_t dmin,Double_t dmax,TF1* df,Double_t thmin,Double_t thmax,TF1* thf,Double_t phimin,Double_t phimax,TF1* phif)
{
// Set parameters for position scrambling of the measurements in local coordinates.
// This scrambling facility is very useful for studying (accidental) background
// correlations and performing blind analyses.
//
// mode   : Mode for performing the position scrambling (see below)
// dmin   : Minimal value for scrambling angular difference (in degrees) or vector length 
// dmax   : Maximal value for scrambling angular difference (in degrees) or vector length
// df     : Randomisation function for angular difference (or vector length) scrambling
// thmin  : Minimal value (in degrees) for scrambling local theta coordinate
// thmax  : Maximal value (in degrees) for scrambling local theta coordinate
// thf    : Randomisation function for local theta coordinate scrambling
// phimin : Minimal value (in degrees) for scrambling local phi coordinate
// phimax : Maximal value (in degrees) for scrambling local phi coordinate
// phif   : Randomisation function for local phi coordinate scrambling
//
// Conventions for the mode parameter :
// ------------------------------------
// mode : 0 ==> No position scrambling is performed.
//        1 ==> Scrambling is only performed when obtaining angular differences.
//              All angular differences are randomly selected from the interval [dmin,dmax].
//              If for this mode dmin<0 it will be set to 0 and if dmax>180 it will be set to 180.
//              This scrambling mode is very useful for scrambling entries within a specific angular range
//              w.r.t. a source (see e.g. NvE, Astroparticle Physics 28 (2008) 540), without modification of
//              the event selection based on the time difference w.r.t. a transient source.
//              This angular difference scrambling will only affect the outcome of the memberfunctions
//              GetDifference(), MatchRefSignal(), MatchSignals() and MatchBurstData() of this class NcAstrolab.
//              So, the actual (stored) event positions are not affected by the scrambling.
//              This implies that setting mode=0 will reproduce again the unscrambled values.
//              Note : Every time an angular difference is requested, a new random value is produced.
//                     As such, scrambled values are not reproducible on subsequent invokations.
//        2 ==> Position scrambling is performed on the local spherical coordinates before storing the measurement.
//              A value is randomly selected from the intervals [dmin,dmax], [thmin,thmax] and [phimin,phimax]
//              and added to the actual local spherical coordinates (r,theta,phi) of the measurement.
//              As such, this scrambling is irreversible but will always provide reproducible results.
//              Notes :
//              This mode=2 scrambling will also result in a corresponding scrambling of e.g. the right ascension.
//              In case the r-coordinate would become zero or negative by the scrambling, its value will be
//              set to a small positive value before storage, in order to keep a physical situation.
//        3 ==> Position scrambling is performed by using a fake local position of the measurement.
//              To create the fake measurement position, a value is randomly selected from the intervals
//              [dmin,dmax], [thmin,thmax] and [phimin,phimax] and added to the actual local spherical coordinates
//              (r,theta,phi) of the measurement.
//              Since the timestamp of the measurement is not modified, this mode=3 scrambling will also result
//              in a corresponding scrambling of e.g. the right ascension.
//              In case the r-coordinate would become zero or negative by the scrambling, its value will be
//              set to a small positive value before storage, in order to keep a physical situation.
//              This position scrambling will only affect the outcome of the memberfunctions
//              GetDifference(), MatchRefSignal(), MatchSignals() and MatchBurstData() of this class NcAstrolab.
//              So, the actual (stored) event positions are not affected by the scrambling.
//              This implies that setting mode=0 will reproduce again the unscrambled values.
//              Note : Every time an angular difference is requested, a new random value is produced.
//                     As such, scrambled values are not reproducible on subsequent invokations.
//       <0 ==> Similar as the corresponding mode>0, but the scrambling is only performed to obtain
//              the off-source background data in the memberfunction MatchBurstData().
//              So, the actual on-source recordings are not affected.  
//              Furthermore, for mode=-2 the scrambled positions are only used after retrieving the measurement
//              data from the storage, so the actual stored measurement data are not modified.   
//              This implies that for position scrambling mode=-2 is equivalent to mode=-3.
//
// Notes :
// -------
// In case df=0 the corresponding scrambling will be performed via a homogeneous solid angle (mode=1) or uniform (mode=2) distribution.
// In case thf=0 the corresponding mode=2 scrambling will be performed via a uniform distribution in cos(theta).
// In case phif=0 the corresponding mode=2 scrambling will be performed via a uniform distribution in phi.
//
// If the provided "max" value is smaller than the provided "min" value, both values are set equal
// to the "min" value. This is the preferred way to study specific angular c.q. distance offsets without
// running into computer accuracy problems. 
//
// The defaults are df=0, thmin=0, thmax=0, thf=0, phimin=0, phimax=0 and phif=0.
//
// In the NcAstrolab constructor the position scrambling is switched off
// by setting mode=0 explicitly.

 // Keep parameters within physical bounds for angular difference scrambling (mode=1)
 if (abs(mode)==1 && dmin<0) dmin=0;
 if (abs(mode)==1 && dmax>180) dmax=180;

 // Check for specific requested offsets
 if (dmax<dmin) dmax=dmin;
 if (thmax<thmin) thmax=thmin;
 if (phimax<phimin) phimax=phimin;

 fRscmode=mode;
 fDscmin=dmin;
 fDscmax=dmax;
 if (fDscfunc)
 {
  delete fDscfunc;
  fDscfunc=0;
 }
 if (df)
 {
  fDscfunc=new TF1(*df);
  if (dmax>dmin) fDscfunc->SetRange(dmin,dmax);
 }
 fThetascmin=thmin;
 fThetascmax=thmax;
 if (fThetascfunc)
 {
  delete fThetascfunc;
  fThetascfunc=0;
 }
 if (thf)
 {
  fThetascfunc=new TF1(*thf);
  if (thmax>thmin) fThetascfunc->SetRange(thmin,thmax);
 }
 fPhiscmin=phimin;
 fPhiscmax=phimax;
 if (fPhiscfunc)
 {
  delete fPhiscfunc;
  fPhiscfunc=0;
 }
 if (phif)
 {
  fPhiscfunc=new TF1(*phif);
  if (phimax>phimin) fPhiscfunc->SetRange(phimin,phimax);
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::GetPositionScramble(Double_t* dmin,Double_t* dmax,TF1* df,Double_t* thmin,Double_t* thmax,TF1* thf,Double_t* phimin,Double_t* phimax,TF1* phif)
{
// Provide parameters which are used for position scrambling of the measurements.
//
// The standard returned value is the "position scrambling mode". 
// The memberfunction arguments provide the possibility of retrieving
// various optional values. The corresponding parameters are :
//
// dmin   : Minimal value for scrambling angular difference (in degrees) or vector length 
// dmax   : Maximal value for scrambling angular difference (in degrees) or vector length
// df     : Randomisation function for angular difference (or vector length) scrambling
// thmin  : Minimal value (in degrees) for scrambling local theta coordinate
// thmax  : Maximal value (in degrees) for scrambling local theta coordinate
// thf    : Randomisation function for local theta coordinate scrambling
// phimin : Minimal value (in degrees) for scrambling local phi coordinate
// phimax : Maximal value (in degrees) for scrambling local phi coordinate
// phif   : Randomisation function for local phi coordinate scrambling
//
// For a detailed description of the various time scrambling parameters
// please refer to the docs of the memberfunction SetPositionScramble.
//
// Note : In case an argument is entered as zero, the corresponding parameter
//        value will not be provided.
//
// Default argument values are dmin=0, dmax=0, df=0, thmin=0, thmax=0, thf=0, phimin=0, phimax=0 and phif=0.  
//
// Invokation example :
// --------------------
// NcAstrolab lab;
// Int_t mode;
// Double_t dlow,dup;
// TF1 func;
// mode=lab.GetPositionScramble(&dlow,&dup,&func);

 if (dmin) *dmin=fDscmin; 
 if (dmax) *dmax=fDscmax;
 if (df) *df=*fDscfunc;
 if (thmin) *thmin=fThetascmin; 
 if (thmax) *thmax=fThetascmax;
 if (thf) *thf=*fThetascfunc;
 if (phimin) *phimin=fPhiscmin; 
 if (phimax) *phimax=fPhiscmax;
 if (phif) *phif=*fPhiscfunc;

 return fRscmode; 
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::DisplaySignal(TString frame,TString mode,NcTimestamp* ts,Int_t j,TString proj,Int_t clr,TString name)
{
// Display a stored signal in a user specified coordinate projection
// at the specific timestamp ts.
//
// The input parameter "j" allows display of measurements or so-called "reference" signals.
// These reference signals may be used to check space-time event coincidences with 
// stored measurement(s) (e.g. coincidence of the measurement(s) with transient phenomena).
//
// j >0 --> Display of the reference signal at the j-th position (j=1 is first)
//   <0 --> Display of the measurement at the j-th position
//   =0 --> Display of the first measurement (only kept for backward compatibility)
//
// The user is advised not to use the obsolete "j=0" functionality anymore.
//
// Default value is j=-1 for backward compatibility.
//
// Measurements are indicated as blue dots.
// Reference signals are indicated as red stars.
// The Galactic Center is indicated as a black cross.
// The attributes of the marker symbols may be tailored via the member functions
// SetMarkerSize(), SetMarkerStyle() and SetMarkerColor().
//
// In case a non-empty string for the input parameter "name" is provided, this name will
// appear in the title text of the display. In this way the user can indicate the name
// of the object that is displayed.
//
// In case no stored signal was available or one of the input arguments was
// invalid, no display is produced.
//
// Note : In case ts=0 the actual recorded timestamp of the signal will be taken.
//        If such a recorded timestamp is absent, the current timestamp of the lab is used.
//
// The input parameter "frame" allows the user to specify the frame to which
// the coordinates refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (d).
//
//          "gal" ==> Galactic coordinates with longitude (l) and latitude (b).
//
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (l) and latitude (b).
//
//          "hor" ==> Horizontal azimuth and altitude coordinates at the NcAstrolab location.
//
//          "icr" ==> ICRS coordinates with longitude (l) and latitude (b).
//
//          "loc" ==> Local spherical angles theta and phi at the NcAstrolab location.
//
// In case the coordinates are the equatorial right ascension and declination (a,d),
// they can represent so-called "mean", "true" or reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,d) are only corrected for precession and not for nutation
// true values  : (a,d) are corrected for both precession and nutation
// epoch values : (a,d) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" specifies either "mean", "true" or "epoch" values for the equatorial (a,d) coordinates.
//
// mode = "M" --> Provided coordinates are the mean values 
//        "T" --> Provided coordinates are the true values 
//        "B" --> Provides Besselian (B1950) reference epoch coordinates 
//        "J" --> Provides Julian (J2000) reference epoch coordinates 
//
// The input parameter "proj" allows the user to specify the desired projection.
// The available projection modes are :
//
// cyl  : Cylindrical equirectangular (straight b vs. l) projection plotted with colored markers
// cylh : Cylindrical equirectangular (straight b vs. l) projection plotted in a 2-D histogram
// ham  : Hammer equal area projection plotted with colored markers
// hamh : Hammer equal area projection plotted in a 2-D histogram
// ait  : Aitoff projection plotted with colored markers
// aith : Aitoff projection plotted in a 2-D histogram
// mer  : Mercator projection plotted with colored markers
// merh : Mercator projection plotted in a 2-D histogram
// ang  : Straight sin(b) vs. l plot with colored markers
// angh : Straight sin(b) vs. l plot in a 2-D histogram
// UTh  : Day view (0-24 hours) of b vs. Universal Time
// LTh  : Day view (0-24 hours) of b vs. Local Time
// GSTh : Day view (0-24 hours) of b vs. Greenwich Siderial Time
// LSTh : Day view (0-24 hours) of b vs. Local Siderial Time
// UYh  : Year view of b vs. day of the year at the Universal Time of the specified timestamp
// LYh  : Year view of b vs. day of the year at the Local Time of the specified timestamp
// GSYh : Year view of b vs. day of the year at the Greenwich Siderial Time of the specified timestamp
// LSYh : Year view of b vs. day of the year at the Local Siderial Time of the specified timestamp
//
// Notes :
// -------
// 1) The ang(h) plot allows for easy identification of an isotropic distribution.
// 2) For the projections "GSTh", "LSTh", "GSYh" and "LSYh", the input argument "mode"
//    also determines whether they show the Mean (mode="M") or Apparent (mode="T") Sidereal Time. 
//
// The input argument "clr" allows to clear (1) the display before drawing or not (0).
//
// The default values are : j=0, proj="ham", clr=0 and name="".
//
// This routine is based on initial work by Garmt de Vries-Uiterweerd.

 // Comply with the new (jref,type) convention for measurements and reference signals.
 Int_t jref=abs(j);
 Int_t type=0;
 if (j<=0) type=1;
 if (!j) jref=1;

 NcSignal* sx=0;
 
 if (!ts)
 {
  sx=GetSignal(jref,type);
  if (!sx) return;
  ts=sx->GetTimestamp();
 }

 Nc3Vector r;
 sx=GetSignal(r,frame,mode,ts,jref,type);

 if (!sx) return;

 // Save name and timestamp to enable timestamp restoration for Solar system objects
 // after a Day View or Year View display 
 TString namesave=sx->GetName();
 NcTimestamp tsave=(*ts);

 // The generic input angles (in rad) for the projections 
 Double_t theta=0;
 Double_t phi=0;

 Double_t pi=acos(-1.);

 if (frame=="equ" || frame=="gal" || frame=="icr" || frame=="ecl" || frame=="loc")
 {
  theta=(pi/2.)-r.GetX(2,"sph","rad");
  phi=r.GetX(3,"sph","rad");
 }

 if (frame=="hor")
 {
  theta=(pi/2.)-r.GetX(2,"sph","rad");
  phi=pi-r.GetX(3,"sph","rad");
 }

 // Automatic choice of central meridian if not selected by the user
 if (!fUsMeridian || abs(fUsMeridian)>1)
 {
  if (frame=="equ")
  {
   fMeridian=pi;
   fUsMeridian=-2;
  }
  if (frame=="gal" || frame=="icr" || frame=="ecl")
  {
   fMeridian=0;
   fUsMeridian=-2;
  }
  if (frame=="hor" || frame=="loc")
  {
   fMeridian=0;
   fUsMeridian=2;
  }
 }

 // Obtain the projected (x,y) position
 Double_t x=0;
 Double_t y=0;
 Project(phi,theta,proj,x,y);

 // X-axis inversion of the display
 if (fUsMeridian<0) x*=-1.;

 Int_t hist=0;
 if (proj=="hamh" || proj=="aith" || proj=="merh" || proj=="cylh" || proj=="angh") hist=1;
 if (proj=="UTh" || proj=="LTh" || proj=="GSTh" || proj=="LSTh") hist=1;
 if (proj=="UYh" || proj=="LYh" || proj=="GSYh" || proj=="LSYh") hist=1;

 // Update the display for this signal position

 // Create a new canvas if needed
 if (!fCanvas || !(gROOT->GetListOfCanvases()->FindObject("NcAstrolab"))) fCanvas=new TCanvas("NcAstrolab","Skymap");

 // Construct the various strings for this map
 TString titleup;  // The upper title string
 TString titlelow; // The lower title string
 TString sleft;    // The most left coordinate indicator
 TString sright;   // The most right coordinate indicator
 TString sup;      // The most upper coordinate indicator
 TString slow;     // The most lower coordinate indicator
 sup="90#circ";
 slow="-90#circ";
 if (name!="")
 {
  titleup=name;
  titleup+=" ";
 }
 if (frame=="equ")
 {
  titleup+="Geocentric Equatorial (";
  titleup+=mode;
  if (mode=="J") titleup+="2000";
  if (mode=="B") titleup+="1950";
  titleup+=") ";
 }
 if (frame=="gal") titleup+="Heliocentric Galactic";
 if (frame=="ecl") titleup+=" Geocentric Ecliptic";
 if (frame=="hor") titleup+="  Standard Horizon";
 if (frame=="icr") titleup+="Static Barycentric ICRS";
 if (frame=="loc")
 {
  titleup+=" User defined Local";
  sup=" 0#circ";
  slow="180#circ";
 }
 titleup+=" Coordinates";
 titlelow="Projection : ";
 if (proj=="ham" || proj=="hamh") titlelow+="Hammer";
 if (proj=="cyl" || proj=="cylh") titlelow+="Cylindrical";
 if (proj=="ait" || proj=="aith") titlelow+="Aitoff";
 if (proj=="mer" || proj=="merh") titlelow+="Mercator";
 if (proj=="ang" || proj=="angh")
 {
  titlelow+="sin(b) vs. l";
  sup=" 1";
  slow=" -1";
 }
 titlelow+="   Central Meridian : ";
 Int_t ang,h,m,s,d;
 Int_t angmax,hmin,hmax,dmin,dmax;
 TString corr;
 TString scenter="";
 if (frame=="equ")
 {
  ang=int(ConvertAngle(fMeridian,"rad","hms"));
  angmax=ang+120000;
  h=ang/10000;
  ang=ang%10000;
  m=ang/100;
  s=ang%100;
  titlelow+=h;
  titlelow+="h ";
  titlelow+=m;
  titlelow+="m ";
  titlelow+=s;
  titlelow+="s";
  hmax=angmax/10000;
  corr="";
  while (hmax>24)
  {
   hmax-=24;
   corr="+";
  }
  hmin=hmax-24;
  while (hmin<-12)
  {
   hmin+=24;
   corr="+";
  }
  sright+=corr;
  if (fUsMeridian<0)
  {
   sright+=hmin;
  }
  else
  {
   sright+=hmax;
  }
  sright+="h";
  if (fUsMeridian<0)
  {
   sleft+=hmax;
  }
  else
  {
   sleft+=hmin;
  }
  sleft+="h";
  scenter+=h;
  scenter+="h";
 }
 else
 {
  ang=int(ConvertAngle(fMeridian,"rad","dms"));
  angmax=ang+1800000;
  d=ang/10000;
  ang=ang%10000;
  m=ang/100;
  s=ang%100;
  titlelow+=d;
  titlelow+="d ";
  titlelow+=m;
  titlelow+="' ";
  titlelow+=s;
  titlelow+="\"";
  dmax=angmax/10000;
  corr="";
  while (dmax>360)
  {
   dmax-=360;
   corr="+";
  }
  dmin=dmax-360;
  while (dmin<-180)
  {
   dmin+=360;
   corr="+";
  }
  sright+=corr;
  if (fUsMeridian<0)
  {
   sright+=dmin;
  }
  else
  {
   sright+=dmax;
  }
  sright+="#circ";
  if (fUsMeridian<0)
  {
   sleft+=dmax;
  }
  else
  {
   sleft+=dmin;
  }
  sleft+="#circ";
  scenter+=d;
  scenter+="#circ";
 }

 if (!hist) // 2-D Marker display (i.e. not a histogram) 
 {
  TMarker* marker=0;
  // Remove existing markers, grid and outline from display if needed
  if (clr==1 || proj!=fProj)
  {
   if (fMarkers)
   {
    delete fMarkers;
    fMarkers=0;
   }
   fCanvas->Clear();
   fProj=proj;
  }

  // Create a new display if needed
  if (!fMarkers)
  {
   fMarkers=new TObjArray();
   fMarkers->SetOwner();

   // Set canvas range, header and axes
   Float_t xup=2; // Maximal x coordinate of the projection
   Float_t yup=1; // maximal y coordinate of the projection
   Float_t xlow=-xup;
   Float_t ylow=-yup;
   Float_t xmargin=0.5; // X margin for canvas size
   Float_t ymargin=0.3; // Y margin for canvas size
   fCanvas->Range(xlow-xmargin,ylow-ymargin,xup+xmargin,yup+ymargin);

   // The ellipse outline with the skymap c.q. projection grid
   if (proj=="ham" || proj=="ait")
   {
    // Draw ellips outline
    TEllipse* outline=new TEllipse(0,0,xup,yup);
    fMarkers->Add(outline);
    outline->Draw();
   } 

   //////////////////////////////////////////
   // Draw the skymap c.q. projection grid //
   //////////////////////////////////////////

   // Drawing of the projected meridians every 30 degrees
   const Int_t nphi=13;
   Double_t gphiarr[nphi]={0,30,60,90,120,150,180,210,240,270,300,330,360};
   Double_t gphi=0;
   Double_t gtheta=0;
   Int_t ndots=100;
   Float_t gstep=180./float(ndots);
   Double_t xgrid=0;
   Double_t ygrid=0;
   for (Int_t iph=0; iph<nphi; iph++)
   {
    gphi=gphiarr[iph]*pi/180.;
    if (frame=="hor") gphi=pi-gphi;
    gtheta=pi/2.;
    for (Int_t ith=1; ith<ndots; ith++)
    {
     gtheta=gtheta-(gstep*pi/180.);
     Project(gphi,gtheta,proj,xgrid,ygrid);
     marker=new TMarker(xgrid,ygrid,fMarkerStyle[3]);
     marker->SetMarkerSize(fMarkerSize[3]);
     marker->SetMarkerColor(fMarkerColor[3]);
     fMarkers->Add(marker);
     marker->Draw();
    }
   } 

   // Drawing of the projected latitude circles every 15 degrees
   const Int_t nth=10;
   Double_t gtharr[nth]={15,30,45,60,75,105,120,135,150,165};
   gphi=0;
   gtheta=0;
   gstep=360./float(ndots);
   TString gs;
   Int_t igs=0;
   TLatex* lgs=0;
   Double_t xtext=0;
   Double_t ytext=0;
   for (Int_t ith=0; ith<nth; ith++)
   {
    gtheta=pi/2.-(gtharr[ith]*pi/180.);
    igs=int(90.-gtharr[ith]);
    if (frame=="loc") igs=int(gtharr[ith]);
    gs="";
    gs+=igs;
    gs+="#circ";
    xtext=0;
    for (Int_t iphi=1; iphi<ndots; iphi++)
    {
     gphi=gphi+gstep;
     Project(gphi,gtheta,proj,xgrid,ygrid);
     marker=new TMarker(xgrid,ygrid,fMarkerStyle[3]);
     marker->SetMarkerSize(fMarkerSize[3]);
     marker->SetMarkerColor(fMarkerColor[3]);
     if (xgrid<xtext)
     {
      xtext=xgrid;
      ytext=ygrid;
     }
     fMarkers->Add(marker);
     marker->Draw();
    }
    lgs=new TLatex;
    fMarkers->Add(lgs);
    if (ytext>0)
    {
     if (proj=="ham" || proj=="ait")
     {
      lgs->DrawLatex(xtext-0.25,ytext,gs.Data());
     }
     else
     {
      lgs->DrawLatex(xtext-0.4,ytext-0.02,gs.Data());
     }
    }
    else
    {
     if (proj=="ham" || proj=="ait")
     {
      lgs->DrawLatex(xtext-0.3,ytext-0.1,gs.Data());
     }
     else
     {
      lgs->DrawLatex(xtext-0.4,ytext-0.02,gs.Data());
     }
    }
   } 

   // The horizontal and vertical axes
   TLine* line=new TLine(xlow,0,xup,0);
   fMarkers->Add(line);
   line->Draw();
   line=new TLine(0,yup,0,ylow);
   fMarkers->Add(line);
   line->Draw();

   // The header and footer text
   TLatex* header=new TLatex;
   fMarkers->Add(header);
   header->SetTextAlign(21); // Text will be horizontally centered
   header->DrawLatex(0,yup+0.2,titleup.Data());
   TLatex* footer=new TLatex;
   fMarkers->Add(footer);
   footer->SetTextAlign(21); // Text will be horizontally centered
   footer->DrawLatex(0,ylow-0.25,titlelow.Data());

   // The left side angular value indicator
   TLatex* left=new TLatex;
   fMarkers->Add(left);
   if (proj=="ham" || proj=="ait")
   {
    left->DrawLatex(xlow-0.4,0,sleft.Data());
   }
   else
   {
    left->DrawLatex(xlow-0.15,yup+0.05,sleft.Data());
   }
   // The right side angular value indicator
   TLatex* right=new TLatex;
   fMarkers->Add(right);
   if (proj=="ham" || proj=="ait")
   {
    right->DrawLatex(xup+0.1,0,sright.Data());
   }
   else
   {
    right->DrawLatex(xup-0.1,yup+0.05,sright.Data());
   }
   // The upper angular value indicator
   TLatex* up=new TLatex;
   fMarkers->Add(up);
   if (proj=="ham" || proj=="ait")
   {
    up->DrawLatex(-0.1,yup+0.05,sup.Data());
   }
   else
   {
    up->DrawLatex(-0.1,yup+0.05,scenter.Data());
    if (proj!="ang")
    {
     up=new TLatex;
     fMarkers->Add(up);
     up->DrawLatex(xlow-0.4,yup-0.04,sup.Data());
    }
   }
   // The lower angular value indicator
   TLatex* low=new TLatex;
   fMarkers->Add(low);
   if (proj=="ham" || proj=="ait")
   {
    low->DrawLatex(-0.15,ylow-0.15,slow.Data());
   }
   else
   {
    if (proj!="ang") low->DrawLatex(xlow-0.4,ylow,slow.Data());
   }

   //////////////////////////////////
   // Indicate the Galactic Center //
   //////////////////////////////////

   // Add the Galactic Center temporarily as a reference signal for coordinate retrieval
   sx=SetSignal(1,0,"deg",0,"deg","gal",0,-1,"J","GC",0);
   Int_t idx=fRefs->IndexOf(sx);
   idx++;
   Nc3Vector rgc;
   sx=GetSignal(rgc,frame,mode,ts,idx,0);
   if (sx)
   {
    Double_t thetagc=0;
    Double_t phigc=0;
    if (frame=="equ" || frame=="gal" || frame=="icr" || frame=="ecl" || frame=="loc")
    {
     thetagc=(pi/2.)-rgc.GetX(2,"sph","rad");
     phigc=rgc.GetX(3,"sph","rad");
    }
    if (frame=="hor")
    {
     thetagc=(pi/2.)-rgc.GetX(2,"sph","rad");
     phigc=pi-rgc.GetX(3,"sph","rad");
    }
    // Obtain the projected (x,y) position
    Double_t xgc=0;
    Double_t ygc=0;
    Project(phigc,thetagc,proj,xgc,ygc);
    if (fUsMeridian<0) xgc*=-1.;
    marker=new TMarker(xgc,ygc,fMarkerStyle[2]);
    marker->SetMarkerSize(fMarkerSize[2]);
    marker->SetMarkerColor(fMarkerColor[2]);
    fMarkers->Add(marker);
    marker->Draw();
    // Remove the temporary Galactic Center object again
    RemoveSignal(idx,0,0);
   }
  }

  // Indicate the measurement(s) or reference signal(s) on the display
  marker=new TMarker(x,y,fMarkerStyle[type]);
  marker->SetMarkerSize(fMarkerSize[type]);
  marker->SetMarkerColor(fMarkerColor[type]);
  fMarkers->Add(marker);
  marker->Draw();
 }
 else if (hist==1) // 2-D display via histogram
 {
  Float_t xfac=90;
  Float_t yfac=90;
  if (frame=="equ") xfac=6;
  if (proj=="angh") yfac=1;
  // Reset the histogram if needed
  if (clr==1 || proj!=fProj || !fHist[type])
  {
   if (clr==1 || proj!=fProj)
   {
    fCanvas->Clear();
    fCanvas->SetGrid();
    for (Int_t i=0; i<2; i++)
    {
     if (fHist[i])
     {
      fHist[i]->Delete();
      fHist[i]=0;
     }
    }
   }
   if (!fHist[type]) fHist[type]=new TH2F();
   fHist[type]->Reset();
   fHist[type]->SetMarkerStyle(fMarkerStyle[type]);
   fHist[type]->SetMarkerSize(fMarkerSize[type]);
   fHist[type]->SetMarkerColor(fMarkerColor[type]);
   TString title=titleup;
   title+="   ";
   title+=titlelow;
   fHist[type]->SetNameTitle("SkyMap",title.Data());
   fHist[type]->GetXaxis()->SetTitle("Degrees from central Meridian");
   if (proj=="angh")
   {
    fHist[type]->SetBins(1000,-181,181,100,-1.1,1.1);
    fHist[type]->GetYaxis()->SetTitle("sin(b)");
   }
   else
   {
    fHist[type]->SetBins(1000,-181,181,500,-91,91);
    fHist[type]->GetYaxis()->SetTitle("Projected Latitude in degrees");
   }
   if (frame=="equ")
   {
    fHist[type]->GetXaxis()->SetTitle("Hours from central Meridian");
    if (proj=="angh")
    {
     fHist[type]->SetBins(200,-12.1,12.1,100,-1.1,1.1);
     if (frame=="equ") fHist[type]->GetYaxis()->SetTitle("sin(#delta)");
    }
    else
    {
     fHist[type]->SetBins(200,-12.1,12.1,500,-91,91);
     if (frame=="equ") fHist[type]->GetYaxis()->SetTitle("Projected Declination in degrees");
    }
   }
   if (frame=="hor")
   {
    if (proj=="angh")
    {
     fHist[type]->GetYaxis()->SetTitle("sin(alt)=cos(zenith)");
    }
    else
    {
     fHist[type]->GetYaxis()->SetTitle("Projected Altitude in degrees");
    }
   }
   if (frame=="loc")
   {
    if (proj=="angh")
    {
     fHist[type]->GetYaxis()->SetTitle("cos(#theta)=sin(b)");
    }
    else
    {
     fHist[type]->GetYaxis()->SetTitle("Projected degrees from the equator");
    }
   }
   fProj=proj;
  }

  if (proj=="merh")
  {
   fHist[type]->Fill(x*xfac,theta*180./pi);
  }
  else if (proj=="hamh" || proj=="aith" || proj=="cylh" || proj=="angh")
  {
   fHist[type]->Fill(x*xfac,y*yfac);
  }
  else if (proj=="UTh" || proj=="LTh" || proj=="GSTh" || proj=="LSTh") // The 24 hour day view
  {
   // Set histogram binning and axes attributes
   fHist[type]->SetBins(100,0,24,181,-90.5,90.5);
   if (!ts) ts=(NcTimestamp*)this;
   NcTimestamp tx=(*ts);
   Double_t toffset=GetLabTimeOffset();
   TString title="Day view";
   if (name!="")
   {
    title+=" of ";
    title+=name;
   }
   title+=" at ";
   title+=GetName();
   title+=" on ";
   TString date;
   ts->GetDayTimeString("UT",0,0,&date);
   title+=date;
   TString tmode="UT";
   if (proj=="LTh") tmode="LMT";
   if (proj=="GSTh")
   {
    tmode="GMST";
    if (mode=="T") tmode="GAST";
   }
   if (proj=="LSTh")
   {
    tmode="LMST";
    if (mode=="T") tmode="LAST";
   }
   if (proj=="UTh") title+=";Universal Time";
   if (proj=="LTh") title+=";Local Time";
   if (proj=="GSTh") title+=";Greenwich Sidereal Time";
   if (proj=="LSTh") title+=";Local Sidereal Time";
   if (proj!="UTh")
   {
    title+=" (";
    title+=tmode;
    title+=")";
   }
   title+=" in hours;";
   TString ytitle="Declination ";
   if (frame=="equ" && mode=="J") ytitle+="(J2000)";
   if (frame=="equ" && mode=="B") ytitle+="(B1950)";
   if (frame=="equ" && mode=="M") ytitle+="(Mean)";
   if (frame=="equ" && mode=="T") ytitle+="(True)";
   if (frame=="gal") ytitle="Galactic latitude";
   if (frame=="ecl") ytitle="Geocentric Ecliptic latitude";
   if (frame=="hor") ytitle="Horizon altitude";
   if (frame=="icr") ytitle="ICRS latitude";
   if (frame=="loc") ytitle="Angle w.r.t. local frame equator";
   ytitle+=" in degrees";
   title+=ytitle;
   fHist[type]->SetTitle(title);
   fHist[type]->SetStats(kFALSE);

   // Fill the day view histogram
   Double_t hour=0;
   Double_t d,a,b;
   for (Int_t i=0; i<24; i++)
   {
    if (tmode=="UT") hour=tx.GetUT();
    if (tmode=="LMT") hour=tx.GetLT(toffset);
    if (tmode=="GMST") hour=tx.GetGMST();
    if (tmode=="GAST") hour=tx.GetGAST();
    if (tmode=="LMST") hour=tx.GetLMST(toffset);
    if (tmode=="LAST") hour=tx.GetLAST(toffset);

    // Get coordinates at this time step
    GetSignal(d,a,"deg",b,"deg",frame,&tx,jref,mode,type);
    
    if (frame=="hor") b=90.-b;
    if (frame=="loc") b=90.-a;

    fHist[type]->Fill(hour,b);

    tx.Add(1); // Add 1 hour for each time step
   }

   // Restore the original timestamp for Solar system objects
   SetSolarSystem(namesave,&tsave,type);
  }
  else if (proj=="UYh" || proj=="LYh" || proj=="GSYh" || proj=="LSYh") // The day of the year view
  {
   // Set histogram binning and axes attributes
   fHist[type]->SetBins(1500,0,370,181,-90.5,90.5);
   if (!ts) ts=(NcTimestamp*)this;
   NcTimestamp tx=(*ts);
   Double_t toffset=GetLabTimeOffset();
   TString year="";
   year+=int(ts->GetEpoch("J"));
   TString tmode="UT";
   if (proj=="LYh") tmode="LMT";
   if (proj=="GSYh")
   {
    tmode="GMST";
    if (mode=="T") tmode="GAST";
   }
   if (proj=="LSYh")
   {
    tmode="LMST";
    if (mode=="T") tmode="LAST";
   }
   TString time;
   if (proj=="UYh" || proj=="GSYh") ts->GetDayTimeString(tmode,0,0,0,&time);
   if (proj=="LYh" || proj=="LSYh") ts->GetDayTimeString(tmode,0,toffset,0,&time);

   TString title="Year view";
   if (name!="")
   {
    title+=" of ";
    title+=name;
   }
   title+=" at ";
   title+=GetName();
   title+=" in ";
   title+=year;
   title+=" at ";
   title+=time;
   title+=";Day of the year;";
   TString ytitle="Declination ";
   if (frame=="equ" && mode=="J") ytitle+="(J2000)";
   if (frame=="equ" && mode=="B") ytitle+="(B1950)";
   if (frame=="equ" && mode=="M") ytitle+="(Mean)";
   if (frame=="equ" && mode=="T") ytitle+="(True)";
   if (frame=="gal") ytitle="Galactic latitude";
   if (frame=="ecl") ytitle="Geocentric Ecliptic latitude";
   if (frame=="hor") ytitle="Horizon altitude";
   if (frame=="icr") ytitle="ICRS latitude";
   if (frame=="loc") ytitle="Angle w.r.t. local frame equator";
   ytitle+=" in degrees";
   title+=ytitle;
   fHist[type]->SetTitle(title);
   fHist[type]->SetStats(kFALSE);

   // Fill the year view histogram
   // Start at 01-jan 00:00:00h for the corresponding year at the selected location
   // and then set the time according to the specified timestamp
   Double_t hour=0;
   if (proj=="UYh" || proj=="GSYh") // Location is Greenwich
   {
    tx.SetUT(year.Atoi(),1,1,"00:00:00");
    hour=ts->GetUT();
   }
   if (proj=="LYh" || proj=="LSYh") // The lab location
   {
    tx.SetLT(toffset,year.Atoi(),1,1,"00:00:00");
    hour=ts->GetLT(toffset);
   }
   tx.Add(hour); // Set the selected time

   Double_t d,a,b;
   Int_t day=0;
   for (Int_t i=0; i<367; i++)
   {

    // Get coordinates at this time step
    GetSignal(d,a,"deg",b,"deg",frame,&tx,jref,mode,type);
    
    if (frame=="hor") b=90.-b;
    if (frame=="loc") b=90.-a;

    if (proj=="UYh" || proj=="GSYh") day=tx.GetDayOfYear();
    if (proj=="LYh" || proj=="LSYh") day=tx.GetDayOfYear(kFALSE,toffset);

    fHist[type]->Fill(day,b);

    tx.Add(24); // Add 1 day (= 24 hours) for each time step
   }

   // Restore the original timestamp for Solar system objects
   SetSolarSystem(namesave,&tsave,type);
  }

  // Draw the selected histogram
  if ((!type && fHist[1]) || (type && fHist[0]))
  {
   fHist[type]->Draw("same");
  }
  else
  {
   fHist[type]->Draw();

   // Draw a horizontal thick line to mark the horizon c.q. equator for the day and year views
   if (proj=="UTh" || proj=="LTh" || proj=="GSTh" || proj=="LSTh" ||
       proj=="UYh" || proj=="LYh" || proj=="GSYh" || proj=="LSYh")
   {
    if (!fMarkers)
    {
     fMarkers=new TObjArray();
     fMarkers->SetOwner();
    }
    TLine* line=0;
    if (proj=="UTh" || proj=="LTh" || proj=="GSTh" || proj=="LSTh") line=new TLine(0,0,24,0);
    if (proj=="UYh" || proj=="LYh" || proj=="GSYh" || proj=="LSYh") line=new TLine(0,0,370,0);
    if (line)
    {
     line->SetLineWidth(3);
     fMarkers->Add(line);
     line->Draw();
    }
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::DisplaySignal(TString frame,TString mode,NcTimestamp* ts,TString name,TString proj,Int_t clr,Int_t type)
{
// Display the stored signal according to "type" with the specified name in a user specified
// coordinate projection at the specific timestamp ts.
//
// The input parameter "type" allows to specify either measurements or reference signals.
//
// type = 0 --> Display the corresponding reference signal
//        1 --> Display the corresponding measurement
//
// Measurements are indicated as blue dots.
// Reference signals are indicated as red stars.
// The Galactic Center is indicated as a black cross.
// The attributes of the marker symbols may be tailored via the member functions
// SetMarkerSize(), SetMarkerStyle() and SetMarkerColor().
//
// Note : In case ts=0 the actual recorded timestamp of the signal will be taken.
//        If such a recorded timestamp is absent, the current timestamp of the lab is used.
//
// In case no such stored signal was available or one of the input arguments was
// invalid, no display is produced.
//
// The input parameter "frame" allows the user to specify the frame to which
// the coordinates refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (d).
//
//          "gal" ==> Galactic coordinates with longitude (l) and latitude (b).
//
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (l) and latitude (b).
//
//          "hor" ==> Horizontal azimuth and altitude coordinates at the NcAstrolab location.
//
//          "icr" ==> ICRS coordinates with longitude (l) and latitude (b).
//
//          "loc" ==> Local spherical angles theta and phi at the NcAstrolab location.
//
// In case the coordinates are the equatorial right ascension and declination (a,d),
// they can represent so-called "mean", "true" or reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,d) are only corrected for precession and not for nutation
// true values  : (a,d) are corrected for both precession and nutation
// epoch values : (a,d) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" specifies either "mean", "true" or "epoch" values for the equatorial (a,d) coordinates.
//
// mode = "M" --> Provided coordinates are the mean values 
//        "T" --> Provided coordinates are the true values 
//        "B" --> Provides Besselian (B1950) reference epoch coordinates 
//        "J" --> Provides Julian (J2000) reference epoch coordinates 
//
// The input parameter "proj" allows the user to specify the desired projection.
// The available projection modes are :
//
// cyl  : Cylindrical equirectangular (straight b vs. l) projection plotted with colored markers
// cylh : Cylindrical equirectangular (straight b vs. l) projection plotted in a 2-D histogram
// ham  : Hammer equal area projection plotted with colored markers
// hamh : Hammer equal area projection plotted in a 2-D histogram
// ait  : Aitoff projection plotted with colored markers
// aith : Aitoff projection plotted in a 2-D histogram
// mer  : Mercator projection plotted with colored markers
// merh : Mercator projection plotted in a 2-D histogram
// ang  : Straight sin(b) vs. l plot with colored markers
// angh : Straight sin(b) vs. l plot in a 2-D histogram
// UTh  : Day view (0-24 hours) of b vs. Universal Time
// LTh  : Day view (0-24 hours) of b vs. Local Time
// GSTh : Day view (0-24 hours) of b vs. Greenwich Siderial Time
// LSTh : Day view (0-24 hours) of b vs. Local Siderial Time
// UYh  : Year view of b vs. day of the year at the Universal Time of the specified timestamp
// LYh  : Year view of b vs. day of the year at the Local Time of the specified timestamp
// GSYh : Year view of b vs. day of the year at the Greenwich Siderial Time of the specified timestamp
// LSYh : Year view of b vs. day of the year at the Local Siderial Time of the specified timestamp
//
// Notes :
// -------
// 1) In case the name specifies a solar system object which was not yet stored according to "type",
//    the corresponding signal will be created and stored with the specified timestamp.
//    All geocentric name specifications for solar system objects as indicated in the
//    docs of NcTimestamp::Almanac() are supported.
// 2) The ang(h) plot allows for easy identification of an isotropic distribution.
// 3) For the projections "GSTh", "LSTh", "GSYh" and "LSYh", the input argument "mode"
//    also determines whether they show the Mean (mode="M") or Apparent (mode="T") Sidereal Time. 
//
// The input argument "clr" allows to clear (1) the display before drawing or not (0).
//
// The default values are : proj="ham", clr=0 and type=0.

 // Create a signal for a solar system object if needed
 Double_t d,a,b;
 GetSignal(d,a,"deg",b,"deg",frame,ts,name,mode,type);
  
 Int_t j=GetSignalIndex(name,type);
 if (j>0)
 {
  if (type) j=-j;
  DisplaySignal(frame,mode,ts,j,proj,clr,name);
 }
 
 // Update the canvas so that the Skymap GUI immediately shows the result
 if (fCanvas) fCanvas->Update();
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::DisplaySignals(TString frame,TString mode,NcTimestamp* ts,TString proj,Int_t clr,Int_t nmax,Int_t j,Int_t type,TString name)
{
// Display of stored signals in a user specified coordinate projection at the specific timestamp,
// if their name matches the provided name pattern "name", where name="*" means all.
//
// All measurements are displayed according to the timestamp of their actual stored recording,
// which may be a scrambled timestamp according to the invokation of the memberfunction SetTimeScramble().
//
// For reference signals the actual timestamp of the j-th (j=1 is first) measurement is used.
// In case j=0 the provided timestamp "ts" is used for all the reference signals.
// For j<0 the timestamp of the actual recording of each reference signal will be used.
//
// In case a timestamp is not present for a certain signal, the provided "ts" is used instead.
// Note : ts=0 corresponds to the current timestamp of the lab.
//
// Measurements are indicated as blue dots.
// Reference signals are indicated as red stars.
// The Galactic Center is indicated as a black cross.
// The attributes of the marker symbols may be tailored via the member functions
// SetMarkerSize(), SetMarkerStyle() and SetMarkerColor().
//
// In case no corresponding stored signal is available or one of the input arguments is
// invalid, no display is produced.
//
// type = 0 --> Display the data of stored reference signals
//        1 --> Display the data of stored measured signals
//      < 0 --> Display the data of both measured and reference signals  
//
// The argument "nmax" specifies the maximum number of measured c.q. reference signals
// that will be listed. When nmax<0 all stored measured c.q. reference signals are listed.
//
// The input parameter "frame" allows the user to specify the frame to which
// the coordinates refer. Available options are :
//
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (d).
//
//          "gal" ==> Galactic coordinates with longitude (l) and latitude (b).
//
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (l) and latitude (b).
//
//          "hor" ==> Horizontal azimuth and altitude coordinates at the NcAstrolab location.
//
//          "icr" ==> ICRS coordinates with longitude (l) and latitude (b).
//
//          "loc" ==> Local spherical angles theta and phi at the NcAstrolab location.
//
// In case the coordinates are the equatorial right ascension and declination (a,d),
// they can represent so-called "mean", "true" or reference "epoch" values.
// The distinction between these representations is the following :
//
// mean values  : (a,d) are only corrected for precession and not for nutation
// true values  : (a,d) are corrected for both precession and nutation
// epoch values : (a,d) are given w.r.t. specific (e.g. B1950 or J2000) reference equinox locations
//
// The input parameter "mode" specifies either "mean", "true" or "epoch" values for the equatorial (a,d) coordinates.
//
// mode = "M" --> Provided coordinates are the mean values 
//        "T" --> Provided coordinates are the true values 
//        "B" --> Provides Besselian (B1950) reference epoch coordinates 
//        "J" --> Provides Julian (J2000) reference epoch coordinates 
//
// The input parameter "proj" allows the user to specify the desired projection.
// The available projection modes are :
//
// cyl  : Cylindrical equirectangular (straight b vs. l) projection plotted with colored markers
// cylh : Cylindrical equirectangular (straight b vs. l) projection plotted in a 2-D histogram
// ham  : Hammer equal area projection plotted with colored markers
// hamh : Hammer equal area projection plotted in a 2-D histogram
// ait  : Aitoff projection plotted with colored markers
// aith : Aitoff projection plotted in a 2-D histogram
// mer  : Mercator projection plotted with colored markers
// merh : Mercator projection plotted in a 2-D histogram
// ang  : Straight sin(b) vs. l plot with colored markers
// angh : Straight sin(b) vs. l plot in a 2-D histogram
// UTh  : Day view (0-24 hours) of b vs. Universal Time
// LTh  : Day view (0-24 hours) of b vs. Local Time
// GSTh : Day view (0-24 hours) of b vs. Greenwich Siderial Time
// LSTh : Day view (0-24 hours) of b vs. Local Siderial Time
// UYh  : Year view of b vs. day of the year at the Universal Time of the specified timestamp
// LYh  : Year view of b vs. day of the year at the Local Time of the specified timestamp
// GSYh : Year view of b vs. day of the year at the Greenwich Siderial Time of the specified timestamp
// LSYh : Year view of b vs. day of the year at the Local Siderial Time of the specified timestamp
//
// Notes :
// -------
// 1) The ang(h) plot allows for easy identification of an isotropic distribution.
// 2) For the projections "GSTh", "LSTh", "GSYh" and "LSYh", the input argument "mode"
//    also determines whether they show the Mean (mode="M") or Apparent (mode="T") Sidereal Time. 
//
// The input argument "clr" allows to clear (1) the display before drawing or not (0).
//
// The default values are : proj="ham", clr=0, nmax=-1, j=-1, type=-1 and name="*".

 NcSignal* sx=0;
 TString namex="";
 NcTimestamp* tx=0;
 Int_t jdisp=0;

 // Display stored reference signals
 if (fRefs && type<=0)
 {
  // Use timestamp of j-th measurement if requested
  if (j>0)
  {
   sx=GetSignal(j,1);
   if (sx) tx=sx->GetTimestamp();
  }

  // Use the provided timestamp
  if (!j || !tx) tx=ts;

  // Use the current lab timestamp if no timestamp selected
  if (!tx) tx=(NcTimestamp*)this;
 
  jdisp=0;
  for (Int_t i=1; i<=fRefs->GetSize(); i++)
  {
   sx=GetSignal(i,0);
   if (!sx) continue;

   jdisp++;
   if (nmax>=0 && jdisp>nmax) break;

   // Check for the name pattern
   namex=sx->GetName();
   if (name!="*" && !namex.Contains(name)) continue;

   // Use the actual timestamp of the reference signal
   if (j<0)
   {
    tx=sx->GetTimestamp();
    if (!tx) tx=ts;
    if (!tx) tx=(NcTimestamp*)this;
   }

   if (name=="*")
   {
    DisplaySignal(frame,mode,tx,i,proj,clr);
   }
   else
   {
    DisplaySignal(frame,mode,tx,i,proj,clr,name);
   }
   clr=0; // No display clear for subsequent signals
  }
 }

 // Display all stored measurements
 if (fSigs && type)
 {
  jdisp=0;
  for (Int_t j=1; j<=fSigs->GetSize(); j++)
  {
   sx=GetSignal(j,1);
   if (!sx) continue;

   jdisp++;
   if (nmax>=0 && jdisp>nmax) break;

   // Check for the name pattern
   namex=sx->GetName();
   if (name!="*" && !namex.Contains(name)) continue;

   tx=sx->GetTimestamp();
   if (!tx) tx=ts;
   if (!tx) tx=(NcTimestamp*)this;
   if (name=="*")
   {
    DisplaySignal(frame,mode,tx,-j,proj,clr);
   }
   else
   {
    DisplaySignal(frame,mode,tx,-j,proj,clr,name);
   }
   clr=0; // No display clear for subsequent signals
  }
 }
 
 // Update the canvas so that the Skymap GUI immediately shows the result
 if (fCanvas) fCanvas->Update();
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetMarkerSize(Float_t size,Int_t type)
{
// Set the size of the marker symbols for skymaps and related histograms.
// Defaults are set in the constructor of this NcAstrolab class. 
//
// type = 0 --> Set marker size for reference signals    (default=1.5)
//        1 --> Set marker size for measurements         (default=1.0)
//        2 --> Set marker size for the Galactic Center  (default=1.5)
//        3 --> Set marker size for the skymap grid dots (default=0.3)

 if (type<0 || type >3) return;

 fMarkerSize[type]=size;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetMarkerStyle(Int_t style,Int_t type)
{
// Set the style of the marker symbols for skymaps and related histograms.
// Defaults are set in the constructor of this NcAstrolab class. 
//
// type = 0 --> Set marker style for reference signals    (default=29 filled star)
//        1 --> Set marker style for measurements         (default=8  scalable dot)
//        2 --> Set marker style for the Galactic Center  (default=34 filled cross)
//        3 --> Set marker style for the skymap grid dots (default=8  scalable dot)

 if (type<0 || type >3) return;

 fMarkerStyle[type]=style;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetMarkerColor(Int_t color,Int_t type)
{
// Set the color of the marker symbols for skymaps and related histograms.
// Defaults are set in the constructor of this NcAstrolab class. 
//
// type = 0 --> Set marker color for reference signals    (default=kRed)
//        1 --> Set marker color for measurements         (default=kBlue)
//        2 --> Set marker color for the Galactic Center  (default=kBlack)
//        3 --> Set marker color for the skymap grid dots (default=kBlack)

 if (type<0 || type >3) return;

 fMarkerColor[type]=color;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetCentralMeridian(Int_t mode,Double_t phi,TString u)
{
// Set the central meridian and the orientation for the sky display.
// By default the central meridian is set at -999 in the constructor.
//
// Input arguments :
// -----------------
// mode =  0 : Automatic meridian and display setting.
//             For this mode the actual values of "phi" and "u" are irrelevant.
//             This is also the default setting in the constructor of this class.
//         1 : The horizontal scale will increase from left to right (e.g. 0h->24h) 
//        -1 : The horizontal scale will decrease from left to right (e.g. 24h->0h) 
// phi  :  Specification of the central meridian
// u    =  "rad" : phi angle provided in radians
//         "deg" : phi angle provided in degrees
//         "dms" : phi angle provided in dddmmss.sss
//         "hms" : phi angle provided in hhmmss.sss
//
// The default values are mode=0, phi=0 and u="deg".
//
// This routine is based on initial work by Garmt de Vries-Uiterweerd.

 fMeridian=ConvertAngle(phi,u,"rad");
 fUsMeridian=0;
 if (mode>0) fUsMeridian=1;
 if (mode<0) fUsMeridian=-1;
 Double_t pi=acos(-1.);
 Double_t twopi=2.*pi;
 // Set range to 0 <= meridian < 2pi
 while (fMeridian>=twopi)
 {
  fMeridian-=twopi;
 }
 while (fMeridian<0)
 {
  fMeridian+=twopi;
 }
 // Prevent accuracy problems
 if (fMeridian>0) fMeridian+=1.e-6;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::Project(Double_t l,Double_t b,TString proj,Double_t& x,Double_t& y)
{
// Generic interface for projection of a (long,lat) pair onto a (x,y) pair.
//
// Meaning of the arguments :
// --------------------------
// l    : Longitude (e.g. right ascension)
// b    : Latitude (e.g. declination)
// proj : Projection mode (e.g. "ham")
// x    : The resulting x coordinate for the selected projection
// y    : The resulting x coordinate for the selected projection
//
// The available projection modes are :
//
// cyl  : Cylindrical equirectangular (straight b vs. l) projection plotted with colored markers
// cylh : Cylindrical equirectangular (straight b vs. l) projection plotted in a 2-D histogram
// ham  : Hammer equal area projection plotted with colored markers
// hamh : Hammer equal area projection plotted in a 2-D histogram
// ait  : Aitoff projection plotted with colored markers
// aith : Aitoff projection plotted in a 2-D histogram
// mer  : Mercator projection plotted with colored markers
// merh : Mercator projection plotted in a 2-D histogram
// ang  : Straight sin(b) vs. l plot with colored markers
// angh : Straight sin(b) vs. l plot in a 2-D histogram
//
// Note : The ang(h) plot allows for easy identification of an isotropic distribution.
//
// This routine is based on initial work by Garmt de Vries-Uiterweerd.

 Double_t pi=acos(-1.);

 // Subtract central meridian from longitude
 l-=fMeridian;

 // Take l between -180 and 180 degrees
 while (l>pi)
 {
  l-=2.*pi;
 }
 while (l<-pi)
 {
  l+=2.*pi;
 }

 x=0;
 y=0;

 // Convert (l,b) to (x,y) with -2 < x <= 2 
 if (proj=="cyl" || proj=="cylh") ProjectCylindrical(l,b,x,y);
 if (proj=="ham" || proj=="hamh") ProjectHammer(l,b,x,y);
 if (proj=="ait" || proj=="aith") ProjectAitoff(l,b,x,y);
 if (proj=="mer" || proj=="merh") ProjectMercator(l,b,x,y);
 if (proj=="ang" || proj=="angh")
 {
  x=2.*l/pi;
  y=sin(b);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ProjectCylindrical(Double_t l,Double_t b,Double_t& x, Double_t& y)
{
// Equirectangular cylindrical projection of a (long,lat) coordinate pair 
// We adopt here a scale factor such that the range for x becomes [-2,2].
//
// This routine is based on initial work by Garmt de Vries-Uiterweerd.

 Double_t pi=acos(-1.);
 x=2.*l/pi;
 y=2.*b/pi;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ProjectHammer(Double_t l,Double_t b,Double_t& x,Double_t& y)
{
// Hammer-Aitoff projection of a (long,lat) coordinate pair. 
// This is an equal-area projection.
// We adopt here a scale factor such that the range for x becomes [-2,2].
//
// This routine is based on initial work by Garmt de Vries-Uiterweerd.

 Double_t k=1./sqrt(1.+cos(b)*cos(l/2.));
 x=2.*k*cos(b)*sin(l/2.);
 y=k*sin(b);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ProjectAitoff(Double_t l,Double_t b,Double_t& x,Double_t& y)
{
// Aitoff projection of a (long,lat) coordinate pair. 
// We adopt here a scale factor such that the range for x becomes [-2,2].
//
// This routine is based on initial work by Garmt de Vries-Uiterweerd.

 Double_t pi=acos(-1.);
 x=0;
 y=0;
 Double_t k=acos(cos(b)*cos(l/2.));
 if(sin(k)!=0)
 {
  x=4.*k*cos(b)*sin(l/2.)/(pi*sin(k));
  y=2.*k*sin(b)/(pi*sin(k));
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ProjectMercator(Double_t l,Double_t b,Double_t& x,Double_t& y)
{
// Mercator projection of a (long,lat) coordinate pair.
// At the poles this projection yields an infinite value for the y-coordinate.
// As such a latitude cut off has to be introduced.
//
// Commonly used cut off values (in degrees) are :
// -85.051 <= b <= 85.051 --> -pi <= y <= pi
// -89 <= b <= 89 --> 4.74 <= y <= 4.74
//
// We adopt here the 85.051 degree cutoff and a scale factor such that
// the range for y becomes [-1,1] and for x [-2,2].

 Double_t pi=acos(-1.);
 Double_t bcut=85.051*pi/180.; // Latitude cut off value in radians

 x=2.*l/pi;
 y=0;
 if (b > bcut) b=bcut;
 if (b < -bcut) b=-bcut;
 y=0.5*log((1.+sin(b))/(1.-sin(b)))/pi;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetPhysicalParameter(TString name,Double_t value)
{
// Facility to modify the value of a physical parameter.
// In the constructor of this class the various parameters have been
// set to the values as provided by the Particle Data Group in 2014.
//
// The parameter is specified via the input argument "name".
// Currently the following parameters are supported :
//
// SpeedC  = The speed of light in vacuum in m/s
// Qe      = The electron charge in C
// Me      = The electron mass in MeV/c^2
// Mmu     = The muon mass in MeV/c^2
// Mtau    = The tau mass in MeV/c^2
// Amu     = The unified atomic mass unit in MeV/c^2
// Mp      = The proton mass in MeV/c^2
// Mn      = The neutron mass in MeV/c^2 
// AlphaEM = The electromagnetic fine-structure constant
// Fermi   = The Fermi coupling constant GF/(hbar*c)^3 in GeV^-2
// MW      = The W mass in GeV/c^2
// GammaW  = Full width of W in GeV
// MZ      = The Z mass in GeV/c^2 
// GammaZ  = Full width of Z in GeV
// Planck  = The value of the Planck constant (h) in J s
// Boltz   = The value of the Boltzmann constant (k) in J K^-1
// Newton  = The Newton gravitational constant in m^3 kg^-1 s^-1
// Gn      = The nominal value of the gravitational acceleration at the Earth surface in m s^-2
// Au      = The astronomical unit in m
// Pc      = The parsec in m
// Hubble  = The Hubble parameter in km s^-1 Mpc^-1
// OmegaM  = The fractional mass energy density
// OmegaR  = The fractional radiation energy density
// OmegaL  = The fractional vacuum energy density
// OmegaB  = The fractional baryon energy density
// OmegaC  = The fractional cold dark matter energy density

 // Variable to correct conversion factors when a parameter is modified
 Double_t frac=1;

 if (name=="SpeedC")
 {
  frac=value/fSpeedC;
  fSpeedC=value;
  fMe*=frac*frac;
  fMmu*=frac*frac;
  fMtau*=frac*frac;
  fAmu*=frac*frac;
  fMp*=frac*frac;
  fMn*=frac*frac;
  fMW*=frac*frac;
  fMZ*=frac*frac;
  fHbarc*=frac;
  fHbarc2*=pow(frac,2.);
 }
 if (name=="Qe")
 {
  frac=value/fQe;
  fQe=value;
  fMe/=frac;
  fMmu/=frac;
  fMtau/=frac;
  fAmu/=frac;
  fMp/=frac;
  fMn/=frac;
  fMW/=frac;
  fMZ/=frac;
  fHbar/=frac;
  fHbarc/=frac;
  fHbarc2/=pow(frac,2.);
 }
 if (name=="Me") fMe=value;
 if (name=="Mmu") fMmu=value;
 if (name=="Mtau") fMtau=value;
 if (name=="Amu")
 {
  frac=value/fAmu;
  fAmu=value;
  fMp*=frac;
  fMn*=frac;
 }
 if (name=="Mp") fMp=value;
 if (name=="Mn") fMn=value;
 if (name=="MW") fMW=value;
 if (name=="GammaW") fGammaW=value;
 if (name=="MZ") fMZ=value;
 if (name=="GammaZ") fGammaZ=value;
 if (name=="AlphaEM") fAlphaEM=value;
 if (name=="Fermi") fFermi=value;
 if (name=="Planck")
 {
  frac=value/fPlanck;
  fPlanck=value;
  fHbar*=frac;
  fHbarc*=frac;
  fFermi/=pow(frac,3.);
 }
 if (name=="Boltz") fBoltz=value;
 if (name=="Newton")
 {
  frac=value/fNewton;
  fNewton=value;
  fGn*=frac;
 }
 if (name=="Gn") fGn=value;
 if (name=="Au") fAu=value;
 if (name=="Pc") fPc=value;
 if (name=="Hubble") fHubble=value;
 if (name=="OmegaM") fOmegaM=value;
 if (name=="OmegaR") fOmegaR=value;
 if (name=="OmegaL") fOmegaL=value;
 if (name=="OmegaB") fOmegaB=value;
 if (name=="OmegaC") fOmegaC=value;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetPhysicalParameter(TString name) const
{
// Provide the (modified) value of a (astro)physical parameter.
//
// The parameter is specified via the input argument "name".
// Please refer to the docs of memberfunction SetPhysicalParameter()
// to see a list of currently supported parameters.
//
// In addition to the standard parameters, also the values of some
// useful derived parameters c.q. conversion constants may be obtained.
// These additional values are :
//
// Hbar   = The value of hbar in MeV s
// Hbarc  = The value of the conversion constant hbar*c in MeV fm 
// Hbarc2 = The value of the conversion constant (hbar*c)^2 in GeV^2 barn
// Mnucl  = The nucleon mass (=average of proton and neutron mass) in MeV/c^2
// Sin2w  = The sin^2 of the Weinberg angle
// Jy     = Flux density of 1 Jansky (1e-23 erg s^-1 cm^-2 Hz^-1)
// Erg    = The equivalent of 1 erg in GeV

 Double_t val=0;

 // Standard parameters
 if (name=="SpeedC") return fSpeedC;
 if (name=="Qe") return fQe;
 if (name=="Me") return fMe;
 if (name=="Mmu") return fMmu;
 if (name=="Mtau") return fMtau;
 if (name=="Amu") return fAmu;
 if (name=="Mp") return fMp;
 if (name=="Mn") return fMn;
 if (name=="MW") return fMW;
 if (name=="GammaW") return fGammaW;
 if (name=="GammaZ") return fGammaZ;
 if (name=="AlphaEM") return fAlphaEM;
 if (name=="Fermi") return fFermi;
 if (name=="Planck") return fPlanck;
 if (name=="Boltz") return fBoltz;
 if (name=="Newton") return fNewton;
 if (name=="Gn") return fGn;
 if (name=="Au") return fAu;
 if (name=="Pc") return fPc;
 if (name=="Hubble") return fHubble;
 if (name=="OmegaM") return fOmegaM;
 if (name=="OmegaR") return fOmegaR;
 if (name=="OmegaL") return fOmegaL;
 if (name=="OmegaB") return fOmegaB;
 if (name=="OmegaC") return fOmegaC;

 // Derived parameters
 if (name=="Hbar") return fHbar;
 if (name=="Hbarc") return fHbarc;
 if (name=="Hbarc2") return fHbarc2;
 if (name=="Mnucl")
 {
  val=(fMp+fMn)/2.;
  return val;
 }
 if (name=="Sin2w")
 {
  val=1.-pow(fMW/fMZ,2.);
  return val;
 }
 if (name=="Jy") return 1e-23;
 if (name=="Erg")
 {
  val=1e-7/(fQe*1e9);
  return val;
 }

 // Unknown parameter
 return 0;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetPhysicalDistance(Double_t z,TString u,Int_t t) const
{
// Provide the physical distance of an object observed with redshift z
// for a flat Friedmann-Lemaitre universe.
//
// The physical distance reflects the distance that one would measure
// with a ruler at a specific time (see below).
//
// Note that the physical distance is also called "proper distance".
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/Home/lectures/thelargescaleuniverse
//
// The input argument "u" allows specification of the required distance units,
// with the following options:
//
// u = "Gpc" (distance in Giga parsec)
//     "Mpc" (distance in Mega parsec)
//     "pc"  (distance in parsec)
//     "ly"  (distance in light years)
//     "km"  (distance in kilometers)
//     "m"   (distance in meters)
//     "cm"  (distance in centimeters)
//
// The argument "t" allows to specify the time of the distance determination.
//
// t = 0 --> The distance is determined at the time of emission of the signal
//     1 --> The distance is determined at the time of observation of the signal
//
// The default values are u="Mpc" and t=1 for backward compatibility.
// In case "u" is incorrectly specified, the value 0 will be returned.

 if (z<=0 || fHubble<=0) return 0;

 Double_t c=fSpeedC/1000.; // Lightspeed in km/s

 TF1 f("f","1./sqrt([0]*pow((1.+x),4)+[1]*pow((1.+x),3)+[2])");

 f.SetParameter(0,fOmegaR);
 f.SetParameter(1,fOmegaM);
 f.SetParameter(2,fOmegaL);
 f.SetRange(0,z);

 Double_t dist=f.Integral(0,z);
 dist*=c/fHubble; // The distance in Mpc

 Double_t distm=dist*1e6*fPc; // corresponding distance in meter

 Double_t val=0;

 if (u=="Gpc") val=dist*1e-3;
 if (u=="Mpc") val=dist;
 if (u=="pc") val=dist*1e6;
 if (u=="ly") val=dist*3.26156e6;
 if (u=="m") val=distm;
 if (u=="km") val=distm*1e-3;
 if (u=="cm") val=distm*1e2;

 if (!t) val=val/(z+1.);

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetProperDistance(Double_t z,TString u,Int_t t) const
{
// Provide the proper distance of an object observed with redshift z
// for a flat Friedmann-Lemaitre universe.
//
// The proper distance reflects the distance that one would measure
// with a ruler at a specific time (see below).
//
// Note that the proper distance is also called "physical distance".
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/Home/lectures/thelargescaleuniverse
//
// The input argument "u" allows specification of the required distance units,
// with the following options:
//
// u = "Gpc" (distance in Giga parsec)
//     "Mpc" (distance in Mega parsec)
//     "pc"  (distance in parsec)
//     "ly"  (distance in light years)
//     "km"  (distance in kilometers)
//     "m"   (distance in meters)
//     "cm"  (distance in centimeters)
//
// The argument "t" allows to specify the time of the distance determination.
//
// t = 0 --> The distance is determined at the time of emission of the signal
//     1 --> The distance is determined at the time of observation of the signal
//
// The default values are u="Mpc" and t=1 for backward compatibility.
// In case "u" is incorrectly specified, the value 0 will be returned.

 Double_t val=GetPhysicalDistance(z,u,t);

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetComovingDistance(Double_t z,TString u) const
{
// Provide the comoving coordinate distance of an object observed with redshift z
// for a flat Friedmann-Lemaitre universe.
//
// The comoving coordinate distance is constant in time and has in principle
// an arbitrary normalization.
// We adopt here the commonly used normalization that the cosmic expansion factor a(t)
// equals 1 at the current time.
// This implies that the comoving coordinate distance is equal to the physical (or proper)
// distance at the time of the observation of a signal from an object with redshift z.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/Home/lectures/thelargescaleuniverse
//
// The input argument "u" allows specification of the required distance units,
// with the following options:
//
// u = "Gpc" (distance in Giga parsec)
//     "Mpc" (distance in Mega parsec)
//     "pc"  (distance in parsec)
//     "ly"  (distance in light years)
//     "km"  (distance in kilometers)
//     "m"   (distance in meters)
//     "cm"  (distance in centimeters)
//
// The default value is u="Mpc" for backward compatibility.
// In case "u" is incorrectly specified, the value 0 will be returned.

 Double_t val=GetPhysicalDistance(z,u,1);

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetLuminosityDistance(Double_t z,TString u) const
{
// Provide the luminosity distance of an object observed with redshift z
// for a flat Friedmann-Lemaitre universe.
//
// Consider an object with known intrinsic luminosity L (erg/s) of which
// a flux F (erg/(s*cm^2) is observed.
// The luminosity distance d is defined such that F=L/(4*pi*d^2), which can
// be alternatively written as d=sqrt(L/(4*pi*F)).
//
// The luminosity distance d can be related to the physical distance D by
// realizing that the observed energy is reduced by a factor (z+1) and that 
// the observed time intervals are stretched by a factor (z+1).
// This results in F=L/[(z+1)*(z+1)*4*pi*D^2] or in other words : d=(z+1)*D.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/Home/lectures/thelargescaleuniverse
//
// The input argument "u" allows specification of the required distance units,
// with the following options:
//
// u = "Gpc" (distance in Giga parsec)
//     "Mpc" (distance in Mega parsec)
//     "pc"  (distance in parsec)
//     "ly"  (distance in light years)
//     "km"  (distance in kilometers)
//     "m"   (distance in meters)
//     "cm"  (distance in centimeters)
//
// The default is u="Mpc" for backward compatibility
// In case "u" is incorrectly specified, the value 0 will be returned.

 Double_t val=GetPhysicalDistance(z,u,1);
 val=val*(z+1.);

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetLightTravelDistance(Double_t z,TString u) const
{
// Provide the light-travel distance of an object observed with redshift z
// for a flat Friedmann-Lemaitre universe.
//
// The light-travel distance reflects the distance (c*dt) that light has traveled
// to reach us from an object observed with a redshift z.
//
// The light-travel distance is determined via a similar integral as the physical distance,
// but taking into account an extra term 1/(1+z) in the integrand.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/Home/lectures/thelargescaleuniverse
//
// The light-travel time may be obtained by dividing the light-travel distance by
// the lightspeed. Alternatively, expressing the light-travel distance in units
// of light years (ly) reflects directly the light-travel time in years.
//
// The input argument "u" allows specification of the required distance units,
// with the following options:
//
// u = "Gpc" (distance in Giga parsec)
//     "Mpc" (distance in Mega parsec)
//     "pc"  (distance in parsec)
//     "ly"  (distance in light years)
//     "km"  (distance in kilometers)
//     "m"   (distance in meters)
//     "cm"  (distance in centimeters)
//
// The default is u="Mpc" for backward compatibility
// In case "u" is incorrectly specified, the value 0 will be returned.

 if (z<=0 || fHubble<=0) return 0;

 Double_t c=fSpeedC/1000.; // Lightspeed in km/s

 TF1 f("f","1./((1.+x)*sqrt([0]*pow((1.+x),4)+[1]*pow((1.+x),3)+[2]))");

 f.SetParameter(0,fOmegaR);
 f.SetParameter(1,fOmegaM);
 f.SetParameter(2,fOmegaL);
 f.SetRange(0,z);

 Double_t dist=f.Integral(0,z);
 dist*=c/fHubble; // The distance in Mpc

 Double_t distm=dist*1e6*fPc; // corresponding distance in meter

 Double_t val=0;

 if (u=="Gpc") val=dist*1e-3;
 if (u=="Mpc") val=dist;
 if (u=="pc") val=dist*1e6;
 if (u=="ly") val=dist*3.26156e6;
 if (u=="m") val=distm;
 if (u=="km") val=distm*1e-3;
 if (u=="cm") val=distm*1e2;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetLightTravelTime(Double_t z) const
{
// Provide the light-travel time (in years) of an object observed with redshift z
// for a flat Friedmann-Lemaitre universe.
//
// The light-travel time reflects the time that light has traveled
// to reach us from an object observed with a redshift z.
//
// The light-travel time is also called the look back time.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/Home/lectures/thelargescaleuniverse
//
// The light-travel time is obtained by dividing the light-travel distance by
// the lightspeed.

 Double_t val=GetLightTravelDistance(z,"ly");

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetHubbleParameter(Double_t z,TString u) const
{
// Provide the Hubble parameter H(z) at a certain redshift z
// for a flat Friedmann-Lemaitre universe.
//
// The input argument "u" allows specification of the required units,
// with the following options:
//
// u = "Gpc" (H(z) in km/s per Giga parsec)
//     "Mpc" (H(z) in km/s per Mega parsec)
//     "pc"  (H(z) in km/s per parsec)
//     "ly"  (H(z) in km/s per light year)
//     "km"  (H(z) in km/s per kilometer)
//     "m"   (H(z) in km/s per meter)
//     "cm"  (H(z) in km/s per centimeter)
//
// The default is u="Mpc" for backward compatibility
// In case "u" is incorrectly specified, the value 0 will be returned.

 if (z<0 || fHubble<=0) return 0;

 TF1 f("f","sqrt([0]*pow((1.+x),4)+[1]*pow((1.+x),3)+[2])");

 f.SetParameter(0,fOmegaR);
 f.SetParameter(1,fOmegaM);
 f.SetParameter(2,fOmegaL);
 f.SetRange(0,z);

 Double_t H=f.Eval(z);
 H*=fHubble; // The current Hubble parameter (H0) in km/s per Mpc

 Double_t Hm=H/(1e6*fPc); // corresponding H in km/s per meter

 Double_t val=0;

 if (u=="Gpc") val=H/1e-3;
 if (u=="Mpc") val=H;
 if (u=="pc") val=H/1e6;
 if (u=="ly") val=H/3.26156e6;
 if (u=="m") val=Hm;
 if (u=="km") val=Hm/1e-3;
 if (u=="cm") val=Hm/1e2;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetNuclearMass(Int_t Z,Int_t N,Int_t mode) const
{
// Provide the nuclear mass or binding energy of a specified nucleus
// with "Z" protons and "N" neutrons.
// Here the semi-empirical mass formula is used as indicated on the slides
// of the lecture "Aspects of Nuclear Physics" by Nick van Eijndhoven.
// For very light nuclei this may be inaccurate.
// In view of astrophysical importance, the literature mass values are taken
// for the Deuteron, Triton, Helion and Alpha particles.
//
// Note : In case of an attractive potential, the binding energy is
//        returned as a positive value.
//
// The input argument "mode" allows to choose between the options :
//
// mode :  1 --> The nuclear mass is returned in GeV/c^2
//        -1 --> The nuclear mass is returned in amu
//         2 --> The total binding energy is returned in MeV
//        -2 --> The total binding energy is returned in amu
//         3 --> The binding energy per nucleon is returned in MeV
//        -3 --> The binding energy per nucleon is returned in amu
//
// Here "amu" stands for unified atomic mass unit. 
//
// The default is mode=1.

 if (Z<0 || N<0) return 0;

 Double_t rz=Z;
 Double_t rn=N;
 Double_t ra=Z+N;

 // Coefficients from a recent fit mentioned in Tipler's modern physics (4th ed.) textbook.
 // The values in the comment field are the ones of the slides mentioned above.
 Double_t a=15.67;  //15.835;
 Double_t b=17.23;  //18.33;
 Double_t s=23.2;   //23.20;
 Double_t d=0.75;   //0.714;
 Double_t delta=12; //11.2;

 Double_t term1=a*ra;                       // Constant bulk binding energy like cohesion in a liquid
 Double_t term2=b*pow(ra,2./3.);            // Surface energy of a sphere like surface tension of liquids
 Double_t term3=s*pow((rn-rz),2.)/ra;       // Symmetry term
 Double_t term4=d*pow(rz,2.)/pow(ra,1./3.); // Coulomb term
 Double_t term5=0;                          // Phenomenological correction for light nuclei (pairing energy term)
 
 Int_t oz=Z%2; // Flag (1) for odd Z nuclei 
 Int_t on=N%2; // Flag (1) for odd N nuclei

 if (oz && on) term5=delta/sqrt(ra);
 if (!oz && !on) term5=-delta/sqrt(ra);

 // Binding energy in MeV
 Double_t bnz=term1-term2-term3-term4-term5;

 // In case a single nucleon was specified
 if ((Z+N)<2)
 {
  bnz=0;
  ra=1;
 }

 // Nuclear mass in MeV/c^2
 Double_t mass=rz*fMp+rn*fMn-bnz;

 // Explicit literature values for very light elements
 if (Z==1 && N==1) // Deuteron
 {
  mass=2.013553212712*fAmu;
  bnz=rz*fMp+rn*fMn-mass;
 }

 if (Z==1 && N==2) // Triton
 {
  mass=3.0155007134*fAmu;
  bnz=rz*fMp+rn*fMn-mass;
 }

 if (Z==2 && N==1) // Helion
 {
  mass=3.0149322468*fAmu;
  bnz=rz*fMp+rn*fMn-mass;
 }

 if (Z==2 && N==2) // Alpha
 {
  mass=4.001506179125*fAmu;
  bnz=rz*fMp+rn*fMn-mass;
 }

 Double_t value=0;

 switch(mode)
 {
  case 1 : // Nuclear mass in GeV/c^2
   value=mass/1000.;
   break;

  case -1 : // Nuclear mass in amu
   value=mass/fAmu;
   break;

  case 2 : // Total binding energy in MeV
   value=bnz;
   break;

  case -2 : // Total binding energy in amu
   value=bnz/fAmu;
   break;

  case 3 : // Binding energy per nucleon in MeV
   value=bnz/ra;
   break;

  case -3 : // Binding energy per nucleon in amu
   value=bnz/(fAmu*ra);
   break;
 }

 return value;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetRadiationLength(Double_t Z,Double_t A,Double_t rho) const
{
// Provide the radiation length of a medium consisting of (averaged) nuclei (Z,A).
// The radiation length is returned in g/cm^2, #nucleons/cm^2 or cm, depending on
// the value of "rho" (see below)
//
// The radition length (X0) is the mean distance over which the energy of a high-energy electron
// is reduced by a factor 1/e due to bremmstrahlung.
// Alternatively, it is also 7/9 times the mean free path of a high-energy photon before
// pair production occurs.
//
// The following approximation was obtained from the Particle Data Group (https://pdg.lbl.gov)
//
// X0=716.4*A/[Z(Z+1)(Ln(287/sqrt(Z))] g/cm^2
//
// Input arguments :
// -----------------
// Z   : The (averaged) atomic number (=number of protons) of the nucleus 
// A   : The (averaged) mass number (=number of nucleons) of the nucleus
// rho : <0 --> [X0]=g/cm^2
//       =0 --> [X0]=#nucleons/cm^2
//       >0 --> rho=density of the medium in g/cm^3 and [X0]=cm
//
// The default value is rho=-1.
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t X0=-1;

 if (Z<=0 || A<1 || A<Z) return -1;

 X0=716.4*A/(Z*(Z+1.)*(log(287./sqrt(Z))));

 Double_t mN=0.001*fGn*fQe/fAmu; // The nucleon mass in gram
 if (rho==0) X0=X0/mN;
 if (rho>0) X0=X0/rho;

 return X0;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetMeanFreePath(Double_t sigma,Double_t rho,Int_t mode) const
{
// Provide the mean free path "lambda" of a high-energy particle passing through a certain medium.
// The input parameter "mode" dictates the units of "rho" and the returned value of "lambda" (see below).
//
// The mean free path (lambda) is the mean distance over which a high-energy particle travels
// inside the medium before an interaction occurs.
// For non (quasi)elastic interactions, this is also called "interaction length",
// "attenuation length" or "absorption length".
//
// The scattering centers (=particles) in the medium are regarded as being stationary,
// such that we can use the formula :
//
// lambda=1/(sigma*n)
//
// where "sigma" represents the cross section and "n" represents the number density
// of the scattering centers in the medium.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// Input arguments :
// -----------------
// sigma : The interaction cross section in barn (1 barn=1e-24 cm^2)
// rho   : The density of the medium (see below)
// mode  : 0 --> rho=#particles/cm^3 and [lambda]=cm
//         1 --> rho=#particles/cm^3 and [lambda]=#particles/cm^2
//         2 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [lambda]=cm
//         3 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [lambda]=g/cm^2
//         4 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [lambda]=#nucleons/cm^2
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t lambda=-1;

 if (sigma<=0 || rho<=0 || mode<0 || mode>4) return -1;

 Double_t mN=0.001*fGn*fQe/fAmu; // The nucleon mass in gram
 Double_t n=fabs(rho)/mN;        // The number of nucleons per cm^3
 if (mode<2) n=rho;              // The number of scattering centers per cm^3
 sigma=sigma*1e-24;              // Convert to cm^2

 lambda=1./(sigma*n);

 if (mode==1 || mode==4) lambda=lambda*n;
 if (mode==3) lambda=lambda*rho;

 return lambda;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetInteractionProbability(Double_t x,Double_t lambda) const
{
// Provide the interaction probability of a high-energy particle traveling
// a distance "x" through a certain medium.
//
// For a mono-energetic beam of identical particles, this probability allows
// to calculate the interaction rate once the beam luminosity is known.
//
// Input arguments :
// -----------------
// x      : The distance traveled by the particle inside the medium
// lambda : The mean free path of the particle in the medium
//
// The formula used is : prob=1-exp(-x/lambda).
//
// So, when x=lambda, the interaction probability is about 0.63.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// Notes :
// -------
// 1) "x" and "lambda" have to be specified in the same units.
// 2) The probability of pair production by a high-energy photon
//    may be obtained from the corresponding radiation length X0
//    by using the fact that lambda=X0*9/7.
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t prob=-1;

 if (x<0 || lambda<=0) return -1;

 prob=1.-exp(-x/lambda);

 return prob;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetInteractionProbability(Double_t x,Double_t sigma,Double_t rho,Int_t mode) const
{
// Provide the interaction probability of a high-energy particle traveling
// a distance "x" through a certain medium.
// The input parameter "mode" dictates the units of "x" and "rho" (see below).
//
// For a mono-energetic beam of identical particles, this probability allows
// to calculate the interaction rate once the beam luminosity is known.
//
// Input arguments :
// -----------------
// x     : The distance traveled by the particle inside the medium (see below)
// sigma : The interaction cross section in barn (1 barn=1e-24 cm^2)
// rho   : The density of the medium (see below)
// mode  : 0 --> rho=#particles/cm^3 and [x]=cm
//         1 --> rho=#particles/cm^3 and [x]=#particles/cm^2
//         2 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=cm
//         3 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=g/cm^2
//         4 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=#nucleons/cm^2
//
// The formula used is : prob=1-exp(-x/lambda),
// where lambda is the corresponding mean free path.
//
// So, when x=lambda, the interaction probability is about 0.63.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t prob=-1;

 Double_t lambda=GetMeanFreePath(sigma,rho,mode);

 if (lambda>0) prob=GetInteractionProbability(x,lambda);

 return prob;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetSurvivalProbability(Double_t x,Double_t lambda) const
{
// Provide the survival probability of a high-energy particle traveling
// a distance "x" through a certain medium.
//
// For a mono-energetic beam of identical particles, this probability
// also represents the beam attenuation factor.
//
// Input arguments :
// -----------------
// x      : The distance traveled by the particle inside the medium
// lambda : The mean free path of the particle in the medium
//
// The formula used is : prob=exp(-x/lambda).
//
// So, when x=lambda, the survial probability is 1/e.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// Notes :
// -------
// 1) "x" and "lambda" have to be specified in the same units.
// 2) The probability for a high-energy photon to survive pair production
//    may be obtained from the corresponding radiation length X0
//    by using the fact that lambda=X0*9/7.
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t prob=-1;

 if (x<0 || lambda<=0) return -1;

 prob=exp(-x/lambda);

 return prob;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetSurvivalProbability(Double_t x,Double_t sigma,Double_t rho,Int_t mode) const
{
// Provide the survival probability of a high-energy particle traveling
// a distance "x" through a certain medium.
// The input parameter "mode" dictates the units of "x" and "rho" (see below).
//
// For a mono-energetic beam of identical particles, this probability
// also represents the beam attenuation factor.
//
// Input arguments :
// -----------------
// x     : The distance traveled by the particle inside the medium (see below)
// sigma : The interaction cross section in barn (1 barn=1e-24 cm^2)
// rho   : The density of the medium (see below)
// mode  : 0 --> rho=#particles/cm^3 and [x]=cm
//         1 --> rho=#particles/cm^3 and [x]=#particles/cm^2
//         2 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=cm
//         3 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=g/cm^2
//         4 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=#nucleons/cm^2
//
// The formula used is : prob=exp(-x/lambda),
// where lambda is the corresponding mean free path.
//
// So, when x=lambda, the survial probability is 1/e.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t prob=-1;

 Double_t lambda=GetMeanFreePath(sigma,rho,mode);

 if (lambda>0) prob=GetSurvivalProbability(x,lambda);

 return prob;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetShieldingThickness(Double_t prob,Double_t lambda) const
{
// Provide the shielding thickness "x" to obtain the specified survival probability
// for a high-energy particle.
// The calculated thickness is returned in the same units as the provided "lambda".
//
// Input arguments :
// -----------------
// prob   : The requested survival probability
// lambda : The mean free path of the particle in the medium
//
// The formula used is : x=-lambda*Ln(prob).
//
// So, when the specified probability is 1/e, the shielding thickness x=lambda.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// Note :
// ------
// The shielding thickness for a high-energy photon to undergo pair production
// may be obtained from the corresponding radiation length X0 by using the fact
// that lambda=X0*9/7.
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t x=-1;

 if (prob<=0 || prob>1 || lambda<=0) return -1;

 x=-lambda*log(prob);

 return x;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetShieldingThickness(Double_t prob,Double_t sigma,Double_t rho,Int_t mode) const
{
// Provide the shielding thickness "x" to obtain the specified survival probability
// for a high-energy particle.
// The input parameter "mode" dictates the units of "rho" and the returned value of "x" (see below).
//
// Input arguments :
// -----------------
// prob  : The requested survival probability
// sigma : The interaction cross section in barn (1 barn=1e-24 cm^2)
// rho   : The density of the medium (see below)
// mode  : 0 --> rho=#particles/cm^3 and [x]=cm
//         1 --> rho=#particles/cm^3 and [x]=#particles/cm^2
//         2 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=cm
//         3 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=g/cm^2
//         4 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=#nucleons/cm^2
//
// The formula used is : x=-lambda*Ln(prob),
// where lambda is the corresponding mean free path.
//
// So, when the specified probability is 1/e, the shielding thickness x=lambda.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t x=-1;

 if (prob<=0 || prob>1) return -1;

 Double_t lambda=GetMeanFreePath(sigma,rho,mode);

 if (lambda>0) x=GetShieldingThickness(prob,lambda);

 return x;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetTargetThickness(Double_t prob,Double_t lambda) const
{
// Provide the target thickness "x" to obtain the specified interaction probability
// for a high-energy particle.
// The calculated thickness is returned in the same units as the provided "lambda".
//
// Input arguments :
// -----------------
// prob   : The requested interaction probability
// lambda : The mean free path of the particle in the medium
//
// The formula used is : x=-lambda*Ln(1-prob).
//
// So, when the specified probability is (1-1/e), the shielding thickness x=lambda.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// Note :
// ------
// The target thickness for a high-energy photon to undergo pair production
// may be obtained from the corresponding radiation length X0 by using the fact
// that lambda=X0*9/7.
//
// In case of inconsistent input the unphysical value -1 will be returned.

 if (prob<=0 || prob>1 || lambda<=0) return -1;

 Double_t p=1.-prob;
 Double_t x=GetShieldingThickness(p,lambda);

 return x;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetTargetThickness(Double_t prob,Double_t sigma,Double_t rho,Int_t mode) const
{
// Provide the target thickness "x" to obtain the specified interaction probability
// for a high-energy particle.
// The input parameter "mode" dictates the units of "rho" and the returned value of "x" (see below).
//
// Input arguments :
// -----------------
// prob  : The requested interaction probability
// sigma : The interaction cross section in barn (1 barn=1e-24 cm^2)
// rho   : The density of the medium (see below)
// mode  : 0 --> rho=#particles/cm^3 and [x]=cm
//         1 --> rho=#particles/cm^3 and [x]=#particles/cm^2
//         2 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=cm
//         3 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=g/cm^2
//         4 --> Atomic c.q. nuclear medium with [rho]=g/cm^3 and [x]=#nucleons/cm^2
//
// The formula used is : x=-lambda*Ln(1-prob),
// where lambda is the corresponding mean free path.
//
// So, when the specified probability is (1-1/e), the shielding thickness x=lambda.
//
// For details please refer to the slides of my lectures :
// https://sites.google.com/site/nickveweb/lectures/scatteringanddecayprocesses
//
// In case of inconsistent input the unphysical value -1 will be returned.

 Double_t x=-1;

 if (prob<=0 || prob>1) return -1;

 Double_t lambda=GetMeanFreePath(sigma,rho,mode);

 Double_t p=1.-prob;
 if (lambda>0) x=GetShieldingThickness(p,lambda);

 return x;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetNeutrinoXsection(Int_t mode,Int_t type,Double_t egev,Double_t xscale,Double_t* eprimgev,Double_t* alpha) const
{
// Provide the neutrino cross section and (optional) the average energy
// and opening angle of the produced charged lepton (CC) or (anti)neutrino (NC).
// Below 10 TeV the cross section determination is based on a simple linear
// relation between the cross section and the neutrino energy : sigma=f*E.
// The coefficients f for neutrinos and antineutrinos are taken from
// the data of the Particle Data Group and from the publication 
// J.Formaggio and G.Zeller, Rev. Mod. Phys. 84.3 (2012) 1307.
// For energies of 10 TeV and above, the cross section parametrisations of
// A. Connolly et al. (Phys. Rev. D83 (2011) 113009 [ArXiv:1102.0691]) are used.
// The average inelasticity parameter y is obtained from the publication of Gandhi et al.
// (Astroparticle Physics 5 (1996) 81 [ArXiv:hep-ph/9512364]).
// The sigma_nc/sigma_cc ratios for Nucleon DIS are obtained from Gandhi et al.
// (Phys. Rev. D58 (1998) 093009 [ArXiv:hep-ph/9807264]).
//
// Input arguments :
// -----------------
// mode   :  1 --> Nucleon target DIS CC cross section
//           2 --> Nucleon target DIS NC cross section
//           3 --> Nucleon target DIS total cross section
//          -1 --> Electron target CC cross section
//          -2 --> Electron target NC cross section
//          -3 --> Electron target total cross section
//          -4 --> Special CC process anu_e+e-->anu_mu+mu (not included in other mode values)
// type   :  1 --> Electron neutrino
//           2 --> Muon neutrino
//           3 --> Tau neutrino
//          -1 --> Electron anti-neutrino
//          -2 --> Muon anti-neutrino
//          -3 --> Tau anti-neutrino
// egev   : Neutrino energy in GeV
// xscale : Cross section scale for return value (1=barn, 0.001=mb etc...)
//
// Note : 1 cm^2 corresponds to 1e24 barn.
//
// The default value is xscale=1.
//
// Optional output arguments :
// ---------------------------
// eprimgev : Average energy in GeV of the produced charged lepton (CC) or (anti)neutrino (NC).
// alpha    : Average opening angle in degrees between the neutrino and the produced charged lepton (CC) or (anti)neutrino (NC).
//
// In case of incompatible input or unavailable data the value 0 will be returned.

 if (eprimgev) *eprimgev=0;
 if (alpha) *alpha=0;
 if (!mode || mode>3 || mode<-4 || !type || abs(type)>3) return 0;

 const Double_t fnumuccn=6.77e-15;             // Nu_mu+Nucleon CC sigma/E in barn/GeV
 const Double_t fanumuccn=3.34e-15;            // Anti-Nu_mu+Nucleon CC sigma/E in barn/GeV

 const Double_t sinw2=GetPhysicalParameter("Sin2w"); // sin^2 of the Weinberg angle

 const Double_t fnuetote=0.25+sinw2+4.*pow(sinw2,2.)/3.;           // Nu_e+e total sigma/sigma0
 const Double_t fanuetote=(1./12.)+(sinw2/3.)+4.*pow(sinw2,2.)/3.; // Anti-Nu_e+e total sigma/sigma0
 Double_t fnumucce=1.;                                             // Nu_mu+e CC sigma/sigma0
 const Double_t fnumunce=0.25-sinw2+4.*pow(sinw2,2.)/3.;           // Nu_mu+e NC sigma/sigma0
 const Double_t fanumunce=(1./12.)-(sinw2/3.)+4.*pow(sinw2,2.)/3.; // Anti-Nu_mu+e NC sigma/sigma0
 const Double_t f4=1./3.;                                          // Anti-Nu_e+e-->Anti-Nu_mu+mu CC sigma/sigma0

 // Parameters for the (anti)neutrino+Nucleon cross section parametrisations of Conolly et al.
 const Double_t c0nu=-1.826;
 const Double_t c1nu=-17.31;
 const Double_t c2nunc=-6.448;
 const Double_t c2nucc=-6.406;
 const Double_t c3nu=1.431;
 const Double_t c4nunc=-18.61;
 const Double_t c4nucc=-17.91;
 const Double_t c0anu=-1.033;
 const Double_t c1anu=-15.95;
 const Double_t c2anunc=-7.296;
 const Double_t c2anucc=-7.247;
 const Double_t c3anu=1.569;
 const Double_t c4anunc=-18.30;
 const Double_t c4anucc=-17.72;

 Double_t rncnu=0.2261/0.7221;  // sigma_nc/sigma_cc for Neutrino+Nucleon DIS at 100 GeV
 Double_t rncanu=0.1307/0.3747; // sigma_nc/sigma_cc for Anti-neutrino+Nucleon DIS at 100 GeV

 // Average inelasticity (y) values from Gandhi et al.
 Double_t ynucc[12]={0.483,0.477,0.472,0.426,0.332,0.237,0.250,0.237,0.225,0.216,0.208,0.205};
 Double_t ynunc[12]={0.474,0.470,0.467,0.428,0.341,0.279,0.254,0.239,0.227,0.217,0.210,0.207};
 Double_t yanucc[12]={0.333,0.340,0.354,0.345,0.301,0.266,0.249,0.237,0.225,0.216,0.208,0.205};
 Double_t yanunc[12]={0.350,0.354,0.368,0.358,0.313,0.273,0.253,0.239,0.227,0.217,0.210,0.207};

 Double_t loge=log10(egev);
 Double_t y=0;
 Int_t index=int(loge+0.5);
 if (index<1) index=1;
 if (index>12) index=12;
 if (eprimgev)
 {
  if (type>0) // Neutrinos
  {
   if (abs(mode)==1) y=ynucc[index-1];
   if (abs(mode)==2) y=ynunc[index-1];
   if (mode==3) y=(ynucc[index-1]+rncnu*ynunc[index-1])/(1.+rncnu);
   if (mode==-3) y=(ynucc[index-1]+fnumunce*ynunc[index-1])/(1.+fnumunce);
  }
  else // Anti-neutrinos
  {
   if (abs(mode)==1) y=yanucc[index-1];
   if (abs(mode)==2) y=yanunc[index-1];
   if (mode==3) y=(yanucc[index-1]+rncanu*yanunc[index-1])/(1.+rncanu);
   if (mode==-3) y=(yanucc[index-1]+fnumunce*yanunc[index-1])/(1.+fnumunce);
  }
  *eprimgev=egev*(1.-y);
 }

 if (alpha)
 {
  Double_t mtarg=fMe;
  if (mode>0) mtarg=GetPhysicalParameter("Mnucl");
  *alpha=sqrt(2.e-3*mtarg/egev)*y*180./((1.-y)*acos(-1.));
 }

 Double_t xsec=0;
 Double_t fact=0;
 Double_t c0=0,c1=0,c2=0,c3=0,c4=0;

 if (mode>0) // DIS on Nucleon target
 {
  if (mode==3) // Total xsec
  {
   // Recursive invokation for NC+CC cross section
   xsec=GetNeutrinoXsection(1,type,egev,xscale);
   xsec+=GetNeutrinoXsection(2,type,egev,xscale);
   return xsec;
  }
  if (egev<1e4) // Energy below 10 TeV
  {
   if (mode==1) // CC xsec
   {
    fact=fnumuccn;
    if (type<0) fact=fanumuccn;
   }
   if (mode==2) // NC xsec
   {
    fact=fnumuccn*rncnu;
    if (type<0) fact=fanumuccn*rncanu;
   }
   xsec=fact*egev;
  }
  else // Energy of 10 TeV and above
  {
   if (mode==1) // CC xsec
   {
    if (type>0)
    {
     c0=c0nu;
     c1=c1nu;
     c2=c2nucc;
     c3=c3nu;
     c4=c4nucc;
    }
    else
    {
     c0=c0anu;
     c1=c1anu;
     c2=c2anucc;
     c3=c3anu;
     c4=c4anucc;
    }
   }
   if (mode==2) // NC xsec
   {
    if (type>0)
    {
     c0=c0nu;
     c1=c1nu;
     c2=c2nunc;
     c3=c3nu;
     c4=c4nunc;
    }
    else
    {
     c0=c0anu;
     c1=c1anu;
     c2=c2anunc;
     c3=c3anu;
     c4=c4anunc;
    }
   }
   Double_t lne=log(loge-c0);
   Double_t logsigma=c1+c2*lne+c3*pow(lne,2.)+c4/lne; // Log10(sigma) in cm^2
   logsigma+=24.;
   xsec=pow(10.,logsigma);
  }
 }
 else // Scattering on electron target
 {
  // Check whether we are in the Glashow resonance regime
  Double_t elow=pow((fMW-2.*fGammaW),2.)/(2.e-3*fMe);  
  Double_t eup=pow((fMW+2.*fGammaW),2.)/(2.e-3*fMe);
  if (mode==-3 && type==-1 && egev>elow && egev<eup) // Total xsec at Glashow resonance
  {
   xsec=5.02e-7/xscale;
   if (eprimgev) *eprimgev=0;
   if (alpha) *alpha=0;
   return xsec;
  }

  // Check if we are above the kinematical threshold energy for CC scattering
  Double_t mlepton=fMe;
  if (abs(type)==2) mlepton=fMmu;
  if (abs(type)==3) mlepton=fMtau;
  Double_t eth=1.e-3*(pow(mlepton,2.)-pow(fMe,2.))/(2.*fMe);

  if (egev<eth) // Below CC kinematical threshold 
  {
   fnumucce=0;
   if (mode==-1) // CC xsec was requested
   {
    if (eprimgev) *eprimgev=egev;
    if (alpha) *alpha=0;
    return 0;
   }
  }

  // The Nu_mu+e CC cross section in barn well above threshold
  Double_t sigma0=pow(fFermi,2.)*fHbarc2*2.e-3*fMe*egev/acos(-1.);

  if (mode==-1) // CC xsec
  {
   if (type>1) fact=fnumucce;
  }
  if (mode==-2) // NC xsec
  {
   if (type>1) fact=fnumunce;
   if (type<-1) fact=fanumunce;
  }
  if (mode==-3) // Total xsec
  {
   if (type==1) fact=fnuetote;
   if (type==-1) fact=fanuetote;
   if (type>1) fact=fnumucce+fnumunce;
   if (type<-1) fact=fanumunce;
  }
  if (mode==-4 && type==-1) fact=f4;
  xsec=fact*sigma0;
 }

 xsec/=xscale;

 return xsec;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetNeutrinoAngle(Double_t E,TString u,Int_t mode,TF1* f)
{
// Provide the kinematic opening angle between a neutrino and the corresponding lepton,
// as produced in a CC interaction on a target at rest.
//
// Input arguments :
// -----------------
// E    : The neutrino energy in GeV
// u    : "deg" --> Angle will be returned in degrees 
//        "rad" --> Angle will be returned in radians
// mode : 0 -> Return the mean angle
//        1 -> Return the median angle
//        2 -> Return a random angle based on a parametrisation of the pdf
// f    : Optional argument to obtain the function describing the used pdf
//        In case f=0 no function will be returned.
//
// Note :
// ------
// The pdf for the opening angle has been obtained via simulations using the
// Pythia based NcCollider facility.
// The data for a 1 TeV muon neutrino incident on a proton at rest have been
// fitted by a Landau distribution.
// For other energies, the Landau parameters will be scaled via 1/sqrt(E/(1TeV)).
//
// Usage example :
// ---------------
// Double_t E=500;
// TF1 func;
// Double_t angle=GetNeutrinoAngle(E,"deg",2,&func);
//
// The default value is f=0.
//
// In case of inconsistent data, the unphysical value of -1 will be returned.

 Double_t value=-1;

 if (E<=0 || mode<0 || mode>2) return value;

 // Convert to TeV
 E*=0.001;

 // The parametrisation (in degrees) for a 1 TeV neutrino
 Double_t mean=1.38711583;
 Double_t median=0.86842105;
 Double_t mpv=0.560150;
 Double_t sigma=0.226679;

 // Scaling the parameters to the provided neutrino energy
 Double_t p=log10(E);
 Double_t scale=1./(pow(1.5,p)*sqrt(E));

 mean*=scale;
 median*=scale;
 mpv*=scale;
 sigma*=scale;

 // Create a normalized Landau distribution template
 if (!fNuAngle)
 {
  fNuAngle=new TF1("NuAngle","TMath::Landau(x,[0],[1],1)",0,90);
  fNuAngle->SetTitle("Landau pdf;Neutrino-lepton opening angle in degrees;PDF");
 }
 
 // Set the parameters for the Landau parametrization
 fNuAngle->SetParameter(0,mpv);
 fNuAngle->SetParameter(1,sigma);

 // Obtain an opening angle value according to the pdf
 Double_t ang=fNuAngle->GetRandom();
 
 if (u=="rad")
 {
  Double_t fact=acos(-1.)/180.; // Coversion factor degrees->radians
  mean*=fact;
  median*=fact;
  ang*=fact;
 }

 // Provide the used pdf if requested
 if (f) fNuAngle->Copy(*f);

 value=mean;
 if (mode==1) value=median;
 if (mode==2) value=ang;

 return value;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::RandomPosition(Nc3Vector& v,Double_t thetamin,Double_t thetamax,Double_t phimin,Double_t phimax)
{
// Provide a random angular position for the vector "v" according to an isotropic solid angle distribution.
// The random position will be located in [thetamin,thetamax] and [phimin,phimax].
//
// Notes :
// -------
// 1) All angles have to be given in degrees with theta=[0,180] and phi=[0,360].
// 2) The vector components c.q. errors of "v" don't have to be initialised.
//    In case the vector components of "v" are not initialised, the norm will be
//    automatically set to 1.
// 3) In case angular errors have been specified for the vector "v", the randomised vector
//    will obtain the same angular errors.

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // with identical measurements and reference signals without explicit initialisation
 // of the randomiser by the user at the start of the analysis.
 if (!fRan) fRan=new NcRandom(-1);

 // Generate random angles in the specified range
 Double_t pi=acos(-1.);
 Double_t cosmax=cos(thetamin*pi/180.);
 Double_t cosmin=cos(thetamax*pi/180.);
 Double_t cost=fRan->Uniform(cosmin,cosmax);
 Double_t theta=acos(cost)*180./pi;
 Double_t phi=fRan->Uniform(phimin,phimax);

 Double_t norm=1;
 if (v.HasVector()) norm=v.GetNorm();

 Double_t err[3]={0,0,0};
 Int_t ier=0;
 if (v.HasErrors())
 {
  ier=1;
  v.GetErrors(err,"car");
 }

 v.SetVector(norm,theta,phi,"sph","deg");
 if (ier) v.SetErrors(err,"car");
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SmearPosition(Nc3Vector& v,Double_t sigma)
{
// Smear the angular position for the vector "v" according to the specified
// pointing accuracy "sigma" in degrees.
// The input argument "sigma" determines the smearing in an angular cone (see below)
// around the actual pointing position of "v".
//
// sigma > 0 : Gaussian smearing in an angular cone with mean=0 and standard deviation "sigma" in degrees.
//       <=0 : Isotropic solid angle smearing in an angular cone of [0,abs(sigma)] in degrees.
// Notes :
// -------
// 1) The vector components of "v" have to be initialised, since they define
//    the center around which the smearing will be performed.
//    However, in case the norm of "v" is not set, the norm will be automatically set to 1.
// 2) In case angular errors have been specified for the vector "v", the smeared vector
//    will obtain the same angular errors.

 if (!v.HasVector()) return;

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // with identical measurements and reference signals without explicit initialisation
 // of the randomiser by the user at the start of the analysis.
 if (!fRan) fRan=new NcRandom(-1);

 Double_t norm=v.GetX(1,"sph","deg");
 Double_t theta=v.GetX(2,"sph","deg");
 Double_t phi=v.GetX(3,"sph","deg");
 Double_t err[3]={0,0,0};
 Int_t ier=0;
 if (v.HasErrors())
 {
  ier=1;
  v.GetErrors(err,"car");
 }
 if (norm<=0)
 {
  norm=1;
  err[0]=0;
 }
 v.SetVector(norm,theta,phi,"sph","deg");

 // The smeared position will be generated as if the actual vector "v" coincided with the positive Z-axis.
 // The actual smeared position will be obtained via a "backward rotation" to the real frame orientation.

 // Determine the rotation matrix for the frame in which "v" coincides with the positive Z-axis.
 TRotMatrix m;
 m.SetAngles(90.+theta,phi,90,phi+90.,theta,phi);

 // Generate smeared position w.r.t. the fictative Z-axis 
 Double_t pi=acos(-1.);
 Double_t cosmax=1;
 Double_t cosmin=cos(fabs(sigma)*pi/180.);
 Double_t phimin=0;
 Double_t phimax=360;
 Double_t cost=0;
 if (sigma<0)
 {
  cost=fRan->Uniform(cosmin,cosmax);
  theta=acos(cost)*180./pi;
 }
 else
 {
  theta=fRan->Gauss(0.,sigma);
 }
 phi=fRan->Uniform(phimin,phimax);

 // Enter the "fake" smeared position into vector "v".
 v.SetVector(norm,theta,phi,"sph","deg");

 // Invoke the inverse rotation to obtain the actual smeared position.
 v=v.GetUnprimed(&m);
 if (ier) v.SetErrors(err,"car");
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ShiftPosition(Nc3Vector& v,Double_t angle)
{
// Shift the angular position for the vector "v" with the specified angular
// offset "angle" in degrees.
// The input argument "angle" determines an angular cone around the original
// pointing position of "v", on which a random new position will be chosen.
//
// Notes :
// -------
// 1) The vector components of "v" have to be initialised, since they define
//    the center around which the smearing will be performed.
//    However, in case the norm of "v" is not set, the norm will be automatically set to 1.
// 2) In case angular errors have been specified for the vector "v", the smeared vector
//    will obtain the same angular errors.

 if (!v.HasVector()) return;

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // under identical conditions without explicit initialisation of the randomiser
 // by the user at the start of the analysis.
 if (!fRan) fRan=new NcRandom(-1);

 Double_t norm=v.GetX(1,"sph","deg");
 Double_t theta=v.GetX(2,"sph","deg");
 Double_t phi=v.GetX(3,"sph","deg");
 Double_t err[3]={0,0,0};
 Int_t ier=0;
 if (v.HasErrors())
 {
  ier=1;
  v.GetErrors(err,"car");
 }
 if (norm<=0)
 {
  norm=1;
  err[0]=0;
 }
 v.SetVector(norm,theta,phi,"sph","deg");

 // The shifted position will be generated as if the actual vector "v" coincided with the positive Z-axis.
 // The actual shifted position will be obtained via a "backward rotation" to the real frame orientation.

 // Determine the rotation matrix for the frame in which "v" coincides with the positive Z-axis.
 TRotMatrix m;
 m.SetAngles(90.+theta,phi,90,phi+90.,theta,phi);

 // Generate the shifted position w.r.t. the fictative Z-axis 
 Double_t phimin=0;
 Double_t phimax=360;
 theta=angle;
 phi=fRan->Uniform(phimin,phimax);

 // Enter the "fake" shifted position into vector "v".
 v.SetVector(norm,theta,phi,"sph","deg");

 // Invoke the inverse rotation to obtain the actual shifted position.
 v=v.GetUnprimed(&m);
 if (ier) v.SetErrors(err,"car");
}
///////////////////////////////////////////////////////////////////////////
TH1F NcAstrolab::GetDxHistogram(TH1* hx,Int_t nc,Double_t dxbin,Double_t dxmin,Double_t dxmax,Int_t mode,Double_t fact)
{
// Provide the interval size (dx) distribution of X-axis intervals between a certain
// fixed amount of consecutive histogram entries of the specified input histogram.
// This facility can for instance be used to investigate the distribution
// of time intervals between observed events.
//
// Input arguments :
// -----------------
// hx    : The input histogram.
// nc    : The step count to arrive at the required consecutive entry (see example below).
// dxbin : The bin size of the X-axis for the dx distribution (see also the note below).
//         dxbin=0  ==> Bin size taken the same as the input histogram "hx".
//         dxbin=-1 ==> Bin size taken to be the minimal encountered dx interval,
//                      or "fact" times the bin size of the input histogram "hx", if the latter
//                      is the larger. This provides a protection against too small bin sizes,
//                      and consequently a very large number of bins, in case an extremely small
//                      dx interval value is encountered.
//                      In case "fact<=0" and the minimal dx interval encountered was 0,
//                      the bin size of the input histogram "hx" is taken. 
//         dxbin=-2 ==> Bin size taken to be "nc" times the bin size of the input histogram "hx".
// dxmin : The lower bound of the produced histogram.
//         dxmin<0 ==> Lower bound taken to be the minimal encountered dx interval.
// dxmax : The upper bound of the produced histogram.
//         dxmax<0 ==> Upper bound taken to be the maximal encountered dx interval
//                     increased with one additional bin size to contain the maximal
//                     encountered dx interval value in the output histogram.
// mode  : 0 ==> Bin contents of the input histogram "hx" are regarded as event counts
//               (rounded to the nearest integer) with as x-value the center of the bin.
//               Multiple entries in the same bin are treated as multiple events with exactly
//               the same x-value, possibly resulting in interval values dx=0.
//               See also note 1) below.
//         1 ==> Same as "mode=0", but the x-value of each entry of the input histogram "hx" will be
//               assigned to a uniform random value within the corresponding bin. See note 1) below.  
//         2 ==> Bin contents of the input histogram "hx" are treated as (weighted) values.
//               Each filled bin is treated as a single event with as x-value the center of the bin.
//         3 ==> Same as "mode=2", but the x-value of each entry of the input histogram "hx" will be
//               assigned to a uniform random value within the corresponding bin. See also note 1) below.
// fact  : Multiplication factor applied to the bin size of the input histogram "hx" to provide
//         a lower limit to the bin size of the produced Dx histogram when "dxbin=-1". See note 1) below.
//
// Returned object : The 1-D histogram (TH1F) containing the dx distribution.
//
// Default values : dxbin=-1, dxmin=-1, dxmax=-1, mode=1 and fact=1.
//
// Note : A too coarse bin size "dxbin" may lead to binning effects which may affect
//        a statistical interpretation of the resulting histogram, for instance in the
//        light of a comparison with known background distributions.
//        In case the input histogram "hx" reflects an unbinned situation, i.e. not more
//        than one single entry per bin, then "dxbin=0" assures that also the produced
//        "dx histogram" will be free of binning effects.
//        For sparsely populated histograms, the same may hold for "dxbin=-2", which has
//        the advantage of having fewer bins which may reduce computing time when
//        performing statistical analyses (e.g. randomised combinatorics) on it.
//        It is advised to always first inspect the produced "dx histogram" before
//        performing a statistics analysis on it.
//
// Example :
// ---------
// Histogram "hx" contains the counts of event times according to a Poisson distribution.
// Specifying "nc=1" will provide the histogram of dt intervals between each consecutive event,
// i.e. the distribution of time intervals between the events (1,2), (2,3), (3,4) etc.
// Specifying "nc=2" will provide the histogram of dt intervals between every 2nd consecutive event,
// i.e. the distribution of time intervals between the events (1,3), (2,4), (3,5) etc.
// In this case of a Poissonian input histogram these produced dt distributions are known
// as the Erlang distributions (see also class NcMath), for which each time interval contains
// exactly "nc" events of which the last event occurs right at the end of the interval.
//
// Notes :
// -------
// 1) In case a certain bin of the input histogram "hx" contains several entries, all these
//    entries are regarded as separate entities with exactly the same x-value.
//    Consequently, this may result in (several) dx intervals of value 0, resulting in
//    entries at 0 in the output histogram.
//    In case one wants to avoid this effect, the binning of the input histogram "hx" could be
//    chosen fine enough to reflect a basically unbinned situation.
//    Another way to avoid this effect is to specify "mode=1", for which all entries in the
//    input histogram "hx" will be assigned a uniform random value within the corresponding bin,
//    instead of the usual center of bin value.
//    The "sensitivity" to this randomisation effect can be tuned by the input argument "fact"
//    which controls the minimal bin size of the produced Dx histogram.
//    Yet another way of avoiding this effect is to specify "mode=2" or "mode=3", which will treat
//    every filled bin of the input histogram as a single event, irrespective of the bin content.
//    These modes 2 or 3 allow the treatment of input histograms for which the bin contents do
//    not represent event counts, e.g. histograms which have been rescaled or filled with weights.
// 2) In case "dxmax<0" the actual bin size of the output histogram (slightly) differs from the
//    provided one because of the addition of one or two bin size(s) to the auto-detected dxmax
//    in order to prevent entries to fall outside the histogram range. 
//    If one wants multiple output histograms resulting from multiple invokations of this function
//    to have identical range and bin size, the user is advised to obtain the output histogram
//    parameters from the first produced histogram and pass these to the subsequent invokations.
// 3) For "mode=1" and "mode=3" a random number sequence is used, which may yield (slightly) different
//    results for multiple invokations. The start of the random number sequence may be synchronised
//    by invokation of the member function SetRandomiser().
// 4) This member function is used recursively.

 TH1F hdx;

 if (mode<0 || mode>3) return hdx;

 if (!hx) return hdx;

 if (nc<1) return hdx;
 
 Int_t nenhx=hx->GetEntries();
 if (nenhx<=nc) return hdx;

 Int_t idxbin=TMath::Nint(dxbin);
 if (idxbin<-2) return hdx;

 // Create the output histogram if all parameters have been specified or determined automatically.
 // If not, this will be done at a recursive invokation (see below) once (some of) the
 // parameters have been determined automatically from the input histogram.
 if (dxmin>=0 && dxmax>=dxmin && dxbin>0)
 {
  Int_t nbins=1;
  Double_t range=dxmax-dxmin;
  if (range>dxbin) nbins=TMath::Nint(range/dxbin);
  hdx.SetBins(nbins,dxmin,dxmax);

  // Add histogram and axes titles
  TString s;
  Double_t binwidth=hdx.GetXaxis()->GetBinWidth(1);
  s.Form("Dx interval distribution between %-i consecutive entries (nc=%-i, mode=%-i);Dx interval;Counts per bin of size %-.3g",nc+1,nc,mode,binwidth);
  hdx.SetNameTitle("DxHistogram",s);
 }

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // under identical conditions without explicit initialisation of the randomiser
 // by the user at the start of the analysis.
 if (!fRan) fRan=new NcRandom(-1);

 // Determine the minimum and maximum encountered dx or fill the output histogram
 Double_t x1,x2,deltax;
 Int_t nx1,nx2;
 Double_t deltaxmin=0;
 Double_t deltaxmax=0;
 Bool_t found=kFALSE;
 Int_t ndxcount=0;
 Int_t jstart;

 Int_t nbhx=hx->GetNbinsX();
 Double_t value=0;
 Double_t xlow=0;
 Double_t xup=0;
 Double_t bsize=0;
 for (Int_t i=1; i<=nbhx; i++)
 {
  deltax=-1;
  ndxcount=0;
  xlow=hx->GetBinLowEdge(i);
  bsize=hx->GetBinWidth(i);
  xup=xlow+bsize;
  x1=hx->GetBinCenter(i);
  if (mode==1 || mode==3) x1=fRan->Uniform(xlow,xup);
  value=hx->GetBinContent(i);
  nx1=0;
  if (value) nx1=1;
  if (mode<2) nx1=TMath::Nint(value);

  while (nx1>0)
  {
   // Check for multiple counts (left) in this bin
   jstart=i+1;
   if (nx1>1) jstart=i;

   for (Int_t j=jstart; j<=nbhx; j++)
   {
    xlow=hx->GetBinLowEdge(j);
    bsize=hx->GetBinWidth(j);
    xup=xlow+bsize;
    x2=hx->GetBinCenter(j);
    if (mode==1 || mode==3) x2=fRan->Uniform(xlow,xup);
    value=hx->GetBinContent(j);
    nx2=0;
    if (value) nx2=1;
    if (mode<2) nx2=TMath::Nint(value);

    if (j==i) nx2=nx1-1; // Counting within the same bin

    // Empty bin
    if (nx2<1) continue;

    ndxcount+=nx2;

    if (ndxcount>=nc)
    {
     deltax=fabs(x2-x1);
     if (dxmin>=0 && dxmax>=dxmin && dxbin>0) // Output histogram has been initialised
     {
      hdx.Fill(deltax);
     }
     else // Auto-determination of the output histogram range
     {
       if (!found || deltax<deltaxmin) deltaxmin=deltax;
       if (!found || deltax>deltaxmax) deltaxmax=deltax;
     }
     ndxcount=0;
     found=kTRUE;
     break;
    }
   }
   nx1--;
  }
 }

 // Check if a suitable configuration of entries was encountered
 if (!found) return hdx;

 // Check if a recursive call is needed to actually create and fill the output histogram
 Int_t nen=hdx.GetEntries();
 if (!nen)
 {
  // Set the bin size (if needed) for the output histogram
  if (!idxbin && dxbin<=0) dxbin=hx->GetBinWidth(1);
  if (idxbin==-1)
  {
   dxbin=(hx->GetBinWidth(1))*fact;
   if (deltaxmin>0 && deltaxmin>dxbin) dxbin=deltaxmin;
   if (dxbin<=0) dxbin=hx->GetBinWidth(1);
  }
  if (idxbin==-2)
  {
   dxbin=hx->GetBinWidth(1);
   dxbin=dxbin*float(nc);
  }

  // Set the auto-determined range of the output histogram
  if (dxmin<0)
  {
   dxmin=deltaxmin;
   // Compensate for randomized x-values within the input histogram bins
   if (mode==1 || mode==3)
   {
    bsize=hx->GetBinWidth(1);
    dxmin=dxmin-2.*bsize;
    if (dxmin<0) dxmin=0;
   }
  }
  if (dxmax<0)
  {
   dxmax=deltaxmax+dxbin;
   // Compensate for randomized x-values within the input histogram bins
   if (mode==1 || mode==3)
   {
    bsize=hx->GetBinWidth(1);
    dxmax=dxmax+2.*bsize;
   }
  }

  // Invoke the recursive call to create and fill the output histogram
  hdx=GetDxHistogram(hx,nc,dxbin,dxmin,dxmax,mode,fact);
 }

 return hdx;
}
///////////////////////////////////////////////////////////////////////////
TH1F NcAstrolab::GetDifHistogram(TH1* hin,Int_t mode,TString s,TF1* f) const
{
// Construct a differential dy/dx vs. x histogram from a 1D regular y vs. x input histogram.
// Such a returned histogram allows an easy fitting of differential exponential and power spectra
// in the case of unequal (or logarithmic) bin sizes.
//
// Example :
// ---------
// The input histogram represents the number of counts (N) in (variable) energy (E) bins and
// these N vs. E data are used to investigate the match with some theoretical dN/dE prediction.
// The returned histogram provides the correctly normalised dN/dE distribution which can directly
// be compared with the model prediction via the appropriate fit.
// By providing the optional function "f" as for instance TF1 f("f","pow(x,2.)")
// the returned histogram reflects the E^2 dN/dE distribution.
// 
//            
//
// Input arguments :
// -----------------
// hin  : The input histogram
// mode : 0 ==> X-axis represents the x value
//        1 ==> X-axis represents the Log10(x) value
//        2 ==> X-axis represents the Ln(x) value
// s    : Character string for the Y-axis label of the returned histogram
//        If string s is not provided the label will be constructed from the
//        labels of the input histogram.
// f    : Optional 1-D function to rescale the Y-values of the returned histogram.
//        The variable "x" of this function (if used in the function description)
//        represents the central value of each X-axis bin.
//        If function f is not provided no rescaling of the Y-values will be performed. 
//
// Returned object : The 1-D histogram (TH1F) containing dy/dx at the Y-axis.

 TH1F hout;

 if (!hin) return hout;
 
 Int_t nbins=hin->GetNbinsX();
 if (!nbins) return hout;

 // Set the X-axis parameters identical to the input histogram
 const TArrayD* xarr=hin->GetXaxis()->GetXbins();
 Int_t xsize=xarr->GetSize();
 if (!xsize)
 {
  Double_t xmin=hin->GetXaxis()->GetXmin();
  Double_t xmax=hin->GetXaxis()->GetXmax();
  hout.SetBins(nbins,xmin,xmax);
 }
 else
 {
  const Double_t* xbins=xarr->GetArray();
  hout.SetBins(nbins,xbins);
 }

 // Set histogram title
 hout.SetNameTitle("DifHistogram",hin->GetTitle());

 // Set axes titles
 TString sxin=hin->GetXaxis()->GetTitle();
 TString syin=hin->GetYaxis()->GetTitle();

 TString sxout=sxin;

 TString syout=s;
 if (syout=="")
 {
  if (f)
  {
   syout=f->GetExpFormula("p");
   syout+="*";
  }
  syout+="d(";
  syout+=syin;
  syout+=")/d(";

  // Remove Log indication from the "hin" X-axis title
  // to get a proper dy/dx label for the "hout" Y-axis
  if (mode)
  {
   sxin.ReplaceAll("^{10}log","");
   sxin.ReplaceAll("^{10}Log","");
   sxin.ReplaceAll("log10","");
   sxin.ReplaceAll("Log10","");
   sxin.ReplaceAll("log","");
   sxin.ReplaceAll("Log","");
   sxin.ReplaceAll("ln","");
   sxin.ReplaceAll("Ln","");
  }
 
  syout+=sxin;
  syout+=")";
  syout.ReplaceAll("((","(");
  syout.ReplaceAll("))",")");
 }

 hout.GetXaxis()->SetTitle(sxout.Data());
 hout.GetYaxis()->SetTitle(syout.Data());

 // Determine the new Y-values and fill the output histogram
 Double_t x=0;
 Double_t y=0;
 Double_t err=0;
 Double_t width=0;
 Double_t binlow=0;
 Double_t binup=0;
 Double_t scale=0;
 for (Int_t i=1; i<=nbins; i++)
 {
  x=hin->GetBinCenter(i);
  y=hin->GetBinContent(i);
  err=fabs(hin->GetBinError(i));
  width=hin->GetBinWidth(i);
  binlow=hin->GetBinLowEdge(i);
  binup=binlow+width;

  // Check if the binwidth is physical
  if (width<=0) continue;

  // Correct for log-scale annotation on X-axis
  if (mode==1)
  {
   x=pow(10.,x);
   width=pow(10.,binup)-pow(10.,binlow);
  }
  if (mode==2)
  {
   x=exp(x);
   width=exp(binup)-exp(binlow);
  }

  y=y/width;
  err=err/width;

  // Rescale via the function "f" if provided
  if (f)
  {
   scale=f->Eval(x);
   y=y*scale;
   err=err*scale;
  }

  hout.SetBinContent(i,y);
  hout.SetBinError(i,err);
 }

 return hout;
}
///////////////////////////////////////////////////////////////////////////
TH1F NcAstrolab::GetCountsHistogram(TF1& spec,Int_t nbins,Double_t xmin,Double_t xmax,Int_t mode,TString s) const
{
// Construct the counts (N) vs. x histogram from a 1D input function describing the differential spectrum dN/dx.
// Such a returned histogram allows an easy way to for instance obtain a primary particle distribution N(E)
// to be used as input for cosmic ray event generators based on a predicted differential power spectrum dN/dE.
//
// Input arguments :
// -----------------
// spec  : The function to describe the dN/dx spectrum
// nbins : Number of bins for the produced histogram
// xmin  : Minimal x value of the produced histogram
// xmax  : Maximum x value of the produced histogram
// mode  : 0 ==> Histogram X-axis represents the x value
//         1 ==> Histogram X-axis represents the Log10(x) value
//         2 ==> Histogram X-axis represents the Ln(x) value
// s     : Character string for the title and axes labels of the returned histogram.
//         Example : s="Energy distribution;^{10}Log(Energy) in GeV;Counts".
//         If string s is not provided the labels will be constructed from the
//         labels of the input function "spec".
//
// Returned object : The 1-D histogram (TH1F) containing the expected counts N vs. x.
//
// By default s="".

 // Setting up the output histogram
 TH1F hout;
 hout.SetName("CountsHistogram");

 // Set histogram title and axes labels
 if (s=="")
 {
  s="CountsHistogram;";
  if (mode==1)
  {
   s+="^{10}Log(";
  }
  else if (mode==2)
  {
   s+="Ln(";
  }
  s+=spec.GetXaxis()->GetTitle();
  if (mode) s+=")";
  s+=";Counts";
 }
 hout.SetTitle(s.Data());

 // Setting histogram binning 
 Double_t step=(xmax-xmin)/double(nbins);
 Double_t* xbins=new Double_t[nbins+1];
 Double_t x=xmin;
 for (Int_t ibin=0; ibin<=nbins; ibin++)
 {
  xbins[ibin]=x;
  x=x+step;
 }

 hout.SetBins(nbins,xbins);

 // Filling the output histogram
 Double_t xval=0;
 Double_t dx=0;
 Double_t N=0;
 x=xmin;
 for (Int_t ibin=1; ibin<=nbins; ibin++)
 {
  if (!mode)
  {
   xval=x;
   dx=xbins[ibin]-xbins[ibin-1];
   N=spec.Eval(x)*dx;
  }
  else if (mode==1)
  {
   xval=pow(10.,x);
   dx=pow(10.,xbins[ibin])-pow(10.,xbins[ibin-1]);
   N=spec.Eval(xval)*dx;
  }
  else if (mode==2)
  {
   xval=exp(x);
   dx=exp(xbins[ibin])-exp(xbins[ibin-1]);
   N=spec.Eval(xval)*dx;
  }
  hout.Fill(x,N);
  x=x+step;
 }

 delete [] xbins;

 return hout;
}
///////////////////////////////////////////////////////////////////////////
TH1F NcAstrolab::GetCountsHistogram(TH1& hin,Int_t mode,TString s,TF1* fscale) const
{
// Construct the counts (N) vs. x histogram from a 1D input histogram describing a differential distribution dN/dx.
// Such a returned histogram allows an easy way to for instance obtain a primary particle distribution N(E)
// to be used as input for cosmic ray event generators based on a predicted differential power spectrum dN/dE.
//
// Input arguments :
// -----------------
// hin    : The input histogram describing the fscale(x)*dN/dx distribution
// mode   : 0 ==> Histogram X-axis represents the x value
//          1 ==> Histogram X-axis represents the Log10(x) value
//          2 ==> Histogram X-axis represents the Ln(x) value
// s      : Character string for the title and axes labels of the returned histogram.
//          Example : s="Energy distribution;^{10}Log(Energy) in GeV;Counts".
//          If string s is not provided the labels will be constructed from the
//          labels of the input histogram "hin".
// fscale : (Optional) scaling function that was used for the distribution in "hin".
//          For example : In case "hin" contained E^2*dN/dE vs. E, than the user
//          should specifiy "fscale" as the function "pow(x,2)" so that this scaling
//          can be compensated for in constructing the N(E) vs. E output histogram.
//          In case "hin" contains a regular dN/dx distribution, "fscale" should not
//          be specified.
//
// Returned object : The 1-D histogram (TH1F) containing the expected counts N vs. x.
//
// The default values are s="" and fscale=0.

 // Setting up the output histogram
 TH1F hout;
 hout.SetName("CountsHistogram");
 Int_t nbins=hin.GetNbinsX();

 if (nbins<1) return hout;

 TAxis* ax=hin.GetXaxis();
 Double_t xmin=ax->GetXmin();
 Double_t xmax=ax->GetXmax();
 hout.SetBins(nbins,xmin,xmax);

 // Set histogram title and axes labels
 if (s=="")
 {
  s=hin.GetTitle();
  s+=";";
  TString tx=ax->GetTitle();
  if (tx!="")
  {
   s+=tx;
  }
  else
  {
   if (mode==0) s+="x";
   if (mode==1) s+="^{10}Log(x)";
   if (mode==2) s+="Ln(x)";
  }
  s+=";Counts";
 }
 hout.SetTitle(s);

 // Filling the output histogram
 Double_t x=0;
 Double_t xlow=0;
 Double_t xup=0;
 Double_t dx=0;
 Double_t xval=0;
 Double_t N=0;
 Double_t fval=0;
 for (Int_t ibin=1; ibin<=nbins; ibin++)
 {
  x=hin.GetBinCenter(ibin);
  xlow=hin.GetBinLowEdge(ibin);
  xup=xlow+hin.GetBinWidth(ibin);
  N=hin.GetBinContent(ibin);
  if (!mode)
  {
   xval=x;
   dx=xup-xlow;
   N=N*dx;
  }
  else if (mode==1)
  {
   xval=pow(10.,x);
   dx=pow(10.,xup)-pow(10.,xlow);
   N=N*dx;
  }
  else if (mode==2)
  {
   xval=exp(x);
   dx=exp(xup)-exp(xlow);
   N=N*dx;
  }

  // Compensate for the Y-axis scaling if needed
  if (fscale)
  {
   fval=fscale->Eval(xval);
   if (fval) N=N/fval;
  }

  hout.Fill(x,N);
 }

 return hout;
}
///////////////////////////////////////////////////////////////////////////
TH1F NcAstrolab::GetLogHistogram(TH1* hin,Int_t mode,TString s) const
{
// Construct a log10(y) or Ln(y) vs. x histogram from a 1D regular y vs. x input histogram.
// Such a returned histogram allows an easy (linear) fitting of exponential and power spectra.  
//
// Input arguments :
// -----------------
// hin  : The input histogram
// mode : 1 ==> Use Log10(y) for the Y-axis 
//        2 ==> Use Ln(y) for the Y-axis 
// s    : Character string for the Y-axis label of the returned histogram
//        If string s is not provided the label will be constructed from the
//        labels of the input histogram.
//
// Returned object : The 1-D histogram (TH1F) containing the Log10(y) or Ln(y) at the Y-axis.
//
// Note : Bins for which Log10(y) or Ln(y) can not be determined will be left empty.

 TH1F hout;

 if (!hin || mode<1 || mode>2) return hout;
 
 Int_t nbins=hin->GetNbinsX();
 if (!nbins) return hout;

 // Set the X-axis parameters identical to the input histogram
 const TArrayD* xarr=hin->GetXaxis()->GetXbins();
 Int_t xsize=xarr->GetSize();
 if (!xsize)
 {
  Double_t xmin=hin->GetXaxis()->GetXmin();
  Double_t xmax=hin->GetXaxis()->GetXmax();
  hout.SetBins(nbins,xmin,xmax);
 }
 else
 {
  const Double_t* xbins=xarr->GetArray();
  hout.SetBins(nbins,xbins);
 }

 // Set histogram and axes titles
 hout.SetNameTitle("LogHistogram",hin->GetTitle());

 if (s=="")
 {
  s="^{10}Log(";
  if (mode==2) s="Ln(";
  s+=hin->GetYaxis()->GetTitle();
  s+=")";
 }

 hout.GetXaxis()->SetTitle(hin->GetXaxis()->GetTitle());
 hout.GetYaxis()->SetTitle(s.Data());

 // Determine the new Y-values and fill the output histogram
 Double_t y=0;
 Double_t err=0;
 Double_t yplus=0;
 for (Int_t i=1; i<=nbins; i++)
 {
  y=hin->GetBinContent(i);
  err=fabs(hin->GetBinError(i));
  yplus=y+err;

  // Check if Log10(y) or Ln(y) is defined
  if (y<=0) continue;

  if (mode==1)
  {
   y=log10(y);
   yplus=log10(yplus);
  }
  else
  {
   y=log(y);
   yplus=log(yplus);
  }

  hout.SetBinContent(i,y);
  err=fabs(yplus-y);
  hout.SetBinError(i,err);
 }

 return hout;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetBackgroundRateProb(Double_t* vars,Double_t* pars)
{
// Provide the posterior Bayesian probability for a certain background rate "b"
// given some specified background (c.q. "off source") observations.
//
// *** This is an internal interface memberfunction used by GetBackgroundRatePDF() ***
//
// Details of the method can be found in the publication Astrop. Phys. 50-52 (2013) 57.
//
// The input arguments :
// ---------------------
// vars[0] (b)    : The background rate (in Hz) for which the posterior probability will be evaluated.
// pars[0] (Noff) : The number of observed background (c.q. "off source") events.
// pars[1] (Toff) : The background (c.q. "off source") exposure time in seconds.
// pars[2] (bmax) : The maximum background event rate in Hz.
//                  If bmax<0 this parameter is automatically set to 100*Noff/Toff
//                  to obtain good prior coverage as indicated below.
// pars[3] (prec) : Cut off value to limit exponential expressions to exp(|prec|) or exp(-|prec|)
//                  to prevent overflow or underflow.
//
// Default values : bmax=-1 and prec=709.
//
// The returned value is p(b|Noff,Toff,I), where "I" indicates the prior information.
//
// The prior p(b|I) for the background rate has been taken as a uniform prior within the range [0,bmax].
// The parameter "bmax" may be provided as a result from previous studies, reflecting prior information
// to be used in the Bayesian evaluation.
// If not provided (or as a negative value) the parameter "bmax" is set automatically
// to provide good prior coverage.

 Double_t b=vars[0];
 Int_t Noff=int(pars[0]);
 Double_t Toff=pars[1];
 Double_t bmax=pars[2];
 Double_t prec=pars[3];

 if (b<=0 || Noff<0 || Toff<=0) return 0;

 Double_t rNoff=Noff;
 if (bmax<0) bmax=100.*rNoff/Toff;
 
 NcMath math;

 Double_t lnU=0;
 Double_t lnD=0;
 Double_t lnprob=0;
 Double_t prob=0;

 // The ln of the numerator of Eq.(15) of the publication mentioned above
 lnU=log(Toff)+rNoff*log(b*Toff)-b*Toff;

 // The ln of the denominator of eq.(15) of the publication mentioned above
 lnD=math.LnGamma(Noff+1,bmax*Toff,1);

 lnprob=lnU-lnD;

 if (lnprob < -fabs(prec)) return 0;

 if (lnprob > fabs(prec)) lnprob=fabs(prec);
 prob=exp(lnprob);

 return prob;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetSignalRateProb(Double_t* vars,Double_t* pars)
{
// Provide the posterior Bayesian probability for a certain source signal rate "s"
// given some specified "on source" and "off source" observations.
//
// *** This is an internal interface memberfunction used by GetSignalRatePDF() ***
//
// Details of the method can be found in the publication Astrop. Phys. 50-52 (2013) 57.
// This implementation is based on the original code of Lionel Brayeur and Krijn de Vries.
//
// Note : It is essential that the provided Non and Noff data correspond to equal areas and
//        detection efficiencies.
//        See the memberfunction GetSignalRatePDF() as an example how to handle this.
//
// The input arguments :
// ---------------------
// vars[0] (s)    : The signal rate (in Hz) for which the posterior probability will be evaluated.
// pars[0] (Non)  : The total number of observed "on source" (signal+background) events.
// pars[1] (Ton)  : The "on source" exposure time in seconds.
// pars[2] (Noff) : The number of observed "off source" (background) events.
// pars[3] (Toff) : The "off source" exposure time in seconds.
// pars[4] (smax) : The maximimal signal event rate in Hz.
//                  If smax<0 this parameter is automatically set to 100*Non/Ton
//                  to obtain good prior coverage as indicated below.
// pars[5] (bmax) : The maximimal background event rate in Hz.
//                  If bmax<0 this parameter is automatically set to 100*Noff/Toff
//                  to obtain good prior coverage as indicated below.
// pars[6] (prec) : Cut off value to limit exponential expressions to exp(|prec|) or exp(-|prec|) 
//                  to prevent overflow or underflow.
//
// Default values : smax=-1, bmax=-1 and prec=709.
//
// The returned value is p(s|Non,Ton,Noff,Toff,I), where "I" indicates the prior information.
//
// The priors p(s|I) and p(b|I) for the signal and background rates, respectively, have been taken
// as uniform priors within the ranges [0,smax] and [0,bmax].
// The parameters "smax" and "bmax" may be provided as results from previous studies, reflecting
// prior information to be used in the Bayesian evaluation.
// If they are not provided (or as negative values) the parameters "smax" and "bmax" are set
// automatically to provide good prior coverage.

 Double_t s=vars[0];
 Int_t Non=TMath::Nint(pars[0]);
 Double_t Ton=pars[1];
 Int_t Noff=TMath::Nint(pars[2]);
 Double_t Toff=pars[3];
 Double_t smax=pars[4];
 Double_t bmax=pars[5];
 Double_t prec=pars[6];

 if (s<0 || Non<0 || Ton<=0 || Noff<0 || Toff<=0) return 0;

 Double_t rNon=Non;
 if (smax<0) smax=100.*rNon/Ton;
 
 Double_t rNoff=Noff;
 if (bmax<0) bmax=100.*rNoff/Toff;
 
 NcMath math;

 //Store factorials in an array to decrease the processing time
 Int_t ndim=Non+Noff+1;
 TArrayD lnfacN(ndim);

 lnfacN[0]=0;
 Double_t x=0;
 for (Int_t i=1; i<ndim; i++)
 {
  x+=log(double(i));
  lnfacN[i]=x;
 }

 Double_t lnU=0;
 Double_t lnD=0;
 Double_t sumU=0;
 Double_t sumD=0;
 Double_t prob=0;
 Double_t gammaP1=0;
 Double_t gammaP2=0;
 Double_t ri=0;

 for(Int_t i=0; i<=Non; i++)
 {
  ri=i;

  // The incomplete gamma functions P(a,x)
  gammaP1=math.Gamma(Non+Noff+1-i,bmax*(Ton+Toff),0);
  gammaP2=math.Gamma(i+1,smax*Ton,0);

  // The ln of the numerator of Eq.(21) of the publication mentioned above normalized by Non!/(Non+Noff)!
  lnU=-s*Ton+ri*log(s)+ri*log(Ton+Toff)-lnfacN[i]-lnfacN[Non-i]+lnfacN[Non+Noff-i]-lnfacN[Non+Noff]+lnfacN[Non];
   
  if ((lnU > -fabs(prec)) && (lnU < fabs(prec))) sumU+=exp(lnU)*gammaP1;
   
  //The ln of the denominator of Eq.(21) of the publication mentioned above normalized by Non!/(Non+Noff)!
  lnD=ri*log(Ton+Toff)-(ri+1.)*log(Ton)-lnfacN[i]-lnfacN[Non-i]+lnfacN[Non+Noff-i]+lnfacN[i]-lnfacN[Non+Noff]+lnfacN[Non];

  if ((lnD > -fabs(prec)) && (lnD < fabs(prec))) sumD+=exp(lnD)*gammaP1*gammaP2;
 }

 if (sumD) prob=sumU/sumD;

 return prob;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcAstrolab::GetBackgroundRatePDF(Double_t Noff,Double_t Toff,Double_t bmax,Double_t prec)
{
// Provide the posterior Bayesian PDF for a background rate "b"
// given the specified background ("off source") observations.
//
// Details of the method can be found in the publication Astrop. Phys. 50-52 (2013) 57.
// The actual code is implemented in the internal memberfunction GetBackgroundRateProb().
//
// The input arguments :
// ---------------------
// Noff : The number of observed background ("off source") events.
// Toff : The ("off source") exposure time in seconds.
// bmax : The maximal background event rate in Hz.
//        If bmax<0 this parameter is automatically set to 100*Noff/Toff
//        to obtain good prior coverage as indicated below.
// prec : Cut off value to limit exponential expressions to exp(|prec|) or exp(-|prec|)
//        to prevent overflow or underflow.
//
// Default values : bmax=-1 and prec=709.
//
// The returned PDF is p(b|Noff,Toff,I), where "I" indicates the prior information.
//
// The prior p(b|I) for the background rate has been taken as a uniform prior within the range [0,bmax].
// The parameter "bmax" may be provided as a result from previous studies, reflecting prior information
// to be used in the Bayesian evaluation.
// If not provided (or as a negative value) the parameter "bmax" is set automatically
// to provide good prior coverage.

 if (bmax<0) bmax=100.*Noff/Toff;

 Int_t npar=4;
 TF1 pdf("BkgRatePDF",this,&NcAstrolab::GetBackgroundRateProb,0,bmax,npar,"NcAstrolab","GetBackgroundRateProb");

 pdf.SetParName(0,"Noff");
 pdf.SetParName(1,"Toff");
 pdf.SetParName(2,"bmax");
 pdf.SetParName(3,"prec");

 pdf.SetParameter("Noff",Noff); 
 pdf.SetParameter("Toff",Toff); 
 pdf.SetParameter("bmax",bmax); 
 pdf.SetParameter("prec",prec); 

 pdf.SetTitle("Bayesian posterior background rate PDF;Background rate B in Hz;p(B|Noff,Toff,I)");
 pdf.SetRange(0,bmax);

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcAstrolab::GetSignalRatePDF(Double_t Non,Double_t Ton,Double_t Noff,Double_t Toff,Double_t Ra,Double_t Re,Double_t smax,Double_t bmax,Double_t prec)
{
// Provide the posterior Bayesian PDF for a source signal rate "s"
// given the specified "on source" and "off source" observations.
//
// Details of the method can be found in the publication Astrop. Phys. 50-52 (2013) 57.
// The actual code is implemented in the internal memberfunction GetSignalRateProb().
//
// The input arguments :
// ---------------------
// Non  : The total number of observed "on source" (signal+background) events.
// Ton  : The "on source" exposure time in seconds.
// Noff : The number of observed "off source" (background) events.
// Toff : The "off source" exposure time in seconds.
// Ra   : The ratio (on source area)/(off source area)
// Re   : The ratio (on source detection efficiency)/(off source detection efficiency)
// smax : The maximimal signal event rate in Hz.
//        If smax<0 this parameter is automatically set to 100*Non/Ton
//        to obtain good prior coverage as indicated below.
// bmax : The maximimal background event rate in Hz.
//        If bmax<0 this parameter is automatically set to 1000*Noff/Toff
//        to obtain good prior coverage as indicated below.
// prec : Cut off value to limit exponential expressions to exp(|prec|) or exp(-|prec|)
//        to prevent overflow or underflow.
//
// Default values : Ra=1, Re=1, smax=-1, bmax=-1 and prec=709.
//
// The returned PDF is p(s|Non,Ton,Noff,Toff,I), where "I" indicates the prior information.
//
// The priors p(s|I) and p(b|I) for the signal and background rates, respectively, have been taken
// as uniform priors within the ranges [0,smax] and [0,bmax].
// The parameters "smax" and "bmax" may be provided as results from previous studies, reflecting
// prior information to be used in the Bayesian evaluation.
// If they are not provided (or as negative values) the parameters "smax" and "bmax" are set
// automatically to provide good prior coverage.

 if (smax<0) smax=100.*Non/Ton;
 
 // Correct the off source observation for different coverage and detection efficiency
 // with respect to the actual on source measurement
 Noff=Noff*Ra*Re;

 if (bmax<0) bmax=100.*Noff/Toff;

 Int_t npar=7;
 TF1 pdf("SignalRatePDF",this,&NcAstrolab::GetSignalRateProb,0,smax,npar,"NcAstrolab","GetSignalRateProb");

 pdf.SetParName(0,"Non");
 pdf.SetParName(1,"Ton");
 pdf.SetParName(2,"Noff");
 pdf.SetParName(3,"Toff");
 pdf.SetParName(4,"smax");
 pdf.SetParName(5,"bmax");
 pdf.SetParName(6,"prec");

 pdf.SetParameter("Non",Non); 
 pdf.SetParameter("Ton",Ton); 
 pdf.SetParameter("Noff",Noff); 
 pdf.SetParameter("Toff",Toff); 
 pdf.SetParameter("smax",smax); 
 pdf.SetParameter("bmax",bmax); 
 pdf.SetParameter("prec",prec); 

 pdf.SetTitle("Bayesian posterior signal rate PDF;Signal rate S in Hz;p(S|Non,Ton,Noff,Toff,I)");
 pdf.SetRange(0,smax);

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetUpperLimit(TF1 pdf,Double_t p)
{
// Provide the Bayesian "p%" upperlimit for the specified PDF.
//
// The specified PDF does not need to be normalised.
//
// Note : The result is provided for the current range (see TF1::SetRange) of the PDF,
//        which is reflected in e.g. TF1::Draw().
//        For a good interpretation of the returned value, the user has to make sure
//        that the range covers the full PDF.
//        This range is automatically set if the PDF is the result of the
//        memberfunctions GetBackgroundRatePDF() or GetSignalRatePDF(). 
//
// In case of inconsistent data 0 is returned.  

 if (p<=0 || p>100) return 0;

 Double_t ua[2];
 Double_t xa[2];
 Int_t nu=0;
 Double_t ul=0;

 xa[0]=p/100.;
 nu=pdf.GetQuantiles(1,ua,xa);

 if (nu) ul=ua[0];

 return ul;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetUpperLimit(TH1* his,Double_t p)
{
// Provide the Bayesian "p%" upperlimit for the specified histogram.
//
// The specified histogram does not need to be normalised.
//
// In case of inconsistent data 0 is returned.  

 if (p<=0 || p>100 || !his) return 0;

 Double_t ua[2];
 Double_t xa[2];
 Int_t nu=0;
 Double_t ul=0;

 // Ensure correct results als for histograms filled via SetBinContent().
 his->ComputeIntegral();

 xa[0]=p/100.;
 nu=his->GetQuantiles(1,ua,xa);

 if (nu) ul=ua[0];

 return ul;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetCredibleInterval(TF1 pdf,Double_t p,Double_t& xlow,Double_t& xup,Int_t n)
{
// Provide lower and upper bounds of the Bayesian "p%" credible interval
// around the mode of the specified PDF.
//
// Note : The results are provided for the current range (see TF1::SetRange) of the PDF.
//        which is reflected in e.g. TF1::Draw().
//        For a good interpretation of the returned values, the user has to make sure
//        that the range covers the full PDF.
//        This range is automatically set if the PDF is the result of the
//        memberfunctions GetBackgroundRatePDF() or GetSignalRatePDF(). 
//
// Input arguments :
// -----------------
// pdf  : The PDF to be analysed.
//        This PDF does not have to be normalised.
// p    : The percentage of the PDF to be covered by the credible interval around the mode.
//        So for a Gaussian PDF, p=68.3 will result in the [mean-sigma,mean+sigma] 68.3% credible interval.
// xlow : The variable for the return of the lower bound of the credible interval. 
// xup  : The variable for the return of the upper bound of the credible interval. 
// n    : The number of intervals (i.e. quantiles) to be scanned, which determines the precision
//        of the results as explained below.
//
// The return value is the resulting fraction of the PDF covered between [xlow,xup].
// The accuracy of this resulting fraction is better than 1/n.
// By default n=1000 which implies that the accuracy of the resulting fraction is better than 0.1%.
// Note that very large values of "n" may result in a rather long computation time. 
//
// In case of inconsistent data all returned values are 0.  

 xlow=0;
 xup=0;

 if (p<=0 || p>100 || n<2) return 0;

 // Set the precision
 Double_t prec=1./double(n);

 // Obtain the n quantiles of the PDF
 Double_t* q=new Double_t[n];
 Double_t* sumq=new Double_t[n];
 Double_t sum=0;
 for (Int_t i=0; i<n; i++)
 {
  sumq[i]=sum;
  sum+=prec;
 }
 Int_t ncalc=pdf.GetQuantiles(n,q,sumq);

 // More than 1 quantile is needed
 if (ncalc<2)
 {
  delete [] q;
  delete [] sumq;
  return 0;
 }

 // Determine the index in the quantiles array q[] corresponding to
 // the X coordinate of the mode of the PDF
 Double_t xmode=pdf.GetMaximumX();
 Int_t imode=0;
 Double_t diff,diffmin;
 diffmin=fabs(q[ncalc-1]-q[0]);
 for (Int_t i=0; i<ncalc; i++)
 {
  diff=fabs(xmode-q[i]);
  if (diff<diffmin)
  {
   diffmin=diff;
   imode=i;
  }
 }

 // Get the total integral over the quantiles range of the PDF
 Double_t xmin=q[0];
 Double_t xmax=q[ncalc-1];
 Double_t totint=pdf.Integral(xmin,xmax);

 // The PDF should have a total integral >0
 if (totint<=0)
 {
  delete [] q;
  delete [] sumq;
  return 0;
 }

 // Determine the requested credible interval around the mode
 Int_t ilow=imode;
 Int_t iup=imode;
 xlow=q[ilow];
 xup=q[iup];
 Double_t ylow=pdf.Eval(q[ilow]);
 Double_t yup=pdf.Eval(q[iup]);
 Double_t frac=p/100.;
 if (frac>1) frac=1;
 Double_t credint=-1;
 while (credint<frac*totint)
 {
  if (yup>ylow && iup<(ncalc-1)) // Shift the upper bound up
  {
   iup++;
   xup=q[iup];
   yup=pdf.Eval(xup);
  }
  else if (ylow>yup && ilow>0) // Shift the lower bound down
  {
   ilow--;
   xlow=q[ilow];
   ylow=pdf.Eval(xlow);
  }
  else if (iup<(ncalc-1)) // Shift the upper bound up in case yup=ylow
  {
   iup++;
   xup=q[iup];
   yup=pdf.Eval(xup);
  }
  else if (ilow>0) // Shift the lower bound down in case yup=ylow
  {
   ilow--;
   xlow=q[ilow];
   ylow=pdf.Eval(xlow);
  }
  else // No shift in any bound -> stop
  {
   break;
  }
  credint=pdf.Integral(xlow,xup);
 }

 // Normalisation for non-normalised PDF
 Double_t intfrac=credint/totint;

 delete [] q;
 delete [] sumq;

 return intfrac;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetCredibleInterval(TF1 pdf,Double_t p,Float_t& xlow,Float_t& xup,Int_t n)
{
// Provide lower and upper bounds of the Bayesian "p%" credible interval
// around the mode of the specified PDF.
//
// Note : The results are provided for the current range (see TF1::SetRange) of the PDF.
//        which is reflected in e.g. TF1::Draw().
//        For a good interpretation of the returned values, the user has to make sure
//        that the range covers the full PDF.
//        This range is automatically set if the PDF is the result of the
//        memberfunctions GetBackgroundRatePDF() or GetSignalRatePDF(). 
//
// Input arguments :
// -----------------
// pdf  : The PDF to be analysed.
//        This PDF does not have to be normalised.
// p    : The percentage of the PDF to be covered by the credible interval around the mode.
//        So for a Gaussian PDF, p=68.3 will result in the [mean-sigma,mean+sigma] 68.3% credible interval.
// xlow : The variable for the return of the lower bound of the credible interval. 
// xup  : The variable for the return of the upper bound of the credible interval. 
// n    : The number of intervals (i.e. quantiles) to be scanned, which determines the precision
//        of the results as explained below.
//
// The return value is the resulting fraction of the PDF covered between [xlow,xup].
// The accuracy of this resulting fraction is better than 1/n.
// By default n=1000 which implies that the accuracy of the resulting fraction is better than 0.1%.
// Note that very large values of "n" may result in a rather long computation time. 
//
// In case of inconsistent data all returned values are 0.  

 Double_t xxl=0;
 Double_t xxu=0;
 Double_t val=0;

 val=GetCredibleInterval(pdf,p,xxl,xxu,n);

 xlow=xxl;
 xup=xxu;
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetCredibleInterval(TH1* his,Double_t p,Double_t& xlow,Double_t& xup)
{
// Provide lower and upper bounds of the Bayesian "p%" credible interval
// around the mode of the specified histogram.
//
// Input arguments :
// -----------------
// his  : The histogram to be analysed.
//        This hitogram does not have to be normalised.
// p    : The percentage of the total integral to be covered by the credible interval around the mode.
//        So for a Gaussian, p=68.3 will result in the [mean-sigma,mean+sigma] 68.3% credible interval.
// xlow : The variable for the return of the lower bound of the credible interval. 
// xup  : The variable for the return of the upper bound of the credible interval. 
//
// The return value is the resulting fraction of the integral covered between [xlow,xup].
//
// Note : The accuracy of the results is depending on the bin size of the histogram.
//
// In case of inconsistent data all returned values are 0.  

 xlow=0;
 xup=0;

 if (p<=0 || p>100 || !his) return 0;

 Int_t nbins=his->GetNbinsX();

 // More than 2 bins are always needed
 if (nbins<2) return 0;

 // Ensure correct results also for histograms filled via SetBinContent()
 his->ComputeIntegral();

 // Obtain the quantiles at the end of each bin of the histogram and at the start of the 1st bin
 Int_t n=nbins+1;
 Double_t* q=new Double_t[n];
 Int_t ncalc=his->GetQuantiles(n,q);

 // More than 1 quantile is needed
 if (ncalc<2)
 {
  delete [] q;
  return 0;
 }

 // Determine the index in the quantiles array q[] corresponding to
 // the X coordinate of the mode of the histogram
 Int_t imode=his->GetMaximumBin();

 // Get the total integral of the histogram over the quantiles range
 Double_t totint=his->Integral(1,ncalc,"width");

 // The histogram should have a total integral >0
 if (totint<=0)
 {
  delete [] q;
  return 0;
 }

 // Determine the requested credible interval around the mode
 Int_t ilow=imode;
 Int_t iup=imode;
 xlow=q[ilow];
 xup=q[iup];
 Double_t ylow=his->GetBinContent(ilow);
 Double_t yup=his->GetBinContent(iup);
 Double_t frac=p/100.;
 if (frac>1) frac=1;
 Double_t credint=-1;
 while (credint<frac*totint)
 {
  if (yup>ylow && iup<(ncalc-1)) // Shift the upper bound up
  {
   iup++;
   xup=q[iup];
   yup=his->GetBinContent(iup);
  }
  else if (ylow>yup && ilow>0) // Shift the lower bound down
  {
   ilow--;
   xlow=q[ilow];
   ylow=his->GetBinContent(ilow);
  }
  else if (iup<(ncalc-1)) // Shift the upper bound up in case yup=ylow
  {
   iup++;
   xup=q[iup];
   yup=his->GetBinContent(iup);
  }
  else if (ilow>0) // Shift the lower bound down in case yup=ylow
  {
   ilow--;
   xlow=q[ilow];
   ylow=his->GetBinContent(ilow);
  }
  else // No shift in any bound -> stop
  {
   break;
  }
  credint=his->Integral(ilow,iup,"width");
 }

 // Normalisation for non-normalised PDF
 Double_t intfrac=credint/totint;

 delete [] q;

 return intfrac;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetCredibleInterval(TH1* his,Double_t p,Float_t& xlow,Float_t& xup)
{
// Provide lower and upper bounds of the Bayesian "p%" credible interval
// around the mode of the specified histogram.
//
// Input arguments :
// -----------------
// his  : The histogram to be analysed.
//        This hitogram does not have to be normalised.
// p    : The percentage of the total integral to be covered by the credible interval around the mode.
//        So for a Gaussian, p=68.3 will result in the [mean-sigma,mean+sigma] 68.3% credible interval.
// xlow : The variable for the return of the lower bound of the credible interval. 
// xup  : The variable for the return of the upper bound of the credible interval. 
//
// The return value is the resulting fraction of the integral covered between [xlow,xup].
//
// Note : The accuracy of the results is depending on the bin size of the histogram.
//
// In case of inconsistent data all returned values are 0.

 Double_t xxl=0;
 Double_t xxu=0;
 Double_t val=0;

 val=GetCredibleInterval(his,p,xxl,xxu);

 xlow=xxl;
 xup=xxu;
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::KolmogorovTest(TString mode,TH1* h1,TH1* h2,TF1* pdf,Double_t nr,TH1F* ksh,Int_t ncut,Double_t* nrx,Int_t mark)
{
// Perform the Kolmogorov-Smirnov (KS) test on a 1-dimensional histogram "h1",
// e.g. an observed distribution which may contain a signal, and a reference
// distribution ("h2" or "pdf"), for instance a background (hypothesis).
// The specification of the reference distribution can be provided either in
// histogram format "h2" or via a probability distribution function "pdf" (but not both),
// as outlined below.
// In case the reference distribution is specified via a histogram "h2", this histogram
// must have the same range and binning as the histogram "h1".
// In case the reference distribution is specified via the function "pdf",
// the range etc. will be automatically selected to match the input histogram "h1" in
// order to perform the KS test.
// Note : The function "pdf" does not need to be normalised.
//
// This facility is an extension of the memberfunction KolmogorovTest of the ROOT class TH1.
// Note that in the ROOT facility the role of "h1" and "h2" is inverted.
//
// The meaning of the returned value depends on the selected testing performance,
// as specified via the input parameter "mode" (see below).
//
// In case the P-value is requested, the return value amounts to the fraction of recorded
// maximum KS distance values with dmax>=d0, based on "nr" repetitions of a KS test for which
// at each repetition "n" random entries have been drawn from the reference distribution (hypothesis).
// Each repetition is called a pseudo experiment.
// The number of random entries "n" of each pseudo experiment is the same as the number of entries of
// the input histogram "h1".
// The value d0 represents the maximum KS distance between "h1" and the reference distribution.
//
// Input arguments :
// -----------------
// mode : A string of characters to specify the required functionality of the KS test (see below)
// nr   : The number of repetitions (see note 2) of the KS test with n random background entries.
// h1   : The observed experimental distribution in histogram format.
// h2   : Reference distribution in histogram format.
// pdf  : Function describing some (hypothetical) reference distribution (see note 4).
// ksh  : Histogram with the KS dmax values obtained from the pseudo experiments (see notes 2 and 3).
// ncut : Number of dmax>=d0 values to be obtained to trigger an early stop of the pseudo experiments.
//        In case ncut=0 all the specified "nr" pseudo experiments will be performed. 
// nrx  : Returned number of actually performed pseudo experiments (see note 1).
//        (only if a non-zero "nrx" value was also provided on input)
// mark : Flag to activate (mark=1) the marking of the threshold dmax value (d0) by a vertical line in the "ksh" histogram.
//        Also the corresponding P-value will be mentioned in the legend.
//
// Via the character string "mode" the performance of the KS test is specified as follows :
//
// "U" : Include Underflows in the KS test
// "O" : Include Overflows in the KS test
// "N" : Include comparison of histogram Normalizations in addition to the shapes for the KS test probability (see note 4)
// "M" : Return the Maximum KS distance
// "K" : Return the standard KS test Probability
// "P" : Return the P-value of the maximum KS distance based on a number of pseudo experiments
// "I" : Provide output with Information of the KS test result
//
// Examples :
// ----------
// mode="UONK" returns the standard KS Probability including Underflows, Overflows and Normalizations
// mode="PI" returns the P-value of the maximum KS distance and provides relevant KS test information
// mode="MPI" returns -1 (conflicting input) 
//
// Notes :
// -------
// 1) The number of repetitions "nr" and "nrx" are of type "double" to allow for large numbers.
//    Obviously these variables are meant to represent only integer counts.
//
// 2) In case a non-zero input argument "ncut" is provided, the number of pseudo experiments will be stopped
//    as soon as "ncut" values of dmax>=d0 are obtained.
//    When a large number of repetitions "nr" was specified, this allows an "early stop" and as such
//    a significant reduction of the CPU time. In case the number "ncut" was not reached, the performance
//    of pseudo experiments will stop as soon as "nr" repetitions have been performed.
//    However, when nr=0 was specified the pseudo experiments will be repeated until the number "ncut"
//    is reached or when the number of repetitions has reached the maximum allowed value of 1e19.
//    In case a non-zero input argument "nrx" is provided the number of actually performed pseudo experiments
//    will be returned via this same argument "nrx".
//
// 3) In case a histogram "ksh" is provided, this function recreates the histogram (and a legend if "mark" is activated).
//
// 4) In case the (hypothetical) reference distribution is specified via the function "pdf", the errors
//    of the reference distribution are set to zero. This is a so called "Single sample test".
//    Consequently, it doesn't make sense to include the normalization (mode="N") in the KS-test, so in
//    these cases the mode="N" will be ignored. 
//
// For practical reasons the maximum value of "nr" has been limited to 1e19, which is about
// the corresponding maximum value of an unsigned 64-bit integer.
//
// Default values are : h2=0, pdf=0, nr=1000, ksh=0, ncut=0, nrx=0 and mark=1.
//
// In the case of inconsistent input, a value of -1 is returned.
//
//--- Nick van Eijndhoven 19-nov-2018 IIHE-VUB Brussel

 Double_t value=-1;

 if (!mode.Contains("M") && !mode.Contains("K") && !mode.Contains("P")) return -1;
 if (mode.Contains("M") && (mode.Contains("K") || mode.Contains("P"))) return -1;
 if (mode.Contains("K") && (mode.Contains("M") || mode.Contains("P"))) return -1;
 if (mode.Contains("P") && (mode.Contains("M") || mode.Contains("K"))) return -1;

 if (!h1) return -1;
 if (!h2 && !pdf) return -1;
 if (h2 && pdf) return -1;

 ULong64_t nrep=ULong64_t(nr);
 ULong64_t jrep;
 if (!nrep)
 {
  if (ncut)
  {
   nrep=ULong64_t(1.e19);
  }
  else
  {
   return -1;
  }
 }

 TAxis* xaxis=h1->GetXaxis();
 Double_t xmin1=xaxis->GetXmin();
 Double_t xmax1=xaxis->GetXmax();
 Double_t range1=xmax1-xmin1;
 Int_t nbins1=h1->GetNbinsX();
 Double_t nen1=h1->GetSumOfWeights();
 Double_t underflow1=h1->GetBinContent(0);
 Double_t overflow1=h1->GetBinContent(nbins1+1);
 if (mode.Contains("U")) nen1=nen1+underflow1;
 if (mode.Contains("O")) nen1=nen1+overflow1;

 if (nbins1<=0 || nen1<=0 || range1<=0)
 {
  cout << " *" << ClassName() << "::KolmogorovTest* Histogram h1 is empty or has inconsistent data." << endl;
  cout << " h1 : nentries=" << nen1 << " nbins=" << nbins1 << " xmin=" << xmin1 << " xmax=" << xmax1 << endl;
  return -1;
 }

 if (h2)
 {
  xaxis=h2->GetXaxis();
  Double_t xmin2=xaxis->GetXmin();
  Double_t xmax2=xaxis->GetXmax();
  Double_t range2=xmax2-xmin2;
  Int_t nbins2=h2->GetNbinsX();
  Double_t nen2=h2->GetSumOfWeights();

  if (nen2<=0 || range2<=0)
  {
   cout << " *" << ClassName() << "::KolmogorovTest* Histogram h2 is empty or has inconsistent data." << endl;
   cout << " h2 : nentries=" << nen2 << " nbins=" << nbins2 << " xmin=" << xmin2 << " xmax=" << xmax2 << endl;
   return -1;
  }
 
  Double_t prec=1e-6;
  if (nbins2!=nbins1 || fabs(xmin2-xmin1)>prec || fabs(xmax2-xmax1)>prec)
  {
   cout << " *" << ClassName() << "::KolmogorovTest* Histograms h1 and h2 do not have the same binning." << endl;
   cout << " h1 : nbins=" << nbins1 << " xmin=" << xmin1 << " xmax=" << xmax1 << endl;
   cout << " h2 : nbins=" << nbins2 << " xmin=" << xmin2 << " xmax=" << xmax2 << endl;
   return -1;
  }
 }

 // Create the "h2" histogram from the "pdf" function to perform the KS test
 if (pdf)
 {
  pdf->SetRange(xmin1,xmax1);
  pdf->SetNpx(nbins1);
  h2=(TH1*)pdf->GetHistogram()->Clone();
  h2->SetName("hpdf");
  // Set all bin errors (incl. underflow and overflow bins) to zero
  for (Int_t i=0; i<=nbins1+1; i++)
  {
   h2->SetBinError(i,0);
  }
 }

 // Convert "mode" into the corresponding character string for TH1::KolmogorovTest
 TString s="";
 if (mode.Contains("U")) s+="U";
 if (mode.Contains("O")) s+="O";
 if (mode.Contains("N") && !pdf) s+="N";

 // Obtain the maximum KS distance (d0) for the input histogram "h1"
 TString s2=s;
 s2+="M";
 Double_t d0=h2->KolmogorovTest(h1,s2.Data());

 // Complete "mode" conversion
 if (mode.Contains("M")) s+="M";
 if (mode.Contains("I")) s+="D";

 // Perform the requested KS test
 if (mode.Contains("I"))
 {
  if (pdf)
  {
   cout << " *" << ClassName() << "::KolmogorovTest* Single sample KS-test results for execution mode "<< mode.Data() << endl;
   if (mode.Contains("N")) cout << " === For a single sample KS-test the mode=N is suppressed ===" << endl;
  }
  else
  {
   cout << " *" << ClassName() << "::KolmogorovTest* Two sample KS-test results for execution mode "<< mode.Data() << endl;
  }
 }
 value=h1->KolmogorovTest(h2,s.Data());

 // Perform the pseudo experiments, if requested
 if (ksh) ksh->SetBins(101,0,1.01);
 Double_t xval=0;
 Double_t dist=0;
 Double_t sumrep=0;
 Int_t sumd=0;
 TH1* htemp=0;
 if (mode.Contains("P"))
 {
  htemp=(TH1*)h1->Clone();
  for (jrep=0; jrep<nrep; jrep++) // Loop of pseudo experiments
  {
   htemp->Reset();
   for (Int_t ien=0; ien<nen1; ien++) // Take the random entries from the reference distribution
   {
    xval=h2->GetRandom();
    htemp->Fill(xval);
   }
   dist=htemp->KolmogorovTest(h2,s2.Data());
   if (ksh) ksh->Fill(dist);
   sumrep+=1;
   if (dist>=d0) sumd++;

   // Stop the pseudo experiments if the required precision is reached
   if (ncut && sumd>=ncut) break;

  } // end loop over pseudo experiments
  value=double(sumd)/sumrep;
  if (nrx) *nrx=sumrep;
  if (mode.Contains("I"))
  {
   cout << " P-value        = " << value << " after " << sumrep << " pseudo experiments." << endl;
  }
 }

 if (mode.Contains("I")) cout << " Returned value = " << value << endl;
 
 // Complete the attributes for the "ksh" histogram
 if (ksh)
 {
  TString xlabel="Dmax";
  TString ylabel="Counts after ";
  ylabel+=sumrep;
  ylabel+=" pseudo experiments";

  ksh->SetTitle("KS-test Dmax distribution from pseudo experiments");
  ksh->SetXTitle(xlabel.Data());
  ksh->SetYTitle(ylabel.Data());

  // Mark the actually observed D0 value by a vertical line in the "ksh" histogram
  // Also the corresponding P-value is mentioned in the legend
  if (mark)
  {
   Float_t x=d0;
   Float_t ymin=0;
   Float_t ymax=ksh->GetMaximum();

   TLine* vline=new TLine(x,ymin,x,ymax);
   vline->SetLineStyle(2); // Dashed line
   vline->SetLineWidth(2);
   vline->SetLineColor(4); // Blue color

   TString title="P-value : %-10.3g";
   TString sh=title.Format(title.Data(),value);

   TLegend* leg=new TLegend(0.6,0.8,0.8,0.9);
   leg->SetFillColor(0);
   leg->SetHeader(sh.Data());
   leg->AddEntry(vline,"Observed Dmax","L");

   TList* hlist=ksh->GetListOfFunctions();
   hlist->Add(vline);
   hlist->Add(leg);
  }
 }

 // Delete temporary histograms, if any
 if (pdf && h2)
 {
  delete h2;
  h2=0;
 }

 if (htemp) delete htemp;

 return value;
}
///////////////////////////////////////////////////////////////////////////
TH1F NcAstrolab::GetCumulHistogram(TH1* h,TString name,TString mode) const
{
// Provide the Cumulative Distribution Histogram from the input 1-D histogram "h".
//
// Input arguments :
// -----------------
// h    : Input 1-D histogram from which the cumulative distribution will be determined
// name : Name for the newly created histogram which contains the cumulative distribution
// mode : "F" --> Determine the cumulative distribution in the forward sense  
//        "B" --> Determine the cumulative distribution in the backward sense  
//        "N" --> Determine the cumulative distribution normalized to 1
//
// Examples :
// ----------
// mode="BN" will provide the backward cumulative distribution, normalized to 1.
// mode="BFN" will provide an empty cumulative distribution (conflicting input).
//
// The default is mode="F".
//
// Note : Underflow and Overflow entries are not taken into account.
//
// In case of inconsistent input, an empty cumulative distribution histogram will be returned.
//
//--- Nick van Eijndhoven 26-nov-2018 IIHE-VUB Brussel

 TH1F hcd;
 TString title="Cumulative Distribution of histogram ";
 hcd.SetNameTitle(name.Data(),title.Data());

 if (!h) return hcd;

 TAxis* xaxis=h->GetXaxis();
 TAxis* yaxis=h->GetYaxis();
 Double_t xmin=xaxis->GetXmin();
 Double_t xmax=xaxis->GetXmax();
 Double_t range=xmax-xmin;
 Int_t nbins=h->GetNbinsX();
 Double_t nen=h->GetSumOfWeights();
 TString nameh=h->GetName();
 TString xtitle=xaxis->GetTitle();
 TString ytitle=yaxis->GetTitle();
 title+=nameh;
 hcd.SetNameTitle(name.Data(),title.Data());
 hcd.SetXTitle(xtitle.Data());
 hcd.SetYTitle(ytitle.Data());

 if (nbins<=0 || nen<=0 || range<=0) return hcd;

 if(!(mode.Contains("F") || mode.Contains("B")) || (mode.Contains("F") && mode.Contains("B"))) return hcd;

 hcd.SetBins(nbins,xmin,xmax);
 title="";
 if (mode.Contains("N")) title="Normalized ";
 if (mode.Contains("F")) title+="Forward ";
 if (mode.Contains("B")) title+="Backward ";
 title+="Cumulative Distribution of histogram ";
 title+=nameh;
 hcd.SetNameTitle(name.Data(),title.Data());
 hcd.SetXTitle(xtitle.Data());
 hcd.SetYTitle(ytitle.Data());

 Double_t norm=1;
 if (mode.Contains("N")) norm=nen;
 Double_t y=0;
 Double_t sum=0;

 if (mode.Contains("F")) // Forward cumulation
 {
  for (Int_t ibin=1; ibin<=nbins; ibin++)
  {
   y=h->GetBinContent(ibin);
   sum+=y/norm;
   hcd.SetBinContent(ibin,sum);
  }
 }
 else // Backward cumulation
 {
  for (Int_t ibin=nbins; ibin>=1; ibin--)
  {
   y=h->GetBinContent(ibin);
   sum+=y/norm;
   hcd.SetBinContent(ibin,sum);
  }
 }
 return hcd;
}
///////////////////////////////////////////////////////////////////////////
TH1F NcAstrolab::GetCumulHistogram(TF1* f,TString name,Int_t nbins,Double_t xmin,Double_t xmax,TString mode) const
{
// Provide the Cumulative Distribution Histogram from the input 1-D function "f".
//
// Input arguments :
// -----------------
// f     : Input 1-D function from which the cumulative distribution will be determined
// name  : Name for the newly created histogram which contains the cumulative distribution
// nbins : The number of bins for the output histogram
// xmin  : Minimum x value for the output histogram
// xmax  : Maximum x value for the output histogram
// mode  : "F" --> Determine the cumulative distribution in the forward sense  
//         "B" --> Determine the cumulative distribution in the backward sense  
//         "N" --> Determine the cumulative distribution normalized to 1
//
// Examples :
// ----------
// mode="BN" will provide the backward cumulative distribution, normalized to 1.
// mode="BFN" will provide an empty cumulative distribution (conflicting input).
//
// The default is mode="F".
//
// In case of inconsistent input, an empty cumulative distribution histogram will be returned.
//
//--- Nick van Eijndhoven 26-nov-2018 IIHE-VUB Brussel

 TH1F hcd;
 TString title="Cumulative Distribution Histogram of function ";
 hcd.SetNameTitle(name.Data(),title.Data());

 if (!f) return hcd;

 // The original range of the function "f"
 Double_t xminold=f->GetXmin();
 Double_t xmaxold=f->GetXmax();

 f->SetRange(xmin,xmax);
 f->SetNpx(nbins);
 TH1* hf=(TH1*)f->GetHistogram();

 hcd=GetCumulHistogram(hf,name,mode);

 if (hcd.GetEntries()>0) // Histogram has been filled
 {
  title="";
  if (mode.Contains("N")) title="Normalized ";
  if (mode.Contains("F")) title+="Forward ";
  if (mode.Contains("B")) title+="Backward ";
  title+="Cumulative Distribution Histogram of function ";
 }

 TString namef=f->GetName();
 title+=namef;

 hcd.SetTitle(title.Data());

 // Restore the original range for the function "f"
 f->SetRange(xminold,xmaxold);

 return hcd;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::InitDataNames(Int_t dir,TString frame,TString mode)
{
// Initialisation of the correspondence table between physical observables
// and their names in a ROOT input Tree.
// The actual correspondence settings are performed by invokation of SetDataNames().
// 
// Input arguments :
// -----------------
// dir   = >0 ==> Coordinates indicate the arrival direction
//         <0 ==> Coordinates indicate the direction of motion
//
// frame = "equ" ==> Equatorial coordinates with right ascension and declination.
//         "gal" ==> Galactic coordinates with longitude and latitude.
//         "ecl" ==> Geocentric ecliptic coordinates with longitude and latitude.
//         "hor" ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle and zenith angle.
//         "icr" ==> ICRS coordinates with longitude and latitude.
//         "loc" ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta and phi.
//
// mode  = "M" ==> Equatorial coordinates are the mean values 
//         "T" ==> Equatorial coordinates are the true values
//         "B" ==> Besselian (B1950) reference epoch equatorial coordinates 
//         "J" ==> Julian (J2000) reference epoch equatorial coordinates 
//
// The default value is mode="J".

 if (!dir || (frame!="equ" && frame!="gal" && frame!="ecl" && frame!="hor" && frame!="icr" && frame!="loc") ||
     (mode!="M" && mode!="T" && mode!="B" && mode!="J"))
 {
  cout << endl;
  cout << " *" << ClassName() << "::InitDataNames* Invalid input encountered." << endl;
  cout << " dir=" << dir << " frame=" << frame << " mode=" << mode << endl;
  return;
 }

 fDataDir=dir;
 fDataFrame=frame;
 fDataMode=mode;
 
 // Reset the correspondence table
 fDataNames.Reset();

 TString sdir="arrival";
 if (dir<0) sdir="moving";
 
 if (frame=="equ")
 {
  if (mode=="M") frame="mean";
  if (mode=="T") frame="true";
  if (mode=="B") frame="B1950";
  if (mode=="J") frame="J2000";
  frame+=" equatorial";
 }
 if (frame=="gal") frame="galactic";
 if (frame=="ecl") frame="ecliptic";
 if (frame=="hor") frame="horizontal";
 if (frame=="icr") frame="ICRS";
 if (frame=="loc") frame="local";

 cout << endl;
 cout << " *" << ClassName() << "::InitDataNames* Prepared for input of " << sdir << " directions in " << frame << " coordinates." << endl; 
 cout << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetDataNames(TString obsname,TString varname,TString units,TString func)
{
// Specification of the correspondence table between pre-defined (physical) observables
// and their names in a ROOT input Tree.
// In order to be compatible with the generic data reading member function LoadInputData(),
// only the pre-defined (physical) observable names listed below are accepted.
//
// Note :
// ------
// Make sure to invoke InitDataNames() first, before providing the various settings here.
//
// Input arguments :
// -----------------
// obsname : Name of the (physical) observable (see below)
// varname : Name of the corresponding variable in the ROOT Tree
// units   : The units in which the input value is provided (see below)
//           This includes scaling w.r.t. standard units like GeV, pc, second etc.
//           Example : units="1e3" would represent TeV instead of the standard GeV in case of an energy.
// func    : Function that has been applied on the value of the observable (see below).
//
// Accepted (physical) observable names are :
// ------------------------------------------
// Name      : Name of the object or observation (e.g. GRB031025A or IC190722A)
// Run       : Run number
// Event     : Event number
// Eventb    : Sub-event number
// VetoLevel : Veto level indicator
// DetId     : Detector identifier
// Date      : The UTC observation date with as possible units "ddmmyyyy", "yyyymmdd", "mmddyyyy" or "yyyyddmm"
// Tobs      : The UTC observation c.q. trigger timestamp with as possible units "JD", "MJD", "TJD", "hms" or "hrs"
// Tstart    : Generic UTC start timestamp with as possible units "JD", "MJD", "TJD", "hms" or "hrs"
// Tend      : Generic UTC end timestamp with as possible units "JD", "MJD", "TJD", "hms" or "hrs"
// d         : Distance with a number as units (Standard unit is pc)
// a         : right ascension, longitude, azimuth angle or spherical angle theta (depending on the reference frame)
// b         : declination, latitude, zenith angle or spherical angle phi (depending on the reference frame)
// z         : Redshift
// E         : Energy with a number as units (Standard unit is GeV)
// L         : Luminosity with a number as units (Standard unit is erg s^-1. [For particle counts : s^-1])
// S         : Fluence with a number as units (Standard unit is erg cm^-2. [For particle counts : cm^-2])
// F         : Flux with a number as units (Standard unit is erg cm^-2 s^-1 [For particle counts : cm^-2 s^-1])
// I         : Intensity with a number as units (Standard unit is erg cm^-2 s^-1 sr^-1 [For particle counts : cm^-2 s^-1 sr^-1])
// J         : Flux density with a number as units (Standard unit is Jy, i.e. 1e-23 erg cm^-2 s^-1 Hz^-1)
// T90       : T90 burst duration with a number as units (Standard unit is second)
// T100      : T100 burst duration with a number as units (Standard unit is second)
// dsigma    : The on d
// csigma    : The angular uncertainty in a cone around the position or direction
// zsigma    : The uncertainty on z
// Esigma    : The uncertainty on E
// Lsigma    : The uncertainty on L
// Ssigma    : The uncertainty on S
// Fsigma    : The uncertainty on F
// Isigma    : The uncertainty on I
// T90sigma  : The uncertainty on T90 
// T100sigma : The uncertainty on T90 
//
// Note : In case Tobs, Tstart or Tend are specified in "hms" or "hrs", also Date has to be provided.
//
// Accepted specifications for units are :
// ---------------------------------------
// "1"   : Standard units are used like GeV, pc, second (or days in case of Julian Dates)
//         Any other numerical value will be used for unit conversion (see below for examples)
// "rad" : Angle provided in radians
// "deg" : Angle provided in degrees
// "dms" : Angle provided in dddmmss.sss
// "hms" : Angle or time provided in hhmmss.sss
// "hrs" : Angle or time provided in fractional hours
// "JD"  : Timestamp provided as Julian Date
// "MJD" : Timestamp provided as Modified Julian Date
// "TJD" : Timestamp provided as Truncated Julian Date
//
// Accepted specifications for func are :
// --------------------------------------
// "Log" : Log10 of the value of the corresponding observable
// "Ln"  : Natural logarithm of the value of the corresponding observable
//
// Input examples :
// ----------------
// ("E","Enu","1","Log")      : The ROOT Tree variable "Enu" represents Log10(energy) in GeV.
// ("Date","date","yyyymmdd") : The ROOT Tree variable "date" represents the date as yyyymmdd.
// ("Tstart","time","hms")    : The ROOT Tree variable "time" represents a timestamp as hhmmss.sss.
// ("d","dist","1e6")         : The ROOT Tree variable "dist" represents the distance in Mpc.
// ("Tobs","trig","MJD")      : The ROOT Tree variable "trig" represents the observation time in MJD.
//
// The defaults are units="1" and func="none".

 Bool_t error=kFALSE;

 if (obsname!="Name" && obsname!="Run" && obsname!="Event" && obsname!="Eventb" && obsname!="VetoLevel"
     && obsname!="DetId" && obsname!="Date" && obsname!="Tobs" && obsname!="Tstart" && obsname!="Tend"
     && obsname!="d" && obsname!="a" && obsname!="b" && obsname!="z"
     && obsname!="E" && obsname!="L" && obsname!="S" && obsname!="F" && obsname!="I" && obsname!="J"
     && obsname!="T90" && obsname!="T100"
     && obsname!="dsigma" && obsname!="csigma" && obsname!="zsigma"
     && obsname!="Esigma" && obsname!="Lsigma" && obsname!="Ssigma" && obsname!="Fsigma" && obsname!="Isigma"
     && obsname!="T90sigma" && obsname!="T100sigma") error=kTRUE;
 if (obsname=="Date" && (units!="ddmmyyyy" && units!="yyyymmdd" && units!="mmddyyyy" && units!="yyyyddmm")) error=kTRUE;
 if ((obsname=="Tobs" || obsname=="Tstart" || obsname=="Tend")
     && (units!="JD" && units!="MJD" && units!="TJD" && units!="hms" && units!="hrs")) error=kTRUE;
 if ((obsname=="a" || obsname=="b" || obsname=="csigma")
     && (units!="rad" && units!="deg" && units!="dms" && units!="hms" && units!="hrs")) error=kTRUE;
 if (func!="none" && func!="Log" && func!="Ln") error=kTRUE;

 if (error)
 {
  cout << " *" << ClassName() << "::SetDataNames* Invalid input encountered." << endl;
  cout << " obsname=" << obsname << " units=" << units << " func=" << func << endl;
  return;
 }

 Int_t n=fDataNames.GetMaxRow();
 TObjString* obs=new TObjString(obsname);
 TObjString* var=new TObjString(varname);
 TObjString* u=new TObjString(units);
 TObjString* f=new TObjString(func);
 TObjString* val=new TObjString(""); // Value will be filled in LoadInputData()

 fDataNames.EnterObject(n+1,1,obs);
 fDataNames.EnterObject(n+1,2,var);
 fDataNames.EnterObject(n+1,3,u);
 fDataNames.EnterObject(n+1,4,f);
 fDataNames.EnterObject(n+1,5,val);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ListDataNames()
{
// Listing of the correspondance table between physical observables
// and their names in a ROOT input Tree.

 TString sdir="undefined";
 if (fDataDir>0) sdir="arrival";
 if (fDataDir<0) sdir="moving";

 TString frame="undefined";
 if (fDataFrame=="equ")
 {
  if (fDataMode=="M") frame="mean";
  if (fDataMode=="T") frame="true";
  if (fDataMode=="B") frame="B1950";
  if (fDataMode=="J") frame="J2000";
  frame+=" equatorial";
 }
 if (fDataFrame=="gal") frame="galactic";
 if (fDataFrame=="ecl") frame="ecliptic";
 if (fDataFrame=="hor") frame="horizontal";
 if (fDataFrame=="icr") frame="ICRS";
 if (fDataFrame=="loc") frame="local";

 cout << endl;
 cout << " *" << ClassName() << "::ListDataNames* Settings for input of " << sdir << " directions in " << frame << " coordinates." << endl; 

 Int_t n=fDataNames.GetMaxRow();

 if (n<1)
 {
  cout << " *** No settings were specified ***" << endl;
 }
 else
 {
  cout << " *** The following " << n << " settings were specified ***" << endl;
 }

 TObjString* obs=0;
 TObjString* var=0;
 TObjString* u=0;
 TObjString* f=0;
 for (Int_t i=1; i<=n; i++)
 {
  obs=(TObjString*)fDataNames.GetObject(i,1);
  var=(TObjString*)fDataNames.GetObject(i,2);
  u=(TObjString*)fDataNames.GetObject(i,3);
  f=(TObjString*)fDataNames.GetObject(i,4);
  cout << " obsname=" << obs->GetString() << " varname=" << var->GetString() << " units=" << u->GetString() << " func=" << f->GetString() << endl;
 }
 cout << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetBurstParameter(TString name,Double_t value)
{
// Specification of a certain transient burst parameter setting.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Input arguments :
// -----------------
// name  : Name of the parameter to be set
// value : The parameter value to be set
//
// Default values will be initialized by the constructor of this class
// and can be displayed by invokation of the member function ListBurstParameters(). 
//
// To reset all parameters to their default values please invoke with name="*".
// This will also remove all the stored histograms related to burst investigations.
// If name="*" the provided value is irrelevant.
//
// The available parameter names are :
//
// Nmaxsrc;  // Maximal number of sources to be accepted for analysis (<0 : no limitation)
// Nmaxevt;  // Maximal number of observed events to be accepted for analysis (<0 : no limitation)
// RAmin     // Minimal right ascension (J2000 in degrees) for source c.q. event position acceptance
// RAmax     // Maximal right ascension (J2000 in degrees) for source c.q. event position acceptance
// Declmin   // Minimal declination (J2000 in degrees) for source c.q. event position acceptance
// Declmax   // Maximal declination (J2000 in degrees) for source c.q. event position acceptance
// T90min    // Minimal duration (t90 in sec) for burst acceptance (<0 : [|T90min|,T90max] with random T90 when missing in loaded data)
// T90max    // Maximal duration (t90 in sec) for burst acceptance
// Zmin      // Minimal redshift for source acceptance (<0 : [|Zmin|,Zmax] with random z when missing in loaded data)
// Zmax      // Maximal redshift for source acceptance
// Sigmamin  // Minimal position uncertainty (sigma in degrees) for source acceptance (<0 : [|Sigmamin|,Sigmamax] with random sigma when missing in loaded data)
// Sigmamax  // Maximal position uncertainty (sigma in degrees) for source acceptance
// Grbnu     // Maximum number of detectable simulated neutrinos per source (<0 : no statistical fluctuations)
// Avgrbz    // Average source redshift (<0 : determine from observations)
// Avgrbt90  // Average burst duration (T90) in seconds (<0 : determine from observations)
// Inburst   // Flag to indicate that the simulated neutrinos are produced coupled (1) or not (0) to the burst duration
// Dtnu      // Mean time diff. (in sec) between gamma/GW and simulated nu production (decoupled) or in T90 units w.r.t. trigger (coupled)
// Dtnus     // Sigma of time difference (in sec) between gamma/GW and simulated nu production (<0 is in T90 units)
// ESigmin   // Minimal event energy (in GeV) for source signal event generation
// ESigmax   // Maximal event energy (in GeV) for source signal event generation
// Ezcor     // Flag to activate redshift correction for the generated energy of simulated source signal events (1) or not (0)
// Emin      // Minimal event energy (in GeV) for event acceptance
// Emax      // Maximal event energy (in GeV) for event acceptance
// Alphasig  // Spectral index for a default dN/dE=E^-Alphasig source induced signal energy spectrum within [ESigmin,ESigmax]. See also MakeBurstEnergydist() 
// Alphabkg  // Spectral index for a default dN/dE=E^-Alphabkg background energy spectrum within [Emin,Emax]. See also MakeBurstEnergydist()
// Kinangle  // Neutrino-lepton kinematic opening angle selection for CC interactions (0=none 1=mean 2=median 3=draw from pdf)
// Angresmin // Minimal event reco angular resolution (sigma in degrees) for event acceptance
// Angresmax // Maximal event reco angular resolution (sigma in degrees) for event acceptance
// Angresfix // Fixed event reco angular resolution within [Angresmin,Angresmax] (also used when no distribution (value) is available)
// Recoangle // Reconstruction angular uncertainty selection (0=use Angresfix value  1=mean 2=median 3=draw from distribution)
// Sumsigmas // Sigma (summation) selection (-1=reco sigma only  0=source sigma only  1=linear summation  2=quadratic summation)
// Timres    // Neutrino detector time resolution (sec)
// Sensarea  // Area (in m^2) covered c.q. overlooked by the neutrino detector sensors (<=0 means not specified) 
// Bkgrate   // Mean rate (in Hz) of background events for the specified [Declmin,Declmax] interval (<0 : rate per steradian)
// Nbkg      // The number of requested background patches per source
// Tunits    // Time units for Tmin, Tmax, Tbin and VarTbin (0=days 1=hours 2=seconds 3=nanoseconds 4=picoseconds) 
// Tmin      // Lower bound of the search time window [Tmin,Tmax] where t=0 indicates the burst trigger
// Tmax      // Upper bound of the search time window [Tmin,Tmax] where t=0 indicates the burst trigger
//           Note : If Tmax<=Tmin the search time window is unrestricted (not applicable for simulation data) 
// Dawin     // Angular search circle (<0 is local zenith band) in degrees or units of sigma around (above/below) the source position (see Datype)
// Datype    // Type of Dawin (<0=Unrestricted 0=degrees 1=units of combined max. source/event sigma 2=units of combined actual source/event sigma) 
// Tbint90   // Flag to indicate whether Tbin represents time units (0) or units of average T90 (1)
// Tbin      // Time bin size (>0=time units or units of average T90  0=variable bins  <0 will result in a mean |Tbin| counts/bin)
// VarTbin   // Size (in time units) of the first time bin in case of variable time bins
// Abin      // Angular bin size in degrees (<0 will result in a mean |Abin| counts/bin)
//
// Note :
// ------
// In case variable time bins are selected, the search time window [Tmin,Tmax] will be centered
// around the burst trigger time (i.e. t=0) and the first bin will have a size of VarTbin time units.
// The size of subsequent time bins is multiplied by the average redshift factor (z+1) at each step.
// This is done in order to acount for the cosmological time dilation, which stretches all time differences.
//
// Default settings (tailored for IceCube 86 strings) :
// ----------------------------------------------------
// Nmaxsrc=-1
// Nmaxevt=-1
// [RAmin,RAmax]=[0,360]
// [Declmin,Declmax]=[-90,90]
// [T90min,T90max]=[1e-5,1e5]
// [Zmin,Zmax]=[-1e-6,20]
// [Sigmamin,Sigmamax]=[0,2]
// Grbnu=0;
// Avgrbz=-1
// Avgrbt90=-1
// Inburst=0
// Dtnu=0
// Dtnus=-0.5
// [ESigmin,ESigmax]=[1e3,1e10]
// Ezcor=1
// [Emin,Emax]=[200,1e7]
// Alphasig=2
// Alphabkg=3.5
// Kinangle=3
// [Angresmin,Angresmax]=[0,2]
// Angresfix=1
// Recoangle=3
// Sumsigmas=2
// Timres=1e-5
// Sensarea=1e6
// Bkgrate=-0.003/(2.*pi)
// Nbkg=1
// Tunits=2
// Tmin=-3600
// Tmax=3600
// Dawin=5
// Datype=0
// Tbint90=1
// Tbin=1
// VarTbin=10
// Abin=1

 if (!fBurstParameters)
 {
  fBurstParameters=new NcDevice();
  fBurstParameters->SetNameTitle("BurstParameters","Parameter settings for transient burst investigations");
 }

 if (name!="*")
 {
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(value,name);
 }
 else
 {
  Double_t pi=acos(-1.);
  name="Nmaxsrc";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-1,name);
  name="Nmaxevt";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-1,name);
  name="RAmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0,name);
  name="RAmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(360,name);
  name="Declmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-90,name);
  name="Declmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(90,name);
  name="T90min";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e-5,name);
  name="T90max";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e5,name);
  name="Zmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-1e-6,name);
  name="Zmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(20,name);
  name="Sigmamin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0,name);
  name="Sigmamax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(2,name);
  name="Grbnu";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0,name);
  name="Avgrbz";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-1,name);
  name="Avgrbt90";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-1,name);
  name="Inburst";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0,name);
  name="Dtnu";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0,name);
  name="Dtnus";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-0.5,name);
  name="ESigmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e3,name);
  name="ESigmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e10,name);
  name="Ezcor";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1,name);
  name="Emin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(200,name);
  name="Emax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e7,name);
  name="Alphasig";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(2,name);
  name="Alphabkg";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(3.5,name);
  name="Kinangle";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(3,name);
  name="Angresmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0,name);
  name="Angresmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(2,name);
  name="Angresfix";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1,name);
  name="Recoangle";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(3,name);
  name="Sumsigmas";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(2,name);
  name="Timres";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e-5,name);
  name="Sensarea";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e6,name);
  name="Bkgrate";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-0.003/(2.*pi),name);
  name="Nbkg";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1,name);
  name="Tunits";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(2,name);
  name="Tmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-3600,name);
  name="Tmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(3600,name);
  name="Dawin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(5,name);
  name="Datype";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0,name);
  name="Tbint90";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1,name);
  name="Tbin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1,name);
  name="VarTbin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(10,name);
  name="Abin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1,name);

  // Remove all histograms related to burst investigations
  fBurstHistos.Clear();
  fBurstHistos.SetOwner();
 }

 ///////////////////////////////////
 // Store some derived parameters //
 ///////////////////////////////////

 // The correction factor to get from Tunits to seconds
 Int_t fTunits=TMath::Nint(fBurstParameters->GetSignal("Tunits"));
 Double_t Tfact=1;
 if (fTunits==0) Tfact=86400;
 if (fTunits==1) Tfact=3600;
 if (fTunits==3) Tfact=1e-9;
 if (fTunits==4) Tfact=1e-12;
 name="Tfact";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(Tfact,name);

 // Combined burst position and event reconstruction angular uncertainty interval (sigma in degrees)
 Int_t fSumsigmas=TMath::Nint(fBurstParameters->GetSignal("Sumsigmas"));
 Int_t fRecoangle=TMath::Nint(fBurstParameters->GetSignal("Recoangle"));
 Float_t fSigmamin=fabs(fBurstParameters->GetSignal("Sigmamin"));
 Float_t fAngresmin=fBurstParameters->GetSignal("Angresmin");
 Float_t fAngresfix=fBurstParameters->GetSignal("Angresfix");
 if (!fRecoangle) fAngresmin=fAngresfix;
 Float_t Minsigmatot=-1;
 if (fSumsigmas==-1) Minsigmatot=fAngresmin;
 if (fSumsigmas==0) Minsigmatot=fSigmamin;
 if (fSumsigmas==1) Minsigmatot=fSigmamin+fAngresmin;
 if (fSumsigmas==2) Minsigmatot= sqrt(fSigmamin*fSigmamin+fAngresmin*fAngresmin);
 name="Minsigmatot";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(Minsigmatot,name);
 Float_t fSigmamax=fBurstParameters->GetSignal("Sigmamax");
 Float_t fAngresmax=fBurstParameters->GetSignal("Angresmax");
 if (!fRecoangle) fAngresmax=fAngresfix;
 Float_t Maxsigmatot=-1;
 if (fSumsigmas==-1) Maxsigmatot=fAngresmax;
 if (fSumsigmas==0) Maxsigmatot=fSigmamax;
 if (fSumsigmas==1) Maxsigmatot=fSigmamax+fAngresmax;
 if (fSumsigmas==2) Maxsigmatot= sqrt(fSigmamax*fSigmamax+fAngresmax*fAngresmax);
 name="Maxsigmatot";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(Maxsigmatot,name);

 // The total search time window in seconds
 Float_t fTmin=fBurstParameters->GetSignal("Tmin");
 Float_t fTmax=fBurstParameters->GetSignal("Tmax");
 Float_t Dtwin=fTmax-fTmin;
 name="Dtwin";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(Dtwin,name);

 Float_t fTbin=fBurstParameters->GetSignal("Tbin");
 if (!fTbin) // Center the time window around the burst trigger for variable time bins
 {
  fTmin=-Dtwin/2.;
  fTmax=Dtwin/2.;
  name="Tmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(fTmin,name);
  name="Tmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(fTmax,name);
 }

 // The solid angle corresponding to the selected declination band
 Float_t fRAmin=fBurstParameters->GetSignal("RAmin");
 Float_t fRAmax=fBurstParameters->GetSignal("RAmax");
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t phimin=fRAmin;
 Float_t phimax=fRAmax;
 Float_t thmin=90.-fDeclmax;
 Float_t thmax=90.-fDeclmin;
 Float_t OmegaDecl=GetSolidAngle(thmin,thmax,"deg",phimin,phimax,"deg");
 name="OmegaDecl";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(OmegaDecl,name);

 // Background event rate from the selected declination band
 Float_t fBkgrate=fBurstParameters->GetSignal("Bkgrate");
 Float_t RbkgDecl=fBkgrate;
 if (fBkgrate<0)
 {
  RbkgDecl=fabs(fBkgrate)*OmegaDecl;
 }
 name="RbkgDecl";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(RbkgDecl,name);

 // Mean number of background events per hour from the selected declination band
 Float_t NbkgHour=RbkgDecl*3600.;
 name="NbkgHour";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(NbkgHour,name);

 // Mean number of background events in the search time window from the selected declination band
 Float_t fDtwin=fBurstParameters->GetSignal("Dtwin");
 Float_t NbkgWin=RbkgDecl*fDtwin*Tfact;
 name="NbkgWin";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(NbkgWin,name);
}
///////////////////////////////////////////////////////////////////////////
NcDevice* NcAstrolab::GetBurstParameters()
{
// Provide a link to the device containing all transient burst parameter settings.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 return fBurstParameters;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ListBurstParameters() const
{
// Listing of all the burst parameter settings
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 // User provided settings
 Int_t fNmaxsrc=TMath::Nint(fBurstParameters->GetSignal("Nmaxsrc"));
 Int_t fNmaxevt=TMath::Nint(fBurstParameters->GetSignal("Nmaxevt"));
 Float_t fRAmin=fBurstParameters->GetSignal("RAmin");
 Float_t fRAmax=fBurstParameters->GetSignal("RAmax");
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t fT90min=fBurstParameters->GetSignal("T90min");
 Float_t fT90max=fBurstParameters->GetSignal("T90max");
 Float_t fZmin=fBurstParameters->GetSignal("Zmin");
 Float_t fZmax=fBurstParameters->GetSignal("Zmax");
 Float_t fSigmamin=fBurstParameters->GetSignal("Sigmamin");
 Float_t fSigmamax=fBurstParameters->GetSignal("Sigmamax");
 Float_t fGrbnu=fBurstParameters->GetSignal("Grbnu");
 Float_t fAvgrbz=fBurstParameters->GetSignal("Avgrbz");
 Float_t fAvgrbt90=fBurstParameters->GetSignal("Avgrbt90");
 Float_t fAvgrbsigma=fBurstParameters->GetSignal("Avgrbsigma");
 Int_t fInburst=TMath::Nint(fBurstParameters->GetSignal("Inburst"));
 Float_t fDtnu=fBurstParameters->GetSignal("Dtnu");
 Float_t fDtnus=fBurstParameters->GetSignal("Dtnus");
 Float_t fESigmin=fBurstParameters->GetSignal("ESigmin");
 Float_t fESigmax=fBurstParameters->GetSignal("ESigmax");
 Int_t fEzcor=TMath::Nint(fBurstParameters->GetSignal("Ezcor"));
 Float_t fEmin=fBurstParameters->GetSignal("Emin");
 Float_t fEmax=fBurstParameters->GetSignal("Emax");
 Float_t fAlphasig=fBurstParameters->GetSignal("Alphasig");
 Float_t fAlphabkg=fBurstParameters->GetSignal("Alphabkg");
 Int_t fKinangle=TMath::Nint(fBurstParameters->GetSignal("Kinangle"));
 Float_t fAngresmin=fBurstParameters->GetSignal("Angresmin");
 Float_t fAngresmax=fBurstParameters->GetSignal("Angresmax");
 Float_t fAngresfix=fBurstParameters->GetSignal("Angresfix");
 Int_t fRecoangle=TMath::Nint(fBurstParameters->GetSignal("Recoangle"));
 Int_t fSumsigmas=TMath::Nint(fBurstParameters->GetSignal("Sumsigmas"));
 Float_t fTimres=fBurstParameters->GetSignal("Timres");
 Float_t fSensarea=fBurstParameters->GetSignal("Sensarea");
 Float_t fBkgrate=fBurstParameters->GetSignal("Bkgrate");
 Int_t fNbkg=TMath::Nint(fBurstParameters->GetSignal("Nbkg"));
 Float_t fTmin=fBurstParameters->GetSignal("Tmin");
 Float_t fTmax=fBurstParameters->GetSignal("Tmax");
 Int_t fTunits=TMath::Nint(fBurstParameters->GetSignal("Tunits"));
 Float_t fDawin=fBurstParameters->GetSignal("Dawin");
 Int_t fDatype=TMath::Nint(fBurstParameters->GetSignal("Datype"));
 Int_t fTbint90=TMath::Nint(fBurstParameters->GetSignal("Tbint90"));
 Float_t fTbin=fBurstParameters->GetSignal("Tbin");
 Float_t fVarTbin=fBurstParameters->GetSignal("VarTbin");
 Float_t fAbin=fBurstParameters->GetSignal("Abin");

 // Derived parameters
 Float_t fMaxsigmatot=fBurstParameters->GetSignal("Maxsigmatot");
 Float_t fMinsigmatot=fBurstParameters->GetSignal("Minsigmatot");
 Float_t fOmegaDecl=fBurstParameters->GetSignal("OmegaDecl");
 Float_t fRbkgDecl=fBurstParameters->GetSignal("RbkgDecl");
 Float_t fNbkgHour=fBurstParameters->GetSignal("NbkgHour");
 Float_t fNbkgWin=fBurstParameters->GetSignal("NbkgWin");

 // Internal statistics
 Int_t fNgrbs=TMath::Nint(fBurstParameters->GetSignal("Ngrbs"));
 Int_t fNevts=TMath::Nint(fBurstParameters->GetSignal("Nevts"));

 TString tu="days";
 if (fTunits==1) tu="hours";
 if (fTunits==2) tu="sec";
 if (fTunits==3) tu="ns";
 if (fTunits==4) tu="ps";

 cout << " ========================= User provided source c.q. burst settings ===============================" << endl;
 if (fNmaxsrc<0)
 {
  printf(" No limitation has been put on the number of sources to be accepted for analysis. \n");
 }
 else
 {
  printf(" Maximal number of sources to be accepted for analysis : %-i \n",fNmaxsrc);
 }
 if (fNmaxevt<0)
 {
  printf(" No limitation has been put on the number of observed events to be accepted for analysis. \n");
 }
 else
 {
  printf(" Maximal number of observed events to be accepted for analysis : %-i \n",fNmaxevt);
 }
 printf(" Right ascension interval (J2000 in degrees) for source c.q. event position acceptance : [%-g,%-g] \n",fRAmin,fRAmax);
 printf(" Declination interval (J2000 in degrees) for source c.q. event position acceptance : [%-g,%-g] \n",fDeclmin,fDeclmax);
 printf(" Redshift interval for source acceptance : [%-g,%-g] \n",fabs(fZmin),fZmax);
 if (fZmin<0) printf(" Random redshift values taken from z-distribution in case of unknown redshift \n");
 if (fAvgrbz>=0) printf(" User defined average source redshift : %-g \n",fAvgrbz);
 printf(" Event energy interval (in GeV) for event acceptance : [%-g,%-g] \n",fEmin,fEmax);
 printf(" Position uncertainty interval (sigma in degrees) for source acceptance : [%-g,%-g] \n",fabs(fSigmamin),fSigmamax);
 if (fSigmamin<0) printf(" Random sigma values taken from sigma-distribution when missing in loaded data \n");
 printf(" Event angular resolution interval (sigma in degrees) for event acceptance : [%-g,%-g] \n",fAngresmin,fAngresmax);
 if (fDatype>0) printf(" Sigma (combination) selection (-1=event sigma only  0=source sigma only  1=linear summation  2=quadratic summation) : %-i \n",fSumsigmas);
 if (fDawin>=0)
 {
  if (fDatype<0) printf(" Unrestricted angular search area. \n");
  if (!fDatype) printf(" Fixed angular search circle around the source position : %-g degrees \n",fDawin);
  if (fDatype==1) printf(" Fixed angular search circle (in combined max. source/event sigma) around the source position : %-g \n",fDawin);
  if (fDatype==2) printf(" Variable angular search circle (in combined actual source/event sigma) around the source position : %-g \n",fDawin);
 }
 else
 {
  if (fDatype<0) printf(" Unrestricted angular search area. \n");
  if (!fDatype) printf(" Fixed angular local zenith band above/below the source position : %-g degrees \n",fabs(fDawin));
  if (fDatype==1) printf(" Fixed angular local zenith band (in combined max. source/event sigma) above/below the source position : %-g \n",fabs(fDawin));
  if (fDatype==2) printf(" Variable angular local zenith band (in combined actual source/event sigma) above/below the source position : %-g \n",fabs(fDawin));
 }
 printf(" Number of requested background patches per source : %-i \n",fNbkg);
 printf(" Duration interval (t90 in sec) for burst acceptance : [%-g,%-g] \n",fabs(fT90min),fT90max);
 if (fT90min<0) printf(" Random values taken from T90-distribution in case T90 and T100 were missing \n");
 if (fAvgrbt90>=0) printf(" User defined average burst T90 duration : %-g sec. \n",fAvgrbt90);
 if (fTmax>fTmin)
 {
  printf(" Total search time window (in %-s) with the burst trigger at t=0 : [%-g,%-g] \n",tu.Data(),fTmin,fTmax);
 }
 else
 {
  printf(" [Tmin,Tmax]=[%-g,%-g] --> Unrestricted search time window (in %-s) with the burst trigger at t=0 \n",fTmin,fTmax,tu.Data());
 }
 if (fTbin<0) printf(" Automatic time histogram binning with as mean number of bkg counts/bin : %-g \n",fabs(fTbin));
 if (!fTbin) printf(" Variable time histogram binning with as size (in %-s) for the first time : %-g \n",tu.Data(),fVarTbin);
 if (fTbin>0)
 {
  if (fTbint90)
  {
   printf(" Time histogram bin size in average T90 units : %-g",fTbin);
   if (fAvgrbt90>0 || fNgrbs>0) printf(" (=%-g sec)",fTbin*fabs(fAvgrbt90));
   printf("\n");
  }
  else
  {
   printf( " Time histogram bin size : %-g %-s \n",fTbin,tu.Data());
  }
 }
 if (fAbin<0)
 {
  printf(" Automatic angular histogram binning with as mean number of bkg counts per bin : %-g \n",fabs(fAbin));
 }
 else
 {
  printf(" Angular histogram bin size : %-g degrees \n",fAbin);
 }

 // Parameters for burst signal and background generation
 if (fGrbnu)
 {
  if (fGrbnu<0)
  {
   printf(" Number of generated neutrinos per burst : %-g without statistical fluctuations \n",fabs(fGrbnu));
  }
  else
  {
   printf(" Maximum number of generated neutrinos per burst : %-g \n",fGrbnu);
   printf(" The actual number of neutrinos may be less due to statistical fluctuations \n");
  }
  printf(" Energy interval (in GeV) for signal event generation at the source : [%-g,%-g] \n",fESigmin,fESigmax);
  if (fEzcor)
  {
   printf(" The signal neutrino energy at Earth will be corrected for redshift \n"); 
  }
  else
  {
   printf(" No redshift correction will be applied on the generated signal neutrino energy \n"); 
  }
  if (!fInburst)
  {
   printf(" Neutrino production is assumed to be NOT coupled to the observed burst duration \n");
   printf(" Mean decoupled time difference between burst gammas/GW and neutrinos : %-g sec. \n",fDtnu);
  }
  else
  {
   printf(" Neutrino production is assumed to be coupled to the observed burst duration \n");
   printf(" Mean coupled time difference (in units of T90 w.r.t. trigger) between burst gammas/GW and neutrinos : %-g",fDtnu);
  }
  if (fDtnus>=0)
  {
   printf(" Sigma of mean time difference between burst gammas/GW and neutrinos : %-g sec. \n",fDtnus);
  }
  else
  {
   printf(" Sigma of mean time difference (in units of T90) between burst gammas/GW and neutrinos : %-g \n",fabs(fDtnus));
  }
  printf(" Default spectral index for a dN/dE=E^-alpha source induced signal energy spectrum within [%-g,%-g] GeV : %-g \n",fEmin,fEmax,fAlphasig);
  printf(" --- The user may have changed the spectrum by invokation of the memberfunction MakeBurstEnergydist(). \n");
  printf(" Default spectral index for a dN/dE=E^-alpha background energy spectrum within [%-g,%-g] GeV : %-g \n",fEmin,fEmax,fAlphabkg);
  printf(" --- The user may have changed the spectrum by invokation of the memberfunction MakeBurstEnergydist(). \n");
  printf(" Fixed event reco angular resolution (sigma in degrees), also used when no distribution (value) is available : %-g \n",fAngresfix);
  printf(" Event reconstruction angular uncertainty selection (0=use fixed value  1=mean 2=median 3=draw from distribution) : %-i \n",fRecoangle);
  printf(" Neutrino-lepton kinematic opening angle selection for CC interactions (0=none 1=mean 2=median 3=draw from pdf) : %-i \n",fKinangle);
 }

 printf("\n");
 printf(" Time resolution of the neutrino detector : %-g sec. \n",fTimres);
 printf(" Area covered c.q. overlooked by the neutrino detector : %-g m^2 \n",fSensarea);
 printf(" Mean rate of background events for the specified declination interval (<0 : rate per steradian) : %-g Hz \n",fBkgrate);

 printf("\n");
 printf(" ============================== Derived parameters ==================================== \n");
 printf(" Combined source position and event reco angular uncertainty interval (sigma in degrees) : [%-g,%-g] \n",fMinsigmatot,fMaxsigmatot);
 printf(" Solid angle coverage corresponding to the selected declination band : %-g steradian \n",fOmegaDecl);
 printf(" Background event rate for the selected declination band : %-g Hz \n",fRbkgDecl);
 printf(" Mean number of background events per hour from the selected declination band : %-g \n",fNbkgHour);
 printf(" Mean number of background events in the time window from the selected declination band : %-g \n",fNbkgWin);
 if (fNgrbs>0)
 {
  printf(" Number of bursts accepted for analysis : %-i \n",fNgrbs);
  printf(" Median source redshift from the data sample : %-g \n",fabs(fAvgrbz));
  printf(" Median burst T90 duration from the data sample : %-g sec. \n",fabs(fAvgrbt90));
  printf(" Median souce position uncertainty (sigma in degrees) from the data sample : %-g \n",fAvgrbsigma);
 }
 if (fNevts>0) printf(" Number of observed events accepted for analysis : %-i \n",fNevts);
 printf(" ====================================================================================== \n");
 printf("\n");
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::LoadInputData(Bool_t src,TString file,TString tree,Int_t date1,Int_t date2,Int_t nmax,TString type)
{
// Generic facility to load source (e.g. GCN) or observed event data.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The input data has to be provided as numerical values via a ROOT Tree which will be
// searched for data on the variable names as specified via SetDataNames().
//
// The necessary minimal set of pre-defined observables (see SetDataNames) is :
// (a,b,Tobs) [and Date in case Tobs is in units "hms" or "hrs"].
// The observables (a,b,Tobs) are used to automatically construct and store the signal entries,
// as outlined in SetSignal().
//
// All requested values will (also) be stored in NcSignal slots with the pre-defined
// observable name and in the standard units as defined in SetDataNames().
//
// Special standard units used for storage are :
// 1) yyyymmdd for Date
// 2) MJD for Tobs, Tstart and Tend.
// 3) Degrees for all angles
//
// Note that the values of "a" and "b" are not stored as NcSignal slots, since their
// meaning depends on the used reference frame.
// Retrieval of the values of "a" and "b" should be performed via the GetSignal() facility.
//
// In case data for a requested observable is not present, the non-physical value -999
// will be stored, unless the user has selected random generation of that observable
// via the specifications in SetBurstParameter(). 
//
// Input arguments :
// -----------------
// src   : Flag to indicate source (e.g. GCN) data (kTRUE) or observed events (kFALSE)
// file  : Name of the input file containing the ROOT Tree (wildcards are allowed)
// tree  : Name of the Tree containing the data
// date1 : The date (yyyymmdd) of the start of the observation period [date1,date2] (0=No restriction)
// date2 : The date (yyyymmdd) of the end of the observation period [date1,date2] (0=No restriction)
// nmax  : Maximum number of sources or events to be accepted from this input file (<0 : no limitation)
// type  : Identifier of the source (c.q. burst) or observed event (alert) type (e.g. "GRB", "GW", "IC",...)
//         In case no "Name" is provided as an "observable" in the input data, the provided "type" will
//         be combined with the Tobs data to construct names like GRB031025, IC190722 etc...
//         However, in case "Name" is provided in the input data, the corresponding string will be
//         used as the name for the source (c.q. burst) or observed event, irrespective of the provided "type". 
//         When type="-" the source type is set to "GRB" and the event type is set to "EVT", in case
//         no "Name" is provided as "observable" in the input data.
//
// The default values are date1=0, date2=0, nmax=-1 and type="-".
//
// Notes :
// -------
// 1) This memberfunction make be invoked several times to read different files to accumulate data.
// 2) The internal storage arrays will be compressed, so that the indices of previously stored objects may change.

 // Set the default type identifier
 if (type=="-" && src) type="GRB";
 if (type=="-" && !src) type="EVT";

 Int_t nvars=fDataNames.GetMaxRow();

 if (nvars<1)
 {
  cout << " *" << ClassName() << "::LoadInputData* No variables were specified for " << type << " data."<< endl;
  return;
 }

 // Set the data mode : observed events or source data
 Int_t iobs=1;
 if (src) iobs=0;

 // Retreive the needed parameters
 Int_t fNmaxsrc=TMath::Nint(fBurstParameters->GetSignal("Nmaxsrc"));
 Int_t fNmaxevt=TMath::Nint(fBurstParameters->GetSignal("Nmaxevt"));
 Float_t fRAmin=fBurstParameters->GetSignal("RAmin");
 Float_t fRAmax=fBurstParameters->GetSignal("RAmax");
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t fT90min=fBurstParameters->GetSignal("T90min");
 Float_t fT90max=fBurstParameters->GetSignal("T90max");
 Float_t fZmin=fBurstParameters->GetSignal("Zmin");
 Float_t fZmax=fBurstParameters->GetSignal("Zmax");
 Float_t fSigmamin=fBurstParameters->GetSignal("Sigmamin");
 Float_t fSigmamax=fBurstParameters->GetSignal("Sigmamax");
 Float_t fEmin=fBurstParameters->GetSignal("Emin");
 Float_t fEmax=fBurstParameters->GetSignal("Emax");
 Float_t fAngresmin=fBurstParameters->GetSignal("Angresmin");
 Float_t fAngresmax=fBurstParameters->GetSignal("Angresmax");

 // Internal statistics
 Int_t fNgrbs=GetNsignals(0);
 Int_t fNevts=GetNsignals(1);

 // Get access to a redshift distribution to draw randomly source redshifts if needed
 TH1* zdist=0;
 if (src && fZmin<0) zdist=GetBurstZdist("LoadInputData()",type);

 // Get access to a T90 distribution to draw randomly source c.q. burst T90 values if needed
 TH1* t90dist=0;
 if (src && fT90min<0) t90dist=GetBurstT90dist("LoadInputData()",type);

 // Get access to a 1-sigma position uncertainty distribution to draw randomly source position uncertaintes
 TH1* sigmaposdist=0;
 if (src && fSigmamin<0) sigmaposdist=GetBurstSigmaPosdist("LoadInputData()",type);

 // The TTree containing the source (c.q. burst) or observed event data
 TChain data(tree.Data());
 data.Add(file.Data());

 // The pre-defined (physical) observables
 TString obsname;
 TString varname;
 TString units;
 TString func;

 // The (physical) observable value in string format
 TObjString* pval=0;
 TString val;

 // The retrieved numerical (physical) observable value from the ROOT Tree
 Double_t value=0;

 // The conversion factor depending on the units specification 
 Double_t fact=0;

 // Some of the pre-defined observable values that are used for selections
 // or that need special treatment
 TString Name,Date,Tobs,Tstart,Tend;
 Double_t d,a,b;
 Float_t z,csigma,T90,T100,E;

 UInt_t yyyy,mm,dd; // The date format
 Int_t h,m;  // The integer hour and minute time format
 Double_t s; // The (fractional) seconds time format
 Int_t dmode=0;
 Int_t idate=0;
 Int_t jdate=0;
 TString grbname;
 NcTimestamp tobs;
 NcTimestamp tstart;
 NcTimestamp tend;
 NcSignal* sx=0;
 Int_t nnew=0;
 TLeaf* lx=0;
 TLeafC* lxc=0;
 Int_t jlast=0;

 // Create or increase the corresponding storage array to hold the new data
 Int_t nent=data.GetEntries();
 Int_t size=0;
 if (src) // Input represents reference objects
 {
  if (!fRefs)
  {
   fRefs=new TObjArray(nent);
   fRefs->SetOwner();
  }
  else
  {
   size=fRefs->GetSize();
   fRefs->Expand(size+nent);
  }
 }
 else // Input represents measured signals
 {
  if (!fSigs)
  {
   fSigs=new TObjArray(nent);
   fSigs->SetOwner();
  }
  else
  {
   size=fSigs->GetSize();
   fSigs->Expand(size+nent);
  }
 }

 // Loop over the data entries in the input Tree
 for (Int_t ient=0; ient<nent; ient++)
 {
  if (nmax>=0 && nnew>=nmax) break;
  if (src && fNmaxsrc>=0 && (fNgrbs+nnew)>=fNmaxsrc) break;
  if (!src && fNmaxevt>=0 && (fNevts+nnew)>=fNmaxevt) break;

  data.GetEntry(ient);

  // Initialisation of the values that are used for selections
  // or that need special treatment
  Name="none";
  Date="none";
  Tobs="none";
  Tstart="none";
  Tend="none";
  d=-999;
  a=-999;
  b=-999;
  z=-999;
  csigma=-999;
  T90=-999;
  T100=-999;
  E=-999;

  // Loop over the selected input variables and retrieve the corresponding input value
  for (Int_t ivar=1; ivar<=nvars; ivar++)
  {
   obsname=((TObjString*)fDataNames.GetObject(ivar,1))->GetString();
   varname=((TObjString*)fDataNames.GetObject(ivar,2))->GetString();
   units=((TObjString*)fDataNames.GetObject(ivar,3))->GetString();
   func=((TObjString*)fDataNames.GetObject(ivar,4))->GetString();

   pval=(TObjString*)fDataNames.GetObject(ivar,5);

   if (!pval) continue;

   lx=data.GetLeaf(varname);
   
   // Record -999 for missing data
   if (!lx)
   {
    pval->SetString("-999");
    continue;
   }

   if (obsname=="Name") // Character string data from the input Tree
   {
    value=0;
    lxc=(TLeafC*)lx;
    Name=lxc->GetValueString();
   }
   else // Numerical data from the input Tree
   {
    value=lx->GetValue();
   }

   if (func=="Log") value=pow(10,value);
   if (func=="Ln") value=exp(value);

   // Convert all angular values to degrees
   if (obsname=="a" || obsname=="b" || obsname=="csigma") value=ConvertAngle(value,units,"deg");

   // Convert numerical values to the standard units
   if (units.IsFloat())
   {
    fact=units.Atof();
    value*=fact;
   }

   // Store the obtained value in string format
   val="";
   val+=value;
   pval->SetString(val);

   // Special values needed for later selections
   if (obsname=="Date")
   {
    Date="";
    Date+=int(value);
    if (units=="ddmmyyyy") dmode=0;
    if (units=="yyyymmdd") dmode=1;
    if (units=="mmddyyyy") dmode=2;
    if (units=="yyyyddmm") dmode=3;
   }
   if (obsname=="Tobs")
   {
    Tobs="set"; // Indicate that Tobs is encountered
    if (units=="JD") tobs.SetJD(value);
    if (units=="MJD") tobs.SetMJD(value);
    if (units=="TJD") tobs.SetTJD(value);
    if (units=="hms")
    {
     Tobs="";
     Tobs+=value;
    }
    if (units=="hrs") // Convert "hrs" to "hms" time format
    {
     Tobs="";
     Convert(value,h,m,s);
     value=s+double(100*m+10000*h);
     Tobs+=value;
    }
   }
   if (obsname=="Tstart")
   {
    Tstart="set"; // Indicate that Tstart is encountered
    if (units=="JD") tstart.SetJD(value);
    if (units=="MJD") tstart.SetMJD(value);
    if (units=="TJD") tstart.SetTJD(value);
    if (units=="hms")
    {
     Tstart="";
     Tstart+=value;
    }
    if (units=="hrs") // Convert "hrs" to "hms" time format
    {
     Tstart="";
     Convert(value,h,m,s);
     value=s+double(100*m+10000*h);
     Tstart+=value;
    }
   }
   if (obsname=="Tend")
   {
    Tend="set"; // Indicate that Tend is encountered
    if (units=="JD") tend.SetJD(value);
    if (units=="MJD") tend.SetMJD(value);
    if (units=="TJD") tend.SetTJD(value);
    if (units=="hms")
    {
     Tend="";
     Tend+=value;
    }
    if (units=="hrs") // Convert "hrs" to "hms" time format
    {
     Tend="";
     Convert(value,h,m,s);
     value=s+double(100*m+10000*h);
     Tend+=value;
    }
   }

   if (obsname=="d") d=value;  
   if (obsname=="a") a=value;  
   if (obsname=="b") b=value;  
   if (obsname=="z") z=value;  
   if (obsname=="csigma") csigma=value;  
   if (obsname=="T90") T90=value;  
   if (obsname=="T100") T100=value;
   if (obsname=="E") E=value;  
  } // End of the loop over the selected input variables

  // For angular coordinates the distance may be irrelevant
  if (d<=0) d=1;

  // Check for the presence of valid location data
  if (a<-900 || b<-900) continue;

  // Check on (RA,DEC) acceptance in case of J2000 equatorial coordinates
  if (fDataFrame=="equ" && fDataMode=="J" && (a<fRAmin || a>fRAmax || b<fDeclmin || b>fDeclmax)) continue;

  // Construct the various timestamps from the (date,time) specification if needed
  if (Tobs!="none" && Tobs!="set" && Date!="none") tobs.SetUT(Date,Tobs,dmode);  
  if (Tstart!="none" && Tstart!="set" && Date!="none") tstart.SetUT(Date,Tstart,dmode);  
  if (Tend!="none" && Tend!="set" && Date!="none") tend.SetUT(Date,Tend,dmode);

  // Check for the presence of a valid observation c.q. trigger timestamp
  if (Tobs=="none" || (Tobs!="set" && Date=="none")) continue;

  // Obtain the date in yyyymmdd format
  tobs.GetDate(kTRUE,0,&yyyy,&mm,&dd);
  idate=dd+100*mm+10000*yyyy;

  if (Name!="none") // Set the name of the object or observation
  {
   grbname=Name;
  }
  else // Compose the name of the source c.q. burst with the common yymmdd suffix
  {
   jdate=idate%1000000;
   grbname=type;
   if (jdate<100000) grbname+="0"; // Add leading zero for the year if needed
   grbname+=jdate;
  }

  if (date1 && idate<date1) continue;
  if (date2 && idate>date2) continue;

  if (src) // Source c.q. burst specific selections
  {
   if (T90<=0) T90=T100;
   if (fT90min<0 && T90<0 && t90dist) T90=t90dist->GetRandom();

   if (T90<fabs(fT90min) || T90>fT90max) continue;

   if (fZmin<0 && z<0 && zdist) z=zdist->GetRandom();

   if (z<fabs(fZmin) || z>fZmax) continue;

   if (fSigmamin<0 && csigma<0 && sigmaposdist) csigma=sigmaposdist->GetRandom();

   if (csigma<fabs(fSigmamin) || csigma>fSigmamax) continue;
  }
  else // Observed event specific selections
  {
   if (E<fEmin || E>fEmax) continue;
   if (csigma<fAngresmin || csigma>fAngresmax) continue;
  }

  // Store the location c.q. arrival direction data
  sx=SetSignal(d,a,"deg",b,"deg",fDataFrame,&tobs,-1,fDataMode,grbname,iobs);

  // Obtain the RA and DEC coordinates for acceptance selection if input was not in J2000
  if (fDataFrame!="equ" || fDataMode!="J")
  {
   jlast=GetSignalIndex(sx,iobs);
   GetSignal(d,a,"deg",b,"deg","equ",&tobs,jlast,"J",iobs);

   // Remove the signal again when it falls outside the acceptance
   if (a<fRAmin || a>fRAmax || b<fDeclmin || b>fDeclmax)
   {
    RemoveSignal(jlast,iobs,0);
    sx=0;
    continue;
   }
  }

  if (!sx) continue;

  nnew++;

  // Storing the requested data in NcSignal slots
  for (Int_t ivar=1; ivar<=nvars; ivar++)
  {
   obsname=((TObjString*)fDataNames.GetObject(ivar,1))->GetString();
   val=((TObjString*)fDataNames.GetObject(ivar,5))->GetString();

   // The observable Name is not stored in the NcSignal slots
   if (obsname=="Name") continue;

   value=val.Atof();

   // The values of the observables a and b depend on the reference frame
   // They should be retrieved via the GetSignal() facility
   if (obsname=="a" || obsname=="b") continue;

   // The Date and timestamps in specific format
   if (obsname=="Date") value=idate;
   if (obsname=="Tobs") value=tobs.GetMJD();
   if (obsname=="Tstart") value=tstart.GetMJD();
   if (obsname=="TEnd") value=tend.GetMJD();

   // Values that may have got random values
   if (obsname=="z") value=z;
   if (obsname=="csigma") value=csigma;
   if (obsname=="T90") value=T90;

   sx->AddNamedSlot(obsname);
   sx->SetSignal(value,obsname);
  }
 } // End of loop over the entries of the input Tree

 // Compress the storage array size to the number of actually stored elements
 RemoveSignal(-1,iobs,1);

 Int_t nstored=GetNsignals(iobs);

 cout << endl;
 if (src)
 {
  // Update internal statistics
  fBurstParameters->AddNamedSlot("Ngrbs");
  fBurstParameters->SetSignal(nstored,"Ngrbs");
  cout << " *" << ClassName() << "::LoadInputData* " << nnew << " new source(s) of type " << type
       << " stored from Tree:" << tree << " of file(s):" << file << endl;
  cout << " Total number of stored sources c.q. bursts : " << nstored << endl;
 }
 else
 {
  // Update internal statistics
  fBurstParameters->AddNamedSlot("Nevts");
  fBurstParameters->SetSignal(nstored,"Nevts");
  cout << " *" << ClassName() << "::LoadInputData* " << nnew
       << " new observed event(s) were stored from Tree:" << tree << " of file(s):" << file << endl;
  cout << " Total number of stored events : " << nstored << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GenBurstGCNdata(Int_t n,TString name,Bool_t scale)
{
// Generate fictative burst GCN data for "n" bursts of (alert) type "name",
// where "name" can be "GRB", "GW", "IC", ....
// Data will be generated according to the parameters as specified
// by the user via prior invokation of the memberfunction SetBurstParameter().
//
// Note :
// ------
// The input parameter "scale" indicates whether "n" represents the actual number
// of bursts in the user defined (RA,DEC) acceptance (scale=kFALSE) or the full sky
// number of bursts (scale=kTRUE).
//
// The default values are name="GRB" and scale=kFALSE.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 // Retreive the needed parameters
 Int_t fNmaxsrc=TMath::Nint(fBurstParameters->GetSignal("Nmaxsrc"));
 Float_t fRAmin=fBurstParameters->GetSignal("RAmin");
 Float_t fRAmax=fBurstParameters->GetSignal("RAmax");
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t fT90min=fBurstParameters->GetSignal("T90min");
 Float_t fT90max=fBurstParameters->GetSignal("T90max");
 Float_t fZmin=fBurstParameters->GetSignal("Zmin");
 Float_t fZmax=fBurstParameters->GetSignal("Zmax");
 Float_t fSigmamin=fBurstParameters->GetSignal("Sigmamin");
 Float_t fSigmamax=fBurstParameters->GetSignal("Sigmamax");
 Float_t fOmegaDecl=fBurstParameters->GetSignal("OmegaDecl");

 if (scale)
 {
  Double_t pi=acos(-1.);
  n=TMath::Nint(double(n)*fOmegaDecl/(4.*pi));
 }

 // Internal statistics
 Int_t fNgrbs=GetNsignals(0);

 // Get access to a redshift distribution to draw randomly redshifts
 TH1* zdist=GetBurstZdist("GenBurstGCNdata()",name);

 // Get access to a T90 distribution to draw randomly T90 values
 TH1* t90dist=GetBurstT90dist("GenBurstGCNdata()",name);

 // Get access to a 1-sigma position uncertainty distribution to draw randomly position uncertaintes
 TH1* sigmaposdist=GetBurstSigmaPosdist("GenBurstGCNdata()",name);

 if (!zdist || !t90dist || !sigmaposdist)
 {
  cout << endl;
  cout << " *" << ClassName() << "::GenBurstGCNdata* A distribution for random values is missing." << endl; 
  cout << endl;
  return;
 }

 Float_t thlow=90.-fDeclmax; // Lower theta angle in overall Earth spherical coordinates (North Pole is theta=0)
 Float_t thup=90.-fDeclmin;  // Upper theta angle in overall Earth spherical coordinates (North Pole is theta=0)
 Float_t phimin=fRAmin;      // Minimal phi angle in overall Earth spherical coordinates
 Float_t phimax=fRAmax;      // Maximal phi angle in overall Earth spherical coordinates
 if (thlow<0) thlow=0;
 if (thup>180) thup=180;

 NcSignal* sx=0;
 NcPosition rgrb;
 Float_t t90grb=0;
 Float_t zgrb=0;
 Float_t sigmagrb=0;
 TString grbname;
 Double_t thetagrb,phigrb,ragrb,decgrb;
 Int_t ngen=0;

 for (Int_t igrb=1; igrb<=n; igrb++)
 {
  if (fNmaxsrc>=0 && (fNgrbs+ngen)>=fNmaxsrc) break;

  zgrb=-1;
  if (fabs(fZmin)==fZmax) zgrb=fZmax; 
  while (zgrb<fabs(fZmin) || zgrb>fZmax)
  {
   zgrb=zdist->GetRandom();
  }

  t90grb=-1;
  if (fabs(fT90min)==fT90max) t90grb=fT90max; 
  while (t90grb<fabs(fT90min) || t90grb>fT90max)
  {
   t90grb=t90dist->GetRandom();
   t90grb=pow(float(10),t90grb);
  }
   
  sigmagrb=-1;
  if (fabs(fSigmamin)==fSigmamax) sigmagrb=fSigmamax; 
  while (sigmagrb<fabs(fSigmamin) || sigmagrb>fSigmamax)
  {
   sigmagrb=sigmaposdist->GetRandom();
  }

  rgrb.SetPosition(1,0,0,"sph","deg");
  RandomPosition(rgrb,thlow,thup,phimin,phimax);
  thetagrb=rgrb.GetX(2,"sph","deg");
  phigrb=rgrb.GetX(3,"sph","deg");

  grbname="Random-";
  grbname+=name;
  grbname+=igrb;
  grbname+="#";
  ragrb=phigrb;
  decgrb=90.-thetagrb;
  sx=SetSignal(1,ragrb,"deg",decgrb,"deg","equ",0,-1,"J",grbname);

  if (!sx) continue;

  ngen++;

  sx->AddNamedSlot("T90");
  sx->SetSignal(t90grb,"T90");
  sx->AddNamedSlot("csigma");
  sx->SetSignal(sigmagrb,"csigma");
  sx->AddNamedSlot("z");
  sx->SetSignal(zgrb,"z");

 }

 // Update internal statistics
 fNgrbs=GetNsignals(0);
 fBurstParameters->AddNamedSlot("Ngrbs");
 fBurstParameters->SetSignal(fNgrbs,"Ngrbs");

 cout << endl;
 cout << " *" << ClassName() << "::GenBurstGCNdata* " << ngen << " new generated bursts with name " << name << " were stored." << endl;
 cout << " Total number of stored bursts : " << fNgrbs << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstZdist(TString file,TString tree,TString name,Int_t nb,Float_t zmin,Float_t zmax)
{
// Read observed archival redshift data and create the corresponding distribution.
// Also the corresponding distribution of the Proper Distance at the time of
// observation (called the Physical Distance) will be created.
// If this memberfunction is invoked before LoadBurstGCNdata() or GenBurstGCNdata(),
// the plain observed redshift distribution will be used to draw random z values
// (if requested) for the bursts without redshift information.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The input data has to be provided via a ROOT Tree which contains at least
// the specified variable name indicated below.
//
// Input arguments :
// -----------------
// file   : Name of the input file(s) containing the ROOT Tree (wildcards are allowed)
// tree   : Name of the Tree containing the data
// name   : Name of the variable in the Tree containing the redshift data
// nb     : Number of bins for the z-distribution
// zmin   : Minimal z-value
// zmax   : Maximal z-value
//
// The default values are nb=200, zmin=0 and zmax=20.
//
// Note : This memberfunction may be invoked several times to read different files
//        to accumulate data.

 // The Tree containing the archival data
 TChain data(tree.Data());
 data.Add(file.Data());

 Int_t nen=data.GetEntries();
 TLeaf* lx=data.FindLeaf(name.Data());

 if (!nen || !lx)
 {
  cout << " *" << ClassName() << "::MakeBurstZdist* Missing information for tree variable:" << name << endl;
  cout << " of Tree:" << tree << " with " << nen <<  " entries in file:" << file << endl;
  return;
 }

 // Create new distributions in case a redshift distribution is not yet present
 TH1* zdist=(TH1*)fBurstHistos.FindObject("hz");
 if (!zdist)
 {
  // Creation of the archival burst redshift histogram
  TH1F* hz=new TH1F("hz","Archival data of observed burst redshifts",nb,zmin,zmax);
  fBurstHistos.Add(hz);
  hz->GetXaxis()->SetTitle("Burst redshift");
  hz->GetYaxis()->SetTitle("Counts");

  // Creation of the corresponding physical distance histo
  Float_t dmin=GetPhysicalDistance(zmin);
  Float_t dmax=GetPhysicalDistance(zmax);
  TH1F* hd=new TH1F("hd","Burst distances derived from the archival redshift data",nb,dmin,dmax);
  fBurstHistos.Add(hd);
  hd->GetXaxis()->SetTitle("Burst physical distance in Mpc");
  hd->GetYaxis()->SetTitle("Counts");
 }

 // Get pointers to the relevant histograms 
 TH1* hz=(TH1*)fBurstHistos.FindObject("hz");
 TH1* hd=(TH1*)fBurstHistos.FindObject("hd");

 Int_t nz=0;
 Double_t z=0;
 Double_t d=0;
 for (Int_t ien=0; ien<nen; ien++)
 {
  data.GetEntry(ien);

  lx=data.GetLeaf(name.Data());
  if (!lx) continue;

  z=lx->GetValue();
  if (z<zmin || z>zmax) continue;

  hz->Fill(z);
  nz++;

  d=GetPhysicalDistance(z);
  hd->Fill(d);
 }

 cout << " *" << ClassName() << "::MakeBurstZdist* " << nz << " archival z-values have been obtained from tree variable:" << name
      << " of Tree:" << tree << " in file(s):" << file << endl;
}
///////////////////////////////////////////////////////////////////////////
TH1* NcAstrolab::GetBurstZdist(TString name,TString type)
{
// Internal member function to provide the archival c.q. fitted burst redshift distribution.
// The input argument "name" serves to identify the calling function in the printout.
// The input argument "type" serves to identify the source class for which a fit has to be provided.
// Currently only a fit for GRB is available.

 TH1* zdist=(TH1*)fBurstHistos.FindObject("hz");
 if (!zdist)
 {
  if (type.Contains("GRB"))
  {
   cout << endl;
   cout << " *" << ClassName() << "::GetBurstZdist* Called from " << name << endl;
   cout << " *** Archival observed redshift distribution not found. ***" << endl;
   cout << " A Landau fit from Swift GRB redshift data will be used to provide missing c.q. random z values." << endl;

   zdist=(TH1*)fBurstHistos.FindObject("hZpdf");
   if (!zdist)
   { 
    TF1 fz("fz","59.54*TMath::Landau(x,1.092,0.5203)");
    fz.SetRange(0,10);
    fz.SetNpx(10000);
    TH1* hfz=fz.GetHistogram();
    zdist=(TH1*)hfz->Clone();
    zdist->SetNameTitle("hZpdf","Landau fit for Swift GRB z data");
    zdist->GetXaxis()->SetTitle("GRB redshift");
    zdist->GetYaxis()->SetTitle("Counts");
    fBurstHistos.Add(zdist);
   }
  }
  else // Source class for which no fit is available
  {
   cout << endl;
   cout << " *" << ClassName() << "::GetBurstZdist* Called from " << name << endl;
   cout << " *** No redshift fit is available for source class " << type << " ***" << endl;
  }
 }

 return zdist;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstT90dist(TString file,TString tree,TString name,Int_t nb,Float_t xmin,Float_t xmax)
{
// Read observed archival T90 data and create a log10(T90) distribution.
// If this memberfunction is invoked before LoadBurstGCNdata() or GenBurstGCNdata(),
// the resulting log10(T90) distribution will be used to draw random T90 values
// (if requested) for the bursts without T90 information.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The input data has to be provided via a ROOT Tree which contains at least
// the specified variable name indicated below.
//
// Input arguments :
// -----------------
// file   : Name of the input file containing the ROOT Tree (wildcards are allowed)
// tree   : Name of the Tree containing the data
// name   : Name of the variable in the Tree containing the T90 data
// nb     : Number of bins for the T90 distribution
// xmin   : Minimal value for log10(T90)
// xmax   : Maximal value for log10(T90)
//
// The default values are nb=50, xmin=-5 and xmax=5.
//
// Note : This memberfunction may be invoked several times to read different files
//        to accumulate data.

 // The Tree containing the burst data
 TChain data(tree.Data());
 data.Add(file.Data());

 Int_t nen=data.GetEntries();
 TLeaf* lx=data.FindLeaf(name.Data());

 if (!nen || !lx)
 {
  cout << " *" << ClassName() << "::MakeBurstT90dist* Missing information for tree variable:" << name << endl;
  cout << " of Tree:" << tree << " with " << nen <<  " entries in file:" << file << endl;
  return;
 }

 // Create a new distribution in case a T90 distribution is not yet present
 TH1* t90dist=(TH1*)fBurstHistos.FindObject("ht90");
 if (!t90dist)
 {
  // Creation of observed burst t90 duration histo
  TH1F* ht90=new TH1F("ht90","Archival data of observed burst durations",nb,xmin,xmax);
  fBurstHistos.Add(ht90);
  ht90->GetXaxis()->SetTitle("Burst duration ^{10}log(T90) in sec.");
  ht90->GetYaxis()->SetTitle("Counts");
 }

 // Get pointer to the relevant histogram 
 TH1* ht90=(TH1*)fBurstHistos.FindObject("ht90");

 Int_t nt90=0;
 Double_t t90=0;
 for (Int_t ien=0; ien<nen; ien++)
 {
  data.GetEntry(ien);

  lx=data.GetLeaf(name.Data());
  if (!lx) continue;

  t90=lx->GetValue();
  if (t90>0)
  {
   ht90->Fill(log10(t90));
   nt90++;
  }
 }

 cout << " *" << ClassName() << "::MakeBurstT90dist* " << nt90 << " archival T90 values have been obtained from variable:" << name
      << " of Tree:" << tree << " in file(s):" << file << endl;
}
///////////////////////////////////////////////////////////////////////////
TH1* NcAstrolab::GetBurstT90dist(TString name,TString type)
{
// Internal member function to provide the archival c.q. fitted burst T90 distribution.
// The input argument "name" serves to identify the calling function in the printout.
// The input argument "type" serves to identify the source class for which a fit has to be provided.
// Currently only a fit for GRB is available.

 TH1* t90dist=(TH1*)fBurstHistos.FindObject("ht90");
 if (!t90dist)
 {
  if (type.Contains("GRB"))
  {
   cout << endl;
   cout << " *" << ClassName() << "::GetBurstT90dist* Called from " << name << endl;
   cout << " *** Observational T90 distribution not found. ***" << endl;
   cout << " A double Gaussian fit from Fermi GRB T90 data will be used to provide missing c.q. random T90 values." << endl;

   t90dist=(TH1*)fBurstHistos.FindObject("hT90pdf");
   if (!t90dist)
   {
    TF1 ft("ft","44.39*TMath::Gaus(x,-0.131,0.481)+193.8*TMath::Gaus(x,1.447,0.4752)");
    ft.SetRange(-5,5);
    ft.SetNpx(10000);
    TH1* hft=ft.GetHistogram();
    t90dist=(TH1*)hft->Clone();
    t90dist->SetNameTitle("hT90pdf","Double Gauss fit for Fermi t90 data");
    t90dist->GetXaxis()->SetTitle("GRB duration ^{10}log(T90) in sec.");
    t90dist->GetYaxis()->SetTitle("Counts");
    fBurstHistos.Add(t90dist);
   }
  }
  else // Source class for which no fit is available
  {
   cout << endl;
   cout << " *" << ClassName() << "::GetBurstT90dist* Called from " << name << endl;
   cout << " *** No T90 fit is available for source class " << type << " ***" << endl;
  }
 }

 return t90dist;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstSigmaPosdist(TString file,TString tree,TString name,TString u,Int_t nb,Float_t xmin,Float_t xmax)
{
// Read observed archival 1-sigma burst position uncertainty data and create the corresponding distribution.
// If this memberfunction is invoked before LoadBurstGCNdata() or GenBurstGCNdata(),
// the resulting distribution will be used to draw random 1-sigma burst position uncertainty values
// (if requested) for the bursts without localisation uncertainty information.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The input data has to be provided via a ROOT Tree which contains at least
// the specified variable name indicated below.
//
// Input arguments :
// -----------------
// file   : Name of the input file containing the ROOT Tree (wildcards are allowed)
// tree   : Name of the Tree containing the data
// name   : Name of the variable in the Tree containing the 1-sigma angular localisation uncertainty data
// u      : Units of the angular localisation uncertainty input data (see below)
// nb     : Number of bins for the resulting distribution
// xmin   : Minimal value for the 1-sigma position uncertainty in degrees
// xmax   : Maximal value for the 1-sigma position uncertainty in degrees
//
// Specification of the angular units :
// ------------------------------------ 
// "rad" : angular uncertainty provided in radians
// "deg" : angular uncertainty provided in degrees
// "dms" : angular uncertainty provided in dddmmss.sss
// "hms" : angular uncertainty provided in hhmmss.sss
// "hrs" : angular uncertainty provided in fractional hours
//
// The default values are nb=900, xmin=0 and xmax=90.
//
// Note : This memberfunction may be invoked several times to read different files
//        to accumulate data.

 // The Tree containing the burst data
 TChain data(tree.Data());
 data.Add(file.Data());

 Int_t nen=data.GetEntries();
 TLeaf* lx=data.FindLeaf(name.Data());

 if (!nen || !lx)
 {
  cout << " *" << ClassName() << "::MakeBurstSigmaPosdist* Missing information for tree variable:" << name << endl;
  cout << " of Tree:" << tree << " with " << nen <<  " entries in file:" << file << endl;
  return;
 }

 // Create a new distribution in case a burst position uncertainty distribution is not yet present
 TH1* sigmaposdist=(TH1*)fBurstHistos.FindObject("hsigmapos");
 if (!sigmaposdist)
 {
  // Creation of observed 1-sigma burst position uncertainty histo
  TH1F* hsigmapos=new TH1F("hsigmapos","Archival data of observed 1-sigma burst position uncertainties",nb,xmin,xmax);
  fBurstHistos.Add(hsigmapos);
  hsigmapos->GetXaxis()->SetTitle("Burst position uncertainty (sigma in degrees)");
  hsigmapos->GetYaxis()->SetTitle("Counts");
 }

 // Get pointer to the relevant histogram 
 TH1* hsigmapos=(TH1*)fBurstHistos.FindObject("hsigmapos");

 Int_t nsigmapos=0;
 Double_t sigmapos=0;
 for (Int_t ien=0; ien<nen; ien++)
 {
  data.GetEntry(ien);

  lx=data.GetLeaf(name.Data());
  if (!lx) continue;

  sigmapos=lx->GetValue();

  // Convert declination to degrees if needed
  sigmapos=ConvertAngle(sigmapos,u,"deg");

  if (sigmapos<xmin || sigmapos>xmax) continue;

  hsigmapos->Fill(sigmapos);
  nsigmapos++;
 }

 cout << " *" << ClassName() << "::MakeBurstSigmaPosdist* " << nsigmapos << " archival sigmapos values have been obtained from variable:" << name
      << " of Tree:" << tree << " in file(s):" << file << endl;
}
///////////////////////////////////////////////////////////////////////////
TH1* NcAstrolab::GetBurstSigmaPosdist(TString name,TString type)
{
// Internal member function to provide the archival c.q. fitted 1-sigma burst position uncertainty distribution.
// The input argument "name" serves to identify the calling function in the printout.
// The input argument "type" serves to identify the source class for which a fit has to be provided.
// Currently only a fit for GRB is available.

 TH1* sigmaposdist=(TH1*)fBurstHistos.FindObject("hsigmapos");
 if (!sigmaposdist)
 {
  if (type.Contains("GRB"))
  {
   cout << endl;
   cout << " *" << ClassName() << "::GetBurstSigmaPosdist* Called from " << name << endl;
   cout << " *** Archival observed GRB position uncertainty distribution not found. ***" << endl;
   cout << " A Landau fit from observed GRB data will be used to provide missing c.q. random 1-sigma uncertainty values." << endl;

   sigmaposdist=(TH1*)fBurstHistos.FindObject("hSigmaSourcePDF");
   if (!sigmaposdist)
   { 
    TF1 fsigmapos("fsigmapos","245.2*TMath::Landau(x,-2.209,0.6721,1)");
    fsigmapos.SetRange(0,90);
    fsigmapos.SetNpx(10000);
    TH1* hfsigmapos=fsigmapos.GetHistogram();
    sigmaposdist=(TH1*)hfsigmapos->Clone();
    sigmaposdist->SetNameTitle("hSigmaSourcePDF","Landau fit for burst 1-sigma position uncertainty data");
    sigmaposdist->GetXaxis()->SetTitle("Burst position uncertainty (sigma in degrees)");
    sigmaposdist->GetYaxis()->SetTitle("Counts");
    fBurstHistos.Add(sigmaposdist);
   }
  }
  else // Source class for which no fit is available
  {
   cout << endl;
   cout << " *" << ClassName() << "::GetBurstSigmaPosdist* Called from " << name << endl;
   cout << " *** No position uncertainty fit is available for source class " << type << " ***" << endl;
  }
 }

 return sigmaposdist;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstEnergydist(Int_t mode,TF1& spec,Double_t Emin,Double_t Emax,Int_t nbins)
{
// Create a background (mode=1) or burst induced signal (mode=2) energy distribution
// on the interval [Emin,Emax] GeV based on the spectral function "spec" describing dN/dE.
// If this memberfunction is invoked before GenBurstSignals(), the resulting 
// energy distribution will be used to draw random energy values for either the
// generated background or burst induced signal events via the memberfunctions
// GetBurstBackgroundEnergy() and GetBurstSignalEnergy(), respectively.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Example: To make a dN/dE=E^-2 burst induced signal energy distribution for 100 Gev < E < 10 PeV
//
// mode=2;
// TF1 spec("spec","pow(x,-2.)");
// xmin=100;
// xmax=1e7;
// nbins=1000;
// MakeBurstEnergydist(mode,spec,Emin,Emax,nbins);
//
// The default value is nbins=1000.

 if (Emin<=0) Emin=1e-10;

 if ((mode!=1 && mode!=2) || Emax<=Emin)
 {
  cout << " *" << ClassName() << "::MakeBurstEnergydist* Inconsistent data: mode=" << mode << " Emin=" << Emin << " Emax=" << Emax << endl;
  return;
 }

 // Convert the energy boundaries to the log10 scale of the X-axis
 Double_t xmin=log10(Emin);
 Double_t xmax=log10(Emax);

 TString sf="dN/dE=";
 sf+=spec.GetExpFormula("p");
 if (mode==1) sf+=" background";
 if (mode==2) sf+=" signal";
 sf+=" distribution";
 sf.ReplaceAll("x","E");
 TString sh="";
 if (mode==1) sh="Background energy distribution;^{10}Log(E) in GeV;";
 if (mode==2) sh="Source induced signal energy distribution;^{10}Log(E) in GeV;";
 sh+=sf;
 TH1F his=GetCountsHistogram(spec,nbins,xmin,xmax,1,sh);

 // Remove the corresponding old distribution (if any) from the storage
 TH1* dist=0;
 if (mode==1) dist=(TH1*)fBurstHistos.FindObject("hBkgEpdf");
 if (mode==2) dist=(TH1*)fBurstHistos.FindObject("hSigEpdf");
 if (dist)
 {
  fBurstHistos.Remove(dist);
  fBurstHistos.Compress();
  delete dist;
  dist=0;
 }

 // Store the newly created distribution
 // and set a flag to indicate a parametrized distribution
 if (mode==1)
 {
  TH1F* hBkgEpdf=(TH1F*)his.Clone();
  hBkgEpdf->SetName("hBkgEpdf");
  fBurstHistos.Add(hBkgEpdf);
  fBurstParameters->AddNamedSlot("PDFbkgE");
  fBurstParameters->SetSignal(1,"PDFbkgE");
 }
 if (mode==2)
 {
  TH1F* hSigEpdf=(TH1F*)his.Clone();
  hSigEpdf->SetName("hSigEpdf");
  fBurstHistos.Add(hSigEpdf);
  fBurstParameters->AddNamedSlot("PDFsigE");
  fBurstParameters->SetSignal(1,"PDFsigE");
 }

 cout << " *" << ClassName() << "::MakeBurstEnergydist* Created a " << sf
      << " on [Emin,Emax]=[" << Emin << "," << Emax << "] GeV" << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstEnergydist(Int_t mode,Double_t alpha,Double_t Emin,Double_t Emax,Int_t nbins)
{
// Create a background (mode=1) or burst induced signal (mode=2) energy distribution
// on the interval [Emin,Emax] GeV based on a single power law with 
// spectral index "alpha" describing dN/dE=E^-alpha.
// If this memberfunction is invoked before GenBurstSignals(), the resulting 
// energy distribution will be used to draw random energy values for either the
// generated background or burst induced signal events via the memberfunctions
// GetBurstBackgroundEnergy() and GetBurstSignalEnergy(), respectively.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Example: To make a dN/dE=E^-2 burst induced signal energy distribution for 100 Gev < E < 10 PeV
//
// mode=2;
// alpha=2;
// Emin=100;
// Emax=1e7;
// nbins=1000;
// MakeBurstEnergydist(mode,alpha,Emin,Emax,nbins);
//
// The default value is nbins=1000.

 TF1 spec("spec","pow(x,[0])");
 spec.SetParameter(0,-alpha);
 MakeBurstEnergydist(mode,spec,Emin,Emax,nbins);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstEnergydist(Int_t mode,TString file,TString tree,TString name1,TString name2,TString u,Double_t Emin,Double_t Emax,Int_t nb)
{
// Create a background (mode=1) or burst induced signal (mode=2) energy distribution
// on the interval [Emin,Emax] GeV based on observed archival energy data.
// If this memberfunction is invoked before GenBurstSignals(), the resulting 
// log10(E) distribution will be used to draw random energy values for either the
// generated background or burst induced signal events via the memberfunctions
// GetBurstBackgroundEnergy() and GetBurstSignalEnergy(), respectively.
//
// Note : Only those data will be used that correspond with the selected
//        declination interval for the burst investigations.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The input data has to be provided via a ROOT Tree which contains at least
// the specified variable names indicated below.
//
// Notes :
// -------
// 1) With mode<0 (see below) this memberfunction may be invoked several times to
//    read different files to accumulate data.
// 2) In case a corresponding parametrized energy distribution exists,
//    this will always be replaced by a new energy distribution based on data.
// 3) In case observed archival data for burst induced signal events is loaded,
//    the redshift correction for signal events will be deactivated. 
//
// Input arguments :
// -----------------
// mode   :  1 --> Always create a new energy distribution for background events.
//        :  2 --> Always create a new energy distribution for burst induced signal events.
//        : -1 --> Create a new (if needed) or add data to an energy distribution for background events.
//        : -2 --> Create a new (if needed) or add data to an energy distribution for burst induced signal events.
// file   : Name of the input file containing the ROOT Tree (wildcards are allowed)
// tree   : Name of the Tree containing the data
// name1  : Name of the tree variable containing the log10(E) data (in GeV)
// name2  : Name of the tree variable containing the declination data
// u      : The units (see below) of the declination data
// Emin   : Minimal energy value in GeV
// Emax   : Maximal energy value in GeV
// nb     : Number of bins for the Energy distribution
//
// Specification of the angular units :
// ------------------------------------ 
// "rad" : input angle provided in radians
// "deg" : input angle provided in degrees
// "dms" : input angle provided in dddmmss.sss
// "hms" : input angle provided in hhmmss.sss
// "hrs" : input angle provided in fractional hours
//
// The default value is nb=1000.

 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");

 if (Emin<=0) Emin=1e-10;

 if ((abs(mode)!=1 && abs(mode)!=2) || Emax<=Emin)
 {
  cout << " *" << ClassName() << "::MakeBurstEnergydist* Inconsistent data: mode=" << mode << " Emin=" << Emin << " Emax=" << Emax << endl;
  return;
 }

 // Convert the energy boundaries to the log10 scale of the X-axis
 Double_t xmin=log10(Emin);
 Double_t xmax=log10(Emax);

 // The Tree containing the burst data
 TChain data(tree.Data());
 data.Add(file.Data());

 Int_t nen=data.GetEntries();

 if (!nen || !data.FindLeaf(name1.Data()) || !data.FindLeaf(name2.Data()))
 {
  cout << " *" << ClassName() << "::MakeBurstEnergydist* Missing information for tree variable:" << name1
       << " and/or tree variable:" << name2 << endl;
  cout << " of Tree:" << tree << " with " << nen <<  " entries in file:" << file << endl;
  return;
 }

 // A corresponding parametrized distribution will always be removed
 Int_t flag=0;
 if (abs(mode)==1) flag=TMath::Nint(fBurstParameters->GetSignal("PDFbkgE"));
 if (abs(mode)==2) flag=TMath::Nint(fBurstParameters->GetSignal("PDFsigE"));

 if (flag) mode=abs(mode);

 // Remove the corresponding old distribution (if requested) from the storage
 TH1* Edist=0;
 if (abs(mode)==1) Edist=(TH1*)fBurstHistos.FindObject("hBkgEpdf");
 if (abs(mode)==2) Edist=(TH1*)fBurstHistos.FindObject("hSigEpdf");

 if (mode>0 && Edist)
 {
  fBurstHistos.Remove(Edist);
  fBurstHistos.Compress();
  delete Edist;
  Edist=0;
 }
 
 // Create a new distribution if needed
 if (!Edist)
 {
  mode=abs(mode);
  if (mode==1)
  {
   // Creation of the observed background energy histo
   TH1F* hBkgEpdf=new TH1F("hBkgEpdf","Archival data of observed background energies",nb,xmin,xmax);
   fBurstHistos.Add(hBkgEpdf);
   hBkgEpdf->GetXaxis()->SetTitle("^{10}log(Energy) in GeV");
   hBkgEpdf->GetYaxis()->SetTitle("Counts");
   fBurstParameters->AddNamedSlot("PDFbkgE");
   fBurstParameters->SetSignal(0,"PDFbkgE");
  }
  if (mode==2)
  {
   // Creation of the observed signal energy histo
   TH1F* hSigEpdf=new TH1F("hSigEpdf","Archival data of observed signal energies",nb,xmin,xmax);
   fBurstHistos.Add(hSigEpdf);
   hSigEpdf->GetXaxis()->SetTitle("^{10}log(Energy) in GeV");
   hSigEpdf->GetYaxis()->SetTitle("Counts");
   fBurstParameters->AddNamedSlot("PDFsigE");
   fBurstParameters->SetSignal(0,"PDFsigE");
   fBurstParameters->SetSignal(0,"Ezcor");
  }
 }

 // Get pointer to the relevant histogram 
 if (abs(mode)==1) Edist=(TH1*)fBurstHistos.FindObject("hBkgEpdf");
 if (abs(mode)==2) Edist=(TH1*)fBurstHistos.FindObject("hSigEpdf");

 Int_t nE=0;
 Double_t logE=0;
 Double_t dec=0;
 TLeaf* lx=0;
 for (Int_t ien=0; ien<nen; ien++)
 {
  data.GetEntry(ien);

  lx=data.GetLeaf(name1.Data());
  if (!lx) continue;

  logE=lx->GetValue();

  lx=data.GetLeaf(name2.Data());
  if (!lx) continue;

  dec=lx->GetValue();

  // Convert declination to degrees if needed
  dec=ConvertAngle(dec,u,"deg");

  if (dec>=fDeclmin && dec<=fDeclmax)
  {
   Edist->Fill(logE);
   nE++;
  }
 }

 TString smode="";
 if (abs(mode)==1) smode="archival background";
 if (abs(mode)==2) smode="archival signal";

 if (mode>0)
 {
  cout << " *" << ClassName() << "::MakeBurstEnergydist* A new " << smode
       << " energy distribution has been created." << endl; 
 }
 else
 {
  cout << " *" << ClassName() << "::MakeBurstEnergydist* Statistics of the existing " << smode
       << " energy distribution have been increased." << endl; 
 }

 cout << "  " << nE << " energy values have been obtained from variable:" << name1
      << " of Tree:" << tree << " in file(s):" << file << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstRecoAngresdist(TString file,TString tree,TString name1,TString name2,TString ua,TString name3,TString ud,Double_t Emin,Double_t Emax,Int_t nbe,Int_t nba)
{
// Create a reconstruction angle resolution vs. energy distribution on the interval [Emin,Emax] GeV
// based on observed or simulated archival data.
// The angular resolution range is fixed to [0,180] degrees. 
// If this memberfunction is invoked before GenBurstSignals(), the resulting
// distribution will be used to determine the reconstruction angular uncertainty
// for individual events by invokation of GetBurstRecoAngres().
//
// Note : Only those data will be used that correspond with the selected
//        declination interval for the burst investigations.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The input data has to be provided via a ROOT Tree which contains at least
// the specified variable names indicated below.
//
// Input arguments :
// -----------------
// file   : Name of the input file containing the ROOT Tree (wildcards are allowed)
// tree   : Name of the Tree containing the data
// name1  : Name of the tree variable containing the log10(E) data (in GeV)
// name2  : Name of the tree variable containing the reconstruction angle uncertainty
// ua     : The units (see below) of the angular uncertainty data
// name3  : Name of the tree variable containing the declination data
// ud     : The units (see below) of the declination data
// Emin   : Minimal energy value in GeV
// Emax   : Maximal energy value in GeV
// nbe    : Number of bins for the Energy axis
// nba    : Number of bins for the Angular axis
//
// Specification of the angular units :
// ------------------------------------ 
// "rad" : input angle provided in radians
// "deg" : input angle provided in degrees
// "dms" : input angle provided in dddmmss.sss
// "hms" : input angle provided in hhmmss.sss
// "hrs" : input angle provided in fractional hours
//
// The default values are nbe=100 and nba=1000.
//
// Note : This memberfunction may be invoked several times to read different files
//        to accumulate data.

 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");

 if (Emin<=0) Emin=1e-10;

 if (Emax<=Emin)
 {
  cout << " *" << ClassName() << "::MakeBurstRecoAngresdist* Inconsistent data: Emin=" << Emin << " Emax=" << Emax << endl;
  return;
 }

 // Convert the energy boundaries to the log10 scale of the X-axis
 Double_t xmin=log10(Emin);
 Double_t xmax=log10(Emax);

 // The Tree containing the burst data
 TChain data(tree.Data());
 data.Add(file.Data());

 Int_t nen=data.GetEntries();

 if (!nen || !data.FindLeaf(name1.Data()) || !data.FindLeaf(name2.Data()) || !data.FindLeaf(name3.Data()))
 {
  cout << " *" << ClassName() << "::MakeBurstRecoAngresdist* Missing information for tree variable:" << name1
       << " and/or tree variable:" << name2 << " and/or tree variable:" << name3 << endl;
  cout << " of Tree:" << tree << " with " << nen <<  " entries in file:" << file << endl;
  return;
 }

 // Create a new distribution in case a reconstruction angle resolution vs. energy distribution is not yet present
 TH2* Angresdist=(TH2*)fBurstHistos.FindObject("hAngresE");
 if (!Angresdist)
 {
  // Creation of the observed reconstruction angle resolution vs. energy histo
  TH2F* hAngresE=new TH2F("hAngresE","Archival data of observed reconstruction angle resolution vs. energy",
  nbe,xmin,xmax,nba,0,180.1);
  fBurstHistos.Add(hAngresE);
  hAngresE->GetXaxis()->SetTitle("^{10}log(Energy) in GeV");
  hAngresE->GetYaxis()->SetTitle("Angular resolution in degrees");
 }

 // Get pointer to the relevant histogram 
 TH2* hAngresE=(TH2*)fBurstHistos.FindObject("hAngresE");

 Int_t nE=0;
 Double_t logE=0;
 Double_t dec=0;
 Double_t dang=0;
 TLeaf* lx=0;
 for (Int_t ien=0; ien<nen; ien++)
 {
  data.GetEntry(ien);

  lx=data.GetLeaf(name1.Data());
  if (!lx) continue;

  logE=lx->GetValue();

  lx=data.GetLeaf(name2.Data());
  if (!lx) continue;

  dang=lx->GetValue();

  // Convert declination to degrees if needed
  dang=ConvertAngle(dang,ua,"deg");

  lx=data.GetLeaf(name3.Data());
  if (!lx) continue;

  dec=lx->GetValue();

  // Convert declination to degrees if needed
  dec=ConvertAngle(dec,ud,"deg");

  if (dec>=fDeclmin && dec<=fDeclmax)
  {
   hAngresE->Fill(logE,dang);
   nE++;
  }
 }

 cout << " *" << ClassName() << "::MakeBurstRecoAngresdist* " << nE << " archival entries have been obtained for variables:" << name2
      << " vs. " << name1 << " of Tree:" << tree << " in file(s):" << file << endl;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetBurstSignalEnergy(Double_t Emin,Double_t Emax) const
{
// Provide the energy in the interval [Emin,Emax] GeV for a Burst signal 
// from the user provided energy spectrum as produced by MakeBurstEdist().
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// If Emin<=0 the lower boundary of the provided spectrum will be used as Emin. 
// If Emax<=0 the upper boundary of the provided spectrum will be used as Emax.
//
// In case of inconsistent data the value -1 is returned.
//
// The default values are Emin=-1 and Emax=-1.

 Double_t E=-1;

 // Get pointer to the relevant histogram 
 TH1* hSigEpdf=(TH1*)fBurstHistos.FindObject("hSigEpdf");

 if (!hSigEpdf) return E;

 Int_t nbins=hSigEpdf->GetNbinsX();
 Int_t nentries=hSigEpdf->GetEntries();

 if (nbins<=0 || nentries<=0) return E;

 TAxis* xaxis=hSigEpdf->GetXaxis();
 
 if (!xaxis) return E;

 Double_t xlow=xaxis->GetBinLowEdge(1);
 Double_t xup=xaxis->GetBinUpEdge(nbins);

 Double_t logEmin=0;
 if (Emin<=0)
 {
  logEmin=xlow;
 }
 else
 {
  logEmin=log10(Emin);
 }

 Double_t logEmax=0;
 if (Emax<=0)
 {
  logEmax=xup;
 }
 else
 {
  logEmax=log10(Emax);
 }

 if (logEmax<=logEmin || logEmin>=xup || logEmax<=xlow) return E;

 while (E<logEmin || E>logEmax)
 {
  E=hSigEpdf->GetRandom();
 }

 E=pow(float(10),E);

 return E;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetBurstBackgroundEnergy(Double_t Emin,Double_t Emax) const
{
// Provide the energy in the interval [Emin,Emax] GeV for backgound events
// from the user provided energy spectrum as produced by MakeBurstBkgEdist().
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// If Emin<=0 the lower boundary of the provided spectrum will be used as Emin. 
// If Emax<=0 the upper boundary of the provided spectrum will be used as Emax.
//
// In case of inconsistent data the value -1 is returned.
//
// The default values are Emin=-1 and Emax=-1.

 Double_t E=-1;

 // Get pointer to the relevant histogram 
 TH1* hBkgEpdf=(TH1*)fBurstHistos.FindObject("hBkgEpdf");

 if (!hBkgEpdf) return E;

 Int_t nbins=hBkgEpdf->GetNbinsX();
 Int_t nentries=hBkgEpdf->GetEntries();

 if (nbins<=0 || nentries<=0) return E;

 TAxis* xaxis=hBkgEpdf->GetXaxis();
 
 if (!xaxis) return E;

 Double_t xlow=xaxis->GetBinLowEdge(1);
 Double_t xup=xaxis->GetBinUpEdge(nbins);

 Double_t logEmin=0;
 if (Emin<=0)
 {
  logEmin=xlow;
 }
 else
 {
  logEmin=log10(Emin);
 }

 Double_t logEmax=0;
 if (Emax<=0)
 {
  logEmax=xup;
 }
 else
 {
  logEmax=log10(Emax);
 }

 if (logEmax<=logEmin || logEmin>=xup || logEmax<=xlow) return E;

 while (E<logEmin || E>logEmax)
 {
  E=hBkgEpdf->GetRandom();
 }

 E=pow(float(10),E);

 return E;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetBurstRecoAngres(Double_t Emin,Double_t Emax,Double_t Amin,Double_t Amax) const
{
// Provide the reconstruction angle resolution (in degrees) for the energy interval [Emin,Emax] GeV.
// Depending on the burst parameter setting "Recoangle", the returned value is either constant
// (i.e. the value of the burst parameter "Angresfix") or determined from the user provided distribution
// as produced by MakeBurstRecoAngresdist().
// In case the user has requested the reconstruction angle resolution to be drawn randomly
// from the distribution, the returned value will be within the interval [Amin,Amax] degrees.
//
// Note : In case no distribution is available, the constant value of "Angresfix" is returned.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// If Emin<=0 the lower boundary of the provided spectrum will be used as Emin. 
// If Emax<=0 the upper boundary of the provided spectrum will be used as Emax.
//
// In case of inconsistent data the value -1 is returned.
//
// The default values are Emin=-1, Emax=-1, Amin=0 and Amax=999.

 Double_t dang=-1;

 if (Amin<0) Amin=0;

 Int_t fRecoangle=TMath::Nint(fBurstParameters->GetSignal("Recoangle"));
 Float_t fAngresfix=fBurstParameters->GetSignal("Angresfix");

 // The user requested a fixed angular resolution value "Angresfix"
 if (!fRecoangle)
 {
  dang=fAngresfix;
  return dang;
 }

 // The user requested an angular resolution based on a distribution

 // Get pointer to the reco angle resolution vs. energy distribution histogram 
 TH2* hAngresE=(TH2*)fBurstHistos.FindObject("hAngresE");

 // No distribution available -> Return the user provided "Angresfix" value
 if (!hAngresE)
 {
  dang=fAngresfix;
  return dang;
 }

 // Obtain the projected reco angle resolution distribution within the [Emin,Emax] interval

 Int_t nbins=hAngresE->GetNbinsX();
 Int_t nentries=hAngresE->GetEntries();

 if (nbins<=0 || nentries<=0) return dang;

 TAxis* xaxis=hAngresE->GetXaxis();
 
 if (!xaxis) return dang;

 Double_t xlow=xaxis->GetBinLowEdge(1);
 Double_t xup=xaxis->GetBinUpEdge(nbins);

 Double_t logEmin=0;
 if (Emin<=0)
 {
  logEmin=xlow;
 }
 else
 {
  logEmin=log10(Emin);
 }

 Double_t logEmax=0;
 if (Emax<=0)
 {
  logEmax=xup;
 }
 else
 {
  logEmax=log10(Emax);
 }

 if (logEmax<logEmin || logEmin>=xup || logEmax<=xlow) return dang;

 Int_t ilow=xaxis->FindBin(logEmin);
 Int_t iup=xaxis->FindBin(logEmax);

 TH1D* hproj=hAngresE->ProjectionY("hproj",ilow,iup);

 if (!hproj) return dang;

 nbins=hproj->GetNbinsX();
 nentries=hproj->GetEntries();

 if (nbins<=0 || nentries<=0) return dang;

 if (fRecoangle==1) dang=hproj->GetMean();

 if (fRecoangle==2)
 {
  NcSample q;
  dang=q.GetMedian(hproj);
 }

 if (fRecoangle==3)
 {
  xaxis=hproj->GetXaxis();
 
  if (!xaxis) return dang;

  xlow=xaxis->GetBinLowEdge(1);
  xup=xaxis->GetBinUpEdge(nbins);

  if (Amax<=Amin || Amin>=xup || Amax<=xlow) return dang;
  
  dang=Amin-1.;
  while (dang<Amin || dang>Amax)
  {
   dang=hproj->GetRandom();
  }
 }

 if (hproj) delete hproj;

 return dang;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GenBurstSignals()
{
// Generate detector signals from transient bursts and (atmospheric) background.
// Data will be generated according to the parameters as specified
// by the user via prior invokation of the memberfunction SetBurstParameter().
//
// Since both the signal and background events are generated according to the
// parameters specified by the user, no additional matching of the (fictative)
// observations with the source positions is needed.
// As such, after the invokation of this GenBurstSignals(), the user may directly
// invoke various statistical evaluations like for instance GetBurstLiMaSignificance(),
// GetBurstBayesianSignalRate() etc...  
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 // Signal and background generation can only be performed for a user defined fixed time window size
 Float_t fTmin=fBurstParameters->GetSignal("Tmin");
 Float_t fTmax=fBurstParameters->GetSignal("Tmax");
 if (fTmax<=fTmin)
 {
  printf("\n *%-s::GenBurstSignals* Error : [Tmin,Tmax]=[%-g,%-g] whereas Tmin<Tmax is required. \n",ClassName(),fTmin,fTmax);
  return;
 }

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // with identical measurements and reference signals without explicit initialisation
 // of the randomiser by the user at the start of the analysis.
 if (!fRan) fRan=new NcRandom(-1);

 ////////////////////////////////////////////
 // Initialise the final sample histograms //
 ////////////////////////////////////////////

 InitBurstHistograms(0);

 // Retrieve the needed parameters
 Float_t fRAmin=fBurstParameters->GetSignal("RAmin");
 Float_t fRAmax=fBurstParameters->GetSignal("RAmax");
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t fTimres=fBurstParameters->GetSignal("Timres");
 Int_t fEzcor=TMath::Nint(fBurstParameters->GetSignal("Ezcor"));
 Int_t fPDFsigE=TMath::Nint(fBurstParameters->GetSignal("PDFsigE"));
 Float_t fEmin=fBurstParameters->GetSignal("Emin");
 Float_t fEmax=fBurstParameters->GetSignal("Emax");
 Int_t fKinangle=TMath::Nint(fBurstParameters->GetSignal("Kinangle"));
 Float_t fAngresmin=fBurstParameters->GetSignal("Angresmin");
 Float_t fAngresmax=fBurstParameters->GetSignal("Angresmax");
 Float_t fAngresfix=fBurstParameters->GetSignal("Angresfix");
 Int_t fRecoangle=TMath::Nint(fBurstParameters->GetSignal("Recoangle"));
 Int_t fSumsigmas=TMath::Nint(fBurstParameters->GetSignal("Sumsigmas"));
 Float_t fTfact=fBurstParameters->GetSignal("Tfact");
 Float_t fDawin=fBurstParameters->GetSignal("Dawin");
 Int_t fDatype=TMath::Nint(fBurstParameters->GetSignal("Datype"));
 Float_t fGrbnu=fBurstParameters->GetSignal("Grbnu");
 Int_t fInburst=TMath::Nint(fBurstParameters->GetSignal("Inburst"));
 Float_t fDtnu=fBurstParameters->GetSignal("Dtnu");
 Float_t fDtnus=fBurstParameters->GetSignal("Dtnus");
 Int_t fNbkg=TMath::Nint(fBurstParameters->GetSignal("Nbkg"));

 // Deactivate redshift correction for source signal events from archival observed data
 if (!fPDFsigE)
 {
  fEzcor=0;
  fBurstParameters->SetSignal(0,"Ezcor");
 } 

 // Derived parameters
 Float_t fMaxsigmatot=fBurstParameters->GetSignal("Maxsigmatot");
 Float_t fNbkgWin=fBurstParameters->GetSignal("NbkgWin");

 ////////////////////////////////////////////////
 // Some Burst statistics from the loaded data //
 ////////////////////////////////////////////////

 Int_t fNgrbs=GetNsignals(0);

 TH1* hSigmaReco=(TH1*)fBurstHistos.FindObject("hSigmaReco");
 TH1* hSigE=(TH1*)fBurstHistos.FindObject("hSigE");
 TH1* hSigEzcor=(TH1*)fBurstHistos.FindObject("hSigEzcor");

 //////////////////////////////////////////////////////////
 // Generation of the signal and background observations //
 // based on the provided user settings                  //
 //////////////////////////////////////////////////////////

 NcSignal* sx=0;
 Float_t zgrb=0;
 Double_t dgrb=0;
 Float_t t90grb=0;
 Float_t sigmagrb=0;
 NcPosition rgrb;
 Int_t nmu;
 Double_t thetagrb,phigrb;
 Double_t dmu,thetamu,phimu;
 Float_t dt=0;
 NcPosition rgrb2; // Unknown actual GRB position from which the neutrinos/muons arrive
 NcPosition rmu;
 Float_t dang;
 Float_t dangmax=0;
 Float_t dangmaxon=0;
 Float_t dangmaxoff=0;
 Float_t thlow,thup;
 Float_t ranlow,ranup;
 Int_t nmugrb=0;
 NcTimestamp* tx=0;
 NcTimestamp tmu;
 Float_t solidangle=0;
 Float_t ramu,decmu; // Temporary RA and DEC of muon for background creation
 Double_t E=0;
 Double_t ang=0;
 Double_t sigmareco=0;
 Float_t sigmatot=0;
 TString name;
 Int_t fixedwinset=0; // Flag to indicate whether a fixed angular search window is set (1) or not (0) for this burst

 // Storage for total on-source and off-source observed and signal injected energies
 fBurstParameters->AddNamedSlot("EnergyOn");
 fBurstParameters->AddNamedSlot("EnergyOff");
 fBurstParameters->AddNamedSlot("EnergySig");

 // Loop over the (fictative) GRB space-time positions in the declination acceptance
 for (Int_t igrb=0; igrb<fNgrbs; igrb++)
 {
  sx=GetSignal(igrb+1);

  if (!sx) continue;

  tx=sx->GetTimestamp();
  zgrb=sx->GetSignal("z");
  t90grb=sx->GetSignal("T90");
  sigmagrb=sx->GetSignal("csigma");
  GetSignal(dgrb,thetagrb,"deg",phigrb,"deg","loc",tx,igrb+1);
  rgrb.SetPosition(1,thetagrb,phigrb,"sph","deg");

  dangmax=-1;
  if (!fDatype) dangmax=fabs(fDawin);
  if (fDatype==1)
  {
   dangmax=0;
   if (fMaxsigmatot>0) dangmax=fabs(fDawin*fMaxsigmatot);
  }

  if (fDatype==2)
  {
   if (!fSumsigmas) dangmax=fabs(fDawin*sigmagrb);
   if (!fRecoangle)
   {
    sigmatot=-1;
    if (fSumsigmas==-1) sigmatot=fAngresfix;
    if (fSumsigmas==1) sigmatot=sigmagrb+fAngresfix;
    if (fSumsigmas==2) sigmatot=sqrt(sigmagrb*sigmagrb+fAngresfix*fAngresfix);
    if (sigmatot>=0) dangmax=fabs(fDawin*sigmatot);
   }
  }

  fixedwinset=1;
  if (dangmax<0) fixedwinset=0;

  // Indicate incompatible input for sigma summation
  if (fDatype==1 && fMaxsigmatot<0) dangmax=-1;

  // New signal slots for storage of maximum angular differences and corresponding solid angles
  name="fixedwinset";
  sx->AddNamedSlot(name);
  sx->SetSignal(fixedwinset,name);
  name="dangmaxOn";
  sx->AddNamedSlot(name);
  sx->SetSignal(-1,name);
  name="dangmaxOff";
  sx->AddNamedSlot(name);
  sx->SetSignal(-1,name);
  name="OmegaOn";
  sx->AddNamedSlot(name);
  sx->SetSignal(0,name);
  name="OmegaOff";
  sx->AddNamedSlot(name);
  sx->SetSignal(0,name);

  // Store the On-source and Off-source maximum (solid) angles for this burst
  if (fixedwinset && dangmax>=0)
  {
   if (fDawin<0) // Local zenith band
   {
    thlow=thetagrb-0.5*dangmax;
    thup=thetagrb+0.5*dangmax;
    solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
   }
   else // Circle around GRB position
   {
    thlow=0;
    thup=dangmax;
    solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
   }
   sx->SetSignal(dangmax,"dangmaxOn");
   sx->SetSignal(solidangle,"OmegaOn");
   sx->SetSignal(dangmax,"dangmaxOff");
   sx->SetSignal(solidangle*float(fNbkg),"OmegaOff");
  }
  else // Initialize the encountered dynamic On-source and Off-source maximum angles for this burst
  {
   dangmaxon=0;
   if (fSumsigmas==0 || fSumsigmas==1 || fSumsigmas==2) dangmaxon=fabs(fDawin*sigmagrb);
   dangmaxoff=dangmaxon;
  }

  // Generate the background events in the search time window 
  // for both this GRB angular cone and the corresponding background bkg patch(es)
  for (Int_t bkgpatch=0; bkgpatch<=fNbkg; bkgpatch++)
  {
   nmu=int(fRan->Poisson(fNbkgWin));
   for (Int_t imu=0; imu<nmu; imu++)
   {
    // Obtain a random event time within the search time window
    ranlow=fTmin;
    ranup=fTmax;
    dt=fRan->Uniform(ranlow,ranup);

    // Create a random background event within the user selected burst RA and Dec interval
    // and convert to local detector coordinates to allow a local zenith band selection.
    // For the conversion, a single, re-usable temp. reference signal will be created, since measurements may get scrambled when stored.
    thlow=90.-fDeclmax; // Lower theta angle in overall Earth spherical coordinates (North Pole is theta=0)
    thup=90.-fDeclmin;  // Upper theta angle in overall Earth spherical coordinates (North Pole is theta=0)
    RandomPosition(rmu,thlow,thup,fRAmin,fRAmax);
    decmu=90.-rmu.GetX(2,"sph","deg");
    ramu=rmu.GetX(3,"sph","deg");
    tmu=*tx;
    tmu.AddSec(dt*fTfact);
    SetSignal(1,ramu,"deg",decmu,"deg","equ",&tmu,fNgrbs+1,"J","bkgtemp",0);
    GetSignal(dmu,thetamu,"deg",phimu,"deg","loc",&tmu,fNgrbs+1);
    rmu.SetPosition(1,thetamu,phimu,"sph","deg");

    if (fDawin<0) // Local zenith band
    {
     dang=fabs(thetagrb-thetamu);
    }
    else // Circle around GRB position
    {
     dang=rgrb.GetOpeningAngle(rmu,"deg");
    }

    // Check if event lies outside the allowed angular area
    if (fDatype>=0 && fixedwinset && dang>dangmax) continue;

    // The energy of the background signal
    E=GetBurstBackgroundEnergy();

    if (E<0 || E<fEmin || E>fEmax) continue;

    // The reconstruction angular resolution of the background signal
    sigmareco=GetBurstRecoAngres(E,E);
    if (sigmareco<0) sigmareco=fAngresfix;

    if (sigmareco<fAngresmin || sigmareco>fAngresmax) continue;

    if (hSigmaReco) hSigmaReco->Fill(sigmareco);

    sigmatot=-1;
    if (fSumsigmas==-1) sigmatot=sigmareco;
    if (fSumsigmas==0) sigmatot=sigmagrb;
    if (fSumsigmas==1) sigmatot=sigmagrb+sigmareco;
    if (fSumsigmas==2) sigmatot=sqrt(sigmagrb*sigmagrb+sigmareco*sigmareco);

    // Determine the dynamic angular window including the track reco uncertainty
    if (!fixedwinset)
    {
     dangmax=-1;
     if (sigmatot>=0) dangmax=fabs(fDawin*sigmatot);
    }

    if (fDatype>=0 && dang>dangmax) continue;

    if (!bkgpatch)
    {
     if (dangmax>dangmaxon) dangmaxon=dangmax;
     fBurstOnReco.Enter(zgrb,sigmagrb,sigmareco,sigmatot);
     fBurstOnMatch.Enter(E,dt,dang,dt/(zgrb+1.));
     fBurstParameters->AddSignal(E,"EnergyOn");
    }
    else
    {
     if (dangmax>dangmaxoff) dangmaxoff=dangmax;
     fBurstOffReco.Enter(zgrb,sigmagrb,sigmareco,sigmatot);
     fBurstOffMatch.Enter(E,dt,dang,dt/(zgrb+1.));
     fBurstParameters->AddSignal(E,"EnergyOff");
    }
   } // End of loop over the background tracks of this patch
  } // End of loop over the patches

  // Generate the GRB related signal event(s) in the search window.
  // The GRB position gets Gaussian smeared to reflect the actual position.
  // The time difference between the gammas and the neutrinos gets corrected
  // for the GRB redshift and smeared by the detector time resolution.
  // The muon direction gets modified to account for the kinematical opening angle
  // w.r.t. the neutrino direction and Gaussian smeared by the detector angular resolution.

  // Prevent statistical overfluctuation in number of GRB signal events if requested by fGrbnu<0
  if (fGrbnu>=0 || nmugrb<int(fabs(fGrbnu)*float(fNgrbs)))
  {
   // Obtain actual GRB position
   rgrb2.Load(rgrb);
   SmearPosition(rgrb2,sigmagrb);

   nmu=int(fabs(fGrbnu));
   if (!nmu && fRan->Uniform()<fabs(fGrbnu)) nmu=1;
   for (Int_t imu=0; imu<nmu; imu++)
   {
    nmugrb++;
    if (!fInburst) // Neutrino and gamma production decoupled
    {
     if (fDtnus<0) // Sigma in units of T90
     {
      dt=fRan->Gauss(fDtnu,fabs(fDtnus)*t90grb);
     }
     else // Sigma in seconds
     {
      dt=fRan->Gauss(fDtnu,fDtnus);
     }
     dt=dt*(zgrb+1.);
    }
    else // Coupled neutrino and gamma production
    {
     if (fDtnus<0) // Sigma in units of T90
     {
      dt=fRan->Gauss(fDtnu*t90grb,fabs(fDtnus)*t90grb);
     }
     else // Sigma in seconds
     {
      dt=fRan->Gauss(fDtnu*t90grb,fDtnus);
     }
    }
    if (fTimres>0) dt=fRan->Gauss(dt,fTimres);

    // Convert dt from seconds to the selected Tunit
    dt=dt/fTfact; 

    // The direction of the GRB signal
    rmu.Load(rgrb2);

    // The energy of the GRB signal
    E=GetBurstSignalEnergy();

    if (hSigE) hSigE->Fill(E);

    // Reduce the energy due to the cosmological redshift effect
    if (fEzcor) E=E/(zgrb+1.);

    if (hSigEzcor) hSigEzcor->Fill(E);

    if (E<0 || E<fEmin || E>fEmax) continue;

    // Modification to account for the neutrino-lepton kinematic opening angle
    if (fKinangle>0)
    {
     Int_t mode=fKinangle-1;
     ang=GetNeutrinoAngle(E,"deg",mode);
     if (ang>0) ShiftPosition(rmu,ang);
    }

    // Smearing according to the reconstruction angular resolution
    sigmareco=GetBurstRecoAngres(E,E);
    if (sigmareco<0) sigmareco=fAngresfix;

    if (sigmareco<fAngresmin || sigmareco>fAngresmax) continue;

    if (hSigmaReco) hSigmaReco->Fill(sigmareco);

    SmearPosition(rmu,sigmareco);

    // Determine angular difference w.r.t. the presumed GRB position
    dang=rgrb.GetOpeningAngle(rmu,"deg");

    sigmatot=-1;
    if (fSumsigmas==-1) sigmatot=sigmareco;
    if (fSumsigmas==0) sigmatot=sigmagrb;
    if (fSumsigmas==1) sigmatot=sigmagrb+sigmareco;
    if (fSumsigmas==2) sigmatot=sqrt(sigmagrb*sigmagrb+sigmareco*sigmareco);

    // Determine the dynamic angular window including the track reco uncertainty
    if (!fixedwinset)
    {
     dangmax=-1;
     if (sigmatot>=0) dangmax=fabs(fDawin*sigmatot);
    }

    if (fDatype>=0 && dang>dangmax) continue;

    if (dangmax>dangmaxon) dangmaxon=dangmax;
    fBurstOnReco.Enter(zgrb,sigmagrb,sigmareco,sigmatot);
    fBurstOnMatch.Enter(E,dt,dang,dt/(zgrb+1.));
    fBurstSigReco.Enter(zgrb,sigmagrb,sigmareco,sigmatot);
    fBurstSignal.Enter(E,dt,dang,dt/(zgrb+1.));
    fBurstParameters->AddSignal(E,"EnergyOn");
    fBurstParameters->AddSignal(E,"EnergySig");
   }
  } // End of loop over the signal events of this burst

  if (fixedwinset) continue;

  // Store the dynamic On-source and Off-source maximum (solid) angles that are encountered for this burst
  if (fDawin<0) // Local zenith band
  {
   thlow=thetagrb-0.5*dangmaxon;
   thup=thetagrb+0.5*dangmaxon;
   solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
   sx->SetSignal(dangmaxon,"dangmaxOn");
   sx->SetSignal(solidangle,"OmegaOn");
   thlow=thetagrb-0.5*dangmaxoff;
   thup=thetagrb+0.5*dangmaxoff;
   solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
   sx->SetSignal(dangmaxoff,"dangmaxOff");
   sx->SetSignal(solidangle*float(fNbkg),"OmegaOff");
  }
  else // Circle around GRB position
  {
   thlow=0;
   thup=dangmaxon;
   solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
   sx->SetSignal(solidangle,"OmegaOn");
   sx->SetSignal(dangmaxon,"dangmaxOn");
   thup=dangmaxoff;
   solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
   sx->SetSignal(dangmaxoff,"dangmaxOff");
   sx->SetSignal(solidangle*float(fNbkg),"OmegaOff");
  }
 } // End of loop over the individual GRBs

 // Remove the temporary storage of the background event
 if (fNgrbs>0) RemoveSignal(fNgrbs+1,0,0);

 // Compensate statistical underfluctuation in number of GRB signal events if requested by fGrbnu<0
 if (fGrbnu<0) BurstCompensate(nmugrb);

 // Determine the On-source and Off-source total stacked solid angles that have been encountered
 name="SolidangleOn";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(0,name);
 name="SolidangleOff";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(0,name);
 for (Int_t igrb=1; igrb<=fNgrbs; igrb++)
 {
  sx=GetSignal(igrb);

  if (!sx) continue;

  solidangle=sx->GetSignal("OmegaOn");
  fBurstParameters->AddSignal(solidangle,"SolidangleOn");
  solidangle=sx->GetSignal("OmegaOff");
  fBurstParameters->AddSignal(solidangle,"SolidangleOff");
 }

 // Determine and list the burst statistics
 MakeBurstDataStats(0,nmugrb);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstDataStats(Int_t mode,Int_t nmugrb)
{
// Internal memberfunction to determine and list the burst-data statistics.
//
// mode : 0 --> Invoked from GenBurstSignals()
//        1 --> Invoked from MatchBurstData()
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Default value is nmugrb=0.

 Double_t pi=acos(-1.);

 // Retrieve the needed parameters
 Int_t fNgrbs=TMath::Nint(fBurstParameters->GetSignal("Ngrbs"));
 Int_t fNbkg=TMath::Nint(fBurstParameters->GetSignal("Nbkg"));
 Int_t fTunits=TMath::Nint(fBurstParameters->GetSignal("Tunits"));
 Float_t fTfact=fBurstParameters->GetSignal("Tfact");
 Float_t fTmin=fBurstParameters->GetSignal("Tmin");
 Float_t fTmax=fBurstParameters->GetSignal("Tmax");
 Int_t fTbint90=TMath::Nint(fBurstParameters->GetSignal("Tbint90"));
 Float_t fTbin=fBurstParameters->GetSignal("Tbin");
 Float_t fVarTbin=fBurstParameters->GetSignal("VarTbin");
 Float_t fAbin=fBurstParameters->GetSignal("Abin");
 Float_t fAvgrbz=fBurstParameters->GetSignal("Avgrbz");
 Float_t fAvgrbt90=fBurstParameters->GetSignal("Avgrbt90");
 Float_t fRbkgDecl=fBurstParameters->GetSignal("RbkgDecl");
 Float_t fSolidangleOn=fBurstParameters->GetSignal("SolidangleOn");
 Float_t fSolidangleOff=fBurstParameters->GetSignal("SolidangleOff");
 Float_t fEnergyOn=fBurstParameters->GetSignal("EnergyOn");
 Float_t fEnergyOff=fBurstParameters->GetSignal("EnergyOff");
 Float_t fEnergySig=fBurstParameters->GetSignal("EnergySig");
 Float_t fEnergyBkg=fEnergyOn-fEnergySig;
 Float_t fSensarea=fBurstParameters->GetSignal("Sensarea");

 TString tu="days";
 if (fTunits==1) tu="hours";
 if (fTunits==2) tu="sec";
 if (fTunits==3) tu="ns";
 if (fTunits==4) tu="ps";

 Int_t non=fBurstOnMatch.GetN();
 Int_t noff=fBurstOffMatch.GetN();
 Int_t nsig=fBurstSignal.GetN();
 Double_t TminOn=fTmin;
 Double_t TmaxOn=fTmax;
 Double_t TminOff=fTmin;
 Double_t TmaxOff=fTmax;

 if (fTmax<=fTmin)
 {
  TminOn=fBurstOnMatch.GetMinimum("dtime");
  TmaxOn=fBurstOnMatch.GetMaximum("dtime");
  TminOff=fBurstOffMatch.GetMinimum("dtime");
  TmaxOff=fBurstOffMatch.GetMaximum("dtime");
 }

 Double_t AminOn=fBurstOnMatch.GetMinimum("dang");
 Double_t AmaxOn=fBurstOnMatch.GetMaximum("dang");
 Double_t AminOff=fBurstOffMatch.GetMinimum("dang");
 Double_t AmaxOff=fBurstOffMatch.GetMaximum("dang");

 // Update the time window sizes and the average number of background events in them
 Float_t DtwinOn=TmaxOn-TminOn;
 Float_t DtwinOff=TmaxOff-TminOff;
 Float_t NbkgWinOn=fRbkgDecl*DtwinOn*fTfact;
 Float_t NbkgWinOff=fRbkgDecl*DtwinOff*fTfact;

 TString title;
 TString name;
 TString s;
 Float_t xmin=0;
 Float_t xmax=0;
 TAxis* axis=0;
 Int_t nbins=0;
 Double_t binsize=0;
 Double_t binsizecos=0;
 Int_t nabinsOn=0;
 Double_t abinsizeOn=0;
 Int_t nabinsOff=0;
 Double_t abinsizeOff=0;

 // The source redshift, position and reconstruction uncertainty histograms
 TH1F* hOnSourceZ=0;
 TH1F* hOffSourceZ=0;
 TH1F* hOnSigSourceZ=0;
 TH1F* hOnSigmaSource=0;
 TH1F* hOffSigmaSource=0;
 TH1F* hOnSigSigmaSource=0;
 TH1F* hOnSigmaReco=0;
 TH1F* hOffSigmaReco=0;
 TH1F* hOnSigSigmaReco=0;
 TH1F* hOnSigmaComb=0;
 TH1F* hOffSigmaComb=0;
 TH1F* hOnSigSigmaComb=0;
 nbins=100;
 if (fBurstOnReco.GetN())
 {
  title.Form("On-Source object redshifts with matching event(s);Redshift;Counts");
  hOnSourceZ=new TH1F("hOnSourceZ",title,nbins,1,0);
  hOnSourceZ->SetBuffer(non);

  title.Form("On-Source object position uncertainties with matching event(s);Object position angular uncertainty (sigma in degrees);Counts");
  hOnSigmaSource=new TH1F("hOnSigmaSource",title,nbins,1,0);
  hOnSigmaSource->SetBuffer(non);

  title.Form("On-source event reconstruction uncertainties in the final sample;Event angular reconstruction uncertainty (sigma in degrees);Counts");
  hOnSigmaReco=new TH1F("hOnSigmaReco",title,nbins,1,0);
  hOnSigmaReco->SetBuffer(non);

  title.Form("On-source combined object position and event reconstruction uncertainty;Combined object position and event reco angular uncertainty (sigma in degrees);Counts");
  hOnSigmaComb=new TH1F("hOnSigmaComb",title,nbins,1,0);
  hOnSigmaComb->SetBuffer(non);
 }

 if (fBurstOffReco.GetN())
 {
  title.Form("Off-Source object redshifts with matching event(s);Redshift;Counts");
  hOffSourceZ=new TH1F("hOffSourceZ",title,nbins,1,0);
  hOffSourceZ->SetBuffer(noff);

  title.Form("Off-Source object position uncertainties with matching event(s);Object position angular uncertainty (sigma in degrees);Counts");
  hOffSigmaSource=new TH1F("hOffSigmaSource",title,nbins,1,0);
  hOffSigmaSource->SetBuffer(noff);

  title.Form("Off-source event reconstruction uncertainties in the final sample;Event angular reconstruction uncertainty (sigma in degrees);Counts");
  hOffSigmaReco=new TH1F("hOffSigmaReco",title,nbins,1,0);
  hOffSigmaReco->SetBuffer(noff);

  title.Form("Off-source combined object position and event reconstruction uncertainty;Combined object position and event reco angular uncertainty (sigma in degrees);Counts");
  hOffSigmaComb=new TH1F("hOffSigmaComb",title,nbins,1,0);
  hOffSigmaComb->SetBuffer(noff);
 }

 if (fBurstSigReco.GetN())
 {
  title.Form("On-Source object redshifts with matching simulated signal event(s);Redshift;Counts");
  hOnSigSourceZ=new TH1F("hOnSigSourceZ",title,nbins,1,0);
  hOnSigSourceZ->SetBuffer(nsig);

  title.Form("On-Source object position uncertainties with matching simulated signal event(s);Object position angular uncertainty (sigma in degrees);Counts");
  hOnSigSigmaSource=new TH1F("hOnSigSigmaSource",title,nbins,1,0);
  hOnSigSigmaSource->SetBuffer(nsig);

  title.Form("On-source simulated signal event reconstruction uncertainties in the final sample;Event angular reconstruction uncertainty (sigma in degrees);Counts");
  hOnSigSigmaReco=new TH1F("hOnSigSigmaReco",title,nbins,1,0);
  hOnSigSigmaReco->SetBuffer(nsig);

  title.Form("On-source combined object position and simulated signal event reconstruction uncertainty;Combined object position and event reco angular uncertainty (sigma in degrees);Counts");
  hOnSigSigmaComb=new TH1F("hOnSigSigmaComb",title,nbins,1,0);
  hOnSigSigmaComb->SetBuffer(nsig);
 }

 // The energy histograms
 TH1F* hOnE=0;
 TH1F* hOffE=0;
 TH1F* hOnSigE=0;
 nbins=1000;
 if (non)
 {
  title.Form("On-source reconstructed event energy in the final sample;Event energy in GeV;Counts");
  if (!mode) title.ReplaceAll("reconstructed","simulated");
  hOnE=new TH1F("hOnE",title,nbins,1,0);
 }
 if (noff)
 {
  title.Form("Off-source reconstructed event energy in the final sample;Event energy in GeV;Counts");
  if (!mode) title.ReplaceAll("reconstructed","simulated");
  hOffE=new TH1F("hOffE",title,nbins,1,0);
 }
 if (nsig)
 {
  title.Form("On-source simulated signal event energy in the final sample;Event energy in GeV;Counts");
  hOnSigE=new TH1F("hOnSigE",title,nbins,1,0);
 }

 // Additional text for histo titles to indicate scrambling of the corresponding data
 TString scrt=""; // Time scrambling
 TString scrp=""; // Position scrambling
 if (fTscmode>0) scrt="(scrambled)";
 if (fRscmode>0 || fTscmode==3) scrp="(scrambled)";

 // Create the angular separation histograms
 TH1F* hOna=0;
 TH1F* hOffa=0;
 TH1F* hOnSiga=0;
 TH1F* hOnCosa=0;
 TH1F* hOffCosa=0;
 TH1F* hOnSigcosa=0;
 if (non && fAbin)
 {
  if (fAbin>0)
  {
   binsize=fAbin;
   nbins=int((AmaxOn-AminOn)/binsize);
  }
  else
  {
   nbins=int(((AmaxOn-AminOn)/180.)*NbkgWinOn*float(fNgrbs)/fabs(fAbin));
   if (nbins) binsize=(AmaxOn-AminOn)/float(nbins);
  }
  if (nbins)
  {
   title.Form("Reconstructed %-s opening angle of on-source events in time window;Opening angle (degrees);Counts per %-.3g degrees",scrp.Data(),binsize);
   hOna=new TH1F("hOna",title,nbins+1,AminOn,AmaxOn+binsize);
  }
  else
  {
   nbins=100;
   binsize=AmaxOn/float(nbins);
   title.Form("Reconstructed %-s opening angle of on-source events in time window;Opening angle (degrees);Counts per %-.3g degrees",scrp.Data(),binsize);
   hOna=new TH1F("hOna",title,nbins+2,AminOn-binsize,AmaxOn+binsize);
  }
  nabinsOn=nbins;
  abinsizeOn=binsize;
  if (nbins) binsizecos=(cos(AminOn*pi/180.)-cos(AmaxOn*pi/180.))/float(nbins);
  if (binsizecos>0)
  {
   title.Form("Reconstructed %-s cos(opening angle) of on-source events in time window;cos(opening angle);Counts per %-.3g",scrp.Data(),binsizecos);
   hOnCosa=new TH1F("hOnCosa",title,nbins+1,cos(AmaxOn*pi/180.),cos(AminOn*pi/180.)+binsizecos);
  }
  else
  {
   nbins=100;
   binsizecos=fabs(cos(AmaxOn*pi/180.))/float(nbins);
   title.Form("Reconstructed %-s cos(opening angle) of on-source events in time window;cos(opening angle);Counts per %-.3g",scrp.Data(),binsizecos);
   hOnCosa=new TH1F("hOnCosa",title,nbins+2,cos(AmaxOn*pi/180.)-binsizecos,cos(AminOn*pi/180.)+binsizecos);
  }
 }
 if (nsig && nbins)
 {
  title.Form("Reconstructed opening angle of on-source simulated signal events in time window;Opening angle (degrees);Counts per %-.3g degrees",binsize);
  hOnSiga=new TH1F("hOnSiga",title,nbins+2,AminOn-binsize,AmaxOn+binsize);
  title.Form("Reconstructed cos(opening angle) of on-source simulated signal events in time window;cos(opening angle);Counts per %-.3g",binsizecos);
  hOnSigcosa=new TH1F("hOnSigcosa",title,nbins+2,cos(AmaxOn*pi/180.)-binsizecos,cos(AminOn*pi/180.)+binsizecos);
 }
 nbins=0;
 binsize=0;
 binsizecos=0;
 if (noff && fAbin)
 {
  if (fAbin>0)
  {
   binsize=fAbin;
   nbins=int((AmaxOff-AminOff)/binsize);
  }
  else
  {
   nbins=int(((AmaxOff-AminOff)/180.)*NbkgWinOff*float(fNgrbs)*float(fNbkg)/fabs(fAbin));
   if (nbins) binsize=(AmaxOff-AminOff)/float(nbins);
  }
  if (nbins)
  {
   title.Form("Reconstructed opening angle of off-source events in time window;Opening angle (degrees);Counts per %-.3g degrees",binsize);
   hOffa=new TH1F("hOffa",title,nbins+1,AminOff,AmaxOff+binsize);
  }
  else
  {
   nbins=100;
   binsize=AmaxOff/float(nbins);
   title.Form("Reconstructed opening angle of off-source events in time window;Opening angle (degrees);Counts per %-.3g degrees",binsize);
   hOffa=new TH1F("hOffa",title,nbins+2,AminOff-binsize,AmaxOff+binsize);
  }
  nabinsOff=nbins;
  abinsizeOff=binsize;
  if (nbins) binsizecos=(cos(AminOff*pi/180.)-cos(AmaxOff*pi/180.))/float(nbins);
  if (binsizecos>0)
  {
   title.Form("Reconstructed cos(opening angle) of off-source events in time window;cos(opening angle);Counts per %-.3g",binsizecos);
   hOffCosa=new TH1F("hOffCosa",title,nbins+1,cos(AmaxOff*pi/180.),cos(AminOff*pi/180.)+binsizecos);
  }
  else
  {
   nbins=100;
   binsizecos=fabs(cos(AmaxOff*pi/180.))/float(nbins);
   title.Form("Reconstructed cos(opening angle) of off-source events in time window;cos(opening angle);Counts per %-.3g",binsizecos);
   hOffCosa=new TH1F("hOffCosa",title,nbins+2,cos(AmaxOff*pi/180.)-binsizecos,cos(AminOff*pi/180.)+binsizecos);
  }
 }

 // Create the arrival time histograms
 TH1F* hOnt=0;
 TH1F* hOfft=0;
 TH1F* hOnSigt=0;
 TH2F* hOnta=0;
 TH2F* hOffta=0;
 TH2F* hOnSigta=0;
 TH2F* hOnZta=0;
 TH2F* hOffZta=0;
 TH2F* hOnSigZta=0;

 // The redshift corrected arrival time histograms
 TH1F* hOnZt=0;
 TH1F* hOffZt=0;
 TH1F* hOnSigZt=0;

 nbins=0;
 binsize=0;
 TString addtitle;
 addtitle.Form(" (=%-g*<T90>)",fTbin);
 if (fTbin<0) addtitle.Form(" (~%-g counts/bin)",fabs(fTbin));
 if (fabs(fTbin)>0) // Fixed time bins
 {
  // Automatic time binning to get the specified maximal bkg counts per bin
  nbins=int(NbkgWinOn*float(fNgrbs)/fabs(fTbin));
  Int_t temp=int(NbkgWinOff*float(fNgrbs)*float(fNbkg)/fabs(fTbin));
  if (temp>nbins) nbins=temp;
  if (nbins) binsize=DtwinOn/float(nbins);

  if (fTbin>0) // User defined time bin size
  {
   binsize=fTbin;
   if (fTbint90) binsize=fTbin*fabs(fAvgrbt90)/fTfact;
   nbins=DtwinOn/binsize;
  }
  if (nbins && non)
  {
   title.Form("Arrival times %-s of on-source events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per %-.3g %-s",scrt.Data(),tu.Data(),binsize,tu.Data());
   if (fTbint90 || fTbin<0) title+=addtitle;
   hOnt=new TH1F("hOnt",title,nbins+2,TminOn-binsize,TmaxOn+binsize);
   title.Form("Arrival time %-s vs. reconstructed %-s opening angle of on-source events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",scrt.Data(),scrp.Data(),tu.Data());
   hOnta=new TH2F("hOnta",title,nabinsOn+2,AminOn-abinsizeOn,AmaxOn+abinsizeOn,nbins+2,TminOn-binsize,TmaxOn+binsize);
   title.Form("Redshift corrected arrival times %-s of on-source events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per %-.3g %-s",scrt.Data(),tu.Data(),binsize,tu.Data());
   if (fTbint90 || fTbin<0) title+=addtitle;
   hOnZt=new TH1F("hOnZt",title,nbins+2,TminOn-binsize,TmaxOn+binsize);
   title.Form("Redshift corrected %-s arrival time vs. reconstructed %-s opening angle of on-source events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",scrt.Data(),scrp.Data(),tu.Data());
   hOnZta=new TH2F("hOnZta",title,nabinsOn+2,AminOn-abinsizeOn,AmaxOn+abinsizeOn,nbins+2,TminOn-binsize,TmaxOn+binsize);
  }
  if (nbins && nsig)
  {
   title.Form("Arrival times of on-source simulated signal events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per %-.3g %-s",tu.Data(),binsize,tu.Data());
   if (fTbint90 || fTbin<0) title+=addtitle;
   hOnSigt=new TH1F("hOnSigt",title,nbins+2,TminOn-binsize,TmaxOn+binsize);
   title.Form("Arrival time vs. reconstructed opening angle of on-source simulated signal events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",tu.Data());
   hOnSigta=new TH2F("hOnSigta",title,nabinsOn+2,AminOn-abinsizeOn,AmaxOn+abinsizeOn,nbins+2,TminOn-binsize,TmaxOn+binsize);
   title.Form("Redshift corrected arrival times of on-source simulated signal events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per %-.3g %-s",tu.Data(),binsize,tu.Data());
   if (fTbint90 || fTbin<0) title+=addtitle;
   hOnSigZt=new TH1F("hOnSigZt",title,nbins+2,TminOn-binsize,TmaxOn+binsize);
   title.Form("Redshift corrected arrival time vs. reconstructed opening angle of on-source simulated signal events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",tu.Data());
   hOnSigZta=new TH2F("hOnSigZta",title,nabinsOn+2,AminOn-abinsizeOn,AmaxOn+abinsizeOn,nbins+2,TminOn-binsize,TmaxOn+binsize);
  }

  if (fTbin>0) // User defined time bin size
  {
   binsize=fTbin;
   if (fTbint90) binsize=fTbin*fabs(fAvgrbt90)/fTfact;
   nbins=DtwinOff/binsize;
  }
  if (nbins && noff)
  {
   title.Form("Arrival times of off-source events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per %-.3g %-s",tu.Data(),binsize,tu.Data());
   if (fTbint90 || fTbin<0) title+=addtitle;
   hOfft=new TH1F("hOfft",title,nbins+2,TminOff-binsize,TmaxOff+binsize);
   title.Form("Arrival time vs. reconstructed opening angle of off-source events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",tu.Data());
   hOffta=new TH2F("hOffta",title,nabinsOff+2,AminOff-abinsizeOff,AmaxOff+abinsizeOff,nbins+2,TminOff-binsize,TmaxOff+binsize);
   title.Form("Redshift corrected arrival times of off-source events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per %-.3g %-s",tu.Data(),binsize,tu.Data());
   if (fTbint90 || fTbin<0) title+=addtitle;
   hOffZt=new TH1F("hOffZt",title,nbins+2,TminOff-binsize,TmaxOff+binsize);
   title.Form("Redshift corrected arrival time vs. reconstructed opening angle of off-source events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",tu.Data());
   hOffZta=new TH2F("hOffZta",title,nabinsOff+2,AminOff-abinsizeOff,AmaxOff+abinsizeOff,nbins+2,TminOff-binsize,TmaxOff+binsize);
  }
 }
 else // Variable time bins
 {
  Double_t* binarr=0;
  Int_t nbx=int(DtwinOn/fVarTbin);
  Float_t gamma=fabs(fAvgrbz)+1.;
  Float_t* bins=new Float_t[nbx];
  nbins=0;
  Float_t xlow=0,xup=0,size=fVarTbin;
  for (Int_t i=0; i<nbx-1; i++)
  {
   xup=xlow+size;
   if (xup>DtwinOn/2.) // Store the last lowerbound
   {
    bins[i]=xlow;
    nbins++;
    break;
   }
   bins[i]=xlow;
   nbins++;
   xlow=xup;
   size=xlow*gamma;
  }
  binarr=new Double_t[2*nbins-1];
  for (Int_t j=nbins; j>0; j--)
  {
   binarr[nbins-j]=-bins[j-1];
   binarr[nbins+j-2]=bins[j-1];
  }
  nbins=2*nbins-2;
  if (nbins && non)
  {
   title.Form("Arrival times %-s of on-source events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per time bin",scrt.Data(),tu.Data());
   hOnt=new TH1F("hOnt",title,nbins,binarr);
   if (nabinsOn)
   {
    title.Form("Arrival time %-s vs. reconstructed %-s opening angle of on-source events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",scrt.Data(),scrp.Data(),tu.Data());
    hOnta=new TH2F("hOnta",title,nabinsOn,AminOn,AmaxOn,nbins,binarr);
   }
  }
  if (nbins && noff)
  {
   title.Form("Arrival times of off-source events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per time bin",tu.Data());
   hOfft=new TH1F("hOfft",title,nbins,binarr);
   if (nabinsOff)
   {
    title.Form("Arrival time vs. reconstructed opening angle of off-source events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",tu.Data());
    hOffta=new TH2F("hOffta",title,nabinsOff,AminOff,AmaxOff,nbins,binarr);
   }
  }
  if (nbins && nsig)
  {
   title.Form("Arrival times of on-source simulated signal events in time window;Event arrival time (in %-s) w.r.t. burst trigger;Counts per time bin",tu.Data());
   hOnSigt=new TH1F("hOnSigt",title,nbins,binarr);
   if (nabinsOn)
   {
    title.Form("Arrival time vs. reconstructed opening angle of on-source simulated signal events in time window;Opening angle (degrees);Event arrival time (in %-s) w.r.t. burst trigger",tu.Data());
    hOnSigta=new TH2F("hOnSigta",title,nabinsOn,AminOn,AmaxOn,nbins,binarr);
   }
  }

  delete[] binarr;
 }

 // Fill the histograms
 Double_t value1=0;
 Double_t value2=0;
 Double_t value3=0;
 Double_t value4=0;

 // The on-source data
 for (Int_t i=1; i<=fBurstOnReco.GetN(); i++)
 {
  value1=fBurstOnReco.GetEntry(i,"zburst");
  value2=fBurstOnReco.GetEntry(i,"sigmaburst");
  value3=fBurstOnReco.GetEntry(i,"sigmareco");
  value4=fBurstOnReco.GetEntry(i,"sigmacomb");
  if (hOnSourceZ) hOnSourceZ->Fill(value1);
  if (hOnSigmaSource) hOnSigmaSource->Fill(value2);
  if (hOnSigmaReco) hOnSigmaReco->Fill(value3);
  if (hOnSigmaComb) hOnSigmaComb->Fill(value4);
 }

 for (Int_t i=1; i<=fBurstOnMatch.GetN(); i++)
 {
  value1=fBurstOnMatch.GetEntry(i,"E");
  value2=fBurstOnMatch.GetEntry(i,"dang");
  value3=fBurstOnMatch.GetEntry(i,"dtime");
  value4=fBurstOnMatch.GetEntry(i,"dtimez");
  if (hOnE) hOnE->Fill(value1);
  if (hOna) hOna->Fill(value2);
  if (hOnCosa) hOnCosa->Fill(cos(value2*pi/180.));
  if (hOnt) hOnt->Fill(value3);
  if (hOnta) hOnta->Fill(value2,value3);
  if (hOnZt) hOnZt->Fill(value4);
  if (hOnZta) hOnZta->Fill(value2,value4);
 }

 // The off-source data
 for (Int_t i=1; i<=fBurstOffReco.GetN(); i++)
 {
  value1=fBurstOffReco.GetEntry(i,"zburst");
  value2=fBurstOffReco.GetEntry(i,"sigmaburst");
  value3=fBurstOffReco.GetEntry(i,"sigmareco");
  value4=fBurstOffReco.GetEntry(i,"sigmacomb");
  if (hOffSourceZ) hOffSourceZ->Fill(value1);
  if (hOffSigmaSource) hOffSigmaSource->Fill(value2);
  if (hOffSigmaReco) hOffSigmaReco->Fill(value3);
  if (hOffSigmaComb) hOffSigmaComb->Fill(value4);
 }

 for (Int_t i=1; i<=fBurstOffMatch.GetN(); i++)
 {
  value1=fBurstOffMatch.GetEntry(i,"E");
  value2=fBurstOffMatch.GetEntry(i,"dang");
  value3=fBurstOffMatch.GetEntry(i,"dtime");
  value4=fBurstOffMatch.GetEntry(i,"dtimez");
  if (hOffE) hOffE->Fill(value1);
  if (hOffa) hOffa->Fill(value2);
  if (hOffCosa) hOffCosa->Fill(cos(value2*pi/180.));
  if (hOfft) hOfft->Fill(value3);
  if (hOffta) hOffta->Fill(value2,value3);
  if (hOffZt) hOffZt->Fill(value4);
  if (hOffZta) hOffZta->Fill(value2,value4);
 }

 // The simulated signal data
 for (Int_t i=1; i<=fBurstSigReco.GetN(); i++)
 {
  value1=fBurstSigReco.GetEntry(i,"zburst");
  value2=fBurstSigReco.GetEntry(i,"sigmaburst");
  value3=fBurstSigReco.GetEntry(i,"sigmareco");
  value4=fBurstSigReco.GetEntry(i,"sigmacomb");
  if (hOnSigSourceZ) hOnSigSourceZ->Fill(value1);
  if (hOnSigSigmaSource) hOnSigSigmaSource->Fill(value2);
  if (hOnSigSigmaReco) hOnSigSigmaReco->Fill(value3);
  if (hOnSigSigmaComb) hOnSigSigmaComb->Fill(value4);
 }

 for (Int_t i=1; i<=fBurstSignal.GetN(); i++)
 {
  value1=fBurstSignal.GetEntry(i,"E");
  value2=fBurstSignal.GetEntry(i,"dang");
  value3=fBurstSignal.GetEntry(i,"dtime");
  value4=fBurstSignal.GetEntry(i,"dtimez");
  if (hOnSigE) hOnSigE->Fill(value1);
  if (hOnSiga) hOnSiga->Fill(value2);
  if (hOnSigcosa) hOnSigcosa->Fill(cos(value2*pi/180.));
  if (hOnSigt) hOnSigt->Fill(value3);
  if (hOnSigta) hOnSigta->Fill(value2,value3);
  if (hOnSigZt) hOnSigZt->Fill(value4);
  if (hOnSigZta) hOnSigZta->Fill(value2,value4);
 }

 // Bayesian block analysis of the arrival times
 NcBlocks BB;
 TH1F hOnBBt;
 TH1F hOnBBzt;
 if (fBurstOnMatch.GetN())
 {
  BB.GetBlocks(fBurstOnMatch,"dtime",0.05,&hOnBBt);
  title.Form("Bayesian blocks for on-source arrival times %-s;Event arrival time (in %-s) w.r.t. burst trigger;Event rate (%-s^{-1}) for %-i stacked time windows",scrt.Data(),tu.Data(),tu.Data(),fNgrbs);
  title.ReplaceAll("days^","day^");
  title.ReplaceAll("hours^","hour^");
  hOnBBt.SetNameTitle("hOnBBt",title);
  BB.GetBlocks(fBurstOnMatch,"dtimez",0.05,&hOnBBzt);
  title.Form("Bayesian blocks for redshift corrected on-source arrival times %-s;Event arrival time (in %-s) w.r.t. burst trigger;Event rate (%-s^{-1}) for %-i stacked time windows",scrt.Data(),tu.Data(),tu.Data(),fNgrbs);
  title.ReplaceAll("days^","day^");
  title.ReplaceAll("hours^","hour^");
  hOnBBzt.SetNameTitle("hOnBBzt",title);
 }
 TH1F hOffBBt;
 TH1F hOffBBzt;
 if (fBurstOffMatch.GetN())
 {
  BB.GetBlocks(fBurstOffMatch,"dtime",0.05,&hOffBBt);
  title.Form("Bayesian blocks for off-source arrival times;Event arrival time (in %-s) w.r.t. burst trigger;Event rate (%-s^{-1}) for %-i stacked time windows",tu.Data(),tu.Data(),fNgrbs*fNbkg);
  title.ReplaceAll("days^","day^");
  title.ReplaceAll("hours^","hour^");
  hOffBBt.SetNameTitle("hOffBBt",title);
  BB.GetBlocks(fBurstOffMatch,"dtimez",0.05,&hOffBBzt);
  title.Form("Bayesian blocks for redshift corrected off-source arrival times;Event arrival time (in %-s) w.r.t. burst trigger;Event rate (%-s^{-1}) for %-i stacked time windows",tu.Data(),tu.Data(),fNgrbs*fNbkg);
  title.ReplaceAll("days^","day^");
  title.ReplaceAll("hours^","hour^");
  hOffBBzt.SetNameTitle("hOffBBzt",title);
 }

 // Ratio On/Off for the Bayesian Block histograms
 Float_t temp=0;
 Int_t nb1=0;
 Int_t nb2=0;

 axis=hOnBBt.GetXaxis();
 xmin=axis->GetXmin();
 xmax=axis->GetXmax();
 axis=hOffBBt.GetXaxis();
 temp=axis->GetXmin();
 if (temp<xmin) xmin=temp;
 temp=axis->GetXmax();
 if (temp>xmax) xmax=temp;

 TH1F hOnUBBt;
 TH1F hOffUBBt;
 TH1F hRatUBBt;
 title.Form("Event rate (%-s^{-1}) scaled per time window",tu.Data());
 title.ReplaceAll("days^","day^");
 title.ReplaceAll("hours^","hour^");
 nb1=BB.Rebin(&hOnBBt,&hOnUBBt,kFALSE,0,xmin,xmax);
 nb2=BB.Rebin(&hOffBBt,&hOffUBBt,kFALSE,0,xmin,xmax);
 if (nb2>nb1) BB.Rebin(&hOnBBt,&hOnUBBt,kFALSE,nb2,xmin,xmax);
 hOnUBBt.SetName("hOnUBBt");
 hOffUBBt.SetName("hOffUBBt");
 if (fNgrbs>1) hOnUBBt.Scale(1./float(fNgrbs));
 if (fNgrbs*fNbkg>1) hOffUBBt.Scale(1./float(fNgrbs*fNbkg));
 hOnUBBt.SetYTitle(title);
 hOffUBBt.SetYTitle(title);
 BB.Divide(&hOnUBBt,&hOffUBBt,&hRatUBBt,kFALSE,1);
 hRatUBBt.SetName("hRatUBBt");
 hRatUBBt.SetYTitle("Ratio");

 nbins=hOnBBzt.GetNbinsX();
 if (hOffBBzt.GetNbinsX()>nbins) nbins=hOffBBzt.GetNbinsX();
 axis=hOnBBzt.GetXaxis();
 xmin=axis->GetXmin();
 xmax=axis->GetXmax();
 axis=hOffBBzt.GetXaxis();
 temp=axis->GetXmin();
 if (temp<xmin) xmin=temp;
 temp=axis->GetXmax();
 if (temp>xmax) xmax=temp;

 TH1F hOnUBBzt;
 TH1F hOffUBBzt;
 TH1F hRatUBBzt;
 nb1=BB.Rebin(&hOnBBzt,&hOnUBBzt,kFALSE,0,xmin,xmax);
 nb2=BB.Rebin(&hOffBBzt,&hOffUBBzt,kFALSE,0,xmin,xmax);
 if (nb2>nb1) BB.Rebin(&hOnBBzt,&hOnUBBzt,kFALSE,nb2,xmin,xmax);
 hOnUBBzt.SetName("hOnUBBzt");
 hOffUBBzt.SetName("hOffUBBzt");
 if (fNgrbs>1) hOnUBBzt.Scale(1./float(fNgrbs));
 if (fNgrbs*fNbkg>1) hOffUBBzt.Scale(1./float(fNgrbs*fNbkg));
 hOnUBBzt.SetYTitle(title);
 hOffUBBzt.SetYTitle(title);
 BB.Divide(&hOnUBBzt,&hOffUBBzt,&hRatUBBzt,kFALSE,1);
 hRatUBBzt.SetName("hRatUBBzt");
 hRatUBBzt.SetYTitle("Ratio");

 // Store the produced histograms
 if (hOnSourceZ) fBurstHistos.Add(hOnSourceZ);
 if (hOffSourceZ) fBurstHistos.Add(hOffSourceZ);
 if (hOnSigSourceZ) fBurstHistos.Add(hOnSigSourceZ);
 if (hOnSigmaSource) fBurstHistos.Add(hOnSigmaSource);
 if (hOffSigmaSource) fBurstHistos.Add(hOffSigmaSource);
 if (hOnSigSigmaSource) fBurstHistos.Add(hOnSigSigmaSource);
 if (hOnSigmaReco) fBurstHistos.Add(hOnSigmaReco);
 if (hOffSigmaReco) fBurstHistos.Add(hOffSigmaReco);
 if (hOnSigSigmaReco) fBurstHistos.Add(hOnSigSigmaReco);
 if (hOnSigmaComb) fBurstHistos.Add(hOnSigmaComb);
 if (hOffSigmaComb) fBurstHistos.Add(hOffSigmaComb);
 if (hOnSigSigmaComb) fBurstHistos.Add(hOnSigSigmaComb);
 if (hOnE) fBurstHistos.Add(hOnE);
 if (hOffE) fBurstHistos.Add(hOffE);
 if (hOnSigE) fBurstHistos.Add(hOnSigE);
 if (hOna) fBurstHistos.Add(hOna);
 if (hOffa) fBurstHistos.Add(hOffa);
 if (hOnSiga) fBurstHistos.Add(hOnSiga);
 if (hOnCosa) fBurstHistos.Add(hOnCosa);
 if (hOffCosa) fBurstHistos.Add(hOffCosa);
 if (hOnSigcosa) fBurstHistos.Add(hOnSigcosa);
 if (hOnt) fBurstHistos.Add(hOnt);
 if (hOfft) fBurstHistos.Add(hOfft);
 if (hOnSigt) fBurstHistos.Add(hOnSigt);
 if (hOnta) fBurstHistos.Add(hOnta);
 if (hOffta) fBurstHistos.Add(hOffta);
 if (hOnSigta)fBurstHistos.Add(hOnSigta);
 if (hOnt && hOnBBt.GetEntries()) fBurstHistos.Add(hOnBBt.Clone());
 if (hOfft && hOffBBt.GetEntries()) fBurstHistos.Add(hOffBBt.Clone());
 if (hOnt && hOnUBBt.GetEntries()) fBurstHistos.Add(hOnUBBt.Clone());
 if (hOfft && hOffUBBt.GetEntries()) fBurstHistos.Add(hOffUBBt.Clone());
 if (hOnt && hOfft && hRatUBBt.GetEntries()) fBurstHistos.Add(hRatUBBt.Clone());
 if (hOnZt) fBurstHistos.Add(hOnZt);
 if (hOffZt) fBurstHistos.Add(hOffZt);
 if (hOnSigZt) fBurstHistos.Add(hOnSigZt);
 if (hOnZta) fBurstHistos.Add(hOnZta);
 if (hOffZta) fBurstHistos.Add(hOffZta);
 if (hOnSigZta) fBurstHistos.Add(hOnSigZta);
 if (hOnZt && hOnBBzt.GetEntries()) fBurstHistos.Add(hOnBBzt.Clone());
 if (hOffZt && hOffBBzt.GetEntries()) fBurstHistos.Add(hOffBBzt.Clone());
 if (hOnZt && hOnUBBzt.GetEntries()) fBurstHistos.Add(hOnUBBzt.Clone());
 if (hOffZt && hOffUBBzt.GetEntries()) fBurstHistos.Add(hOffUBBzt.Clone());
 if (hOnZt && hOffZt && hRatUBBzt.GetEntries()) fBurstHistos.Add(hRatUBBzt.Clone());

 // Make sure that also the auto-binned histograms have their axes ranges set
 Int_t nh=fBurstHistos.GetEntries();
 for (Int_t ih=0; ih<nh; ih++)
 {
  TH1* hx=(TH1*)fBurstHistos.At(ih);
  if (!hx) continue;
  hx->BufferEmpty(1);

  name=hx->GetName();

  if (name=="hTdiff")
  {
   binsize=hx->GetBinWidth(1);
   s.Form("Counts per %-.3g %-s",binsize,tu.Data());
   axis=hx->GetYaxis();
   if (axis) axis->SetTitle(s);
  }

  if (name=="hAdiff" || name.Contains("Sigma"))
  {
   binsize=hx->GetBinWidth(1);
   s.Form("Counts per %-.3g degrees",binsize);
   axis=hx->GetYaxis();
   if (axis) axis->SetTitle(s);
  }

  if (name=="hOnE" || name=="hOffE" || name=="hOnSigE")
  {
   binsize=hx->GetBinWidth(1);
   s.Form("Counts per %-.3g GeV",binsize);
   axis=hx->GetYaxis();
   if (axis) axis->SetTitle(s);
  }
 }

 // Determination of the on-source and off-source event rates
 Float_t nOn=non;
 Float_t nOff=noff;
 Float_t nsigOn=nsig;
 Float_t nbkgOn=nOn-nsigOn;
 Float_t Ton=DtwinOn*fTfact*float(fNgrbs);                // Total on-source exposure time in seconds
 Float_t Toff=DtwinOff*fTfact*float(fNgrbs)*float(fNbkg); // Total off-source exposure time in seconds
 Float_t rateOn=nOn/(DtwinOn*fTfact);
 Float_t rateOff=nOff/(DtwinOff*fTfact);

 // (Average) values per patch
 Float_t TwinOn=DtwinOn*fTfact;
 Float_t TwinOff=DtwinOff*fTfact;
 Float_t AvSolidangleOn=0;
 Float_t AvSolidangleOff=0;
 Float_t AvNon=0;
 Float_t AvNoff=0;
 Float_t AvRon=0;
 Float_t AvRoff=0;
 Float_t AvEon=0;
 Float_t AvEoff=0;
 Float_t AvNsigOn=0;
 Float_t AvNbkgOn=0;
 Float_t AvRsigOn=0;
 Float_t AvRbkgOn=0;
 Float_t AvEsigOn=0;
 Float_t AvEbkgOn=0;
 Float_t scale=fNgrbs;
 if (scale)
 {
  AvSolidangleOn=fSolidangleOn/scale;
  AvNon=nOn/scale;
  AvRon=nOn/Ton;
  AvEon=fEnergyOn/scale;
  AvNsigOn=nsigOn/scale;
  AvNbkgOn=nbkgOn/scale;
  AvRsigOn=nsigOn/Ton;
  AvRbkgOn=nbkgOn/Ton;
  AvEsigOn=fEnergySig/scale;
  AvEbkgOn=fEnergyBkg/scale;
 }
 scale=fNgrbs*fNbkg;
 if (scale)
 {
  AvSolidangleOff=fSolidangleOff/scale;
  AvNoff=nOff/scale;
  AvRoff=nOff/Toff;
  AvEoff=fEnergyOff/scale;
 }

 // Statistics of the stacked event samples
 printf("\n *%-s::MakeBurstDataStats* Statistics of the stacked observed event samples. \n",ClassName());
 printf(" Integrated on-source  exposure time of the %-i stacked time windows : %-g %-s \n",fNgrbs,Ton/fTfact,tu.Data());
 printf(" Integrated off-source exposure time of the %-i*%-i stacked time windows : %-g %-s \n",fNgrbs,fNbkg,Toff/fTfact,tu.Data());
 printf(" Total accumulated on-source  solid angle : %-g sr in %-i stacked patches --> Average per patch : %-g sr \n",fSolidangleOn,fNgrbs,AvSolidangleOn); 
 printf(" Total accumulated off-source solid angle : %-g sr in %-i*%-i stacked patches --> Average per patch : %-g sr \n",fSolidangleOff,fNgrbs,fNbkg,AvSolidangleOff);
 if (fSensarea>0) printf(" Area covered c.q. overlooked by the detector sensors : %-g m^2. \n",fSensarea);
 fSensarea*=1e4; // Convert to cm^2

 printf(" *On source*  Total number of recorded on-source events : %-g --> Average per patch : %-g events. \n",nOn,AvNon);
 printf("              --- Average values per on-source patch --- \n");
 printf("              Steady event rate during the time window : %-g Hz",AvRon);
 if (AvSolidangleOn) printf(" --> %-g Hz sr^-1",AvRon/AvSolidangleOn);
 printf("\n");
 if (fSensarea>0)
 {
  printf("              Particle fluence : %-g cm^-2",AvNon/fSensarea);
  if (AvSolidangleOn) printf(" --> %-g cm^-2 sr^-1",AvNon/(fSensarea*AvSolidangleOn));
  printf("\n");
  printf("              Particle flux : %-g cm^-2 s^-1",AvRon/fSensarea);
  if (AvSolidangleOn) printf(" --> Intensity : %-g cm^-2 s^-1 sr^-1",AvRon/(fSensarea*AvSolidangleOn));
  printf("\n");
 }
 printf("              Cumulated energy : %-g GeV",AvEon);
 if (AvSolidangleOn) printf(" --> %-g GeV sr^-1",AvEon/AvSolidangleOn);
 printf("\n");
 printf("              Power : %-g GeV/s",AvEon/TwinOn);
 if (AvSolidangleOn) printf(" --> %-g GeV s^-1 sr^-1",AvEon/(TwinOn*AvSolidangleOn));
 printf("\n");
 if (fSensarea>0)
 {
  printf("              Energy fluence : %-g GeV cm^-2",AvEon/fSensarea);
  if (AvSolidangleOn) printf(" --> %-g GeV cm^-2 sr^-1",AvEon/(fSensarea*AvSolidangleOn));
  printf("\n");
  printf("              Energy flux : %-g GeV cm^-2 s^-1",AvEon/(fSensarea*TwinOn));
  if (AvSolidangleOn) printf(" --> Intensity : %-g GeV cm^-2 s^-1 sr^-1",AvEon/(fSensarea*TwinOn*AvSolidangleOn));
  printf("\n");
 }

 if (fNbkg)
 {
  printf(" *Off source* Total number of recorded off-source (background) events : %-g --> Average per patch : %-g events. \n",nOff,AvNoff);
  printf("              --- Average values per off-source patch --- \n");
  printf("              Steady event rate during the time window : %-g Hz",AvRoff);
  if (AvSolidangleOff) printf(" --> %-g Hz sr^-1",AvRoff/AvSolidangleOff);
  printf("\n");
  if (fSensarea>0)
  {
   printf("              Particle fluence : %-g cm^-2",AvNoff/fSensarea);
   if (AvSolidangleOff) printf(" --> %-g cm^-2 sr^-1",AvNoff/(fSensarea*AvSolidangleOff));
   printf("\n");
   printf("              Particle flux : %-g cm^-2 s^-1",AvRoff/fSensarea);
   if (AvSolidangleOff) printf(" --> Intensity : %-g cm^-2 s^-1 sr^-1",AvRoff/(fSensarea*AvSolidangleOff));
   printf("\n");
  }
  printf("              Cumulated energy : %-g GeV",AvEoff);
  if (AvSolidangleOff) printf(" --> %-g GeV sr^-1",AvEoff/AvSolidangleOff);
  printf("\n");
  printf("              Power : %-g GeV/s",AvEoff/TwinOff);
  if (AvSolidangleOff) printf(" --> %-g GeV s^-1 sr^-1",AvEoff/(TwinOn*AvSolidangleOff));
  printf("\n");
  if (fSensarea>0)
  {
   printf("              Energy fluence : %-g GeV cm^-2",AvEoff/fSensarea);
   if (AvSolidangleOff) printf(" --> %-g GeV cm^-2 sr^-1",AvEoff/(fSensarea*AvSolidangleOff));
   printf("\n");
   printf("              Energy flux : %-g GeV cm^-2 s^-1",AvEoff/(fSensarea*TwinOff));
   if (AvSolidangleOff) printf(" --> Intensity : %-g GeV cm^-2 s^-1 sr^-1",AvEoff/(fSensarea*TwinOff*AvSolidangleOff));
   printf("\n");
  }
 }

 // On-source signal and background info is only available for simulated data
 if (!mode)
 {
  printf(" -(Unknown)-  Total number of injected on-source signal events : %-i \n",nmugrb);
  printf("              Total number of recorded on-source signal events : %-g",nsigOn);
  if (fNgrbs) printf(" --> Average per patch : %-g events.",nsigOn/float(fNgrbs));
  printf("\n");
  printf("              Total number of recorded on-source bkg    events : %-g",nbkgOn);
  if (fNgrbs) printf(" --> Average per patch : %-g events.",nbkgOn/float(fNgrbs));
  printf("\n");

  printf("              --- Average signal values per on-source patch --- \n");
  printf("              Steady event rate during the time window : %-g Hz",AvRsigOn);
  if (AvSolidangleOn) printf(" --> %-g Hz sr^-1",AvRsigOn/AvSolidangleOn);
  printf("\n");
  if (fSensarea>0)
  {
   printf("              Particle fluence : %-g cm^-2",AvNsigOn/fSensarea);
   if (AvSolidangleOn) printf(" --> %-g cm^-2 sr^-1",AvNsigOn/(fSensarea*AvSolidangleOn));
   printf("\n");
   printf("              Particle flux : %-g cm^-2 s^-1",AvRsigOn/fSensarea);
   if (AvSolidangleOn) printf(" --> Intensity : %-g cm^-2 s^-1 sr^-1",AvRsigOn/(fSensarea*AvSolidangleOn));
   printf("\n");
  }
  printf("              Cumulated energy : %-g GeV",AvEsigOn);
  if (AvSolidangleOn) printf(" --> %-g GeV sr^-1",AvEsigOn/AvSolidangleOn);
  printf("\n");
  printf("              Power : %-g GeV/s",AvEsigOn/TwinOn);
  if (AvSolidangleOn) printf(" --> %-g GeV s^-1 sr^-1",AvEsigOn/(TwinOn*AvSolidangleOn));
  printf("\n");
  if (fSensarea>0)
  {
   printf("              Energy fluence : %-g GeV cm^-2",AvEsigOn/fSensarea);
   if (AvSolidangleOn) printf(" --> %-g GeV cm^-2 sr^-1",AvEsigOn/(fSensarea*AvSolidangleOn));
   printf("\n");
   printf("              Energy flux : %-g GeV cm^-2 s^-1",AvEsigOn/(fSensarea*TwinOn));
   if (AvSolidangleOn) printf(" --> Intensity : %-g GeV cm^-2 s^-1 sr^-1",AvEsigOn/(fSensarea*TwinOn*AvSolidangleOn));
   printf("\n");
  }
  printf("              --- Average background values per on-source patch --- \n");
  printf("              Steady event rate during the time window : %-g Hz",AvRbkgOn);
  if (AvSolidangleOn) printf(" --> %-g Hz sr^-1",AvRbkgOn/AvSolidangleOn);
  printf("\n");
  if (fSensarea>0)
  {
   printf("              Particle fluence : %-g cm^-2",AvNbkgOn/fSensarea);
   if (AvSolidangleOn) printf(" --> %-g cm^-2 sr^-1",AvNbkgOn/(fSensarea*AvSolidangleOn));
   printf("\n");
   printf("              Particle flux : %-g cm^-2 s^-1",AvRbkgOn/fSensarea);
   if (AvSolidangleOn) printf(" --> Intensity : %-g cm^-2 s^-1 sr^-1",AvRbkgOn/(fSensarea*AvSolidangleOn));
   printf("\n");
  }
  printf("              Cumulated energy : %-g GeV",AvEbkgOn);
  if (AvSolidangleOn) printf(" --> %-g GeV sr^-1",AvEbkgOn/AvSolidangleOn);
  printf("\n");
  printf("              Power : %-g GeV/s",AvEbkgOn/TwinOn);
  if (AvSolidangleOn) printf(" --> %-g GeV s^-1 sr^-1",AvEbkgOn/(TwinOn*AvSolidangleOn));
  printf("\n");
  if (fSensarea>0)
  {
   printf("              Energy fluence : %-g GeV cm^-2",AvEbkgOn/fSensarea);
   if (AvSolidangleOn) printf(" --> %-g GeV cm^-2 sr^-1",AvEbkgOn/(fSensarea*AvSolidangleOn));
   printf("\n");
   printf("              Energy flux : %-g GeV cm^-2 s^-1",AvEbkgOn/(fSensarea*TwinOn));
   if (AvSolidangleOn) printf(" --> Intensity : %-g GeV cm^-2 s^-1 sr^-1",AvEbkgOn/(fSensarea*TwinOn*AvSolidangleOn));
   printf("\n");
  }
 }
 printf("\n");

 // Update internal statistics
 fBurstParameters->AddNamedSlot("TminOn");
 fBurstParameters->SetSignal(TminOn,"TminOn");   
 fBurstParameters->AddNamedSlot("TmaxOn");
 fBurstParameters->SetSignal(TmaxOn,"TmaxOn");   
 fBurstParameters->AddNamedSlot("TminOff");
 fBurstParameters->SetSignal(TminOff,"TminOff");   
 fBurstParameters->AddNamedSlot("TmaxOff");
 fBurstParameters->SetSignal(TmaxOff,"TmaxOff");   
 fBurstParameters->AddNamedSlot("DtwinOn");
 fBurstParameters->SetSignal(DtwinOn,"DtwinOn");   
 fBurstParameters->AddNamedSlot("DtwinOff");
 fBurstParameters->SetSignal(DtwinOff,"DtwinOff");   
 fBurstParameters->AddNamedSlot("NbkgWinOn");
 fBurstParameters->SetSignal(NbkgWinOn,"NbkgWinOn");
 fBurstParameters->AddNamedSlot("NbkgWinOff");
 fBurstParameters->SetSignal(NbkgWinOff,"NbkgWinOff");
 fBurstParameters->AddNamedSlot("TtotOn");  // The on-source total exposure time in sec.
 fBurstParameters->SetSignal(Ton,"TtotOn");   
 fBurstParameters->AddNamedSlot("TtotOff"); // The off-source total exposure time ins sec.
 fBurstParameters->SetSignal(Toff,"TtotOff");   
 fBurstParameters->AddNamedSlot("AvSolidangleOn");  // The average on-source solid angle
 fBurstParameters->SetSignal(AvSolidangleOn,"AvSolidangleOn");   
 fBurstParameters->AddNamedSlot("AvSolidangleOff"); // The average off-source solid angle
 fBurstParameters->SetSignal(AvSolidangleOff,"AvSolidangleOff");   

 fBurstParameters->AddNamedSlot("rateOn");
 fBurstParameters->SetSignal(rateOn,"rateOn");
 fBurstParameters->AddNamedSlot("rateOff");
 fBurstParameters->SetSignal(rateOff,"rateOff");

 // Update old parameters
 fBurstParameters->SetSignal(TminOn,"Tmin");   
 fBurstParameters->SetSignal(TmaxOn,"Tmax");   
 fBurstParameters->SetSignal(DtwinOn,"Dtwin");   
 fBurstParameters->SetSignal(NbkgWinOn,"NbkgWin");
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MatchBurstData(NcDevice& matches,Int_t i1,Int_t i2,Int_t itype,Int_t j1,Int_t j2,Int_t jtype)
{
// Provide information about the matching in space and time of the stored
// reference signal(s) and/or measurement(s).
// Signals are regarded as matching if the positional angular difference
// and the absolute value of the time difference don't exceed the parameters
// as specified by SetBurstParameter().
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The information (see below) for the observed matches is provided via the NcDevice argument "matches",
// where each matching pair is recorded as a "hit" with the corresponding NcSignal data.
// Consequently, the number of matches can be obtained as the number of "hits" in the NcDevice.
//
// Note : This memberfunction will completely reset and re-define the NcDevice "matches".
//
// It is possible to investigate only a sub-set of the stored reference signals c.q. measurements
// via the input arguments "i1", "i2", "itype", "j1" and "j2", "jtype" as follows :
//
// i1    : Start index of the stored signals to be investigated (1=first stored signal)
// i2    : End index of the stored signals to be investigated (0=last stored signal)
// itype : 0 ==> [i1,i2] refer to stored reference signals
//         1 ==> [i1,i2] refer to stored measurements
// j1    : Start index of the stored signals to be investigated (1=first stored signal)
// j2    : End index of the stored signals to be investigated (0=last stored signal)
// jtype : 0 ==> [j1,j2] refer to stored reference signals
//         1 ==> [j1,j2] refer to stored measurements
//
// Note : Using itype=jtype allows investigation of (self)correlations within a sample. 
//        In this case the matching of a signal with itself will be skipped.
//
// The default values are i1=1, i2=0, itype=0, j1=1, j2=0 and jtype=1.
//
// The data for each matching signal pair provided as a "hit" of the NcDevice object is the following :
// Name   : Indicating the name (if any) of object1
// Title  : Indicating the name (if any) of object2
// Slot 1 : "type1"  --> Type of object1 (0=reference signal, 1=measurement) 
// Slot 2 : "index1" --> Storage index of object1 (see memberfunction GetSignal)
// Slot 3 : "type2"  --> Type of object2 (0=reference signal, 1=measurement) 
// Slot 4 : "index2" --> Storage index of object2 (see memberfunction GetSignal)
// Slot 5 : "psi"    --> Opening angle between object2 and object1 in units as specified via "au"
// Slot 6 : "t2-t1"  --> Time offset between object2 and object1 in units as specified via "tu" and "mode"
//                       (t2-t1)<0 in case object2 was recorded earlier than object1.
//
// The NcDevice object itself provides the data of the best encountered matchings as follows :
//
// Slot 1 : "psimin" --> Minimal encountered opening angle in units as specified via "au"
// Slot 2 : "dtmin"  --> Minimal encountered time offset |t2-t1| in units as specified via "tu" and "mode"
// Slot 3 : "ipsi"   --> Index of the hit that corresponds to the minimal opening angle
// Slot 4 : "idt"    --> Index of the hit that corresponds to the minimal time offset
//
// A full overview of all matchings can be obtained via NcDevice::Data().
//
// Note : In case scrambling has been activated, the results reflect the scrambled measurement(s).
//
// Usage example :
// ---------------
// NcAstrolab lab;
// // Enter various measurements and reference signals into "lab"
// lab.SetSignal(...);
// lab.SetSignal(...);
// lab.SetSignal(...);
// ...
// ...
// ...
// // Look for matches between all measurements and reference signals
// NcDevice matches;
// lab.MatchBurstData(matches);
//
// // List all the encountered matching details
// matches.Data();
//
// // Retrieve the minimal encountered opening angle
// Float_t angmin=matches.GetSignal(1);
//
// // Access the data of the matching pair (i.e. "hit") that yielded the minimal time offset
// Int_t ih=matches.GetSignal("idt");
// NcSignal* sx=matches.GetHit(ih);
// sx->Data();
//
// // Access the originally recorded signal of object1 that yielded the minimal opening angle
// // and retrieve its timestamp and the corresponding MJD. 
// ih=matches.GetSignal("ipsi");
// sx=matches.GetHit(ih);
// Int_t itype=sx->GetSignal("type1");
// Int_t i=sx->GetSignal("index1");
// sx=lab.GetSignal(i,itype);
// NcTimestamp* tx=sx->GetTimestamp();
// tx->Date();
// Double_t mjd1=tx->GetMJD();

 Float_t fAngresfix=fBurstParameters->GetSignal("Angresfix");
 Int_t fRecoangle=TMath::Nint(fBurstParameters->GetSignal("Recoangle"));
 Int_t fSumsigmas=TMath::Nint(fBurstParameters->GetSignal("Sumsigmas"));
 Int_t fTunits=TMath::Nint(fBurstParameters->GetSignal("Tunits"));
 Float_t fTmin=fBurstParameters->GetSignal("Tmin");
 Float_t fTmax=fBurstParameters->GetSignal("Tmax");
 Float_t fDawin=fBurstParameters->GetSignal("Dawin");
 Int_t fDatype=TMath::Nint(fBurstParameters->GetSignal("Datype"));
 Float_t fMaxsigmatot=fBurstParameters->GetSignal("Maxsigmatot");
 Int_t fNbkg=TMath::Nint(fBurstParameters->GetSignal("Nbkg"));

 // Update internal statistics to account for objects introduced by hand
 Int_t fNgrbs=GetNsignals(0);
 Int_t fNevts=GetNsignals(1);
 fBurstParameters->AddNamedSlot("Ngrbs");
 fBurstParameters->AddNamedSlot("Nevts");
 fBurstParameters->SetSignal(fNgrbs,"Ngrbs");
 fBurstParameters->SetSignal(fNevts,"Nevts");

 // Initialize generic histograms for analysis
 InitBurstHistograms(1);

 TString tu="d";
 if (fTunits==1) tu="hrs";
 if (fTunits==2) tu="s";
 if (fTunits==3) tu="ns";
 if (fTunits==4) tu="ps";

 // Initialize the Device/Hit structure to contain the correlation info 
 matches.Reset(1);
 matches.SetHitCopy(1);

 TString name="Matches";
 TString title="Space and time matchings of NcAstrolab stored signals";
 matches.SetNameTitle(name,title);
 TString tux=tu;
 if (tu=="d") tux="days";
 if (tu=="hrs") tux="hours";
 if (tu=="s") tux="sec";
 TString namedamin="psimin in deg"; 
 TString namedtmin="dtmin in ";
 namedtmin+=tux;
 matches.AddNamedSlot(namedamin);
 matches.AddNamedSlot(namedtmin);
 matches.AddNamedSlot("ipsi");
 matches.AddNamedSlot("idt");

 NcSignal data;
 TString nameda="psi in deg"; 
 TString namedt="t2-t1 in ";
 namedt+=tux;
 data.AddNamedSlot("type1");
 data.AddNamedSlot("index1");
 data.AddNamedSlot("type2");
 data.AddNamedSlot("index2");
 data.AddNamedSlot(nameda);
 data.AddNamedSlot(namedt);

 if ((!itype || !jtype) && !fRefs)
 {
  printf(" *%-s::MatchBurstData* Error: itype=%-i jtype=%-i but no reference signals are present. \n",ClassName(),itype,jtype); 
  return;
 }

 if ((itype || jtype) && !fSigs)
 {
  printf(" *%-s::MatchBurstData* Error: itype=%-i jtype=%-i but no measurements are present. \n",ClassName(),itype,jtype); 
  return;
 }

 Int_t nrefs=0;
 if (fRefs) nrefs=fRefs->GetSize();
 Int_t nsigs=0;
 if (fSigs) nsigs=fSigs->GetSize();

 // Make input data consistent with conventions
 if (itype) itype=1;
 if (jtype) jtype=1;
 if (!itype)
 {
  if (i2<1 || i2>nrefs) i2=nrefs;
 }
 else
 {
  if (i2<1 || i2>nsigs) i2=nsigs;
 }
 if (!jtype)
 {
  if (j2<1 || j2>nrefs) j2=nrefs;
 }
 else
 {
  if (j2<1 || j2>nsigs) j2=nsigs;
 }

 if (i1<1 || j1<1 || i1>i2 || j1>j2)
 {
  printf(" *%-s::MatchBurstData* Inconsistent parameters: i1=%-i i2=%-i itype=%-i j1=%-i j2=%-i jtype=%-i. \n",ClassName(),i1,i2,itype,j1,j2,jtype); 
  return;
 }

 if (fDatype==1 && fMaxsigmatot<0)
 {
  printf(" *%-s::MatchBurstData* Incompatible parameter settings Datype=%-i Maxsigmatot=%-g \n",ClassName(),fDatype,fMaxsigmatot);
  printf(" === No matching analysis will be performed === \n");
 }

 // The number of actually stored items
 Int_t ni=GetNsignals(itype);
 Int_t nj=GetNsignals(jtype);

 // Additional text for histo titles to indicate scrambling of the corresponding data
 TString scrt=""; // Time scrambling
 TString scrp=""; // Position scrambling
 if (fTscmode>0) scrt="(scrambled)";
 if (fRscmode>0 || fTscmode==3) scrp="(scrambled)";

 // The auto binned event-burst time difference histo for the full selected sample
 Int_t nbins=10000;
 title.Form("Time difference %-s between events and bursts for the full selected dataset;Tevent-Tburst in %-s;Counts",scrt.Data(),tux.Data());
 TH1F* hTdiff=new TH1F("hTdiff",title,nbins,1,0);
 if (ni && nj) hTdiff->SetBuffer(ni*nj);

 // The auto binned event-burst angular difference histo for the full selected sample
 nbins=100;
 title.Form("Angular separation %-s between events and bursts for the full selected dataset;Opening angle in degrees;Counts",scrp.Data());
 TH1F* hAdiff=new TH1F("hAdiff",title,nbins,1,0);
 if (ni && nj) hAdiff->SetBuffer(ni*nj);

 // Recording of the cumulated on-source reconstructed event energy
 fBurstParameters->AddNamedSlot("EnergyOn");
 fBurstParameters->AddNamedSlot("EnergyOff");

 Double_t dang,dtime,diftheta;
 Int_t ix=0;
 Int_t jx=0;
 NcSignal* sxi=0;
 NcSignal* sxj=0;
 NcSignal* sxgrb=0;
 NcSignal* sxevt=0;
 Float_t sigmai=0;
 Float_t sigmaj=0;
 Float_t sigmagrb=0;
 Float_t sigmareco=0;
 Float_t sigmatot=0;
 Int_t id=0;
 Double_t dangmin=0;
 Double_t dtmin=0;
 Int_t idamin=0;
 Int_t idtmin=0;
 Bool_t first=kTRUE;
 Double_t dangmax=-1;
 Float_t Ereco=0;
 Float_t thlow=0;
 Float_t thup=0;
 Float_t thetagrb=0;
 Float_t solidangle=0;
 Int_t grbtype=0;
 Int_t k1=0;
 Int_t k2=0;
 Float_t zgrb=0;
 for (Int_t bkgpatch=0; bkgpatch<=fNbkg; bkgpatch++) // On-source and Off-source patches
 {
  if (bkgpatch && !fTscmode && !fRscmode) break; // No background data without time and/or position scrambling

  for (Int_t i=i1; i<=i2; i++)
  {
   sxi=GetSignal(i,itype);
   if (!sxi) continue;

   sigmai=sxi->GetSignal("csigma");
 
   if (itype && !fRecoangle) sigmai=fAngresfix;

   for (Int_t j=j1; j<=j2; j++)
   {
    // Skip matching a signal with itself
    if (itype==jtype && i==j) continue;

    sxj=GetSignal(j,jtype);
    if (!sxj) continue;

    sigmaj=sxj->GetSignal("csigma");
 
    if (jtype && !fRecoangle) sigmaj=fAngresfix;

    if (itype==jtype) // Self correlations
    {
     sigmagrb=sigmai;
     sxgrb=sxi;
     sigmareco=sigmaj;
     sxevt=sxj;
    }
    else // Correlations between sources and measurements
    {
     if (itype)
     {
      sigmareco=sigmai;
      sxevt=sxi;
      sigmagrb=sigmaj;
      sxgrb=sxj;
     }
     else
     {
      sigmagrb=sigmai;
      sxgrb=sxi;
      sigmareco=sigmaj;
      sxevt=sxj;
     }
    }

    sigmatot=-1;
    if (fSumsigmas==-1) sigmatot=sigmareco;
    if (fSumsigmas==0) sigmatot=sigmagrb;
    if (fSumsigmas==1) sigmatot=sigmagrb+sigmareco;
    if (fSumsigmas==2) sigmatot=sqrt(sigmagrb*sigmagrb+sigmareco*sigmareco);

    dangmax=-1;
    if (!fDatype) dangmax=fabs(fDawin);
    if (fDatype==1)
    {
     dangmax=0;
     if (fMaxsigmatot>0) dangmax=fabs(fDawin*fMaxsigmatot);
    }
    if (fDatype==2)
    {
     if (sigmatot>=0) dangmax=fabs(fDawin*sigmatot);
    }

    // Flag incompatible input for sigma summation
    if (fDatype==1 && fMaxsigmatot<0) dangmax=-1;

    ix=i;
    if (itype) ix=-i;
    jx=j;
    if (jtype) jx=-j;

    if (tu!="hrs")
    {
     dang=GetSeparation(ix,jx,"deg",dtime,tu,1,bkgpatch,&diftheta);
    }
    else
    {
     dang=GetSeparation(ix,jx,"deg",dtime,"s",1,bkgpatch,&diftheta);
     dtime=dtime/3600.;
    }
    
    if (!bkgpatch) // On-source data
    {
     // Fill the time difference histogram for full selected dataset
     if (hTdiff) hTdiff->Fill(dtime);

     // Fill the angular difference histogram for full selected dataset
     if (hAdiff) hAdiff->Fill(dang);

     // Initialize the On-source total solid angle for this source
     if (dangmax>=0 && !(sxgrb->GetSlotIndex("OmegaOn")))
     {
      sxgrb->AddNamedSlot("OmegaOn");
      if (fDawin<0) // Local zenith band
      {
       thlow=thetagrb-0.5*dangmax;
       thup=thetagrb+0.5*dangmax;
       solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
      }
      else // Circle around GRB position
      {
       thlow=0;
       thup=dangmax;
       solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
      }
      sxgrb->SetSignal(solidangle,"OmegaOn");
     }
    }
    else // Off-source c.q. background data 
    {
     // Initialize the Off-source total solid angle for this source
     if (dangmax>=0 && !(sxgrb->GetSlotIndex("OmegaOff")))
     {
      sxgrb->AddNamedSlot("OmegaOff");
      if (fDawin<0) // Local zenith band
      {
       thlow=thetagrb-0.5*dangmax;
       thup=thetagrb+0.5*dangmax;
       solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
      }
      else // Circle around GRB position
      {
       thlow=0;
       thup=dangmax;
       solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
      }
      sxgrb->SetSignal(solidangle,"OmegaOff");
     }
    }

    if (fDatype>=0)
    {
     if (fDawin<0) // Declination band
     {
      if (diftheta<thlow || diftheta>thup) continue;
     }
     else // Circle around the GRB
     {
      if (fabs(dang)>dangmax) continue;
     }
    }

    if (fTmax>fTmin && (dtime<fTmin || dtime>fTmax)) continue;

    if (!bkgpatch) // On-source data
    {
     // Tag this source of having an on-source matching event
     if (!(sxgrb->GetSlotIndex("HasMatchOn")))
     {
      sxgrb->AddNamedSlot("HasMatchOn");
      sxgrb->SetSignal(1,"HasMatchOn");
     }

     data.Reset();
     name="Object1=";
     name+=sxi->GetName();
     title="Object2=";
     title+=sxj->GetName();
     id++;
     data.SetNameTitle(name,title);
     data.SetUniqueID(id);
     data.SetSignal(itype,"type1");
     data.SetSignal(i,"index1");
     data.SetSignal(jtype,"type2");
     data.SetSignal(j,"index2");
     data.SetSignal(dtime,namedt);
     data.SetSignal(dang,nameda);
     matches.AddHit(data);

     // Update the maximum encountered dynamic On-source solid angle for this source
     if (fDatype==2)
     {
      if (fDawin<0) // Local zenith band
      {
       thlow=thetagrb-0.5*dangmax;
       thup=thetagrb+0.5*dangmax;
       solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
      }
      else // Circle around GRB position
      {
       thlow=0;
       thup=dangmax;
       solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
      }
      if (solidangle>sxgrb->GetSignal("OmegaOn")) sxgrb->SetSignal(solidangle,"OmegaOn");
     }
    }
    else // Off-source c.q. background data
    {
     // Tag this source of having an off-source matching event
     if (!(sxgrb->GetSlotIndex("HasMatchOff")))
     {
      sxgrb->AddNamedSlot("HasMatchOff");
      sxgrb->SetSignal(1,"HasMatchOff");
     }

     // Update the maximum encountered dynamic Off-source solid angle for this source
     if (fDatype==2)
     {
      if (fDawin<0) // Local zenith band
      {
       thlow=thetagrb-0.5*dangmax;
       thup=thetagrb+0.5*dangmax;
       solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
      }
      else // Circle around GRB position
      {
       thlow=0;
       thup=dangmax;
       solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
      }
      if (solidangle>sxgrb->GetSignal("OmegaOff")) sxgrb->SetSignal(solidangle,"OmegaOff");
     }
    }

    // Fill the histograms for the matching data
    if (!bkgpatch) // On-source data
    {
     Ereco=sxevt->GetSignal("E");
     zgrb=sxgrb->GetSignal("z");
     fBurstOnReco.Enter(zgrb,sigmagrb,sigmareco,sigmatot);
     fBurstOnMatch.Enter(Ereco,dtime,dang,dtime/(zgrb+1.));
     fBurstParameters->AddSignal(Ereco,"EnergyOn");

     // Record the data for the minimal encountered opening angle
     if (first || fabs(dang)<dangmin)
     {
      dangmin=fabs(dang);
      idamin=id;
     }

     // Record the data for the minimal encountered time difference
     if (first || fabs(dtime)<fabs(dtmin))
     {
      dtmin=dtime;
      idtmin=id;
     }
    
     first=kFALSE;
    }
    else // Off-source c.q. backgound data
    {
     Ereco=sxevt->GetSignal("E");
     zgrb=sxgrb->GetSignal("z");
     fBurstOffReco.Enter(zgrb,sigmagrb,sigmareco,sigmatot);
     fBurstOffMatch.Enter(Ereco,dtime,dang,dtime/(zgrb+1.));
     fBurstParameters->AddSignal(Ereco,"EnergyOff");
    }
   } // End of the j-loop
  } // End of the i-loop
 } // End of loop over the patches

 // Store the filled histograms in the storage container
 if (hTdiff->GetEntries()) fBurstHistos.Add(hTdiff);
 if (hAdiff->GetEntries()) fBurstHistos.Add(hAdiff);

 // Recording of the On-source and Off-source total stacked solid angles
 name="SolidangleOn";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(0,name);
 name="SolidangleOff";
 fBurstParameters->AddNamedSlot(name);
 fBurstParameters->SetSignal(0,name);
 if (itype==jtype) // Self correlations
 {
  grbtype=itype;
  k1=i1;
  k2=i2;
 }
 else // Correlations between sources and measurements
 {
  if (itype)
  {
   grbtype=jtype;
   k1=j1;
   k2=j2;
  }
  else
  {
   grbtype=itype;
   k1=i1;
   k2=i2;
  }
 }
 // Loop over all grbs in the selection
 for (Int_t k=k1; k<=k2; k++)
 {
  sxgrb=GetSignal(k,grbtype);
  if (!sxgrb) continue;

  solidangle=sxgrb->GetSignal("OmegaOn");
  fBurstParameters->AddSignal(solidangle,"SolidangleOn");

  solidangle=sxgrb->GetSignal("OmegaOff");
  fBurstParameters->AddSignal(solidangle*float(fNbkg),"SolidangleOff");
 }

 // Store the data for the minimal encountered opening angle and time difference
 matches.SetSignal(dangmin,namedamin);
 matches.SetSignal(dtmin,namedtmin);
 matches.SetSignal(idamin,"ipsi");
 matches.SetSignal(idtmin,"idt");

 // Determine and list the burst statistics
 MakeBurstDataStats(1);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MatchBurstData(NcDevice& matches,TString name,Int_t itype,Int_t j1,Int_t j2,Int_t jtype)
{
// Provide information about the matching in space and time of the stored
// reference signal(s) and/or measurement(s).
// Signals are regarded as matching if the positional angular difference
// and the absolute value of the time difference don't exceed the parameters
// as specified by SetBurstParameter().
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The information (see below) for the observed matches is provided via the NcDevice argument "matches",
// where each matching pair is recorded as a "hit" with the corresponding NcSignal data.
// Consequently, the number of matches can be obtained as the number of "hits" in the NcDevice.
//
// Note : This memberfunction will completely reset and re-define the NcDevice "matches".
//
// It is possible to investigate only a sub-set of the stored reference signals c.q. measurements
// via the input arguments "j1" and "j2", "jtype" as follows :
//
// name  : The name of the object to be used for the matching with the stored signals 
// itype : 0 ==> The object "name" resides among the stored reference signals
//         1 ==> The object "name" resides among the stored measurements
// j1    : Start index of the stored signals to be investigated (1=first stored signal)
// j2    : End index of the stored signals to be investigated (0=last stored signal)
// jtype : 0 ==> [j1,j2] refer to stored reference signals
//         1 ==> [j1,j2] refer to stored measurements
//
// The default values are itype=0, j1=1, j2=0 and jtype=1.
//
// Note : In case the name specifies a solar system object which was not yet stored,
//        the corresponding signal will be created according to "itype" and initially
//        stored with the current timestamp of this NcAstrolab instance.
//        In the matching process the location of the solar system object will be updated
//        according to the timestamp of the other signal. 
//        All geocentric name specifications for solar system objects as indicated in the
//        docs of NcTimestamp::Almanac() are supported.
//
// The data for each matching signal pair provided as a "hit" of the NcDevice object is the following :
// Name   : Indicating the name (if any) of object1
// Title  : Indicating the name (if any) of object2
// Slot 1 : "type1"  --> Type of object1 (0=reference signal, 1=measurement) 
// Slot 2 : "index1" --> Storage index of object1 (see memberfunction GetSignal)
// Slot 3 : "type2"  --> Type of object2 (0=reference signal, 1=measurement) 
// Slot 4 : "index2" --> Storage index of object2 (see memberfunction GetSignal)
// Slot 5 : "psi"    --> Opening angle between object2 and object1 in units as specified via "au"
// Slot 6 : "t2-t1"  --> Time offset between object2 and object1 in units as specified via "tu" and "mode"
//                       (t2-t1)<0 in case object2 was recorded earlier than object1.
//
// The NcDevice object itself provides the data of the best encountered matchings as follows :
//
// Slot 1 : "psimin" --> Minimal encountered opening angle in units as specified via "au"
// Slot 2 : "dtmin"  --> Minimal encountered time offset |t2-t1| in units as specified via "tu" and "mode"
// Slot 3 : "ipsi"   --> Index of the hit that corresponds to the minimal opening angle
// Slot 4 : "idt"    --> Index of the hit that corresponds to the minimal time offset
//
// A full overview of all matchings can be obtained via NcDevice::Data().
//
// Note : In case scrambling has been activated, the results reflect the scrambled measurement(s).
//
// Usage example :
// ---------------
// NcAstrolab lab;
// // Enter various measurements and reference signals into "lab"
// lab.SetSignal(...);
// lab.SetSignal(...);
// lab.SetSignal(...);
// ...
// ...
// ...
// // Look for matches between all measurements and GRB190215A as reference signal
// NcDevice matches;
// lab.MatchBurstData(matches,"GRB190215A");
//
// // List all the encountered matching details
// matches.Data();
//
// // Retrieve the minimal encountered opening angle
// Float_t angmin=matches.GetSignal(1);
//
// // Access the data of the matching pair (i.e. "hit") that yielded the minimal time offset
// Int_t ih=matches.GetSignal("idt");
// NcSignal* sx=matches.GetHit(ih);
// sx->Data();
//
// // Access the originally recorded signal of object1 that yielded the minimal opening angle
// // and retrieve its timestamp and the corresponding MJD. 
// ih=matches.GetSignal("ipsi");
// sx=matches.GetHit(ih);
// Int_t itype=sx->GetSignal("type1");
// Int_t i=sx->GetSignal("index1");
// sx=lab.GetSignal(i,itype);
// NcTimestamp* tx=sx->GetTimestamp();
// tx->Date();
// Double_t mjd1=tx->GetMJD();

 Int_t i=GetSignalIndex(name,itype);

 if (i==-1) // Add the info for the requested Solar system object if not already stored
 {
  SetSolarSystem(name,0,itype);
  i=GetSignalIndex(name,itype);
  if (i>0) fSolUpdate=1;
 }

 if (i<1)
 {
  printf(" *%-s::MatchBurstData* Object %-s not found for itype=%-i. \n",ClassName(),name.Data(),itype); 
 }
 else
 {
  MatchBurstData(matches,i,i,itype,j1,j2,jtype);
 }

 fSolUpdate=0;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::InitBurstHistograms(Int_t mode)
{
// Internal memberfunction to initialize the histograms for transient burst analyses.
//
// mode : 0 --> Invoked from GenBurstSignals()
//        1 --> Invoked from MatchBurstData()
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 // Retreive the needed parameters
 Float_t fT90min=fBurstParameters->GetSignal("T90min");
 Float_t fT90max=fBurstParameters->GetSignal("T90max");
 Float_t fESigmin=fBurstParameters->GetSignal("ESigmin");
 Float_t fESigmax=fBurstParameters->GetSignal("ESigmax");
 Float_t fEmin=fBurstParameters->GetSignal("Emin");
 Float_t fEmax=fBurstParameters->GetSignal("Emax");
 Float_t fAlphasig=fBurstParameters->GetSignal("Alphasig");
 Float_t fAlphabkg=fBurstParameters->GetSignal("Alphabkg");
 Float_t fAvgrbz=fBurstParameters->GetSignal("Avgrbz");
 Float_t fAvgrbt90=fBurstParameters->GetSignal("Avgrbt90");
 Int_t fTunits=TMath::Nint(fBurstParameters->GetSignal("Tunits"));

 TString tu="days";
 if (fTunits==1) tu="hours";
 if (fTunits==2) tu="sec";
 if (fTunits==3) tu="ns";
 if (fTunits==4) tu="ps";

 TString title;
 TString s;

 // Initialize the On-source and Off-source data samples
 fBurstOnReco.Reset();
 fBurstOnMatch.Reset();
 fBurstSigReco.Reset();
 fBurstSignal.Reset();
 fBurstOffReco.Reset();
 fBurstOffMatch.Reset();
 fBurstOnReco.SetNameTitle("BurstOnReco","On-source reco data");
 fBurstOnReco.SetStoreMode();
 fBurstOnReco.SetNames("zburst","sigmaburst","sigmareco","sigmacomb");
 fBurstOnMatch.SetNameTitle("BurstOnMatch","On-source matching data");
 fBurstOnMatch.SetStoreMode();
 fBurstOnMatch.SetNames("E","dtime","dang","dtimez");
 fBurstSigReco.SetNameTitle("BurstSigReco","Simulated signal reco data");
 fBurstSigReco.SetStoreMode();
 fBurstSigReco.SetNames("zburst","sigmaburst","sigmareco","sigmacomb");
 fBurstSignal.SetNameTitle("BurstSignal","Simulated signal events");
 fBurstSignal.SetStoreMode();
 fBurstSignal.SetNames("E","dtime","dang","dtimez");
 fBurstOffReco.SetNameTitle("BurstOffReco","Off-source reco data");
 fBurstOffReco.SetStoreMode();
 fBurstOffReco.SetNames("zburst","sigmaburst","sigmareco","sigmacomb");
 fBurstOffMatch.SetNameTitle("BurstOffMatch","Off-source matching data");
 fBurstOffMatch.SetStoreMode();
 fBurstOffMatch.SetNames("E","dtime","dang","dtimez");

 // Set default simulation signal and background energy spectra if needed
 if (!mode)
 {
  TH1* edist=0;
  edist=(TH1*)fBurstHistos.FindObject("hSigEpdf");
  if (!edist) MakeBurstEnergydist(2,fAlphasig,fESigmin,fESigmax,10000);
  edist=(TH1*)fBurstHistos.FindObject("hBkgEpdf");
  if (!edist) MakeBurstEnergydist(1,fAlphabkg,fEmin,fEmax,10000);
 }

 //////////////////////////////////////////
 // Some statistics from the loaded data //
 //////////////////////////////////////////

 Int_t fNgrbs=GetNsignals(0);
 Int_t fNevts=GetNsignals(1);

 Float_t xmin=0;
 Float_t xmax=0;
 Float_t range=0;
 Int_t nbins=100;
 Float_t binsize=0;
 Int_t srcbufsize=10000;
 if (fNgrbs && fNgrbs<srcbufsize) srcbufsize=fNgrbs;
 Int_t evtbufsize=10000;
 if (fNevts && fNevts<evtbufsize) evtbufsize=fNevts;

 // Creation of the burst redshift histo
 title.Form("Redshifts for the selected source sample;Redshift;Counts");
 TH1F* hSourceZ=new TH1F("hSourceZ",title,nbins,1,0);
 hSourceZ->SetBuffer(srcbufsize);

 // Creation of the corresponding physical distance histo
 nbins=100;
 title.Form("Distances for the selected source sample derived from the redshifts;Physical distance in Mpc;Counts");
 TH1F* hSourceD=new TH1F("hSourceD",title,nbins,xmin,xmax);
 hSourceD->SetBuffer(srcbufsize);

 // Creation of the burst t90 duration histo
 xmin=-5;
 if (fabs(fT90min)>0) xmin=log10(fabs(fT90min));
 xmax=5;
 if (fT90max>0) xmax=log10(fT90max);
 range=xmax-xmin;
 binsize=0.2; // Bins of 0.2
 nbins=TMath::Nint(range/binsize);
 if (nbins<1)
 {
  xmin=xmin-1.;
  xmax=xmax+1.;
  nbins=10;
 }
 title.Form("Burst durations for the selected source sample;Burst duration ^{10}log(T90) in sec.;Counts");
 TH1F* hBurstT90=new TH1F("hBurstT90",title,nbins,xmin,xmax);

 // Creation of the full selected sample burst position uncertainty histo with automatic binning
 nbins=100;
 title.Form("Position uncertainties for the selected source sample;Position angular uncertainty (sigma in degrees);Counts");
 TH1F* hSigmaSource=new TH1F("hSigmaSource",title,nbins,1,0);
 hSigmaSource->SetBuffer(srcbufsize);

 // Creation of the real event reconstructed energy histo with automatic binning
 TH1F* hEreco=0;
 nbins=1000;
 if (mode==1)
 {
  title.Form("Reconstructed energy for the full selected real event sample;Reconstructed event energy in GeV;Counts");
  hEreco=new TH1F("hEreco",title,nbins,1,0);
  hEreco->SetBuffer(evtbufsize);
 }

 // Creation of the injected source signal event energy histo with automatic binning
 TH1F* hSigE=0;
 TH1F* hSigEzcor=0;
 nbins=1000;
 if (mode==0)
 {
  title.Form("Injected signal energy at the source;Event energy in GeV;Counts");
  hSigE=new TH1F("hSigE",title,nbins,1,0);
  fBurstHistos.Add(hSigE);
  title.Form("(Redshift corrected) injected signal energy arriving at Earth;Event energy in GeV;Counts");
  hSigEzcor=new TH1F("hSigEzcor",title,nbins,1,0);
  fBurstHistos.Add(hSigEzcor);
 }

 // Creation of the full selected sample event reconstruction uncertainty histo with automatic binning
 TH1F* hSigmaReco=0;
 nbins=100;
 title.Form("Event reconstruction uncertainties for the full selected sample;Event angular reconstruction uncertainty (sigma in degrees);Counts");
 hSigmaReco=new TH1F("hSigmaReco",title,nbins,1,0);
 hSigmaReco->SetBuffer(evtbufsize);

 // Fill the generic source c.q. burst histograms
 NcSignal* sx=0;
 Float_t zgrb=0;
 Double_t dgrb=0;
 Float_t t90grb=0;
 Float_t sigmagrb=0;
 NcSample zsample;
 zsample.SetStoreMode();
 NcSample t90sample;
 t90sample.SetStoreMode();
 NcSample sigmasample;
 sigmasample.SetStoreMode();
 Int_t nsig=GetNsignals(0,1);
 for (Int_t i=1; i<=nsig; i++)
 {
  sx=GetSignal(i,0);

  if (!sx) continue;

  zgrb=sx->GetSignal("z");
  dgrb=GetPhysicalDistance(zgrb);
  t90grb=sx->GetSignal("T90");
  sigmagrb=sx->GetSignal("csigma");

  hSourceZ->Fill(zgrb);
  hSourceD->Fill(dgrb);
  if (t90grb>0) hBurstT90->Fill(log10(t90grb));
  hSigmaSource->Fill(sigmagrb);

  if (fAvgrbz<0) zsample.Enter(zgrb);
  if (fAvgrbt90<0) t90sample.Enter(t90grb);
  sigmasample.Enter(sigmagrb);
 }

 // Add the filled histograms to the storage container
 if (hSourceZ->GetEntries()) fBurstHistos.Add(hSourceZ);
 if (hSourceD->GetEntries()) fBurstHistos.Add(hSourceD);
 if (hBurstT90->GetEntries()) fBurstHistos.Add(hBurstT90);
 if (hSigmaSource->GetEntries()) fBurstHistos.Add(hSigmaSource);

 // Determine median redshift if requested
 if (fAvgrbz<0)
 {
  fAvgrbz=zsample.GetMedian(1);
  fAvgrbz*=-1.;
 }

 // Determine median T90 duration if requested
 if (fAvgrbt90<0)
 {
  fAvgrbt90=t90sample.GetMedian(1);
  fAvgrbt90*=-1.;
 }

 Float_t fAvgrbsigma=sigmasample.GetMedian(1);

 // Fill the generic event histograms
 Float_t sigmareco=0;
 Float_t Ereco=0;
 nsig=GetNsignals(1,1);
 for (Int_t i=1; i<=nsig; i++)
 {
  sx=GetSignal(i,1);

  if (!sx) continue;

  sigmareco=sx->GetSignal("csigma");
  Ereco=sx->GetSignal("E");

  if (hSigmaReco) hSigmaReco->Fill(sigmareco);
  if (hEreco) hEreco->Fill(Ereco);
 }

 // Add the filled histograms to the storage container
 if (hSigmaReco->GetEntries()) fBurstHistos.Add(hSigmaReco);
 if (hEreco)
 {
  if (hEreco->GetEntries()) fBurstHistos.Add(hEreco);
 }

 // Update internal statistics
 fBurstParameters->SetSignal(fAvgrbz,"Avgrbz");
 fBurstParameters->SetSignal(fAvgrbt90,"Avgrbt90");
 fBurstParameters->AddNamedSlot("Avgrbsigma");
 fBurstParameters->SetSignal(fAvgrbsigma,"Avgrbsigma");
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::BurstCompensate(Int_t& nmugrb)
{
// Compensate statistical underfluctuation in the number of transient burst muons.

 // Retreive the needed parameters
 Float_t fGrbnu=fBurstParameters->GetSignal("Grbnu");
 Int_t fNgrbs=TMath::Nint(fBurstParameters->GetSignal("Ngrbs"));
 Int_t fInburst=TMath::Nint(fBurstParameters->GetSignal("Inburst"));
 Float_t fDtnu=fBurstParameters->GetSignal("Dtnu");
 Float_t fDtnus=fBurstParameters->GetSignal("Dtnus");
 Float_t fTimres=fBurstParameters->GetSignal("Timres");
 Int_t fDatype=TMath::Nint(fBurstParameters->GetSignal("Datype"));
 Float_t fDawin=fBurstParameters->GetSignal("Dawin");
 Float_t fTfact=fBurstParameters->GetSignal("Tfact");
 Int_t fEzcor=TMath::Nint(fBurstParameters->GetSignal("Ezcor"));
 Int_t fPDFsigE=TMath::Nint(fBurstParameters->GetSignal("PDFsigE"));
 Float_t fEmin=fBurstParameters->GetSignal("Emin");
 Float_t fEmax=fBurstParameters->GetSignal("Emax");
 Int_t fKinangle=TMath::Nint(fBurstParameters->GetSignal("Kinangle"));
 Float_t fAngresmin=fBurstParameters->GetSignal("Angresmin");
 Float_t fAngresmax=fBurstParameters->GetSignal("Angresmax");
 Float_t fAngresfix=fBurstParameters->GetSignal("Angresfix");
 Int_t fSumsigmas=TMath::Nint(fBurstParameters->GetSignal("Sumsigmas"));

 // Deactivate redshift correction for source signal events from archival observed data
 if (!fPDFsigE)
 {
  fEzcor=0;
  fBurstParameters->SetSignal(0,"Ezcor");
 } 

 Int_t nmu=int(fabs(fGrbnu)*float(fNgrbs));
 Int_t jgrb=0;
 NcSignal* sx=0;
 NcTimestamp* tx=0;
 Float_t t90grb=0;
 Float_t zgrb=0;
 Float_t sigmagrb=0;
 NcPosition rgrb;
 NcPosition rgrb2;
 Float_t dt=0;
 Double_t dgrb=0;
 Double_t thetagrb=0;
 Double_t phigrb=0;
 Float_t dang=0;
 Float_t dangmax=0;
 Float_t dangmaxOn=0;
 NcPosition rmu;
 Double_t E=0;
 Double_t sigmareco=0;
 Float_t sigmatot=0;
 Float_t OmegaOn=0; // The current on-source solid angle probed for a certain GRB
 Float_t thlow=0;
 Float_t thup=0;
 Float_t solidangle=0;
 Int_t fixedwinset=0; // Flag to indicate whether a fixed angular search window was set (1) or not (0) for this burst 

 TH1* hSigE=(TH1*)fBurstHistos.FindObject("hSigE");
 TH1* hSigEzcor=(TH1*)fBurstHistos.FindObject("hSigEzcor");

 while (nmugrb<nmu)
 {
  // Pick randomly one of the stored GRBs
  jgrb=int(fRan->Uniform(0.,float(fNgrbs)));
  if (jgrb==0) jgrb=1;
  sx=GetSignal(jgrb);

  if (!sx) continue;

  tx=sx->GetTimestamp();
  GetSignal(dgrb,thetagrb,"deg",phigrb,"deg","loc",tx,jgrb);
  rgrb.SetPosition(1,thetagrb,phigrb,"sph","deg");
  zgrb=sx->GetSignal("z");
  t90grb=sx->GetSignal("T90");
  sigmagrb=sx->GetSignal("csigma");
  fixedwinset=TMath::Nint(sx->GetSignal("fixedwinset"));
  dangmaxOn=sx->GetSignal("dangmaxOn");
  OmegaOn=sx->GetSignal("OmegaOn");

  dangmax=dangmaxOn;

  // Obtain actual GRB position
  rgrb2.Load(rgrb);
  SmearPosition(rgrb2,sigmagrb);

  nmugrb++;

  if (!fInburst) // Neutrino and gamma production decoupled
  {
   if (fDtnus<0) // Sigma in units of T90
   {
    dt=fRan->Gauss(fDtnu,fabs(fDtnus)*t90grb);
   }
   else // Sigma in seconds
   {
    dt=fRan->Gauss(fDtnu,fDtnus);
   }
   dt=dt*(zgrb+1.);
  }
  else // Coupled neutrino and gamma production
  {
   if (fDtnus<0) // Sigma in units of T90
   {
    dt=fRan->Gauss(fDtnu*t90grb,fabs(fDtnus)*t90grb);
   }
   else // Sigma in seconds
   {
    dt=fRan->Gauss(fDtnu*t90grb,fDtnus);
   }
  }
  if (fTimres>0) dt=fRan->Gauss(dt,fTimres);

  // Convert dt from seconds to the selected Tunits
  dt=dt/fTfact;

  // The direction of the GRB signal
  rmu.Load(rgrb2);

  // The energy of the GRB signal
  E=GetBurstSignalEnergy();

  if (hSigE) hSigE->Fill(E);

  // Reduce the energy due to the cosmological redshift effect
  if (fEzcor) E=E/(zgrb+1.);

  if (hSigEzcor) hSigEzcor->Fill(E);

  if (E<0 || E<fEmin || E>fEmax) continue;

  // Modification to account for the neutrino-lepton kinematic opening angle
  if (fKinangle>0)
  {
   Int_t mode=fKinangle-1;
   Double_t ang=GetNeutrinoAngle(E,"deg",mode);
   if (ang>0) ShiftPosition(rmu,ang);
  }

  // Smearing according to the reconstruction angular resolution
  sigmareco=GetBurstRecoAngres(E,E);
  if (sigmareco<0) sigmareco=fAngresfix;

  if (sigmareco<fAngresmin || sigmareco>fAngresmax) continue;

  SmearPosition(rmu,sigmareco);

  // Determine angular difference w.r.t. the presumed GRB position
  dang=rgrb.GetOpeningAngle(rmu,"deg");

  sigmatot=-1;
  if (fSumsigmas==-1) sigmatot=sigmareco;
  if (fSumsigmas==0) sigmatot=sigmagrb;
  if (fSumsigmas==1) sigmatot=sigmagrb+sigmareco;
  if (fSumsigmas==2) sigmatot=sqrt(sigmagrb*sigmagrb+sigmareco*sigmareco);

  // Determine the dynamic angular window including the track reco uncertainty
  if (!fixedwinset)
  {
   dangmax=-1;
   if (sigmatot>=0) dangmax=fabs(fDawin*sigmatot);
  }

  if (fDatype>=0 && dang>dangmax) continue;

  fBurstOnReco.Enter(zgrb,sigmagrb,sigmareco,sigmatot);
  fBurstOnMatch.Enter(E,dt,dang,dt/(zgrb+1.));
  fBurstSigReco.Enter(zgrb,sigmagrb,sigmareco,sigmatot);
  fBurstSignal.Enter(E,dt,dang,dt/(zgrb+1.));
  fBurstParameters->AddSignal(E,"EnergyOn");
  fBurstParameters->AddSignal(E,"EnergySig");

  if (fixedwinset) continue;
  
  // Update the dynamic On-source maximum (solid) angle that is encountered for this burst
  if (fDawin<0) // Local zenith band
  {
   thlow=thetagrb-0.5*dangmax;
   thup=thetagrb+0.5*dangmax;
   solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
  }
  else // Circle around GRB position
  {
   thlow=0;
   thup=dangmax;
   solidangle=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
  }
  if (dangmax>dangmaxOn) sx->SetSignal(dangmax,"dangmaxOn");
  if (solidangle>OmegaOn) sx->SetSignal(solidangle,"OmegaOn");
 }
}
///////////////////////////////////////////////////////////////////////////
TH1* NcAstrolab::GetBurstBayesianSignalRate(Double_t p,Double_t& rlow,Double_t& rup,Int_t n)
{
// Provide the transient burst Bayesian signal rate and the lower and upper bounds of the
// Bayesian "p%" credible interval [rlow,rup] around the mode of the signal PDF.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Input arguments :
// -----------------
// p    : The percentage of the PDF to be covered by the credible interval around the mode.
//        So for a Gaussian PDF, p=68.3 will result in the [mean-sigma,mean+sigma] 68.3% credible interval.
// rlow : The variable for the return of the lower bound of the credible interval. 
// rup  : The variable for the return of the upper bound of the credible interval. 
// n    : The precision on the result, expressed as 1/n.
//
// By default n=1000 which implies that the accuracy of the result is better than 0.1%.
// Note that very large values of "n" may result in a rather long computation time.
//
// The return argument is the histogram for the Bayesian signal rate PDF.
//
// In case of inconsistent data all returned values are 0.

 rlow=0;
 rup=0;

 // The number of on-source and off-source patches
 Int_t fNgrbs=GetNsignals(0);
 Int_t fNbkg=TMath::Nint(fBurstParameters->GetSignal("Nbkg"));

 // The recorded (stacked) on-source and off-source number of events
 Double_t Non=fBurstOnMatch.GetN();
 Double_t Noff=fBurstOffMatch.GetN();

 if (fNgrbs<=0 || fNbkg<=0 || Non<=0 || Noff<=0)
 {
  printf(" \n *%-s::GetBurstBayesianSignalRate* \n",ClassName());
  if (fNgrbs<=0 || Non<=0) printf(" === No on-source data available === \n");
  if (fNbkg<=0 || Noff<=0) printf(" === No off-source data available === \n");
  return 0;
 }

 Int_t fTunits=TMath::Nint(fBurstParameters->GetSignal("Tunits"));
 Float_t fTfact=fBurstParameters->GetSignal("Tfact");

 TString tu="days";
 if (fTunits==1) tu="hours";
 if (fTunits==2) tu="sec";
 if (fTunits==3) tu="ns";
 if (fTunits==4) tu="ps";

 // The (stacked) on-source and off-source solid angles that were probed
 Double_t fSolidangleOn=fBurstParameters->GetSignal("SolidangleOn");
 Double_t fSolidangleOff=fBurstParameters->GetSignal("SolidangleOff");
 Double_t fAvSolidangleOn=fBurstParameters->GetSignal("AvSolidangleOn");
 Double_t fAvSolidangleOff=fBurstParameters->GetSignal("AvSolidangleOff");
 Double_t Ra=-1;
 if (fAvSolidangleOff) Ra=fAvSolidangleOn/fAvSolidangleOff;

 // The integrated on-source and off-source exposure times in seconds
 Double_t Ton=fBurstParameters->GetSignal("TtotOn");
 Double_t Toff=fBurstParameters->GetSignal("TtotOff");

 // The Bayesian posterior background and signal rate PDFs
 TF1 fbkgrpdf=GetBackgroundRatePDF(Noff,Toff);
 TF1 fsigrpdf=GetSignalRatePDF(Non,Ton,Noff,Toff,Ra);

 Double_t rmode=fsigrpdf.GetMaximumX();
 Double_t bkgrmode=fbkgrpdf.GetMaximumX();

 // Determine the "p%" credible interval for the signal rate
 Float_t frac=0;
 frac=GetCredibleInterval(fsigrpdf,p,rlow,rup,n);

 // Provide the signal and background rate PDFs as histograms in the output file
 fbkgrpdf.SetRange(0,3.*Noff/Toff);
 fbkgrpdf.SetNpx(n);
 TH1* hBkgRatePDF=(TH1*)fbkgrpdf.GetHistogram()->Clone();
 hBkgRatePDF->SetName("hBkgRatePDF");
 fBurstHistos.Add(hBkgRatePDF);
 fsigrpdf.SetRange(0,3.*Non/Ton);
 fsigrpdf.SetNpx(n);
 TH1* hSigRatePDF=(TH1*)fsigrpdf.GetHistogram()->Clone();
 hSigRatePDF->SetName("hSigRatePDF");
 fBurstHistos.Add(hSigRatePDF);

 printf("\n *%-s::GetBurstBayesianSignalRate* Credible interval [rlow,rup] for p=%-g%% with a precision of 1/%-i \n",ClassName(),p,n);

 // Issue a warning in case variable angular cones based on actual track reco sigmas were used.
 // This can induce large variations in the on-source and off-source event counts
 Int_t fDatype=TMath::Nint(fBurstParameters->GetSignal("Datype"));
 Int_t fRecoangle=TMath::Nint(fBurstParameters->GetSignal("Recoangle"));
 Int_t fSumsigmas=TMath::Nint(fBurstParameters->GetSignal("Sumsigmas"));

 if (fRecoangle && fSumsigmas && fDatype==2)
 {
  printf(" === Warning: Variable angular cones based on actual track reconstruction sigmas were used. \n");
  printf("              Large variations between the on-source and off-source background counts may be present. \n");
 }

 printf(" The %-g%% credible interval from the Bayesian posterior signal pdf : [%-g,%-g] Hz \n",100.*frac,rlow,rup);
 printf(" Modes of the on-source posterior PDFs : Signal=%-g Hz Background=%-g Hz",rmode,bkgrmode);
 if (bkgrmode) printf(" Signal/Background=%-g",rmode/bkgrmode);
 printf("\n");
 cout << " The following signal and background rate PDF histograms have been generated :" << endl;
 cout << " ... " << hSigRatePDF->GetName() << " : " << hSigRatePDF->GetTitle() << endl;      
 cout << " ... " << hBkgRatePDF->GetName() << " : " << hBkgRatePDF->GetTitle() << endl;      

 // Provide statistics for the lower and upper signal rate boundaries and for the mode of the signal PDF as well 
 Float_t fSensarea=fBurstParameters->GetSignal("Sensarea");
 Float_t TwinOn=Ton/float(fNgrbs); // The average time window in seconds

 printf(" Integrated on-source  exposure time of the %-i stacked time windows : %-g %-s \n",fNgrbs,Ton/fTfact,tu.Data());
 printf(" Integrated off-source exposure time of the %-i*%-i stacked time windows : %-g %-s \n",fNgrbs,fNbkg,Toff/fTfact,tu.Data());
 printf(" Total accumulated on-source  solid angle : %-g sr in %-i stacked patches --> Average per patch : %-g sr \n",fSolidangleOn,fNgrbs,fAvSolidangleOn); 
 printf(" Total accumulated off-source solid angle : %-g sr in %-i*%-i stacked patches --> Average per patch : %-g sr \n",fSolidangleOff,fNgrbs,fNbkg,fAvSolidangleOff);
 if (fSensarea>0) printf(" Area covered c.q. overlooked by the detector sensors : %-g m^2. \n",fSensarea);
 fSensarea*=1e4; // Convert to cm^2

 printf(" Total number of recorded on-source  events : %-g --> Average per patch : %-g events. \n",Non,Non/float(fNgrbs));
 printf(" Total number of recorded off-source events : %-g --> Average per patch : %-g events. \n",Noff,Noff/float(fNgrbs*fNbkg));

 printf(" --- Average values per on-source patch based on the posterior PDFs --- \n");

 printf(" *Lower bound* Steady signal event rate during the time window : %-g Hz",rlow);
 if (fAvSolidangleOn) printf(" --> %-g Hz sr^-1",rlow/fAvSolidangleOn);
 printf("\n");
 printf("               Expected number of signal events : %-g",rlow*TwinOn);
 if (fAvSolidangleOn) printf(" --> %-g events/sr",rlow*TwinOn/fAvSolidangleOn);
 printf("\n");
 if (fSensarea>0)
 {
  printf("               Expected signal particle fluence : %-g cm^-2",rlow*TwinOn/fSensarea);
  if (fAvSolidangleOn) printf(" --> : %-g cm^-2 sr^-1",rlow*TwinOn/(fSensarea*fAvSolidangleOn));
  printf("\n");
  printf("               Expected signal particle flux : %-g cm^-2 s^-1",rlow/fSensarea);
  if (fAvSolidangleOn) printf(" --> Intensity : %-g cm^-2 s^-1 sr^-1",rlow/(fSensarea*fAvSolidangleOn));
  printf("\n");
 }

 printf("    *Mode*     Steady signal event rate during the time window : %-g Hz",rmode);
 if (fAvSolidangleOn) printf(" --> %-g Hz sr^-1",rmode/fAvSolidangleOn);
 printf("\n");
 printf("               Expected number of signal events : %-g",rmode*TwinOn);
 if (fAvSolidangleOn) printf(" --> %-g events/sr",rmode*TwinOn/fAvSolidangleOn);
 printf("\n");
 if (fSensarea>0)
 {
  printf("               Expected signal particle fluence : %-g cm^-2",rmode*TwinOn/fSensarea);
  if (fAvSolidangleOn) printf(" --> : %-g cm^-2 sr^-1",rmode*TwinOn/(fSensarea*fAvSolidangleOn));
  printf("\n");
  printf("               Expected signal particle flux : %-g cm^-2 s^-1",rmode/fSensarea);
  if (fAvSolidangleOn) printf(" --> Intensity : %-g cm^-2 s^-1 sr^-1",rmode/(fSensarea*fAvSolidangleOn));
  printf("\n");
 }

 printf(" *Upper bound* Steady signal event rate during the time window : %-g Hz",rup);
 if (fAvSolidangleOn) printf(" --> %-g Hz sr^-1",rup/fAvSolidangleOn);
 printf("\n");
 printf("               Expected number of signal events : %-g",rup*TwinOn);
 if (fAvSolidangleOn) printf(" --> %-g events/sr",rup*TwinOn/fAvSolidangleOn);
 printf("\n");
 if (fSensarea>0)
 {
  printf("               Expected signal particle fluence : %-g cm^-2",rup*TwinOn/fSensarea);
  if (fAvSolidangleOn) printf(" --> : %-g cm^-2 sr^-1",rup*TwinOn/(fSensarea*fAvSolidangleOn));
  printf("\n");
  printf("               Expected signal particle flux : %-g cm^-2 s^-1",rup/fSensarea);
  if (fAvSolidangleOn) printf(" --> Intensity : %-g cm^-2 s^-1 sr^-1",rup/(fSensarea*fAvSolidangleOn));
  printf("\n");
 }

 printf(" *Background*  Steady background event rate during the time window : %-g Hz",bkgrmode);
 if (fAvSolidangleOn) printf(" --> %-g Hz sr^-1",bkgrmode/fAvSolidangleOn);
 printf("\n");
 printf("               Expected number of background events : %-g",bkgrmode*TwinOn);
 if (fAvSolidangleOn) printf(" --> %-g events/sr",bkgrmode*TwinOn/fAvSolidangleOn);
 printf("\n");
 if (fSensarea>0)
 {
  printf("               Expected background particle fluence : %-g cm^-2",bkgrmode*TwinOn/fSensarea);
  if (fAvSolidangleOn) printf(" --> : %-g cm^-2 sr^-1",bkgrmode*TwinOn/(fSensarea*fAvSolidangleOn));
  printf("\n");
  printf("               Expected background particle flux : %-g cm^-2 s^-1",bkgrmode/fSensarea);
  if (fAvSolidangleOn) printf(" --> Intensity : %-g cm^-2 s^-1 sr^-1",bkgrmode/(fSensarea*fAvSolidangleOn));
  printf("\n");
 }

 return hSigRatePDF;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetBurstLiMaSignificance() const
{
// Provide the transient burst Li-Ma signal significance in terms of the amount of
// standard deviations w.r.t. the "on source" and "off source" observations.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// In case of inconsistent data the value 0 is returned.

 Double_t sigma=0;

 // The recorded (stacked) "on source" and "off source" number of events
 Double_t Non=fBurstOnMatch.GetN();
 Double_t Noff=fBurstOffMatch.GetN();

 if (Non<=0 || Noff<=0)
 {
  printf(" \n *%-s::GetBurstLiMaSignificance* \n",ClassName());
  if (Non<=0) printf(" === No on source data available === \n");
  if (Noff<=0) printf(" === No off source data available === \n");
  return 0;
 }

 // The (stacked) "on source" and "off source" solid angles that were probed
 Double_t fAvSolidangleOn=fBurstParameters->GetSignal("AvSolidangleOn");
 Double_t fAvSolidangleOff=fBurstParameters->GetSignal("AvSolidangleOff");
 Double_t Ra=-1;
 if (fAvSolidangleOff) Ra=fAvSolidangleOn/fAvSolidangleOff;

 // The (stacked) "on source" and "off source" exposure times
 Double_t Ton=fBurstParameters->GetSignal("TtotOn");
 Double_t Toff=fBurstParameters->GetSignal("TtotOff");

 NcMath m;
 sigma=m.LiMaSignificance(Non,Ton,Noff,Toff,Ra);

 printf("\n *%-s::GetBurstLiMaSignificance* The Li-Ma signal significance is : %-g \n",ClassName(),sigma);

 // Issue a warning in case variable angular cones based on actual track reco sigmas were used.
 // This can induce large variations in the on-source and off-source event counts
 Int_t fDatype=TMath::Nint(fBurstParameters->GetSignal("Datype"));
 Int_t fRecoangle=TMath::Nint(fBurstParameters->GetSignal("Recoangle"));
 Int_t fSumsigmas=TMath::Nint(fBurstParameters->GetSignal("Sumsigmas"));

 if (fRecoangle && fSumsigmas && fDatype==2)
 {
  printf(" === Warning: Variable angular cones based on actual track reconstruction sigmas were used. \n");
  printf("              Large variations between the on-source and off-source background counts may be present. \n");
 }

 return sigma;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GetBurstBayesianPsiStatistics(TString type,Double_t nr,Int_t ncut,Int_t ndt,Bool_t zcor,Int_t freq)
{
// Provide the transient burst Bayesian Psi statistics for the (stacked) distributions of the
// observed arrival times and opening angles w.r.t. the corresponding bursts.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Consider a hypothesis B_m representing a counting experiment with m different
// possible outcomes and which is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
// In mathematical terms such a hypothesis belongs to the Bernoulli class B_m. 
//
// The Psi value of n trials of B_m provides (in dB scale) the amount of support
// that the data can maximally give to any Bernoulli class hypothesis different
// from the currently specified B_m.
//
// To be specific : Psi=-10*log[p(D|B_m I)]
//
// where p(D|B_m I) represents the likelihood of the data D under the condition
// that B_m (given some prior information I) is true.
//
// In our current situation, the hypotheses B_m (i.e. Probability Distribution Functions)
// for the various observed distributions are known and "m" just represents the number
// of bins, and "n" represents the number of entries of the corresponding histogram.
// As such, the Psi value (psi0) of the actual observation can be determined.
//
// Further mathematical details can be found in the publication
// N. van Eijndhoven, Astropart. Phys. 28 (2008) 540 (astro-ph/0702029).
//
// This memberfunction may also provide the statistical P-value (i.e. the fraction of
// recorded psi values with psi>=psi0) for the actually observed psi value (psi0)
// based on "nr" repetitions of the counting experiment corresponding to B_m
// with "n" independent random trials.
//
// Input arguments :
// -----------------
// type : "time"   --> Provide statistics for the observed arrival times.
//                     This will investigate the deviation from a uniform background time spectrum.
//        "BBtime" --> Provide statistics for the Bayesian blocks corresponding to the observed arrival times.
//                     This will investigate the deviation from a uniform event rate.
//        "BBrat"  --> Provide statistics for the normalized On-source/Off-source ratio of the Bayesian blocks
//                     corresponding to the observed arrival times.
//                     This will investigate the relative deviation in event rate.
//        "angle"  --> Provide statistics for the observed opening angles.
//                     This will investigate the deviation from a isotropic background angular spectrum.
//        "cosa"   --> Provide statistics for the cosine of the observed opening angles.
//                     This will investigate the deviation from a uniform background cos(angle) spectrum.
//        "dt"     --> Provide statistics for the time intervals between the observed arrival times.
//                     This will investigate the deviation from dt spectrum expected from Poisson statistics.
// nr   : (Maximum) number of randomised configurations for psi P-value determination.
//        nr<0 implies that no psi P-values will be determined (saves CPU time).
//        nr=0 implies the allowed maximum of 1e19 randomisations.
// ncut : Number of obtained randomised psi entries above the actual observed psi value
//        at which randomisations will be terminated (to save CPU time).
//        ncut=0 implies no early termination.
// ndt  : The step count to arrive at the required consecutive entry to perform the dt statistics.
//        Specifying "ndt=1" will provide the statistics of dt intervals between each consecutive event,
//        i.e. time intervals between the events (1,2), (2,3), (3,4) etc.
//        Specifying "ndt=2" will provide the statistics of dt intervals between every 2nd consecutive event,
//        i.e. time intervals between the events (1,3), (2,4), (3,5) etc.
// zcor : Use redshift corrected arrival times (kTRUE) or not (kFALSE)
// freq : Use frequentist's approximation (1) or exact Bayesian expression (0) for psi.
//
// The default values are nr=-1, ncut=10, ndt=2, zcor=kFALSE and freq=0.

 NcMath math;

 TString text="none";
 if (type=="time") text="arrival time";
 if (type=="BBtime") text="Bayesian Block event rate";
 if (type=="BBrat") text="normalized On-source/Off-source Bayesian Block event rate ratio";
 if (type=="angle") text="opening angle";
 if (type=="cosa") text="cos(opening angle)";
 if (type=="dt") text="arrival time interval";

 if (zcor)
 {
  text.ReplaceAll("arrival time","redshift corrected arrival time");
  text.ReplaceAll("event rate","redshift corrected event rate");
 }

 cout << endl; 
 if (text=="none")
 {
  cout << " *" << ClassName() << "::GetBurstBayesianPsiStatistics* Unknown statistics type : " << type << endl;
  return;
 }
 else
 {
  cout << " *" << ClassName() << "::GetBurstBayesianPsiStatistics* Analysis of " << text << " statistics" << endl;
 }

 Int_t fTunits=TMath::Nint(fBurstParameters->GetSignal("Tunits"));
 TString tu="days";
 if (fTunits==1) tu="hours";
 if (fTunits==2) tu="sec";
 if (fTunits==3) tu="ns";
 if (fTunits==4) tu="ps";

 TH1* tot=0;
 TH1* bkg=0;
 TH1* rat=0;

 Double_t psitot=-1, psibkg=-1, psirat=-1;
 Float_t psidif=0;
 Float_t psimintot=-1, psimaxtot=-1, psifractot=0;
 Float_t psiminbkg=-1, psimaxbkg=-1, psifracbkg=0;
 Float_t psiminrat=-1, psimaxrat=-1, psifracrat=0;
 Double_t nrxtot=-1, nrxbkg=-1, nrxrat=-1;
 Double_t pvaluetot=-1, pvaluebkg=-1, pvaluerat=-1;

 TH1F* hPsiOn=0;
 TH1F* hPsiOff=0;
 TH1F* hPsiRat=0;
 TH1F* rtot=0;
 TH1F* rbkg=0;
 TH1F* rrat=0;

 ////////////////////////////////////////////
 // Arrival time histo Bayesian statistics //
 ////////////////////////////////////////////
 if (type=="time")
 {
  if (!zcor) // Plain observed arrival times
  {
   tot=(TH1*)fBurstHistos.FindObject("hOnt");
   bkg=(TH1*)fBurstHistos.FindObject("hOfft");
  }
  else // Redshift corrected arrival times
  {
   tot=(TH1*)fBurstHistos.FindObject("hOnZt");
   bkg=(TH1*)fBurstHistos.FindObject("hOffZt");
  }

  if (!tot) printf(" === No on source data available === \n");
  if (!bkg) printf(" === No off source data available === \n");

  if (!tot && !bkg) return;

  if (tot) psitot=math.PsiValue(tot,0,0,freq);
  if (bkg) psibkg=math.PsiValue(bkg,0,0,freq);
  psidif=psitot-psibkg;

  // Extreme Psi values for a pure background hypothesis of the recorded arrival time entries
  if (tot)
  {
   psimintot=math.PsiExtreme(tot,0,0,-2);
   if (psitot<psimintot) psimintot=psitot;
   psimaxtot=math.PsiExtreme(tot,0,0,-1);
   if (psimaxtot>psimintot) psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  }
  if (bkg)
  {
   psiminbkg=math.PsiExtreme(bkg,0,0,-2); 
   if (psibkg<psiminbkg) psiminbkg=psibkg;
   psimaxbkg=math.PsiExtreme(bkg,0,0,-1);
   if (psimaxbkg>psiminbkg) psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);
  }

  // P-value determination
  if (nr>=0)
  {
   if (!zcor) // Plain observed arrival times
   {
    hPsiOn=(TH1F*)fBurstHistos.FindObject("hPsiOnt");
    hPsiOff=(TH1F*)fBurstHistos.FindObject("hPsiOfft");
   }
   else // Redshift corrected arrival times
   {
    hPsiOn=(TH1F*)fBurstHistos.FindObject("hPsiOnZt");
    hPsiOff=(TH1F*)fBurstHistos.FindObject("hPsiOffZt");
   }

   if (hPsiOn)
   {
    rtot=(TH1F*)hPsiOn->Clone();
    rtot->Reset();
   }
   else
   {
    if (tot)
    {
     if (!zcor) rtot=new TH1F("hPsiOnt","Psi distr. for bkg hypothesis of on-source arrival time data",100,psimintot-1.,psimaxtot+1.);
     if (zcor)  rtot=new TH1F("hPsiOnZt","Psi distr. for bkg hypothesis of redshift corrected on-source arrival time data",100,psimintot-1.,psimaxtot+1.);
    }
   }

   if (hPsiOff)
   {
    rbkg=(TH1F*)hPsiOff->Clone();
    rbkg->Reset();
   }
   else
   {
    if (bkg)
    {
     if (!zcor) rbkg=new TH1F("hPsiOfft","Psi distr. for bkg hypothesis of off-source arrival time data",100,psiminbkg-1.,psimaxbkg+1.);
     if (zcor)  rbkg=new TH1F("hPsiOffZt","Psi distr. for bkg hypothesis of redshift corrected off-source arrival time data",100,psiminbkg-1.,psimaxbkg+1.);
    }
   }

   if (tot)
   {
    pvaluetot=math.PsiPvalue(-1,nr,tot,0,0,freq,0,rtot,ncut,&nrxtot);
    fBurstHistos.Add(rtot);
   }
   if (bkg)
   {
    pvaluebkg=math.PsiPvalue(-1,nr,bkg,0,0,freq,0,rbkg,ncut,&nrxbkg);
    fBurstHistos.Add(rbkg);
   }

   cout << " The following randomised Psi histograms have been generated :" << endl;
   if (rtot) cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
   if (rbkg) cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }
 }

 ////////////////////////////////////////////
 // Event rate histo Bayesian statistics   //
 ////////////////////////////////////////////
 if (type=="BBtime")
 {
  if (!zcor) // Plain observed arrival times
  {
   tot=(TH1*)fBurstHistos.FindObject("hOnBBt");
   bkg=(TH1*)fBurstHistos.FindObject("hOffBBt");
  }
  else // Redshift corrected arrival times
  {
   tot=(TH1*)fBurstHistos.FindObject("hOnBBzt");
   bkg=(TH1*)fBurstHistos.FindObject("hOffBBzt");
  }

  if (!tot) printf(" === No on source data available === \n");
  if (!bkg) printf(" === No off source data available === \n");

  if (!tot && !bkg) return;

  // Temporarily offset the bin contents so that the lowest bin content is 1 for the psi analysis
  if (tot)
  {
   for (Int_t i=1; i<=tot->GetNbinsX(); i++)
   {
    tot->AddBinContent(i,1);
   }
  }
  
  if (bkg)
  {
   for (Int_t i=1; i<=bkg->GetNbinsX(); i++)
   {
    bkg->AddBinContent(i,1);
   }
  }

  if (tot) psitot=math.PsiValue(tot,0,0,freq);
  if (bkg) psibkg=math.PsiValue(bkg,0,0,freq);
  psidif=psitot-psibkg;

  // Extreme Psi values for a pure background hypothesis of the recorded arrival time entries
  if (tot)
  {
   psimintot=math.PsiExtreme(tot,0,0,-2);
   if (psitot<psimintot) psimintot=psitot;
   psimaxtot=math.PsiExtreme(tot,0,0,-1);
   if (psimaxtot>psimintot) psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  }
  if (bkg)
  {
   psiminbkg=math.PsiExtreme(bkg,0,0,-2); 
   if (psibkg<psiminbkg) psiminbkg=psibkg;
   psimaxbkg=math.PsiExtreme(bkg,0,0,-1);
   if (psimaxbkg>psiminbkg) psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);
  }

  // P-value determination
  if (nr>=0)
  {
   if (!zcor) // Plain observed arrival times
   {
    hPsiOn=(TH1F*)fBurstHistos.FindObject("hPsiOnBBt");
    hPsiOff=(TH1F*)fBurstHistos.FindObject("hPsiOffBBt");
   }
   else // Redshift corrected arrival times
   {
    hPsiOn=(TH1F*)fBurstHistos.FindObject("hPsiOnBBzt");
    hPsiOff=(TH1F*)fBurstHistos.FindObject("hPsiOffBBzt");
   }

   if (hPsiOn)
   {
    rtot=(TH1F*)hPsiOn->Clone();
    rtot->Reset();
   }
   else
   {
    if (tot)
    {
     if (!zcor) rtot=new TH1F("hPsiOnBBt","Psi distr. for bkg hypothesis of on-source event rate Bayesian Block data",100,psimintot-1.,psimaxtot+1.);
     if (zcor)  rtot=new TH1F("hPsiOnBBzt","Psi distr. for bkg hypothesis of redshift corrected on-source event rate Bayesian Block data",100,psimintot-1.,psimaxtot+1.);
    }
   }

   if (hPsiOff)
   {
    rbkg=(TH1F*)hPsiOff->Clone();
    rbkg->Reset();
   }
   else
   {
    if (bkg)
    {
     if (!zcor) rbkg=new TH1F("hPsiOffBBt","Psi distr. for bkg hypothesis of off-source event rate Bayesian Block data",100,psiminbkg-1.,psimaxbkg+1.);
     if (zcor)  rbkg=new TH1F("hPsiOffBBzt","Psi distr. for bkg hypothesis of redshift corrected off-source event rate Bayesian Block data",100,psiminbkg-1.,psimaxbkg+1.);
    }
   }

   if (tot)
   {
    pvaluetot=math.PsiPvalue(-1,nr,tot,0,0,freq,0,rtot,ncut,&nrxtot);
    fBurstHistos.Add(rtot);
   }
   if (bkg)
   {
    pvaluebkg=math.PsiPvalue(-1,nr,bkg,0,0,freq,0,rbkg,ncut,&nrxbkg);
    fBurstHistos.Add(rbkg);
   }

   cout << " The following randomised Psi histograms have been generated :" << endl;
   if (rtot) cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
   if (rbkg) cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }

  // Restore the original histogram data
  if (tot)
  {
   for (Int_t i=1; i<=tot->GetNbinsX(); i++)
   {
    tot->AddBinContent(i,-1);
   }
  }
  if (bkg)
  {
   for (Int_t i=1; i<=bkg->GetNbinsX(); i++)
   {
    bkg->AddBinContent(i,-1);
   }
  }
 }

 /////////////////////////////////////////////////////////////////////
 // On-source/Off-source event rate ratio histo Bayesian statistics //
 /////////////////////////////////////////////////////////////////////
 if (type=="BBrat")
 {
  if (!zcor) // Plain observed arrival times
  {
   rat=(TH1*)fBurstHistos.FindObject("hRatUBBt");
  }
  else // Redshift corrected arrival times
  {
   rat=(TH1*)fBurstHistos.FindObject("hRatUBBzt");
  }

  if (!rat)
  {
   printf(" === No data available === \n");
   return;
  }

  psirat=math.PsiValue(rat,0,0,freq);
  psidif=psitot-psibkg;

  // Extreme Psi values for a pure background hypothesis of the recorded arrival time entries
  psiminrat=math.PsiExtreme(rat,0,0,-2); 
  if (psirat<psiminrat) psiminrat=psirat;
  psimaxrat=math.PsiExtreme(rat,0,0,-1);
  if (psimaxrat>psiminrat) psifracrat=(psimaxrat-psirat)/(psimaxrat-psiminrat);

  // P-value determination
  if (nr>=0)
  {
   if (!zcor) // Plain observed arrival times
   {
    hPsiRat=(TH1F*)fBurstHistos.FindObject("hPsiRatUBBt");
   }
   else // Redshift corrected arrival times
   {
    hPsiRat=(TH1F*)fBurstHistos.FindObject("hPsiRatUBBzt");
   }

   if (hPsiRat)
   {
    rrat=(TH1F*)hPsiRat->Clone();
    rrat->Reset();
   }
   else
   {
    if (!zcor) rrat=new TH1F("hPsiRatUBBt","Psi distr. for bkg hypothesis of on/off source event rate Bayesian Block data",100,psiminrat-1.,psimaxrat+1.);
    if (zcor)  rrat=new TH1F("hPsiRatUBBzt","Psi distr. for bkg hypothesis of redshift corrected on/off source event rate Bayesian Block data",100,psiminrat-1.,psimaxrat+1.);
   }

   pvaluerat=math.PsiPvalue(-1,nr,rat,0,0,freq,0,rrat,ncut,&nrxrat);
   fBurstHistos.Add(rrat);

   cout << " The following randomised Psi histograms have been generated :" << endl;
   if (rrat) cout << " ... " << rrat->GetName() << " : " << rrat->GetTitle() << endl;
  }
 }
 
 /////////////////////////////////////////////
 // Opening angle histo Bayesian statistics //
 /////////////////////////////////////////////
 if (type=="angle")
 {
  tot=(TH1*)fBurstHistos.FindObject("hOna");
  bkg=(TH1*)fBurstHistos.FindObject("hOffa");

  if (!tot) printf(" === No on source data available === \n");
  if (!bkg) printf(" === No off source data available === \n");

  if (!tot && !bkg) return;

  TF1 pdfa("pdfa","sin(x*acos(-1.)/180.)");
  if (tot) psitot=math.PsiValue(tot,0,&pdfa,freq);
  if (bkg) psibkg=math.PsiValue(bkg,0,&pdfa,freq);
  psidif=psitot-psibkg;

  // Extreme Psi values for a pure background hypothesis of the recorded opening angle entries
  if (tot)
  {
   psimintot=math.PsiExtreme(tot,0,&pdfa,-2);
   if (psitot<psimintot) psimintot=psitot;
   psimaxtot=math.PsiExtreme(tot,0,&pdfa,-1);
   if (psimaxtot>psimintot) psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  }
  if (bkg)
  {
   psiminbkg=math.PsiExtreme(bkg,0,&pdfa,-2);
   if (psibkg<psiminbkg) psiminbkg=psibkg;
   psimaxbkg=math.PsiExtreme(bkg,0,&pdfa,-1);
   if (psimaxbkg>psiminbkg) psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);
  }

  // P-value determination
  if (nr>=0)
  {
   hPsiOn=(TH1F*)fBurstHistos.FindObject("hPsiOna");
   hPsiOff=(TH1F*)fBurstHistos.FindObject("hPsiOffa");

   if (hPsiOn)
   {
    rtot=(TH1F*)hPsiOn->Clone();
    rtot->Reset();
   }
   else
   {
    if (tot) rtot=new TH1F("hPsiOna","Psi distr. for bkg hypothesis of on-source opening angle data",100,psimintot-1.,psimaxtot+1.);
   }

   if (hPsiOff)
   {
    rbkg=(TH1F*)hPsiOff->Clone();
    rbkg->Reset();
   }
   else
   {
    if (bkg) rbkg=new TH1F("hPsiOffa","Psi distr. for bkg hypothesis of off-source opening angle data",100,psiminbkg-1.,psimaxbkg+1.);
   }

   if (tot)
   {
    pvaluetot=math.PsiPvalue(-1,nr,tot,0,&pdfa,freq,0,rtot,ncut,&nrxtot);
    fBurstHistos.Add(rtot);
   }
   if (bkg)
   {
    pvaluebkg=math.PsiPvalue(-1,nr,bkg,0,&pdfa,freq,0,rbkg,ncut,&nrxbkg);
    fBurstHistos.Add(rbkg);
   }

   cout << " The following randomised Psi histograms have been generated :" << endl;
   if (rtot) cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
   if (rbkg) cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }      
 }

 ///////////////////////////////////////////////////////
 // Cosine of opening angle histo Bayesian statistics //
 ///////////////////////////////////////////////////////
 if (type=="cosa")
 {
  tot=(TH1*)fBurstHistos.FindObject("hOnCosa");
  bkg=(TH1*)fBurstHistos.FindObject("hOffCosa");

  if (!tot) printf(" === No on source data available === \n");
  if (!bkg) printf(" === No off source data available === \n");

  if (!tot && !bkg) return;

  if (tot) psitot=math.PsiValue(tot,0,0,freq);
  if (bkg) psibkg=math.PsiValue(bkg,0,0,freq);
  psidif=psitot-psibkg;

  // Extreme Psi values for a pure background hypothesis of the recorded cos(opening angle) entries
  if (tot)
  {
   psimintot=math.PsiExtreme(tot,0,0,-2);
   if (psitot<psimintot) psimintot=psitot;
   psimaxtot=math.PsiExtreme(tot,0,0,-1);
   if (psimaxtot>psimintot) psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  }
  if (bkg)
  {
   psiminbkg=math.PsiExtreme(bkg,0,0,-2);
   if (psibkg<psiminbkg) psiminbkg=psibkg;
   psimaxbkg=math.PsiExtreme(bkg,0,0,-1);
   if (psimaxbkg>psiminbkg) psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);
  }

  // P-value determination
  if (nr>=0)
  {
   hPsiOn=(TH1F*)fBurstHistos.FindObject("hPsiOnCosa");
   hPsiOff=(TH1F*)fBurstHistos.FindObject("hPsiOffCosa");

   if (hPsiOn)
   {
    rtot=(TH1F*)hPsiOn->Clone();
    rtot->Reset();
   }
   else
   {
    if (tot) rtot=new TH1F("hPsiOnCosa","Psi distr. for bkg hypothesis of on-source cos(opening angle) data",100,psimintot-1.,psimaxtot+1.);
   }

   if (hPsiOff)
   {
    rbkg=(TH1F*)hPsiOff->Clone();
    rbkg->Reset();
   }
   else
   {
    if (bkg) rbkg=new TH1F("hPsiOffCosa","Psi distr. for bkg hypothesis of off-source cos(opening angle) data",100,psiminbkg-1.,psimaxbkg+1.);
   }

    if (tot)
    {
     pvaluetot=math.PsiPvalue(-1,nr,tot,0,0,freq,0,rtot,ncut,&nrxtot);
     fBurstHistos.Add(rtot);
    }
    if (bkg)
    {
     pvaluebkg=math.PsiPvalue(-1,nr,bkg,0,0,freq,0,rbkg,ncut,&nrxbkg);
     fBurstHistos.Add(rbkg);
    }

    cout << " The following randomised Psi histograms have been generated :" << endl;
    if (rtot) cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
    if (rbkg) cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }
 }

 ///////////////////////////////////////////////
 // Arrival time interval Bayesian statistics //
 ///////////////////////////////////////////////
 if (type=="dt")
 {
  TH1F hOndt;
  TH1F hOffdt;
  TF1 pdfdttot;
  TF1 pdfdtbkg;

  GetBurstDtDistributions(ndt,hOndt,pdfdttot,hOffdt,pdfdtbkg,zcor);

  Int_t ntot=hOndt.GetEntries();
  Int_t nbkg=hOffdt.GetEntries();

  if (!ntot) printf(" === No on source data available === \n");
  if (!nbkg) printf(" === No off source data available === \n");

  if (!ntot && !nbkg) return;

  if (ntot) psitot=math.PsiValue(&hOndt,0,&pdfdttot,freq);
  if (nbkg) psibkg=math.PsiValue(&hOffdt,0,&pdfdtbkg,freq);
  psidif=psitot-psibkg;

  if (ntot)
  {
   psimintot=math.PsiExtreme(&hOndt,0,&pdfdttot,-2);
   if (psitot<psimintot) psimintot=psitot;
   psimaxtot=math.PsiExtreme(&hOndt,0,&pdfdttot,-1);
   if (psimaxtot>psimintot) psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  }
  if (nbkg)
  {
   psiminbkg=math.PsiExtreme(&hOffdt,0,&pdfdtbkg,-2);
   if (psibkg<psiminbkg) psiminbkg=psibkg;
   psimaxbkg=math.PsiExtreme(&hOffdt,0,&pdfdtbkg,-1);
   if (psimaxbkg>psiminbkg) psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);
  }

  // P-value determination
  if (nr>=0)
  {
   TString nametot;
   TString namebkg;
   if (!zcor)
   {
    nametot.Form("hPsiOndt%-i",ndt);
    namebkg.Form("hPsiOffdt%-i",ndt);
   }
   else
   {
    nametot.Form("hPsiOnZdt%-i",ndt);
    namebkg.Form("hPsiOffZdt%-i",ndt);
   }

   TH1F* hPsiOn=(TH1F*)fBurstHistos.FindObject(nametot);
   TH1F* hPsiOff=(TH1F*)fBurstHistos.FindObject(namebkg);

   TString title;
   if (hPsiOn)
   {
    rtot=(TH1F*)hPsiOn->Clone();
    rtot->Reset();
   }
   else
   {
    if (!zcor) title.Form("Psi distr. for bkg hypothesis of on-source dt data for n=%-i",ndt);
    if (zcor)  title.Form("Psi distr. for bkg hypothesis of redshift corrected on-source dt data for n=%-i",ndt);
    if (ntot) rtot=new TH1F(nametot,title,100,psimintot-1.,psimaxtot+1.);
   }

   if (hPsiOff)
   {
    rbkg=(TH1F*)hPsiOff->Clone();
    rbkg->Reset();
   }
   else
   {
    if (!zcor) title.Form("Psi distr. for bkg hypothesis of off-source dt data for n=%-i",ndt);
    if (zcor)  title.Form("Psi distr. for bkg hypothesis of redshift corrected off-source dt data for n=%-i",ndt);
    if (nbkg) rbkg=new TH1F(namebkg,title,100,psiminbkg-1.,psimaxbkg+1.);
   }

   if (ntot)
   {
    pvaluetot=math.PsiPvalue(-1,nr,&hOndt,0,&pdfdttot,freq,0,rtot,ncut,&nrxtot);
    fBurstHistos.Add(rtot);
   }
   if (nbkg)
   {
    pvaluebkg=math.PsiPvalue(-1,nr,&hOffdt,0,&pdfdtbkg,freq,0,rbkg,ncut,&nrxbkg);
    fBurstHistos.Add(rbkg);
   }

   cout << " The following randomised Psi histograms have been (re)generated :" << endl;
   if (rtot) cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
   if (rbkg) cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }
 }

 // Listing of the statistics results
 cout << " *** Observed Psi values (in dB) for the hypothesis of no burst signal ***" << endl;
 if (psitot>=0) cout << " For the on-source  stacked patches : psi = " << psitot << endl;
 if (psibkg>=0) cout << " For the off-source stacked patches : psi = " << psibkg << endl;
 if (psitot>=0 && psibkg>=0) cout << " --> Difference between observed on-source and off-source psi values : " << psidif << endl;
 if (psirat>=0) cout << " For the on/off source event rate Bayesian Blocks : psi = " << psirat << endl;
 cout << " *** Extreme Psi values for the case of pure background ***" << endl;
 if (psimintot>=0) cout << " For on-source  psimin : " << psimintot << " psimax : " << psimaxtot;
 if (psifractot>0) printf(" (psimax-psi)/range : %-g",psifractot);
 if (psimintot>=0 || psifractot>0) printf("\n");
 if (psiminbkg>=0) cout << " For off-source psimin : " << psiminbkg << " psimax : " << psimaxbkg;
 if (psifracbkg>0) printf(" (psimax-psi)/range : %-g",psifracbkg);
 if (psiminbkg>=0 || psifracbkg>0) printf("\n");
 if (psiminrat>=0) cout << " For on/off source event rate Bayesian Blocks psimin : " << psiminrat << " psimax : " << psimaxrat;
 if (psifracrat>0) printf(" (psimax-psi)/range : %-g",psifracrat);
 if (psiminrat>=0 || psifracrat>0) printf("\n");

 if (nr>=0)
 {
  cout << " *** P-values of the observed on-source and off-source psi values ***" << endl;
  if (nrxtot>0) cout << " For the on-source  stacked patches : P-value = " << pvaluetot << " Used number of randomisations : " << nrxtot << endl;
  if (nrxbkg>0) cout << " For the off-source stacked patches : P-value = " << pvaluebkg << " Used number of randomisations : " << nrxbkg << endl;
  if (nrxrat>0) cout << " For the on/off source event rate Bayesian Blocks : P-value = " << pvaluerat << " Used number of randomisations : " << nrxrat << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GetBurstChi2Statistics(TString type,Int_t ndt,Bool_t zcor)
{
// Provide the transient burst Chi-squared statistics for the (stacked) distributions
// of the observed arrival times and opening angles w.r.t. the corresponding bursts.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Input arguments :
// -----------------
// type : "time"   --> Provide statistics for the observed arrival times
//                     This will investigate the deviation from a uniform background time spectrum 
//        "BBtime" --> Provide statistics for the Bayesian blocks corresponding to the observed arrival times
//                     This will investigate the deviation from a uniform event rate
//        "BBrat"  --> Provide statistics for the normalized On-source/Off-source ratio of the Bayesian blocks
//                     corresponding to the observed arrival times.
//                     This will investigate the relative deviation in event rate.
//        "angle"  --> Provide statistics for the observed opening angles
//                     This will investigate the deviation from a isotropic background angular spectrum 
//        "cosa"   --> Provide statistics for the cosine of the observed opening angles
//                     This will investigate the deviation from a uniform background cos(angle) spectrum 
//        "dt"     --> Provide statistics for the time intervals between the observed arrival times
//                     This will investigate the deviation from dt spectrum expected from Poisson statistics
// ndt  : The step count to arrive at the required consecutive entry to perform the dt statistics.
//        Specifying "ndt=1" will provide the statistics of dt intervals between each consecutive event,
//        i.e. time intervals between the events (1,2), (2,3), (3,4) etc.
//        Specifying "ndt=2" will provide the statistics of dt intervals between every 2nd consecutive event,
//        i.e. time intervals between the events (1,3), (2,4), (3,5) etc.
// zcor : Use redshift corrected arrival times (kTRUE) or not (kFALSE)
//
// The default values are ndt=2 and zcor=kFALSE.

 NcMath math;

 TString text="none";
 if (type=="time") text="arrival time";
 if (type=="BBtime") text="Bayesian Block event rate";
 if (type=="BBrat") text="normalized On-source/Off-source Bayesian Block event rate ratio";
 if (type=="angle") text="opening angle";
 if (type=="cosa") text="cos(opening angle)";
 if (type=="dt") text="arrival time interval";

 if (zcor)
 {
  text.ReplaceAll("arrival time","redshift corrected arrival time");
  text.ReplaceAll("event rate","redshift corrected event rate");
 }

 cout << endl; 
 if (text=="none")
 {
  cout << " *" << ClassName() << "::GetBurstChi2Statistics* Unknown statistics type : " << type << endl;
  return;
 }
 else
 {
  cout << " *" << ClassName() << "::GetBurstChi2Statistics* Analysis of " << text << " statistics" << endl;
 }

 TH1* tot=0;
 TH1* bkg=0;
 TH1* rat=0;

 Int_t ndftot=0;
 Int_t ndfbkg=0;
 Int_t ndfrat=0;
 Float_t chitot=0;
 Float_t chibkg=0;
 Float_t chirat=0;

 ///////////////////////////////////////////////
 // Arrival time histo Chi-squared statistics //
 ///////////////////////////////////////////////
 if (type=="time")
 {
  if (!zcor)
  {
   tot=(TH1*)fBurstHistos.FindObject("hOnt");
   bkg=(TH1*)fBurstHistos.FindObject("hOfft");
  }
  else
  {
   tot=(TH1*)fBurstHistos.FindObject("hOnZt");
   bkg=(TH1*)fBurstHistos.FindObject("hOffZt");
  }

  if (!tot) printf(" === No on source data available === \n");
  if (!bkg) printf(" === No off source data available === \n");

  if (!tot && !bkg) return;

  if (tot) chitot=math.Chi2Value(tot,0,0,&ndftot);
  if (bkg) chibkg=math.Chi2Value(bkg,0,0,&ndfbkg);
 }

 /////////////////////////////////////////////
 // Event rate histo Chi-squared statistics //
 /////////////////////////////////////////////
 if (type=="BBtime")
 {
  if (!zcor)
  {
   tot=(TH1*)fBurstHistos.FindObject("hOnBBt");
   bkg=(TH1*)fBurstHistos.FindObject("hOffBBt");
  }
  else
  {
   tot=(TH1*)fBurstHistos.FindObject("hOnBBzt");
   bkg=(TH1*)fBurstHistos.FindObject("hOffBBzt");
  }

  if (!tot) printf(" === No on source data available === \n");
  if (!bkg) printf(" === No off source data available === \n");

  if (!tot && !bkg) return;

  if (tot) chitot=math.Chi2Value(tot,0,0,&ndftot);
  if (bkg) chibkg=math.Chi2Value(bkg,0,0,&ndfbkg);
 }

 ////////////////////////////////////////////////////////////////////////
 // On-source/Off-source event rate ratio histo Chi-squared statistics //
 ////////////////////////////////////////////////////////////////////////
 if (type=="BBrat")
 {
  if (!zcor)
  {
   rat=(TH1*)fBurstHistos.FindObject("hRatUBBt");
  }
  else
  {
   rat=(TH1*)fBurstHistos.FindObject("hRatUBBzt");
  }

  if (!rat)
  {
   printf(" === No data available === \n");
   return;
  }

  chirat=math.Chi2Value(rat,0,0,&ndfrat);
 }

 ////////////////////////////////////////////////
 // Opening angle histo Chi-squared statistics //
 ////////////////////////////////////////////////
 if (type=="angle")
 {
  tot=(TH1*)fBurstHistos.FindObject("hOna");
  bkg=(TH1*)fBurstHistos.FindObject("hOffa");

  if (!tot) printf(" === No on source data available === \n");
  if (!bkg) printf(" === No off source data available === \n");

  if (!tot && !bkg) return;

  TF1 pdf("pdf","sin(x*acos(-1.)/180.)");
  if (tot) chitot=math.Chi2Value(tot,0,&pdf,&ndftot);
  if (bkg) chibkg=math.Chi2Value(bkg,0,&pdf,&ndfbkg);
 }

 //////////////////////////////////////////////////////////
 // Cosine of opening angle histo Chi-squared statistics //
 //////////////////////////////////////////////////////////
 if (type=="cosa")
 {
  tot=(TH1*)fBurstHistos.FindObject("hOnCosa");
  bkg=(TH1*)fBurstHistos.FindObject("hOffCosa");

  if (!tot) printf(" === No on source data available === \n");
  if (!bkg) printf(" === No off source data available === \n");

  if (!tot && !bkg) return;

  if (tot) chitot=math.Chi2Value(tot,0,0,&ndftot);
  if (bkg) chibkg=math.Chi2Value(bkg,0,0,&ndfbkg);
 }

 //////////////////////////////////////////////////
 // Arrival time interval Chi-squared statistics //
 //////////////////////////////////////////////////
 if (type=="dt")
 {
  TH1F hOndt;
  TH1F hOffdt;
  TF1 pdfdttot;
  TF1 pdfdtbkg;

  GetBurstDtDistributions(ndt,hOndt,pdfdttot,hOffdt,pdfdtbkg,zcor);

  Int_t ntot=hOndt.GetEntries();
  Int_t nbkg=hOffdt.GetEntries();

  if (!ntot) printf(" === No on source data available === \n");
  if (!nbkg) printf(" === No off source data available === \n");

  if (!ntot && !nbkg) return;

  if (ntot) chitot=math.Chi2Value(&hOndt,0,&pdfdttot,&ndftot);
  if (nbkg) chibkg=math.Chi2Value(&hOffdt,0,&pdfdtbkg,&ndfbkg);
 }

 // Listing of the statistics results
 Float_t chidif=chitot-chibkg;
 cout << " *** Observed Chi-squared values for the hypothesis of no burst signal ***" << endl;
 if (chitot>0) cout << " For the on-source  stacked patches : chi2 = " << chitot << " ndf = " << ndftot << endl;
 if (chibkg>0) cout << " For the off-source stacked patches : chi2 = " << chibkg << " ndf = " << ndfbkg << endl;
 if (chitot>0 && chibkg>0) cout << " --> Difference between observed on-source and off-source chi2 values : " << chidif << endl;
 if (chirat>0) cout << " For the on/off source event rate Bayesian Blocks : chi2 = " << chirat << " ndf = " << ndfrat << endl;

 Float_t ptot=-1;
 Float_t sigmatot=-1;
 Float_t pbkg=-1;
 Float_t sigmabkg=-1;
 Float_t prat=-1;
 Float_t sigmarat=-1;

 if (chitot)
 {
  ptot=math.Chi2Pvalue(chitot,ndftot);
  sigmatot=math.Chi2Pvalue(chitot,ndftot,0,1);
 }
 if (chibkg)
 {
  pbkg=math.Chi2Pvalue(chibkg,ndfbkg);
  sigmabkg=math.Chi2Pvalue(chibkg,ndfbkg,0,1);
 }
 if (chirat)
 {
  prat=math.Chi2Pvalue(chirat,ndfrat);
  sigmarat=math.Chi2Pvalue(chirat,ndfrat,0,1);
 }

 cout << " *** P-values of the observed on-source and off-source chi2 values ***" << endl;
 if (ptot>=0) cout << " For the on-source  stacked patches : P-value = " << ptot << " (" << sigmatot << " sigma)" << endl;
 if (pbkg>=0) cout << " For the off-source stacked patches : P-value = " << pbkg << " (" << sigmabkg << " sigma)" << endl;
 if (prat>=0) cout << " For the on/off source event rate Bayesian Blocks : P-value = " << prat << " (" << sigmarat << " sigma)" << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GetBurstDtDistributions(Int_t ndt,TH1F& hisdtOn,TF1& pdfdtOn,TH1F& hisdtOff,TF1& pdfdtOff,Bool_t zcor)
{
// Internal member function to provide the Dt distributions from the Dt NcSamples
//
// Function arguments :
// --------------------
// ndt      : The step count to arrive at the required consecutive entry to produce the dt distributions.
//            Specifying "ndt=1" will provide the distributions of dt intervals between each consecutive event,
//            i.e. time intervals between the events (1,2), (2,3), (3,4) etc.
//            Specifying "ndt=2" will provide the distributions of dt intervals between every 2nd consecutive event,
//            i.e. time intervals between the events (1,3), (2,4), (3,5) etc.
// hisdtOn  : The resulting histogram of observed dt intervals from the on source data  
// pdfdtOn  : The Poisson dt pdf corresponding to the observed event rate from the on source data  
// hisdtOff : The resulting histogram of observed dt intervals from the off source data  
// pdfdtOff : The Poisson dt pdf corresponding to the observed event rate from the off source data
// zcor     : Use redshift corrected arrival times (kTRUE) or not (kFALSE)
 

 hisdtOn.Reset();
 hisdtOff.Reset();

 Int_t fTunits=TMath::Nint(fBurstParameters->GetSignal("Tunits"));
 TString tu="days";
 if (fTunits==1) tu="hours";
 if (fTunits==2) tu="sec";
 if (fTunits==3) tu="ns";
 if (fTunits==4) tu="ps";

 Float_t fTfact=fBurstParameters->GetSignal("Tfact");

 TString nametot;
 TString namebkg;
 TString varname;

 // Create the automatically binned delta t histograms
 if (!zcor)
 {
  nametot.Form("hOndt%-i",ndt);
  namebkg.Form("hOffdt%-i",ndt);
  varname="dtime";
 }
 else
 {
  nametot.Form("hOnZdt%-i",ndt);
  namebkg.Form("hOffZdt%-i",ndt);
  varname="dtimez";
 }
 TH1F* hOndt=(TH1F*)fBurstHistos.FindObject(nametot);
 TH1F* hOffdt=(TH1F*)fBurstHistos.FindObject(namebkg);

 TString title;
 Double_t deltatbin=0;
 Int_t nOndt=0;
 Int_t nOffdt=0;

 if (!hOndt)
 {
  NcSample sOndt=fBurstOnMatch.GetDtSample(varname,ndt);
  nOndt=sOndt.GetN();

  if (nOndt)
  {
   hOndt=new TH1F(nametot,"histo",10000,1,0);

   hOndt->SetBuffer(nOndt);

   for (Int_t i=1; i<=nOndt; i++)
   {
    hOndt->Fill(sOndt.GetEntry(i,1));
   }

   hOndt->BufferEmpty(1);

   // Create title and labels for this delta t histograms
   deltatbin=hOndt->GetXaxis()->GetBinWidth(1);
   if (!zcor) title.Form("Time intervals between %-i consecutive events in the on-source time window;dt in %-s;Counts per %-.3g %-s",(ndt+1),tu.Data(),deltatbin,tu.Data());
   if (zcor)  title.Form("Time intervals between %-i consecutive events in the redshift corrected on-source time window;dt in %-s;Counts per %-.3g %-s",(ndt+1),tu.Data(),deltatbin,tu.Data());
   hOndt->SetTitle(title);

   fBurstHistos.Add(hOndt);
  }
 }

 if (!hOffdt)
 {
  NcSample sOffdt=fBurstOffMatch.GetDtSample(varname,ndt);
  nOffdt=sOffdt.GetN();

  if (nOffdt)
  {
   hOffdt=new TH1F(namebkg,"histo",10000,1,0);

   hOffdt->SetBuffer(nOffdt);

   for (Int_t i=1; i<=nOffdt; i++)
   {
    hOffdt->Fill(sOffdt.GetEntry(i,1));
   }

   hOffdt->BufferEmpty(1);

   // Create title and labels for this delta t histograms
   deltatbin=hOffdt->GetXaxis()->GetBinWidth(1);
   if (!zcor) title.Form("Time intervals between %-i consecutive events in the off-source time window;dt in %-s;Counts per %-.3g %-s",(ndt+1),tu.Data(),deltatbin,tu.Data());
   if (zcor)  title.Form("Time intervals between %-i consecutive events in the redshift corrected off-source time window;dt in %-s;Counts per %-.3g %-s",(ndt+1),tu.Data(),deltatbin,tu.Data());
   hOffdt->SetTitle(title);

   fBurstHistos.Add(hOffdt);
  }
 }

 if (hOndt || hOffdt) cout << " The following arrival time interval (dt) histograms have been generated :" << endl;
 if (hOndt) cout << " ... " << hOndt->GetName() << " : " << hOndt->GetTitle() << endl;      
 if (hOffdt) cout << " ... " << hOffdt->GetName() << " : " << hOffdt->GetTitle() << endl;

 if (hOndt) hisdtOn=*hOndt;
 if (hOffdt) hisdtOff=*hOffdt;

 // Creation of the Poisson based dt PDFs from the observed data for a background only hypothesis 

 Double_t rateOn=fBurstParameters->GetSignal("rateOn");
 Double_t rateOff=fBurstParameters->GetSignal("rateOff");

 // Convert the event rates into the correct units
 rateOn*=fTfact;
 rateOff*=fTfact;

 if (zcor) // Redshift corrected arrival times
 {
  Double_t nevt=fBurstOnMatch.GetN();
  Double_t tmin=fBurstOnMatch.GetMinimum(varname);
  Double_t tmax=fBurstOnMatch.GetMaximum(varname);
  Double_t twin=tmax-tmin;
  rateOn=0;
  if (twin) rateOn=nevt/twin;
  nevt=fBurstOffMatch.GetN();
  tmin=fBurstOffMatch.GetMinimum(varname);
  tmax=fBurstOffMatch.GetMaximum(varname);
  twin=tmax-tmin;
  rateOff=0;
  if (twin) rateOff=nevt/twin;
 }

 // Determine the corresponding dt PDFs based on Poisson statistics
 // Only the bkg dt PDF is used, since this may be obtained from off-source measurements.
 // Using the total dt PDF would artificially lower the sensitivity due to possible signal events.

 NcMath math;
 pdfdtOn=math.PoissonDtDist(rateOn,ndt);   // Poisson dt pdf for the observed on source event rate
 pdfdtOff=math.PoissonDtDist(rateOff,ndt); // Poisson dt pdf for the observed off source event rate

 if (!zcor)
 {
  nametot.Form("hpdfOndt%-i",ndt);
  namebkg.Form("hpdfOffdt%-i",ndt);
 }
 else
 {
  nametot.Form("hpdfOnZdt%-i",ndt);
  namebkg.Form("hpdfOffZdt%-i",ndt);
 }
 TH1* hpdfOndt=(TH1*)fBurstHistos.FindObject(nametot);
 TH1* hpdfOffdt=(TH1*)fBurstHistos.FindObject(namebkg);

 Double_t xmaxfdt=1;
 Double_t deltatmax=0;
 if (hOndt)
 {
  deltatmax=hOndt->GetXaxis()->GetXmax();
  xmaxfdt=deltatmax;
 }
 if (hOffdt)
 {
  deltatmax=hOffdt->GetXaxis()->GetXmax();
  if (deltatmax>xmaxfdt) xmaxfdt=deltatmax;
 }
 Int_t npx=10000;
 pdfdtOn.SetRange(0,xmaxfdt);
 pdfdtOn.SetNpx(npx);
 pdfdtOff.SetRange(0,xmaxfdt);
 pdfdtOff.SetNpx(npx);

 // Provide the dt PDFs as histograms in the output file
 if (!hpdfOndt && nOndt)
 {
  hpdfOndt=(TH1*)pdfdtOn.GetHistogram()->Clone();
  hpdfOndt->SetName(nametot.Data());
  title.Form("dt in %-s",tu.Data());
  hpdfOndt->GetXaxis()->SetTitle(title);
  fBurstHistos.Add(hpdfOndt);
 }

 if (!hpdfOffdt && nOffdt)
 {
  hpdfOffdt=(TH1*)pdfdtOff.GetHistogram()->Clone();
  hpdfOffdt->SetName(namebkg.Data());
  hpdfOffdt->GetXaxis()->SetTitle(title);
  fBurstHistos.Add(hpdfOffdt);
 }

 if (hpdfOndt || hpdfOffdt) cout << " The following arrival time interval (dt) PDFs have been generated :" << endl;
 if (hpdfOndt) cout << " ... " << hpdfOndt->GetName() << " : " << hpdfOndt->GetTitle() << endl;      
 if (hpdfOffdt) cout << " ... " << hpdfOffdt->GetName() << " : " << hpdfOffdt->GetTitle() << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ListBurstHistograms() const
{
// Provide a list of all the stored transient burst histograms
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 Int_t nh=fBurstHistos.GetEntries();
 cout << endl;
 cout << " =============== The following " << nh << " histograms have been generated ===============" << endl;
 for (Int_t ih=0; ih<nh; ih++)
 {
  TObject* hx=fBurstHistos.At(ih);
  if (!hx) continue;
  cout << " " << hx->GetName() << " : " << hx->GetTitle() << endl;
 }
 cout << " ===============================================================================" << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::WriteBurstHistograms(TString filename)
{
// Write all the generated transient burst histograms to a ROOT file with the specified filename.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 // The output file for the produced histograms
 TFile fout(filename.Data(),"RECREATE","NcAstrolab analysis results");

 // Write all the histos to the output file
 Int_t nh=fBurstHistos.GetEntries();
 for (Int_t ih=0; ih<nh; ih++)
 {
  TObject* hx=fBurstHistos.At(ih);
  if (!hx) continue;
  hx->Write();
 }

 fout.Write();

 cout << endl;
 cout << " *" << ClassName() << "::WriteBurstHistograms* All generated histograms have been written to file " << filename << endl;
 ListBurstHistograms();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::SkyMapPanel()
{
// Main steering routine for the SkyMapPanel GUI.
// Invokation of this memberfunction will open an interactive GUI
// to provide a user friendly interface to this NcAstrolab instance
// to Enter, Remove, List and Display entries and their properties.

 if (gROOT->IsBatch())
 {
  printf("\n *%-s::SkyMapPanel* GUI is not available in batch mode. \n",ClassName());
  return;
 }

 // Select the lab timestamp as default
 fMapTS.SetMJD(fMJD,fJsec,fJns,fJps,"A");
 
 // Import current Lab settings
 Double_t l=0;
 Double_t b=0;
 GetLabPosition(l,b,"deg");
 fMapLabLocL=l;
 fMapLabLocB=b;
 fMapLabExpName=GetExperiment();
 fMapLabId=fLabId;
 // Get the lab timestamp data in the text box
 UInt_t year=0;
 UInt_t month=0;
 UInt_t day=0;
 GetDate(kTRUE,0,&year,&month,&day);
 GetDayTimeString("UTC",3,0,0,&fMapTime);
 fMapDate.Form("%02i-%02i-%-i",day,month,year);
 fMapTime.ReplaceAll(" UTC","");
 fMapDateTime=fMapDate;
 fMapDateTime+="/";
 fMapDateTime+=fMapTime;

 // Re-invokation of the SkyMapPanel
 if (fSkyMapPanel)
 {
  // Import the current lab settings
  Int_t idx=1;
  TString names[9]={"User","IceCube","RNO-G","ARA","Amanda","WSRT","Astron","Greenwich","ARCA"};
  for (Int_t i=1; i<=9; i++)
  {
   if (names[i-1]==fExperiment) idx=i;
  }
  fMapLabE->Select(idx,kTRUE);

  MapLocEnter();

  // The Lab UTC time
  fMapTStimetype->Select(1,kTRUE);
  MapTimeType(10);

  // Map all subwindows of main frame
  fSkyMapPanel->MapSubwindows();

  // Initialize the layout algorithm
  fSkyMapPanel->Resize(fSkyMapPanel->GetDefaultSize());

  // Map main frame
  fSkyMapPanel->MapWindow();
  
  return;
 }

 // New initialization of the SkyMapPanel
 UInt_t border=5;
 fSkyMapPanel=new TGMainFrame(gClient->GetRoot());
 fSkyMapPanel->SetWindowName("SkyMapPanel");
 fSkyMapPanel->Connect("CloseWindow()","NcAstrolab",this,"MapClose()");

 // Define the various sub-frames and fill them with the various panels 
 TGCompositeFrame* frames[4]={0,0,0,0};
 TGLayoutHints* layouts[4]={0,0,0,0};

 // The Lab specification and timestamp frame
 frames[0]=new TGCompositeFrame(fSkyMapPanel,1,1,kHorizontalFrame|kSunkenFrame);
 layouts[0]=new TGLayoutHints(kLHintsExpandX,border,border,0,0);
 LabLocationPanel(frames[0]);
 TimestampPanel(frames[0]);

 // The local reference and info frame
 frames[1]=new TGCompositeFrame(fSkyMapPanel,1,1,kHorizontalFrame|kSunkenFrame);
 layouts[1]=new TGLayoutHints(kLHintsExpandX,border,border,0,0);
 LabLocalFramePanel(frames[1]);
 InfoPanel(frames[1]);

 // The entries frame
 frames[2]=new TGCompositeFrame(fSkyMapPanel,1,1,kHorizontalFrame|kSunkenFrame);
 layouts[2]=new TGLayoutHints(kLHintsExpandX,border,border,0,0);
 EntriesPanel(frames[2]);

 // The drawing/listing options and command buttons frame
 frames[3]=new TGCompositeFrame(fSkyMapPanel,1,1,kHorizontalFrame|kSunkenFrame);
 layouts[3]=new TGLayoutHints(kLHintsExpandX,border,border,0,0);
 MapListOptionsPanel(frames[3]);
 CommandPanel(frames[3]);

 // Add all subframes to the mainframe
 for (Int_t i=0; i<4; i++)
 {
  if (frames[i]) fSkyMapPanel->AddFrame(frames[i],layouts[i]);
 }

 // Map all subwindows of main frame
 fSkyMapPanel->MapSubwindows();

 // Initialize the layout algorithm
 fSkyMapPanel->Resize(fSkyMapPanel->GetDefaultSize());

 // Map main frame
 fSkyMapPanel->MapWindow();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::LabLocationPanel(TGCompositeFrame* frame)
{
// Internal memberfunction to render the Lab location and experiment specification GUI panel.

 if (!frame) return;

 TGGroupFrame* panel=new TGGroupFrame(frame,"Lab longitude, latitude, experiment site and detector ID",kHorizontalFrame);
 panel->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(panel);

 // The lab longitude entry field
 fMapLabLBI[0]=new TGNumberEntryField(panel,-1,fMapLabLocL);
 fMapLabLBI[0]->SetToolTipText("Longitude");
 fMapLabLBI[0]->Connect("TextChanged(const char*)","NcAstrolab",this,"MapLocl(const char*)");
 fMapLabLBI[0]->Resize(65,20);
 panel->AddFrame(fMapLabLBI[0]);

 // The lab latitude entry field
 fMapLabLBI[1]=new TGNumberEntryField(panel,-1,fMapLabLocB);
 fMapLabLBI[1]->SetToolTipText("Latitude");
 fMapLabLBI[1]->Connect("TextChanged(const char*)","NcAstrolab",this,"MapLocb(const char*)");
 fMapLabLBI[1]->Resize(65,20);
 panel->AddFrame(fMapLabLBI[1]);

 // The lab longitude and latitude type selection box
 fMapLabU=new TGComboBox(panel);
 fMapLabU->Connect("Selected(Int_t)","NcAstrolab",this,"MapUloc(Int_t)");
 fMapLabU->AddEntry("deg",1);
 fMapLabU->AddEntry("dms",2);
 fMapLabU->AddEntry("hms",3);
 fMapLabU->AddEntry("rad",4);
 fMapLabU->Resize(50,20);
 panel->AddFrame(fMapLabU);
 fMapLabU->Select(1,kTRUE);

 // The experiment name selection box
 fMapLabE=new TGComboBox(panel);
 fMapLabE->Connect("Selected(Int_t)","NcAstrolab",this,"MapExperiment(Int_t)");
 Int_t idx=1;
 TString names[9]={"User","IceCube","RNO-G","ARA","Amanda","WSRT","Astron","Greenwich","ARCA"};
 for (Int_t i=1; i<=9; i++)
 {
  fMapLabE->AddEntry(names[i-1],i);
  if (names[i-1]==fExperiment) idx=i;
 }
 fMapLabE->Resize(90,20);
 panel->AddFrame(fMapLabE);
 fMapLabE->Select(idx,kTRUE);

 // The detector element ID field
 fMapLabLBI[2]=new TGNumberEntryField(panel,-1,fLabId,TGNumberFormat::kNESInteger);
 fMapLabLBI[2]->SetToolTipText("The (optional) detector element ID (0=global)");
 fMapLabLBI[2]->Connect("TextChanged(const char*)","NcAstrolab",this,"MapLocId(const char*)");
 fMapLabLBI[2]->Resize(40,20);
 panel->AddFrame(fMapLabLBI[2]);

 // The button to enter the provided data
 TGTextButton* enter=new TGTextButton(panel,"Enter");
 enter->SetToolTipText("Enter the provided data");
 enter->Connect("Clicked()","NcAstrolab",this,"MapLocEnter()");
 TGLayoutHints* Lenter=new TGLayoutHints(kLHintsCenterX,10,0,0,0);
 panel->AddFrame(enter,Lenter);

 MapLocEnter();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapLocl(const char* text)
{
// Action on the lab longitude entry field
 TString s=text;
 fMapLabLocL=s.Atof();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapLocb(const char* text)
{
// Action on the lab latitude entry field
 TString s=text;
 fMapLabLocB=s.Atof();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapUloc(Int_t i)
{
// Action on a button from the lab angular unit selection box
 TString s[4]={"deg","dms","hms","rad"};
 if (i<=4) fMapLabLocU=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapExperiment(Int_t i)
{
// Action on a button from the lab experiment site box
 TString s[9]={"User","IceCube","RNO-G","ARA","Amanda","WSRT","Astron","Greenwich","ARCA"};
 if (i<=9) fMapLabExpName=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapLocId(const char* text)
{
// Action on the lab detector element Id entry field
 TString s=text;
 fMapLabId=s.Atoi();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapLocEnter()
{
// Enter the provided lab location entry

 fSkyMapPanel->RequestFocus();

 SetLabPosition(fMapLabLocL,fMapLabLocB,fMapLabLocU);

 if (fMapLabExpName=="User")
 {
  SetNameTitle("User","Virtual Lab for general use");
  MapLocId("0");
  printf("\n *** Settings adopted for a virtual lab for general use. *** \n");
 }
 else
 {
  SetExperiment(fMapLabExpName,fMapLabId);
  fMapLabId=GetLabDetectorId();
  // Update the longitude, latitude and detector ID  selection boxes
  GetLabPosition(fMapLabLocL,fMapLabLocB,"deg");
  TString s;
  s.Form("%-.3f",fMapLabLocL);
  fMapLabLBI[0]->SetText(s);
  s.Form("%-.3f",fMapLabLocB);
  fMapLabLBI[1]->SetText(s);
  fMapLabU->Select(1,kTRUE);
  MapUloc(1);
  s.Form("%-i",fMapLabId);
  fMapLabLBI[2]->SetText(s);
  printf("\n *** Lab settings adopted for the %-s location. *** \n",fMapLabExpName.Data());
 }

 // Update the local frame data for the selected lab
 TString val;
 for (Int_t i=0; i<6; i++)
 {
  val.Form("%-.2f",fAxes[i]);
  if (fMapLabLframe[i]) fMapLabLframe[i]->SetText(val);
 }
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::TimestampPanel(TGCompositeFrame* frame)
{
// Internal memberfunction to render the Timestamp specification GUI panel.

 if (!frame) return;

 TGGroupFrame* panel=new TGGroupFrame(frame,"Timestamp to be used for Entries, List and Map",kHorizontalFrame);
 panel->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(panel);

 // The Date/Time textbox
 fMapTSdatetime=new TGTextEntry(panel,fMapDateTime.Data());
 fMapTSdatetime->SetToolTipText("Date/Time as dd-mm-yyyy/hh:mm:ss.sss or MJD");
 fMapTSdatetime->Connect("TextChanged(const char*)","NcAstrolab",this,"MapDateTime(const char*)");
 fMapTSdatetime->SetAlignment(kTextRight);
 fMapTSdatetime->Resize(170,20);
 panel->AddFrame(fMapTSdatetime);

 // The Time type selection box
 fMapTStimetype=new TGComboBox(panel);
 fMapTStimetype->Connect("Selected(Int_t)","NcAstrolab",this,"MapTimeType(Int_t)");
 fMapTStimetype->AddEntry("UTC",1);
 fMapTStimetype->AddEntry("LMT",2);
 fMapTStimetype->AddEntry("UT1",3);
 fMapTStimetype->AddEntry("MJD",4);
 fMapTStimetype->AddEntry("JD",5);
 fMapTStimetype->AddEntry("TJD",6);
 fMapTStimetype->AddEntry("Unix",7);
 fMapTStimetype->AddEntry("GPS",8);
 fMapTStimetype->AddEntry("TAI",9);
 fMapTStimetype->AddEntry("TT",10);
 fMapTStimetype->AddEntry("SysClock",11);
 fMapTStimetype->AddEntry("Lab",12);
 fMapTStimetype->AddEntry("EntryName",13);
 fMapTStimetype->Resize(100,20);
 panel->AddFrame(fMapTStimetype);
 fMapTStimetype->Select(1,kTRUE);
 MapTimeType(1);

 // The Lab TS modification button
 TGTextButton* labTS=new TGTextButton(panel,"Store as Lab TS");
 labTS->SetToolTipText("Store the current selection as Lab timestamp");
 labTS->Connect("Clicked()","NcAstrolab",this,"MapLabTS()");
 TGLayoutHints* LlabTS=new TGLayoutHints(kLHintsCenterX,10,0,0,0);
 panel->AddFrame(labTS,LlabTS);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapDateTime(const char* text)
{
// Action on the time specification entry field
 fMapDateTime=text;
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapTimeType(Int_t i)
{
// Action on the time type selection box
 TString s[13]={"UTC","LMT","UT1","MJD","JD","TJD","Unix","GPS","TAI","TT","SysClock","Lab","EntryName"};

 if (i<=13) fMapTimeType=s[i-1];

 if (fMapTimeType=="SysClock" || fMapTimeType=="Lab") SetMapTS();
 if (fMapTimeType=="MJD" || fMapTimeType=="JD" || fMapTimeType=="TJD" || fMapTimeType=="Unix" || fMapTimeType.Contains("Name")) fMapTSdatetime->SetText("");
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapLabTS()
{
// Use the current date/time selection as the Lab timestamp

 SetMapTS();

 fSkyMapPanel->RequestFocus();

 Int_t mjd,sec,ns;
 fMapTS.GetMJD(mjd,sec,ns);
 Int_t ps=fMapTS.GetPs();
 this->SetMJD(mjd,sec,ns,ps,"U");

 printf("\n *** Lab timestamp modified *** \n");
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::LabLocalFramePanel(TGCompositeFrame* frame)
{
// Internal memberfunction to render the Lab local reference frame specification GUI panel.

 if (!frame) return;

 TGGroupFrame* panel=new TGGroupFrame(frame,"Local frame axes orientations w.r.t. X0=South, Y0=East, Z0=Zenith)",kHorizontalFrame);
 panel->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(panel);

 // The local X-axis theta (=zenith) angle w.r.t. the MRF
 fMapLabLframe[0]=new TGNumberEntryField(panel,-1,fAxes[0]);
 fMapLabLframe[0]->SetToolTipText("Local X-axis zenith angle in deg");
 fMapLabLframe[0]->Resize(55,20);
 panel->AddFrame(fMapLabLframe[0]);

 // The local X-axis phi angle w.r.t. the MRF
 fMapLabLframe[1]=new TGNumberEntryField(panel,-1,fAxes[1]);
 fMapLabLframe[1]->SetToolTipText("Local X-axis phi angle in deg");
 fMapLabLframe[1]->Resize(55,20);
 panel->AddFrame(fMapLabLframe[1]);

 // The local Y-axis theta (=zenith) angle w.r.t. the MRF
 fMapLabLframe[2]=new TGNumberEntryField(panel,-1,fAxes[2]);
 fMapLabLframe[2]->SetToolTipText("Local Y-axis zenith angle in deg");
 fMapLabLframe[2]->Resize(55,20);
 panel->AddFrame(fMapLabLframe[2]);

 // The local Y-axis phi angle w.r.t. the MRF
 fMapLabLframe[3]=new TGNumberEntryField(panel,-1,fAxes[3]);
 fMapLabLframe[3]->SetToolTipText("Local Y-axis phi angle in deg");
 fMapLabLframe[3]->Resize(55,20);
 panel->AddFrame(fMapLabLframe[3]);

 // The local Z-axis theta (=zenith) angle w.r.t. the MRF
 fMapLabLframe[4]=new TGNumberEntryField(panel,-1,fAxes[4]);
 fMapLabLframe[4]->SetToolTipText("Local Z-axis zenith angle in deg");
 fMapLabLframe[4]->Resize(55,20);
 panel->AddFrame(fMapLabLframe[4]);

 // The local Z-axis phi angle w.r.t. the MRF
 fMapLabLframe[5]=new TGNumberEntryField(panel,-1,fAxes[5]);
 fMapLabLframe[5]->SetToolTipText("Local Z-axis phi angle in deg");
 fMapLabLframe[5]->Resize(55,20);
 panel->AddFrame(fMapLabLframe[5]);

 MapLabLframeEnter();

 // The button to enter the provided data
 TGTextButton* enter=new TGTextButton(panel,"Enter");
 enter->SetToolTipText("Enter the provided data");
 enter->Connect("Clicked()","NcAstrolab",this,"MapLabLframeEnter()");
 TGLayoutHints* Lenter=new TGLayoutHints(kLHintsLeft,10,0,0,0);
 panel->AddFrame(enter,Lenter);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapLabLframeEnter()
{
// Enter the specified local frame parameters.
// Note : Local frame parameters for established experimental sites
//        may NOT be modified.

 fSkyMapPanel->RequestFocus();

 if (fMapLabExpName=="IceCube" || fMapLabExpName=="RNO-G" || fMapLabExpName=="Amanda")
 {
  printf("\n *** Local frame will NOT be changed for experiment site %-s *** \n",fMapLabExpName.Data());

  // Update the local frame data for the selected lab
  TString val;
  for (Int_t i=0; i<6; i++)
  {
   val.Form("%-.2f",fAxes[i]);
   if (fMapLabLframe[i]) fMapLabLframe[i]->SetText(val);
  }
  return;
 }

 for (Int_t i=0; i<6; i++)
 {
  fAxes[i]=fMapLabLframe[i]->GetNumber();
 }
 
 SetLocalFrame(fAxes[0],fAxes[1],fAxes[2],fAxes[3],fAxes[4],fAxes[5]);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::InfoPanel(TGCompositeFrame* frame)
{
// The Information panel

 if (!frame) return;

 TGGroupFrame* panel=new TGGroupFrame(frame,"Informative output",kHorizontalFrame);
 panel->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(panel);

 // The info category selection box
 TGComboBox* cinfo=new TGComboBox(panel);
 cinfo->Connect("Selected(Int_t)","NcAstrolab",this,"MapCinfo(Int_t)");
 cinfo->AddEntry("Lab",1);
 cinfo->AddEntry("TSbox",2);
 cinfo->AddEntry("Entry",3);
 cinfo->AddEntry("Nstore",4);
 cinfo->Resize(60,20);
 panel->AddFrame(cinfo);
 cinfo->Select(1,kTRUE);
 MapCinfo(1);

 // The lab info time type selection box
 TGComboBox* tinfo=new TGComboBox(panel);
 tinfo->Connect("Selected(Int_t)","NcAstrolab",this,"MapTinfo(Int_t)");
 tinfo->AddEntry("LAT/LAST",1);
 tinfo->AddEntry("LMT/LMST",2);
 tinfo->AddEntry("Julian",3);
 tinfo->AddEntry("UT1",4);
 tinfo->AddEntry("UTC",5);
 tinfo->AddEntry("TAI",6);
 tinfo->AddEntry("GPS",7);
 tinfo->AddEntry("TT",8);
 tinfo->AddEntry("Unix",9);
 tinfo->Resize(85,20);
 panel->AddFrame(tinfo);
 tinfo->Select(1,kTRUE);
 MapTinfo(1);

 // The lab info longitude and latitude type selection box
 TGComboBox* uinfo=new TGComboBox(panel);
 uinfo->Connect("Selected(Int_t)","NcAstrolab",this,"MapUinfo(Int_t)");
 uinfo->AddEntry("deg",1);
 uinfo->AddEntry("dms",2);
 uinfo->AddEntry("hms",3);
 uinfo->AddEntry("rad",4);
 uinfo->Resize(50,20);
 panel->AddFrame(uinfo);
 uinfo->Select(1,kTRUE);
 MapUinfo(1);

 // The entry name textbox
 TGTextEntry* mapiname=new TGTextEntry(panel,"");
 mapiname->SetToolTipText("Stored entry name for info");
 mapiname->Connect("TextChanged(const char*)","NcAstrolab",this,"MapIname(const char*)");
 mapiname->SetAlignment(kTextRight);
 mapiname->Resize(100,20);
 panel->AddFrame(mapiname);

 // The info button
 TGTextButton* info=new TGTextButton(panel,"Info");
 info->Connect("Clicked()","NcAstrolab",this,"MapInfo()");
 info->SetToolTipText("Provide info on the specified item");
 TGLayoutHints* Linfo=new TGLayoutHints(kLHintsLeft,10,0,0,0);
 panel->AddFrame(info,Linfo);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapCinfo(Int_t i)
{
// Action on a button from the category selection box
 TString s[4]={"Lab","TS box","EntryName","Nstore"};
 if (i<=4) fMapCinfo=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapTinfo(Int_t i)
{
// Action on a button from the time type selection box
 if (i==1)
 {
  fMapTinfo=-1;
 }
 else
 {
  fMapTinfo=i-1;
 }
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapUinfo(Int_t i)
{
// Action on a button from the info angular format selection box
 TString s[4]={"deg","dms","hms","rad"};
 if (i<=4) fMapUinfo=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapIname(const char* text)
{
// Treating the text of the entry name for the information panel.
 fMapIname=text; 
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapInfo()
{
// Provide the requested info

 SetMapTS();

 TString modes[9]={"LAT/LAST","LMT/LMST","Julian","UT1","UTC","TAI","GPS","TT","Unix"};
 TString datetime;

 if (fMapCinfo=="Nstore")
 {
  Int_t nref=GetNsignals(0);
  Int_t nmeas=GetNsignals(1);
  Int_t ntot=nref+nmeas;
  printf("\n *** Info about the stored entries *** \n");
  printf(" Ntotal=%-i Nrefs=%-i Nmeas=%-i \n",ntot,nref,nmeas);
 }
 else if (fMapCinfo=="Lab")
 {
  if (fMapTinfo<3)
  {
   printf("\n *** Info about the current Lab settings *** \n");
   Data(fMapTinfo,fMapUinfo);
  }
  else
  {
   printf("\n *** Info for the Lab timestamp *** \n");
   if (fMapTinfo==8)
   {
    printf(" Unix time : %-.12f \n",GetUnixTime());
   }
   else
   {
    datetime=GetDayTimeString(modes[fMapTinfo],12);
    printf(" %-s \n",datetime.Data());
   }
  }
 }
 else if (fMapCinfo.Contains("TS"))
 {
  printf("\n *** Info for the timestamp in the user selection box *** \n");
  if (fMapTinfo<3)
  {
   printf(" Lab time offset w.r.t. UT : "); PrintTime(fToffset,12); printf("\n");
   fMapTS.Date(fMapTinfo,fToffset);
   fMapTS.Date(4);
  }
  else
  {
   if (fMapTinfo==8)
   {
    printf(" Unix time : %-.12f \n",fMapTS.GetUnixTime());
   }
   else
   {
    datetime=fMapTS.GetDayTimeString(modes[fMapTinfo],12);
    printf(" %-s \n",datetime.Data());
   }
  }
 }
 else // Information of the specified entry name
 {
  NcSignal* sx=0;
  NcTimestamp* tx=0;
  sx=GetSignal(fMapIname,0);
  if(!sx) sx=GetSignal(fMapIname,1);
  if (sx)
  {
   printf("\n *** Info for entry : %-s *** \n",fMapIname.Data());
   sx->Data("sph",fMapUinfo);
   tx=sx->GetTimestamp();
   if (tx)
   {
    NcTimestamp tx2(*tx);
    tx2.LoadUTCparameterFiles();
    if (fMapTinfo<3)
    {
     printf(" Lab time offset w.r.t. UT : "); PrintTime(fToffset,12); printf("\n");
     tx2.Date(fMapTinfo,fToffset);
     tx2.Date(4);
    }
    else
    {
     if (fMapTinfo==8)
     {
      printf(" Unix time : %-.12f \n",tx2.GetUnixTime());
     }
     else
     {
      datetime=tx2.GetDayTimeString(modes[fMapTinfo],12);
      printf(" %-s \n",datetime.Data());
     }
    }
   }
  }
  else
  {
   printf("\n *** No entry found with name : %-s *** \n",fMapIname.Data());
  }
 }
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::EntriesPanel(TGCompositeFrame* frame)
{
// Internal memberfunction to render the GUI panel to enter c.q. remove data entries.

 if (!frame) return;

 TGGroupFrame* panel=new TGGroupFrame(frame,"Entries in (a,b) coordinates",kHorizontalFrame);
 panel->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(panel);

 // The a coordinate entry field
 TGNumberEntryField* ea=new TGNumberEntryField(panel,-1,0);
 ea->SetToolTipText("Angle a");
 ea->Connect("TextChanged(const char*)","NcAstrolab",this,"MapEa(const char*)");
 ea->Resize(100,20);
 panel->AddFrame(ea);
 MapEa("0");

 // The a coordinate type selection box
 TGComboBox* ua=new TGComboBox(panel);
 ua->Connect("Selected(Int_t)","NcAstrolab",this,"MapUa(Int_t)");
 ua->AddEntry("deg",1);
 ua->AddEntry("dms",2);
 ua->AddEntry("hms",3);
 ua->AddEntry("rad",4);
 ua->AddEntry("hrs",5);
 ua->Resize(50,20);
 panel->AddFrame(ua);
 ua->Select(1,kTRUE);
 MapUa(1);

 // The b coordinate entry field
 TGNumberEntryField* eb=new TGNumberEntryField(panel,-1,0);
 eb->SetToolTipText("Angle b");
 eb->Connect("TextChanged(const char*)","NcAstrolab",this,"MapEb(const char*)");
 eb->Resize(100,20);
 panel->AddFrame(eb);
 MapEb("0");

 // The b coordinate type selection box
 TGComboBox* ub=new TGComboBox(panel);
 ub->Connect("Selected(Int_t)","NcAstrolab",this,"MapUb(Int_t)");
 ub->AddEntry("deg",1);
 ub->AddEntry("dms",2);
 ub->AddEntry("hms",3);
 ub->AddEntry("rad",4);
 ub->AddEntry("hrs",5);
 ub->Resize(50,20);
 panel->AddFrame(ub);
 ub->Select(1,kTRUE);
 MapUb(1);

 TGComboBox* ecoords=new TGComboBox(panel);
 ecoords->Connect("Selected(Int_t)","NcAstrolab",this,"MapEcoord(Int_t)");
 ecoords->AddEntry("(ra,dec) (J2000)",1);
 ecoords->AddEntry("(ra,dec) (Mean)",2);
 ecoords->AddEntry("(ra,dec) (True)",3);
 ecoords->AddEntry("(ra,dec) (B1950)",4);
 ecoords->AddEntry("Galactic (l,b)",5);
 ecoords->AddEntry("Ecliptic (l,b)",6);
 ecoords->AddEntry("Horizon (azi,zen)",7);
 ecoords->AddEntry("ICR (l,b)",8);
 ecoords->AddEntry("Local (theta,phi)",9);
 ecoords->AddEntry("pdir (theta,phi)",10);
 ecoords->Resize(125,20);
 panel->AddFrame(ecoords);
 ecoords->Select(1,kTRUE);
 MapEcoord(1);

 // The signal type
 TGComboBox* etypes=new TGComboBox(panel);
 etypes->Connect("Selected(Int_t)","NcAstrolab",this,"MapEtype(Int_t)");
 etypes->AddEntry("Meas",1);
 etypes->AddEntry("Ref",2);
 etypes->Resize(55,20);
 panel->AddFrame(etypes);
 etypes->Select(1,kTRUE);
 MapEtype(1);

 // The (optional) signal name
 TGTextEntry* ename=new TGTextEntry(panel,"");
 ename->SetToolTipText("The (optional) entry name");
 ename->Connect("TextChanged(const char*)","NcAstrolab",this,"MapEname(const char*)");
 ename->SetAlignment(kTextRight);
 ename->Resize(100,20);
 panel->AddFrame(ename);

 // The button to Enter the entry
 TGTextButton* enter=new TGTextButton(panel,"Enter");
 enter->SetToolTipText("Enter the provided entry");
 enter->Connect("Clicked()","NcAstrolab",this,"MapEnter()");
 TGLayoutHints* Lenter=new TGLayoutHints(kLHintsCenterX,10,0,0,0);
 panel->AddFrame(enter,Lenter);

 // The button to Remove the entry
 TGTextButton* remove=new TGTextButton(panel,"Remove");
 remove->SetToolTipText("Remove the entry specified by type and name pattern (name=* means all)");
 remove->Connect("Clicked()","NcAstrolab",this,"MapRemove()");
 TGLayoutHints* Lremove=new TGLayoutHints(kLHintsCenterX,10,0,0,0);
 panel->AddFrame(remove,Lremove);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapEa(const char* text)
{
// Action on the entries a coordinate specification field
 TString s=text;
 fMapEa=s.Atof();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapUa(Int_t i)
{
// Action on a button from the a coordinate angular format selection box
 TString s[5]={"deg","dms","hms","rad","hrs"};
 if (i<=5) fMapEua=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapEb(const char* text)
{
// Action on the entries b coordinate specification field
 TString s=text;
 fMapEb=s.Atof();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapUb(Int_t i)
{
// Action on a button from the b coordinate angular format selection box
 TString s[5]={"deg","dms","hms","rad","hrs"};
 if (i<=5) fMapEub=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapEcoord(Int_t i)
{
// Action on a button from the entry coordinate frame selection box
 TString system[10]={"equ","equ","equ","equ","gal","ecl","hor","icr","loc","pdir"};
 TString mode[4]={"J","M","T","B"};
 
 if (i<=10) fMapEcoord=system[i-1];

 if (i<=4) fMapEmode=mode[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapEtype(Int_t i)
{
// Action on a button from the entry type selection box
 fMapEtype=1;
 if (i==2) fMapEtype=0;
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapEname(const char* text)
{
// Action on the (optional) entry name selection field
 fMapEname=text;
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapEnter()
{
// Enter the provided entry

 SetMapTS(); // Set the selected timestamp

 fSkyMapPanel->RequestFocus();

 SetSignal(1,fMapEa,fMapEua,fMapEb,fMapEub,fMapEcoord,&fMapTS,-1,fMapEmode,fMapEname,fMapEtype);

 printf("\n *** Specified entry stored *** \n");
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapRemove()
{
// Remove the entry/entries specified by type and name pattern

 SetMapTS(); // Set the selected timestamp

 fSkyMapPanel->RequestFocus();

 Int_t nrem=0;
 nrem=RemoveSignals(fMapEname,fMapEtype,1);

 printf("\n *** Number of specified entries have been removed : %-i *** \n",nrem);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapListOptionsPanel(TGCompositeFrame* frame)
{
// Internal memberfunction to render the GUI panel to specify various Mapping and Listing options

 if (!frame) return;

 TGGroupFrame* panel=new TGGroupFrame(frame,"Map/List options for the (a,b) coordinates",kHorizontalFrame);
 panel->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(panel);

 // A frame with various additional settings
 TGVerticalFrame* render=new TGVerticalFrame(panel,1,1);
 panel->AddFrame(render);

 // The coordinate system selection box
 TGGroupFrame* coordsys=new TGGroupFrame(render,"Coordinate system",kHorizontalFrame);
 coordsys->SetTitlePos(TGGroupFrame::kCenter);
 render->AddFrame(coordsys);
 TGComboBox* dcoords=new TGComboBox(coordsys);
 dcoords->Connect("Selected(Int_t)","NcAstrolab",this,"MapDcoord(Int_t)");
 dcoords->AddEntry("Equatorial (J2000)",1);
 dcoords->AddEntry("Equatorial (Mean)",2);
 dcoords->AddEntry("Equatorial (True)",3);
 dcoords->AddEntry("Equatorial (B1950)",4);
 dcoords->AddEntry("Galactic",5);
 dcoords->AddEntry("Ecliptic",6);
 dcoords->AddEntry("Horizon",7);
 dcoords->AddEntry("ICR",8);
 dcoords->AddEntry("Local",9);
 dcoords->Resize(140,20);
 coordsys->AddFrame(dcoords);
 dcoords->Select(1,kTRUE);
 MapDcoord(1);

 // The Map representation selection box
 TGGroupFrame* mapview=new TGGroupFrame(render,"Map representation",kHorizontalFrame);
 mapview->SetTitlePos(TGGroupFrame::kCenter);
 render->AddFrame(mapview);
 TGComboBox* projs=new TGComboBox(mapview);
 projs->Connect("Selected(Int_t)","NcAstrolab",this,"MapProj(Int_t)");
 projs->AddEntry("Hammer projection",1);
 projs->AddEntry("Aitoff projection",2);
 projs->AddEntry("Mercator projection",3);
 projs->AddEntry("sin(b) vs. a",4);
 projs->AddEntry("b vs. a",5);
 projs->AddEntry("b vs. UT (0-24 hrs)",6);
 projs->AddEntry("b vs. LT (0-24 hrs)",7);
 projs->AddEntry("b vs. GST (0-24 hrs)",8);
 projs->AddEntry("b vs. LST (0-24 hrs)",9);
 projs->AddEntry("b vs. Day at UT",10);
 projs->AddEntry("b vs. Day at LT",11);
 projs->AddEntry("b vs. Day at GST",12);
 projs->AddEntry("b vs. Day at LST",13);
 projs->Resize(150,20);
 mapview->AddFrame(projs);
 projs->Select(1,kTRUE);
 MapProj(1);

 // The meridian representation options
 TGGroupFrame* meridian=new TGGroupFrame(render,"Meridian ordering",kHorizontalFrame);
 meridian->SetTitlePos(TGGroupFrame::kCenter);
 render->AddFrame(meridian);

 // The meridian mode selection box
 TGComboBox* mermode=new TGComboBox(meridian);
 mermode->Connect("Selected(Int_t)","NcAstrolab",this,"MapMerMode(Int_t)");
 mermode->AddEntry("Auto",1);
 mermode->AddEntry("---->",2);
 mermode->AddEntry("<----",3);
 mermode->Resize(55,20);
 meridian->AddFrame(mermode);
 mermode->Select(1,kTRUE);
 MapMerMode(1);

 // The meridian central value
 TGNumberEntryField* merc=new TGNumberEntryField(meridian,-1,0);
 merc->SetToolTipText("Central meridian position");
 merc->Connect("TextChanged(const char*)","NcAstrolab",this,"MapMerC(const char*)");
 merc->Resize(65,20);
 meridian->AddFrame(merc);
 MapMerC("0");

 // The meridian central value type selection box
 TGComboBox* meruc=new TGComboBox(meridian);
 meruc->Connect("Selected(Int_t)","NcAstrolab",this,"MapMerUc(Int_t)");
 meruc->AddEntry("deg",1);
 meruc->AddEntry("dms",2);
 meruc->AddEntry("hms",3);
 meruc->AddEntry("rad",4);
 meruc->Resize(50,20);
 meridian->AddFrame(meruc);
 meruc->Select(1,kTRUE);
 MapMerUc(1);

 // The options group
 TGVButtonGroup* options=new TGVButtonGroup(panel,"Options");
 options->SetTitlePos(TGGroupFrame::kCenter);
 options->Connect("Clicked(Int_t)","NcAstrolab",this,"MapDoptions(Int_t)");
 TGCheckButton* bhist=new TGCheckButton(options,"Hist");
 bhist->SetToolTipText("Project data in a histogram");
 TGCheckButton* bclr=new TGCheckButton(options,"Clr");
 bclr->SetToolTipText("Clear display before drawing");
 TGCheckButton* bref=new TGCheckButton(options,"Ref");
 bref->SetToolTipText("Display reference signals");
 TGCheckButton* bmeas=new TGCheckButton(options,"Meas");
 bmeas->SetToolTipText("Display measured signals");
 TGCheckButton* brefts=new TGCheckButton(options,"RefTS");
 brefts->SetToolTipText("Display/list each reference signal using its actual recorded timestamp");
 panel->AddFrame(options);
 options->Show();
 options->SetButton(1,kFALSE);
 options->SetButton(2,kTRUE);
 options->SetButton(3,kTRUE);
 options->SetButton(4,kTRUE);
 options->SetButton(5,kFALSE);
 fMapDoptions[0]=kFALSE;
 fMapDoptions[1]=kTRUE;
 fMapDoptions[2]=kTRUE;
 fMapDoptions[3]=kTRUE;
 fMapDoptions[4]=kFALSE;

 // The Nmax entry field
 TGNumberEntryField* nmax=new TGNumberEntryField(options,-1,-1,TGNumberFormat::kNESInteger);
 nmax->SetToolTipText("Max. number of Drawn/Listed entries per type (-1=all)");
 nmax->Connect("TextChanged(const char*)","NcAstrolab",this,"MapNmax(const char*)");
 nmax->Resize(40,20);
 options->AddFrame(nmax);
 MapNmax("-1");

 // The signal name pattern for matching
 TGTextEntry* sname=new TGTextEntry(options,"*");
 sname->SetToolTipText("The requested entry name pattern (*=all)");
 sname->Connect("TextChanged(const char*)","NcAstrolab",this,"MapDname(const char*)");
 sname->SetAlignment(kTextRight);
 sname->Resize(100,20);
 TGLayoutHints* Lsname=new TGLayoutHints(kLHintsLeft,0,0,5,0);
 options->AddFrame(sname,Lsname);
 MapDname("*");

 // The Ndigs entry field
 TGNumberEntryField* ndigs=new TGNumberEntryField(options,-1,1,TGNumberFormat::kNESInteger);
 ndigs->SetToolTipText("Number of digits for the listed entries");
 ndigs->Connect("TextChanged(const char*)","NcAstrolab",this,"MapNdigs(const char*)");
 ndigs->Resize(40,20);
 options->AddFrame(ndigs);
 MapNdigs("1");

 // A frame with various additional settings
 TGVerticalFrame* others=new TGVerticalFrame(panel,1,1);
 panel->AddFrame(others);

 // The Marker size frame
 TGGroupFrame* markers=new TGGroupFrame(others,"Marker settings",kHorizontalFrame);
 markers->SetTitlePos(TGGroupFrame::kCenter);
 others->AddFrame(markers);

 // The marker size
 TGNumberEntryField* marksize=new TGNumberEntryField(markers,-1,1);
 marksize->SetToolTipText("Marker size");
 marksize->Connect("TextChanged(const char*)","NcAstrolab",this,"MapMarkSize(const char*)");
 marksize->Resize(40,20);
 markers->AddFrame(marksize);
 MapMarkSize("1");

 // The marker style
 TGComboBox* markstyle=new TGComboBox(markers);
 markstyle->Connect("Selected(Int_t)","NcAstrolab",this,"MapMarkStyle(Int_t)");
 markstyle->AddEntry("Dot",1);
 markstyle->AddEntry("Star",2);
 markstyle->AddEntry("Square",3);
 markstyle->AddEntry("Utriangle",4);
 markstyle->AddEntry("Dtriangle",5);
 markstyle->AddEntry("Diamond",6);
 markstyle->AddEntry("Cross",7);
 markstyle->AddEntry("Ast",8);
 markstyle->AddEntry("Plus",9);
 markstyle->AddEntry("Times",10);
 markstyle->AddEntry("Circle",11);
 markstyle->AddEntry("oStar",12);
 markstyle->AddEntry("oSquare",13);
 markstyle->AddEntry("oUtriangle",14);
 markstyle->AddEntry("oDtriangle",15);
 markstyle->AddEntry("oDiamond",16);
 markstyle->AddEntry("oCross",17);
 markstyle->Resize(90,20);
 markers->AddFrame(markstyle);
 markstyle->Select(1,kTRUE);
 MapMarkStyle(1);

 // The marker color selection box
 TGComboBox* markcolor=new TGComboBox(markers);
 markcolor->Connect("Selected(Int_t)","NcAstrolab",this,"MapMarkColor(Int_t)");
 markcolor->AddEntry("Black",1);
 markcolor->AddEntry("Red",2);
 markcolor->AddEntry("Blue",3);
 markcolor->AddEntry("Green",4);
 markcolor->AddEntry("Yellow",5);
 markcolor->AddEntry("Magenta",6);
 markcolor->AddEntry("Cyan",7);
 markcolor->AddEntry("Orange",8);
 markcolor->AddEntry("Violet",9);
 markcolor->AddEntry("Pink",10);
 markcolor->AddEntry("Azure",11);
 markcolor->AddEntry("Spring",12);
 markcolor->AddEntry("Teal",13);
 markcolor->AddEntry("Gray",14);
 markcolor->AddEntry("White",15);
 markcolor->Resize(80,20);
 markers->AddFrame(markcolor);
 markcolor->Select(3,kTRUE);
 MapMarkColor(3);

 // The marker type selection box
 TGComboBox* marktype=new TGComboBox(markers);
 marktype->Connect("Selected(Int_t)","NcAstrolab",this,"MapMarkType(Int_t)");
 marktype->AddEntry("Ref",1);
 marktype->AddEntry("Meas",2);
 marktype->AddEntry("GC",3);
 marktype->AddEntry("Grid",4);
 marktype->Resize(55,20);
 markers->AddFrame(marktype);
 marktype->Select(2,kTRUE);
 MapMarkType(2);

 // The Solar system group
 TGGroupFrame* solar=new TGGroupFrame(others,"Selection of Solar system reference objects",kHorizontalFrame);
 solar->SetTitlePos(TGGroupFrame::kCenter);
 others->AddFrame(solar);

 TGButtonGroup* Ssolar=new TGButtonGroup(solar,0,3,5);
 Ssolar->SetTitlePos(TGGroupFrame::kCenter);
 Ssolar->Connect("Clicked(Int_t)","NcAstrolab",this,"MapSolar(Int_t)");
 TGCheckButton* bsun=new TGCheckButton(Ssolar,"Sun");
 bsun->SetToolTipText("Enter/Remove the Sun as a reference entry");
 TGCheckButton* bmoon=new TGCheckButton(Ssolar,"Moon");
 bmoon->SetToolTipText("Enter/Remove the Moon as a reference entry");
 TGCheckButton* bmercury=new TGCheckButton(Ssolar,"Mercury");
 bmercury->SetToolTipText("Enter/Remove Mercury as a reference entry");
 TGCheckButton* bvenus=new TGCheckButton(Ssolar,"Venus");
 bvenus->SetToolTipText("Enter/Remove Venus as a reference entry");
 TGCheckButton* bearth=new TGCheckButton(Ssolar,"Earth");
 bearth->SetToolTipText("Enter/Remove the Earth as a reference entry");
 TGCheckButton* bmars=new TGCheckButton(Ssolar,"Mars");
 bmars->SetToolTipText("Enter/Remove Mars as a reference entry");
 TGCheckButton* bjupiter=new TGCheckButton(Ssolar,"Jupiter");
 bjupiter->SetToolTipText("Enter/Remove Jupiter as a reference entry");
 TGCheckButton* bsaturn=new TGCheckButton(Ssolar,"Saturn");
 bsaturn->SetToolTipText("Enter/Remove Saturn as a reference entry");
 TGCheckButton* buranus=new TGCheckButton(Ssolar,"Uranus");
 buranus->SetToolTipText("Enter/Remove Uranus as a reference entry");
 TGCheckButton* bneptune=new TGCheckButton(Ssolar,"Neptune");
 bneptune->SetToolTipText("Enter/Remove Neptune as a reference entry");
 solar->AddFrame(Ssolar);
 Ssolar->Show();
 Ssolar->SetButton(1,kFALSE);
 Ssolar->SetButton(2,kFALSE);
 Ssolar->SetButton(3,kFALSE);
 Ssolar->SetButton(4,kFALSE);
 Ssolar->SetButton(5,kFALSE);
 Ssolar->SetButton(6,kFALSE);
 Ssolar->SetButton(7,kFALSE);
 Ssolar->SetButton(8,kFALSE);
 Ssolar->SetButton(9,kFALSE);
 Ssolar->SetButton(10,kFALSE);
 for (Int_t i=0; i<10; i++)
 {
  fMapSolar[i]=kFALSE;
 }

 // The Solar system Enter and Remove command buttons
 TGVerticalFrame* comms=new TGVerticalFrame(solar,1,1);
 TGLayoutHints* Lcomms=new TGLayoutHints(kLHintsLeft,10,0,15,0);
 solar->AddFrame(comms,Lcomms); // Command buttons

 // The button to Enter the solar system entries
 TGTextButton* enter=new TGTextButton(comms,"Enter");
 enter->SetToolTipText("Enter the selected Solar system objects as reference signals");
 enter->Connect("Clicked()","NcAstrolab",this,"MapEnterSolar()");
 TGLayoutHints* Lenter=new TGLayoutHints(kLHintsCenterX,0,0,10,15);
 comms->AddFrame(enter,Lenter);

 // The button to Remove the solar system entries
 TGTextButton* remove=new TGTextButton(comms,"Remove");
 remove->SetToolTipText("Remove the selected Solar system objects from the reference signals");
 remove->Connect("Clicked()","NcAstrolab",this,"MapRemoveSolar()");
 TGLayoutHints* Lremove=new TGLayoutHints(kLHintsCenterX,0,0,0,0);
 comms->AddFrame(remove,Lremove);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapDcoord(Int_t i)
{
// Action on a button from the coordinate system selection box
 TString system[9]={"equ","equ","equ","equ","gal","ecl","hor","icr","loc"};
 TString mode[4]={"J","M","T","B"};
 
 if (i<=9) fMapDcoord=system[i-1];

 if (i<=4) fMapDmode=mode[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapProj(Int_t i)
{
// Action on a button from the projection selection box
 TString s[13]={"ham","ait","mer","ang","cyl","UTh","LTh","GSTh","LSTh","UYh","LYh","GSYh","LSYh"};
 TString sh[13]={"hamh","aith","merh","angh","cylh","UTh","LTh","GSTh","LSTh","UYh","LYh","GSYh","LSYh"};
 if (!fMapDoptions[0] && i<13) fMapProj=s[i-1];
 if (fMapDoptions[0] && i<13) fMapProj=sh[i-1];
 for (Int_t k=0; k<13; k++)
 {
  if (fMapDoptions[0] && fMapProj==s[k]) fMapProj=sh[k];
  if (!fMapDoptions[0] && fMapProj==sh[k]) fMapProj=s[k];
 }
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapMerMode(Int_t i)
{
// Action on a button from the meridian ordering selection box
 Int_t modes[3]={0,1,-1};
 if (i<=3) fMapMerMode=modes[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapMerC(const char* text)
{
// Action on the central meridian specification field
 TString s=text;
 fMapMerC=s.Atof();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapMerUc(Int_t i)
{
// Action on a button from the central meridian angular format selection box
 TString s[4]={"deg","dms","hms","rad"};
 if (i<=4) fMapMerUc=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapDoptions(Int_t i)
{
// Action on an button from the draw/list options group

 if (i>5) return;

 if (!fMapDoptions[i-1])
 {
  fMapDoptions[i-1]=1;
 }
 else
 {
  fMapDoptions[i-1]=0;
 }
 if (i==1) MapProj(14); // Set histogram selection
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapNmax(const char* text)
{
// Action on the draw/list Nmax selection field
 TString s=text;
 fMapNmax=s.Atoi();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapNdigs(const char* text)
{
// Action on the draw/list Ndigs selection field
 TString s=text;
 fMapNdigs=s.Atoi();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapDname(const char* text)
{
// Action on the draw/list entry name selection field
 fMapDname=text;
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapMarkSize(const char* text)
{
// Action on the marker size selection field
 TString s=text;
 fMapMarkSize=s.Atof();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapMarkStyle(Int_t i)
{
// Action on the marker style selection box
 Int_t styles[17]={8,29,21,22,23,33,34,31,2,5,24,30,25,26,32,27,28};

 if (i>17) return;

 fMapMarkStyle=styles[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapMarkColor(Int_t i)
{
// Action on the marker color selection box
 Int_t colors[15]={kBlack,kRed,kBlue,kGreen,kYellow,kMagenta,kCyan,kOrange,kViolet,kPink,kAzure,kSpring,kTeal,kGray,kWhite};

 if (i>15) return;

 fMapMarkColor=colors[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapMarkType(Int_t i)
{
// Action on the markers entry type selection box
 if (i<=4) fMapMarkType=i-1;
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapSolar(Int_t i)
{
// Action on a button from the Solar system selection group
 if (i>10) return;

 if (!fMapSolar[i-1])
 {
  fMapSolar[i-1]=1;
 }
 else
 {
  fMapSolar[i-1]=0;
 }
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapEnterSolar()
{
// Enter the seleced Solar system objects as reference object
 TString names[10]={"Sun","Moon","Mercury","Venus","Earth","Mars","Jupiter","Saturn","Uranus","Neptune"};

 SetMapTS(); // Get the selected timestamp

 fSkyMapPanel->RequestFocus();

 for (Int_t i=0; i<10; i++)
 {
  if (fMapSolar[i]) GetSignal(names[i],0,&fMapTS);
 }
 printf("\n *** Selected Solar system object(s) entered *** \n");
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapRemoveSolar()
{
// Remove the seleced Solar system object(s) from the reference objects
 TString names[10]={"Sun","Moon","Mercury","Venus","Earth","Mars","Jupiter","Saturn","Uranus","Neptune"};

 SetMapTS(); // Get the selected timestamp

 fSkyMapPanel->RequestFocus();

 Int_t nrem=0;
 Int_t nremx=0;
 for (Int_t i=0; i<10; i++)
 {
  if (fMapSolar[i])
  {
   nremx=RemoveSignal(names[i],0,1);
   if (nremx) nrem++;
  }
 }

 printf("\n *** Number of Solar system object that have been removed : %-i *** \n",nrem);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::CommandPanel(TGCompositeFrame* frame)
{
// Internal memberfunction to render the GUI panel with the various command buttons.

 if (!frame) return;

 TGGroupFrame* panel=new TGGroupFrame(frame,"Commands",kVerticalFrame);
 panel->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(panel);

 TGTextButton* list=new TGTextButton(panel,"List");
 list->Connect("Clicked()","NcAstrolab",this,"MapList()");
 list->SetToolTipText("List the selected entries");
 TGLayoutHints* Llist=new TGLayoutHints(kLHintsCenterX,0,0,10,10);
 panel->AddFrame(list,Llist);

 TGTextButton* map=new TGTextButton(panel,"Map");
 map->Connect("Clicked()","NcAstrolab",this,"MapDraw()");
 map->SetToolTipText("Display the selected entries");
 TGLayoutHints* Lmap=new TGLayoutHints(kLHintsCenterX,0,0,10,10);
 panel->AddFrame(map,Lmap);

 TGTextButton* close=new TGTextButton(panel,"Close");
 close->Connect("Clicked()","NcAstrolab",this,"MapClose()");
 close->SetToolTipText("Close this panel window");
 TGLayoutHints* Lclose=new TGLayoutHints(kLHintsCenterX,0,0,10,10);
 panel->AddFrame(close,Lclose);

 TGTextButton* exit=new TGTextButton(panel,"Exit");
 exit->Connect("Clicked()","NcAstrolab",this,"MapExit()");
 exit->SetToolTipText("Exit this ROOT session");
 TGLayoutHints* Lexit=new TGLayoutHints(kLHintsCenterX,0,0,10,10);
 panel->AddFrame(exit,Lexit);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapList()
{
// List the entries of the skymap

 SetMapTS(); // Set the skymap timestamp

 fSkyMapPanel->RequestFocus();

 Int_t type=0;
 if (fMapDoptions[3]) type=1;
 if (fMapDoptions[2] && fMapDoptions[3]) type=-1;

 NcTimestamp* ts=&fMapTS; // User selected timestamp
 if (fMapLabTS) ts=0; // To get Lab timestamp notification in listings

 Int_t j=0;
 if (fMapDoptions[4]) j=-1; // Individual reference timestamps

 printf("\n");
 ListSignals(fMapDcoord,fMapDmode,fMapNdigs,"T",fMapNmax,j,type,ts,fMapDname);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapDraw()
{
// Draw the skymap

 SetMapTS(); // Set the skymap timestamp

 fSkyMapPanel->RequestFocus();

 Int_t type=0;
 if (fMapDoptions[3]) type=1;
 if (fMapDoptions[2] && fMapDoptions[3]) type=-1;

 NcTimestamp* ts=&fMapTS; // User selected timestamp
 if (fMapLabTS) ts=0; // To get Lab timestamp notification in listings

 Int_t j=0;
 if (fMapDoptions[4]) j=-1; // Individual reference timestamps

 SetCentralMeridian(fMapMerMode,fMapMerC,fMapMerUc);
 SetMarkerSize(fMapMarkSize,fMapMarkType);
 SetMarkerStyle(fMapMarkStyle,fMapMarkType);
 SetMarkerColor(fMapMarkColor,fMapMarkType);

 DisplaySignals(fMapDcoord,fMapDmode,ts,fMapProj,fMapDoptions[1],fMapNmax,j,type,fMapDname);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapClose()
{
// Close the SkyMapPanel window but do NOT exit the current ROOT session.

 // De-activate all automatic CloseWindow() actions of the system window manager
 // in order to fully control it in this function 
 fSkyMapPanel->DontCallClose();

 // To prevent crash when the cursor is still left active in a TextEntry
 fSkyMapPanel->RequestFocus();

 // Unmap the display window
 fSkyMapPanel->UnmapWindow();
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::MapExit()
{
// Exit this ROOT session

 fSkyMapPanel->RequestFocus();
 fSkyMapPanel->Cleanup();
 gApplication->Terminate(0);
}
///////////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetMapTS()
{
// Internal memberfunction to compose the timestamp within the SkyMapPanel GUI.

 fMapLabTS=kFALSE;
 Int_t imjd=0;
 Int_t isec=0;
 Int_t ins=0;
 Int_t ips=0;

 if (fMapTimeType=="SysClock") // Use the system clock time
 {
  fMapTS.SetSystemTime();
 }
 else if (fMapTimeType=="Lab") // Use the Lab timestamp
 {
  fMapTS.SetMJD(fMJD,fJsec,fJns,fJps);
  fMapLabTS=kTRUE;
 }
 else if (fMapTimeType=="MJD") // Modified Julian Date entry
 {
  Double_t mjd=fMapDateTime.Atof();
  fMapTS.SetMJD(mjd);
 }
 else if (fMapTimeType=="JD") // Julian Date entry
 {
  Double_t jd=fMapDateTime.Atof();
  fMapTS.SetJD(jd);
 }
 else if (fMapTimeType=="TJD") // Truncated Julian Date entry
 {
  Double_t tjd=fMapDateTime.Atof();
  fMapTS.SetTJD(tjd);
 }
 else if (fMapTimeType=="Unix") // Unix time
 {
  Double_t val=fMapDateTime.Atof();
  fMapTS.SetUnixTime(val);
 }
 else
 {
  if (fMapTimeType.Contains("Name")) // Get timestamp of the specified named entry
  {
   NcSignal* sx=0;
   NcTimestamp* tx=0;
   TString name=fMapDateTime;
   sx=GetSignal(name,1);
   if (!sx) sx=GetSignal(name,0);
   if (sx) tx=sx->GetTimestamp();
   if (tx) // Stored entry was found
   {
    tx->GetMJD(imjd,isec,ins);
    ips=tx->GetPs();
    fMapTS.SetMJD(imjd,isec,ins,ips);
   }
   else
   {
    printf("\n *** No stored entry with name %-s found --> Lab TS will be used *** \n",name.Data());
    GetMJD(imjd,isec,ins);
    ips=GetPs();
    fMapTS.SetMJD(imjd,isec,ins,ips);
    fMapLabTS=kTRUE;
   }
    fMapTS.GetDayTimeString("UTC",12,0,&fMapDate,&fMapTime,kFALSE);
  }
  else // Date/Time entry
  {
   fMapDate=fMapDateTime;
   fMapDate.Remove(fMapDateTime.Index("/"),fMapDateTime.Length());
   fMapTime=fMapDateTime;
   fMapTime.Remove(0,fMapDateTime.Index("/")+1);
  }

  // Set the timestamp according to the date/time strings
  fMapTS.SetUT(fMapDate,fMapTime,0);
  if (fMapTimeType=="UT1") fMapTS.SetUT(fMapDate,fMapTime,0,"U");
  if (fMapTimeType=="LMT") fMapTS.SetLT(fToffset,fMapDate,fMapTime,0);
  if (fMapTimeType=="GPS" || fMapTimeType=="TAI" || fMapTimeType=="TT")
  {
   fMapTS.SetTAI(fMapTimeType,fMapDate,fMapTime,0,"A",0);
  }
 }

 // Show the new timestamp in the textbox
 fMapTS.GetDayTimeString("UTC",3,0,&fMapDate,&fMapTime,kFALSE);
 fMapDateTime=fMapDate;
 fMapDateTime+="/";
 fMapDateTime+=fMapTime;
 fMapTSdatetime->SetText(fMapDateTime);

 // Adapt the timestamp type for the updated text window contents
 if (fMapTStimetype) fMapTStimetype->Select(1,kTRUE);
 MapTimeType(1);
}
///////////////////////////////////////////////////////////////////////////
TObject* NcAstrolab::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers when adding objects in case the container owns the objects.

 NcAstrolab* lab=new NcAstrolab(*this);
 if (name)
 {
  if (strlen(name)) lab->SetName(name);
 }
 return lab;
}
///////////////////////////////////////////////////////////////////////////
