/*******************************************************************************
~~~
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
~~~
 *******************************************************************************/

///////////////////////////////////////////////////////////////////////////
/** @class RnoDANT
~~~
// Class RnoDANT
// Signal/Hit handling of an RNO-G Deep Antenna (DANT).
// Basically this class provides an RNO-G tailored user interface
// to the functionality of the class NcDevice via the generic RnoGANT class.
//
// See RnoGANT for some usage examples.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, May 3, 2021  10:52Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, June 23, 2021  15:32Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "RnoDANT.h"
#include "Riostream.h"
 
ClassImp(RnoDANT); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
RnoDANT::RnoDANT(const char* name,const char* title) : RnoGANT(name,title)
{
/**
~~~
// Default constructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
RnoDANT::~RnoDANT()
{
/**
~~~
// Default destructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
RnoDANT::RnoDANT(const RnoDANT& q) : RnoGANT(q)
{
/**
~~~
// Copy constructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
TObject* RnoDANT::Clone(const char* name) const
{
/**
~~~
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects. This feature allows e.g. NcEvent
// to store either RnoDANT objects or objects derived from RnoDANT
// via tha AddDevice memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 
~~~
**/

 RnoDANT* q=new RnoDANT(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
