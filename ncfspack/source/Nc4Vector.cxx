/**  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
~~~
 * Copyright(c) 1999 NCFS/IIHE, All Rights Reserved.                           *
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
~~~
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///////////////////////////////////////////////////////////////////////////
/** @class Nc4Vector
~~~
// Class Nc4Vector
// Handling of Lorentz 4-vectors in various reference frames.
//
// This class is meant to serve as a base class for all objects
// that have Lorentz 4-vector characteristics.
// Error propagation is performed automatically.
//
// All 4-vectors are treated in the contravariant form and the convention
// for the metric and the 4-vector components is according to the one
// used in the book "Classical Electrodynamics" by J.D. Jackson.
//
// A 4-vector is said to have a scalar part and a 3-vector part,
// which is indicated by the notation
//
//    x^i = (x^0,x^1,x^2,x^3)
// 
// The scalar part   = x^0
// The 3-vector part = (x^1,x^2,x^3)
//
// In view of accuracy and the fact that e.g. particle identity (mass)
// is preserved in many physics processes, the Lorentz invariant 
// (x^i*x_i) is internally saved together with the scalar part.
//
// This allows the following two modes of functionality :
//
// Scalar mode    : The scalar part and the 3-vector part are considered
//                  as basic quantities and the invariant with its error
//                  is derived from these.  
// Invariant mode : The invariant and the 3-vector part are considered
//                  as basic quantities and the scalar with its error
//                  is derived from these.
//
// The philosophy followed here is the following :
// ===============================================
//
// 1) Invokation of SetVector() sets the scalar and 3-vector parts 
//    and the invariant is calculated from these.
//    Automatically the scalar mode is selected and invokation of
//    SetErrors() will calculate the error on the invariant.
//
// 2) In case the scalar part is modified via SetScalar(), scalar mode is
//    automatically selected and the Lorentz invariant (x^i*x_i) and its
//    error are updated accordingly.
//    The 3-vector part is NOT modified.
//    This situation arises when one e.g. precisely determines the time
//    or energy (x^0).     
//
// 3) In case the Lorentz invariant (x^i*x_i) is modified via SetInvariant(),
//    invariant mode is selected automatically and the scalar part and its
//    error are updated accordingly.
//    The 3-vector part is NOT modified.
//    This situation arises when one e.g. precisely determines the mass.     
//
// 4) In case the vector part is modified via Set3Vector(), then the 
//    current mode determines whether the scalar or the invariant is updated. 
//    Scalar mode    : The Lorentz invariant (x^i*x_i) and its error are updated;
//                     the scalar part and its error are NOT modified. 
//                     This situation arises when one e.g. improves the 3-position
//                     vector for a particle with a very precise timing.     
//    Invariant mode : The scalar part and its error are updated;
//                     the Lorentz invariant (x^i*x_i) and its error are NOT modified.
//                     This situation arises when one e.g. improves the 3-momentum
//                     vector for a particle with known mass.     
//
// The dotproduct is defined such that p.Dot(p) yields the Lorentz invariant
// scalar of the 4-vector p (i.e. m**2 in case p is a 4-momentum).   
//
// Note :
// ------
// Vectors (v), Errors (e), reference frames (f) and angular units (u)
// are specified via
// SetVector(Float_t* v,TString f,TString u)
// SetErrors(Float_t* e,TString f,TString u)
// under the following conventions :
//
// f="car" ==> v in Cartesian coordinates   (x,y,z)
// f="sph" ==> v in Spherical coordinates   (r,theta,phi)
// f="cyl" ==> v in Cylindrical coordinates (rho,phi,z)
//
// u="rad" ==> angles in radians
// u="deg" ==> angles in degrees
//
// The "f" and "u" facilities only serve as a convenient user interface.
// Internally the actual storage of the various components is performed
// in a unique way. This allows setting/retrieval of vector components in a
// user selected frame/unit convention at any time. 
//
// Example :
// ---------
//
// Nc4Vector a;
//
// Float_t v[4]={25,-1,3,7};
// a.SetVector(v,"car");
//
// Float_t vec[4];
// a.GetVector(vec,"sph","deg");
//
// Nc4Vector b;
// Float_t v2[4]={33,6,-18,2};
// b.SetVector(v2,"car");
//
// Float_t dotpro=a.Dot(b);
//
// Float_t x0=16;
// Nc3Vector x;
// Float_t vec2[3]={1,2,3};
// x.SetVector(vec2,"car");
//
// Nc4Vector c;
// c.SetVector(x0,x);
// c.GetVector(vec,"car");
// c.Data("cyl");
// c=a+b;
// c=a-b;
// c=a*5;
//
//--- Author: Nick van Eijndhoven 01-apr-1999 Utrecht University
//- Modified: NvE $Date: 2011-11-09 11:10:37 +0100 (Wed, 09 Nov 2011) $ NCFS
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "Nc4Vector.h"
#include "Riostream.h"
 
ClassImp(Nc4Vector); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
Nc4Vector::Nc4Vector()
{
/**
~~~
// Creation of a contravariant 4-vector and initialisation of parameters.
// All values are initialised to 0.
// Scalar mode is initially selected.
~~~
**/

 fUser=0;
 SetZero();
 fScalar=1;
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector::~Nc4Vector()
{
/**
~~~
// Destructor to delete dynamically allocated memory.
~~~
**/

 if (fUser)
 {
  delete fUser;
  fUser=0;
 }
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector::Nc4Vector(const Nc4Vector& v)
{
/**
~~~
// Copy constructor.
~~~
**/

 fScalar=v.fScalar;
 fV2=v.fV2;
 fDv2=v.fDv2;
 fV0=v.fV0;
 fDv0=v.fDv0;
 fDresult=v.fDresult;
 fV=v.fV;
 fUser=0;
 SetUserData(v.fUser);
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector& Nc4Vector::operator=(const Nc4Vector& q)
{
/**
~~~
// Set the current vector equal to vector q.
~~~
**/

 fScalar=q.fScalar;
 fV2=q.fV2;
 fDv2=q.fDv2;
 fV0=q.fV0;
 fDv0=q.fDv0;
 fDresult=q.fDresult;
 fV=q.fV;
 SetUserData(q.fUser);

 return *this;
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::Load(Nc4Vector& q)
{
/**
~~~
// Load all attributes of the input Nc4Vector into this Nc4Vector object.
~~~
**/

 fScalar=q.fScalar;
 fV2=q.fV2;
 fDv2=q.fDv2;
 fV0=q.fV0;
 fDv0=q.fDv0;
 fDresult=q.fDresult;
 fV=q.fV;
 SetUserData(q.fUser);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetZero()
{
/**
~~~
// (Re)set all attributes to zero.
// Note : The (de)selection of the scalar mode is not modified.
~~~
**/

 fV2=0;
 fDv2=0;
 fV0=0;
 fDv0=0;
 fDresult=0;
 fV.SetZero();
 if (fUser)
 {
  delete fUser;
  fUser=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetVector(Double_t v0,Nc3Vector& v)
{
/**
~~~
// Store contravariant vector.
// The error on the scalar part is initialised to 0.
// The errors on the vector part are taken from the input Nc3Vector.
// Scalar mode is automatically selected. 
// The error on scalar result operations is reset to 0.
~~~
**/

 fScalar=1;
 fV0=v0;
 fV=v;
 fV2=pow(v0,2)-fV.Dot(fV);
 SetScalarError(0);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetVector(Double_t* v,TString f,TString u)
{
/**
~~~
// Store vector according to reference frame f.
// Only vector components will be stored, no errors.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// Scalar mode is automatically selected. 
// The error on scalar result operations is reset to 0.
~~~
**/

 fScalar=1;
 Double_t a[3];
 for (Int_t i=0; i<3; i++)
 {
  a[i]=v[i+1];
 }
 fV0=v[0];
 fV.SetVector(a,f,u);
 fV2=pow(fV0,2)-fV.Dot(fV);
 fDv2=0;
 fDv0=0;
 fDresult=0;
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetVector(Double_t v0,Double_t v1,Double_t v2,Double_t v3,TString f,TString u)
{
/**
~~~
// Store vector according to reference frame f.
// Only vector components will be stored, no errors.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// Scalar mode is automatically selected. 
// The error on scalar result operations is reset to 0.
~~~
**/

 Double_t vec[4];
 vec[0]=v0;
 vec[1]=v1;
 vec[2]=v2;
 vec[3]=v3;
 SetVector(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::GetVector(Double_t* v,TString f,TString u)
{
/**
~~~
// Provide 4-vector components according to reference frame f
// and according to the current mode.
// Scalar mode    : The scalar part is directly returned via v[0].
// Invariant mode : The scalar part is re-calculated via the value
//                  of the Lorentz invariant and then returned via v[0].
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
~~~
**/

 if (fScalar)
 {
  v[0]=fV0;
 }
 else
 {
  v[0]=sqrt(fV.Dot(fV)+fV2);
 } 
 Double_t a[3];
 fV.GetVector(a,f,u);
 for (Int_t i=0; i<3; i++)
 {
  v[i+1]=a[i];
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetVector(Float_t* v,TString f,TString u)
{
/**
~~~
// Store vector according to reference frame f.
// Only vector components will be stored, no errors.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// All errors are initialised to 0.
//
// Scalar mode is automatically selected. 
// The error on scalar result operations is reset to 0.
~~~
**/

 Double_t vec[4];
 for (Int_t i=0; i<4; i++)
 {
  vec[i]=v[i];
 }
 SetVector(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::GetVector(Float_t* v,TString f,TString u)
{
/**
~~~
// Provide 4-vector components according to reference frame f
// and according to the current mode.
// Scalar mode    : The scalar part is directly returned via v[0].
// Invariant mode : The scalar part is re-calculated via the value
//                  of the Lorentz invariant and then returned via v[0].
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
~~~
**/

 Double_t vec[4];
 GetVector(vec,f,u);
 for (Int_t i=0; i<4; i++)
 {
  v[i]=vec[i];
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetScalar()
{
/**
~~~
// Provide the scalar part.
// The error on the scalar value is available via GetResultError()
// after invokation of GetScalar().
~~~
**/

 if (fScalar)
 {
  fDresult=fDv0;
  return fV0;
 }
 else
 {
  Double_t dot=fV.Dot(fV);
  Double_t ddot=fV.GetResultError();
  Double_t v02=dot+fV2;
  Double_t dv02=sqrt(pow(ddot,2)+pow(fDv2,2));
  Double_t v0=sqrt(fabs(v02));
  Double_t dv0=0;
  if (v0) dv0=dv02/(2.*v0);
  fDresult=dv0;
  return v0;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t Nc4Vector::HasVector() const
{
/**
~~~
// Check whether the 3-vector components have been set.
// In case the 3-vector components have been set, the value 1 is returned.
// Otherwise, the value 0 is returned.
~~~
**/

 Int_t val=fV.HasVector();
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t Nc4Vector::HasErrors() const
{
/**
~~~
// Check whether the errors on the 3-vector components have been set.
// In case the errors have been set, the value 1 is returned.
// Otherwise, the value 0 is returned.
~~~
**/

 Int_t val=fV.HasErrors();
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetResultError() const
{
/**
~~~
// Provide the error on the result of an operation yielding a scalar
// E.g. GetScalar(), GetInvariant() or Dot()
~~~
**/

 return fDresult;
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetScalar(Double_t v0,Double_t dv0)
{
/**
~~~
// Modify the scalar part (v0) and its error (dv0).
// The default value for dv0 is 0.
// The vector part is not modified. 
// Scalar mode is automatically selected
// ==> Lorentz invariant and its error are updated. 
// The error on scalar result operations is reset to 0.
~~~
**/

 fScalar=1;
 fV0=v0;
 fV2=pow(v0,2)-fV.Dot(fV);
 SetScalarError(dv0);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetScalarError(Double_t dv0)
{
/**
~~~
// Set the error on the scalar part.
// If in scalar mode, update error on the invariant accordingly.
// The error on scalar result operations is reset to 0.
~~~
**/

 fDv0=dv0;
 if (fScalar)
 {
  Double_t norm=fV.GetNorm();
  Double_t dnorm=fV.GetResultError();
  fDv2=sqrt(pow(2.*fV0*fDv0,2)+pow(2.*norm*dnorm,2));
 } 
 fDresult=0;
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::Set3Vector(Nc3Vector& v)
{
/**
~~~
// Set the 3-vector part, the errors are taken from the input Nc3Vector
// Scalar mode    : The scalar part and its error are not modified,
//                  the Lorentz invariantand its error are re-calculated.
// Invariant mode : The Lorentz invariant and its error are not modified,
//                  the scalar part and its error are re-calculated.
// The error on scalar result operations is reset to 0.
~~~
**/

 fV=v;
 if (fScalar)
 {
  SetScalar(fV0,fDv0);
 }
 else
 {
  SetInvariant(fV2,fDv2);
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::Set3Vector(Double_t* v,TString f,TString u)
{
/**
~~~
// Set the 3-vector part according to reference frame f
// Only vector components will be stored, no errors.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// Scalar mode    : The scalar part and its error are not modified,
//                  the Lorentz invariantand its error are re-calculated.
// Invariant mode : The Lorentz invariant and its error are not modified,
//                  the scalar part and its error are re-calculated.
// The error on scalar result operations is reset to 0.
~~~
**/

 Double_t a[3];
 for (Int_t i=0; i<3; i++)
 {
  a[i]=v[i];
 }
 fV.SetVector(a,f,u);

 if (fScalar)
 {
  SetScalar(fV0,fDv0);
 }
 else
 {
  SetInvariant(fV2,fDv2);
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::Set3Vector(Float_t* v,TString f,TString u)
{
/**
~~~
// Set the 3-vector part according to reference frame f
// Only vector components will be stored, no errors.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// The Lorentz invariant is not modified
// The error on scalar result operations is reset to 0.
~~~
**/

 Double_t vec[3];
 for (Int_t i=0; i<3; i++)
 {
  vec[i]=v[i];
 }
 Set3Vector(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::Set3Vector(Double_t v1,Double_t v2,Double_t v3,TString f,TString u)
{
/**
~~~
// Set the 3-vector part according to reference frame f
// Only vector components will be stored, no errors.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// Scalar mode    : The scalar part and its error are not modified,
//                  the Lorentz invariantand its error are re-calculated.
// Invariant mode : The Lorentz invariant and its error are not modified,
//                  the scalar part and its error are re-calculated.
// The error on scalar result operations is reset to 0.
~~~
**/

 Double_t vec[3];
 vec[0]=v1;
 vec[1]=v2;
 vec[2]=v3;
 Set3Vector(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetInvariant(Double_t v2,Double_t dv2)
{
/**
~~~
// Modify the Lorentz invariant (v2) quantity v^i*v_i and its error (dv2).
// The default value for the error dv2 is 0.
// The vector part is not modified.
// Invariant mode is automatically selected
// ==> the scalar part and its error are updated.
// The error on scalar result operations is reset to 0.
~~~
**/

 fScalar=0;
 fV2=v2;
 fDv2=dv2;
 fV0=GetScalar();
 fDv0=GetResultError();
 fDresult=0;
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetInvariantError(Double_t dv2)
{
/**
~~~
// Set the error on the Lorentz invariant.
// If in invariant mode, update error on the scalar part accordingly.
// The error on scalar result operations is reset to 0.
~~~
**/

 fDv2=dv2;
 if (!fScalar)
 {
  fDv0=GetResultError();
 }
 fDresult=0; 
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetInvariant()
{
/**
~~~
// Provide the Lorentz invariant v^i*v_i.
// The error on the Lorentz invariant is available via GetResultError()
// after invokation of GetInvariant().
~~~
**/

 if (!fScalar)
 {
  fDresult=fDv2;
  return fV2;
 }
 else
 {
  Double_t inv=Dot(*this);
  return inv;
 }
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc4Vector::Get3Vector() const
{
/**
~~~
// Provide the 3-vector part.
~~~
**/

 return fV;
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetErrors(Double_t* e,TString f,TString u)
{
/**
~~~
// Store errors for vector v^i according to reference frame f
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// If in scalar mode, update error on the invariant accordingly.
// The error on scalar result operations is reset to 0.
~~~
**/

 Double_t a[3];
 for (Int_t i=0; i<3; i++)
 {
  a[i]=e[i+1];
 }
 SetScalarError(e[0]);
 fV.SetErrors(a,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetErrors(Double_t e0,Double_t e1,Double_t e2,Double_t e3,TString f,TString u)
{
/**
~~~
// Store errors for vector v^i according to reference frame f
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// If in scalar mode, update error on the invariant accordingly.
// The error on scalar result operations is reset to 0.
~~~
**/

 Double_t vec[4];
 vec[0]=e0;
 vec[1]=e1;
 vec[2]=e2;
 vec[3]=e3;
 SetErrors(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetErrors(Float_t* e,TString f,TString u)
{
/**
~~~
// Store errors for vector v^i according to reference frame f
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// If in scalar mode, update error on the invariant accordingly.
// The error on scalar result operations is reset to 0.
~~~
**/

 Double_t a[4];
 for (Int_t i=0; i<4; i++)
 {
  a[i]=e[i];
 }
 SetErrors(a,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::GetErrors(Double_t* e,TString f,TString u)
{
/**
~~~
// Provide errors for vector v^i according to reference frame f
// and according to the current mode.
// Scalar mode    : The error on the scalar part is directly returned via e[0].
// Invariant mode : The error on the scalar part is re-calculated via the error
//                  value on the Lorentz invariant and then returned via e[0].
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
~~~
**/

 Double_t a[3];
 fV.GetErrors(a,f,u);

 // Dummy invokation of GetScalar to obtain automatic proper error determination 
 e[0]=GetScalar();

 e[0]=GetResultError();

 for (Int_t i=0; i<3; i++)
 {
  e[i+1]=a[i];
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::GetErrors(Float_t* e,TString f,TString u)
{
/**
~~~
// Provide errors for vector v^i according to reference frame f
// and according to the current mode.
// Scalar mode    : The error on the scalar part is directly returned via e[0].
// Invariant mode : The error on the scalar part is re-calculated via the error
//                  value on the Lorentz invariant and then returned via e[0].
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
~~~
**/

 Double_t a[4];
 GetErrors(a,f,u);
 for (Int_t i=0; i<4; i++)
 {
  e[i]=a[i];
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::Data(TString f,TString u)
{
/**
~~~
// Print contravariant vector components and errors according to
// reference frame f and according to the current mode.
// Scalar mode    : The scalar part and its error are directly returned.
// Invariant mode : The scalar part and its error are re-calculated via the
//                  value (and error) of the Lorentz invariant.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The defaults are f="car" and u="rad".
~~~
**/

 if (f=="car" || f=="sph" || f=="cyl")
 {
  Double_t vec[4],err[4];
  GetVector(vec,f,u);
  GetErrors(err,f,u);
  Double_t inv=GetInvariant();
  Double_t dinv=GetResultError();
  if (fV.HasVector())
  {
   cout << " Contravariant vector in " << f.Data() << " (" << u.Data() << ") coordinates : "
        << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3] << endl;
  }
  else
  {
   cout << " 4-Vector not initialised." << endl;
  }
  if (fV.HasErrors())
  {
   cout << " ------------- Errors in " << f.Data() << " (" << u.Data() << ") coordinates : "
        << err[0] << " " << err[1] << " " << err[2] << " " << err[3] << endl;
  }
  if (fV.HasVector())
  {
   cout << " --- Lorentz invariant (v^2) : " << inv << " error : " << dinv << endl;
  }
 }
 else
 {
  cout << " *Nc4Vector::Data* Unsupported frame : " << f.Data() << endl
       << "  Possible frames are 'car', 'sph' and 'cyl'." << endl; 
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::Dot(Nc4Vector& q)
{
/**
~~~
// Provide the dot product of the current vector with vector q.
~~~
**/

 Double_t dotpro=0;
 Double_t a0=GetScalar();
 Double_t da0=GetResultError();
 if ((this) == &q) // Check for special case v.Dot(v)
 {
  Double_t norm=fV.GetNorm();
  Double_t dnorm=fV.GetResultError();
  dotpro=pow(a0,2)-pow(norm,2);
  fDresult=sqrt(pow(2.*a0*da0,2)+pow(2.*norm*dnorm,2));
 }
 else
 {
  Double_t b0=q.GetScalar();
  Double_t db0=q.GetResultError();
  Nc3Vector b=q.Get3Vector();

  Double_t dot=fV.Dot(b);
  Double_t ddot=fV.GetResultError();

  dotpro=a0*b0-dot;

  fDresult=sqrt(pow(b0*da0,2)+pow(a0*db0,2)+pow(ddot,2));
 }

 return dotpro;
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector Nc4Vector::operator+(Nc4Vector& q)
{
/**
~~~
// Add 4-vector q to the current 4-vector.
// Error propagation is performed automatically
~~~
**/

 Double_t a0=GetScalar();
 Double_t da0=GetResultError();
 Nc3Vector a=Get3Vector();
 Double_t b0=q.GetScalar();
 Double_t db0=q.GetResultError();
 Nc3Vector b=q.Get3Vector();

 Double_t c0=a0+b0;
 Nc3Vector c=a+b;
 Double_t dc0=sqrt(pow(da0,2)+pow(db0,2));

 Nc4Vector v;
 v.SetVector(c0,c);
 v.SetScalarError(dc0);  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector Nc4Vector::operator-(Nc4Vector& q)
{
/**
~~~
// Subtract 4-vector q from the current 4-vector.
// Error propagation is performed automatically
~~~
**/

 Double_t a0=GetScalar();
 Double_t da0=GetResultError();
 Nc3Vector a=Get3Vector();
 Double_t b0=q.GetScalar();
 Double_t db0=q.GetResultError();
 Nc3Vector b=q.Get3Vector();

 Double_t c0=a0-b0;
 Nc3Vector c=a-b;
 Double_t dc0=sqrt(pow(da0,2)+pow(db0,2));

 Nc4Vector v;
 v.SetVector(c0,c);
 v.SetScalarError(dc0);  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector Nc4Vector::operator*(Double_t s)
{
/**
~~~
// Multiply the current 4-vector with a scalar s.
// Error propagation is performed automatically
~~~
**/

 Double_t a0=GetScalar();
 Double_t da0=GetResultError();
 Nc3Vector a=Get3Vector();

 a0*=s;
 a*=s;
 da0*=s;

 Nc4Vector v;
 v.SetVector(a0,a);
 v.SetScalarError(da0);  
  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector Nc4Vector::operator/(Double_t s)
{
/**
~~~
// Divide the current vector by a scalar s.
// Error propagation is performed automatically
~~~
**/

 if (fabs(s)<1.e-20) // Protect against division by 0
 {
  cout << " *Nc4Vector::/* Division by 0 detected. No action taken." << endl;
  return *this;
 }
 else
 {
  Double_t a0=GetScalar();
  Double_t da0=GetResultError();
  Nc3Vector a=Get3Vector();

  a0/=s;
  a/=s;
  da0/=s;

  Nc4Vector v;
  v.SetVector(a0,a);
  v.SetScalarError(da0);  
  
  return v;
 }
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector& Nc4Vector::operator+=(Nc4Vector& q)
{
/**
~~~
// Add 4-vector q to the current 4-vector.
// Error propagation is performed automatically
~~~
**/

 Double_t a0=GetScalar();
 Double_t da0=GetResultError();
 Nc3Vector a=Get3Vector();
 Double_t b0=q.GetScalar();
 Double_t db0=q.GetResultError();
 Nc3Vector b=q.Get3Vector();

 Double_t c0=a0+b0;
 Nc3Vector c=a+b;
 Double_t dc0=sqrt(pow(da0,2)+pow(db0,2));

 SetVector(c0,c);
 SetScalarError(dc0);  
  
 return *this;
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector& Nc4Vector::operator-=(Nc4Vector& q)
{
/**
~~~
// Subtract 4-vector q from the current 4-vector.
// Error propagation is performed automatically
~~~
**/

 Double_t a0=GetScalar();
 Double_t da0=GetResultError();
 Nc3Vector a=Get3Vector();
 Double_t b0=q.GetScalar();
 Double_t db0=q.GetResultError();
 Nc3Vector b=q.Get3Vector();

 Double_t c0=a0-b0;
 Nc3Vector c=a-b;
 Double_t dc0=sqrt(pow(da0,2)+pow(db0,2));

 SetVector(c0,c);
 SetScalarError(dc0);  

 return *this;
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector& Nc4Vector::operator*=(Double_t s)
{
/**
~~~
// Multiply the current 4-vector with a scalar s.
// Error propagation is performed automatically
~~~
**/

 Double_t a0=GetScalar();
 Double_t da0=GetResultError();
 Nc3Vector a=Get3Vector();

 a0*=s;
 a*=s;
 da0*=s;

 SetVector(a0,a);
 SetScalarError(da0);  

 return *this;
}
///////////////////////////////////////////////////////////////////////////
Nc4Vector& Nc4Vector::operator/=(Double_t s)
{
/**
~~~
// Divide the current vector by a scalar s.
// Error propagation is performed automatically
~~~
**/

 if (fabs(s)<1.e-20) // Protect against division by 0
 {
  cout << " *Nc4Vector::/* Division by 0 detected. No action taken." << endl;
  return *this;
 }
 else
 {
  Double_t a0=GetScalar();
  Double_t da0=GetResultError();
  Nc3Vector a=Get3Vector();

  a0/=s;
  a/=s;
  da0/=s;

  SetVector(a0,a);
  SetScalarError(da0);  
  
  return *this;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t Nc4Vector::GetScalarFlag() const
{
/**
~~~
// Provide the value of the fScalar flag (for internal use only).
~~~
**/

 return fScalar;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc4Vector::GetVecTrans() const
{
/**
~~~
// Provide the transverse vector part w.r.t. z-axis.
// Error propagation is performed automatically
~~~
**/
  
 return fV.GetVecTrans();
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc4Vector::GetVecLong() const
{
/**
~~~
// Provide the longitudinal vector part w.r.t. z-axis.
// Error propagation is performed automatically
~~~
**/
  
 return fV.GetVecLong();
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetScaTrans()
{
/**
~~~
// Provide the "transverse value" of the scalar part w.r.t. z-axis.
// This provides a basis for e.g. E_trans calculation.
// Note : the returned value is always positive or zero.
// The error on the value is available via GetResultError()
// after invokation of GetScaTrans().
~~~
**/

 Double_t a[3],ea[3];

 fV.GetVector(a,"sph");
 fV.GetErrors(ea,"sph");

 Double_t s=GetScalar();
 Double_t ds=GetResultError();

 Double_t st,dst2;
 st=s*sin(a[1]);
 dst2=pow((sin(a[1])*ds),2)+pow((s*cos(a[1])*ea[1]),2);

 fDresult=sqrt(dst2);  
 return fabs(st);
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetScaLong()
{
/**
~~~
// Provide the "longitudinal value" of the scalar part w.r.t. z-axis.
// This provides a basis for e.g. E_long calculation.
// Note : the returned value can also be negative.
// The error on the value is available via GetResultError()
// after invokation of GetScaLong().
~~~
**/

 Double_t a[3],ea[3];

 fV.GetVector(a,"sph");
 fV.GetErrors(ea,"sph");

 Double_t s=GetScalar();
 Double_t ds=GetResultError();

 Double_t sl,dsl2;
 sl=s*cos(a[1]);
 dsl2=pow((cos(a[1])*ds),2)+pow((s*sin(a[1])*ea[1]),2);

 fDresult=sqrt(dsl2);  
 return sl;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetPseudoRapidity()
{
/**
~~~
// Provide the pseudorapidity value of the vector part w.r.t. z-axis.
// The error on the value is available via GetResultError()
// after invokation of GetPseudoRapidity().
~~~
**/

 Double_t eta=fV.GetPseudoRapidity();
 fDresult=fV.GetResultError();
 return eta;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc4Vector::GetBetaVector() const
{
/**
~~~
// Provide the beta 3-vector (i.e. v/c) corresponding to this 4-vector.
~~~
**/

 Nc3Vector beta;
 if (fabs(fV0)>0.) beta=fV/fV0;
 if (fabs(fDv0)>0.)
 {
  Double_t vecv[3],errv[3],errb[3];
  Double_t sqerr=0;
  fV.GetVector(vecv,"car");
  fV.GetErrors(errv,"car");
  for (Int_t i=0; i<3; i++)
  {
   sqerr=pow((errv[i]/fV0),2)+pow((vecv[i]*fDv0/(fV0*fV0)),2);
   errb[i]=sqrt(sqerr);
  }
  beta.SetErrors(errb,"car");
 }
 return beta;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetBeta()
{
/**
~~~
// Provide the beta value (i.e. v/c) corresponding to this 4-vector.
~~~
**/

 Nc3Vector beta=GetBetaVector();
 Double_t val=beta.GetNorm();
 fDresult=beta.GetResultError();
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetGamma()
{
/**
~~~
// Provide the Lorentz gamma factor corresponding to this 4-vector.
// In case the gamma factor is infinite a value of -1 is returned.
~~~
**/

 Double_t gamma=-1;
 fDresult=0;
 Double_t inv=sqrt(fabs(fV2));
 if (inv>0.)
 {
  Double_t dinv=fDv2/(2.*inv);
  gamma=fV0/inv;
  Double_t sqerr=pow((fDv0/inv),2)+pow((fV0*dinv/fV2),2);
  fDresult=sqrt(sqerr);
 }
 return gamma;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetX(Int_t i,TString f,TString u)
{
/**
~~~
// Provide i-th vector component according to reference frame f.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// The vector components are addressed via the generic x0,x1,x2,x3 notation.
// So, i=0 denotes the scalar component and i=1 denotes the first 3-vector component.
// The error on the selected component can be obtained via the
// usual GetResultError() facility.
~~~
**/
 
 fDresult=0;

 if (i<0 || i>3) return 0;

 Double_t x=0;

 if (i==0)
 {
  x=GetScalar();
 }
 else
 {
  x=fV.GetX(i,f,u);
  fDresult=fV.GetResultError();
 }

 return x;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetOpeningAngle(Nc4Vector& q,TString u)
{
/**
~~~
// Provide the opening angle between 3-vector parts with 4-vector q.
// The string argument "u" allows to choose between different output units.
// u = "rad" : opening angle provided in radians
//     "deg" : opening angle provided in degrees
//
// The default is u="rad".
~~~
**/

 Nc3Vector v1=fV;
 Nc3Vector v2=q.Get3Vector();

 Double_t ang=v1.GetOpeningAngle(v2,u);
 fDresult=v1.GetResultError();

 return ang;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc4Vector::GetOpeningAngle(Nc3Vector& q,TString u)
{
/**
~~~
// Provide the opening angle between the 3-vector part and 3-vector q.
// The string argument "u" allows to choose between different output units.
// u = "rad" : opening angle provided in radians
//     "deg" : opening angle provided in degrees
//
// The default is u="rad".
~~~
**/

 Nc3Vector v1=fV;

 Double_t ang=v1.GetOpeningAngle(q,u);
 fDresult=v1.GetResultError();

 return ang;
}
///////////////////////////////////////////////////////////////////////////
void Nc4Vector::SetUserData(NcSignal* s)
{
/**
~~~
// Enter the NcSignal object containing the user data.
//
// The main goal of these user data is to provide a flexible facility
// to provide user defined observables to tracks and events, which
// can in turn be retreived into the produced Tree structure of NcTreeMaker.
// It is also possible to use the user data to perform a specific sorting
// or selection of tracks or events.
// The name of the user defined variable is taken from the NcSignal slotname
// and the value of the variable is taken from the corresponding signal value.
// As such it is advised to use only short, single named variable names.
// Furthermore, the variable name should not contain mathematical symbols
// like '+', '-' etc... otherwise an analysis via the ROOT TreeViewer
// gets confused.
//
// In case an object to hold user data was already present, this
// will be deleted first before the new one is stored.
// This means that SetUserData(0) can be used to just remove the
// existing object with the user data.
//
// Note : The entered NcSignal object is owned by this Nc4Vector instance.
//        As such, a private copy of it will be stored using the Clone()
//        memberfunction.
~~~
**/

 if (fUser)
 {
  delete fUser;
  fUser=0;
 }

 if (s) fUser=(NcSignal*)s->Clone();
}
///////////////////////////////////////////////////////////////////////////
NcSignal* Nc4Vector::GetUserData() const
{
/**
~~~
// Provide the pointer to the NcSignal object containing the user data.
~~~
**/

 return fUser;
}
///////////////////////////////////////////////////////////////////////////
