//////////////////////////////////////////////////////////////////////
// ROOT macro to investigate RNO-G data files containing RnoEvent
// structures that have been produced by RnoConvert.
// See the macro convert.cc in this folder for details.
//
// All input and output data files are specified in this ROOT macro.
// In this template the input data file is named "data.rnopack".
//
// In this macro the task RnoMonitor is invoked in order to
// search for recurrent astrophysical signals due to Earth's rotation.
// The produced monitoring histograms are written to an output file
// which here is specified as monitor.root.
//
// Keeping these generic names allows the user to use this ROOT macro "as is"
// and provide/rename the corresponding files via the convert.sub HTCondor
// batch processing script.
//
// To run this macro interactively, just do ($ is prompt)
//
// $root analyze.cc
//
// or for batch processing
//
// $root -b -q analyze.cc >output.log
//
//--- Nick van Eijndhoven, IIHE-VUB, Brussels, November 17, 2022  12:34Z
//////////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");
 gSystem->Load("rnopack");

 // Specify the print frequency to provide a short event summary
 // every "printfreq" events.
 // No printout is provided when printfreq=0.
 Int_t printfreq=10;

 // Top level job structure
 NcJob job("NcJob","Investigation of RNO-G data in rnopack format");

 // The monitoring task
 RnoMonitor* moni=new RnoMonitor();
 moni->SetDeviceClass("RnoULPDA"); // Only use the upward LPDAs
 moni->SetSampleVariable("ADC","0.618*x-8.133"); // Convert ADC->mV
 moni->DefineStatistic("RMSdeviation");
 moni->SetNbins24(24*4);

 // Add the task(s) to the top level job structure
 job.Add(moni);

 // List all (sub)tasks of the job structure
 job.ListEnvironment();

 // Access to the input data
 TChain* data=new TChain("T");
 data->Add("data.rnopack");

 // Define a pointer for an event
 RnoEvent* evt=0;

 // Branch in the tree for the event input
 data->SetBranchAddress("Events",&evt);

 Double_t nen=data->GetEntries();

 cout << endl;
 cout << " *READ* nentries : " << nen;
 cout << endl;

 // Loop over the events and execute the task(s)
 for (Int_t ien=0; ien<nen; ien++)
 {
  data->GetEntry(ien);

  if (!evt) continue;

  // Provide a printout every "printfreq" events
  if (printfreq)
  {
   if (!((ien+1)%printfreq))
   {
    cout << endl;
    evt->HeaderData();
   }
  }

  // Execute the task(s) of this job for the current event
  job.ProcessObject(evt);
 }

 // Write the monitoring histograms to a ROOT output file
 moni->WriteHistograms("monitor.root");
}
