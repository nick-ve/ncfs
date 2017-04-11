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

// $Id: IceCleanHits.cxx 57 2012-02-07 10:05:54Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class IceCleanHits
// TTask derived class to perform hit cleaning.
//
// In case an event has been rejected by an NcEventSelector (based) processor,
// this task (and its sub-tasks) is not executed.
//
// Criteria applied for Amanda MuDaq data :
// ----------------------------------------
// 1) ADC within [min,max]  Default : [0.3,999999] PE
// 2) TOT within [min,max]  Default : electrical [125,2000] optical [20,2000] ns
// 3) abs(LE-Ttrig)<=win    Default : win=2250 TDC counts
//    where : LE=uncalibrated hit LE (i.e. TDC counts)   Ttrig=selected trigger time in TDC counts
// 4) At least one hit in another OM within radius R and time difference dt
//    to remove isolated hits. Defaults : R=70 m  dt=500 ns
//
// Criteria applied for Amanda TWRDaq data :
// ----------------------------------------
// 1) ADC within [min,max]  Default : [0.3,999999] PE
// 2) TOT within [min,max]  Default : electrical [125,2000] optical [20,2000] ns
// 3) abs(LE-Ttrig)<=win    Default : win=3000 ns
//    where : LE=uncalibrated hit LE    Ttrig=selected trigger time
// 4) At least one hit in another OM within radius R and time difference dt
//    to remove isolated hits. Defaults : R=70 m  dt=500 ns
//
// Criteria applied for standard IceCube InIce DOM data :
// ------------------------------------------------------
// 1) ADC within [min,max]  Default : [0,999999] PE
// 2) TOT within [min,max]  Default : [0,999999] ns
// 3) abs(LE-Ttrig)<=win    Default : win=3000 ns
//    where : LE=hit LE     Ttrig=selected trigger time(s)
// 4) At least one hit in another DOM within radius R and time difference dt
//    to remove isolated hits. Defaults : R=150 m  dt=1000 ns
//
// Criteria applied for DeepCore DOM data :
// ----------------------------------------
// 1) ADC within [min,max]  Default : [0,999999] PE
// 2) TOT within [min,max]  Default : [0,999999] ns
// 3) abs(LE-Ttrig)<=win    Default : win=1000 ns
//    where : LE=hit LE     Ttrig=selected trigger time(s)
// 4) At least one hit in another DOM within radius R and time difference dt
//    to remove isolated hits. Defaults : R=75 m  dt=500 ns
//
// Criteria applied for IceTop DOM data :
// --------------------------------------
// 1) ADC within [min,max]  Default : [0,999999] PE
// 2) TOT within [min,max]  Default : [0,999999] ns
// 3) abs(LE-Ttrig)<=win    Default : win=999999 ns
//    where : LE=hit LE     Ttrig=selected trigger time(s)
// 4) At least one hit in another DOM within radius R and time difference dt
//    to remove isolated hits. Defaults : R=150 m  dt=1000 ns
//
// For AMANDA data the actual DAQ system is obtained automatically from the IceEvent structure
// via the device called "Daq".
//
// The defaults of the various parameters can be changed by the corresponding
// Set memberfunctions.
//
// Information about the actual parameter settings can be found in the event
// structure itself via the device named "IceCleanHits".
//
// Concerning the trigger time :
// -----------------------------
// The user can impose a specific trigger name or time to be used
// by invokation of the memberfunctions SetTname or SetTtime, respectively.
// The (uncalibrated) LE of a specified (by name) trigger pulse is used
// as obtained from the IceEvent structure (via the GetTriggerTime facility).
// Specification of a negative trigger time will result in the automatic
// trigger time setting corresponding to the median of recorded hit times
// or determination of multiple trigger times based on a spectral analysis
// of the observed hit times.
// In case of automatic trigger time selection, only non-isolated HLC hits
// will be used to determine the trigger time(s).
// For further details please refer to the docs of the memberfunctions
// SetTname and SetTtime.
//
// The hits which do not fullfill the criteria are flagged "dead" for the
// corresponding signal slot. This means they are still present in the
// IceEvent structure and are as such still accessible.
// It is left to the user to decide (based on the various "dead" flag settings)
// whether or not to use these hits in his/her reconstruction or analysis.
//
// Note : This processor only works properly on Time and ADC calibrated data.
//
//--- Author: Nick van Eijndhoven 13-oct-2005 Utrecht University
//- Modified: NvE $Date: 2012-02-07 11:05:54 +0100 (Tue, 07 Feb 2012) $ NCFS
///////////////////////////////////////////////////////////////////////////
 
#include "IceCleanHits.h"
#include "Riostream.h"

ClassImp(IceCleanHits) // Class implementation to enable ROOT I/O

IceCleanHits::IceCleanHits(const char* name,const char* title) : TTask(name,title)
{
// Default constructor.
 fEvt=0;
 fAdcminAM=0.3;
 fAdcmaxAM=999999;
 fAdcminAT=0.3;
 fAdcmaxAT=999999;
 fAdcminT=0;
 fAdcmaxT=999999;
 fAdcminIC=0;
 fAdcmaxIC=999999;
 fAdcminDC=0;
 fAdcmaxDC=999999;
 fTotminAEM=125;
 fTotmaxAEM=2000;
 fTotminAOM=20;
 fTotmaxAOM=2000;
 fTotminAET=125;
 fTotmaxAET=2000;
 fTotminAOT=20;
 fTotmaxAOT=2000;
 fTotminT=0;
 fTotmaxT=999999;
 fTotminIC=0;
 fTotmaxIC=999999;
 fTotminDC=0;
 fTotmaxDC=999999;
 fRmaxA=70;
 fDtmaxA=500;
 fRmaxT=150;
 fDtmaxT=1000;
 fRmaxIC=150;
 fDtmaxIC=1000;
 fRmaxDC=75;
 fDtmaxDC=500;
 fTnamAM="main";
 fTtimAM=-1;
 fTnamAT="main";
 fTtimAT=-1;
 fTnamT="user";
 fSlcT=0;
 fMultT=1;
 fTtimT=-1;
 fTnamIC="user";
 fSlcIC=0;
 fMultIC=1;
 fTtimIC=-1;
 fTnamDC="user";
 fSlcDC=0;
 fMultDC=1;
 fTtimDC=-1;
 fTwinAM=2250;
 fTwinAT=3000;
 fTwinT=999999;
 fTwinIC=3000;
 fTwinDC=1000;
}
///////////////////////////////////////////////////////////////////////////
IceCleanHits::~IceCleanHits()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::SetAdcRange(Float_t min,Float_t max,TString s)
{
// Set ADC range in PE.
//
// The argument "s" specifies the sensor system as follows :
//
// s = "MuDaq"  ==> Amanda modules under the MuDaq system
//     "TWRDaq" ==> Amanda modules under the TWRDaq system
//     "TDOM"   ==> IceTop DOMs
//     "ICDOM"  ==> Standard IceCube InIce DOMs
//     "DCDOM"  ==> DeepCore DOMs

 if (s=="MuDaq")
 {
  fAdcminAM=min;
  fAdcmaxAM=max;
 }
 if (s=="TWRDaq")
 {
  fAdcminAT=min;
  fAdcmaxAT=max;
 }
 if (s=="TDOM")
 {
  fAdcminT=min;
  fAdcmaxT=max;
 }
 if (s=="ICDOM")
 {
  fAdcminIC=min;
  fAdcmaxIC=max;
 }
 if (s=="DCDOM")
 {
  fAdcminDC=min;
  fAdcmaxDC=max;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::SetTotRange(Float_t min,Float_t max,TString s)
{
// Set TOT range in ns.
//
// The argument "s" specifies the sensor system as follows :
//
// s = "AEM"   ==> Amanda modules with electrical readout under the MuDaq system
//     "AOM"   ==> Amanda modules with optical fibre readout under the MuDaq system
//     "AET"   ==> Amanda modules with electrical readout under the TWRDaq system
//     "AOT"   ==> Amanda modules with optical fibre readout under the TWRDaq system
//     "TDOM    ==> IceTop DOMs
//     "ICDOM"  ==> Standard IceCube InIce DOMs
//     "DCDOM"  ==> DeepCore DOMs

 if (s=="AEM")
 {
  fTotminAEM=min;
  fTotmaxAEM=max;
 }
 if (s=="AOM")
 {
  fTotminAOM=min;
  fTotmaxAOM=max;
 }
 if (s=="AET")
 {
  fTotminAET=min;
  fTotmaxAET=max;
 }
 if (s=="AOT")
 {
  fTotminAOT=min;
  fTotmaxAOT=max;
 }
 if (s=="TDOM")
 {
  fTotminT=min;
  fTotmaxT=max;
 }
 if (s=="ICDOM")
 {
  fTotminIC=min;
  fTotmaxIC=max;
 }
 if (s=="DCDOM")
 {
  fTotminDC=min;
  fTotmaxDC=max;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::SetIsolation(Float_t rmax,Float_t dtmax,TString s)
{
// Set maximum hit isolation radius (in m) and time difference (in ns).
//
// The argument "s" specifies the sensor system as follows :
//
// s = "AOM"    ==> Amanda modules
//     "TDOM    ==> IceTop DOMs
//     "ICDOM"  ==> Standard IceCube InIce DOMs
//     "DCDOM"  ==> DeepCore DOMs
//
// Note : In case one of the arguments "rmax" or "dtmax" is negative,
//        the corresponding value will not be modified.

 if (s=="AOM")
 {
  if (rmax >= 0) fRmaxA=rmax;
  if (dtmax >= 0) fDtmaxA=dtmax;
 }
 if (s=="TDOM")
 {
  if (rmax >= 0) fRmaxT=rmax;
  if (dtmax >= 0) fDtmaxT=dtmax;
 }
 if (s=="ICDOM")
 {
  if (rmax >= 0) fRmaxIC=rmax;
  if (dtmax >= 0) fDtmaxIC=dtmax;
 }
 if (s=="DCDOM")
 {
  if (rmax >= 0) fRmaxDC=rmax;
  if (dtmax >= 0) fDtmaxDC=dtmax;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::SetTwindow(Float_t dtmax,TString s)
{
// Set the maximal time window around the trigger time.
// Only hits which occur in [T-dtmax,T+dtmax] will be kept,
// where T indicates the trigger time.
// For the Amanda MuDaq hardware, the times are all in TDC counts,
// where 1 TDC corresponds to about 1.04 ns. 
// For all other systems the times are all in nanoseconds.
//
// The argument "s" specifies the sensor system as follows :
//
// s = "MuDaq"  ==> Amanda modules under the MuDaq system
//     "TWRDaq" ==> Amanda modules under the TWRDaq system
//     "TDOM"   ==> IceTop DOMs
//     "ICDOM"  ==> Standard IceCube InIce DOMs
//     "DCDOM"  ==> DeepCore DOMs

 if (s=="MuDaq") fTwinAM=dtmax;
 if (s=="TWRDaq") fTwinAT=dtmax;
 if (s=="TDOM") fTwinT=dtmax;
 if (s=="ICDOM") fTwinIC=dtmax;
 if (s=="DCDOM") fTwinDC=dtmax;
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::SetTname(TString name,TString s)
{
// Specify the trigger name to be used to derive the trigger time for the
// time cleaning of a certain class of sensors.
//
// Note : Specification of name="Average" will result in a trigger time
//        being the "median" of all encountered non-zero trigger times.
//
// The argument "s" specifies the sensor system as follows :
//
// s = "MuDaq"  ==> Amanda modules under the MuDaq system
//     "TWRDaq" ==> Amanda modules under the TWRDaq system
//     "TDOM"   ==> IceTop DOMs
//     "ICDOM"  ==> Standard IceCube InIce DOMs
//     "DCDOM"  ==> DeepCore DOMs
//
// Specification of a non-existing trigger name will result in a trigger time
// value of 0.
//
// The used trigger time value will be stored in the corresponding slot of the
// "IceCleanHits" device in the event structure.
//
// Note : The user can also impose a certain trigger time without using any
//        trigger information.
//        This is achieved by invoking the memberfunction SetTtime.

 if (s=="MuDaq")
 {
  fTnamAM=name;
  fTtimAM=0;
  if (name=="Average") fTtimAM=-1;
 }
 if (s=="TWRDaq")
 {
  fTnamAT=name;
  fTtimAT=0;
  if (name=="Average") fTtimAT=-1;
 }
 if (s=="TDOM")
 {
  fTnamT=name;
  fTtimT=0;
  if (name=="Average") fTtimT=-1;
 }
 if (s=="ICDOM")
 {
  fTnamIC=name;
  fTtimIC=0;
  if (name=="Average") fTtimIC=-1;
 }
 if (s=="DCDOM")
 {
  fTnamDC=name;
  fTtimDC=0;
  if (name=="Average") fTtimDC=-1;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::SetTtime(Float_t t,TString s,Int_t slc,Int_t mult)
{
// Set the trigger time(s) for a certain class of sensors.
// For the Amanda MuDaq hardware, the times are all in TDC counts,
// where 1 TDC corresponds to about 1.04 ns. 
// For the other systems the times are all in nanoseconds.
//
// The argument "s" specifies the sensor system as follows :
//
// s = "MuDaq"  ==> Amanda modules under the MuDaq system
//     "TWRDaq" ==> Amanda modules under the TWRDaq system
//     "TDOM"   ==> IceTop DOMs
//     "ICDOM"  ==> Standard IceCube InIce DOMs
//     "DCDOM"  ==> DeepCore DOMs
//
// A negative value will induce automatic trigger time(s) setting.
//
// In case of automatic trigger time determination, the "median" of the hit times
// of the specified class of sensors, as recorded in the IceEvent structure,
// is calculated.
// Via the argument "slc" the user can select to use (1) or not use (0) SLC hits
// in the automatic trigger time determination for the specified class of sensors.
// This "median" value will then be stored (as a negative value) in the
// corresponding slot of the "IceCleanHits" device in the event structure.
//
// The argument "mult" allows to select (1) or not select (0) a spectral analysis
// of the hit time distributions of the corresponding class of sensors.
// In this analysis various peaks in the hit time distribution will be identified
// with different trigger times. Each identified peak will be treated as an individual
// trigger time with the corresponding time window around it.
// This will allow to cope with coincident tracks in the detector.
// In case no peaks are found, the "median" trigger time as mentioned above will be used.
// 
// For a user selected trigger time, this trigger time will be stored in the
// corresponding slot of the "IceCleanHits" device in the event structure.
//
// The default values are : slc=0 and mult=1.
//
// Notes :
// -------
// 1) The arguments "slc" and "mult" are only used for IceCube hits, not for Amanda.
// 2) In the constructor of this class the defaults have been set to always perform
//    automatic trigger time determination with slc=0 and mult=1.

 if (s=="MuDaq")
 {
  fTtimAM=t;
  fTnamAM="user";
 }
 if (s=="TWRDaq")
 {
  fTtimAT=t;
  fTnamAT="user";
 }
 if (s=="TDOM")
 {
  fTtimT=t;
  fSlcT=slc;
  fMultT=mult;
  fTnamT="user";
 }
 if (s=="ICDOM")
 {
  fTtimIC=t;
  fSlcIC=slc;
  fMultIC=mult;
  fTnamIC="user";
 }
 if (s=="DCDOM")
 {
  fTtimDC=t;
  fSlcDC=slc;
  fMultDC=mult;
  fTnamDC="user";
 }
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::Exec(Option_t* opt)
{
// Implementation of the hit cleaning procedures.

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

 // Storage of the used parameters in the IceCleanHits device
 NcDevice params;
 params.SetNameTitle("IceCleanHits","IceCleanHits processor parameters");
 params.AddNamedSlot("AdcminAM");
 params.AddNamedSlot("AdcmaxAM");
 params.AddNamedSlot("AdcminAT");
 params.AddNamedSlot("AdcmaxAT");
 params.AddNamedSlot("AdcminT");
 params.AddNamedSlot("AdcmaxT");
 params.AddNamedSlot("AdcminIC");
 params.AddNamedSlot("AdcmaxIC");
 params.AddNamedSlot("AdcminDC");
 params.AddNamedSlot("AdcmaxDC");
 params.AddNamedSlot("TotminAEM");
 params.AddNamedSlot("TotmaxAEM");
 params.AddNamedSlot("TotminAOM");
 params.AddNamedSlot("TotmaxAOM");
 params.AddNamedSlot("TotminAET");
 params.AddNamedSlot("TotmaxAET");
 params.AddNamedSlot("TotminAOT");
 params.AddNamedSlot("TotmaxAOT");
 params.AddNamedSlot("TotminT");
 params.AddNamedSlot("TotmaxT");
 params.AddNamedSlot("TotminIC");
 params.AddNamedSlot("TotmaxIC");
 params.AddNamedSlot("TotminDC");
 params.AddNamedSlot("TotmaxDC");
 params.AddNamedSlot("RmaxA");
 params.AddNamedSlot("DtmaxA");
 params.AddNamedSlot("RmaxT");
 params.AddNamedSlot("DtmaxT");
 params.AddNamedSlot("RmaxIC");
 params.AddNamedSlot("DtmaxIC");
 params.AddNamedSlot("RmaxDC");
 params.AddNamedSlot("DtmaxDC");
 params.AddNamedSlot("TwinAM");
 params.AddNamedSlot("TtimAM");
 params.AddNamedSlot("TwinAT");
 params.AddNamedSlot("TtimAT");
 params.AddNamedSlot("TwinT");
 params.AddNamedSlot("SlcT");
 params.AddNamedSlot("MultT");
 params.AddNamedSlot("TtimT");
 params.AddNamedSlot("TwinIC");
 params.AddNamedSlot("SlcIC");
 params.AddNamedSlot("MultIC");
 params.AddNamedSlot("TtimIC");
 params.AddNamedSlot("TwinDC");
 params.AddNamedSlot("SlcDC");
 params.AddNamedSlot("MultDC");
 params.AddNamedSlot("TtimDC");

 params.SetSignal(fAdcminAM,"AdcminAM");
 params.SetSignal(fAdcmaxAM,"AdcmaxAM");
 params.SetSignal(fAdcminAT,"AdcminAT");
 params.SetSignal(fAdcmaxAT,"AdcmaxAT");
 params.SetSignal(fAdcminT,"AdcminT");
 params.SetSignal(fAdcmaxT,"AdcmaxT");
 params.SetSignal(fAdcminIC,"AdcminIC");
 params.SetSignal(fAdcmaxIC,"AdcmaxIC");
 params.SetSignal(fAdcminDC,"AdcminDC");
 params.SetSignal(fAdcmaxDC,"AdcmaxDC");
 params.SetSignal(fTotminAEM,"TotminAEM");
 params.SetSignal(fTotmaxAEM,"TotmaxAEM");
 params.SetSignal(fTotminAOM,"TotminAOM");
 params.SetSignal(fTotmaxAOM,"TotmaxAOM");
 params.SetSignal(fTotminAET,"TotminAET");
 params.SetSignal(fTotmaxAET,"TotmaxAET");
 params.SetSignal(fTotminAOT,"TotminAOT");
 params.SetSignal(fTotmaxAOT,"TotmaxAOT");
 params.SetSignal(fTotminT,"TotminT");
 params.SetSignal(fTotmaxT,"TotmaxT");
 params.SetSignal(fTotminIC,"TotminIC");
 params.SetSignal(fTotmaxIC,"TotmaxIC");
 params.SetSignal(fTotminDC,"TotminDC");
 params.SetSignal(fTotmaxDC,"TotmaxDC");
 params.SetSignal(fRmaxA,"RmaxA");
 params.SetSignal(fDtmaxA,"DtmaxA");
 params.SetSignal(fRmaxT,"RmaxT");
 params.SetSignal(fDtmaxT,"DtmaxT");
 params.SetSignal(fRmaxIC,"RmaxIC");
 params.SetSignal(fDtmaxIC,"DtmaxIC");
 params.SetSignal(fRmaxDC,"RmaxDC");
 params.SetSignal(fDtmaxDC,"DtmaxDC");
 params.SetSignal(fTwinAM,"TwinAM");
 params.SetSignal(fTtimAM,"TtimAM");
 params.SetSignal(fTwinAT,"TwinAT");
 params.SetSignal(fTtimAT,"TtimAT");
 params.SetSignal(fTwinT,"TwinT");
 params.SetSignal(fSlcT,"SlcT");
 params.SetSignal(fMultT,"MultT");
 params.SetSignal(fTtimT,"TtimT");
 params.SetSignal(fTwinIC,"TwinIC");
 params.SetSignal(fSlcIC,"SlcIC");
 params.SetSignal(fMultIC,"MultIC");
 params.SetSignal(fTtimIC,"TtimIC");
 params.SetSignal(fTwinDC,"TwinDC");
 params.SetSignal(fSlcDC,"SlcDC");
 params.SetSignal(fMultDC,"MultDC");
 params.SetSignal(fTtimDC,"TtimDC");

 fEvt->AddDevice(params);

 Amanda();
 IceTop();
 ICdoms();
 DCdoms();
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::Amanda()
{
// Hit cleaning for Amanda modules.

 NcDevice* daq=(NcDevice*)fEvt->GetDevice("Daq");
 if (!daq) return;

 if (daq->GetSignal("Muon")) MuDaq();
 if (daq->GetSignal("TWR")) TWRDaq();
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::MuDaq()
{
// Hit cleaning for Amanda MuDaq data.

 // All Amanda OMs with a signal
 TObjArray* aoms=fEvt->GetDevices("IceAOM");
 if (!aoms) return;

 // Local OM array with bad/dead OMs (as indicated via IceCalibrate) discarded
 TObjArray oms;
 IceAOM* omx=0;
 for (Int_t i=0; i<aoms->GetEntries(); i++)
 {
  omx=(IceAOM*)aoms->At(i);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  oms.Add(omx);
 }

 // Determination of the selected trigger time
 if (fTnamAM != "user") // Trigger time derived from trigger data
 {
  fTtimAM=fEvt->GetTriggerTime(fTnamAM);
 }
 else // Trigger time from median of hit times
 {
  if (fTtimAM<0)
  {
   fTtimAM=fEvt->GetTriggerTime(fTnamAM,&oms);

   // Make the value negative to stay with median of hit times selection
   // for all following invokations.
   if (fTtimAM>0) fTtimAM*=-1.;
   if (!fTtimAM) fTtimAM=-0.001;
  }
 }

 // Store the trigger time in the parameters device
 NcDevice* params=(NcDevice*)fEvt->GetDevice("IceCleanHits");
 if (params) params->SetSignal(fTtimAM,"TtimAM");

 // Local array with clean hits
 TObjArray hits;
 Int_t omid=0;
 Int_t clean=1;
 NcSignal* sx=0;
 Float_t adc,le,tot;
 Int_t readout;
 for (Int_t iom=0; iom<oms.GetEntries(); iom++)
 {
  omx=(IceAOM*)oms.At(iom);
  if (!omx) continue;
  omid=omx->GetUniqueID();
  readout=int(omx->GetSignal("READOUT"));
  // General readout setting in case info was missing 
  if (!readout)
  {
   readout=1;
   if (omid>=303) readout=2; // Optical OMs
  }
  for (Int_t ih=1; ih<=omx->GetNhits(); ih++)
  {
   sx=omx->GetHit(ih);
   if (!sx) continue;
   adc=sx->GetSignal("ADC",7);
   le=sx->GetSignal("LE",7);
   tot=sx->GetSignal("TOT",7);

   // Remove hits with an ADC value outside the range
   if (adc<fAdcminAM || adc>fAdcmaxAM)
   {
    sx->SetDead("ADC");
    clean=0;
   }
   // Remove hits with a TOT value outside the range
   // Note : Different ranges for electrical and optical modules
   if (readout==1) // Electrical OMs
   {
    if (tot<fTotminAEM || tot>fTotmaxAEM)
    {
     sx->SetDead("TOT");
     clean=0;
    }
   }
   else // Optical OMs
   {
    if (tot<fTotminAOM || tot>fTotmaxAOM)
    {
     sx->SetDead("TOT");
     clean=0;
    }
   }
   // Remove hits that are outside the trigger time window.
   // Since the trigger time was determined from uncalibrated LE's
   // (to include cable length effects) the uncalibrated LE of each
   // hit should be used here as well. 
   le=sx->GetSignal("LE",-7);
   if (fabs(le-fTtimAM)>fTwinAM)
   {
     sx->SetDead("LE");
     clean=0;
   }
   // Store only the current clean hits in our local hit array
   // This will save CPU time for the isolation criterion 
   if (clean) hits.Add(sx);
  }
 }
 
 // Isolation cut
 // Only retain hits that have at least one hit of another OM within a certain
 // radius and within a certain time window
 Int_t nhits=hits.GetEntries();
 NcSignal* sx1=0;
 NcSignal* sx2=0;
 Float_t t1,t2;
 IceAOM* omx1=0;
 IceAOM* omx2=0;
 NcPosition r1;
 NcPosition r2;
 Float_t dt,dr;
 Int_t iso;
 for (Int_t jh1=0; jh1<nhits; jh1++)
 {
  sx1=(NcSignal*)hits.At(jh1);
  if (!sx1) continue;

  iso=1;
  for (Int_t jh2=0; jh2<nhits; jh2++)
  {
   sx2=(NcSignal*)hits.At(jh2);
   if (!sx2) continue;

   omx1=(IceAOM*)sx1->GetDevice();
   omx2=(IceAOM*)sx2->GetDevice();
   if (omx1==omx2) continue;

   t1=sx1->GetSignal("LE",7);
   t2=sx2->GetSignal("LE",7);
   dt=fabs(t2-t1);
   if (dt>fDtmaxA) continue;

   if (omx1 && omx2)
   {
    r1=omx1->GetPosition();
    r2=omx2->GetPosition();
    dr=r1.GetDistance(r2);
    if (dr>fRmaxA) continue;
    iso=0;
   }
  }
  if (iso) sx1->SetDead("LE");
 }   
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::TWRDaq()
{
// Hit cleaning for Amanda TWRDaq data.

 // All Amanda OMs with a signal
 TObjArray* aoms=fEvt->GetDevices("IceAOM");
 if (!aoms) return;

 // Local OM array with bad/dead OMs (as indicated via IceCalibrate) discarded
 TObjArray oms;
 IceAOM* omx=0;
 for (Int_t i=0; i<aoms->GetEntries(); i++)
 {
  omx=(IceAOM*)aoms->At(i);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  oms.Add(omx);
 }

 // Determination of the selected trigger time
 if (fTnamAT != "user") // Trigger time derived from trigger data
 {
  fTtimAT=fEvt->GetTriggerTime(fTnamAT);
 }
 else // Trigger time from median of hit times
 {
  if (fTtimAT<0)
  {
   fTtimAT=fEvt->GetTriggerTime(fTnamAT,&oms);

   // Make the value negative to stay with median of hit times selection
   // for all following invokations.
   if (fTtimAT>0) fTtimAT*=-1.;
   if (!fTtimAT) fTtimAT=-0.001;
  }
 }

 // Store the trigger time in the parameters device
 NcDevice* params=(NcDevice*)fEvt->GetDevice("IceCleanHits");
 if (params) params->SetSignal(fTtimAT,"TtimAT");

 // Local array with clean hits
 TObjArray hits;
 Int_t omid=0;
 Int_t clean=1;
 NcSignal* sx=0;
 Float_t adc,le,tot;
 Int_t readout;
 for (Int_t iom=0; iom<oms.GetEntries(); iom++)
 {
  omx=(IceAOM*)oms.At(iom);
  if (!omx) continue;
  omid=omx->GetUniqueID();
  readout=int(omx->GetSignal("READOUT"));
  // General readout setting in case info was missing 
  if (!readout)
  {
   readout=1;
   if (omid>=303) readout=2; // Optical OMs
  }
  for (Int_t ih=1; ih<=omx->GetNhits(); ih++)
  {
   sx=omx->GetHit(ih);
   if (!sx) continue;
   adc=sx->GetSignal("ADC",7);
   le=sx->GetSignal("LE",7);
   tot=sx->GetSignal("TOT",7);

   // Remove hits with an ADC value outside the range
   if (adc<fAdcminAT || adc>fAdcmaxAT)
   {
    sx->SetDead("ADC");
    clean=0;
   }
   // Remove hits with a TOT value outside the range
   // Note : Different ranges for electrical and optical modules
   if (readout==1) // Electrical OMs
   {
    if (tot<fTotminAET || tot>fTotmaxAET)
    {
     sx->SetDead("TOT");
     clean=0;
    }
   }
   else // Optical OMs
   {
    if (tot<fTotminAOT || tot>fTotmaxAOT)
    {
     sx->SetDead("TOT");
     clean=0;
    }
   }
   // Remove hits that are outside the trigger time window.
   // Since the trigger time was determined from uncalibrated LE's
   // (to include cable length effects) the uncalibrated LE of each
   // hit should be used here as well. 
   le=sx->GetSignal("LE",-7);
   if (fabs(le-fTtimAT)>fTwinAT)
   {
     sx->SetDead("LE");
     clean=0;
   }
   // Store only the current clean hits in our local hit array
   // This will save CPU time for the isolation criterion 
   if (clean) hits.Add(sx);
  }
 }
 
 // Isolation cut
 // Only retain hits that have at least one hit of another OM within a certain
 // radius and within a certain time window
 Int_t nhits=hits.GetEntries();
 NcSignal* sx1=0;
 NcSignal* sx2=0;
 Float_t t1,t2;
 IceAOM* omx1=0;
 IceAOM* omx2=0;
 NcPosition r1;
 NcPosition r2;
 Float_t dt,dr;
 Int_t iso;
 for (Int_t jh1=0; jh1<nhits; jh1++)
 {
  sx1=(NcSignal*)hits.At(jh1);
  if (!sx1) continue;

  iso=1;
  for (Int_t jh2=0; jh2<nhits; jh2++)
  {
   sx2=(NcSignal*)hits.At(jh2);
   if (!sx2) continue;

   omx1=(IceAOM*)sx1->GetDevice();
   omx2=(IceAOM*)sx2->GetDevice();
   if (omx1==omx2) continue;

   t1=sx1->GetSignal("LE",7);
   t2=sx2->GetSignal("LE",7);
   dt=fabs(t2-t1);
   if (dt>fDtmaxA) continue;

   if (omx1 && omx2)
   {
    r1=omx1->GetPosition();
    r2=omx2->GetPosition();
    dr=r1.GetDistance(r2);
    if (dr>fRmaxA) continue;
    iso=0;
   }
  }
  if (iso) sx1->SetDead("LE");
 }   
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::IceTop()
{
// Hit cleaning for IceTop DOMs.

 // All IceTop DOMs with a signal
 TObjArray* doms=fEvt->GetDevices("IceTDOM");
 if (!doms) return;

 TObjArray hits; // Local hit array
 IceGOM* omx=0;
 NcSignal* sx=0;
 for (Int_t i=0; i<doms->GetEntries(); i++)
 {
  omx=(IceGOM*)doms->At(i);
  if (!omx) continue;
  for (Int_t ih=1; ih<=omx->GetNhits(); ih++)
  {
   sx=omx->GetHit(ih);
   if (!sx) continue;

   if (omx->GetDeadValue("ADC")) sx->SetDead("ADC");
   if (omx->GetDeadValue("LE")) sx->SetDead("LE");
   if (omx->GetDeadValue("TOT")) sx->SetDead("TOT");

   hits.Add(sx);
  }
 }

 // ADC and TOT cleaning
 Amplitude(hits,fAdcminT,fAdcmaxT,fTotminT,fTotmaxT);

 // Isolated hit cleaning
 Isolation(hits,fDtmaxT,fRmaxT);

 // Determination of the selected trigger time
 TArrayF peaks;
 TArrayF* refp=0;
 if (fMultT) refp=&peaks;
 if (fTnamT != "user") // Trigger time derived from trigger data
 {
  fTtimT=fEvt->GetTriggerTime(fTnamT);
 }
 else // Trigger time from median of hit times
 {
  if (fTtimT<0)
  {
   fTtimT=fEvt->GetTriggerTime(fTnamT,&hits,fSlcT,refp);

   // Make the value negative to stay with median of hit times selection
   // for all following invokations.
   if (fTtimT>0) fTtimT*=-1.;
   if (!fTtimT) fTtimT=-0.001;
  }
 }

 // Store the trigger time in the parameters device
 NcDevice* params=(NcDevice*)fEvt->GetDevice("IceCleanHits");
 if (params) params->SetSignal(fTtimT,"TtimT");

 // Time window cleaning 
 TimeWindow(hits,fTtimT,fTwinT,refp);
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::ICdoms()
{
// Hit cleaning for the standard IceCube InIce DOMs.

 // All standard IceCube DOMs with a signal
 TObjArray* doms=fEvt->GetDevices("IceICDOM");
 if (!doms) return;

 TObjArray hits; // Local hit array
 IceGOM* omx=0;
 NcSignal* sx=0;
 for (Int_t i=0; i<doms->GetEntries(); i++)
 {
  omx=(IceGOM*)doms->At(i);
  if (!omx) continue;
  for (Int_t ih=1; ih<=omx->GetNhits(); ih++)
  {
   sx=omx->GetHit(ih);
   if (!sx) continue;

   if (omx->GetDeadValue("ADC")) sx->SetDead("ADC");
   if (omx->GetDeadValue("LE")) sx->SetDead("LE");
   if (omx->GetDeadValue("TOT")) sx->SetDead("TOT");

   hits.Add(sx);
  }
 }

 // ADC and TOT cleaning
 Amplitude(hits,fAdcminIC,fAdcmaxIC,fTotminIC,fTotmaxIC);

 // Isolated hit cleaning
 Isolation(hits,fDtmaxIC,fRmaxIC);

 // Determination of the selected trigger time
 TArrayF peaks;
 TArrayF* refp=0;
 if (fMultIC) refp=&peaks;
 if (fTnamIC != "user") // Trigger time derived from trigger data
 {
  fTtimIC=fEvt->GetTriggerTime(fTnamIC);
 }
 else // Trigger time from median of hit times
 {
  if (fTtimIC<0)
  {
   fTtimIC=fEvt->GetTriggerTime(fTnamIC,&hits,fSlcIC,refp);

   // Make the value negative to stay with median of hit times selection
   // for all following invokations.
   if (fTtimIC>0) fTtimIC*=-1.;
   if (!fTtimIC) fTtimIC=-0.001;
  }
 }

 // Store the trigger time in the parameters device
 NcDevice* params=(NcDevice*)fEvt->GetDevice("IceCleanHits");
 if (params) params->SetSignal(fTtimIC,"TtimIC");

 // Time window cleaning 
 TimeWindow(hits,fTtimIC,fTwinIC,refp);
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::DCdoms()
{
// Hit cleaning for the DeepCore DOMs.

 // All DeepCore DOMs with a signal
 TObjArray* doms=fEvt->GetDevices("IceDCDOM");
 if (!doms) return;

 TObjArray hits; // Local hit array
 IceGOM* omx=0;
 NcSignal* sx=0;
 for (Int_t i=0; i<doms->GetEntries(); i++)
 {
  omx=(IceGOM*)doms->At(i);
  if (!omx) continue;
  for (Int_t ih=1; ih<=omx->GetNhits(); ih++)
  {
   sx=omx->GetHit(ih);
   if (!sx) continue;

   if (omx->GetDeadValue("ADC")) sx->SetDead("ADC");
   if (omx->GetDeadValue("LE")) sx->SetDead("LE");
   if (omx->GetDeadValue("TOT")) sx->SetDead("TOT");

   hits.Add(sx);
  }
 }

 // ADC and TOT cleaning
 Amplitude(hits,fAdcminDC,fAdcmaxDC,fTotminDC,fTotmaxDC);

 // Isolated hit cleaning
 Isolation(hits,fDtmaxDC,fRmaxDC);

 // Determination of the selected trigger time
 TArrayF peaks;
 TArrayF* refp=0;
 if (fMultDC) refp=&peaks;
 if (fTnamDC != "user") // Trigger time derived from trigger data
 {
  fTtimDC=fEvt->GetTriggerTime(fTnamDC);
 }
 else // Trigger time from median of hit times
 {
  if (fTtimDC<0)
  {
   fTtimDC=fEvt->GetTriggerTime(fTnamDC,&hits,fSlcDC,refp);

   // Make the value negative to stay with median of hit times selection
   // for all following invokations.
   if (fTtimDC>0) fTtimDC*=-1.;
   if (!fTtimDC) fTtimDC=-0.001;
  }
 }

 // Store the trigger time in the parameters device
 NcDevice* params=(NcDevice*)fEvt->GetDevice("IceCleanHits");
 if (params) params->SetSignal(fTtimDC,"TtimDC");

 // Time window cleaning 
 TimeWindow(hits,fTtimDC,fTwinDC,refp);
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::Amplitude(TObjArray& hits,Float_t adcmin,Float_t adcmax,Float_t totmin,Float_t totmax) const
{
// Perform hit cleaning based on amplitude (ADC) and duration (TOT).

 NcSignal* sx=0;
 Float_t adc,tot;
 for (Int_t ih=0; ih<hits.GetEntries(); ih++)
 {
  sx=(NcSignal*)hits.At(ih);
  if (!sx) continue;
  adc=sx->GetSignal("ADC",7);
  tot=sx->GetSignal("TOT",7);

  // Reject hits with an ADC value outside the range
  if (adc<adcmin || adc>adcmax) sx->SetDead("ADC");

  // Reject hits with a TOT value outside the range
  if (tot<totmin || tot>totmax) sx->SetDead("TOT");
 }
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::Isolation(TObjArray& hits,Float_t dtmax,Float_t rmax) const
{
// Perform hit cleaning based on isolation criteria.
// Only retain hits that have at least one hit of another OM within a certain
// radius and within a certain time window

 NcSignal* sx1=0;
 NcSignal* sx2=0;
 Float_t t1,t2;
 IceGOM* omx1=0;
 IceGOM* omx2=0;
 NcPosition r1;
 NcPosition r2;
 Float_t dt,dr;
 Int_t iso;
 Int_t nhits=hits.GetEntries();
 for (Int_t jh1=0; jh1<nhits; jh1++)
 {
  sx1=(NcSignal*)hits.At(jh1);
  if (!sx1) continue;

  // No need to investigate further if LE was already rejected
  if (sx1->GetDeadValue("LE")) continue;

  iso=1;

  for (Int_t jh2=0; jh2<nhits; jh2++)
  {
   sx2=(NcSignal*)hits.At(jh2);
   if (!sx2) continue;

   if (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT")) continue;

   omx1=(IceGOM*)sx1->GetDevice();
   omx2=(IceGOM*)sx2->GetDevice();
   if (omx1==omx2) continue;

   t1=sx1->GetSignal("LE",7);
   t2=sx2->GetSignal("LE",7);
   dt=fabs(t2-t1);
   if (dt>dtmax) continue;

   if (omx1 && omx2)
   {
    r1=omx1->GetPosition();
    r2=omx2->GetPosition();
    dr=r1.GetDistance(r2);
    if (dr>rmax) continue;
    iso=0;
    break;
   }
  } // End of loop over hit2

  if (iso) sx1->SetDead("LE");
 } // End of loop over hit1   
}
///////////////////////////////////////////////////////////////////////////
void IceCleanHits::TimeWindow(TObjArray& hits,Float_t ttrig,Float_t twin,TArrayF* peaks) const
{
// Perform hit cleaning based on a time window around a trigger time.

 NcSignal* sx=0;
 Float_t le;
 Int_t npeaks=0;
 if (peaks) npeaks=peaks->GetSize();
 if (npeaks<1) npeaks=1;
 for (Int_t ih=0; ih<hits.GetEntries(); ih++)
 {
  sx=(NcSignal*)hits.At(ih);
  if (!sx) continue;

  // No need to investigate further if LE was already rejected
  if (sx->GetDeadValue("LE")) continue;

  le=sx->GetSignal("LE",7);

  // Retain only hits that are inside a trigger time window.
  sx->SetDead("LE");
  for (Int_t ipeak=0; ipeak<npeaks; ipeak++)
  {
   if (peaks) ttrig=peaks->At(ipeak);
   if (fabs(le-fabs(ttrig))<=twin)
   {
    sx->SetAlive("LE");
    break;
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
