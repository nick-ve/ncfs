/**  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
~~~
 * Copyright(c) 2016 NCFS/IIHE, All Rights Reserved.                           *
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
/** @class NcVeto
~~~
// Class NcVeto
// NcDevice derived class to serve as a generic interface for event vetoing procedures.
//
// This class provides a generic facility to store the resulting veto level of event vetoing
// procedures in a unique way in the event structure.
// The veto level is stored in the NcEvent structure via a device named "NcVeto" with a
// signal slot named "VetoLevel".
// This allows other processors (e.g. NcEventSelector) to access the event veto level
// and act upon this as requested by the user.
//
// In order to store the produced veto level correctly, the member function SetVetoLevel()
// of this class has to be invoked at the end of any event vetoing procedure.
//
// Note :
// ------ 
// Only one veto level is supported for each NcEvent, which implies that at every SetVetoLevel()
// invokation the veto level will be overwritten.
//
//--- Author: Nick van Eijndhoven 09-jun-2016 IIHE-VUB, Brussels
//- Modified: NvE $Date: 2016-08-24 13:11:33 +0200 (Wed, 24 Aug 2016) $ IIHE-VUB
~~~
**/
///////////////////////////////////////////////////////////////////////////
 
#include "NcVeto.h"
#include "Riostream.h"

ClassImp(NcVeto); // Class implementation to enable ROOT I/O

///////////////////////////////////////////////////////////////////////////
NcVeto::NcVeto() : NcDevice()
{
/**
~~~
// Default constructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
NcVeto::~NcVeto()
{
/**
~~~
// Default destructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
NcVeto::NcVeto(const NcVeto& v) : NcDevice(v)
{
/**
~~~
// Copy constructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
void NcVeto::StoreVetoLevel(NcEvent* evt,Double_t level)
{
/**
~~~
// Store the veto level in the event structure.
//
// Input arguments :
// -----------------
// evt   : Pointer to the NcEvent (or derived) object
// level : The user defined veto level
//
// The veto level will be stored in the event via an NcDevice with name "NcVeto"
// and the name of the signal slot is "VetoLevel".
// These standardised names allow the use of the veto level in other tasks
// like for instance NcEventSelector.
~~~
**/

 if (!evt) return;

 NcDevice* veto=evt->GetDevice("NcVeto");

 if (veto)
 {
  veto->AddNamedSlot("VetoLevel"); // To make sure the slot exists
  veto->SetSignal(level,"VetoLevel");
 }
 else
 {
  NcVeto params;
  params.SetNameTitle("NcVeto","Event veto processor result");
  params.AddNamedSlot("VetoLevel");
  params.SetSignal(level,"VetoLevel");

  evt->AddDevice(params);
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* NcVeto::Clone(const char* name) const
{
/**
~~~
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects. This feature allows e.g. IceEvent (or NcEvent)
// to store either IceGOM objects or objects derived from IceGOM
// via tha AddDevice memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 
~~~
**/

 NcVeto* v=new NcVeto(*this);
 if (name)
 {
  if (strlen(name)) v->SetName(name);
 }
 return v;
}
///////////////////////////////////////////////////////////////////////////
