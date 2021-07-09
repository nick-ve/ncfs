// ROOT macro to test the RNO-G data conversion into RnoEvent structures
// Nick van Eijndhoven, IIHE-VUB, Brussels, July 9, 2021  12:11Z
{
 gSystem->Load("ncfspack");
 gSystem->Load("rnopack");

 RnoConvert q("RnoConvert","RNO-G data to RnoEvent data structure conversion");

 // Limit the number of entries for testing
 q.SetMaxEvents(-1);

 // Print frequency to produce a short summary print every printfreq events
 q.SetPrintFreq(5);

 // The RNO-G Root data input filename(s)
 q.AddInputFile("pulse-waveforms.root","wf");

 // Output file for the event structures
 q.SetOutputFile("myevents.rnopack");

 ///////////////////////////////////////////////////////////////////
 // Here the user can specify his/her sub-tasks to be executed
 // on an event-by-event basis after the IceEvent structure
 // has been filled and before the data is written out.
 // Sub-tasks (i.e. a user classes derived from TTask) are entered
 // as follows :
 //
 //    MyClean  task1("task1","Data cleaning");
 //    MyReco   task2("task2","Track reconstruction");
 //    MyFilter task3("task3","Event filtering");
 //    q.Add(&task1);
 //    q.Add(&task2);
 //    q.Add(&task3);
 //
 // The sub-tasks will be executed in the order as they are entered.
 ///////////////////////////////////////////////////////////////////

 // Perform the conversion and execute subtasks (if any)
 // on an event-by-event basis
 q.ExecuteJob();
}
