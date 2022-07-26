/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2021, NCFS/IIHE, All Rights Reserved.                     *
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
// Class NcTagger
// NcDevice derived (base) class to handle generic event classification tags.
//
// During data taking or online/offline processing one usually records
// various tags for each event to indicate which selection criteria
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
// Tailoring of the tag settings is provided via the member functions SetPass() and SetWrite().
//
// Note :
// ------
// The standard tagging logic reflects the situation without downscaling,
// which corresponds to the following default behaviour of SetPass() :
//
// pass=kFALSE --> write=kFALSE
// pass=kTRUE  --> write=kTRUE
//
// In case the user wants to reflect a different relation between the "pass" and "write" flags,
// as is the case for a downscaled data stream, the SetWrite() member function has to be invoked
// AFTER invokation of the corresponding SetPass(). 
//
// This class provides a generic basis to handle various event classification tags.
// By introducing instances of this (or a derived) class into an NcEvent c.q. NcDetector
// (or derived) structure, these may be accessed via the usual device retrieval functions.
//
// Usage example :
// =============== 
//
// // Load the necessary libraries
// gSystem->Load("ncfspack");
// gSystem->Load("icepack");
//
// IceEvent evt;
//
// NcTagger trig("Trigger","IceCube trigger info");
//
// NcTagger filt("Filter","IceCube filtered stream");
//
// while (......) // Loop over the recorded events
// {
//  // Reset all data containers
//  evt.Reset();
//  trig.Reset();
//  filt.Reset();
// 
//  // Fill the event structure with recorded data
//  ... read DAQ structures
//  ... obtain the trigger information
//  ... etc.
//
//  // The trigger data
//  trig.SetPass("SMT3",kTRUE);  // Simple Multiplicity Trigger (3 coincidences)
//  trig.SetPass("SMT8",kFALSE); // Simple Multiplicity Trigger (8 coincidences)
//  trig.SetPass("ITOP",kFALSE); // IceTop trigger
//
//  // From the trigger info, this seems to be a low multipicity (=low energy) event.
//  // Note that by default the "write" tag is set accordingly for all these triggers.
//
//  // Perform some reconstruction tasks
//  ... track reconstruction
//  ... energy reconstruction
//  ... topology reconstruction
//  ... etc.
//
//  // The filtered event stream
//  filt.SetPass("Muon",kFALSE);   // Reconstructed muon track 
//  filt.SetPass("EHE",kFALSE);    // Extreme High Energy event
//  filt.SetPass("LowUp",kTRUE);   // Low energy upgoing event
//  filt.SetWrite("LowUp",kFALSE); // This event will not be written due to "LowUp" downscaling
//  filt.SetPass("Solar",kTRUE);   // Event related to solar activity
//
//  // Based on the filter info, the event will be written because of the "Solar" filter.
//
//  // Add the trigger and filter data into the event structure
//  evt.AddDevice(trig);
//  evt.AddDevice(filt);
// }
//
// In case this sample of events is written into a file, the trigger and filter statistics
// may be investigated by means of the NcTaggingStats or NcDataStreamStats facilities. 
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, July 12, 2021  16:36Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 26, 2022  08:05Z
///////////////////////////////////////////////////////////////////////////

#include "NcTagger.h"
#include "Riostream.h"
 
ClassImp(NcTagger) // Class implementation to enable ROOT I/O
 
NcTagger::NcTagger(const char* name,const char* title) : NcDevice(name,title)
{
// Default constructor.
}
///////////////////////////////////////////////////////////////////////////
NcTagger::~NcTagger()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
NcTagger::NcTagger(const NcTagger& q) : NcDevice(q)
{
// Copy constructor.
}
///////////////////////////////////////////////////////////////////////////
void NcTagger::SetPass(TString name,Bool_t flag)
{
// Set the pass flag of the tag with the specified name.
//
// Note :
// ------
// The standard tagging logic reflects the situation without downscaling,
// which corresponds to the following default behaviour :
//
// flag=kFALSE --> pass=kFALSE and write=kFALSE
// flag=kTRUE  --> pass=kTRUE  and write=kTRUE
//
// In case the user wants to reflect a different relation between the "pass" and "write" flags,
// as is the case for a downscaled data stream, the SetWrite() member function has to be invoked
// AFTER this SetPass() invokation. 

 Float_t value=0;
 if (flag) value=1;

 SetTag(name,"Pass",value);
 SetTag(name,"Write",value);
}
///////////////////////////////////////////////////////////////////////////
void NcTagger::SetWrite(TString name,Bool_t flag)
{
// Set the write flag of the tag with the specified name.
//
// Note : The setting of the pass flag is not modified.

 Float_t value=0;
 if (flag) value=1;

 SetTag(name,"Write",value);
}
///////////////////////////////////////////////////////////////////////////
Bool_t NcTagger::GetPass(TString name)
{
// Provide the pass flag of the tag with the specified name.

 Bool_t flag=kFALSE;
 
 NcSignal* sx=GetHit(name);

 if (!sx) return flag;

 Int_t value=TMath::Nint(sx->GetSignal("Pass"));
 if (value) flag=kTRUE;

 return flag;
}
///////////////////////////////////////////////////////////////////////////
Bool_t NcTagger::GetWrite(TString name)
{
// Provide the pass flag of the tag with the specified name.

 Bool_t flag=kFALSE;
 
 NcSignal* sx=GetHit(name);

 if (!sx) return flag;

 Int_t value=TMath::Nint(sx->GetSignal("Write"));
 if (value) flag=kTRUE;

 return flag;
}
///////////////////////////////////////////////////////////////////////////
void NcTagger::SetTag(TString hitname,TString slotname,Float_t value)
{
// Internal member function to record the tag data.

 NcSignal* sx=GetHit(hitname);

 if (sx)
 {
  sx->SetSignal(value,slotname);
 }
 else
 {
  NcSignal sig(hitname);
  sig.AddNamedSlot("Pass");
  sig.AddNamedSlot("Write");
  sig.SetSignal(0,"Pass");
  sig.SetSignal(0,"Write");
  sig.SetSignal(value,slotname);
  AddHit(sig);
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* NcTagger::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects. This feature allows e.g. NcEvent
// to store either NcTagger objects or objects derived from NcTagger
// via tha AddDevice memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 

 NcTagger* q=new NcTagger(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
