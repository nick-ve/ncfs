/*******************************************************************************
 * Copyright(c) 2022, RNO-G Experiment at Summit Station. All rights reserved.
 *
 * Author: The RNO-G NCFS-based Offline Project.
 * Contributors are mentioned in the code where appropriate.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation strictly for non-commercial purposes is hereby granted
 * without fee, provided that the above copyright notice appears in all
 * copies and that both the copyright notice and this permission notice
 * appear in the supporting documentation.
 * The authors make no claims about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *******************************************************************************/

///////////////////////////////////////////////////////////////////////////
// Class RnoMonitor
// TTask derived class to perform RNO-G data monitoring over certain time periods.
//
// This processor can be used to e.g. search for recurrent astrophysical signals
// due to the rotation of the Earth.
// For this, several 24 hour time windows are defined in different time conventions
// (i.e. Universal Time, Local Time and Local Mean Sidereal Time) in which event
// data is recorded in histograms based on the event time stamps.
// Data recording is performed in a "stacked" fashion, meaning that events that
// occurred at different days but at approximately the same daytime, will be 
// recorded within the same histogram bin.
// This procedure will amplify signals recurring at the same daytime, whereas
// random occurrences will be averaged out.
//
// During the processing, various histograms are created which can be
// written to an output file by invoking the member function WriteHistograms()
// after the processing of all data has completed. 
//
// This monitoring can be performed "on the fly" by declaring this facility
// as a sub-task of RnoConvert, as can be seen in the ROOT macro convert.cc
// which is located in the folder /rnopack/macros.
//
// However, this task can also be performed on the rnopack data structures
// which reside in the files that are created by RnoConvert.
// An example of such an implementation can be seen in the ROOT macro analyze.cc
// which is also located in the folder /rnopack/macros.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, July 6, 2022  09:51Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 10, 2022  21:14Z
///////////////////////////////////////////////////////////////////////////
 
#include "RnoMonitor.h"
#include "Riostream.h"

ClassImp(RnoMonitor) // Class implementation to enable ROOT I/O

RnoMonitor::RnoMonitor(const char* name,const char* title) : TTask(name,title)
{
// Default constructor.

 fEvt=0;
 fDevClass="RnoGANT";
 fDevSample=1;
 fVarIndex=1;
 fVarName="";
 fVarFunc=0;
 fNbins24=24;
 fHistos.SetOwner();
 fValues.SetStoreMode(1);
 fAvMode="Median";
 fFirst=kTRUE;

 NcAstrolab lab;
 lab.SetExperiment("RNO-G");
 fOffset=lab.GetLabTimeOffset();
}
///////////////////////////////////////////////////////////////////////////
RnoMonitor::~RnoMonitor()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::SetDeviceClass(TString devclass)
{
// Specify the class of the (derived) devices to be analysed.
//
// In the default constructor this is set to "RnoGANT" to select all antennas.

 fDevClass=devclass;
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::SetDeviceSample(Int_t j)
{
// Specify the sample (1=first) to be used from the stored device samples.
//
// In the default constructor this is set to 1 to select the first sample.

 fDevSample=j;
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::SetSampleVariable(Int_t i,TString f)
{
// Specify the (function of the) sample variable to be used for monitoring.
//
// Input arguments :
// -----------------
// i : Index (1=first) of the sample variable to be used
// f : Function of the specified variable to obtain the observable to be monitored.
//
// The 1D function "f" has to be specified following the TF1 string format convention.
// In case f="-" (which is the default) is specified, just the value of the i-th variable is used.  
//
// In the default constructor i=1 and f="-" are set.

 fVarIndex=i;
 fVarName="";

 if (fVarFunc)
 {
  delete fVarFunc;
  fVarFunc=0;
 }

 if (f!="-") fVarFunc=new TF1("VarFunc",f);
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::SetSampleVariable(TString name,TString f)
{
// Specify the (function of the) sample variable to be used for monitoring.
//
// Input arguments :
// -----------------
// name : Name of the sample variable to be used
// f    : Function of the specified variable to obtain the observable to be monitored.
//
// The 1D function "f" has to be specified following the TF1 string format convention.
// In case f="-" (which is the default) is specified, just the value of the specified variable is used.  

 fVarName=name;
 fVarIndex=0;

 if (fVarFunc)
 {
  delete fVarFunc;
  fVarFunc=0;
 }

 if (f!="-") fVarFunc=new TF1("VarFunc",f);
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::DefineCentralValue(TString mode)
{
// Specify the central value to be used for monitoring.
// Supported (case sensitive) input is "Mean", "Median" or "RMS".
//
// In the default constructor the mode "Median" is initialised.

 if (mode!="Mean" && mode!="Median" && mode!="RMS")
 {
  cout << " *" << ClassName() << "::DefineCentralValue* Unknown mode : " << mode << endl;
  cout << " Will continue with current mode : " << fAvMode << endl;
  return;
 }

 fAvMode="Median";
 if (mode=="Mean") fAvMode="Mean";
 if (mode=="RMS") fAvMode="RMS";

 if (fAvMode=="Mean" || fAvMode=="RMS")
 {
  fValues.Reset();
  fValues.SetStoreMode(0);
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::SetNbins24(Int_t n)
{
// Set the number of bins for the 24 hour monitoring histograms.
// In the default constructor the number of bins is set to 24,
// which corresponds to a daily time resolution of 1 hour.

 if (n<1)
 {
  cout << " *" << ClassName() << "::SetNbins24* Inconsistent input : " << n << endl;
  cout << " Will continue with current value : " << fNbins24 << endl;
  return;
 }

 fNbins24=n; 
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::Exec(Option_t* opt)
{
// Implementation of the monitoring procedures.

 TString name=opt;
 NcJob* parent=(NcJob*)(gROOT->GetListOfTasks()->FindObject(name.Data()));

 if (!parent) return;

 fEvt=(RnoEvent*)parent->GetObject("RnoEvent");
 if (!fEvt) return;

 // Do not process rejected events
 Int_t select=fEvt->GetSelectLevel();
 if (select<0) return;

 RnoGANT* antx=0;   // Pointer for a generic RNO antenna
 Int_t ista=0;      // Station number
 Int_t ichan=0;     // Channel number
 TObjArray* devs=0; // List of selected devices
 TString str;
 TString str2;
 Int_t ndevs=0;
 NcSample* sx=0;
 TString sxname="none";
 TString varname="none";
 TString monval="none";
 Float_t val=0;
 Double_t time=0; // Time in fractional hours

 // The daily 24 hour monitoring histograms
 TH1F* hut24=0;
 TH1I* hnut24=0;
 TH1F* hlt24=0;
 TH1I* hnlt24=0;
 TH1F* hlmst24=0;
 TH1I* hnlmst24=0;

 // The bin size in minutes
 Float_t bsize=24.*60./float(fNbins24);

 devs=fEvt->GetDevices(fDevClass);
 ndevs=devs->GetEntries();

 for (Int_t idev=0; idev<ndevs; idev++)
 {
  antx=(RnoGANT*)devs->At(idev);

  if (!antx) continue;

  ista=antx->GetStation();
  ichan=antx->GetNumber(-1);
   
  sx=antx->GetSample(fDevSample);

  if (!sx) continue;

  sxname=sx->GetName();
  if (!fVarIndex) fVarIndex=sx->GetIndex(fVarName);
  varname=sx->GetVariableName(fVarIndex);
  monval=fAvMode;
  monval+="[";
  if (!fVarFunc)
  {
   if (fAvMode=="Mean") val=sx->GetMean(fVarIndex);
   if (fAvMode=="Median") val=sx->GetMedian(fVarIndex);
   if (fAvMode=="RMS") val=sx->GetRMS(fVarIndex);
   monval+=varname;
  }
  else
  {
   fValues.Reset();
   for (Int_t ival=1; ival<=sx->GetN(); ival++)
   {
    val=sx->GetEntry(ival,fVarIndex);
    val=fVarFunc->Eval(val);
    fValues.Enter(val);
   }
   if (fAvMode=="Mean") val=fValues.GetMean(1);
   if (fAvMode=="Median") val=fValues.GetMedian(1);
   if (fAvMode=="RMS") val=fValues.GetRMS(1);
   monval+=fVarFunc->GetExpFormula("p");
   monval.ReplaceAll("x",varname);
  }
  monval+="]";

  str="hUT24-S";
  str+=ista;
  str+="Ch";
  str+=ichan;

  str2=str;
  str2+="-N";

  hut24=(TH1F*)fHistos.FindObject(str);
  hnut24=(TH1I*)fHistos.FindObject(str2);

  if (!hut24)
  {
   hnut24=new TH1I(str2,"UT24 bin entry counts;Universal Time (hours);Number of entries",fNbins24,0,24);
   fHistos.Add(hnut24);
   hut24=new TH1F(str,"UT daily monitoring",fNbins24,0,24);
   hut24->Sumw2();
   str.Form("Daily monitoring (%-.3g min. periods) of Station%-i ",bsize,ista);
   str+=antx->GetName();
   str2=str;
   str2+=";Universal Time (hours);Averaged ";
   str2+=monval;
   hut24->SetTitle(str2);
   fHistos.Add(hut24);
  }

  str="hLT24-S";
  str+=ista;
  str+="Ch";
  str+=ichan;

  str2=str;
  str2+="-N";

  hlt24=(TH1F*)fHistos.FindObject(str);
  hnlt24=(TH1I*)fHistos.FindObject(str2);

  if (!hlt24)
  {
   hnlt24=new TH1I(str2,"LT24 bin entry counts;Summit Local Time (hours);Number of entries",fNbins24,0,24);
   fHistos.Add(hnlt24);
   hlt24=new TH1F(str,"LT daily monitoring",fNbins24,0,24);
   hlt24->Sumw2();
   str.Form("Daily monitoring (%-.3g min. periods) of Station%-i ",bsize,ista);
   str+=antx->GetName();
   str2=str;
   str2+=";Summit Local Time (hours);Averaged ";
   str2+=monval;
   hlt24->SetTitle(str2);
   fHistos.Add(hlt24);
  }

  str="hLMST24-S";
  str+=ista;
  str+="Ch";
  str+=ichan;

  str2=str;
  str2+="-N";

  hlmst24=(TH1F*)fHistos.FindObject(str);
  hnlmst24=(TH1I*)fHistos.FindObject(str2);

  if (!hlmst24)
  {
   hnlmst24=new TH1I(str2,"LMST24 bin entry counts;Summit Mean Siderial Time (hours);Number of entries",fNbins24,0,24);
   fHistos.Add(hnlmst24);
   hlmst24=new TH1F(str,"LMST daily monitoring",fNbins24,0,24);
   hlmst24->Sumw2();
   str.Form("Daily monitoring (%-.3g min. periods) of Station%-i ",bsize,ista);
   str+=antx->GetName();
   str2=str;
   str2+=";Summit Mean Siderial Time (hours);Averaged ";
   str2+=monval;
   hlmst24->SetTitle(str2);
   fHistos.Add(hlmst24);
  }

  time=fEvt->GetUT();
  hut24->Fill(time,val);
  hnut24->Fill(time);

  time=fEvt->GetLT(fOffset);
  hlt24->Fill(time,val);
  hnlt24->Fill(time);

  time=fEvt->GetLMST(fOffset);
  hlmst24->Fill(time,val);
  hnlmst24->Fill(time);
 } // End of loop over devices

 // Storage of the used parameters in the RnoMonitor device
 NcDevice params;
 params.SetNameTitle("RnoMonitor","RnoMonitor processor parameters");
 str="DeviceClass=";
 str+=fDevClass;
 params.AddNamedSlot(str);
 params.SetSignal(0,str);
 str="DeviceSample";
 if (sxname!="")
 {
  str+="=";
  str+=sxname;
 }
 params.AddNamedSlot(str);
 params.SetSignal(fDevSample,str);
 str="SampleVariable=";
 str+=varname;
 params.AddNamedSlot(str);
 params.SetSignal(fVarIndex,str);
 str="MonitorValue=";
 str+=monval;
 params.AddNamedSlot(str);
 params.SetSignal(0,str);
 params.AddNamedSlot("Nbins24");
 params.SetSignal(fNbins24,"Nbins24");

 fEvt->AddDevice(params);

 if (fFirst)
 {
  cout << endl;
  cout << " *" << ClassName() << "::Exec* Processor parameter settings." << endl;
  params.Data();
 }

 fFirst=kFALSE;
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::ListHistograms() const
{
// Provide a list of all the stored histograms.

 Int_t nh=fHistos.GetEntries();
 cout << endl;
 cout << " === The following " << nh << " histograms have been generated for DeviceClass : " << fDevClass << endl;
 for (Int_t ih=0; ih<nh; ih++)
 {
  TObject* hx=fHistos.At(ih);
  if (!hx) continue;
  cout << " " << hx->GetName() << " : " << hx->GetTitle() << endl;
 }
 cout << " ===============================================================================" << endl;
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::WriteHistograms(TString filename)
{
// Write all the generated histograms to a ROOT file with the specified filename.

 // The output file for the produced histograms
 TFile fout(filename,"RECREATE","RnoMonitor results");

 // Write all the histos to the output file
 Int_t nh=fHistos.GetEntries();
 TString name;
 Int_t nen=0;
 Int_t nbins=0;
 Int_t nk=0;
 Double_t sumw2;
 Double_t mu=0;
 Double_t msq=0;
 Double_t rms=0;
 Double_t cval=0;
 Double_t var=0;
 Double_t s=0;
 for (Int_t ih=0; ih<nh; ih++)
 {
  TH1* hx=(TH1*)fHistos.At(ih);
  if (!hx) continue;

  // Get the corresponding histogram with the individual bin counts
  name=hx->GetName();
  name+="-N";
  TH1* hn=(TH1*)fHistos.FindObject(name);

  // Determine the central value and its error of the individual bin samplings
  if (hn)
  {
   nbins=hx->GetNbinsX();
   nen=hx->GetEntries();

   TArrayD* arrw2=hx->GetSumw2();
   for (Int_t ibin=1; ibin<=nbins; ibin++)
   {
    nk=hn->GetBinContent(ibin);
    if (!nk) continue;

    mu=hx->GetBinContent(ibin)/double(nk);
    sumw2=arrw2->At(ibin);
    msq=sumw2/double(nk);
    rms=sqrt(msq);

    cval=mu;
    if (fAvMode=="RMS") cval=rms;
    
    var=msq-pow(mu,2);
    s=sqrt(fabs(var));

    hx->SetBinContent(ibin,cval);
    hx->SetBinError(ibin,s);
   }
   hx->SetEntries(nen);
  }

  hx->Write(); // Write this histogram to the output file
 }

 fout.Write();

 cout << endl;
 cout << " *" << ClassName() << "::WriteHistograms* All generated histograms have been written to file : " << filename << endl;
 ListHistograms();
}
///////////////////////////////////////////////////////////////////////////
