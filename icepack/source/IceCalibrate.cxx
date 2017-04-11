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

// $Id: IceCalibrate.cxx 55 2012-01-03 13:30:57Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class IceCalibrate
// TTask derived class to perform the various calibrations.
//
// In case an event has been rejected by an NcEventSelector (based) processor,
// this task (and its sub-tasks) is not executed.
//
// This task takes the current event in memory and uses the attached
// OM database to access the various calibration functions.
// A specific OM database may be attached by means of the SetOMdbase()
// or SetCalibFile() memberfunctions.
// Further details about the OM database can be found in the docs
// of IceCal2Root and IceDB2Root.
//
// In the calibration procedure, all event data in memory is scanned and
// replaced by calibrated data if a calibration function is present.
// When data is succesfully calibrated, the corresponding de-calibration
// function is stored in the event data at the appropriate place to allow
// access to uncalibrated data as well (see NcSignal::GetSignal for 
// further details).
// When the input event in memory already contained calibrated data
// (i.e. de-calibration functions are present in the event data), the event
// data is first de-calibrated (using the corresponding de-calibration functions
// contained in the event data) before the new calibration is performed.
// In case no corresponding calibration function is present, the calibration
// of those specific data will not be performed.
// This implies that running this task on calibrated data without having
// attached an OM database, will result in fully de-calibrated data. 
// In case an OM slot was flagged as bad in the OM database, this flag
// will be copied into the event data for the corresponding OM.
//
// Information about the actual parameter settings can be found in the event
// structure itself via the device named "IceCalibrate".
//
//--- Author: Nick van Eijndhoven 18-sep-2005 Utrecht University
//- Modified: NvE $Date: 2012-01-03 14:30:57 +0100 (Tue, 03 Jan 2012) $ NCFS
///////////////////////////////////////////////////////////////////////////
 
#include "IceCalibrate.h"
#include "Riostream.h"

ClassImp(IceCalibrate) // Class implementation to enable ROOT I/O

IceCalibrate::IceCalibrate(const char* name,const char* title) : TTask(name,title)
{
// Default constructor.
 fCalfile=0;
 fMuDaqDB=0;
 fTWRDaqDB=0;
 fJEBTDaqDB=0;
 fJEBADaqDB=0;
}
///////////////////////////////////////////////////////////////////////////
IceCalibrate::~IceCalibrate()
{
// Default destructor.
 if (fCalfile)
 {
  delete fCalfile;
  fCalfile=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceCalibrate::SetOMdbase(NcObjMatrix* omdb, TString name)
{
// Set the pointer to the OM database.
// The following names can be used: MuDaq, TWRDaq, JEBTDaq and JEBADaq.
// Note : this will overrule a previously attached database. 
 if(name=="MuDaq") fMuDaqDB=omdb;
 else if(name=="TWRDaq") fTWRDaqDB=omdb;
 else if(name=="JEBTDaq") fJEBTDaqDB=omdb;
 else if(name=="JEBADaq") fJEBADaqDB=omdb;
 else cout << "IceCalibrate::SetOMdbase Unknown database: " << name.Data() << endl;
}
///////////////////////////////////////////////////////////////////////////
void IceCalibrate::SetCalibFile(TString name)
{
// Set the calibration ROOT file as created with IceCal2Root.
// Note : this will overrule a previously attached database. 
 if (fCalfile)
 {
  delete fCalfile;
  fCalfile=0;
 }
 fCalfile=new TFile(name.Data());

 fMuDaqDB=(NcObjMatrix*)fCalfile->Get("MuDaq-OMDBASE");
 fTWRDaqDB=(NcObjMatrix*)fCalfile->Get("TWRDaq-OMDBASE");
 fJEBTDaqDB=(NcObjMatrix*)fCalfile->Get("JEBTDaq-OMDBASE");
 fJEBADaqDB=(NcObjMatrix*)fCalfile->Get("JEBADaq-OMDBASE");
}
///////////////////////////////////////////////////////////////////////////
void IceCalibrate::Exec(Option_t* opt)
{
// Implementation of the various calibration procedures.

 TString name=opt;
 NcJob* parent=(NcJob*)(gROOT->GetListOfTasks()->FindObject(name.Data()));

 if (!parent) return;

 IceEvent* evt=(IceEvent*)parent->GetObject("IceEvent");
 if (!evt) return;

 // Only process accepted events
 NcDevice* seldev=(NcDevice*)evt->GetDevice("NcEventSelector");
 if (seldev)
 {
  if (seldev->GetSignal("Select") < 0.1) return;
 }

 Int_t mudaq=0;
 Int_t twrdaq=0;
 Int_t jebdaq=0;
 NcSignal* daq=(NcSignal*)evt->GetDevice("Daq");
 mudaq=int(daq->GetSignal("Muon"));
 twrdaq=int(daq->GetSignal("TWR"));
 jebdaq=int(daq->GetSignal("JEB"));

 // Storage of the used parameters in the IceCalibrate device
 NcDevice params;
 params.SetNameTitle("IceCalibrate","IceCalibrate processor parameters");
 params.AddNamedSlot("MuDaq-OMDBASE");
 if (fMuDaqDB) params.SetSignal(1,"MuDaq-OMDBASE");
 params.AddNamedSlot("TWRDaq-OMDBASE");
 if (fTWRDaqDB) params.SetSignal(1,"TWRDaq-OMDBASE");
 params.AddNamedSlot("JEBTDaq-OMDBASE");
 if (fJEBTDaqDB) params.SetSignal(1,"JEBTDaq-OMDBASE");
 params.AddNamedSlot("JEBADaq-OMDBASE");
 if (fJEBADaqDB) params.SetSignal(1,"JEBADaq-OMDBASE");

 evt->AddDevice(params);

 // All OMs with a signal
 TObjArray* mods=evt->GetDevices("IceGOM");
 if (!mods) return;

 Int_t nmods=mods->GetEntries();
 if (!nmods) return;

 IceGOM* ome=0; // The event OM pointer
 IceGOM* omd=0; // The database OM pointer
 Int_t id=0;
 Float_t adc=0,le=0,tot=0;    // Uncalibrated values
 Float_t cadc=0,cle=0,ctot=0; // Calibrated values
 TF1* fcal=0;
 TF1* fdecal=0;
 TString slotname;

 for (Int_t imod=0; imod<nmods; imod++)
 {
  ome=(IceGOM*)mods->At(imod);
  if (!ome) continue;

  id=ome->GetUniqueID();

  omd=0;

  // Amanda OM
  if(ome->InheritsFrom("IceAOM")){
   if(mudaq && fMuDaqDB) omd=(IceGOM*)fMuDaqDB->GetObject(id,1);
   if(twrdaq && fTWRDaqDB) omd=(IceGOM*)fTWRDaqDB->GetObject(id,1);
   if(jebdaq && fJEBTDaqDB) omd=(IceGOM*)fJEBTDaqDB->GetObject(id,1);
  }
  // IceCube DOM
  if(ome->InheritsFrom("IceDOM")){
   if(jebdaq && fJEBADaqDB) omd=(IceGOM*)fJEBADaqDB->GetObject(id,1);
  }

  // Set global OM constants
  if (omd)
  {
   ome->SetPosition((Nc3Vector&)omd->GetPosition());
   for (Int_t ind=1; ind<=omd->GetNnames(); ind++)
   {
    slotname=omd->GetSlotName(ind);
    ome->AddNamedSlot(slotname);
   }
   for (Int_t isd=1; isd<=omd->GetNvalues(); isd++)
   {
    ome->SetSignal(omd->GetSignal(isd),omd->GetSlotName(isd));
   }
  }

  // Make signals of bad modules available
  ome->SetAlive("ADC");
  ome->SetAlive("LE");
  ome->SetAlive("TOT");

  // (De)calibrate all hit signals for this OM
  for (Int_t ithit=1; ithit<=ome->GetNhits(); ithit++)
  {
   NcSignal* sx=ome->GetHit(ithit);
   if (!sx) continue;

   // ADC (de)calibration
   adc=sx->GetSignal("ADC",-7); // Get uncalibrated signal
   fcal=0;
   fdecal=0;
   if (omd)
   {
    fcal=omd->GetCalFunction("ADC");
    fdecal=omd->GetDecalFunction("ADC");
   }
   if (fcal) // Store calibrated signal
   {
    cadc=fcal->Eval(adc);
    sx->SetSignal(cadc,"ADC");
   }
   else // Store uncalibrated signal
   {
    sx->SetSignal(adc,"ADC");
   }

   // LE (TDC) (de)calibration
   le=sx->GetSignal("LE",-7); // Get uncalibrated signal
   fcal=0;
   fdecal=0;
   if (omd)
   {
    fcal=omd->GetCalFunction("LE");
    fdecal=omd->GetDecalFunction("LE");
   }

   // MuDaq only : Store the hit-specific ADC dependent (de)calibration function in the hit itself
   if (mudaq)
   {
    sx->SetCalFunction(fcal,"LE");
    sx->SetDecalFunction(fdecal,"LE");
    fcal=sx->GetCalFunction("LE");
    fdecal=sx->GetDecalFunction("LE");
    if (adc>0)
    {
     if (fcal) fcal->SetParameter(3,adc);
     if (fdecal) fdecal->SetParameter(3,adc);
    }
    else
    {
     if (fcal) fcal->SetParameter(3,1.e20);
     if (fdecal) fdecal->SetParameter(3,1.e20);
    }
   }

   if (fcal) // Store calibrated signal
   {
    cle=fcal->Eval(le);
    sx->SetSignal(cle,"LE");
    if (mudaq) // MuDaq only
    {
     sx->SetCalFunction(0,"LE");
     sx->SetDecalFunction(fdecal,"LE");
    }
   }
   else // Store uncalibrated signal
   {
    sx->SetSignal(le,"LE");
    if (mudaq) // MuDaq only
    {
     sx->SetCalFunction(fcal,"LE");
     sx->SetDecalFunction(0,"LE");
    }
   }

   // TOT (de)calibration
   tot=sx->GetSignal("TOT",-7); // Get uncalibrated signal
   fcal=0;
   fdecal=0;
   if (omd)
   {
    fcal=omd->GetCalFunction("TOT");
    fdecal=omd->GetDecalFunction("TOT");
   }
   if (fcal) // Store calibrated signal
   {
    ctot=fcal->Eval(tot);
    sx->SetSignal(ctot,"TOT");
   }
   else // Store uncalibrated signal
   {
    sx->SetSignal(tot,"TOT");
   }
  } // End of loop over hits of the OM

  // Set bad OM flags according to dbase info
  if (omd)
  {  
   if (omd->GetDeadValue("ADC")) ome->SetDead("ADC");
   if (omd->GetDeadValue("LE")) ome->SetDead("LE");
   if (omd->GetDeadValue("TOT")) ome->SetDead("TOT");
  }

  // Store ADC (de)calibration function in this OM according to dbase info
  fcal=0;
  fdecal=0;
  if (omd)
  {
   fcal=omd->GetCalFunction("ADC");
   fdecal=omd->GetDecalFunction("ADC");
  }
  if (fcal) // Calibrated ADC signals were stored
  {
   ome->SetCalFunction(0,"ADC");
   ome->SetDecalFunction(fdecal,"ADC");
  }
  else // Uncalibrated ADC signals were stored
  {
   ome->SetCalFunction(fcal,"ADC");
   ome->SetDecalFunction(0,"ADC");
  }

  // Store LE (de)calibration function in this OM according to dbase info
  // Note for MuDaq only : This is the ADC independent function.
  //                       The ADC dependent calibration function is in the hits themselves
  fcal=0;
  fdecal=0;
  if (omd)
  {
   fcal=omd->GetCalFunction("LE");
   fdecal=omd->GetDecalFunction("LE");
  }
  if (fcal) // Calibrated LE signals were stored
  {
   ome->SetCalFunction(0,"LE");
   ome->SetDecalFunction(fdecal,"LE");
  }
  else // Uncalibrated LE signals were stored
  {
   ome->SetCalFunction(fcal,"LE");
   ome->SetDecalFunction(0,"LE");
  }

  // Store TOT (de)calibration function in this OM according to dbase info
  fcal=0;
  fdecal=0;
  if (omd)
  {
   fcal=omd->GetCalFunction("TOT");
   fdecal=omd->GetDecalFunction("TOT");
  }
  if (fcal) // Calibrated TOT signals were stored
  {
   ome->SetCalFunction(0,"TOT");
   ome->SetDecalFunction(fdecal,"TOT");
  }
  else // Uncalibrated TOT signals were stored
  {
   ome->SetCalFunction(fcal,"TOT");
   ome->SetDecalFunction(0,"TOT");
  }
 } // End of loop over OM's of the event
}
///////////////////////////////////////////////////////////////////////////
