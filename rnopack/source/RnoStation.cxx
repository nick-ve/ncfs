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
// Class RnoStation
// Handling of RNO-G event data.
// Basically this class provides an RNO-G tailored user interface
// to the functionality of the class NcDetectorUnit.
//
// Notes :
// ------
// * In the event structure MC tracks are labeled with a negative track ID,
//   whereas reconstructed tracks are labeled with a positive track ID.
//   This allows for a direct selection of either MC or Reco tracks via the
//   GetIdTrack() facility.
// * The particle codes used for the various tracks are the PDG ones.
//   In the need of RNO-G specific "particle" types the PDG database
//   may be extended in the usual way.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, June 22, 2021  08:23Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 11, 2021  00:34Z
///////////////////////////////////////////////////////////////////////////

#include "RnoStation.h"
#include "Riostream.h"
 
ClassImp(RnoStation) // Class implementation to enable ROOT I/O
 
RnoStation::RnoStation(const char* name,const char* title) : NcDetectorUnit(name,title)
{
// Default constructor.

 fCanvas=0;
}
///////////////////////////////////////////////////////////////////////////
RnoStation::~RnoStation()
{
// Default destructor.

 if (fCanvas)
 {
  delete fCanvas;
  fCanvas=0;
 }
}
///////////////////////////////////////////////////////////////////////////
RnoStation::RnoStation(const RnoStation& q) : NcDetectorUnit(q)
{
// Copy constructor.
}
///////////////////////////////////////////////////////////////////////////
TCanvas* RnoStation::DisplayWaveform(Int_t ich,Int_t j)
{
// Display the waveform of the j-th sampled observable (1=first) for the selected channel number "ich".
// The graph will display the values of the j-th observable versus the sample entry number.
//
// If ich<0 the corresponding waveforms of all channels from this station are displayed.
//
// The returned argument is the pointer to the created canvas.
// For extended functionality, please refer to the (inherited) memberfunction DisplaySample().
//
// The default value is j=1.

 if (fCanvas)
 {
  delete fCanvas;
  fCanvas=0;
 }

 fCanvas=new TCanvas();

 TString devname="Ch";
 devname+=ich;

 Int_t id=GetUniqueID(); // The station ID
 TString staname="Station";
 staname+=id;

 TString title="";

 // Display the waveform of a single channel
 if (ich>=0)
 {
  title=staname;
  title+=" ";
  title+=devname;
  fCanvas->SetName(ClassName());
  fCanvas->SetTitle(title);
  RnoGANT* ant=(RnoGANT*)GetDevice(devname,kTRUE);
  if (ant) ant->DisplaySample(j);

  // Change the title of the graph
  TNamed* obj=(TNamed*)fCanvas->FindObject("NcSample");
  if (obj) obj->SetTitle(title);

  return fCanvas;
 }

 // Display the waveforms of all channels
 title=staname;
 fCanvas->SetName(ClassName());
 fCanvas->SetTitle(title);
 fCanvas->Divide(4,6);
 for (Int_t jch=0; jch<24; jch++)
 {
  fCanvas->cd(jch+1);
  devname="Ch";
  devname+=jch;
  RnoGANT* ant=(RnoGANT*)GetDevice(devname,kTRUE);
  if (ant) ant->DisplaySample(j);

  // Change the title of the graph
  title=staname;
  title+=" ";
  title+=devname;
  TVirtualPad* pad=fCanvas->GetPad(jch+1);
  if (pad)
  {
   TNamed* obj=(TNamed*)pad->FindObject("NcSample");
   if (obj) obj->SetTitle(title);
  }
 }
 
 return fCanvas;
}
///////////////////////////////////////////////////////////////////////////
TObject* RnoStation::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.

 RnoStation* q=new RnoStation(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
