/////////////////////////////////////////////////////////////////////////
// ROOT macro to convert RNO-G plain ROOT data into RnoEvent structures.
//
// In this macro also the task RnoMonitor is invoked in order to
// search for recurrent astrophysical signals due to Earth's rotation.
// The produced monitoring histograms are written to an output file
// which here is specified as monitor.root.
// In addition to this, the task NcTaggingStats is invoked to provide
// the trigger statistics. 
//
// To run this macro interactively, just do ($ is prompt)
//
// $root convert.cc
//
// or for batch processing
//
// $root -b -q convert.cc >output.log
//
// Note :
// ------
// In case the RNO-G mattak software package is not loaded, the ROOT
// processor may issue several warnings about missing dictionaries
// for the mattak classes.
// Since the mattak package is not needed here, these warnings may be
// safely ignored.
//
// Nick van Eijndhoven, IIHE-VUB, Brussels, November 17, 2022  12:43Z
/////////////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");
 gSystem->Load("rnopack");

 RnoConvert q("RnoConvert","RNO-G data to RnoEvent data structure conversion");

 // Optionally limit the number of entries for testing
 q.SetMaxEvents(-1);

 // Print frequency to produce a short summary print every printfreq events
 q.SetPrintFreq(10);

 // Set the required event selection level interval for events to be written out
 q.SetSelectLevels(0,-1);

 // The RNO-G Root data input filename(s)
 q.AddInputFile("./data/station21/random-triggers/run442/combined.root","combined");

 // Output file for the event structures
 // Comment the line below if you want to suppress producing an output file
 q.SetOutputFile("myevents.rnopack");

 // Provide an overview listing of the input data chain
 q.ListInput();

 ///////////////////////////////////////////////////////////////////
 // Here the user can specify his/her sub-tasks to be executed
 // on an event-by-event basis after the RnoEvent structure
 // has been filled and before the data is written out.
 // Sub-tasks (i.e. a user classes derived from TTask) are entered
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
 //    q.Add(task1);
 //    q.Add(task2);
 //    q.Add(task3);
 //    q.Add(task4);
 //
 // The sub-tasks will be executed in the order as they are entered.
 //
 // Note :
 // ------
 // It is advised to dynamically create the various sub-tasks via the "new"
 // constructor in order to prevent destructor issues for nested sub-tasks
 // after completion of the ROOT macro.
 ///////////////////////////////////////////////////////////////////

 // Add a monitoring task
 RnoMonitor* moni=new RnoMonitor();
 moni->SetDeviceClass("RnoULPDA"); // Only use the upward LPDAs
 moni->SetSampleVariable("ADC","0.618*x-8.133"); // Convert ADC->mV
 moni->DefineStatistic("RMSdeviation");
 moni->SetNbins24(24*4);

 // Specify a trigger statistics task
 NcTaggingStats* trigstat=new NcTaggingStats("RnoTrigger","RNO-G trigger statistics");
 trigstat->SetDevice("Trigger");
 trigstat->ActivateTag("radiant"); // The Radiant (=surface) triggers
 trigstat->ActivateTag("lt");      // The Low Threshold trigger 

 q.Add(moni);
 q.Add(trigstat);

 // Perform the conversion and execute subtasks (if any)
 // on an event-by-event basis
 // Invokation with the default argument is essential
 // to obtain event-by-event processing of the sub-tasks.
 q.ExecuteJob();

 // Write the monitoring histograms to a ROOT output file
 moni->WriteHistograms("monitor.root");
 
 // Provide the trigger statistics
 trigstat->ShowStatistics();
}
