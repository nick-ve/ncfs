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
// Note : Events that have been rejected by invokation of NcEventSelector
//        will not be taken into account for the monitoring.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, July 6, 2022  09:51Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, March 8, 2023  08:57Z
///////////////////////////////////////////////////////////////////////////
 
#include "RnoMonitor.h"
#include "Riostream.h"

ClassImp(RnoMonitor) // Class implementation to enable ROOT I/O

RnoMonitor::RnoMonitor(const char* name,const char* title) : TTask(name,title)
{
// Default constructor.

 fEvt=0;
 fDevSample=1;
 fVarIndex=1;
 fVarName="";
 fVarFunc=0;
 fNbins24=24;
 fHistos.SetOwner();
 fFirst=kTRUE;

 SetDevices("RnoGANT");
 DefineStatistic("RMSdeviation");
 SetBaselineMode(0);
 SetBandFilters(fBands,0);

 NcAstrolab lab;
 lab.SetExperiment("RNO-G");
 fOffset=lab.GetLabTimeOffset();
}
///////////////////////////////////////////////////////////////////////////
RnoMonitor::~RnoMonitor()
{
// Default destructor.

 if (fVarFunc)
 {
  delete fVarFunc;
  fVarFunc=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::SetDevices(TString devclass,Int_t ista,Int_t ichan)
{
// Specify the devices to be analysed.
//
// Input arguments :
// -----------------
// devclass : The class of the (derived) devices to be analysed.
// ista     : The station number (<0 means all available stations)
// ichan    : The channel number (<0 means all available channels)
//
// In the default constructor devclass is set to "RnoGANT" to select all antenna types
// and also all channels and all stations are selected.
// At invokation of this member function, the default values are ista=-1 and ichan=-1.

 fDevClass=devclass;
 fSta=ista;
 fChan=ichan;
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
void RnoMonitor::DefineStatistic(TString mode)
{
// Specify the statistic to be used for monitoring of the (function) values
// val(i) of the selected sample variable, as specified by SetSampleVariable().
//
// Supported (case sensitive) input for "mode" is :
// ------------------------------------------------
// "Mean"         : The mean of all val(i)
// "Median"       : The median of all val(i)
// "RMS"          : The Root Mean Square of all val(i)
//                  Note : This is NOT the RMS deviation defined as sqrt(variance)
// "SpreadMean"   : The average of all |mean-val(i)|
// "SpreadMedian" : The average of all |median-val(i)|
// "RMSdeviation" : The Root Mean Square deviation from the mean of all val(i)
//                  This is also known as sqrt(variance)
// Notes :
// -------
// 1) The statistics "Mean", "Median" and "RMS" are sensitive to pedestal offsets,
//    whereas all statistics are sensitive to baseline variations within the recorded time trace.
//    Please refer to the member function SetBaselineMode() to mitigate these effects.
// 2) For large data samples, the statistics "Median", "SpreadMean" and "SpreadMedian"
//    may lead to long(er) CPU times.
//
// In the default constructor mode="RMSdeviation" is initialised.

 if (mode!="Mean" && mode!="Median" && mode!="RMS" && mode!="SpreadMean" && mode!="SpreadMedian" && mode!="RMSdeviation")
 {
  cout << " *" << ClassName() << "::DefineStatistic* Unknown mode : " << mode << endl;
  cout << " Will continue with current mode : " << fAvMode << endl;
  return;
 }

 fAvMode=mode;

 fValues.Reset();
 fValues.SetStoreMode(0);
 if (fAvMode=="Median"  || fAvMode=="SpreadMean"  || fAvMode=="SpreadMedian") fValues.SetStoreMode(1);
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::SetBaselineMode(Int_t mode,Int_t n,Double_t nrms,Double_t fpr)
{
// Specify the (variable) baseline to be subtracted from the recorded waveform before extracting
// the values of the statistic selected for monitoring, as specified by DefineStatistic().
// Variable baseline subtraction will allow to obtain a time trace with an overall baseline at 0.
//
// Input arguments :
// -----------------
// mode : 0 --> No baseline subtraction
//        1 --> Represent the baseline as fixed blocks of "n" consecutive samplings
//              with as amplitude the mean of the corresponding "n" samplings
//        2 --> Represent the baseline as fixed blocks of "n" consecutive samplings
//              with as amplitude the median of the corresponding "n" samplings
//        3 --> Represent the baseline as dynamic Bayesian Blocks obtained with the
//              parameters "nrms" and "fpr" as outlined below.
//
// The input argument "nrms" tailors the sensitivity to fluctuations for the
// Bayesian Block baseline analysis, in case that is selected.
// Each recorded value in the time trace is given an error of "nrms" times the
// RMS deviation of all the recorded values.
// The input argument "fpr" represents the False Positive Rate for the Bayesian Block analysis.
// See NcBlocks::GetBlocks() for further details.
//
// Notes :
// -------
// 1) All statistics are sensitive to baseline variations within the recorded time trace.
//    If mode>0, the corresponding baseline correction of the time trace is performed to
//    correct for pedestal offsets c.q. baseline variations.
//    In general this will result in a time trace with a baseline at 0.
// 2) Parameter values (n, nrms or fpr) that are not applicable to the selected "mode"
//    are set to the unphysical value -1.
// 3) For large data samples, the invokation of the Bayesian Block analysis may lead
//    to rather long CPU times.
//
// In the default constructor mode=0 is initialised, which also implies n=-1, nrms=-1 and fpr=-1.
// At invokation of this member function, the default values are n=128, nrms=1.2 and fpr=0.1. 

 if (mode<0 || mode>3 || n<1 || nrms<0 || fpr<0)
 {
  cout << " *" << ClassName() << "::SetBaselineMode* Inconsistent input." << endl;
  cout << " mode:" << mode << " n:" << n << " nrms:" << nrms << " fpr:" << fpr << endl;
  cout << " Will continue with current settings : mode=" << mode << " n=" << n << " nrms=" << nrms << " fpr=" << fpr << endl;
  return;
 }

 if (mode<3)
 {
  nrms=-1;
  fpr=-1;
 }
 if (!mode || mode==3) n=-1;

 fBasemode=mode;
 fBlocksize=n;
 fNrms=nrms;
 fFPR=fpr;
}
///////////////////////////////////////////////////////////////////////////
void RnoMonitor::SetBandFilters(TArray& freqs,Int_t n)
{
// Specify the frequency bands to be used for digital filtering the recorded waveform before extracting
// the values of the statistic selected for monitoring, as specified by DefineStatistic().
//
// The filtering procedure is based on a convolution of the various provided Blackman
// single Low Pass and/or High Pass and/or Band Pass and/or Band Reject filters (see below).
//
// Large values of "n" will result in sharp transitions at the edges of the specified bands,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in less sharp transitions at the edges
// of the specified bands, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) at the edges of the specified bands (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) The size of the TArray "freqs" must be even and at least twice the number of
//    frequency bands to be specified (see below).
// 2) Specifying the same frequency band more than once will further suppress the unwanted frequencies. 
//
// For further details, please refer to the docs of the memberfunction FilterMultiBand() of the class NcDSP.
//
// Input arguments :
// -----------------
// freqs  : Array containing the lower and upper bounds of the frequency band(s) in MHz.
//          The array elements represent the various pairs [flow,fup] to define the frequency bands,
//          ordered as (f1low,f1up,f2low,f2up,...).
//          The following conventions are used :
//          flow>0 and fup>0 --> Apply a Band Pass filter over [flow,fup]
//          flow<0 and fup<0 --> Apply a Band Reject filter over [|flow|,|fup|]
//          flow<0 and fup>0 --> Apply a Low Pass filter with fcut=fup
//          flow>0 and fup<0 --> Apply a High Pass filter with fcut=flow
//          In case flow=0 or fup=0 the pair [flow,fup] is neglected.
// n      : The number of points in the corresponding filter kernels
//          For best functionality this must be an odd integer.
//          An even value of "n" will be increased by 1 to obtain an odd value. 
//
// In case the provided array has a length 0 or n<1, all band filter settings will be removed.

 Int_t arrsize=freqs.GetSize();

 // Removing all band filter settings
 if (!arrsize || n<1)
 {
  fBands.Set(0);
  fNkernel=0;
  return;
 }

 Int_t oddsize=arrsize%2;
 Int_t nbands=arrsize/2;
 if (nbands<1 || n<1 || oddsize)
 {
  printf(" *%-s::SetBandFilters* Invalid input array size=%-i nbands=%-i n=%-i \n",ClassName(),arrsize,nbands,n);
  fBands.Set(0);
  fNkernel=0;
  return;
 }

 fNkernel=n;
 fBands.Set(arrsize);
 for (Int_t i=0; i<arrsize; i++)
 {
  fBands[i]=freqs.GetAt(i);
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
//
// Note : The determination of the final bin values and errors is performed
//        in the member function WriteHistograms() after all data 
//        have been processed.

 TString name=opt;
 NcJob* parent=(NcJob*)(gROOT->GetListOfTasks()->FindObject(name.Data()));

 if (!parent) return;

 fEvt=(RnoEvent*)parent->GetObject("RnoEvent");
 if (!fEvt) return;

 // Do not process rejected events
 Int_t select=fEvt->GetSelectLevel();
 if (select<0) return;

 // Get the sampling frequency
 Double_t fsample=0;
 NcDevice* daq=fEvt->GetDevice("DAQ");
 if (daq) fsample=daq->GetSignal("Sampling-rate");
 fDSP.SetSamplingFrequency(fsample);

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
 Double_t val=0;
 Double_t time=0; // Time in fractional hours
 Double_t x=0; // The x value of a point in the graph

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

  // Check for the user selected station and channel numbers
  if (fSta>=0 && ista!=fSta) continue;
  if (fChan>=0 && ichan!=fChan) continue;
   
  sx=antx->GetSample(fDevSample);

  if (!sx) continue;

  // Construct the text for the monitored observable
  sxname=sx->GetName();
  if (!fVarIndex) fVarIndex=sx->GetIndex(fVarName);
  varname=sx->GetVariableName(fVarIndex);
  monval=fAvMode;
  monval+="[";
  if (!fVarFunc)
  {
   monval+=varname;
  }
  else
  {
   monval+=fVarFunc->GetExpFormula("p");
   monval.ReplaceAll("x",varname);
  }
  monval+="]";

  Int_t ndata=sx->GetN();

  if (!ndata) continue;

  // Fill the data array with the recorded samplings
  TArrayD data(ndata);
  for (Int_t j=0; j<ndata; j++)
  {
   data[j]=sx->GetEntry(j+1,fVarIndex);
  }

  // Perform the (Bayesian) Block baseline correction if requested
  if (fBasemode)
  {
   fGin=fEvt->GetSamplingGraph(ista,ichan);
   if (fBasemode==3)
   {
    fBB.GetBlocks(fGin,fNrms,fFPR,&fHblock);
   }
   else
   {
    fBB.GetBlocks(&fGin,&fHblock,fBlocksize,fBasemode-1);
   }
   fBB.Add(&fGin,&fHblock,&fGout,-1);

   ndata=fGout.GetN();

   if (!ndata) continue;

   // Fill the data array with the modified data
   data.Set(ndata);
   for (Int_t j=0; j<ndata; j++)
   {
    fGout.GetPoint(j,x,data[j]);
   }
  }

  // Perform the frequency band filtering if requested
  Int_t narr=fBands.GetSize();
  if (narr && fNkernel && fsample>0)
  {
   fDSP.Load(&data);
   // Convert the filter bands from MHz to fractions of the sampling frequency
   TArrayD bands(narr);
   for (Int_t j=0; j<narr; j++)
   {
    bands[j]=fBands[j]*1.e6/fsample;
   }
   TH1F* hkern=(TH1F*)fHistos.FindObject("hFilterKernel");
   if (!hkern)
   {
    hkern=new TH1F();
    hkern->SetName("hFilterKernel");
    fDSP.GetMultiBandKernel(bands,fNkernel,hkern);   
    fHistos.Add(hkern);
   }
   Int_t i1=0;
   Int_t i2=0;
   TArrayD temp=fDSP.FilterMultiBand(bands,fNkernel,0,0,0,&i1,&i2);
   ndata=i2-i1+1;

   if (ndata<1) continue;

   // Fill the data array with the modified data
   data.Set(ndata);
   Int_t index=0;
   for (Int_t j=i1; j<=i2; j++)
   {
    if (index>=ndata) break;
    data[index]=temp[j];
    index++;
   }
  }

  // Construct the sample with the selected statistic values 
  fValues.Reset();
  for (Int_t j=0; j<ndata; j++)
  {
   val=data[j];
   if (fVarFunc) val=fVarFunc->Eval(val);
   fValues.Enter(val);
  }

  if (fAvMode=="Mean")         val=fValues.GetMean(1);
  if (fAvMode=="Median")       val=fValues.GetMedian(1);
  if (fAvMode=="RMS")          val=fValues.GetRMS(1);
  if (fAvMode=="SpreadMean")   val=fValues.GetSpread(1,1);
  if (fAvMode=="SpreadMedian") val=fValues.GetSpread(1,0);
  if (fAvMode=="RMSdeviation") val=fValues.GetSigma(1,0);

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
   str2+=";Universal Time (hours);";
   if (fBasemode || fNkernel) str2+="*";
   str2+="Av. ";
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
   str2+=";Summit Local Time (hours);";
   if (fBasemode || fNkernel) str2+="*";
   str2+="Av. ";
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
   str2+=";Summit Mean Siderial Time (hours);";
   if (fBasemode || fNkernel) str2+="*";
   str2+="Av. ";
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

 if (fFirst)
 {
  TString sbase="None";
  if (fBasemode==1) sbase="Mean of consecutive samples per block";
  if (fBasemode==2) sbase="Median of consecutive samples per block";
  if (fBasemode==3) sbase="Bayesian Blocks";
  printf("\n");
  printf(" *%-s::Exec* Processor parameter settings. \n",ClassName());
  printf(" Processor name ............ : %-s \n",GetName());
  printf(" Processor title ........... : %-s \n",GetTitle());
  printf(" Device class .............. : %-s \n",fDevClass.Data());
  printf(" Station number ............ : %-i (<0 means all stations) \n",fSta);
  printf(" Channel number ............ : %-i (<0 means all channels) \n",fChan);
  printf(" Device sample ............. : Index=%-i Name=%-s \n",fDevSample,sxname.Data());
  printf(" Sample variable ........... : Index=%-i Name=%-s \n",fVarIndex,varname.Data());
  printf(" Monitor value ............. : %-s \n",monval.Data());
  printf(" Baseline correction mode .. : %-i (%-s) \n",fBasemode,sbase.Data());
  if (fBasemode==1 || fBasemode==2)
  {
   printf(" Baseline block size ....... : %-i samples \n",fBlocksize);
  }
  if (fBasemode==3)
  {
   printf(" Bayesian Block nrms ....... : %-g \n",fNrms);
   printf(" Bayesian Block FPR ........ : %-g \n",fFPR);
  }
  printf(" Band Filter kernel size ... : %-i \n",fNkernel);
  printf(" Number of bins for 24 hours : %-i \n",fNbins24);
  fFirst=kFALSE;
 }
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
// Write the baseline parameter settings and all the generated histograms
// to a ROOT file with the specified filename.

 // The Tree with the baseline parameter settings
 TTree Parameters("Parameters","Parameter settings");
 Parameters.Branch("BaseMode",&fBasemode,"BaseMode/I Baseline");
 Parameters.Branch("Blocksize",&fBlocksize,"Blocksize/I # samples");
 Parameters.Branch("Nrms",&fNrms,"Nrms/D # RMS deviations");
 Parameters.Branch("FPR",&fFPR,"FPR/D False Positive Rate");
 Parameters.Fill();

 // The output file for the produced histograms
 TFile fout(filename,"RECREATE","RnoMonitor results");

 // Write the parameter settings to the output file
 Parameters.Write();

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
    sumw2=0;
    if (arrw2) sumw2=arrw2->At(ibin);
    msq=sumw2/double(nk);
    rms=sqrt(msq);

    cval=mu;
    if (fAvMode=="RMS") cval=rms;
    
    var=msq-pow(mu,2);
    s=0;
    if (var>=0) s=sqrt(var);

    hx->SetBinContent(ibin,cval);
    hx->SetBinError(ibin,s);
   }
   hx->SetEntries(nen);
  }

  hx->Write(); // Write this histogram to the output file
 }

 // Flush the output file buffers to disk
 fout.Write();

 cout << endl;
 cout << " *" << ClassName() << "::WriteHistograms* All generated histograms have been written to file : " << filename << endl;
 ListHistograms();
}
///////////////////////////////////////////////////////////////////////////
