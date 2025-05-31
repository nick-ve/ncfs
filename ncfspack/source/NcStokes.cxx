/**  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
~~~
 * Copyright(c) 2021 NCFS/IIHE, All Rights Reserved.                           *
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
/** @class NcStokes
~~~
// Class NcStokes
// Treatment of Stokes parameters for EM polarisation studies.
//
// The Stokes parameters provide a convenient way to describe the polarisation
// of electromagnetic radiation based on observed intensities, without the need
// for detailed phase information.
// The Stokes parameters are represented by 4 observables, denoted as components
// of a so-called Stokes vector (S0,S1,S2,S3) also known as (I,Q,U,V).
//
// Since EM radiation is transversely polarized, the polarisation can be described
// by two components of the electric field vector E in a plane perpendicular to
// the direction of propagation.
// These two electric field components may oscillate independently, provided that
// the magnetic field vector B oscillates correspondingly such that it stays
// perpendicular to the electric field in the plane of oscillation.
// This allows a rotating E vector, describing an ellipse as the most generic result,
// with as extreme cases a circle (circular polarisation) or a line (linear polarisation).
//
// A generic polarisation description is obtained via a 2-dimensional complex vector,
// called a Jones vector: J=(E1,E2)=(A1*exp(i*phi1),A2*exp(i*phi2))*exp(i*omega*t).
// Here |E1|=A1 and |E2|=A2 are the amplitudes, and phi1 and phi2 are the phases
// with respect to some orthogonal basis.
// The term exp(i*omega*t) represents the oscillation with angular frequency omega.
// The physical electrical field is the real part of the Jones vector.
//
// The physical interpretation of the Stokes parameters is that each parameter represents
// a net polarisation intensity along a certain coordinate axis of an orthogonal basis.
// The Stokes parameter I represents the total beam intensity, so we need 3 different
// orthogonal bases to describe the net polarisation intensities via the parameters Q, U and V.
//
// The convention used here is to define a right-handed Cartesian coordinate system (x,y,z),
// in which the EM wave moves in the +z direction and the oscillation takes
// place in the x-y plane.
// The right hand rule, with the thumb pointing in the direction of propagation,
// defines the rotation direction with positive helicity.
//
// This Cartesian coordinate system defines the first orthogonal basis (x,y).
// The 2nd orthogonal basis (a,b) is obtained by rotating the (x,y) basis over 45 degrees.
// In other words : a=(x+y)/sqrt(2) and b=(-x+y)/sqrt(2)
// The 3rd orthogonal basis (R,L) is a circular basis with R=(x+iy)/sqrt(2) and L=(x-iy)/sqrt(2).
// Here R and L represent Right handed and Left handed rotation, respectively.
//
// Since intensity=|E|^2 we can obtain the intensities along the various base vectors
// by taking the dot product of J with the corresponding base vector.
// So, for instance I(x)=|<J,x>|^2 where <J,x> denotes the dot product of J with the x base vector.
// Note that for a complex vector E we have |E|^2=E*E^*  where E^* indicates the complex conjugate of E.
//   
// The Stokes parameters represent the following intensities :
//
// I=I(x)+I(y) The sum of the two (x,y) linear polarisation intensities.
// Q=I(x)-I(y) The difference of the two (x,y) linear polarisation intensities.
// U=I(a)-I(b) The difference of the two (a,b) linear polarisation intensities.
//             Note that Q=0 at the axes of the (a,b) frame and U=0 at the axes of the (x,y) frame.
// V=I(R)-I(L) The difference of the right and left circular polarisation intensities.
//
// In other words :
//
// I : represents the total beam intensity.
// Q : represents a net horizontal (Q>0) or vertical (Q<0) linear polarisation component.
// U : represents a net diagonal linear polarisation component.
// V : represents a net circular polarisation (right:V>0  left:V<0) component.
//
// whereas Q=U=V=0 represents an unpolarized beam of EM radiation.
//
// For a fully polarized monochromatic beam of EM radiation in our Cartesian (x,y,z) frame,
// the Stokes parameters are obtained from the projections of the Jones vector as follows :
//
// I=|E1|^2+|E2|^2=A1^2+A2^2
// Q=|E1|^2-|E2|^2=A1^2-A2^2
// U=2*Re(E1*E2^*)=2*A1*A2*cos(phi1-phi2)
// V=2*Im(E1*E2^*)=2*A1*A2*sin(phi1-phi2)
//
// In terms of a polarisation ellipse with semi major axis A, semi minor axis B and
// orientation angle theta between a and the x-axis we have :
//
// I=A^2+B^2
// Q=(A^2-B^2)*cos(2*theta)
// U=(A^2-B^2)*sin(2*theta)
// V=2*A*B*helicity
//
// It is seen that the Stokes parameters reflect intensities without any
// time dependent phase, and as such can be treated via simple addition and subtraction.
// In particular, the above shows that I^2=Q^2+U^2+V^2.
//
// Since a beam of EM radiation may contain both polarized and unpolarized radiation,
// only a fraction f of the total beam intensity I will be polarized.
// Consequently, we should have used only the polarized intensity P=f*I in the above expressions,
// such that P^2=Q^2+U^2+V^2.
// However, the Stokes parameter I always represents the total beam intensity, so that I=P/f
// for a beam with a certain amount of polarisation.
// P=sqrt(Q^2+U^2+V^2) represents the total polarisation intensity and the ratio P/I is called
// the polarisation fraction.
// Furthermore, L=sqrt(Q^2+U^2) is called the linear polarisation intensity,
// whereas |V| represents the circular polarisation intensity.
//
// Inversion of the above equations yields for the Jones vector :
//
// A1=sqrt((P+Q)/2) A2=sqrt((P-Q)/2) and (phi1-phi2)=arctan(V/U)
//
// whereas for the polarisation ellipse parameters we obtain :
//
// A=sqrt((P+L)/2) B=sqrt((P-L)/2) theta=0.5*arctan(U/Q)
//
// This class provides memberfunctions to enter data either directly via the Stokes parameters,
// see SetStokesParameters(), via specification of the Jones vector, see SetJonesVector(),
// or by providing the geometrical parameters of the polarisation ellipse, see SetEllipseParameters().
// Once the data have been entered, the various parameter values may be obtained via GetParameter()
// or listed via the Data() memberfunction.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, August 21, 2021  13:39Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, August 27, 2021  10:48Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "NcStokes.h"
#include "Riostream.h"
 
ClassImp(NcStokes); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
NcStokes::NcStokes()
{
/**
~~~
// Default constructor.
~~~
**/

 fI=0;
 fQ=0;
 fU=0;
 fV=0;
}
///////////////////////////////////////////////////////////////////////////
NcStokes::~NcStokes()
{
/**
~~~
// Default destructor.
~~~
**/
}
///////////////////////////////////////////////////////////////////////////
NcStokes::NcStokes(const NcStokes& q)
{
/**
~~~
// Copy constructor.
~~~
**/

 fI=q.fI;
 fQ=q.fQ;
 fU=q.fU;
 fV=q.fV;
}
///////////////////////////////////////////////////////////////////////////
void NcStokes::SetStokesParameters(Double_t I,Double_t Q,Double_t U,Double_t V)
{
/**
~~~
// Set the values of the Stokes parameters I, Q, U and V.
// These parameters are also known as S0, S1, S2 and S3, respectively.
~~~
**/

 if (I<=0)
 {
  cout << " *NcStokes::SetStokesParameters* Inconsistent data." << endl;
  cout << " I=" << fI << " Q=" << fQ << " U=" << fU << " V=" << fV << endl;
  return;
 }

 fI=I;
 fQ=Q;
 fU=U;
 fV=V;
}
///////////////////////////////////////////////////////////////////////////
void NcStokes::SetEllipseParameters(Double_t a,Double_t b,Double_t theta,TString u,Int_t h,Double_t p)
{
/**
~~~
// Set the parameters of the polarisation ellipse.
//
// Input arguments :
// -----------------
// a     : semi major axis
// b     : semi minor axis
// theta : angle between the major axis and the x-axis
// u     : "deg" --> theta provide in degrees
//         "rad" --> theta provide in radians
// h     : helicity (+1=right-handed  -1=left-handed)
// p     : total polarisation fraction
//
// The default value is p=1.
~~~
**/

 fI=0;
 fQ=0;
 fU=0;
 fV=0;

 if (a<0 || b<0 || p<=0)
 {
  cout << " *NcStokes::SetEllipseParameters* Inconsistent input." << endl;
  cout << " a:" << a << " b:" << b << " theta:" << theta << " " << u << ". h:" << h << " p:" << p << endl;
  return;
 }

 Double_t pi=acos(-1.);

 if (u=="deg") theta*=pi/180.;

 Double_t a2=a*a;
 Double_t b2=b*b;

 fI=(a2+b2)/p;
 fQ=(a2-b2)*cos(2.*theta);
 fU=(a2-b2)*sin(2.*theta);
 fV=2.*a*b;
 if (h<0) fV=-fV;
}
///////////////////////////////////////////////////////////////////////////
void NcStokes::SetJonesVector(Double_t A1,Double_t A2,Double_t phi,TString u,Double_t p)
{
/**
~~~
// Set the components of the Jones field vector (E1,E2)=(A1*exp(i*phi1),A2*exp(i*phi2))
// with phase difference phi=phi1-phi2.
//
// u : "deg" --> phi is given in degrees
// u : "rad" --> phi is given in radians
// p : total polarisation fraction
//
// The default value is p=1.
~~~
**/

 fI=0;
 fQ=0;
 fU=0;
 fV=0;

 if (A1<0 || A2<0 || A1*A2==0 || p<=0)
 {
  cout << " *NcStokes::SetJonesVector* Inconsistent input." << endl;
  cout << " A1:" << A1 << " A2:" << A2 << " phi:" << phi << " " << u << "." << " p:" << p << endl;
  return;
 }

 Double_t pi=acos(-1.);

 if (u=="deg") phi*=pi/180.;

 fI=(A1*A1+A2*A2)/p;
 fQ=A1*A1-A2*A2;
 fU=2.*A1*A2*cos(phi);
 fV=2.*A1*A2*sin(phi);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcStokes::GetParameter(TString name,TString u)
{
/**
~~~
// Provide the value of the parameter with the specified name.
// For details about the various parameters, please refer to the
// general documentation of this class.
//
// u = "deg" --> Angles are returned in degrees 
//     "rad" --> Angles are returned in radians
//
// Supported parameter names are :
// -------------------------------
// I (or S0) : The total beam intensity I(x)+I(y) or equivalently I(a)+I(b)
// Q (or S1) : The net linear polarisation intensity I(x)-I(y)
// U (or S2) : The 45 degree rotated net linear polarisation intensity I(a)-I(b)
// V (or S3) : The net circular polarisation intensity I(R)-I(L)
// P         : The total polarisation intensity
// L         : The linear polarisation intensity
// C         : The circular polarisation intensity
// a         : The semi major axis of the polarisation ellipse
// b         : The semi minor axis of the polarisation ellipse
// e         : The eccentricity of the polarisation ellipse
// theta     : The polarisation angle (=orientation angle of the ellipse)
// beta      : The ellipticity angle of the polarisation ellipse, i.e. arctan(b/a)
// helicity  : The helicity of the polarisation (+1=right handed  -1=left handed)
// A1        : Amplitude |E1| of the Jones field vector (E1,E2)=(A1*exp(i*phi1),A2*exp(i*phi2))
// A2        : Amplitude |E2| of the Jones field vector (E1,E2)=(A1*exp(i*phi1),A2*exp(i*phi2))
// phi       : Phase difference (phi1-phi2) between the Jones vector components 
//
// The default value is u="rad".
~~~
**/

 Double_t value=0;

 if (fI<=0)
 {
  cout << endl;
  cout << " *NcStokes::GetParameter* Inconsistent data encountered : I=" << fI << endl;
  cout << " --- The Stokes vector (I,Q,U,V) is also known as (S0,S1,S2,S3)." << endl;
  return 0;
 }

 Double_t pi=acos(-1.);

 Double_t P=sqrt(fQ*fQ+fU*fU+fV*fV);
 Double_t L=sqrt(fQ*fQ+fU*fU);
 Double_t a=sqrt(0.5*(P+L));
 Double_t b=sqrt(0.5*(P-L));
 Double_t A1=sqrt(0.5*(P+fQ));
 Double_t A2=sqrt(0.5*(P-fQ));

 if (name=="I" || name=="S0") value=fI;
 if (name=="Q" || name=="S1") value=fQ;
 if (name=="U" || name=="S2") value=fU;
 if (name=="V" || name=="S3") value=fV;
 if (name=="P") value=P;
 if (name=="L") value=L;
 if (name=="C") value=fabs(fV);
 if (name=="a") value=a;
 if (name=="b") value=b;
 if (name=="e" && P) value=sqrt(1.-(b*b)/(a*a));
 if (name=="A1") value=A1;
 if (name=="A2") value=A2;
 if (name=="theta" && L)
 {
  value=0.5*atan2(fU,fQ);
  if (u=="deg") value*=180./pi;
 }
 if (name=="beta" && P)
 {
  value=atan2(b,a);
  if (u=="deg") value*=180./pi;
 }
 if (name=="helicity" && fV)
 {
  value=1;
  if (fV<0) value=-1;
 }
 if (name=="phi" && (fV || fU))
 {
  value=atan2(fV,fU);
  if (u=="deg") value*=180./pi;
 }

 return value;
}
///////////////////////////////////////////////////////////////////////////
void NcStokes::Data(TString u)
{
/**
~~~
// Provide all polarisation related information.
// For details about the various parameters, please refer to the
// general documentation of this class.
//
// The input argument "u" allows to select the units for angles.
//
// u = "deg" --> Angles are given in degrees
//     "rad" --> Angles are given in radians
//
// The default value is u="rad".
~~~
**/

 if (fI<=0)
 {
  cout << endl;
  cout << " *NcStokes::Data* Inconsistent data encountered : I=" << fI << endl;
  cout << " --- The Stokes vector (I,Q,U,V) is also known as (S0,S1,S2,S3)." << endl;
  return;
 }

 Double_t P=GetParameter("P");
 Double_t L=GetParameter("L");
 Int_t h=TMath::Nint(GetParameter("helicity"));
 Double_t A1=GetParameter("A1");
 Double_t A2=GetParameter("A2");
 Double_t phi=GetParameter("phi",u);
 Double_t a=GetParameter("a");
 Double_t b=GetParameter("b");
 Double_t e=GetParameter("e");
 Double_t beta=GetParameter("beta",u);
 Double_t theta=GetParameter("theta",u);

 cout << " *NcStokes::Data* Listing of all polarisation information." << endl;
 cout << " --- The Stokes vector (I,Q,U,V) is also known as (S0,S1,S2,S3)." << endl;
 cout << " Stokes parameters....................: I=" << fI << " Q=" << fQ << " U=" << fU << " V=" << fV << endl;
 cout << " Total beam intensity.................: I=" << fI << endl;
 cout << " Total polarisation intensity.........: P=" << P << endl;
 cout << " Linear polarisation intensity........: L=" << L << endl;
 cout << " Circular polarisation intensity......: C=" << fabs(fV) << endl;
 cout << " Fractional polarisation intensities..: P/I=" << P/fI << " L/I=" << L/fI << " C/I=" << fabs(fV/fI) << endl;
 if (P)
 {
  cout << " Jones (x,y) field vector (E1,E2).....: |E1|=" << A1 << " |E2|=" << A2 << " (phi1-phi2)=" << phi << " " << u << "." << endl;
  cout << " Polarisation ellipse (semi axes a,b) : a=" << a << " b=" << b << " eccentricity=" << e << " arctan(b/a)=" << beta << " " << u << "." << endl;
  if (L) cout << " Ellipse orientation angle............: " << theta << " " << u << "." << endl;
  if (fV) cout << " Polarisation helicity................: " << h << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
