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
