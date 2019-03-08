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

// $Id: NcAttrib.cxx 67 2012-07-18 14:51:01Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcAttrib
// Generic handling of detector signal (calibration) attributes.
// Normally this class is only used as a base class to provide the various
// attributes to the derived class. An example of this is NcSignal.
// However, one can of course also use this class on its own as shown
// in the simple example hereafter.
//
// Example :
// ---------
// NcAttrib a;
// a.SetSlotName("PMT amplitude in Volt");
// a.SetGain(250.7);
// a.SetSlotName("Time of flight in ns",2);
// a.SetOffset(-22.5,2);
// a.SetSlotName("PMT amplitude in ADC",3);
// a.SetGain(1340,3);
// a.SetSlotName("TDC",4);
// a.SetOffset(10.75,"TDC");
// a.SetEdgeOn(3);
// a.SetDead(1);
// a.List();
//
//--- Author: Nick van Eijndhoven 18-sep-2003 Utrecht University
//- Modified: NvE $Date: 2012-07-18 16:51:01 +0200 (Wed, 18 Jul 2012) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "NcAttrib.h"
#include "Riostream.h"
 
ClassImp(NcAttrib) // Class implementation to enable ROOT I/O
 
NcAttrib::NcAttrib()
{
// Creation of an NcAttrib object and initialisation of parameters.
// Several values of the same type (e.g. gain) can be stored in different slots.
// If needed, the storage for values will be expanded automatically
// when entering values.
 fGains=0;
 fOffsets=0;
 fCalflags=0;
 fNames=0;
 fCalfuncs=0;
 fDecalfuncs=0;
}
///////////////////////////////////////////////////////////////////////////
NcAttrib::~NcAttrib()
{
// Destructor to delete dynamically allocated memory
 if (fGains)
 {
  delete fGains;
  fGains=0;
 }
 if (fOffsets)
 {
  delete fOffsets;
  fOffsets=0;
 }
 if (fCalflags)
 {
  delete fCalflags;
  fCalflags=0;
 }
 if (fNames)
 {
  delete fNames;
  fNames=0;
 }
 if (fCalfuncs)
 {
  delete fCalfuncs;
  fCalfuncs=0;
 }
 if (fDecalfuncs)
 {
  delete fDecalfuncs;
  fDecalfuncs=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcAttrib::NcAttrib(const NcAttrib& a)
{
// Copy constructor
 fGains=0;
 fOffsets=0;
 fCalflags=0;
 fNames=0;
 fCalfuncs=0;
 fDecalfuncs=0;

 Int_t n=0;
 Double_t val=0;

 n=a.GetNgains();
 for (Int_t ig=1; ig<=n; ig++)
 {
  val=a.GetGain(ig);
  if (a.GetGainFlag(ig)) SetGain(val,ig);
 }

 n=a.GetNoffsets();
 for (Int_t io=1; io<=n; io++)
 {
  val=a.GetOffset(io);
  if (a.GetOffsetFlag(io)) SetOffset(val,io);
 }

 n=a.GetNcalflags();
 for (Int_t ic=1; ic<=n; ic++)
 {
  SetEdgeValue(a.GetEdgeValue(ic),ic);
  if (a.GetLockValue(ic)) Lock(ic);
  if (a.GetDeadValue(ic)) SetDead(ic);
 }

 n=a.GetNnames();
 TString s;
 for (Int_t in=1; in<=n; in++)
 {
  s=a.GetSlotName(in);
  if (s!="") SetSlotName(s,in);
 }

 n=a.GetNcalfuncs();
 for (Int_t icalf=1; icalf<=n; icalf++)
 {
  TF1* f=a.GetCalFunction(icalf);
  if (f) SetCalFunction(f,icalf);
 }

 n=a.GetNdecalfuncs();
 for (Int_t idecalf=1; idecalf<=n; idecalf++)
 {
  TF1* f=a.GetDecalFunction(idecalf);
  if (f) SetDecalFunction(f,idecalf);
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetNgains() const
{
// Provide the number of specified gains for this attribute.

 if (!fGains) return 0;

 Int_t n=0;
 for (Int_t i=1; i<=fGains->GetSize(); i++)
 {
  if (GetGainFlag(i)) n=i;
 }

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetNoffsets() const
{
// Provide the number of specified offsets for this attribute.

 if (!fOffsets) return 0;

 Int_t n=0;
 for (Int_t i=1; i<=fOffsets->GetSize(); i++)
 {
  if (GetOffsetFlag(i)) n=i;
 }

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetNcalflags() const
{
// Provide the number of specified calib. flags for this attribute.

 if (!fCalflags) return 0;

 Int_t n=0;
 for (Int_t i=1; i<=fCalflags->GetSize(); i++)
 {
  if (fCalflags->At(i-1)) n=i;
 }

 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetNnames() const
{
// Provide the maximum number of specified names for this attribute.

 if (!fNames) return 0;

 Int_t n=0;
 for (Int_t i=1; i<=fNames->GetSize(); i++)
 {
  if (fNames->At(i-1)) n=i;
 }

 return n;
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetGain(Double_t gain,Int_t j)
{
// Store gain value of the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for gain values, the number of reserved slots for the gain
// values is increased automatically.

 if (j<1) 
 {
  cout << " *NcAttrib::SetGain* Invalid argument j = " << j << endl;
  return;
 }

 if (!fGains)
 {
  fGains=new TArrayF(j);
 }

 Int_t size=fGains->GetSize();

 if (j>size)
 {
  fGains->Set(j);
 }

 fGains->AddAt(float(gain),j-1);

 Int_t oflag=GetOffsetFlag(j);

 SetCalFlags(1,oflag,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetGain(Double_t gain,TString name)
{
// Store gain value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) SetGain(gain,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetOffset(Double_t off,Int_t j)
{
// Store offset value of the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for offset values, the number of reserved slots for the offset
// values is increased automatically.

 if (j<1) 
 {
  cout << " *NcAttrib::GetOffset* Invalid argument j = " << j << endl;
  return;
 }

 if (!fOffsets)
 {
  fOffsets=new TArrayF(j);
 }

 Int_t size=fOffsets->GetSize();

 if (j>size)
 {
  fOffsets->Set(j);
 }

 fOffsets->AddAt(float(off),j-1);

 Int_t gflag=GetGainFlag(j);

 SetCalFlags(gflag,1,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetOffset(Double_t off,TString name)
{
// Store offset value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) SetOffset(off,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetCalFlags(Int_t gainflag,Int_t offsetflag,Int_t j)
{
// Store calibration flags of the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the calib. flags, the number of reserved slots for the calib.
// flags is increased automatically.
// The value stored is : 10000*edge +1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::GetCalFlags* Invalid argument j = " << j << endl;
  return;
 }

 if (!fCalflags)
 {
  fCalflags=new TArrayI(j);
 }

 Int_t size=fCalflags->GetSize();

 if (j>size)
 {
  fCalflags->Set(j);
 }

 Int_t edge=GetEdgeValue(j);
 Int_t lock=GetLockValue(j);
 Int_t dead=GetDeadValue(j);

 Int_t word=10000*edge+1000*lock+100*dead+10*gainflag+offsetflag;
 
 fCalflags->AddAt(word,j-1);
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetGainFlag(Int_t j) const
{
// Provide gain flag of the j-th (default j=1) attribute slot.
//
// flag = 1 : Gain was set
//        0 : Gain was not set
//
// Note : The first attribute slot is at j=1.
// In case j is invalid, 0 is returned.

 if (j<1) 
 {
  cout << " *NcAttrib::GetGainFlag* Invalid argument j = " << j << endl;
  return 0;
 }
 Int_t gflag=0;
 if (fCalflags)
 {
  if (j>0 && j<=(fCalflags->GetSize()))
  {
   Int_t word=fCalflags->At(j-1);
   word=word%100;
   gflag=word/10;
  }
 }
 return gflag;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetGainFlag(TString name) const
{
// Provide gain flag of the name-specified attribute slot.
//
// flag = 1 : Gain was set
//        0 : Gain was not set
//
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 Int_t flag=0;
 if (j>0) flag=GetGainFlag(j);
 return flag;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetOffsetFlag(Int_t j) const
{
// Provide offset flag of the j-th (default j=1) attribute slot.
//
// flag = 1 : Offset was set
//        0 : Offset was not set
//
// Note : The first attribute slot is at j=1.
// In case j is invalid, 0 is returned.

 if (j<1) 
 {
  cout << " *NcAttrib::GetOffsetFlag* Invalid argument j = " << j << endl;
  return 0;
 }

 Int_t oflag=0;
 if (fCalflags)
 {
  if (j>0 && j<=(fCalflags->GetSize()))
  {
   Int_t word=fCalflags->At(j-1);
   oflag=word%10;
  }
 }
 return oflag;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetOffsetFlag(TString name) const
{
// Provide ofset flag of the name-specified attribute slot.
//
// flag = 1 : Offset was set
//        0 : Offset was not set
//
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 Int_t flag=0;
 if (j>0) flag=GetOffsetFlag(j);
 return flag;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetCalWord(Int_t j) const
{
// Provide calib. word of the j-th (default j=1) attribute slot.
// The word value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.
//
// Note : The first attribute slot is at j=1.
// In case j is invalid, 0 is returned.

 if (j<1) 
 {
  cout << " *NcAttrib::GetCalWord* Invalid argument j = " << j << endl;
  return 0;
 }

 Int_t word=0;
 if (fCalflags)
 {
  if (j>0 && j<=(fCalflags->GetSize())) word=fCalflags->At(j-1);
 }
 return word;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetCalWord(TString name) const
{
// Provide calib. word of the name-specified attribute slot.
// The word value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 Int_t word=0;
 if (j>0) word=GetCalWord(j);
 return word;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcAttrib::GetGain(Int_t j) const
{
// Provide gain value of the j-th (default j=1) attribute slot.
// The first attribute slot is at j=1.
// In case no gain value was set or the argument j is invalid, 0 is returned.
// Note : Use GetGainFlag(j) to check whether this gain was set or not.

 if (j<1) 
 {
  cout << " *NcAttrib::GetGain* Invalid argument j = " << j << endl;
  return 0;
 }

 Float_t gain=0;
 if (fGains)
 {
  if (j>0 && j<=(fGains->GetSize()))
  {
   if (GetGainFlag(j)) gain=fGains->At(j-1);
  }
 }
 return gain;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcAttrib::GetGain(TString name) const
{
// Provide gain value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 Float_t gain=0;
 if (j>0) gain=GetGain(j);
 return gain;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcAttrib::GetOffset(Int_t j) const
{
// Provide offset value of the j-th (default j=1) attribute slot.
// The first attribute slot at j=1.
// In case no offset value was set or the argument j is invalid, 0 is returned.
// Note : Use GetOffsetFlag(j) to check whether this offset was set or not.

 if (j<1) 
 {
  cout << " *NcAttrib::GetOffset* Invalid argument j = " << j << endl;
  return 0;
 }

 Float_t offset=0;
 if (fOffsets)
 {
  if (j>0 && j<=(fOffsets->GetSize()))
  {
   if (GetOffsetFlag(j)) offset=fOffsets->At(j-1);
  }
 }
 return offset;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcAttrib::GetOffset(TString name) const
{
// Provide offset value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 Float_t offset=0;
 if (j>0) offset=GetOffset(j);
 return offset;
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::ResetGain(Int_t j)
{
// Reset the gain value of the j-th (default j=1) attribute slot.
// Notes : The first attribute slot is at j=1.
//         j=0 ==> All gain values will be reset.
 
 if (!fGains) return;

 Int_t size=fGains->GetSize();

 if ((j>=0) && (j<=size))
 {
  if (j)
  {
   fGains->AddAt(0,j-1);
   Int_t oflag=GetOffsetFlag(j);
   SetCalFlags(0,oflag,j);
  }
  else
  {
   for (Int_t i=0; i<size; i++)
   {
    fGains->AddAt(0,i);
    Int_t oflag=GetOffsetFlag(i);
    SetCalFlags(0,oflag,i);
   }
  }
 }
 else
 {
  cout << " *NcAttrib::ResetGain* Index j = " << j << " invalid." << endl;
  return;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::ResetGain(TString name)
{
// Reset the gain value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) ResetGain(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::ResetOffset(Int_t j)
{
// Reset the offset value of the j-th (default j=1) attribute slot.
// Notes : The first attribute slot is at j=1.
//         j=0 ==> All offset values will be reset.
 
 if (!fOffsets) return;

 Int_t size=fOffsets->GetSize();

 if ((j>=0) && (j<=size))
 {
  if (j)
  {
   fOffsets->AddAt(0,j-1);
   Int_t gflag=GetGainFlag(j);
   SetCalFlags(gflag,0,j);
  }
  else
  {
   for (Int_t i=0; i<size; i++)
   {
    fOffsets->AddAt(0,i);
    Int_t gflag=GetGainFlag(i);
    SetCalFlags(gflag,0,i);
   }
  }
 }
 else
 {
  cout << " *NcAttrib::ResetOffset* Index j = " << j << " invalid." << endl;
  return;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::ResetOffset(TString name)
{
// Reset the offset value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) ResetOffset(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::DeleteCalibrations(Int_t mode)
{
// User selected delete of all gains and/or offsets.
// mode = 0 : All attributes (names,gains,offsets,(de)calfuncs, edge and dead values) are deleted.
//        1 : Only the gains are deleted.
//        2 : Only the offsets are deleted.
//        3 : Gains, offsets and (de)calfuncs are deleted, but names, edge and dead values are kept.
//        4 : Only the calib. functions are deleted.
//        5 : Only the de-calib. functions are deleted.
//        6 : Only the calib. and de-calib. functions are deleted.
//
// The default when invoking DeleteCalibrations() corresponds to mode=0.

 if (mode<0 || mode>6)
 {
  cout << " *NcAttrib::DeleteCalibrations* Unknown mode : " << mode << endl;
  cout << " Default mode=0 will be used." << endl;
  mode=0;
 }

 if (mode==0 || mode==3)
 {
  ResetGain(0);
  if (fGains)
  {
   delete fGains;
   fGains=0;
  }
  ResetOffset(0);
  if (fOffsets)
  {
   delete fOffsets;
   fOffsets=0;
  }
  if (fCalflags && mode==0)
  {
   delete fCalflags;
   fCalflags=0;
  }
  if (fNames && mode==0)
  {
   delete fNames;
   fNames=0;
  }
  if (fCalfuncs)
  {
   delete fCalfuncs;
   fCalfuncs=0;
  }
  if (fDecalfuncs)
  {
   delete fDecalfuncs;
   fDecalfuncs=0;
  }
  return;
 }

 if (mode==1)
 {
  ResetGain(0);
  if (fGains)
  {
   delete fGains;
   fGains=0;
  }
 }

 if (mode==2)
 {
  ResetOffset(0);
  if (fOffsets)
  {
   delete fOffsets;
   fOffsets=0;
  }
 }

 if (mode==4 || mode==6)
 {
  if (fCalfuncs)
  {
   delete fCalfuncs;
   fCalfuncs=0;
  }
 }

 if (mode==5 || mode==6)
 {
  if (fDecalfuncs)
  {
   delete fDecalfuncs;
   fDecalfuncs=0;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetDead(Int_t j)
{
// Set the dead flag to 1 for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, the number of reserved slots for the flags
// is increased automatically.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::SetDead* Invalid argument j = " << j << endl;
  return;
 }

 if (!fCalflags)
 {
  fCalflags=new TArrayI(j);
 }

 Int_t size=fCalflags->GetSize();

 if (j>size)
 {
  fCalflags->Set(j);
 }

 Int_t dead=1;
 Int_t oflag=GetOffsetFlag(j);
 Int_t gflag=GetGainFlag(j);
 Int_t lock=GetLockValue(j);
 Int_t edge=GetEdgeValue(j);

 Int_t word=10000*edge+1000*lock+100*dead+10*gflag+oflag;
 
 fCalflags->AddAt(word,j-1);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetDead(TString name)
{
// Set the dead flag to 1 for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) SetDead(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetAlive(Int_t j)
{
// Set the dead flag to 0 for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, no action is taken since by default the dead flag is 0.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::SetAlive* Invalid argument j = " << j << endl;
  return;
 }

 if (!fCalflags || j>fCalflags->GetSize()) return;

 Int_t dead=0;
 Int_t oflag=GetOffsetFlag(j);
 Int_t gflag=GetGainFlag(j);
 Int_t lock=GetLockValue(j);
 Int_t edge=GetEdgeValue(j);

 Int_t word=10000*edge+1000*lock+100*dead+10*gflag+oflag;
 
 fCalflags->AddAt(word,j-1);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetAlive(TString name)
{
// Set the dead flag to 0 for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) SetAlive(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::Lock(Int_t j)
{
// Set the lock flag to 1 for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, the number of reserved slots for the flags
// is increased automatically.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::Lock* Invalid argument j = " << j << endl;
  return;
 }

 if (!fCalflags)
 {
  fCalflags=new TArrayI(j);
 }

 Int_t size=fCalflags->GetSize();

 if (j>size)
 {
  fCalflags->Set(j);
 }

 Int_t lock=1;
 Int_t dead=GetDeadValue(j);
 Int_t oflag=GetOffsetFlag(j);
 Int_t gflag=GetGainFlag(j);
 Int_t edge=GetEdgeValue(j);

 Int_t word=10000*edge+1000*lock+100*dead+10*gflag+oflag;
 
 fCalflags->AddAt(word,j-1);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::Lock(TString name)
{
// Set the lock flag to 1 for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) Lock(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::Unlock(Int_t j)
{
// Set the lock flag to 0 for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, no action is taken since by default the dead flag is 0.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::Unlock* Invalid argument j = " << j << endl;
  return;
 }

 if (!fCalflags || j>fCalflags->GetSize()) return;

 Int_t lock=0;
 Int_t dead=GetDeadValue();
 Int_t oflag=GetOffsetFlag(j);
 Int_t gflag=GetGainFlag(j);
 Int_t edge=GetEdgeValue(j);

 Int_t word=10000*edge+1000*lock+100*dead+10*gflag+oflag;
 
 fCalflags->AddAt(word,j-1);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::Unlock(TString name)
{
// Set the lock flag to 0 for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) Unlock(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetEdgeOn(Int_t j)
{
// Set the edge value to 1 for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, the number of reserved slots for the flags
// is increased automatically.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::SetEdgeOn* Invalid argument j = " << j << endl;
  return;
 }

 SetEdgeValue(1,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetEdgeOn(TString name)
{
// Set the edge value to 1 for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) SetEdgeOn(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetEdgeOff(Int_t j)
{
// Set the edge value to 0 for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, no action is taken since by default the edge flag is 0.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::SetEdgeOff* Invalid argument j = " << j << endl;
  return;
 }

 if (!fCalflags || j>fCalflags->GetSize()) return;

 SetEdgeValue(0,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetEdgeOff(TString name)
{
// Set the edge value to 0 for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) SetEdgeOff(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetEdgeValue(Int_t val,Int_t j)
{
// Set the edge value to "val" for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, the number of reserved slots for the flags
// is increased automatically.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::SetEdgeValue* Invalid argument j = " << j << endl;
  return;
 }

 if (!fCalflags)
 {
  fCalflags=new TArrayI(j);
 }

 Int_t size=fCalflags->GetSize();

 if (j>size)
 {
  fCalflags->Set(j);
 }

 Int_t edge=val;
 Int_t lock=GetLockValue(j);
 Int_t dead=GetDeadValue(j);
 Int_t gflag=GetGainFlag(j);
 Int_t oflag=GetOffsetFlag(j);

 Int_t word=10000*edge+1000*lock+100*dead+10*gflag+oflag;
 
 fCalflags->AddAt(word,j-1);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetEdgeValue(Int_t val,TString name)
{
// Set the edge value to "val" for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) SetEdgeValue(val,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::IncreaseEdgeValue(Int_t j)
{
// Increase the edge value by 1 for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, the number of reserved slots for the flags
// is increased automatically.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::IncreaseEdgeValue* Invalid argument j = " << j << endl;
  return;
 }

 Int_t edge=GetEdgeValue();
 SetEdgeValue(edge+1,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::IncreaseEdgeValue(TString name)
{
// Increase the edge value by 1 for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) IncreaseEdgeValue(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::DecreaseEdgeValue(Int_t j)
{
// Decrease the edge value by 1 for the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the flags, the number of reserved slots for the flags
// is increased automatically.
// The value stored is : 10000*edge + 1000*lock + 100*dead + 10*gainflag + offsetflag.

 if (j<1) 
 {
  cout << " *NcAttrib::DecreaseEdgeValue* Invalid argument j = " << j << endl;
  return;
 }

 Int_t edge=GetEdgeValue();
 SetEdgeValue(edge-1,j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::DecreaseEdgeValue(TString name)
{
// Decrease the edge value by 1 for the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) DecreaseEdgeValue(j);
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetEdgeValue(Int_t j) const
{
// Provide edge value of the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case j is invalid, 0 is returned.

 if (j<1) 
 {
  cout << " *NcAttrib::GetEdgeValue* Invalid argument j = " << j << endl;
  return 0;
 }

 Int_t edge=0;
 if (fCalflags)
 {
  if (j>0 && j<=(fCalflags->GetSize()))
  {
   Int_t word=fCalflags->At(j-1);
   edge=word/10000;
  }
 }
 return edge;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetEdgeValue(TString name) const
{
// Provide edge value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 Int_t val=0;
 if (j>0) val=GetEdgeValue(j);
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetDeadValue(Int_t j) const
{
// Provide dead value of the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case j is invalid, 0 is returned.

 if (j<1) 
 {
  cout << " *NcAttrib::GetDeadValue* Invalid argument j = " << j << endl;
  return 0;
 }

 Int_t dead=0;
 if (fCalflags)
 {
  if (j>0 && j<=(fCalflags->GetSize()))
  {
   Int_t word=fCalflags->At(j-1);
   word=word%1000;
   dead=word/100;
  }
 }
 return dead;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetDeadValue(TString name) const
{
// Provide dead value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 Int_t val=0;
 if (j>0) val=GetDeadValue(j);
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetLockValue(Int_t j) const
{
// Provide lock value of the j-th (default j=1) attribute slot.
// Note : The first attribute slot is at j=1.
// In case j is invalid, 0 is returned.

 if (j<1) 
 {
  cout << " *NcAttrib::GetLockValue* Invalid argument j = " << j << endl;
  return 0;
 }

 Int_t lock=0;
 if (fCalflags)
 {
  if (j>0 && j<=(fCalflags->GetSize()))
  {
   Int_t word=fCalflags->At(j-1);
   word=word%10000;
   lock=word/1000;
  }
 }
 return lock;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetLockValue(TString name) const
{
// Provide lock value of the name-specified attribute slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 Int_t val=0;
 if (j>0) val=GetLockValue(j);
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetNslots() const
{
// Provide the number of existing slots.

  Int_t n=GetNcalflags();
  Int_t nn=GetNnames();
  Int_t ncalf=GetNcalfuncs();
  Int_t ndecalf=GetNdecalfuncs();
  if (n<nn) n=nn;
  if (n<ncalf) n=ncalf;
  if (n<ndecalf) n=ndecalf;

  return n;
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::AddNamedSlot(TString s)
{
// Add a new slot with the specified name.
// In case a slot with the specified name already exists, no action is taken.

 if (GetSlotIndex(s)) return;

 Int_t n=GetNslots();
 SetSlotName(s,n+1);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetSlotName(TString s,Int_t j)
{
// Set a user defined name for the j-th (default j=1) slot. 
// Note : The first attribute slot is at j=1.
// In case the value of the index j exceeds the maximum number of reserved
// slots for the names, the number of reserved slots for the names
// is increased automatically to the corresponding value.

 if (j<1) 
 {
  cout << " *NcAttrib::SetSlotName* Invalid argument j = " << j << endl;
  return;
 }

 if (!fNames)
 {
  fNames=new TObjArray(j);
  fNames->SetOwner();
 }

 if (j>fNames->GetSize()) fNames->Expand(j);

 TObjString* so=(TObjString*)fNames->At(j-1);
 if (!so)
 {
  so=new TObjString(s.Data());
  fNames->AddAt(so,j-1);
 }
 else
 {
  so->SetString(s);
 }
}
///////////////////////////////////////////////////////////////////////////
TString NcAttrib::GetSlotName(Int_t j) const
{
// Provide the user defined name for the j-th (default j=1) slot. 
// Note : The first attribute slot is at j=1.

 TString s="";
 if (j<1) 
 {
  cout << " *NcAttrib::GetSlotName* Invalid argument j = " << j << endl;
  return s;
 }

 if (fNames)
 {
  if (j<=fNames->GetSize())
  {
   TObjString* so=(TObjString*)fNames->At(j-1);
   if (so) s=so->GetString();
  }
 }
 return s;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetSlotIndex(TString name,Int_t opt) const
{
// Provide the slot index for the matching name.
//
// opt = 0 --> The specified name has to match exactly the slotname
//       1 --> The specified name string has to be contained in the slotname
//
// The default is opt=0.
//
// If no matching name is found, 0 is returned.
// Note : The first attribute slot is at j=1.

 Int_t index=0;

 if (fNames)
 {
  TString s;
  Int_t size=fNames->GetSize();
  for (Int_t i=0; i<size; i++)
  {
   TObjString* so=(TObjString*)fNames->At(i);
   if (so) s=so->GetString();
   if ((!opt && s==name) || (opt && s.Contains(name.Data())))
   {
    index=i+1;
    break;
   }
  }
 }
 return index;
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::List(Int_t j) const
{
// Provide attribute information for the j-th slot.
// The first slot is at j=1.
// In case j=0 (default) the data of all slots will be listed.

 if (j<0) 
 {
  cout << " *NcAttrib::Data* Invalid argument j = " << j << endl;
  return;
 }

 if (j>0)
 {
  if (GetGainFlag(j)) cout << " gain : " << GetGain(j);
  if (GetOffsetFlag(j)) cout << " offset : " << GetOffset(j);
  if (GetEdgeValue(j)) cout << " edge : " << GetEdgeValue(j);
  if (GetDeadValue(j)) cout << " dead : " << GetDeadValue(j);
  if (GetLockValue(j)) cout << " lock : " << GetLockValue(j);
  if (GetCalFunction(j)) cout << " *Fcalib*";
  if (GetDecalFunction(j)) cout << " *Fdecalib*";
  TString s=GetSlotName(j);
  if (s!="") cout << " name : " << s.Data();
 }
 else
 {
  Int_t ng=GetNgains();
  Int_t no=GetNoffsets();
  Int_t nf=0;
  if (fCalflags) nf=fCalflags->GetSize();
  Int_t nn=GetNnames();
  Int_t n=ng;
  if (n<no) n=no;
  if (n<nn) n=nn;
  if (n<nf) n=nf;
  Int_t printf=0;
  TString s;
  for (Int_t i=1; i<=n; i++)
  {
   printf=0;
   if (GetGainFlag(i))      {cout << " gain : " << GetGain(i); printf=1;}
   if (GetOffsetFlag(i))    {cout << " offset : " << GetOffset(i); printf=1;}
   if (GetEdgeValue(i))     {cout << " edge : " << GetEdgeValue(i); printf=1;}
   if (GetDeadValue(i))     {cout << " dead : " << GetDeadValue(i); printf=1;}
   if (GetLockValue(i))     {cout << " lock : " << GetLockValue(i); printf=1;}
   if (GetCalFunction(i))   {cout << " *Fcalib*"; printf=1;}
   if (GetDecalFunction(i)) {cout << " *Fdecalib*"; printf=1;}
   s=GetSlotName(i);
   if (s!="") {cout << " name : " << s.Data(); printf=1;}
   if (printf) cout << endl;
  }
 }
} 
///////////////////////////////////////////////////////////////////////////
void NcAttrib::List(TString name) const
{
// Provide attribute information for the name-specified slot.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) List(j);
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::Load(NcAttrib& a,Int_t j)
{
// Load attributes of the j-th slot of the input NcAttrib into this NcAttrib object.
// 
// Note : if j=0, then all attributes of all slots are loaded
//
// The default is j=0.

 if (j<0) 
 {
  cout << " *NcAttrib::Load* Invalid argument j = " << j << endl;
  return;
 }

 Int_t n=0;

 if (j==0) // load attributes for all slots
 {
  n=a.GetNgains();
  for (Int_t ig=1; ig<=n; ig++)
  {
   if (a.GetGainFlag(ig))
   {
    SetGain(a.GetGain(ig),ig);
   }
   else
   {
    ResetGain(ig);
   }
  }
  n=a.GetNoffsets();
  for (Int_t io=1; io<=n; io++)
  {
   if (a.GetOffsetFlag(io))
   {
    SetOffset(a.GetOffset(io),io);
   }
   else
   {
    ResetOffset(io);
   }
  }
  n=a.GetNcalflags();
  for (Int_t ic=1; ic<=n; ic++)
  {
   SetEdgeValue(a.GetEdgeValue(ic),ic);
   if (a.GetLockValue(ic))
   {
    Lock(ic);
   }
   else
   {
    Unlock(ic);
   }
   if (a.GetDeadValue(ic))
   {
    SetDead(ic);
   }
   else
   {
    SetAlive(ic);
   }
  }
  n=a.GetNnames();
  TString s;
  for (Int_t in=1; in<=n; in++)
  {
   s=a.GetSlotName(in);
   SetSlotName(s,in);
  }
  n=a.GetNcalfuncs();
  for (Int_t icalf=1; icalf<=n; icalf++)
  {
   TF1* f=a.GetCalFunction(icalf);
   SetCalFunction(f,icalf);
  }
  n=a.GetNdecalfuncs();
  for (Int_t idecalf=1; idecalf<=n; idecalf++)
  {
   TF1* f=a.GetDecalFunction(idecalf);
   SetDecalFunction(f,idecalf);
  }
 }
 else // load attributes for specified j-th slot only
 {
  n=a.GetNgains();
  if (j<=n)
  {
   if (a.GetGainFlag(j))
   {
    SetGain(a.GetGain(j),j);
   }
   else
   {
    ResetGain(j);
   }
  }
  n=a.GetNoffsets();
  if (j<=n)
  {
   if (a.GetOffsetFlag(j))
   {
    SetOffset(a.GetOffset(j),j);
   }
   else
   {
    ResetOffset(j);
   } 
  }
  n=a.GetNcalflags();
  if (j<=n)
  {
   SetEdgeValue(a.GetEdgeValue(j),j);
   if (a.GetLockValue(j))
   {
    Lock(j);
   }
   else
   {
    Unlock(j);
   }
   if (a.GetDeadValue(j))
   {
    SetDead(j);
   }
   else
   {
    SetAlive(j);
   }
  }
  n=a.GetNnames();
  TString s;
  if (j<=n)
  {
   s=a.GetSlotName(j);
   SetSlotName(s,j);
  }
  n=a.GetNcalfuncs();
  if (j<=n)
  {
   TF1* f=a.GetCalFunction(j);
   SetCalFunction(f,j);
  }
  n=a.GetNdecalfuncs();
  if (j<=n)
  {
   TF1* f=a.GetDecalFunction(j);
   SetDecalFunction(f,j);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::Load(NcAttrib& a,TString name)
{
// Load attributes of the name-specified slot of the input NcAttrib into
// this NcAttrib object.
//
// This procedure involves a slot-index search based on the specified name
// at each invokation. This may become slow in case many slots have been
// defined and/or when this procedure is invoked many times.
// In such cases it is preferable to use indexed addressing in the user code
// either directly or via a few invokations of GetSlotIndex().

 Int_t j=GetSlotIndex(name);
 if (j>0) Load(a,j);
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetNcalfuncs() const
{
// Provide the number of specified calib. functions for this attribute.

 if (!fCalfuncs) return 0;

 Int_t n=0;
 for (Int_t i=1; i<=fCalfuncs->GetSize(); i++)
 {
  if (fCalfuncs->At(i-1)) n=i;
 }
 return n;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcAttrib::GetNdecalfuncs() const
{
// Provide the number of specified de-calib. functions for this attribute.

 if (!fDecalfuncs) return 0;

 Int_t n=0;
 for (Int_t i=1; i<=fDecalfuncs->GetSize(); i++)
 {
  if (fDecalfuncs->At(i-1)) n=i;
 }
 return n;
}
///////////////////////////////////////////////////////////////////////////
TF1* NcAttrib::GetCalFunction(Int_t j) const
{
// Provide pointer to the calib. function of the j-th (default j=1) slot.
// Note : The first attribute slot is at j=1.

 TF1* f=0;
 if (j>0 && j<=GetNcalfuncs()) f=(TF1*)fCalfuncs->At(j-1);
 return f;
}
///////////////////////////////////////////////////////////////////////////
TF1* NcAttrib::GetCalFunction(TString name) const
{
// Provide pointer to the calib. function of the name-specified slot.
// In case no match is found, zero is returned.

 TF1* f=0;
 Int_t j=GetSlotIndex(name);
 if (j>0) f=GetCalFunction(j);
 return f;
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetCalFunction(TF1* f,Int_t j)
{
// Set the calib. function of the j-th (default j=1) slot.
// Note : The first attribute slot is at j=1.
//
// In case the value of the index j exceeds the maximum number of reserved
// positions for the functions, the number of reserved positions for the functions
// is increased automatically.
//
// In case the function pointer argument has the same value as the current function
// pointer value, no action is taken since the user has already modified the actual
// function.
//
// In case the function pointer argument is zero, the current function
// is deleted and the pointer set to zero.
//
// In all other cases the current function is deleted and a new
// copy of the input function is created which becomes the current function.

 if (j<1) return;

 if (!fCalfuncs)
 {
  fCalfuncs=new TObjArray(j);
  fCalfuncs->SetOwner();
 }

 if (j > fCalfuncs->GetSize()) fCalfuncs->Expand(j);

 TF1* fcur=(TF1*)fCalfuncs->At(j-1);
 if (f != fcur)
 {
  if (fcur)
  {
   fCalfuncs->Remove(fcur);
   delete fcur;
   fcur=0;
  }
  if (f)
  {
   fcur=new TF1(*f);
   fCalfuncs->AddAt(fcur,j-1);
  }
 } 
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetCalFunction(TF1* f,TString name)
{
// Set the calib. function of the name-specified slot.
//
// In case the function pointer argument has the same value as the current function
// pointer value, no action is taken since the user has already modified the actual
// function.
//
// In case the function pointer argument is zero, the current function
// is deleted and the pointer set to zero.
//
// In all other cases the current function is deleted and a new
// copy of the input function is created which becomes the current function.

 Int_t j=GetSlotIndex(name);
 if (j>0) SetCalFunction(f,j);
}
///////////////////////////////////////////////////////////////////////////
TF1* NcAttrib::GetDecalFunction(Int_t j) const
{
// Provide pointer to the de-calib. function of the j-th (default j=1) slot.
// Note : The first attribute slot is at j=1.

 TF1* f=0;
 if (j>0 && j<=GetNdecalfuncs()) f=(TF1*)fDecalfuncs->At(j-1);
 return f;
}
///////////////////////////////////////////////////////////////////////////
TF1* NcAttrib::GetDecalFunction(TString name) const
{
// Provide pointer to the de-calib. function of the name-specified slot.
// In case no match is found, zero is returned.

 TF1* f=0;
 Int_t j=GetSlotIndex(name);
 if (j>0) f=GetDecalFunction(j);
 return f;
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetDecalFunction(TF1* f,Int_t j)
{
// Set the de-calib. function of the j-th (default j=1) slot.
// Note : The first attribute slot is at j=1.
//
// In case the value of the index j exceeds the maximum number of reserved
// positions for the functions, the number of reserved positions for the functions
// is increased automatically.
//
// In case the function pointer argument has the same value as the current function
// pointer value, no action is taken since the user has already modified the actual
// function.
//
// In case the function pointer argument is zero, the current function
// is deleted and the pointer set to zero.
//
// In all other cases the current function is deleted and a new
// copy of the input function is created which becomes the current function.

 if (j<1) return;

 if (!fDecalfuncs)
 {
  fDecalfuncs=new TObjArray(j);
  fDecalfuncs->SetOwner();
 }

 if (j > fDecalfuncs->GetSize()) fDecalfuncs->Expand(j);

 TF1* fcur=(TF1*)fDecalfuncs->At(j-1);
 if (f != fcur)
 {
  if (fcur)
  {
   fDecalfuncs->Remove(fcur);
   delete fcur;
   fcur=0;
  }
  if (f)
  {
   fcur=new TF1(*f);
   fDecalfuncs->AddAt(fcur,j-1);
  }
 } 
}
///////////////////////////////////////////////////////////////////////////
void NcAttrib::SetDecalFunction(TF1* f,TString name)
{
// Set the de-calib. function of the name-specified slot.
//
// In case the function pointer argument has the same value as the current function
// pointer value, no action is taken since the user has already modified the actual
// function.
//
// In case the function pointer argument is zero, the current function
// is deleted and the pointer set to zero.
//
// In all other cases the current function is deleted and a new
// copy of the input function is created which becomes the current function.

 Int_t j=GetSlotIndex(name);
 if (j>0) SetDecalFunction(f,j);
}
///////////////////////////////////////////////////////////////////////////
