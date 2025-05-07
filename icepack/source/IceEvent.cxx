/*******************************************************************************
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
 *******************************************************************************/

///////////////////////////////////////////////////////////////////////////
// Class IceEvent
// Handling of IceCube event data.
// Basically this class provides an IceCube tailored user interface
// to the functionality of the class NcEvent.
//
// Notes :
// ------
// * In the event structure MC tracks are labeled with a negative track ID,
//   whereas reconstructed tracks are labeled with a positive track ID.
//   This allows for a direct selection of either MC or Reco tracks via the
//   GetIdTrack() facility.
// * The particle codes used for the various tracks are the PDG ones.
//   For IceCube specific "particle" types (e.g. deltae) the PDG database
//   has been extended, as can be seen in the IceF2k class which provides
//   a conversion facility from the F2K into the Ralice/IcePack format.
//
// Examples :
// ==========
//
// Creation and filling of an event with some fictitious module data
// -----------------------------------------------------------------
// IceEvent* evt=new IceEvent();
// evt->SetOwner();
//
// // The starting unique signal ID.
// // In this example it will be increased everytime
// // when a new signal is created.
// Int_t sid=1;
//
// // Amanda module
// IceAOM m;
// m.SetUniqueID(123);
// m.SetNameTitle("OM123","Amanda module");
//
// Float_t pos[3]={1,2,3};
// m.SetPosition(pos,"car");
//
// NcSignal s;
//
// s.SetSlotName("ADC",1);
// s.SetSlotName("LE",2);
// s.SetSlotName("TOT",3);
//
// s.Reset();
// s.SetName("OM123 Hit 1");
// s.SetUniqueID(sid++);
// s.SetSignal(100,"ADC");
// s.SetSignal(-100,"LE");
// s.SetSignal(-1000,"TOT");
// m.AddHit(s);
//
// s.Reset();
// s.SetName("OM123 Hit 2");
// s.SetUniqueID(sid++);
// s.SetSignal(110,"ADC");
// s.SetSignal(-101,"LE");
// s.SetSignal(1001,"TOT");
// m.AddHit(s);
//
// s.Reset();
// s.SetName("OM123 Hit 3");
// s.SetUniqueID(sid++);
// s.SetSignal(120,"ADC");
// s.SetSignal(-102,"LE");
// s.SetSignal(-1002,"TOT");
// m.AddHit(s);
//
// evt->AddDevice(m);
//
// m.Reset();
// m.SetUniqueID(456);
// m.SetName("OM456");
//
// pos[0]=-4;
// pos[1]=-5;
// pos[2]=-6;
// m.SetPosition(pos,"car");
//
// s.Reset();
// s.SetName("OM456 Hit 1");
// s.SetUniqueID(sid++);
// s.SetSignal(20,"ADC");
// s.SetSignal(-200,"LE");
// s.SetSignal(-2000,"TOT");
// m.AddHit(s);
//
// s.Reset();
// s.SetName("OM456 Hit 2");
// s.SetUniqueID(sid++);
// s.SetSignal(21,"ADC");
// s.SetSignal(-201,"LE");
// s.SetSignal(2001,"TOT");
// m.AddHit(s);
//
// s.Reset();
// s.SetName("OM456 Hit 3");
// s.SetUniqueID(sid++);
// s.SetSignal(22,"ADC");
// s.SetSignal(-202,"LE");
// s.SetSignal(-2002,"TOT");
// m.AddHit(s);
//
// evt->AddDevice(m);
//
// // IceCube in-ice DOM
// IceIDOM mid;
// mid.SetUniqueID(958);
// mid.SetNameTitle("OM958","IceCube in-ice module");
//
// pos[0]=9;
// pos[1]=5;
// pos[2]=8;
// mid.SetPosition(pos,"car");
//
// s.Reset();
// s.SetName("OM958 Hit 1");
// s.SetUniqueID(sid++);
// s.SetSignal(40,"ADC");
// s.SetSignal(-400,"LE");
// s.SetSignal(-4000,"TOT");
// mid.AddHit(s);
//
// s.Reset();
// s.SetName("OM958 Hit 2");
// s.SetUniqueID(sid++);
// s.SetSignal(41,"ADC");
// s.SetSignal(-401,"LE");
// s.SetSignal(4001,"TOT");
// mid.AddHit(s);
//
// s.Reset();
// s.SetName("OM958 Hit 3");
// s.SetUniqueID(sid++);
// s.SetSignal(42,"ADC");
// s.SetSignal(-402,"LE");
// s.SetSignal(-4002,"TOT");
// mid.AddHit(s);
//
// evt->AddDevice(mid);
//
// // IceTop DOM
// IceTDOM mtd;
// mtd.SetUniqueID(4958);
// mtd.SetNameTitle("OM4958","IceTop module");
//
// pos[0]=49;
// pos[1]=5;
// pos[2]=8;
// mtd.SetPosition(pos,"car");
//
// s.Reset();
// s.SetName("OM4958 Hit 1");
// s.SetUniqueID(sid++);
// s.SetSignal(50,"ADC");
// s.SetSignal(-500,"LE");
// s.SetSignal(-5000,"TOT");
// mtd.AddHit(s);
//
// s.Reset();
// s.SetName("OM4958 Hit 2");
// s.SetUniqueID(sid++);
// s.SetSignal(51,"ADC");
// s.SetSignal(-501,"LE");
// s.SetSignal(5001,"TOT");
// mtd.AddHit(s);
//
// s.Reset();
// s.SetName("OM4958 Hit 3");
// s.SetUniqueID(sid++);
// s.SetSignal(52,"ADC");
// s.SetSignal(-502,"LE");
// s.SetSignal(-5002,"TOT");
// mtd.AddHit(s);
//
// evt->AddDevice(mtd);
//
// Investigation of the event contents
// -----------------------------------
// // Provide event data overview
// evt->Data();
//
// // Select a specific device (i.e. OM) from the event
// NcDevice* dx=(NcDevice*)evt->GetIdDevice(958);
// if (dx) dx->Data();
//
// // Select a specific hit from the event
// NcSignal* sx=evt->GetIdHit(5,"IceGOM");
// if (sx) sx->Data();
//
// // Dump all the information for the various stored devices
// Int_t ndev=evt->GetNdevices();
// for (Int_t idev=1; idev<=ndev; idev++)
// {
//  IceGOM* om=(IceGOM*)evt->GetDevice(idev);
//  if (om) om->Data();
// }
//
// // Dump all the information for the various stored hits
// // Obtain pointers to the hits for all generic OM's (i.e. IceGOM)
// TObjArray* hits=evt->GetHits("IceGOM");
// Int_t nhits=0;
// if (hits) nhits=hits->GetEntries();
// for (Int_t ih=0; ih<nhits; ih++)
// {
//  NcSignal* sx=(NcSignal*)hits->At(ih);
//  if (sx) sx->Data();
// }
//
// // Obtain the minimum and maximum recorded TOT value 
// Float_t vmin,vmax;
// evt->GetExtremes("IceGOM",vmin,vmax,"TOT");
// cout << " Extreme values : vmin = " << vmin << " vmax = " << vmax << endl;
//
// Some simple module and hit manipulations
// ----------------------------------------
// // Ordered hits w.r.t. decreasing TOT
// TObjArray* ordered=evt->SortHits("IceGOM","TOT",-1);
// nhits=0;
// if (ordered) nhits=ordered->GetEntries();
// for (Int_t i=0; i<nhits; i++)
// {
//  NcSignal* sx=(NcSignal*)ordered->At(i);
//  if (sx) sx->Data();
// }
//
// // Ordered devices from the already ordered hit array
// TObjArray* devs=evt->SortDevices(ordered,0,0);
// ndev=0;
// if (devs) ndev=devs->GetEntries();
// for (Int_t id=0; id<ndev; id++)
// {
//  NcDevice* dx=(NcDevice*)devs->At(id);
//  if (dx) dx->Data();
// }
//
// // Newly ordered devices w.r.t. decreasing ADC
// TObjArray* devs=evt->SortDevices("IceGOM","ADC",-1);
// ndev=0;
// if (devs) ndev=devs->GetEntries();
// for (Int_t id2=0; id2<ndev; id2++)
// {
//  NcDevice* dx=(NcDevice*)devs->At(id2);
//  if (dx) dx->Data();
// }
//
// A simple 3D event display of the modules
// ----------------------------------------
// TCanvas* c1=new TCanvas("c1","c1");
// c1->x3d();
// TView* view=new TView(1);
// view->SetRange(-50,-50,-50,50,50,50);
// view->ShowAxis();
//
// evt->DisplayHits("IceGOM","TOT",1e4,1);
//
//
//--- Author: Nick van Eijndhoven 23-jun-2004 Utrecht University
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, May 7, 2025  07:29Z
///////////////////////////////////////////////////////////////////////////

#include "IceEvent.h"
#include "Riostream.h"
 
ClassImp(IceEvent) // Class implementation to enable ROOT I/O
 
IceEvent::IceEvent() : NcEvent()
{
// Default constructor.

 fStrings=0;
}
///////////////////////////////////////////////////////////////////////////
IceEvent::~IceEvent()
{
// Default destructor.

 if (fStrings)
 {
  delete fStrings;
  fStrings=0;
 }
}
///////////////////////////////////////////////////////////////////////////
IceEvent::IceEvent(const IceEvent& evt) : NcEvent(evt)
{
// Copy constructor.

 fStrings=0;
}
///////////////////////////////////////////////////////////////////////////
void IceEvent::Reset()
{
// Reset all values.

 NcEvent::Reset();

 if (fStrings)
 {
  delete fStrings;
  fStrings=0;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t IceEvent::GetNstrings(TString classname)
{
// Provide the number of fired strings of good modules of the specified classname for this event.

 // Fetch all fired modules of the specified class for this event
 TObjArray* mods=GetDevices(classname);
 if (!mods) return 0;
 Int_t nmods=mods->GetEntries();
 if (!nmods) return 0;

 // Check for the string ids of the good fired modules
 if (!fStrings) fStrings=new TArrayI(200);
 fStrings->Reset();
 Int_t idxmax=0;
 Int_t nstrings=0;
 Int_t jstring=0;
 Int_t match=0;
 for (Int_t imod=0; imod<nmods; imod++)
 {
  IceGOM* omx=(IceGOM*)mods->At(imod);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;

  // Update the number of fired strings
  jstring=omx->GetString();
  match=0;
  idxmax=nstrings;
  for (Int_t idx=0; idx<idxmax; idx++)
  {
   if (jstring==fStrings->At(idx))
   {
    match=1;
    break;
   }
  } 
  if (!match) // String number was not present in array
  {
   nstrings++;
   fStrings->AddAt(jstring,nstrings-1);
  }
 }

 return nstrings; 
}
///////////////////////////////////////////////////////////////////////////
Int_t IceEvent::GetNstrings(NcTrack& t,TString classname)
{
// Provide the number of fired strings of modules of the specified classname,
// associated with the specified track.

 // Fetch all associated signals for this track
 Int_t nh=t.GetNsignals();
 if (!nh) return 0;

 // Check for the string ids of the associated good fired modules of the specified class
 if (!fStrings) fStrings=new TArrayI(200);
 fStrings->Reset();
 Int_t idxmax=0;
 Int_t nstrings=0;
 Int_t jstring=0;
 Int_t match=0;
 for (Int_t ih=1; ih<=nh; ih++)
 {
  NcSignal* sx=t.GetSignal(ih);
  if (!sx) continue;
  NcDevice* dev=sx->GetDevice();
  if (!dev) continue;
  if (!(dev->InheritsFrom(classname.Data()))) continue;
  IceGOM* omx=(IceGOM*)dev;

  // Update the number of fired strings
  jstring=omx->GetString();
  match=0;
  idxmax=nstrings;
  for (Int_t idx=0; idx<idxmax; idx++)
  {
   if (jstring==fStrings->At(idx))
   {
    match=1;
    break;
   }
  } 
  if (!match) // String number was not present in array
  {
   nstrings++;
   fStrings->AddAt(jstring,nstrings-1);
  }
 }

 return nstrings; 
}
///////////////////////////////////////////////////////////////////////////
Int_t IceEvent::GetNstrings(NcJet& j,TString classname)
{
// Provide the number of fired strings of modules of the specified classname,
// associated with the specified jet.

 Int_t ntk=j.GetNtracks();
 if (!ntk) return 0;

 if (!fStrings) fStrings=new TArrayI(200);
 fStrings->Reset();
 Int_t idxmax=0;
 Int_t nstrings=0;
 Int_t jstring=0;
 Int_t match=0;
 Int_t nh=0;
 for (Int_t itk=1; itk<=ntk; itk++)
 {
  NcTrack* tx=j.GetTrack(itk);
  if (!tx) continue;

  // Fetch all associated signals for this track
  nh=tx->GetNsignals();
  if (!nh) return 0;

  // Check for the string ids of the associated good fired modules of the specified class
  for (Int_t ih=1; ih<=nh; ih++)
  {
   NcSignal* sx=tx->GetSignal(ih);
   if (!sx) continue;
   NcDevice* dev=sx->GetDevice();
   if (!dev) continue;
   if (!(dev->InheritsFrom(classname.Data()))) continue;
   IceGOM* omx=(IceGOM*)dev;

   // Update the number of fired strings
   jstring=omx->GetString();
   match=0;
   idxmax=nstrings;
   for (Int_t idx=0; idx<idxmax; idx++)
   {
    if (jstring==fStrings->At(idx))
    {
     match=1;
     break;
    }
   } 
   if (!match) // String number was not present in array
   {
    nstrings++;
    fStrings->AddAt(jstring,nstrings-1);
   }
  } // End of loop over the track hits
 } // End of loop over the tracks

 return nstrings; 
}
///////////////////////////////////////////////////////////////////////////
Int_t IceEvent::GetNmodules(NcTrack& t,TString classname)
{
// Provide the number of fired (D)OMs of the specified classname,
// associated with the specified track.

 TObjArray* hits=t.GetSignals(classname,1);
 if (!hits) return 0;

 Int_t nmods=GetNdevices(classname.Data(),hits);
 return nmods;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceEvent::GetNmodules(NcJet& j,TString classname)
{
// Provide the number of fired (D)OMs of the specified classname,
// associated with the specified track.

 TObjArray* hits=j.GetSignals(classname,1);
 if (!hits) return 0;

 Int_t nmods=GetNdevices(classname.Data(),hits);
 return nmods;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceEvent::GetStringMax(TString classname,Int_t* id,Float_t* x,Float_t* y)
{
// Provide the max. number of fired good (D)OMs of the specified classname at a single string.
// In case the arguments "id", "x" or "y" are specified, the following information
// is returned via the corresponding pointer :
//
// id : The ID of the string that had the maximum number of firing good (D)OMs
// x  : The X coordinate (in m) of the string that had the maximum number of firing good (D)OMs
// y  : The Y coordinate (in m) of the string that had the maximum number of firing good (D)OMs
//
// By default id=0, x=0 and y=0 which implies that no info is returned.

 // Reset the values of the return arguments
 if (id) *id=0;
 if (x) *x=0;
 if (y) *y=0;

 // Fetch all fired modules of the specified class for this event
 TObjArray* mods=GetDevices(classname);
 if (!mods) return 0;
 Int_t nmods=mods->GetEntries();
 if (!nmods) return 0;

 // Array for the string ids of the good fired modules
 if (!fStrings) fStrings=new TArrayI(200);
 fStrings->Reset();

 Int_t ncount=0;
 Int_t nmax=0;
 Int_t idmax=0;
 Float_t xmax=0;
 Float_t ymax=0;
 Float_t scale=0;
 Int_t jstring=0;
 for (Int_t imod=0; imod<nmods; imod++)
 {
  IceGOM* omx=(IceGOM*)mods->At(imod);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;

  // Update the count for the corresponding string
  jstring=omx->GetString();
  ncount=fStrings->At(jstring);
  ncount++;
  fStrings->AddAt(ncount,jstring);
  if (ncount>nmax)
  {
   idmax=jstring;
   scale=omx->GetUnitScale();
   xmax=float(omx->GetX(1,"car"))*scale;
   ymax=float(omx->GetX(2,"car"))*scale;
   nmax=ncount;
   fStrings->AddAt(nmax,0);
  }
 }

 if (id) *id=idmax;
 if (x) *x=xmax;
 if (y) *y=ymax;

 return nmax; 
}
///////////////////////////////////////////////////////////////////////////
Float_t IceEvent::GetTriggerTime(TString trigname,TObjArray* arr,Int_t slc,TArrayF* peaks) const
{
// Determination of the requested trigger time.
//
// Input arguments :
// -----------------
// trigname : The name of the requested trigger.
// arr      : Array containing optical modules or hits to determine the median of recorded hit times.
// slc      : Flag to denote to use (1) or not use (0) SLC hits for the trigger time determination
// peaks    : Array with the hit time values where hit clusters (=peaks) were found in the hit time distr. 
//
// Notes :
// -------
// 1) Specification of the trigger name "Average" will provide the median of all the recorded
//    trigger times for the current event (provided no array "arr" is specified as explained below).
//    In the determination of this median value the GLOBAL triggers are not taken into account.
// 2) In case an array "arr" is specified, the trigger time will be taken as the median of all
//    the recorded hit times.
//    When an array "arr" is specified, the specified "trigname" is irrelevant.
// 3) The array "arr" may either contain optical modules or hits, but NOT a mix of both.
// 4) In case an array pointer "peaks" is specified in addition to an array "arr", the hit time distribution
//    will be analysed for hit clusters (=peaks). The hit times corresponding to the peaks are returned via
//    this array "peaks". The number of detected peaks may be obtained via peaks->GetSize(). 
//
// Default : arr=0, slc=0 and peaks=0.
//
// In case of missing information a trigger time value of 0 will be returned.

 NcSample times;
 times.SetStoreMode(1);
 TString tname;
 Float_t ttime=0;
 NcSignal* sx=0;
 IceGOM* omx=0;
 TObjArray* oms=0;
 TObjArray* hits=0;
 Float_t time=0;
 Float_t tfirst=999999;
 Float_t tlast=-999999;
 Float_t tlow=0;
 Float_t tup=0;
 Float_t dtbin=500; // Bin size in ns for peak search histo
 Int_t nbins=0;

 // Detect from inheritance whether "arr" contains optical modules or hits
 if (arr)
 {
  TObject* obj=arr->At(0);
  if (obj)
  {
   if (obj->InheritsFrom("IceGOM"))
   {
    oms=arr;
   }
   else
   {
    hits=arr;
   }
  }
 }

 if (!arr) // Trigger time derived from trigger data
 {
  NcDevice* tdev=(NcDevice*)GetDevice("Trigger");
  if (!tdev) return 0;

  if (trigname=="Average") // Take median triggertime
  {
   for (Int_t itrig=1; itrig<=tdev->GetNhits(); itrig++)
   {
    NcSignal* trig=tdev->GetHit(itrig);
    ttime=0;
    if (trig)
    {
     // Don't take the GLOBAL triggers into account.
     // They are just there for administrative purposes.
     tname=trig->GetName();
     if (tname.Contains("GLOBAL")) continue;

     ttime=trig->GetSignal("trig_pulse_le");
     times.Enter(ttime);
    } 
   }
  ttime=times.GetMedian(1); 
  }
  else // Take triggertime from the specified trigger
  {
   NcSignal* trig=tdev->GetHit(trigname);
   if (trig) ttime=trig->GetSignal("trig_pulse_le");
  }
 }

 if (oms) // Trigger time from array of optical modules via (median of) recorded hit times
 {
  for (Int_t iom=0; iom<oms->GetEntries(); iom++)
  {
   omx=(IceGOM*)oms->At(iom);
   if (!omx) continue;
   for (Int_t ih=1; ih<=omx->GetNhits(); ih++)
   {
    sx=omx->GetHit(ih);
    if (!sx) continue;
    if (sx->GetDeadValue("LE")) continue;

    ttime=sx->GetSignal("LE",7);
    if (ttime<tfirst) tfirst=ttime;
    if (ttime>tlast) tlast=ttime;

    if (!slc && sx->GetSignal("SLC")>0.5) continue;

    times.Enter(ttime);
   }
  }
  ttime=times.GetMedian(1);
 }

 if (hits) // Trigger time from array of hits via (median of) recorded hit times
 {
  for (Int_t ih=0; ih<hits->GetEntries(); ih++)
  {
   sx=(NcSignal*)hits->At(ih);
   if (!sx) continue;
   if (sx->GetDeadValue("LE")) continue;

   ttime=sx->GetSignal("LE",7);
   if (ttime<tfirst) tfirst=ttime;
   if (ttime>tlast) tlast=ttime;

   if (!slc && sx->GetSignal("SLC")>0.5) continue;

   times.Enter(ttime);
  }
  ttime=times.GetMedian(1);
 }

 if (peaks)
 {
  peaks->Set(1);
  peaks->AddAt(ttime,0);
 }

 Int_t ntimes=times.GetN();
 if (peaks && tlast>tfirst && ntimes>1)
 {
  tlow=tfirst-dtbin;
  tup=tlast+dtbin;
  nbins=int((tup-tlow)/dtbin);
  TH1F thist("thist","hit times",nbins,tlow,tup);
  for (Int_t it=1; it<=ntimes; it++)
  {
   time=times.GetEntry(it,1);
   thist.Fill(time);
  }
// Next line modified because of an incompatible change in ROOT6
//@@@  TSpectrum spec;
  NcSpectrum spec;
  spec.SetDeconIterations(100);
  Int_t oldlevel=gErrorIgnoreLevel;
  gErrorIgnoreLevel=kFatal; // Suppress all (TSpectrum c.q. NcSpectrum) error and warning messages
  Int_t npeaks=spec.Search(&thist,1,"goff",0.3);
  gErrorIgnoreLevel=oldlevel; // Re-activate previous info level
  if (npeaks)
  {
   peaks->Set(npeaks);
   Float_t* specarr=spec.GetPositionX();
   for (Int_t ip=0; ip<npeaks; ip++)
   {
    peaks->AddAt(specarr[ip],ip);
   }
  }
 }

 return ttime;
}
///////////////////////////////////////////////////////////////////////////
TObject* IceEvent::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects. This feature allows e.g. NcEvent
// to store either IceEvent objects or objects derived from IceEvent
// via tha AddDevice memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 

 IceEvent* evt=new IceEvent(*this);
 if (name)
 {
  if (strlen(name)) evt->SetName(name);
 }
 return evt;
}
///////////////////////////////////////////////////////////////////////////
