//////////////////////////////////////////////////////////
// ROOT macro to convert the summary text file of the   //
// GRBweb database of Paul Coppin into a ROOT Tree.     //
//                                                      //
// --- Nick van Eijndhoven 13-sep-2018 IIHE-VUB Brussel //
// --- Updated by NvE December 11, 2023  12:19Z         //
//////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");

 // The GRB input data 

 ifstream gcn;
 gcn.clear();
 gcn.open("Summary_table.txt");
 if (!gcn.good())
 {
  cout << " *** Data file for GRB not found ***" << endl;
  return;
 }
 gcn.seekg(0); // Position at begin of file

 // Input variables
 TString namegcn,namefermi,idname,name,ttrig,t90start,gbmloc;
 Float_t ra,decl,sigmapos,t90,sigmat90,fluence,sigmafluence,z,t100;
 Double_t mjdtrig,mjdt90start,mjdt90end;

 // Additional variables for conversion and/or output
 Int_t date,gbmpos,leapsec;
 Float_t dut;
 Double_t dutx;

 NcTimestamp ts;
 ts.LoadUTCparameterFiles();

 // The produced output structure
 TFile* output=new TFile("GRBweb.root","RECREATE","GRB database");
 TTree* otree=new TTree("T","GRB data");

 // The output variables for the Tree
 otree->Branch("name",(void*)(idname.Data()),"name/C");
 otree->Branch("date",&date,"date/I");
 otree->Branch("leapsec",&leapsec,"leapsec/I");
 otree->Branch("dut",&dut,"dut/F");
 otree->Branch("gbmpos",&gbmpos,"gbmpos/I");
 otree->Branch("ra",&ra,"ra/F");
 otree->Branch("dec",&decl,"dec/F");
 otree->Branch("sigmapos",&sigmapos,"sigmapos/F");
 otree->Branch("t90",&t90,"t90/F");
 otree->Branch("sigmat90",&sigmat90,"sigmat90/F");
 otree->Branch("mjdtrig",&mjdtrig,"mjdtrig/D");
 otree->Branch("mjdt90start",&mjdt90start,"mjdt90start/D");
 otree->Branch("mjdt90end",&mjdt90end,"mjdt90end/D");
 otree->Branch("t100",&t100,"t100/F");
 otree->Branch("fluence",&fluence,"fluence/F");
 otree->Branch("sigmafluence",&sigmafluence,"sigmafluence/F");
 otree->Branch("z",&z,"z/F");

 ///////////////////////////////////////////////////
 // Read the GRB data and produce the output Tree //
 ///////////////////////////////////////////////////

 // Read the title lines
 string line;
 for (Int_t i=0; i<4; i++)
 {
  getline(gcn,line);
 }

 Int_t n=0;
 while (gcn >> namegcn >> namefermi >> ttrig >> ra >> decl >> sigmapos >> t90 >> sigmat90 >> t90start
            >> fluence >> sigmafluence >> z >> t100 >> gbmloc >> mjdtrig)
 {
  // Check for consistent data
  idname=namegcn;
  if (idname=="None") idname=namefermi;

  if (idname=="None" || ttrig=="-999" || mjdtrig<=0) continue;

  /////////////////////////////////
  // Create some additional data //
  /////////////////////////////////

  // Obtain the UTC parameters for Leap Seconds and dUT=UT1-UTC at the trigger time
  ts.GetUTCparameters(int(mjdtrig),leapsec,dutx);
  dut=float(dutx);

  // Set the gbmpos flag to indicate whether the position was determined by the Fermi GBM (1) or not (0)
  gbmpos=0;
  if (gbmloc=="True") gbmpos=1;

  // Extract the date as yymmdd from the idname
  name=idname;
  name.Remove(0,3);
  name.Remove(6);

  // Expand to the full yyyymmdd date indication 
  if (int(mjdtrig)<51544)
  {
   name.Prepend("19");
  }
  else
  {
   name.Prepend("20");
  }

  date=name.Atoi();
  // Set the trigger timestamp based on the date and UTC trigger time
  // Note : The trigger MJD in the input file was NOT corrected for dUT=UT1-UTC
  // So, we will use the UTC trigger time and store the corrected trigger MJD
  ts.SetTAI("UTC",name,ttrig,1,"A",0);
  mjdtrig=ts.GetMJD();

  // Set the dUT=UT1-UTC corrected MJD for the T90start and T90end times
  mjdt90start=0;
  mjdt90end=0;
  if (t90start!="-999")
  {
   ts.SetTAI("UTC",name,t90start,1,"A",0);
   mjdt90start=ts.GetMJD();
   ts.AddSec(t90);
   mjdt90end=ts.GetMJD();
  }

  otree->Fill();

  n++;
 }

 cout << endl;
 cout << " *** Number of database Tree entries : " << n << endl;

 // Write the produced structures to the output file
 output->Write();
 output->Close();
}
