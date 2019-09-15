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

// $Id: NcCalmodule.cxx 5 2010-03-19 10:10:02Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcCalmodule
// Description of a module in a calorimeter system.
// A matrix geometry is assumed, such that a module
// is identified by (row,col) and contains a certain signal.
// Note : row and col start counting at 1.
//
//--- Author: Nick van Eijndhoven 13-jun-1997 Utrecht University
//- Modified: NvE $Date: 2010-03-19 11:10:02 +0100 (Fri, 19 Mar 2010) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "NcCalmodule.h"
#include "Riostream.h"
 
ClassImp(NcCalmodule) // Class implementation to enable ROOT I/O
 
NcCalmodule::NcCalmodule() : NcSignal()
{
// Default constructor, all module data is set to 0
 fRow=0;
 fCol=0;
 fSigc=0;
}
///////////////////////////////////////////////////////////////////////////
NcCalmodule::~NcCalmodule()
{
// Default destructor
}
///////////////////////////////////////////////////////////////////////////
NcCalmodule::NcCalmodule(const NcCalmodule& m) : NcSignal(m)
{
// Copy constructor
 fRow=m.fRow;
 fCol=m.fCol;
 fSigc=m.fSigc;
}
///////////////////////////////////////////////////////////////////////////
NcCalmodule::NcCalmodule(Int_t row,Int_t col,Double_t sig) : NcSignal()
{
// Module constructor with initialisation of module data
 fRow=row;
 fCol=col;
 NcSignal::SetSignal(sig);
 fSigc=sig;
}
///////////////////////////////////////////////////////////////////////////
void NcCalmodule::SetRow(Int_t i)
{
// Set the row number for this module
 fRow=i;
}
///////////////////////////////////////////////////////////////////////////
void NcCalmodule::SetColumn(Int_t i)
{
// Set the column number for this module
 fCol=i;
}
///////////////////////////////////////////////////////////////////////////
void NcCalmodule::SetSignal(Double_t sig,Int_t j)
{
// Set or change the data of the module.
// This is an extension of NcSignal::SetSignal in view of the clustered signal.
 NcSignal::SetSignal(sig,j);
 if (j==1) fSigc=sig;
}
///////////////////////////////////////////////////////////////////////////
void NcCalmodule::AddSignal(Double_t sig,Int_t j)
{
// Add or change the data of the module
// This is an extension of NcSignal::AddSignal in view of the clustered signal.
 NcSignal::AddSignal(sig,j);
 if (j==1) fSigc+=sig;
}
///////////////////////////////////////////////////////////////////////////
void NcCalmodule::SetClusteredSignal(Double_t sig)
{
// Set or change the signal of the module after clustering
 fSigc=sig;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCalmodule::GetRow() const
{
// Provide the row number of the module
 return fRow;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCalmodule::GetColumn() const
{
// Provide the column number of the module
 return fCol;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcCalmodule::GetClusteredSignal() const
{
// Provide the signal of the module after clustering.
 Int_t dead=GetDeadValue();
 if (!dead)
 {
  return fSigc;
 }
 else
 {
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* NcCalmodule::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcCalorimeter when adding objects in case the
// container owns the objects. This feature allows e.g. NcCalorimeter
// to store either NcCalmodule objects or objects derived from NcCalmodule
// via tha AddSignal memberfunction, provided these derived classes also have
// a proper Clone memberfunction. 

 NcCalmodule* m=new NcCalmodule(*this);
 if (name)
 {
  if (strlen(name)) m->SetName(name);
 }
 return m;
}
///////////////////////////////////////////////////////////////////////////
