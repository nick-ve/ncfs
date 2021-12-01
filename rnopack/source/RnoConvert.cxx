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
// Note that the data structures are only written out if an outputfile has
// been specified via the SetOutputFile memberfunction.
// In case no outputfile has been specified, this class provides a facility
// to investigate/analyse RNO-G data using the NCFS/RnoPack analysis tools.
//
// Please refer to /macros/convert.cc for a usage example.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, July 9, 2021  10:09Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, December 1, 2021  14:45Z
///////////////////////////////////////////////////////////////////////////
 
#include "RnoConvert.h"
#include "Riostream.h"

ClassImp(RnoConvert) // Class implementation to enable ROOT I/O

RnoConvert::RnoConvert(const char* name,const char* title) : NcJob(name,title)
{
// Default constructor.
// By default maxevent=-1, split=0, bsize=32000, printfreq=1.

 fSplit=0;
 fBsize=32000;
 fMaxevt=-1;
 fPrintfreq=1;
 fOutfile=0;
 fData=0;

 NcVersion version;
 version.Data();
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
// Note : The name of the Tree has to be the same for all added input files.

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

 if (fOutfile) delete fOutfile;
 fOutfile=new TFile(name.Data(),"RECREATE","RNO-G data in RnoEvent structure");
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
 }

 ListEnvironment();

 // The number of entries in the input chain
 Int_t nen=fData->GetEntries();

 // Check for the maximum number of events to be processed
 if (fMaxevt>-1 && nen>fMaxevt) nen=fMaxevt;

 //////////////////////////////////////
 // Variables in the RNO-G data tree //
 //////////////////////////////////////

 // Header info
 Int_t run=0;
 Int_t event=-1;
 Int_t station=-1;
 Double_t trigtime=0;

 // Trigger info
 NcTagger trigger("Trigger","Trigger tags");
 const Int_t ntrig=7;
 TString trignames[ntrig]={"rf","force","pps","ext","radiant","lt","surface"};

 // The waveforms of all channels are stored as Short_t radiant_data[24][2048]
 // but will be readout linearly, as indicated below. 

 // The pedestals of all channels are stored as UShort_t pedestals[24][4096]
 // but will be readout linearly, as indicated below. 

 // Loop over the entries in the input data chain
 TLeaf* lx=0;
 Int_t idx=0;
 Int_t nsamples=0; // The sampling buffer length
 NcDevice* devx=0;
 NcSample signal("Signals","Radiant signals");
 signal.SetStoreMode(1);
 NcSample pedestal("Pedestals","Pedestal values");
 pedestal.SetStoreMode(1);
 Int_t idsample=0; // Index for the NcSample in the NcDevice storage
 TString name="";
 Float_t value=0;
 Bool_t flag=kFALSE;
 for (Int_t ient=0; ient<nen; ient++)
 {
  // Reset the detector structure
  det.Reset();
  idsample=0;

  fData->GetEntry(ient);

  // Header data
  if (fData->GetBranch("header"))
  {
   lx=fData->GetLeaf("header.run_number","run_number");
   if (lx) run=lx->GetValue();
   lx=fData->GetLeaf("header.event_number","event_number");
   if (lx) event=lx->GetValue();
   lx=fData->GetLeaf("header.station_number","station_number");
   if (lx) station=lx->GetValue();
   lx=fData->GetLeaf("header.trigger_time","trigger_time");
   if (lx) trigtime=lx->GetValue();
  }
  else // The corresponding data from a pedestal file
  {
   lx=fData->GetLeaf("station_number");
   if (lx) station=lx->GetValue();
   lx=fData->GetLeaf("when");
   if (lx) trigtime=lx->GetValue();
  }

  // Create this station in the detector structure
  RnoStation* stax=det.GetStation(station,kTRUE);

  if (!stax) break;

  // Trigger data
  trigger.Reset();

  for (Int_t jtrig=0; jtrig<ntrig; jtrig++)
  {
   name="trigger_info.";
   name+=trignames[jtrig];
   name+="_trigger";
   lx=fData->GetLeaf(name);
   if (lx)
   {
    flag=kFALSE;
    if (TMath::Nint(lx->GetValue())) flag=kTRUE;
    trigger.SetPass(trignames[jtrig],flag);
    trigger.SetWrite(trignames[jtrig],flag);
   }
  }

  det.AddDevice(trigger);

  // Readout the signal waveforms of all Radiant channels
  nsamples=0;
  lx=fData->GetLeaf("waveforms.buffer_length","buffer_length");
  if (lx) nsamples=TMath::Nint(lx->GetValue());
  if (!nsamples) nsamples=2048;
  lx=fData->GetLeaf("radiant_data");
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
  lx=fData->GetLeaf("pedestals");
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

  // Invoke all available sub-tasks (if any) and write event to tree
  CleanTasks();
  ExecuteTasks(opt);

  // Write the event to the output file (if needed)
  if (otree) otree->Fill();

  if (fPrintfreq)
  {
   if (!((ient+1)%fPrintfreq))
   {
    cout << endl;
    evt->HeaderData();
   }
  }
 } // End of loop over the entries

 // Flush possible memory resident data to the output file
 if (fOutfile) fOutfile->Write();

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
