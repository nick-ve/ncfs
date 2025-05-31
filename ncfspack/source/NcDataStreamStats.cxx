/**  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
~~~
 * Copyright(c) 2028 NCFS/IIHE, All Rights Reserved.                           *
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
~~~
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///////////////////////////////////////////////////////////////////////////
/** @class NcDataStreamStats
~~~
// Class NcDataStreamStats
// TTask derived class to perform statistics of event classification tags.
//
// During data taking or online/offline processing one usually records
// various tags for each event to indicate which trigger or filter criteria
// were fulfilled. This also allows to provide different data streams. 
//
// It is common that a certain tag has the following two indicators
// pass  : Indicates whether the tag criteria have been fulfilled (1) or not (0)
// write : Indicates whether the event is actually labeled to be written (1) or not (0)
//
// In case of high-frequency tag streams it is common that not every event
// will be written to storage in order to limit the data volume.
// In the Data Acquisition (DAQ) system this is implemented by a downscaling
// of the corresponding data stream via so-called prescaler settings.
// The effect is that although for a certain tag the passing criteria have
// been fulfilled (i.e. the "pass" indicator is set), the "write" indicator is not always set,
// but for instance only after every 1000 instances of an activated "pass" indicator.
// This results in a downscale factor of 1000 for that tag stream.
//
// This class provides a processor to analyse data samples and provide an overview
// of the various event tags that were encountered and the corresponding event rates.
//
// In addition to this, it offers also the possibility to tailor a re-tagging procedure,
// of which the results are provided next to the regular tag statistics.
// Via the memberfunctions ActivateTag() and DeactivateTag() the user can (de)activate
// certain tags in order to perform a re-tagging to investigate the effect in view of
// defining specific event samples or background reduction.
// In case none of these memberfunctions ActivateTag() or DeactivateTag() are invoked,
// all tags are regarded to be active in the re-tagging procedure. 
// In case DeactivateTag() is invoked, all tags that are not explicitly activated by the user
// will be considered de-activated in the re-tagging procedure.
// It should be noted that the tags which are explicitly de-activated via DeactivateTag()
// can not be activated anymore via invokation of ActivateTag().
// Please refer to the docs of these memberfunctions for further details.
//
// The input data is specified via the memberfunction AddInputFile().
// All data files that contain NcEvent (or derived) data structures are allowed.
//
// In order to obtain the required data, the NcEvent (or derived) structure should
// contain an NcDevice (based) object with the corresponding name (e.g. Trigger, Filter,...).
//
// The following two varieties of such an NcDevice (based) object may be present :
//
// 1) NcTagger (derived) devices. (The recommended option)
//    These devices have pre-defined memberfunctions and an internal data structure
//    which is tailored for the handling of event tags.
//    Specification of the device to be investigated is performed via invokation of
//    the memberfunction SetDevice().
//
// 2) A generic NcDevice. (Not recommended; Only for backward compatibility with old data files)
//    For a generic NcDevice object to serve as a tagging device, the user has to create a device 
//    in which each tag is stored as a Hit (=NcSignal) with the corresponding tag name (e.g. "LowPt").
//    Each Hit has to have at least 2 signal slots, each with the appropriate name to indicate
//    the "pass" indicator value and the "write" indicator value.
//    Instead of the names "pass" and "write", various other naming conventions
//    (e.g. "condition" and "prescale") are often used.
//    The name of the required device (e.g. "Filter") and the naming conventions of the
//    "pass" indicator and "write" indicator (e.g. "condition" and "prescale", respectively)
//    are specified via the memberfunction SetDeviceNames().
//
// Once the user has provided all necessary information, the analysis is performed
// by invoking the statement "ExecuteTask()" as indicated in the example below.   
//  
// This class is a generalisation of the original trigger-stats.cc and filter-stats.cc
// ROOT macros of Nick van Eijndhoven (IIHE-VUB, Brussel, 09-dec-2009).
//
// Note : The class NcTaggingStats provides a similar facility that can be invoked
//        as a (sub)task in an event-by-event task processing environment, which in some
//        cases might provide more flexibility.
//
// Example 1:
// ==========
//
// // This example performs an analysis of IceCube data contained as IceEvent structures
// // in *.icepack files within a Branch "IceEvent" of a Tree "T".
// // Since these are rather old files, they contain generic NcDevice objects to handle
// // the tags, since the class NcTagger was not yet available at the time of data recording.
// // The "pass" and "write" tags were named "condition" and "prescale", respectively.
//
// // Load the necessary libraries
// gSystem->Load("ncfspack");
// gSystem->Load("icepack");
//
// // Initialisation to perform an analysis of IceCube event filter statistics
// NcDataStreamStats fstat("FilterStats","IceCube event filter statistics");
//
// fstat.AddInputFile("*.icepack","T","IceEvent");
//
// fstat.ListInputFiles();
//
// // Provide a progress output line every 1000 events 
// fstat.SetPrintFrequency(1000);
//
// // Specify that we will access the NcDevice named "Filter"
// // and that the "pass" indicator is called "condition"
// // and the "write" indicator is called "prescale"
// fstat.SetDeviceNames("Filter","condition","prescale");
//
// // The statement below shows the alternative for a "Trigger" analysis
// // where the trigger tags do not have "pass" nor "write" indicators.
// // fstat.SetDeviceNames("Trigger","*","*");
//
// // Activate some tags to investigate re-tagging for a selected event sample
// fstat.ActivateTag("GFU");         // Gamma ray Follow Up stream
// fstat.ActivateTag("HESE");        // High-Energy Starting Event stream
// fstat.ActivateTag("EstresAlert"); // Enhanced Starting track alert stream
// fstat.ActivateTag("EHEAlert");    // Extreme High Energy event alert stream
//
// // De-activate some tags to investigate re-tagging for background reduction
// // This can also be used to de-activate tag names that were activated above
// // because of a matching name pattern.
// fstat.DeactivateTag("EHEAlertFilterHB"); // Extreme High Energy alert Heart Beat stream
//
// // Perform the analysis.
// fstat.ExecuteTask();
//
// Example 2:
// ==========
//
// // This example performs an analysis of RNO-G data contained as RnoEvent structures
// // in *.rnopack files within a Branch "Events" of a Tree "T".
// // At data recording, an NcTagger (derived) object was used and given the name "Trigger".
//
// // Load the necessary libraries
// gSystem->Load("ncfspack");
// gSystem->Load("rnopack");
//
// // Initialisation to perform an analysis of RNO-G event filter statistics
// NcDataStreamStats trigstat("TriggerStats","RNO-G trigger statistics");
//
// trigstat.AddInputFile("*.rnopack","T","Events");
//
// trigstat.ListInputFiles();
//
// // Provide a progress output line every 1000 events 
// trigstat.SetPrintFrequency(1000);
//
// // Specify that we will access the NcTagger (derived) device named "Trigger"
// trigstat.SetDevice("Trigger");
//
// // Activate some tags to investigate re-tagging for a selected event sample
// trigstat.ActivateTag("radiant"); // RADIANT trigger
// trigstat.ActivateTag("lt");      // Low threshold trigger
//
// // De-activate some tags to investigate re-tagging for background reduction
// // This can be used to de-activate tag names that were activated above
// // because of a matching name pattern.
// trigstat.DeactivateTag("radiant_surface"); // Surface trigger (fictative)
//
// // Perform the analysis.
// trigstat.ExecuteTask();
//
//--- Author: Nick van Eijndhoven 15-jun-2018, IIHE-VUB, Brussel
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, December 21, 2022  12:49Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "NcDataStreamStats.h"
#include "Riostream.h"
 
ClassImp(NcDataStreamStats); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
NcDataStreamStats::NcDataStreamStats(const char* name,const char* title) : NcTaggingBase(name,title)
{
/**
~~~
// Constructor and initialisation of default parameters.
~~~
**/

 fData=0;
 fBranchname="none";
 fMaxevt=0;
 fPfreq=0;
}
///////////////////////////////////////////////////////////////////////////
NcDataStreamStats::~NcDataStreamStats()
{
/**
~~~
// Default destructor.
~~~
**/

 if (fData)
 {
  delete fData;
  fData=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcDataStreamStats::NcDataStreamStats(const NcDataStreamStats& q) : NcTaggingBase(q)
{
/**
~~~
// Copy constructor.
~~~
**/

 fData=0;
 if (q.fData) fData=(TChain*)(q.fData->Clone());
 fBranchname=q.fBranchname;
 fMaxevt=q.fMaxevt;
 fPfreq=q.fPfreq;
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::Exec(Option_t* opt)
{
/**
~~~
// Perform the analysis.
// The input argument "opt" is irrelevant.
~~~
**/

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

 // Branch in the tree for the event input
 fData->SetBranchAddress(fBranchname.Data(),&fEvt);

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
   cout << " === Tag names (*=wildcard) that are activated by the user for re-tagged event sample studies ===" << endl;
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
   cout << " === Tag names (*=wildcard) that are de-activated by the user for re-tagged background reduction studies ===" << endl;
   cout << " The de-activated tags are flagged as \"dead\" in the combined \"Passing*Writing\" listing." << endl;
   cout << " However, they are still shown in the corresponding tag matrix to identify the missed tags." << endl; 
   cout << endl;
  }
  TObjString* tagx=(TObjString*)fDeact->At(i);
  if (!tagx) continue;

  cout << " ... *" << tagx->GetString() << "*" << endl;
 }
 cout << endl;

 fNevt=0;     // Number of events that were tagged
 fNevtComb=0; // Number of user reduced events via selected re-tagging

 TString title;
 title=fPassname;
 if (title=="*") title="Passing";
 title+=" statistics";
 fPass.SetNameTitle(fDevname.Data(),title.Data());
 fApass.Set(fMaxtag);
 fMpass.ResizeTo(fMaxtag,fMaxtag);
 fMpass.Zero();
 title=fWritename;
 if (title=="*") title="Writing";
 title+=" statistics";
 fWrites.SetNameTitle(fDevname.Data(),title.Data());
 fAwrites.Set(fMaxtag);
 fMwrites.ResizeTo(fMaxtag,fMaxtag);
 fMwrites.Zero();
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
 fCombis.SetNameTitle(fDevname.Data(),title.Data());
 fAcombis.Set(fMaxtag);
 fMcombis.ResizeTo(fMaxtag,fMaxtag);
 fMcombis.Zero();

 Int_t select=0;
 Int_t ntags=0;
 Int_t jrun=0;
 Int_t jevt=0;
 Int_t perc=0;
 TString name;
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
  if (!fEvt) continue;

  // Do not process rejected events
  select=fEvt->GetSelectLevel();
  if (select<0) continue;

  ntags=0;
  NcDevice* tagdev=(NcDevice*)fEvt->GetDevice(fDevname);
  if (!tagdev) continue;

  ntags=tagdev->GetNhits();
  if (ntags>fMaxtag) ntags=fMaxtag;

  fApass.Reset();
  fAwrites.Reset();
  fAcombis.Reset();

  jrun=fEvt->GetRunNumber();
  jevt=fEvt->GetEventNumber();

  // Determine the first and last recorded event
  if (!fEvtFirst) fEvtFirst=new NcEvent(*fEvt);
  if (!fEvtLast) fEvtLast=new NcEvent(*fEvt);
  if (fEvtFirst->GetDifference(fEvt,"s")<0)
  {
   delete fEvtFirst;
   fEvtFirst=new NcEvent(*fEvt);
  }
  if (fEvtLast->GetDifference(fEvt,"s")>0)
  {
   delete fEvtLast;
   fEvtLast=new NcEvent(*fEvt);
  }

  fNevt++;

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
   fPass.AddNamedSlot(name);
   fWrites.AddNamedSlot(name);
   fCombis.AddNamedSlot(name);
   fPass.AddSignal(ipass,name);
   idx=fPass.GetSlotIndex(name);
   if (idx && idx<=fMaxtag) fApass.AddAt(ipass,idx-1);
   fWrites.AddSignal(iwrite,name);
   idx=fWrites.GetSlotIndex(name);
   if (idx && idx<=fMaxtag) fAwrites.AddAt(iwrite,idx-1);
   fCombis.AddSignal(ipass*iwrite,name);
   idx=fCombis.GetSlotIndex(name);
   if (idx && idx<=fMaxtag) fAcombis.AddAt(ipass*iwrite,idx-1);

   ////////////////////////////////////////////////////////////////////////
   // (De)activate some tags for the combined pass*write statistics      //
   // which will enable to study event samples or background reduction   //
   ////////////////////////////////////////////////////////////////////////

   namex="";
   jdeact=0;
   if (nact) jdeact=1; // The user has specified the tags to be activated

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
    fCombis.SetDead(name);
   }
   else
   {
    if (ipass*iwrite) combflag=1;
   }
  } // End of tag loop

  if (combflag) fNevtComb++;

  // Update the various tag matrices for this event
  ntags=fPass.GetNslots();
  if (ntags>fMaxtag) ntags=fMaxtag;

  for (Int_t irow=0; irow<ntags; irow++)
  {
   ival1=fApass.At(irow);
   ival2=fAwrites.At(irow);
   ival3=fAcombis.At(irow);
   if (!ival1 && !ival2 && !ival3) continue;
   for (Int_t icol=0; icol<ntags; icol++)
   {
    ival4=fApass.At(icol);
    if (ival1 && ival4) fMpass(irow,icol)+=float(ival4);
    ival4=fAwrites.At(icol);
    if (ival2 && ival4) fMwrites(irow,icol)+=float(ival4);
    ival4=fAcombis.At(icol);
    if (ival3 && ival4) fMcombis(irow,icol)+=float(ival4);
   }
  }  

  if (fPfreq>0 && (!(ient%fPfreq) || (ient+1)==nen))
  {
   perc=100*(ient+1)/nen;
   cout << " *** Processed input entry : " << ient << " run : " << jrun << " event : " << jevt << " (" << perc << "%)" << endl;
  }

 } // End of event loop

 // List the final statistics overview
 ListStatistics();
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::AddInputFile(TString file,TString tree,TString branch)
{
/**
~~~
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
//
// 1) Environment variables may be used as $(...) in the filenname for convenience.
//    For example "$(HOME)/my-data/sample.ncfspack".
// 2) This memberfunction may be invoked several times to accumulate data files.
// 3) The "tree" name may vary for different input files, but the "branch" name
//    has to be the same for all the input files.
~~~
**/

 // Expand the path name of the provided input file
 file=gSystem->ExpandPathName(file.Data());

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
/**
~~~
// Set the maximum number of events to be processed.
// The default constructor has set n=0 which means no limitation.
~~~
**/

 fMaxevt=n;
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::SetPrintFrequency(Int_t m)
{
/**
~~~
// Set print frequency to provide a progress output line every "m" events.
// The default constructor has set m=0 which means that no progress printout is produced.
~~~
**/

 fPfreq=m;
}
///////////////////////////////////////////////////////////////////////////
void NcDataStreamStats::ListInputFiles(Option_t* opt) const
{
/**
~~~
// Provide an overview of the various input files.
// The input argument "opt" corresponds to the available options of TChain::Print().
// By default opt="".
~~~
**/

 if (fData)
 {
  cout << endl;
  fData->Print(opt);
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* NcDataStreamStats::Clone(const char* name) const
{
/**
~~~
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers when adding objects in case the container owns the objects.
~~~
**/

 NcDataStreamStats* fstat=new NcDataStreamStats(*this);
 if (name)
 {
  if (strlen(name)) fstat->SetName(name);
 }
 return fstat;
}
///////////////////////////////////////////////////////////////////////////
