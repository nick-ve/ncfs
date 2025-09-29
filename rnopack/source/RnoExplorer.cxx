/*******************************************************************************
~~~
 * Copyright(c) 2023, RNO-G Experiment at Summit Station. All rights reserved.
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
~~~
 *******************************************************************************/

///////////////////////////////////////////////////////////////////////////
/** @class RnoExplorer
~~~
// Class RnoExplorer
// Graphical User Interface (GUI) to explore RNO-G event data.
//
// This class is derived from TChain to directly access the data (files).
//
//--- Author: Nick van Eijndhoven 19-jul-2023 IIHE-VUB Brussel.
//- Modified: Nick van Eijndhoven, IIHE-VUB Brussel, September 24, 2025  09:23Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "RnoExplorer.h"
#include "Riostream.h"
 
ClassImp(RnoExplorer); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
RnoExplorer::RnoExplorer(const char* name,const char* title) : TChain(name,title)
{
/**
~~~
// Default constructor.
// The "name"and "title" input arguments have the same meaning as for the
// default constructor of TChain.
//
// The default values are name="T" and title="RnoExplorer".
~~~
**/

 fExpPanel=0;
 fEnt=0;
 fEvt=0;
 fSample=0;
 fStation=0;
 fChannel=-1;
 fMode="ADC(s)";
 fFmV="x*2.5e3/(pow(2,12)-1)";
 fOpsMode="None";
 for (Int_t i=0; i<3; i++)
 {
  fOpsPars[0]="";
 }

 for (Int_t i=0; i<3; i++)
 {
  fCanvas[i]=0;
 }
 fFiles=0;
 fNentries=0;
 fConnected=kFALSE;
 fEntry=0;
 for (Int_t i=0; i<4; i++)
 {
  fHead[i]=0;
 }
 for (Int_t i=0; i<3; i++)
 {
  fParams[i]=0;
 }

 fDevName="*";

 for (Int_t i=0; i<24; i++)
 {
  fFilterBands[i].Set(0);
 }

 fIndexed=kFALSE;

 ExplorePanel();
}
///////////////////////////////////////////////////////////////////////////
RnoExplorer::~RnoExplorer()
{
/**
~~~
// Destructor to delete all allocated memory.
~~~
**/

 for (Int_t i=0; i<3; i++)
 {
  if (fCanvas[i])
  {
   TString s=ClassName();
   s+=i;
   if (gROOT->GetListOfCanvases()->FindObject(s)) delete fCanvas[i];
   fCanvas[i]=0;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExplorePanel()
{
/**
~~~
// Main internal steering routine for the ExplorePanel GUI.
// Invokation of this memberfunction will open an interactive GUI
// to provide a user friendly interface to this NcEvent instance
// to explore the event contents.
~~~
**/

 if (gROOT->IsBatch())
 {
  printf(" *%-s::ExplorePanel* ExplorePanel() can not be invoked in batch mode. \n",ClassName());
  return;
 }

 // Re-invokation of the ExplorePanel
 if (fExpPanel)
 {
  ShowPanel();
  return;
 }

 // New initialization of the ExplorePanel
 NcVersion v;
 v.Data();
 printf(" ********************************************************************************* \n");
 printf(" *      Welcome to the %-s GUI facility to explore RNO-G event data      *\n",ClassName());
 printf(" * After window closure, the GUI panel can be re-opened via invoking ShowPanel() * \n");
 printf(" ********************************************************************************* \n");
 printf("\n");

 UInt_t border=5;
 fExpPanel=new TGMainFrame(gClient->GetRoot());
 fExpPanel->SetWindowName(ClassName());
 fExpPanel->Connect("CloseWindow()",ClassName(),this,"ExpClose()");
 // Define the various sub-frames and fill them with the various panels
 TGCompositeFrame* frames[4]={0,0,0,0};
 TGLayoutHints* layouts[4]={0,0,0,0};

 // The input file specification frame
 frames[0]=new TGCompositeFrame(fExpPanel,1,1,kHorizontalFrame|kSunkenFrame);
 layouts[0]=new TGLayoutHints(kLHintsExpandX,border,border,0,0);
 FilesPanel(frames[0]);

 // The event header info frame
 frames[1]=new TGCompositeFrame(fExpPanel,1,1,kHorizontalFrame|kSunkenFrame);
 layouts[1]=new TGLayoutHints(kLHintsExpandX,border,border,0,0);
 HeaderPanel(frames[1]);

 // The sampling selection frame
 frames[2]=new TGCompositeFrame(fExpPanel,1,1,kHorizontalFrame|kSunkenFrame);
 layouts[2]=new TGLayoutHints(kLHintsExpandX,border,border,0,0);
 SamplingsPanel(frames[2]);

 // The command panel frame
 frames[3]=new TGCompositeFrame(fExpPanel,1,1,kHorizontalFrame|kSunkenFrame);
 layouts[3]=new TGLayoutHints(kLHintsExpandX,border,border,0,0);
 CommandPanel(frames[3]);

 // Add all subframes to the mainframe
 for (Int_t i=0; i<4; i++)
 {
  if (frames[i]) fExpPanel->AddFrame(frames[i],layouts[i]);
 }

 ShowPanel();
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::FilesPanel(TGCompositeFrame* frame)
{
/**
~~~
// Internal memberfunction to render the input files GUI panel.
~~~
**/

 if (!frame) return;

 // The input file(s) specification
 TGGroupFrame* files=new TGGroupFrame(frame,"Input filename e.g. $(HOME)/my-data/*.rnopack",kHorizontalFrame);
 files->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(files);
 fFiles=new TGTextEntry(files);
 fFiles->SetAlignment(kTextLeft);
 fFiles->Resize(400,20);
 files->AddFrame(fFiles);

 TGTextButton* add=new TGTextButton(files,"AddFile");
 add->Connect("Clicked()",ClassName(),this,"FileAdd()");
 add->SetToolTipText("Add the specified file to the input data chain");
 TGLayoutHints* Ladd=new TGLayoutHints(kLHintsLeft,10,0,0,-5);
 files->AddFrame(add,Ladd);

 // The number of entries indicator
 TGGroupFrame* entries=new TGGroupFrame(frame,"# Entries",kHorizontalFrame);
 entries->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(entries);
 fNentries=new TGTextEntry(entries,"0");
 fNentries->SetAlignment(kTextRight);
 fNentries->Resize(90,20);
 entries->AddFrame(fNentries);

 // The panel for window c.q. session controls 
 TGGroupFrame* session=new TGGroupFrame(frame,"Window / Session",kHorizontalFrame);
 session->SetTitlePos(TGGroupFrame::kCenter);
 TGLayoutHints* Lsession=new TGLayoutHints(kLHintsRight,0,0,0,0);
 frame->AddFrame(session,Lsession);

 TGTextButton* close=new TGTextButton(session,"Close");
 close->Connect("Clicked()",ClassName(),this,"ExpClose()");
 close->SetToolTipText("Close this panel window");
 TGLayoutHints* Lclose=new TGLayoutHints(kLHintsCenterX,0,6,3,-5);
 session->AddFrame(close,Lclose);

 TGTextButton* exit=new TGTextButton(session,"Exit");
 exit->Connect("Clicked()",ClassName(),this,"ExpExit()");
 exit->SetToolTipText("Exit this ROOT session");
 TGLayoutHints* Lexit=new TGLayoutHints(kLHintsCenterX,6,0,3,-5);
 session->AddFrame(exit,Lexit);
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::FileAdd()
{
/**
~~~
// Add the specified input file(s).
~~~
**/

 if (!fFiles) return;

 // Expand the pathname of the specified filename
 TString filename=fFiles->GetText();
 filename=gSystem->ExpandPathName(filename.Data());

 printf("\n *** Adding file : %-s *** \n",filename.Data());

 Add(filename.Data());

 fIndexed=kFALSE;

 if (!fConnected)
 {
  SetBranchAddress("Events",&fEvt);

  if (GetBranch("Events")) fConnected=kTRUE;
 }

 Long64_t nen=GetEntries();
 if (nen)
 {
  GetEntry(0);
  fEntry->SetText("0");
  HeadEnter();
 }

 TString s="";
 s+=nen;
 fNentries->SetText(s.Data());
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::HeaderPanel(TGCompositeFrame* frame)
{
/**
~~~
// Internal memberfunction to render the event header info GUI panel.
~~~
**/

 if (!frame) return;

 // The entry specification
 TGGroupFrame* entry=new TGGroupFrame(frame,"Entry",kHorizontalFrame);
 entry->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(entry);
 fEntry=new TGTextEntry(entry);
 fEntry->SetAlignment(kTextRight);
 fEntry->Resize(90,20);
 entry->AddFrame(fEntry);

 TGTextButton* getentry=new TGTextButton(entry,"LoadEntry");
 getentry->Connect("Clicked()",ClassName(),this,"LoadEntry()");
 getentry->SetToolTipText("Load the specified entry (0=first)");
 TGLayoutHints* Lgetentry=new TGLayoutHints(kLHintsLeft,10,0,0,-5);
 entry->AddFrame(getentry,Lgetentry);

 // The event Date/Time
 TGGroupFrame* ts=new TGGroupFrame(frame,"Date/Time",kHorizontalFrame);
 ts->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(ts);
 fHead[0]=new TGTextEntry(ts);
 fHead[0]->SetAlignment(kTextRight);
 fHead[0]->Resize(225,20);
 ts->AddFrame(fHead[0]);

 // The station number
 TGGroupFrame* station=new TGGroupFrame(frame,"Station",kHorizontalFrame);
 station->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(station);
 fHead[1]=new TGTextEntry(station);
 fHead[1]->SetAlignment(kTextRight);
 fHead[1]->Resize(30,20);
 station->AddFrame(fHead[1]);

 // The run number
 TGGroupFrame* run=new TGGroupFrame(frame,"Run",kHorizontalFrame);
 run->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(run);
 fHead[2]=new TGTextEntry(run);
 fHead[2]->SetAlignment(kTextRight);
 fHead[2]->Resize(80,20);
 run->AddFrame(fHead[2]);

 // The event number
 TGGroupFrame* event=new TGGroupFrame(frame,"Event",kHorizontalFrame);
 event->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(event);
 fHead[3]=new TGTextEntry(event);
 fHead[3]->SetAlignment(kTextRight);
 fHead[3]->Resize(80,20);
 event->AddFrame(fHead[3]);

 TGTextButton* getevent=new TGTextButton(event,"LoadEvent");
 getevent->Connect("Clicked()",ClassName(),this,"LoadEvent()");
 getevent->SetToolTipText("Load the specified (run,event)");
 TGLayoutHints* Lgetevent=new TGLayoutHints(kLHintsLeft,10,0,0,-5);
 event->AddFrame(getevent,Lgetevent);
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::LoadEntry()
{
/**
~~~
// Load the specified entry.
~~~
**/

 TString s=fEntry->GetText();
 Int_t i=s.Atoi();

 if (GetEntry(i))
 {
  fEnt=i;
  HeadEnter();
  printf(" *** Loaded entry : %-i *** \n",fEnt);
 }
 else
 {
  printf("\n *** Non-existing entry number : %-i *** \n",i);
  s="";
  s+=fEnt;
  fEntry->SetText(s.Data());
  LoadEntry();
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::LoadEvent()
{
/**
~~~
// Load the specified entry.
~~~
**/

  // Build the indexing for fast (station,run,event) access
  // using the fact that fUniqueID=station+100*run.
  if (!fIndexed)
  {
   BuildIndex("fUniqueID","fEvent");
   fIndexed=kTRUE;
  }

 TString station=fHead[1]->GetText();
 TString run=fHead[2]->GetText();
 TString event=fHead[3]->GetText();

 Int_t ist=station.Atoi();
 Int_t irun=run.Atoi();
 Int_t ievt=event.Atoi();

 Int_t id=ist+100*irun;
 Int_t ien=GetEntryNumberWithIndex(id,ievt);

 TString entry="";
 if (ien>=0)
 {
  entry+=ien;
  fEntry->SetText(entry.Data());
  LoadEntry();
 }
 else
 {
  printf("\n *** Non-existing (station,run,event) specification : (%-i,%-i,%-i) *** \n",ist,irun,ievt);
  entry+=fEnt;
  fEntry->SetText(entry.Data());
  LoadEntry();
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::HeadEnter()
{
/**
~~~
// Load the event header data.
~~~
**/

 if (!fEvt)
 {
  printf("\n *** No RnoEvent structure loaded (yet). *** \n");
  return;
 }

 // Obtain the DAQ sampling rate
 fSample=0;
 NcDevice* dev=fEvt->GetDevice("DAQ");
 if (dev) fSample=dev->GetSignal("Sampling-rate");

 // The event timestamp
 TString text=fEvt->GetDayTimeString("UT",3);
 fHead[0]->SetText(text.Data());

 // The station
 TObjArray* arr=fEvt->GetDevices("RnoStation");
 dev=(NcDevice*)arr->At(0);
 fStation=dev->GetUniqueID();
 text="";
 if (dev) text+=fStation;
 fHead[1]->SetText(text.Data());

 // The run number
 text="";
 text+=fEvt->GetRunNumber();
 fHead[2]->SetText(text.Data());

 // The event number
 text="";
 text+=fEvt->GetEventNumber();
 fHead[3]->SetText(text.Data());
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::SamplingsPanel(TGCompositeFrame* frame)
{
/**
~~~
// Internal memberfunction to render the samplings selection GUI panel.
~~~
**/

 if (!frame) return;

 TGGroupFrame* samplings=new TGGroupFrame(frame,"Input selection",kHorizontalFrame);
 samplings->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(samplings);

 // The channel specification
 TGNumberEntryField* channel=new TGNumberEntryField(samplings,-1,-1,TGNumberFormat::kNESInteger);
 channel->SetToolTipText("Channel number (-1=all)");
 channel->Connect("TextChanged(const char*)",ClassName(),this,"ExpChannel(const char*)");
 channel->Resize(30,20);
 samplings->AddFrame(channel);

 // The selection box for the sampling mode
 TGComboBox* mode=new TGComboBox(samplings);
 mode->Connect("Selected(Int_t)",ClassName(),this,"ExpMode(Int_t)");
 mode->AddEntry("ADC(s)",1);
 mode->AddEntry("ADC(t)",2);
 mode->AddEntry("Store1",3);
 mode->AddEntry("Store2",4);
 mode->AddEntry("Store3",5);
 mode->Resize(70,20);
 TGLayoutHints* Lmode=new TGLayoutHints(kLHintsLeft,10,0,0,-5);
 samplings->AddFrame(mode,Lmode);
 mode->Select(1,kTRUE);

 TGGroupFrame* operations=new TGGroupFrame(frame,"Operation selection                Parameter values",kHorizontalFrame);
 operations->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(operations);

 // The selection box for the operations processing mode
 TGComboBox* opsmode=new TGComboBox(operations);
 opsmode->Connect("Selected(Int_t)",ClassName(),this,"ExpOpsMode(Int_t)");
 opsmode->AddEntry("None",1);
 opsmode->AddEntry("Convert ADC-->mV",2);
 opsmode->AddEntry("DFT Time-->Frequency",3);
 opsmode->AddEntry("DFT Frequency-->Time",4);
 opsmode->AddEntry("Add FilterBand [f1,f2]",5);
 opsmode->AddEntry("Show BandFilterKernel",6);
 opsmode->AddEntry("BandFilter",7);
 opsmode->AddEntry("Show MovingAvgKernel",8);
 opsmode->AddEntry("MovingAvgFilter",9);
 opsmode->AddEntry("Blocks of n samples",10);
 opsmode->AddEntry("Bayesian FPR nrms ntrig",11);
 opsmode->AddEntry("Bayesian FPR f(y) ntrig",12);
 opsmode->AddEntry("Subtract",13);
 opsmode->AddEntry("Correlate",14);
 opsmode->AddEntry("Get SNR",15);
 opsmode->Resize(170,20);
 operations->AddFrame(opsmode);
 opsmode->Select(1,kTRUE);
 ExpOpsMode(1);

 // The operations parameter1 specification
 fParams[0]=new TGTextEntry(operations);
 fParams[0]->SetAlignment(kTextRight);
 fParams[0]->Connect("TextChanged(const char*)",ClassName(),this,"ExpOpsPar1(const char*)");
 fParams[0]->Resize(50,20);
 operations->AddFrame(fParams[0]);

 // The operations parameter2 specification
 fParams[1]=new TGTextEntry(operations);
 fParams[1]->SetAlignment(kTextRight);
 fParams[1]->Connect("TextChanged(const char*)",ClassName(),this,"ExpOpsPar2(const char*)");
 fParams[1]->Resize(170,20);
 operations->AddFrame(fParams[1]);

 // The operations parameter2 specification
 fParams[2]=new TGTextEntry(operations);
 fParams[2]->SetAlignment(kTextRight);
 fParams[2]->Connect("TextChanged(const char*)",ClassName(),this,"ExpOpsPar3(const char*)");
 fParams[2]->Resize(45,20);
 operations->AddFrame(fParams[2]);

 TGGroupFrame* storedraw=new TGGroupFrame(frame,"Output Store&Draw",kHorizontalFrame);
 storedraw->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(storedraw);

 // The selection box for the storage/drawing execution mode
 TGComboBox* execmode=new TGComboBox(storedraw);
 execmode->Connect("Selected(Int_t)",ClassName(),this,"ExpExecMode(Int_t)");
 execmode->AddEntry("Store1",1);
 execmode->AddEntry("Store2",2);
 execmode->AddEntry("Store3",3);
 execmode->Resize(65,20);
 storedraw->AddFrame(execmode);
 execmode->Select(1,kTRUE);
 ExpExecMode(1);

 TGTextButton* execute=new TGTextButton(storedraw,"Exec");
 execute->Connect("Clicked()",ClassName(),this,"ExpExecute()");
 execute->SetToolTipText("Execute the selected actions for the requested channel(s)");
 TGLayoutHints* Lexecute=new TGLayoutHints(kLHintsLeft,10,0,0,-2);
 storedraw->AddFrame(execute,Lexecute);
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpChannel(const char* text)
{
/**
~~~
// Action on the channel entry field.
~~~
**/

 TString s=text;
 fChannel=s.Atoi();
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpMode(Int_t i)
{
/**
~~~
// Set the selected sampling mode.
~~~
**/

 TString s[5]={"ADC(s)","ADC(t)","Store1","Store2","Store3"};

 if (i<1 || i>5) return;

 fMode=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpFmV(const char* text)
{
/**
~~~
// Action on the ADC->mV conversion description entry field.
~~~
**/

 fFmV=text;

 // Convert f(y) into f(x) to comply with TF1 format
 fFmV.ReplaceAll("y","x");
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpOpsMode(Int_t i)
{
/**
~~~
// Set the selected mode for the operations treatment.
~~~
**/

 TString s[15]={"None","mV","DFT","IDFT","AddFilter","BKernel","BFilter","MKernel","MFilter","Bn","BBnrms","BBf","Sub","Corr","SNR"};

 if (i<1 || i>15) return;

 fOpsMode=s[i-1];

 if (!fParams[0]) return;

 // Clear all parameters and tooltip texts
 for (Int_t j=0; j<3; j++)
 {
  fOpsPars[j]="";
  fParams[j]->SetToolTipText("");
 }

 // Initialize the relevant operation parameter values
 if (fOpsMode=="Bn") // Fixed n sample blocks
 {
  fOpsPars[0]="64";
  fParams[0]->SetToolTipText("n value");
  fOpsPars[1]="Median";
  fParams[1]->SetToolTipText("Median, Mean or RMS to be used for averaging");
 }

 if (fOpsMode=="BBnrms") // Bayesian blocks with FPR nrms and ntrig
 {
  fOpsPars[0]="0.1";
  fOpsPars[1]="2";
  fOpsPars[2]="0";
  fParams[0]->SetToolTipText("FPR value");
  fParams[1]->SetToolTipText("nrms value for input y-error: err(y_i)=nrms*RMSdev[all y_i]");
  fParams[2]->SetToolTipText("ntrig value (0=no trigger)");
 }

 if (fOpsMode=="BBf") // Bayesian blocks with FPR f(y) and ntrig
 {
  fOpsPars[0]="0.1";
  fOpsPars[1]="sqrt(abs(y))";
  fOpsPars[2]="0";
  fParams[0]->SetToolTipText("FPR value");
  fParams[1]->SetToolTipText("TF1 expression for input y-error: err(y_i)=f(y_i)");
  fParams[2]->SetToolTipText("ntrig value (0=no trigger)");
 }

 if (fOpsMode=="mV") // ADC to mV conversion
 {
  fOpsPars[1]="ADC*2.5e3/(pow(2,12)-1)";
  fParams[1]->SetToolTipText("TF1 expression for ADC->mV conversion");
 }

 if (fOpsMode=="DFT") // DFT Time to Frequency conversion
 {
  fOpsPars[0]="dB";
  fOpsPars[1]="Hz";
  fParams[0]->SetToolTipText("Y-axis value: dB, AMP, PHID, PHIR, RE or IM");
  fParams[1]->SetToolTipText("X-axis value: Hz, k (index) or f (fraction of sampling rate)");
  fParams[2]->SetToolTipText("Set to 2 to obtain all N data points instead of the usual (N/2)+1");
 }

 if (fOpsMode=="IDFT") // Inverse DFT Frequency to Time conversion
 {
  fOpsPars[0]="n";
  fParams[0]->SetToolTipText("X-axis value:  n (index) or t (time in seconds)");
  fParams[2]->SetToolTipText("Set to 2 to obtain all N data points instead of the usual (N/2)+1");
 }

 if (fOpsMode=="Sub") // Subtraction of spectra
 {
  fOpsPars[0]="0";
  fOpsPars[1]="Store2";
  fParams[0]->SetToolTipText("Channel number (0-23) for the spectrum to be subtracted");
  fParams[1]->SetToolTipText("Storage to retrieve the spectrum to be subtracted");
 }

 if (fOpsMode=="Corr") // Correlation of spectra
 {
  fOpsPars[0]="0";
  fOpsPars[1]="Store2";
  fOpsPars[2]="GNCC";
  fParams[0]->SetToolTipText("Channel number (0-23) for the reference spectrum");
  fParams[1]->SetToolTipText("Storage to retrieve the reference spectrum for correlation");
  fParams[2]->SetToolTipText("Normalization mode : NONE, GNCC, NCC, ZNCC");
 }

 if (fOpsMode=="SNR") // Get the SNR of the distribution
 {
  fOpsPars[1]="Bipolar-Amplitudes";
  fParams[1]->SetToolTipText("Y-values are RMS, Power or Bipolar-Amplitudes");
 }

 if (fOpsMode=="AddFilter") // Add a band filter
 {
  fOpsPars[0]="f1";
  fOpsPars[1]="f2";
  fOpsPars[2]="Pass";
  fParams[0]->SetToolTipText("f1 in MHz (*=Reset all bands)");
  fParams[1]->SetToolTipText("f2 in MHz (*=unlimited)");
  fParams[2]->SetToolTipText("Pass or Reject");
 }

 if (fOpsMode=="BKernel") // Provide the BandFilter kernel
 {
  fOpsPars[0]="0.01";
  fOpsPars[1]="hisf";
  fOpsPars[2]="Hz";
  fParams[0]->SetToolTipText("Roll-off in fraction of the sampling frequency");
  fParams[1]->SetToolTipText("Provide time domain (hist) or frequency domain (hisf) histogram");
  fParams[2]->SetToolTipText("hisf: Hz or f (fraction of sampling rate) hist: n (index) or t (time in seconds)");
 }

 if (fOpsMode=="BFilter") // Perform the Band filtering
 {
  fOpsPars[0]="0.01";
  fOpsPars[1]="hist";
  fOpsPars[2]="n";
  fParams[0]->SetToolTipText("Roll-off in fraction of the sampling frequency");
  fParams[1]->SetToolTipText("Provide time domain (hist) or frequency domain (hisf) histogram");
  fParams[2]->SetToolTipText("hist: n (index) or t (time in seconds) hisf: Hz or f (fraction of sampling rate)");
 }

 if (fOpsMode=="MKernel") // Provide the Moving Average Filter kernel
 {
  fOpsPars[0]="2";
  fOpsPars[1]="hisf";
  fOpsPars[2]="Hz";
  fParams[0]->SetToolTipText("Noise reduction factor");
  fParams[1]->SetToolTipText("Provide time domain (hist) or frequency domain (hisf) histogram");
  fParams[2]->SetToolTipText("hisf: Hz or f (fraction of sampling rate) hist: n (index) or t (time in seconds)");
 }

 if (fOpsMode=="MFilter") // Perform the Moving Average filtering
 {
  fOpsPars[0]="2";
  fOpsPars[1]="hist";
  fOpsPars[2]="n";
  fParams[0]->SetToolTipText("Noise reduction factor");
  fParams[1]->SetToolTipText("Provide time domain (hist) or frequency domain (hisf) histogram");
  fParams[2]->SetToolTipText("hist: n (index) or t (time in seconds) hisf: Hz or f (fraction of sampling rate)");
 }

 // Indicate the initial parameter values in the corresponding text boxes
 // and convert the actual function variable name to "x" to comply with the TF1 format.
 for (Int_t j=0; j<3; j++)
 {
  fParams[j]->SetText(fOpsPars[j]);
  fOpsPars[j].ReplaceAll("y","x");
  fOpsPars[j].ReplaceAll("ADC","x");
  fOpsPars[j].ReplaceAll("adc","x");
 }
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpOpsPar1(const char* text)
{
/**
~~~
// Action on the operations parameter1 entry field.
~~~
**/

 fOpsPars[0]=text;
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpOpsPar2(const char* text)
{
/**
~~~
// Action on the operations parameter2 entry field.
~~~
**/

 fOpsPars[1]=text;

 // Convert the function variable name to "x" to comply with the TF1 format.
 fOpsPars[1].ReplaceAll("y","x");
 fOpsPars[1].ReplaceAll("ADC","x");
 fOpsPars[1].ReplaceAll("adc","x");
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpOpsPar3(const char* text)
{
/**
~~~
// Action on the operations parameter3 entry field.
~~~
**/

 fOpsPars[2]=text;
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpExecMode(Int_t i)
{
/**
~~~
// Set the selected mode Store/Draw treatment.
~~~
**/

 TString s[3]={"Store1","Store2","Store3"};

 if (i<1 || i>3) return;

 fExecMode=s[i-1];
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpExecute()
{
/**
~~~
// Execute the selected actions for the requested channel(s).
~~~
**/

 // Adding or Resetting of filter bands
 if (fOpsMode=="AddFilter")
 {
  if (fSample<=0)
  {
   printf("\n *** Unknown sampling frequency ==> Can't set Filter bands *** \n");
   for (Int_t i=0; i<24; i++)
   {
    fFilterBands[i].Set(0);
   }
   return;
  }

  Double_t f1=fOpsPars[0].Atof();
  Double_t f2=fOpsPars[1].Atof();

  if (fOpsPars[2]!="Pass" && fOpsPars[2]!="Reject")
  {
   printf("\n *** Invalid Filter mode specification : %-s *** \n",fOpsPars[2].Data());
   return; 
  }

  if (fOpsPars[0]!="*")
  {
   if ((!f1 && fOpsPars[0]!="0") || (!f2 && fOpsPars[1]!="*") || f1<0 || f2<0)
   {
    printf("\n *** Invalid Filter Band specification [%-s,%-s] *** \n",fOpsPars[0].Data(),fOpsPars[1].Data());
    return; 
   }
  }

  if (fOpsPars[0]=="0") f1=-1;
  if (fOpsPars[1]=="*") f2=-1;

  if (fOpsPars[2]=="Reject")
  {
   // Rejecting a LowPass is equivalent to applying a HighPass filter
   if (f1<0 && f2>0)
   {
    f1=f2;
    f2=-1;
   }
   // Rejecting a HighPass is equivalent to applying a LowPass filter
   else if (f1>0 && f2<0)
   {
    f2=f1;
    f1=-1;
   }
   // A normal BandReject filter
   else if (f1>0 && f2>0)
   {
    f1=-f1;
    f2=-f2;
   }
  }

  Int_t jstart=0;
  Int_t jstop=23;
  if (fChannel>=0)
  {
   jstart=fChannel;
   jstop=jstart;
  }

  for (Int_t i=jstart; i<=jstop; i++)
  {
   if (fOpsPars[0]=="*") // Resetting all filter bands
   {
    fFilterBands[i].Set(0);
    if (i==jstart) printf("\n *** All filter bands have been removed *** \n");
   }
   else // Add this filter band
   {
    Int_t n=fFilterBands[i].GetSize();
    fFilterBands[i].Set(n+2);
    fFilterBands[i].SetAt(f1*1e6/fSample,n);
    fFilterBands[i].SetAt(f2*1e6/fSample,n+1);

    if (i==jstart)
    {
     if (f1>0)
     {
      if (f2>0) printf("\n *** [%-g,%-g] MHz BandPass filter added",f1,f2);
      if (f2<0) printf("\n *** %-g MHz HighPass filter added",f1);
     }
     else if (f1<0)
     {
      if (f2<0) printf("\n *** [%-g,%-g] MHz BandReject filter added",fabs(f1),fabs(f2));
      if (f2>0) printf("\n *** %-g MHz LowPass filter added",f2);
     }
     else
     {
      printf("\n *** [%-g,%-g] MHz is nor a valid filter band *** \n",f1,f2);
      return;
     }
     if (fChannel>=0)
     {
      printf(" for channel %-i *** \n",fChannel);
     }
     else
     {
      printf(" for all channels *** \n");
     }
    }
   }
  } // End of loop over the channels
  return;
 } // End of AddFilter processing 

 TString str=fExecMode;
 str.ReplaceAll("Store","");
 Int_t jstore=str.Atoi();
 GetHistograms(jstore);
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::GetHistograms(Int_t jstore)
{
/**
~~~
// Obtain the requested output histogram(s) for storage selection i.
~~~
**/

 // Clear all histograms for this storage
 if (!fMode.Contains("Store"))
 {
  for (Int_t i=0; i<24; i++)
  { 
   fHistos[jstore-1][i].Reset();
  }
 }

 if (!fEvt || jstore<1 || jstore>3) return;

 TGraph Gin;  // Graph for input sampling
 TH1F Hin;    // Histogram for input sampling
 TH1F Hout;   // Resulting output histogram after performing an operation
 TGraph Gout; // Resulting output graph after performing an operation

 TString htitle;   // The histogram/graph title
 TString xtitle;   // The histogram/graph X-axis label
 TString ytitle;   // The histogram/graph Y-axis label
 TString str,str2; // Multifunctional strings
 Int_t npoints=0;
 Double_t x=0;
 Double_t y=0;
 Double_t xfirst=0;
 Double_t xlast=0;
 Double_t xmin=0;
 Double_t xmax=0;
 Int_t nbins=0;
 TAxis* axis=0;
 TH1F htemp;        // Multifunctional temporary histogram
 Double_t freq=0;   // Variable to allow choice of providing the sampling frequency (fSample)
 Bool_t draw=kTRUE; // Flag to activate drawing of the result

 // Interpretation of the various operations parameters
 Int_t n=fOpsPars[0].Atoi();       // The number of samples in case of fixed blocks
 Double_t fpr=fOpsPars[0].Atof();  // The False Positive Rate for Bayesian Block processing
 Double_t nrms=fOpsPars[1].Atof(); // The number of standard deviations for the Bayesian Block y-errors
 TString fyerr=fOpsPars[1];        // The function (in TF1 format) to describe the Bayesian Block y-errors
 Int_t ntrig=fOpsPars[2].Atoi();   // The trigger parameter for Bayesian Block processing
 if (fOpsMode=="mV") fFmV=fOpsPars[1];
 TF1 adc2mV("adc2mV",fFmV);
 TString DFTsel; // The output selection(s) for the Discrete Fourier Transform
 str=fMode;
 str.ReplaceAll("Store","");
 Int_t StoreIn=str.Atoi();  // The requested input storage
 str=fOpsPars[0];
 str.ReplaceAll("Store","");
 Int_t jch2=str.Atoi(); // The requested 2nd channel number for combinations of spectra
 str=fOpsPars[1];
 str.ReplaceAll("Store","");
 Int_t StoreIn2=str.Atoi(); // The requested 2nd input storage for combinations of spectra
 Double_t BW=fOpsPars[0].Atof(); // The filter band roll-off in fraction of the sampling frequency
 Double_t MW=fOpsPars[0].Atof(); // The moving average roll-off in the number of samples
 Int_t nkernel=0;                // The number of filter kernel points
 Float_t snr=0;   // The SNR value of spectra
 Float_t snrdb=0; // The SNR value in dB
 Int_t snrmode=4;
 if (fOpsPars[1]=="RMS" || fOpsPars[1]=="Power") snrmode=3;

 NcSample sample;

 Int_t jstart=0;
 Int_t jstop=23;
 if (fChannel>=0) // Single selected channel
 {
  jstart=fChannel;
  jstop=fChannel;
 }

 for (Int_t jch=jstart; jch<=jstop; jch++)
 {
  if (StoreIn>0 && StoreIn<4)
  {
   Hin=fHistos[StoreIn-1][jch];
  }
  else if (fMode.Contains("ADC"))
  {
   // Get the raw ADC sampling input graph
   Gin=fEvt->GetSamplingGraph(fStation,jch);

   // Create the raw ADC input histogram
   Gin.Sort();
   npoints=Gin.GetN();
   Gin.GetPoint(0,xfirst,y);
   Gin.GetPoint(npoints-1,xlast,y);
   xmin=xfirst-1;
   xmax=xlast;
   nbins=npoints;
   Hin.Reset();
   Hin.SetBins(nbins,xmin,xmax);
   if (fMode.Contains("(t)"))
   {
    Hin.SetBins(nbins,xmin/fSample,xmax/fSample);
   }

   for (Int_t i=0; i<npoints; i++)
   {
    Gin.GetPoint(i,x,y);
    // Perform the ADC->mV conversion if requested
    if (fMode.Contains("(t)")) x=x/fSample;
    Gin.SetPoint(i,x,y);
    Hin.SetBinContent(i+1,y);
   }

   // Set the corresponding title and axes labels
   htitle=Gin.GetTitle();
   xtitle="Sample";
   if (fMode.Contains("(t)")) xtitle="Time in seconds";
   ytitle="ADC";
   str.Form("%-s;%-s;%-s",htitle.Data(),xtitle.Data(),ytitle.Data());
   Gin.SetTitle(str);
   Hin.SetTitle(str);
   Hin.SetStats(kFALSE);
  }

  if (fMode.Contains("Store"))
  {
   // Create an input graph from the input histogram
   npoints=Hin.GetNbinsX();
   for (Int_t i=0; i<npoints; i++)
   {
    x=Hin.GetBinCenter(i+1);
    y=Hin.GetBinContent(i+1);
    Gin.SetPoint(i,x,y);
   }
  
   // Update the axes labels for the input graph 
   htitle=Hin.GetTitle();
   axis=Hin.GetXaxis();
   xtitle=axis->GetTitle();
   axis=Hin.GetYaxis();
   ytitle=axis->GetTitle();
   str.Form("%-s;%-s;%-s",htitle.Data(),xtitle.Data(),ytitle.Data());
   Gin.SetTitle(str);
  }

  // The requested operation for the selected input sampling(s)
  Hout=Hin;

  if (fOpsMode=="mV") // ADC->mV conversion
  {
   if (!(ytitle.Contains("ADC")) || ytitle.Contains("mV"))
   {
    printf("\n *** This is not an ADC sampling ==> No conversion performed *** \n");
    return;
   }
   else
   {
    npoints=Hin.GetNbinsX();
    for (Int_t i=0; i<npoints; i++)
    {
     x=Hin.GetBinCenter(i+1);
     y=Hin.GetBinContent(i+1);
     y=adc2mV.Eval(y);
     Hin.SetBinContent(i+1,y);
     Gin.SetPoint(i,x,y);
    }
    ytitle.Form("mV=[%-s]",(adc2mV.GetExpFormula("p")).Data());
    ytitle.ReplaceAll("x","ADC");
    str.Form("%-s;%-s;%-s",htitle.Data(),xtitle.Data(),ytitle.Data());
    Hin.SetTitle(str);
    Gin.SetTitle(str);

    Hout=Hin;
   }
  }

  if (fOpsMode=="DFT" || fOpsMode=="IDFT") // DFT operations
  {
   if (fOpsMode=="DFT" && !(ytitle.Contains("ADC")) && !(ytitle.Contains("mV")))
   {
    printf("\n *** This is not a time domain ==> No DFT performed *** \n");
    return;
   }
   else if (fOpsMode=="IDFT" && (StoreIn<1 || StoreIn>3))
   {
    printf("\n *** No Store%-i data available ==> No Inverse DFT performed *** \n",StoreIn);
    return;
   }
   else if (fOpsMode=="IDFT" && (ytitle.Contains("ADC") || ytitle.Contains("mV")))
   {
    printf("\n *** This is not a frequency domain ==> No inverse DFT performed *** \n");
    return;
   }
   else
   {
    DFTsel=fOpsPars[0];
    DFTsel+=" ";
    DFTsel+=fOpsPars[1];
    DFTsel+=" ";
    DFTsel+=fOpsPars[2];

    if (fOpsMode=="DFT") // DFT time->frequency
    {
     freq=0;
     if (DFTsel.Contains("Hz")) freq=fSample;
     fDSP.Load((TH1*)&Hin,freq);
     fDSP.Fourier("R2C",&Hout,DFTsel);
     fReArr[jstore-1][jch]=fDSP.GetData("RE out");
     fImArr[jstore-1][jch]=fDSP.GetData("IM out");
     fYtitles[jstore-1][jch]=ytitle;
    }
    if (fOpsMode=="IDFT") // Inverse DFT frequency->time
    {
     freq=0;
     if (DFTsel.Contains("t")) freq=fSample;
     fDSP.Load(&fReArr[StoreIn-1][jch],&fImArr[StoreIn-1][jch],freq);
     fDSP.Fourier("C2R",&Hout,DFTsel);
     axis=Hout.GetYaxis();
     str=fYtitles[StoreIn-1][jch];
     axis->SetTitle(str);
    }

    htitle=Hin.GetTitle();
    axis=Hout.GetXaxis();
    xtitle=axis->GetTitle();
    axis=Hout.GetYaxis();
    ytitle=axis->GetTitle();
    str.Form("%-s;%-s;%-s",htitle.Data(),xtitle.Data(),ytitle.Data());
    Hout.SetTitle(htitle);
   }
  }

  if (fOpsMode=="Sub") // Subtraction of spectra
  {
   axis=Hin.GetXaxis();
   str=axis->GetTitle();
   axis=fHistos[StoreIn2-1][jch2].GetXaxis();
   str2=axis->GetTitle();
   if (str.Contains(str2) || str2.Contains(str))
   {
    fBB.Add(&Hin,&fHistos[StoreIn2-1][jch2],&Hout,kFALSE,-1);
   }
   else
   {
    printf("\n *** Units on the X-axes don't match ==> No action performed *** \n");
    return;
   }
  }

  if (fOpsMode=="Corr") // Correlation of spectra
  {
   axis=Hin.GetXaxis();
   str=axis->GetTitle();
   freq=0;
   if (str.Contains("Time")) freq=fSample;
   fDSP.Load((TH1*)&Hin,freq);
   // Ensure identical binning for the data and the search pattern
   fBB.Rebin(&fHistos[StoreIn2-1][jch2],&htemp,kFALSE);
   fDSP.SetWaveform((TH1*)&htemp);
   fDSP.Correlate(&Hout,0,0,0,fOpsPars[2]);
  }

  if (fOpsMode=="SNR") // Provide the SNR of the distribution(s)
  {
   sample.Load((TH1*)&Hin);
   snr=sample.GetSNR(1,snrmode,kFALSE);
   if (snr>0) snrdb=10.*log10(snr);
   if (jch==jstart) printf("\n");
   printf(" *** Ch%-i in Store%-i : SNR=%-g (%-g dB) *** \n",jch,StoreIn,snr,snrdb);
   draw=kFALSE;
  }

  if (fOpsMode=="BKernel") // Provide the multiband filter kernel
  {
   if (!fFilterBands[jch].GetSize())
   {
    if (jch==jstart) printf("\n");
    printf(" *** No FilterBands specified for channel %-i *** \n",jch);
    if (fChannel>=0) return;
    continue;
   }
   nkernel=0;
   if (BW>0) nkernel=4./BW;
   freq=0;
   if (fOpsPars[2]=="Hz" || fOpsPars[2]=="t") freq=fSample;
   fDSP.SetSamplingFrequency(freq);
   if (fOpsPars[1]=="hisf")
   {
    fDSP.GetMultiBandKernel(fFilterBands[jch],nkernel,&Hout);
    fReArr[jstore-1][jch]=fDSP.GetData("RE out");
    fImArr[jstore-1][jch]=fDSP.GetData("IM out");
   }
   else
   {
    fDSP.GetMultiBandKernel(fFilterBands[jch],nkernel,0,0,&Hout);
   }
  }

  if (fOpsMode=="BFilter") // Perform the multiband filtering
  {
   if (!(ytitle.Contains("ADC")) && !(ytitle.Contains("mV")))
   {
    printf("\n *** This is not a time domain ==> No filtering performed *** \n");
    return;
   }
   if (!fFilterBands[jch].GetSize())
   {
    if (jch==jstart) printf("\n");
    printf(" *** No FilterBands specified for channel %-i *** \n",jch);
    if (fChannel>=0) return;
    continue;
   }
   nkernel=0;
   if (BW>0) nkernel=4./BW;
   freq=0;
   if (fOpsPars[2]=="Hz" || fOpsPars[2]=="t") freq=fSample;
   fDSP.Load((TH1*)&Hin,freq);
   if (fOpsPars[1]=="hisf")
   {
    fDSP.FilterMultiBand(fFilterBands[jch],nkernel,&Hout,kTRUE);
    fReArr[jstore-1][jch]=fDSP.GetData("RE out");
    fImArr[jstore-1][jch]=fDSP.GetData("IM out");
   }
   else
   {
    fDSP.FilterMultiBand(fFilterBands[jch],nkernel,0,kTRUE,&Hout);
    axis=Hout.GetYaxis();
    axis->SetTitle(ytitle);
   }
  }

  if (fOpsMode=="MKernel") // Provide the moving average filter kernel
  {
   if (MW<1)
   {
    if (jch==jstart) printf("\n");
    printf(" *** Invalid noise reduction factor : %-g *** \n",MW);
    if (fChannel>=0) return;
    continue;
   }
   nkernel=TMath::Nint(pow(MW,2));
   freq=0;
   if (fOpsPars[2]=="Hz" || fOpsPars[2]=="t") freq=fSample;
   fDSP.SetSamplingFrequency(freq);
   if (fOpsPars[1]=="hisf")
   {
    fDSP.GetMovingAverageKernel(nkernel,&Hout);
    fReArr[jstore-1][jch]=fDSP.GetData("RE out");
    fImArr[jstore-1][jch]=fDSP.GetData("IM out");
   }
   else
   {
    fDSP.GetMovingAverageKernel(nkernel,0,0,&Hout);
   }
  }

  if (fOpsMode=="MFilter") // Perform the moving avrage filtering
  {
   if (!(ytitle.Contains("ADC")) && !(ytitle.Contains("mV")))
   {
    printf("\n *** This is not a time domain ==> No filtering performed *** \n");
    return;
   }
   if (MW<1)
   {
    if (jch==jstart) printf("\n");
    printf(" *** Invalid noise reduction factor : %-g *** \n",MW);
    if (fChannel>=0) return;
    continue;
   }
   nkernel=TMath::Nint(pow(MW,2));
   freq=0;
   if (fOpsPars[2]=="Hz" || fOpsPars[2]=="t") freq=fSample;
   fDSP.Load((TH1*)&Hin,freq);
   if (fOpsPars[1]=="hisf")
   {
    fDSP.FilterMovingAverage(nkernel,"conv",0,0,0,&Hout,kTRUE);
    fReArr[jstore-1][jch]=fDSP.GetData("RE out");
    fImArr[jstore-1][jch]=fDSP.GetData("IM out");
   }
   else
   {
    fDSP.FilterMovingAverage(nkernel,"conv",&Hout);
    axis=Hout.GetYaxis();
    axis->SetTitle(ytitle);
   }
  }

  if (fOpsMode=="Bn" || fOpsMode=="BBnrms" || fOpsMode=="BBf") // Blocked samplings
  {
   if (fOpsMode=="Bn") // Fixed size blocks
   {
    if (fOpsPars[1]=="Mean") fBB.GetBlocks(&Gin,&Hout,n,0);
    if (fOpsPars[1]=="Median") fBB.GetBlocks(&Gin,&Hout,n,1);
    if (fOpsPars[1]=="RMS") fBB.GetBlocks(&Gin,&Hout,n,2);
    str.Form("%-s Fixed Blocks grouped in %-i consecutive samples",htitle.Data(),n);
   }
   else // Bayesian blocks
   {
    if (jch==jstart) printf("\n *** Bayesian Blocks processing in progress .... \n");

    if (fOpsMode=="BBnrms") // Bayesian Blocks with nrms errors
    {
     fBB.GetBlocks(Gin,nrms,fpr,&Hout,ntrig);
     str.Form("%-s Bayesian Blocks with FPR=%-.3g and nrms=%-.3g",htitle.Data(),fpr,nrms);
    }
    if (fOpsMode=="BBf") // Bayesian Blocks with f(y_i) value errors
    {
     fBB.GetBlocks(Gin,fyerr,fpr,&Hout,ntrig);
     str.Form("%-s Bayesian Blocks with FPR=%-.3g and input errors: %-s",htitle.Data(),fpr,fyerr.Data());
     str.ReplaceAll("x","y");
    }

    if (jch==jstart) printf("\n *** Bayesian Blocks processing completed *** \n");
   }

   // Update the title and axes labels for the output histogram
   str+=";Blocked samplings";
   if (xtitle.Contains("seconds")) str+=" in seconds";
   if (xtitle.Contains("Hz")) str+=" in Hz";
   if (fOpsMode==3 || fOpsMode==4)
   {
    str+=" (ntrig=";
    str+=ntrig;
    str+=")";
   }
   str+=";";
   axis=Hout.GetYaxis();
   str2=axis->GetTitle();
   str+=str2;
   Hout.SetTitle(str);
   Hout.SetStats(kFALSE);
  }

  // Store the resulting histogram
  fHistos[jstore-1][jch]=Hout;

  // Construct the output TGraph from the output histogram
  npoints=Hout.GetNbinsX();
  for (Int_t i=0; i<npoints; i++)
  {
   x=Hout.GetBinCenter(i+1);
   y=Hout.GetBinContent(i+1);
   Gout.SetPoint(i,x,y);
  }
  htitle=Hout.GetTitle();
  axis=Hout.GetXaxis();
  xtitle=axis->GetTitle();
  axis=Hout.GetYaxis();
  ytitle=axis->GetTitle();
  str.Form("%-s;%-s;%-s",htitle.Data(),xtitle.Data(),ytitle.Data());
  Gout.SetTitle(str);
 } // End of loop over the channels

 // Draw the result(s)
 if (draw) DrawResult(jstore);
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::DrawResult(Int_t jstore)
{
/**
~~~
// Draw the result of the selected operation(s) for the requested channel(s).
~~~
**/

 if (!fEvt) return;

 ////////////////////////////////////////////////////////
 // Create a new canvas for (modified) raw DAQ spectra //
 ////////////////////////////////////////////////////////
 TString cname=ClassName();
 cname+=jstore;
 if (fCanvas[jstore-1])
 {
  if (gROOT->GetListOfCanvases()->FindObject(cname)) delete fCanvas[jstore-1];
  fCanvas[jstore-1]=0;
 }

 Int_t topx=0;
 Int_t topy=0;
 if (jstore==2) topx=636;
 if (jstore==3)
 {
  topx=318;
  topy=150;
 }
 fCanvas[jstore-1]=new TCanvas("cx","cx",topx,topy,635,450);
 fCanvas[jstore-1]->SetName(cname);

 // The initial part of the canvas title
 TString ctitle;
 ctitle.Form("Store%-i: ",jstore);

 TVirtualPad* pad=0;

 if (fChannel<0) fCanvas[jstore-1]->Divide(4,6);

 // Update the canvas title
 ctitle+="Station";
 ctitle+=fStation;
 if (fChannel>=0) // Add Channel number in case of selected channel
 {
  ctitle+=" Ch";
  ctitle+=fChannel;
 }

 Int_t jstart=0;
 Int_t jstop=23;
 if (fChannel>=0) // Single selected channel
 {
  jstart=fChannel;
  jstop=fChannel;
 }

 for (Int_t jch=jstart; jch<=jstop; jch++)
 {
  if (fChannel<0) // All channels were requested
  {
   pad=fCanvas[jstore-1]->cd(jch+1);

   if (!pad) continue;

   pad->SetGrid();
  }
  else // Single channel was selected 
  {
   fCanvas[jstore-1]->SetGrid();
  }

  // Update the canvas title in case an operation was performed
  if (jch==jstart)
  {
   if (fOpsMode=="None") ctitle+=" ADC";
   if (fOpsMode=="mV") ctitle+=" mV";
   if (fOpsMode=="DFT") ctitle+=" DFT";
   if (fOpsMode=="IDFT") ctitle+=" Inverse DFT";
   if (fOpsMode=="Bn") ctitle+=" Fixed Blocks";
   if (fOpsMode=="BBnrms") ctitle+=" Bayesian Blocks with nrms errors";
   if (fOpsMode=="BBf") ctitle+=" Bayesian Blocks with f(y_i) errors";
   if (fOpsMode=="Sub") ctitle+=" Subtraction";
   if (fOpsMode=="Corr") ctitle+=" Correlation";
   if (fOpsMode=="BKernel") ctitle+=" Band filter kernel";
   if (fOpsMode=="BFilter") ctitle+=" Band filter result";
   if (fOpsMode=="MKernel") ctitle+=" Moving average filter kernel";
   if (fOpsMode=="MFilter") ctitle+=" Moving average filter result";
  }

  fCanvas[jstore-1]->SetTitle(ctitle);

  fHistos[jstore-1][jch].DrawCopy();
  fCanvas[jstore-1]->Update();
 } // End of loop over the channels
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::CommandPanel(TGCompositeFrame* frame)
{
/**
~~~
// Internal memberfunction to render the command GUI panel.
~~~
**/

 if (!frame) return;

 // Event listing controls 
 TGGroupFrame* listings=new TGGroupFrame(frame,"Event Listings",kHorizontalFrame);
 listings->SetTitlePos(TGGroupFrame::kCenter);
 frame->AddFrame(listings);

 // The event header
 TGTextButton* date=new TGTextButton(listings,"Date/Time");
 date->Connect("Clicked()",ClassName(),this,"ListDate()");
 date->SetToolTipText("List the detailed event date/time info");
 listings->AddFrame(date);

 TGTextButton* daq=new TGTextButton(listings,"DAQ");
 daq->Connect("Clicked()",ClassName(),this,"ListDAQ()");
 daq->SetToolTipText("Listing of the DAQ status");
 TGLayoutHints* Ldaq=new TGLayoutHints(kLHintsLeft,5,0,0,-5);
 listings->AddFrame(daq,Ldaq);

 TGTextButton* triggers=new TGTextButton(listings,"Triggers");
 triggers->Connect("Clicked()",ClassName(),this,"ListTriggers()");
 triggers->SetToolTipText("Listing of the event trigger data");
 TGLayoutHints* Ltriggers=new TGLayoutHints(kLHintsLeft,5,0,0,-5);
 listings->AddFrame(triggers,Ltriggers);

 TGTextButton* tags=new TGTextButton(listings,"Tags");
 tags->Connect("Clicked()",ClassName(),this,"ListTags()");
 tags->SetToolTipText("Listing of the event tag data");
 TGLayoutHints* Ltags=new TGLayoutHints(kLHintsLeft,5,0,0,-5);
 listings->AddFrame(tags,Ltags);

 // The device specification for the listing
 TGTextEntry* devname=new TGTextEntry(listings,"*");
 devname->SetAlignment(kTextRight);
 devname->Connect("TextChanged(const char*)",ClassName(),this,"ExpDevName(const char*)");
 devname->SetToolTipText("Device (class) name (*=all)");
 devname->Resize(80,20);
 TGLayoutHints* Ldevname=new TGLayoutHints(kLHintsLeft,5,0,0,-5);
 listings->AddFrame(devname,Ldevname);

 TGTextButton* devs=new TGTextButton(listings,"Device(s)");
 devs->Connect("Clicked()",ClassName(),this,"ListDevices()");
 devs->SetToolTipText("List the selected device (class)");
 TGLayoutHints* Ldevs=new TGLayoutHints(kLHintsLeft,5,0,0,-5);
 listings->AddFrame(devs,Ldevs);
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpDevName(const char* text)
{
/**
~~~
// Action on the device (class) name entry field.
~~~
**/

 fDevName=text;
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::ListDate()
{
/**
~~~
// List the detailed event date/time info.
~~~
**/

 if (!fEvt)
 {
  printf("\n *** No RnoEvent structure loaded (yet). *** \n");
  return;
 }

 // Get the detailed date/time info for this event via NcAstrolab
 Int_t mjd,sec,ns,ps;
 fEvt->GetMJD(mjd,sec,ns);
 ps=fEvt->GetPs();

 TString station=fHead[1]->GetText();
 Int_t ist=station.Atoi();

 NcAstrolab lab;
 lab.SetRandomiser(0);
 lab.SetExperiment("RNO-G",ist);
 lab.SetMJD(mjd,sec,ns,ps,"A");

 printf("\n");
 lab.Data(3);
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::ListDevices()
{
/**
~~~
// List the devices.
~~~
**/

 if (!fEvt)
 {
  printf("\n *** No RnoEvent structure loaded (yet). *** \n");
  return;
 }

 printf("\n");
 if (fDevName.Contains("*") || fDevName.Contains("Rno")) // Device class name requested
 {
  fEvt->ShowDevices(fDevName,2);
 }
 else // Specific device requested
 {
  NcDevice* dx=fEvt->GetDevice(fDevName);
  if (dx)
  {
   if (dx->InheritsFrom("NcDetectorUnit"))
   {
    ((NcDetectorUnit*)dx)->ShowDevices(2);
   }
   else
   {
    dx->Data();
   }
  }
  else
  {
   printf("\n *** Device %-s not found. *** \n",fDevName.Data());
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::ListDAQ()
{
/**
~~~
// List the event DAQ status.
~~~
**/

 if (!fEvt)
 {
  printf("\n *** No RnoEvent structure loaded (yet). *** \n");
  return;
 }

 NcDevice* dx=fEvt->GetDevice("DAQ");

 if (dx)
 {
  printf("\n");
  dx->Data();
 }
 else
 {
  printf("\n *** No DAQ device present. *** \n");
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::ListTriggers()
{
/**
~~~
// List the event Trigger data.
~~~
**/

 if (!fEvt)
 {
  printf("\n *** No RnoEvent structure loaded (yet). *** \n");
  return;
 }

 NcDevice* dx=fEvt->GetDevice("Trigger");

 if (dx)
 {
  printf("\n");
  dx->Data();
 }
 else
 {
  printf("\n *** No Trigger device present. *** \n");
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::ListTags()
{
/**
~~~
// List the event Filter data.
~~~
**/

 if (!fEvt)
 {
  printf("\n *** No RnoEvent structure loaded (yet). *** \n");
  return;
 }

 NcDevice* dx=fEvt->GetDevice("Filter");

 if (dx)
 {
  printf("\n");
  dx->Data();
 }
 else
 {
  printf("\n *** No Tag/Filter device present. *** \n");
 }
}
///////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpClose()
{
/**
~~~
// Close the ExplorePanel window but do NOT exit the current ROOT session.
~~~
**/

 // De-activate all automatic CloseWindow() actions of the system window manager
 // in order to fully control it in this function 
 fExpPanel->DontCallClose();

 // To prevent crash when the cursor is still left active in a TextEntry
 fExpPanel->RequestFocus();

 // Unmap the display window
 fExpPanel->UnmapWindow();
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ExpExit()
{
/**
~~~
// Exit this ROOT session.
~~~
**/

 fExpPanel->RequestFocus();
 fExpPanel->Cleanup();
 gApplication->Terminate(0);
}
///////////////////////////////////////////////////////////////////////////////
void RnoExplorer::ShowPanel()
{
/**
~~~
// Show the GUI panel again after the window was closed.
~~~
**/

  // Map all subwindows of main frame
  fExpPanel->MapSubwindows();

  // Initialize the layout algorithm
  fExpPanel->Resize(fExpPanel->GetDefaultSize());

  // Map main frame
  fExpPanel->MapWindow();
}
///////////////////////////////////////////////////////////////////////////////
RnoEvent* RnoExplorer::GetEvent()
{
/**
~~~
// Provide a pointer to the event structure.
~~~
**/

 return fEvt;
}
///////////////////////////////////////////////////////////////////////////////
