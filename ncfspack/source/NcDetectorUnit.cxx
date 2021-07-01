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
// Class NcDetectorUnit
// Handling of a collection of generic devices.
//
// Please refer to the documentation of NcDetector for further details.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, June 22, 2021  08:40Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 1, 2021  21:20Z
///////////////////////////////////////////////////////////////////////////

#include "NcDetectorUnit.h"
#include "Riostream.h"
 
ClassImp(NcDetectorUnit) // Class implementation to enable ROOT I/O
 
NcDetectorUnit::NcDetectorUnit(const char* name,const char* title) : NcDevice(name,title)
{
// Default constructor.
//
// By default NO private copies will be made of the added devices.
// See the memberfunction SetDevCopy() to change this.

 fDevices=0;
 fDevCopy=0;
 fDevs=0;
 fThits=0;
 fOrdered=0;
 fDisplay=0;
}
///////////////////////////////////////////////////////////////////////////
NcDetectorUnit::~NcDetectorUnit()
{
// Default destructor.

 if (fDevices)
 {
  delete fDevices;
  fDevices=0;
 }
 if (fDevs)
 {
  delete fDevs;
  fDevs=0;
 }
 if (fThits)
 {
  delete fThits;
  fThits=0;
 }
 if (fOrdered)
 {
  delete fOrdered;
  fOrdered=0;
 }
 if (fDisplay)
 {
  delete fDisplay;
  fDisplay=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcDetectorUnit::NcDetectorUnit(const NcDetectorUnit& q) : NcDevice(q)
{
// Copy constructor.

 fDevCopy=q.fDevCopy;

 fDevs=0;
 fThits=0;
 fOrdered=0;
 fDisplay=0;

 fDevices=0;
 Int_t ndevs=q.fDevices->GetEntries();
 if (ndevs)
 {
  fDevices=new TObjArray(ndevs);
  if (fDevCopy) fDevices->SetOwner();
  for (Int_t i=1; i<=ndevs; i++)
  {
   NcDevice* dev=q.GetDevice(i);
   if (dev)
   {
    if (fDevCopy)
    {
     fDevices->Add(dev->Clone());
    }
    else
    {
     fDevices->Add(dev);
    }
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::SetDevCopy(Int_t j)
{
// (De)activate the creation of private copies of the added devices.
// j=0 ==> No private copies are made; pointers of original devices are stored.
// j=1 ==> Private copies of the devices are made and these pointers are stored.
//
// By default NO private copies will be made of the added devices.
//
// Notes :
//  In case a private copy is made, this is performed via the Clone() memberfunction.
//  All devices (i.e. classes derived from TObject) have the default TObject::Clone() 
//  memberfunction.
//  However, devices generally contain an internal (signal) data structure
//  which may include pointers to other objects. Therefore it is recommended to provide
//  for all devices a specific copy constructor and override the default Clone()
//  memberfunction using this copy constructor.
//  An example for this may be seen from NcCalorimeter.   
//
//  Once the storage contains pointer(s) to device(s) one cannot
//  change the DevCopy mode anymore.
//  To change the DevCopy mode for an existing NcDetectorUnit containing
//  devices one first has to invoke Reset().

 if (!fDevices)
 {
  if (j==0 || j==1)
  {
   fDevCopy=j;
  }
  else
  {
   cout << " *" << ClassName() << "::SetDevCopy* Invalid argument : " << j << endl;
  }
 }
 else
 {
  cout << " *" << ClassName() << "::SetDevCopy* Storage already contained devices."
       << "  ==> DevCopy mode not changed." << endl; 
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcDetectorUnit::GetDevCopy() const
{
// Provide value of the DevCopy mode.
// 0 ==> No private copies are made; pointers of original devices are stored.
// 1 ==> Private copies of the devices are made and these pointers are stored.
//
// Note :
// In case a private copy is made, this is performed via the Clone() memberfunction.
// All devices (i.e. classes derived from TObject) have the default TObject::Clone() 
// memberfunction.
// However, devices generally contain an internal (signal) data structure
// which may include pointers to other objects. Therefore it is recommended to provide
// for all devices a specific copy constructor and override the default Clone()
// memberfunction using this copy constructor.
// An example for this may be seen from NcCalorimeter.   

 return fDevCopy;
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::SetOwner(Bool_t own)
{
// Set ownership of all added objects. 
// The default parameter is own=kTRUE.
//
// Invokation of this memberfunction also sets all the copy modes
// (e.g. HitCopy and DevCopy) according to the value of own.
//
// This function (with own=kTRUE) is particularly useful when reading data
// from a tree/file, since Reset() will then actually remove all the
// added objects from memory irrespective of the copy mode settings
// during the tree/file creation process. In this way it provides a nice way
// of preventing possible memory leaks in the reading/analysis process.
//
// In addition this memberfunction can also be used as a shortcut to set all
// copy modes in one go during a tree/file creation process.
// However, in this case the user has to take care to only set/change the
// ownership (and copy mode) for empty objects (e.g. newly created objects
// or after invokation of the Reset() memberfunction) otherwise it will
// very likely result in inconsistent destructor behaviour.

 Int_t mode=1;
 if (!own) mode=0;
 if (fDevices) fDevices->SetOwner(own);
 fDevCopy=mode;

 NcDevice::SetOwner(own);
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::Reset(Int_t mode)
{
// Reset registered devices, hits and NcSignal attributes.
// Note : The status word and HitCopy flag are NOT modified.
//        Use SetStatus() and SetHitCopy() to modify these parameters. 
// See NcSignal::Reset() for further details.

 NcDevice::Reset(mode);

 if (fDevices)
 {
  delete fDevices;
  fDevices=0;
 }
 if (fThits)
 {
  delete fThits;
  fThits=0;
 }
 if (fOrdered)
 {
  delete fOrdered;
  fOrdered=0;
 }
 if (fDevs)
 {
  delete fDevs;
  fDevs=0;
 }
 if (fDisplay)
 {
  delete fDisplay;
  fDisplay=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::AddDevice(NcDevice& d)
{
// Add a device to the detector unit.
//
// Note :
// In case a private copy is made, this is performed via the Clone() memberfunction.
// All devices (i.e. classes derived from TObject) have the default TObject::Clone() 
// memberfunction.
// However, devices generally contain an internal (signal) data structure
// which may include pointers to other objects. Therefore it is recommended to provide
// for all devices a specific copy constructor and override the default Clone()
// memberfunction using this copy constructor.
// An example for this may be seen from NcCalorimeter.   

 if (!fDevices)
 {
  fDevices=new TObjArray();
  if (fDevCopy) fDevices->SetOwner();
 }
 
 // Add the device to this detector unit
 if (fDevCopy)
 {
  fDevices->Add(d.Clone());
 }
 else
 {
  fDevices->Add(&d);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::RemoveDevice(NcDevice* d)
{
// Remove the specified device from the detector unit.

 if (!fDevices || !d) return;

 TObject* obj=fDevices->Remove(d);
 if (obj)
 {
  if (fDevCopy) delete obj;
  fDevices->Compress();
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcDetectorUnit::GetNdevices(Bool_t follow)
{
// Provide the number of stored devices
//
// follow = kTRUE  ==> Count the number of all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only count the number of devices directly linked to this NcDetectorUnit

 Int_t ndevs=0;

 if (!follow)
 {
  if (fDevices) ndevs=fDevices->GetEntries();
 }
 else
 {
  TObjArray devs;
  LoadDevices("*",follow,&devs);
  ndevs=devs.GetEntries();
 }

 return ndevs;
} 
///////////////////////////////////////////////////////////////////////////
Int_t NcDetectorUnit::GetNdevices(TString classname,Bool_t follow,TObjArray* hits)
{
// Provide the number of devices of the specified class.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Count the number of all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only count the number of devices directly linked to this NcDetectorUnit
//
// In case an array "hits" is provided, the contents of the provided
// hit array are used to determine the number of different devices of the
// specified class to which the hits belong.
// Note that identification of the different hit parent devices only works
// for devices that have been given a unique identifier.
// In case no hit array is provided, just the number of stored devices of the
// specified class is returned. 
//
// By default hits=0.
 
 if (classname=="*") classname="NcDevice";

 Int_t ndevs=0;

 if (!hits) // Investigate the stored devices
 {
  TObjArray devs;
  LoadDevices(classname,follow,&devs);
  ndevs=devs.GetEntries();
 }
 else // Investigate the provided hit array
 {
  Int_t nh=hits->GetEntries();
  if (!nh) return 0;

  // Check for the encountered unique device identifiers of the specified class
  TArrayI devids(nh);
  Int_t idxmax=0;
  Int_t jdev=0;
  Int_t match=0;
  for (Int_t ih=0; ih<nh; ih++)
  {
   TObject* obj=hits->At(ih);
   if (!obj) continue;

   NcSignal* sx=0;
   if (obj->InheritsFrom("NcSignal")) sx=(NcSignal*)obj;
   if (!sx) continue;

   NcDevice* dev=sx->GetDevice();
   if (!dev) continue;
   if (!(dev->InheritsFrom(classname))) continue;

   // Update the number of devices
   jdev=dev->GetUniqueID();
   match=0;
   idxmax=ndevs;
   for (Int_t idx=0; idx<idxmax; idx++)
   {
    if (jdev==devids.At(idx))
    {
     match=1;
     break;
    }
   } 
   if (!match) // Device number was not present in array
   {
    ndevs++;
    devids.AddAt(jdev,ndevs-1);
   }
  }
 }

 return ndevs;
} 
///////////////////////////////////////////////////////////////////////////
NcDevice* NcDetectorUnit::GetDevice(Int_t i) const
{
// Return the i-th device of this detector unit.
// The first device corresponds to i=1.

 if (!fDevices)
 {
  return 0;
 }
 else
 {
  Int_t ndevs=fDevices->GetEntries();
  if (i<=0 || i>ndevs)
  {
   cout << " *" << ClassName() << "::GetDevice* Invalid argument i : " << i
        << " ndevs = " << ndevs << endl;
   return 0;
  }
  else
  {
   return (NcDevice*)fDevices->At(i-1);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
NcDevice* NcDetectorUnit::GetDevice(TString name,Bool_t follow)
{
// Return the device with name tag "name"
//
// follow = kTRUE  ==> Search all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only search the devices directly linked to this NcDetectorUnit

 if (!fDevices)
 {
  return 0;
 }
 else
 {
  TString s;
  TObjArray devs;
  LoadDevices("NcDevice",follow,&devs);
  Int_t ndevs=devs.GetEntries();
  for (Int_t i=0; i<ndevs; i++)
  {
   NcDevice* dev=(NcDevice*)devs.At(i);
   if (dev)
   {
    s=dev->GetName();
    if (s==name) return dev;
   }
  }

  return 0; // No matching name found
 }
}
///////////////////////////////////////////////////////////////////////////
NcDevice* NcDetectorUnit::GetIdDevice(Int_t id,Bool_t follow,TObjArray* devs)
{
// Return the NcDevice with identifier "id" from the specified array "devs".
// In case devs=0 (which is the default) the devices stored in the detector unit
// structure will be evaluated.
//
// follow = kTRUE  ==> Search all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only search the devices directly linked to this NcDetectorUnit
//
// Note : In case of multiple occurrences of identifier "id", the first
//        encountered matching device will be returned.

 TObjArray temp;

 TObjArray* arr=devs;
 if (!arr)
 {
  if (!follow)
  {
   arr=fDevices;
  }
  else
  {
   arr=&temp;
   LoadDevices("NcDevice",follow,arr);
  }
 }

 if (!arr || id<0) return 0;

 Int_t idx=0;
 for (Int_t i=0; i<arr->GetSize(); i++)
 {
  TObject* obj=arr->At(i);

  if (!obj) continue;

  NcDevice* dev=0;
  if (obj->InheritsFrom("NcDevice")) dev=(NcDevice*)obj;
  if (dev)
  {
   idx=dev->GetUniqueID();
   if (idx==id) return dev;
  }
 }
 return 0; // No matching id found
}
///////////////////////////////////////////////////////////////////////////
NcDevice* NcDetectorUnit::GetIdDevice(Int_t id,TString classname,Bool_t follow)
{
// Return the device with identifier "id" of the specified class.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Search all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only search the devices directly linked to this NcDetectorUnit
//
// Note : In case of multiple occurrences of identifier "id", the first
//        encountered matching device will be returned.

 if (!fDevices || id<0) return 0;

 if (classname=="*") classname="NcDevice";

 TObjArray temp;

 TObjArray* arr=fDevices;
 if (follow)
 {
  arr=&temp;
  LoadDevices(classname,follow,arr);
 }

 Int_t idx=0;
 for (Int_t i=0; i<arr->GetEntries(); i++)
 {
  NcDevice* dev=(NcDevice*)arr->At(i);
  if (dev)
  {
   idx=dev->GetUniqueID();
   if (idx==id && dev->InheritsFrom(classname)) return dev;
  }
 }
 return 0; // No matching id found for the specified class
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDetectorUnit::GetDevices(TString classname,Bool_t follow,TObjArray* devices)
{
// Provide the references to the various devices derived from the specified class.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Search all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only search the devices directly linked to this NcDetectorUnit
//
// Note :
// ------
// In case devices=0 the selected device pointers are returned via a multi-purpose array,
// which will be overwritten by subsequent selections.
// It is recommended to provide a user defined array via the argument "devices" in order to
// omit the danger of overwriting the selection and to allow to use the selected device list
// amongst other selections.
// In case a user defined array "devices" is provided, this memberfunction returns 0 for the
// return argument.
//
// The default is devices=0.

 if (classname=="*") classname="NcDevice";

 TObjArray* arr=0;

 if (devices)
 {
  devices->Clear();
  arr=devices;
 }
 else
 {
  if (fDevs)
  {
   fDevs->Clear();
  }
  else
  {
   fDevs=new TObjArray();
  }
  arr=fDevs;
 }

 LoadDevices(classname,follow,arr);

 if (devices)
 {
  return 0;
 }
 else
 {
  return fDevs;
 }
}
///////////////////////////////////////////////////////////////////////////
NcDetectorUnit* NcDetectorUnit::GetDetectorUnit(TString name,Bool_t follow)
{
// Return the detector unit with name tag "name"
//
// follow = kTRUE  ==> Search all detector units related this NcDetectorUnit at any level
//          kFALSE ==> Only search the detector units directly linked to this NcDetectorUnit

 if (!fDevices)
 {
  return 0;
 }
 else
 {
  TString s;
  TObjArray devs;
  LoadDevices("NcDetectorUnit",follow,&devs);
  Int_t ndevs=devs.GetEntries();
  for (Int_t i=0; i<ndevs; i++)
  {
   NcDevice* dev=(NcDevice*)devs.At(i);
   if (dev)
   {
    if (!(dev->InheritsFrom("NcDetectorUnit"))) continue;
    s=dev->GetName();
    if (s==name) return (NcDetectorUnit*)dev;
   }
  }

  return 0; // No matching name found
 }
}
///////////////////////////////////////////////////////////////////////////
NcDetectorUnit* NcDetectorUnit::GetIdDetectorUnit(Int_t id,Bool_t follow,TObjArray* devs)
{
// Return the detector unit with identifier "id" from the specified array "devs".
// In case devs=0 (which is the default) the devices stored in the current detector unit
// structure will be evaluated.
//
// follow = kTRUE  ==> Search all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only search the devices directly linked to this NcDetectorUnit
//
// Note : In case of multiple occurrences of identifier "id", the first
//        encountered matching device will be returned.

 TObjArray temp;

 TObjArray* arr=devs;
 if (!arr)
 {
  if (!follow)
  {
   arr=fDevices;
  }
  else
  {
   arr=&temp;
   LoadDevices("NcDetectorUnit",follow,arr);
  }
 }

 if (!arr || id<0) return 0;

 Int_t idx=0;
 for (Int_t i=0; i<arr->GetSize(); i++)
 {
  TObject* obj=arr->At(i);

  if (!obj) continue;

  NcDetectorUnit* dev=0;
  if (obj->InheritsFrom("NcDetectorUnit")) dev=(NcDetectorUnit*)obj;
  if (dev)
  {
   idx=dev->GetUniqueID();
   if (idx==id) return dev;
  }
 }
 return 0; // No matching id found
}
///////////////////////////////////////////////////////////////////////////
NcDetectorUnit* NcDetectorUnit::GetIdDetectorUnit(Int_t id,TString classname,Bool_t follow)
{
// Return the detector unit with identifier "id" of the specified class.
// For classname="*", no selection on the detector unit class will be performed.
//
// follow = kTRUE  ==> Search all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only search the devices directly linked to this NcDetectorUnit
//
// Note : In case of multiple occurrences of identifier "id", the first
//        encountered matching detector unit will be returned.

 if (!fDevices || id<0) return 0;

 if (classname=="*") classname="NcDetectorUnit";

 TObjArray temp;

 TObjArray* arr=fDevices;
 if (follow)
 {
  arr=&temp;
  LoadDevices(classname,follow,arr);
 }

 Int_t idx=0;
 for (Int_t i=0; i<arr->GetEntries(); i++)
 {
  NcDevice* dev=(NcDevice*)arr->At(i);
  if (dev)
  {
   if (!(dev->InheritsFrom("NcDetectorUnit"))) continue;
   idx=dev->GetUniqueID();
   if (idx==id && dev->InheritsFrom(classname)) return (NcDetectorUnit*)dev;
  }
 }
 return 0; // No matching id found for the specified class
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::Data(TString f,TString u)
{
// Provide detector unit information within the coordinate frame f
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The defaults are f="car" and u="rad".

 const char* name=GetName();
 const char* title=GetTitle();
 cout << " *" << ClassName() << "::Data* Id : " << GetUniqueID();
 if (strlen(name))  cout << " Name : " << GetName();
 if (strlen(title)) cout << " Title : " << GetTitle();
 cout << endl;

 // Provide an overview of the stored devices
 ShowDevices(0);
 if (GetNdevices(kFALSE))
 {
  cout << " More detailed info may be obtained by invokation of ShowDevices()." << endl;
  cout << " For the data of these contained devices, please use the corresponding investigation tools." << endl;
 }

 cout << " === Data of the current detector unit object ===" << endl;

 if (fNv)
 {
  cout << "   Position";
  NcPosition::Data(f,u);
 }

 if (fDevice)
 {
  const char* devname=fDevice->GetName();
  const char* devtitle=fDevice->GetTitle();
  cout << "   Owned by device : " << fDevice->ClassName() << " Id : " << fDevice->GetUniqueID();
  if (strlen(devname))  cout << " Name : " << devname;
  if (strlen(devtitle)) cout << " Title : " << devtitle;
  cout << endl;
 }

 // Provide an overview of the stored waveforms
 ListWaveform(-1);

 // Provide an overview of the stored samples
 ListSample(-1);

 // Provide an overview of the associated tracks
 ListTrack(-1);

 // Provide an overview of all the data and attribute slots
 List(-1);

 // Provide an overview of the stored hits
 Int_t nhits=GetNhits();
 if (nhits)
 {
  cout << " The following " << nhits << " hits are registered for this object : " << endl;
  ShowHit(0,f,u);
 }
 else
 {
  cout << " No hits have been registered for this object." << endl;
 }
} 
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::ShowDevices(Int_t mode,Bool_t header)
{
// Provide an overview of the available devices.
// The input argument "header" determines whether header info is printed (kTRUE) or not (kFALSE).
//
// The argument mode determines the amount of information as follows :
// mode = 0 ==> Only printout of the number of directly linked devices
//        1 ==> Provide a listing with 1 line of info for each directly linked device
//        2 ==> Provide a listing with 1 line of info for each linked device at any level
//
// The default values are mode=1 and header=kTRUE.

 ShowDevices("*",mode,header);
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::ShowDevices(TString classname,Int_t mode,Bool_t header)
{
// Provide an overview of the available devices of the specified (or derived) class.
// For classname="*", no selection on the device class will be performed.
// The input argument "header" determines whether header info is printed (kTRUE) or not (kFALSE).
//
// The argument mode determines the amount of information as follows :
// mode = 0 ==> Only printout of the number of directly linked devices
//        1 ==> Provide a listing with 1 line of info for each directly linked device
//        2 ==> Provide a listing with 1 line of info for each linked device at any level
//
// The default values are mode=1 and header=kTRUE.

 Bool_t follow=kFALSE;
 if (mode==2) follow=kTRUE;

 Int_t ndevs=GetNdevices(classname,follow);

 if (header)
 {
  cout << " *" << ClassName() <<"::ShowDevices* For mode=" << mode << " there are in total " << ndevs;
  if (classname!="*") cout << " " << classname << " (derived)";
  cout << " devices found." << endl;
 }

 if (classname=="*") classname="NcDevice";

 ShowDevicesTree(classname,mode);
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::ShowDevicesTree(TString classname,Int_t mode,NcDetectorUnit* unit)
{
// Internal memberfunction to provide an overview of the available devices of the specified class.
// For classname="*", no selection on the device class will be performed.
//
// The argument mode determines the amount of information as follows :
// mode = 0 ==> Only printout of the number of directly linked devices
//        1 ==> Provide a listing with 1 line of info for each directly linked device
//        2 ==> Provide a listing with 1 line of info for each device at any level
//
// This memberfunction is invoked recursively to access also the devices of stored NcDetectorUnit objects (if any).
//
// The defaults are mode=1 and unit=0.
 
 if (classname=="*") classname="NcDevice";

 if (!unit) unit=this;

 TObjArray units;

 TString name=unit->GetName();

 TString type="NcDetectorUnit";
 if (unit->InheritsFrom("NcDetector")) type="NcDetector";

 Int_t ndevs=unit->GetNdevices(kFALSE);
 if (!ndevs)
 {
  cout << " No devices present for " << type << " : " << name << endl;
  return;
 }

 Int_t ndevs2=unit->GetNdevices(classname,kFALSE);

 if (mode<2)
 {
  cout << " There are " << ndevs2;
  if (classname!="NcDevice") cout << " " << classname << " (derived)";
  cout << " devices available for the " << type << " : " << name << endl;
  if (!mode) return;
 }

 if (ndevs2)
 {
  cout << " The following " << ndevs2;
  if (classname!="NcDevice") cout << " " << classname << " (derived)";
  cout << " devices are available for the " << type << " : " << name << endl;
 }

 Int_t nd=0;
 Int_t nh=0;
 Int_t nw=0;
 Int_t ns=0;
 NcDevice* dev=0;
 NcDetectorUnit* du=0;
 for (Int_t i=1; i<=ndevs; i++)
 {
  dev=unit->GetDevice(i);
 
  if (!dev) continue;

  if (dev->InheritsFrom(classname))
  {
   type="NcDevice";
   if (dev->InheritsFrom("NcDetectorUnit")) type="NcDetectorUnit";
   if (dev->InheritsFrom("NcDetector")) type="NcDetector";
   name=dev->GetName();
   cout << " Device number : " << i;
   cout << " Class : " << dev->ClassName();
   if (type!=(dev->ClassName())) cout << " (" << type << ")";
   cout << " Id : " << dev->GetUniqueID();
   if (name!="") cout << " Name : " << name;
   du=0;
   if (dev->InheritsFrom("NcDetectorUnit"))
   {
    du=(NcDetectorUnit*)dev;
    nd=du->GetNdevices(kFALSE);
    if (nd) cout << " Ndevices : " << nd;
   }
   nh=dev->GetNhits();
   if (nh) cout << " Nhits : " << nh;
   nw=dev->GetNwaveforms();
   if (nw) cout << " Nwaveforms : " << nw;
   ns=dev->GetNsamples();
   if (ns) cout << " Nsamples : " << ns;
   cout << endl;
  }

  if (mode==2 && dev->InheritsFrom("NcDetectorUnit")) units.Add(dev);
 }

 // Recursive invokation of the encountered detector units
 for (Int_t i=0; i<units.GetEntries(); i++)
 {
  du=(NcDetectorUnit*)units.At(i);
  if (du) ShowDevicesTree(classname,mode,du);
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcDetectorUnit::GetNhitsDevices(TString classname,Bool_t follow,Bool_t inc)
{
// Provide the number of hits registered to the specified device (or derived) class.
// The specified device class has to be derived from NcDevice.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Search all hits related this NcDetectorUnit at any level
//          kFALSE ==> Only search the devices directly linked to this NcDetectorUnit
//
// inc = kFALSE --> Do NOT include the hits stored in the NcDetectorUnit itself
//       kTRUE  --> Include the hits stored in the NcDetectorUnit itself
//                  if it satisfies the specified device class. 
//
// It is possible to indicate with the argument "classname" a specific
// device instead of a whole class of devices. However, in such a case
// it is more efficient to use the GetDevice() memberfunction directly.

 if (classname=="*") classname="NcDevice";

 TObjArray hits;
 GetHitsDevices(classname,follow,inc,&hits);
 Int_t nhits=hits.GetEntries();
 return nhits;
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDetectorUnit::GetHitsDevices(TString classname,Bool_t follow,Bool_t inc,TObjArray* hits,TString name,Int_t mode,Int_t opt)
{
// Provide the references to all the hits registered to the device (or derived) class
// as specified by the input argument "classname".
// The specified device class has to be derived from NcDevice.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Search all hits related this NcDetectorUnit at any level
//          kFALSE ==> Only search the hits directly linked to this NcDetectorUnit
//
// inc  = kFALSE --> Do NOT include the hits stored in the NcDetectorUnit itself
//        kTRUE  --> Include the hits stored in the NcDetectorUnit itself
//                   if it satisfies the specified device class. 
//
// It is possible to indicate with the argument "classname" a specific
// device instead of a whole class of devices. However, in such a case
// it is more efficient to use the GetDevice() memberfunction directly.
//
// The argument "hits" may be used to retreive the required hit pointers.
// In case hits=0 the selected hit pointers are returned via a multi-purpose array,
// which will be overwritten by subsequent selections.
// It is recommended to provide a user defined array via the argument "hits" 
// to omit the danger of overwriting the selection and to allow to use the selected hit list
// amongst other selections.
//
// The input arguments "inc", "name", "mode" and "opt" allow for further selection criteria
// as indicated below.
//
// name : The user provided hit or signal slot name to be used for the hit selection.
//        If name="none", no selections will be made on the name matching. 
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
// The defaults are : hits=0, name="none", mode=0 and opt=0.

 if (classname=="*") classname="NcDevice";

 LoadHits(classname,follow,inc,hits);

 // Check for further hit selection criteria
 if (name!="none")
 {
  TObjArray* selected=hits;
  if (!selected) selected=fThits;
  TObjArray ahits(*selected);
  GetHits(*selected,name,mode,opt,&ahits);
 }

 if (hits)
 {
  return 0;
 }
 else
 {
  return fThits;
 }
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcDetectorUnit::GetIdHit(Int_t id,TString classname,Bool_t follow,Bool_t inc)
{
// Return the hit with unique identifier "id" for the specified device (or derived) class.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Search all hits related this NcDetectorUnit at any level
//          kFALSE ==> Only search the hits directly linked to this NcDetectorUnit
//
// inc = kFALSE --> Do NOT include the hits stored in the NcDetectorUnit itself
//       kTRUE  --> Include the hits stored in the NcDetectorUnit itself
//                  if it satisfies the specified device class. 

 if (id<0) return 0;

 if (classname=="*") classname="NcDevice";

 TObjArray hits;
 LoadHits(classname,follow,inc,&hits);

 Int_t nhits=hits.GetEntries();

 if (!nhits) return 0;

 NcSignal* sx=0;
 Int_t sid=0;
 for (Int_t i=0; i<nhits; i++)
 {
  sx=(NcSignal*)hits.At(i);
  if (sx)
  {
   sid=sx->GetUniqueID();
   if (id==sid) return sx;
  }
 }
 return 0; // No matching id found
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::ShowHits(TString classname,Bool_t follow,Bool_t inc,Int_t mode,TString f,TString u)
{
// Show all the hits registered to the specified device (or derived) class.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Search all hits related this NcDetectorUnit at any level
//          kFALSE ==> Only search the hits directly linked to this NcDetectorUnit
//
// inc  = kFALSE --> Do NOT include the hits stored in the NcDetectorUnit itself
//        kTRUE  --> Include the hits stored in the NcDetectorUnit itself
//                   if it satisfies the specified device class. 
//
// mode = 0 ==> Only the number of hits will be provided.
//        1 ==> Full listing of all the hits will be provided.
//        2 ==> Same as mode=1 but with additional location info of the owning device.
//
// The arguments "f" and "u" have the same meaning as in the memberfunction Data(). 
//
// Default values are mode=1, f="car" and u="rad".
//
// Note : This memberfunction will show hits in printable format.
//        To obtain a graphic hit display please refer to DisplayHits(). 

 if (classname=="*") classname="NcDevice";

 TObjArray hits;
 LoadHits(classname,follow,inc,&hits);

 Int_t nhits=hits.GetEntries();

 cout << " *" << ClassName() << "::ShowHits* There are " << nhits
      << " hits recorded for device class " << classname << endl;

 if (!nhits || !mode) return;

 NcPosition r;
 for (Int_t i=0; i<hits.GetEntries(); i++)
 {
  NcSignal* sx=(NcSignal*)hits.At(i);
  if (sx) sx->Data(f,u);
  if (mode==2)
  {
   NcDevice* dev=(NcDevice*)sx->GetDevice();
   if (!dev) continue;
   r=dev->GetPosition();
   cout << "   Device Position";
   r.Data(f,u);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDetectorUnit::SortHits(TString classname,Bool_t follow,Bool_t inc,Int_t idx,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various hits registered to the specified device (or derived) class.
// For classname="*", no selection on the device class will be performed.
//
// The ordered array is returned as a TObjArray either via a user provided array "ordered"
// or as a returned pointer.
//
// follow = kTRUE  ==> Search all hits related this NcDetectorUnit at any level
//          kFALSE ==> Only search the hits directly linked to this NcDetectorUnit
//
// inc = kFALSE --> Do NOT include the hits stored in the NcDetectorUnit itself
//       kTRUE  --> Include the hits stored in the NcDetectorUnit itself
//                  if it satisfies the specified device class. 
//
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
//
// For more extended functionality see class NcDevice.

 if (classname=="*") classname="NcDevice";

 if (ordered) ordered->Clear();

 if (idx<=0 || abs(mode)!=1) return 0;

 TObjArray hits;
 LoadHits(classname,follow,inc,&hits);

 TObjArray* arr=SortHits(idx,mode,&hits,mcal,deadcheck,ordered);

 if (ordered) return 0;

 if (fThits)
 {
  delete fThits;
  fThits=0;
 }
 if (arr) fThits=new TObjArray(*arr);
 return fThits;
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDetectorUnit::SortHits(TString classname,Bool_t follow,Bool_t inc,TString name,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various hits registered to the specified device (or derived) class.
// For classname="*", no selection on the device class will be performed.
//
// The ordered array is returned as a TObjArray either via a user provided array "ordered"
// or as a returned pointer.
//
// follow = kTRUE  ==> Search all hits related this NcDetectorUnit at any level
//          kFALSE ==> Only search the hits directly linked to this NcDetectorUnit
//
// inc = kFALSE --> Do NOT include the hits stored in the NcDetectorUnit itself
//       kTRUE  --> Include the hits stored in the NcDetectorUnit itself
//                  if it satisfies the specified device class. 
//
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
//
// For more extended functionality see class NcDevice.

 if (classname=="*") classname="NcDevice";

 if (ordered) ordered->Clear();
 
 if (abs(mode)!=1) return 0;

 TObjArray hits;
 LoadHits(classname,follow,inc,&hits);

 TObjArray* arr=SortHits(name,mode,&hits,mcal,deadcheck,ordered);

 if (ordered) return 0;

 if (fThits)
 {
  delete fThits;
  fThits=0;
 }
 if (arr) fThits=new TObjArray(*arr);
 return fThits;
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::DisplayHits(TString classname,Bool_t follow,Bool_t inc,Int_t idx,Float_t scale,Int_t dp,Int_t mode,Int_t mcol)
{
// 3D color display of the various hits registered to the specified device (or derived) class.
// For classname="*", no selection on the device class will be performed.
//
// The user can specify the index (default=1) of the signal slot to perform the display for.
// The marker size will indicate the absolute value of the signal (specified by the slotindex)
// as a percentage of the input argument "scale".
// In case scale<0 the maximum absolute signal value encountered in the hit array will be used
// to define the 100% scale. The default is scale=-1.
// In case dp=1 the owning device position will be used, otherwise the hit position will
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
// For more extended functionality see class NcDevice.
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

 if (idx<=0) return;

 if (classname=="*") classname="NcDevice";

 TObjArray hits;
 GetHitsDevices(classname,follow,inc,&hits);

 NcDevice* dev=new NcDevice();
 dev->DisplayHits(idx,scale,&hits,dp,mode,mcol);

 if (fDisplay)
 {
  delete fDisplay;
  fDisplay=0;
 }
 fDisplay=dev;
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::DisplayHits(TString classname,Bool_t follow,Bool_t inc,TString name,Float_t scale,Int_t dp,Int_t mode,Int_t mcol)
{
// 3D color display of the various hits registered to the specified device (or derived) class.
// For classname="*", no selection on the device class will be performed.
//
// The user can specify the name of the signal slot to perform the display for.
// The marker size will indicate the absolute value of the signal (specified by the slotname)
// as a percentage of the input argument "scale".
// In case scale<0 the maximum absolute signal value encountered in the hit array will be used
// to define the 100% scale. The default is scale=-1.
// In case dp=1 the owning device position will be used, otherwise the hit position will
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
// For more extended functionality see class NcDevice.
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

 if (classname=="*") classname="NcDevice";

 TObjArray hits;
 GetHitsDevices(classname,follow,inc,&hits,name,1);

 NcDevice* dev=new NcDevice();
 dev->DisplayHits(name,scale,&hits,dp,mode,mcol);

 if (fDisplay)
 {
  delete fDisplay;
  fDisplay=0;
 }
 fDisplay=dev;
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDetectorUnit::SortDevices(TString classname,Bool_t follow,Bool_t inc,TString name,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various devices based on hit signals registered
// to the specified device (or derived) class.
// For classname="*", no selection on the device class will be performed.
//
// The ordered array is returned as a TObjArray either via a user provided array "ordered"
// or as a returned pointer.
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
// In case ordered=0 the ordered device pointers are returned via a multi-purpose array,
// which may be overwritten by other memberfunctions (not restricted to device ordering).
// It is recommended to provide a user defined array via the argument "ordered" to omit
// the danger of overwriting (or being overwritten by) other selections and to allow to use
// the ordered device list amongst other selections.
// In case a user defined array "ordered" is provided, this memberfunction returns 0 for the
// return argument.
//
// The default is ordered=0.

 if (classname=="*") classname="NcDevice";

 if (ordered) ordered->Clear();

 TObjArray hits;
 SortHits(classname,follow,inc,name,mode,mcal,deadcheck,&hits);
 
 TObjArray* devs=SortDevices(&hits,"*",0,mcal,deadcheck,ordered);

 if (ordered)
 {
  return 0;
 }
 else
 {
  return devs;
 }
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDetectorUnit::SortDevices(TString classname,Bool_t follow,Bool_t inc,Int_t idx,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various devices based on hit signals registered
// to the specified device (or derived) class.
// For classname="*", no selection on the device class will be performed.
//
// The ordered array is returned as a TObjArray either via a user provided array "ordered"
// or as a returned pointer.
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
// In case ordered=0 the ordered device pointers are returned via a multi-purpose array,
// which may be overwritten by other memberfunctions (not restricted to device ordering).
// It is recommended to provide a user defined array via the argument "ordered" to omit
// the danger of overwriting (or being overwritten by) other selections and to allow to use
// the ordered device list amongst other selections.
// In case a user defined array "ordered" is provided, this memberfunction returns 0 for the
// return argument.
//
// The default is ordered=0.

 if (ordered) ordered->Clear();

 if (classname=="*") classname="NcDevice";

 TObjArray hits;
 SortHits(classname,follow,inc,idx,mode,mcal,deadcheck,&hits);
 
 TObjArray* devs=SortDevices(&hits,0,0,mcal,deadcheck,ordered);

 if (ordered)
 {
  return 0;
 }
 else
 {
  return devs;
 }
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcDetectorUnit::SortDevices(TObjArray* hits,TString name,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various devices based on hit signals contained
// in the input array.
// The ordered array is returned as a TObjArray either via a user provided array "ordered"
// or as a returned pointer.
// A "hit" represents an abstract object which is derived from NcSignal.
// The user can specify the name of the signal slot to perform the sorting on.
// In case no matching slotname is found, the signal will be skipped.
// Via the "mode" argument the user can specify ordering in decreasing
// order (mode=-1), ordering in increasing order (mode=1) or no ordering (mode=0).
// The latter option provides a means to quickly obtain an ordered devices list
// when the hits in the array were already ordered by the user. In this case
// the input argument "name" is irrelevant.
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
// In case ordered=0 the ordered device pointers are returned via a multi-purpose array,
// which may be overwritten by other memberfunctions (not restricted to device ordering).
// It is recommended to provide a user defined array via the argument "ordered" to omit
// the danger of overwriting (or being overwritten by) other selections and to allow to use
// the ordered device list amongst other selections.
// In case a user defined array "ordered" is provided, this memberfunction returns 0 for the
// return argument.
//
// The default is ordered=0.

 if (ordered)
 {
  ordered->Clear();
 }
 else
 {
  if (fOrdered)
  {
   fOrdered->Clear();
  }
  else
  {
   fOrdered=new TObjArray();
  }
 }

 if (!hits) return 0;

 TObjArray* sorthits=new TObjArray(*hits);
 if (mode) SortHits(name,mode,hits,mcal,deadcheck,sorthits);

 TObjArray* arr=0; // Generic array pointer to be used in the logic below
 
 if (ordered)
 {
  arr=ordered;
 }
 else
 {
  arr=fOrdered;
 } 

 Int_t nhits=sorthits->GetEntries();
 Int_t exist=0;
 for (Int_t ih=0; ih<nhits; ih++)
 {
  NcSignal* sx=(NcSignal*)sorthits->At(ih);
  if (!sx) continue;
  NcDevice* dx=sx->GetDevice();
  exist=0;
  for (Int_t id=0; id<arr->GetEntries(); id++)
  {
   NcDevice* odx=(NcDevice*)arr->At(id);
   if (dx==odx)
   {
    exist=1;
    break;
   }
  }
  if (!exist) arr->Add(dx);
 }

 delete sorthits;
 
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
TObjArray* NcDetectorUnit::SortDevices(TObjArray* hits,Int_t idx,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various devices based on hit signals contained
// in the input array.
// The ordered array is returned as a TObjArray either via a user provided array "ordered"
// or as a returned pointer.
// A "hit" represents an abstract object which is derived from NcSignal.
// The user can specify the index of the signal slot to perform the sorting on.
// By default the slotindex will be 1.
// Via the "mode" argument the user can specify ordering in decreasing
// order (mode=-1), ordering in increasing order (mode=1) or no ordering (mode=0).
// The latter option provides a means to quickly obtain an ordered devices list
// when the hits in the array were already ordered by the user. In this case
// the input argument "idx" is irrelevant.
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
// In case ordered=0 the ordered device pointers are returned via a multi-purpose array,
// which may be overwritten by other memberfunctions (not restricted to device ordering).
// It is recommended to provide a user defined array via the argument "ordered" to omit
// the danger of overwriting (or being overwritten by) other selections and to allow to use
// the ordered device list amongst other selections.
// In case a user defined array "ordered" is provided, this memberfunction returns 0 for the
// return argument.
//
// The default is ordered=0.

 if (ordered)
 {
  ordered->Clear();
 }
 else
 {
  if (fOrdered)
  {
   fOrdered->Clear();
  }
  else
  {
   fOrdered=new TObjArray();
  }
 }

 if (!hits) return 0;

 TObjArray* sorthits=new TObjArray(*hits);
 if (mode) SortHits(idx,mode,hits,mcal,deadcheck,sorthits);

 TObjArray* arr=0; // Generic array pointer to be used in the logic below
 
 if (ordered)
 {
  arr=ordered;
 }
 else
 {
  arr=fOrdered;
 } 

 Int_t nhits=sorthits->GetEntries();
 Int_t exist=0;
 for (Int_t ih=0; ih<nhits; ih++)
 {
  NcSignal* sx=(NcSignal*)sorthits->At(ih);
  if (!sx) continue;
  NcDevice* dx=sx->GetDevice();
  exist=0;
  for (Int_t id=0; id<arr->GetEntries(); id++)
  {
   NcDevice* odx=(NcDevice*)arr->At(id);
   if (dx==odx)
   {
    exist=1;
    break;
   }
  }
  if (!exist) arr->Add(dx);
 }

 delete sorthits;
 
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
void NcDetectorUnit::LoadHits(TString classname,Bool_t follow,Bool_t inc,TObjArray* hits,NcDetectorUnit* unit)
{
// Internal memberfunction to load the references to the various hits registered to the specified device (or derived) class.
// The specified device class has to be derived from NcDevice.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Load all hits related to this NcDetectorUnit at any level
//          kFALSE ==> Only load the hits directly linked to this NcDetectorUnit
//
// inc = kFALSE --> Do NOT include the hits stored in the NcDetectorUnit itself
//       kTRUE  --> Include the hits stored in the NcDetectorUnit itself
//                  if it satisfies the specified device class. 
//
// This memberfunction is invoked recursively to access also the hit data of stored NcDetectorUnit objects (if any).
//
// Note :
// ------
// In case hits=0 the selected hit pointers are returned via a multi-purpose array,
// which will be overwritten by subsequent selections.
// It is recommended to provide a user defined array via the argument "hits" 
// to omit the danger of overwriting the selection and to allow to use the selected hit list
// amongst other selections.
//
// The default are hits=0 and unit=0.

 if (classname=="*") classname="NcDevice";

 if (!unit) // Direct invokation, not recursive
 {
  unit=this;

  if (hits)
  {
   hits->Clear();
  }
  else
  {
   if (fThits)
   {
    fThits->Clear();
   }
   else
   {
    fThits=new TObjArray();
   }
  }
 }

 NcSignal* sx=0;
 if (inc && unit->InheritsFrom(classname))
 {
  for (Int_t ih=1; ih<=unit->GetNhits(); ih++)
  {
   sx=unit->GetHit(ih);
   if (!sx) continue;
   if (hits)
   {
    hits->Add(sx);
   }
   else
   {
    fThits->Add(sx);
   }
  }
 }

 TObjArray units;

 NcDevice* dev=0;
 NcDetectorUnit* du=0;
 for (Int_t idev=1; idev<=unit->GetNdevices(kFALSE); idev++)
 {
  dev=unit->GetDevice(idev);
  if (!dev) continue;

  if (follow && dev->InheritsFrom("NcDetectorUnit")) units.Add(dev);

  if (!(unit->InheritsFrom(classname)) && !(dev->InheritsFrom(classname))) continue;
  
  for (Int_t ih=1; ih<=dev->GetNhits(); ih++)
  {
   sx=dev->GetHit(ih);
   if (!sx) continue;
   if (hits)
   {
    hits->Add(sx);
   }
   else
   {
    fThits->Add(sx);
   }
  }
 }

 // Recursive invokation of the encountered detector units
 for (Int_t i=0; i<units.GetEntries(); i++)
 {
  du=(NcDetectorUnit*)units.At(i);
  if (du) LoadHits(classname,follow,inc,hits,du);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDetectorUnit::LoadDevices(TString classname,Bool_t follow,TObjArray* devs,NcDetectorUnit* unit)
{
// Internal memberfunction to load the references to the various devices derived from the specified device class.
// The specified device class has to be derived from NcDevice.
// For classname="*", no selection on the device class will be performed.
//
// follow = kTRUE  ==> Load all devices related this NcDetectorUnit at any level
//          kFALSE ==> Only load the devices directly linked to this NcDetectorUnit
//
// This memberfunction is invoked recursively to access also the device data of stored NcDetectorUnit objects (if any).
//
// Note :
// ------
// In case devs=0 the selected device pointers are returned via a multi-purpose array,
// which will be overwritten by subsequent selections.
// It is recommended to provide a user defined array via the argument "devs" 
// to omit the danger of overwriting the selection and to allow to use the selected hit list
// amongst other selections.
//
// The default are devs=0 and unit=0.

 if (classname=="*") classname="NcDevice";

 if (!unit) // Direct invokation, not recursive
 {
  unit=this;

  if (devs)
  {
   devs->Clear();
  }
  else
  {
   if (fDevs)
   {
    fDevs->Clear();
   }
   else
   {
    fDevs=new TObjArray();
   }
  }
 }

 TObjArray units;

 Int_t ndevs=unit->GetNdevices(kFALSE);

 if (!ndevs) return;

 NcDevice* dev=0;
 NcDetectorUnit* du=0;
 for (Int_t i=1; i<=ndevs; i++)
 {
  dev=unit->GetDevice(i);
 
  if (!dev) continue;

  if (follow && dev->InheritsFrom("NcDetectorUnit")) units.Add(dev);

  if (dev->InheritsFrom(classname))
  {
   if (devs)
   {
    devs->Add(dev);
   }
   else
   {
    fDevs->Add(dev);
   }
  }
 }

 // Recursive invokation of the encountered detector units
 for (Int_t i=0; i<units.GetEntries(); i++)
 {
  du=(NcDetectorUnit*)units.At(i);
  if (du) LoadDevices(classname,follow,devs,du);
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* NcDetectorUnit::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects. This feature allows e.g. NcEvent to store
// either NcDetectorUnit objects or objects derived from NcDetectorUnit
// via tha AddDevice memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 

 NcDetectorUnit* q=new NcDetectorUnit(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
