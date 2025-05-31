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
/** @class Nc3Vector
~~~
// Class Nc3Vector
// Handling of 3-vectors in various reference frames.
//
// This class is meant to serve as a base class for NCFS (derived) objects
// that have 3-dimensional vector characteristics.
// Error propagation is performed automatically. 
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
// Nc3Vector a;
// Float_t v[3]={-1,25,7};
// Float_t e[3]={0.03,0.5,0.21};
// a.SetVector(v,"car");
// a.SetErrors(e,"car");
// a.Data();
//
// Float_t vec[3];
// Float_t err[3];
// a.GetVector(vec,"sph","deg");
// a.GetErrors(vec,"sph","deg");
//
// Nc3Vector b;
// Float_t v2[3]={6,-18,33};
// Float_t e2[3]={0.19,0.45,0.93};
// b.SetVector(v2,"car");
// b.SetErrors(e2,"car");
//
// Float_t dotpro=a.Dot(b);
// Float_t doterror=a.GetResultError();
//
// Nc3Vector c=a.Cross(b);
// c.Data("sph");
// c.GetVector(vec,"cyl");
// c.GetErrors(err,"cyl");
//
// Float_t norm=c.GetNorm();
// Float_t normerror=c.GetResultError();
//
// c=a+b;
// c=a-b;
// c=a*5;
//
//--- Author: Nick van Eijndhoven 30-mar-1999 Utrecht University
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 16, 2023  02:29Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "Nc3Vector.h"
#include "Riostream.h"
 
ClassImp(Nc3Vector); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
Nc3Vector::Nc3Vector()
{
/**
~~~
// Creation of an Nc3Vector object and initialisation of parameters
// All attributes initialised to 0
~~~
**/

 fNv=0;
 fV=0;
 fDresult=0;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector::~Nc3Vector()
{
/**
~~~
// Destructor to delete dynamically allocated memory
~~~
**/

 if (fV)
 {
  delete[] fV;
  fV=0;
 }
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector::Nc3Vector(const Nc3Vector& v)
{
/**
~~~
// Copy constructor
~~~
**/

 fNv=v.fNv;
 fV=0;
 if (fNv) fV=new Double32_t[fNv];
 for (Int_t i=0; i<fNv; i++)
 {
  fV[i]=v.fV[i];
 }
 fDresult=v.fDresult;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector& Nc3Vector::operator=(const Nc3Vector& q)
{
/**
~~~
// Set the current vector equal to vector q.
~~~
**/

 fNv=0;
 if (fV)
 {
  delete[] fV;
  fV=0;
 }

 if (q.HasVector())
 {
  Double_t a[3];
  q.GetVector(a,"sph");
  SetVector(a,"sph");
 }

 if (q.HasErrors())
 {
  Double_t ea[3];
  q.GetErrors(ea,"car");
  SetErrors(ea,"car");
 }

 fDresult=q.fDresult;
  
 return *this;
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::Load(Nc3Vector& q)
{
/**
~~~
// Load all attributes of the input Nc3Vector into this Nc3Vector object.
~~~
**/

 fNv=q.fNv;
 if (fV)
 {
  delete[] fV;
  fV=0;
 }
 if (fNv) fV=new Double32_t[fNv];
 for (Int_t i=0; i<fNv; i++)
 {
  fV[i]=q.fV[i];
 }
 fDresult=q.fDresult;
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::SetZero()
{
/**
~~~
// (Re)set all attributes to zero.
~~~
**/

 fNv=0;
 if (fV)
 {
  delete[] fV;
  fV=0;
 }
 fDresult=0;
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::SetVector(Double_t* v,TString f,TString u)
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
~~~
**/

 fDresult=0;

 // Initialisation of the new storage array
 fNv=3;
 if (fV) delete[] fV;
 fV=new Double32_t[fNv];

 Double_t pi=acos(-1.);
 Double_t twopi=2.*pi;

 Double_t fu=1.;
 if (u == "deg") fu=pi/180.;

 Int_t frame=0;
 if (f == "car") frame=1;
 if (f == "sph") frame=2;
 if (f == "cyl") frame=3;

 Double_t x,y,z,r,theta,phi,rho;

 switch (frame)
 {
  case 1: // Cartesian coordinates
   x=v[0];
   y=v[1];
   z=v[2];
   r=sqrt(x*x+y*y+z*z);

   // Determine theta in [0,pi]
   theta=0;
   if (z<0) theta=pi;
   if (r)
   {
    if (fabs(z/r)<=1) theta=acos(z/r); // Puts theta in [0,pi]
   }
   if (theta<0) theta=fabs(theta); // To catch possible accuracy issue

   // Determine phi in [0,twopi]
   phi=0;
   if (x || y) phi=atan2(y,x); // Puts phi in [-pi,pi]
   if (phi<0) phi+=twopi;

   fV[0]=r;
   fV[1]=theta;
   fV[2]=phi;
   break;

  case 2: // Spherical coordinates
   r=v[0];
   theta=v[1]*fu;
   phi=v[2]*fu;

   // Limit phi to the interval [0,twopi]
   while (phi<0)
   {
    phi+=twopi;
   }
   while (phi>twopi)
   {
    phi-=twopi;
   }

   // Limit theta to the interval [-pi,pi]
   while (theta<-pi)
   {
    theta+=twopi;
   }
   while (theta>pi)
   {
    theta-=twopi;
   }
   // Limit theta to the interval [0,pi]
   if (theta<0)
   {
    theta=fabs(theta);
    phi+=pi;
    if (phi>twopi) phi-=twopi;
   }

   fV[0]=r;
   fV[1]=theta;
   fV[2]=phi;
   break;

  case 3: // Cylindrical coordinates
   rho=v[0];
   phi=v[1]*fu;
   z=v[2];
   r=sqrt(rho*rho+z*z);

   // Limit phi to the interval [0,twopi]
   while (phi<0)
   {
    phi+=twopi;
   }
   while (phi>twopi)
   {
    phi-=twopi;
   }

   // Determine theta in [0,pi]
   theta=0;
   if (z<0) theta=pi;
   if (rho && r)
   {
    if (fabs(z/r)<=1) theta=acos(z/r); // Puts theta in [0,pi]
   }
   if (theta<0) theta=fabs(theta); // To catch possible accuracy issue

   fV[0]=r;
   fV[1]=theta;
   fV[2]=phi;
   break;

  default: // Unsupported reference frame
   cout << "*Nc3Vector::SetVector* Unsupported frame : " << f << endl
        << " Possible frames are 'car', 'sph' and 'cyl'." << endl; 

   fNv=0;
   if (fV) delete[] fV;
   fV=0;
   break;
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::SetVector(Double_t v1,Double_t v2,Double_t v3,TString f,TString u)
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
~~~
**/

 Double_t vec[3];
 vec[0]=v1;
 vec[1]=v2;
 vec[2]=v3;
 SetVector(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::GetVector(Double_t* v,TString f,TString u) const
{
/**
~~~
// Provide vector according to reference frame f
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
~~~
**/

 if (!fNv)
 {
  v[0]=0;
  v[1]=0;
  v[2]=0;
  return;
 }

 Double_t pi=acos(-1.);

 Double_t fu=1.;
 if (u == "deg") fu=180./pi;

 Double_t r=fV[0];
 Double_t theta=fV[1];
 Double_t phi=fV[2];

 Int_t frame=0;
 if (f == "car") frame=1;
 if (f == "sph") frame=2;
 if (f == "cyl") frame=3;

 switch (frame)
 {
  case 1: // Cartesian coordinates
   v[0]=r*sin(theta)*cos(phi);
   v[1]=r*sin(theta)*sin(phi);
   v[2]=r*cos(theta);
   break;

  case 2: // Spherical coordinates
   v[0]=r;
   v[1]=theta*fu;
   v[2]=phi*fu;
   break;

  case 3: // Cylindrical coordinates
   v[0]=r*sin(theta);
   v[1]=phi*fu;
   v[2]=r*cos(theta);
   break;

  default: // Unsupported reference frame
   cout << "*Nc3Vector::GetVector* Unsupported frame : " << f.Data() << endl
        << " Possible frames are 'car', 'sph' and 'cyl'." << endl; 
   for (Int_t i=0; i<3; i++)
   {
    v[i]=0;
   }
   break;
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::SetVector(Float_t* v,TString f,TString u)
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
~~~
**/

 Double_t vec[3];
 for (Int_t i=0; i<3; i++)
 {
  vec[i]=v[i];
 }
 SetVector(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::GetVector(Float_t* v,TString f,TString u) const
{
/**
~~~
// Provide vector according to reference frame f
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
~~~
**/

 Double_t vec[3];
 GetVector(vec,f,u);
 for (Int_t i=0; i<3; i++)
 {
  v[i]=vec[i];
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::SetErrors(Double_t* e,TString f,TString u)
{
/**
~~~
// Store errors according to reference frame f.
//
// Note : The errors can only be set after the vector components have
//        been specified via the SetVector memberfunction.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// The error on scalar results is reset to 0.
~~~
**/

 if (!fNv) return;

 Double_t pi=acos(-1.);

 Double_t fu=1.;
 if (u == "deg") fu=pi/180.;

 fDresult=0;

 Int_t frame=0;
 if (f == "car") frame=1;
 if (f == "sph") frame=2;
 if (f == "cyl") frame=3;

 Double_t r=fV[0];
 Double_t theta=fV[1];
 Double_t phi=fV[2];

 // Initialisation of the new storage array
 if (fV) delete[] fV;
 fNv=6;
 if (!frame) fNv=3;
 fV=new Double32_t[fNv];
 fV[0]=r;
 fV[1]=theta;
 fV[2]=phi;

 Double_t rho;
 Double_t dx2,dy2,dz2;

 switch (frame)
 {
  case 1: // Cartesian coordinates
   fV[3]=fabs(e[0]);
   fV[4]=fabs(e[1]);
   fV[5]=fabs(e[2]);
   break;

  case 2: // Spherical coordinates
   dx2=pow((cos(phi)*sin(theta)*e[0]),2)+pow((r*cos(theta)*cos(phi)*e[1]*fu),2)
       +pow((r*sin(theta)*sin(phi)*e[2]*fu),2);
   dy2=pow((sin(phi)*sin(theta)*e[0]),2)+pow((r*cos(theta)*sin(phi)*e[1]*fu),2)
       +pow((r*sin(theta)*cos(phi)*e[2]*fu),2);
   dz2=pow((cos(theta)*e[0]),2)+pow((r*sin(theta)*e[1]*fu),2);
   fV[3]=sqrt(dx2);
   fV[4]=sqrt(dy2);
   fV[5]=sqrt(dz2);
   break;

  case 3: // Cylindrical coordinates
   rho=r*sin(theta);
   dx2=pow((cos(phi)*e[0]),2)+pow((rho*sin(phi)*e[1]*fu),2);
   dy2=pow((sin(phi)*e[0]),2)+pow((rho*cos(phi)*e[1]*fu),2);
   fV[3]=sqrt(dx2);
   fV[4]=sqrt(dy2);
   fV[5]=fabs(e[2]);
   break;

  default: // Unsupported reference frame
   cout << "*Nc3Vector::SetErrors* Unsupported frame : " << f.Data() << endl
        << " Possible frames are 'car', 'sph' and 'cyl'." << endl; 
   break;
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::SetErrors(Double_t e1,Double_t e2,Double_t e3,TString f,TString u)
{
/**
~~~
// Store errors according to reference frame f.
//
// Note : The errors can only be set after the vector components have
//        been specified via the SetVector memberfunction.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// The error on scalar results is reset to 0
~~~
**/

 Double_t vec[3];
 vec[0]=e1;
 vec[1]=e2;
 vec[2]=e3;
 SetErrors(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::GetErrors(Double_t* e,TString f,TString u) const
{
/**
~~~
// Provide errors according to reference frame f.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
~~~
**/

 if (fNv<4)
 {
  e[0]=0;
  e[1]=0;
  e[2]=0;
  return;
 }

 Double_t pi=acos(-1.);

 Double_t fu=1.;
 if (u == "deg") fu=180./pi;

 Int_t frame=0;
 if (f == "car") frame=1;
 if (f == "sph") frame=2;
 if (f == "cyl") frame=3;

 Double_t r=fV[0];
 Double_t theta=fV[1];
 Double_t dx=fV[3];
 Double_t dy=fV[4];
 Double_t dz=fV[5];

 Double_t dr2,dtheta2,dphi2,rho,drho2;
 Double_t v[3]; 
 Double_t rxy2; // Shorthand for (x*x+y*y)

 switch (frame)
 {
  case 1: // Cartesian coordinates
   e[0]=dx;
   e[1]=dy;
   e[2]=dz;
   break;

  case 2: // Spherical coordinates
   GetVector(v,"car");
   rxy2=pow(v[0],2)+pow(v[1],2);
   if (sqrt(rxy2)<(r*1e-10)) rxy2=0;
   if (r) 
   {
    dr2=(pow((v[0]*dx),2)+pow((v[1]*dy),2)+pow((v[2]*dz),2))/(r*r);
   }
   else
   {
    dr2=0;
   }
   if (r)
   {
    dtheta2=rxy2*pow(dz,2)/pow(r,4);
    if (v[2] && rxy2)
    {
     dtheta2+=rxy2*pow(v[2],2)*(pow((v[0]*dx),2)+pow((v[1]*dy),2)) /
              pow(((pow(v[2],2)*rxy2)+pow(rxy2,2)),2);
    }
   }
   else
   {
    dtheta2=0;
   }
   if (rxy2)
   {
    dphi2=(pow((v[1]*dx),2)+pow((v[0]*dy),2))/(pow(rxy2,2));
   }
   else
   {
    dphi2=0;
   }
   e[0]=sqrt(dr2);
   e[1]=sqrt(dtheta2);
   if (e[1]>pi) e[1]=pi;
   e[2]=sqrt(dphi2);
   if (e[2]>(2.*pi)) e[2]=2.*pi;
   e[1]*=fu;
   e[2]*=fu;
   break;

  case 3: // Cylindrical coordinates
   GetVector(v,"car");
   rho=fabs(r*sin(theta));
   if (rho<(r*1e-10)) rho=0;
   if (rho) 
   {
    drho2=(pow((v[0]*dx),2)+pow((v[1]*dy),2))/(rho*rho);
   }
   else
   {
    drho2=0;
   }
   if (rho)
   {
    dphi2=(pow((v[1]*dx),2)+pow((v[0]*dy),2))/(pow(rho,4));
   }
   else
   {
    dphi2=0;
   }
   e[0]=sqrt(drho2);
   e[1]=sqrt(dphi2);
   if (e[1]>(2.*pi)) e[1]=2.*pi;
   e[2]=dz;
   e[1]*=fu;
   break;

  default: // Unsupported reference frame
   cout << "*Nc3Vector::GetErrors* Unsupported frame : " << f.Data() << endl
        << " Possible frames are 'car', 'sph' and 'cyl'." << endl; 
   for (Int_t i=0; i<3; i++)
   {
    e[i]=0;
   }
   break;
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::SetErrors(Float_t* e,TString f,TString u)
{
/**
~~~
// Store errors according to reference frame f.
//
// Note : The errors can only be set after the vector components have
//        been specified via the SetVector memberfunction.
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
//
// The error on scalar results is reset to 0
~~~
**/

 Double_t vec[3];
 for (Int_t i=0; i<3; i++)
 {
  vec[i]=e[i];
 }
 SetErrors(vec,f,u);
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::GetErrors(Float_t* e,TString f,TString u) const
{
/**
~~~
// Provide errors according to reference frame f
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
// u = "rad" : angles provided in radians
//     "deg" : angles provided in degrees
//
// The default is u="rad".
~~~
**/

 Double_t vec[3];
 GetErrors(vec,f,u);
 for (Int_t i=0; i<3; i++)
 {
  e[i]=vec[i];
 }
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::Data(TString f,TString u) const
{
/**
~~~
// Print vector components according to reference frame f
//
// f = "car" : Cartesian coordinates
//     "sph" : Spherical coordinates
//     "cyl" : Cylindrical coordinates
//
// The string argument "u" allows to choose between different angular units
// in case e.g. a spherical frame is selected.
//
// u = "rad" : angle provided in radians
//     "deg" : angle provided in degrees
//     "dms" : angle provided in dddmmss.sss
//     "hms" : angle provided in hhmmss.sss
//
// The defaults are f="car" and  u="rad".
~~~
**/

 if ((f=="car" || f=="sph" || f=="cyl") && (u=="rad" || u=="deg" || u=="dms"|| u=="hms"))
 {
  if (!fNv)
  {
   cout << " 3-Vector not initialised." << endl;
  }
  else
  {
   Double_t vec[3];
   GetVector(vec,f,"deg");
   if (f=="car")
   {
    printf(" Vector in %-s coordinates : %.3e %.3e %.3e \n",f.Data(),vec[0],vec[1],vec[2]);
   }
   if (f=="sph")
   {
    printf(" Vector in %-s coordinates : %.3e ",f.Data(),vec[0]);
    PrintAngle(vec[1],"deg",u,3,kTRUE); printf(" ");
    PrintAngle(vec[2],"deg",u,3,kTRUE); printf("\n");
   }
   if (f=="cyl")
   {
    printf(" Vector in %-s coordinates : %.3e ",f.Data(),vec[0]);
    PrintAngle(vec[1],"deg",u,3,kTRUE); printf(" ");
    printf(" %.3e \n",vec[2]);
   }
  }
  if (fNv==6)
  {
   Double_t err[3];
   GetErrors(err,f,"deg");
   if (f=="car")
   {
    printf("   Err. in %-s coordinates : %.3e %.3e %.3e \n",f.Data(),err[0],err[1],err[2]);
   }
   if (f=="sph")
   {
    printf("   Err. in %-s coordinates : %.3e ",f.Data(),err[0]);
    PrintAngle(err[1],"deg",u,3,kTRUE); printf(" ");
    PrintAngle(err[2],"deg",u,3,kTRUE); printf("\n");
   }
   if (f=="cyl")
   {
    printf("   Err. in %-s coordinates : %.3e ",f.Data(),err[0]);
    PrintAngle(err[1],"deg",u,3,kTRUE); printf(" ");
    printf(" %.3e \n",err[2]);
   }
  }
 }
 else
 {
  printf(" *Nc3Vector::Data* Unsupported input frame=%-s format=%-s \n",f.Data(),u.Data());
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t Nc3Vector::HasVector() const
{
/**
~~~
// Check whether the vector components have been set.
// In case the vector components have been set, the value 1 is returned.
// Otherwise, the value 0 is returned.
~~~
**/

 Int_t val=0;
 if (fNv) val=1;
 return val;
}
///////////////////////////////////////////////////////////////////////////
Int_t Nc3Vector::HasErrors() const
{
/**
~~~
// Check whether the errors on the vector components have been set.
// In case the errors have been set, the value 1 is returned.
// Otherwise, the value 0 is returned.
~~~
**/

 Int_t val=0;
 if (fNv==6) val=1;
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc3Vector::GetNorm()
{
/**
~~~
// Provide the norm of the current vector.
// The error on the scalar result (norm) is updated accordingly
~~~
**/

 Double_t norm=0;
 if (fV) norm=fV[0];

 fDresult=0;
 if (fNv==6)
 {
  Double_t e[3];
  GetErrors(e,"sph");
  fDresult=e[0];
 } 

 return norm;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc3Vector::GetPseudoRapidity()
{
/**
~~~
// Provide the pseudo-rapidity w.r.t. the z-axis.
// In other words : eta=-log(tan(theta/2))
// The error on the scalar result (pseudo-rap.) is updated accordingly
~~~
**/

 Double_t pi=acos(-1.);
 Double_t v[3];
 GetVector(v,"sph");
 Double_t thetahalf=v[1]/2.;
 Double_t arg=0;
 if (v[1]<pi) arg=tan(thetahalf);
 Double_t eta=9999;
 if (arg>0) eta=-log(arg);

 fDresult=0;
 if (fNv==6)
 {
  Double_t e[3];
  GetErrors(e,"sph");
  Double_t prod=cos(thetahalf)*sin(thetahalf);
  fDresult=0;
  if (prod) fDresult=fabs(e[1]/2.*prod);
 }

 return eta;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc3Vector::Dot(Nc3Vector& q)
{
/**
~~~
// Provide the dot product of the current vector with vector q.
// The error on the scalar result (dotproduct) is updated accordingly
~~~
**/

 Double_t dotpro=0;

 if ((this) == &q) // Check for special case v.Dot(v)
 {
  Double_t norm=GetNorm();
  Double_t dnorm=GetResultError();
  dotpro=pow(norm,2);
  fDresult=2.*norm*dnorm;
 }
 else
 {
  Double_t a[3],b[3];
  Double_t ea[3],eb[3];
  Double_t d2=0;

  GetVector(a,"car");
  GetErrors(ea,"car");
  q.GetVector(b,"car");
  q.GetErrors(eb,"car");
  for (Int_t i=0; i<3; i++)
  {
   dotpro+=a[i]*b[i];
   d2+=pow(b[i]*ea[i],2)+pow(a[i]*eb[i],2);
  }
  fDresult=sqrt(d2);
 }

 return dotpro;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc3Vector::GetResultError() const
{
/**
~~~
// Provide the error on the result of an operation yielding a scalar.
// E.g. GetNorm() or Dot()
~~~
**/

 return fDresult;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::Cross(Nc3Vector& q) const
{
/**
~~~
// Provide the cross product of the current vector with vector q.
// Error propagation is performed automatically
~~~
**/

 Double_t a[3],b[3],c[3];

 GetVector(a,"car");
 q.GetVector(b,"car");

 c[0]=a[1]*b[2]-a[2]*b[1];
 c[1]=a[2]*b[0]-a[0]*b[2];
 c[2]=a[0]*b[1]-a[1]*b[0];

 Nc3Vector v;
 if (fNv && q.fNv) v.SetVector(c,"car");

 if (fNv==6 || q.fNv==6)
 {
  Double_t ea[3],eb[3],ec[3],d2;
  GetErrors(ea,"car");
  q.GetErrors(eb,"car");

  d2=pow(b[2]*ea[1],2)+pow(a[1]*eb[2],2)
    +pow(b[1]*ea[2],2)+pow(a[2]*eb[1],2);
  ec[0]=sqrt(d2);

  d2=pow(b[0]*ea[2],2)+pow(a[2]*eb[0],2)
    +pow(b[2]*ea[0],2)+pow(a[0]*eb[2],2);
  ec[1]=sqrt(d2);

  d2=pow(b[1]*ea[0],2)+pow(a[0]*eb[1],2)
    +pow(b[0]*ea[1],2)+pow(a[1]*eb[0],2);
  ec[2]=sqrt(d2);

  v.SetErrors(ec,"car");
 }
  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::operator+(Nc3Vector& q) const
{
/**
~~~
// Add vector q to the current vector.
// Error propagation is performed automatically
~~~
**/

 Double_t a[3],b[3];

 GetVector(a,"car");
 q.GetVector(b,"car");

 for (Int_t i=0; i<3; i++)
 {
  a[i]+=b[i];
 }

 Nc3Vector v;
 if (fNv || q.fNv) v.SetVector(a,"car");

 if (fNv==6 || q.fNv==6)
 {
  Double_t ea[3],eb[3];
  GetErrors(ea,"car");
  q.GetErrors(eb,"car");

  for (Int_t j=0; j<3; j++)
  {
   ea[j]=sqrt(pow(ea[j],2)+pow(eb[j],2));
  }

  v.SetErrors(ea,"car");
 }
  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::operator-(Nc3Vector& q) const
{
/**
~~~
// Subtract vector q from the current vector.
// Error propagation is performed automatically
~~~
**/

 Double_t a[3],b[3];

 GetVector(a,"car");
 q.GetVector(b,"car");

 for (Int_t i=0; i<3; i++)
 {
  a[i]-=b[i];
 }

 Nc3Vector v;
 if (fNv || q.fNv) v.SetVector(a,"car");

 if (fNv==6 || q.fNv==6)
 {
  Double_t ea[3],eb[3];
  GetErrors(ea,"car");
  q.GetErrors(eb,"car");

  for (Int_t j=0; j<3; j++)
  {
   ea[j]=sqrt(pow(ea[j],2)+pow(eb[j],2));
  }

  v.SetErrors(ea,"car");
 }
  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::operator*(Double_t s) const
{
/**
~~~
// Multiply the current vector with a scalar s.
// Error propagation is performed automatically.
~~~
**/

 Double_t a[3];
 GetVector(a,"car");

 for (Int_t i=0; i<3; i++)
 {
  a[i]*=s;
 }

 Nc3Vector v;
 if (fNv) v.SetVector(a,"car");

 if (fNv==6)
 {
  Double_t ea[3];
  GetErrors(ea,"car");

  for (Int_t j=0; j<3; j++)
  {
   ea[j]*=s;
  }

  v.SetErrors(ea,"car");
 }
  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::operator/(Double_t s) const
{
/**
~~~
// Divide the current vector by a scalar s.
// Error propagation is performed automatically
~~~
**/

 if (fabs(s)<1.e-20) // Protect against division by 0
 {
  cout << " *Nc3Vector::/* Division by 0 detected. No action taken." << endl;
  return *this;
 }
 else
 {
  Double_t a[3];
  GetVector(a,"car");

  for (Int_t i=0; i<3; i++)
  {
   a[i]/=s;
  }

  Nc3Vector v;
  if (fNv) v.SetVector(a,"car");

  if (fNv==6)
  {
   Double_t ea[3];
   GetErrors(ea,"car");

   for (Int_t j=0; j<3; j++)
   {
    ea[j]/=s;
   }

   v.SetErrors(ea,"car");
  }

  return v;
 }
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector& Nc3Vector::operator+=(Nc3Vector& q)
{
/**
~~~
// Add vector q to the current vector.
// Error propagation is performed automatically
~~~
**/

 Double_t a[3],b[3],ea[3],eb[3];

 Int_t nv=fNv;
 GetVector(a,"car");
 GetErrors(ea,"car");
 q.GetVector(b,"car");
 q.GetErrors(eb,"car");

 for (Int_t i=0; i<3; i++)
 {
  a[i]+=b[i];
 }

 if (nv || q.fNv) SetVector(a,"car");

 if (nv==6 || q.fNv==6)
 {
  for (Int_t j=0; j<3; j++)
  {
   ea[j]=sqrt(pow(ea[j],2)+pow(eb[j],2));
  }
  SetErrors(ea,"car");
 }
  
 return *this;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector& Nc3Vector::operator-=(Nc3Vector& q)
{
/**
~~~
// Subtract vector q from the current vector.
// Error propagation is performed automatically
~~~
**/

 Double_t a[3],b[3],ea[3],eb[3];

 Int_t nv=fNv;
 GetVector(a,"car");
 GetErrors(ea,"car");
 q.GetVector(b,"car");
 q.GetErrors(eb,"car");

 for (Int_t i=0; i<3; i++)
 {
  a[i]-=b[i];
 }

 if (nv || q.fNv) SetVector(a,"car");

 if (nv==6 || q.fNv==6)
 {
  for (Int_t j=0; j<3; j++)
  {
   ea[j]=sqrt(pow(ea[j],2)+pow(eb[j],2));
  }
  SetErrors(ea,"car");
 }
  
 return *this;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector& Nc3Vector::operator*=(Double_t s)
{
/**
~~~
// Multiply the current vector with a scalar s.
// Error propagation is performed automatically
~~~
**/

 Double_t a[3],ea[3];

 Int_t nv=fNv;
 GetVector(a,"car");
 GetErrors(ea,"car");

 for (Int_t i=0; i<3; i++)
 {
  a[i]*=s;
 }

 if (nv) SetVector(a,"car");

 if (nv==6)
 {
  for (Int_t j=0; j<3; j++)
  {
   ea[j]*=s;
  }
  SetErrors(ea,"car");
 }
  
 return *this;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector& Nc3Vector::operator/=(Double_t s)
{
/**
~~~
// Divide the current vector by a scalar s.
// Error propagation is performed automatically
~~~
**/

 if (fabs(s)<1.e-20) // Protect against division by 0
 {
  cout << " *Nc3Vector::/=* Division by 0 detected. No action taken." << endl;
  return *this;
 }
 else
 {
  Double_t a[3],ea[3];

  Int_t nv=fNv;
  GetVector(a,"car");
  GetErrors(ea,"car");

  for (Int_t i=0; i<3; i++)
  {
   a[i]/=s;
  }

  if (nv) SetVector(a,"car");

  if (nv==6)
  {
   for (Int_t j=0; j<3; j++)
   {
    ea[j]/=s;
   }
   SetErrors(ea,"car");
  }
  
  return *this;
 }
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::GetVecTrans() const
{
/**
~~~
// Provide the transverse vector w.r.t. z-axis.
// Error propagation is performed automatically
~~~
**/

 Double_t pi=acos(-1.);
 Double_t a[3],ea[3];

 GetVector(a,"sph");
 GetErrors(ea,"sph");

 Double_t vt=a[0]*sin(a[1]);
 Double_t dvt2=pow((sin(a[1])*ea[0]),2)+pow((a[0]*cos(a[1])*ea[1]),2);

 a[0]=fabs(vt);
 a[1]=pi/2.;

 Nc3Vector v;
 if (fNv) v.SetVector(a,"sph");

 if (fNv==6)
 {
  ea[0]=sqrt(dvt2);
  ea[1]=0;
  v.SetErrors(ea,"sph");
 }
  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::GetVecLong() const
{
/**
~~~
// Provide the longitudinal vector w.r.t. z-axis.
// Error propagation is performed automatically
~~~
**/

 Double_t pi=acos(-1.);
 Double_t a[3],ea[3];

 GetVector(a,"sph");
 GetErrors(ea,"sph");

 Double_t vl=a[0]*cos(a[1]);
 Double_t dvl2=pow((cos(a[1])*ea[0]),2)+pow((a[0]*sin(a[1])*ea[1]),2);

 a[0]=fabs(vl);
 a[1]=0;
 if (vl<0) a[1]=pi;
 a[2]=0;

 Nc3Vector v;
 if (fNv) v.SetVector(a,"sph");

 if (fNv==6)
 {
  ea[0]=sqrt(dvl2);
  ea[1]=0;
  ea[2]=0;
  v.SetErrors(ea,"sph");
 }
  
 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::GetPrimed(TRotMatrix* m) const
{
/**
~~~
// Provide vector components (and errors) in a rotated frame.
// The orientation of the rotated frame is described by the TRotMatrix
// input argument.
~~~
**/

 Nc3Vector v(*this);
 if (!m) return v;

 Double_t* mat=m->GetMatrix();

 Double_t a[3],aprim[3];

 if (fNv)
 {
  GetVector(a,"car");
  aprim[0]=a[0]*mat[0]+a[1]*mat[1]+a[2]*mat[2];
  aprim[1]=a[0]*mat[3]+a[1]*mat[4]+a[2]*mat[5];
  aprim[2]=a[0]*mat[6]+a[1]*mat[7]+a[2]*mat[8];
  v.SetVector(aprim,"car");
 }

 if (fNv==6)
 {
  GetErrors(a,"car");
  aprim[0]=sqrt(pow(a[0]*mat[0],2)+pow(a[1]*mat[1],2)+pow(a[2]*mat[2],2));
  aprim[1]=sqrt(pow(a[0]*mat[3],2)+pow(a[1]*mat[4],2)+pow(a[2]*mat[5],2));
  aprim[2]=sqrt(pow(a[0]*mat[6],2)+pow(a[1]*mat[7],2)+pow(a[2]*mat[8],2));
  v.SetErrors(aprim,"car");
 }

 return v;
}
///////////////////////////////////////////////////////////////////////////
Nc3Vector Nc3Vector::GetUnprimed(TRotMatrix* m) const
{
/**
~~~
// Provide original vector components (and errors) from the rotated ones.
// The orientation of the rotated frame is described by the TRotMatrix
// input argument.
// So, this is the inverse of the GetPrimed() memberfunction.
// This memberfunction makes use of the fact that the inverse of a certain
// TRotMatrix is given by its transposed matrix.
~~~
**/

 Nc3Vector v(*this);
 if (!m) return v;

 Double_t* mat=m->GetMatrix();

 Double_t a[3],aprim[3];

 if (fNv)
 {
  GetVector(aprim,"car");
  a[0]=aprim[0]*mat[0]+aprim[1]*mat[3]+aprim[2]*mat[6];
  a[1]=aprim[0]*mat[1]+aprim[1]*mat[4]+aprim[2]*mat[7];
  a[2]=aprim[0]*mat[2]+aprim[1]*mat[5]+aprim[2]*mat[8];
  v.SetVector(a,"car");
 }

 if (fNv==6)
 {
  GetErrors(aprim,"car");
  a[0]=sqrt(pow(aprim[0]*mat[0],2)+pow(aprim[1]*mat[3],2)+pow(aprim[2]*mat[6],2));
  a[1]=sqrt(pow(aprim[0]*mat[1],2)+pow(aprim[1]*mat[4],2)+pow(aprim[2]*mat[7],2));
  a[2]=sqrt(pow(aprim[0]*mat[2],2)+pow(aprim[1]*mat[5],2)+pow(aprim[2]*mat[8],2));
  v.SetErrors(a,"car");
 }

 return v;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc3Vector::GetX(Int_t i,TString f,TString u)
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
// The vector components are addressed via the generic x1,x2,x3 notation.
// So, i=1 denotes the first vector component.
// The error on the selected component can be obtained via the
// usual GetResultError() facility.
~~~
**/
 
 fDresult=0;

 if (!fNv || i<1 || i>3) return 0;

 Double_t vec[3];
 GetVector(vec,f,u);

 if (fNv==6)
 {
  Double_t err[3];
  GetErrors(err,f,u);
  fDresult=err[i-1];
 }

 return vec[i-1];
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc3Vector::GetOpeningAngle(Nc3Vector& q,TString u)
{
/**
~~~
// Provide the opening angle with vector q.
// The string argument "u" allows to choose between different output units.
// u = "rad" : opening angle provided in radians
//     "deg" : opening angle provided in degrees
//
// The default is u="rad".
~~~
**/

 Double_t ang=0;

 if (GetNorm()<=0. || q.GetNorm()<=0.) return ang;

 Double_t vec[3];
 Double_t err[3];

 Nc3Vector v1;
 GetVector(vec,"sph");
 vec[0]=1.;
 v1.SetVector(vec,"sph");

 if (fNv==6)
 {
  GetErrors(err,"sph");
  err[0]=0.;
  v1.SetErrors(err,"sph");
 }

 Nc3Vector v2;
 q.GetVector(vec,"sph");
 vec[0]=1.;
 v2.SetVector(vec,"sph");

 if (q.fNv==6)
 {
  q.GetErrors(err,"sph");
  err[0]=0.;
  v2.SetErrors(err,"sph");
 }

 Double_t x=v1.Dot(v2);
 Double_t dx=fDresult;
 if (x>1.) x=1;
 if (x<-1.) x=-1;
 ang=acos(x);
 fDresult=0;
 if (fabs(x)<1.-dx) fDresult=dx/sqrt(1.-x*x);

 if (u == "deg")
 {
  Double_t pi=acos(-1.);
  ang*=180./pi;
  fDresult*=180./pi;
 }

 return ang;
}
///////////////////////////////////////////////////////////////////////////
Double_t Nc3Vector::ConvertAngle(Double_t a,TString in,TString out) const
{
/**
~~~
// Conversion of various angular formats.
//
// The input argument "a" denotes the angle to be converted. 
// The string arguments "in" and "out" specify the angular I/O formats.
//
// in = "rad" : input angle provided in radians
//      "deg" : input angle provided in degrees
//      "dms" : input angle provided in dddmmss.sss
//      "hms" : input angle provided in hhmmss.sss
//      "hrs" : input angle provided in fractional hours
//
// out = "rad" : output angle provided in radians
//       "deg" : output angle provided in degrees
//       "dms" : output angle provided in dddmmss.sss
//       "hms" : output angle provided in hhmmss.sss
//       "hrs" : output angle provided in fractional hours
~~~
**/
 
 if (in==out) return a;

 // Convert input to its absolute value in (fractional) degrees. 
 Double_t pi=acos(-1.);
 Double_t epsilon=1.e-12; // Accuracy in (arc)seconds
 Int_t word=0,ddd=0,hh=0,mm=0,ss=0;
 Double_t s=0;

 Double_t b=fabs(a);

 if (in=="rad") b*=180./pi;

 if (in=="hrs") b*=15.;

 if (in=="dms")
 {
  word=Int_t(b);
  ddd=word/10000;
  word=word%10000;
  mm=word/100;
  ss=word%100;
  s=b-Double_t(ddd*10000+mm*100+ss);
  b=Double_t(ddd)+Double_t(mm)/60.+(Double_t(ss)+s)/3600.;
 }

 if (in=="hms")
 {
  word=Int_t(b);
  hh=word/10000;
  word=word%10000;
  mm=word/100;
  ss=word%100;
  s=b-Double_t(hh*10000+mm*100+ss);
  b=15.*(Double_t(hh)+Double_t(mm)/60.+(Double_t(ss)+s)/3600.);
 }

 while (b>360)
 {
  b-=360.;
 }

 if (out=="rad") b*=pi/180.;

 if (out=="hrs") b/=15.;

 if (out=="dms")
 {
  ddd=Int_t(b);
  b=b-Double_t(ddd);
  b*=60.;
  mm=Int_t(b);
  b=b-Double_t(mm);
  b*=60.;
  ss=Int_t(b);
  s=b-Double_t(ss);
  if (s>(1.-epsilon))
  {
   s=0.;
   ss++;
  }
  while (ss>=60)
  {
   ss-=60;
   mm++;
  }
  while (mm>=60)
  {
   mm-=60;
   ddd++;
  }
  while (ddd>=360)
  {
   ddd-=360;
  }
  b=Double_t(10000*ddd+100*mm+ss)+s;
 }

 if (out=="hms")
 {
  b/=15.;
  hh=Int_t(b);
  b=b-Double_t(hh);
  b*=60.;
  mm=Int_t(b);
  b=b-Double_t(mm);
  b*=60.;
  ss=Int_t(b);
  s=b-Double_t(ss);
  if (s>(1.-epsilon))
  {
   s=0.;
   ss++;
  }
  while (ss>=60)
  {
   ss-=60;
   mm++;
  }
  while (mm>=60)
  {
   mm-=60;
   hh++;
  }
  while (hh>=24)
  {
   hh-=24;
  }
  b=Double_t(10000*hh+100*mm+ss)+s;
 }

 if (a<0) b=-b;

 return b;
}
///////////////////////////////////////////////////////////////////////////
void Nc3Vector::PrintAngle(Double_t a,TString in,TString out,Int_t ndig,Bool_t align) const
{
/**
~~~
// Printing of angles in various formats.
//
// The input argument "a" denotes the angle to be printed. 
// The string arguments "in" and "out" specify the angular I/O formats.
//
// in = "rad" : input angle provided in radians
//      "deg" : input angle provided in degrees
//      "dms" : input angle provided in dddmmss.sss
//      "hms" : input angle provided in hhmmss.sss
//
// out = "rad" : output angle provided in radians
//       "deg" : output angle provided in degrees
//       "dms" : output angle provided in dddmmss.sss
//       "hms" : output angle provided in hhmmss.sss
//
// The argument "ndig" specifies the number of digits for the fractional
// part (e.g. ndig=6 for "dms" corresponds to micro-arcsecond precision).
// Rounding will be performed, so an arcsecond count of 3.473 with ndig=1
// will appear as 03.5 on the output, but with ndig=2 it will show 03.47.
// Due to computer accuracy, precision on the pico-arcsecond level may get lost.
//
// The argument "align", when set to kTRUE, ensures that for a certain selected
// output format, all provided output strings will have the same length.
// This will allow the user to provide data in an aligned tabular format.
// When "align" is set to kFALSE, the produced output will be in the most
// compact format.
//
// The defaults are ndig=1 and align=kFALSE.
//
// Note : The angle info is printed without additional spaces or "endline".
//        This allows the print to be included in various composite output formats.
~~~
**/

 Double_t b=ConvertAngle(a,in,out);

 if (out=="deg" || out=="rad")
 {
  if (align)
  {
   printf("%*.*f %-s",5+ndig,ndig,b,out.Data());
  }
  else
  {
   printf("%-.*f %-s",ndig,b,out.Data());
  }
  return; 
 }

 Double_t epsilon=1.e-12; // Accuracy in (arc)seconds
 Int_t word=0,ddd=0,hh=0,mm=0,ss=0;
 Double_t s;

 if (out=="dms")
 {
  word=Int_t(b);
  word=abs(word);
  ddd=word/10000;
  word=word%10000;
  mm=word/100;
  ss=word%100;
  s=fabs(b)-Double_t(ddd*10000+mm*100+ss);
  if (s>(1.-epsilon))
  {
   s=0.;
   ss++;
  }
  while (ss>=60)
  {
   ss-=60;
   mm++;
  }
  while (mm>=60)
  {
   mm-=60;
   ddd++;
  }
  while (ddd>=360)
  {
   ddd-=360;
  }
  if (b<0) ddd=-ddd;
  s+=double(ss);
  if (align)
  {
   printf("%4dd %02d' %0*.*f\"",ddd,mm,3+ndig,ndig,s);
  }
  else
  {
   printf("%-dd %-d' %-.*f\"",ddd,mm,ndig,s);
  }
  return;
 }

 if (out=="hms")
 {
  word=Int_t(b);
  word=abs(word);
  hh=word/10000;
  word=word%10000;
  mm=word/100;
  ss=word%100;
  s=fabs(b)-Double_t(hh*10000+mm*100+ss);
  if (s>(1.-epsilon))
  {
   s=0.;
   ss++;
  }
  while (ss>=60)
  {
   ss-=60;
   mm++;
  }
  while (mm>=60)
  {
   mm-=60;
   hh++;
  }
  while (hh>=24)
  {
   hh-=24;
  }
  if (b<0) hh=-hh;
  s+=double(ss);
  if (align)
  {
   printf("%3dh %02dm %0*.*fs",hh,mm,3+ndig,ndig,s);
  }
  else
  {
   printf("%-dh %-dm %-.*fs",hh,mm,ndig,s);
  }
  return;
 }
}
///////////////////////////////////////////////////////////////////////////
