//////////////////////////////////////////////////////////////////////
// ROOT macro to investigate RNO-G data files containing RnoEvent
// structures that have been produced by RnoConvert.
// See the macro convert.cc in this folder for details.
//
// Processing is performed via user defined (sub)tasks, which are
// invoked on an event-by-event basis after the RnoEvent structure
// has been filled and before any data is written out.
//
// All input and output data files are specified in this ROOT macro.
// In case a selection of the input data is written to an output file,
// it is suggested to use the name "data.rnopack" for the output file.
//
// In this macro the task RnoMonitor is invoked in order to
// search for recurrent astrophysical signals due to Earth's rotation.
// The produced monitoring histograms are written to an output file
// which here is specified as monitor.root.
//
// Keeping these generic names allows the user to use this ROOT macro "as is"
// and provide/rename the corresponding files via the analyze-stack.sub
// HTCondor batch processing script.
//
// To run this macro interactively, just do ($ is prompt)
//
// $root analyze.cc
//
// or for batch processing
//
// $root -b -q analyze.cc >output.log
//
//--- Nick van Eijndhoven, IIHE-VUB, Brussels, November 17, 2022  12:36Z
//////////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");
 gSystem->Load("rnopack");

 // Top level job structure
 NcJob job("NcJob","Investigation of RNO-G data in rnopack format");

 // Generic access pointer to the input data
 TChain* data=new TChain("T");

 // Specify the print frequency to provide a short event summary
 // every "printfreq" events.
 // No printout is provided when printfreq=0.
 Int_t printfreq=10;

 ///////////////////////////////////////////////
 // The RNO-G RnoEvent data input filename(s) //
 ///////////////////////////////////////////////

 // Definition of the input file(s)
 TString dir="/pnfs/iihe/rno-g/rnopack-data/2021/station22/run";
 TString file;
 for (Int_t irun=306; irun<=309; irun++)
 {
  file=dir;
  file+=irun;
  file+="/combined.rnopack";
  data->Add(file);
 }

 ///////////////////////////////////////////////////////////////////
 // Here the user can specify his/her (sub)tasks to be executed
 // on an event-by-event basis after the RnoEvent structure
 // has been filled and before any data is written out.
 // (Sub)tasks (i.e. a user classes derived from TTask) are entered
 // as follows :
 //
 //    MyClean*   task1=new MyClean("task1","Data cleaning");
 //    MyReco*    task2=new MyReco("task2","Track reconstruction");
 //    MyControl* task2b=new MyControl("task2b","Track quality control");
 //    MyTagger*  task3=new MyTagger("task3","Event tagging");
 //    MySelect*  task4=new MySelect("task4","Event selection");
 //
 //    task2->Add(task2b); // Make task2b a nested sub-task (of task2)
 //
 //    job.Add(task1);
 //    job.Add(task2);
 //    job.Add(task3);
 //    job.Add(task4);
 //
 // The (sub)tasks will be executed in the order as they are entered.
 //
 // Note :
 // ------
 // It is advised to dynamically create the various (sub)tasks via the "new"
 // constructor in order to prevent destructor issues for nested sub-tasks
 // after completion of the ROOT macro.
 ///////////////////////////////////////////////////////////////////

 // The monitoring task
 RnoMonitor* moni=new RnoMonitor();
 moni->SetDeviceClass("RnoULPDA");
 moni->SetSampleVariable("ADC","0.618*x-8.133");
 moni->DefineStatistic("RMSdeviation");
 moni->SetNbins24(24*4);

 // Add the task(s) to the top level job structure
 job.Add(moni);

 // List all (sub)tasks of the job structure
 job.ListEnvironment();

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
