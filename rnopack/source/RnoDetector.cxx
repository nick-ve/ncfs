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
// Class RnoDetector
// Handling of RNO-G detector data.
// Basically this class provides an RNO-G tailored user interface
// to the functionality of the class NcDetector.
//
// The RNO-G detector array is foreseen to consist of 35 so called detector stations,
// each of which consists of 3 vertical strings deep in the ice equipped with antennas.
// These antennas come in two varieties, namely horizontally polarized (HPOL)
// and vertically polarized (VPOL).
// In addition to this, there are also LogPeriodic Dipole Antennas (LPDAs)
// located in 3 trenches at the surface.
// Also these LPDA antennas come in two varieties, namely upward oriented (ULPDA)
// and downward oriented (DLPDA).
// For the storage and analysis of the RNO-G data, the following hierarchical
// detector structure has been developed :
// * Every antenna is represented via an NcDevice.
//   To distinguish the different antenna types, new classes (derived from NcDevice)
//   are introduced, namely RnoHPOL, RnoVPOL, RnoULPDA and RnoDLPDA.
// * To represent the various strings, a new class RnoString (derived from NcDetectorUnit)
//   is introduced.
// * All RnoHPOL and RnoVPOL objects are grouped in the corresponding RnoString objects.
// * The RnoULPDA and RnoDLPDA objects are treated as individual entities.
// * The 3 RnoString objects and the LPDA antennas are grouped in a RnoStation object
//   that is derived from NcDetectorUnit.
// * Finally all 35 RnoStation objects are grouped into the overall RnoDetector.
//
// The various stations are labeled via a unique identifier reflecting a column and row index,
// representing the station position in the rectangular grid.
// The column and row indexing reflect a matrix (col,row) numbering scheme, starting at the
// left lower corner of the matrix. 
// This implies that the row index increases in the direction of True South->North and the
// column index increases in the direction True West->East. 
// For instance, id=13 represents the station at grid location (col,row)=(1,3).
//
// The name of a station is composed of the word "Station" and the corresponding id.
// So, the name of the station with id=13 is "Station13". 
// Station21 is the one closest to the Summit Science building and DISC borehole.
//
// Within a certain station, the strings are named "String1" (=power string),
// "String2" (=helper string 1) and "String3" (=helper string 2).
//
// The names of the various antennas within a certain station are composed of the word "Ch"
// and the corresponding channel number.
// So, the name of the antenna with channel number 11 is "Ch11" (=HPOL on helper string 1).
//
// In addition to the above device naming, all devices also have been given a unique ID.
// This unique ID is composed of the station id, string number and antenna number.
// The string number corresponds to the number in the name of the string, whereas the
// antennas are numbered according to their depth, starting with 1 for the top most position
// on a string. So, the antenna number of Ch5 will be 3 and that of Ch10 will be 8.
// Since the surface LPDAs are not connected to a string, their string number will be 0,
// and the LPDAs CH12-20 are given the antenna numbers 1-9 in consecutive order. 
// This unique ID allows for each device to immediately retrieve the corresponding station,
// string and antenna number via their GetStation(), GetString() and GetNumber() memberfuntions.
//
// The (data of the) various detector units and devices can be accessed using the standard
// NcDetector, NcDetectorUnit, NcDevice and NcSignal memberfunctions.
// For easy access to a certain class of devices, the following base classes have been introduced :
//
// RnoGANT : Comprising all Generic antennas (RnoDANT and RnoSANT)
// RnoDANT : Comprising all Deep in-ice antennas (RnoVPOL and RnoHPOL)
// RnoSANT : Comprising all Surface antennas (RnoLPDA)
// RnoLPDA : Comprising all LPDA antennas (RnoULPDA and RnoDLPDA)
//
// So, an access request for RnoDANT derived devices will provide all RnoVPOL and RnoHPOL antennas.
//
// The status of the various detector units is indicated by a user defined status word,
// which is accessible via the standard memberfunctions SetStatus() and GetStatus().
// The used conventions for the status word are :
// status : 0 --> Unit is working properly.
//         -1 --> Unit is switched off.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, July 6, 2021  15:42Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, November 25, 2021  14:10Z
///////////////////////////////////////////////////////////////////////////

#include "RnoDetector.h"
#include "Riostream.h"
 
ClassImp(RnoDetector) // Class implementation to enable ROOT I/O
 
RnoDetector::RnoDetector(const char* name,const char* title) : NcDetector(name,title)
{
// Default constructor.

 TString sname=name;
 TString stitle=title;

 if (sname=="") sname="RNO-G";
 if (stitle=="") stitle="The Radio Neutrino Observatory in Greenland";

 SetNameTitle(sname,stitle);
}
///////////////////////////////////////////////////////////////////////////
RnoDetector::~RnoDetector()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
RnoDetector::RnoDetector(const RnoDetector& d) : NcDetector(d)
{
// Copy constructor.
}
///////////////////////////////////////////////////////////////////////////
RnoStation* RnoDetector::GetStation(Int_t id,Bool_t create)
{
// Provide the pointer to the station with the specified "id".
// In case create=kTRUE the station will be created if it is not already present.
// If a station with the specified id can not be found, the value 0 is returned.

 // Only allow the "matrix element" labeled station IDs
 if (id<0 || !id%10)
 {
  cout << " *" << ClassName() << "::GetStation* Invalid station ID : " << id << endl;
  return 0;
 }

 RnoStation* station=0;

 TString name="Station";
 name+=id;

 station=(RnoStation*)GetDetectorUnit(name,kTRUE);
 
 if (!create) return station;

 // Creation of a new station
 CreateStation(id);

 station=(RnoStation*)GetDetectorUnit(name,kTRUE);

 return station;
}
///////////////////////////////////////////////////////////////////////////
void RnoDetector::CreateStation(Int_t id)
{
// Internal memberfunction to create a station with the specified id.

 RnoStation station("Station","RNO-G detector station");

 RnoString string("String","RNO-G string");

//@@@@ NcDetectorUnit trench("Trench","RNO-G surface trench");

 RnoULPDA ulpda("Channel","LPDA-up antenna");
 RnoDLPDA dlpda("Channel","LPDA-down antenna");
 RnoVPOL vpol("Channel","Vpol antenna");
 RnoHPOL hpol("Channel","Hpol antenna");

 Int_t nstr=3; // Number of strings per station
 Int_t ntre=3; // Number of surface trenches per station

 Int_t staid=id; // The unique station ID
 Int_t strid=0;  // The unique string ID
 Int_t antid=0;  // The unique antenna ID
 TString name="";
 Float_t depths[9]={-40,-60,-80,-92,-93,-94,-95,-96,-97}; // All antenna depths

 Int_t icha=0; // The RADIANT channel number

 name="Station";
 name+=staid;
 station.SetName(name);
 station.SetUniqueID(staid);

 // Loop over the strings of this station
 for (Int_t istr=1; istr<=nstr; istr++)
 {
  strid=1000*staid+100*istr;
  string.Reset();
  string.SetUniqueID(strid);

  name="String";
  name+=istr;
  string.SetName(name);

  // Power string
  if (istr==1) // Power string
  {
   for (Int_t ia=1; ia<=9; ia++)
   {
    antid=strid+ia;
    vpol.Reset();
    vpol.SetUniqueID(antid);
    vpol.SetPosition(0,-20,depths[ia-1],"car");

    hpol.Reset();
    hpol.SetUniqueID(antid);
    hpol.SetPosition(0,-20,depths[ia-1],"car");

    if (ia<4 || ia>5) // VPOL setting
    {
     icha=8-ia;
     if (ia>5) icha++;
     name="Ch";
     name+=icha;
     vpol.SetName(name);
     string.AddDevice(vpol);
    }
    if (ia==4 || ia==5) // HPOL setting
    {
     if (ia==4) icha=4;
     if (ia==5) icha=8;
     name="Ch";
     name+=icha;
     hpol.SetName(name);
     string.AddDevice(hpol);
    }
   }
  }

  // Helper string
  if (istr>1)
  {
   for (Int_t ia=7; ia<=9; ia++)
   {
    antid=strid+ia;
    vpol.Reset();
    vpol.SetUniqueID(antid);
    vpol.SetPosition(45-istr*15,10,depths[ia-1],"car");

    hpol.Reset();
    hpol.SetUniqueID(antid);
    hpol.SetPosition(45-istr*15,10,depths[ia-1],"car");

    icha=18-ia; // Helper string 1
    if (istr==3) icha=14+ia; // Helper string 2

    if (ia==7)
    {
     name="Ch";
     name+=icha;
     hpol.SetName(name);
     string.AddDevice(hpol);
    }
    else
    {
     name="Ch";
     name+=icha;
     vpol.SetName(name);
     string.AddDevice(vpol);
    }
   }
  }
  station.AddDevice(string);
 } // End of string loop

 // Loop over the surface trenches with surface antennas of this station
 icha=12;
 Int_t iadd=0;
 for (Int_t itre=1; itre<=ntre; itre++)
 {
  for (Int_t ia=1; ia<=3; ia++)
  {
   ulpda.Reset();
   ulpda.SetUniqueID(ia+iadd+1000*staid);
   ulpda.SetPosition(ia-2,itre,0,"car");

   dlpda.Reset();
   dlpda.SetUniqueID(ia+iadd+1000*staid);
   dlpda.SetPosition(ia,itre,0,"car");

   if (ia==1 || ia==3)
   {
    name="Ch";
    name+=icha;
    dlpda.SetName(name);
    station.AddDevice(dlpda);
   }
   else
   {
    name="Ch";
    name+=icha;
    ulpda.SetName(name);
    station.AddDevice(ulpda);
   }
   icha++;
  }
  iadd+=3;
 } // End of trench loop

 AddDevice(station);
}
///////////////////////////////////////////////////////////////////////////
TGraph* RnoDetector::DisplaySampling(Int_t ista,Int_t ich,Int_t j)
{
// Display the sampling of the j-th sampled observable (1=first) for the selected channel number "ich"
// of the station with ID=ista.
// The graph will display the values of the j-th observable versus the sample entry number.
//
// The returned argument is the pointer to the created graph.
// For extended functionality, please refer to the (inherited) memberfunction DisplaySample().
//
// The default value is j=1.

 RnoStation* sta=GetStation(ista,kFALSE);

 if (!sta)
 {
  cout << " *" << ClassName() << "::DisplayWaveform* Could not find Station" << ista << endl;
  return 0;
 }

 TGraph* gr=sta->DisplaySampling(ich,j);
 return gr;
}
///////////////////////////////////////////////////////////////////////////
TCanvas* RnoDetector::DisplaySamplings(Int_t ista,Int_t j)
{
// Display the samplings of the j-th sampled observable (1=first) for all channels
// of the station with ID=ista.
// The graph will display the values of the j-th observable versus the sample entry number.
//
// The returned argument is the pointer to the created canvas.
// For extended functionality, please refer to the (inherited) memberfunction DisplaySample().
//
// The default value is j=1.

 RnoStation* sta=GetStation(ista,kFALSE);

 if (!sta)
 {
  cout << " *" << ClassName() << "::DisplayWaveform* Could not find Station" << ista << endl;
  return 0;
 }

 TCanvas* c=sta->DisplaySamplings(j);
 return c;
}
///////////////////////////////////////////////////////////////////////////
TObject* RnoDetector::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.

 RnoDetector* d=new RnoDetector(*this);
 if (name)
 {
  if (strlen(name)) d->SetName(name);
 }
 return d;
}
///////////////////////////////////////////////////////////////////////////
