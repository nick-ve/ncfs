//////////////////////////////////////////////////
// ROOT macro to modify specific constants in an
// existing Amanda/IceCube calib. dbase root file.
//
// To execute the macro just enter the command
// 
// root -b -q modcal.cc
//
// This will create the updated calib. dbase
// in a newly created output calib. dbase file.
//
// Specifications of all input and output files
// are performed in the macro code itself as given
// below.
//
// Note :
// ------
// The new values of the various constants to be
// updated are read from separate ASCII files.
// The user has to provide these files and modify
// the current macro such that the new values are
// read-in correctly.
//
// The current macro takes the file cal2007.root
// as input and modifies the Amanda TWR values for
// stop-delays, T0's and ADC amplitude calibs.
// The corresponding ASCII input files are :
//
// stop-delay-mc2007.txt
// twrsim-t0-calib2007.txt
// twrsim-pe-area2007.txt 
//
// NvE 26mar-2008 Utrecht University
//////////////////////////////////////////////////
{
 gSystem->Load("ralice");
 gSystem->Load("icepack");
 gSystem->Load("iceconvert");

 TFile fdbin("cal2007.root");
 AliObjMatrix* mudaq=(AliObjMatrix*)fdbin.Get("MuDaq-OMDBASE");
 AliObjMatrix* twrdaq=(AliObjMatrix*)fdbin.Get("TWRDaq-OMDBASE");
 AliObjMatrix* jebtdaq=(AliObjMatrix*)fdbin.Get("JEBTDaq-OMDBASE");
 AliObjMatrix* jebadaq=(AliObjMatrix*)fdbin.Get("JEBADaq-OMDBASE");
 TDatabasePDG* pdg=(TDatabasePDG*)fdbin.Get("PDG-DBASE");

 TFile fdbout("twrsim-cal2007.root","RECREATE","Modified TWR sim calib. dbase in IcePack structure");
 AliObjMatrix* twrdaq2=new AliObjMatrix(*twrdaq);
 AliObjMatrix* jebtdaq2=new AliObjMatrix(*jebtdaq);

 // Modify database values

 IceGOM* om=0;
 Int_t omid;

 // Stop-delay modifications
 ifstream fin1("stop-delay-mc2007.txt");
 Int_t string,level;
 Float_t extstop;
 IceAOM omdum;
 while (fin1 >> string >> level >> extstop)
 {
  omid=omdum.GetOMId(string,level);
  om=(IceGOM*)twrdaq2->GetObject(omid,1);
  if (om) om->SetSignal(extstop,"EXTSTOP");
  om=(IceGOM*)jebtdaq2->GetObject(omid,1);
  if (om) om->SetSignal(extstop,"EXTSTOP");
 }

 // T0 modifications
 ifstream fin2("twrsim-t0-calib2007.txt");
 Float_t t0;
 TF1* func=0;
 while (fin2 >> omid >> t0)
 {
  om=(IceGOM*)twrdaq2->GetObject(omid,1);
  func=om->GetCalFunction("LE");
  if (func) func->SetParameter(0,t0);
  func=om->GetDecalFunction("LE");
  if (func) func->SetParameter(0,t0);
  om=(IceGOM*)jebtdaq2->GetObject(omid,1);
  func=om->GetCalFunction("LE");
  if (func) func->SetParameter(0,t0);
  func=om->GetDecalFunction("LE");
  if (func) func->SetParameter(0,t0);
 }

 // ADC calib modifications
 ifstream fin3("twrsim-pe-area2007.txt");
 Float_t adc;
 while (fin3 >> omid >> adc)
 {
  adc/=10.; // Correction for nC/PE our convention
  om=(IceGOM*)twrdaq2->GetObject(omid,1);
  func=om->GetCalFunction("ADC");
  if (func) func->SetParameter(0,adc);
  func=om->GetDecalFunction("ADC");
  if (func) func->SetParameter(0,adc);
  om=(IceGOM*)jebtdaq2->GetObject(omid,1);
  func=om->GetCalFunction("ADC");
  if (func) func->SetParameter(0,adc);
  func=om->GetDecalFunction("ADC");
  if (func) func->SetParameter(0,adc);
 }

 fdbout.cd();
 mudaq->Write();
 jebadaq->Write();
 pdg->Write();

 twrdaq2->Write();
 jebtdaq2->Write();
}