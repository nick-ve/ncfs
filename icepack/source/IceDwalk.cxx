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

// $Id: IceDwalk.cxx 83 2013-03-15 15:22:45Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class IceDwalk
// IceRecoBase derived TTask processor to perform direct walk track reconstruction.
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
// For details please refer to Astroparticle Physics 28 (2007) 456.
// This distinction between v_phase and v_group can be (de)activated via the
// memberfunction SetVgroupUsage(). By default the distinction between v_phase
// and v_group is activated in the constructor of this class.
//
// To prevent waisting CPU time in trying to reconstruct (high-energy) cascade
// events, or to select specifically reconstruction of low multiplicity events,
// the user may invoke the memberfunctions SetMaxMod() and SetMinMod().
// This allows selection of events for processing with a certain maximum and/or
// minimum number of good (D)OMs firing.
// The maximum number of good HLC hits per (D)OM to be used for the reconstruction
// can be specified via the memberfunction SetMaxHits().
// Since for Amanda there is no concept of HLC hits, all Amanda hits are treated as HLC.
// Note that hits in Amanda OMs are ordered w.r.t. increasing hit time (LE), whereas
// hits in IceCube DOMs are ordered w.r.t. decreasing amplitude (ADC).
//
// By setting the maximum number of good HLC hits per (D)OM to "n", all good (HLC and SLC)
// hits of each (D)OM up to the nth HLC hit (incl.) are used.
//
// Note that when all the good hits of a (D)OM are used, this may lead to large
// processing time in case many noise and/or afterpulse signals are not
// recognised by the hit cleaning procedure.
// To cope with this, the user may specify an automatic switch to use only the good
// hits up to the first HLC hit for large events via the memberfunction SetSingleHit().
//
// The SetMaxHits() facility may also be used to select a specific detector
// configuration (e.g. "IC" and "DC" without "I") to be used for the reconstruction.
// This can be achieved by suppressing processing of e.g. the "I" configuration
// by setting the corresponding maximum number of hits to -1.
// Note that there is a subtle difference between "detector configuration" and
// "optimised reconstruction parameters" as outlined in the docs of SetConditionalReco().
// In case the labels "IC", "DC" and "I" only indicate the different reconstruction parameters,
// the settings for "IC" and "DC" of SetMaxHits() indeed only affect the specified DOMs
// but still all "IC", "DC" and "I" optimised reconstructions are performed.
// This means that in those cases the effect of e.g. SetMaxHits(-1,"DC") is that all reconstructions
// "IC", "I" and "DC" will be performed, but without using DeepCore hits.
// This allows to use less hits per DOM for noisy detector components. 
// Invokation of SetMaxHits() with argument "I" has no effect in those cases.
//
// Another facility to (automatically) select reconstruction of specific detector
// configurations, reconstruction parameters and/or save CPU time is the so-called
// conditional processing mode.
// For further details please refer to the docs of the memberfunction SetConditionalReco(). 
//
// All reconstructed tracks will be stored in the IceEvent structure with as
// default the Classname of the producing processor as the basename of the track.
// A suffix "A" for an Amanda (OM) track, "IC" for a standard IceCube InIce track,
// "I" for a hybrid InIce (IC+DC) track or "DC" for a DeepCore track will be
// added to the basename automatically.
// For further details concerning the meaning of the suffix "A", "IC", "I" or "DC"
// please refer to the docs of the memberfunction SetConditionalReco(). 
// The track basename identifier can be modified by the user via the
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
// whereas the t0 is also available from the track timestamp .
//
// Information about the actual parameter settings can be found in the event
// structure itself via the device named "IceDwalk".
// All default parameter settings can be seen from the default constructor of this class.
//
// The various reconstruction steps are summarised as follows :
//
// 1) Construction of track elements (TE's).
//    A track element is a straight line connecting two HLC hits that
//    appeared at some minimum distance dmin and within some time interval dt
//    around the expected tgeo, according to eq. (20) of the NIM article.
//    The minimum distance can be specified via the memberfunction SetDmin().
//    The ideal time difference is given by tgeo=(hit distance)/c but an additional
//    time margin may be specified via the memberfunction SetDtmarg().    
//    The reference point r0 of the TE is taken as the center between
//    the two hit positions and the TE timestamp t0 at the position r0
//    is taken as the IceEvent timestamp increased by the average of the
//    two hit times. So, all timestamps contain the overall IceEvent
//    timestamp as a basis. This means that time differences can be
//    obtained via the NcTimestamp facilities (supporting upto picosecond
//    precision when available).
//    The TE direction is given by the relative position of the two hits.
//
//    Note : For the construction of TE's only HLC hits are used.
//
// 2) Each TE will obtain so called associated hits.
//    A hit is associated to a TE when it fulfills both the conditions
//
//      dtmin < tres < dtmax
//      dhit/lambda < F
//
//    tres   : time residual (in ns)
//             Difference between the observed hit time and the time expected
//             for a direct photon hit.     
//    dhit   : Distance (in m) traveled by the cherenkov photon from the track to the hit position
//    lambda : Effective photon scattering length in ice (in meter)
//
//    The time difference intervals may be specified via the memberfunction SetDthit().
//    The effective scattering length (in meter) for various detector locations may be specified
//    via the memberfunction SetScatteringLength().
//    The value of the parameter F may be specified via the memberfunction SetMaxDhit().
//
//    Note : For IceCube hits the user may select whether or not to allow SLC hits to be
//           associated via the memberfunction SetSLChitUsage().
//
// 3) Construction of track candidates (TC's).
//    These are TE's that fulfill both the conditions
//
//     nax > 0
//     qtc >= 0.8*qtcmax
//
//    where we have defined :
//
//    nax    : Associated hits and/or strings value for the specific TE.
//             For details see the docs of the memberfunction SetAsType().
//    qtc    : The track quality number (see hereafter).
//    qtcmax : Maximum quality number encountered for the TE's.
//
//    Notes :
//    -------
//    1) In case qtcmax<=0 no track candidate will be formed.
//    2) The selection to use the number of associated hits and/or strings
//       for the track quality number can be performed via the
//       memberfunction SetAsType().
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
//    In addition a track candidate must be within a certain maximum distance
//    of the jet starting TC in order to get clustered. 
//    The latter criterion prevents clustering of (nearly) parallel track candidates
//    crossing the detector a very different locations (e.g. muon bundles).
//    The maximum track opening angle may be specified via the SetTangmax() memberfunction.
//    The maximum track distance may be specified via the SetTdistmax() memberfunction.
//    This memberfunction also allows to specify whether the distance is determined within
//    the detector volume or not.
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
//    As such each jet is given an absolute quality indicator defined as :
//
//      avqtc=|jet momentum|/ntracks 
//
//    This jet quality indicator is further refined using also the number of hits and (D)OMs
//    associated to the jet as :
//
//      qvalue=avqtc/qtcmax+nam/nammax+nah/nahmax+nahlc/nahlcmax
//
//    where we have defined :
//
//    nam      : Number of associated modules, i.e. (D)OMs, for the specific jet.
//    nammax   : Maximum number of associated modules encountered for the jets.
//    nah      : Number of associated hits for the specific jet.
//    nahmax   : Maximum number of associated hits encountered for the jets.
//    nahhlc   : Number of associated HLC hits for the specific jet.
//    nahlcmax : Maximum number of associated HLC hits encountered for the jets.
//
//    With this definition the qvalue always lies in the range <0,4] which provides
//    an absolute quality scale.
//
//    The jet qvalue is then used to order the various jets w.r.t.
//    decreasing qvalue quality number.
//
//    Note : The jet qvalue is stored as "energy" of the jet, such that
//           it is always available for each jet and can also be used for
//           ordering the jets according to this value using the generic
//           NcEvent::SortJets() facility. 
//
// 5) The jets (after having been ordered w.r.t. decreasing jet qvalue)
//    are merged when their directions are within a certain maximum opening angle.
//    In addition a jet must be within a certain maximum distance of the starting jet
//    in order to get merged. 
//    The latter criterion prevents merging of (nearly) parallel tracks/jets
//    crossing the detector a very different locations (e.g. muon bundles).
//    The jet ordering before the merging process is essential, since the starting jet
//    will "eat up" the jets that will be merged into it. 
//    The jet ordering ensures that the jet with the highest quality number will
//    always initiate the merging process.
//    The default maximum opening angle is half the TC maximum opening angle,
//    but can be modified via the SetJangmax() memberfunction. This memberfunction
//    also allows to specify whether jet merging will be performed iteratively or not.
//    In case iteration has been activated, the jet ordering is performed after each
//    iteration step. This has to be done because the quality numbers of the
//    resulting merged jets have been automatically updated in the merging process.
//    
//    The maximum jet distance may be specified via the SetJdistmax() memberfunction.
//    This memberfunction also allows to specify whether the distance is determined
//    within the detector volume or not.
//
//    Note : Setting the maximum jet opening angle to <=0 will prevent
//           the merging of jets.
//
//    The average of all the r0 and t0 values of the merged jets will provide
//    the r0 and t0 (i.e. reference point) of the final jet.
//
// 6) The remaining (merged) jets are ordered w.r.t. decreasing jet qvalue.
//    As such the jet with the highest qvalue (Qmax) will be the first one in the list,
//    which will result in the fact that the final tracks are also ordered
//    w.r.t. decreasing qvalue.
//    Each remaining jet will provide the parameters (e.g. direction)
//    for a reconstructed track.
//    However, a reconstructed track will only be provided if it has sufficient associated hits.
//    The minimum number of associated hits for a reconstructed track to be produced 
//    can be specified via the memberfunction SetMinHits().
//    The track 3-momentum is set to the total jet 3-momentum, normalised to 1 GeV.
//    The mass and charge of the track are left 0.
//    The reference point data of the jet will provide the r0 and t0
//    (i.e. reference point) of the track.
//    The final track quality number and the various contributing parameters
//    are stored as fit details for the corresponding track(s) and can be retrieved
//    afterwards for each reconstructed track.  
//
// 7) The direction of a track is reversed in case a the track has a polar angle
//    theta<"thetatrk" and the (time ordered) hit pattern reflects a direction with
//    polar angle theta>"thetahits", where "thetatrk" and "thetahits" indicate angular thresholds.
//    This allows an additional reduction of the background for upgoing tracks
//    due to mis-reconstructed downgoing muons.
//
//    The polar angle thresholds "thetatrk" and "thetahits" (in degrees) may be set by the user
//    via the member function SetFlipAngles(), where the convention is that theta=0 indicates a
//    straight upgoing track.
//
//    Note : The flipping of the track direction may be de-activated by
//           setting thetatrk<0 and/or thetahits>180.
//
//    The default values thetatrk=-999 and thetahits=999 are set in the constructor of this class
//    which implies that the flipping of the tracks direction is de-activated by default.
//    Based on simulated data it was seen that with thetatrk=90 and thetahits=100 the direction flipping
//    did a good job in reducing downgoing Corsika events, but it eliminated too many upgoing signal events.
//    Maybe further fine tuning can make this facility useful in the future.
//
//    All reconstructed tracks with qvalue>=qcut*Qmax will be stored in the IceEvent structure
//    using the naming conventions outlined above.
//    The value of the parameter "qcut" may be specified via the member function SetQvalueCut().
//
//    Note : In case the maximum jet opening angle was specified <0,
//           only the jet with the highest quality number will appear
//           as a reconstructed track in the IceEvent structure.
//           This will allow comparison with the old Amanda (Sieglinde)
//           single track direct walk reconstruction results. 
//    
// For further details the user is referred to NIM A524 (2004) 169.
//
// Note : This algorithm works best on data which has been calibrated and
//        cross talk corrected (Amanda).
//        Due to the built-in causality requirements, the data do not need to be
//        cleaned from noise hits etc. (see the processor IceCleanHits).
//
//--- Author: Nick van Eijndhoven 07-oct-2005 Utrecht University
//- Modified: NvE $Date: 2013-03-15 16:22:45 +0100 (Fri, 15 Mar 2013) $ IIHE-VUB, Brussels
///////////////////////////////////////////////////////////////////////////
 
#include "IceDwalk.h"
#include "Riostream.h"

ClassImp(IceDwalk) // Class implementation to enable ROOT I/O

IceDwalk::IceDwalk(const char* name,const char* title) : IceRecoBase(name,title)
{
// Default constructor.
// The various reconstruction parameters are initialised to the values
// as mentioned in the general documentation of this class.
// The angular separation parameter for jet merging is initialised as half
// the value of the angular separation parameter for track candidate clustering.    

 SetCleaned(1,"A");
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
 SetMaxHits(3,"I");
 SetMaxHits(3,"IC");
 SetMaxHits(3,"DC");

 SetMinAhits(0,"A");
 SetMinAhits(0,"I");
 SetMinAhits(0,"IC");
 SetMinAhits(0,"DC");

 SetMinAmods(0,"A");
 SetMinAmods(6,"I");
 SetMinAmods(6,"IC");
 SetMinAmods(2,"DC");

 SetSLChitUsage(1,"I");
 SetSLChitUsage(1,"IC");
 SetSLChitUsage(1,"DC");

 SetFlipAngles(-999,999);

 SetScatteringLength(33.3,"A");
 SetScatteringLength(30,"UD");
 SetScatteringLength(5,"DL");
 SetScatteringLength(40,"LD");

 SetDmin(75,"A");
 SetDmin(120,"IC");
 SetDmin(85,"I");
 SetDmin(45,"DC");

 SetDtmarg(0,"A");
 SetDtmarg(-1,"IC");
 SetDtmarg(-1,"I");
 SetDtmarg(-1,"DC");

 SetMaxDhit(3.07126,"A");
 SetMaxDhit(3,"IC");
 SetMaxDhit(6,"I");
 SetMaxDhit(12,"DC");

 SetDthit(-30,300,"A");
 SetDthit(-50,250,"IC");
 SetDthit(-100,300,"I");
 SetDthit(-150,350,"DC");

 SetTangmax(15,"A");
 SetTangmax(15,"IC");
 SetTangmax(15,"I");
 SetTangmax(15,"DC");

 SetTdistmax(20,"A",1);
 SetTdistmax(20,"IC",1);
 SetTdistmax(20,"I",1);
 SetTdistmax(20,"DC",1);

 SetJangmax(fTangmaxA/2.,"A",1);
 SetJangmax(fTangmaxIC/2.,"IC",1);
 SetJangmax(fTangmaxI/2.,"I",1);
 SetJangmax(fTangmaxDC/2.,"DC",1);

 SetJdistmax(30,"A",1);
 SetJdistmax(30,"IC",1);
 SetJdistmax(30,"I",1);
 SetJdistmax(30,"DC",1);

 SetVgroupUsage(1,"A");
 SetVgroupUsage(1,"IC");
 SetVgroupUsage(1,"I");
 SetVgroupUsage(1,"DC");

 SetAsType(3,"A",2);
 SetAsType(-5,"IC",2);
 SetAsType(-5,"I",2);
 SetAsType(-5,"DC",2);

 SetHitWeight(-2);

 SetSingleHit(0,"A");
 SetSingleHit(200,"IC",20);
 SetSingleHit(200,"I",20);
 SetSingleHit(200,"DC",20);

 SetConditionalReco(5);

 SetQvalueCut(0.8);
}
///////////////////////////////////////////////////////////////////////////
IceDwalk::~IceDwalk()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetDmin(Float_t d,TString s)
{
// Set minimum hit distance (in m) to form a track element.
// For default values please refer to the default constructor of this class.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction
 
 if (s=="A")
 {
  fDminA=d;
  fParams.AddNamedSlot("DminA");
  fParams.SetSignal(fDminA,"DminA");
 }
 if (s=="IC")
 {
  fDminIC=d;
  fParams.AddNamedSlot("DminIC");
  fParams.SetSignal(fDminIC,"DminIC");
 }
 if (s=="DC")
 {
  fDminDC=d;
  fParams.AddNamedSlot("DminDC");
  fParams.SetSignal(fDminDC,"DminDC");
 }
 if (s=="I")
 {
  fDminI=d;
  fParams.AddNamedSlot("DminI");
  fParams.SetSignal(fDminI,"DminI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetDtmarg(Int_t dt,TString s)
{
// Set maximum hit time residual margin (in ns) for track elements w.r.t.
// the expected hit time difference based on a particle traveling at c between
// the two corresponding DOMs. This will guarantee causality for the track elements.
// Specification of dt<0 will result in using the same time window as for
// hit association (see memberfunction SetDthit).
// For default values please refer to the default constructor of this class.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction

 if (s=="A")
 {
  fDtmargA=dt;
  fParams.AddNamedSlot("DtmargA");
  fParams.SetSignal(fDtmargA,"DtmargA");
 }
 if (s=="IC")
 {
  fDtmargIC=dt;
  fParams.AddNamedSlot("DtmargIC");
  fParams.SetSignal(fDtmargIC,"DtmargIC");
 }
 if (s=="DC")
 {
  fDtmargDC=dt;
  fParams.AddNamedSlot("DtmargDC");
  fParams.SetSignal(fDtmargDC,"DtmargDC");
 }
 if (s=="I")
 {
  fDtmargI=dt;
  fParams.AddNamedSlot("DtmargI");
  fParams.SetSignal(fDtmargI,"DtmargI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetMaxDhit(Float_t d,TString s)
{
// Set maximum distance (in scattering length) for a hit to get associated.
// For default values please refer to the default constructor of this class.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction

 if (s=="A")
 {
  fMaxdhitA=d;
  fParams.AddNamedSlot("MaxdhitA");
  fParams.SetSignal(fMaxdhitA,"MaxdhitA");
 }
 if (s=="IC")
 {
  fMaxdhitIC=d;
  fParams.AddNamedSlot("MaxdhitIC");
  fParams.SetSignal(fMaxdhitIC,"MaxdhitIC");
 }
 if (s=="DC")
 {
  fMaxdhitDC=d;
  fParams.AddNamedSlot("MaxdhitDC");
  fParams.SetSignal(fMaxdhitDC,"MaxdhitDC");
 }
 if (s=="I")
 {
  fMaxdhitI=d;
  fParams.AddNamedSlot("MaxdhitI");
  fParams.SetSignal(fMaxdhitI,"MaxdhitI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetDthit(Float_t dtmin,Float_t dtmax,TString s)
{
// Set time interval [dtmin,dtmax] (in ns) around the expected hit time (t=0)
// for a hit to get associated.
// By the definition of t=0 for the expected hit time, this just means that
// the time residual (tres) has to fall within [dtmin,dtmax].
//
// Note : dtmin may be negative to account for early arrival times due to
//        jitter in the (D)OM electronics.
//
// For default values please refer to the default constructor of this class.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction

 if (s=="A")
 {
  fDtminA=dtmin;
  fDtmaxA=dtmax;
  fParams.AddNamedSlot("DtminA");
  fParams.AddNamedSlot("DtmaxA");
  fParams.SetSignal(fDtminA,"DtminA");
  fParams.SetSignal(fDtmaxA,"DtmaxA");
 }

 if (s=="IC")
 {
  fDtminIC=dtmin;
  fDtmaxIC=dtmax;
  fParams.AddNamedSlot("DtminIC");
  fParams.AddNamedSlot("DtmaxIC");
  fParams.SetSignal(fDtminIC,"DtminIC");
  fParams.SetSignal(fDtmaxIC,"DtmaxIC");
 }

 if (s=="DC")
 {
  fDtminDC=dtmin;
  fDtmaxDC=dtmax;
  fParams.AddNamedSlot("DtminDC");
  fParams.AddNamedSlot("DtmaxDC");
  fParams.SetSignal(fDtminDC,"DtminDC");
  fParams.SetSignal(fDtmaxDC,"DtmaxDC");
 }

 if (s=="I")
 {
  fDtminI=dtmin;
  fDtmaxI=dtmax;
  fParams.AddNamedSlot("DtminI");
  fParams.AddNamedSlot("DtmaxI");
  fParams.SetSignal(fDtminI,"DtminI");
  fParams.SetSignal(fDtmaxI,"DtmaxI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetTangmax(Float_t ang,TString s)
{
// Set maximum angular separation (in deg) for track candidate clustering into jets.
// For the default value please refer to the default constructor of this class.
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction
//
// Note : This function also sets automatically the value of the maximum
//        angular separation for jet merging into 1 single track to ang/2.
//        In order to specify a different max. jet merging separation angle,
//        one has to invoke the memberfunction SetJangmax afterwards.
 
 if (s=="A")
 {
  fTangmaxA=ang;
  fJangmaxA=ang/2.;
  fParams.AddNamedSlot("TangmaxA");
  fParams.SetSignal(fTangmaxA,"TangmaxA");
  fParams.AddNamedSlot("JangmaxA");
  fParams.SetSignal(fJangmaxA,"JangmaxA");
 }
 
 if (s=="IC")
 {
  fTangmaxIC=ang;
  fJangmaxIC=ang/2.;
  fParams.AddNamedSlot("TangmaxIC");
  fParams.SetSignal(fTangmaxIC,"TangmaxIC");
  fParams.AddNamedSlot("JangmaxIC");
  fParams.SetSignal(fJangmaxIC,"JangmaxIC");
 }
 
 if (s=="DC")
 {
  fTangmaxDC=ang;
  fJangmaxDC=ang/2.;
  fParams.AddNamedSlot("TangmaxDC");
  fParams.SetSignal(fTangmaxDC,"TangmaxDC");
  fParams.AddNamedSlot("JangmaxDC");
  fParams.SetSignal(fJangmaxDC,"JangmaxDC");
 }
 
 if (s=="I")
 {
  fTangmaxI=ang;
  fJangmaxI=ang/2.;
  fParams.AddNamedSlot("TangmaxI");
  fParams.SetSignal(fTangmaxI,"TangmaxI");
  fParams.AddNamedSlot("JangmaxI");
  fParams.SetSignal(fJangmaxI,"JangmaxI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetTdistmax(Float_t d,TString s,Int_t invol)
{
// Set maximum distance (in m) of the two track candidates in the track
// clustering process.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction
//
// The distance between the two tracks can be determined restricted to the
// detector volume (invol=1) or in the overall space (invol=0).  
// The former will prevent clustering of (nearly) parallel tracks which cross
// the detector volume at very different locations, whereas the latter will
// enable clustering of tracks with a common location of origin (e.g. muon
// bundles from an air shower) even if they cross the detector volume at
// very different locations. 
// At invokation of this memberfunction the default is invol=1.
// For default values please refer to the default constructor of this class.
 
 if (s=="A")
 {
  fTdistmaxA=d;
  fTinvolA=invol;
  fParams.AddNamedSlot("TdistmaxA");
  fParams.AddNamedSlot("TinvolA");
  fParams.SetSignal(fTdistmaxA,"TdistmaxA");
  fParams.SetSignal(fTinvolA,"TinvolA");
 }
 
 if (s=="IC")
 {
  fTdistmaxIC=d;
  fTinvolIC=invol;
  fParams.AddNamedSlot("TdistmaxIC");
  fParams.AddNamedSlot("TinvolIC");
  fParams.SetSignal(fTdistmaxIC,"TdistmaxIC");
  fParams.SetSignal(fTinvolIC,"TinvolIC");
 }
 
 if (s=="DC")
 {
  fTdistmaxDC=d;
  fTinvolDC=invol;
  fParams.AddNamedSlot("TdistmaxDC");
  fParams.AddNamedSlot("TinvolDC");
  fParams.SetSignal(fTdistmaxDC,"TdistmaxDC");
  fParams.SetSignal(fTinvolDC,"TinvolDC");
 }
 
 if (s=="I")
 {
  fTdistmaxI=d;
  fTinvolI=invol;
  fParams.AddNamedSlot("TdistmaxI");
  fParams.AddNamedSlot("TinvolI");
  fParams.SetSignal(fTdistmaxI,"TdistmaxI");
  fParams.SetSignal(fTinvolI,"TinvolI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetJangmax(Float_t ang,TString s,Int_t iter)
{
// Set angular separation (in deg) within which jets are merged into 1 single track.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction
//
// The merging process is a dynamic procedure and can be carried out by
// iteration (iter=1) until no further merging of the various jets occurs anymore.
// However, by specification of iter=0 the user can also select to go only
// once through all the jet combinations to check for mergers.
// For large events the latter will in general result in more track candidates.  
// At invokation of this memberfunction the default is iter=1.
// In the constructor the default angle has been set to half  of the value of the
// default track candidate clustering separation angle.
// For default values please refer to the default constructor of this class.
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
  fParams.AddNamedSlot("JangmaxA");
  fParams.AddNamedSlot("JiterateA");
  fParams.SetSignal(fJangmaxA,"JangmaxA");
  fParams.SetSignal(fJiterateA,"JiterateA");
 }

 if (s=="IC")
 {
  fJangmaxIC=ang;
  fJiterateIC=iter;
  fParams.AddNamedSlot("JangmaxIC");
  fParams.AddNamedSlot("JiterateIC");
  fParams.SetSignal(fJangmaxIC,"JangmaxIC");
  fParams.SetSignal(fJiterateIC,"JiterateIC");
 }

 if (s=="DC")
 {
  fJangmaxDC=ang;
  fJiterateDC=iter;
  fParams.AddNamedSlot("JangmaxDC");
  fParams.AddNamedSlot("JiterateDC");
  fParams.SetSignal(fJangmaxDC,"JangmaxDC");
  fParams.SetSignal(fJiterateDC,"JiterateDC");
 }

 if (s=="I")
 {
  fJangmaxI=ang;
  fJiterateI=iter;
  fParams.AddNamedSlot("JangmaxI");
  fParams.AddNamedSlot("JiterateI");
  fParams.SetSignal(fJangmaxI,"JangmaxI");
  fParams.SetSignal(fJiterateI,"JiterateI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetJdistmax(Float_t d,TString s,Int_t invol)
{
// Set maximum distance (in m) of the two jets in the jet merging process.
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction
//
// The distance between the two jets can be determined restricted to the
// detector volume (invol=1) or in the overall space (invol=0).  
// The former will prevent clustering of (nearly) parallel tracks which cross
// the detector volume at very different locations, whereas the latter will
// enable clustering of tracks with a common location of origin (e.g. muon
// bundles from an air shower) even if they cross the detector volume at
// very different locations. 
// At invokation of this memberfunction the default is invol=1.
// For default values please refer to the default constructor of this class.
 
 if (s=="A")
 {
  fJdistmaxA=d;
  fJinvolA=invol;
  fParams.AddNamedSlot("JdistmaxA");
  fParams.AddNamedSlot("JinvolA");
  fParams.SetSignal(fJdistmaxA,"JdistmaxA");
  fParams.SetSignal(fJinvolA,"JinvolA");
 }
 
 if (s=="IC")
 {
  fJdistmaxIC=d;
  fJinvolIC=invol;
  fParams.AddNamedSlot("JdistmaxIC");
  fParams.AddNamedSlot("JinvolIC");
  fParams.SetSignal(fJdistmaxIC,"JdistmaxIC");
  fParams.SetSignal(fJinvolIC,"JinvolIC");
 }
 
 if (s=="DC")
 {
  fJdistmaxDC=d;
  fJinvolDC=invol;
  fParams.AddNamedSlot("JdistmaxDC");
  fParams.AddNamedSlot("JinvolDC");
  fParams.SetSignal(fJdistmaxDC,"JdistmaxDC");
  fParams.SetSignal(fJinvolDC,"JinvolDC");
 }
 
 if (s=="I")
 {
  fJdistmaxI=d;
  fJinvolI=invol;
  fParams.AddNamedSlot("JdistmaxI");
  fParams.AddNamedSlot("JinvolI");
  fParams.SetSignal(fJdistmaxI,"JdistmaxI");
  fParams.SetSignal(fJinvolI,"JinvolI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetAsType(Int_t flag,TString s,Float_t w)
{
// Select number of associated hits and/or strings as quality indicator
// for each Track Candidate (TC).
//
// flag =  1 : Weighted number of associated hits (nah) is used as quality indicator
//      =  2 : Number of associated strings (nas) is used as quality indicator
//      =  3 : nah*nas is used as quality indicator
//      =  4 : nah+nahlc/nah+w*(nas-1)/nas is used as quality indicator
//      = -1 : Number of associated modules (nam) and strings is used as (nam+w*nas) quality indicator
//      = -2 : nam+nah/nam is used as quality indicator
//      = -3 : nam*nas is used as quality indicator
//      = -4 : nam+nahlc/nah+w*(nas-1)/nas is used as quality indicator
//      = -5 : nam+nah+nahlc/nah+w*(nas-1)/nas is used as quality indicator
//
// where nahlc is the weighted number of associated HLC hits and w is the weight
// of the string count w.r.t. the hit or module count in the quality indicator.
//
// Each associated hit is counted with a value=hitweight*lambda/d, where lambda is the scattering length
// and d is the distance traveled by a cherenkov photon to the fired module.
// Hits with d<lambda are given a value=1.
// For details about the hitweight please refer to the memberfunction SetHitWeight().
//
// The input argument "s" allows for detector specification.
//
// s = "A"  --> Amanda reconstruction
//     "IC" --> Standard IceCube optimised reconstruction
//     "DC" --> DeepCore optimised reconstruction
//     "I"  --> InIce (IC+DC hybrid) optimised reconstruction
//
// In case the input argument "w" is not provided, the current weight will not
// be modified. 
//
// For default values please refer to the default constructor of this class.

 if (s=="A")
 {
  if (flag>=-5 && flag<=4) fAsTypeA=flag;
  if (w>0) fWstringA=w;
  fParams.AddNamedSlot("AsTypeA");
  fParams.AddNamedSlot("WstringA");
  fParams.SetSignal(fAsTypeA,"AsTypeA");
  fParams.SetSignal(fWstringA,"WstringA");
 }

 if (s=="IC")
 {
  if (flag>=-5 && flag<=4) fAsTypeIC=flag;
  if (w>0) fWstringIC=w;
  fParams.AddNamedSlot("AsTypeIC");
  fParams.AddNamedSlot("WstringIC");
  fParams.SetSignal(fAsTypeIC,"AsTypeIC");
  fParams.SetSignal(fWstringIC,"WstringIC");
 }

 if (s=="DC")
 {
  if (flag>=-5 && flag<=4) fAsTypeDC=flag;
  if (w>0) fWstringDC=w;
  fParams.AddNamedSlot("AsTypeDC");
  fParams.AddNamedSlot("WstringDC");
  fParams.SetSignal(fAsTypeDC,"AsTypeDC");
  fParams.SetSignal(fWstringDC,"WstringDC");
 }

 if (s=="I")
 {
  if (flag>=-5 && flag<=4) fAsTypeI=flag;
  if (w>0) fWstringI=w;
  fParams.AddNamedSlot("AsTypeI");
  fParams.AddNamedSlot("WstringI");
  fParams.SetSignal(fAsTypeI,"AsTypeI");
  fParams.SetSignal(fWstringI,"WstringI");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetHitWeight(Float_t w)
{
// Set weight for associated hit counting.
// Every associated hit will contribute to the quality of a track candidate as :
//
// value=w*lambda/dist
//
// Here "lambda" is the average photon scattering length and "dist" is the
// distance traveled by a cherenkov photon to the fired (D)OM.
//
// The weighted number of associated hits (nah) is represented by the sum of all
// the above mentioned "value" contributions.
//
// Notes :
// -------
// 1) For dist<lambda the fraction lambda/dist will always be set to 1.
// 2) Setting w=0 will deactivate nah as track candidate quality indicator.
// 3) Setting w=-1 will result in value=1, so that nah represents an unweighted hit count.
// 3) Setting w=-2 will result in value=amp*lambda/dist, where "amp" is the hit amplitude in npe
//
// For default values please refer to the default constructor of this class.

 fHitweight=w;  

 fParams.AddNamedSlot("Hitweight");
 fParams.SetSignal(fHitweight,"Hitweight");
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetConditionalReco(Int_t flag)
{
// Set flag for conditional reconstruction of the various detector combinations.
// This will allow to save considerably on cpu time, especially for large events.
//
// flag :  0 ==> Invoke all the various reconstructions in the order "IC", "I" and "DC"
//               by using only the hits belonging to the corresponding detector (combination).
//         1 ==> Same as flag=0 but no "I" reconstruction in case at least one "IC" track was found.
//         2 ==> Same as flag=1 but also no "DC" reconstruction in case at least one "IC" or "I" track was found.
//         3 ==> Invoke all the various reconstructions in the order "IC", "I" and "DC"
//               by using all the InIce hits.
//         4 ==> Same as flag=3 but no "I" reconstruction in case at least one "IC" track was found.
//         5 ==> Same as flag=4 but also no "DC" reconstruction in case at least one "IC" or "I" track was found.
//         6 ==> Invoke all the various reconstructions in the order "IC", "I" and "DC"
//               by using all the InIce hits and removing all the hits associated to tracks of a previous
//               reconstruction before the next reconstruction is invoked.
//         7 ==> Same as flag=6 but no "I" reconstruction in case at least one "IC" track was found.
//         8 ==> Same as flag=7 but also no "DC" reconstruction in case at least one "IC" or "I" track was found.
//
// For the default value please refer to the default constructor of this class.
//
// Notes :
// -------
// 1) In case of inconsistent input the current setting will not be modified.
// 2) For flag<=2 the indications "IC", "I" and "DC" reflect both the used hits and the reconstruction parameters,
//    whereas for flag>=3 these labels only reflect the used reconstruction parameters. 
//    The various reconstruction parameters are tuned as follows :
//    "IC" : The procedure is optimised to provide a direct walk track with better than average
//           pointing accuracy (i.e. slightly higher energy threshold) on the expense of sometimes
//           not making a reco track at all.
//    "I"  : The procedure is optimised to provide at least one direct walk track
//           in most of the events (i.e. opt for somewhat lower energy threshold) at the
//           expense of having a pointing accuracy slightly worse than average. 
//    "DC" : The procedure is optimised to provide a direct walk track with the
//           lowest possible energy threshold at the expense of having a worse than average
//           pointing accuracy at these low energies.
// 3) Invokation of the SetMaxHits memberfunction with a negative number will de-activate
//    the "IC", "I" or "DC" detector configuration for reconstruction if flag<=2, but for flag>=3
//    it will only de-activate the "IC", "I" or "DC" selection criteria while keeping all InIce hits.

 if (flag>=0 && flag<=8) fConditional=flag;

 fParams.AddNamedSlot("ConditionalReco");
 fParams.SetSignal(fConditional,"ConditionalReco");
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetQvalueCut(Float_t qcut)
{
// Set threshold for final track Qvalue selection.
// Tracks with Qvalue<qcut*qmax will not be kept.
//
// For the default value please refer to the default constructor of this class.

 fQcut=qcut;

 fParams.AddNamedSlot("QvalueCut");
 fParams.SetSignal(fQcut,"QvalueCut");
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::Exec(Option_t* opt)
{
// Implementation of the direct walk track reconstruction.

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
 fParams.SetNameTitle(ClassName(),"Reco parameters");

 // Add the fParams device to the IceEvent structure
 fEvt->AddDevice(fParams);

 // Flag to indicate that a track has been found
 // to enable condional reconstruction
 Int_t track=0;

 /////////////////////////////////////////////////////////
 // Perform the various reconstructions (conditionally) //
 /////////////////////////////////////////////////////////

 Amanda(); // The (old) Amanda reconstruction

 TObjArray hits; // Storage area for hits to be used in reconstruction

 track=IceCube(hits);
 if (fConditional==0 || fConditional==3 || fConditional==6 || !track) track=InIce(hits);
 if (fConditional==0 || fConditional==1 || fConditional==3 || fConditional==4 || fConditional==6 || fConditional==7 || !track) track=DeepCore(hits);
}
///////////////////////////////////////////////////////////////////////////
Int_t IceDwalk::Amanda()
{
// The direct walk track reconstruction for Amanda OM signals.

 if (fMaxhitsA<0) return 0;

 // Fetch all fired Amanda OMs for this event
 TObjArray* devs=fEvt->GetDevices("IceAOM");
 if (!devs) return 0;
 Int_t naoms=devs->GetEntries();
 if (!naoms) return 0;

 // Secure the OM pointers in a private array
 TObjArray aoms;
 for (Int_t i=0; i<naoms; i++)
 {
  aoms.Add(devs->At(i));
 }

 // Check for the minimum and/or maximum number of good fired Amanda OMs
 Int_t ngood=0;
 for (Int_t iom=0; iom<naoms; iom++)
 {
  IceGOM* omx=(IceGOM*)aoms.At(iom);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  ngood++;
 } 
 if (ngood<fMinmodA || ngood>fMaxmodA) return 0;

 Int_t ngood1=fEvt->GetStringMax("IceAOM");

 Int_t maxhits=fMaxhitsA;
 if (fSingleA && ngood>=fSingleA) maxhits=1;
 if (fSingle1A && ngood1>=fSingle1A) maxhits=1;

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
 Float_t dtres;
 TObjArray hits;
 TObjArray* ordered;

 Float_t dtmin=fDtminA;
 Float_t dtmax=fDtmaxA;
 if (fDtmargA>=0)
 {
  dtmin=-fDtmargA;
  dtmax=fDtmargA;
 }

 // Check the hits of Amanda OM pairs for possible track elements.
 // Also all the good hits are stored in the meantime (to save CPU time)
 // for hit association with the various track elements lateron.
 NcTrack* te=0;
 for (Int_t i1=0; i1<naoms; i1++) // First OM of the pair
 {
  IceGOM* omx1=(IceGOM*)aoms.At(i1);
  if (!omx1) continue;
  if (omx1->GetDeadValue("LE")) continue;
  r1=omx1->GetPosition();
  // Select all the good hits of this first OM
  hits1.Clear();
  // Determine the max. number of hits to be processed for this OM
  ordered=0;
  if (maxhits>0 && omx1->GetNhits()>maxhits) ordered=omx1->SortHits("LE",1,0,7);
  nh1=0;
  for (Int_t j1=1; j1<=omx1->GetNhits(); j1++)
  {
   if (ordered)
   {
    if (nh1>=maxhits) break;
    sx1=(NcSignal*)ordered->At(j1-1);
   }
   else
   {
    sx1=omx1->GetHit(j1);
   }
   if (!sx1) continue;
   if (fCleanA && (sx1->GetDeadValue("ADC") || sx1->GetDeadValue("LE") || sx1->GetDeadValue("TOT"))) continue;
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
   IceGOM* omx2=(IceGOM*)aoms.At(i2);
   if (!omx2) continue;
   if (omx2->GetDeadValue("LE")) continue;
   r2=omx2->GetPosition();
   r12=r2-r1;
   dist=r12.GetNorm();

   if (dist<fDminA) continue;

   // Select all the good hits of this second OM
   hits2.Clear();
   // Determine the max. number of hits to be processed for this OM
   ordered=0;
   if (maxhits>0 && omx2->GetNhits()>maxhits) ordered=omx2->SortHits("LE",1,0,7);
   nh2=0;
   for (Int_t j2=1; j2<=omx2->GetNhits(); j2++)
   {
    if (ordered)
    {
     if (nh2>=maxhits) break;
     sx2=(NcSignal*)ordered->At(j2-1);
    }
    else
    {
     sx2=omx2->GetHit(j2);
    }
    if (!sx2) continue;
    if (fCleanA && (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT"))) continue;
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
     dtres=fabs(dt)-dist/c;
     t0=(t1+t2)/2.;

     if (dtres<dtmin || dtres>dtmax) continue;

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
 AssociateHits(tes,hits,fAsTypeA,fWstringA,fDtminA,fDtmaxA,fMaxdhitA,fVgroupA,fCleanA,1,qmax);

 // Skip poorly reconstructed events
 if (qmax<=0) return 0;

 // Selection on quality (Q value) in case of multiple track candidates
 SelectQvalue(tes,qmax);

 Int_t nte=tes.GetEntries();
 if (!nte) return 0;

 // Clustering of track candidates into jets
 TObjArray jets;
 jets.SetOwner();
 ClusterTracks(tes,jets,fTangmaxA,fTinvolA,fTdistmaxA,qmax);

 Int_t njets=jets.GetEntries();
 if (!njets) return 0;

 // Order the jets w.r.t. decreasing quality value
 ordered=fEvt->SortJets(-2,&jets);
 TObjArray jets2(*ordered);

 // Merging of jets
 MergeJets(jets2,fJangmaxA,fJdistmaxA,fJinvolA,fJiterateA,qmax);

 // Production and storage of the final tracks
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="A";
 TString title=ClassName();
 title+=" Amanda track";
 Int_t ntk=StoreTracks(jets2,fMinahitsA,fMinamodsA,fJangmaxA,name,title,hits);

 return ntk;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceDwalk::IceCube(TObjArray& hits)
{
// The direct walk track reconstruction for the standard IceCube InIce DOM signals c.q. criteria.
// The procedure here is optimised to provide a direct walk track with better than average
// pointing accuracy (i.e. slightly higher energy threshold) on the expense of sometimes
// not making a reco track at all.
// The reconstruction details are steered via the various parameters and the choice of
// (un)conditional processing.
//
// The return argument indicates whether or not a track has been found

 // Determination and storage of track elements.
 TObjArray tes;
 tes.SetOwner();
 Int_t gethits=1;
 TString domclass="IceICDOM";
 if (fConditional>=3) domclass="IceIDOM";
 Int_t ntes=MakeTEs(fCleanIC,fMaxhitsIC,fDminIC,fDtmargIC,fDtminIC,fDtmaxIC,domclass,tes,hits,gethits);
 if (!ntes) return 0;

 // Association of hits to the various track elements.
 Float_t qmax=0;
 AssociateHits(tes,hits,fAsTypeIC,fWstringIC,fDtminIC,fDtmaxIC,fMaxdhitIC,fVgroupIC,fCleanIC,fSlcIC,qmax);

 // Skip poorly reconstructed events
 if (qmax<=0) return 0;

 // Selection on quality (Q value) in case of multiple track candidates
 SelectQvalue(tes,qmax);

 Int_t nte=tes.GetEntries();
 if (!nte) return 0;

 // Clustering of track candidates into jets
 TObjArray jets;
 jets.SetOwner();
 ClusterTracks(tes,jets,fTangmaxIC,fTinvolIC,fTdistmaxIC,qmax);

 Int_t njets=jets.GetEntries();
 if (!njets) return 0;

 // Order the jets w.r.t. decreasing quality value
 TObjArray* ordered=fEvt->SortJets(-2,&jets);
 TObjArray jets2(*ordered);

 // Merging of jets
 MergeJets(jets2,fJangmaxIC,fJdistmaxIC,fJinvolIC,fJiterateIC,qmax);

 // Production and storage of the final tracks
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="IC";
 TString title=ClassName();
 title+=" standard IceCube track";
 Int_t ntk=StoreTracks(jets2,fMinahitsIC,fMinamodsIC,fJangmaxIC,name,title,hits);

 return ntk;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceDwalk::InIce(TObjArray& hits)
{
// The direct walk track reconstruction for all InIce DOM signals c.q. criteria.
// The procedure here is optimised to provide at least one direct walk track
// in most of the events (i.e. opt for somewhat lower energy threshold) at the
// expense of having a pointing accuracy slightly worse than average. 
// The reconstruction details are steered via the various parameters and the choice of
// (un)conditional processing.
//
// The return argument indicates whether or not a track has been found

 // Determination and storage of track elements.
 TObjArray tes;
 tes.SetOwner();
 Int_t gethits=1;
 if (fConditional>=3 && fMaxhitsIC>=0) gethits=0;
 Int_t ntes=MakeTEs(fCleanI,fMaxhitsI,fDminI,fDtmargI,fDtminI,fDtmaxI,"IceIDOM",tes,hits,gethits);
 if (!ntes) return 0;

 // Association of hits to the various track elements.
 Float_t qmax=0;
 AssociateHits(tes,hits,fAsTypeI,fWstringI,fDtminI,fDtmaxI,fMaxdhitI,fVgroupI,fCleanI,fSlcI,qmax);

 // Skip poorly reconstructed events
 if (qmax<=0) return 0;

 // Selection on quality (Q value) in case of multiple track candidates
 SelectQvalue(tes,qmax);

 Int_t nte=tes.GetEntries();
 if (!nte) return 0;

 // Clustering of track candidates into jets
 TObjArray jets;
 jets.SetOwner();
 ClusterTracks(tes,jets,fTangmaxI,fTinvolI,fTdistmaxI,qmax);

 Int_t njets=jets.GetEntries();
 if (!njets) return 0;

 // Order the jets w.r.t. decreasing quality value
 TObjArray* ordered=fEvt->SortJets(-2,&jets);
 TObjArray jets2(*ordered);

 // Merging of jets
 MergeJets(jets2,fJangmaxI,fJdistmaxI,fJinvolI,fJiterateI,qmax);

 // Production and storage of the final tracks
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="I";
 TString title=ClassName();
 title+=" InIce track";
 Int_t ntk=StoreTracks(jets2,fMinahitsI,fMinamodsI,fJangmaxI,name,title,hits);

 return ntk;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceDwalk::DeepCore(TObjArray& hits)
{
// The direct walk track reconstruction for the DeepCore DOM signals c.q. criteria.
// The procedure here is optimised to provide a direct walk track with the
// lowest possible energy threshold at the expense of having a worse than average
// pointing accuracy at these low energies.
// The reconstruction details are steered via the various parameters and the choice of
// (un)conditional processing.
//
// The return argument indicates whether or not a track has been found

 // Determination and storage of track elements.
 TObjArray tes;
 tes.SetOwner();
 Int_t gethits=1;
 TString domclass="IceDCDOM";
 if (fConditional>=3)
 {
  domclass="IceIDOM";
  if (fMaxhitsIC>=0 || fMaxhitsI>=0) gethits=0;
 }
 Int_t ntes=MakeTEs(fCleanDC,fMaxhitsDC,fDminDC,fDtmargDC,fDtminDC,fDtmaxDC,domclass,tes,hits,gethits);
 if (!ntes) return 0;

 // Association of hits to the various track elements.
 Float_t qmax=0;
 AssociateHits(tes,hits,fAsTypeDC,fWstringDC,fDtminDC,fDtmaxDC,fMaxdhitDC,fVgroupDC,fCleanDC,fSlcDC,qmax);

 // Skip poorly reconstructed events
 if (qmax<=0) return 0;

 // Selection on quality (Q value) in case of multiple track candidates
 SelectQvalue(tes,qmax);

 Int_t nte=tes.GetEntries();
 if (!nte) return 0;

 // Clustering of track candidates into jets
 TObjArray jets;
 jets.SetOwner();
 ClusterTracks(tes,jets,fTangmaxDC,fTinvolDC,fTdistmaxDC,qmax);

 Int_t njets=jets.GetEntries();
 if (!njets) return 0;

 // Order the jets w.r.t. decreasing quality value
 TObjArray* ordered=fEvt->SortJets(-2,&jets);
 TObjArray jets2(*ordered);

 // Merging of jets
 MergeJets(jets2,fJangmaxDC,fJdistmaxDC,fJinvolDC,fJiterateDC,qmax);

 // Production and storage of the final tracks
 TString name=fTrackname;
 if (name=="") name=ClassName();
 name+="DC";
 TString title=ClassName();
 title+=" DeepCore track";
 Int_t ntk=StoreTracks(jets2,fMinahitsDC,fMinamodsDC,fJangmaxDC,name,title,hits);

 return ntk;
}
///////////////////////////////////////////////////////////////////////////
Int_t IceDwalk::MakeTEs(Int_t cln,Int_t maxhits,Float_t dmin,Float_t dtmarg,Float_t dtmin,Float_t dtmax,TString domclass,TObjArray& tes,TObjArray& hits,Int_t gethits)
{
// Creation of Track Elements (TEs) which (via hit association) may form track candidates (TCs).
// The TE selection criteria (reflecting IC, I or DC parameters) are steered via "maxhits", "dmin", "dtmarg",
// "dtmin" and "dtmax", whereas the DOMs to be used are specified independently via "domclass". 
// The created TEs are returned via the array "tes".
// Also the hits for further processing are selected and returned via the array "hits".
// In case the input argument "gethits" is set to 1, the array "hits" will be newly filled with the hits
// recorded by the DOMs corresponding to "domclass".
// However, when gethits=0, the existing hits in the array "hits" will be used.
// The latter case allows to remove hits from the existing list in subsequent processing steps
// (e.g. remove hits which were already associated to tracks reconstructed previously with other criteria).

 if (fConditional<=2 && maxhits<0) return 0;

 // Fetch all fired "domclass" DOMs for this event
 TObjArray* devs=fEvt->GetDevices(domclass);
 if (!devs) return 0;
 Int_t ndoms=devs->GetEntries();
 if (!ndoms) return 0;

 // Secure the DOM pointers in a private array
 TObjArray doms;
 for (Int_t i=0; i<ndoms; i++)
 {
  doms.Add(devs->At(i));
 }

 // Check for the minimum and/or maximum number of good fired DOMs
 Int_t ngoodIC=0;
 Int_t ngoodI=0;
 Int_t ngoodDC=0;
 for (Int_t idom=0; idom<ndoms; idom++)
 {
  IceGOM* omx=(IceGOM*)doms.At(idom);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  if (omx->InheritsFrom("IceICDOM")) ngoodIC++;
  if (omx->InheritsFrom("IceIDOM")) ngoodI++;
  if (omx->InheritsFrom("IceDCDOM")) ngoodDC++;
 } 
 if (ngoodIC<fMinmodIC || ngoodIC>fMaxmodIC) return 0;
 if (ngoodI<fMinmodI || ngoodI>fMaxmodI) return 0;
 if (ngoodDC<fMinmodDC || ngoodDC>fMaxmodDC) return 0;

 Int_t ngood1IC=fEvt->GetStringMax("IceICDOM");
 Int_t ngood1I=fEvt->GetStringMax("IceIDOM");
 Int_t ngood1DC=fEvt->GetStringMax("IceDCDOM");

 const Float_t c=0.299792458; // Light speed in vacuum in meters per ns

 // Use dtmarg as symmetric causality time window margin if selected.
 // Otherwise the specified timeresidual windows will be used. 
 if (dtmarg>=0)
 {
  dtmin=-dtmarg;
  dtmax=dtmarg;
 }

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
 Float_t dtres;
 TObjArray* ordered;

 // Check the hits of DOM pairs for possible track elements.
 // Also all the good hits are stored in the meantime (to save CPU time)
 // for hit association with the various track elements lateron.
 NcTrack* te=0;
 for (Int_t i1=0; i1<ndoms; i1++) // First DOM of the pair
 {
  IceGOM* omx1=(IceGOM*)doms.At(i1);
  if (!omx1) continue;
  if (omx1->GetDeadValue("ADC") || omx1->GetDeadValue("LE") || omx1->GetDeadValue("TOT")) continue;
  r1=omx1->GetPosition();

  // Select all the good hits of this first DOM
  hits1.Clear();

  if (gethits) // New filling of the hit array data
  { 
   // Determine the max. number of hits to be processed for this DOM
   if (omx1->InheritsFrom("IceICDOM"))
   {
    maxhits=fMaxhitsIC;
    if (fSingleIC && ngoodIC>=fSingleIC) maxhits=1;
    if (fSingle1IC && ngood1IC>=fSingle1IC) maxhits=1;
   }
   if (omx1->InheritsFrom("IceDCDOM"))
   {
    maxhits=fMaxhitsDC;
    if (fSingleDC && ngoodDC>=fSingleDC) maxhits=1;
    if (fSingle1DC && ngood1DC>=fSingle1DC) maxhits=1;
   }
   if (fSingleI && ngoodI>=fSingleI) maxhits=1;
   if (fSingle1I && ngood1I>=fSingle1I) maxhits=1;

   if (maxhits<0) continue;

   ordered=0;
   if (maxhits>0 && omx1->GetNhits()>maxhits) ordered=omx1->SortHits("ADC",-1,0,7);
   nh1=0;
   for (Int_t j1=1; j1<=omx1->GetNhits(); j1++)
   {
    if (ordered)
    {
     if (nh1>=maxhits) break;
     sx1=(NcSignal*)ordered->At(j1-1);
    }
    else
    {
     sx1=omx1->GetHit(j1);
    }
    if (!sx1) continue;
    if (cln && (sx1->GetDeadValue("ADC") || sx1->GetDeadValue("LE") || sx1->GetDeadValue("TOT"))) continue;
    if (sx1->GetSignal("SLC")<0.5)
    {
     hits1.Add(sx1);
     nh1++;
    }
    // Also store all good hits in the total hit array
    hits.Add(sx1);
   }
  }
  else // Use the existing hits in the array for this DOM
  {
   for (Int_t j1=0; j1<hits.GetEntries(); j1++)
   {
    sx1=(NcSignal*)hits.At(j1);
    if (!sx1) continue;
    if (cln && (sx1->GetDeadValue("ADC") || sx1->GetDeadValue("LE") || sx1->GetDeadValue("TOT"))) continue;
    if (sx1->GetSignal("SLC")>0.5) continue;
    IceGOM* omx=(IceGOM*)sx1->GetDevice();
    if (omx==omx1) hits1.Add(sx1);
   }
  }

  // No further pair to be formed with the last DOM in the list 
  if (i1==(ndoms-1)) break;

  nh1=hits1.GetEntries();
  if (!nh1) continue;

  for (Int_t i2=i1+1; i2<ndoms; i2++) // Second DOM of the pair
  {
   IceGOM* omx2=(IceGOM*)doms.At(i2);
   if (!omx2) continue;
   if (omx2->GetDeadValue("ADC") || omx2->GetDeadValue("LE") || omx2->GetDeadValue("TOT")) continue;
   r2=omx2->GetPosition();
   r12=r2-r1;
   dist=r12.GetNorm();
   if (dist<dmin) continue;

   // Select all the good hits of this second DOM
   hits2.Clear();

   if (gethits) // New filling of the hit array data
   { 
    // Determine the max. number of hits to be processed for this DOM
    if (omx2->InheritsFrom("IceICDOM"))
    {
     maxhits=fMaxhitsIC;
     if (fSingleIC && ngoodIC>=fSingleIC) maxhits=1;
     if (fSingle1IC && ngood1IC>=fSingle1IC) maxhits=1;
    }
    if (omx2->InheritsFrom("IceDCDOM"))
    {
     maxhits=fMaxhitsDC;
     if (fSingleDC && ngoodDC>=fSingleDC) maxhits=1;
     if (fSingle1DC && ngood1DC>=fSingle1DC) maxhits=1;
    }
    if (fSingleI && ngoodI>=fSingleI) maxhits=1;
    if (fSingle1I && ngood1I>=fSingle1I) maxhits=1;

    if (maxhits<0) continue;

    ordered=0;
    if (maxhits>0 && omx2->GetNhits()>maxhits) ordered=omx2->SortHits("ADC",-1,0,7);
    nh2=0;
    for (Int_t j2=1; j2<=omx2->GetNhits(); j2++)
    {
     if (ordered)
     {
      if (nh2>=maxhits) break;
      sx2=(NcSignal*)ordered->At(j2-1);
     }
     else
     {
      sx2=omx2->GetHit(j2);
     }
     if (!sx2) continue;
     if (cln && (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT"))) continue;
     if (sx2->GetSignal("SLC")>0.5) continue; 
     hits2.Add(sx2);
     nh2++;
    }
   }
   else // Use the existing hits in the array for this DOM
   {
    for (Int_t j2=0; j2<hits.GetEntries(); j2++)
    {
     sx2=(NcSignal*)hits.At(j2);
     if (!sx2) continue;
     if (cln && (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT"))) continue;
     if (sx2->GetSignal("SLC")>0.5) continue;
     IceGOM* omx=(IceGOM*)sx2->GetDevice();
     if (omx==omx2) hits2.Add(sx2);
    }
   }

   nh2=hits2.GetEntries();
   if (!nh2) continue;

   // Position r0 in between the two DOMs and normalised relative direction r12
   rsum=(r1+r2)/2.;
   r0.SetPosition((Nc3Vector&)rsum);
   r12/=dist; // Make r12 a unit vector

   // Check all hit pair combinations of these two DOMs for possible track elements  
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
     dtres=fabs(dt)-dist/c;
     t0=(t1+t2)/2.;

     if (dtres<dtmin || dtres>dtmax) continue;

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

 Int_t ntes=tes.GetEntries();

 return ntes;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::AssociateHits(TObjArray& tes,TObjArray& hits,Int_t astype,Float_t ws,Float_t dtmin,Float_t dtmax,Float_t maxdhit,Int_t vgroup,Int_t cln,Int_t slc,Float_t& qmax)
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

 Float_t zhit;
 Float_t lambda=0;

 Int_t nte=tes.GetEntries();
 Int_t nh=hits.GetEntries();
 Float_t d=0;
 Nc3Vector p;
 NcPosition rhit;
 Float_t thit;
 Nc3Vector r12;
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
 fit.AddNamedSlot("Nmods");
 fit.AddNamedSlot("Nhits");
 fit.AddNamedSlot("Nhlc");
 fit.AddNamedSlot("Nax");
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
 Float_t nah=0;   // Weighted number of associated hits
 Float_t nahlc=0; // Weighted number of associated HLC hits
 Int_t nas=0;     // Number of associated strings
 Int_t nam=0;     // Number of associated modules
 Float_t qtc=0;   // Quality parameter for the track (candidate)
 Float_t nax=0;   // Associated hits, mods and/or strings value to build the QTC for a certain TE
 Float_t frac=0;
 Float_t amp=0;
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
  if (!p.HasVector() || !p.GetNorm()) continue;
  levers.Reset();
  hprojs.Reset();
  times.Reset();
  nah=0;
  nahlc=0;
  for (Int_t jh=0; jh<nh; jh++)
  {
   NcSignal* sx=(NcSignal*)hits.At(jh);
   if (!sx) continue;

   if (cln && (sx->GetDeadValue("ADC") || sx->GetDeadValue("LE") || sx->GetDeadValue("TOT"))) continue;
   if (!slc && sx->GetSignal("SLC")>0.5) continue;

   IceGOM* omx=(IceGOM*)sx->GetDevice();
   if (!omx) continue;

   // The hit position dependent scattering length
   if (omx->InheritsFrom("IceAOM")) // Amanda reconstruction
   {
    lambda=fLambdaA;
   }
   else // IceCube reconstruction
   {
    lambda=fLambdaDL; // The ice at the Dust Layer
    zhit=omx->GetX(3,"car");
    if (zhit>-50) lambda=fLambdaUD;  // The Ice in the Upper Detector above the dustlayer
    if (zhit<-150) lambda=fLambdaLD; // Clearest Ice in the Lower Detector under the dustlayer
   }

   rhit=omx->GetPosition();
   d=te->GetDistance(rhit);
   r12=rhit-(*tr0);
   hproj=p.Dot(r12)/p.GetNorm();
   dist=fabs(hproj)+d/tan(pi/2.-thetac-alphac);
   if (hproj<0) dist=-dist;
   tgeo=t0+dist/c;
   thit=sx->GetSignal("LE",7);
   tres=thit-tgeo;

   d=d/sin(thetac); // The distance traveled by a cherenkov photon

   if (tres<dtmin || tres>dtmax || d>maxdhit*lambda) continue;

   // Associate this hit to the TE
   te->AddSignal(*sx);
   levers.Enter(fabs(hproj));
   hprojs.Enter(hproj);
   times.Enter(tres);
   frac=d/lambda;
   if (frac<1) frac=1;
   if (fHitweight>=0)
   {
    nah=nah+(fHitweight/frac);
    if (sx->GetSignal("SLC")<0.5) nahlc=nahlc+(fHitweight/frac);
   }
   else if (fHitweight>-1.5)
   {
    nah=nah+1;
    if (sx->GetSignal("SLC")<0.5) nahlc=nahlc+1;
   }
   else
   {
    amp=sx->GetSignal("ADC",7);
    nah=nah+(amp/frac);
    if (sx->GetSignal("SLC")<0.5) nahlc=nahlc+(amp/frac);
   }
  }

  // Determine the Q quality of the various TE's.
  // Good quality TE's will be called track candidates (TC's)
  nas=fEvt->GetNstrings(*te,"IceGOM");
  nam=fEvt->GetNmodules(*te,"IceGOM");
  nax=0;
  frac=0;
  if (nah>0) frac=nahlc/nah;
  if (nas>0 && nam>0)
  {
   if (astype==1) nax=nah;
   if (astype==2) nax=float(nas);
   if (astype==3) nax=nah*float(nas);
   if (astype==4) nax=nah+frac+ws*float(nas-1)/float(nas);
   if (astype==-1) nax=float(nam)+ws*float(nas);
   if (astype==-2) nax=float(nam)+nah/float(nam);
   if (astype==-3) nax=float(nam*nas);
   if (astype==-4) nax=float(nam)+frac+ws*float(nas-1)/float(nas);
   if (astype==-5) nax=float(nam)+nah+frac+ws*float(nas-1)/float(nas);
  }
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

  if (qtc>qmax) qmax=qtc;

  fit.SetSignal(qtc,"QTC");
  fit.SetSignal(nas,"Nstrings");
  fit.SetSignal(nam,"Nmods");
  fit.SetSignal(nah,"Nhits");
  fit.SetSignal(nahlc,"Nhlc");
  fit.SetSignal(nax,"Nax");
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
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SelectQvalue(TObjArray& tes,Float_t qmax)
{
 // Perform selection on Q value in case of multiple track candidates

 Int_t nte=tes.GetEntries();
 Float_t qtc=0;
 Float_t nax=0;
 Nc3Vector p;
 for (Int_t jtc=0; jtc<nte; jtc++)
 {
  NcTrack* te=(NcTrack*)tes.At(jtc);
  if (!te) continue;
  NcSignal* sx=(NcSignal*)te->GetFitDetails();
  qtc=-1;
  nax=0;
  if (sx)
  {
   qtc=sx->GetSignal("QTC");
   nax=sx->GetSignal("Nax");
  }

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
void IceDwalk::ClusterTracks(TObjArray& tes,TObjArray& jets,Float_t tangmax,Int_t tinvol,Float_t tdistmax,Float_t qmax)
{
 // Cluster track candidates within a certain opening angle into jets.
 // Also the track should be within a certain maximum distance of the
 // starting track in order to get clustered.
 // The latter prevents clustering of (nearly) parallel track candidates
 // crossing the detector a very different locations (e.g. muon bundles).
 // The average r0 and t0 of the constituent tracks will be taken as the
 // jet reference point. 

 NcSignal usd; // Storage of total Q value etc... in a jet via user data
 usd.AddNamedSlot("Qvalue");
 usd.AddNamedSlot("Ntcs");
 usd.AddNamedSlot("Ntcsmax");
 usd.AddNamedSlot("Nstrings");
 usd.AddNamedSlot("Nstringsmax");
 usd.AddNamedSlot("Nmods");
 usd.AddNamedSlot("Nmodsmax");
 usd.AddNamedSlot("Nhits");
 usd.AddNamedSlot("Nhitsmax");
 usd.AddNamedSlot("Nhitshlc");
 usd.AddNamedSlot("Nhitshlcmax");
 usd.AddNamedSlot("AvQTC");
 usd.AddNamedSlot("QTCmax");

 Int_t nte=tes.GetEntries();
 Float_t ang=0;
 NcSample pos;
 NcSample time;
 Float_t vec[3],err[3];
 NcPosition r0;
 Float_t t0,dist,dist2;
 Int_t ntk=0;      // Number of track candidates used for a jet
 Int_t ntkmax=0;   // Maximum number of track candidates used for a jet
 Int_t nam=0;      // Number of associated (D)OMs
 Int_t nammax=0;   // Maximum number of associated (D)OMs
 Int_t nah=0;      // Number of associated hits
 Int_t nahmax=0;   // Maximum number of associated hits
 Int_t nahlc=0;    // Number of associated HLC hits
 Int_t nahlcmax=0; // Maximum number of associated HLC hits
 Int_t nas=0;      // Number of associated strings
 Int_t nasmax=0;   // Maximum number of associated strings
 Float_t qtc=0;
 Float_t avqtc=0;
 TObjArray* signals=0;

 // Loop over the various TCs to start the various jets 
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

  // Look for additional TCs to be clustered into this jet
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
   ntk=jx->GetNtracks();
   if (ntk>ntkmax) ntkmax=ntk;
   nam=fEvt->GetNmodules(*jx,"IceGOM");
   if (nam>nammax) nammax=nam;
   nas=fEvt->GetNstrings(*jx,"IceGOM");
   if (nas>nasmax) nasmax=nas;
   nah=jx->GetNsignals("IceGOM",2);
   if (nah>nahmax) nahmax=nah;
   nahlc=0;
   signals=jx->GetSignals("IceGOM",2);
   if (signals)
   {
    for (Int_t is=0; is<signals->GetEntries(); is++)
    {
     NcSignal* sx=(NcSignal*)signals->At(is);
     if (!sx) continue;
     if (sx->GetSignal("SLC")<0.5) nahlc++;
    }
   }
   if (nahlc>nahlcmax) nahlcmax=nahlc;
  }
  else // Only keep single-track jets which have qtc=qmax 
  {
   NcSignal* sx1=(NcSignal*)te->GetFitDetails();
   qtc=-1;
   if (sx1) qtc=sx1->GetSignal("QTC");
   if (qtc>=(qmax-1.e-10))
   {
    jets.Add(jx);
    ntk=jx->GetNtracks();
    if (ntk>ntkmax) ntkmax=ntk;
    nam=fEvt->GetNmodules(*jx,"IceGOM");
    if (nam>nammax) nammax=nam;
    nas=fEvt->GetNstrings(*jx,"IceGOM");
    if (nas>nasmax) nasmax=nas;
    nah=jx->GetNsignals("IceGOM",2);
    if (nah>nahmax) nahmax=nah;
    nahlc=0;
    signals=jx->GetSignals("IceGOM",2);
    if (signals)
    {
     for (Int_t is=0; is<signals->GetEntries(); is++)
     {
      NcSignal* sx=(NcSignal*)signals->At(is);
      if (!sx) continue;
      if (sx->GetSignal("SLC")<0.5) nahlc++;
     }
    }
    if (nahlc>nahlcmax) nahlcmax=nahlc;
   }
   else
   {
    delete jx;
   }
  }
 }

 Int_t njets=jets.GetEntries();
 if (!njets) return;

 // For each jet the sum of nam/nammax, nah/nahmax, nahlc/nahlcmax and (1/qmax) times the average qtc value per jet-track
 // will be stored as the jet energy to enable sorting on this value lateron
 Float_t sortval=0;
 for (Int_t ijet=0; ijet<njets; ijet++)
 {
  NcJet* jx=(NcJet*)jets.At(ijet);
  if (!jx) continue;
  nah=jx->GetNsignals("IceGOM",2);
  nas=fEvt->GetNstrings(*jx,"IceGOM");
  nam=fEvt->GetNmodules(*jx,"IceGOM");
  qtc=jx->GetMomentum();
  avqtc=0;
  ntk=jx->GetNtracks();
  if (ntk) avqtc=qtc/float(ntk);
  nahlc=0;
  signals=jx->GetSignals("IceGOM",2);
  if (signals)
  {
   for (Int_t is=0; is<signals->GetEntries(); is++)
   {
    NcSignal* sx=(NcSignal*)signals->At(is);
    if (!sx) continue;
    if (sx->GetSignal("SLC")<0.5) nahlc++;
   }
  }
  sortval=0;
  if (qmax>0) sortval=avqtc/qmax;
  if (nammax>0) sortval+=float(nam)/float(nammax);
  if (nahmax>0) sortval+=float(nah)/float(nahmax);
  if (nahlcmax>0) sortval+=float(nahlc)/float(nahlcmax);
  jx->SetScalar(sortval);

  usd.SetSignal(sortval,"Qvalue");
  usd.SetSignal(ntk,"Ntcs");
  usd.SetSignal(ntkmax,"Ntcsmax");
  usd.SetSignal(nas,"Nstrings");
  usd.SetSignal(nasmax,"Nstringsmax");
  usd.SetSignal(nam,"Nmods");
  usd.SetSignal(nammax,"Nmodsmax");
  usd.SetSignal(nah,"Nhits");
  usd.SetSignal(nahmax,"Nhitsmax");
  usd.SetSignal(nahlc,"Nhitshlc");
  usd.SetSignal(nahlcmax,"Nhitshlcmax");
  usd.SetSignal(avqtc,"AvQTC");
  usd.SetSignal(qmax,"QTCmax");
  jx->SetUserData(usd);
 }
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::MergeJets(TObjArray& jets2,Float_t jangmax,Float_t jdistmax,Int_t jinvol,Int_t jiterate,Float_t qmax)
{
 // Merge jets within a certain opening to provide the final track(s).
 // Also the jet should be within a certain maximum distance of the
 // starting jet in order to get merged.
 // The latter prevents merging of (nearly) parallel jets/tracks
 // crossing the detector a very different locations (e.g. muon bundles).
 // The average r0 and t0 of the constituent jets will be taken as the
 // final reference point. 

 NcSignal usd; // Storage of average Q value etc... in a jet via user data
 usd.AddNamedSlot("Qvalue");
 usd.AddNamedSlot("Ntcs");
 usd.AddNamedSlot("Ntcsmax");
 usd.AddNamedSlot("Nstrings");
 usd.AddNamedSlot("Nstringsmax");
 usd.AddNamedSlot("Nmods");
 usd.AddNamedSlot("Nmodsmax");
 usd.AddNamedSlot("Nhits");
 usd.AddNamedSlot("Nhitsmax");
 usd.AddNamedSlot("Nhitshlc");
 usd.AddNamedSlot("Nhitshlcmax");
 usd.AddNamedSlot("AvQTC");
 usd.AddNamedSlot("QTCmax");

 Int_t njets=jets2.GetEntries();
 NcJet* jx1=0;
 NcJet* jx2=0;
 Int_t merged=1;
 Int_t ntk=0;      // Number of track candidates in a jet
 Int_t ntkmax=0;   // Maximum number of track candidates in a jet
 Int_t nam=0;      // Number of associated (D)OMs
 Int_t nammax=0;   // Maximum number of associated (D)OMs
 Int_t nah=0;      // Number of associated hits
 Int_t nahmax=0;   // Maximum number of associated hits
 Int_t nahlc=0;    // Number of associated HLC hits
 Int_t nahlcmax=0; // Maximum number of associated HLC hits
 Int_t nas=0;      // Number of associated strings
 Int_t nasmax=0;   // Maximum number of associated strings
 Float_t qtc=0;
 Float_t avqtc=0;
 Float_t ang,dist,dist2,t0;
 NcSample pos;
 NcSample time;
 NcPosition r0;
 Float_t vec[3],err[3];
 Float_t sortval;
 TObjArray* signals=0;
 if (jangmax>=0)
 {
  while (merged)
  {
   merged=0;
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

    ntk=jx1->GetNtracks();
    if (ntk>ntkmax) ntkmax=ntk;
    nas=fEvt->GetNstrings(*jx1,"IceGOM");
    if (nas>nasmax) nasmax=nas;
    nam=fEvt->GetNmodules(*jx1,"IceGOM");
    if (nam>nammax) nammax=nam;
    nah=jx1->GetNsignals("IceGOM",2);
    if (nah>nahmax) nahmax=nah;
    nahlc=0;
    signals=jx1->GetSignals("IceGOM",2);
    if (signals)
    {
     for (Int_t is=0; is<signals->GetEntries(); is++)
     {
      NcSignal* sx=(NcSignal*)signals->At(is);
      if (!sx) continue;
      if (sx->GetSignal("SLC")<0.5) nahlc++;
     }
    }
    if (nahlc>nahlcmax) nahlcmax=nahlc;
   } // End of jet1 loop

   jets2.Compress();

   // For each jet the sum of nam/nammax, nah/nahmax, nahlc/nahlcmax and (1/qmax) times the average qtc value per jet-track
   // will be stored as the jet energy to enable sorting on this value lateron
   for (Int_t jjet=0; jjet<njets; jjet++)
   {
    NcJet* jx=(NcJet*)jets2.At(jjet);
    if (!jx) continue;
    nah=jx->GetNsignals("IceGOM",2);
    nas=fEvt->GetNstrings(*jx,"IceGOM");
    nam=fEvt->GetNmodules(*jx,"IceGOM");
    qtc=jx->GetMomentum();
    avqtc=0;
    ntk=jx->GetNtracks();
    if (ntk) avqtc=qtc/float(ntk);
    nahlc=0;
    signals=jx->GetSignals("IceGOM",2);
    if (signals)
    {
     for (Int_t is=0; is<signals->GetEntries(); is++)
     {
      NcSignal* sx=(NcSignal*)signals->At(is);
      if (!sx) continue;
      if (sx->GetSignal("SLC")<0.5) nahlc++;
     }
    }
    sortval=0;
    if (qmax>0) sortval=avqtc/qmax;
    if (nammax>0) sortval+=float(nam)/float(nammax);
    if (nahmax>0) sortval+=float(nah)/float(nahmax);
    if (nahlcmax>0) sortval+=float(nahlc)/float(nahlcmax);
    jx->SetScalar(sortval);

    usd.SetSignal(sortval,"Qvalue");
    usd.SetSignal(ntk,"Ntcs");
    usd.SetSignal(ntkmax,"Ntcsmax");
    usd.SetSignal(nas,"Nstrings");
    usd.SetSignal(nasmax,"Nstringsmax");
    usd.SetSignal(nam,"Nmods");
    usd.SetSignal(nammax,"Nmodsmax");
    usd.SetSignal(nah,"Nhits");
    usd.SetSignal(nahmax,"Nhitsmax");
    usd.SetSignal(nahlc,"Nhitshlc");
    usd.SetSignal(nahlcmax,"Nhitshlcmax");
    usd.SetSignal(avqtc,"AvQTC");
    usd.SetSignal(qmax,"QTCmax");
    jx->SetUserData(usd);
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
Int_t IceDwalk::StoreTracks(TObjArray& jets2,Int_t minahits,Int_t minamods,Float_t jangmax,TString name,TString title,TObjArray& hits)
{
 // Store every jet as a reconstructed track in the event structure,
 // provided it has sufficient associated hits.
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
 Int_t nah=0;
 Int_t nam=0;
 Float_t qval=0;
 Float_t qcut=-1;
 Int_t ntk=0;
 for (Int_t jet=0; jet<njets; jet++)
 {
  NcJet* jx=(NcJet*)jets2.At(jet);
  if (!jx) continue;
  NcPosition* ref=jx->GetReferencePoint();
  if (!ref) continue;

  // Keep only tracks with sufficient associated hits
  nah=jx->GetNsignals("IceGOM",2);
  if (nah<minahits) continue;

  // Keep only tracks with sufficient associated (D)oms
  nam=fEvt->GetNmodules(*jx,"IceGOM");
  if (nam<minamods) continue;

  // Keep only the tracks above a certain Qvalue threshold
  qval=jx->GetScalar();
  if (qcut<0) qcut=fQcut*qval; // The first jet has the highest Qvalue
  if (qval<qcut) continue;

  // Create a new track in the event structure and retrieve its pointer
  fEvt->AddTrack(t);
  NcTrack* trk=fEvt->GetTrack(fEvt->GetNtracks());
  if (!trk) continue;

  ntk++;
  trk->SetId(fEvt->GetNtracks(1)+1);
  p=jx->Get3Momentum();
  p/=p.GetNorm();
  trk->Set3Momentum(p);
  trk->SetReferencePoint(*ref);
  NcTimestamp* tt0=ref->GetTimestamp();
  if (tt0) trk->SetTimestamp(*tt0);

  // Store the jet user data as track fit details
  NcSignal* usd=(NcSignal*)jx->GetUserData();
  if (usd) trk->SetFitDetails(usd);

  // Link the associated hits to the created track
  for (Int_t jt=1; jt<=jx->GetNtracks(); jt++)
  {
   NcTrack* tx=jx->GetTrack(jt);
   if (!tx) continue;
   for (Int_t is=1; is<=tx->GetNsignals(); is++)
   {
    NcSignal* sx=tx->GetSignal(is);
    if (sx)
    {
     sx->AddTrack(*trk);
     if (fConditional>=6) hits.Remove(sx);
    }
   }
  }

  // Check whether the track direction has to be reversed
  FlipTrack(trk);

  // Only take the jet with the highest quality number
  // (i.e. the first jet in the list) when the user had selected
  // this reconstruction mode.
  if (jangmax<0) break;
 }
 if (fConditional>=6) hits.Compress();

 return ntk;
}
///////////////////////////////////////////////////////////////////////////
