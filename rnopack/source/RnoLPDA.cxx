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
/** @class RnoLPDA
~~~
// Class RnoLPDA
// Signal/Hit handling of an RNO-G surface LogPeriodic Dipole Antenna (LPDA).
// Basically this class provides an RNO-G tailored user interface
// to the functionality of the class NcDevice via the generic RnoLPDA
// and RnoGANT classes.
//
// See RnoGANT for some usage examples.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, May 3, 2021  10:52Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, June 23, 2021  15:34Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "RnoLPDA.h"
#include "Riostream.h"
 
ClassImp(RnoLPDA); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
RnoLPDA::RnoLPDA(const char* name,const char* title) : RnoSANT(name,title)
{
/**
~~~
// Default constructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
RnoLPDA::~RnoLPDA()
{
/**
~~~
// Default destructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
RnoLPDA::RnoLPDA(const RnoLPDA& q) : RnoSANT(q)
{
/**
~~~
// Copy constructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
TObject* RnoLPDA::Clone(const char* name) const
{
/**
~~~
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects. This feature allows e.g. NcEvent
// to store either RnoLPDA objects or objects derived from RnoLPDA
// via tha AddDevice memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 
~~~
**/

 RnoLPDA* q=new RnoLPDA(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
