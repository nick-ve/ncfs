///////////////////////////////////////////////////////////////////////////////////
// ROOT macro to convert FITS data into a plain ROOT Tree structure.
// A description of the variables and units is given in the Branch definitions
// and is visible via TTree->Print() or mousing over the variables in a TBrowser.
//
// Note : This ROOT macro uses the NCFSPack analysis framework.
//        For details see https://nick-ve.github.io/ncfs/
//
// The input and output data files are specified in this ROOT macro.
// In this template the FITS input data file is named "data.fits" and
// the ROOT output file is named "data.root".
//
// Keeping these generic names allows the user to use this ROOT macro "as is"
// and provide/rename the corresponding files via the fits2root.sub HTCondor
// batch processing script.
//
// This template is tailored for IceCube data.
//
//-- Nick van Eijndhoven, IIHE-VUB Brussel, February 7, 2022  13:36Z
///////////////////////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");

 NcVersion version;
 version.Data();

 NcFITSIO q;

 if (!q.OpenInputFile("data.fits[1]")) return;

 q.ListFileHeader();

 Int_t nrows=q.GetTableNrows();
 Int_t ncols=q.GetTableNcolumns();
 cout << endl;
 cout << " *** nrows:" << nrows << " ncols:" << ncols << endl; 
 
 q.ListTable();

 q.ListTable(10,1,10);

 // Title string for the created output file and Tree
 TString title="IceCube GFU data";

 // The produced output structure
 TFile* output=new TFile("data.root","RECREATE",title);
 TTree* otree=new TTree("T",title);

 // Event reconstruction data
 Long64_t run,event;
 Int_t subevent;
 Double_t mjd;
 Float_t logE,AngErr,ra,decl,phi,zen,bdt;

 // Additional data from Monte Carlo simulations
 Float_t mclogEnu,mcra,mcdecl,mcphi,mczen,mcwt;

 // Good Run List (GRL) data
 Long64_t nevents;
 Double_t mjdstart,mjdstop;
 Float_t livetime;
 Int_t goodrun;

 ///////////////////////////////////////
 // The output variables for the Tree //
 ///////////////////////////////////////

 // Event reconstruction data
 if (q.GetColumnNumber("run"))      otree->Branch("run",&run,"run/L");
 if (q.GetColumnNumber("event"))    otree->Branch("event",&event,"event/L");
 if (q.GetColumnNumber("subevent")) otree->Branch("subevent",&subevent,"subevent/I");
 if (q.GetColumnNumber("time"))     otree->Branch("mjd",&mjd,"mjd/D");
 if (q.GetColumnNumber("logE"))     otree->Branch("logE",&logE,"logE/F [GeV]");
 if (q.GetColumnNumber("angErr"))   otree->Branch("AngErr",&AngErr,"AngErr/F [rad]");
 if (q.GetColumnNumber("ra"))       otree->Branch("ra",&ra,"ra/F J2000 [rad]");
 if (q.GetColumnNumber("dec"))      otree->Branch("dec",&decl,"dec/F J2000 [rad]");
 if (q.GetColumnNumber("azi"))      otree->Branch("phi",&phi,"phi/F local phi angle [rad]");
 if (q.GetColumnNumber("zen"))      otree->Branch("zen",&zen,"zen/F zenith angle [rad]");
 if (q.GetColumnNumber("bdtScore")) otree->Branch("bdt",&bdt,"bdt/F BDT score");

 // Additional data from Monte Carlo simulations
 if (q.GetColumnNumber("trueAzi")) otree->Branch("MCphi",&mcphi,"MCphi/F phi angle [rad]");
 if (q.GetColumnNumber("trueZen")) otree->Branch("MCzen",&mczen,"MCzen/F zenith angle [rad]");
 if (q.GetColumnNumber("trueRa"))  otree->Branch("MCra",&mcra,"MCra/F J2000 [rad]");
 if (q.GetColumnNumber("trueDec")) otree->Branch("MCdec",&mcdecl,"MCdec/F J2000 [rad]");
 if (q.GetColumnNumber("trueE"))   otree->Branch("MClogEnu",&mclogEnu,"MClogEnu/F [GeV]");
 if (q.GetColumnNumber("ow"))      otree->Branch("MCwt",&mcwt,"MCwt/F OneWeight/nevt");

 // (Additional) data from the Good Run List (GRL)
 if (q.GetColumnNumber("start"))    otree->Branch("mjdstart",&mjdstart,"mjdstart/D");
 if (q.GetColumnNumber("stop"))     otree->Branch("mjdstop",&mjdstop,"mjdstop/D");
 if (q.GetColumnNumber("livetime")) otree->Branch("livetime",&livetime,"livetime/F [days]");
 if (q.GetColumnNumber("events"))   otree->Branch("nevents",&nevents,"nevents/L");
 if (q.GetColumnNumber("good_i3"))  otree->Branch("goodrun",&goodrun,"goodrun/I Flag");

 Double_t val=0;
 for (Int_t i=1; i<=nrows; i++)
 {
  // Event reconstruction data
  q.GetTableCell(val,i,"run");
  run=val+0.001;
  q.GetTableCell(val,i,"event");
  event=val+0.001;
  q.GetTableCell(val,i,"subevent");
  subevent=TMath::Nint(val);
  q.GetTableCell(val,i,"time");
  mjd=val;
  q.GetTableCell(val,i,"logE");
  logE=val;
  q.GetTableCell(val,i,"angErr");
  AngErr=val;
  q.GetTableCell(val,i,"ra");
  ra=val;
  q.GetTableCell(val,i,"dec");
  decl=val;
  q.GetTableCell(val,i,"azi");
  phi=val;
  q.GetTableCell(val,i,"zen");
  zen=val;

  // Additional data from Monte Carlo simulations
  q.GetTableCell(val,i,"trueAzi");
  mcphi=val;
  q.GetTableCell(val,i,"trueZen");
  mczen=val;
  q.GetTableCell(val,i,"trueRa");
  mcra=val;
  q.GetTableCell(val,i,"trueDec");
  mcdecl=val;
  q.GetTableCell(val,i,"trueE");
  mclogEnu=-9;
  if (val>0) mclogEnu=log10(val);
  q.GetTableCell(val,i,"ow");
  mcwt=val;

  // (Additional) data from the Good Run List
  q.GetTableCell(val,i,"start");
  mjdstart=val;
  q.GetTableCell(val,i,"stop");
  mjdstop=val;
  q.GetTableCell(val,i,"livetime");
  livetime=val;
  q.GetTableCell(val,i,"events");
  nevents=val+0.001;
  q.GetTableCell(val,i,"good_i3");
  goodrun=TMath::Nint(val);

  otree->Fill();
 }

 cout << endl;
 cout << " *** Overview of the created ROOT output Tree ***" << endl;
 cout << endl; 

 otree->Print();
 
 output->Write();
 output->Close();
}
