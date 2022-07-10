/*******************************************************************************
 * Copyright(c) 2021, RNO-G Experiment at Summit Station. All rights reserved.
 *
 * Author: The RNO-G NCFS-based Offline Project.
 * Contributors are mentioned in the code where appropriate.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation strictly for non-commercial purposes is hereby granted
 * without fee, provided that the above copyright notice appears in all
 * copies and that both the copyright notice and this permission notice
 * appear in the supporting documentation.
 * The authors make no claims about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *******************************************************************************/

///////////////////////////////////////////////////////////////////////////
// Class RnoGANT
// Signal/Hit handling of an RNO-G Generic Antenna (GANT).
// Basically this class provides an RNO-G tailored user interface
// to the functionality of the class NcDevice.
// This class is meant to provide a base class for more specific antennas
// (i.e. LPDAs, HPOLs or VPOLs).
// To specifically address Surface or Deep antennas, please refer to the 
// derived classes RnoSANT and RnoDANT, respectively.
//
// Example :
// =========
//
// Creation and filling of an RNO-G Generic Antenna with fictituous data
// ---------------------------------------------------------------------
//
// For further functionality please refer to NcDevice, NcSignal and NcAttrib.
//
// RnoGANT m;
// m.SetUniqueID(123);
// m.SetNameTitle("ANT123","Generic RNO-G antenna");
//
// // Indicate status (e.g. version of readout electronics)
// // via a user definable status word.
// Int_t stat=20031;
// m.SetStatus(stat);
//
// Float_t pos[3]={1,2,3};
// m.SetPosition(pos,"car");
//
// // The starting unique signal ID.
// // In this example it will be increased automatically
// // whenever a new signal is created.
// Int_t sid=10;
//
// NcSignal s;
//
// s.SetSlotName("ADC",1);
// s.SetSlotName("LE",2);
// s.SetSlotName("TOT",3);
//
// s.Reset();
// s.SetName("ANT123 Hit 1");
// s.SetUniqueID(sid++);
// s.SetSignal(100,"ADC");
// s.SetSignal(-100,"LE");
// s.SetSignal(-1000,"TOT");
// m.AddHit(s);
//
// s.Reset();
// s.SetName("ANT123 Hit 2");
// s.SetUniqueID(sid++);
// s.SetSignal(110,"ADC");
// s.SetSignal(-101,"LE");
// s.SetSignal(1001,"TOT");
// m.AddHit(s);
//
// s.Reset();
// s.SetName("ANT123 Hit 3");
// s.SetUniqueID(sid++);
// s.SetSignal(120,"ADC");
// s.SetSignal(-102,"LE");
// s.SetSignal(-1002,"TOT");
// m.AddHit(s);
//
// // Provide module data overview
// m.Data();
//
// // Accessing the 3rd stored hit
// NcSignal* sx=m.GetHit(3);
// if (sx) sx->Data();
//
// // Explicit hit selection via unique ID
// NcSignal* sx=m.GetIdHit(12);
// if (sx) sx->Data();
//
// // Obtain the minimum and maximum recorded TOT value 
// Float_t vmin,vmax;
// m.GetExtremes(vmin,vmax,"TOT");
// cout << " Extreme values : vmin = " << vmin << " vmax = " << vmax << endl;
// 
// // Ordered hits w.r.t. decreasing TOT
// TObjArray* ordered=m.SortHits("TOT",-1);
// Int_t  nhits=0;
// if (ordered) nhits=ordered->GetEntries();
// for (Int_t i=0; i<nhits; i++)
// {
//  NcSignal* sx=(NcSignal*)ordered->At(i);
//  if (sx) sx->Data();
// }
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, May 3, 2021  10:52Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 5, 2022  10:27Z
///////////////////////////////////////////////////////////////////////////

#include "RnoGANT.h"
#include "Riostream.h"
 
ClassImp(RnoGANT) // Class implementation to enable ROOT I/O
 
RnoGANT::RnoGANT(const char* name,const char* title) : NcDevice(name,title)
{
// Default constructor.
}
///////////////////////////////////////////////////////////////////////////
RnoGANT::~RnoGANT()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
RnoGANT::RnoGANT(const RnoGANT& q) : NcDevice(q)
{
// Copy constructor.
}
///////////////////////////////////////////////////////////////////////////
Int_t RnoGANT::GetStation(Int_t id) const
{
// Provide the corresponding station number for this antenna.
//
// In case the user has specified the input argument id>0, the station number
// corresponding to the specified id for the current antenna class will be returned.
// Otherwise the station number corresponding with the current antenna will be returned.
//
// In case of inconsistent data, the value -1 is returned.
//
// The default value is id=0;

 Int_t antid=GetUniqueID();
 if (id>0) antid=id;

 if (antid<=0) return -1;

 Int_t station=-1;

 station=antid/1000;

 return station;
}
///////////////////////////////////////////////////////////////////////////
Int_t RnoGANT::GetString(Int_t id) const
{
// Provide the string number for the antenna with the specified unique "id".
//
// For id=0, the string number corresponding with the current antenna will be returned.
//
// Note : For the surface antennas the value 0 will be returned. 
//
// In case of inconsistent data, the value -1 is returned.
//
// The default value is id=0;

 Int_t antid=GetUniqueID();
 if (id>0) antid=id;

 if (antid<=0) return -1;

 Int_t string=-1;

 if (InheritsFrom("RnoSANT"))
 {
  string=0;
 }
 else
 {
  antid=antid%1000;
  string=antid/100;
 }

 return string;
}
///////////////////////////////////////////////////////////////////////////
Int_t RnoGANT::GetNumber(Int_t id) const
{
// Provide the antenna or DAQ channel number according to the specified unique "id".
//
// Input argument :
// ----------------
// id > 0 : Antenna number (see below) for the specified unique "id" will be provided.
//    = 0 : Antenna number (see below) for the current antenna will be provided.
//    < 0 : DAQ channel number for the current antenna will be provided.
//
// Definition of antenna number :
// ------------------------------
// Number=j indicates the j-th antenna on the string, where j=1
// corresponds to the antenna at the top of the power string.
//
// The top level antennas on the helper strings correspond to number=7.
//
// The surface antennas are not connected to a string, but they are
// numbered 1-9 to reflect the Channels 12-20 consecutively.
//
// The default value is id=0;
//
// In case of inconsistent input the value -1 is returned.

 Int_t number=-1;

 // DAQ channel number was requested
 if (id<0)
 {
  TString name=GetName();

  if (!name.Contains("Ch")) return -1;

  name.ReplaceAll("Ch","");
  number=name.Atoi();
  return number;
 }

 // Antenna number was requested 
 Int_t antid=GetUniqueID();
 if (id>0) antid=id;

 Int_t station=GetStation(antid);
 Int_t string=GetString(antid);

 if (station<0 || string<0) return -1;

 number=antid%100;

 return number;
}
///////////////////////////////////////////////////////////////////////////
Int_t RnoGANT::GetANTId(Int_t station,Int_t string,Int_t number) const
{
// Provide the antenna unique ID based on the station, string and number indicators.
//
// Definition of antenna number :
// ------------------------------
// Number=j indicates the j-th antenna on the string, where j=1
// corresponds to the antenna at the top of the power string.
//
// The top level antennas on the helper strings correspond to number=7.
//
// The surface antennas are not connected to a string, but they are
// numbered 1-9 to reflect the Channels 12-20 consecutively.
//
// In case of inconsistent input the value -1 is returned.

 Int_t antid=-1;

 if (station<11 || station>77 || string<1 || string>3 || number<=0 || number>9) return -1;

 if (!(station%10)) return -1; // Station IDs must represent the matrix (col,row) indexing

 if ((string==2 || string==3) && number<7) return -1; // Helper string antenna numbers start at 7

 antid=1000*station+100*string+number;

 return antid;
}
///////////////////////////////////////////////////////////////////////////
Double_t RnoGANT::GetTimeResidual(NcEvent* evt,NcTrack* t,NcSignal* s,TString name,Int_t mode,Int_t vgroup) const
{
// Provide the time residual (in nanoseconds) of a recorded signal w.r.t. a track.
//
// Input arguments :
// -----------------
//
// evt    : Event pointer.
// t      : Track pointer.
// s      : Signal pointer.
// name   : Name of the signal slot that contains the recorded hit time.
// mode   : Mode to retrieve the signal value.
//          (see the memberfunction GetSignal() of class NcSignal for details)
// vgroup : Take the difference between group and phase velocity of the Cherenkov front
//          into account (1) or not (0).
//
// The default values are : name="LE", mode=7 and vgroup=1.
//
// Note : In case of inconsistent input a value of -99999 will be returned. 
//
// For further details concerning the definition and calculation of the
// time residual please refer to Astroparticle Physics 28 (2007) 456.

 Double_t tres=-99999;

 if (!evt || !t || !s) return tres;

 RnoGANT* ant=(RnoGANT*)s->GetDevice();
 if (!ant) return tres;

 Nc3Vector p=t->Get3Momentum();
 if (!p.HasVector() || !p.GetNorm()) return tres;

 const Float_t pi=acos(-1.);
 const Float_t c=0.299792458;         // Light speed in vacuum in meters per ns
 const Float_t npice=1.78829165;      // Phase refractive index (c/v_phase) of ice (IceCube optical 1.31768387)
 const Float_t ngice=1.0251*npice;    // Group refractive index (c/v_group) of ice (IceCube optical 1.35075806)
 const Float_t thetac=acos(1./npice); // Cherenkov angle (in radians)

 // Angular reduction of complement of thetac due to v_phase and v_group difference
 Float_t alphac=0;
 if (vgroup) alphac=atan((1.-npice/ngice)/sqrt(npice*npice-1.));

 // The track reference point
 NcPosition* r0=t->GetReferencePoint();
 if (!r0) return tres;

 // Absolute (UT) time stamp of the track reference point
 NcTimestamp* tt0=r0->GetTimestamp();
 if (!tt0) return tres;

 // Time stamp of the track relative to the event time stamp 
 Float_t t0=evt->GetDifference(tt0,"ns");

 NcPosition rhit=ant->GetPosition();
 Float_t d=t->GetDistance(rhit);
 Nc3Vector r12=rhit-(*r0);
 Float_t hproj=p.Dot(r12)/p.GetNorm();
 Float_t dist=fabs(hproj)+d/tan(pi/2.-thetac-alphac);
 if (hproj<0) dist=-dist;
 Float_t tgeo=t0+dist/c;               // The predicted geometrical hit time
 Float_t thit=s->GetSignal(name,mode); // Hit time relative to the event time stamp

 tres=thit-tgeo;
 return tres;
}
///////////////////////////////////////////////////////////////////////////
Double_t RnoGANT::GetTimeResidual(NcEvent* evt,NcTrack* t,Int_t j,TString name,Int_t mode,Int_t vgroup) const
{
// Provide the time residual (in nanoseconds) of the j-th hit (first hit is at j=1) w.r.t. a track.
//
// Input arguments :
// -----------------
//
// evt    : Event pointer.
// t      : Track pointer.
// j      : Index to the j-th hit of the current device (j=1 is first hit).
// name   : Name of the signal slot that contains the recorded hit time.
// mode   : Mode to retrieve the signal value.
//          (see the memberfunction GetSignal() of class NcSignal for details)
// vgroup : Take the difference between group and phase velocity of the Cherenkov front
//          into account (1) or not (0).
//
// The default values are : name="LE", mode=7 and vgroup=1.
//
// Note : In case of inconsistent input a value of -99999 will be returned. 
//
// For further details concerning the definition and calculation of the
// time residual please refer to Astroparticle Physics 28 (2007) 456.

 NcSignal* s=GetHit(j);
 Double_t tres=GetTimeResidual(evt,t,s,name,mode,vgroup);
 return tres;
}
///////////////////////////////////////////////////////////////////////////
TObject* RnoGANT::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects. This feature allows e.g. NcEvent
// to store either RnoGANT objects or objects derived from RnoGANT
// via tha AddDevice memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 

 RnoGANT* q=new RnoGANT(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
