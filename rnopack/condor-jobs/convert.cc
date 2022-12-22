/////////////////////////////////////////////////////////////////////////
// ROOT macro to convert RNO-G plain ROOT data into RnoEvent structures.
//
// The input and output data files are specified in this ROOT macro.
// In this template the RNO-G input data file is named "data.root" and
// the converted output file is named "data.rnopack".
//
// Keeping these generic names allows the user to use this ROOT macro "as is"
// and provide/rename the corresponding files via the convert.sub HTCondor
// batch processing script.
//
// Note :
// ------
// By commenting out the SetOutputFile("data.rnopack") statement below,
// the production of the rnopack output file is suppressed. 
//
// In this macro also the task RnoMonitor is invoked in order to
// search for recurrent astrophysical signals due to Earth's rotation.
// The produced monitoring histograms are written to an output file
// which here is specified as monitor.root.
// The task NcTaggingSelector is used to select only events that were
// triggered by the RNO-G force_trigger.
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
// Nick van Eijndhoven, IIHE-VUB, Brussels, December 22, 2022  02:41Z
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

 // The RNO-G Root data input and output filename(s)
 q.AddInputFile("data.root","combined");

 // Comment the line below if you want to suppress producing an output file
 q.SetOutputFile("data.rnopack");

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

 // Specify a trigger selection task
 NcTaggingSelector* trigsel=new NcTaggingSelector("TriggerSel","RNO-G trigger selection");
 trigsel->SetDevice("Trigger");
 trigsel->ActivateTag("force");
 trigsel->DeactivateTag("pps");
 trigsel->DeactivateTag("ext");
 trigsel->DeactivateTag("lt");
 trigsel->DeactivateTag("radiant");
 trigsel->DeactivateTag("LPDA");

 // Add a monitoring task
 RnoMonitor* moni=new RnoMonitor();
 moni->SetDevices("RnoULPDA"); // Only use the upward LPDAs
 moni->SetSampleVariable("ADC","0.618*x-8.133"); // Convert ADC->mV
 moni->DefineStatistic("RMSdeviation");
 moni->SetBaselineMode(0); // No baseline correction
 moni->SetNbins24(24*4);

 q.Add(trigsel);
 q.Add(moni);

 // Perform the conversion and execute subtasks (if any)
 // on an event-by-event basis
 // Invokation with the default argument is essential
 // to obtain event-by-event processing of the sub-tasks.
 q.ExecuteJob();

 // Write the monitoring histograms to a ROOT output file
 moni->WriteHistograms("monitor.root");
}
