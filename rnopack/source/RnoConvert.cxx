/*******************************************************************************
 * Copyright(c) 2021, RNO-G Experiment at Summit Station. All rights reserved.
 *
 * Author: The RNO-G NCFS-based Offline Project.
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
// Class RnoConvert
// Conversion of RNO-G Root data into RnoEvent data structures.
// This class is derived from NcJob providing a task-based processing
// structure on an event-by-event basis.
// The main object in the job environment is an RnoEvent* pointer.
// In case the user has provided sub-tasks, these will be executed
// on an event-by-event basis after the RnoEvent structure has been filled
// with the RNO-G data and before the final structures are written out.
// In case the sub-tasks contained event selection procedures, like for
// instance NcEventSelector, only the events that have a proper
// event selection level, see SetSelectLevels(), will be written out.
// Note that the data structures are only written out if an outputfile has
// been specified via the SetOutputFile memberfunction.
// In case no outputfile has been specified, this class provides a facility
// to investigate/analyse RNO-G data using the NCFS/RnoPack analysis tools.
//
// Please refer to /macros/convert.cc for a usage example.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, July 9, 2021  10:09Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 28, 2022  11:08Z
///////////////////////////////////////////////////////////////////////////
 
#include "RnoConvert.h"
#include "Riostream.h"

ClassImp(RnoConvert) // Class implementation to enable ROOT I/O

RnoConvert::RnoConvert(const char* name,const char* title) : NcJob(name,title)
{
// Default constructor.
// By default maxevent=-1, split=0, bsize=32000, printfreq=1 and select=[0,-1].

 fSplit=0;
 fBsize=32000;
 fMaxevt=-1;
 fPrintfreq=1;
 fOutfile=0;
 fData=0;
 fMinSelectLevel=0;
 fMaxSelectLevel=-1;
}
///////////////////////////////////////////////////////////////////////////
RnoConvert::~RnoConvert()
{
// Default destructor.

 if (fData)
 {
  delete fData;
  fData=0;
 }

 if (fOutfile)
 {
  delete fOutfile;
  fOutfile=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::SetMaxEvents(Int_t n)
{
// Set the maximum number of events to be processed.
// n=-1 implies processing of the complete input file, which is the default
// initialisation in the constructor.

 fMaxevt=n;
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::SetPrintFreq(Int_t f)
{
// Set the printfrequency to produce info every f events.
// f=1 is the default initialisation in the constructor.

 if (f>=0) fPrintfreq=f;
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::SetSplitLevel(Int_t split)
{
// Set the split level for the ROOT data file.
// split=0 is the default initialisation in the constructor.

 if (split>=0) fSplit=split;
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::SetBufferSize(Int_t bsize)
{
// Set the buffer size for the ROOT data file.
// bsize=32000 is the default initialisation in the constructor.

 if (bsize>=0) fBsize=bsize;
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::AddInputFile(TString file,TString tree)
{
// Add the RNO-G input file to the data chain.
//
// file : Name of the input file to be added (wildcards are allowed)
// tree : Name of the Tree containing the data
//
// Environment variables may be used as $(...) in the filenname for convenience.
// For example "$(HOME)/my-data/station11/combined.root".
//
// Note : The name of the Tree has to be the same for all added input files.

 // Expand the path name of the provided input file
 file=gSystem->ExpandPathName(file.Data());

 if (!fData) fData=new TChain(tree.Data());

 if (fData)
 {
  fData->Add(file.Data());
  cout << " *" << ClassName() << "::AddInputFile* Added RNO-G data input file : " << file << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::SetOutputFile(TFile* ofile)
{
// Set the output file for the RnoEvent data.

 if (fOutfile) delete fOutfile;
 fOutfile=ofile;
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::SetOutputFile(TString name)
{
// Create the output file for the RnoEvent data.
//
// Environment variables may be used as $(...) in the filenname for convenience.
// For example "$(HOME)/my-data/sample.rnopack".

 // Expand the path name of the specified output file
 name=gSystem->ExpandPathName(name.Data());

 if (fOutfile) delete fOutfile;
 fOutfile=new TFile(name.Data(),"RECREATE","RNO-G data in RnoEvent structure");
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::SetSelectLevels(Int_t min,Int_t max)
{
// Set the required event selection level interval [min,max] for events to be written out.
// The generic (NcEvent) convention is <0:reject 0:undecided >0:accept.
//
// Note : If max<min there will be no check on the maximum value.
//
// min=0 and max=-1 are the default initialisations in the constructor.

 fMinSelectLevel=min;
 fMaxSelectLevel=max;
}
///////////////////////////////////////////////////////////////////////////
Int_t RnoConvert::GetMinSelectLevel() const
{
// Provide the minimum required event selection level for events to be written out.

 return fMinSelectLevel;
}
///////////////////////////////////////////////////////////////////////////
Int_t RnoConvert::GetMaxSelectLevel() const
{
// Provide the maximum required event selection level for events to be written out.

 return fMaxSelectLevel;
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::ListInput(Option_t* opt)
{
// Provide an overview listing of the input data chain.
// The input argument "opt" has the same meaning as for the ROOT TTree::Print().
// The default is opt="".

 TString s=opt;
 if (s=="") s="Default";
 if (fData)
 {
  cout << endl;
  cout << " *" << ClassName() << "::ListInput* Overview of the input data with option : " << s << endl;
  fData->Print(opt);
 }
 else
 {
  cout << endl;
  cout << " *" << ClassName() << "::ListInput* No input file has been attached." << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
TFile* RnoConvert::GetOutputFile()
{
// Provide pointer to the RnoEvent output file.

 return fOutfile;
}
///////////////////////////////////////////////////////////////////////////
void RnoConvert::Exec(Option_t* opt)
{
// Job to loop over the specified number of events and convert the 
// RNO-G Root data into the RnoEvent structure.
// If maxevents<0 (default) all the entries of the input file
// will be processed.
// Every "printfreq" events a short event summary will be printed.
// The default value is printfreq=1.
// The output will be written on a standard output tree named "T".
//
// Notes :
// -------
// 1) This class is derived from NcJob, allowing a task based processing.
//    After the conversion of an RNO-G Root data event into an RnoEvent structure,
//    the processing of all available sub-tasks (if any) is invoked.
//    This provides an event-by-event (sub)task processing before the
//    final data structures are written out.
// 2) The main object in this job environment is an RnoEvent* pointer.

 if (!fData)
 {
  cout << " *" << ClassName() << "::Exec* No data input file(s) specified." << endl;
  return;
 }

 // Create output tree if necessary
 TTree* otree=0;
 if (fOutfile)
 {
  otree=new TTree("T","RNO-G data converted to RnoEvent structures");
  otree->SetDirectory(fOutfile);
 }

 // An initial RNO-G detector structure
 RnoDetector det;

 // Create the RnoEvent structure
 RnoEvent* evt=new RnoEvent();
 evt->SetOwner(1);

 // Branch in the tree for the event structure
 if (otree) otree->Branch("Events","RnoEvent",&evt,fBsize,fSplit); 

 // Initialise the job working environment
 SetMainObject(evt);

 // Some output for the user's convenience
 cout << endl;
 cout << " *" << ClassName() << "::Exec* Overview of scheduled processing." << endl;
 cout << " ***" << endl;
 cout << " *** Start processing of job " << GetName() << " ***" << endl;
 cout << " ***" << endl;
 cout << " Maximum number of events to be processed : " << fMaxevt << endl;
 cout << " Print frequency : " << fPrintfreq << endl;
 if (fOutfile)
 {
  cout << " RnoEvent output file : " << fOutfile->GetName() << endl;
  cout << " Output characteristics : splitlevel = " << fSplit << " buffersize = " << fBsize << endl;
  cout << " Required event selection level interval for output : [" << fMinSelectLevel << "," << fMaxSelectLevel << "]" << endl;
 }
 cout << endl;

 ListEnvironment();

 // The number of entries in the input chain
 Int_t nen=fData->GetEntries();

 // Check for the maximum number of events to be processed
 if (fMaxevt>-1 && nen>fMaxevt) nen=fMaxevt;

 // The leaves in the input chain
 TObjArray* leaves=fData->GetListOfLeaves();
 Int_t nleaves=leaves->GetEntries();

 //////////////////////////////////////
 // Variables in the RNO-G data tree //
 //////////////////////////////////////

 // Header info
 Int_t run=0;
 Int_t event=0;
 Int_t station=0;
 Double_t trigtime=0;

 // DAQ info
 NcDevice daq("DAQ","DAQ status");
 daq.AddNamedSlot("RADIANT-update");
 daq.AddNamedSlot("FLOWER-update");
 daq.AddNamedSlot("Sampling-rate");

 // Trigger info
 NcTagger trigger("Trigger","Trigger tags");

 // The waveforms of all channels are provided as Short_t radiant_data[24][2048]
 // but will be readout linearly and stored as an NcSample for each channel, as indicated below. 
 NcSample signal("Signals","Radiant signals");
 signal.SetStoreMode(1);

 // The pedestals of all channels are provided as UShort_t pedestals[24][4096]
 // but will be readout linearly and stored as an NcSample for each channel, as indicated below. 
 NcSample pedestal("Pedestals","Pedestal values");
 pedestal.SetStoreMode(1);

 ///////////////////////////////////////////////////
 // Loop over the entries in the input data chain //
 ///////////////////////////////////////////////////

 TLeaf* lx=0;
 TLeaf* lradiant=0;  // Pointer to the Radiant waveform data
 TLeaf* lpedestal=0; // Pointer to the Pedestal waveform data
 Int_t idx=0;
 Int_t nsamples=0; // The sampling buffer length
 NcDevice* devx=0;
 Int_t idsample=0; // Index for the NcSample in the NcDevice storage
 TString name="";
 TString namex1="";
 TString namex2="";
 TString namex3="";
 Float_t value=0;
 Int_t ivalue=0;
 Bool_t flag=kFALSE;
 Int_t iradiant=0; // RADIANT firmware update sequence number
 Int_t iflower=0;  // FLOWER firmware update sequence number
 Float_t fsample;  // The DAQ sampling rate in Hz
 Int_t nwritten=0; // The number of events written to output

 for (Int_t ient=0; ient<nen; ient++)
 {
  // Reset the detector structure
  det.Reset();
  idsample=0;

  fData->GetEntry(ient);

  // Loop over all the leaves and extract the relevant data for this entry.
  // This approach makes the functionality independent of the Tree/Branch structure.
  run=0;
  event=0;
  station=0;
  trigtime=0;
  nsamples=0;
  trigger.Reset();
  lradiant=0;
  lpedestal=0;
  for (Int_t ileaf=0; ileaf<nleaves; ileaf++)
  {
   lx=(TLeaf*)leaves->At(ileaf);

   if (!lx) continue;

   name=lx->GetName();

   // Header data
   if (!run && name=="run_number") run=lx->GetValue();
   if (!event && name=="event_number") event=lx->GetValue();
   if (!station && name=="station_number") station=lx->GetValue();
   if (!trigtime && name=="trigger_time") trigtime=lx->GetValue();
   if (!trigtime && name=="when") trigtime=lx->GetValue(); // In case of a pedestal data file
   if (!nsamples && name=="buffer_length") nsamples=lx->GetValue();

   // Trigger data
   if (name.Contains("trigger_info"))
   {
    name.ReplaceAll("trigger_info.","");
    value=lx->GetValue();
    ivalue=TMath::Nint(value);
    namex1="";
    namex2="";
    namex3="";

    // The low threshold time window and number of coincidences
    if (name.Contains("lt_info.window")) namex1="lt-window";
    if (name.Contains("lt_info.num_coinc")) namex1="lt-ncoinc";

    if (namex1!="")
    {
     trigger.AddNamedSlot(namex1);
     trigger.SetSignal(value,namex1);
     continue;
    }

    // The various radiant (=surface) time windows and number of coincidences
    if (name.Contains("radiant_info.RF_window"))
    {
     namex1="LPDA-up-window";
     namex2="LPDA-down-window";
     value=lx->GetValue(0);
     trigger.AddNamedSlot(namex1);
     trigger.SetSignal(value,namex1);
     value=lx->GetValue(1);
     trigger.AddNamedSlot(namex2);
     trigger.SetSignal(value,namex2);
     continue;
    }
    if (name.Contains("radiant_info.RF_ncoinc"))
    {
     namex1="LPDA-up-ncoinc";
     namex2="LPDA-down-ncoinc";
     value=lx->GetValue(0);
     trigger.AddNamedSlot(namex1);
     trigger.SetSignal(value,namex1);
     value=lx->GetValue(1);
     trigger.AddNamedSlot(namex2);
     trigger.SetSignal(value,namex2);
     continue;
    }

    // The actual trigger tags
    if (name.Contains("_trigger"))
    {
     // Settings of the various radiant (=surface) triggers
     if (name.Contains("which_radiant"))
     {
      namex1="LPDA-up_trigger";
      namex2="LPDA-down_trigger";
      namex3="radiant-unknown_trigger";
      if (ivalue<-100) // None of the surface triggers
      {
       trigger.SetPass(namex1,kFALSE);
       trigger.SetPass(namex2,kFALSE);
       trigger.SetPass(namex3,kFALSE);
      }
      if (ivalue==-1) // Unknown radiant trigger
      {
       trigger.SetPass(namex1,kFALSE);
       trigger.SetPass(namex2,kFALSE);
       trigger.SetPass(namex3,kTRUE);
      }
      if (ivalue==0) // Only upward surface trigger
      {
       trigger.SetPass(namex1,kTRUE);
       trigger.SetPass(namex2,kFALSE);
       trigger.SetPass(namex3,kFALSE);
      }
      if (ivalue==1) // Only downward surface trigger
      {
       trigger.SetPass(namex1,kFALSE);
       trigger.SetPass(namex2,kTRUE);
       trigger.SetPass(namex3,kFALSE);
      }
     }
     else
     {
      flag=kFALSE;
      if (ivalue) flag=kTRUE;
      trigger.SetPass(name,flag);
     }
    }
   } // End of trigger data

   // Pointers to the waveform data
   if (name=="radiant_data") lradiant=lx;
   if (name=="pedestals") lpedestal=lx;   

  } // End of loop over the leaves

  // Create this station in the detector structure
  RnoStation* stax=det.GetStation(station,kTRUE);

  if (!stax) break;

  // DAQ info
  iradiant=2;    // The RADIANT update version
  iflower=0;     // The FLOWER update version
  fsample=3.2e9; // The sampling rate in Hz
  if (station==11)
  {
   if (run<571) iradiant=1;
   if (run<474) iradiant=0;
   if (run>=571) iradiant=2;
  }
  if (station==21)
  {
   if (run<753) iradiant=1;
   if (run<646) iradiant=0;
  }
  if (station==22)
  {
   if (run<656) iradiant=1;
   if (run<574) iradiant=0;
  }

  daq.SetSignal(iradiant,"RADIANT-update");
  daq.SetSignal(iflower,"FLOWER-update");
  daq.SetSignal(fsample,"Sampling-rate");

  stax->AddDevice(daq);

  stax->AddDevice(trigger);

  // Readout the signal waveforms of all Radiant channels
  if (!nsamples) nsamples=2048;
  lx=lradiant;
  if (lx)
  {
   idsample++;
   idx=0;
   for (Int_t i=0; i<24; i++)
   {
    // Access the corresponding channel of this station
    name="Ch";
    name+=i;
    devx=stax->GetDevice(name,kTRUE);

    if (!devx) continue;

    signal.Reset();
    signal.SetNames("ADC");
    for (Int_t j=0; j<nsamples; j++)
    {
     // Retrieve the value of radiant[i][j]
     value=lx->GetValue(idx);
     signal.Enter(value);
     idx++;
    }
    devx->SetSample(&signal,idsample);
   }
  }

  // Readout the pedestals of all Radiant channels
  nsamples=4096;
  lx=lpedestal;
  if (lx)
  {
   idsample++;
   idx=0;
   for (Int_t i=0; i<24; i++)
   {
    // Access the corresponding channel of this station
    name="Ch";
    name+=i;
    devx=stax->GetDevice(name,kTRUE);

    if (!devx) continue;

    pedestal.Reset();
    pedestal.SetNames("ADC");
    for (Int_t j=0; j<nsamples; j++)
    {
     // Retrieve the value of pedestals[i][j]
     value=lx->GetValue(idx);
     pedestal.Enter(value);
     idx++;
    }
    devx->SetSample(&pedestal,idsample);
   }
  }

  // Transfer the RNO-G data into the RnoEvent structure
  evt->Reset();
  evt->SetUnixTime(trigtime,"A");
  evt->SetRunNumber(run);
  evt->SetEventNumber(event);
  evt->SetDetector(det);

  // Invoke all available sub-tasks (if any)
  CleanTasks();
  ExecuteTasks(GetName());

  // Provide a printout every "Printfreq" events
  if (fPrintfreq)
  {
   if (!((ient+1)%fPrintfreq))
   {
    cout << endl;
    evt->HeaderData();
   }
  }

  // Write the event to the output file (if the event select level is o.k.)
  Int_t select=evt->GetSelectLevel();
  if (select<fMinSelectLevel) continue;
  if (fMaxSelectLevel>=fMinSelectLevel && select>fMaxSelectLevel) continue;

  if (otree) otree->Fill();
  nwritten++;
 } // End of loop over the entries

 // Flush possible memory resident data to the output file
 if (fOutfile)
 {
  fOutfile->Write();
  cout << endl;
  cout << " *" << ClassName() << "::Exec* Number of (selected) events written to output : " << nwritten << endl;
 }

 // Remove the RnoEvent object from the environment
 // and delete it as well
 if (evt)
 {
  RemoveObject(evt);
  delete evt;
 }

 // Delete the input data chain
 if (fData)
 {
  delete fData;
  fData=0;
 }
}
///////////////////////////////////////////////////////////////////////////
