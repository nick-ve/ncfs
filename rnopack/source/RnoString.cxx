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
// Class RnoString
// Data handling of an RNO-G string.
// Basically this class provides an RNO-G tailored user interface
// to the functionality of the class NcDetectorUnit.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, June 24, 2021  16:00Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, June 24, 2021  16:00Z
///////////////////////////////////////////////////////////////////////////

#include "RnoString.h"
#include "Riostream.h"
 
ClassImp(RnoString) // Class implementation to enable ROOT I/O
 
RnoString::RnoString(const char* name,const char* title) : NcDetectorUnit(name,title)
{
// Default constructor.
}
///////////////////////////////////////////////////////////////////////////
RnoString::~RnoString()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
RnoString::RnoString(const RnoString& q) : NcDetectorUnit(q)
{
// Copy constructor.
}
///////////////////////////////////////////////////////////////////////////
TObject* RnoString::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.

 RnoString* q=new RnoString(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
