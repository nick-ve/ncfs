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
// Class NcTaggingBase
// TTask derived base class to facilitate processing of event classification tags.
//  
// This base class is used by NcDataStreamStats, NcTaggingStats and NcTaggingSelector.
// Please refer to the documentation of these derived classes for further details.
//
//--- Author: Nick van Eijndhoven 20-dec-2022, IIHE-VUB, Brussel
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, December 22, 2022  01:28Z
///////////////////////////////////////////////////////////////////////////

#include "NcTaggingBase.h"
#include "Riostream.h"
 
ClassImp(NcTaggingBase) // Class implementation to enable ROOT I/O
 
NcTaggingBase::NcTaggingBase(const char* name,const char* title) : TTask(name,title)
{
// Constructor and initialisation of default parameters.

 // Items for general processing
 fMaxtag=99;
 fDevname="none";
 fPassname="*";
 fWritename="*";
 fAct=0;
 fDeact=0;
 fFirst=kTRUE;

 // Items for statistics processing
 fEvt=0;
 fEvtFirst=0;
 fEvtLast=0;
 fNevt=0;
 fNevtComb=0;
}
///////////////////////////////////////////////////////////////////////////
NcTaggingBase::~NcTaggingBase()
{
// Default destructor.
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

 if (fEvtFirst)
 {
  delete fEvtFirst;
  fEvtFirst=0;
 }
 if (fEvtLast)
 {
  delete fEvtLast;
  fEvtLast=0;
 }

 // Remove the subtasks from the internal TTask list without deleting them
 if (fTasks) fTasks->Clear();

}
///////////////////////////////////////////////////////////////////////////
NcTaggingBase::NcTaggingBase(const NcTaggingBase& q) : TTask(q)
{
// Copy constructor

 // Items for general processing
 fMaxtag=q.fMaxtag;
 fDevname=q.fDevname;
 fPassname=q.fPassname;
 fWritename=q.fWritename;
 fAct=0;
 if (q.fAct) fAct=(TObjArray*)(q.fAct->Clone());
 fDeact=0;
 if (q.fDeact) fDeact=(TObjArray*)(q.fDeact->Clone());
 fFirst=kTRUE;

 // Items for statistics processing
 fEvt=0;
 fEvtFirst=0;
 fEvtLast=0;
 fNevt=0;
 fNevtComb=0;
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingBase::SetMaxTag(Int_t n)
{
// Set the maximum number of tags to be processed.
// The default constructor has set n=99.
 fMaxtag=n;
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingBase::SetDeviceNames(TString devname,TString passname,TString writename)
{
// Set the name of a generic NcDevice to be investigated and the corresponding names
// of the tag passing and writing indicators. 
// Note that the device must have the specific data structure as outlined in the general
// documentation of the derived classes NcDataStreamStats, NcTaggingStats and NcTaggingSelector.
// 
// It is common that a certain tag has the following two indicators
// pass  : Indicates whether the tag criteria have been fulfilled (1) or not (0)
// write : Indicates whether the event is actually labeled to be written (1) or not (0)
//
// Instead of the names "pass" and "write", various other naming conventions
// (e.g. "condition" and "prescale") are often used.
// Via the input arguments "passname" and "writename" one can specify the names
// used by the current experiment.
//
// Notes :
// -------
// 1) This memberfunction allows the investigation of user defined tagging devices,
//    to be backward compatible with old data files.
//    For new(er) data, the use of NcTagger (derived) devices is recommended.
// 2) In case passname="*" and/or writename="*", no check will be made for the
//    corresponding indicator setting and the tag criteria will be regarded as fulfilled.

 fDevname=devname;
 fPassname=passname; 
 fWritename=writename; 
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingBase::SetDevice(TString devname,Bool_t passcheck,Bool_t writecheck)
{
// Set the name of an NcTagger (derived) device and the check modes of the tag
// passing and writing indicators.
// 
// The pre-defined tag passing and writing indicators are :
// Pass  : Indicates whether the tag criteria have been fulfilled (1) or not (0)
// Write : Indicates whether the event is actually labeled to be written (1) or not (0)
//
// In case passcheck=kFALSE and/or writecheck=kFALSE, no check will be made for the
// corresponding indicator setting and the tag criteria will be regarded as fulfilled.
//
// The default values are passcheck=kTRUE and writecheck=kTRUE.

 fDevname=devname;
 fPassname="Pass"; 
 fWritename="Write";

 if (!passcheck) fPassname="*"; 
 if (!writecheck) fWritename="*"; 
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingBase::ActivateTag(TString name)
{
// In case the specified "name" matches (part of) a certain tag name, that
// specific tag will be used by NcTaggingSelector to mark candidate events
// for selection for further analysis.
// This memberfunction may be invoked several times to activate various tags
// before executing the task.
// At least one of the tags that have been specified here needs to have the
// corresponding tag criteria fulfilled (i.e. the "pass" indicator set to 1)
// to mark the event as a candidate for selection.
//
// For investigation of event tag statistics by NcDataStreamStats or NcTaggingStats,
// this will activate the corresponding tag for the combined pass*write statistics
// which will enable to study user selected event samples.
// It should be noted that this doesn't affect the results for the
// regular individual "pass" and "write" statistics.
//
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
//    by the user will be considered de-activated for the combined pass*write statistics.
// 3) Tags that are explicitly de-activated by the user via invokation of DeactivateTag()
//    can not be effectively activated anymore.

 if (!fAct)
 {
  fAct=new TObjArray();
  fAct->SetOwner();
 }
 TObjString* s=new TObjString();
 s->SetString(name);
 fAct->Add(s);
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingBase::DeactivateTag(TString name)
{
// In case the specified "name" matches (part of) a certain tag name, the
// corresponding tag criteria will be required to be NOT fulfilled 
// (i.e. the "pass" indicator set to 0) by the event in order not to be
// flagged "rejected" for further analysis by NcTaggingSelector.
//
// For investigation of event tag statistics by NcDataStreamStats or NcTaggingStats,
// this will de-activate the corresponding tag for the combined pass*write statistics
// which will enable to study background reduction.
// It should be noted that this doesn't affect the results for the
// regular individual "pass" and "write" statistics.
//
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
// 2) Tags that are explicitly de-activated via this memberfunction
//    can not be effectively activated anymore via invokation of ActivateTag().

 if (!fDeact)
 {
  fDeact=new TObjArray();
  fDeact->SetOwner();
 }
 TObjString* s=new TObjString();
 s->SetString(name);
 fDeact->Add(s);
}
///////////////////////////////////////////////////////////////////////////
void NcTaggingBase::ListStatistics()
{
// Provide the final statistics.

  cout << endl;
  cout << " *" << ClassName() << "* Final tagging statistics." << endl;
  cout << endl;

 if (fDevname=="none")
 {
  cout << " *** No device name for the event tags was provided. ***" << endl;
  return;
 }
 else
 {
  cout << " *** Device and indicator identifiers ***" << endl;
  cout << " Tag device name : " << fDevname << endl;
  cout << " Pass indicator  : " << fPassname << endl;
  cout << " Write indicator : " << fWritename << endl;
 }

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

 cout << endl;
 cout << " Total number of tagged events : " << fNevt << endl;
 cout << endl;

 if (!fNevt) return;

 cout << " ===== First tagged event =====" << endl;
 if (fEvtFirst) fEvtFirst->HeaderData();
 cout << endl;

 cout << " ===== Last tagged event =====" << endl;
 if (fEvtLast) fEvtLast->HeaderData();
 cout << endl;

 Double_t dt=fEvtFirst->GetDifference(fEvtLast,"s");
 cout << " Elapsed timespan (sec.) : " << dt << endl;
 if (dt<=0) cout << " *** Incorrect timespan. No rates calculated. ***" << endl;

 // The various tagged event rates
 if (dt>0)
 {
  Double_t ratetot=double(fNevt)/dt;
  cout << " Total tagged event rate (Hz) : " << ratetot << endl; 
  Double_t ratecombitot=double(fNevtComb)/dt;
  if (nact || ndeact) cout << " User reduced event rate (Hz) : " << ratecombitot << endl;

  // Rate per tag channel
  TString name;
  Double_t ratecond=0;
  Double_t ratewrite=0;
  Double_t ratio=0;
  cout << endl;
  cout << " ===== Total tagging rates (Hz) for the various channels =====" << endl;
  for (Int_t i=1; i<=fPass.GetNslots(); i++)
  {
   name=fPass.GetSlotName(i);
   ratecond=fPass.GetSignal(i)/dt;
   ratewrite=fWrites.GetSignal(i)/dt;
   ratio=1;
   if (ratecond>0) ratio=ratewrite/ratecond;
   cout << " " << name << " ... pass : " << ratecond << "  write : " << ratewrite << " ===> write/pass : " << ratio << endl;
  }
 }

 // Overview of the various tagging counts
 Int_t ntags=fPass.GetNslots();
 if (ntags>fMaxtag) ntags=fMaxtag;

 Int_t ndigits1=log10(float(ntags))+1.;
 Int_t ndigits2=log10(float(fNevt))+1.;

 Int_t ival=0;

 cout << endl;
 cout << " ===== Total tag passing statistics =====" << endl;
 fPass.Data();
 cout << endl;
 cout << " The tag passing matrix with empty slots (=rows/columns) suppressed : " << endl;
 cout << " -------------------------------------------------------------------- " << endl;
 cout << setfill(' ');
 for (Int_t irow=-1; irow<ntags; irow++)
 {
  // Skip empty rows to reduce printed matrix size
  if (irow>-1)
  {
   if (fPass.GetSignal(irow+1)<1) continue;
  }
  cout << " " << setw(ndigits1) << (irow+1);
  for (Int_t icol=0; icol<ntags; icol++)
  {
   // Skip empty columns to reduce printed matrix size
   if (fPass.GetSignal(icol+1)<1) continue;
   if (irow==-1)
   {
    ival=icol+1;
   }
   else
   {
    ival=int(fMpass(irow,icol));
   }
   cout << " " << setw(ndigits2) << ival;
  }
  cout << endl;
 }
  
 if (fWritename!="*")
 {
  cout << endl;
  cout << " ===== Total tag event writing (prescale) statistics =====" << endl;
  fWrites.Data();
  cout << endl;
  cout << " The tag event writing (prescale) matrix with empty slots (=rows/columns) suppressed : " << endl;
  cout << " ------------------------------------------------------------------------------------- " << endl;
  for (Int_t irow=-1; irow<ntags; irow++)
  {
   // Skip empty rows to reduce printed matrix size
   if (irow>-1)
   {
    if (fWrites.GetSignal(irow+1)<1) continue;
   }
   cout << " " << setw(ndigits1) << (irow+1);
   for (Int_t icol=0; icol<ntags; icol++)
   {
    // Skip empty columns to reduce printed matrix size
    if (fWrites.GetSignal(icol+1)<1) continue;
    if (irow==-1)
    {
     ival=icol+1;
    }
    else
    {
     ival=int(fMwrites(irow,icol));
    }
    cout << " " << setw(ndigits2) << ival;
   }
   cout << endl;
  }
 }

 cout << endl;
 cout << " =====  Combined tag passing and event writing (prescale) statistics     =====" << endl;
 cout << " ===== with user selected de-activations to study background reduction   =====" << endl;
 fCombis.Data();
 cout << endl;
 cout << " The combined tag passing and event writing (prescale) matrix with empty slots (=rows/columns) suppressed : " << endl;
 cout << " ---------------------------------------------------------------------------------------------------------- " << endl;
 for (Int_t irow=-1; irow<ntags; irow++)
 {
  // Skip empty rows to reduce printed matrix size
  if (irow>-1)
  {
   if (fCombis.GetSignal(irow+1)<1) continue;
  }
  cout << " " << setw(ndigits1) << (irow+1);
  for (Int_t icol=0; icol<ntags; icol++)
  {
   // Skip empty columns to reduce printed matrix size
   if (fCombis.GetSignal(icol+1)<1) continue;
   if (irow==-1)
   {
    ival=icol+1;
   }
   else
   {
    ival=int(fMcombis(irow,icol));
   }
   cout << " " << setw(ndigits2) << ival;
  }
  cout << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
