////////////////////////////////////////////////////////
// Macro to test performance of track reconstruction
// by producing a ROOT Tree output of IcePack structures.
//
// To run this macro, just do
//
// $ root -b -q track-stat.cc
//
//--- NvE 11-jan-2010 IIHE, Brussel
/////////////////////////////////////////////
{
 gSystem->Load("ncfspack");
 gSystem->Load("icepack");

 // Access to the input data
 TChain* data=new TChain("T");
 data->AddFile("./ic77-nugen-numu-2111-part0.icepack");
 data->AddFile("./ic77-nugen-numu-2111-part1.icepack");
 data->AddFile("./ic77-nugen-numu-2111-part2.icepack");

 // Define a pointer for an event
 IceEvent* evt=0;

 // Branch in the tree for the event input
 data->SetBranchAddress("IceEvent",&evt);

 // The main data processing job
 NcJob* job=new NcJob("NcJob","Processing of the IceCube event ROOT data");

 // The Tree production job
 NcJob* treedump=new NcJob("Tree","Production of the event in ROOT Tree format");

 // The task to convert the IceEvent data into a tree structure
 NcTreeMaker* mktree=new NcTreeMaker();
 mktree->SetOutputFile("./ic77-nugen-numu-2111-tree.root","IC77 track reco stats");
 mktree->UseTracks("MostEnergeticTrack");
 mktree->UseTracks("IceLinefitI");
 mktree->UseTracks("IceDwalkI");
 mktree->UseTracks("IceDwalkIC");
 mktree->UseTracks("IceDwalkDC");
 mktree->Select("track","p");
 mktree->Select("track","q");

 treedump->Add(mktree);

 // The feature extractor and hitmaker task
 IceMakeHits* mkhits=new IceMakeHits();
 mkhits->SetWaveformNameI("CAL-ATWD");

 // The hit cleaning processor task
 IceCleanHits* clean=new IceCleanHits();

 // The linefit reconstruction task
 IceLinefit* lfit=new IceLinefit();

 // The direct walk reconstruction task
 IceDwalk* dwalk=new IceDwalk();

 // The Pandel fitting procedure
 IcePandel* pandel=new IcePandel();
 pandel->UseTracks("IceLinefit");
 pandel->UseTracks("IceDwalk");
// pandel->SetPrintLevel(0);

 // Add the various processors as subtasks to the main job
 job->Add(mkhits);
 job->Add(clean);
 job->Add(lfit);
 job->Add(dwalk);
// job->Add(pandel);

 Int_t nen=(int)data->GetEntries();
 cout << endl;
 cout << " *READ* nentries : " << nen << endl;
 cout << endl;

 // Limit the number of entries for testing
// nen=500;

 TObjArray* tracks=0;
 Double_t alpha=0;
 NcTrack* mcmu=0;
 NcTrack* tx=0;
 for (Int_t ien=0; ien<nen; ien++)
 {
  if (evt) evt->Reset();

  data->GetEntry(ien);
  if (!evt) continue;

  if (!(ien%10)) cout << " Processing entry : " << ien << endl;

  job->ProcessObject(evt);

  // Use the track charge to denote the opening angle (in deg.)
  // with the original MC muon track.
  mcmu=0;
  tracks=evt->GetTracks("MostEnergeticTrack");
  if (tracks) mcmu=(NcTrack*)tracks->At(0);

  if (!mcmu) continue;

  tracks=evt->GetTracks("IceDwalkI");
  if (tracks)
  {
   for (Int_t it=0; it<tracks->GetEntries(); it++)
   {
    tx=(NcTrack*)tracks->At(it);
    if (!tx) continue;
    alpha=tx->GetOpeningAngle(*mcmu,"deg");
    tx->SetCharge(alpha);
   }
  }

  tracks=evt->GetTracks("IceDwalkIC");
  if (tracks)
  {
   for (Int_t it=0; it<tracks->GetEntries(); it++)
   {
    tx=(NcTrack*)tracks->At(it);
    if (!tx) continue;
    alpha=tx->GetOpeningAngle(*mcmu,"deg");
    tx->SetCharge(alpha);
   }
  }

  tracks=evt->GetTracks("IceDwalkDC");
  if (tracks)
  {
   for (Int_t it=0; it<tracks->GetEntries(); it++)
   {
    tx=(NcTrack*)tracks->At(it);
    if (!tx) continue;
    alpha=tx->GetOpeningAngle(*mcmu,"deg");
    tx->SetCharge(alpha);
   }
  }

  tracks=evt->GetTracks("IceLinefitI");
  if (tracks)
  {
   for (Int_t it=0; it<tracks->GetEntries(); it++)
   {
    tx=(NcTrack*)tracks->At(it);
    if (!tx) continue;
    alpha=tx->GetOpeningAngle(*mcmu,"deg");
    tx->SetCharge(alpha);
   }
  }

  treedump->ProcessObject(evt);
 }

 mktree->CloseTree();
}
