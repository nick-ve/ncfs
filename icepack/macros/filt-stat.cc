////////////////////////////////////////////////////////
// ROOT macro to investigate IceCube filter statistics.
//
// To run this macro, just do ($ is the command prompt)
//
// $ root -b -q filt-stat.cc
//
//--- Nick van Eijndhoven 08-jul-2010 IIHE, Brussel.
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

 Int_t nevt=0; // Number of events that were filtered
 Int_t nevtcomb=0; // Number of events with selected (combined) filters

 NcSignal conds;
 conds.SetNameTitle("Filtstats","IceCube filter condition statistics");
 NcSignal writes;
 writes.SetNameTitle("Filtstats","IceCube filter write (prescale) statistics");
 NcSignal combis;
 combis.SetNameTitle("Filtstats","IceCube combined condition*write filter statistics");

 TString name;
 IceEvent* first=0;
 IceEvent* last=0;
 Int_t condition=0;
 Int_t prescale=0;
 Int_t combflag=0;

 for (Int_t ient=0; ient<nen; ient++)
 {
  data->GetEntry(ient);
  if (!evt) continue;

  NcDevice* filtdev=(NcDevice*)evt->GetDevice("Filter");
  if (!filtdev) continue;

  if (!first) first=new IceEvent(*evt);
  last=evt;
  nevt++;

  combflag=0;
  for (Int_t ifilt=1; ifilt<=filtdev->GetNhits(); ifilt++)
  {
   NcSignal* sx=filtdev->GetHit(ifilt);
   if (!sx) continue;
   name=sx->GetName();
   condition=int(sx->GetSignal("condition")); // Flag to indicate if the filter condition is satisfied (1) or not (0)
   prescale=int(sx->GetSignal("prescale"));   // Flag to indicate if this filter would write the event (1) or not (0)

   // Update stats for this filter name
   conds.AddNamedSlot(name);
   writes.AddNamedSlot(name);
   combis.AddNamedSlot(name);
   conds.AddSignal(condition,name);
   writes.AddSignal(prescale,name);
   combis.AddSignal(condition*prescale,name);

   //////////////////////////////////////////////////////////////////////////
   // De-activate some filters for the combined condition*write statistics //
   // which will enable to study background reduction for some analyses    //
   //////////////////////////////////////////////////////////////////////////

   if (name.Contains("IceTop") || name.Contains("GC") || name.Contains("Moon") || name.Contains("Sun") || name.Contains("Slop") ||
       name.Contains("MinBias") || name.Contains("FSS") || name.Contains("FixedRate") || name.Contains("VEF"))
   {
    combis.SetDead(name);
   }
   else
   {
    if (condition*prescale) combflag=1;
   }
  }
  if (combflag) nevtcomb++;  
 } // End of event loop

 // The final statistics
 cout << " Total number of filtered events : " << nevt << endl;
 cout << endl;
 if (nevt)
 {
  cout << " ===== First filtered event =====" << endl;
  first->HeaderData();
  cout << endl;

  cout << " ===== Last filtered event =====" << endl;
  last->HeaderData();
  cout << endl;

  Double_t dt=first->GetDifference(last,"s");
  cout << " Elapsed timespan (sec.) : " << dt << endl;
  if (dt<=0) cout << " *** Incorrect timespan. No rates calculated. ***" << endl;

  // The various filter rates
  if (dt>0)
  {
   Double_t ratetot=double(nevt)/dt;
   cout << " Total filtered event rate (Hz) : " << ratetot << endl; 
   Double_t ratecombitot=double(nevtcomb)/dt;
   cout << " Selective filter suppressed (see below) event rate (Hz) : " << ratecombitot << endl;

   // Rate per filter channel
   Double_t ratecond=0;
   Double_t ratewrite=0;
   Double_t ratio=0;
   TString name;
   cout << endl;
   cout << " ===== Total filter rates (Hz) for the various channels =====" << endl;
   for (Int_t i=1; i<=conds.GetNslots(); i++)
   {
    name=conds.GetSlotName(i);
    ratecond=conds.GetSignal(i)/dt;
    ratewrite=writes.GetSignal(i)/dt;
    ratio=1;
    if (ratecond>0) ratio=ratewrite/ratecond;
    cout << " " << name.Data() << " condition : " << ratecond << " write (prescale) : " << ratewrite 
         << " ===> write/cond : " << ratio << endl;
   }
  }

  // Overview of the various Filter counts
  cout << endl;
  cout << " ===== Total filter condition passing statistics =====" << endl;
  conds.Data();
  cout << endl;
  cout << " ===== Total filter event writing (prescale) statistics =====" << endl;
  writes.Data();
  cout << endl;
  cout << " ===== Combined condition and event writing (prescale) filter statistics =====" << endl;
  cout << " ===== with user selected de-activations to study background reduction   =====" << endl;
  combis.Data();
 }
}
