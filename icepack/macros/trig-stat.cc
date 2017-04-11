////////////////////////////////////////////////////////
// ROOT macro to investigate IceCube trigger statistics.
//
// To run this macro, just do ($ is the command prompt)
//
// $ root -b -q trig-stat.cc
//
//--- Nick van Eijndhoven 09-dec-2009 IIHE, Brussel.
////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");
 gSystem->Load("icepack");

 // Access to the input data
 TChain* data=new TChain("T");
// data->Add("/data/IC86/exp/test/PFFilt_TestData_PhysicsFiltering_Run00119982_Subrun00000000_00000000.icepack"); //  8hr test
 data->Add("/data/IC86/exp/test/PFFilt_TestData_PhysicsFiltering_Run00120030_Subrun00000000_00000000.icepack"); // 24hr test

 // Define a pointer for an event
 IceEvent* evt=0;

 // Branch in the tree for the event input
 data->SetBranchAddress("IceEvent",&evt);

 Int_t nen=(int)data->GetEntries();
 cout << endl;
 cout << " Number of entries available : " << nen << endl;
 cout << endl;

 Int_t nevt=0;    // Number of events that were triggered
 Int_t nevtsel=0; // Number of events with selected triggers

 NcSignal trigs;
 trigs.SetNameTitle("Trigstats","IceCube trigger statistics");
 NcSignal seltrigs;
 seltrigs.SetNameTitle("Trigstats","IceCube selected trigger statistics");

 TString name;
 IceEvent* first=0;
 IceEvent* last=0;
 Int_t selflag=0;

 for (Int_t ient=0; ient<nen; ient++)
 {
  data->GetEntry(ient);
  if (!evt) continue;

  NcDevice* trigdev=(NcDevice*)evt->GetDevice("Trigger");
  if (!trigdev) continue;

  if (!first) first=new IceEvent(*evt);
  last=evt;
  nevt++;

  selflag=0;
  for (Int_t itrig=1; itrig<=trigdev->GetNhits(); itrig++)
  {
   NcSignal* sx=trigdev->GetHit(itrig);
   if (!sx) continue;
   name=sx->GetName();

   // Update stats for this trigger name
   trigs.AddNamedSlot(name);
   seltrigs.AddNamedSlot(name);
   trigs.AddSignal(1,name);
   seltrigs.AddSignal(1,name);

   //////////////////////////////////////////////////////////////////////////
   // De-activate some triggers for the user selected statistics           //
   // which will enable to study background reduction for some analyses    //
   //////////////////////////////////////////////////////////////////////////

   if (name.Contains("GLOBAL") || name.Contains("ICE_TOP") || name.Contains("MIN_BIAS") || name.Contains("STRING"))
   {
    seltrigs.SetDead(name);
   }
   else
   {
    selflag=1;
   }
  }
  if (selflag) nevtsel++;  
 } // End of event loop

 // The final statistics
 cout << " Total number of triggered events : " << nevt << endl;
 cout << endl;
 if (nevt)
 {
  cout << " ===== First triggered event =====" << endl;
  first->HeaderData();
  cout << endl;

  cout << " ===== Last triggered event =====" << endl;
  last->HeaderData();
  cout << endl;

  Double_t dt=first->GetDifference(last,"s");
  cout << " Elapsed timespan (sec.) : " << dt << endl;
  if (dt<=0) cout << " *** Incorrect timespan. No rates calculated. ***" << endl;

  // The various trigger rates
  if (dt>0)
  {
   Double_t ratetot=double(nevt)/dt;
   cout << " Total triggered event rate (Hz) : " << ratetot << endl; 
   Double_t rateseltot=double(nevtsel)/dt;
   cout << " Selective trigger suppressed (see below) event rate (Hz) : " << rateseltot << endl;

   // Rate per trigger channel
   Double_t rate=0;
   TString name;
   cout << endl;
   cout << " ===== Total trigger rates (Hz) for the various channels =====" << endl;
   for (Int_t i=1; i<=trigs.GetNslots(); i++)
   {
    name=trigs.GetSlotName(i);
    rate=trigs.GetSignal(i)/dt;
    cout << " " << name.Data() << " rate : " << rate << endl;
   }
  }

  // Overview of the various trigger counts
  cout << endl;
  cout << " ===== Total trigger statistics =====" << endl;
  trigs.Data();
  cout << endl;
  cout << " ===== Trigger statistics with user selected de-activations to study background reduction   =====" << endl;
  seltrigs.Data();
 }
}
