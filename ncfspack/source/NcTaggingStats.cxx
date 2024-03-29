/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2022, NCFS/IIHE, All Rights Reserved.                     *
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
// Class NcTaggingStats
// TTask derived class to perform statistics of event classification tags.
//  
// This class is the event-by-event processor version of NcDataStreamStats, and has to be
// run as a sub-task within an NcJob environment that performs event-by-event analysis. 
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
// This class provides a processor to investigate the tag settings on an event-by-event
// basis and provide an overview of the various event tags that were encountered
// and the corresponding event rates.
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
// The examples below show how to configure an NcTaggingStats object, that is meant to be
// run as a sub-task in an NcJob (derived) environment for an event-by-event analysis.
//
// Example 1:
// ==========
//
// // This example is part of an analysis of IceCube data contained in IceEvent structures.
// // Since these are rather old files, they contain generic NcDevice objects to handle
// // the tags, since the class NcTagger was not yet available at the time of data recording.
// // The "pass" and "write" tags were named "condition" and "prescale", respectively.
//
// // Initialisation to perform an analysis of IceCube event filter statistics
// NcTaggingStats* fstat=new NcTaggingStats("FilterStats","IceCube event filter statistics");
//
// // Specify that we will access the NcDevice named "Filter"
// // and that the "pass" indicator is called "condition"
// // and the "write" indicator is called "prescale"
// fstat->SetDeviceNames("Filter","condition","prescale");
//
// // The statement below shows the alternative for a "Trigger" analysis
// // where the trigger tags do not have "pass" nor "write" indicators.
// // fstat->SetDeviceNames("Trigger","*","*");
//
// // Activate some tags to investigate re-tagging for a selected event sample
// fstat->ActivateTag("GFU");         // Gamma ray Follow Up stream
// fstat->ActivateTag("HESE");        // High-Energy Starting Event stream
// fstat->ActivateTag("EstresAlert"); // Enhanced Starting track alert stream
// fstat->ActivateTag("EHEAlert");    // Extreme High Energy event alert stream
//
// // De-activate some tags to investigate re-tagging for background reduction
// // This can be used to de-activate tag names that were activated above
// // because of a matching name pattern.
// fstat->DeactivateTag("EHEAlertFilterHB"); // Extreme High Energy alert Heart Beat stream
//
//
// Example 2:
// ==========
//
// // This example is part of an analysis of RNO-G data contained in RnoEvent structures.
// // At data conversion (see RnoConvert), an NcTagger (derived) object was introduced
// // and given the name "Trigger".
//
// // Initialisation to perform an analysis of RNO-G event trigger statistics
// NcTaggingStats* trigstat=new NcTaggingStats("TriggerStats","RNO-G trigger statistics");
//
// // Specify that we will access the NcTagger (derived) device named "Trigger"
// trigstat->SetDevice("Trigger");
//
// // Activate some tags to investigate re-tagging for a selected event sample
// trigstat->ActivateTag("radiant"); // RADIANT trigger
// trigstat->ActivateTag("lt");      // Low threshold trigger
//
// // De-activate some tags to investigate re-tagging for background reduction
// // This can be used to de-activate tag names that were activated above
// // because of a matching name pattern.
// trigstat->DeactivateTag("radiant_surface"); // Surface trigger (fictative)
//
// Once the user has configured the NcTaggingStats object, it has to be added as a (sub)task
// into the NcJob (derived) environment for the event-by-event analysis.
// The final tagging statistics are obtained by invoking ShowStatistics() after the
// NcJob (based) processing has completed.
//
// Templates for a pure NcJob and an NcJob derived event-by-event task processing
// can be found in /rnopack/macros/analyze.cc and /rnopack/macros/convert.cc, respectively.
//
//--- Author: Nick van Eijndhoven 18-jul-2022, IIHE-VUB, Brussel
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, December 22, 2022  01:30Z
///////////////////////////////////////////////////////////////////////////

#include "NcTaggingStats.h"
#include "Riostream.h"
 
ClassImp(NcTaggingStats) // Class implementation to enable ROOT I/O
 
NcTaggingStats::NcTaggingStats(const char* name,const char* title) : NcTaggingBase(name,title)
{
// Constructor and initialisation of default parameters.
}
///////////////////////////////////////////////////////////////////////////
NcTaggingStats::~NcTaggingStats()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingStats::Exec(Option_t* opt)
{
// Perform the tagging analysis.

 Int_t nact=0;
 Int_t ndeact=0;
 if (fAct) nact=fAct->GetEntries();
 if (fDeact) ndeact=fDeact->GetEntries();

 // Some intialisations at the first invokation only
 if (fFirst)
 {
  fFirst=kFALSE;

  TString title;
  title=fPassname;
  if (title=="*") title="Passing";
  title+=" statistics";
  fPass.SetNameTitle(fDevname,title);
  fApass.Set(fMaxtag);
  fMpass.ResizeTo(fMaxtag,fMaxtag);
  fMpass.Zero();
  title=fWritename;
  if (title=="*") title="Writing";
  title+=" statistics";
  fWrites.SetNameTitle(fDevname,title);
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
  fCombis.SetNameTitle(fDevname,title);
  fAcombis.Set(fMaxtag);
  fMcombis.ResizeTo(fMaxtag,fMaxtag);
  fMcombis.Zero();

  cout << endl;
  cout << " *" << ClassName() << "::Exec* Processor parameter settings." << endl;
  cout << " Processor name  : " << GetName() << endl;
  cout << " Processor title : " << GetTitle() << endl;

  if (fDevname=="none")
  {
   cout << " *** No device name for the event tags was provided. ***" << endl;
  }
  else
  {
   cout << " Tag device name : " << fDevname << endl;
   cout << " Pass indicator  : " << fPassname << endl;
   cout << " Write indicator : " << fWritename << endl;
   cout << endl;
  }
 }

 TString name=opt;
 NcJob* parent=(NcJob*)(gROOT->GetListOfTasks()->FindObject(name));

 if (!parent) return;

 fEvt=(NcEvent*)parent->GetObject("NcEvent");
 if (!fEvt) return;

 // Do not process rejected events
 Int_t select=fEvt->GetSelectLevel();
 if (select<0) return;

 // Reset the various tagging flags for this event
 fApass.Reset();
 fAwrites.Reset();
 fAcombis.Reset();

 NcDevice* tagdev=(NcDevice*)fEvt->GetDevice(fDevname);
 if (!tagdev) return;

 Int_t ntags=tagdev->GetNhits();
 if (ntags>fMaxtag) ntags=fMaxtag;

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

 Int_t ipass=0;
 Int_t iwrite=0;
 Int_t idx=0;
 Int_t combflag=0;
 TString namex="";
 Int_t jdeact=0;
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

  jdeact=0;
  if (nact) jdeact=1; // The user has specified the tags to be activated

  // Look for a user selection of activated tags
  for (Int_t i=0; i<nact; i++)
  {
   TObjString* sox=(TObjString*)fAct->At(i);
   if (!sox) continue;
   namex=sox->GetString();
   if (name.Contains(namex))
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
   if (name.Contains(namex))
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

 Int_t ival1=0;
 Int_t ival2=0;
 Int_t ival3=0;
 Int_t ival4=0;
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
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingStats::ShowStatistics()
{
// Provide The final statistics.

 ListStatistics();
}
///////////////////////////////////////////////////////////////////////////
