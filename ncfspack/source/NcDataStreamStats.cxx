/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.                     *
 *                                                                             *
 * Authors: The Netherlands Center for Fundamental Studies (NCFS).             *
 *          The Inter-university Institute for High Energies (IIHE).           *                 
 *                    Website : http://www.iihe.ac.be                          *
 *            Contact : Nick van Eijndhoven (nickve.nl@gmail.com)              *
 *                                                                             *
 * Contributors are mentioned in the code where appropriate.                   *
 *                                                                             * 
 * No part of this software may be used, copied, modified or distributed       *
 * by any means nor transmitted or translated into machine language for        *
 * commercial purposes without written permission by the IIHE representative.  *
 * Permission to use the software strictly for non-commercial purposes         *
 * is hereby granted without fee, provided that the above copyright notice     *
 * appears in all copies and that both the copyright notice and this           *
 * permission notice appear in the supporting documentation.                   *
 * This software is provided "as is" without express or implied warranty.      *
 * The authors make no claims that this software is free of error, or is       *
 * consistent with any particular standard of merchantability, or that it      *
 * will meet your requirements for any particular application, other than      *
 * indicated in the corresponding documentation.                               *
 * This software should not be relied on for solving a problem whose           *
 * incorrect solution could result in injury to a person or loss of property.  *
 * If you do use this software in such a manner, it is at your own risk.       *
 * The authors disclaim all liability for direct or consequential damage       *
 * resulting from your use of this software.                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///////////////////////////////////////////////////////////////////////////
// Class NcDataStreamStats
// TTask derived class to perform statistics of event classification tags.
//
// During data taking or online/offline processing one usually records
// various tags for each event to indicate which trigger or filter criteria
// were fulfilled. This also allows to provide different data streams. 
//
// It is common that a certain tag has the following two indicators
// pass  : This means that the tag criteria have been fulfilled
// write : This means that the event is actually labeled to be written
//
// In case of high-frequency tag streams it is common that not every event
// will be written to storage in order to limit the data volume.
// In the DAQ system this is implemented by a downscaling of the corresponding
// data stream via so-called prescaler settings.
// The effect is that although for a certain tag the passing criteria have
// been fulfilled (i.e. the "pass" flag is set), the "write" flag is not always set,
// but for instance only after every 1000 instances of an activated "pass" flag.
// This results in a downscale factor of 1000 for that tag stream.
//
// This class provides a processor to analyse data samples and provide an overview
// of the various event tags that were encountered and the corresponding event rates.
// Via the memberfunctions ActivateTag() and DeactivateTag() the user can (de)activate
// certain tags in order to investigate the effect in view of defining event samples
// or background reduction.
// In case none of these memberfunctions ActivateTag() or DeactivateTag() are invoked,
// all tags are regarded to be active. 
// The user selected re-tagging results are provided in addition to the regular tag statistics.
// It should be noted that tags that are explicitly de-activated via DeactivateTag()
// can not be activated anymore via invokation of ActivateTag().
// Please refer to the docs of these memberfunctions for further details.
//
// The input data is specified via the memberfunction AddInputFile().
// All data files that contain NcEvent (or derived) data structures are allowed.
// In order to obtain the required data, the NcEvent (or derived) structure should
// contain a NcDevice with the corresponding name (e.g. DAQ, Trigger, Filter,...) and
// in which each tag is stored as a Hit (=NcSignal) with the corresponding tag name (e.g. "LowPt").
// Each Hit has to have at least 2 signal slots, each with the appropriate name to indicate
// the "pass flag" value and the "write flag" value.
// Instead of the names "pass" and "write", various other naming conventions
// (e.g. "condition" and "prescale") are often used.
// The name of the required device (e.g. "Filter") and the naming conventions of the
// "pass flag" and "write flag" (e.g. "condition" and "prescale", respectively) are
// specified via the memberfunction SetDeviceNames().
//
// Once the user has provided all necessary information, the analysis is performed
// by invoking the statement "ExecuteTask()" as indicated in the example below.   
//  
// This class is a generalisation of the original trigger-stats.cc and filter-stats.cc
// ROOT macros of Nick van Eijndhoven (IIHE-VUB, Brussel, 09-dec-2009). 
//
// Example :
// =========
//
// // This example performs an analysis of IceCube data
// // contained as IceEvent structures in *.icepack files
// // within a Branch "IceEvent" of a Tree "T".
//
// // Load the necessary libraries
// gSystem->Load("ncfspack");
// gSystem->Load("icepack");
//
// // Initialisation to perform an analysis of IceCube event filter statistics
// NcDataStreamStats fstat;
//
// fstat.AddInputFile("*.icepack","T","IceEvent");
//
// fstat.ListInputFiles();
//
// // Activate some tags to investigate a certan event sample
// fstat.ActivateTag("GFU");
// fstat.ActivateTag("HESE");
// fstat.ActivateTag("EstresAlert");
// fstat.ActivateTag("EHEAlert");
//
// // De-activate some tags to investigate background reduction
// fstat.DeactivateTag("EHEAlertFilterHB");
//
// // Provide a progress output line every 1000 events 
// fstat.SetPrintFrequency(1000);
//
// // Specify that we will access the NcDevice named "Filter"
// // and that the "pass flag" is called "condition"
// // and the "write flag" is called "prescale"
// fstat.SetDeviceNames("Filter","condition","prescale");
//
// // The statement below shows the alternative for a "Trigger" analysis
// // where the trigger tags do not have "pass" nor "write" indicators.
// // fstat.SetDeviceNames("Trigger","*","*");
//
// // Perform the analysis.
// fstat.ExecuteTask();
//
//
//--- Author: Nick van Eijndhoven 15-jun-2018, IIHE-VUB, Brussel
//
///////////////////////////////////////////////////////////////////////////

#include "NcDataStreamStats.h"
#include "Riostream.h"
 
ClassImp(NcDataStreamStats) // Class implementation to enable ROOT I/O
 
NcDataStreamStats::NcDataStreamStats(const char* name,const char* title) : TTask(name,title)
{
// Constructor and initialisation of default parameters.

 fData=0;
 fBranchname="none";
 fMaxevt=0;
 fMaxtag=99;
 fPfreq=0;
 fDevname="none";
 fPassname="*";
 fWritename="*";
 fAct=0;
 fDeact=0;
}
///////////////////////////////////////////////////////////////////////////
NcDataStreamStats::~NcDataStreamStats()
{
// Default destructor.

 if (fData)
 {
  delete fData;
  fData=0;
 }
 if (fAct)
 {
  delete fAct;
  fAct=0;
 }
 if (fDeact)
 {
  delete fDeact;
  fDeact=0;
 }

 // Remove the subtasks from the internal TTask list without deleting them
 if (fTasks) fTasks->Clear();

}
///////////////////////////////////////////////////////////////////////////
NcDataStreamStats::NcDataStreamStats(const NcDataStreamStats& q) : TTask(q)
{
// Copy constructor

 fData=0;
 if (q.fData) fData=(TChain*)(q.fData->Clone());
 fBranchname=q.fBranchname;
 fMaxevt=q.fMaxevt;
 fMaxtag=q.fMaxtag;
 fPfreq=q.fPfreq;
 fDevname=q.fDevname;
 fPassname=q.fPassname;
 fWritename=q.fWritename;
 fAct=0;
 if (q.fAct) fAct=(TObjArray*)(q.fAct->Clone());
 fDeact=0;
 if (q.fDeact) fDeact=(TObjArray*)(q.fDeact->Clone());
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::Exec(Option_t* opt)
{
// Perform the analysis.
// The input argument "opt" is irrelevant.

 if (!fData)
 {
  cout << endl;
  cout << " *NcDataStreamStats::Exec* No input data provided." << endl;
  return;
 }

 if (fDevname=="")
 {
  cout << endl;
  cout << " *NcDataStreamStats::Exec* No device name for the event tags provided." << endl;
  return;
 }

 cout << endl;
 cout << " *** Device and indicator identifiers ***" << endl;
 cout << " Tag device name : " << fDevname << endl;
 cout << " Pass indicator  : " << fPassname << endl;
 cout << " Write indicator : " << fWritename << endl;

 // Define a pointer for an event
 NcEvent* evt=0;

 // Branch in the tree for the event input
 fData->SetBranchAddress(fBranchname.Data(),&evt);

 Int_t nen=(int)fData->GetEntries();
 cout << endl;
 cout << " Number of entries available : " << nen << endl;
 cout << endl;

 if (fMaxevt && fMaxevt<nen) nen=fMaxevt;
 cout << " *** Number of entries to be processed : " << nen << endl;

 Int_t nact=0;
 Int_t ndeact=0;
 if (fAct) nact=fAct->GetEntries();
 if (fDeact) ndeact=fDeact->GetEntries();
 for (Int_t i=0; i<nact; i++)
 {
  if (!i)
  {
   cout << endl;
   cout << " === Tag names (*=wildcard) that are activated by the user for event sample studies ===" << endl;
   cout << " The non-activated tags are flagged as \"dead\" in the combined \"Passing*Writing\" listing." << endl;
   cout << " However, they are still shown in the corresponding tag matrix to identify the missed tags." << endl; 
   cout << endl;
  }
  TObjString* tagx=(TObjString*)fAct->At(i);
  if (!tagx) continue;

  cout << " ... *" << tagx->GetString() << "*" << endl;
 }
 for (Int_t i=0; i<ndeact; i++)
 {
  if (!i)
  {
   cout << endl;
   cout << " === Tag names (*=wildcard) that are de-activated by the user for background reduction studies ===" << endl;
   cout << " The de-activated tags are flagged as \"dead\" in the combined \"Passing*Writing\" listing." << endl;
   cout << " However, they are still shown in the corresponding tag matrix to identify the missed tags." << endl; 
   cout << endl;
  }
  TObjString* tagx=(TObjString*)fDeact->At(i);
  if (!tagx) continue;

  cout << " ... *" << tagx->GetString() << "*" << endl;
 }
 cout << endl;

 Int_t nevt=0;     // Number of events that were tagged
 Int_t nevtcomb=0; // Number of user reduced events via selected re-tagging

 NcSignal pass;
 TString title;
 title=fPassname;
 if (title=="*") title="Passing";
 title+=" statistics";
 pass.SetNameTitle(fDevname.Data(),title.Data());
 TArrayI* apass=new TArrayI(fMaxtag);
 TMatrixF* mpass=new TMatrixF(fMaxtag,fMaxtag);
 NcSignal writes;
 title=fWritename;
 if (title=="*") title="Writing";
 title+=" statistics";
 writes.SetNameTitle(fDevname.Data(),title.Data());
 TArrayI* awrites=new TArrayI(fMaxtag);
 TMatrixF* mwrites=new TMatrixF(fMaxtag,fMaxtag);
 NcSignal combis;
 title="Combined ";
 if (nact || ndeact) title="User reduced combined ";
 if (fPassname!="*" && fWritename!="*")
 {
  title+=fPassname;
  title+="*";
  title+=fWritename;
 }
 else
 {
  title+="Passing*Writing";
 }
 title+=" statistics"; 
 combis.SetNameTitle(fDevname.Data(),title.Data());
 TArrayI* acombis=new TArrayI(fMaxtag);
 TMatrixF* mcombis=new TMatrixF(fMaxtag,fMaxtag);

 Int_t ntags=0;
 Int_t jrun=0;
 Int_t jevt=0;
 Int_t perc=0;
 TString name;
 NcEvent* first=0;
 NcEvent* last=0;
 Int_t ipass=0;
 Int_t iwrite=0;
 Int_t combflag=0;
 Int_t idx=0;
 Int_t ival1=0;
 Int_t ival2=0;
 Int_t ival3=0;
 Int_t ival4=0;

 TString namex;
 Int_t jdeact=0;

 for (Int_t ient=0; ient<nen; ient++)
 {
  fData->GetEntry(ient);
  if (!evt) continue;

  ntags=0;
  NcDevice* tagdev=(NcDevice*)evt->GetDevice(fDevname);
  if (!tagdev) continue;

  ntags=tagdev->GetNhits();
  if (ntags>fMaxtag) ntags=fMaxtag;

  apass->Reset();
  awrites->Reset();
  acombis->Reset();

  jrun=evt->GetRunNumber();
  jevt=evt->GetEventNumber();

  // Determine the first and last recorded event
  if (!first) first=new NcEvent(*evt);
  if (!last) last=new NcEvent(*evt);
  if (first->GetDifference(evt,"s")<0)
  {
   delete first;
   first=new NcEvent(*evt);
  }
  if (last->GetDifference(evt,"s")>0)
  {
   delete last;
   last=new NcEvent(*evt);
  }

  nevt++;

  combflag=0;
  for (Int_t itag=1; itag<=ntags; itag++)
  {
   NcSignal* sx=tagdev->GetHit(itag);
   if (!sx) continue;
   name=sx->GetName();
   if (sx->GetSlotIndex(fPassname))
   {
    ipass=int(sx->GetSignal(fPassname)+0.1); // Flag to indicate if the tag passing criteria are satisfied (1) or not (0)
   }
   else
   {
    ipass=1;
   }
   if (sx->GetSlotIndex(fWritename))
   {
    iwrite=int(sx->GetSignal(fWritename)+0.1);   // Flag to indicate if this tag would write the event (1) or not (0)
   }
   else
   {
    iwrite=1;
   }

   // Update stats for this tag name
   pass.AddNamedSlot(name);
   writes.AddNamedSlot(name);
   combis.AddNamedSlot(name);
   pass.AddSignal(ipass,name);
   idx=pass.GetSlotIndex(name);
   if (idx && idx<=fMaxtag) apass->AddAt(ipass,idx-1);
   writes.AddSignal(iwrite,name);
   idx=writes.GetSlotIndex(name);
   if (idx && idx<=fMaxtag) awrites->AddAt(iwrite,idx-1);
   combis.AddSignal(ipass*iwrite,name);
   idx=combis.GetSlotIndex(name);
   if (idx && idx<=fMaxtag) acombis->AddAt(ipass*iwrite,idx-1);

   ////////////////////////////////////////////////////////////////////////
   // (De)activate some tags for the combined pass*write statistics      //
   // which will enable to study event samples or background reduction   //
   ////////////////////////////////////////////////////////////////////////

   namex="";
   jdeact=0;
   if (fAct) jdeact=1; // The user has specified the tags to be activated

   // Look for a user selection of activated tags
   for (Int_t i=0; i<nact; i++)
   {
    TObjString* sox=(TObjString*)fAct->At(i);
    if (!sox) continue;
    namex=sox->GetString();
    if (name.Contains(namex.Data()))
    {
     jdeact=0;
     break;
    }
   }
   // Look for a user selection of de-activated tags
   // This may de-activate a previously activated tag
   for (Int_t i=0; i<ndeact; i++)
   {
    TObjString* sox=(TObjString*)fDeact->At(i);
    if (!sox) continue;
    namex=sox->GetString();
    if (name.Contains(namex.Data()))
    {
     jdeact=1;
     break;
    }
   }

   if (jdeact)
   {
    combis.SetDead(name);
   }
   else
   {
    if (ipass*iwrite) combflag=1;
   }
  } // End of tag loop

  if (combflag) nevtcomb++;

  // Update the various tag matrices for this event
  ntags=pass.GetNslots();
  if (ntags>fMaxtag) ntags=fMaxtag;

  for (Int_t irow=0; irow<ntags; irow++)
  {
   ival1=apass->At(irow);
   ival2=awrites->At(irow);
   ival3=acombis->At(irow);
   if (!ival1 && !ival2 && !ival3) continue;
   for (Int_t icol=0; icol<ntags; icol++)
   {
    ival4=apass->At(icol);
    if (ival1 && ival4) (*mpass)(irow,icol)+=float(ival4);
    ival4=awrites->At(icol);
    if (ival2 && ival4) (*mwrites)(irow,icol)+=float(ival4);
    ival4=acombis->At(icol);
    if (ival3 && ival4) (*mcombis)(irow,icol)+=float(ival4);
   }
  }  

  if (fPfreq>0 && (!(ient%fPfreq) || (ient+1)==nen))
  {
   perc=100*(ient+1)/nen;
   cout << " *** Processed input entry : " << ient << " run : " << jrun << " event : " << jevt << " (" << perc << "%)" << endl;
  }

 } // End of event loop

 // The final statistics
 cout << endl;
 cout << " Total number of tagged events : " << nevt << endl;
 cout << endl;
 if (nevt)
 {
  cout << " ===== First tagged event =====" << endl;
  first->HeaderData();
  cout << endl;

  cout << " ===== Last tagged event =====" << endl;
  last->HeaderData();
  cout << endl;

  Double_t dt=first->GetDifference(last,"s");
  cout << " Elapsed timespan (sec.) : " << dt << endl;
  if (dt<=0) cout << " *** Incorrect timespan. No rates calculated. ***" << endl;

  // The various tagged event rates
  if (dt>0)
  {
   Double_t ratetot=double(nevt)/dt;
   cout << " Total tagged event rate (Hz) : " << ratetot << endl; 
   Double_t ratecombitot=double(nevtcomb)/dt;
   if (ndeact) cout << " User reduced event rate (Hz) : " << ratecombitot << endl;

   // Rate per tag channel
   Double_t ratecond=0;
   Double_t ratewrite=0;
   Double_t ratio=0;
   cout << endl;
   cout << " ===== Total tagging rates (Hz) for the various channels =====" << endl;
   for (Int_t i=1; i<=pass.GetNslots(); i++)
   {
    name=pass.GetSlotName(i);
    ratecond=pass.GetSignal(i)/dt;
    ratewrite=writes.GetSignal(i)/dt;
    ratio=1;
    if (ratecond>0) ratio=ratewrite/ratecond;
    cout << " " << name.Data() << " ... pass : " << ratecond << "  write : " << ratewrite << " ===> write/pass : " << ratio << endl;
   }
  }

  // Overview of the various tagging counts
  ntags=pass.GetNslots();
  if (ntags>fMaxtag) ntags=fMaxtag;

  Int_t ndigits1=log10(float(ntags))+1.;
  Int_t ndigits2=log10(float(nevt))+1.;

  cout << endl;
  cout << " ===== Total tag passing statistics =====" << endl;
  pass.Data();
  cout << endl;
  cout << " The tag passing matrix with empty slots (=rows/columns) suppressed : " << endl;
  cout << " -------------------------------------------------------------------- " << endl;
  cout << setfill(' ');
  for (Int_t irow=-1; irow<ntags; irow++)
  {
   // Skip empty rows to reduce printed matrix size
   if (irow>-1)
   {
    if (pass.GetSignal(irow+1)<1) continue;
   }
   cout << " " << setw(ndigits1) << (irow+1);
   for (Int_t icol=0; icol<ntags; icol++)
   {
    // Skip empty columns to reduce printed matrix size
    if (pass.GetSignal(icol+1)<1) continue;
    if (irow==-1)
    {
     ival1=icol+1;
    }
    else
    {
     ival1=int((*mpass)(irow,icol));
    }
    cout << " " << setw(ndigits2) << ival1;
   }
   cout << endl;
  }
  
  if (fWritename!="*")
  {
   cout << endl;
   cout << " ===== Total tag event writing (prescale) statistics =====" << endl;
   writes.Data();
   cout << endl;
   cout << " The tag event writing (prescale) matrix with empty slots (=rows/columns) suppressed : " << endl;
   cout << " ------------------------------------------------------------------------------------- " << endl;
   for (Int_t irow=-1; irow<ntags; irow++)
   {
    // Skip empty rows to reduce printed matrix size
    if (irow>-1)
    {
     if (writes.GetSignal(irow+1)<1) continue;
    }
    cout << " " << setw(ndigits1) << (irow+1);
    for (Int_t icol=0; icol<ntags; icol++)
    {
     // Skip empty columns to reduce printed matrix size
     if (writes.GetSignal(icol+1)<1) continue;
     if (irow==-1)
     {
      ival1=icol+1;
     }
     else
     {
      ival1=int((*mwrites)(irow,icol));
     }
     cout << " " << setw(ndigits2) << ival1;
    }
    cout << endl;
   }
  }

  cout << endl;
  cout << " =====  Combined tag passing and event writing (prescale) statistics     =====" << endl;
  cout << " ===== with user selected de-activations to study background reduction   =====" << endl;
  combis.Data();
  cout << endl;
  cout << " The combined tag passing and event writing (prescale) matrix with empty slots (=rows/columns) suppressed : " << endl;
  cout << " ---------------------------------------------------------------------------------------------------------- " << endl;
  for (Int_t irow=-1; irow<ntags; irow++)
  {
   // Skip empty rows to reduce printed matrix size
   if (irow>-1)
   {
    if (combis.GetSignal(irow+1)<1) continue;
   }
   cout << " " << setw(ndigits1) << (irow+1);
   for (Int_t icol=0; icol<ntags; icol++)
   {
    // Skip empty columns to reduce printed matrix size
    if (combis.GetSignal(icol+1)<1) continue;
    if (irow==-1)
    {
     ival1=icol+1;
    }
    else
    {
     ival1=int((*mcombis)(irow,icol));
    }
    cout << " " << setw(ndigits2) << ival1;
   }
   cout << endl;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::AddInputFile(TString file,TString tree,TString branch)
{
// Add an input file containing NcEvent (or derived) data structures to be analysed.
//
// Input arguments :
// -----------------
// file   : Name of the input file containing the ROOT Tree (wildcards are allowed)
// tree   : Name of the Tree containing the data
// branch : Name of the Branch containing the NcEvent (or derived) data structures
//
// Notes :
// -------
// 1) This memberfunction may be invoked several times to accumulate data files.
// 2) The "tree" name may vary for different input files, but the "branch" name
//    has to be the same for all the input files.

 // Initialise the master data Chain and the Branch name if needed
 if (!fData)
 {
  fData=new TChain("Data");
  fBranchname=branch;
 }

 TChain input(tree.Data());
 input.Add(file.Data());

 // Add the file(s) to the master data chain
 fData->Add(&input);
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::SetMaxEvt(Int_t n)
{
// Set the maximum number of events to be processed.
// The default constructor has set n=0 which means no limitation.
 fMaxevt=n;
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::SetMaxTag(Int_t n)
{
// Set the maximum number of tags to be processed.
// The default constructor has set n=99.
 fMaxtag=n;
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::SetPrintFrequency(Int_t m)
{
// Set print frequency to provide a progress output line every "m" events.
// The default constructor has set m=0 which means that no progress printout is produced.
 fPfreq=m;
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::SetDeviceNames(TString devname,TString passname,TString writename)
{
// Set the name of the NcDevice and the names of the tag passing criteria.
// It is common that a certain tag has the following two indicators
// pass  : This means that the tag criteria have been fulfilled
// write : This means that the event is actually labeled to be written
//
// In case of high-frequency tag streams it is common that not every event
// will be written to storage in order to limit the data volume.
// In the DAQ system this is implemented by a downscaling of the corresponding
// data stream via so-called prescaler settings.
// The effect is that although for a certain tag the passing criteria have
// been fulfilled (i.e. the "pass" flag is set), the "write" flag is not always set,
// but for instance only after every 1000 instances of an activated "pass" flag.
// This results in a downscale factor of 1000 for that tag stream.
//
// Instead of the names "pass" and "write", various other naming conventions
// (e.g. "condition" and "prescale") are often used.
// Via this memberfunction one can specify the names used by the current experiment.
//
// Note : In case passname="*" and/or writename="*", no check will be made for the
//        corresponding flag setting and a value of 1 will be assumed for both.

 fDevname=devname;
 fPassname=passname; 
 fWritename=writename; 
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::ListInputFiles(Option_t* opt) const
{
// Provide an overview of the various input files.
// The input argument "opt" corresponds to the available options of TChain::Print().
// By default opt="".

 if (fData)
 {
  cout << endl;
  fData->Print(opt);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::ActivateTag(TString name)
{
// Activate a certain tag for the combined pass*write statistics
// which will enable to study user selected event samples.
// It should be noted that this doesn't affect the results for the
// regular individual "pass" and "write" statistics.
//
// In case the specified "name" matches (part of) a certain tag name, that
// specific tag will be activated.
// All characters in "name" are taken literally, so wildcards are not allowed.   
// 
// Example :
// ---------
// If name="LowPt" it would activate both tags "LowPtMuon" and "LowPtPion".
//
// Notes :
// -------
// 1) If this memberfunction is not invoked, all tags are regarded as active.
// 2) If this memberfunction is invoked, all tags that are not explicitly activated
//    by the user will be considered de-activated.
// 3) This memberfunction may be invoked several times to activate
//    various tags before executing the task.
// 4) Tags that are explicitly de-activated by the user via invokation of DeactivateTag()
//    can not be activated anymore.

 if (!fAct)
 {
  fAct=new TObjArray();
  fAct->SetOwner();
 }
 TObjString* s=new TObjString();
 s->SetString(name.Data());
 fAct->Add(s);
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::DeactivateTag(TString name)
{
// De-activate a certain tag for the combined pass*write statistics
// which will enable to study background reduction.
// It should be noted that this doesn't affect the results for the
// regular individual "pass" and "write" statistics.
//
// In case the specified "name" matches (part of) a certain tag name, that
// specific tag will be de-activated.
// All characters in "name" are taken literally, so wildcards are not allowed.   
// 
// Example :
// ---------
// If name="LowPt" it would de-activate both tags "LowPtMuon" and "LowPtPion".
//
// Notes :
// -------
// 1) This memberfunction may be invoked several times to de-activate
//    various tags before executing the task.
// 3) Tags that are explicitly de-activated via this memberfunction
//    can not be activated anymore via invokation of ActivateTag().

 if (!fDeact)
 {
  fDeact=new TObjArray();
  fDeact->SetOwner();
 }
 TObjString* s=new TObjString();
 s->SetString(name.Data());
 fDeact->Add(s);
}
///////////////////////////////////////////////////////////////////////////
TObject* NcDataStreamStats::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers when adding objects in case the container owns the objects.

 NcDataStreamStats* fstat=new NcDataStreamStats(*this);
 if (name)
 {
  if (strlen(name)) fstat->SetName(name);
 }
 return fstat;
}
///////////////////////////////////////////////////////////////////////////
