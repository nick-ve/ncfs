// ROOT macro to test the RNO-G data conversion into RnoEvent structures
// Nick van Eijndhoven, IIHE-VUB, Brussels, January 7, 2022  12:02Z
{
 gSystem->Load("ncfspack");
 gSystem->Load("rnopack");

 RnoConvert q("RnoConvert","RNO-G data to RnoEvent data structure conversion");

 // Limit the number of entries for testing
 q.SetMaxEvents(-1);

 // Print frequency to produce a short summary print every printfreq events
 q.SetPrintFreq(5);

 // Set the required event selection level interval for events to be written out
 q.SetSelectLevels(0,-1);

 // The RNO-G Root data input filename(s)
 q.AddInputFile("./data/station11/run101/combined.root","combined");

 // Output file for the event structures
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

 // Perform the conversion and execute subtasks (if any)
 // on an event-by-event basis
 // Invokation with the default argument is essential
 // to obtain event-by-event processing of the sub-tasks.
 q.ExecuteJob();
}
