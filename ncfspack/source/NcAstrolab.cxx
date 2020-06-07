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
// The lab can be given a terrestrial location via the usual longitude
// and latitude specifications.
// Since this class is derived from NcTimestamp, a lab can also be
// given a specific timestamp. Together with the terrestrial location
// this provides access to local (sidereal) times etc...
// In addition to the usual astronomical reference frames, a local
// lab reference frame can also be specified. Together with the lab's
// timestamp this uniquely defines all the coordinate transformations
// between the various reference frames.
// These lab characteristics allow space and time correlations of lab
// observations with external (astrophysical) phenomena.
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
// By default different random sequences are ensured for different NcAstrolab instances,
// even with identical stored measurements and reference signals.  
// In case the user wants to control the random sequence (e.g. to obtain reproducible results),
// the randomiser has to be explicitly initialised via the member function SetRandomiser().
// 
// Also graphical facilities (e.g. DisplaySignals) are available to
// provide skymaps in various projections.
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
// ts.Add(0,5,0);
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
//- Modified: Nick van Eijndhoven, IIHE-VUB Brussel, UTC June 7, 2020 11:46
///////////////////////////////////////////////////////////////////////////

#include "NcAstrolab.h"
#include "Riostream.h"
 
ClassImp(NcAstrolab) // Class implementation to enable ROOT I/O
 
NcAstrolab::NcAstrolab(const char* name,const char* title) : TTask(name,title),NcTimestamp()
{
// Default constructor

 fToffset=0;
 fRefs=0;
 fSigs=0;
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
 fMarkerSize[0]=1; 
 fMarkerSize[1]=1;
 fMarkerSize[2]=1.5;
 fMarkerSize[3]=0.3;
 fMarkerStyle[0]=23;
 fMarkerStyle[1]=20;
 fMarkerStyle[2]=29;
 fMarkerStyle[3]=8;
 fMarkerColor[0]=kRed;
 fMarkerColor[1]=kBlue;
 fMarkerColor[2]=kRed;
 fMarkerColor[3]=kBlack;
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

 // Storage for transient burst investigations
 fBurstParameters=0;

 // Initialize the default values for the burst parameters
 SetBurstParameter("*",0);

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
  delete fCanvas;
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
 
 const char* name=GetName();
 const char* title=GetTitle();
 cout << " *" << ClassName() << "::Data*";
 if (strlen(name))  cout << " Name : " << GetName();
 if (strlen(title)) cout << " Title : " << GetTitle();
 cout << endl;

 Double_t l,b;
 GetLabPosition(l,b,"deg");
 cout << " Lab position longitude : "; PrintAngle(l,"deg",u,2);
 cout << " latitude : "; PrintAngle(b,"deg",u,2);
 cout << endl;
 cout << " Lab time offset w.r.t. UT : "; PrintTime(fToffset,12);
 cout << endl;

 // UT and Local time info
 Date(mode,fToffset);

 // Add the UTC and TAI related date/time information if requested
 if (utc && mode!=4) Date(4);

 if (fTscmode)
 {
  cout << " ------------------" << endl;
  if (fTscmode==1) cout << " *** The obtained time differences will be scrambled (mode " << fTscmode << ") ***" << endl;
  if (fTscmode==2) cout << " *** The stored measurements are time scrambled (mode " << fTscmode << ") ***" << endl;
  cout << " tmin : " << fTscmin << " tmax : " << fTscmax << " sec.";
  if (fTscfunc)
  {
   cout << " Randomising TF1 function " << fTscfunc->GetName() << " is used." << endl;
  }
  else
  {
   cout << " Uniform randomisation is used." << endl;
  }
 }

 if (fRscmode==1)
 {
  cout << " ------------------" << endl;
  cout << " *** The obtained angular differences will be scrambled (mode " << fRscmode << ") ***" << endl;
  cout << " dangmin : " << fDscmin << " dangmax : " << fDscmax << " degrees.";
  if (fDscfunc)
  {
   cout << " Randomising TF1 function " << fDscfunc->GetName() << " is used." << endl;
  }
  else
  {
   cout << " Homogeneous solid angle randomisation is used." << endl;
  }
 }

 if (fRscmode==2)
 {
  cout << " ------------------" << endl;
  cout << " *** The stored observations are position scrambled (mode " << fRscmode << ") ***" << endl;
  cout << " drmin : " << fDscmin << " drmax : " << fDscmax;
  if (fDscfunc)
  {
   cout << " Randomising TF1 function " << fDscfunc->GetName() << " is used." << endl;
  }
  else
  {
   cout << " Uniform randomisation is used." << endl;
  }
  cout << " thetamin : " << fThetascmin << " thetamax : " << fThetascmax << " degrees.";
  if (fThetascfunc)
  {
   cout << " Randomising TF1 function " << fThetascfunc->GetName() << " is used." << endl;
  }
  else
  {
   cout << " Uniform cos(theta) randomisation is used." << endl;
  }
  cout << " phimin : " << fPhiscmin << " phimax : " << fPhiscmax << " degrees.";
  if (fPhiscfunc)
  {
   cout << " Randomising TF1 function " << fPhiscfunc->GetName() << " is used." << endl;
  }
  else
  {
   cout << " Uniform phi randomisation is used." << endl;
  }
 }
 
 cout << " ------------------" << endl;
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
 cout << " ------------------" << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetLabPosition(Nc3Vector& p)
{
// Set the lab position in the terrestrial coordinates and its corresponding
// time offset w.r.t. UT.
// The right handed position reference frame is defined such that the North Pole
// corresponds to a polar angle theta=0 and the Greenwich meridian corresponds
// to an azimuth angle phi=0, with phi increasing eastwards.

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
// Positions east of Greenwich have l>0, whereas l<0 indicates
// locations west of Greenwich.
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
void NcAstrolab::SetExperiment(TString name)
{
// Set position, local frame definition and time offset w.r.t. UT
// for the experiment as specified via the argument "name".
//
// Currently the supported experiment names are :
//
// Amanda  : Antarctic Muon And Neutrino Detector Array
// IceCube : The IceCube neutrino observatory at the South Pole
// WSRT    : The Westerbork Synthesis Radio Telescope in the Netherlands
// Astron  : The Netherlands Institute for Radio Astronomy in Dwingeloo
// ARA     : The Askaryan Radio Array at the South Pole
// RNO-G   : The Greenland Radio Neutrino Observatory at Summit Station
//
// Note : The name and title for the NcAstrolab object are updated automatically
//        according to the specified experiment name at invokation of this
//        memberfunction.
//        In case a different name and/or title is required, please use the 
//        usual SetNameTitle facility after invokation of this memberfunction.

 Double_t l=0; // Longitude
 Double_t b=0; // Lattitude

 if (name=="Amanda")
 {
  SetNameTitle("Amanda","Antarctic Muon And Neutrino Detector Array");
  SetLabPosition(0,-90,"deg"); // South Pole
  // Right handed Amanda local grid frame has Y-North (to Greenwich), X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
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
  return;
 }

 if (name=="WSRT")
 {
  SetNameTitle("WSRT","The Westerbork Synthesis Radio Telescope");
  SetLabPosition(63612.74,525454.33,"dms");
  // Right handed WSRT local grid frame has Y-North, X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
  return;
 }

 if (name=="Astron")
 {
  SetNameTitle("Astron","The Netherlands Institute for Radio Astronomy");
  SetLabPosition(62346.23,524843.99,"dms");
  // Right handed Astron local grid frame has Y-North, X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
  return;
 }

 if (name=="ARA")
 {
  SetNameTitle("ARA","The Askaryan Radio Array at the South Pole");
  SetLabPosition(0,-90,"deg"); // South Pole
  // Right handed ARA local grid frame has Y-North (to Greenwich), X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
  return;
 }

 if (name=="RNO-G")
 {
  SetNameTitle("RNO-G","The Greenland Radio Neutrino Observatory at Summit Station");
  l=-38.46;
  b=72.58;
  SetLabPosition(l,b,"deg"); // Summit Station
  // Right handed RNO-G local grid frame has Y-North, X-East and Z-Zenith
  SetLocalFrame(90,90,90,180,0,0);
  return;
 }

 cout << " *" << ClassName() << "::SetExperiment* Unsupported experiment name : " << name.Data() << endl;
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
void NcAstrolab::PrintAngle(Double_t a,TString in,TString out,Int_t ndig) const
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
// No rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.4 on the output.
// Due to computer accuracy, precision on the pico-arcsecond level may get lost.
//
// The default is ndig=1.
//
// Note : The angle info is printed without additional spaces or "endline".
//        This allows the print to be included in various composite output formats.

 Double_t b=ConvertAngle(a,in,out);

 if (out=="deg" || out=="rad")
 {
  cout.setf(ios::fixed,ios::floatfield);
  Int_t nold=cout.precision(ndig);
  cout << b << " " << out.Data();
  cout.precision(nold);
  cout.unsetf(ios::floatfield);
  return; 
 }

 Double_t epsilon=1.e-12; // Accuracy in (arc)seconds
 Int_t word=0,ddd=0,hh=0,mm=0,ss=0;
 Double_t s;
 ULong64_t sfrac=0;

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
  s*=pow(10.,ndig);
  sfrac=ULong64_t(s);
  if (b<0) cout << "-";
  cout << ddd << "d " << mm << "' " << ss << "."
       << setfill('0') << setw(ndig) << sfrac << "\"";
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
  s*=pow(10.,ndig);
  sfrac=ULong64_t(s);
  if (b<0) cout << "-";
  cout << hh << "h " << mm << "m " << ss << "."
       << setfill('0') << setw(ndig) << sfrac << "s";
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
//
// The default values are jref=0, name="" and type=0 to obtain backward compatible functionality.
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
  sx=SetSignal(&q,"hor",mode,ts,jref,name,type);
  return sx;
 }

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // with identical measurements and reference signals without explicit initialisation
 // of the randomiser by the user at the start of the analysis.
 if (!fRan && type && (fTscmode==2 || fRscmode==2)) fRan=new NcRandom(-1);

 // Local timestamp copy to allow time scrambling
 NcTimestamp tx(*ts);

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
    fTscfunc->SetRange(fTscmin,fTscmax);
    dt=fTscfunc->GetRandom(fTscmin,fTscmax);
   }
  }
  else
  {
   if (fTscmax>fTscmin) dt=fRan->Uniform(fTscmin,fTscmax);
  }
  dt/=3600.; // Convert to (fractional) hours
  tx.Add(dt);
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
 if (type) // Storage of a measurement
 {
  NcSignal* sxsig=0;
  if (!fSigs) 
  {
   fSigs=new TObjArray();
   fSigs->SetOwner();
  }
  // Expand array size if needed
  if (jref>0 && jref>=fSigs->GetSize()) fSigs->Expand(jref+1);
  sxsig=GetSignal(jref,type);
  if (!sxsig)
  {
   sxsig=new NcSignal();
  }
  else
  {
   sxsig->Reset(1);
  }
  if (name != "") sxsig->SetName(name);
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
  if (jref>0 && jref>=fRefs->GetSize()) fRefs->Expand(jref+1);
  sxref=GetSignal(jref,type);
  if (!sxref)
  {
   sxref=new NcSignal();
  }
  else
  {
   sxref->Reset(1);
  }
  if (name != "") sxref->SetName(name);
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
   fDscfunc->SetRange(fDscmin,fDscmax);
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
   fThetascfunc->SetRange(fThetascmin,fThetascmax);
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
   fPhiscfunc->SetRange(fPhiscmin,fPhiscmax);
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
void NcAstrolab::SetSolarSystem(TString name,NcTimestamp* ts,Int_t type)
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
// All geocentric name specifications as indicated in the docs of NcTimestamp::Almanac()
// are supported here.
//
// Note : In case ts=0 the current timestamp of this NcAstrolab instance will be taken.
//
// The default value is type=0 for backward compatibility.

 // Only geocentric positions are allowed
 if (name.Contains("*")) return;

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
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (b).
//          "gal" ==> Galactic coordinates with longitude (a) and latitude (b).
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (a) and latitude (b).
//          "hor" ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle (a) and zenith angle (b).
//          "icr" ==> ICRS coordinates with longitude (a) and latitude (b).
//          "loc" ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta (a) and phi (b).
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
 if (frame=="loc")
 {
  vec[1]=a;
  vec[2]=b;
 }

 r.SetVector(vec,"sph","deg");
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
//  frame = "equ" ==> Equatorial coordinates with right ascension (a) and declination (b).
//          "gal" ==> Galactic coordinates with longitude (a) and latitude (b).
//          "ecl" ==> Geocentric ecliptic coordinates with longitude (a) and latitude (b).
//          "hor" ==> Horizontal coordinates at the NcAstrolab location, with azimuth angle (a) and zenith angle (b).
//          "icr" ==> ICRS coordinates with longitude (a) and latitude (b).
//          "loc" ==> Local coordinates at the NcAstrolab location, with usual spherical angles theta (a) and phi (b).
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
//        1 --> The requested number corresponds to the stored  measured signal
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
// Default values are jref=0 and type=0 for backward compatibility.
//
// Note : In case ts=0 the current timestamp of the lab will be taken.

 r.SetZero();

 if (frame!="equ" && frame!="gal" && frame!="ecl" && frame!="hor" && frame!="icr" && frame!="loc") return 0;

 if (frame=="equ" && mode!="M" && mode!="m" && mode!="T" && mode!="t" && mode!="B" && mode!="b" && mode!="J" && mode!="j") return 0;

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

 if (!type && j==-1) // Set and store info for the requested Solar system object if not already stored
 {
  SetSolarSystem(name,ts,type);
  j=GetSignalIndex(name,type);
 }

 if (j>=0) sx=GetSignal(j,type);
 return sx;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::RemoveRefSignal(Int_t j,Int_t compress)
{
// ***********************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility ***
// *** The user is advised to use the memberfunction RemoveSignal() instead        ***
// ***********************************************************************************
//
// Remove the reference signal which was stored at the j-th position (j=1 is first).
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

 if (!fRefs) return;

 // Clearing of the complete storage
 if (!j)
 {
  delete fRefs;
  fRefs=0;
  return;
 }

 // Removing a specific reference signal
 if (j>0 && j<=fRefs->GetSize())
 {
  TObject* obj=fRefs->RemoveAt(j-1);
  if (obj) delete obj;
 }

 // Compression of the storage array
 if (compress) fRefs->Compress();
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::RemoveRefSignal(TString name,Int_t compress)
{
// ***********************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility ***
// *** The user is advised to use the memberfunction RemoveSignal() instead        ***
// ***********************************************************************************
//
// Remove the reference signal with the specified name.
//
// The "compress" parameter allows compression of the ref. signal storage array.
//
// compress = 1 --> Array will be compressed
//            0 --> Array will not be compressed
//
// Note : Compression of the storage array means that the indices of the
//        reference signals in the storage array will change.

 Int_t j=GetSignalIndex(name);
 if (j>0) RemoveRefSignal(j,compress);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::RemoveSignal(Int_t j,Int_t type,Int_t compress)
{
// Remove the signal of type "type" which was stored at the j-th position (j=1 is first).
// Note : j=0 means that all stored signals of type "type" will be removed.
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
 
 TObjArray* arr=fRefs;
 if (type) arr=fSigs;

 if (!arr) return;

 // Clearing of the complete storage
 if (!j)
 {
  delete arr;
  arr=0;
  return;
 }

 // Removing a specific reference signal
 if (j>0 && j<=arr->GetSize())
 {
  TObject* obj=arr->RemoveAt(j-1);
  if (obj) delete obj;
 }

 // Compression of the storage array
 if (compress) arr->Compress();
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::RemoveSignal(TString name,Int_t type,Int_t compress)
{
// Remove the signal with the specified name and type.
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
// In case no matching signal with the specified name and type is found,
// no action will be taken.

 Int_t j=GetSignalIndex(name,type);
 if (j>0) RemoveSignal(j,type,compress);
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAstrolab::GetSignalIndex(TString name,Int_t type)
{
// Provide storage index of the signal with the specified name.
//
// type = 0 --> Provide the index of a reference signal
//      = 1 --> Provide the index of a measured signal
//
// In case no signal with the specified name was found, the value -1 is returned.
//
// The default value is type=0 for backward compatibility.

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
void NcAstrolab::PrintSignal(TString frame,TString mode,NcTimestamp* ts,Int_t ndig,Int_t jref,TString emode,Int_t type)
{
// Print data of a stored signal in user specified coordinates at the specific timestamp ts.
// In case ts=0 the actual timestamp of the stored signal will be taken.
// In case no stored signal was available or one of the input arguments was
// invalid, no printout is produced.
//
// The argument "ndig" specifies the number of digits for the fractional
// part (e.g. ndig=6 for "dms" corresponds to micro-arcsecond precision).
// No rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.4 on the output.
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
// Default values are jref=0, emode="T" and type=0.

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

 TString name=sx->GetName();
 if (name != "") cout << name.Data() << " ";

 if (frame=="equ")
 {
  Double_t a,d;
  d=90.-r.GetX(2,"sph","deg");
  a=r.GetX(3,"sph","rad");
  if (mode=="B" || mode=="b") mode="B1950";
  if (mode=="J" || mode=="j") mode="J2000";
  cout << "Equatorial (" << mode.Data() <<") a : "; PrintAngle(a,"rad","hms",ndig);
  cout << " ("; PrintAngle(a,"rad","deg",ndig); cout << ")";
  cout << " d : "; PrintAngle(d,"deg","dms",ndig);
  cout << " ("; PrintAngle(d,"deg","deg",ndig); cout << ")";
  cout << " " << slha.Data() << " : "; PrintAngle(lha,"deg","hms",ndig);
  cout << " ("; PrintAngle(lha,"deg","deg",ndig); cout << ")";
  return;
 }

 if (frame=="gal")
 {
  Double_t l,b;
  b=90.-r.GetX(2,"sph","deg");
  l=r.GetX(3,"sph","deg");
  cout << "Galactic l : "; PrintAngle(l,"deg","deg",ndig);
  cout << " ("; PrintAngle(l,"deg","dms",ndig); cout << ")";
  cout << " b : "; PrintAngle(b,"deg","deg",ndig); 
  cout << " ("; PrintAngle(b,"deg","dms",ndig); cout << ")";
  cout << " " << slha.Data() << " : "; PrintAngle(lha,"deg","hms",ndig);
  cout << " ("; PrintAngle(lha,"deg","deg",ndig); cout << ")";
  return;
 }

 if (frame=="icr")
 {
  Double_t l,b;
  b=90.-r.GetX(2,"sph","deg");
  l=r.GetX(3,"sph","deg");
  cout << "ICRS l : "; PrintAngle(l,"deg","deg",ndig);
  cout << " ("; PrintAngle(l,"deg","dms",ndig); cout << ")";
  cout << " b : "; PrintAngle(b,"deg","deg",ndig);
  cout << " ("; PrintAngle(b,"deg","dms",ndig); cout << ")";
  cout << " " << slha.Data() << " : "; PrintAngle(lha,"deg","hms",ndig);
  cout << " ("; PrintAngle(lha,"deg","deg",ndig); cout << ")";
  return;
 }

 if (frame=="ecl")
 {
  Double_t l,b;
  b=90.-r.GetX(2,"sph","deg");
  l=r.GetX(3,"sph","deg");
  cout << "Geocentric ecliptic l : "; PrintAngle(l,"deg","deg",ndig);
  cout << " ("; PrintAngle(l,"deg","dms",ndig); cout << ")";
  cout << " b : "; PrintAngle(b,"deg","deg",ndig);
  cout << " ("; PrintAngle(b,"deg","dms",ndig); cout << ")";
  cout << " " << slha.Data() << " : "; PrintAngle(lha,"deg","hms",ndig);
  cout << " ("; PrintAngle(lha,"deg","deg",ndig); cout << ")";
  return;
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
  cout << "Horizontal azi : "; PrintAngle(azi,"deg","deg",ndig);
  cout << " ("; PrintAngle(azi,"deg","dms",ndig); cout << ")";
  cout << " alt : "; PrintAngle(alt,"deg","deg",ndig);
  cout << " ("; PrintAngle(alt,"deg","dms",ndig); cout << ")";
  cout << " " << slha.Data() << " : "; PrintAngle(lha,"deg","hms",ndig);
  cout << " ("; PrintAngle(lha,"deg","deg",ndig); cout << ")";
  return;
 }

 if (frame=="loc")
 {
  Double_t theta=r.GetX(2,"sph","deg");
  Double_t phi=r.GetX(3,"sph","deg");
  cout << "Local-frame phi : "; PrintAngle(phi,"deg","deg",ndig);
  cout << " ("; PrintAngle(phi,"deg","dms",ndig); cout << ")";
  cout << " theta : "; PrintAngle(theta,"deg","deg",ndig);
  cout << " ("; PrintAngle(theta,"deg","dms",ndig); cout << ")";
  cout << " " << slha.Data() << " : "; PrintAngle(lha,"deg","hms",ndig);
  cout << " ("; PrintAngle(lha,"deg","deg",ndig); cout << ")";
  return;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::PrintSignal(TString frame,TString mode,NcTimestamp* ts,Int_t ndig,TString name,TString emode,Int_t type)
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
// No rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.4 on the output.
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
// The default values are emode="T" and type=0.

 // Set c.q. update coordinates for Solar system objects
 SetSolarSystem(name,ts,type);

 Int_t j=GetSignalIndex(name,type);
 if (j>=0) PrintSignal(frame,mode,ts,ndig,j,emode,type);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::ListSignals(TString frame,TString mode,Int_t ndig,TString emode,Int_t nmax,Int_t j,Int_t type)
{
// List stored measurements and/or reference signals in user specified coordinates
// at a specific timestamp.
// Measurements are listed according to the timestamp of their actual stored recording, which may be
// a scrambled timestamp according to the invokation of the memberfunction SetTimeScramble().
// For reference signals the actual timestamp of the j-th (j=1 is first) stored measurement is used.
// In case j=0 or no (timestamp of the) j-th measurement is available,
// the current timestamp of the lab will be taken.
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
// No rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.4 on the output.
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
// The default values are ndig=1, emode="T", nmax=10, j=-1 and type=-1.

 Int_t iprint=0;

 NcSignal* sx=0;
 NcTimestamp* tx=0;

 Int_t dform=1;
 if (mode=="T" || mode=="t") dform=-1;
 if ((mode=="B" || mode=="b" || mode=="J" || mode=="j") && emode=="T") dform=-1;

 if (j>0) sx=GetSignal(j,1);
 if (sx)
 {
  tx=sx->GetTimestamp();
  if (!tx) tx=(NcTimestamp*)this;
  cout << " *" << ClassName() << "::ListSignals* List of stored signals." << endl;
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
  for (Int_t i=1; i<=arr->GetSize(); i++)
  {
   sx=GetSignal(i,type);
   if (!sx) continue;

   jlist++;
   if (nmax>=0 && jlist>nmax) break;

   if (!iprint)
   {
    cout << " *" << ClassName() << "::ListSignals* List of stored signals." << endl;
    tx=(NcTimestamp*)this;
    cout << " Current timestamp of the laboratory (Lab time offset w.r.t. UT : "; PrintTime(fToffset,12); cout << ")";
    cout << endl;
    tx->Date(dform,fToffset);
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
   if (!type && !j) tx=(NcTimestamp*)this;
   cout << " Index : " << i << " "; PrintSignal(frame,mode,tx,ndig,i,emode,type); cout << endl;
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
// w.r.t. a so called Master Reference Frame (MRF), with the same convention
// as the input arguments of the ROOT facility TRotMatrix::SetAngles.
//
// The right handed Master Reference Frame is defined as follows :
//  Z-axis : Points to the local Zenith
//  X-axis : Makes an angle of 90 degrees with the Z-axis and points South
//  Y-axis : Makes an angle of 90 degrees with the Z-axis and points East
//
// Once the user has specified the local reference frame, any observed event
// can be related to astronomical space-time locations via the SetSignal
// and GetSignal memberfunctions.

 // Set the matrix for the conversion of our reference frame coordinates
 // into the local-frame ones.

 fL.SetAngles(t1,p1,t2,p2,t3,p3);
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
// Set the NcTimestamp parameters corresponding to the local date and time (LT)
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
// Set the NcTimestamp parameters corresponding to the local date and time (LT)
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
// Set the NcTimestamp parameters corresponding to the local date and time (LT)
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
// Set the NcTimestamp parameters corresponding to the local date and time (LT)
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
// ************************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility. ***
// *** The user is advised to use the memberfunction MatchSignals() instead.        ***
// ************************************************************************************
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
Double_t NcAstrolab::GetDifference(Int_t i,Int_t j,TString au,Double_t& dt,TString tu,Int_t mode)
{
// Internal memberfunction to provide space and time difference between the i-th and j-th stored signal.
// This memberfunction serves the MatchSignals() facility.
//
// i > 0 : Indicates a reference signal stored at index i (i=1 is the first)
// i < 0 : Indicates a measurement stored at index abs(i) (i=-1 is the first)
// j > 0 : Indicates a reference signal stored at index j (j=1 is the first)
// j < 0 : Indicates a measurement stored at index abs(j) (j=-1 is the first)
// 
// The return value of this memberfunction provides the positional angular
// difference, whereas the output argument "dt" provides the time difference.
//
// In case of inconsistent input the return value will have the unphysical value of 999.
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
// For the time difference the signal indicated via "i" is used as the standard.
// This means that in case of a positive time difference, the j-th stored
// signal occurred later than i-th one.
//
// Note : In case scrambling has been activated, the results will be scrambled accordingly.

 Double_t dang=999;
 dt=1.e30;

 if (!i || !j) return dang;
 if ((i>0 || j>0) && !fRefs) return dang;
 if ((i<0 || j<0) && !fSigs) return dang;

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

 // Update the location of Solar system objects if requested
 if (fSolUpdate)
 {
  ti=tj;
  TString name=si->GetName();
  SetSolarSystem(name,ti,itype);
 }

 GetSignal(ri,"icr","M",ti,i,itype);
 GetSignal(rj,"icr","M",tj,j,jtype);

 // Setting the parameters in the correct units for time difference scrambling
 Double_t tlow=fTscmin;
 Double_t tup=fTscmax;
 if (tu=="d")
 {
  tlow/=double(24*3600);
  tup/=double(24*3600);
 }
 if (tu=="ns")
 {
  tlow*=1.e9;
  tup*=1.e9;
 }
 if (tu=="ps")
 {
  tlow*=1.e12;
  tup*=1.e12;
 }
 if (fTscfunc) fTscfunc->SetRange(tlow,tup);

 // Setting the parameters for angular difference scrambling
 if (fDscfunc) fDscfunc->SetRange(fDscmin,fDscmax);

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // with identical measurements and reference signals without explicit initialisation
 // of the randomiser by the user at the start of the analysis.
 if (!fRan && (fRscmode || fTscmode)) fRan=new NcRandom(-1);

 Double_t pi=acos(-1.);

 //////////////////////////////////////////////////////////////////
 // Space and time difference w.r.t. the specified signals       //
 //////////////////////////////////////////////////////////////////

 if (fRscmode != 1)
 {
  dang=ri.GetOpeningAngle(rj,au);
 }
 else
 {
  if (!fDscfunc)
  {
   Float_t cosmin=cos(fDscmin*pi/180.);
   Float_t cosmax=cos(fDscmax*pi/180.);
   if (cosmin>cosmax)
   {
    Float_t temp=cosmin;
    cosmin=cosmax;
    cosmax=temp;
   }
   Double_t cosang=fRan->Uniform(cosmin,cosmax);
   dang=acos(cosang);
   if (au=="deg") dang*=180./pi;
  }
  else
  {
   dang=fDscfunc->GetRandom(fDscmin,fDscmax);
   if (au=="rad") dang*=pi/180.;
  }
 }

 if (fTscmode != 1)
 {
  dt=ti->GetDifference(tj,tu,mode);
 }
 else
 {
  if (!fTscfunc)
  {
   dt=fRan->Uniform(float(tlow),float(tup));
  }
  else
  {
   dt=fTscfunc->GetRandom(tlow,tup);
  }
 }

 return dang;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcAstrolab::GetDifference(TString name,TString au,Double_t& dt,TString tu,Int_t mode)
{
// ************************************************************************************
// *** This memberfunction is obsolete and is only kept for backward compatibility. ***
// *** The user is advised to use the memberfunction MatchSignals() instead.        ***
// ************************************************************************************
//
// Provide space and time difference between the stored reference signal
// with the specified name and the stored measurement.
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

 matches.Reset(1);
 matches.SetHitCopy(1);

 TString name="Matches";
 TString title="Space and time matchings of NcAstrolab stored signals";
 matches.SetNameTitle(name.Data(),title.Data());
 TString namedamin="psimin in "; 
 namedamin+=au;
 TString namedtmin="dtmin in ";
 namedtmin+=tu;
 matches.AddNamedSlot(namedamin);
 matches.AddNamedSlot(namedtmin);
 matches.AddNamedSlot("ipsi");
 matches.AddNamedSlot("idt");

 NcSignal data;
 TString nameda="psi in "; 
 nameda+=au;
 TString namedt="t2-t1 in ";
 namedt+=tu;
 data.AddNamedSlot("type1");
 data.AddNamedSlot("index1");
 data.AddNamedSlot("type2");
 data.AddNamedSlot("index2");
 data.AddNamedSlot(nameda);
 data.AddNamedSlot(namedt);

 if ((!itype || !jtype) && !fRefs) return;
 if ((itype || jtype) && !fSigs) return;

 Int_t nrefs=0;
 if (fRefs) nrefs=fRefs->GetSize();
 Int_t nsigs=0;
 if (fSigs) nsigs=fSigs->GetSize();


 // Make input data consistent with conventions
 if (itype) itype=1;
 if (jtype) jtype=1;
 if (i1<1) i1=1;
 if (!itype)
 {
  if (i2<1 || i2>nrefs) i2=nrefs;
 }
 else
 {
  if (i2<1 || i2>nsigs) i2=nsigs;
 }
 if (j1<1) j1=1;
 if (!jtype)
 {
  if (j2<1 || j2>nrefs) j2=nrefs;
 }
 else
 {
  if (j2<1 || j2>nsigs) j2=nsigs;
 }

 if (i1>i2 || j1>j2) return;

 Double_t dang,dtime;
 Int_t ix=0;
 Int_t jx=0;
 NcSignal* sx=0;
 Int_t id=0;
 Double_t dangmin=99999;
 Double_t dtmin=1e20;
 Int_t idamin=0;
 Int_t idtmin=0;
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

   dang=GetDifference(ix,jx,au,dtime,tu,mode);

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
    if (fabs(dang)<dangmin)
    {
     dangmin=fabs(dang);
     idamin=id;
    }

    // Record the data for the minimal encountered time difference
    if (fabs(dtime)<fabs(dtmin))
    {
     dtmin=dtime;
     idtmin=id;
    }
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
// Signals are regarded as matching if the positional angular difference
// doesn't exceed "da" and the absolute value of the time difference doesn't exceed "dt".
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

 MatchSignals(matches,da,au,dt,tu,mode,i,i,itype,j1,j2,jtype);

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
//        1 ==> Scrambling is only performed when obtaining time differences.
//              All time differences are randomly selected from the interval [tmin,tmax].
//              As such, this scrambling mode is very useful for scrambling entries in
//              specific time windows (see e.g. NvE, Astroparticle Physics 28 (2008) 540).
//              This time difference scrambling will only affect the outcome of the memberfunctions
//              GetDifference(), MatchRefSignal() and MatchSignals() of this class NcAstrolab.
//              So, the actual (stored) event timestamps and the NcTimestamp::GetDifference()
//              are not affected by the scrambling.
//              This implies that setting mode=0 will reproduce again the unscrambled values.
//              Note : Every time a time difference is requested, a new random value is produced.
//                     As such, scrambled values are not reproducible on subsequent invokations.
//        2 ==> Time scrambling is performed before storing the measurement.
//              A value is randomly selected from the interval [tmin,tmax] and added to the 
//              actual timestamp of the measurement.
//              As such, this scrambling is irreversible but will always provide reproducible results.
//              Note : In case the measurement is entered in NcAstrolab specific coordinates
//                     (i.e. local or horizontal position coordinates) then this mode=2 scrambling
//                     will also result in a corresponding scrambling of e.g. the right ascension.
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
 if (frndm) fTscfunc=new TF1(*frndm);
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
//              This scrambling mode is very useful for scrambling entries in specific
//              angular cones (see e.g. NvE, Astroparticle Physics 28 (2008) 540).
//              This angular difference scrambling will only affect the outcome of the memberfunctions
//              GetDifference(), MatchRefSignal() and MatchSignals() of this class NcAstrolab.
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
//              In case the r-coordinate would become zero or negative by the scrambling, it's value will be
//              set to a small positive value before storage, in order to keep a physical situation. 
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
 if (mode==1 && dmin<0) dmin=0;
 if (mode==1 && dmax>180) dmax=180;

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
 if (df) fDscfunc=new TF1(*df);
 fThetascmin=thmin;
 fThetascmax=thmax;
 if (fThetascfunc)
 {
  delete fThetascfunc;
  fThetascfunc=0;
 }
 if (thf) fThetascfunc=new TF1(*thf);
 fPhiscmin=phimin;
 fPhiscmax=phimax;
 if (fPhiscfunc)
 {
  delete fPhiscfunc;
  fPhiscfunc=0;
 }
 if (phif) fPhiscfunc=new TF1(*phif);
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
void NcAstrolab::DisplaySignal(TString frame,TString mode,NcTimestamp* ts,Int_t j,TString proj,Int_t clr)
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
// Default value is j=0 for backward compatibility.
//
// Measurements are indicated as blue dots.
// Reference signals are indicated as red triangles.
// The Galactic Center is indicated as a red star.
// The size of the marker symbols may be tailored via the member function SetMarkerSize().
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
//
// Note : The ang(h) plot allows for easy identification of an isotropic distribution.
//
// The input argument "clr" allows to clear (1) the display before drawing or not (0).
//
// The default values are : j=0, proj="ham" and clr=0.
//
// This routine is based on initial work by Garmt de Vries-Uiterweerd.

 // Comply with the new (jref,type) convention for measurements and reference signals.
 Int_t jref=abs(j);
 Int_t type=0;
 if (j<0) type=1;
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

 // Update the display for this signal position

 // Create a new canvas if needed
 if (!fCanvas) fCanvas=new TCanvas("NcAstrolab","Skymap");

 // Construct the various strings for this map
 TString titleup;  // The upper title string
 TString titlelow; // The lower title string
 TString sleft;    // The most left coordinate indicator
 TString sright;   // The most right coordinate indicator
 TString sup;      // The most upper coordinate indicator
 TString slow;     // The most lower coordinate indicator
 sup="90#circ";
 slow="-90#circ";
 if (frame=="equ")
 {
  titleup="Geocentric Equatorial (";
  titleup+=mode;
  if (mode=="J") titleup+="2000";
  if (mode=="B") titleup+="1950";
  titleup+=") ";
 }
 if (frame=="gal") titleup="Heliocentric Galactic";
 if (frame=="ecl") titleup=" Geocentric Ecliptic";
 if (frame=="hor") titleup="  Standard Horizon";
 if (frame=="icr") titleup="Static Barycentric ICRS";
 if (frame=="loc")
 {
  titleup=" User defined Local";
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
   header->DrawLatex(-1.2,yup+0.2,titleup.Data());
   TLatex* footer=new TLatex;
   fMarkers->Add(footer);
   footer->DrawLatex(-1.7,ylow-0.25,titlelow.Data());

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
  else
  {
   fHist[type]->Fill(x*xfac,y*yfac);
  }
  if ((!type && fHist[1]) || (type && fHist[0]))
  {
   fHist[type]->Draw("same");
  }
  else
  {
   fHist[type]->Draw();
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
// Reference signals are indicated as red triangles.
// The Galactic Center is indicated as a red star.
// The size of the marker symbols may be tailored via the member function SetMarkerSize().
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
//
// Note : The ang(h) plot allows for easy identification of an isotropic distribution.
//
// The input argument "clr" allows to clear (1) the display before drawing or not (0).
//
// The default values are : proj="ham", clr=0 and type=0.

 Int_t j=GetSignalIndex(name,type);
 if (j>0)
 {
  if (type) j=-j;
  DisplaySignal(frame,mode,ts,j,proj,clr);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::DisplaySignals(TString frame,TString mode,NcTimestamp* ts,TString proj,Int_t clr,Int_t nmax,Int_t j,Int_t type)
{
// Display of stored signals in a user specified coordinate projection at the specific timestamp.
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
// Reference signals are indicated as red triangles.
// The Galactic Center is indicated as a red star.
// The size of the marker symbols may be tailored via the member function SetMarkerSize().
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
//
// Note : The ang(h) plot allows for easy identification of an isotropic distribution.
//
// The input argument "clr" allows to clear (1) the display before drawing or not (0).
//
// The default values are : proj="ham", clr=0, nmax=-1, j=-1 and type=-1.

 NcSignal* sx=0;
 NcTimestamp* tx=0;
 Int_t nstored=0;
 Int_t jdisp=0;

 // Display stored reference signals
 if (fRefs && type<=0)
 {
  // Use timestamp of j-th measurement if requested
  if (j>0) sx=GetSignal(j,1);
  if (sx) tx=sx->GetTimestamp();

  // Use the provided timestamp
  tx=ts;

  // Use the current lab timestamp if no timestamp selected
  if (!tx) tx=(NcTimestamp*)this;
 
  nstored=fRefs->GetEntries();
  jdisp=0;
  for (Int_t i=1; i<=fRefs->GetSize(); i++)
  {
   sx=GetSignal(i,0);
   if (!sx) continue;

   jdisp++;
   if (nmax>=0 && jdisp>nmax) break;

   // Use the actual timestamp of the reference signal
   if (j<0)
   {
    tx=sx->GetTimestamp();
    if (!tx) tx=ts;
    if (!tx) tx=(NcTimestamp*)this;
   }

   DisplaySignal(frame,mode,tx,i,proj,clr);
   clr=0; // No display clear for subsequent signals
  }
 }

 // Display all stored measurements
 if (fSigs && type)
 {
  nstored=fSigs->GetEntries();
  jdisp=0;
  for (Int_t j=1; j<=fSigs->GetSize(); j++)
  {
   sx=GetSignal(j,1);
   if (!sx) continue;

   jdisp++;
   if (nmax>=0 && jdisp>nmax) break;

   tx=sx->GetTimestamp();
   if (!tx) tx=ts;
   if (!tx) tx=(NcTimestamp*)this;
   DisplaySignal(frame,mode,tx,-j,proj,clr);
   clr=0; // No display clear for subsequent signals
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::SetMarkerSize(Float_t size,Int_t type)
{
// Set the size of the marker symbols for skymaps and related histograms.
// By default all sizes are set to 1 in the constructor of this NcAstrolab class. 
//
// type = 0 --> Set marker size for reference signals
//        1 --> Set marker size for measurements
//        2 --> Set marker size for the Galactic Center
//        3 --> Set marker size for the skymap grid dots

 if (type<0 || type >3) return;

 fMarkerSize[type]=size;
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
 if (name=="Newton") fNewton=value;
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
Double_t NcAstrolab::GetNuclearMass(Int_t z,Int_t n,Int_t mode) const
{
// Provide the nuclear mass or binding energy of a specified nucleus
// with "z" protons and "n" neutrons.
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

 if (z<0 || n<0) return 0;

 Double_t rz=z;
 Double_t rn=n;
 Double_t ra=z+n;

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
 
 Int_t oz=z%2; // Flag (1) for odd Z nuclei 
 Int_t on=n%2; // Flag (1) for odd N nuclei

 if (oz && on) term5=delta/sqrt(ra);
 if (!oz && !on) term5=-delta/sqrt(ra);

 // Binding energy in MeV
 Double_t bnz=term1-term2-term3-term4-term5;

 // In case a single nucleon was specified
 if ((z+n)<2)
 {
  bnz=0;
  ra=1;
 }

 // Nuclear mass in MeV/c^2
 Double_t mass=rz*fMp+rn*fMn-bnz;

 // Explicit literature values for very light elements
 if (z==1 && n==1) // Deuteron
 {
  mass=2.013553212712*fAmu;
  bnz=rz*fMp+rn*fMn-mass;
 }

 if (z==1 && n==2) // Triton
 {
  mass=3.0155007134*fAmu;
  bnz=rz*fMp+rn*fMn-mass;
 }

 if (z==2 && n==1) // Helion
 {
  mass=3.0149322468*fAmu;
  bnz=rz*fMp+rn*fMn-mass;
 }

 if (z==2 && n==2) // Alpha
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
//        :  3 --> Nucleon target DIS total cross section
//        : -1 --> Electron target CC cross section
//          -2 --> Electron target NC cross section
//        : -3 --> Electron target total cross section
//        : -4 --> Special CC process anu_e+e-->anu_mu+mu (not included in other mode values)
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

  s="Dx interval distribution between ";
  s+=nc+1;
  s+=" consecutive entries (nc=";
  s+=nc;
  s+=", mode=";
  s+=mode;
  s+=")";
  hdx.SetNameTitle("DxHistogram",s.Data());

  Double_t binwidth=hdx.GetXaxis()->GetBinWidth(1);
  s.Form("Counts per bin of size %-10.3g",binwidth);
  hdx.GetXaxis()->SetTitle("Dx interval");
  hdx.GetYaxis()->SetTitle(s.Data());
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
  if (!idxbin) dxbin=hx->GetBinWidth(1);
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
  syout="d(";
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
 Double_t x=0;
 Double_t y=0;
 Double_t err=0;
 Double_t yplus=0;
 for (Int_t i=1; i<=nbins; i++)
 {
  x=hin->GetBinCenter(i);
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
 Int_t Non=int(pars[0]);
 Double_t Ton=pars[1];
 Int_t Noff=int(pars[2]);
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
TF1 NcAstrolab::GetBackgroundRatePDF(Int_t Noff,Double_t Toff,Double_t bmax,Double_t prec)
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

 Double_t rNoff=Noff;
 if (bmax<0) bmax=100.*rNoff/Toff;

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
TF1 NcAstrolab::GetSignalRatePDF(Int_t Non,Double_t Ton,Int_t Noff,Double_t Toff,Double_t Ra,Double_t Re,Double_t smax,Double_t bmax,Double_t prec)
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

 Double_t rNon=Non;
 if (smax<0) smax=100.*rNon/Ton;
 
 Double_t rNoff=Noff;
 // Correct the off source observation for different coverage and detection efficiency
 // with respect to the actual on source measurement
 rNoff=rNoff*Ra*Re;
 Noff=rNoff;
 if ((rNoff-double(Noff))>0.5) Noff+=1;
 rNoff=Noff;

 if (bmax<0) bmax=100.*rNoff/Toff;

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
// Provide the Cumulative Distribution Hstogram from the input 1-D histogram "h".
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
// Nmax;     // Maximal number of bursts to be accepted for analysis (<0 : no limitation)
// Declmin   // Minimal declination (J2000 in degrees) for burst position acceptance
// Declmax   // Maximal declination (J2000 in degrees) for burst position acceptance
// T90min    // Minimal duration (t90 in sec) for burst acceptance (<0 : [|T90min|,T90max] with random T90 when T90 or T100 is unknown)
// T90max    // Maximal duration (t90 in sec) for burst acceptance
// Zmin      // Minimal redshift for burst acceptance (<0 : [|fZmin|,fZmax] with random z when redshift is unknown)
// Zmax      // Maximal redshift for burst acceptance
// Sigmagrb  // Angular uncertainty (sigma in degrees) on burst position (<0 : determine from observations)
// Maxsigma  // Maximal combined burst position and track angular uncertainty (sigma in degrees) for acceptance
// Grbnu     // Maximum number of detectable neutrinos per burst (<0 : no stat. fluct.)
// Avgrbz    // Average burst redshift (<0 : determine from observations)
// Avgrbt90  // Average burst duration (T90) in seconds (<0 : determine from observations)
// Inburst   // Flag to indicate that neutrinos are produced coupled (1) or not (0) to the burst duration
// Dtnu      // Mean time diff. (in sec) between gamma/GW and nu production (decoupled) or in T90 units w.r.t. trigger (coupled)
// Dtnus     // Sigma of time difference (in sec) between gamma/GW and nu production (<0 is in T90 units)
// Kinangle  // Neutrino-lepton kinematic opening angle selection for CC interactions (0=none 1=mean 2=median 3=draw from pdf)
// Angres    // Detector angular resolution (degrees)
// Timres    // Detector time resolution (sec)
// Bkgrate   // Mean rate (in Hz) of background events for the specified [Declmin,Declmax] interval (<0 : rate per steradian)
// Dtwin     // Total search time window (in sec) centered at the burst trigger
// Dawin     // Angular search circle (<0 is local zenith band) in degrees or sigma around (above/below) the burst position
// Datype    // Type of angular window specification (0=in degrees 1=in units of combined burst/track sigma) 
// Nbkg      // Mean number of counts per bin for auto-binning
// Tbint90   // Time bin size in units of average T90 (0 : Time bin size determined by Tbin) 
// Tbin      // Time bin size in seconds (0=variable bins  <0 will result in a mean Nbkg counts/bin)
// VarTbin   // Size (in sec) of the first time bin in case of variable time bins
// Abin      // Angular bin size in degrees (<0 will result in a mean Nbkg counts/bin)

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
  name="Nmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-1,name);
  name="Declmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-90,name);
  name="Declmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(90,name);
  name="T90min";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e-6,name);
  name="T90max";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e6,name);
  name="Zmin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-1e-6,name);
  name="Zmax";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(9999,name);
  name="Sigmagrb";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-2.5,name);
  name="Maxsigma";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(999,name);
  name="Grbnu";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-0.05,name);
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
  fBurstParameters->SetSignal(-60,name);
  name="Dtnus";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(-0.5,name);
  name="Kinangle";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(3,name);
  name="Angres";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0.5,name);
  name="Timres";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(1e-5,name);
  name="Bkgrate";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0.003,name);
  name="Dtwin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(7200,name);
  name="Dawin";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(5,name);
  name="Datype";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0,name);
  name="Nbkg";
  fBurstParameters->AddNamedSlot(name);
  fBurstParameters->SetSignal(0.5,name);
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

 // The solid angle corresponding to the selected declination band
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t thlow=90.-fDeclmax;
 Float_t thup=90.-fDeclmin;
 Float_t OmegaDecl=GetSolidAngle(thlow,thup,"deg",0,360,"deg");
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
 Float_t NbkgWin=RbkgDecl*fDtwin;
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
 Int_t fNmax=fBurstParameters->GetSignal("Nmax");
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t fT90min=fBurstParameters->GetSignal("T90min");
 Float_t fT90max=fBurstParameters->GetSignal("T90max");
 Float_t fZmin=fBurstParameters->GetSignal("Zmin");
 Float_t fZmax=fBurstParameters->GetSignal("Zmax");
 Float_t fSigmagrb=fBurstParameters->GetSignal("Sigmagrb");
 Float_t fMaxsigma=fBurstParameters->GetSignal("Maxsigma");
 Float_t fGrbnu=fBurstParameters->GetSignal("Grbnu");
 Float_t fAvgrbz=fBurstParameters->GetSignal("Avgrbz");
 Float_t fAvgrbt90=fBurstParameters->GetSignal("Avgrbt90");
 Int_t fInburst=fBurstParameters->GetSignal("Inburst");
 Float_t fDtnu=fBurstParameters->GetSignal("Dtnu");
 Float_t fDtnus=fBurstParameters->GetSignal("Dtnus");
 Int_t fKinangle=fBurstParameters->GetSignal("Kinangle");
 Float_t fAngres=fBurstParameters->GetSignal("Angres");
 Float_t fTimres=fBurstParameters->GetSignal("Timres");
 Float_t fBkgrate=fBurstParameters->GetSignal("Bkgrate");
 Float_t fDtwin=fBurstParameters->GetSignal("Dtwin");
 Float_t fDawin=fBurstParameters->GetSignal("Dawin");
 Int_t fDatype=fBurstParameters->GetSignal("Datype");
 Float_t fNbkg=fBurstParameters->GetSignal("Nbkg");
 Float_t fTbint90=fBurstParameters->GetSignal("Tbint90");
 Float_t fTbin=fBurstParameters->GetSignal("Tbin");
 Float_t fVarTbin=fBurstParameters->GetSignal("VarTbin");
 Float_t fAbin=fBurstParameters->GetSignal("Abin");

 // Derived parameters
 Float_t fOmegaDecl=fBurstParameters->GetSignal("OmegaDecl");
 Float_t fRbkgDecl=fBurstParameters->GetSignal("RbkgDecl");
 Float_t fNbkgHour=fBurstParameters->GetSignal("NbkgHour");
 Float_t fNbkgWin=fBurstParameters->GetSignal("NbkgWin");

 // Internal statistics
 Int_t fNgrbs=fBurstParameters->GetSignal("Ngrbs");
 Float_t fMaxtotsigma=fBurstParameters->GetSignal("Maxtotsigma");

 cout << " ========================= User provided burst settings ===============================" << endl;
 if (fNmax<0)
 {
  cout << " No limitation has been put on the number of bursts to be accepted for analysis." << endl;
 }
 else
 {
  cout << " Maximal number of bursts to be accepted for analysis : " << fNmax << endl;
 }
 cout << " Declination interval (J2000 in degrees) for burst position acceptance : [" << fDeclmin << "," << fDeclmax << "]" << endl;
 cout << " Duration interval (t90 in sec) for burst acceptance : [" << fabs(fT90min) << "," << fT90max << "]" << endl;
 if (fT90min<0) cout << " Random values taken from T90-distribution in case T90 and T100 were missing" << endl;
 cout << " Redshift interval for burst acceptance : [" << fabs(fZmin) << "," << fZmax << "]" << endl;
 if (fZmin<0) cout << " Random redshift values taken from z-distribution in case of unknown redshift" << endl;
 if (fSigmagrb>=0) cout << " Fixed burst position uncertainty (sigma in degrees) : " << fSigmagrb << endl;
 cout << " Maximal combined burst position and track angular uncertainty (sigma in degrees) for acceptance : " << fMaxsigma << endl;
 if (fAvgrbz>=0) cout << " User defined average burst redshift : " << fAvgrbz << endl;
 if (fAvgrbt90>=0) cout << " User defined average burst T90 duration : " << fAvgrbt90 << endl;
 if (!fInburst)
 {
  cout << " Neutrino production was assumed to be NOT coupled to the observed burst duration" << endl;
  cout << " Mean decoupled time difference (in sec) between burst gammas/GW and nus : " << fDtnu << endl;
 }
 else
 {
  cout << " Neutrino production was assumed to be coupled to the observed burst duration" << endl;
  cout << " Mean coupled time difference (in units of T90 w.r.t. trigger) between burst gammas/GW and nus : " << fDtnu << endl;
 }
 if (fDtnus>=0)
 {
 cout << " Sigma of mean time difference (in sec) between burst gammas/GW and nus : " << fDtnus << endl;
 }
 else
 {
 cout << " Sigma of mean time difference (in units of T90) between burst gammas/GW and nus : " << fabs(fDtnus) << endl;
 }
 if (fGrbnu<0)
 {
  cout << " Number of generated neutrinos per burst : " << fabs(fGrbnu) << " without statistical fluctuations" << endl; 
 }
 else
 {
  cout << " Maximum number of generated neutrinos per burst : " << fGrbnu << endl;
  cout << " The actual number of neutrinos may be less due to statistical fluctuations" << endl;
 }
 cout << " Neutrino-lepton kinematic opening angle selection for CC interactions (0=none 1=mean 2=median 3=draw from pdf) : " << fKinangle << endl;
 cout << " Angular resolution (degrees) of the detector : " << fAngres << endl;
 cout << " Time resolution (sec) of the detector : " << fTimres << endl;
 cout << " Mean rate (Hz) of background events for the specified declination interval (<0 : rate per steradian) " << fBkgrate << endl;
 cout << " Total search time window (in sec) centered at the burst trigger : " << fDtwin << endl;
 if (fDawin>=0)
 {
  if (!fDatype)
  {
   cout << " Angular search circle (in degrees) around the burst position : " << fDawin << endl;
  }
  else
  {
   cout << " Angular search circle (in combined burst/track sigma) around the burst position : " << fDawin << endl;
  }
 }
 else
 {
  if (!fDatype)
  {
   cout << " Angular local zenith band (in degrees) above/below the burst position : " << fabs(fDawin) << endl;
  }
  else
  {
   cout << " Angular local zenith band (in combined burst/track sigma) above/below the burst position : " << fabs(fDawin) << endl;
  }
 }
 if (fTbin<0) cout << " Automatic time binning with as mean number of bkg counts/bin : " << fNbkg << endl;
 if (!fTbin) cout << " Variable time binning with as size (in sec) for the first time : " << fVarTbin << endl;
 if (fTbin>0)
 {
  if (fTbint90)
  {
   cout << " Time bin size in average T90 units : " << fTbint90;
   if (fAvgrbt90>0 || fNgrbs>0) cout << " (=" << fTbin << " sec)";
   cout << endl;
  }
  else
  {
   cout << " Time bin size in seconds : " << fTbin << endl;
  }
 }
 if (fAbin<0)
 {
  cout << " Automatic angular binning with as mean number of bkg counts per bin : " << fNbkg << endl;
 }
 else
 {
  cout << " Angular bin size in degrees : " << fAbin << endl;
 }

 cout << endl;
 cout << " ============================== Derived parameters ====================================" << endl;
 cout << " Solid angle coverage (in steradian) corresponding to the selected declination band : " << fOmegaDecl << endl;
 cout << " Background event rate (Hz) for the selected declination band : " << fRbkgDecl << endl;
 cout << " Mean number of background events per hour from the selected declination band : " << fNbkgHour << endl;
 cout << " Mean number of background events in the time window from the selected declination band : " << fNbkgWin << endl;
 if (fNgrbs>0)
 {
  cout << " Number of bursts accepted for analysis : " << fNgrbs << endl;
  cout << " Median redshift from the data sample : " << fabs(fAvgrbz) << endl;
  cout << " Median T90 duration from the data sample : " << fabs(fAvgrbt90) << endl;
 }
 cout << " ======================================================================================" << endl;
 cout << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::LoadBurstGCNdata(TString file,TString tree,Int_t date1,Int_t date2,Int_t nmax,TString type)
{
// Load observed burst GCN data, e.g. GRB data from GCN notices as available from
// https://icecube.wisc.edu/~grbweb_public.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// The input data has to be provided via a ROOT Tree which will be searched
// for data on the variable names specified below.
// In case data for a certain variable is not present, the non-physical value -999
// will be stored, unless the user has selected random generation of T90 and/or z
// as specified via SetBurstParameter(). 
//
//  Name             Description
// ---------------------------------------------------------------
// "date"        Observation date as yyyymmdd
// "ra"          Right ascension (J2000) in decimal degrees
// "dec"         Declination (J2000) in decimal degrees
// "sigmapos"    The error on the burst position in decimal degrees
// "t90"         The T90 burst duration in seconds
// "mjdtrig"     The (fractional) MJD of the burst trigger
// "mjdt90start" The (fractional) MJD of the T90 start time
// "t100"        The T100 burst duration in seconds
// "fluence"     The observed fluence of the burst in erg/cm^2
// "z"           The redshift of the burst
//
// Input arguments :
// -----------------
// file  : Name of the input file containing the ROOT Tree (wildcards are allowed)
// tree  : Name of the Tree containing the data
// date1 : The date (yyyymmdd) of the start of the observation period [date1,date2] (0=No restriction)
// date2 : The date (yyyymmdd) of the end of the observation period [date1,date2] (0=No restriction)
// nmax  : Maximum number of bursts to be accepted from this input file (<0 : no limitation)
// type  : Identifier of the transient (alert) type (e.g. "GRB", "GW", "IC",....)
//
// The default values are date1=0, date2=0, nmax=-1 and type="GRB".
//
// Note : This memberfunction make be invoked several times to read different files
//        to accumulate data.

 // Retreive the needed parameters
 Int_t fNmax=fBurstParameters->GetSignal("Nmax");
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t fSigmagrb=fBurstParameters->GetSignal("Sigmagrb");
 Float_t fMaxsigma=fBurstParameters->GetSignal("Maxsigma");
 Float_t fT90min=fBurstParameters->GetSignal("T90min");
 Float_t fT90max=fBurstParameters->GetSignal("T90max");
 Float_t fZmin=fBurstParameters->GetSignal("Zmin");
 Float_t fZmax=fBurstParameters->GetSignal("Zmax");
 Float_t fAngres=fBurstParameters->GetSignal("Angres");

 // Internal statistics
 Int_t fNgrbs=fBurstParameters->GetSignal("Ngrbs");
 Float_t fMaxtotsigma=fBurstParameters->GetSignal("Maxtotsigma");

 // Get access to a redshift distribution to draw randomly redshifts if needed
 TH1* zdist=0;
 if (fZmin<0)
 {
  zdist=(TH1*)fBurstHistos.FindObject("hz");
  if (!zdist)
  {
   cout << " *" << ClassName() << "::LoadBurstGCNdata* Archival observed redshift distribution not found." << endl;
   cout << " A Landau fit from Swift GRB redshift data will be used to provide missing z values." << endl;
   cout << endl;

   zdist=(TH1*)fBurstHistos.FindObject("hzfit");
   if (!zdist)
   {
    TF1 f("f","59.54*TMath::Landau(x,1.092,0.5203)");
    f.SetRange(0,10);
    f.SetNpx(10000);
    TH1* hf=f.GetHistogram();
    zdist=(TH1*)hf->Clone();
    zdist->SetNameTitle("hzfit","Landau fit for Swift GRB z data");
    zdist->GetXaxis()->SetTitle("GRB redshift");
    zdist->GetYaxis()->SetTitle("Counts");
    fBurstHistos.Add(zdist);
   }
  }
 }

 // Get access to a T90 distribution to draw randomly T90 values if needed
 TH1* t90dist=0;
 if (fT90min<0)
 {
  t90dist=(TH1*)fBurstHistos.FindObject("ht90");
  if (!t90dist)
  {
   cout << " *" << ClassName() << "::LoadBurstGCNData* Observational T90 distribution not found." << endl;
   cout << " A double Gaussian fit from Fermi GRB T90 data will be used to provide missing T90 values." << endl;
   cout << endl;
 
   t90dist=(TH1*)fBurstHistos.FindObject("ht90fit");
   if (!t90dist)
   {
    TF1 ft("ft","44.39*TMath::Gaus(x,-0.131,0.481)+193.8*TMath::Gaus(x,1.447,0.4752)");
    ft.SetRange(-5,5);
    ft.SetNpx(10000);
    TH1* hft=ft.GetHistogram();
    t90dist=(TH1*)hft->Clone();
    t90dist->SetNameTitle("ht90fit","Double Gauss fit for Fermi t90 data");
    t90dist->GetXaxis()->SetTitle("GRB duration ^{10}log(T90) in sec.");
    t90dist->GetYaxis()->SetTitle("Counts");
    fBurstHistos.Add(t90dist);
   } 
  }
 }

 // The TTree containing the burst data
 TChain gcn(tree.Data());
 gcn.Add(file.Data());

 Int_t date;
 Float_t ra,dec,sigmapos,t90,t100,fluence,z;
 Double_t mjdtrig,mjdt90start;

 Float_t sigmagrb=fSigmagrb;
 Float_t totsigma=0;
 fNgrbs=GetNsignals(0);
 Float_t t90grb=0;
 Double_t zgrb=0;
 Int_t idate=0;
 TString grbname;
 NcTimestamp ts;
 NcSignal* sx=0;
 Int_t ngcn=0;
 TLeaf* lx=0;
 for (Int_t ient=0; ient<gcn.GetEntries(); ient++)
 {
  if (nmax>=0 && ngcn>=nmax) break;
  if (fNmax>=0 && (fNgrbs+ngcn)>=fNmax) break;

  gcn.GetEntry(ient);

  date=-999;
  lx=gcn.GetLeaf("date");
  if (lx) date=lx->GetValue();  
  ra=-999;
  lx=gcn.GetLeaf("ra");
  if (lx) ra=lx->GetValue();  
  dec=-999;
  lx=gcn.GetLeaf("dec");
  if (lx) dec=lx->GetValue();  
  sigmapos=-999;
  lx=gcn.GetLeaf("sigmapos");
  if (lx) sigmapos=lx->GetValue();  
  t90=-999;
  lx=gcn.GetLeaf("t90");
  if (lx) t90=lx->GetValue();  
  mjdtrig=-999;
  lx=gcn.GetLeaf("mjdtrig");
  if (lx) mjdtrig=lx->GetValue();  
  mjdt90start=-999;
  lx=gcn.GetLeaf("mjdt90start");
  if (lx) mjdt90start=lx->GetValue();  
  t100=-999;
  lx=gcn.GetLeaf("t100");
  if (lx) t100=lx->GetValue();  
  fluence=-999;
  lx=gcn.GetLeaf("fluence");
  if (lx) fluence=lx->GetValue();  
  z=-999;
  lx=gcn.GetLeaf("z");
  if (lx) z=lx->GetValue();  

  if (date1 && date<date1) continue;
  if (date2 && date>date2) continue;

  if (mjdtrig<0 || mjdt90start<0) continue;

  // Use the GCN 1 sigma position uncertainty (in degrees) if requested
  if (fSigmagrb<0) sigmagrb=fabs(sigmapos);

  // Determine the combined GRB position and track resolution uncertainty.
  totsigma=sigmagrb*sigmagrb+fAngres*fAngres;
  totsigma=sqrt(totsigma);
   
  if (dec<fDeclmin || dec>fDeclmax || totsigma>fMaxsigma) continue;

  t90grb=t90;
  if (t90grb<=0) t90grb=t100;
  if (fT90min<0 && t90grb<0 && t90dist) t90grb=t90dist->GetRandom();

  if (t90grb<fabs(fT90min) || t90grb>fT90max) continue;

  zgrb=z;
  if (fZmin<0 && zgrb<0 && zdist) zgrb=zdist->GetRandom();

  if (zgrb<fabs(fZmin) || zgrb>fZmax) continue;

  idate=date%1000000;
  grbname=type;
  grbname+=idate;
  ts.SetMJD(mjdtrig);
  sx=SetSignal(1,ra,"deg",dec,"deg","equ",&ts,-1,"J",grbname);

  if (!sx) continue;

  ngcn++;

  sx->AddNamedSlot("t90");
  sx->SetSignal(t90grb,"t90");
  sx->AddNamedSlot("sigmagrb");
  sx->SetSignal(sigmagrb,"sigmagrb");
  sx->AddNamedSlot("totsigma");
  sx->SetSignal(totsigma,"totsigma");
  sx->AddNamedSlot("fluence");
  sx->SetSignal(fluence,"fluence");
  sx->AddNamedSlot("z");
  sx->SetSignal(zgrb,"z");

  if (totsigma>fMaxtotsigma) fMaxtotsigma=totsigma;
 }

 // Update internal statistics
 fBurstParameters->AddNamedSlot("Maxtotsigma");
 fBurstParameters->SetSignal(fMaxtotsigma,"Maxtotsigma");
 fNgrbs=GetNsignals(0);
 fBurstParameters->AddNamedSlot("Ngrbs");
 fBurstParameters->SetSignal(fNgrbs,"Ngrbs");

 cout << "*" << ClassName() << "::LoadBurstGCNdata* " << ngcn << " bursts of type " << type
      << " were stored from Tree:" << tree << " of file(s):" << file << endl;
 cout << " Total number of stored bursts : " << fNgrbs << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GenBurstGCNdata(Int_t n,TString name)
{
// Generate fictative burst GCN data for "n" bursts of (alert) type "name",
// where "name" can be "GRB", "GW", "IC", ....
//
// The default value is name="GRB".
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 // Retreive the needed parameters
 Int_t fNmax=fBurstParameters->GetSignal("Nmax");
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t fSigmagrb=fBurstParameters->GetSignal("Sigmagrb");
 Float_t fMaxsigma=fBurstParameters->GetSignal("Maxsigma");
 Float_t fT90min=fBurstParameters->GetSignal("T90min");
 Float_t fT90max=fBurstParameters->GetSignal("T90max");
 Float_t fZmin=fBurstParameters->GetSignal("Zmin");
 Float_t fZmax=fBurstParameters->GetSignal("Zmax");
 Float_t fAngres=fBurstParameters->GetSignal("Angres");

 // Internal statistics
 Int_t fNgrbs=fBurstParameters->GetSignal("Ngrbs");
 Float_t fMaxtotsigma=fBurstParameters->GetSignal("Maxtotsigma");

 // Get access to a redshift distribution to draw randomly redshifts
 TH1* zdist=(TH1*)fBurstHistos.FindObject("hz");
 if (!zdist)
 {
  cout << " *" << ClassName() << "::GenBurstGCNdata* Archival observed redshift distribution not found." << endl;
  cout << " A Landau fit from Swift GRB redshift data will be used to provide random z values." << endl;
  cout << endl;

  zdist=(TH1*)fBurstHistos.FindObject("hzfit");
  if (!zdist)
  { 
   TF1 fz("fz","59.54*TMath::Landau(x,1.092,0.5203)");
   fz.SetRange(0,10);
   fz.SetNpx(10000);
   TH1* hfz=fz.GetHistogram();
   zdist=(TH1*)hfz->Clone();
   zdist->SetNameTitle("hzfit","Landau fit for Swift GRB z data");
   zdist->GetXaxis()->SetTitle("GRB redshift");
   zdist->GetYaxis()->SetTitle("Counts");
   fBurstHistos.Add(zdist);
  }
 }

 // Get access to a T90 distribution to draw randomly T90 values
 TH1* t90dist=(TH1*)fBurstHistos.FindObject("ht90");
 if (!t90dist)
 {
  cout << " *" << ClassName() << "::GenBurstGCNData* Observational T90 distribution not found." << endl;
  cout << " A double Gaussian fit from Fermi GRB T90 data will be used to provide random T90 values." << endl;
  cout << endl;

  t90dist=(TH1*)fBurstHistos.FindObject("ht90fit");
  if (!t90dist)
  {
   TF1 ft("ft","44.39*TMath::Gaus(x,-0.131,0.481)+193.8*TMath::Gaus(x,1.447,0.4752)");
   ft.SetRange(-5,5);
   ft.SetNpx(10000);
   TH1* hft=ft.GetHistogram();
   t90dist=(TH1*)hft->Clone();
   t90dist->SetNameTitle("ht90fit","Double Gauss fit for Fermi t90 data");
   t90dist->GetXaxis()->SetTitle("GRB duration ^{10}log(T90) in sec.");
   t90dist->GetYaxis()->SetTitle("Counts");
   fBurstHistos.Add(t90dist);
  } 
 }

 Float_t thlow=fDeclmin+90.;
 Float_t thup=fDeclmax+90.;
 if (thup>180) thup=180;

 NcSignal* sx=0;
 NcPosition rgrb;
 Float_t t90grb=0;
 Double_t zgrb=0;
 TString grbname;
 Float_t sigmagrb=fabs(fSigmagrb);
 Float_t totsigma=0;
 Double_t thetagrb,phigrb;
 Int_t ngen=0;
 fNgrbs=GetNsignals(0);

 for (Int_t igrb=1; igrb<=n; igrb++)
 {
  if (fNmax>=0 && (fNgrbs+ngen)>=fNmax) break;

  zgrb=-1;
  while (zgrb<fabs(fZmin) || zgrb>fZmax)
  {
   zgrb=zdist->GetRandom();
  }
  rgrb.SetPosition(zgrb,0,0,"sph","deg");
  RandomPosition(rgrb,thlow,thup,0,360);
  thetagrb=rgrb.GetX(2,"sph","deg");
  phigrb=rgrb.GetX(3,"sph","deg");

  t90grb=-1;
  while (t90grb<fabs(fT90min) || t90grb>fT90max)
  {
   t90grb=t90dist->GetRandom();
   t90grb=pow(float(10),t90grb);
  }

  // Determine the combined GRB position and track resolution uncertainty.
  totsigma=sigmagrb*sigmagrb+fAngres*fAngres;
  totsigma=sqrt(totsigma);
   
  if (totsigma>fMaxsigma) continue;

  grbname="Random-";
  grbname+=name;
  grbname+=igrb;
  sx=SetSignal(zgrb,thetagrb,"deg",phigrb,"deg","loc",0,-1,"M",grbname);

  if (!sx) continue;

  ngen++;

  sx->AddNamedSlot("t90");
  sx->SetSignal(t90grb,"t90");
  sx->AddNamedSlot("sigmagrb");
  sx->SetSignal(sigmagrb,"sigmagrb");
  sx->AddNamedSlot("totsigma");
  sx->SetSignal(totsigma,"totsigma");
  sx->AddNamedSlot("z");
  sx->SetSignal(zgrb,"z");

  if (totsigma>fMaxtotsigma) fMaxtotsigma=totsigma;
 }

 // Update internal statistics
 fBurstParameters->AddNamedSlot("Maxtotsigma");
 fBurstParameters->SetSignal(fMaxtotsigma,"Maxtotsigma");
 fNgrbs=GetNsignals(0);
 fBurstParameters->AddNamedSlot("Ngrbs");
 fBurstParameters->SetSignal(fNgrbs,"Ngrbs");

 cout << "*" << ClassName() << "::GenBurstGCNdata* " << ngen << " generated bursts with name " << name << " were stored." << endl;
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
  cout << "*" << ClassName() << "::MakeBurstZdist* Missing information for tree variable:" << name << endl;
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

 cout << "*" << ClassName() << "::MakeBurstZdist* " << nz << " archival z-values have been obtained from tree variable:" << name
      << " of Tree:" << tree << " in file(s):" << file << endl;
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
  cout << "*" << ClassName() << "::MakeBurstT90dist* Missing information for tree variable:" << name << endl;
  cout << " of Tree:" << tree << " with " << nen <<  " entries in file:" << file << endl;
  return;
 }

 // Create a new distribution in case a T90 distribution is not yet present
 TH1* t90dist=(TH1*)fBurstHistos.FindObject("ht90");
 if (!t90dist)
 {
  // Creation of observed burst t90 duration histo
  TH1F* ht90=new TH1F("ht90","Archival data of observed burst durations",50,-5,5);
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

 cout << "*" << ClassName() << "::MakeBurstT90dist* " << nt90 << " archival T90 values have been obtained from variable:" << name
      << " of Tree:" << tree << " in file(s):" << file << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstBkgEdist(TString file,TString tree,TString name1,TString name2,TString u,Double_t Emin,Double_t Emax,Int_t nb)
{
// Create a background energy distribution on the interval [Emin,Emax] GeV
// based on observed archival energy data. 
// If this memberfunction is invoked before GenBurstSignals(), the resulting
// log10(E) distribution will be used to draw random Energy values
// for the background events.
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
// name2  : Name of the tree variable containing the declination data (in degrees)
// u      : The units ("deg"=degrees or "rad"=radians) of the declination data
// Emin   : Minimal energy value in GeV
// Emax   : Maximal energy value in GeV
// nb     : Number of bins for the Energy distribution
//
// The default value is nb=1000.
//
// Note : This memberfunction may be invoked several times to read different files
//        to accumulate data.

 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");

 if (Emin<=0) Emin=1e-10;

 if (Emax<=Emin)
 {
  cout << "*" << ClassName() << "::MakeBurstBkgEdist* Inconsistent data: Emin=" << Emin << " Emax=" << Emax << endl;
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
  cout << "*" << ClassName() << "::MakeBurstBkgEdist* Missing information for tree variable:" << name1
       << " and/or tree variable:" << name2 << endl;
  cout << " of Tree:" << tree << " with " << nen <<  " entries in file:" << file << endl;
  return;
 }

 // Create a new distribution in case an energy distribution is not yet present
 TH1* Edist=(TH1*)fBurstHistos.FindObject("hbkgE");
 if (!Edist)
 {
  // Creation of the observed background energy histo
  TH1F* hbkgE=new TH1F("hbkgE","Archival data of observed energies",nb,xmin,xmax);
  fBurstHistos.Add(hbkgE);
  hbkgE->GetXaxis()->SetTitle("^{10}log(Energy) in GeV");
  hbkgE->GetYaxis()->SetTitle("Counts");
 }

 // Get pointer to the relevant histogram 
 TH1* hbkgE=(TH1*)fBurstHistos.FindObject("hbkgE");

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
  if (u=="rad") dec*=180./acos(-1.); 

  if (dec>=fDeclmin && dec<=fDeclmax)
  {
   hbkgE->Fill(logE);
   nE++;
  }
 }

 cout << "*" << ClassName() << "::MakeBurstBkgEdist* " << nE << " archival Energy values have been obtained from variable:" << name1
      << " of Tree:" << tree << " in file(s):" << file << endl;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstEdist(TF1& spec,Double_t Emin,Double_t Emax,Int_t nbins)
{
// Create an energy distribution on the interval [Emin,Emax] GeV based on the 
// provided spectral function "spec" describing dN/dE.
// If this memberfunction is invoked before GenBurstSignals(), the resulting 
// energy distribution will be used to draw random energy values
// for the burst induced signal events.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Example: To make a dN/dE=E^-2 energy spectrum for 100 Gev < E < 10 PeV
//
// TF1 spec("spec","pow(x,-2.)");
// xmin=100;
// xmax=1e7;
// nbins=1000;
// MakeBurstEdist(spec,Emin,Emax,nbins);
//
// The default value is nbins=1000.

 if (Emin<=0) Emin=1e-10;

 if (Emax<=Emin)
 {
  cout << "*" << ClassName() << "::MakeBurstEdist* Inconsistent data: Emin=" << Emin << " Emax=" << Emax << endl;
  return;
 }

 // Convert the energy boundaries to the log10 scale of the X-axis
 Double_t xmin=log10(Emin);
 Double_t xmax=log10(Emax);

 TString s="Burst induced signal energy distribution;^{10}Log(Energy) in GeV;pdf";
 TH1F his=GetCountsHistogram(spec,nbins,xmin,xmax,1,s);
 TH1F* hpdfE=(TH1F*)his.Clone();
 hpdfE->SetName("hpdfE");
 fBurstHistos.Add(hpdfE);
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::MakeBurstEdist(Double_t gamma,Double_t Emin,Double_t Emax,Int_t nbins)
{
// Create an energy distribution on the interval [Emin,Emax] GeV based on a 
// a single power law with spectral index "gamma" describing dN/dE.
// If this memberfunction is invoked before GenBurstSignals(), the resulting 
// energy distribution will be used to draw random energy values
// for the burst induced signal events.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************
//
// Example: To make a dN/dE=E^-2 energy spectrum for 100 Gev < E < 10 PeV
//
// gamma=2;
// Emin=100;
// Emax=1e7;
// nbins=1000;
// MakeBurstEdist(gamma,Emin,Emax,nbins);
//
// The default value is nbins=1000.

 TF1 spec("spec","pow(x,[0])");
 spec.SetParameter(0,-gamma);
 MakeBurstEdist(spec,Emin,Emax,nbins);
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
// If Emin<0 the lower boundary of the provided spectrum will be used as Emin. 
// If Emax<0 the upper boundary of the provided spectrum will be used as Emax.
//
// In case of inconsistent data the value -1 is returned.
//
// The default values are Emin=-1 and Emax=-1.

 Double_t E=-1;

 // Get pointer to the relevant histogram 
 TH1* hpdfE=(TH1*)fBurstHistos.FindObject("hpdfE");

 if (!hpdfE) return E;

 Int_t nbins=hpdfE->GetNbinsX();

 if (nbins<=0) return E;

 TAxis* xaxis=hpdfE->GetXaxis();
 
 if (!xaxis) return E;

 Double_t xlow=xaxis->GetBinLowEdge(1);
 Double_t xup=xaxis->GetBinUpEdge(nbins);

 Double_t logEmin=0;
 if (Emin<0)
 {
  logEmin=xlow;
 }
 else
 {
  logEmin=log10(Emin);
 }

 Double_t logEmax=0;
 if (Emax<0)
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
  E=hpdfE->GetRandom();
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
// If Emin<0 the lower boundary of the provided spectrum will be used as Emin. 
// If Emax<0 the upper boundary of the provided spectrum will be used as Emax.
//
// In case of inconsistent data the value -1 is returned.
//
// The default values are Emin=-1 and Emax=-1.

 Double_t E=-1;

 // Get pointer to the relevant histogram 
 TH1* hbkgE=(TH1*)fBurstHistos.FindObject("hbkgE");

 if (!hbkgE) return E;

 Int_t nbins=hbkgE->GetNbinsX();

 if (nbins<=0) return E;

 TAxis* xaxis=hbkgE->GetXaxis();
 
 if (!xaxis) return E;

 Double_t xlow=xaxis->GetBinLowEdge(1);
 Double_t xup=xaxis->GetBinUpEdge(nbins);

 Double_t logEmin=0;
 if (Emin<0)
 {
  logEmin=xlow;
 }
 else
 {
  logEmin=log10(Emin);
 }

 Double_t logEmax=0;
 if (Emax<0)
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
  E=hbkgE->GetRandom();
 }

 E=pow(float(10),E);

 return E;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GenBurstSignals()
{
// Generate detector signals from transient bursts.
//
// **********************************************************************************
// * This is a beta test version, so currently no backward compatibility guaranteed *
// **********************************************************************************

 // If needed, initialise the randomiser with a "date/time driven" seed
 // using the timestamp of the moment of this invokation of the member function.
 // This will ensure different random sequences if the user repeats analyses
 // with identical measurements and reference signals without explicit initialisation
 // of the randomiser by the user at the start of the analysis.
 if (!fRan) fRan=new NcRandom(-1);

 // Retreive the needed parameters
 Float_t fDeclmin=fBurstParameters->GetSignal("Declmin");
 Float_t fDeclmax=fBurstParameters->GetSignal("Declmax");
 Float_t fSigmagrb=fBurstParameters->GetSignal("Sigmagrb");
 Float_t fMaxtotsigma=fBurstParameters->GetSignal("Maxtotsigma");
 Float_t fTimres=fBurstParameters->GetSignal("Timres");
 Int_t fKinangle=fBurstParameters->GetSignal("Kinangle");
 Float_t fAngres=fBurstParameters->GetSignal("Angres");
 Float_t fAvgrbz=fBurstParameters->GetSignal("Avgrbz");
 Float_t fAvgrbt90=fBurstParameters->GetSignal("Avgrbt90");
 Float_t fBkgrate=fBurstParameters->GetSignal("Bkgrate");
 Float_t fDtwin=fBurstParameters->GetSignal("Dtwin");
 Float_t fDawin=fBurstParameters->GetSignal("Dawin");
 Float_t fDatype=fBurstParameters->GetSignal("Datype");
 Float_t fTbin=fBurstParameters->GetSignal("Tbin");
 Float_t fTbint90=fBurstParameters->GetSignal("Tbint90");
 Float_t fNbkg=fBurstParameters->GetSignal("Nbkg");
 Float_t fVarTbin=fBurstParameters->GetSignal("VarTbin");
 Float_t fAbin=fBurstParameters->GetSignal("Abin");
 Float_t fGrbnu=fBurstParameters->GetSignal("Grbnu");
 Int_t fInburst=fBurstParameters->GetSignal("Inburst");
 Float_t fDtnu=fBurstParameters->GetSignal("Dtnu");
 Float_t fDtnus=fBurstParameters->GetSignal("Dtnus");

 // Derived parameters
 Float_t fOmegaDecl=fBurstParameters->GetSignal("OmegaDecl");
 Float_t fRbkgDecl=fBurstParameters->GetSignal("RbkgDecl");
 Float_t fNbkgHour=fBurstParameters->GetSignal("NbkgHour");
 Float_t fNbkgWin=fBurstParameters->GetSignal("NbkgWin");

 Float_t t90,z;
 TString name;

 ////////////////////////////////////////////////
 // Some Burst statistics from the loaded data //
 ////////////////////////////////////////////////

 Int_t fNgrbs=GetNsignals(0);

 // Creation of the burst position uncertainty histo
 TH1F* hsigmagrb=new TH1F("hsigmagrb","Burst position uncertainty",450,0,90);
 fBurstHistos.Add(hsigmagrb);
 hsigmagrb->GetXaxis()->SetTitle("Burst position uncertainty (sigma in degrees)");
 hsigmagrb->GetYaxis()->SetTitle("Counts");

 // Creation of the combined burst position and track resolution uncertainty histo
 TH1F* htotsigma=new TH1F("htotsigma","Combined burst position and event direction uncertainty",450,0,90);
 fBurstHistos.Add(htotsigma);
 htotsigma->GetXaxis()->SetTitle("Combined burst position and event direction uncertainty (sigma in degrees)");
 htotsigma->GetYaxis()->SetTitle("Counts");

 NcSignal* sx=0;
 NcSample zsample;
 zsample.SetStoreMode();
 NcSample t90sample;
 t90sample.SetStoreMode();
 Int_t nsig=GetNsignals(0,1);
 for (Int_t i=1; i<=nsig; i++)
 {
  sx=GetSignal(i,0);

  if (!sx) continue;

  hsigmagrb->Fill(sx->GetSignal("sigmagrb"));
  htotsigma->Fill(sx->GetSignal("totsigma"));

  if (fAvgrbz<0) zsample.Enter(sx->GetSignal("z"));
  if (fAvgrbt90<0) t90sample.Enter(sx->GetSignal("t90"));
 }

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

 //////////////////////////////////////////////
 // The implementation of the actual program //
 //////////////////////////////////////////////

 Float_t pi=acos(-1.);

 Float_t danglow=0;     // Lower value (in degrees) of angular difference histo
 Float_t dangup=fDawin; // Upper value (in degrees) of angular difference histo
 if (fDatype) dangup=fDawin*fabs(fMaxtotsigma);
 if (dangup<0 || dangup>180) dangup=180;

 //////////////////////////////////////////////////////////////////////////
 // Automatic definition of the various signal and background histograms //
 // based on the provided user settings                                  //
 //////////////////////////////////////////////////////////////////////////

 Int_t ntbins=0;
 Double_t* binarr=0;
 if (fabs(fTbin)>0) // Fixed time bins
 {
  if (fTbin>0) // User specified time binning
  {
   if (fTbint90) fTbin=fTbint90*fabs(fAvgrbt90); 
   ntbins=int(fDtwin/fTbin);
  }
  else // Automatic time binning to get the specified maximal bkg counts per bin
  {
   ntbins=int(fNbkgWin*float(fNgrbs)/fNbkg);
  }
 }
 else // Variable time bins
 {
  Int_t nbx=int(fDtwin/fVarTbin);
  Float_t gamma=fabs(fAvgrbz)+1.;
  Float_t* bins=new Float_t[nbx];
  ntbins=0;
  Float_t xlow=0,xup=0,size=fVarTbin;
  for (Int_t i=0; i<nbx-1; i++)
  {
   xup=xlow+size;
   if (xup>fDtwin/2.) // Store the last lowerbound
   {
    bins[i]=xlow;
    ntbins++;
    break;
   }
   bins[i]=xlow;
   ntbins++;
   xlow=xup;
   size=xlow*gamma;
  }
  binarr=new Double_t[2*ntbins-1];
  for (Int_t j=ntbins; j>0; j--)
  {
   binarr[ntbins-j]=-bins[j-1];
   binarr[ntbins+j-2]=bins[j-1];
  }
  ntbins=2*ntbins-2;
 }

 // Binning for the opening angle histo
 Int_t nabins=int((dangup-danglow)/fAbin);
 if (fAbin<0) nabins=int(((dangup-danglow)/180.)*fNbkgWin*float(fNgrbs)/fNbkg);

 // Binning for the cos(opening angle) histo
 Float_t upcos=cos(danglow*pi/180.);
 Float_t lowcos=cos(dangup*pi/180.);
 Int_t nabins2=int((upcos-lowcos)/(1.-cos(fAbin*pi/180.)));
 if (fAbin<0) nabins2=int(((upcos-lowcos)/2.)*fNbkgWin*float(fNgrbs)/fNbkg);
 if (nabins2<0) nabins2*=-1;

 if (ntbins<2) ntbins=2;
 if (nabins<2) nabins=2;
 if (nabins2<2) nabins2=2;

 gStyle->SetOptStat("e"); // Only display number of entries in stats box

 Float_t tbinfine=0.1; // Bin size (in sec) for the fine binned histos
 Int_t ntbinsfine=int(fDtwin/tbinfine);

 TString title,s;
 title="Arrival times of off-source events in time window";
 title+=";Event arrival time (in sec) w.r.t. burst trigger;Counts per bin of size %-10.3g";
 s=title.Format(title.Data(),tbinfine);
 TH1F* bkgtfine=new TH1F("bkgtfine",s.Data(),ntbinsfine,-fDtwin/2.,fDtwin/2.);
 fBurstHistos.Add(bkgtfine);

 title="Arrival times of on-source events in time window";
 title+=";Event arrival time (in sec) w.r.t. burst trigger;Counts per bin of size %-10.3g";
 s=title.Format(title.Data(),tbinfine);
 TH1F* tottfine=new TH1F("tottfine",s.Data(),ntbinsfine,-fDtwin/2.,fDtwin/2.);
 fBurstHistos.Add(tottfine);

 TH1F* bkgt=0;
 TH1F* tott=0;
 TH2F* bkg2=0;
 TH2F* tot2=0;
 if (fabs(fTbin)>0) // Fixed time bins
 {
  bkgt=new TH1F("bkgt","Arrival times of off-source events in time window",ntbins,-fDtwin/2.,fDtwin/2.);
  tott=new TH1F("tott","Arrival times of on-source events in time window",ntbins,-fDtwin/2.,fDtwin/2.);
  bkg2=new TH2F("bkg2","Arrival time vs. opening angle of off-source events in time window",
                nabins/10,danglow,dangup,ntbins,-fDtwin/2.,fDtwin/2.);
  tot2=new TH2F("tot2","Arrival time vs. opening angle of on-soure events in time window",
                nabins/10,danglow,dangup,ntbins,-fDtwin/2.,fDtwin/2.);
 }
 else // Variable time bins
 {
  bkgt=new TH1F("bkgt","Arrival times of off-source events in time window",ntbins,binarr);
  tott=new TH1F("tott","Arrival times of on-source events in time window",ntbins,binarr);
  bkg2=new TH2F("bkg2","Arrival time vs. opening angle of off-source events in time window",
                nabins/10,danglow,dangup,ntbins,binarr);
  tot2=new TH2F("tot2","Arrival time  vs. opening angle of on-source events in time window",
                nabins/10,danglow,dangup,ntbins,binarr);
 }
 fBurstHistos.Add(bkgt);
 fBurstHistos.Add(tott);
 fBurstHistos.Add(bkg2);
 fBurstHistos.Add(tot2);

 // The opening angle histo
 TH1F* bkga=new TH1F("bkga","Opening angle of off-source events in time window;Opening angle (degrees);Counts",
                     nabins,danglow,dangup);
 TH1F* tota=new TH1F("tota","Opening angle of on-source events in time window;Opening angle (degrees);Counts",
                     nabins,danglow,dangup);
 fBurstHistos.Add(bkga);
 fBurstHistos.Add(tota);

 // The cos(opening angle) histo
 TH1F* bkgcosa=new TH1F("bkgcosa","cos(opening angle) of off-source events in time window;cos(opening angle);Counts",
                        nabins2,lowcos,upcos);
 TH1F* totcosa=new TH1F("totcosa","cos(opening angle) of on-source events in time window;cos(opening angle);Counts",
                        nabins2,lowcos,upcos);
 fBurstHistos.Add(bkgcosa);
 fBurstHistos.Add(totcosa);

 Int_t itbin=int(fTbin);
 if (fTbin<0) itbin=int(fDtwin/float(ntbins));
 s="Counts per ";
 if (fabs(fTbin)>0)
 {
  s+=itbin;
  s+=" seconds";
 }
 else
 {
  s+="time bin";
 }
 bkgt->GetXaxis()->SetTitle("Event arrival time (in sec) w.r.t. burst trigger");
 bkgt->GetYaxis()->SetTitle(s.Data());
 tott->GetXaxis()->SetTitle("Event arrival time (in sec) w.r.t. burst trigger");
 tott->GetYaxis()->SetTitle(s.Data());
 bkg2->GetXaxis()->SetTitle("Opening angle (degrees)");
 bkg2->GetYaxis()->SetTitle("Event arrival time (in sec) w.r.t. burst trigger");
 tot2->GetXaxis()->SetTitle("Opening angle (degrees)");
 tot2->GetYaxis()->SetTitle("Event arrival time (in sec) w.r.t. burst trigger");

 //////////////////////////////////////////////////////////
 // Generation of the signal and background observations //
 // based on the provided user settings                  //
 //////////////////////////////////////////////////////////

 Float_t t90grb=0;
 Double_t zgrb=0;
 Float_t sigmagrb=0;
 Float_t totsigma=0;
 NcPosition rgrb;
 Int_t nmu;
 Double_t thetagrb,phigrb;
 Double_t dmu,thetamu,phimu,cost;
 Float_t dt=0;
 NcPosition rgrb2; // Actual GRB position from which the neutrinos/muons arrive
 NcPosition rmu;
 Float_t dang;
 Float_t thlow,thup;
 Float_t ranlow,ranup;
 Int_t nmugrb=0;
 NcTimestamp* tx=0;
 NcTimestamp tmu;
 Float_t solidangle=0; // Total stacked solid angle
 Float_t ramu,decmu;   // Temporary RA and DEC of muon for background creation

 // Obtain the (fictative) GRB space-time positions in the declination acceptance
 for (Int_t igrb=0; igrb<fNgrbs; igrb++)
 {
  sx=GetSignal(igrb+1);

  if (!sx) continue;

  tx=sx->GetTimestamp();
  t90grb=sx->GetSignal("t90");
  sigmagrb=sx->GetSignal("sigmagrb");
  totsigma=sx->GetSignal("totsigma");
  GetSignal(zgrb,thetagrb,"deg",phigrb,"deg","loc",tx,igrb+1);
  rgrb.SetPosition(zgrb,thetagrb,phigrb,"sph","deg");

  // Update the total stacked solid angle that is probed
  if (fDawin<0) // Local zenith band
  {
   if (!fDatype)
   {
    thlow=thetagrb-0.5*fabs(fDawin);
    thup=thetagrb+0.5*fabs(fDawin);
   }
   else
   {
    thlow=thetagrb-0.5*fabs(fDawin*totsigma);
    thup=thetagrb+0.5*fabs(fDawin*totsigma);
   }
  }
  else // Circle around GRB position
  {
   if (!fDatype)
   {
    thlow=0;
    thup=fabs(fDawin);
   }
   else
   {
    thlow=0;
    thup=fabs(fDawin*totsigma);
   }
  }

  solidangle+=GetSolidAngle(thlow,thup,"deg",0,360,"deg");    

  // Generate the background events in the search time window 
  // for both this GRB angular cone and the corresponding "opposite RA" bkg patch
  for (Int_t bkgpatch=0; bkgpatch<=1; bkgpatch++)
  {
   nmu=int(fRan->Poisson(fNbkgWin));
   for (Int_t imu=0; imu<nmu; imu++)
   {
    ranlow=-fDtwin/2.;
    ranup=fDtwin/2.;
    dt=fRan->Uniform(ranlow,ranup);
/************
@@@@@@@@@@@@
    // Smear the time difference with the Gaussian time resolution
    if (fTimres>0) dt=fRan->Gauss(dt,fTimres); //@@@ Is this needed ?
**********/
//@@@@@@@@    RandomPosition(rmu,90,180,0,360);
    // Create a random background event within the user selected burst declination interval
    // and convert to local detector coordinates to allow a local zenith band selection.
    // For the conversion, a temp. reference signal will be created, since measurements may get scrambled.
    thlow=90.-fDeclmax;
    thup=90.-fDeclmin;
    RandomPosition(rmu,thlow,thup,0,360);
    decmu=90.-rmu.GetX(2,"sph","deg");
    ramu=rmu.GetX(3,"sph","deg");
    tmu=*tx;
    tmu.AddSec(dt);
    SetSignal(1,ramu,"deg",decmu,"deg","equ",&tmu,fNgrbs+1,"J","bkgtemp",0);
    GetSignal(dmu,thetamu,"deg",phimu,"deg","loc",&tmu,fNgrbs+1);
    rmu.SetPosition(1,thetamu,phimu,"sph","deg");
/************
@@@@@@@@@@
    // Smear the direction of the upgoing bkg muon according to  the detector resolution
    SmearPosition(rmu,fAngres); //@@@ Is this needed
*********/
//@@@@    thetamu=rmu.GetX(2,"sph","deg");
//@@@@    phimu=rmu.GetX(3,"sph","deg");

    if (fDawin<0) // Local zenith band
    {
     dang=fabs(thetagrb-thetamu);
    }
    else // Circle around GRB position
    {
     dang=rgrb.GetOpeningAngle(rmu,"deg");
    }

    if ((!fDatype && dang>fabs(fDawin)) || (fDatype && dang>fabs(fDawin*totsigma))) continue;

    if (!bkgpatch)
    {
     tottfine->Fill(dt);
     tott->Fill(dt);
     tota->Fill(dang);
     totcosa->Fill(cos(dang*pi/180.));
     tot2->Fill(dang,dt);
    }
    else
    {
     bkgtfine->Fill(dt);
     bkgt->Fill(dt);
     bkga->Fill(dang);
     bkgcosa->Fill(cos(dang*pi/180.));
     bkg2->Fill(dang,dt);
    }
   }
  }

  // Generate the GRB related signal event(s) in the search window.
  // The GRB position gets Gaussian smeared to reflect the actual position.
  // The time difference between the gammas and the neutrinos gets corrected
  // for the GRB redshift and smeared by the detector time resolution.
  // The muon direction gets modified to account for the kinematical opening angle
  // w.r.t. the neutrino direction and Gaussian smeared by the detector angular resolution.

  // Prevent statistical overfluctuation in number of GRB signal events if requested by fGrbnu<0
  if (fGrbnu<0 && nmugrb>=int(fabs(fGrbnu)*float(fNgrbs))) continue;

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
    else
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
    else
    {
     dt=fRan->Gauss(fDtnu*t90grb,fDtnus);
    }
   }
   if (fTimres>0) dt=fRan->Gauss(dt,fTimres);

   // The direction of the GRB signal muon
   rmu.Load(rgrb2);

   // Modification to account for the neutrino-lepton kinematic opening angle
   if (fKinangle>0)
   {
    Double_t E=GetBurstSignalEnergy();
    if (E>0)
    {
     Int_t mode=fKinangle-1;
     Double_t ang=GetNeutrinoAngle(E,"deg",mode);
     if (ang>0) ShiftPosition(rmu,ang);
    }
   }

   // Smearing according to the detector resolution
   if (fAngres>0) SmearPosition(rmu,fAngres);

   // Determine angular difference w.r.t. the presumed GRB position
   dang=rgrb.GetOpeningAngle(rmu,"deg");

   if ((!fDatype && dang>fabs(fDawin)) || (fDatype && dang>fabs(fDawin*totsigma))) continue;

   tottfine->Fill(dt);
   tott->Fill(dt);
   tota->Fill(dang);
   totcosa->Fill(cos(dang*pi/180.));
   tot2->Fill(dang,dt);
  }
 } // End of loop over the individual GRBs

 // Remove the temporary storage of the background event
 if (fNgrbs>0) RemoveSignal(fNgrbs+1,0,0);

 // Compensate statistical underfluctuation in number of GRB signal events if requested by fGrbnu<0
 if (fGrbnu<0) BurstCompensate(nmugrb,fGrbnu,fNgrbs,fInburst,fDtnu,fDtnus,fAngres,fTimres,fDatype,fDawin);

 // Determination of total and background event rates
 Int_t nbt=tott->GetNbinsX();
 Int_t nba=tota->GetNbinsX();
 Int_t nba2=totcosa->GetNbinsX();
 Float_t underflow, overflow;
 Float_t nentot=tott->GetEntries();
 underflow=tott->GetBinContent(0);
 overflow=tott->GetBinContent(nbt+1);
 nentot=nentot-(underflow+overflow);
 Float_t nenbkg=bkgt->GetEntries();
 underflow=bkgt->GetBinContent(0);
 overflow=bkgt->GetBinContent(nbt+1);
 nenbkg=nenbkg-(underflow+overflow);

 Float_t ratetot=nentot/(fDtwin);
 Float_t ratebkg=nenbkg/(fDtwin);

 // Statistics of the stacked event samples
 cout << " *** Statistics of the stacked observed event samples ***" << endl;
 cout << " Total stacked solid angle (in sr) : " << solidangle << " in " << fNgrbs << " stacked patches." << endl; 
 cout << " *On source* Number of events : " << nentot << endl;
 cout << " Stacked \"on source\" event rate (Hz) : " << ratetot << " --> Event rate (Hz) per burst : " << ratetot/float(fNgrbs) << endl;
 cout << " *Off source* Number of (bkg) events : " << nenbkg << endl;
 cout << " Stacked \"off source\" event rate (Hz) : " << ratebkg << " --> Bkg event rate (Hz) per burst : " << ratebkg/float(fNgrbs) << endl;
 cout << " --- (Unknown) Number of \"on source\" signal events : " << nmugrb << " Number of \"on source\" bkg events : " << (nentot-nmugrb) << endl;
 cout << endl; 

 // Update internal statistics
 fBurstParameters->SetSignal(fAvgrbz,"Avgrbz");
 fBurstParameters->SetSignal(fAvgrbt90,"Avgrbt90");
 fBurstParameters->SetSignal(fTbin,"Tbin");
 fBurstParameters->AddNamedSlot("solidangle");
 fBurstParameters->SetSignal(solidangle,"solidangle");
 fBurstParameters->AddNamedSlot("nentot");
 fBurstParameters->SetSignal(nentot,"nentot");
 fBurstParameters->AddNamedSlot("nenbkg");
 fBurstParameters->SetSignal(nenbkg,"nenbkg");
 fBurstParameters->AddNamedSlot("ratetot");
 fBurstParameters->SetSignal(ratetot,"ratetot");
 fBurstParameters->AddNamedSlot("ratebkg");
 fBurstParameters->SetSignal(ratebkg,"ratebkg");
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::BurstCompensate(Int_t& nmugrb,Float_t fGrbnu,Float_t fNgrbs,Int_t fInburst,Float_t fDtnu,Float_t fDtnus,Float_t fAngres,Float_t fTimres,Float_t fDatype,Float_t fDawin)
{
// Compensate statistical underfluctuation in the number of transient burst muons.

 // Retreive the needed parameters
 Int_t fKinangle=fBurstParameters->GetSignal("Kinangle");

 Int_t nmu=int(fabs(fGrbnu)*float(fNgrbs));
 Int_t jgrb=0;
 NcSignal* sx=0;
 NcTimestamp* tx=0;
 Float_t t90grb=0;
 Float_t sigmagrb=0;
 Float_t totsigma=0;
 NcPosition rgrb;
 NcPosition rgrb2;
 Float_t dt=0;
 Double_t zgrb=0;
 Double_t thetagrb=0;
 Double_t phigrb=0;
 Float_t dang=0;
 NcPosition rmu;

 TH1* tottfine=(TH1*)fBurstHistos.FindObject("tottfine");
 TH1* tott=(TH1*)fBurstHistos.FindObject("tott");
 TH1* tota=(TH1*)fBurstHistos.FindObject("tota");
 TH1* totcosa=(TH1*)fBurstHistos.FindObject("totcosa");
 TH2* tot2=(TH2*)fBurstHistos.FindObject("tot2");

 Double_t pi=acos(-1.);

 while (nmugrb<nmu)
 {
  // Pick randomly one of the stored GRBs
   jgrb=int(fRan->Uniform(0.,float(fNgrbs)));
   if (jgrb==0) jgrb=1;
   sx=GetSignal(jgrb);

   if (!sx) continue;

   tx=sx->GetTimestamp();
   t90grb=sx->GetSignal("t90");
   GetSignal(zgrb,thetagrb,"deg",phigrb,"deg","loc",tx,jgrb);
   rgrb.SetPosition(zgrb,thetagrb,phigrb,"sph","deg");
   sigmagrb=sx->GetSignal("sigmagrb");
   totsigma=sx->GetSignal("totsigma");

   // Obtain actual GRB position
   rgrb2.Load(rgrb);
   SmearPosition(rgrb2,sigmagrb); //@@@

   nmugrb++;

   if (!fInburst) // Neutrino and gamma production decoupled
   {
    if (fDtnus<0) // Sigma in units of T90
    {
     dt=fRan->Gauss(fDtnu,fabs(fDtnus)*t90grb);
    }
    else
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
    else
    {
     dt=fRan->Gauss(fDtnu*t90grb,fDtnus);
    }
   }
   if (fTimres>0) dt=fRan->Gauss(dt,fTimres);

   // The direction of the GRB signal muon
   rmu.Load(rgrb2);

   // Modification to account for the neutrino-lepton kinematic opening angle
   if (fKinangle>0)
   {
    Double_t E=GetBurstSignalEnergy();
    if (E>0)
    {
     Int_t mode=fKinangle-1;
     Double_t ang=GetNeutrinoAngle(E,"deg",mode);
     if (ang>0) ShiftPosition(rmu,ang);
    }
   }

   // Smearing according to the detector resolution
   if (fAngres>0) SmearPosition(rmu,fAngres);

   // Determine angular difference w.r.t. the presumed GRB position
   dang=rgrb.GetOpeningAngle(rmu,"deg");

   if ((!fDatype && dang>fabs(fDawin)) || (fDatype && dang>fabs(fDawin*totsigma))) continue;

   if (tottfine) tottfine->Fill(dt);
   if (tott) tott->Fill(dt);
   if (tota) tota->Fill(dang);
   if (totcosa) totcosa->Fill(cos(dang*pi/180.));
   if (tot2) tot2->Fill(dang,dt);
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

 TH1* tott=(TH1*)fBurstHistos.FindObject("tott");
 TH1* bkgt=(TH1*)fBurstHistos.FindObject("bkgt");

 if (!tott || !bkgt) return 0;

 Double_t underflow,overflow;
 Int_t nbins=0;
 Double_t nentot=tott->GetEntries();
 nbins=tott->GetNbinsX();
 underflow=tott->GetBinContent(0);
 overflow=tott->GetBinContent(nbins+1);
 nentot=nentot-(underflow+overflow);
 Double_t nenbkg=bkgt->GetEntries();
 nbins=bkgt->GetNbinsX();
 underflow=bkgt->GetBinContent(0);
 overflow=bkgt->GetBinContent(nbins+1);
 nenbkg=nenbkg-(underflow+overflow);

 if (nentot<=0 || nenbkg<=0) return 0;

 Int_t fNgrbs=GetNsignals(0);
 Float_t fDtwin=fBurstParameters->GetSignal("Dtwin");

 // The Bayesian posterior background and signal rate PDFs
 Double_t Non=nentot;
 Double_t Ton=fDtwin*float(fNgrbs);
 Double_t Noff=nenbkg;
 Double_t Toff=Ton;
 TF1 fbkgrpdf=GetBackgroundRatePDF(Noff,Toff);
 TF1 fsigrpdf=GetSignalRatePDF(Non,Ton,Noff,Toff);

 // Determine the "p%" credible interval for the signal rate
 Float_t frac=0;
 frac=GetCredibleInterval(fsigrpdf,p,rlow,rup,n);

 // Provide the signal and background rate PDFs as histograms in the output file
 fbkgrpdf.SetRange(0,3.*Noff/Toff);
 fbkgrpdf.SetNpx(n);
 TH1* hpdfbkgr=(TH1*)fbkgrpdf.GetHistogram()->Clone();
 hpdfbkgr->SetName("hpdfbkgr");
 fBurstHistos.Add(hpdfbkgr);
 fsigrpdf.SetRange(0,3.*Non/Ton);
 fsigrpdf.SetNpx(n);
 TH1* hpdfsigr=(TH1*)fsigrpdf.GetHistogram()->Clone();
 hpdfsigr->SetName("hpdfsigr");
 fBurstHistos.Add(hpdfsigr);

 cout << endl;
 cout << " *" << ClassName() << "::GetBurstBayesianSignalRate* Credible interval [rlow,rup] for p=" << p << "%"
      << " with a precision of 1/" << n << endl;
 cout << " The " << frac << "% credible interval from the Bayesian signal pdf :"
      << " rlow=" << rlow << " rup=" << rup << endl;
 cout << " The following signal and background rate PDF histograms have been generated :" << endl;
 cout << " ... " << hpdfsigr->GetName() << " : " << hpdfsigr->GetTitle() << endl;      
 cout << " ... " << hpdfbkgr->GetName() << " : " << hpdfbkgr->GetTitle() << endl;      

 return hpdfsigr;
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

 TH1* tott=(TH1*)fBurstHistos.FindObject("tott");
 TH1* bkgt=(TH1*)fBurstHistos.FindObject("bkgt");

 if (!tott || !bkgt) return 0;

 Double_t underflow,overflow;
 Int_t nbins=0;
 Double_t nentot=tott->GetEntries();
 nbins=tott->GetNbinsX();
 underflow=tott->GetBinContent(0);
 overflow=tott->GetBinContent(nbins+1);
 nentot=nentot-(underflow+overflow);
 Double_t nenbkg=bkgt->GetEntries();
 nbins=bkgt->GetNbinsX();
 underflow=bkgt->GetBinContent(0);
 overflow=bkgt->GetBinContent(nbins+1);
 nenbkg=nenbkg-(underflow+overflow);

 if (nentot<=0 || nenbkg<=0) return 0;

 Int_t fNgrbs=GetNsignals(0);
 Float_t fDtwin=fBurstParameters->GetSignal("Dtwin");

 // The "on source" and "off source" data
 Int_t Non=int(nentot);
 Double_t Ton=fDtwin*float(fNgrbs);
 Int_t Noff=int(nenbkg);
 Double_t Toff=Ton;

 NcMath m;
 sigma=m.LiMaSignificance(Non,Ton,Noff,Toff);

 cout << endl;
 cout << " *" << ClassName() << "::GetBurstLiMaSignificance* The Li-Ma signal significance is : " << sigma << " sigma." << endl;

 return sigma;
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GetBurstBayesianPsiStatistics(TString type,Double_t nr,Int_t ncut,Int_t ndt,Int_t mode, Double_t fact,Int_t freq)
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
// type : "time"  --> Provide statistics for the observed arrival times
//                    This will investigate the deviation from a uniform background time spectrum 
//        "angle" --> Provide statistics for the observed opening angles
//                    This will investigate the deviation from a isotropic background angular spectrum 
//        "cosa"  --> Provide statistics for the cosine of the observed opening angles
//                    This will investigate the deviation from a uniform background cos(angle) spectrum 
//        "dt"    --> Provide statistics for the time intervals between the observed arrival times
//                    This will investigate the deviation from dt spectrum expected from Poisson statistics
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
// mode : The "mode" parameter as documented in the member function GetDxHistogram().
// fact : The "fact" parameter as documented in the member function GetDxHistogram().
// freq : Use frequentist's approximation (1) or exact Bayesian expression (0) for psi.
//
// The default values are nr=-1, ncut=10, ndt=2, mode=1, fact=1 and freq=0.

 NcMath math;

 TString text="none";
 if (type=="time") text="arrival time";
 if (type=="angle") text="opening angle";
 if (type=="cosa") text="cos(opening angle)";
 if (type=="dt") text="arrival time interval";

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

 Double_t psitot=-1, psibkg=-1;
 Float_t psidif=0;
 Float_t psimintot=-1, psimaxtot=-1, psifractot=0;
 Float_t psiminbkg=-1, psimaxbkg=-1, psifracbkg=0;
 Double_t nrxtot=-1, nrxbkg=-1;
 Double_t pvaluetot=-1, pvaluebkg=-1;
 TH1F* rtot=0;
 TH1F* rbkg=0;

 ////////////////////////////////////////////
 // Arrival time histo Bayesian statistics //
 ////////////////////////////////////////////
 if (type=="time")
 {
  TH1* tot=(TH1*)fBurstHistos.FindObject("tott");
  TH1* bkg=(TH1*)fBurstHistos.FindObject("bkgt");

  if (!tot || !bkg) return;

  psitot=math.PsiValue(tot,0,0,freq);
  psibkg=math.PsiValue(bkg,0,0,freq);
  psidif=psitot-psibkg;

  // Extreme Psi values for a pure background hypothesis of the recorded arrival time entries
  psimintot=math.PsiExtreme(tot,0,0,-2);
  psimaxtot=math.PsiExtreme(tot,0,0,-1);
  psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  psiminbkg=math.PsiExtreme(bkg,0,0,-2); 
  psimaxbkg=math.PsiExtreme(bkg,0,0,-1);
  psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);

  // P-value determination
  if (nr>=0)
  {
   TH1F* hrpsitott=(TH1F*)fBurstHistos.FindObject("hrpsitott");
   TH1F* hrpsibkgt=(TH1F*)fBurstHistos.FindObject("hrpsibkgt");

   if (hrpsitott)
   {
    rtot=(TH1F*)hrpsitott->Clone();
    rtot->Reset();
   }
   else
   {
    rtot=new TH1F("hrpsitott","Random #psi distr. for bkg hypothesis of on-source arrival time data",100,psimintot-1.,psimaxtot+1.);
   }

   if (hrpsibkgt)
   {
    rbkg=(TH1F*)hrpsibkgt->Clone();
    rbkg->Reset();
   }
   else
   {
    rbkg=new TH1F("hrpsibkgt","Random #psi distr. for bkg hypothesis of off-source arrival time data",100,psiminbkg-1.,psimaxbkg+1.);
   }

   pvaluetot=math.PsiPvalue(-1,nr,tot,0,0,freq,0,rtot,ncut,&nrxtot);
   pvaluebkg=math.PsiPvalue(-1,nr,bkg,0,0,freq,0,rbkg,ncut,&nrxbkg);
   fBurstHistos.Add(rtot);
   fBurstHistos.Add(rbkg);

   cout << " The following randomised Psi histograms have been generated :" << endl;
   cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
   cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }
 }
 
 /////////////////////////////////////////////
 // Opening angle histo Bayesian statistics //
 /////////////////////////////////////////////
 if (type=="angle")
 {
  TH1* tot=(TH1*)fBurstHistos.FindObject("tota");
  TH1* bkg=(TH1*)fBurstHistos.FindObject("bkga");

  if (!tot || !bkg) return;

  TF1 pdfa("pdfa","sin(x*acos(-1.)/180.)");
  psitot=math.PsiValue(tot,0,&pdfa,freq);
  psibkg=math.PsiValue(bkg,0,&pdfa,freq);
  psidif=psitot-psibkg;

  // Extreme Psi values for a pure background hypothesis of the recorded opening angle entries
  psimintot=math.PsiExtreme(tot,0,&pdfa,-2);
  psimaxtot=math.PsiExtreme(tot,0,&pdfa,-1);
  psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  psiminbkg=math.PsiExtreme(bkg,0,&pdfa,-2);
  psimaxbkg=math.PsiExtreme(bkg,0,&pdfa,-1);
  psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);

  // P-value determination
  if (nr>=0)
  {
   TH1F* hrpsitota=(TH1F*)fBurstHistos.FindObject("hrpsitota");
   TH1F* hrpsibkga=(TH1F*)fBurstHistos.FindObject("hrpsibkga");

   if (hrpsitota)
   {
    rtot=(TH1F*)hrpsitota->Clone();
    rtot->Reset();
   }
   else
   {
    rtot=new TH1F("hrpsitota","Random #psi distr. for bkg hypothesis of on-source opening angle data",100,psimintot-1.,psimaxtot+1.);
   }

   if (hrpsibkga)
   {
    rbkg=(TH1F*)hrpsibkga->Clone();
    rbkg->Reset();
   }
   else
   {
    rbkg=new TH1F("hrpsibkga","Random #psi distr. for bkg hypothesis of off-source opening angle data",100,psiminbkg-1.,psimaxbkg+1.);
   }

   pvaluetot=math.PsiPvalue(-1,nr,tot,0,&pdfa,freq,0,rtot,ncut,&nrxtot);
   pvaluebkg=math.PsiPvalue(-1,nr,bkg,0,&pdfa,freq,0,rbkg,ncut,&nrxbkg);
   fBurstHistos.Add(rtot);
   fBurstHistos.Add(rbkg);

   cout << " The following randomised Psi histograms have been generated :" << endl;
   cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
   cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }      
 }

 ///////////////////////////////////////////////////////
 // Cosine of opening angle histo Bayesian statistics //
 ///////////////////////////////////////////////////////
 if (type=="cosa")
 {
  TH1* tot=(TH1*)fBurstHistos.FindObject("totcosa");
  TH1* bkg=(TH1*)fBurstHistos.FindObject("bkgcosa");

  if (!tot || !bkg) return;

  psitot=math.PsiValue(tot,0,0,freq);
  psibkg=math.PsiValue(bkg,0,0,freq);
  psidif=psitot-psibkg;

  // Extreme Psi values for a pure background hypothesis of the recorded cos(opening angle) entries
  psimintot=math.PsiExtreme(tot,0,0,-2);
  psimaxtot=math.PsiExtreme(tot,0,0,-1);
  psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  psiminbkg=math.PsiExtreme(bkg,0,0,-2);
  psimaxbkg=math.PsiExtreme(bkg,0,0,-1);
  psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);

  // P-value determination
  if (nr>=0)
  {
   TH1F* hrpsitotcosa=(TH1F*)fBurstHistos.FindObject("hrpsitotcosa");
   TH1F* hrpsibkgcosa=(TH1F*)fBurstHistos.FindObject("hrpsibkgcosa");

   if (hrpsitotcosa)
   {
    rtot=(TH1F*)hrpsitotcosa->Clone();
    rtot->Reset();
   }
   else
   {
    rtot=new TH1F("hrpsitotcosa","Random #psi distr. for bkg hypothesis of on-source cos(opening angle) data",100,psimintot-1.,psimaxtot+1.);
   }

   if (hrpsibkgcosa)
   {
    rbkg=(TH1F*)hrpsibkgcosa->Clone();
    rbkg->Reset();
   }
   else
   {
    rbkg=new TH1F("hrpsibkgcosa","Random #psi distr. for bkg hypothesis of off-source cos(opening angle) data",100,psiminbkg-1.,psimaxbkg+1.);
   }
    pvaluetot=math.PsiPvalue(-1,nr,tot,0,0,freq,0,rtot,ncut,&nrxtot);
    pvaluebkg=math.PsiPvalue(-1,nr,bkg,0,0,freq,0,rbkg,ncut,&nrxbkg);
    fBurstHistos.Add(rtot);
    fBurstHistos.Add(rbkg);

    cout << " The following randomised Psi histograms have been generated :" << endl;
    cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
    cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }
 }

 ///////////////////////////////////////////////
 // Arrival time interval Bayesian statistics //
 ///////////////////////////////////////////////
 if (type=="dt")
 {
  TH1* tot=(TH1*)fBurstHistos.FindObject("tottfine");
  TH1* bkg=(TH1*)fBurstHistos.FindObject("bkgtfine");

  if (!tot || !bkg) return;

  // Create the delta t histograms
  TString nametot="htotdt";
  nametot+=ndt;
  TString namebkg="hbkgdt";
  namebkg+=ndt;
  TH1F* htotdt=(TH1F*)fBurstHistos.FindObject(nametot.Data());
  TH1F* hbkgdt=(TH1F*)fBurstHistos.FindObject(namebkg.Data());
  if (!htotdt && !hbkgdt)
  {
   htotdt=(TH1F*)(GetDxHistogram(tot,ndt,-1,0,-1,mode,fact).Clone(nametot.Data()));
   Double_t deltatbin=htotdt->GetXaxis()->GetBinWidth(1);
   hbkgdt=(TH1F*)(GetDxHistogram(bkg,ndt,deltatbin,0,-1,mode,fact).Clone(namebkg.Data()));

   // Create titles and labels for the delta t histograms
   TString title="Time intervals between ";
   title+=ndt+1;
   title+=" consecutive events in the on-source time window";
   title+=";dt (in sec);Counts per bin of size %-10.3g";
   TString s=title.Format(title.Data(),deltatbin);
   htotdt->SetTitle(s.Data());

   title="Time intervals between ";
   title+=ndt+1;
   title+=" consecutive events in the off-source time window";
   title+=";dt (in sec);Counts per bin of size %-10.3g";
   s=title.Format(title.Data(),deltatbin);
   hbkgdt->SetTitle(s.Data());

   fBurstHistos.Add(htotdt);
   fBurstHistos.Add(hbkgdt);

   cout << " The following arrival time interval (dt) histograms have been generated :" << endl;
   cout << " ... " << htotdt->GetName() << " : " << htotdt->GetTitle() << endl;      
   cout << " ... " << hbkgdt->GetName() << " : " << hbkgdt->GetTitle() << endl;
  }

  // Creation of the Poisson based dt PDFs from the observed data for a background only hypothesis 
  Double_t underflow,overflow;
  Int_t nbins=0;
  Double_t nentot=tot->GetEntries();
  nbins=tot->GetNbinsX();
  underflow=tot->GetBinContent(0);
  overflow=tot->GetBinContent(nbins+1);
  nentot=nentot-(underflow+overflow);
  Double_t nenbkg=bkg->GetEntries();
  nbins=bkg->GetNbinsX();
  underflow=bkg->GetBinContent(0);
  overflow=bkg->GetBinContent(nbins+1);
  nenbkg=nenbkg-(underflow+overflow);

  if (nentot<=0 || nenbkg<=0) return;

  Double_t fDtwin=fBurstParameters->GetSignal("Dtwin");
  Double_t ratetot=nentot/(fDtwin);
  Double_t ratebkg=nenbkg/(fDtwin);

  // Determine the corresponding dt PDFs based on Poisson statistics
  // Only the bkg dt PDF is used, since this may be obtained from off-source measurements.
  // Using the total dt PDF would artificially lower the sensitivity due to possible signal events.

  TF1 fdttot=math.PoissonDtDist(ratetot,ndt); // Only for reference, not used in the analysis
  TF1 fdtbkg=math.PoissonDtDist(ratebkg,ndt);

  nametot="hpdftotdt";
  nametot+=ndt;
  namebkg="hpdfbkgdt";
  namebkg+=ndt;
  TH1* hpdftotdt=(TH1*)fBurstHistos.FindObject(nametot.Data());
  TH1* hpdfbkgdt=(TH1*)fBurstHistos.FindObject(namebkg.Data());

  // Provide the dt PDFs as histograms in the output file
  if (!hpdftotdt && !hpdfbkgdt)
  {
   Double_t deltatmax=htotdt->GetXaxis()->GetXmax();
   Double_t xmaxfdt=deltatmax;
   deltatmax=hbkgdt->GetXaxis()->GetXmax();
   if (deltatmax>xmaxfdt) xmaxfdt=deltatmax;
   fdttot.SetRange(0,xmaxfdt);
   fdttot.SetNpx(10000);
   hpdftotdt=(TH1*)fdttot.GetHistogram()->Clone();
   hpdftotdt->SetName(nametot.Data());
   fdtbkg.SetRange(0,xmaxfdt);
   fdtbkg.SetNpx(10000);
   hpdfbkgdt=(TH1*)fdtbkg.GetHistogram()->Clone();
   hpdfbkgdt->SetName(namebkg.Data());
   fBurstHistos.Add(hpdftotdt);
   fBurstHistos.Add(hpdfbkgdt);

   cout << " The following arrival time interval (dt) PDFs have been generated :" << endl;
   cout << " ... " << hpdftotdt->GetName() << " : " << hpdftotdt->GetTitle() << endl;      
   cout << " ... " << hpdfbkgdt->GetName() << " : " << hpdfbkgdt->GetTitle() << endl;
  }

  psitot=math.PsiValue(htotdt,0,&fdtbkg,freq);
  psibkg=math.PsiValue(hbkgdt,0,&fdtbkg,freq);
  psidif=psitot-psibkg;

  psimintot=math.PsiExtreme(htotdt,0,&fdtbkg,-2);
  psimaxtot=math.PsiExtreme(htotdt,0,&fdtbkg,-1);
  psifractot=(psimaxtot-psitot)/(psimaxtot-psimintot);
  psiminbkg=math.PsiExtreme(hbkgdt,0,&fdtbkg,-2);
  psimaxbkg=math.PsiExtreme(hbkgdt,0,&fdtbkg,-1);
  psifracbkg=(psimaxbkg-psibkg)/(psimaxbkg-psiminbkg);

  // P-value determination
  if (nr>=0)
  {
   nametot="hrpsitotdt";
   nametot+=ndt;
   namebkg="hrpsibkgdt";
   namebkg+=ndt;

   TH1F* hrpsitotdt=(TH1F*)fBurstHistos.FindObject(nametot.Data());
   TH1F* hrpsibkgdt=(TH1F*)fBurstHistos.FindObject(namebkg.Data());

   TString title;
   if (hrpsitotdt)
   {
    rtot->Reset();
   }
   else
   {
    title="Random #psi distr. for bkg hypothesis of on-source dt data for n=";
    title+=ndt;
    rtot=new TH1F(nametot.Data(),title.Data(),100,psimintot-1.,psimaxtot+1.);
   }

   if (hrpsibkgdt)
   {
    rbkg->Reset();
   }
   else
   {
    title="Random #psi distr. for bkg hypothesis of off-source dt data for n=";
    title+=ndt;
    rbkg=new TH1F(namebkg.Data(),title.Data(),100,psiminbkg-1.,psimaxbkg+1.);
   }

   pvaluetot=math.PsiPvalue(-1,nr,htotdt,0,&fdtbkg,freq,0,rtot,ncut,&nrxtot);
   pvaluebkg=math.PsiPvalue(-1,nr,hbkgdt,0,&fdtbkg,freq,0,rbkg,ncut,&nrxbkg);
   fBurstHistos.Add(rtot);
   fBurstHistos.Add(rbkg);

   cout << " The following randomised Psi histograms have been (re)generated :" << endl;
   cout << " ... " << rtot->GetName() << " : " << rtot->GetTitle() << endl;      
   cout << " ... " << rbkg->GetName() << " : " << rbkg->GetTitle() << endl;
  }
 }

 // Listing of the statistics results
 cout << " *** Observed Psi values (in dB) for the hypothesis of no burst signal ***" << endl;
 cout << " For the \"on source\" stacked patches : psi = " << psitot << endl;
 cout << " For the corresponding \"opposite RA\" stacked \"off source\" patches : psi = " << psibkg << endl;
 cout << " --> Difference between observed \"on source\" and \"off source\" psi values : " << psidif << endl;
 cout << " *** Extreme Psi values for the case of pure background ***" << endl;
 cout << " For \"on source\"  psimin : " << psimintot << " psimax : " << psimaxtot << " (psimax-psi)/range : " << psifractot << endl;
 cout << " For \"off source\" psimin : " << psiminbkg << " psimax : " << psimaxbkg << " (psimax-psi)/range : " << psifracbkg << endl;

 if (nr>=0)
 {
  cout << " *** P-values of the observed \"on source\" and \"off source\" psi values ***" << endl;
  cout << " For the \"on source\"  stacked patches : P-value = " << pvaluetot << " Used number of randomisations : " << nrxtot << endl;
  cout << " For the \"off source\" stacked patches : P-value = " << pvaluebkg << " Used number of randomisations : " << nrxbkg << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAstrolab::GetBurstChi2Statistics(TString type,Int_t ndt,Int_t mode,Double_t fact)
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
// type : "time"  --> Provide statistics for the observed arrival times
//                    This will investigate the deviation from a uniform background time spectrum 
//        "angle" --> Provide statistics for the observed opening angles
//                    This will investigate the deviation from a isotropic background angular spectrum 
//        "cosa"  --> Provide statistics for the cosine of the observed opening angles
//                    This will investigate the deviation from a uniform background cos(angle) spectrum 
//        "dt"    --> Provide statistics for the time intervals between the observed arrival times
//                    This will investigate the deviation from dt spectrum expected from Poisson statistics
// ndt  : The step count to arrive at the required consecutive entry to perform the dt statistics.
//        Specifying "ndt=1" will provide the statistics of dt intervals between each consecutive event,
//        i.e. time intervals between the events (1,2), (2,3), (3,4) etc.
//        Specifying "ndt=2" will provide the statistics of dt intervals between every 2nd consecutive event,
//        i.e. time intervals between the events (1,3), (2,4), (3,5) etc.
// mode : The "mode" parameter as documented in the member function GetDxHistogram().
// fact : The "fact" parameter as documented in the member function GetDxHistogram().
//
// The default values are ndt=2, mode=1 and fact=1.

 NcMath math;

 TString text="none";
 if (type=="time") text="arrival time";
 if (type=="angle") text="opening angle";
 if (type=="cosa") text="cos(opening angle)";
 if (type=="dt") text="arrival time interval";

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

 TH1F* rtot=0;
 TH1F* rbkg=0;
 Int_t ndftot=0;
 Int_t ndfbkg=0;
 Float_t chitot=0;
 Float_t chibkg=0;

 ///////////////////////////////////////////////
 // Arrival time histo Chi-squared statistics //
 ///////////////////////////////////////////////
 if (type=="time")
 {
  TH1* tot=(TH1*)fBurstHistos.FindObject("tott");
  TH1* bkg=(TH1*)fBurstHistos.FindObject("bkgt");

  if (!tot || !bkg) return;

  chitot=math.Chi2Value(tot,0,0,&ndftot);
  chibkg=math.Chi2Value(bkg,0,0,&ndfbkg);
 }

 ////////////////////////////////////////////////
 // Opening angle histo Chi-squared statistics //
 ////////////////////////////////////////////////
 if (type=="angle")
 {
  TH1* tot=(TH1*)fBurstHistos.FindObject("tota");
  TH1* bkg=(TH1*)fBurstHistos.FindObject("bkga");

  if (!tot || !bkg) return;

  TF1 pdf("pdf","sin(x*acos(-1.)/180.)");
  chitot=math.Chi2Value(tot,0,&pdf,&ndftot);
  chibkg=math.Chi2Value(bkg,0,&pdf,&ndfbkg);
 }

 //////////////////////////////////////////////////////////
 // Cosine of opening angle histo Chi-squared statistics //
 //////////////////////////////////////////////////////////
 if (type=="cosa")
 {
  TH1* tot=(TH1*)fBurstHistos.FindObject("totcosa");
  TH1* bkg=(TH1*)fBurstHistos.FindObject("bkgcosa");

  if (!tot || !bkg) return;

  chitot=math.Chi2Value(tot,0,0,&ndftot);
  chibkg=math.Chi2Value(bkg,0,0,&ndfbkg);
 }

 ///////////////////////////////////////////////
 // Arrival time interval Bayesian statistics //
 ///////////////////////////////////////////////
 if (type=="dt")
 {
  TH1* tot=(TH1*)fBurstHistos.FindObject("tottfine");
  TH1* bkg=(TH1*)fBurstHistos.FindObject("bkgtfine");

  if (!tot || !bkg) return;
 
  // Create the delta t histograms
  TString nametot="htotdt";
  nametot+=ndt;
  TString namebkg="hbkgdt";
  namebkg+=ndt;
  TH1F* htotdt=(TH1F*)fBurstHistos.FindObject(nametot.Data());
  TH1F* hbkgdt=(TH1F*)fBurstHistos.FindObject(namebkg.Data());
  Double_t deltatbin=0, deltatmin=0, deltatmax=0;
  if (!htotdt && !hbkgdt)
  {
   htotdt=(TH1F*)(GetDxHistogram(tot,ndt,-1,0,-1,mode,fact).Clone(nametot.Data()));
   deltatbin=htotdt->GetXaxis()->GetBinWidth(1);
   deltatmin=htotdt->GetXaxis()->GetXmin();
   deltatmax=htotdt->GetXaxis()->GetXmax();
   hbkgdt=(TH1F*)(GetDxHistogram(bkg,ndt,deltatbin,deltatmin,deltatmax,mode,fact).Clone(namebkg.Data()));

   // Create titles and labels for the delta t histograms
   TString title="Time intervals between ";
   title+=ndt+1;
   title+=" consecutive events in the on-source time window";
   title+=";dt (in sec);Counts per bin of size %-10.3g";
   TString s=title.Format(title.Data(),deltatbin);
   htotdt->SetTitle(s.Data());

   title="Time intervals between ";
   title+=ndt+1;
   title+=" consecutive events in the off-source time window";
   title+=";dt (in sec);Counts per bin of size %-10.3g";
   s=title.Format(title.Data(),deltatbin);
   hbkgdt->SetTitle(s.Data());

   fBurstHistos.Add(htotdt);
   fBurstHistos.Add(hbkgdt);

   cout << " The following arrival time interval (dt) histograms have been generated :" << endl;
   cout << " ... " << htotdt->GetName() << " : " << htotdt->GetTitle() << endl;      
   cout << " ... " << hbkgdt->GetName() << " : " << hbkgdt->GetTitle() << endl;
  }

  // Creation of the Poisson based dt PDFs from the observed data for a background only hypothesis 
  Double_t underflow,overflow;
  Int_t nbins=0;
  Double_t nentot=tot->GetEntries();
  nbins=tot->GetNbinsX();
  underflow=tot->GetBinContent(0);
  overflow=tot->GetBinContent(nbins+1);
  nentot=nentot-(underflow+overflow);
  Double_t nenbkg=bkg->GetEntries();
  nbins=bkg->GetNbinsX();
  underflow=bkg->GetBinContent(0);
  overflow=bkg->GetBinContent(nbins+1);
  nenbkg=nenbkg-(underflow+overflow);

  if (nentot<=0 || nenbkg<=0) return;

  Double_t fDtwin=fBurstParameters->GetSignal("Dtwin");
  Double_t ratetot=nentot/(fDtwin);
  Double_t ratebkg=nenbkg/(fDtwin);

  TF1 fdttot=math.PoissonDtDist(ratetot,ndt); // Only for reference, not used in the analysis
  TF1 fdtbkg=math.PoissonDtDist(ratebkg,ndt);

  nametot="hpdftotdt";
  nametot+=ndt;
  namebkg="hpdfbkgdt";
  namebkg+=ndt;
  TH1* hpdftotdt=(TH1*)fBurstHistos.FindObject(nametot.Data());
  TH1* hpdfbkgdt=(TH1*)fBurstHistos.FindObject(namebkg.Data());

  // Provide the dt PDFs as histograms in the output file
  if (!hpdftotdt && !hpdfbkgdt)
  {
   deltatmax=htotdt->GetXaxis()->GetXmax();
   fdttot.SetRange(0,deltatmax);
   fdttot.SetNpx(10000);
   hpdftotdt=(TH1*)fdttot.GetHistogram()->Clone();
   hpdftotdt->SetName(nametot.Data());
   deltatmax=hbkgdt->GetXaxis()->GetXmax();
   fdtbkg.SetRange(0,deltatmax);
   fdtbkg.SetNpx(10000);
   hpdfbkgdt=(TH1*)fdtbkg.GetHistogram()->Clone();
   hpdfbkgdt->SetName(namebkg.Data());
   fBurstHistos.Add(hpdftotdt);
   fBurstHistos.Add(hpdfbkgdt);

   cout << " The following arrival time interval (dt) PDFs have been generated :" << endl;
   cout << " ... " << hpdftotdt->GetName() << " : " << hpdftotdt->GetTitle() << endl;      
   cout << " ... " << hpdfbkgdt->GetName() << " : " << hpdfbkgdt->GetTitle() << endl;
  }

  chitot=math.Chi2Value(htotdt,0,&fdttot,&ndftot);
  chibkg=math.Chi2Value(hbkgdt,0,&fdtbkg,&ndfbkg);
 }

 // Listing of the statistics results
 Float_t chidif=chitot-chibkg;
 cout << " *** Observed Chi-squared values for the hypothesis of no burst signal ***" << endl;
 cout << " For the \"on source\" stacked patches : chi2 = " << chitot << " ndf = " << ndftot << endl;
 cout << " For the corresponding \"opposite RA\" stacked \"off source\" patches : chi2 = " << chibkg << " ndf = " << ndfbkg << endl;
 cout << " --> Difference between observed \"on source\" and \"off source\" chi2 values : " << chidif << endl;

 Float_t ptot=math.Chi2Pvalue(chitot,ndftot);
 Float_t sigmatot=math.Chi2Pvalue(chitot,ndftot,0,1);
 Float_t pbkg=math.Chi2Pvalue(chibkg,ndfbkg);
 Float_t sigmabkg=math.Chi2Pvalue(chibkg,ndfbkg,0,1);

 cout << " *** P-values of the observed \"on source\" and \"off source\" chi2 values ***" << endl;
 cout << " For the \"on source\"  stacked patches : P-value = " << ptot << " (" << sigmatot << " sigma)" << endl;
 cout << " For the \"off source\" stacked patches : P-value = " << pbkg << " (" << sigmabkg << " sigma)" << endl;
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
