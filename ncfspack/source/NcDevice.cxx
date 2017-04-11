/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2009, NCFS, All Rights Reserved.                          *
 *                                                                             *
 * Author: The Netherlands Center for Fundamental Studies (NCFS).              *
 *         http://sites.google.com/site/ncfsweb ncfs.nl@gmail.com              *
 *                                                                             *
 * Contributors are mentioned in the code where appropriate.                   *
 *                                                                             * 
 * No part of this software may be used, copied, modified or distributed       *
 * by any means nor transmitted or translated into machine language without    *
 * written permission by the NCFS.                                             *
 * Permission to use the documentation strictly for non-commercial purposes    *
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

// $Id: NcDevice.cxx 135 2016-09-13 14:27:42Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcDevice
// Signal (Hit) handling of a generic device.
// Basically this class provides a user interface to group and handle
// various instances of NcSignal objects, called generically "hits".
// An NcDevice object itself has (in addition to hit storage) also the
// complete functionality of the class NcSignal.
//
// Example :
// =========
//
// NcDevice m;
// // Set user defined status word to indicate e.g. readout electronics version 
// m.SetStatus(100201);
// m.SetHitCopy(1);
// m.SetName("OM123");
//
// Float_t pos[3]={1,2,3};
// m.SetPosition(pos,"car");
//
// NcSignal s;
//
// s.Reset(1);
// s.SetName("OM123 Hit 1");
// s.SetSlotName("ADC");
// s.SetSignal(10);
// s.SetSlotName("LE",2);
// s.SetSignal(-100,2);
// s.SetSlotName("TOT",3);
// s.SetSignal(-1000,3);
// m.AddHit(s);
//
// s.Reset(1);
// s.SetName("OM123 Hit 2");
// s.SetSlotName("ADC");
// s.SetSignal(11);
// s.SetSlotName("LE",2);
// s.SetSignal(-101,2);
// s.SetSlotName("TOT",3);
// s.SetSignal(1001,3);
// m.AddHit(s);
//
// s.Reset(1);
// s.SetName("OM123 Hit 3");
// s.SetSlotName("ADC");
// s.SetSignal(12);
// s.SetSlotName("LE",2);
// s.SetSignal(-102,2);
// s.SetSlotName("TOT",3);
// s.SetSignal(-1002,3);
// m.AddHit(s);
//
// TObjArray* ordered=m.SortHits("TOT");
// nhits=ordered->GetEntries();
// for (Int_t i=0; i<nhits; i++)
// {
//  NcSignal* sx=(NcSignal*)ordered->At(i);
//  if (sx) sx->Data();
// }
//
//--- Author: Nick van Eijndhoven 23-jun-2004 Utrecht University
//- Modified: NvE $Date: 2016-09-13 16:27:42 +0200 (Tue, 13 Sep 2016) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "NcDevice.h"
#include "Riostream.h"
 
ClassImp(NcDevice) // Class implementation to enable ROOT I/O
 
NcDevice::NcDevice() : NcSignal()
{
// Default constructor.
// The user definable status word is set to zero.
// By default private copies of the recorded hits will be made.
// This implies that by default the device will own the registered hits.
// See the SetHitCopy() memberfunction for further details.
 fStatus=0;
 fHitCopy=1;
 fHits=0;
 fOrdered=0;
 fMarkers=0;
}
///////////////////////////////////////////////////////////////////////////
NcDevice::~NcDevice()
{
// Default destructor.

 // Remove backward links to this device from the hits which were not owned by it.
 // Note : If a hit has been deleted in the meantime the NcSignal destructor has already
 //        automatically removed the corresponding pointer from the storage of this device.
 if (!fHitCopy)
 {
  for (Int_t ih=1; ih<=GetNhits(); ih++)
  {
   NcSignal* sx=GetHit(ih);
   if (sx) sx->ResetLinks(this);
  }
 }

 if (fHits)
 {
  delete fHits;
  fHits=0;
 }

 if (fOrdered)
 {
  delete fOrdered;
  fOrdered=0;
 }

 if (fMarkers)
 {
  delete fMarkers;
  fMarkers=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcDevice::NcDevice(const NcDevice& dev) : NcSignal(dev)
{
// Copy constructor.

 fHits=0;
 fOrdered=0;
 fMarkers=0;

 fStatus=dev.GetStatus();
 fHitCopy=dev.GetHitCopy();

 Int_t nhits=dev.GetNhits();
 if (nhits)
 {
  fHits=new TObjArray(nhits);
  if (fHitCopy) fHits->SetOwner();
  NcSignal* sx=0;
  for (Int_t ih=1; ih<=nhits; ih++)
  {
   sx=dev.GetHit(ih);

   if (!sx) continue;

   if (fHitCopy)
   {
    fHits->Add(sx->Clone());
    NcSignal* s=(NcSignal*)fHits->Last();
    s->ResetLinks((NcDevice*)&dev);
    s->SetDevice(this);
   }
   else
   {
    TArrayI js;
    TArrayI ks;
    Int_t nlinks=sx->GetIndices((NcDevice*)&dev,js,ks);
    Int_t j=0;
    Int_t k=0;
    for (Int_t il=0; il<nlinks; il++)
    {
     j=js.At(il);
     k=ks.At(il);
     sx->SetLink(this,j,k);
    }
    fHits->Add(sx);
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::Reset(Int_t mode)
{
// Reset registered hits and NcSignal attributes.
// Note : The status word and HitCopy flag are NOT modified.
//        Use SetStatus() and SetHitCopy() to modify these parameters. 
// See NcSignal::Reset() for further details.
 RemoveHits();
 NcSignal::Reset(mode);
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::SetHitCopy(Int_t j)
{
// (De)activate the creation of private copies of the NcSignals added as hits.
// j=0 ==> No private copies are made; pointers of original hits are stored.
// j=1 ==> Private copies of the hits are made and these pointers are stored.
//
// Note : Once the storage contains pointer(s) to hit(s) one cannot
//        change the HitCopy mode anymore.
//        To change the HitCopy mode for an existing NcDevice containing
//        hits one first has to invoke either RemoveHits() or Reset().
 if (!fHits)
 {
  if (j==0 || j==1)
  {
   fHitCopy=j;
  }
  else
  {
   cout << "*NcDevice::SetHitCopy* Invalid argument : " << j << endl;
  }
 }
 else
 {
  cout << "*NcDevice::SetHitCopy* Storage already contained hits."
       << "  ==> HitCopy mode not changed." << endl; 
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcDevice::GetHitCopy() const
{
// Provide value of the HitCopy mode.
// 0 ==> No private copies are made; pointers of original hits are stored.
// 1 ==> Private copies of the hits are made and these pointers are stored.
 return fHitCopy;
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::SetStatus(Int_t word)
{
// Set a user defined status word for this device.
 fStatus=word;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcDevice::GetStatus() const
{
// Provide the user defined status word for this device.
 return fStatus;
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::AddHit(NcSignal& s)
{
// Register an NcSignal object as a hit to this device.
// Note : In case this device owns the NcSignal object, the pointer to
//        this device will be stored in the special owning device
//        pointer of the NcSignal object and all (backward) links to this device
//        will be removed from the NcSignal object.
//        In case this device does not own the NcSignal object, a (backward)
//        link to this device is added to the first slot of the NcSignal
//        if there was no link to this device already present.
//        This (backward) link is essential to prevent pointers to non-existing
//        NcSignal objects when the corresponding NcSignal object is deleted.

 if (!fHits)
 {
  fHits=new TObjArray(1);
  if (fHitCopy) fHits->SetOwner();
 }

 // Check if this signal is already stored for this device.
 Int_t nhits=GetNhits();
 for (Int_t i=0; i<nhits; i++)
 {
  if (&s==fHits->At(i)) return; 
 }

 // Check for existing (backward) link to this device.
 Int_t nlinks=s.GetNlinks(this);

 if (fHitCopy)
 {
  fHits->Add(s.Clone());
  // Remove unnecessary backward link(s) from the various slots
  // and set the owning link to this device
  NcSignal* sx=(NcSignal*)fHits->Last();
  if (nlinks) sx->ResetLinks(this);
  sx->SetDevice(this);
 }
 else
 {
  fHits->Add(&s);
  // Set (backward) link to this device
  if (!nlinks) s.AddLink(this);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::RemoveHit(NcSignal& s)
{
// Remove NcSignal object registered as a hit from this device.
 if (fHits)
 {
  NcSignal* test=(NcSignal*)fHits->Remove(&s);
  if (test)
  {
   fHits->Compress();
   if (fHitCopy) delete test;
  }
 }
 if (fOrdered)
 {
  NcSignal* test=(NcSignal*)fOrdered->Remove(&s);
  if (test) fOrdered->Compress();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::RemoveHits()
{
// Remove all NcSignal objects registered as hits from this device.
 if (fHits)
 {
  delete fHits;
  fHits=0;
 }
 if (fOrdered)
 {
  delete fOrdered;
  fOrdered=0;
 }
 if (fMarkers)
 {
  delete fMarkers;
  fMarkers=0;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcDevice::GetNhits() const
{
// Provide the number of registered hits for this device.
 Int_t nhits=0;
 if (fHits) nhits=fHits->GetEntries();
 return nhits;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcDevice::GetNhits(TString name,Int_t mode,Int_t opt) const
{
// Provide the number of hits registered with the specified hit or slot name.
//
// mode = 0 --> Only hits with a matching hit name will be considered
//        1 --> Only hits with a matching slot name will be considered
//        2 --> Hits matching in either hit name or slot name will be considered
//
// opt = 0 --> The specified name has to match exactly the hit or slotname
//       1 --> The specified name string has to be contained in the hit or slotname
//
// The defaults are mode=0 and opt=0.

 if (!fHits) return 0;

 Int_t nfound=0;
 TString hitname;
 Int_t nhits=GetNhits();
 Int_t flag=0;
 for (Int_t i=0; i<nhits; i++)
 {
  NcSignal* sx=(NcSignal*)fHits->At(i);
  if (sx)
  {
   flag=0;
   hitname=sx->GetName();
   if ((!opt && hitname==name) || (opt && hitname.Contains(name.Data()))) flag=1;
   if (sx->GetSlotIndex(name,opt)) flag=2;
   if ((mode==0 && flag==1) || (mode==1 && flag==2) || (mode==2 && flag)) nfound++;
  }
 }

 return nfound;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcDevice::GetHit(Int_t j) const
{
// Provide the NcSignal object registered as hit number j.
// Note : j=1 denotes the first hit.
 if (!fHits) return 0;

 if ((j >= 1) && (j <= GetNhits()))
 {
  return (NcSignal*)fHits->At(j-1);
 }
 else
 {
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcDevice::GetHit(TString name,Int_t mode,Int_t opt) const
{
// Provide the NcSignal object registered as hit with the specified hit or slot name.
// Note : The first hit encountered with the specified name will be provided.
//
// mode = 0 --> Only hits with a matching hit name will be considered
//        1 --> Only hits with a matching slot name will be considered
//        2 --> Hits matching in either hit name or slot name will be considered
//
// opt = 0 --> The specified name has to match exactly the hit or slotname
//       1 --> The specified name string has to be contained in the hit or slotname
//
// The defaults are mode=0 and opt=0.

 if (!fHits) return 0;

 TString hitname;
 Int_t nhits=GetNhits();
 Int_t flag=0;
 for (Int_t i=0; i<nhits; i++)
 {
  NcSignal* sx=(NcSignal*)fHits->At(i);
  if (sx)
  {
   flag=0;
   hitname=sx->GetName();
   if ((!opt && hitname==name) || (opt && hitname.Contains(name.Data()))) flag=1;
   if (sx->GetSlotIndex(name,opt)) flag=2;
   if ((mode==0 && flag==1) || (mode==1 && flag==2) || (mode==2 && flag)) return sx;
  }
 }

 return 0; // No matching name found
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcDevice::GetIdHit(Int_t id) const
{
// Return the hit with unique identifier "id".
 if (!fHits || id<0) return 0;

 NcSignal* sx=0;
 Int_t sid=0;
 for (Int_t i=0; i<GetNhits(); i++)
 {
  sx=(NcSignal*)fHits->At(i);
  if (sx)
  {
   sid=sx->GetUniqueID();
   if (id==sid) return sx;
  }
 }
 return 0; // No matching id found
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDevice::GetHits()
{
// Provide the references to all the registered hits.
 return fHits;
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::GetHits(TObjArray& selected,TString name,Int_t mode,Int_t opt,TObjArray* hits) const
{
// Provide the references to selected hits by looping over the input array "hits"
// and checking for the specified hit or signal slot name.
// A "hit" represents an abstract object which is derived from NcSignal.
// The selected hits are returned via the user provided TObjArray "selected".
// In case hits=0 (default), all the registered hits of the current device are used in the search. 
// Note that the input array "hits" (when provided) is not modified.
//
// selected : The user provided array which will hold all the references to the selected hits.
//
// name : The user provided hit or signal slot name to be used for the hit selection.
//
// mode = 0 --> Hits with a matching hit name will be selected
//        1 --> Hits with a matching signal slot name will be selected
//        2 --> Hits matching in either hit name or signal slot name will be selected
//       -1 --> Hits with NO matching hit name will be selected
//       -2 --> Hits with NO matching signal slot name will be selected
//       -3 --> Hits with NO matching hit name nor signal slot name will be selected
//
// opt = 0 --> The specified name has to match exactly the hit or slotname
//       1 --> The specified name string has to be contained in the hit or slotname
//
// hits : Optional input array with NcSignal objects to be used for the search. 
//
// The defaults are mode=0, opt=0 and hits=0.

 selected.Clear();

 TObjArray* ahits=hits;
 if (!ahits) ahits=fHits;

 Int_t nhits=0;
 if (ahits) nhits=ahits->GetEntries();
 
 if (!ahits || !nhits) return;

 TString hitname;
 Int_t idx=0;
 Int_t flag=0; // 0=no match 1=hit name match 2=slot name match 3=both hit and slot name match
 for (Int_t i=0; i<nhits; i++)
 {
  NcSignal* sx=(NcSignal*)ahits->At(i);

  if (!sx) continue;

  flag=0;
  hitname=sx->GetName();
  idx=sx->GetSlotIndex(name,opt);

  if ((!opt && hitname==name) || (opt && hitname.Contains(name.Data()))) flag=1;

  if (sx->GetSignalFlag(idx))
  {
   if (!flag)
   {
    flag=2;
   }
   else
   {
    flag=3;
   }
  }
  if ((mode==0 && (flag==1 || flag==3)) || (mode==1 && flag>1) || (mode==2 && flag)) selected.Add(sx);
  if ((mode==-1 && (flag==0 || flag==2)) || (mode==-2 && flag<2) || (mode==-3 && !flag)) selected.Add(sx);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::ShowHit(Int_t j,TString f,TString u) const
{
// Show data of the registered j-th hit according to the specified 
// coordinate frame f.
// If j=0 all associated hits will be shown.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The defaults are j=0, f="car" and "u=rad".

 if (!j)
 {
  Int_t nhits=GetNhits();
  for (Int_t ih=1; ih<=nhits; ih++)
  {
   NcSignal* sx=GetHit(ih);
   if (sx) sx->Data(f,u);
  }
 }
 else
 {
  NcSignal* s=GetHit(j);
  if (s) s->Data(f,u);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::Data(TString f,TString u) const
{
// Print the device and all registered hit info according to the specified
// coordinate frame f.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The defaults are f="car" and u="rad".

 NcSignal::Data(f,u);
 Int_t nhits=GetNhits();
 if (nhits)
 {
  cout << " The following " << nhits << " hits are registered : " << endl;
  ShowHit(0,f,u);
 }
 else
 {
  cout << " No hits have been registered for this device." << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::GetExtremes(Float_t& vmin,Float_t& vmax,Int_t idx,TObjArray* hits,Int_t mode,Int_t deadcheck) const
{
// Provide the min. and max. signal values of an array of hits.
// The input argument "idx" denotes the index of the signal slots to be investigated.
// The default is idx=1;
// In case hits=0 (default), the registered hits of the current device are used. 
// The gain etc... corrected signals will be used in the process as specified
// by the  "mode" argument. The definition of this "mode" parameter corresponds to
// the description provided in the GetSignal memberfunction of class NcSignal.
// The default is mode=1 (for backward compatibility reasons).
// The argument "deadcheck" allows to reject signals which were declared as "Dead".
// If deadcheck=0 the dead signals will be treated in the same way as the other signals.
// To achieve an identical treatment of dead and alive signals, the setting of deadcheck=0
// will automatically set also mode=0 to retrieve the stored signal values "as is".
// The default is deadcheck=1 (for backward compatibility reasons).

 vmin=0;
 vmax=0;

 TObjArray* ahits=hits;
 if (!ahits) ahits=fHits;
 
 if (idx<=0 || !ahits) return;

 // Take the stored signal value "as is" to ensure identical treatment of dead and alive signals
 if (!deadcheck) mode=0;

 Int_t nhits=ahits->GetEntries();

 Float_t sig=0;
 TObject* obj=0;
 NcSignal* sx=0;
 for (Int_t i=0; i<nhits; i++)
 {
  obj=ahits->At(i);
  if (!obj) continue;
  if (!obj->InheritsFrom("NcSignal")) continue;
  sx=(NcSignal*)obj;

  if (idx > sx->GetNvalues()) continue; // User specified slotindex out of range for this signal
  if (deadcheck && sx->GetDeadValue(idx)) continue;  // Only take alive signals

  sig=sx->GetSignal(idx,mode);
  if (i==0)
  {
   vmin=sig;
   vmax=sig;
  }
  else
  {
   if (sig<vmin) vmin=sig;
   if (sig>vmax) vmax=sig;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::GetExtremes(Float_t& vmin,Float_t& vmax,TString name,TObjArray* hits,Int_t mode,Int_t deadcheck) const
{
// Provide the min. and max. signal values of an array of hits.
// The input argument "name" denotes the name of the signal slots to be investigated.
// In case hits=0 (default), the registered hits of the current device are used. 
// The gain etc... corrected signals will be used in the process as specified
// by the  "mode" argument. The definition of this "mode" parameter corresponds to
// the description provided in the GetSignal memberfunction of class NcSignal.
// The default is mode=1 (for backward compatibility reasons).
// The argument "deadcheck" allows to reject signals which were declared as "Dead".
// If deadcheck=0 the dead signals will be treated in the same way as the other signals.
// To achieve an identical treatment of dead and alive signals, the setting of deadcheck=0
// will automatically set also mode=0 to retrieve the stored signal values "as is".
// The default is deadcheck=1 (for backward compatibility reasons).

 vmin=0;
 vmax=0;

 TObjArray* ahits=hits;
 if (!ahits) ahits=fHits;
 
 if (!ahits) return;

 // Take the stored signal value "as is" to ensure identical treatment of dead and alive signals
 if (!deadcheck) mode=0;

 Int_t nhits=ahits->GetEntries();

 Int_t idx=0; // The signal slotindex to perform the sorting on

 Float_t sig=0;
 TObject* obj=0;
 NcSignal* sx=0;
 for (Int_t i=0; i<nhits; i++)
 {
  obj=ahits->At(i);
  if (!obj) continue;
  if (!obj->InheritsFrom("NcSignal")) continue;
  sx=(NcSignal*)obj;

  // Obtain the slotindex corresponding to the user selection
  idx=sx->GetSlotIndex(name);
  if (!idx) continue;

  if (deadcheck && sx->GetDeadValue(idx)) continue; // Only take alive signals

  sig=sx->GetSignal(idx,mode);
  if (i==0)
  {
   vmin=sig;
   vmax=sig;
  }
  else
  {
   if (sig<vmin) vmin=sig;
   if (sig>vmax) vmax=sig;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDevice::SortHits(Int_t idx,Int_t mode,TObjArray* hits,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to an array of hits by looping over the input array "hits"
// and checking the signal value.
// The ordered array is returned as a TObjArray either via a user provided array "ordered"
// or as a returned pointer.
// In case hits=0 (default), the registered hits of the current device are used. 
// Note that the original hit array is not modified.
// A "hit" represents an abstract object which is derived from NcSignal.
// The user can specify the index of the signal slot to perform the sorting on.
// By default the slotindex will be 1.
// Via the "mode" argument the user can specify ordering in decreasing
// order (mode=-1) or ordering in increasing order (mode=1).
// The default is mode=-1.
// The gain etc... corrected signals will be used in the ordering process as
// specified by the "mcal" argument. The definition of this "mcal" parameter
// corresponds to the signal correction mode described in the GetSignal
// memberfunction of class NcSignal.
// The default is mcal=1 (for backward compatibility reasons).
// The argument "deadcheck" allows to reject signals which were declared as "Dead".
// If deadcheck=0 the dead signals will be treated in the same way as the other signals.
// To achieve an identical treatment of dead and alive signals, the setting of deadcheck=0
// will automatically set also mcal=0 to retrieve the stored signal values "as is".
// The default is deadcheck=1 (for backward compatibility reasons).
//
// Note :
// ------
// In case ordered=0 the ordered hit pointers are returned via a multi-purpose array,
// which may be overwritten by other memberfunctions (not restricted to hit ordering).
// It is recommended to provide a user defined array via the argument "ordered" to omit
// the danger of overwriting (or being overwritten by) other selections and to allow to use
// the ordered hit list amongst other selections.
// In case a user defined array "ordered" is provided, this memberfunction returns 0 for the
// return argument.
//
// The default is ordered=0.

 if (ordered) ordered->Clear();

 TObjArray* ahits=hits;
 if (!ahits) ahits=fHits;

 Int_t nhits=0;
 if (ahits) nhits=ahits->GetEntries();
 
 if (idx<=0 || abs(mode)!=1 || !ahits || !nhits) return 0;

 if (ordered)
 {
  ordered->Expand(nhits);
 }
 else
 {
  if (fOrdered) delete fOrdered;
  fOrdered=new TObjArray(nhits);
 }

 // Take the stored signal value "as is" to ensure identical treatment of dead and alive signals
 if (!deadcheck) mcal=0;

 TObjArray* arr=0;   // Generic sorted array pointer to be used in the logic below

 if (ordered)
 {
  arr=ordered;
 }
 else
 {
  arr=fOrdered;
 }
 
 TObject* obj=0;
 NcSignal* s=0;
 Int_t nord=0;
 for (Int_t i=0; i<nhits; i++) // Loop over all hits of the array
 {
  obj=ahits->At(i);
  if (!obj) continue;
  if (!obj->InheritsFrom("NcSignal")) continue;
  s=(NcSignal*)obj;

  if (!s) continue;

  if (idx > s->GetNvalues()) continue; // User specified slotindex out of range for this signal
  if (deadcheck && s->GetDeadValue(idx)) continue;  // Only take alive signals
 
  if (nord == 0) // store the first hit with a signal at the first ordered position
  {
   nord++;
   arr->AddAt(s,nord-1);
   continue;
  }
 
  for (Int_t j=0; j<=nord; j++) // put hit in the right ordered position
  {
   if (j == nord) // module has smallest (mode=-1) or largest (mode=1) signal seen so far
   {
    nord++;
    arr->AddAt(s,j); // add hit at the end
    break; // go for next hit
   }
 
   if (mode==-1 && s->GetSignal(idx,mcal) <= ((NcSignal*)arr->At(j))->GetSignal(idx,mcal)) continue;
   if (mode==1 && s->GetSignal(idx,mcal) >= ((NcSignal*)arr->At(j))->GetSignal(idx,mcal)) continue;
 
   nord++;
   for (Int_t k=nord-1; k>j; k--) // create empty position
   {
    arr->AddAt(arr->At(k-1),k);
   }
   arr->AddAt(s,j); // put hit at empty position
   break; // go for next hit
  }
 }

 if (ordered)
 {
  return 0;
 }
 else
 {
  return fOrdered;
 }
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDevice::SortHits(TString name,Int_t mode,TObjArray* hits,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to an array of hits by looping over the input array "hits"
// and checking the signal value.
// The ordered array is returned as a TObjArray either via a user provided array "ordered"
// or as a returned pointer.
// In case hits=0 (default), the registered hits of the current device are used. 
// Note that the input array is not modified.
// A "hit" represents an abstract object which is derived from NcSignal.
// The user can specify the name of the signal slot to perform the sorting on.
// In case no matching slotname is found, the signal will be skipped.
// Via the "mode" argument the user can specify ordering in decreasing
// order (mode=-1) or ordering in increasing order (mode=1).
// The default is mode=-1.
// The gain etc... corrected signals will be used in the ordering process as
// specified by the "mcal" argument. The definition of this "mcal" parameter
// corresponds to the signal correction mode described in the GetSignal
// memberfunction of class NcSignal.
// The default is mcal=1 (for backward compatibility reasons).
// The argument "deadcheck" allows to reject signals which were declared as "Dead".
// If deadcheck=0 the dead signals will be treated in the same way as the other signals.
// To achieve an identical treatment of dead and alive signals, the setting of deadcheck=0
// will automatically set also mcal=0 to retrieve the stored signal values "as is".
// The default is deadcheck=1 (for backward compatibility reasons).
//
// Note :
// ------
// In case ordered=0 the ordered hit pointers are returned via a multi-purpose array,
// which may be overwritten by other memberfunctions (not restricted to hit ordering).
// It is recommended to provide a user defined array via the argument "ordered" to omit
// the danger of overwriting (or being overwritten by) other selections and to allow to use
// the ordered hit list amongst other selections.
// In case a user defined array "ordered" is provided, this memberfunction returns 0 for the
// return argument.
//
// The default is ordered=0.

 if (ordered) ordered->Clear();

 TObjArray* ahits=hits;
 if (!ahits) ahits=fHits;

 Int_t nhits=0;
 if (ahits) nhits=ahits->GetEntries();

 if (abs(mode)!=1 || !ahits || !nhits) return 0;

 if (ordered)
 {
  ordered->Expand(nhits);
 }
 else
 {
  if (fOrdered) delete fOrdered;
  fOrdered=new TObjArray(nhits);
 }

 Int_t idx=0; // The signal slotindex to perform the sorting on

 // Take the stored signal value "as is" to ensure identical treatment of dead and alive signals
 if (!deadcheck) mcal=0;


 TObjArray* arr=0;   // Generic sorted array pointer to be used in the logic below

 if (ordered)
 {
  arr=ordered;
 }
 else
 {
  arr=fOrdered;
 }
 
 TObject* obj=0;
 NcSignal* s=0;
 Int_t nord=0;
 for (Int_t i=0; i<nhits; i++) // loop over all hits of the array
 {
  obj=ahits->At(i);
  if (!obj) continue;
  if (!obj->InheritsFrom("NcSignal")) continue;
  s=(NcSignal*)obj;

  if (!s) continue;

  // Obtain the slotindex corresponding to the user selection
  idx=s->GetSlotIndex(name);
  if (!idx) continue;

  if (deadcheck && s->GetDeadValue(idx)) continue; // only take alive signals
 
  if (nord == 0) // store the first hit with a signal at the first ordered position
  {
   nord++;
   arr->AddAt(s,nord-1);
   continue;
  }
 
  for (Int_t j=0; j<=nord; j++) // put hit in the right ordered position
  {
   if (j == nord) // module has smallest (mode=-1) or largest (mode=1) signal seen so far
   {
    nord++;
    arr->AddAt(s,j); // add hit at the end
    break; // go for next hit
   }
 
   if (mode==-1 && s->GetSignal(idx,mcal) <= ((NcSignal*)arr->At(j))->GetSignal(idx,mcal)) continue;
   if (mode==1 && s->GetSignal(idx,mcal) >= ((NcSignal*)arr->At(j))->GetSignal(idx,mcal)) continue;
 
   nord++;
   for (Int_t k=nord-1; k>j; k--) // create empty position
   {
    arr->AddAt(arr->At(k-1),k);
   }
   arr->AddAt(s,j); // put hit at empty position
   break; // go for next hit
  }
 }

 if (ordered)
 {
  return 0;
 }
 else
 {
  return fOrdered;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::DisplayHits(Int_t idx,Float_t scale,TObjArray* hits,Int_t dp,Int_t mode,Int_t mcol)
{
// 3D color display of an array hits.
// The user can specify the index (default=1) of the signal slot to perform the display for.
// The marker size will indicate the absolute value of the signal (specified by the slotindex)
// as a percentage of the input argument "scale".
// In case scale<0 the maximum absolute signal value encountered in the hit array will be used
// to define the 100% scale. The default is scale=-1.
// In case hits=0 (default), the registered hits of the current device are used. 
// Note that the input array is not modified.
// In case dp=1 the device position will be used, otherwise the hit position will
// be used in the display. The default is dp=0.
// Via the "mcol" argument the user can specify the marker color (see TAttMarker).
// The default is mcol=4 (blue).
// Signals which were declared as "Dead" will not be displayed.
// The gain etc... corrected signals will be used to determine the marker size.
// The gain correction is performed according to "mode" argument. The definition of this
// "mode" parameter corresponds to the description provided in the GetSignal
// memberfunction of class NcSignal.
// The default is mode=1 (for backward compatibility reasons).
//
// Note :
// ------
// Before any display activity, a TCanvas and a TView have to be initiated
// first by the user like for instance
// 
// TCanvas* c1=new TCanvas("c1","c1");
// TView* view=new TView(1);
// view->SetRange(-1000,-1000,-1000,1000,1000,1000);
// view->ShowAxis();

 Int_t thisdev=0; // Indicate whether this is the owning device or not 
 TObjArray* ahits=hits;
 if (!ahits)
 {
  ahits=fHits;
  thisdev=1;
 }
 
 if (idx<=0 || !ahits) return;

 Int_t nhits=ahits->GetEntries();
 if (!nhits) return;

 Float_t sigmax=fabs(scale);
 if (scale<0)
 {
  Float_t vmin,vmax;
  GetExtremes(vmin,vmax,idx,ahits,mode);
  sigmax=fabs(vmax);
  if (fabs(vmin)>sigmax) sigmax=fabs(vmin);
 }

 if (sigmax <=0) return;

 if (fMarkers)
 {
  delete fMarkers;
  fMarkers=0;
 }
 fMarkers=new TObjArray(nhits);
 fMarkers->SetOwner();

 Float_t pos[3];
 GetPosition(pos,"car");

 Float_t sig=0;
 TObject* obj=0;
 NcSignal* sx=0;
 for (Int_t ih=0; ih<nhits; ih++)
 {
  obj=ahits->At(ih);
  if (!obj) continue;
  if (!obj->InheritsFrom("NcSignal")) continue;
  sx=(NcSignal*)obj;

  if (!dp)
  {
   sx->GetPosition(pos,"car");
  }
  else
  {
   if (!thisdev)
   {
    NcDevice* dev=sx->GetDevice();
    if (dev) dev->GetPosition(pos,"car");
   }
  }
  sig=sx->GetSignal(idx,mode);

  // Skip dead signals
  if (fabs(sig) <= 0.) continue;

  TPolyMarker3D* m=new TPolyMarker3D();
  m->SetMarkerStyle(8);
  m->SetMarkerColor(mcol);
  m->SetMarkerSize(100.*fabs(sig)/sigmax);
  m->SetPoint(0,pos[0],pos[1],pos[2]);
  fMarkers->Add(m);
  m->Draw();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDevice::DisplayHits(TString name,Float_t scale,TObjArray* hits,Int_t dp,Int_t mode,Int_t mcol)
{
// 3D color display of an array hits.
// The user can specify the name of the signal slot to perform the display for.
// The marker size will indicate the absolute value of the signal (specified by the slotname)
// as a percentage of the input argument "scale".
// In case scale<0 the maximum absolute signal value encountered in the hit array will be used
// to define the 100% scale. The default is scale=-1.
// In case hits=0 (default), the registered hits of the current device are used. 
// Note that the input array is not modified.
// In case dp=1 the device position will be used, otherwise the hit position will
// be used in the display. The default is dp=0.
// The marker size will indicate the percentage of the maximum encountered value
// of the absolute value of the name-specified input signal slots.
// Via the "mcol" argument the user can specify the marker color (see TAttMarker).
// The default is mcol=4 (blue).
// Signals which were declared as "Dead" will not be displayed.
// The gain etc... corrected signals will be used to determine the marker size.
// The gain correction is performed according to "mode" argument. The definition of this
// "mode" parameter corresponds to the description provided in the GetSignal
// memberfunction of class NcSignal.
// The default is mode=1 (for backward compatibility reasons).
//
// Note :
// ------
// Before any display activity, a TCanvas and a TView have to be initiated
// first by the user like for instance
// 
// TCanvas* c1=new TCanvas("c1","c1");
// TView* view=new TView(1);
// view->SetRange(-1000,-1000,-1000,1000,1000,1000);
// view->ShowAxis();

 Int_t thisdev=0; // Indicate whether this is the owning device or not 
 TObjArray* ahits=hits;
 if (!ahits)
 {
  ahits=fHits;
  thisdev=1;
 }
 
 if (!ahits) return;

 Int_t nhits=ahits->GetEntries();

 if (!nhits) return;

 Float_t sigmax=fabs(scale);
 if (scale<0)
 {
  Float_t vmin,vmax;
  GetExtremes(vmin,vmax,name,ahits,mode);
  sigmax=fabs(vmax);
  if (fabs(vmin)>sigmax) sigmax=fabs(vmin);
 }

 if (sigmax <=0) return;

 if (fMarkers)
 {
  delete fMarkers;
  fMarkers=0;
 }
 fMarkers=new TObjArray(nhits);
 fMarkers->SetOwner();

 Float_t pos[3];
 GetPosition(pos,"car");

 Int_t idx=0; // The slot index corresponding to the user specified name
 Float_t sig=0;
 TObject* obj=0;
 NcSignal* sx=0;
 for (Int_t ih=0; ih<nhits; ih++)
 {
  obj=ahits->At(ih);
  if (!obj) continue;
  if (!obj->InheritsFrom("NcSignal")) continue;
  sx=(NcSignal*)obj;
  idx=sx->GetSlotIndex(name);
  if (!idx) continue;
  if (!dp)
  {
   sx->GetPosition(pos,"car");
  }
  else
  {
   if (!thisdev)
   {
    NcDevice* dev=sx->GetDevice();
    if (dev) dev->GetPosition(pos,"car");
   }
  }
  sig=sx->GetSignal(idx,mode);

  // Skip dead signals
  if (fabs(sig) <= 0.) continue;

  TPolyMarker3D* m=new TPolyMarker3D();
  m->SetMarkerStyle(8);
  m->SetMarkerColor(mcol);
  m->SetMarkerSize(100.*fabs(sig)/sigmax);
  m->SetPoint(0,pos[0],pos[1],pos[2]);
  fMarkers->Add(m);
  m->Draw();
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcDevice::SumSignals(Int_t idx,Int_t mode,TObjArray* hits)
{
// Summation of selected signal values by looping over the input array "hits".
// In case hits=0 (default), the registered hits of the current device are used. 
// Note that the original hit array is not modified.
// A "hit" represents an abstract object which is derived from NcSignal.
// The user has to specify the index "idx" of the signal slot to perform the summation on.
// Note that idx=1 corresponds to the first signal slot.
// The gain etc... corrected signals may be used in the summation process as
// specified by the "mode" argument. The definition of this "mode" parameter
// corresponds to the signal correction mode described in the GetSignal
// memberfunction of class NcSignal.
// The default in the summation process is mode=1.

 TObjArray* ahits=hits;
 if (!ahits) ahits=fHits;

 Int_t nhits=0;
 if (ahits) nhits=ahits->GetEntries();
 
 if (idx<=0 || !ahits || !nhits) return 0;
 
 TObject* obj=0;
 NcSignal* s=0;
 Double_t sum=0;
 for (Int_t i=0; i<nhits; i++) // Loop over all hits of the array
 {
  obj=ahits->At(i);
  if (!obj) continue;
  if (!obj->InheritsFrom("NcSignal")) continue;
  s=(NcSignal*)obj;

  if (!s) continue;

  if (idx > s->GetNvalues()) continue; // User specified slotindex out of range for this signal

  sum+=s->GetSignal(idx,mode);
 }

 return sum;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcDevice::SumSignals(TString name,Int_t mode,TObjArray* hits)
{
// Summation of selected signal values by looping over the input array "hits".
// In case hits=0 (default), the registered hits of the current device are used. 
// Note that the original hit array is not modified.
// A "hit" represents an abstract object which is derived from NcSignal.
// The user has to specify the signal slot to perform the summation on via
// the input argument "name".
// The gain etc... corrected signals may be used in the summation process as
// specified by the "mode" argument. The definition of this "mode" parameter
// corresponds to the signal correction mode described in the GetSignal
// memberfunction of class NcSignal.
// The default in the summation process is mode=1.

 TObjArray* ahits=hits;
 if (!ahits) ahits=fHits;

 Int_t nhits=0;
 if (ahits) nhits=ahits->GetEntries();
 
 if (!ahits || !nhits) return 0;
 
 TObject* obj=0;
 NcSignal* s=0;
 Double_t sum=0;
 for (Int_t i=0; i<nhits; i++) // Loop over all hits of the array
 {
  obj=ahits->At(i);
  if (!obj) continue;
  if (!obj->InheritsFrom("NcSignal")) continue;
  s=(NcSignal*)obj;

  if (!s) continue;

  sum+=s->GetSignal(name,mode);
 }

 return sum;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcDevice::SlideWindow(TObjArray* hits,Double_t thres,Double_t swin,TString sname,Int_t smode,TString wname,Int_t wmode,Int_t* i1,Int_t* i2) const
{
// Perform a sliding window scan of some cumulated signal by looping over the input array "hits".
// A "hit" represents an abstract object which is (derived from) NcSignal.
// Note that the input array "hits" is not modified.
//
// The input array "hits" will be scanned from the start by setting "v1" as the value
// of the observable "sname" at the starting hit. The scanning will continue with the
// subsequent "sname" values and a (weighted) sum of these values will be determined.
// Every time a new "sname" value is investigated a variable "v2" is set to that "sname" value,
// which defines the (growing) scan window [v1,v2].
// The scanning stops if either the (weighted) sum reaches (or exceeds) the threshold value "thres"
// or the absolute size of the scan window |v2-v1| exceeds the maximum size "swin".
// In the former case the value "v2" is returned, whereas in the latter case the scan
// will be repeated starting from the hit following the previous starting hit in the array "hits".
// In case none of the search windows could reach (or exceed) the threshold "thres", the value 0 will be returned. 
//
// Input arguments :
// -----------------
// hits  : Input array with NcSignal (derived) objects to be used for the search. 
// thres : The threshold for the cumulative signal.
// swin  : The maximum size of the search interval window for the observable "sname".
// sname : The signal slot name to be used as observable. 
// smode : The signal retrieval mode as documented in NcSignal::GetSignal.
// wname : The signal slot name to be used as weight. 
// wmode : The weight retrieval mode as documented in NcSignal::GetSignal.
// i1    : Optional retrieved index in the "hits" array of the NcSignal object for which "v1" was obtained.
// i2    : Optional retrieved index in the "hits" array of the NcSignal object for which "v2" was obtained.
//         The values of i1 and/or i2 are only returned if the corresponding input argument was a valid,
//         non-zero pointer value.
//         In case no search window [v1,v2] satisfied the search criteria the values i1=-1 and i2=-1 will be returned.
//
// Return value :
// --------------
// The value "v2" of the observable "sname" for which the search window [v1,v2] satisfies
// the user defined search criteria, or 0 in all other cases.  
//
// Notes :
// -------
// 1) If wname="none" no weighting will be performed (i.e. all weights are set to 1)
// 2) For the scanning procedure the "sname" values should appear in a consecutive order.
//    This implies that the user should take care of correctly ordering the input array "hits",
//    for instance by using the SortHits() member function. 
// 3) Using the return values of "i1" and "i2" the user can investigate the complete resulting search window
//    and define any signal value other than the returned "v2" as the observable of interest.
//
// The defaults are smode=0, wname="none", wmode=0, i1=0 and i2=0.
// This implies that by default the array indices of the NcSignal objects corresponding to "v1" and "v2" are not provided.
//
// Example :
// ---------
// Assume that an NcEvent has a device of (an NcDevice derived) class "MyTimer" with many hits of which each hit contains
// an individual hit time "T" (in ms) and amplitude "Amp" (in ADC counts).
// We would like to determine an event start time at which at least 5% of the total observed "MyTimer" amplitude is recorded
// within a time window of 6 ms.
// This event start time can be obtained as follows ("evt" is the pointer to the NcEvent structure) :
//
// TObjArray* hits=new TObjArray();
// evt->SortHits("MyTimer","T",1,8,1,hits); // Sort hits with increasing hit time
// NcDevice scanner;
// Int_t i1,i2;
// Double_t totamp=scanner.SumSignals("Amp",8,hits);
// Double_t thres=0.05*totamp;
// Double_t tstart=scanner.SlideWindow(hits,thres,6,"T",8,"Amp",8,&i1,&i2);
// // Investigate all the signals in the found time window
// for (Int_t i=i1; i<=i2; i++)
// {
//  if (i<0) break;
//  NcSignal* sx=(NcSignal*)hits->At(i);
//  if (sx) sx->Data();
// }

 Int_t nhits=0;
 if (hits) nhits=hits->GetEntries();
 
 if (!nhits) return 0;

 Double_t v1=0;
 Double_t v2=0;
 Double_t wsize=0;
 Double_t sum=0;
 Double_t w=1;
 Int_t wflag=1;
 if (wname=="none") wflag=0;
 Int_t ifirst=0;
 TObject* obj=0;
 NcSignal* sx1=0;
 NcSignal* sx2=0;
 while (ifirst<nhits)
 {
  obj=hits->At(ifirst);
  if (!(obj->InheritsFrom("NcSignal")))
  {
   ifirst++;
   continue;
  }

  // The starting signal of this window
  sx1=(NcSignal*)obj;
  v1=sx1->GetSignal(sname,smode);
  if (wflag) w=sx1->GetSignal(wname,wmode);
  sum=w;
  if (sum>=thres)
  {
   if (i1) *i1=ifirst;
   if (i2) *i2=ifirst;
   return v1;
  }

  // Scan the allowed window size 
  for (Int_t ilast=ifirst+1; ilast<nhits; ilast++)
  {
   obj=hits->At(ilast);
   if (!(obj->InheritsFrom("NcSignal"))) continue;

   sx2=(NcSignal*)obj;
   v2=sx2->GetSignal(sname,smode);

   wsize=fabs(v2-v1);
   if (wsize>swin) break;

   if (wflag) w=sx2->GetSignal(wname,wmode);
   sum+=w;   
   if (sum>=thres)
   {
    if (i1) *i1=ifirst;
    if (i2) *i2=ilast;
    return v2;
   }

  }  
  ifirst++;
  sum=0;
 }

 // No window found to match the selection criteria
 if (i1) *i1=-1;
 if (i2) *i2=-1;
 return 0;
}
///////////////////////////////////////////////////////////////////////////
TObject* NcDevice::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects. This feature allows e.g. NcEvent
// to store either NcDevice objects or objects derived from NcDevice
// via tha AddDevice memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 

 NcDevice* dev=new NcDevice(*this);
 if (name)
 {
  if (strlen(name)) dev->SetName(name);
 }
 return dev;
}
///////////////////////////////////////////////////////////////////////////
