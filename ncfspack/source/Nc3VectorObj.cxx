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

// $Id: Nc3VectorObj.cxx 5 2010-03-19 10:10:02Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class Nc3VectorObj
// Handling of 3-vectors in various reference frames.
//
// This class is meant to provide an Nc3Vector object which is derived
// from TObject such that it can be stored in e.g. TObjArray etc...
// and that it can be written out using the ROOT I/O machinery.
//
// Example :
// =========
//
// Float_t a[3]={1,2,3};
// Float_t ea[3]={0.01,0.02,0.03};
// Float_t b[3]={4,5,6};
// Float_t eb[3]={0.04,0.05,0.06};
//
// Nc3Vector v,w;
//
// v.SetVector(a,"car");
// v.SetErrors(ea,"car");
// w.SetVector(b,"car");
// w.SetErrors(eb,"car");
//
// Nc3Vector cross=v.Cross(w);
//
// Nc3Vector add=v+w;
//
// Nc3VectorObj vec1(cross);
//
// Nc3VectorObj vec2;
// vec2.Load(add);
//
// vec1.Data();
// vec2.Data();
//
//--- Author: Nick van Eijndhoven 18-oct-1999 Utrecht University
//- Modified: NvE $Date: 2010-03-19 11:10:02 +0100 (Fri, 19 Mar 2010) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "Nc3VectorObj.h"
 
ClassImp(Nc3VectorObj) // Class implementation to enable ROOT I/O
 
Nc3VectorObj::Nc3VectorObj() : TObject(),Nc3Vector()
{
// Default constructor
// Creation of an Nc3VectorObj object and initialisation of parameters.
// All attributes initialised to 0.
}
///////////////////////////////////////////////////////////////////////////
Nc3VectorObj::Nc3VectorObj(Nc3Vector& q) : TObject(),Nc3Vector(q)
{
// Creation of an Nc3VectorObj object and initialisation of parameters.
// All attributes are initialised to the values of the input Nc3Vector.
}
///////////////////////////////////////////////////////////////////////////
Nc3VectorObj::~Nc3VectorObj()
{
// Destructor to delete dynamically allocated memory.
}
///////////////////////////////////////////////////////////////////////////
Nc3VectorObj::Nc3VectorObj(Nc3VectorObj& v) : TObject(v),Nc3Vector(v)
{
// Copy constructor
}
///////////////////////////////////////////////////////////////////////////
