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

// $Id: IceRecoBase.cxx 83 2013-03-15 15:22:45Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class IceRecoBase
// TTask derived base class for the various IceCube reconstruction tasks.
//
// This base class provides a generic facility to specify reconstruction
// parameters which are common to various IceCube reconstruction tasks.
// As such, each IceCube reconstruction task is expected to be derived from
// this base class and should provide the code for the "Exec()" member function,
// which performs the actual reconstruction method.
// The "Exec()" memberfunction of this base class may serve as a template. 
//
// Special data members :
// ----------------------
//
// fEvt      : Pointer to the IceEvent structure
// fParams   : Device to contain info on the used reconstruction parameters
// fFirst    : Flag (if set to 1) denoting the very first invokation of the processor 
// fUseNames : Array with the names of the tracks to be used as input for the reconstruction
// FuseNtk   : Array with the maximum number of tracks of each name to be used 
//
// If needed, additional parameters can of course be specified in the derived class.
// These additional parameters have then to be added to the "fParams" device in order
// to be introduced into the event structure for further reference.
// Please refer to one of the "Set" member functions for an example.
//
// The default values set in this base class will not impose any restrictions
// on the data to be processed.
// Note that in this base class only the parameter values are set and that no selection
// is performed on the data. The latter has to be implemented in the actual reconstruction
// code of the derived class by making use of the parameter variables defined here.
// In a derived class the default settings have to be specified by invokation of
// the corresponding "Set" functions, via which also automatically the fParams device
// is filled with data.
// Note that the default constructor of this base class sets the values of the various
// reconstruction parameters directly via the data members.
// This implies that the default constructor of this base class results in an empty fParams device.
// This allows to trace omitted parameter settings in a derived class.   
//
//--- Author: Nick van Eijndhoven 18-jan-2013 IIHE-VUB, Brussels
//- Modified: NvE $Date: 2013-03-15 16:22:45 +0100 (Fri, 15 Mar 2013) $ IIHE-VUB
///////////////////////////////////////////////////////////////////////////
 
#include "IceRecoBase.h"
#include "Riostream.h"

ClassImp(IceRecoBase) // Class implementation to enable ROOT I/O

IceRecoBase::IceRecoBase(const char* name,const char* title) : TTask(name,title)
{
// Default constructor.
 fFirst=1;
 fEvt=0;
 fCleanA=0;
 fMaxmodA=999999;
 fMinmodA=0;
 fMaxhitsA=0;
 fSingleA=0;
 fMinahitsA=0;
 fMinamodsA=0;
 fCleanI=0;
 fMaxmodI=999999;
 fMinmodI=0;
 fMaxhitsI=0;
 fSingleI=0;
 fMinahitsI=0;
 fMinamodsI=0;
 fCleanIC=0;
 fMaxmodIC=999999;
 fMinmodIC=0;
 fMaxhitsIC=0;
 fSingleIC=0;
 fMinahitsIC=0;
 fMinamodsIC=0;
 fCleanDC=0;
 fMaxmodDC=999999;
 fMinmodDC=0;
 fMaxhitsDC=0;
 fSingleDC=0;
 fMinahitsDC=0;
 fMinamodsDC=0;
 fSlcI=1;
 fSlcIC=1;
 fSlcDC=1;
 fThetatrk=-999;
 fThetahits=999;
 fLambdaA=33.3;
 fLambdaUD=30;
 fLambdaDL=5;
 fLambdaLD=40;
 fLabsA=50;
 fLabsUD=100;
 fLabsDL=10;
 fLabsLD=150;
 fTsigmaA=10;
 fTsigmaIC=5;
 fTsigmaDC=5;
 fTrackname="";
 fCharge=0;
 fUseNames=0;
 fUseNtk=0;
}
///////////////////////////////////////////////////////////////////////////
IceRecoBase::~IceRecoBase()
{
// Default destructor.

 if (fUseNames)
 {
  delete fUseNames;
  fUseNames=0;
 }
 if (fUseNtk)
 {
  delete fUseNtk;
  fUseNtk=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetCleaned(Int_t flag,TString s)
{
// Set flag to select usage of only cleaned hits or all hits.
//
// flag = 0 : All hits are used
//        1 : Only cleaned hits are used
//
// For default values please refer to the general documentation of this class.
//
// The input argument "s" allows specification for the various reconstructions.
//
// s = "A"  --> Amanda reconstruction
//     "I"  --> Hybrid InIce reconstruction
//     "IC" --> Standard IceCube reconstruction
//     "DC" --> DeepCore reconstruction

 if (s=="A")
 {
  fCleanA=flag;
  fParams.AddNamedSlot("CleanA");
  fParams.SetSignal(fCleanA,"CleanA");
 }
 if (s=="I")
 {
  fCleanI=flag;
  fParams.AddNamedSlot("CleanI");
  fParams.SetSignal(fCleanI,"CleanI");
 }
 if (s=="IC")
 {
  fCleanIC=flag;
  fParams.AddNamedSlot("CleanIC");
  fParams.SetSignal(fCleanIC,"CleanIC");
 }
 if (s=="DC")
 {
  fCleanDC=flag;
  fParams.AddNamedSlot("CleanDC");
  fParams.SetSignal(fCleanDC,"CleanDC");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetMaxMod(Int_t nmax,TString s)
{
// Set the maximum number of good (D)OMs that may have fired
// in order to process this event.
// This allows suppression of processing (high-energy) cascade events
// with e.g. track reconstruction to prevent waisting cpu time for cases
// in which tracking doesn't make sense anyhow.
// Furthermore it allows selection of low multiplicity events for processing.
// For default values please refer to the general documentation of this class.
// See also the memberfunction SetMinMod().
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs

 if (s=="A")
 {
  fMaxmodA=nmax;
  fParams.AddNamedSlot("MaxmodA");
  fParams.SetSignal(fMaxmodA,"MaxmodA");
 }
 if (s=="I")
 {
  fMaxmodI=nmax;
  fParams.AddNamedSlot("MaxmodI");
  fParams.SetSignal(fMaxmodI,"MaxmodI");
 }
 if (s=="IC")
 {
  fMaxmodIC=nmax;
  fParams.AddNamedSlot("MaxmodIC");
  fParams.SetSignal(fMaxmodIC,"MaxmodIC");
 }
 if (s=="DC")
 {
  fMaxmodDC=nmax;
  fParams.AddNamedSlot("MaxmodDC");
  fParams.SetSignal(fMaxmodDC,"MaxmodDC");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetMinMod(Int_t nmin,TString s)
{
// Set the minimum number of good (D)OMs that must have fired
// in order to process this event.
// This allows selection of a minimal multiplicity for events to be processed.
// For default values please refer to the general documentation of this class.
// See also the memberfunction SetMaxMod().
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs

 if (s=="A")
 {
  fMinmodA=nmin;
  fParams.AddNamedSlot("MinmodA");
  fParams.SetSignal(fMinmodA,"MinmodA");
 }
 if (s=="I")
 {
  fMinmodI=nmin;
  fParams.AddNamedSlot("MinmodI");
  fParams.SetSignal(fMinmodI,"MinmodI");
 }
 if (s=="IC")
 {
  fMinmodIC=nmin;
  fParams.AddNamedSlot("MinmodIC");
  fParams.SetSignal(fMinmodIC,"MinmodIC");
 }
 if (s=="DC")
 {
  fMinmodDC=nmin;
  fParams.AddNamedSlot("MinmodDC");
  fParams.SetSignal(fMinmodDC,"MinmodDC");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetMaxHits(Int_t nmax,TString s)
{
// Set the maximum number of good hits per (D)OM to be processed.
//
// Special values :
// nmax = 0 : No maximum limit set; all good hits will be processed
//      < 0 : No hits will be processed
//
// In case the user selects a maximum number of good hits per module, all the
// hits of each Amanda OM will be ordered w.r.t. increasing hit time (LE),
// whereas the hits of each InIce DOM will be ordered w.r.t. decreasing amplitude (ADC).
// This allows selection of processing e.g. only the first c.q. largest hits etc...
// For default values please refer to the general documentation of this class.
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs

 if (s=="A")
 {
  fMaxhitsA=nmax;
  fParams.AddNamedSlot("MaxhitsA");
  fParams.SetSignal(fMaxhitsA,"MaxhitsA");
 }
 if (s=="I")
 {
  fMaxhitsI=nmax;
  fParams.AddNamedSlot("MaxhitsI");
  fParams.SetSignal(fMaxhitsI,"MaxhitsI");
 }
 if (s=="IC")
 {
  fMaxhitsIC=nmax;
  fParams.AddNamedSlot("MaxhitsIC");
  fParams.SetSignal(fMaxhitsIC,"MaxhitsIC");
 }
 if (s=="DC")
 {
  fMaxhitsDC=nmax;
  fParams.AddNamedSlot("MaxhitsDC");
  fParams.SetSignal(fMaxhitsDC,"MaxhitsDC");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetSingleHit(Int_t ndoms,TString s,Int_t ndoms1)
{
// Set minimum number of good firing (D)OMs at which only the first hits
// (for instance after ordering the hits per (D)OM w.r.t. decreasing amplitude)
// will be used for reconstruction.
// This will allow to prevent very long processing time for large events.
// For default values please refer to the general documentation of this class.
//
// ndoms  : The minimum of good firing (D)OMs for the whole event
// ndoms1 : The minimum of good firing (D)OMs on a single string
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "IC" --> Standard IceCube DOMs
//     "DC" --> DeepCore DOMs
//     "I"  --> InIce (IC+DC) DOMs
//
// Note : Setting ndoms=0 and ndoms1=0 implies no automatic switching to single hit processing.
//
// The default is ndoms1=0 for backward compatibility.

 if (s=="A")
 {
  if (ndoms>=0)
  {
   fSingleA=ndoms;
   fParams.AddNamedSlot("SingleA");
   fParams.SetSignal(fSingleA,"SingleA");
  }
  if (ndoms1>=0)
  {
   fSingle1A=ndoms1;
   fParams.AddNamedSlot("Single1A");
   fParams.SetSignal(fSingle1A,"Single1A");
  }
 }
 if (s=="IC")
 {
  if (ndoms>=0)
  {
   fSingleIC=ndoms;
   fParams.AddNamedSlot("SingleIC");
   fParams.SetSignal(fSingleIC,"SingleIC");
  }
  if (ndoms1>=0)
  {
   fSingle1IC=ndoms1;
   fParams.AddNamedSlot("Single1IC");
   fParams.SetSignal(fSingle1IC,"Single1IC");
  }
 }
 if (s=="DC")
 {
  if (ndoms>=0)
  {
   fSingleDC=ndoms;
   fParams.AddNamedSlot("SingleDC");
   fParams.SetSignal(fSingleDC,"SingleDC");
  }
  if (ndoms1>=0)
  {
   fSingle1DC=ndoms1;
   fParams.AddNamedSlot("Single1DC");
   fParams.SetSignal(fSingle1DC,"Single1DC");
  }
 }
 if (s=="I")
 {
  if (ndoms>=0)
  {
   fSingleI=ndoms;
   fParams.AddNamedSlot("SingleI");
   fParams.SetSignal(fSingleI,"SingleI");
  }
  if (ndoms1>=0)
  {
   fSingle1I=ndoms1;
   fParams.AddNamedSlot("Single1I");
   fParams.SetSignal(fSingle1I,"Single1I");
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetMinAhits(Int_t nmin,TString s)
{
// Set the minimum number of associated hits to produce a reconstruction result 
// like for instance a reconstructed track.
//
// For default values please refer to the general documentation of this class.
//
// The input argument "s" allows specification for the various reconstructions.
//
// s = "A"  --> Amanda reconstruction
//     "I"  --> Hybrid InIce reconstruction
//     "IC" --> Standard IceCube reconstruction
//     "DC" --> DeepCore reconstruction

 if (s=="A")
 {
  fMinahitsA=nmin;
  fParams.AddNamedSlot("MinahitsA");
  fParams.SetSignal(fMinahitsA,"MinahitsA");
 }
 if (s=="I")
 {
  fMinahitsI=nmin;
  fParams.AddNamedSlot("MinahitsI");
  fParams.SetSignal(fMinahitsI,"MinahitsI");
 }
 if (s=="IC")
 {
  fMinahitsIC=nmin;
  fParams.AddNamedSlot("MinahitsIC");
  fParams.SetSignal(fMinahitsIC,"MinahitsIC");
 }
 if (s=="DC")
 {
  fMinahitsDC=nmin;
  fParams.AddNamedSlot("MinahitsDC");
  fParams.SetSignal(fMinahitsDC,"MinahitsDC");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetMinAmods(Int_t nmin,TString s)
{
// Set the minimum number of associated (D)OMs to produce a reconstruction result
// like for instance a reconstructed track.
//
// For default values please refer to the general documentation of this class.
//
// The input argument "s" allows specification for the various reconstructions.
//
// s = "A"  --> Amanda reconstruction
//     "I"  --> Hybrid InIce reconstruction
//     "IC" --> Standard IceCube reconstruction
//     "DC" --> DeepCore reconstruction

 if (s=="A")
 {
  fMinamodsA=nmin;
  fParams.AddNamedSlot("MinamodsA");
  fParams.SetSignal(fMinamodsA,"MinamodsA");
 }
 if (s=="I")
 {
  fMinamodsI=nmin;
  fParams.AddNamedSlot("MinamodsI");
  fParams.SetSignal(fMinamodsI,"MinamodsI");
 }
 if (s=="IC")
 {
  fMinamodsIC=nmin;
  fParams.AddNamedSlot("MinamodsIC");
  fParams.SetSignal(fMinamodsIC,"MinamodsIC");
 }
 if (s=="DC")
 {
  fMinamodsDC=nmin;
  fParams.AddNamedSlot("MinamodsDC");
  fParams.SetSignal(fMinamodsDC,"MinamodsDC");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetSLChitUsage(Int_t flag,TString s)
{
// (De)activate the usage of Soft Local Coincidence hits for reconstruction.
//
// flag = 0 : SLC hits are not used for reconstruction
//      = 1 : SLC hits are used for reconstruction
//
// The input argument "s" allows for detector specification.
//
// s = "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs
//
// For default values please refer to the general documentation of this class.

 if (s=="I")
 {
  fSlcI=flag;
  fParams.AddNamedSlot("SlcI");
  fParams.SetSignal(fSlcI,"SlcI");
 }
 if (s=="IC")
 {
  fSlcIC=flag;
  fParams.AddNamedSlot("SlcIC");
  fParams.SetSignal(fSlcIC,"SlcIC");
 }
 if (s=="DC")
 {
  fSlcDC=flag;
  fParams.AddNamedSlot("SlcDC");
  fParams.SetSignal(fSlcDC,"SlcDC");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetFlipAngles(Float_t thetatrk,Float_t thetahits)
{
// Set the angular thresholds (in degrees) for the track (thetatrk) and
// hit path (thetahits) polar angles for the track direction flipping.
// The polar angle (theta) is defined such that theta=0 corresponds to
// a straight upgoing track.
// In case a track has theta<thetatrk and the (time ordered) hit pattern
// reflects a direction with theta>thetahits, the direction of the track
// will be reversed by invokation of the memberfunction FlipTrack().
// This allows an additional reduction of the background for upgoing tracks
// due to mis-reconstructed downgoing muons.
// However, this may result in a (significant) loss of upgoing signal events.
//
// Note : The flipping of the track direction may be de-activated by
//        setting thetatrk<0 and/or thetahits>180.
//
// For default values please refer to the general documentation of this class.

 fThetatrk=thetatrk;
 fThetahits=thetahits;

 fParams.AddNamedSlot("Thetatrk");
 fParams.AddNamedSlot("Thetahits");
 fParams.SetSignal(fThetatrk,"Thetatrk");
 fParams.SetSignal(fThetahits,"Thetahits");
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetScatteringLength(Float_t lambda,TString s)
{
// Set average photon scattering length in meter.
// For default values please refer to the general documentation of this class.
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "UD" --> InIce DOMs in the Upper Detector above the dust layer
//     "DL" --> InIce DOMs in the Dust Layer
//     "LD" --> InIce DOMs in the Lower Detector below the dust layer

 if (s=="A")
 {
  fLambdaA=lambda;
  fParams.AddNamedSlot("LambdaA");
  fParams.SetSignal(fLambdaA,"LambdaA");
 }
 if (s=="UD")
 {
  fLambdaUD=lambda;
  fParams.AddNamedSlot("LambdaUD");
  fParams.SetSignal(fLambdaUD,"LambdaUD");
 }
 if (s=="DL")
 {
  fLambdaDL=lambda;
  fParams.AddNamedSlot("LambdaDL");
  fParams.SetSignal(fLambdaDL,"LambdaDL");
 }
 if (s=="LD")
 {
  fLambdaLD=lambda;
  fParams.AddNamedSlot("LambdaLD");
  fParams.SetSignal(fLambdaLD,"LambdaLD");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetAbsorptionLength(Float_t lambda,TString s)
{
// Set average photon absorption length in meter.
// For default values please refer to the general documentation of this class.
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "UD" --> InIce DOMs in the Upper Detector above the dust layer
//     "DL" --> InIce DOMs in the Dust Layer
//     "LD" --> InIce DOMs in the Lower Detector below the dust layer

 if (s=="A")
 {
  fLabsA=lambda;
  fParams.AddNamedSlot("LabsA");
  fParams.SetSignal(fLabsA,"LabsA");
 }
 if (s=="UD")
 {
  fLabsUD=lambda;
  fParams.AddNamedSlot("LabsUD");
  fParams.SetSignal(fLabsUD,"LabsUD");
 }
 if (s=="DL")
 {
  fLabsDL=lambda;
  fParams.AddNamedSlot("LabsDL");
  fParams.SetSignal(fLabsDL,"LabsDL");
 }
 if (s=="LD")
 {
  fLabsLD=lambda;
  fParams.AddNamedSlot("LabsLD");
  fParams.SetSignal(fLabsLD,"LabsLD");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetTimeJitter(Float_t sigma,TString s)
{
// Set the time jitter of the various PMTs.
// The input argument "sigma" represents the standard deviation in ns.
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "IC" --> Standard IceCube DOMs
//     "DC" --> DeepCore DOMs
//

 if (s=="A")
 {
  fTsigmaA=sigma;
  fParams.AddNamedSlot("TsigmaA");
  fParams.SetSignal(fTsigmaA,"TsigmaA");
 }
 if (s=="IC")
 {
  fTsigmaIC=sigma;
  fParams.AddNamedSlot("TsigmaIC");
  fParams.SetSignal(fTsigmaIC,"TsigmaIC");
 }
 if (s=="DC")
 {
  fTsigmaDC=sigma;
  fParams.AddNamedSlot("TsigmaDC");
  fParams.SetSignal(fTsigmaDC,"TsigmaDC");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetVgroupUsage(Int_t flag,TString s)
{
// (De)activate the distinction between v_phase and v_group of the Cherenkov light.
//
// flag = 0 : No distinction between v_phase and v_group
//      = 1 : Separate treatment of v_phase and v_group
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction
//
// By default the distinction between v_phase and v_group is activated
// in the constructor of this class.

 if (s=="A")
 {
  fVgroupA=flag;
  fParams.AddNamedSlot("VgroupA");
  fParams.SetSignal(fVgroupA,"VgroupA");
 }
 if (s=="IC")
 {
  fVgroupIC=flag;
  fParams.AddNamedSlot("VgroupIC");
  fParams.SetSignal(fVgroupIC,"VgroupIC");
 }
 if (s=="DC")
 {
  fVgroupDC=flag;
  fParams.AddNamedSlot("VgroupDC");
  fParams.SetSignal(fVgroupDC,"VgroupDC");
 }
 if (s=="I")
 {
  fVgroupI=flag;
  fParams.AddNamedSlot("VgroupI");
  fParams.SetSignal(fVgroupI,"VgroupI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetTrackName(TString s)
{
// Set (alternative) name identifier for the produced reconstructed tracks.
// This allows unique identification of (newly) produced reco tracks
// in case of re-processing of existing data with different criteria.
// By default the produced reco tracks have the name of the class
// by which they were produced.

 fTrackname=s;

 fParams.AddNamedSlot(fTrackname);
 fParams.SetSignal(1,fTrackname);
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::SetCharge(Float_t charge)
{
// Set user defined charge for the produced reconstructed tracks.
// This allows identification of these tracks on color displays.
// By default the produced reco tracks have charge=0
// which is set in the constructor of this class.

 fCharge=charge;
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::UseTracks(TString classname,Int_t n)
{
// Specification of the input tracks to be used in a track based reconstruction.
//
// classname : Specifies the input track trackname (e.g. "IceDwalkIC").
// n : Specifies the max. number of these tracks to be used
//
// Note : n<0 will use all the existing tracks of the specified classname
//
// The default is n=-1.
//
// Consecutive invokations of this memberfunction with different classnames
// will result in an incremental effect.
//
// Example :
// ---------
// UseTracks("IceDwalkIC",5);
// UseTracks("IceLinefit",2);
// UseTracks("IceDwalkDC");
//
// This will use the first 5 IceDwalkIC, the first 2 IceLinefit and all the
// IceDwalkDC tracks which are encountered in the event structure.

 if (!fUseNames)
 {
  fUseNames=new TObjArray();
  fUseNames->SetOwner();
 }
 
 if (!fUseNtk) fUseNtk=new TArrayI();

 // Check if this classname has already been specified before 
 TString s;
 Int_t nen=fUseNames->GetEntries();
 for (Int_t i=0; i<nen; i++)
 {
  TObjString* sx=(TObjString*)fUseNames->At(i);
  if (!sx) continue;
  s=sx->GetString();
  if (s==classname) return;
 }

 // New classname to be added into the storage
 if (nen >= fUseNames->GetSize()) fUseNames->Expand(nen+1);
 if (nen >= fUseNtk->GetSize()) fUseNtk->Set(nen+1);
 
 TObjString* name=new TObjString();
 name->SetString(classname);
 fUseNames->Add(name);
 fUseNtk->AddAt(n,nen);

 fParams.AddNamedSlot("UseTracks");
 fParams.SetSignal(1,"UseTracks");
}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::Exec(Option_t* opt)
{
////////////////////////////////////////////////////////////////////
// Template for the implementation of a reconstruction processor. //
////////////////////////////////////////////////////////////////////

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
 if (!fUseNames) // Reconstruction procedure on complete event
 {
  fParams.SetNameTitle("IceRecoBase","IceRecoBase complete event reco parameters");
 }
 else // Reconstruction procedure on track associated data
 {
  fParams.SetNameTitle("IceRecoBase4Track","IceRecoBase track based reco parameters");
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
  cout << " *IceRecoBase* First guess selections to be processed (-1=all)." << endl;
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

 /////////////////////////////////////////////////////////////////
 // Enter here your code for the actual reconstruction process. //
 /////////////////////////////////////////////////////////////////

}
///////////////////////////////////////////////////////////////////////////
void IceRecoBase::FlipTrack(NcTrack* t) const
{
// Reverse the direction of the track "t" in case a the track has a polar angle
// theta<thetatrk and the (time ordered) hit pattern reflects a direction with
// polar angle theta>thetahits.
// This allows an additional reduction of the background for upgoing tracks
// due to mis-reconstructed downgoing muons.
//
// The polar angle thresholds "thetatrk" and "thetahits" may be set by the user
// via the member function SetFlipAngles(). 
// For default values please refer to the general documentation of this class.
//
// Note : The flipping of the track direction may be de-activated by
//        setting thetatrk<0 and/or thetahits>180.
 
 if (!t || fThetatrk<0 || fThetahits>180) return;

 Nc3Vector p;
 Float_t theta=0; 
 p=t->Get3Momentum();
 theta=p.GetX(2,"sph","deg");

 if (theta>=fThetatrk) return;

 // Check the HitPath of the time ordered hits associated to this track
 TObjArray* hits=t->GetSignals("IceGOM",1);
 if (!hits) return;
 NcDevice sorter;
 TObjArray* sorted=sorter.SortHits("LE",1,hits,7);
 if (!sorted) return;

 // Make sure that the starting hit is an HLC hit
 for (Int_t i=0; i<sorted->GetEntries(); i++)
 {
  NcSignal* sx=(NcSignal*)sorted->At(i);
  if (!sx) continue;

  // For Amanda the concept of HLC and SLC hits didn't exist
  NcDevice* omx=sx->GetDevice();
  if (!omx) continue;
  if (omx->InheritsFrom("IceAOM")) continue;

  if (sx->GetSignal("SLC")<0.5) break;
  sorted->Remove(sx);
 }
 sorted->Compress();

 Nc3Vector hitpath;
 hitpath=fEvt->GetHitPath(sorted,1);
 theta=hitpath.GetX(2,"sph","deg");

 if (theta<=fThetahits) return;

 // Flip the track direction
 p*=-1.;
 t->Set3Momentum(p);
 NcSignal* fitstats=(NcSignal*)t->GetFitDetails();
 if (fitstats)
 {
  fitstats->AddNamedSlot("TrackFlip");
  fitstats->SetSignal(1,"TrackFlip");
 }
}
///////////////////////////////////////////////////////////////////////////
