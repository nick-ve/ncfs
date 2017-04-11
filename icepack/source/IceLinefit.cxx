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

// $Id: IceLinefit.cxx 72 2013-01-21 13:13:40Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class IceLinefit
// IceRecoBase derived TTask processor to perform a linefit track reconstruction.
//
// In case an event has been rejected by an NcEventSelector (based) processor,
// this task (and its sub-tasks) is not executed.
//
// Note : Amanda OMs, standard IceCube (IC), DeepCore (DC) and InIce (I) DOMs are treated
//        seperately, which means that for events with various OMs and/or DOMs firing,
//        several linefit tracks may be produced. The various linefit tracks can be
//        distinguished on basis of their name as explained below. 
//
// The procedure is based on the method described in the Amanda publication
// in Nuclear Instruments and Methods A524 (2004) 179-180.
// To prevent waisting CPU time in trying to reconstruct (high-energy) cascade
// events, or to select specifically reconstruction of low multiplicity events,
// the user may invoke the memberfunctions SetMaxMod() and SetMinMod().
// This allows selection of events for processing with a certain maximum
// and/or minimum number of good (D)OMs firing.
// By default the minimum and maximum are set to 0 and 999999, respectively,
// in the constructor, which implies no multiplicity selection. 
//
// The maximum number of good hits per (D)OM to be used for the reconstruction
// can be specified via the memberfunction SetMaxHits().
// By default the maximum number of hits per (D)OM is set to 0 for "A" and "I", which
// implies just processing all good Amanda and/or InIce hits without any maximum limit.
// However, the user may want to restrict this number to the first n hits of each (D)OM
// to account for possible noise and/or afterpulse signals that are not recognised by the
// hit cleaning procedure.
//
// The SetMaxHits() facility may also be used to select a specific detector
// configuration (e.g. "IC" and "DC" without "I") to be used for the reconstruction.
// This can be achieved by suppressing processing of e.g. the "I" configuration
// by setting the corresponding maximum number of hits to -1. 
// For the "IC" and "DC" configurations the default maximum number of hits per DOM
// is set to -1, which implies that no specific "IC" or "DC" linefit reconstructions will be made.
//
// The minimum number of associated hits for a reconstructed track to be produced 
// can be specified via the memberfunction SetMinAhits().
// By default the minimum number of associated hits is set to 0 for all reconstruction modes.
//
// The minimum number of associated modules for a reconstructed track to be produced 
// can be specified via the memberfunction SetMinAmods().
// By default the minimum number of associated modules is set to 0 for all reconstruction modes.
//
// The IceLinefit processor can also be applied to only the hits associated to a track from a
// previous (e.g. IceDwalk first guess) reconstruction.
// This mode is automatically activated at invokation of the member function UseTracks() to specify
// the names of the input tracks to be used.
// This so called "track based reconstruction" allows to obtain rather accurate results without
// the need of a hit cleaning procedure.
// In this "track based reconstruction" mode all the hits associated to the input track will be 
// used. However, for the final result a selection is made based on the settings of SetMinAhits() and SetMinAmods().
// The produced track names will look like for instance IceLinefit4DwalkIC when the input track was an IceDwalkIC track.
//
// The direction of a track is reversed in case a the track has a polar angle
// theta<"thetatrk" and the (time ordered) hit pattern reflects a direction with
// polar angle theta>"thetahits", where "thetatrk" and "thetahits" indicate angular thresholds.
// This allows an additional reduction of the background for upgoing tracks
// due to mis-reconstructed downgoing muons.
//
// The polar angle thresholds "thetatrk" and "thetahits" (in degrees) may be set by the user
// via the member function SetFlipAngles(), where the convention is that theta=0 indicates a
// straight upgoing track.
//
// Note : The flipping of the track direction may be de-activated by
//        setting thetatrk<0 and/or thetahits>180.
//
// The default values thetatrk=-999 and thetahits=999 are set in the constructor of this class
// which implies that the flipping of the tracks direction is de-activated by default.
// Based on simulated data it was seen that with thetatrk=90 and thetahits=100 the direction flipping
// did a good job in reducing downgoing Corsika events, but it eliminated too many upgoing signal events.
// Maybe further fine tuning can make this facility useful in the future.
//
// Information about the actual parameter settings can be found in the event
// structure itself via the device named "IceLinefit" (or "IceLinefit4Track").
//
// All reconstructed tracks will be stored in the IceEvent structure with as
// default the Classname of the producing processor as the basename of the track.
// A suffix "A" for an Amanda (OM) track, "IC" for a standard IceCube InIce (DOM) track,
// "I" for a hybrid InIce (DOM) track or "DC" for a DeepCore (DOM) track will be
// added to the name automatically in the case of complete event reconstruction.
// In case of a "track based reconstruction" the suffix will be for instance "4DwalkIC" as explained above.
// This track basename identifier can be modified by the user via the
// SetTrackName() memberfunction. This will allow unique identification
// of tracks which are produced when re-processing existing data with
// different criteria.
// Note that a suffix "A", "IC", "I", "DC", or "4..."  will always be generated automatically.
// The track 3-momentum is set to the reconstructed velocity direction, normalised
// to 1 GeV. The mass and charge of the track are left 0, since no distinction can
// be made between positive or negative tracks and also no mass can be determined.
// However, the user can define the track charge by invokation of the
// memberfunction SetCharge().
// This facility may be used to distinguish tracks produced by the
// various reconstruction algorithms in a (3D) colour display
// (see the class NcHelix for further details).  
// The r0 and t0 can be obtained from the reference point of the track,
// whereas the t0 ia also available from the track timestamp .
// The value of beta=v/c for the reconstructed velocity is available
// from the fitdetails as stored for the reconstructed track. 
//
// For further details the user is referred to NIM A524 (2004) 169.
//
// Note : This algorithm works best on data which has been calibrated and
//        cross talk corrected (Amanda).
//        Usage of data cleaned from noise hits etc. (see e.g. IceCleanHits)
//        will in general lead to more accurate results.
//
//--- Author: Nick van Eijndhoven 10-mar-2006 Utrecht University
//- Modified: NvE $Date: 2013-01-21 14:13:40 +0100 (Mon, 21 Jan 2013) $ NCFS
///////////////////////////////////////////////////////////////////////////
 
#include "IceLinefit.h"
#include "Riostream.h"

ClassImp(IceLinefit) // Class implementation to enable ROOT I/O

IceLinefit::IceLinefit(const char* name,const char* title) : IceRecoBase(name,title)
{
// Default constructor.

 SetCleaned(0,"A");
 SetCleaned(0,"I");
 SetCleaned(0,"IC");
 SetCleaned(0,"DC");

 SetMaxMod(999999,"A");
 SetMaxMod(999999,"I");
 SetMaxMod(999999,"IC");
 SetMaxMod(999999,"DC");

 SetMinMod(0,"A");
 SetMinMod(0,"I");
 SetMinMod(0,"IC");
 SetMinMod(0,"DC");

 SetMaxHits(0,"A");
 SetMaxHits(0,"I");
 SetMaxHits(-1,"IC");
 SetMaxHits(-1,"DC");

 SetMinAhits(0,"A");
 SetMinAhits(0,"I");
 SetMinAhits(0,"IC");
 SetMinAhits(0,"DC");

 SetMinAmods(0,"A");
 SetMinAmods(0,"I");
 SetMinAmods(0,"IC");
 SetMinAmods(0,"DC");

 SetSLChitUsage(0,"I");
 SetSLChitUsage(0,"IC");
 SetSLChitUsage(0,"DC");

 SetFlipAngles(-999,999);
}
///////////////////////////////////////////////////////////////////////////
IceLinefit::~IceLinefit()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
void IceLinefit::Exec(Option_t* opt)
{
// Implementation of the linefit reconstruction.

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
 if (!fUseNames) // Linefit procedure on complete event
 {
  fParams.SetNameTitle("IceLinefit","IceLinefit complete event reco parameters");
 }
 else // Linefit procedure on track associated hits
 {
  fParams.SetNameTitle("IceLinefit4Track","IceLinefit track based reco parameters");
 }

 // Add the fParams device to the IceEvent structure
 fEvt->AddDevice(fParams);

 // Printout information on used tracks (if any) at first startup of the processor task
 if (fUseNames && fFirst)
 {
  Int_t nclasses=0;
  if (fUseNames) nclasses=fUseNames->GetEntries(); // Number of first guess classes to be processed
  Int_t ntkmax=0; // Max. number of tracks for a certain class
  TObjString* strx=0;
  TString str;
  cout << " *IceLinefit* First guess selections to be processed (-1=all)." << endl;
  for (Int_t i=0; i<nclasses; i++)
  {
   strx=(TObjString*)fUseNames->At(i);
   if (!strx) continue;
   str=strx->GetString();
   ntkmax=fUseNtk->At(i);
   cout << " Maximally " << ntkmax << " track(s) per event for procedure : " << str.Data() << endl;
  }
  cout << endl;

  fFirst=0;
 }

 // Perform linefit reconstruction for the various (associated) hits

 if (!fUseNames) // Linefit procedure on complete event
 {
  Amanda();
  InIce();
  IceCube();
  DeepCore();
 }
 else // Linefit procedure on track associated hits
 {
  Tracks();
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t IceLinefit::Amanda()
{
// Performing the linefit complete event reconstruction for Amanda OMs.
// The return argument indicates whether (1) or not (0) a track has been found

 if (fMaxhitsA<0) return 0;

 // Fetch all fired Amanda OMs for this event
 TObjArray* aoms=fEvt->GetDevices("IceAOM");

 // Perform the reconstruction
 NcTrack* trk=Reconstruct(aoms,fCleanA,fMinmodA,fMaxmodA,fMaxhitsA,fMinahitsA,fMinamodsA,1);

 if (!trk) return 0;

 // Give the reconstructed track the proper name and title
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="A";
 TString title=ClassName();
 title+=" Amanda track";
 trk->SetNameTitle(name.Data(),title.Data());

 return 1;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceLinefit::InIce()
{
// Performing the linefit complete event reconstruction for all InIce DOMs.
// The return argument indicates whether (1) or not (0) a track has been found

 if (fMaxhitsI<0) return 0;

 // Fetch all fired InIce DOMs for this event
 TObjArray* doms=fEvt->GetDevices("IceIDOM");

 // Perform the reconstruction
 NcTrack* trk=Reconstruct(doms,fCleanI,fMinmodI,fMaxmodI,fMaxhitsI,fMinahitsI,fMinamodsI,fSlcI);

 if (!trk) return 0;

 // Give the reconstructed track the proper name and title
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="I";
 TString title=ClassName();
 title+=" InIce track";
 trk->SetNameTitle(name.Data(),title.Data());

 return 1;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceLinefit::IceCube()
{
// Performing the linefit complete event reconstruction for standard IceCube InIce DOMs.
// The return argument indicates whether (1) or not (0) a track has been found

 if (fMaxhitsIC<0) return 0;

 // Fetch all fired standard IceCube InIce DOMs for this event
 TObjArray* doms=fEvt->GetDevices("IceICDOM");

 // Perform the reconstruction
 NcTrack* trk=Reconstruct(doms,fCleanIC,fMinmodIC,fMaxmodIC,fMaxhitsIC,fMinahitsIC,fMinamodsIC,fSlcIC);

 if (!trk) return 0;

 // Give the reconstructed track the proper name and title
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="IC";
 TString title=ClassName();
 title+=" Standard IceCube InIce track";
 trk->SetNameTitle(name.Data(),title.Data());

 return 1;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceLinefit::DeepCore()
{
// Performing the linefit complete event reconstruction for DeepCore DOMs.
// The return argument indicates whether (1) or not (0) a track has been found

 if (fMaxhitsDC<0) return 0;

 // Fetch all fired DeepCore DOMs for this event
 TObjArray* doms=fEvt->GetDevices("IceDCDOM");

 // Perform the reconstruction
 NcTrack* trk=Reconstruct(doms,fCleanDC,fMinmodDC,fMaxmodDC,fMaxhitsDC,fMinahitsDC,fMinamodsDC,fSlcDC);

 if (!trk) return 0;

 // Give the reconstructed track the proper name and title
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="DC";
 TString title=ClassName();
 title+=" DeepCore track";
 trk->SetNameTitle(name.Data(),title.Data());

 return 1;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceLinefit::Tracks()
{
// Performing the linefit track based associated hit reconstruction.
// The return argument indicates whether (1) or not (0) a track has been found

 if (!fUseNames) return 0;

 Int_t nclasses=fUseNames->GetEntries(); // Number of first guess classes to be processed
 Int_t ntkmax=0; // Max. number of tracks for a certain class
 TObjString* strx=0;
 TString str;

 // Track by track processing of the selected first guess classes
 Int_t ntk=0;
 NcTrack* track=0;
 TObjArray* hits=0;
 Int_t trkflag=0;
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

   // Retrieval of the associated hits to be used in the reconstruction procedure
   hits=track->GetSignals("IceGOM",2);

   // Perform the reconstruction for this track
   NcTrack* trk=Reconstruct(hits,0,0,0,0,fMinahitsI,fMinamodsI,1);

   if (!trk) continue;

   // Give the reconstructed track the proper name and title
   trkflag=1;
   newname=fTrackname;
   if (newname=="") newname=ClassName();
   newname+="4";
   newname+=trackname;
   trk->SetNameTitle(newname.Data(),"Linefit reco for all assoc. hits");
   // Link this newly created track as a hypothesis to the parent first guess track
   track->SetHypCopy(0);
   track->AddTrackHypothesis(*trk);
  } // End of loop over the tracks of the current first guess class
 } // End of loop over first guess classes

 return trkflag;
}
///////////////////////////////////////////////////////////////////////////
NcTrack* IceLinefit::Reconstruct(TObjArray* arr,Int_t cln,Int_t minmod,Int_t maxmod,Int_t maxhits,Int_t minahits,Int_t minamods,Int_t slc)
{
// Implementation of the linefit reconstruction algorithm.

 if (maxhits<0) return 0;

 if (!arr) return 0;
 Int_t narr=arr->GetEntries();
 if (narr<=0) return 0;

 if (!fUseNames) // Complete event reco via provided array of (D)OMs
 {
  // Check for the minimum and/or maximum number of good fired DOMs
  Int_t ngood=0;
  for (Int_t iarr=0; iarr<narr; iarr++)
  {
   IceGOM* omx=(IceGOM*)arr->At(iarr);
   if (!omx) continue;
   if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
   ngood++;
  } 
  if (ngood<minmod || ngood>maxmod) return 0;
 }

 const Float_t c=0.299792; // Light speed in vacuum in meters per ns

 IceGOM* omx=0;
 NcSignal* sx=0;
 Nc3Vector rom,sumr;
 Nc3Vector rt,sumrt;
 Float_t thit;
 Float_t sumt=0,sumt2=0;
 TObjArray hits;
 TObjArray* ordered=0;
 Int_t nhitloop=0;
 Int_t nh;

 // Loop over all DOMs and/or hits to determine the linefit parameters.
 // Also all the used hits are recorded for association with newly created reco track.
 for (Int_t iarr=0; iarr<narr; iarr++)
 {
  if (!fUseNames) // Complete event reco via provided array of (D)OMs
  {
   omx=(IceGOM*)arr->At(iarr);
   if (!omx) continue;
   if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
   // Use the specified good hits of this (D)OM
   ordered=0;
   if (maxhits>0 && omx->GetNhits()>maxhits)
   {
    if (omx->InheritsFrom("IceAOM")) ordered=omx->SortHits("LE",1,0,7);
    if (omx->InheritsFrom("IceDOM")) ordered=omx->SortHits("ADC",-1,0,7);
   }
   nhitloop=omx->GetNhits();
  }
  else // Track based reco via provided array of associated hits
  {
   nhitloop=1;
  }

  // Loop over all the hits of the current (D)OM.
  // In case an associated hit array was provided, only a single pass is made.
  nh=0;
  for (Int_t ih=1; ih<=nhitloop; ih++)
  {
   if (ordered) // Only take the first "maxhits" ordered hits for this (D)OM 
   {
    if (nh>=maxhits) break;
    sx=(NcSignal*)ordered->At(ih-1);
   }
   else // Take all available hits
   {
    if (!fUseNames) // Obtain the hits from the provided (D)OMs
    {
     sx=omx->GetHit(ih);
    }
    else // Obtain the hits directly from the provided array
    {
     sx=(NcSignal*)arr->At(iarr);
     omx=(IceGOM*)sx->GetDevice();
    }
   }

   if (!omx || !sx) continue;

   if (cln && (sx->GetDeadValue("ADC") || sx->GetDeadValue("LE") || sx->GetDeadValue("TOT"))) continue;
   if (!slc && sx->GetSignal("SLC")) continue; 

   thit=sx->GetSignal("LE",7);
   rom=(Nc3Vector)omx->GetPosition();
   rt=rom*thit;
   sumr+=rom;
   sumrt+=rt;
   sumt+=thit;
   sumt2+=thit*thit;

   // Record this hit for association with the track
   hits.Add(sx);
   nh++;
  }
 }

 Int_t nused=hits.GetEntries();
 if (!nused || nused<minahits) return 0;

 Int_t nasmods=fEvt->GetNdevices("IceGOM",&hits);
 if (nasmods<minamods) return 0;

 sumr/=float(nused);
 sumrt/=float(nused);
 sumt/=float(nused);
 sumt2/=float(nused);

 Nc3Vector v;
 Nc3Vector temp;
 temp=sumr*sumt;
 v=sumrt-temp;
 Float_t dum=sumt2-(sumt*sumt);
 if (dum) v/=dum;

 Float_t beta=v.GetNorm()/c;
 NcSignal fitstats;
 fitstats.SetNameTitle("Fitstats","Fit stats for IceLinefit");
 fitstats.AddNamedSlot("Beta");
 fitstats.SetSignal(beta,"Beta");

 Nc3Vector r;
 temp=v*sumt;
 r=sumr-temp;

 NcTrack t; 
 t.SetCharge(fCharge);
 fEvt->AddTrack(t);
 NcTrack* trk=fEvt->GetTrack(fEvt->GetNtracks());
 if (!trk) return 0;

 trk->SetId(fEvt->GetNtracks(1)+1);

 Nc3Vector p;
 Float_t vec[3];
 v.GetVector(vec,"sph");
 vec[0]=1;
 p.SetVector(vec,"sph");

 NcPosition r0;
 r0.SetPosition(r);
 r0.SetTimestamp((NcTimestamp&)*fEvt);
 NcTimestamp* t0=r0.GetTimestamp();
 t0->Add(0,0,(int)sumt);

 trk->Set3Momentum(p);
 trk->SetReferencePoint(r0);
 trk->SetTimestamp(*t0);
 trk->SetFitDetails(fitstats);

 // Link the used hits to the track (and vice versa)
 for (Int_t i=0; i<nused; i++)
 {
  sx=(NcSignal*)hits.At(i);
  if (sx) sx->AddTrack(*trk);
 }

 // Check whether the track direction should be flipped
 FlipTrack(trk);
 
 return trk; 
}
///////////////////////////////////////////////////////////////////////////
