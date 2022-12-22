/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 2022, NCFS/IIHE, All Rights Reserved.                          *
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
// Class NcTaggingSelector
// TTask derived class to perform event selection based on the event classification tag settings.
//  
// This class has to be run as a sub-task within an NcJob environment that performs
// event-by-event analysis. 
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
// basis and provide an event selection based on these tag settings.
// The resulting "select" or "reject" decision will be indicated in the corresponding
// NcEvent (or derived) structure via the NcEvent::SetSelectLevel() and is later on
// available via the NcEvent::GetSelectLevel() during subsequent data analysis.
//
// Via the memberfunctions ActivateTag() and DeactivateTag() the user can specify
// certain tags in order to tailor the selection criteria.
// At least one of the tags that have been specified via ActivateTag() needs to have
// the corresponding tag criteria fulfilled (i.e. the "pass" indicator set to 1)
// to mark the event as a candidate for selection.
// All tags that have been specified via DeactivateTag() need to have the corresponding
// tag criteria NOT fulfilled (i.e. the "pass" indicator set to 0) in order not to reject
// the candidate event.
// 
// In case none of these memberfunctions ActivateTag() or DeactivateTag() are invoked,
// no check on the event tag settings will be performed, and the event selection level
// will remain unchanged.
//
// It should be noted that the tags which are specified via DeactivateTag()
// can not be specified anymore via invokation of ActivateTag().
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
// The examples below show how to configure an NcTaggingSelector object, that is meant to be
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
// NcTaggingSelector* fsel=new NcTaggingSelector("FilterSel","IceCube event filter selection");
//
// // Specify that we will access the NcDevice named "Filter"
// // and that the "pass" indicator is called "condition"
// // and the "write" indicator is called "prescale"
// fsel->SetDeviceNames("Filter","condition","prescale");
//
// // The statement below shows the alternative for a "Trigger" analysis
// // where the trigger tags do not have "pass" nor "write" indicators.
// // fsel->SetDeviceNames("Trigger","*","*");
//
// // Specify some tags to mark candidate events for selection if the tagging criteria are fulfilled
// fsel->ActivateTag("GFU");         // Gamma ray Follow Up stream
// fsel->ActivateTag("HESE");        // High-Energy Starting Event stream
// fsel->ActivateTag("EstresAlert"); // Enhanced Starting track alert stream
// fsel->ActivateTag("EHEAlert");    // Extreme High Energy event alert stream
//
// // Specify some tags which will be required to have NOT been fulfilled for selecting the event.
// // This can also be used to de-activate tag names that were activated above
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
// NcTaggingSelector* trigsel=new NcTaggingSelector("TriggerSel","RNO-G event trigger selection");
//
// // Specify that we will access the NcTagger (derived) device named "Trigger"
// trigsel->SetDevice("Trigger");
//
// // Specify some tags to mark candidate events for selection if the tagging criteria are fulfilled
// trigsel->ActivateTag("radiant"); // RADIANT trigger
// trigsel->ActivateTag("lt");      // Low threshold trigger
//
// // Specify some tags which will be required to have NOT been fulfilled for selecting the event.
// // This can also be used to de-activate tag names that were activated above
// // because of a matching name pattern.
// trigsel->DeactivateTag("radiant_surface"); // Surface trigger (fictative)
//
//
// Once the user has configured the NcTaggingSelector object, it has to be added as a (sub)task
// into the NcJob (derived) environment for the event-by-event analysis.
//
// Templates for a pure NcJob and an NcJob derived event-by-event task processing
// can be found in /rnopack/macros/analyze.cc and /rnopack/macros/convert.cc, respectively.
//
//--- Author: Nick van Eijndhoven 21-dec-2022, IIHE-VUB, Brussel
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, December 22, 2022  01:28Z
///////////////////////////////////////////////////////////////////////////

#include "NcTaggingSelector.h"
#include "Riostream.h"
 
ClassImp(NcTaggingSelector) // Class implementation to enable ROOT I/O
 
NcTaggingSelector::NcTaggingSelector(const char* name,const char* title) : NcTaggingBase(name,title)
{
// Constructor and initialisation of default parameters.
}
///////////////////////////////////////////////////////////////////////////
NcTaggingSelector::~NcTaggingSelector()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingSelector::Exec(Option_t* opt)
{
// Perform the tagging analysis.

 Int_t nact=0;
 Int_t ndeact=0;
 if (fAct) nact=fAct->GetEntries();
 if (fDeact) ndeact=fDeact->GetEntries();

 // Provide parameter settings at the first invokation only
 if (fFirst)
 {
  fFirst=kFALSE;

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
   cout << " Pass indicator  : " << fPassname;
   if (fPassname=="*") cout << " (no event rejection)";
   cout << endl;
   cout << endl;
  }

  if (fPassname=="*" || (!nact && !ndeact))
  {
   cout << " *** No events will be rejected, since no Pass indicator or tag selections were specified. ***" << endl;
  }

  if (fPassname!="*")
  {
   for (Int_t i=0; i<nact; i++)
   {
    if (!i)
    {
     cout << " === Tag names (*=wildcard) to mark an event as candidate when any of the corresponding \"" << fPassname << "\" flags is set ===" << endl;
     cout << endl;
    }
    TObjString* tagx=(TObjString*)fAct->At(i);
    if (!tagx) continue;

    cout << " ... *" << tagx->GetString() << "*" << endl;
   }
   if (nact) cout << endl;

   for (Int_t i=0; i<ndeact; i++)
   {
    if (!i)
    {
     cout << " === Tag names (*=wildcard) to reject the candidate event when any of the corresponding \"" << fPassname << "\" flags is set ===" << endl;
     cout << endl;
    }
    TObjString* tagx=(TObjString*)fDeact->At(i);
    if (!tagx) continue;

    cout << " ... *" << tagx->GetString() << "*" << endl;
   }
   if (ndeact) cout << endl;
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

 if (!nact && !ndeact) return;

 NcDevice* tagdev=(NcDevice*)fEvt->GetDevice(fDevname);
 if (!tagdev) return;

 Int_t ntags=tagdev->GetNhits();
 if (ntags>fMaxtag) ntags=fMaxtag;

 if (!ntags) return;

 Int_t ipass=0;
 TString namex="";
 select=0;
 for (Int_t itag=1; itag<=ntags; itag++)
 {
  NcSignal* sx=tagdev->GetHit(itag);
  if (!sx) continue;
  name=sx->GetName();
  if (!sx->GetSlotIndex(fPassname)) continue;

  // Flag to indicate if the tag passing criteria are satisfied (1) or not (0)
  ipass=TMath::Nint(sx->GetSignal(fPassname));

  // Look for user specified tags that should be set for this event to be selected
  for (Int_t i=0; i<nact; i++)
  {
   TObjString* sox=(TObjString*)fAct->At(i);
   if (!sox) continue;
   namex=sox->GetString();

   if (!name.Contains(namex)) continue; 

   // Acceptance if the name (pattern) matches and the corresponding tag is set
   if (ipass)
   {
    select=1;
    break;
   }
  }

  // Look for user specified tags that should NOT be set for this event to be selected
  for (Int_t i=0; i<ndeact; i++)
  {
   TObjString* sox=(TObjString*)fDeact->At(i);
   if (!sox) continue;
   namex=sox->GetString();

   if (!name.Contains(namex)) continue; 

   // Rejection if the name (pattern) matches but the corresponding tag is set
   if (ipass)
   {
    select=-1;
    break;
   }
  }
 } // End of tag loop

 // Update the event selection flag in case of rejection
 if (select) fEvt->SetSelectLevel(select);
}
///////////////////////////////////////////////////////////////////////////
