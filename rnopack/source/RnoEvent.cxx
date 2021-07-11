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
// Class RnoEvent
// Handling of RNO-G event data.
// Basically this class provides an RNO-G tailored user interface
// to the functionality of the class NcEvent.
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
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, June 22, 2021  08:24Z
///////////////////////////////////////////////////////////////////////////

#include "RnoEvent.h"
#include "Riostream.h"
 
ClassImp(RnoEvent) // Class implementation to enable ROOT I/O
 
RnoEvent::RnoEvent() : NcEvent()
{
// Default constructor.
}
///////////////////////////////////////////////////////////////////////////
RnoEvent::~RnoEvent()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
RnoEvent::RnoEvent(const RnoEvent& evt) : NcEvent(evt)
{
// Copy constructor.
}
///////////////////////////////////////////////////////////////////////////
void RnoEvent::Reset()
{
// Reset all values.

 NcEvent::Reset();
}
///////////////////////////////////////////////////////////////////////////
TGraph* RnoEvent::DisplaySampling(Int_t ista,Int_t ich,Int_t j)
{
// Display the sampling of the j-th sampled observable (1=first) for the selected channel number "ich"
// of the station with ID=ista.
// The graph will display the values of the j-th observable versus the sample entry number.
//
// The returned argument is the pointer to the created graph.
// For extended functionality, please refer to the (inherited) memberfunction DisplaySample().
//
// The default value is j=1.

 TString name="Station";
 name+=ista;
 RnoStation* sta=(RnoStation*)GetDevice(name);

 if (!sta)
 {
  cout << " *" << ClassName() << "::DisplaySampling* Could not find Station" << ista << endl;
  return 0;
 }

 TGraph* gr=sta->DisplaySampling(ich,j);

 if (!gr) return 0;

 // Add run and event numbers to the graph title
 TString title=gr->GetTitle();
 title+=" Run:";
 title+=GetRunNumber();
 title+=" Event:";
 title+=GetEventNumber();
 gr->SetTitle(title);

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TCanvas* RnoEvent::DisplaySamplings(Int_t ista,Int_t j)
{
// Display the samplings of the j-th sampled observable (1=first) for all channels
// of the station with ID=ista.
// The graph will display the values of the j-th observable versus the sample entry number.
//
// The returned argument is the pointer to the created canvas.
// For extended functionality, please refer to the (inherited) memberfunction DisplaySample().
//
// The default value is j=1.

 TString name="Station";
 name+=ista;
 RnoStation* sta=(RnoStation*)GetDevice(name);

 if (!sta)
 {
  cout << " *" << ClassName() << "::DisplaySamplings* Could not find Station" << ista << endl;
  return 0;
 }

 TCanvas* c=sta->DisplaySamplings(j);

 if (!c) return 0;

 // Indicate the Run and Event number at each sampling display
 TVirtualPad* pad=0;
 TGraph* gr=0;
 TString title="";
 for (Int_t i=1; i<=24; i++)
 {
  pad=c->cd(i);
  if (!pad) continue;

  gr=(TGraph*)pad->FindObject("NcSample");
  if (!gr) continue;

  title=gr->GetTitle();
  title+=" Run:";
  title+=GetRunNumber();
  title+=" Event:";
  title+=GetEventNumber();
  gr->SetTitle(title);
 } // End of loop over the channels

 return c;
}
///////////////////////////////////////////////////////////////////////////
TObject* RnoEvent::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.

 RnoEvent* evt=new RnoEvent(*this);
 if (name)
 {
  if (strlen(name)) evt->SetName(name);
 }
 return evt;
}
///////////////////////////////////////////////////////////////////////////
