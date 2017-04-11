//////////////////////////////////////////////////////////////
// Macro to investigate hit statistics from IcePack data.
//
// To run this macro, just do ($ is the command prompt)
//
// $ root -b -q hit-stat.cc
//
// This will produce all IceCube hit data in a plain ROOT
// tree which may be analysed interactively via the standard
// ROOT TBrowser and TreeViewer facilities.
//
// The actual data tree structures are defined in the file
// "hit-stat.cxx".
// Due to the fact that the data tree had to be created with
// the use of std::vector, the TreeViewer does not show the
// data tree down to the deepest tree level.
// To explore the data tree to the full depth, please use the
// standard ROOT TBrowser facility.
// However, various selections, plots etc. down to the deepest
// data level can be made by using the TreeViewer facility. 
//
//--- NvE 30-nov-2010 IIHE Brussel
//////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");
 gSystem->Load("icepack");

 // Load the definitions of the various data structures
 // and the event loop module called "run()"
 gROOT->LoadMacro("hit-stat.cxx+");

 // Access to the input data
 TChain* data=new TChain("T");
 data->Add("$(EXTDRIVE)/WORK/data/icecube/ic86/exp/ic86test_Run00118201_sdst_filt2.icepack");

 // Define a pointer for an IcePack event
 IceEvent* evt=0;

 // Branch in the tree for the event input
 data->SetBranchAddress("IceEvent",&evt);

 // The main data processing job
 NcJob* job=new NcJob("NcJob","Processing of the IceCube event IcePack data");

 // The feature extractor and hitmaker task
// IceMakeHits* mkhits=new IceMakeHits();
// mkhits->UseWaveformNameI("CAL");
// mkhits->UseWaveformNameI("RAW");

 // The hit cleaning processor task
 IceCleanHits* clean=new IceCleanHits();
 clean->SetTotRange(0,99999,"TDOM");
 clean->SetTotRange(0,99999,"ICDOM");
 clean->SetTotRange(0,99999,"DCDOM");

 // Add the various processors as subtasks to the main job
// job->Add(mkhits);
 job->Add(clean);

 // The output tree with the hit information
 TFile* output=new TFile("hit-stat.root","RECREATE","IcePack hit data");
 TTree* otree=new TTree("T","hit data");

 Long64_t nen=(Long64_t)data->GetEntries();
 cout << endl;
 cout << " *READ* nentries : " << nen << endl;
 cout << endl;

 // Limit the number of entries for testing
 nen=100;
 cout << " Number of entries that will be processed : " << nen << endl;

 // Execute the event loop module
 run(data,evt,job,otree,nen);

 // Write the produced structures to the output file(s)
 otree->Print(); // Provide overview of the output Tree contents
 output->Write();
 output->Close();
}
