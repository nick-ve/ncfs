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

// $Id: NcEvent.cxx 132 2016-09-01 16:50:25Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcEvent
// Creation and investigation of an NCFS generic event structure.
// An NcEvent can be constructed by adding NcTracks, NcVertices, NcJets
// and/or devices like NcCalorimeters or NcDevice (derived) objects.
//
// All objects which are derived from TObject can be regarded as a device.
// However, NcDevice (or derived) objects profit from additional hit
// handling facilities.
// A "hit" is a generic name indicating an NcSignal (or derived) object.
// Note that NcEvent does NOT own hits; it only provides references to hits
// obtained from the various devices.
// This implies that hits should be owned by the devices themselves.
//
// The basic functionality of NcEvent is identical to the one of NcVertex.
// So, an NcEvent may be used as the primary vertex with some additional
// functionality compared to NcVertex.
//
// To provide maximal flexibility to the user, the two modes of track/jet/vertex
// storage as described in NcJet and NcVertex can be used.
// In addition an identical structure is provided for the storage of devices like
// NcCalorimeter objects, which can be selected by means of the memberfunction
// SetDevCopy().
//
// a) SetDevCopy(0) (which is the default).
//    Only the pointers of the 'added' devices are stored.
//    This mode is typically used by making studies based on a fixed set
//    of devices which stays under user control or is kept on an external
//    file/tree. 
//    In this way the NcEvent just represents a 'logical structure' for the
//    physics analysis.
//
//    Note :
//    Modifications made to the original devices also affect the device
//    objects which are stored in the NcEvent. 
//
// b) SetDevCopy(1).
//    Of every 'added' device a private copy will be made of which the pointer
//    will be stored.
//    In this way the NcEvent represents an entity on its own and modifications
//    made to the original calorimeters do not affect the NcCalorimeter objects
//    which are stored in the NcEvent. 
//    This mode will allow 'adding' many different devices into an NcEvent by
//    creating only one device instance in the main programme and using the
//    Reset() and parameter setting memberfunctions of the object representing the device.
//
//    Note :
//    The copy is made using the Clone() memberfunction.
//    All devices (i.e. classes derived from TObject) have the default TObject::Clone() 
//    memberfunction.
//    However, devices generally contain an internal (signal) data structure
//    which may include pointers to other objects. Therefore it is recommended to provide
//    for all devices a specific copy constructor and override the default Clone()
//    memberfunction using this copy constructor.
//    Examples for this may be seen from NcCalorimeter, NcSignal and NcDevice.   
//
// See also the documentation provided for the memberfunction SetOwner(). 
//
// Coding example to make an event consisting of a primary vertex,
// 2 secondary vertices and a calorimeter.
// --------------------------------------------------------------
// vp contains the tracks 1,2,3 and 4 (primary vertex)
// v1 contains the tracks 5,6 and 7   (sec. vertex)
// v2 contains the jets 1 and 2       (sec. vertex)
//
//        NcEvent evt;
//
// Specify the event object as the repository of all objects
// for the event building and physics analysis.
// 
//        evt.SetDevCopy(1);
//        evt.SetTrackCopy(1);
//
// Fill the event structure with the basic objects
// 
//        NcCalorimeter emcal1;
//        NcCalorimeter emcal2;
//         ...
//         ... // code to fill the emcal1 and emcal2 calorimeter data
//         ...
//
//        evt.AddDevice(emcal1);
//        evt.AddDevice(emcal2);
//
//        // Assume NcTOF has been derived from NcDevice
//        NcTOF tof1;
//        NcTOF tof2;
//         ...
//         ... // code to fill the tof1 and tof2 data
//         ...
//
//        evt.AddDevice(tof1);
//        evt.AddDevice(tof2);
//
//        NcTrack* tx=new NcTrack();
//        for (Int_t i=0; i<10; i++)
//        {
//         ...
//         ... // code to fill the track data
//         ...
//         evt.AddTrack(tx);
//         tx->Reset(); 
//        }
//
//        if (tx)
//        {
//         delete tx;
//         tx=0;
//        }
//
// Order and investigate all the hits of all the TOF devices
//
//        TObjArray* hits=evt.GetHits("NcTOF");
//        TObjArray* orderedtofs=evt.SortHits(hits);
//        Int_t nhits=0;
//        if (orderedtofs) nhits=orderedtofs->GetEntries();
//        for (Int_t i=0; i<nhits; i++)
//        {
//         NcSignal* sx=(NcSignal*)orderedtofs->At(i);
//         if (sx) sx->Data();
//        }
//
// Order and investigate all the hits of all the calorimeter devices
//
//        TObjArray* hits=evt.GetHits("NcCalorimeter");
//        TObjArray* orderedcals=evt.SortHits(hits);
//        Int_t nhits=0;
//        if (orderedcals) nhits=orderedcals->GetEntries();
//        for (Int_t i=0; i<nhits; i++)
//        {
//         NcSignal* sx=(NcSignal*)orderedcals->At(i);
//         if (sx) sx->Data();
//        }
//
// Build the event structure (vertices, jets, ...) for physics analysis
// based on the basic objects from the event repository.
//
//        NcJet j1,j2;
//        for (Int_t i=0; i<evt.GetNtracks(); i++)
//        {
//         tx=evt.GetTrack(i);
//         ...
//         ... // code to fill the jet data
//         ...
//        }
//
//        NcVertex vp;
//        tx=evt.GetTrack(1);
//        vp.AddTrack(tx);
//        tx=evt.GetTrack(2);
//        vp.AddTrack(tx);
//        tx=evt.GetTrack(3);
//        vp.AddTrack(tx);
//        tx=evt.GetTrack(4);
//        vp.AddTrack(tx);
//
//        Float_t rp[3]={2.4,0.1,-8.5};
//        vp.SetPosition(rp,"car");
//
//        NcVertex v1;
//        tx=evt.GetTrack(5);
//        v1.AddTrack(tx);
//        tx=evt.GetTrack(6);
//        v1.AddTrack(tx);
//        tx=evt.GetTrack(7);
//        v1.AddTrack(tx);
//
//        Float_t r1[3]={1.6,-3.2,5.7};
//        v1.SetPosition(r1,"car");
//
//
//        NcVertex v2;
//        v2.SetJetCopy(1);
//        v2.AddJet(j1);
//        v2.AddJet(j2);
//
//        Float_t r2[3]={6.2,4.8,1.3};
//        v2.SetPosition(r2,"car");
//
// Specify the vertices v1 and v2 as secondary vertices of the primary
//
//        vp.SetVertexCopy(1);
//        vp.AddVertex(v1);
//        vp.AddVertex(v2);
//
// Enter the physics structures into the event
//        evt.SetVertexCopy(1);
//        evt.AddVertex(vp,0);
//
// The jets j1 and j2 are already available via sec. vertex v2,
// but can be made available also from the event itself if desired.
//        NcJet* jx;
//        jx=v2.GetJet(1);
//        evt.AddJet(jx,0); 
//        jx=v2.GetJet(2);
//        evt.AddJet(jx,0); 
// 
//        evt.Data("sph");
//        v1.ListAll();
//        v2.List("cyl");
//
//        Float_t etot=evt.GetEnergy();
//        Nc3Vector ptot=evt.Get3Momentum();
//        Float_t loc[3];
//        evt.GetPosition(loc,"sph");
//        NcPosition r=v1.GetPosition();
//        r.Data(); 
//        Int_t nt=v2.GetNtracks();
//        NcTrack* tv=v2.GetTrack(1); // Access track number 1 of Vertex v2
//
//        evt.List();
//
//        Int_t nv=evt.GetNvtx();
//        NcVertex* vx=evt.GetVertex(1); // Access primary vertex
//        Float_t e=vx->GetEnergy();
//
//        Float_t M=evt.GetInvmass(); 
//
// Reconstruct the event from scratch
//
//        evt.Reset();
//        evt.SetNvmax(25); // Increase initial no. of sec. vertices
//        ...
//        ... // code to create tracks etc... 
//        ...
//
// Note : By default all quantities are in meter, GeV, GeV/c or GeV/c**2
//        but the user can indicate the usage of a different scale for
//        the metric and/or energy-momentum units via the SetUnitScale()
//        and SetEscale() memberfunctions, respectively.
//        The actual metric and energy-momentum unit scales in use can be
//        obtained via the GetUnitScale() and GetEscale() memberfunctions.
//
//--- Author: Nick van Eijndhoven 27-may-2001 Utrecht University
//- Modified: NvE $Date: 2016-09-01 18:50:25 +0200 (Thu, 01 Sep 2016) $
///////////////////////////////////////////////////////////////////////////

#include "NcEvent.h"
#include "Riostream.h"
 
ClassImp(NcEvent) // Class implementation to enable ROOT I/O
 
NcEvent::NcEvent() : NcVertex(),NcTimestamp()
{
// Default constructor.
// All variables initialised to default values.
 fRun=0;
 fEvent=0;
 fWeight=1;
 fDevices=0;
 fDevCopy=0;
 fHits=0;
 fOrdered=0;
 fDisplay=0;
 fDevs=0;
}
///////////////////////////////////////////////////////////////////////////
NcEvent::NcEvent(Int_t n) : NcVertex(n),NcTimestamp()
{
// Create an event to hold initially a maximum of n tracks
// All variables initialised to default values
 if (n<=0)
 {
  cout << " *** This NcVertex initialisation was invoked via the NcEvent ctor." << endl;
 }
 fRun=0;
 fEvent=0;
 fWeight=1;
 fDevices=0;
 fDevCopy=0;
 fHits=0;
 fOrdered=0;
 fDisplay=0;
 fDevs=0;
}
///////////////////////////////////////////////////////////////////////////
NcEvent::~NcEvent()
{
// Default destructor
 if (fDevices)
 {
  delete fDevices;
  fDevices=0;
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
 if (fDisplay)
 {
  delete fDisplay;
  fDisplay=0;
 }
 if (fDevs)
 {
  delete fDevs;
  fDevs=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcEvent::NcEvent(const NcEvent& evt) : NcVertex(evt),NcTimestamp(evt)
{
// Copy constructor.
 fRun=evt.fRun;
 fEvent=evt.fEvent;
 fWeight=evt.fWeight;
 fDevCopy=evt.fDevCopy;

 fHits=0;
 fOrdered=0;
 fDisplay=0;
 fDevs=0;

 fDevices=0;
 Int_t ndevs=evt.GetNdevices();
 if (ndevs)
 {
  fDevices=new TObjArray(ndevs);
  if (fDevCopy) fDevices->SetOwner();
  for (Int_t i=1; i<=ndevs; i++)
  {
   NcDevice* dev=evt.GetDevice(i);
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
void NcEvent::Reset()
{
// Reset all variables to default values
// The max. number of tracks is set to the initial value again
// The max. number of vertices is set to the default value again
// The event weight is set to 1 again
// Note : The DevCopy mode is maintained as it was set by the user before.

 NcVertex::Reset();

 Set();
 fRun=0;
 fEvent=0;
 fWeight=1;

 if (fDevices)
 {
  delete fDevices;
  fDevices=0;
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
 if (fDisplay)
 {
  delete fDisplay;
  fDisplay=0;
 }
 if (fDevs)
 {
  delete fDevs;
  fDevs=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetOwner(Bool_t own)
{
// Set ownership of all added objects. 
// The default parameter is own=kTRUE.
//
// Invokation of this memberfunction also sets all the copy modes
// (e.g. TrackCopy & co.) according to the value of own.
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

 NcVertex::SetOwner(own);
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetDayTime(TTimeStamp& stamp)
{
// Set the date and time stamp for this event.
// An exact copy of the entered date/time stamp will be saved with an
// accuracy of 1 nanosecond.
//
// Note : Since the introduction of the more versatile class NcTimestamp
//        and the fact that NcEvent has now been derived from it,
//        this memberfunction has become obsolete.
//        It is recommended to use the corresponding NcTimestamp
//        functionality directly for NcEvent instances.
//        This memberfunction is only kept for backward compatibility.

 Set(stamp.GetDate(),stamp.GetTime(),0,kTRUE,0);
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetDayTime(TDatime& stamp)
{
// Set the date and time stamp for this event.
// The entered date/time will be interpreted as being the local date/time
// and the accuracy is 1 second.
//
// This function with the TDatime argument is mainly kept for backward
// compatibility reasons.
// It is recommended to use the corresponding NcTimestamp functionality
// directly for NcEvent instances.

 Set(stamp.GetDate(),stamp.GetTime(),0,kFALSE,0);
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetRunNumber(Int_t run)
{
// Set the run number for this event
 fRun=run;
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetEventNumber(Int_t evt)
{
// Set the event number for this event
 fEvent=evt;
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetWeight(Double_t weight)
{
// Set the weight for this event.
// By default the weight is set to 1 in the constructor.
 fWeight=weight;
}
///////////////////////////////////////////////////////////////////////////
TTimeStamp NcEvent::GetDayTime() const
{
// Provide the date and time stamp for this event
//
// Note : Since the introduction of the more versatile class NcTimestamp
//        and the fact that NcEvent has now been derived from it,
//        this memberfunction has become obsolete.
//        It is recommended to use the corresponding NcTimestamp
//        functionality directly for NcEvent instances.
//        This memberfunction is only kept for backward compatibility.

 return (TTimeStamp)(*this);
}
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetRunNumber() const
{
// Provide the run number for this event
 return fRun;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetEventNumber() const
{
// Provide the event number for this event
 return fEvent;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcEvent::GetWeight() const
{
// Provide the weight for this event
 return fWeight;
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetProjectile(Int_t a,Int_t z,Double_t pnuc,Int_t id)
{
// Set the projectile A, Z, momentum per nucleon and user defined particle ID.
// If not explicitly specified by the user, the projectile particle ID is set
// to zero by default and will not be stored in the event structure.
// The projectile specifications will be stored in a device named "Beam"
// which is an instance of NcSignal.
// As such these data are easily retrievable from the event structure.
// However, for backward compatibility reasons the beam data can also be
// retrieved via memberfunctions like GetProjectileA() etc...

 Int_t newdev=0;
 
 NcDevice* beam=GetDevice("Beam");
 
 if (!beam)
 {
  beam=new NcDevice();
  beam->SetNameTitle("Beam","Beam and target specifications");
  newdev=1;
 }

 if (a || z)
 {
  beam->AddNamedSlot("Aproj");
  beam->SetSignal(a,"Aproj");
  beam->AddNamedSlot("Zproj");
  beam->SetSignal(z,"Zproj");
 }
 beam->AddNamedSlot("Pnucproj");
 beam->SetSignal(pnuc,"Pnucproj");
 if (id)
 {
  beam->AddNamedSlot("Idproj");
  beam->SetSignal(id,"Idproj");
 }

 if (newdev)
 {
  AddDevice(beam);
  if (fDevCopy) delete beam;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetProjectile(Int_t a,Int_t z,Nc3Vector& p,Int_t id)
{
// Set the projectile A, Z, 3-momentum per nucleon and user defined particle ID.
// If not explicitly specified by the user, the projectile particle ID is set
// to zero by default and will not be stored in the event structure.
// The projectile specifications will be stored in a device named "Beam"
// which is an instance of NcSignal.
// As such these data are easily retrievable from the event structure.
// However, for backward compatibility reasons beam data can also be
// retrieved via memberfunctions like GetProjectileA() etc...

 Int_t newdev=0;
 
 NcDevice* beam=GetDevice("Beam");
 
 if (!beam)
 {
  beam=new NcDevice();
  beam->SetNameTitle("Beam","Beam and target specifications");
  newdev=1;
 }

 Double_t px=p.GetX(1,"car");
 Double_t py=p.GetX(2,"car");
 Double_t pz=p.GetX(3,"car");
 Double_t pnuc=p.GetNorm();

 if (a || z)
 {
  beam->AddNamedSlot("Aproj");
  beam->SetSignal(a,"Aproj");
  beam->AddNamedSlot("Zproj");
  beam->SetSignal(z,"Zproj");
 }
 beam->AddNamedSlot("Pnucproj");
 beam->SetSignal(pnuc,"Pnucproj");
 beam->AddNamedSlot("Pxnucproj");
 beam->SetSignal(px,"Pxnucproj");
 beam->AddNamedSlot("Pynucproj");
 beam->SetSignal(py,"Pynucproj");
 beam->AddNamedSlot("Pznucproj");
 beam->SetSignal(pz,"Pznucproj");
 if (id)
 {
  beam->AddNamedSlot("Idproj");
  beam->SetSignal(id,"Idproj");
 }

 if (newdev)
 {
  AddDevice(beam);
  if (fDevCopy) delete beam;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetProjectileA() const
{
// Provide the projectile A value.
 Int_t val=0;
 NcDevice* beam=GetDevice("Beam");
 if (beam) val=int(beam->GetSignal("Aproj"));
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetProjectileZ() const
{
// Provide the projectile Z value.
 Int_t val=0;
 NcDevice* beam=GetDevice("Beam");
 if (beam) val=int(beam->GetSignal("Zproj"));
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcEvent::GetProjectilePnuc() const
{
// Provide the projectile momentum value per nucleon.
 Double_t val=0;
 NcDevice* beam=GetDevice("Beam");
 if (beam) val=beam->GetSignal("Pnucproj");
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetProjectileId() const
{
// Provide the user defined particle ID of the projectile.
 Int_t val=0;
 NcDevice* beam=GetDevice("Beam");
 if (beam) val=int(beam->GetSignal("Idproj"));
 return val;
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetTarget(Int_t a,Int_t z,Double_t pnuc,Int_t id)
{
// Set the target A, Z, momentum per nucleon and user defined particle ID.
// If not explicitly specified by the user, the target particle ID is set
// to zero by default and will not be stored in the event structure.
// The target specifications will be stored in a device named "Beam"
// which is an instance of NcSignal.
// As such these data are easily retrievable from the event structure.
// However, for backward compatibility reasons the beam data can also be
// retrieved via memberfunctions like GetTargetA() etc...

 Int_t newdev=0;
 
 NcDevice* beam=GetDevice("Beam");
 
 if (!beam)
 {
  beam=new NcDevice();
  beam->SetNameTitle("Beam","Beam and target specifications");
  newdev=1;
 }

 if (a || z)
 {
  beam->AddNamedSlot("Atarg");
  beam->SetSignal(a,"Atarg");
  beam->AddNamedSlot("Ztarg");
  beam->SetSignal(z,"Ztarg");
 }
 beam->AddNamedSlot("Pnuctarg");
 beam->SetSignal(pnuc,"Pnuctarg");
 if (id)
 {
  beam->AddNamedSlot("Idtarg");
  beam->SetSignal(id,"Idtarg");
 }

 if (newdev)
 {
  AddDevice(beam);
  if (fDevCopy) delete beam;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetTarget(Int_t a,Int_t z,Nc3Vector& p,Int_t id)
{
// Set the target A, Z, 3-momentum per nucleon and user defined particle ID.
// If not explicitly specified by the user, the target particle ID is set
// to zero by default and will not be stored in the event structure.
// The target specifications will be stored in a device named "Beam"
// which is an instance of NcSignal.
// As such these data are easily retrievable from the event structure.
// However, for backward compatibility reasons beam data can also be
// retrieved via memberfunctions like GetTargetA() etc...

 Int_t newdev=0;
 
 NcDevice* beam=GetDevice("Beam");
 
 if (!beam)
 {
  beam=new NcDevice();
  beam->SetNameTitle("Beam","Beam and target specifications");
  newdev=1;
 }

 Double_t px=p.GetX(1,"car");
 Double_t py=p.GetX(2,"car");
 Double_t pz=p.GetX(3,"car");
 Double_t pnuc=p.GetNorm();

 if (a || z)
 {
  beam->AddNamedSlot("Atarg");
  beam->SetSignal(a,"Atarg");
  beam->AddNamedSlot("Ztarg");
  beam->SetSignal(z,"Ztarg");
 }
 beam->AddNamedSlot("Pnuctarg");
 beam->SetSignal(pnuc,"Pnuctarg");
 beam->AddNamedSlot("Pxnuctarg");
 beam->SetSignal(px,"Pxnuctarg");
 beam->AddNamedSlot("Pynuctarg");
 beam->SetSignal(py,"Pynuctarg");
 beam->AddNamedSlot("Pznuctarg");
 beam->SetSignal(pz,"Pznuctarg");
 if (id)
 {
  beam->AddNamedSlot("Idtarg");
  beam->SetSignal(id,"Idtarg");
 }

 if (newdev)
 {
  AddDevice(beam);
  if (fDevCopy) delete beam;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetTargetA() const
{
// Provide the target A value.
 Int_t val=0;
 NcDevice* beam=GetDevice("Beam");
 if (beam) val=int(beam->GetSignal("Atarg"));
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetTargetZ() const
{
// Provide the target Z value.
 Int_t val=0;
 NcDevice* beam=GetDevice("Beam");
 if (beam) val=int(beam->GetSignal("Ztarg"));
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcEvent::GetTargetPnuc() const
{
// Provide the target momentum value per nucleon.
 Double_t val=0;
 NcDevice* beam=GetDevice("Beam");
 if (beam) val=beam->GetSignal("Pnuctarg");
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetTargetId() const
{
// Provide the user defined particle ID of the target.
 Int_t val=0;
 NcDevice* beam=GetDevice("Beam");
 if (beam) val=int(beam->GetSignal("Idtarg"));
 return val;
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::HeaderData()
{
// Provide event header information
 const char* name=GetName();
 const char* title=GetTitle();
 cout << " *" << ClassName() << "::Data*";
 if (strlen(name))  cout << " Name : " << GetName();
 if (strlen(title)) cout << " Title : " << GetTitle();
 cout << endl;
 Date(1);
 cout << "  Run : " << fRun << " Event : " << fEvent
      << " Weight : " << fWeight << endl;
 ShowDevices(0);
 ShowTracks(0);
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::Data(TString f,TString u)
{
// Provide event information within the coordinate frame f
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The defaults are f="car" and u="rad".

 HeaderData();
 NcVertex::Data(f,u);
} 
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetNdevices() const
{
// Provide the number of stored devices
 Int_t ndevs=0;
 if (fDevices) ndevs=fDevices->GetEntries();
 return ndevs;
} 
///////////////////////////////////////////////////////////////////////////
Int_t NcEvent::GetNdevices(const char* classname,TObjArray* hits) const
{
// Provide the number of devices of the specified class.
// In case an array "hits" is provided, the contents of the provided
// hit array are used to determine the number of different devices of the
// specified class to which the hits belong.
// Note that identification of the different hit parent devices only works
// for devices that have been given a unique identifier.
// In case no hit array is provided, just the number of stored devices of the
// specified class is returned. 
//
// By default hits=0.

 Int_t ndevs=0;

 if (!hits) // Investigate the stored devices
 {
  for (Int_t idev=1; idev<=GetNdevices(); idev++)
  {
   NcDevice* dev=GetDevice(idev);
   if (!dev) continue;
   if (dev->InheritsFrom(classname)) ndevs++;
  }
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
void NcEvent::AddDevice(NcDevice& d)
{
// Add a device to the event.
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
 
 // Add the device to this event
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
void NcEvent::RemoveDevice(NcDevice* d)
{
// Remove the specified device from the event.

 if (!fDevices || !d) return;

 TObject* obj=fDevices->Remove(d);
 if (obj)
 {
  if (fDevCopy) delete obj;
  fDevices->Compress();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::SetDevCopy(Int_t j)
{
// (De)activate the creation of private copies of the added devices.
// j=0 ==> No private copies are made; pointers of original devices are stored.
// j=1 ==> Private copies of the devices are made and these pointers are stored.
//
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
//  To change the DevCopy mode for an existing NcEvent containing
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
Int_t NcEvent::GetDevCopy() const
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
NcDevice* NcEvent::GetDevice(Int_t i) const
{
// Return the i-th device of this event.
// The first device corresponds to i=1.

 if (!fDevices)
 {
  return 0;
 }
 else
 {
  Int_t ndevs=GetNdevices();
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
NcDevice* NcEvent::GetDevice(TString name) const
{
// Return the device with name tag "name"
 if (!fDevices)
 {
  return 0;
 }
 else
 {
  TString s;
  Int_t ndevs=GetNdevices();
  for (Int_t i=0; i<ndevs; i++)
  {
   NcDevice* dev=(NcDevice*)fDevices->At(i);
   if (dev)
   {
    s=dev->GetName();
    if (s == name) return dev;
   }
  }

  return 0; // No matching name found
 }
}
///////////////////////////////////////////////////////////////////////////
NcDevice* NcEvent::GetIdDevice(Int_t id,TObjArray* devs) const
{
// Return the NcDevice with identifier "id" from the specified array "devs".
// In case devs=0 (which is the default) all devices stored in the event
// structure will be evaluated.
// Note : In case of multiple occurrences of identifier "id", the first
//        encountered matching device will be returned.

 TObjArray* arr=devs;
 if (!arr) arr=fDevices;

 if (!arr || id<0) return 0;

 Int_t idx=0;
 for (Int_t i=0; i<arr->GetSize(); i++)
 {
  TObject* obj=arr->At(i);
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
NcDevice* NcEvent::GetIdDevice(Int_t id,const char* classname) const
{
// Return the device with identifier "id" of the specified class.
// Note : In case of multiple occurrences of identifier "id", the first
//        encountered matching device will be returned.

 if (!fDevices || id<0) return 0;

 Int_t idx=0;
 for (Int_t i=0; i<GetNdevices(); i++)
 {
  NcDevice* dev=(NcDevice*)fDevices->At(i);
  if (dev)
  {
   idx=dev->GetUniqueID();
   if (idx==id && dev->InheritsFrom(classname)) return dev;
  }
 }
 return 0; // No matching id found for the specified class
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::ShowDevices(Int_t mode) const
{
// Provide an overview of the available devices.
// The argument mode determines the amount of information as follows :
// mode = 0 ==> Only printout of the number of devices
//        1 ==> Provide a listing with 1 line of info for each device
//
// The default is mode=1.
//
 Int_t ndevs=GetNdevices();
 if (ndevs)
 {
  if (!mode)
  {
   cout << " There are " << ndevs << " devices available." << endl; 
  }
  else
  {
   cout << " The following " << ndevs << " devices are available :" << endl; 
   Int_t nh=0,nw=0;
   for (Int_t i=1; i<=ndevs; i++)
   {
    NcDevice* dev=GetDevice(i);
    if (dev)
    {
     const char* name=dev->GetName();
     cout << " Device number : " << i;
     cout << " Class : " << dev->ClassName() << " Id : " << dev->GetUniqueID();
     if (strlen(name)) cout << " Name : " << name;
     nh=dev->GetNhits();
     if (nh) cout << " Nhits : " << nh;
     nw=dev->GetNwaveforms();
     if (nw) cout << " Nwaveforms : " << nw;
     cout << endl;
    }
   }
  }
 }
 else
 {
  cout << " No devices present for this event." << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::ShowDevices(const char* classname,Int_t mode) const
{
// Provide an overview of the available devices of the specified class.
// The argument mode determines the amount of information as follows :
// mode = 0 ==> Only printout of the number of devices
//        1 ==> Provide a listing with 1 line of info for each device
//
// The default is mode=1.
//
 Int_t ndevs=GetNdevices();
 if (ndevs)
 {
  Int_t ndevs2=GetNdevices(classname);
  if (!mode || !ndevs2)
  {
   cout << " There are " << ndevs2 << " selected devices available." << endl; 
  }
  else
  {
   cout << " The following " << ndevs2 << " selected devices are available :" << endl; 
   Int_t nh=0,nw=0;
   for (Int_t i=1; i<=ndevs; i++)
   {
    NcDevice* dev=GetDevice(i);
    if (dev)
    {
     if (dev->InheritsFrom(classname))
     {
      const char* name=dev->GetName();
      cout << " Device number : " << i;
      cout << " Class : " << dev->ClassName() << " Id : " << dev->GetUniqueID();
      if (strlen(name)) cout << " Name : " << name;
      nh=dev->GetNhits();
      if (nh) cout << " Nhits : " << nh;
      nw=dev->GetNwaveforms();
      if (nw) cout << " Nwaveforms : " << nw;
      cout << endl;
     }
    }
   }
  }
 }
 else
 {
  cout << " No devices present for this event." << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcEvent::GetDevices(const char* classname,TObjArray* devices)
{
// Provide the references to the various devices derived from the specified class.
//
// Note :
// ------
// In case devices=0 the selected device pointers are returned via a multi-purpose array,
// which will be overwritten by subsequent selections.
// It is recommended to provide a user defined array via the argument "devices" 
// to omit the danger of overwriting the selection and to allow to use the selected device list
// amongst other selections.
// In case a user defined array "devices" is provided, this memberfunction returns 0 for the
// return argument.
//
// The default is devices=0.

 if (devices)
 {
  devices->Clear();
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

 Int_t ndev=GetNdevices();
 for (Int_t idev=1; idev<=ndev; idev++)
 {
  NcDevice* dev=GetDevice(idev);
  if (!dev) continue;

  if (dev->InheritsFrom(classname))
  {
   if (devices)
   {
    devices->Add(dev);
   }
   else
   {
    fDevs->Add(dev);
   }
  }
 }

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
Int_t NcEvent::GetNhits(const char* classname)
{
// Provide the number of hits registered to the specified device class.
// The specified device class has to be derived from NcDevice.
// It is possible to indicate with the argument "classname" a specific
// device instead of a whole class of devices. However, in such a case
// it is more efficient to use the GetDevice() memberfunction directly.

 TObjArray hits;
 GetHits(classname,&hits);
 Int_t nhits=hits.GetEntries();
 return nhits;
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcEvent::GetHits(const char* classname,TObjArray* hits,TString name,Int_t mode,Int_t opt)
{
// Provide the references to all the hits registered to device class as specified
// by the input argument "classname".
// The specified device class has to be derived from NcDevice.
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
// The input arguments "name", "mode" and "opt" allow for further selection criteria
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

 LoadHits(classname,hits);

 // Check for further hit selection criteria
 if (name!="none")
 {
  NcDevice selector;
  TObjArray* selected=hits;
  if (!selected) selected=fHits;
  TObjArray ahits(*selected);
  selector.GetHits(*selected,name,mode,opt,&ahits);
 }

 if (hits)
 {
  return 0;
 }
 else
 {
  return fHits;
 }
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcEvent::GetIdHit(Int_t id,const char* classname)
{
// Return the hit with unique identifier "id" for the specified device class.
 if (id<0) return 0;

 Int_t nhits=GetNhits(classname);
 if (!nhits) return 0;

 NcSignal* sx=0;
 Int_t sid=0;
 for (Int_t i=0; i<nhits; i++)
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
void NcEvent::LoadHits(const char* classname,TObjArray* hits)
{
// Load the references to the various hits registered to the specified device class.
// The specified device class has to be derived from NcDevice.
//
// Note :
// ------
// In case hits=0 the selected hit pointers are returned via a multi-purpose array,
// which will be overwritten by subsequent selections.
// It is recommended to provide a user defined array via the argument "hits" 
// to omit the danger of overwriting the selection and to allow to use the selected hit list
// amongst other selections.
//
// The default is hits=0.

 if (hits)
 {
  hits->Clear();
 }
 else
 {
  if (fHits)
  {
   fHits->Clear();
  }
  else
  {
   fHits=new TObjArray();
  }
 }

 for (Int_t idev=1; idev<=GetNdevices(); idev++)
 {
  NcDevice* dev=GetDevice(idev);
  if (!dev) continue;

  if (!(dev->InheritsFrom(classname))) continue;
  
  for (Int_t ih=1; ih<=dev->GetNhits(); ih++)
  {
   NcSignal* sx=dev->GetHit(ih);
   if (!sx) continue;
   if (hits)
   {
    hits->Add(sx);
   }
   else
   {
    fHits->Add(sx);
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcEvent::SortHits(const char* classname,Int_t idx,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various hits registered to the specified device class.
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

 if (ordered) ordered->Clear();

 if (idx<=0 || abs(mode)!=1) return 0;

 TObjArray hits;
 LoadHits(classname,&hits);

 NcDevice dev;
 TObjArray* arr=dev.SortHits(idx,mode,&hits,mcal,deadcheck,ordered);

 if (ordered) return 0;

 if (fHits)
 {
  delete fHits;
  fHits=0;
 }
 if (arr) fHits=new TObjArray(*arr);
 return fHits;
}
///////////////////////////////////////////////////////////////////////////
TObjArray* NcEvent::SortHits(const char* classname,TString name,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various hits registered to the specified device class.
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

 if (ordered) ordered->Clear();
 
 if (abs(mode)!=1) return 0;

 TObjArray hits;
 LoadHits(classname,&hits);

 NcDevice dev;
 TObjArray* arr=dev.SortHits(name,mode,&hits,mcal,deadcheck,ordered);

 if (ordered) return 0;

 if (fHits)
 {
  delete fHits;
  fHits=0;
 }
 if (arr) fHits=new TObjArray(*arr);
 return fHits;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector NcEvent::GetHitPath(TObjArray* hits,Int_t pos) const
{
// Provide the average direction of the hit pattern contained in the array "hits".
// The direction is obtained by starting at the first hit in the array
// and then a summation of all the relative hit locations while jumping
// from one hit location to the other.
// Since the obtained direction is obviously depending on the order in which the
// hits appear, the user should take care of providing a correctly ordered hit array.
//
// pos = 0 ==> The position of the hit signal itself is used.
//       1 ==> The position of the parent device of the hit signal is used.
//
// The default is pos=0.
//
// Note : In case of inconsistent input a "zero vector" will be returned.

 Nc3Vector v;

 if (!hits) return v;

 Int_t nh=hits->GetEntries();
 if (!nh) return v;

 NcSignal* s1=(NcSignal*)hits->At(0);
 if (!s1) return v;

 NcDevice* dev1=s1->GetDevice();
 if (pos && !dev1) return v;

 // Define the starting hit to be the origin
 Nc3Vector r1;
 r1.SetZero();

 Nc3Vector r2;
 Nc3Vector r12;
 for (Int_t ih=1; ih<nh; ih++)
 {
  NcSignal* s2=(NcSignal*)hits->At(ih);
  if (!s2) continue;

  NcDevice* dev2=s2->GetDevice();
  if (pos && !dev2) continue;

  if (!pos)
  {
   r2=(Nc3Vector)s2->GetPosition();
  }
  else
  {
   r2=(Nc3Vector)dev2->GetPosition();
  }
  r12=r2-r1;
  v+=r12;

  r1=r2;
 }

 return v;
}
///////////////////////////////////////////////////////////////////////////
NcPosition NcEvent::GetCOG(TObjArray* hits,Int_t pos,TString slotname,Int_t mode) const
{
// Provide the Center Of Gravity of the hits contained in the array "hits".
// Each hit can be given a weight according to the absolute value of the signal
// contained in the slot with the name "slotname".
// In case slotname="none" each hit will obtain a weight equal to 1.
// The input argument "mode" has the same meaning as in the GetSignal() member function
// of the class NcSignal.
//
// pos = 0 ==> The position of the hit signal itself is used.
//       1 ==> The position of the parent device of the hit signal is used.
//
// The defaults are pos=0, slotname="none" and mode=0.
//
// Note : In case of inconsistent input a "zero vector" will be returned.

 NcPosition cog;

 if (!hits) return cog;

 Int_t nh=hits->GetEntries();
 if (!nh) return cog;

 Nc3Vector rx;
 Nc3Vector rsum;
 Float_t w=1;
 Double_t wsum=0;
 for (Int_t ih=0; ih<nh; ih++)
 {
  NcSignal* sx=(NcSignal*)hits->At(ih);
  if (!sx) continue;

  NcDevice* devx=sx->GetDevice();
  if (pos && !devx) continue;

  if (!pos)
  {
   rx=(Nc3Vector)sx->GetPosition();
  }
  else
  {
   rx=(Nc3Vector)devx->GetPosition();
  }

  if (slotname != "none")
  {
   w=fabs(sx->GetSignal(slotname,mode));
   rx*=w;
  }
  rsum+=rx;
  wsum+=double(w);
 }

 if (wsum>0) rsum/=wsum;
 cog.SetPosition(rsum);
 return cog;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcEvent::GetCVAL(TObjArray* hits,TString obsname,TString weightname,Int_t mode,Int_t type) const
{
// Provide the Central Value of the observed signals contained in the slot with name "obsname"
// in the array "hits". Depending on the input argument "type" the central value represents
// either the median (type=1) or the mean (type=2).
// Each hit can be given a weight according to the absolute value of the signal
// contained in the slot with the name "weightname".
// In case weightname="none" each hit will obtain a weight equal to 1.
// The input argument "mode" has the same meaning as in the GetSignal() member function
// of the class NcSignal.
//
// The defaults are weightname="none", mode=0 and type=1.
//
// Note : In case of inconsistent input 0 will be returned.

 if (type!=1 && type!=2)
 {
  cout << " *NcEvent::GetCVAL* Unknown type : " << type << endl;
  return 0;
 }

 if (!hits) return 0;

 Int_t nh=hits->GetEntries();
 if (!nh) return 0;

 Double_t cval=0;

 Double_t val=0;
 Double_t w=1;
 Double_t wsum=0;
 NcSample stat;
 if (type==1) stat.SetStoreMode();
 Int_t iw=0;
 for (Int_t ih=0; ih<nh; ih++)
 {
  NcSignal* sx=(NcSignal*)hits->At(ih);
  if (!sx) continue;

  val=sx->GetSignal(obsname,mode);

  if (weightname != "none") w=fabs(sx->GetSignal(weightname,mode));

  if (type==1) // Weighted median
  {
   iw=TMath::Nint(w);
   if (iw<1) iw=1;
   for (Int_t i=0; i<iw; i++)
   {
    stat.Enter(val);
   }
  }

  if (type==2) // Weighted mean
  {
   val*=w;
   stat.Enter(val);
  }
  wsum+=w;
 }

 if (type==1) cval=stat.GetMedian(1);

 if (type==2)
 {
  if (wsum>0) cval=stat.GetSum(1)/wsum;
 }

 return cval;
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::GetExtremes(const char* classname,Float_t& vmin,Float_t& vmax,Int_t idx,Int_t mode,Int_t deadcheck)
{
// Provide the min. and max. signal values of the various hits registered
// to the specified device class.
// The input argument "idx" denotes the index of the signal slots to be investigated.
// The default is idx=1;
// The gain etc... corrected signals will be used in the process as specified
// by the  "mode" argument. The definition of this "mode" parameter corresponds to
// the description provided in the GetSignal memberfunction of class NcSignal.
// The default is mode=1 (for backward compatibility reasons).
// The argument "deadcheck" allows to reject signals which were declared as "Dead".
// If deadcheck=0 the dead signals will be treated in the same way as the other signals.
// To achieve an identical treatment of dead and alive signals, the setting of deadcheck=0
// will automatically set also mode=0 to retrieve the stored signal values "as is".
// The default is deadcheck=1 (for backward compatibility reasons).
//
// For more extended functionality see class NcDevice.

 if (idx<=0) return;

 TObjArray hits;
 LoadHits(classname,&hits);

 NcDevice dev;
 dev.GetExtremes(vmin,vmax,idx,&hits,mode,deadcheck);
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::GetExtremes(const char* classname,Float_t& vmin,Float_t& vmax,TString name,Int_t mode,Int_t deadcheck)
{
// Provide the min. and max. signal values of the various hits registered
// to the specified device class.
// The input argument "name" denotes the name of the signal slots to be investigated.
// The gain etc... corrected signals will be used in the process as specified
// by the  "mode" argument. The definition of this "mode" parameter corresponds to
// the description provided in the GetSignal memberfunction of class NcSignal.
// The default is mode=1 (for backward compatibility reasons).
// The argument "deadcheck" allows to reject signals which were declared as "Dead".
// If deadcheck=0 the dead signals will be treated in the same way as the other signals.
// To achieve an identical treatment of dead and alive signals, the setting of deadcheck=0
// will automatically set also mode=0 to retrieve the stored signal values "as is".
// The default is deadcheck=1 (for backward compatibility reasons).
//
// For more extended functionality see class NcDevice.

 TObjArray hits;
 LoadHits(classname,&hits);

 NcDevice dev;
 dev.GetExtremes(vmin,vmax,name,&hits,mode,deadcheck);
}
///////////////////////////////////////////////////////////////////////////
void NcEvent::DisplayHits(const char* classname,Int_t idx,Float_t scale,Int_t dp,Int_t mode,Int_t mcol)
{
// 3D color display of the various hits registered to the specified device class.
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

 TObjArray hits;
 LoadHits(classname,&hits);

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
void NcEvent::DisplayHits(const char* classname,TString name,Float_t scale,Int_t dp,Int_t mode,Int_t mcol)
{
// 3D color display of the various hits registered to the specified device class.
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

 TObjArray hits;
 LoadHits(classname,&hits);

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
void NcEvent::ShowHits(const char* classname,Int_t mode,TString f,TString u)
{
// Show all the hits registered to the specified device class.
//
// mode = 0 ==> Only the number of hits will be provided.
//        1 ==> Full listing of all the hits will be provided.
//        2 ==> Same as mode=1 but with additional location info of the owning device.
// 
// Default value is mode=1.
//
// The arguments "f" and "u" have the same meaning as in the memberfunction Data(). 
//
// Note : This memberfunction will show hits in printable format.
//        To obtain a graphic hit display please refer to DisplayHits(). 

 TObjArray hits;
 LoadHits(classname,&hits);

 Int_t nhits=hits.GetEntries();

 cout << " *NcEvent::ShowHits* There are " << nhits
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
TObjArray* NcEvent::SortDevices(const char* classname,TString name,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various devices based on hit signals registered
// to the specified device class.
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

 if (ordered) ordered->Clear();

 TObjArray hits;
 SortHits(classname,name,mode,mcal,deadcheck,&hits);
 
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
TObjArray* NcEvent::SortDevices(const char* classname,Int_t idx,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
{
// Order the references to the various devices based on hit signals registered
// to the specified device class.
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

 TObjArray hits;
 SortHits(classname,idx,mode,mcal,deadcheck,&hits);
 
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
TObjArray* NcEvent::SortDevices(TObjArray* hits,TString name,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
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
 NcDevice dev;
 if (mode) dev.SortHits(name,mode,hits,mcal,deadcheck,sorthits);

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
TObjArray* NcEvent::SortDevices(TObjArray* hits,Int_t idx,Int_t mode,Int_t mcal,Int_t deadcheck,TObjArray* ordered)
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
 NcDevice dev;
 if (mode) dev.SortHits(idx,mode,hits,mcal,deadcheck,sorthits);

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
TObject* NcEvent::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers when adding objects in case the container owns the objects.
// This feature allows to store either NcEvent objects or objects derived from
// NcEvent via some generic AddEvent memberfunction, provided these derived
// classes also have a proper Clone memberfunction. 

 NcEvent* evt=new NcEvent(*this);
 if (name)
 {
  if (strlen(name)) evt->SetName(name);
 }
 return evt;
}
///////////////////////////////////////////////////////////////////////////

