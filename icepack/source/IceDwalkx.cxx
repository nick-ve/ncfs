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

// $Id: IceDwalkx.cxx 63 2012-07-13 14:17:43Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class IceDwalkx
// TTask derived class to perform direct walk track reconstruction.
//
// In case an event has been rejected by an NcEventSelector (based) processor,
// this task (and its sub-tasks) is not executed.
//
// Note : Amanda OMs, standard IceCube (IC), DeepCore (DC) and InIce (I) DOMs are treated
//        seperately, which means that for events with various OMs and/or DOMs firing,
//        several direct walk tracks may be produced. The various direct walk tracks can be
//        distinguished on basis of their name as explained below. 
//
// The procedure is based on the method described in the Amanda publication
// in Nuclear Instruments and Methods A524 (2004) 179-180.
// However, the Amanda method has been extended with the intention to
// take also multiple (muon) tracks within 1 event into account.
// This will not only provide a means to reconstruct muon bundles and
// multiple track events in IceCube, but will also allow to reduce the
// background of faked upgoing muons as a result of multiple downgoing
// muons hitting the top and bottom parts of the detector.
//
// A further extension of the original Amanda method is the separate treatment
// of the phase and group velocities as introduced in collaboration with
// George Japaridze (Clark Atlanta University, USA) which will provide more
// accurate time residuals due to the different velocities of the Cerenkov
// wave front (v_phase) and the actually detected photons (v_group).
// This distinction between v_phase and v_group can be (de)activated via the
// memberfunction SetVgroupUsage(). By default the distinction between v_phase
// and v_group is activated in the constructor of this class.
//
// To prevent waisting CPU time in trying to reconstruct (high-energy) cascade
// events, or to select specifically reconstruction of low multiplicity events,
// the user may invoke the memberfunctions SetMaxMod() and SetMinMod().
// This allows selection of events for processing with a certain maximum and/or
// minimum number of good (D)OMs firing.
// By default the minimum and maximum are set to 0 and 999999, respectively,
// in the constructor, which implies no multiplicity selection. 
// The maximum number of good hits per (D)OM to be used for the reconstruction
// can be specified via the memberfunction SetMaxHits().
// By default only the first good hit of each (D)OM is used but the user may want
// to extend this number to the first n hits of each (D)OM to account for possible
// noise and/or afterpulse signals that are not recognised by the hit cleaning procedure.
// Note that when all the good hits of a (D)OM are used, this may lead to large
// processing time in case many noise and/or afterpulse signals are not
// recognised by the hit cleaning procedure.
//
// The SetMaxHits() facility may also be used to select a specific detector
// configuration (e.g. "IC" and "DC" without "I") to be used for the reconstruction.
// This can be achieved by suppressing processing of e.g. the "I" configuration
// by setting the corresponding maximum number of hits to -1. 
//
// Another facility to (automatically) select reconstruction of specific detector
// configurations and/or save CPU time is the so-called conditional processing mode.
// For further details please refer to the docs of the memberfunction SetConditionalReco(). 
//
// All reconstructed tracks will be stored in the IceEvent structure with as
// default the Classname of the producing processor as the name of the track.
// A suffix "A" for an Amanda (OM) track, "IC" for a standard IceCube InIce (DOM) track,
// "I" for a hybrid InIce (DOM) track or "DC" for a DeepCore (DOM) track will be
// added to the name automatically.
// This track name identifier can be modified by the user via the
// SetTrackName() memberfunction. This will allow unique identification
// of tracks which are produced when re-processing existing data with
// different criteria.
// Note that a suffix "A", "IC", "I" or "DC" will always be generated automatically.
//
// The track 3-momentum is set to the reconstructed direction, normalised to 1 GeV.
// The mass and charge of the track are left 0, since no distinction can
// be made between positive or negative tracks and also no mass can be determined.
// However, the user can define the track charge by invokation of the
// memberfunction SetCharge().
// This facility may be used to distinguish tracks produced by the
// various reconstruction algorithms in a (3D) colour display
// (see the class NcHelix for further details).  
// The r0 and t0 can be obtained from the reference point of the track,
// whereas the t0 ia also available from the track timestamp .
//
// Information about the actual parameter settings can be found in the event
// structure itself via the device named "IceDwalkx".
//
// The various reconstruction steps are summarised as follows :
//
// 1) Construction of track elements (TE's).
//    A track element is a straight line connecting two hits that
//    appeared at some minimum distance d and within some maximum
//    time difference dt, according to eq. (20) of the NIM article.
//    The default value for d is 75 (A), 120 (IC), 60 (I) and 50 (DC) meter,
//    but this can be modified via the memberfunction SetDmin().
//    By default dt=(hit distance)/c but an additional time margin
//    (set to 0 by default) may be specified via the memberfunction SetDtmarg().    
//    The reference point r0 of the TE is taken as the center between
//    the two hit positions and the TE timestamp t0 at the position r0
//    is taken as the IceEvent timestamp increased by the average of the
//    two hit times. So, all timestamps contain the overall IceEvent
//    timestamp as a basis. This means that time differences can be
//    obtained via the NcTimestamp facilities (supporting upto picosecond
//    precision when available).
//    The TE direction is given by the relative position of the two hits.
//
// 2) Each TE will obtain so called associated hits.
//    A hit is associated to a TE when it fulfills both the conditions
//
//      -30 < tres < 300 ns
//      dhit/lambda < F
//
//    tres   : time residual
//             Difference between the observed hit time and the time expected
//             for a direct photon hit.     
//    dhit   : Distance traveled by the cherenkov photon from the track to the hit position
//    lambda : Photon scattering length in ice
//
//    The default values of the scattering length are 33.3 (A), 30 (IC), 33 (I) and 35 (DC) meter,
//    but this can be modified via the memberfunction SetScatteringLength().
//    By default F is set to 3.07126 (A) and 3 (IC, I, DC), but this can be modified via
//    the memberfunction SetMaxDhit(). 
//
// 3) Construction of track candidates (TC's).
//    These are TE's that fulfill both the conditions
//
//     nax >= 1
//     qtc >= 0.8*qtcmax
//
//    where we have defined :
//
//    nax    : Associated hits and/or strings value for the specific TE.
//    qtc    : The track quality number (see hereafter).
//    qtcmax : Maximum quality number encountered for the TE's.
//
//    Note : The selection to use the number of associated hits and/or strings
//           for the track quality number can be performed via the
//           memberfunction SetAsType(). 
//
//    The track quality number qtc is defined as follows :
//
//     qtc=nax*(term1+term2)-term3-term4-term5
//
//    here we have defined :
//
//    term1=2*spread/span
//    term2=2*spreadL/spanL
//    term3=|spread-expspread|/spread
//    term4=|spreadL-expspreadL|/spreadL
//    term5=|medianT|/spreadT
//
//    The central observables here are the projected positions X on the track
//    of the various associated hits w.r.t. the track reference point r0.
//    Note that X can be negative as well as positive.
//    Therefore we also introduce XL=|X|. 
//
//    span       : max(X)-min(X)
//    spanL      : max(XL)-min(XL)
//    Xmedian    : median of X
//    XmedianL   : median of XL
//    spread     : < |X-Xmedian| >
//    spreadL    : < |XL-XmedianL| >
//    expspread  : expected spread in X for a flat distribution of nah hits over span
//    expspreadL : expected spread in XL for a flat distribution of nah hits over spanL
//    medianT    : median of tres
//    spreadT    : < |tres-medianT| >
//
//    However, if |Xmedian| > span/2 we set qtc=0 in order to always require
//    projected hits to appear on both sides of r0 on the track.
//
//    Note : The qtc quality number is used to define the norm of the momentum
//           of the track candidate. As such it serves as a weight for the jet
//           momentum (direction) after clustering of the TC's and lateron
//           merging of the jets (see hereafter).
//
// 4) The remaining track candidates are clustered into jets when their directions
//    are within a certain maximum opening angle.
//    In addition a track candidate must within a certain maximum distance
//    of the jet starting TC in order to get clustered. 
//    The latter criterion prevents clustering of (nearly) parallel track candidates
//    crossing the detector a very different locations (e.g. muon bundles).
//    The default maximum track opening angle is 15 degrees, but can be modified
//    via the SetTangmax memberfunction.
//    The default maximum track distance is 20 meters, but can be modified
//    via the SetTdistmax memberfunction. This memberfunction also allows to
//    specify whether the distance is determined within the detector volume or not.
//
//    The average of all the r0 and t0 values of the constituent TC's
//    of the jet will provide the r0 and t0 (i.e. reference point) of the jet.
//
//    The jet total momentum consists of the vector sum of the momenta of the
//    constituent TC's. This implies that the qtc quality numbers of the various
//    TC's define a weight for each track in the construction of the jet direction.
//    In addition it means that the total jet momentum represents the sum of the
//    qtc quality numbers of the constituent TC's weighted by the opening angles
//    between the various TC's.  
//    As such each jet is given an absolute quality number defined as :
//
//      qtcjet=|jet momentum|/ntracks 
//
//    This jet quality number is refined on basis of the number of hits
//    associated to the jet as :
//
//      qtcjet=qtcjet+0.2*(nax-naxmax)
//
//    where we have defined :
//
//    nax    : Associated hits and/or strings value for the specific jet.
//    naxmax : Maximum number of associated hits (or strings) encountered for the jets.
//
//    This qtcjet value is then used to order the various jets w.r.t.
//    decreasing qtcjet quality number.
//
//    Note : The qtcjet value is stored as "energy" of the jet, such that
//           it is always available for each jet and can also be used for
//           ordering the jets according to this value using the generic
//           NcEvent::SortJets() facility. 
//
// 5) The jets (after having been ordered w.r.t. decreasing qtcjet value)
//    are merged when their directions are within a certain maximum opening angle.
//    In addition a jet must within a certain maximum distance of the starting jet
//    in order to get merged. 
//    The latter criterion prevents merging of (nearly) parallel tracks/jets
//    crossing the detector a very different locations (e.g. muon bundles).
//    The jet ordering before the merging process is essential, since the starting jet
//    will "eat up" the jets that will be merged into it. 
//    The jet ordering ensures that the jet with the highest quality number will
//    always initiate the merging process.
//    The default maximum opening angle is half the TC maximum opening angle,
//    but can be modified via the SetJangmax memberfunction. This memberfunction
//    also allows to specify whether jet merging will be performed iteratively or not.
//    In case iteration has been activated, the jet ordering is performed after each
//    iteration step. This has to be done because since the quality numbers of the
//    resulting merged jets have been automatically updated in the merging process.
//    
//    The default maximum jet distance is 30 meters, but can be modified
//    via the SetJdistmax memberfunction. This memberfunction also allows to
//    specify whether the distance is determined within the detector volume or not.
//
//    Note : Setting the maximum jet opening angle to <=0 will prevent
//           the merging of jets.
//
//    The average of all the r0 and t0 values of the merged jets will provide
//    the r0 and t0 (i.e. reference point) of the final jet.
//
// 6) The remaining (merged) jets are ordered w.r.t. decreasing jet quality number.
//    As such the jet with the highest quality number will be the first one
//    in the list, which will result in the fact that the final tracks are also
//    ordered w.r.t. decreasing quality number, as outlined hereafter.
//    Each remaining jet will provide the parameters (e.g. direction)
//    for a reconstructed track.
//    The track 3-momentum is set to the total jet 3-momentum, normalised
//    to 1 GeV. The mass and charge of the track are left 0.
//    The reference point data of the jet will provide the r0 and t0
//    (i.e. reference point) of the track.
//
//    All these reconstructed tracks will be stored in the IceEvent structure
//    using the naming conventions outlined above.
//
//    Note : In case the maximum jet opening angle was specified <0,
//           only the jet with the highest quality number will appear
//           as a reconstructed track in the IceEvent structure.
//           This will allow comparison with the old Amanda (Sieglinde)
//           single track direct walk reconstruction results. 
//    
// For further details the user is referred to NIM A524 (2004) 169.
//
// Note : This algorithm works best on data which has been calibrated,
//        cross talk corrected (Amanda) and cleaned from noise hits etc. (IceCleanHits).
//
//--- Author: Nick van Eijndhoven 07-oct-2005 Utrecht University
//- Modified: NvE $Date: 2012-07-13 16:17:43 +0200 (Fri, 13 Jul 2012) $ NCFS
///////////////////////////////////////////////////////////////////////////
 
#include "IceDwalkx.h"
#include "Riostream.h"

ClassImp(IceDwalkx) // Class implementation to enable ROOT I/O

IceDwalkx::IceDwalkx(const char* name,const char* title) : IceLinefit(name,title)
{
// Default constructor.
// The various reconstruction parameters are initialised to the values
// as mentioned in the general documentation of this class.
// The angular separation parameter for jet merging is initialised as half
// the value of the angular separation parameter for track candidate clustering.    
 fEvt=0;
 fDminA=75;
 fDminI=60;
 fDminIC=120;
 fDminDC=50;
 fDtmargA=0;
 fDtmargI=0;
 fDtmargIC=0;
 fDtmargDC=0;
 fMaxdhitA=3.07126;
 fMaxdhitI=3;
 fMaxdhitIC=3;
 fMaxdhitDC=3;
 fTangmaxA=15;
 fTangmaxI=15;
 fTangmaxIC=15;
 fTangmaxDC=15;
 fTdistmaxA=20;
 fTdistmaxI=20;
 fTdistmaxIC=20;
 fTdistmaxDC=20;
 fTinvolA=1;
 fTinvolI=1;
 fTinvolIC=1;
 fTinvolDC=1;
 fJangmaxA=fTangmaxA/2.;
 fJangmaxI=fTangmaxI/2.;
 fJangmaxIC=fTangmaxIC/2.;
 fJangmaxDC=fTangmaxDC/2.;
 fJiterateA=1;
 fJiterateI=1;
 fJiterateIC=1;
 fJiterateDC=1;
 fJdistmaxA=30;
 fJdistmaxI=30;
 fJdistmaxIC=30;
 fJdistmaxDC=30;
 fJinvolA=1;
 fJinvolI=1;
 fJinvolIC=1;
 fJinvolDC=1;
 fMaxmodA=999999;
 fMaxmodI=999999;
 fMaxmodIC=999999;
 fMaxmodDC=999999;
 fMinmodA=0;
 fMinmodI=0;
 fMinmodIC=0;
 fMinmodDC=0;
 fMaxhitsA=1;
 fMaxhitsI=1;
 fMaxhitsIC=1;
 fMaxhitsDC=1;
 fVgroupA=1;
 fVgroupI=1;
 fVgroupIC=1;
 fVgroupDC=1;
 fAsTypeA=3;
 fAsTypeI=3;
 fAsTypeIC=3;
 fAsTypeDC=3;
 fLambdaA=33.3;
 fLambdaI=33;
 fLambdaIC=30;
 fLambdaDC=35;
 fSlcI=0;
 fSlcIC=0;
 fSlcDC=0;
 fTrackname="";
 fCharge=0;
 fConditional=1;
}
///////////////////////////////////////////////////////////////////////////
IceDwalkx::~IceDwalkx()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetDmin(Float_t d,TString s)
{
// Set minimum hit distance (in m) to form a track element.
// For default values please refer to the general documentation of this class.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs
 
 if (s=="A") fDminA=d;
 if (s=="I") fDminI=d;
 if (s=="IC") fDminIC=d;
 if (s=="DC") fDminDC=d;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetDtmarg(Int_t dt,TString s)
{
// Set maximum hit time difference margin (in ns) for track elements.
// For default values please refer to the general documentation of this class.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs

 if (s=="A") fDtmargA=dt;
 if (s=="I") fDtmargI=dt;
 if (s=="IC") fDtmargIC=dt;
 if (s=="DC") fDtmargDC=dt;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetMaxDhit(Float_t d,TString s)
{
// Set maximum distance (in scattering length) for a hit to get associated.
// For default values please refer to the general documentation of this class.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs

 if (s=="A") fMaxdhitA=d;
 if (s=="I") fMaxdhitI=d;
 if (s=="IC") fMaxdhitIC=d;
 if (s=="DC") fMaxdhitDC=d;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetTangmax(Float_t ang,TString s)
{
// Set maximum angular separation (in deg) for track candidate clustering
// into jets.
// In the constructor the default has been set to 15 deg, in accordance
// to NIM A524 (2004) 180.
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs
//
// Note : This function also sets automatically the value of the maximum
//        angular separation for jet merging into 1 single track to ang/2.
//        In order to specify a different max. jet merging separation angle,
//        one has to invoke the memberfunction SetJangmax afterwards.
 
 if (s=="A")
 {
  fTangmaxA=ang;
  fJangmaxA=ang/2.;
 }
 
 if (s=="I")
 {
  fTangmaxI=ang;
  fJangmaxI=ang/2.;
 }
 
 if (s=="IC")
 {
  fTangmaxIC=ang;
  fJangmaxIC=ang/2.;
 }
 
 if (s=="DC")
 {
  fTangmaxDC=ang;
  fJangmaxDC=ang/2.;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetTdistmax(Float_t d,TString s,Int_t invol)
{
// Set maximum distance (in m) of the two track candidates in the track
// clustering process.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs
//
// The distance between the two tracks can be determined restricted to the
// detector volume (invol=1) or in the overall space (invol=0).  
// The former will prevent clustering of (nearly) parallel tracks which cross
// the detector volume at very different locations, whereas the latter will
// enable clustering of tracks with a common location of origin (e.g. muon
// bundles from an air shower) even if they cross the detector volume at
// very different locations. 
// At invokation of this memberfunction the default is invol=1.
// In the constructor the default has been set to 20 meter with invol=1.
 
 if (s=="A")
 {
  fTdistmaxA=d;
  fTinvolA=invol;
 }
 
 if (s=="I")
 {
  fTdistmaxI=d;
  fTinvolI=invol;
 }
 
 if (s=="IC")
 {
  fTdistmaxIC=d;
  fTinvolIC=invol;
 }
 
 if (s=="DC")
 {
  fTdistmaxDC=d;
  fTinvolDC=invol;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetJangmax(Float_t ang,TString s,Int_t iter)
{
// Set angular separation (in deg) within which jets are merged into 1
// single track.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs
//
// The merging process is a dynamic procedure and can be carried out by
// iteration (iter=1) until no further merging of the various jets occurs anymore.
// However, by specification of iter=0 the user can also select to go only
// once through all the jet combinations to check for mergers.
// For large events the latter will in general result in more track candidates.  
// At invokation of this memberfunction the default is iter=1.
// In the constructor the default angle has been set 7.5 deg, being half
// of the value of the default track candidate clustering separation angle.
// The iteration flag was set to 1 in the constructor.
//
// Notes :
// -------
// 1)  Setting ang=0 will prevent jet merging.
//     Consequently, every jet will appear as a separate track in the
//     reconstruction result.  
// 2)  Setting ang<0 will prevent jet merging.
//     In addition, only the jet with the maximum number of tracks will
//     appear as a track in the reconstruction result.
//     This situation resembles the standard Sieglinde direct walk processing
//     and as such can be used to perform comparison studies.

 if (s=="A")
 {
  fJangmaxA=ang;
  fJiterateA=iter;
 }

 if (s=="I")
 {
  fJangmaxI=ang;
  fJiterateI=iter;
 }

 if (s=="IC")
 {
  fJangmaxIC=ang;
  fJiterateIC=iter;
 }

 if (s=="DC")
 {
  fJangmaxDC=ang;
  fJiterateDC=iter;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetJdistmax(Float_t d,TString s,Int_t invol)
{
// Set maximum distance (in m) of the two jets in the jet merging process.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs
//
// The distance between the two jets can be determined restricted to the
// detector volume (invol=1) or in the overall space (invol=0).  
// The former will prevent clustering of (nearly) parallel tracks which cross
// the detector volume at very different locations, whereas the latter will
// enable clustering of tracks with a common location of origin (e.g. muon
// bundles from an air shower) even if they cross the detector volume at
// very different locations. 
// At invokation of this memberfunction the default is invol=1.
// In the constructor the default has been set to 30 meter with invol=1.
 
 if (s=="A")
 {
  fJdistmaxA=d;
  fJinvolA=invol;
 }
 
 if (s=="I")
 {
  fJdistmaxI=d;
  fJinvolI=invol;
 }
 
 if (s=="IC")
 {
  fJdistmaxIC=d;
  fJinvolIC=invol;
 }
 
 if (s=="DC")
 {
  fJdistmaxDC=d;
  fJinvolDC=invol;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetVgroupUsage(Int_t flag,TString s)
{
// (De)activate the distinction between v_phase and v_group of the Cherenkov light.
//
// flag = 0 : No distinction between v_phase and v_group
//      = 1 : Separate treatment of v_phase and v_group
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs
//
// By default the distinction between v_phase and v_group is activated
// in the constructor of this class.

 if (s=="A") fVgroupA=flag;
 if (s=="I") fVgroupI=flag;
 if (s=="IC") fVgroupIC=flag;
 if (s=="DC") fVgroupDC=flag;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetAsType(Int_t flag,TString s)
{
// Select number of associated hits and/or strings as quality indicator.
//
// flag = 1 : Number of associated hits (nah) is used as quality indicator
//      = 2 : Number of associated strings (nas) is used as quality indicator
//      = 3 : nah*nas is used as quality indicator
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs
//
// By default the flag value is set to 3 in the constructor of this class.

 if (s=="A") fAsTypeA=flag;
 if (s=="I") fAsTypeI=flag;
 if (s=="IC") fAsTypeIC=flag;
 if (s=="DC") fAsTypeDC=flag;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetScatteringLength(Float_t lambda,TString s)
{
// Set average photon scattering length in meter.
// For default values please refer to the general documentation of this class.
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda OMs
//     "I"  --> InIce DOMs
//     "IC" --> Standard IceCube InIce DOMs
//     "DC" --> DeepCore DOMs

 if (s=="A") fLambdaA=lambda;
 if (s=="I") fLambdaI=lambda;
 if (s=="IC") fLambdaIC=lambda;
 if (s=="DC") fLambdaDC=lambda;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SetConditionalReco(Int_t flag)
{
// Set flag for conditional reconstruction of the various detector combinations.
// This will allow to save considerably on cpu time, especially on large events.
//
// flag : 0 ==> Always invoke all the various reconstructions
//        1 ==> No "I" reconstruction in case "IC" track was found
//        2 ==> Same as flag=1 but also no "DC" reconstruction in case "IC" or "I" track was found
//
// By default flag=1 is set in the constructor.
//
// Notes :
// -------
// 1) Reconstruction in a specific detector can always be de-activated by
//    invokation of the SetMaxHits memberfunction. 
// 2) In case of inconsistent input the current setting will not be modified. 

 if (flag>=0 && flag<=2) fConditional=flag;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::Exec(Option_t* opt)
{
// Implementation of the direct walk track reconstruction.

 TString name=opt;
 NcJob* parent=(NcJob*)(gROOT->GetListOfTasks()->FindObject(name.Data()));

 if (!parent) return;

 fEvt=(IceEvent*)parent->GetObject("IceEvent");
 if (!fEvt) return;

 // Only process accepted events
 NcDevice* seldev=(NcDevice*)fEvt->GetDevice("NcEventSelector");
 if (seldev)
 {
  if (seldev->GetSignal("Select") < 0.1) return;
 }

 // Enter the reco parameters as a device in the event
 NcDevice params;
 params.SetNameTitle(ClassName(),"Reco parameters");
 params.AddNamedSlot("DminA");
 params.AddNamedSlot("DtmargA");
 params.AddNamedSlot("MaxdhitA");
 params.AddNamedSlot("TangmaxA");
 params.AddNamedSlot("TdistmaxA");
 params.AddNamedSlot("TinvolA");
 params.AddNamedSlot("JangmaxA");
 params.AddNamedSlot("JiterateA");
 params.AddNamedSlot("JdistmaxA");
 params.AddNamedSlot("JinvolA");
 params.AddNamedSlot("MaxmodA");
 params.AddNamedSlot("MinmodA");
 params.AddNamedSlot("MaxhitsA");
 params.AddNamedSlot("VgroupA");
 params.AddNamedSlot("AsTypeA");
 params.AddNamedSlot("LambdaA");

 params.SetSignal(fDminA,"DminA");
 params.SetSignal(fDtmargA,"DtmargA");
 params.SetSignal(fMaxdhitA,"MaxhitA");
 params.SetSignal(fTangmaxA,"TangmaxA");
 params.SetSignal(fTdistmaxA,"TdistmaxA");
 params.SetSignal(fTinvolA,"TinvolA");
 params.SetSignal(fJangmaxA,"JangmaxA");
 params.SetSignal(fJiterateA,"JiterateA");
 params.SetSignal(fJdistmaxA,"JdistmaxA");
 params.SetSignal(fJinvolA,"JinvolA");
 params.SetSignal(fMaxmodA,"MaxmodA");
 params.SetSignal(fMinmodA,"MinmodA");
 params.SetSignal(fMaxhitsA,"MaxhitsA");
 params.SetSignal(fVgroupA,"VgroupA");
 params.SetSignal(fAsTypeA,"AsTypeA");
 params.SetSignal(fLambdaA,"LambdaA");

 params.AddNamedSlot("DminI");
 params.AddNamedSlot("DtmargI");
 params.AddNamedSlot("MaxdhitI");
 params.AddNamedSlot("TangmaxI");
 params.AddNamedSlot("TdistmaxI");
 params.AddNamedSlot("TinvolI");
 params.AddNamedSlot("JangmaxI");
 params.AddNamedSlot("JiterateI");
 params.AddNamedSlot("JdistmaxI");
 params.AddNamedSlot("JinvolI");
 params.AddNamedSlot("MaxmodI");
 params.AddNamedSlot("MinmodI");
 params.AddNamedSlot("MaxhitsI");
 params.AddNamedSlot("VgroupI");
 params.AddNamedSlot("AsTypeI");
 params.AddNamedSlot("LambdaI");
 params.AddNamedSlot("SlcI");

 params.SetSignal(fDminI,"DminI");
 params.SetSignal(fDtmargI,"DtmargI");
 params.SetSignal(fMaxdhitI,"MaxhitI");
 params.SetSignal(fTangmaxI,"TangmaxI");
 params.SetSignal(fTdistmaxI,"TdistmaxI");
 params.SetSignal(fTinvolI,"TinvolI");
 params.SetSignal(fJangmaxI,"JangmaxI");
 params.SetSignal(fJiterateI,"JiterateI");
 params.SetSignal(fJdistmaxI,"JdistmaxI");
 params.SetSignal(fJinvolI,"JinvolI");
 params.SetSignal(fMaxmodI,"MaxmodI");
 params.SetSignal(fMinmodI,"MinmodI");
 params.SetSignal(fMaxhitsI,"MaxhitsI");
 params.SetSignal(fVgroupI,"VgroupI");
 params.SetSignal(fAsTypeI,"AsTypeI");
 params.SetSignal(fLambdaI,"LambdaI");
 params.SetSignal(fSlcI,"SlcI");

 params.AddNamedSlot("DminIC");
 params.AddNamedSlot("DtmargIC");
 params.AddNamedSlot("MaxdhitIC");
 params.AddNamedSlot("TangmaxIC");
 params.AddNamedSlot("TdistmaxIC");
 params.AddNamedSlot("TinvolIC");
 params.AddNamedSlot("JangmaxIC");
 params.AddNamedSlot("JiterateIC");
 params.AddNamedSlot("JdistmaxIC");
 params.AddNamedSlot("JinvolIC");
 params.AddNamedSlot("MaxmodIC");
 params.AddNamedSlot("MinmodIC");
 params.AddNamedSlot("MaxhitsIC");
 params.AddNamedSlot("VgroupIC");
 params.AddNamedSlot("AsTypeIC");
 params.AddNamedSlot("LambdaIC");
 params.AddNamedSlot("SlcIC");

 params.SetSignal(fDminIC,"DminIC");
 params.SetSignal(fDtmargIC,"DtmargIC");
 params.SetSignal(fMaxdhitIC,"MaxhitIC");
 params.SetSignal(fTangmaxIC,"TangmaxIC");
 params.SetSignal(fTdistmaxIC,"TdistmaxIC");
 params.SetSignal(fTinvolIC,"TinvolIC");
 params.SetSignal(fJangmaxIC,"JangmaxIC");
 params.SetSignal(fJiterateIC,"JiterateIC");
 params.SetSignal(fJdistmaxIC,"JdistmaxIC");
 params.SetSignal(fJinvolIC,"JinvolIC");
 params.SetSignal(fMaxmodIC,"MaxmodIC");
 params.SetSignal(fMinmodIC,"MinmodIC");
 params.SetSignal(fMaxhitsIC,"MaxhitsIC");
 params.SetSignal(fVgroupIC,"VgroupIC");
 params.SetSignal(fAsTypeIC,"AsTypeIC");
 params.SetSignal(fLambdaIC,"LambdaIC");
 params.SetSignal(fSlcIC,"SlcIC");

 params.AddNamedSlot("DminDC");
 params.AddNamedSlot("DtmargDC");
 params.AddNamedSlot("MaxdhitDC");
 params.AddNamedSlot("TangmaxDC");
 params.AddNamedSlot("TdistmaxDC");
 params.AddNamedSlot("TinvolDC");
 params.AddNamedSlot("JangmaxDC");
 params.AddNamedSlot("JiterateDC");
 params.AddNamedSlot("JdistmaxDC");
 params.AddNamedSlot("JinvolDC");
 params.AddNamedSlot("MaxmodDC");
 params.AddNamedSlot("MinmodDC");
 params.AddNamedSlot("MaxhitsDC");
 params.AddNamedSlot("VgroupDC");
 params.AddNamedSlot("AsTypeDC");
 params.AddNamedSlot("LambdaDC");
 params.AddNamedSlot("SlcDC");

 params.SetSignal(fDminDC,"DminDC");
 params.SetSignal(fDtmargDC,"DtmargDC");
 params.SetSignal(fMaxdhitDC,"MaxhitDC");
 params.SetSignal(fTangmaxDC,"TangmaxDC");
 params.SetSignal(fTdistmaxDC,"TdistmaxDC");
 params.SetSignal(fTinvolDC,"TinvolDC");
 params.SetSignal(fJangmaxDC,"JangmaxDC");
 params.SetSignal(fJiterateDC,"JiterateDC");
 params.SetSignal(fJdistmaxDC,"JdistmaxDC");
 params.SetSignal(fJinvolDC,"JinvolDC");
 params.SetSignal(fMaxmodDC,"MaxmodDC");
 params.SetSignal(fMinmodDC,"MinmodDC");
 params.SetSignal(fMaxhitsDC,"MaxhitsDC");
 params.SetSignal(fVgroupDC,"VgroupDC");
 params.SetSignal(fAsTypeDC,"AsTypeDC");
 params.SetSignal(fLambdaDC,"LambdaDC");
 params.SetSignal(fSlcDC,"SlcDC");

 params.AddNamedSlot("ConditionalReco");
 params.SetSignal(fConditional,"ConditionalReco");

 fEvt->AddDevice(params);

 // Flag to indicate that a track has been found
 // to enable condional reconstruction
 Int_t track=0;

 // Perform the various reconstructions (conditionally)
 Amanda();
 track=IceCube();
 if (!fConditional || !track) track=InIce();
 if (fConditional != 2 || !track) track=DeepCore();
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::Amanda()
{
// The direct walk track reconstruction for Amanda OM signals.

 if (fMaxhitsA<0) return;

 // Fetch all fired Amanda OMs for this event
 TObjArray* aoms=fEvt->GetDevices("IceAOM");
 if (!aoms) return;
 Int_t naoms=aoms->GetEntries();
 if (!naoms) return;

 // Check for the minimum and/or maximum number of good fired Amanda OMs
 Int_t ngood=0;
 for (Int_t iom=0; iom<naoms; iom++)
 {
  IceGOM* omx=(IceGOM*)aoms->At(iom);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  ngood++;
 } 
 if (ngood<fMinmodA || ngood>fMaxmodA) return;

 const Float_t c=0.299792458; // Light speed in vacuum in meters per ns

 // Storage of track elements.
 TObjArray tes;
 tes.SetOwner();

 NcPosition r1;
 NcPosition r2;
 Nc3Vector r12;
 Nc3Vector rsum;
 NcPosition r0;
 TObjArray hits1;
 TObjArray hits2;
 Int_t nh1,nh2;
 NcSignal* sx1=0;
 NcSignal* sx2=0;
 Float_t dist=0;
 Float_t t1,t2,dt,t0;
 Float_t dtmax;
 TObjArray hits;
 TObjArray* ordered;

 // Check the hits of Amanda OM pairs for possible track elements.
 // Also all the good hits are stored in the meantime (to save CPU time)
 // for hit association with the various track elements lateron.
 NcTrack* te=0;
 for (Int_t i1=0; i1<naoms; i1++) // First OM of the pair
 {
  IceGOM* omx1=(IceGOM*)aoms->At(i1);
  if (!omx1) continue;
  if (omx1->GetDeadValue("LE")) continue;
  r1=omx1->GetPosition();
  // Select all the good hits of this first OM
  hits1.Clear();
  // Determine the max. number of hits to be processed for this OM
  ordered=0;
  if (fMaxhitsA>0 && omx1->GetNhits()>fMaxhitsA) ordered=omx1->SortHits("LE",1,0,7);
  nh1=0;
  for (Int_t j1=1; j1<=omx1->GetNhits(); j1++)
  {
   if (ordered)
   {
    if (nh1>=fMaxhitsA) break;
    sx1=(NcSignal*)ordered->At(j1-1);
   }
   else
   {
    sx1=omx1->GetHit(j1);
   }
   if (!sx1) continue;
   if (sx1->GetDeadValue("ADC") || sx1->GetDeadValue("LE") || sx1->GetDeadValue("TOT")) continue;
   hits1.Add(sx1);
   // Also store all good hits in the total hit array
   hits.Add(sx1);
   nh1++;
  }

  // No further pair to be formed with the last OM in the list 
  if (i1==(naoms-1)) break;

  nh1=hits1.GetEntries();
  if (!nh1) continue;

  for (Int_t i2=i1+1; i2<naoms; i2++) // Second OM of the pair
  {
   IceGOM* omx2=(IceGOM*)aoms->At(i2);
   if (!omx2) continue;
   if (omx2->GetDeadValue("LE")) continue;
   r2=omx2->GetPosition();
   r12=r2-r1;
   dist=r12.GetNorm();

   if (dist<=fDminA) continue;

   // Select all the good hits of this second OM
   hits2.Clear();
   // Determine the max. number of hits to be processed for this OM
   ordered=0;
   if (fMaxhitsA>0 && omx2->GetNhits()>fMaxhitsA) ordered=omx2->SortHits("LE",1,0,7);
   nh2=0;
   for (Int_t j2=1; j2<=omx2->GetNhits(); j2++)
   {
    if (ordered)
    {
     if (nh2>=fMaxhitsA) break;
     sx2=(NcSignal*)ordered->At(j2-1);
    }
    else
    {
     sx2=omx2->GetHit(j2);
    }
    if (!sx2) continue;
    if (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT")) continue;
    hits2.Add(sx2);
    nh2++;
   }
 
   nh2=hits2.GetEntries();
   if (!nh2) continue;

   // Position r0 in between the two OMs and normalised relative direction r12
   rsum=(r1+r2)/2.;
   r0.SetPosition((Nc3Vector&)rsum);
   r12/=dist;

   // Check all hit pair combinations of these two OMs for possible track elements  
   dtmax=dist/c+float(fDtmargA);
   for (Int_t ih1=0; ih1<nh1; ih1++) // Hits of first OM
   {
    sx1=(NcSignal*)hits1.At(ih1);
    if (!sx1) continue;
    for (Int_t ih2=0; ih2<nh2; ih2++) // Hits of second OM
    {
     sx2=(NcSignal*)hits2.At(ih2);
     if (!sx2) continue;
     t1=sx1->GetSignal("LE",7);
     t2=sx2->GetSignal("LE",7);
     dt=t2-t1;
     t0=(t1+t2)/2.;

     if (fabs(dt)>=dtmax) continue;

     te=new NcTrack();
     tes.Add(te);
     if (dt<0) r12*=-1.;
     r0.SetTimestamp((NcTimestamp&)*fEvt);
     NcTimestamp* tsx=r0.GetTimestamp();
     tsx->Add(0,0,(int)t0);
     te->SetReferencePoint(r0);
     te->Set3Momentum(r12);
    }
   }
  } // end of loop over the second OM of the pair
 } // end of loop over first OM of the pair

 // Association of hits to the various track elements.
 Float_t qmax=0;
 AssociateHits(tes,hits,fVgroupA,fMaxdhitA,fAsTypeA,qmax,fLambdaA);

 // Selection on quality (Q value) in case of multiple track candidates
 SelectQvalue(tes,fAsTypeA,qmax);

 Int_t nte=tes.GetEntries();
 if (!nte) return;

 // Clustering of track candidates into jets
 TObjArray jets;
 jets.SetOwner();
 ClusterTracks(tes,jets,fTangmaxA,fTinvolA,fTdistmaxA,fAsTypeI,qmax);

 Int_t njets=jets.GetEntries();
 if (!njets) return;

 // Order the jets w.r.t. decreasing quality value
 ordered=fEvt->SortJets(-2,&jets);
 TObjArray jets2(*ordered);

 // Merging of jets
 MergeJets(jets2,fJangmaxA,fJdistmaxA,fJinvolA,fJiterateA,fAsTypeA);

 // Production and storage of the final tracks
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="A";
 TString title=ClassName();
 title+=" Amanda track";
 StoreTracks(jets2,fJangmaxA,name,title);
}
///////////////////////////////////////////////////////////////////////////
Int_t IceDwalkx::InIce()
{
// The direct walk track reconstruction for all InIce DOM signals.
// The procedure here is optimised to provide at least one direct walk track
// in most of the events (i.e. opt for somewhat lower energy threshold) at the
// expense of having a pointing accuracy slightly worse than average. 
//
// The return argument indicates whether or not a track has been found

 if (fMaxhitsI<0) return 0;

 // Fetch all fired InIce DOMs for this event
 TObjArray* idoms=fEvt->GetDevices("IceIDOM");
 if (!idoms) return 0;
 Int_t nidoms=idoms->GetEntries();
 if (!nidoms) return 0;

 // Check for the minimum and/or maximum number of good fired DOMs
 Int_t ngood=0;
 for (Int_t idom=0; idom<nidoms; idom++)
 {
  IceGOM* omx=(IceGOM*)idoms->At(idom);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  ngood++;
 } 
 if (ngood<fMinmodI || ngood>fMaxmodI) return 0;

 const Float_t c=0.299792458; // Light speed in vacuum in meters per ns

 // Storage of track elements.
 TObjArray tes;
 tes.SetOwner();

 NcPosition r1;
 NcPosition r2;
 Nc3Vector r12;
 Nc3Vector rsum;
 NcPosition r0;
 TObjArray hits1;
 TObjArray hits2;
 Int_t nh1,nh2;
 NcSignal* sx1=0;
 NcSignal* sx2=0;
 Float_t dist=0;
 Float_t t1,t2,dt,t0;
 Float_t dtmax;
 TObjArray hits;
 TObjArray* ordered;

 // Check the hits of DOM pairs for possible track elements.
 // Also all the good hits are stored in the meantime (to save CPU time)
 // for hit association with the various track elements lateron.
 NcTrack* te=0;
 for (Int_t i1=0; i1<nidoms; i1++) // First DOM of the pair
 {
  IceGOM* omx1=(IceGOM*)idoms->At(i1);
  if (!omx1) continue;
  if (omx1->GetDeadValue("LE")) continue;
  r1=omx1->GetPosition();
  // Select all the good hits of this first DOM
  hits1.Clear();
  // Determine the max. number of hits to be processed for this DOM
  ordered=0;
  if (fMaxhitsI>0 && omx1->GetNhits()>fMaxhitsI) ordered=omx1->SortHits("LE",1,0,7);
  nh1=0;
  for (Int_t j1=1; j1<=omx1->GetNhits(); j1++)
  {
   if (ordered)
   {
    if (nh1>=fMaxhitsI) break;
    sx1=(NcSignal*)ordered->At(j1-1);
   }
   else
   {
    sx1=omx1->GetHit(j1);
   }
   if (!sx1) continue;
   if (sx1->GetDeadValue("ADC") || sx1->GetDeadValue("LE") || sx1->GetDeadValue("TOT")) continue;
   if (!fSlcI && sx1->GetSignal("SLC")) continue; 
   hits1.Add(sx1);
   // Also store all good hits in the total hit array
   hits.Add(sx1);
   nh1++;
  }

  // No further pair to be formed with the last DOM in the list 
  if (i1==(nidoms-1)) break;

  nh1=hits1.GetEntries();
  if (!nh1) continue;

  for (Int_t i2=i1+1; i2<nidoms; i2++) // Second DOM of the pair
  {
   IceGOM* omx2=(IceGOM*)idoms->At(i2);
   if (!omx2) continue;
   if (omx2->GetDeadValue("LE")) continue;
   r2=omx2->GetPosition();
   r12=r2-r1;
   dist=r12.GetNorm();

   if (dist<=fDminI) continue;

   // Select all the good hits of this second DOM
   hits2.Clear();
   // Determine the max. number of hits to be processed for this DOM
   ordered=0;
   if (fMaxhitsI>0 && omx2->GetNhits()>fMaxhitsI) ordered=omx2->SortHits("LE",1,0,7);
   nh2=0;
   for (Int_t j2=1; j2<=omx2->GetNhits(); j2++)
   {
    if (ordered)
    {
     if (nh2>=fMaxhitsI) break;
     sx2=(NcSignal*)ordered->At(j2-1);
    }
    else
    {
     sx2=omx2->GetHit(j2);
    }
    if (!sx2) continue;
    if (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT")) continue;
    if (!fSlcI && sx2->GetSignal("SLC")) continue; 
    hits2.Add(sx2);
    nh2++;
   }
 
   nh2=hits2.GetEntries();
   if (!nh2) continue;

   // Position r0 in between the two DOMs and normalised relative direction r12
   rsum=(r1+r2)/2.;
   r0.SetPosition((Nc3Vector&)rsum);
   r12/=dist;

   // Check all hit pair combinations of these two DOMs for possible track elements  
   dtmax=dist/c+float(fDtmargI);
   for (Int_t ih1=0; ih1<nh1; ih1++) // Hits of first DOM
   {
    sx1=(NcSignal*)hits1.At(ih1);
    if (!sx1) continue;
    for (Int_t ih2=0; ih2<nh2; ih2++) // Hits of second DOM
    {
     sx2=(NcSignal*)hits2.At(ih2);
     if (!sx2) continue;
     t1=sx1->GetSignal("LE",7);
     t2=sx2->GetSignal("LE",7);
     dt=t2-t1;
     t0=(t1+t2)/2.;

     if (fabs(dt)>=dtmax) continue;

     te=new NcTrack();
     tes.Add(te);
     if (dt<0) r12*=-1.;
     r0.SetTimestamp((NcTimestamp&)*fEvt);
     NcTimestamp* tsx=r0.GetTimestamp();
     tsx->Add(0,0,(int)t0);
     te->SetReferencePoint(r0);
     te->Set3Momentum(r12);
    }
   }
  } // end of loop over the second DOM of the pair
 } // end of loop over first DOM of the pair

 // Association of hits to the various track elements.
 Float_t qmax=0;
 AssociateHits(tes,hits,fVgroupI,fMaxdhitI,fAsTypeI,qmax,fLambdaI);

 // Selection on quality (Q value) in case of multiple track candidates
 SelectQvalue(tes,fAsTypeI,qmax);

 Int_t nte=tes.GetEntries();
 if (!nte) return 0;

 // Clustering of track candidates into jets
 TObjArray jets;
 jets.SetOwner();
 ClusterTracks(tes,jets,fTangmaxI,fTinvolI,fTdistmaxI,fAsTypeI,qmax);

 Int_t njets=jets.GetEntries();
 if (!njets) return 0;

 // Order the jets w.r.t. decreasing quality value
 ordered=fEvt->SortJets(-2,&jets);
 TObjArray jets2(*ordered);

 // Merging of jets
 MergeJets(jets2,fJangmaxI,fJdistmaxI,fJinvolI,fJiterateI,fAsTypeI);

 // Production and storage of the final tracks
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="I";
 TString title=ClassName();
 title+=" InIce track";
 StoreTracks(jets2,fJangmaxI,name,title);

 return 1;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceDwalkx::IceCube()
{
// The direct walk track reconstruction for the standard IceCube InIce DOM signals.
// The procedure here is optimised to provide a direct walk track with better than average
// pointing accuracy (i.e. slightly higher energy threshold) on the expense of sometimes
// not making a reco track at all.
//
// The return argument indicates whether or not a track has been found

 if (fMaxhitsIC<0) return 0;

 // Fetch all fired standard IceCube InIce DOMs for this event
 TObjArray* idoms=fEvt->GetDevices("IceICDOM");
 if (!idoms) return 0;
 Int_t nidoms=idoms->GetEntries();
 if (!nidoms) return 0;

 // Check for the minimum and/or maximum number of good fired DOMs
 Int_t ngood=0;
 for (Int_t idom=0; idom<nidoms; idom++)
 {
  IceGOM* omx=(IceGOM*)idoms->At(idom);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  ngood++;
 } 
 if (ngood<fMinmodIC || ngood>fMaxmodIC) return 0;

 const Float_t c=0.299792458; // Light speed in vacuum in meters per ns

 // Storage of track elements.
 TObjArray tes;
 tes.SetOwner();

 NcPosition r1;
 NcPosition r2;
 Nc3Vector r12;
 Nc3Vector rsum;
 NcPosition r0;
 TObjArray hits1;
 TObjArray hits2;
 Int_t nh1,nh2;
 NcSignal* sx1=0;
 NcSignal* sx2=0;
 Float_t dist=0;
 Float_t t1,t2,dt,t0;
 Float_t dtmax;
 TObjArray hits;
 TObjArray* ordered;

 // Check the hits of DOM pairs for possible track elements.
 // Also all the good hits are stored in the meantime (to save CPU time)
 // for hit association with the various track elements lateron.
 NcTrack* te=0;
 for (Int_t i1=0; i1<nidoms; i1++) // First DOM of the pair
 {
  IceGOM* omx1=(IceGOM*)idoms->At(i1);
  if (!omx1) continue;
  if (omx1->GetDeadValue("LE")) continue;
  r1=omx1->GetPosition();
  // Select all the good hits of this first DOM
  hits1.Clear();
  // Determine the max. number of hits to be processed for this DOM
  ordered=0;
  if (fMaxhitsIC>0 && omx1->GetNhits()>fMaxhitsIC) ordered=omx1->SortHits("LE",1,0,7);
  nh1=0;
  for (Int_t j1=1; j1<=omx1->GetNhits(); j1++)
  {
   if (ordered)
   {
    if (nh1>=fMaxhitsIC) break;
    sx1=(NcSignal*)ordered->At(j1-1);
   }
   else
   {
    sx1=omx1->GetHit(j1);
   }
   if (!sx1) continue;
   if (sx1->GetDeadValue("ADC") || sx1->GetDeadValue("LE") || sx1->GetDeadValue("TOT")) continue;
   if (!fSlcIC && sx1->GetSignal("SLC")) continue; 
   hits1.Add(sx1);
   // Also store all good hits in the total hit array
   hits.Add(sx1);
   nh1++;
  }

  // No further pair to be formed with the last DOM in the list 
  if (i1==(nidoms-1)) break;

  nh1=hits1.GetEntries();
  if (!nh1) continue;

  for (Int_t i2=i1+1; i2<nidoms; i2++) // Second DOM of the pair
  {
   IceGOM* omx2=(IceGOM*)idoms->At(i2);
   if (!omx2) continue;
   if (omx2->GetDeadValue("LE")) continue;
   r2=omx2->GetPosition();
   r12=r2-r1;
   dist=r12.GetNorm();

   if (dist<=fDminIC) continue;

   // Select all the good hits of this second DOM
   hits2.Clear();
   // Determine the max. number of hits to be processed for this DOM
   ordered=0;
   if (fMaxhitsIC>0 && omx2->GetNhits()>fMaxhitsIC) ordered=omx2->SortHits("LE",1,0,7);
   nh2=0;
   for (Int_t j2=1; j2<=omx2->GetNhits(); j2++)
   {
    if (ordered)
    {
     if (nh2>=fMaxhitsIC) break;
     sx2=(NcSignal*)ordered->At(j2-1);
    }
    else
    {
     sx2=omx2->GetHit(j2);
    }
    if (!sx2) continue;
    if (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT")) continue;
    if (!fSlcIC && sx2->GetSignal("SLC")) continue; 
    hits2.Add(sx2);
    nh2++;
   }
 
   nh2=hits2.GetEntries();
   if (!nh2) continue;

   // Position r0 in between the two DOMs and normalised relative direction r12
   rsum=(r1+r2)/2.;
   r0.SetPosition((Nc3Vector&)rsum);
   r12/=dist;

   // Check all hit pair combinations of these two DOMs for possible track elements  
   dtmax=dist/c+float(fDtmargIC);
   for (Int_t ih1=0; ih1<nh1; ih1++) // Hits of first DOM
   {
    sx1=(NcSignal*)hits1.At(ih1);
    if (!sx1) continue;
    for (Int_t ih2=0; ih2<nh2; ih2++) // Hits of second DOM
    {
     sx2=(NcSignal*)hits2.At(ih2);
     if (!sx2) continue;
     t1=sx1->GetSignal("LE",7);
     t2=sx2->GetSignal("LE",7);
     dt=t2-t1;
     t0=(t1+t2)/2.;

     if (fabs(dt)>=dtmax) continue;

     te=new NcTrack();
     tes.Add(te);
     if (dt<0) r12*=-1.;
     r0.SetTimestamp((NcTimestamp&)*fEvt);
     NcTimestamp* tsx=r0.GetTimestamp();
     tsx->Add(0,0,(int)t0);
     te->SetReferencePoint(r0);
     te->Set3Momentum(r12);
    }
   }
  } // end of loop over the second DOM of the pair
 } // end of loop over first DOM of the pair

 // Association of hits to the various track elements.
 Float_t qmax=0;
 AssociateHits(tes,hits,fVgroupIC,fMaxdhitIC,fAsTypeIC,qmax,fLambdaIC);

 // Selection on quality (Q value) in case of multiple track candidates
 SelectQvalue(tes,fAsTypeIC,qmax);

 Int_t nte=tes.GetEntries();
 if (!nte) return 0;

 // Clustering of track candidates into jets
 TObjArray jets;
 jets.SetOwner();
 ClusterTracks(tes,jets,fTangmaxIC,fTinvolIC,fTdistmaxIC,fAsTypeIC,qmax);

 Int_t njets=jets.GetEntries();
 if (!njets) return 0;

 // Order the jets w.r.t. decreasing quality value
 ordered=fEvt->SortJets(-2,&jets);
 TObjArray jets2(*ordered);

 // Merging of jets
 MergeJets(jets2,fJangmaxIC,fJdistmaxIC,fJinvolIC,fJiterateIC,fAsTypeIC);

 // Production and storage of the final tracks
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="IC";
 TString title=ClassName();
 title+=" standard IceCube track";
 StoreTracks(jets2,fJangmaxI,name,title);

 return 1;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceDwalkx::DeepCore()
{
// The direct walk track reconstruction for the DeepCore DOM signals.
// The procedure here is optimised to provide a direct walk track with the
// lowest possible energy threshold at the expense of having a worse than average
// pointing accuracy at these low energies.
//
// The return argument indicates whether or not a track has been found

 if (fMaxhitsDC<0) return 0;

 // Fetch all fired DeepCore DOMs for this event
 TObjArray* idoms=fEvt->GetDevices("IceDCDOM");
 if (!idoms) return 0;
 Int_t nidoms=idoms->GetEntries();
 if (!nidoms) return 0;

 // Check for the minimum and/or maximum number of good fired DOMs
 Int_t ngood=0;
 for (Int_t idom=0; idom<nidoms; idom++)
 {
  IceGOM* omx=(IceGOM*)idoms->At(idom);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  ngood++;
 } 
 if (ngood<fMinmodDC || ngood>fMaxmodDC) return 0;

 const Float_t c=0.299792458; // Light speed in vacuum in meters per ns

 // Storage of track elements.
 TObjArray tes;
 tes.SetOwner();

 NcPosition r1;
 NcPosition r2;
 Nc3Vector r12;
 Nc3Vector rsum;
 NcPosition r0;
 TObjArray hits1;
 TObjArray hits2;
 Int_t nh1,nh2;
 NcSignal* sx1=0;
 NcSignal* sx2=0;
 Float_t dist=0;
 Float_t t1,t2,dt,t0;
 Float_t dtmax;
 TObjArray hits;
 TObjArray* ordered;

 // Check the hits of DOM pairs for possible track elements.
 // Also all the good hits are stored in the meantime (to save CPU time)
 // for hit association with the various track elements lateron.
 NcTrack* te=0;
 for (Int_t i1=0; i1<nidoms; i1++) // First DOM of the pair
 {
  IceGOM* omx1=(IceGOM*)idoms->At(i1);
  if (!omx1) continue;
  if (omx1->GetDeadValue("LE")) continue;
  r1=omx1->GetPosition();
  // Select all the good hits of this first DOM
  hits1.Clear();
  // Determine the max. number of hits to be processed for this DOM
  ordered=0;
  if (fMaxhitsDC>0 && omx1->GetNhits()>fMaxhitsDC) ordered=omx1->SortHits("LE",1,0,7);
  nh1=0;
  for (Int_t j1=1; j1<=omx1->GetNhits(); j1++)
  {
   if (ordered)
   {
    if (nh1>=fMaxhitsDC) break;
    sx1=(NcSignal*)ordered->At(j1-1);
   }
   else
   {
    sx1=omx1->GetHit(j1);
   }
   if (!sx1) continue;
   if (sx1->GetDeadValue("ADC") || sx1->GetDeadValue("LE") || sx1->GetDeadValue("TOT")) continue;
   if (!fSlcDC && sx1->GetSignal("SLC")) continue; 
   hits1.Add(sx1);
   // Also store all good hits in the total hit array
   hits.Add(sx1);
   nh1++;
  }

  // No further pair to be formed with the last DOM in the list 
  if (i1==(nidoms-1)) break;

  nh1=hits1.GetEntries();
  if (!nh1) continue;

  for (Int_t i2=i1+1; i2<nidoms; i2++) // Second DOM of the pair
  {
   IceGOM* omx2=(IceGOM*)idoms->At(i2);
   if (!omx2) continue;
   if (omx2->GetDeadValue("LE")) continue;
   r2=omx2->GetPosition();
   r12=r2-r1;
   dist=r12.GetNorm();

   if (dist<=fDminDC) continue;

   // Select all the good hits of this second DOM
   hits2.Clear();
   // Determine the max. number of hits to be processed for this DOM
   ordered=0;
   if (fMaxhitsDC>0 && omx2->GetNhits()>fMaxhitsDC) ordered=omx2->SortHits("LE",1,0,7);
   nh2=0;
   for (Int_t j2=1; j2<=omx2->GetNhits(); j2++)
   {
    if (ordered)
    {
     if (nh2>=fMaxhitsDC) break;
     sx2=(NcSignal*)ordered->At(j2-1);
    }
    else
    {
     sx2=omx2->GetHit(j2);
    }
    if (!sx2) continue;
    if (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT")) continue;
    if (!fSlcDC && sx2->GetSignal("SLC")) continue; 
    hits2.Add(sx2);
    nh2++;
   }
 
   nh2=hits2.GetEntries();
   if (!nh2) continue;

   // Position r0 in between the two DOMs and normalised relative direction r12
   rsum=(r1+r2)/2.;
   r0.SetPosition((Nc3Vector&)rsum);
   r12/=dist;

   // Check all hit pair combinations of these two DOMs for possible track elements  
   dtmax=dist/c+float(fDtmargDC);
   for (Int_t ih1=0; ih1<nh1; ih1++) // Hits of first DOM
   {
    sx1=(NcSignal*)hits1.At(ih1);
    if (!sx1) continue;
    for (Int_t ih2=0; ih2<nh2; ih2++) // Hits of second DOM
    {
     sx2=(NcSignal*)hits2.At(ih2);
     if (!sx2) continue;
     t1=sx1->GetSignal("LE",7);
     t2=sx2->GetSignal("LE",7);
     dt=t2-t1;
     t0=(t1+t2)/2.;

     if (fabs(dt)>=dtmax) continue;

     te=new NcTrack();
     tes.Add(te);
     if (dt<0) r12*=-1.;
     r0.SetTimestamp((NcTimestamp&)*fEvt);
     NcTimestamp* tsx=r0.GetTimestamp();
     tsx->Add(0,0,(int)t0);
     te->SetReferencePoint(r0);
     te->Set3Momentum(r12);
    }
   }
  } // end of loop over the second DOM of the pair
 } // end of loop over first DOM of the pair

 // Association of hits to the various track elements.
 Float_t qmax=0;
 AssociateHits(tes,hits,fVgroupDC,fMaxdhitDC,fAsTypeDC,qmax,fLambdaDC);

 // Selection on quality (Q value) in case of multiple track candidates
 SelectQvalue(tes,fAsTypeDC,qmax);

 Int_t nte=tes.GetEntries();
 if (!nte) return 0;

 // Clustering of track candidates into jets
 TObjArray jets;
 jets.SetOwner();
 ClusterTracks(tes,jets,fTangmaxDC,fTinvolDC,fTdistmaxDC,fAsTypeDC,qmax);

 Int_t njets=jets.GetEntries();
 if (!njets) return 0;

 // Order the jets w.r.t. decreasing quality value
 ordered=fEvt->SortJets(-2,&jets);
 TObjArray jets2(*ordered);

 // Merging of jets
 MergeJets(jets2,fJangmaxDC,fJdistmaxDC,fJinvolDC,fJiterateDC,fAsTypeDC);

 // Production and storage of the final tracks
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="DC";
 TString title=ClassName();
 title+=" DeepCore track";
 StoreTracks(jets2,fJangmaxI,name,title);

 return 1;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::AssociateHits(TObjArray& tes,TObjArray& hits,Int_t vgroup,Float_t maxdhit,Int_t astype,Float_t& qmax,Float_t lambda)
{
 // Association of hits to the various track elements.

 const Float_t pi=acos(-1.);
 const Float_t c=0.299792458;         // Light speed in vacuum in meters per ns
 const Float_t npice=1.31768387;      // Phase refractive index (c/v_phase) of ice
 const Float_t ngice=1.35075806;      // Group refractive index (c/v_group) of ice
 const Float_t thetac=acos(1./npice); // Cherenkov angle (in radians)

 // Angular reduction of complement of thetac due to v_phase and v_group difference
 Float_t alphac=0;
 if (vgroup) alphac=atan((1.-npice/ngice)/sqrt(npice*npice-1.));

 Int_t nte=tes.GetEntries();
 Int_t nh=hits.GetEntries();
 Float_t d=0;
 Nc3Vector p;
 NcPosition r1;
 NcPosition r2;
 Nc3Vector r12;
 Float_t t1;
 Float_t dist,t0,tgeo,tres;
 NcSample levers;      // Statistics of the assoc. hit lever arms
 levers.SetStoreMode(1);// Enable median calculation
 NcSample hprojs;      // Statistics of the assoc. hit position projections on the track w.r.t. r0
 hprojs.SetStoreMode(1);// Enable median calculation
 NcSample times;       // Statistics of the time residuals of the associated hits
 times.SetStoreMode(1); // Enable median calculation
 NcSignal fit;         // Storage of Q value etc... for each track candidate
 fit.AddNamedSlot("QTC");
 fit.AddNamedSlot("Nstrings");
 fit.AddNamedSlot("SpanL");
 fit.AddNamedSlot("MedianL");
 fit.AddNamedSlot("MeanL");
 fit.AddNamedSlot("SigmaL");
 fit.AddNamedSlot("SpreadL");
 fit.AddNamedSlot("ExpSpreadL");
 fit.AddNamedSlot("Span");
 fit.AddNamedSlot("Median");
 fit.AddNamedSlot("Mean");
 fit.AddNamedSlot("Sigma");
 fit.AddNamedSlot("Spread");
 fit.AddNamedSlot("ExpSpread");
 fit.AddNamedSlot("MedianT");
 fit.AddNamedSlot("MeanT");
 fit.AddNamedSlot("SigmaT");
 fit.AddNamedSlot("SpreadT");
 fit.AddNamedSlot("term1");
 fit.AddNamedSlot("term2");
 fit.AddNamedSlot("term3");
 fit.AddNamedSlot("term4");
 fit.AddNamedSlot("term5");
 Int_t nah=0;   // Number of associated hits
 Int_t nas=0;   // Number of associated strings
 Float_t qtc=0; // Quality parameter for the track (candidate)
 Float_t nax=0; // Associated hits and/or strings value to build the QTC for a certain TE
 Float_t lmin,lmax,spanl,medianl,meanl,sigmal,spreadl,expspreadl;
 Float_t hproj,hprojmin,hprojmax,span,median,mean,sigma,spread,expspread;
 Float_t mediant,meant,sigmat,spreadt;
 Float_t term1,term2,term3,term4,term5;
 qmax=0;
 for (Int_t jte=0; jte<nte; jte++)
 {
  NcTrack* te=(NcTrack*)tes.At(jte);
  if (!te) continue;
  NcPosition* tr0=te->GetReferencePoint();
  NcTimestamp* tt0=tr0->GetTimestamp();
  t0=fEvt->GetDifference(tt0,"ns");
  p=te->Get3Momentum();
  levers.Reset();
  hprojs.Reset();
  times.Reset();
  for (Int_t jh=0; jh<nh; jh++)
  {
   NcSignal* sx1=(NcSignal*)hits.At(jh);
   if (!sx1) continue;
   IceGOM* omx=(IceGOM*)sx1->GetDevice();
   if (!omx) continue;
   r1=omx->GetPosition();
   d=te->GetDistance(r1);
   r12=r1-(*tr0);
   hproj=p.Dot(r12);
   dist=hproj+d/tan(pi/2.-thetac-alphac);
   tgeo=t0+dist/c;
   t1=sx1->GetSignal("LE",7);
   tres=t1-tgeo;

   d=d/sin(thetac); // The distance traveled by a cherenkov photon

   if (tres<-30 || tres>300 || d>maxdhit*lambda) continue;

   // Associate this hit to the TE
   te->AddSignal(*sx1);
   levers.Enter(fabs(hproj));
   hprojs.Enter(hproj);
   times.Enter(tres);
  }

  // Determine the Q quality of the various TE's.
  // Good quality TE's will be called track candidates (TC's)
  nah=te->GetNsignals();
  nas=fEvt->GetNstrings(*te,"IceGOM");
  nax=0;
  if (astype==1) nax=nah;
  if (astype==2) nax=nas;
  if (astype==3) nax=nah*nas;
  lmin=levers.GetMinimum(1);
  lmax=levers.GetMaximum(1);
  spanl=lmax-lmin;
  medianl=levers.GetMedian(1);
  meanl=levers.GetMean(1);
  sigmal=levers.GetSigma(1);
  spreadl=levers.GetSpread(1);
  // Expected spread for a flat distribution
  expspreadl=0;
  if (spanl>0) expspreadl=(0.5*pow(lmin,2)+0.5*pow(lmax,2)+pow(medianl,2)-medianl*(lmin+lmax))/spanl;
  hprojmin=hprojs.GetMinimum(1);
  hprojmax=hprojs.GetMaximum(1);
  span=hprojmax-hprojmin;
  median=hprojs.GetMedian(1);
  mean=hprojs.GetMean(1);
  sigma=hprojs.GetSigma(1);
  spread=hprojs.GetSpread(1);
  // Expected spread for a flat distribution
  expspread=0;
  if (span>0) expspread=(0.5*pow(hprojmin,2)+0.5*pow(hprojmax,2)+pow(median,2)-median*(hprojmin+hprojmax))/span;
  mediant=times.GetMedian(1);
  meant=times.GetMean(1);
  sigmat=times.GetSigma(1);
  spreadt=times.GetSpread(1);

  term1=0;
  if (span>0) term1=2.*spread/span;

  term2=0;
  if (spanl>0) term2=2.*spreadl/spanl;

  term3=0;
  if (spread>0) term3=fabs(spread-expspread)/spread;

  term4=0;
  if (spreadl>0) term4=fabs(spreadl-expspreadl)/spreadl;

  term5=0;
  if (spreadt>0) term5=fabs(mediant)/spreadt;

  qtc=nax*(term1+term2)-term3-term4-term5;
  if (fabs(median)>span/2.) qtc=0; // Require projected hits on both sides of r0

  fit.SetSignal(qtc,"QTC");
  fit.SetSignal(nas,"Nstrings");
  fit.SetSignal(spanl,"SpanL");
  fit.SetSignal(medianl,"MedianL");
  fit.SetSignal(meanl,"MeanL");
  fit.SetSignal(sigmal,"SigmaL");
  fit.SetSignal(spreadl,"SpreadL");
  fit.SetSignal(expspreadl,"ExpSpreadL");
  fit.SetSignal(span,"Span");
  fit.SetSignal(median,"Median");
  fit.SetSignal(mean,"Mean");
  fit.SetSignal(sigma,"Sigma");
  fit.SetSignal(spread,"Spread");
  fit.SetSignal(expspread,"ExpSpread");
  fit.SetSignal(mediant,"MedianT");
  fit.SetSignal(meant,"MeanT");
  fit.SetSignal(sigmat,"SigmaT");
  fit.SetSignal(spreadt,"SpreadT");
  fit.SetSignal(term1,"term1");
  fit.SetSignal(term2,"term2");
  fit.SetSignal(term3,"term3");
  fit.SetSignal(term4,"term4");
  fit.SetSignal(term5,"term5");
  te->SetFitDetails(fit);
  if (qtc>qmax) qmax=qtc;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::SelectQvalue(TObjArray& tes,Int_t astype,Float_t qmax)
{
 // Perform selection on Q value in case of multiple track candidates

 Int_t nte=tes.GetEntries();
 Int_t nah=0;
 Int_t nas=0;
 Float_t nax=0;
 Float_t qtc=0;
 Nc3Vector p;
 for (Int_t jtc=0; jtc<nte; jtc++)
 {
  NcTrack* te=(NcTrack*)tes.At(jtc);
  if (!te) continue;
  NcSignal* sx1=(NcSignal*)te->GetFitDetails();
  qtc=-1;
  nah=te->GetNsignals();
  nas=0;
  if (sx1)
  {
   qtc=sx1->GetSignal("QTC");
   nas=int(sx1->GetSignal("Nstrings"));
  }
  nax=0;
  if (astype==1) nax=nah;
  if (astype==2) nax=nas;
  if (astype==3) nax=nah*nas;

  if (nax<=0 || qtc<0.8*qmax)
  {
   tes.RemoveAt(jtc);
   delete te;
  }
  else // Set Q value as momentum to provide a weight for jet clustering
  {
   if (qtc>0)
   {
    p=te->Get3Momentum();
    p*=qtc;
    te->Set3Momentum(p);
   }
  }
 } 
 tes.Compress();
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::ClusterTracks(TObjArray& tes,TObjArray& jets,Float_t tangmax,Int_t tinvol,Float_t tdistmax,Int_t astype,Float_t qmax)
{
 // Cluster track candidates within a certain opening angle into jets.
 // Also the track should be within a certain maximum distance of the
 // starting track in order to get clustered.
 // The latter prevents clustering of (nearly) parallel track candidates
 // crossing the detector a very different locations (e.g. muon bundles).
 // The average r0 and t0 of the constituent tracks will be taken as the
 // jet reference point. 

 Int_t nte=tes.GetEntries();
 Float_t ang=0;
 NcSample pos;
 NcSample time;
 Float_t vec[3],err[3];
 NcPosition r0;
 Float_t t0,dist,dist2;
 Int_t nah=0; // Number of associated hits
 Int_t nas=0; // Number of associated strings
 Float_t nax=0,naxmax=0; // Associated hits and/or strings value for the jets
 Float_t qtc;
 for (Int_t jtc1=0; jtc1<nte; jtc1++)
 {
  NcTrack* te=(NcTrack*)tes.At(jtc1);
  if (!te) continue;
  NcPosition* x1=te->GetReferencePoint();
  if (!x1) continue;
  NcTimestamp* ts1=x1->GetTimestamp();
  if (!ts1) continue;
  NcJet* jx=new NcJet();
  jx->AddTrack(te);
  pos.Reset();
  time.Reset();
  x1->GetPosition(vec,"car");
  pos.Enter(vec[0],vec[1],vec[2]);
  t0=fEvt->GetDifference(ts1,"ns");
  time.Enter(t0);
  for (Int_t jtc2=0; jtc2<nte; jtc2++)
  {
   if (jtc2==jtc1) continue;
   NcTrack* te2=(NcTrack*)tes.At(jtc2);
   if (!te2) continue;
   ang=te->GetOpeningAngle(*te2,"deg");
   if (ang<=tangmax)
   {
    NcPosition* x2=te2->GetReferencePoint();
    if (!x2) continue;
    NcTimestamp* ts2=x2->GetTimestamp();
    if (!ts2) continue;
    if (!tinvol)
    {
     dist=te->GetDistance(te2);
    }
    else
    {
     dist=te->GetDistance(x2);
     dist2=te2->GetDistance(x1);
     if (dist2<dist) dist=dist2;
    }
    if (dist<=tdistmax)
    {
     x2->GetPosition(vec,"car");
     pos.Enter(vec[0],vec[1],vec[2]);
     t0=fEvt->GetDifference(ts2,"ns");
     time.Enter(t0);
     jx->AddTrack(te2);
    }
   }
  }

  // Set the reference point data for this jet
  for (Int_t j=1; j<=3; j++)
  {
   vec[j-1]=pos.GetMean(j);
   err[j-1]=pos.GetSigma(j);
  }
  r0.SetPosition(vec,"car");
  r0.SetPositionErrors(err,"car");
  r0.SetTimestamp((NcTimestamp&)*fEvt);
  NcTimestamp* jt0=r0.GetTimestamp();
  t0=time.GetMean(1);
  jt0->Add(0,0,(int)t0);
  jx->SetReferencePoint(r0);

  // Store this jet for further processing if ntracks>1
  if (jx->GetNtracks() > 1 || tangmax<=0)
  {
   jets.Add(jx);
   nah=jx->GetNsignals();
   nas=fEvt->GetNstrings(*jx,"IceGOM");
   nax=0;
   if (astype==1) nax=nah;
   if (astype==2) nax=nas;
   if (astype==3) nax=nah*nas;
   if (nax>naxmax) naxmax=nax;
  }
  else // Only keep single-track jets which have qtc=qmax 
  {
   NcSignal* sx1=(NcSignal*)te->GetFitDetails();
   qtc=-1;
   if (sx1) qtc=sx1->GetSignal("QTC");
   if (qtc>=(qmax-1.e-10))
   {
    jets.Add(jx);
    nah=jx->GetNsignals();
    nas=fEvt->GetNstrings(*jx,"IceGOM");
    nax=0;
    if (astype==1) nax=nah;
    if (astype==2) nax=nas;
    if (astype==3) nax=nah*nas;
    if (nax>naxmax) naxmax=nax;
   }
   else
   {
    delete jx;
   }
  }
 }

 Int_t njets=jets.GetEntries();
 if (!njets) return;

 // The sum of 0.15*(nax-naxmax) and average qtc value per track for each jet
 // will be stored as the jet energy to enable sorting on this value lateron
 Float_t sortval=0;
 Int_t ntk=0;
 for (Int_t ijet=0; ijet<njets; ijet++)
 {
  NcJet* jx=(NcJet*)jets.At(ijet);
  if (!jx) continue;
  nah=jx->GetNsignals();
  nas=fEvt->GetNstrings(*jx,"IceGOM");
  nax=0;
  if (astype==1) nax=nah;
  if (astype==2) nax=nas;
  if (astype==3) nax=nah*nas;
  ntk=jx->GetNtracks();
  sortval=0.15*(nax-naxmax);
  if (ntk) sortval+=jx->GetMomentum()/float(ntk);
  jx->SetScalar(sortval);
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::MergeJets(TObjArray& jets2,Float_t jangmax,Float_t jdistmax,Int_t jinvol,Int_t jiterate,Int_t astype)
{
 // Merge jets within a certain opening to provide the final track(s).
 // Also the jet should be within a certain maximum distance of the
 // starting jet in order to get merged.
 // The latter prevents merging of (nearly) parallel jets/tracks
 // crossing the detector a very different locations (e.g. muon bundles).
 // The average r0 and t0 of the constituent jets will be taken as the
 // final reference point. 

 Int_t njets=jets2.GetEntries();
 NcJet* jx1=0;
 NcJet* jx2=0;
 Int_t merged=1;
 Int_t ntk;
 Int_t nah=0;
 Int_t nas=0;
 Float_t nax=0,naxmax=0;
 Float_t ang,dist,dist2,t0;
 NcSample pos;
 NcSample time;
 NcPosition r0;
 Float_t vec[3],err[3];
 Float_t sortval;
 if (jangmax>=0)
 {
  while (merged)
  {
   merged=0;
   naxmax=0;
   for (Int_t jet1=0; jet1<njets; jet1++)
   {
    jx1=(NcJet*)jets2.At(jet1);
    if (!jx1) continue;
    NcPosition* x1=jx1->GetReferencePoint();
    if (!x1) continue;
    NcTimestamp* ts1=x1->GetTimestamp();
    if (!ts1) continue;
    pos.Reset();
    time.Reset();
    x1->GetPosition(vec,"car");
    pos.Enter(vec[0],vec[1],vec[2]);
    t0=fEvt->GetDifference(ts1,"ns");
    time.Enter(t0);
    for (Int_t jet2=0; jet2<njets; jet2++)
    {
     jx2=(NcJet*)jets2.At(jet2);
     if (!jx2 || jet2==jet1) continue;
     NcPosition* x2=jx2->GetReferencePoint();
     if (!x2) continue;
     NcTimestamp* ts2=x2->GetTimestamp();
     if (!ts2) continue;
     ang=jx1->GetOpeningAngle(*jx2,"deg");
     if (ang<=jangmax)
     {
      if (!jinvol)
      {
       dist=jx1->GetDistance(jx2);
      }
      else
      {
       dist=jx1->GetDistance(x2);
       dist2=jx2->GetDistance(x1);
       if (dist2<dist) dist=dist2;
      }
      if (dist<=jdistmax)
      {
       x2->GetPosition(vec,"car");
       pos.Enter(vec[0],vec[1],vec[2]);
       t0=fEvt->GetDifference(ts2,"ns");
       time.Enter(t0);
       for (Int_t jtk=1; jtk<=jx2->GetNtracks(); jtk++)
       {
        NcTrack* te=jx2->GetTrack(jtk);
        if (!te) continue;
        jx1->AddTrack(te);
       }
       jets2.RemoveAt(jet2);
       if (jiterate) merged=1;
      }
     }
    } // End of jet2 loop

    // Set the reference point data for this jet
    for (Int_t k=1; k<=3; k++)
    {
     vec[k-1]=pos.GetMean(k);
     err[k-1]=pos.GetSigma(k);
    }
    r0.SetPosition(vec,"car");
    r0.SetPositionErrors(err,"car");
    r0.SetTimestamp((NcTimestamp&)*fEvt);
    NcTimestamp* jt0=r0.GetTimestamp();
    t0=time.GetMean(1);
    jt0->Add(0,0,(int)t0);
    jx1->SetReferencePoint(r0);

    nah=jx1->GetNsignals();
    nas=fEvt->GetNstrings(*jx1,"IceGOM");
    nax=0;
    if (astype==1) nax=nah;
    if (astype==2) nax=nas;
    if (astype==3) nax=nah*nas;
    if (nax>naxmax) naxmax=nax;
   } // End of jet1 loop

   jets2.Compress();

   // The sum of 0.15*(nax-naxmax) and average qtc value per track for each jet
   // will be stored as the jet energy to enable sorting on this value
   for (Int_t jjet=0; jjet<njets; jjet++)
   {
    NcJet* jx=(NcJet*)jets2.At(jjet);
    if (!jx) continue;
    nah=jx->GetNsignals();
    nas=fEvt->GetNstrings(*jx,"IceGOM");
    nax=0;
    if (astype==1) nax=nah;
    if (astype==2) nax=nas;
    if (astype==3) nax=nah*nas;
    ntk=jx->GetNtracks();
    sortval=0.15*float(nax-naxmax);
    if (ntk) sortval+=jx->GetMomentum()/float(ntk);
    jx->SetScalar(sortval);
   }

   // Order the jets w.r.t. decreasing quality value
   TObjArray* ordered=fEvt->SortJets(-2,&jets2);
   njets=ordered->GetEntries();
   jets2.Clear();
   for (Int_t icopy=0; icopy<njets; icopy++)
   {
    jets2.Add(ordered->At(icopy));
   }
  } // End of iterative while loop
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalkx::StoreTracks(TObjArray& jets2,Float_t jangmax,TString name,TString title)
{
 // Store every jet as a reconstructed track in the event structure.
 // The jet 3-momentum (normalised to 1) and reference point
 // (i.e.the average r0 and t0 of the constituent tracks) will make up
 // the final track parameters.
 // All the associated hits of all the constituent tracks of the jet
 // will be associated to the final track.
 // In case the jet angular separation was set <0, only the jet with
 // the maximum number of tracks (i.e. the first one in the array)
 // will be used to form a track. This will allow comparison with
 // the standard Sieglinde processing.

 Int_t njets=jets2.GetEntries();
 NcTrack t; 
 t.SetNameTitle(name.Data(),title.Data());
 t.SetCharge(fCharge);
 Nc3Vector p;
 for (Int_t jet=0; jet<njets; jet++)
 {
  NcJet* jx=(NcJet*)jets2.At(jet);
  if (!jx) continue;
  NcPosition* ref=jx->GetReferencePoint();
  if (!ref) continue;
  fEvt->AddTrack(t);
  NcTrack* trk=fEvt->GetTrack(fEvt->GetNtracks());
  if (!trk) continue;
  trk->SetId(fEvt->GetNtracks(1)+1);
  p=jx->Get3Momentum();
  p/=p.GetNorm();
  trk->Set3Momentum(p);
  trk->SetReferencePoint(*ref);
  NcTimestamp* tt0=ref->GetTimestamp();
  if (tt0) trk->SetTimestamp(*tt0);
  for (Int_t jt=1; jt<=jx->GetNtracks(); jt++)
  {
   NcTrack* tx=jx->GetTrack(jt);
   if (!tx) continue;
   for (Int_t is=1; is<=tx->GetNsignals(); is++)
   {
    NcSignal* sx1=tx->GetSignal(is);
    if (sx1) sx1->AddTrack(*trk);
   }
  }

  // Only take the jet with the highest quality number
  // (i.e. the first jet in the list) when the user had selected
  // this reconstruction mode.
  if (jangmax<0) break;
 }
}
///////////////////////////////////////////////////////////////////////////
