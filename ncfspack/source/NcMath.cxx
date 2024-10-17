/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997, NCFS/IIHE, All Rights Reserved.                          *
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

///////////////////////////////////////////////////////////////////////////
// Class NcMath
// Various mathematical tools which may be very convenient while
// performing scientific analysis.
//
// Example : Probability of a Chi-squared value
// =========
//
// NcMath M;
// Float_t chi2=20;            // The chi-squared value
// Int_t ndf=12;               // The number of degrees of freedom
// Float_t p=M.Prob(chi2,ndf); // The probability that at least a Chi-squared
//                             // value of chi2 will be observed, even for a
//                             // correct model
//
//--- Author: Nick van Eijndhoven 14-nov-1998 Utrecht University
//- Modified: Nick van Eijndhoven, IIHE-VUB Brussel, May 7, 2024  10:30Z
///////////////////////////////////////////////////////////////////////////

#include "NcMath.h"
#include "Riostream.h"
 
ClassImp(NcMath) // Class implementation to enable ROOT I/O
 
NcMath::NcMath() : TObject()
{
// Default constructor
}
///////////////////////////////////////////////////////////////////////////
NcMath::~NcMath()
{
// Destructor
}
///////////////////////////////////////////////////////////////////////////
NcMath::NcMath(const NcMath& m) : TObject(m)
{
// Copy constructor
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Zeta(Double_t x, Int_t nterms) const
{
// Computation of the Riemann Zeta function Zeta(x) for all x>1.
//
// The input argument "nterms" determines the number of terms that will be
// evaluated in the summation series.
// Default value : nterms=100000 which provides an accuracy of about 10^-5.
//
// In case of invalid input, the value 0 is returned.
//
//--- Nick van Eijndhoven 14-may-2012, IIHE, Brussels.
 
 if (x<=1)
 {
  cout << "*Zeta(x)* Wrong argument x = " << x << endl;
  return 0;
 }

 Double_t zeta=0;
 Double_t r=0;
 for (Int_t i=1; i<=nterms; i++)
 {
  r=double(i);
  zeta+=1./pow(r,x);
 }

 return zeta;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Gamma(Double_t z) const
{
// Computation of Gamma(z) for all z>0.
//
// The algorithm is based on the article by C.Lanczos [1] as denoted in
// Numerical Recipes 2nd ed. on p. 207 (W.H.Press et al.).
//
// [1] C.Lanczos, SIAM Journal of Numerical Analysis B1 (1964), 86.
//
//--- Nve 14-nov-1998 UU-SAP Utrecht
 
 if (z<=0.)
 {
  cout << "*Gamma(z)* Wrong argument z = " << z << endl;
  return 0;
 }
 
 Double_t v=LnGamma(z);
 return exp(v);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Gamma(Double_t a,Double_t x,Int_t mode) const
{
// Computation of the incomplete gamma function P(a,x) or gamma(a,x).
//
// where : P(a,x)=gamma(a,x)/Gamma(a)
//
// mode = 0 : Value of P(a,x) is returned
//        1 : Value of gamma(a,x) is returned
//
// By default mode=0.
//
// The algorithm is based on the formulas and code as denoted in
// Numerical Recipes 2nd ed. on p. 210-212 (W.H.Press et al.).
//
//--- Nve 14-nov-1998 UU-SAP Utrecht
 
 Double_t value=0;

 if (a<=0.)
 {
  cout << "*Gamma(a,x)* Invalid argument a = " << a << endl;
  return 0;
 }
 
 if (x<=0.)
 {
  if (x<0) cout << "*Gamma(a,x)* Invalid argument x = " << x << endl;
  return 0;
 }
 
 if (x<(a+1.))
 {
  value=GamSer(a,x);
 }
 else
 {
  value=GamCf(a,x);
 }

 if (mode) value=value*Gamma(a);
 return value;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::LnGamma(Double_t z) const
{
// Computation of ln[Gamma(z)] for all z>0.
//
// The algorithm is based on the article by C.Lanczos [1] as denoted in
// Numerical Recipes 2nd ed. on p. 207 (W.H.Press et al.).
//
// [1] C.Lanczos, SIAM Journal of Numerical Analysis B1 (1964), 86.
//
// The accuracy of the result is better than 2e-10.
//
//--- Nve 14-nov-1998 UU-SAP Utrecht
 
 if (z<=0.)
 {
  cout << "*LnGamma(z)* Wrong argument z = " << z << endl;
  return 0;
 }
 
 // Coefficients for the series expansion
 Double_t c[7];
 c[0]=  2.5066282746310005;
 c[1]= 76.18009172947146;
 c[2]=-86.50532032941677;
 c[3]= 24.01409824083091;
 c[4]= -1.231739572450155;
 c[5]=  0.1208650973866179e-2;
 c[6]= -0.5395239384953e-5;
 
 Double_t x=z;
 Double_t y=x;
 Double_t tmp=x+5.5;
 tmp=(x+0.5)*log(tmp)-tmp;
 Double_t ser=1.000000000190015;
 for (Int_t i=1; i<7; i++)
 {
  y+=1.;
  ser+=c[i]/y;
 }
 Double_t v=tmp+log(c[0]*ser/x);
 return v;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::LnGamma(Double_t a,Double_t x,Int_t mode) const
{
// Computation of the ln of the incomplete gamma function P(a,x) or gamma(a,x).
//
// where : P(a,x)=gamma(a,x)/Gamma(a)
//
// mode = 0 : Value of ln[P(a,x)] is returned
//        1 : Value of ln[gamma(a,x)] is returned
//
// By default mode=0.
//
//--- Nick van Eijndhoven 07-feb-2016, IIHE-VUB, Brussel
 
 Double_t value=0;
 Double_t gammaP=0;

 gammaP=Gamma(a,x,0);

 if (gammaP)
 {
  value=log(gammaP);
  if (mode) value=value+LnGamma(a);
 }

 return value;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::GamSer(Double_t a,Double_t x) const
{
// Computation of the incomplete gamma function P(a,x)
// via its series representation.
//
// The algorithm is based on the formulas and code as denoted in
// Numerical Recipes 2nd ed. on p. 210-212 (W.H.Press et al.).
//
//--- Nve 14-nov-1998 UU-SAP Utrecht
 
 Int_t itmax=100;   // Maximum number of iterations
 Double_t eps=3.e-7; // Relative accuracy
 
 if (a<=0.)
 {
  cout << "*GamSer(a,x)* Invalid argument a = " << a << endl;
  return 0;
 }
 
 if (x<=0.)
 {
  if (x<0) cout << "*GamSer(a,x)* Invalid argument x = " << x << endl;
  return 0;
 }
 
 Double_t gln=LnGamma(a);
 Double_t ap=a;
 Double_t sum=1./a;
 Double_t del=sum;
 for (Int_t n=1; n<=itmax; n++)
 {
  ap+=1.;
  del=del*x/ap;
  sum+=del;
  if (fabs(del)<fabs(sum*eps)) break;
  if (n==itmax) cout << "*GamSer(a,x)* a too large or itmax too small" << endl;
 }
 Double_t v=sum*exp(-x+a*log(x)-gln);
 return v;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::GamCf(Double_t a,Double_t x) const
{
// Computation of the incomplete gamma function P(a,x)
// via its continued fraction representation.
//
// The algorithm is based on the formulas and code as denoted in
// Numerical Recipes 2nd ed. on p. 210-212 (W.H.Press et al.).
//
//--- Nve 14-nov-1998 UU-SAP Utrecht
 
 Int_t itmax=100;      // Maximum number of iterations
 Double_t eps=3.e-7;    // Relative accuracy
 Double_t fpmin=1.e-30; // Smallest Double_t value allowed here
 
 if (a<=0.)
 {
  cout << "*GamCf(a,x)* Invalid argument a = " << a << endl;
  return 0;
 }
 
 if (x<=0.)
 {
  if (x<0) cout << "*GamCf(a,x)* Invalid argument x = " << x << endl;
  return 0;
 }
 
 Double_t gln=LnGamma(a);
 Double_t b=x+1.-a;
 Double_t c=1./fpmin;
 Double_t d=1./b;
 Double_t h=d;
 Double_t an,del;
 for (Int_t i=1; i<=itmax; i++)
 {
  an=double(-i)*(double(i)-a);
  b+=2.;
  d=an*d+b;
  if (fabs(d)<fpmin) d=fpmin;
  c=b+an/c;
  if (fabs(c)<fpmin) c=fpmin;
  d=1./d;
  del=d*c;
  h=h*del;
  if (fabs(del-1.)<eps) break;
  if (i==itmax) cout << "*GamCf(a,x)* a too large or itmax too small" << endl;
 }
 Double_t v=exp(-x+a*log(x)-gln)*h;
 return (1.-v);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Erf(Double_t x) const
{
// Computation of the error function erf(x).
//
//--- NvE 14-nov-1998 UU-SAP Utrecht
 
 return (1.-Erfc(x));
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Erfc(Double_t x) const
{
// Computation of the complementary error function erfc(x).
//
// The algorithm is based on a Chebyshev fit as denoted in
// Numerical Recipes 2nd ed. on p. 214 (W.H.Press et al.).
//
// The fractional error is always less than 1.2e-7.
//
//--- Nve 14-nov-1998 UU-SAP Utrecht
 
 // The parameters of the Chebyshev fit
 const Double_t ka1=-1.26551223,  ka2=1.00002368,
                ka3= 0.37409196,  ka4=0.09678418,
                ka5=-0.18628806,  ka6=0.27886807,
                ka7=-1.13520398,  ka8=1.48851587,
                ka9=-0.82215223, ka10=0.17087277;
 
 Double_t v=1.; // The return value
 
 Double_t z=fabs(x);
 
 if (z <= 0.) return v; // erfc(0)=1
 
 Double_t t=1./(1.+0.5*z);
 
 v=t*exp((-z*z)
   +ka1+t*(ka2+t*(ka3+t*(ka4+t*(ka5+t*(ka6+t*(ka7+t*(ka8+t*(ka9+t*ka10)))))))));
 
 if (x < 0.) v=2.-v; // erfc(-x)=2-erfc(x)
 
 return v;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Prob(Double_t chi2,Int_t ndf,Int_t mode) const
{
// Computation of the probability for a certain Chi-squared (chi2)
// and number of degrees of freedom (ndf).
//
// A more clear and flexible facility is offered by Chi2Pvalue. 
//
// According to the value of the parameter "mode" various algorithms
// can be selected.
//
// mode = 0 : Calculations are based on the incomplete gamma function P(a,x),
//            where a=ndf/2 and x=chi2/2.
//
//        1 : Same as for mode=0. However, in case ndf=1 an exact expression
//            based on the error function Erf() is used.
//
//        2 : Same as for mode=0. However, in case ndf>30 a Gaussian approximation
//            is used instead of the gamma function.
//
// When invoked as Prob(chi2,ndf) the default mode=1 is used.
//
// P(a,x) represents the probability that the observed Chi-squared
// for a correct model is at most the value chi2.
//
// The returned probability corresponds to 1-P(a,x),
// which denotes the probability that an observed Chi-squared is at least
// the value chi2 by chance, even for a correct model.
//
//--- NvE 14-nov-1998 UU-SAP Utrecht
 
 if (ndf <= 0) return 0; // Set CL to zero in case ndf<=0
 
 if (chi2 <= 0.)
 {
  if (chi2 < 0.)
  {
    return 0;
  }
  else
  {
   return 1;
  }
 }

 Double_t v=-1.;

 switch (mode)
 {
  case 1: // Exact expression for ndf=1 as alternative for the gamma function
   if (ndf==1) v=1.-Erf(sqrt(chi2)/sqrt(2.));
   break;
 
  case 2: // Gaussian approximation for large ndf (i.e. ndf>30) as alternative for the gamma function
   if (ndf>30)
   {
    Double_t q=sqrt(2.*chi2)-sqrt(double(2*ndf-1));
    if (q>0.) v=0.5*(1.-Erf(q/sqrt(2.)));
   }
   break;
 }
 
 if (v<0.)
 {
  // Evaluate the incomplete gamma function
  Double_t a=double(ndf)/2.;
  Double_t x=chi2/2.;
  v=1.-Gamma(a,x);
 }

 return v;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::BesselI0(Double_t x) const
{
// Computation of the modified Bessel function I_0(x) for any real x.
//
// The algorithm is based on the article by Abramowitz and Stegun [1]
// as denoted in Numerical Recipes 2nd ed. on p. 230 (W.H.Press et al.).
//
// [1] M.Abramowitz and I.A.Stegun, Handbook of Mathematical Functions,
//     Applied Mathematics Series vol. 55 (1964), Washington.  
//
//--- NvE 12-mar-2000 UU-SAP Utrecht

 // Parameters of the polynomial approximation  
 const Double_t kp1=1.0,          kp2=3.5156229,    kp3=3.0899424,
                kp4=1.2067492,    kp5=0.2659732,    kp6=3.60768e-2,  kp7=4.5813e-3;

 const Double_t kq1= 0.39894228,  kq2= 1.328592e-2, kq3= 2.25319e-3,
                kq4=-1.57565e-3,  kq5= 9.16281e-3,  kq6=-2.057706e-2,
                kq7= 2.635537e-2, kq8=-1.647633e-2, kq9= 3.92377e-3; 

 Double_t ax=fabs(x);

 Double_t y=0,result=0;

 if (ax < 3.75)
 {
  y=pow(x/3.75,2);
  result=kp1+y*(kp2+y*(kp3+y*(kp4+y*(kp5+y*(kp6+y*kp7)))));
 }
 else
 {
  y=3.75/ax;
  result=(exp(ax)/sqrt(ax))
         *(kq1+y*(kq2+y*(kq3+y*(kq4+y*(kq5+y*(kq6+y*(kq7+y*(kq8+y*kq9))))))));
 }

 return result;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::BesselK0(Double_t x) const
{
// Computation of the modified Bessel function K_0(x) for positive real x.
//
// The algorithm is based on the article by Abramowitz and Stegun [1]
// as denoted in Numerical Recipes 2nd ed. on p. 230 (W.H.Press et al.).
//
// [1] M.Abramowitz and I.A.Stegun, Handbook of Mathematical Functions,
//     Applied Mathematics Series vol. 55 (1964), Washington.  
//
//--- NvE 12-mar-2000 UU-SAP Utrecht

 // Parameters of the polynomial approximation  
 const Double_t kp1=-0.57721566,  kp2=0.42278420,   kp3=0.23069756,
                kp4= 3.488590e-2, kp5=2.62698e-3,   kp6=1.0750e-4,    kp7=7.4e-6;

 const Double_t kq1= 1.25331414,  kq2=-7.832358e-2, kq3= 2.189568e-2,
                kq4=-1.062446e-2, kq5= 5.87872e-3,  kq6=-2.51540e-3,  kq7=5.3208e-4;

 if (x <= 0)
 {
  cout << " *BesselK0* Invalid argument x = " << x << endl;
  return 0;
 }

 Double_t y=0,result=0;

 if (x <= 2)
 {
  y=x*x/4.;
  result=(-log(x/2.)*BesselI0(x))
         +(kp1+y*(kp2+y*(kp3+y*(kp4+y*(kp5+y*(kp6+y*kp7))))));
 }
 else
 {
  y=2./x;
  result=(exp(-x)/sqrt(x))
         *(kq1+y*(kq2+y*(kq3+y*(kq4+y*(kq5+y*(kq6+y*kq7))))));
 }

 return result;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::BesselI1(Double_t x) const
{
// Computation of the modified Bessel function I_1(x) for any real x.
//
// The algorithm is based on the article by Abramowitz and Stegun [1]
// as denoted in Numerical Recipes 2nd ed. on p. 230 (W.H.Press et al.).
//
// [1] M.Abramowitz and I.A.Stegun, Handbook of Mathematical Functions,
//     Applied Mathematics Series vol. 55 (1964), Washington.  
//
//--- NvE 12-mar-2000 UU-SAP Utrecht

 // Parameters of the polynomial approximation  
 const Double_t kp1=0.5,          kp2=0.87890594,   kp3=0.51498869,
                kp4=0.15084934,   kp5=2.658733e-2,  kp6=3.01532e-3,  kp7=3.2411e-4;

 const Double_t kq1= 0.39894228,  kq2=-3.988024e-2, kq3=-3.62018e-3,
                kq4= 1.63801e-3,  kq5=-1.031555e-2, kq6= 2.282967e-2,
                kq7=-2.895312e-2, kq8= 1.787654e-2, kq9=-4.20059e-3; 

 Double_t ax=fabs(x);

 Double_t y=0,result=0;

 if (ax < 3.75)
 {
  y=pow(x/3.75,2);
  result=x*(kp1+y*(kp2+y*(kp3+y*(kp4+y*(kp5+y*(kp6+y*kp7))))));
 }
 else
 {
  y=3.75/ax;
  result=(exp(ax)/sqrt(ax))
         *(kq1+y*(kq2+y*(kq3+y*(kq4+y*(kq5+y*(kq6+y*(kq7+y*(kq8+y*kq9))))))));
  if (x < 0) result=-result;
 }

 return result;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::BesselK1(Double_t x) const
{
// Computation of the modified Bessel function K_1(x) for positive real x.
//
// The algorithm is based on the article by Abramowitz and Stegun [1]
// as denoted in Numerical Recipes 2nd ed. on p. 230 (W.H.Press et al.).
//
// [1] M.Abramowitz and I.A.Stegun, Handbook of Mathematical Functions,
//     Applied Mathematics Series vol. 55 (1964), Washington.  
//
//--- NvE 12-mar-2000 UU-SAP Utrecht

 // Parameters of the polynomial approximation  
 const Double_t kp1= 1.,          kp2= 0.15443144,  kp3=-0.67278579,
                kp4=-0.18156897,  kp5=-1.919402e-2, kp6=-1.10404e-3,  kp7=-4.686e-5;

 const Double_t kq1= 1.25331414,  kq2= 0.23498619,  kq3=-3.655620e-2,
                kq4= 1.504268e-2, kq5=-7.80353e-3,  kq6= 3.25614e-3,  kq7=-6.8245e-4;

 if (x <= 0)
 {
  cout << " *BesselK1* Invalid argument x = " << x << endl;
  return 0;
 }

 Double_t y=0,result=0;

 if (x <= 2)
 {
  y=x*x/4.;
  result=(log(x/2.)*BesselI1(x))
         +(1./x)*(kp1+y*(kp2+y*(kp3+y*(kp4+y*(kp5+y*(kp6+y*kp7))))));
 }
 else
 {
  y=2./x;
  result=(exp(-x)/sqrt(x))
         *(kq1+y*(kq2+y*(kq3+y*(kq4+y*(kq5+y*(kq6+y*kq7))))));
 }

 return result;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::BesselK(Int_t n,Double_t x) const
{
// Computation of the Integer Order Modified Bessel function K_n(x)
// for n=0,1,2,... and positive real x.
//
// The algorithm uses the recurrence relation
//
//               K_n+1(x) = (2n/x)*K_n(x) + K_n-1(x) 
//
// as denoted in Numerical Recipes 2nd ed. on p. 232 (W.H.Press et al.).
//
//--- NvE 12-mar-2000 UU-SAP Utrecht

 if (x <= 0 || n < 0)
 {
  cout << " *BesselK* Invalid argument(s) (n,x) = (" << n << " , " << x << ")" << endl;
  return 0;
 }

 if (n==0) return BesselK0(x);

 if (n==1) return BesselK1(x);

 // Perform upward recurrence for all x
 Double_t tox=2./x;
 Double_t bkm=BesselK0(x);
 Double_t bk=BesselK1(x);
 Double_t bkp=0;
 for (Int_t j=1; j<n; j++)
 {
  bkp=bkm+double(j)*tox*bk;
  bkm=bk;
  bk=bkp;
 }

 return bk;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::BesselI(Int_t n,Double_t x) const
{
// Computation of the Integer Order Modified Bessel function I_n(x)
// for n=0,1,2,... and any real x.
//
// The algorithm uses the recurrence relation
//
//               I_n+1(x) = (-2n/x)*I_n(x) + I_n-1(x) 
//
// as denoted in Numerical Recipes 2nd ed. on p. 232 (W.H.Press et al.).
//
//--- NvE 12-mar-2000 UU-SAP Utrecht

 Int_t iacc=40; // Increase to enhance accuracy
 Double_t bigno=1.e10, bigni=1.e-10;

 if (n < 0)
 {
  cout << " *BesselI* Invalid argument (n,x) = (" << n << " , " << x << ")" << endl;
  return 0;
 }

 if (n==0) return BesselI0(x);

 if (n==1) return BesselI1(x);

 if (fabs(x) < 1.e-10) return 0;

 Double_t tox=2./fabs(x);
 Double_t bip=0,bim=0;
 Double_t bi=1;
 Double_t result=0;
 Int_t m=2*((n+int(sqrt(float(iacc*n))))); // Downward recurrence from even m
 for (Int_t j=m; j<=1; j--)
 {
  bim=bip+double(j)*tox*bi;
  bip=bi;
  bi=bim;
  if (fabs(bi) > bigno) // Renormalise to prevent overflows
  {
   result*=bigni;
   bi*=bigni;
   bip*=bigni;
  }
  if (j==n) result=bip;
 }

 result*=BesselI0(x)/bi; // Normalise with I0(x)
 if ((x < 0) && (n%2 == 1)) result=-result;

 return result;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::Chi2Dist(Int_t ndf) const
{
// Provide the Chi-squared PDF corresponding to the specified ndf degrees of freedom.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <chi2>=ndf  Var(chi2)=2*ndf
 
 TF1 pdf("Chi2PDF","1./(TMath::Gamma([0]/2.)*pow(2,[0]/2.))*pow(x,[0]/2.-1.)*exp(-x/2.)");
 pdf.SetParName(0,"ndf");
 pdf.SetParameter(0,ndf);
 TString title="#chi^{2} PDF (ndf=";
 title+=ndf;
 title+=");#chi^{2};p(#chi^{2}|ndf)"; 
 pdf.SetTitle(title.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::Chi2CDF(Int_t ndf) const
{
// Provide the Chi-squared cumulative distribution function corresponding to the
// specified ndf degrees of freedom.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <chi2>=ndf  Var(chi2)=2*ndf
 
 TF1 cdf("Chi2CDF","1.-TMath::Prob(x,[0])");
 cdf.SetParName(0,"ndf");
 cdf.SetParameter(0,ndf);
 TString title="#chi^{2} CDF (ndf=";
 title+=ndf;
 title+=");#chi^{2};CDF for p(#chi^{2}|ndf)"; 
 cdf.SetTitle(title.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::StudentDist(Double_t ndf) const
{
// Provide the Student's T PDF corresponding to the specified ndf degrees of freedom.
//
// In a frequentist approach, the Student's T distribution is particularly
// useful in making inferences about the mean of an underlying population
// based on the data from a random sample.
//
// In a Bayesian context it is used to characterise the posterior PDF
// for a particular state of information. 
//
// Note : ndf is not restricted to integer values
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <T>=0  Var(T)=ndf/(ndf-2)
 
 TF1 pdf("StudentPDF","(TMath::Gamma(([0]+1.)/2.)/(sqrt(pi*[0])*TMath::Gamma([0]/2.)))*pow(1.+x*x/[0],-([0]+1.)/2.)");
 pdf.SetParName(0,"ndf");
 pdf.SetParameter(0,ndf);
 TString title="Student's T PDF (ndf=";
 title+=ndf; 
 title+=");T;p(T|ndf)"; 
 pdf.SetTitle(title.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::StudentCDF(Double_t ndf) const
{
// Provide the Student's T cumulative distribution function corresponding to the
// specified ndf degrees of freedom.
//
// In a frequentist approach, the Student's T distribution is particularly
// useful in making inferences about the mean of an underlying population
// based on the data from a random sample.
//
// In a Bayesian context it is used to characterise the posterior PDF
// for a particular state of information. 
//
// Note : ndf is not restricted to integer values
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <T>=0  Var(T)=ndf/(ndf-2)
 
 TF1 cdf("StudentCDF","TMath::StudentI(x,[0])");
 cdf.SetParName(0,"ndf");
 cdf.SetParameter(0,ndf);
 TString title="Student's T CDF (ndf=";
 title+=ndf; 
 title+=");T;CDF for p(T|ndf)"; 
 cdf.SetTitle(title.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::FratioDist(Int_t ndf1,Int_t ndf2) const
{
// Provide the F (ratio) PDF corresponding to the specified ndf1 and ndf2
// degrees of freedom of the two samples.
//
// In a frequentist approach, the F (ratio) distribution is particularly useful
// in making inferences about the ratio of the variances of two underlying
// populations based on a measurement of the variance of a random sample taken
// from each one of the two populations.
// So the F test provides a means to investigate the degree of equality of
// two populations.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <F>=ndf2/(ndf2-2)  Var(F)=2*ndf2*ndf2*(ndf2+ndf1-2)/(ndf1*(ndf2-1)*(ndf2-1)*(ndf2-4))
 
 TF1 pdf("FratioPDF",
 "(TMath::Gamma(([0]+[1])/2.)/(TMath::Gamma([0]/2.)*TMath::Gamma([1]/2.)))*pow([0]/[1],[0]/2.)*pow(x,([0]-2.)/2.)/pow(1.+x*[0]/[1],([0]+[1])/2.)");
 pdf.SetParName(0,"ndf1");
 pdf.SetParameter(0,ndf1);
 pdf.SetParName(1,"ndf2");
 pdf.SetParameter(1,ndf2);
 TString title="F(ratio) PDF (ndf1=";
 title+=ndf1;
 title+=" and ndf2="; 
 title+=ndf2;
 title+=");F;p(F|ndf1,ndf2)";
 pdf.SetTitle(title.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::FratioCDF(Int_t ndf1,Int_t ndf2) const
{
// Provide the F (ratio) cumulative distribution function corresponding to the
// specified ndf1 and ndf2 degrees of freedom of the two samples.
//
// In a frequentist approach, the F (ratio) distribution is particularly useful
// in making inferences about the ratio of the variances of two underlying
// populations based on a measurement of the variance of a random sample taken
// from each one of the two populations.
// So the F test provides a means to investigate the degree of equality of
// two populations.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <F>=ndf2/(ndf2-2)  Var(F)=2*ndf2*ndf2*(ndf2+ndf1-2)/(ndf1*(ndf2-1)*(ndf2-1)*(ndf2-4))
 
 TF1 cdf("FratioCDF","TMath::FDistI(x,[0],[1])");
 cdf.SetParName(0,"ndf1");
 cdf.SetParameter(0,ndf1);
 cdf.SetParName(1,"ndf2");
 cdf.SetParameter(1,ndf2);
 TString title="F(ratio) CDF (ndf1=";
 title+=ndf1;
 title+=" and ndf2="; 
 title+=ndf2;
 title+=");F;CDF for p(F|ndf1,ndf2)";
 cdf.SetTitle(title.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::BinomialDist(Int_t n,Double_t p) const
{
// Provide the Binomial PDF corresponding to the specified number of trials n
// and probability p of success.
//
// p(k|n,p) = probability to obtain exactly k successes in n trials
//            given the probability p of success. 
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <k>=n*p  Var(k)=n*p*(1-p)
 
 TF1 pdf("BinomialPDF","TMath::Binomial(int([0]),int(x))*pow([1],int(x))*pow(1.-[1],int([0])-int(x))");
 pdf.SetParName(0,"n");
 pdf.SetParameter(0,n);
 pdf.SetParName(1,"p");
 pdf.SetParameter(1,p);
 TString title="Binomial PDF for n=";
 title+=n;
 title+=" and p=%-10.3g"; 
 title+=";k;p(k|n,p)";
 TString s=title.Format(title.Data(),p);
 pdf.SetTitle(s.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::BinomialCDF(Int_t n,Double_t p) const
{
// Provide the Binomial cumulative distribution function corresponding to the
// specified number of trials n and probability p of success.
//
// p(k|n,p) = probability to obtain exactly k successes in n trials
//            given the probability p of success. 
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <k>=n*p  Var(k)=n*p*(1-p)
 
 TF1 cdf("BinomialCDF","1.-TMath::BetaIncomplete([1],(x+1.),([0]-x))");
 cdf.SetParName(0,"n");
 cdf.SetParameter(0,n);
 cdf.SetParName(1,"p");
 cdf.SetParameter(1,p);
 TString title="Binomial CDF for n=";
 title+=n;
 title+=" and p=%-10.3g"; 
 title+=";k;CDF for p(k|n,p)";
 TString s=title.Format(title.Data(),p);
 cdf.SetTitle(s.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::NegBinomialnDist(Int_t k,Double_t p) const
{
// Provide the Negative Binomial PDF corresponding to the specified number of
// successes k and probability p of success.
//
// p(n|k,p) = probability for the number of needed trials n to reach k successes
//            given the probability p of success. 
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <n>=k/p  Var(n)=k*(1-p)/(p*p) 
 
 TF1 pdf("NegBinomialnPDF","TMath::Binomial(int(x)-1,int([0])-1)*pow([1],int([0]))*pow(1.-[1],int(x)-int([0]))");
 pdf.SetParName(0,"k");
 pdf.SetParameter(0,k);
 pdf.SetParName(1,"p");
 pdf.SetParameter(1,p);
 TString title="Negative Binomial PDF for k=";
 title+=k;
 title+=" and p=%-10.3g"; 
 title+=";Number of trials n;p(n|k,p)";
 TString s=title.Format(title.Data(),p);
 pdf.SetTitle(s.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::NegBinomialnCDF(Int_t k,Double_t p) const
{
// Provide the Negative Binomial cumulative distribution function corresponding to the
// specified number of successes k and probability p of success.
//
// p(n|k,p) = probability for the number of needed trials n to reach k successes
//            given the probability p of success. 
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <n>=k/p  Var(n)=k*(1-p)/(p*p) 
 
 TF1 cdf("NegBinomialnCDF","TMath::BetaIncomplete([1],[0],x-[0]+1.)");
 cdf.SetParName(0,"k");
 cdf.SetParameter(0,k);
 cdf.SetParName(1,"p");
 cdf.SetParameter(1,p);
 TString title="Negative Binomial CDF for k=";
 title+=k;
 title+=" and p=%-10.3g"; 
 title+=";Number of trials n;CDF for p(n|k,p)";
 TString s=title.Format(title.Data(),p);
 cdf.SetTitle(s.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::NegBinomialxDist(Int_t k,Double_t p) const
{
// Provide the Negative Binomial PDF corresponding to the specified number of
// successes k and probability p of success.
//
// p(x|k,p) = probability for the number of failures x before k successes are reached
//            given the probability p of success. 
//
// Note : In case k=1 the function p(x|1,p) is known as the Geometric PDF.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <x>=k*(1-p)/p  Var(x)=k*(1-p)/(p*p) 
 
 TF1 pdf("NegBinomialxPDF","TMath::Binomial(int(x)+[0]-1,int([0])-1)*pow([1],int([0]))*pow(1.-[1],int(x))");
 pdf.SetParName(0,"k");
 pdf.SetParameter(0,k);
 pdf.SetParName(1,"p");
 pdf.SetParameter(1,p);
 TString title="Negative Binomial PDF for k=";
 title+=k;
 title+=" and p=%-10.3g"; 
 title+=";Number of failures x;p(x|k,p)";
 TString s=title.Format(title.Data(),p);
 pdf.SetTitle(s.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::NegBinomialxCDF(Int_t k,Double_t p) const
{
// Provide the Negative Binomial cumulative distribution function corresponding to the
// specified number of successes k and probability p of success.
//
// p(x|k,p) = probability for the number of failures x before k successes are reached
//            given the probability p of success. 
//
// Note : In case k=1 the function p(x|1,p) is known as the Geometric PDF.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <x>=k*(1-p)/p  Var(x)=k*(1-p)/(p*p) 
 
 TF1 cdf("NegBinomialxCDF","TMath::BetaIncomplete([1],[0],x+1.)");
 cdf.SetParName(0,"k");
 cdf.SetParameter(0,k);
 cdf.SetParName(1,"p");
 cdf.SetParameter(1,p);
 TString title="Negative Binomial CDF for k=";
 title+=k;
 title+=" and p=%-10.3g"; 
 title+=";Number of failures x;CDF for p(x|k,p)";
 TString s=title.Format(title.Data(),p);
 cdf.SetTitle(s.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::PoissonDist(Double_t mu) const
{
// Provide the Poisson PDF p(n|mu).
//
// p(n|mu) = pdf for observing n events given an average number mu 
//           of occurrences in time or space.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <n>=mu  Var(n)=mu
 
 TF1 pdf("PoissPDFmu","exp(-[0])*pow([0],int(x))/TMath::Factorial(int(x))");
 pdf.SetParName(0,"mu");
 pdf.SetParameter(0,mu);
 TString title="Poisson PDF for #mu=%-10.3g";
 title+=";n;p(n|#mu)";
 TString s=title.Format(title.Data(),mu);
 pdf.SetTitle(s.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::PoissonCDF(Double_t mu) const
{
// Provide the Poisson cumulative distribution function for p(n|mu).
//
// p(n|mu) = pdf for observing n events given an average number mu 
//           of occurrences in time or space.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <n>=mu  Var(n)=mu
 
 TF1 cdf("PoissCDFmu","1.-TMath::Gamma(x,[0])");
 cdf.SetParName(0,"mu");
 cdf.SetParameter(0,mu);
 TString title="Poisson PDF for #mu=%-10.3g";
 title+=";n;CDF for p(n|#mu)";
 TString s=title.Format(title.Data(),mu);
 cdf.SetTitle(s.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::PoissonDist(Double_t r,Double_t dt) const
{
// Provide the Poisson PDF p(n|r,dt).
//
// p(n|r,dt) = pdf for observing n events in a certain time or space interval dt
//             given a constant rate r of occurrences.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <n>=rdt  Var(n)=rdt
 
 TF1 pdf("PoissPDFrdt","exp(-[0]*[1])*pow(([0]*[1]),int(x))/TMath::Factorial(int(x))");
 pdf.SetParName(0,"r");
 pdf.SetParameter(0,r);
 pdf.SetParName(1,"dt");
 pdf.SetParameter(1,dt);
 TString title="Poisson PDF for r=%-10.3g";
 title+=" dt=%-10.3g";
 title+=";n;p(n|r,dt)";
 TString s=title.Format(title.Data(),r,dt);
 pdf.SetTitle(s.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::PoissonCDF(Double_t r,Double_t dt) const
{
// Provide the Poisson cumulative distribution function for p(n|r,dt).
//
// p(n|r,dt) = pdf for observing n events in a certain time or space interval dt
//             given a constant rate r of occurrences.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <n>=rdt  Var(n)=rdt
 
 TF1 cdf("PoissCDFrdt","1.-TMath::Gamma(x,[0]*[1])");
 cdf.SetParName(0,"r");
 cdf.SetParameter(0,r);
 cdf.SetParName(1,"dt");
 cdf.SetParameter(1,dt);
 TString title="Poisson CDF for r=%-10.3g";
 title+=" dt=%-10.3g";
 title+=";n;CDF for p(n|r,dt)";
 TString s=title.Format(title.Data(),r,dt);
 cdf.SetTitle(s.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::PoissonDtDist(Double_t r,Int_t n) const
{
// Provide the Poisson related PDF p(dt|r,n).
//
// p(dt|r,n) = pdf for a time or space interval dt in which exactly
//             n events are observed given a constant rate r of occurrences.
//
// This function is also called the Erlang distribution.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <dt>=n/r  Var(dt)=n/(r*r)
 
 TF1 pdf("PoissDtPDF","exp(-[0]*x)*pow(([0]),[1])*pow(x,([1]-1.))/TMath::Factorial(int([1]-1.))");
 pdf.SetParName(0,"r");
 pdf.SetParameter(0,r);
 pdf.SetParName(1,"n");
 pdf.SetParameter(1,n);
 TString title="Poisson related dt PDF for n=";
 title+=n;
 title+=" and r=%-10.3g";
 title+=";dt;p(dt|r,n)";
 TString s=title.Format(title.Data(),r);
 pdf.SetTitle(s.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::PoissonDtCDF(Double_t r,Int_t n) const
{
// Provide the cumulative distribution for the Poisson related pdf p(dt|r,n).
//
// p(dt|r,n) = pdf for a time or space interval dt in which exactly
//             n events are observed given a constant rate r of occurrences.
//
// The function p(dt|r,n) is also called the Erlang distribution.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <dt>=n/r  Var(dt)=n/(r*r)
 
 TF1 cdf("PoissDtCDF","TMath::Gamma([1],[0]*x)");
 cdf.SetParName(0,"r");
 cdf.SetParameter(0,r);
 cdf.SetParName(1,"n");
 cdf.SetParameter(1,n);
 TString title="Poisson related dt CDF for n=";
 title+=n;
 title+=" and r=%-10.3g";
 title+=";dt;CDF for p(dt|r,n)";
 TString s=title.Format(title.Data(),r);
 cdf.SetTitle(s.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::GammaDtDist(Double_t r,Double_t z) const
{
// Provide the Gamma function related PDF p(dt|r,z).
//
// p(dt|r,z) = pdf for a time or space interval dt in which exactly
//             z occurrences are observed given a constant rate r.
//
// Note : In case z is a positive integer the user is referred to the member function
//        PoissonDtDist(), aka the Erlang distribution.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <dt>=z/r  Var(dt)=z/(r*r)
 
 TF1 pdf("GammaDtPDF","exp(-[0]*x)*pow([0],[1])*pow(x,([1]-1.))/TMath::Gamma([1])");
 pdf.SetParName(0,"r");
 pdf.SetParameter(0,r);
 pdf.SetParName(1,"z");
 pdf.SetParameter(1,z);
 TString title="Gamma related dt PDF for r=%-10.3g";
 title+=" z=%-10.3g";
 title+=";dt;p(dt|r,z)";
 TString s=title.Format(title.Data(),r,z);
 pdf.SetTitle(s.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::GaussDist(Double_t mu,Double_t sigma) const
{
// Provide the Gaussian PDF p(x|mu,sigma).
//
// p(x|mu,sigma) = pdf for obtaining a value x given a mean value mu 
//                 and a standard deviation sigma.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <x>=mu  Var(x)=sigma*sigma
 
 TF1 pdf("GaussPDF","TMath::Gaus(x,[0],[1],1)");
 pdf.SetParName(0,"mu");
 pdf.SetParameter(0,mu);
 pdf.SetParName(1,"sigma");
 pdf.SetParameter(1,sigma);
 TString title="Gaussian PDF for #mu=%-10.3g";
 title+=" #sigma=%-10.3g";
 title+=";x;p(x|#mu,#sigma)";
 TString s=title.Format(title.Data(),mu,sigma);
 pdf.SetTitle(s.Data());

 return pdf;
}
///////////////////////////////////////////////////////////////////////////
TF1 NcMath::GaussCDF(Double_t mu,Double_t sigma) const
{
// Provide the cumulative distribution function for the Gaussian p(x|mu,sigma).
//
// p(x|mu,sigma) = pdf for obtaining a value x given a mean value mu 
//                 and a standard deviation sigma.
//
// Details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <x>=mu  Var(x)=sigma*sigma
 
 TF1 cdf("GaussCDF","0.5*(1.+TMath::Erf((x-[0])/([1]*sqrt(2.))))");
 cdf.SetParName(0,"mu");
 cdf.SetParameter(0,mu);
 cdf.SetParName(1,"sigma");
 cdf.SetParameter(1,sigma);
 TString title="Gaussian CDF for #mu=%-10.3g";
 title+=" #sigma=%-10.3g";
 title+=";x;CDF for p(x|#mu,#sigma)";
 TString s=title.Format(title.Data(),mu,sigma);
 cdf.SetTitle(s.Data());

 return cdf;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::GaussProb(Double_t q,Double_t mean,Double_t sigma,Int_t isig) const
{
// Computation of the integrated probability P(|x-mean|<=dist) for a
// normalised Gaussian pdf, characterised by the "mean" and "sigma".
//
// The argument "isig" allows for different specifications of "dist".
//
// isig = 0 : dist=|q-mean|.
//        1 : dist=|q*sigma|.
//  
// The default values are : mean=0, sigma=1 and isig=0.
//
// In case of inconsistent input, a value of -1 is returned.
//
//--- NvE 27-nov-2008 NCFS

 Double_t val=-1;
 if (!isig)
 {
  val=Erf(fabs(q-mean)/(sigma*sqrt(2.)));
 }
 else
 {
  val=Erf(fabs(q)/sqrt(2.));
 }
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::GaussPvalue(Double_t q,Double_t mean,Double_t sigma,Int_t sides,Int_t isig) const
{
// Computation of the P-value of "q" w.r.t. a normalised Gaussian pdf,
// characterised by the "mean" and "sigma".
//
// The P-value for a certain value "q" corresponds to the integrated probability
// to obtain a value x which lies at least as far from the mean as "q".
//
// In view of the symmetry of the Gaussian, one distinguishes between a so called
// "double-sided" and "single-sided" P-value.
//
// Double-sided : P-value=P(|x-mean|>=|q-mean|)
// Single-sided : if x>=mean ==> P-value=P(x-mean>=|q-mean|)
//                if x<=mean ==> P-value=P(mean-x>=|q-mean|)
// 
// With the "sides" parameter a single-sided or double-sided P-value can be selected.
//
// sides = 1 : Single-sided P-value.
//         2 : Double-sided P-value.
//
// The argument "isig" allows for the specification of "q" in units of sigma
// or for the return value to represent (q-mean) expressed as a (fractional) number of sigma.
//
// isig = 0 : "q" represents a regular real number; the P-value will be returned.
//        1 : "q" represents a certain (fractional) amount of sigma; the P-value will be returned.
//       -1 : "q" represents a regular number; the (q-mean) will be returned in units of sigma.
//            Note : In this case the returned value may be negative.
//
// The default values are mean=0, sigma=1, sides=2 and isig=0.
//
// In case of inconsistent input, a value of -1 is returned.
//
//--- NvE 21-may-2005 Utrecht University

 Double_t val=-1;
 if (!isig)
 {
  val=Erfc(fabs(q-mean)/(sigma*sqrt(2.)));
 }
 else
 {
  if (isig==1) val=Erfc(fabs(q)/sqrt(2.));
  if (isig==-1) val=(q-mean)/sigma;
 }
 if (sides==1 && isig!=-1) val/=2.;
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Chi2Pvalue(Double_t chi2,Int_t ndf,Int_t sides,Int_t sigma,Int_t mode) const
{
// Computation of the P-value for a certain specified Chi-squared (chi2) value 
// for a Chi-squared distribution with ndf degrees of freedom.
//
// The P-value for a certain Chi-squared value chi2 corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield a Chi-squared value at least (at most) the value chi2 for an
// upper (lower) tail test in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <Chi2>=ndf  Var(Chi2)=2*ndf
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input chi2 value w.r.t. <Chi2> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference chi2-<Chi2> expressed in units of sigma
//             Note : This difference may be negative.
//  
// According to the value of the parameter "mode" various algorithms
// can be selected.
//
// mode = 0 : Calculations are based on the incomplete gamma function.
//
//        1 : Same as for mode=0. However, in case ndf=1 an exact expression
//            based on the error function Erf() is used.
//
//        2 : Same as for mode=0. However, in case ndf>30 a Gaussian approximation
//            is used instead of the gamma function.
//
// The default values are sides=0, sigma=0 and mode=1.
//
//--- NvE 21-may-2005 Utrecht University

 if (ndf<=0) return 0;

 Double_t mean=ndf;

 if (!sides) // Automatic one-sided test
 {
  sides=1;
  if (chi2<mean) sides=-1;
 }

 if (sides==3) // Automatic two-sided test
 {
  sides=2;
  if (chi2<mean) sides=-2;
 }

 Double_t val=0;
 if (sigma) // P-value in units of sigma
 {
  Double_t s=sqrt(double(2*ndf));
  val=(chi2-mean)/s;
 }
 else // P-value from tail contents
 {
  if (sides>0) // Upper tail
  { 
   val=Prob(chi2,ndf,mode);
  }
  else // Lower tail
  {
   val=1.-Prob(chi2,ndf,mode);
  }
 }

 if (abs(sides)==2) val=val*2.;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::StudentPvalue(Double_t t,Double_t ndf,Int_t sides,Int_t sigma) const
{
// Computation of the P-value for a certain specified Student's t value 
// for a Student's T distribution with ndf degrees of freedom.
//
// In a frequentist approach, the Student's T distribution is particularly
// useful in making inferences about the mean of an underlying population
// based on the data from a random sample.
//
// The P-value for a certain t value corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield a T value at least (at most) the value t for an upper (lower)
// tail test in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <T>=0  Var(T)=ndf/(ndf-2)
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input t value w.r.t. <T> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference t-<T> expressed in units of sigma
//             Note : This difference may be negative and sigma
//                    is only defined for ndf>2.
//  
// The default values are sides=0 and sigma=0.
//
//--- NvE 21-may-2005 Utrecht University

 if (ndf<=0) return 0;

 Double_t mean=0;

 if (!sides) // Automatic one-sided test
 {
  sides=1;
  if (t<mean) sides=-1;
 }

 if (sides==3) // Automatic two-sided test
 {
  sides=2;
  if (t<mean) sides=-2;
 }

 Double_t val=0;
 if (sigma) // P-value in units of sigma
 { 
  if (ndf>2) // Sigma is only defined for ndf>2
  {
   Double_t s=sqrt(ndf/(ndf-2.));
   val=t/s;
  }
 }
 else // P-value from tail contents
 {
  if (sides>0) // Upper tail
  {
   val=1.-TMath::StudentI(t,ndf);
  }
  else // Lower tail
  {
   val=TMath::StudentI(t,ndf);
  }
 }

 if (abs(sides)==2) val=val*2.;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::FratioPvalue(Double_t f,Int_t ndf1,Int_t ndf2,Int_t sides,Int_t sigma) const
{
// Computation of the P-value for a certain specified F ratio f value 
// for an F (ratio) distribution with ndf1 and ndf2 degrees of freedom
// for the two samples X,Y respectively to be compared in the ratio X/Y.
//
// In a frequentist approach, the F (ratio) distribution is particularly useful
// in making inferences about the ratio of the variances of two underlying
// populations based on a measurement of the variance of a random sample taken
// from each one of the two populations.
// So the F test provides a means to investigate the degree of equality of
// two populations.
//
// The P-value for a certain f value corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield an F value at least (at most) the value f for an upper (lower)
// tail test in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <F>=ndf2/(ndf2-2)  Var(F)=2*ndf2*ndf2*(ndf2+ndf1-2)/(ndf1*(ndf2-1)*(ndf2-1)*(ndf2-4))
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input f value w.r.t. <F> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference f-<F> expressed in units of sigma
//             Note : This difference may be negative and sigma
//                    is only defined for ndf2>4.
//  
// The default values are sides=0 and sigma=0.
//
//--- NvE 21-may-2005 Utrecht University

 if (ndf1<=0 || ndf2<=0 || f<=0) return 0;

 Double_t mean=double(ndf2/(ndf2-2));

 if (!sides) // Automatic one-sided test
 {
  sides=1;
  if (f<mean) sides=-1;
 }

 if (sides==3) // Automatic two-sided test
 {
  sides=2;
  if (f<mean) sides=-2;
 }

 Double_t val=0;
 if (sigma) // P-value in units of sigma
 { 
  // Sigma is only defined for ndf2>4
  if (ndf2>4)
  {
   Double_t s=sqrt(double(ndf2*ndf2*(2*ndf2+2*ndf1-4))/double(ndf1*pow(double(ndf2-1),2)*(ndf2-4)));
   val=(f-mean)/s;
  }
 }
 else // P-value from tail contents 
 {
  if (sides>0) // Upper tail
  {
   val=1.-TMath::FDistI(f,ndf1,ndf2);
  }
  else // Lower tail
  {
   val=TMath::FDistI(f,ndf1,ndf2);
  }
 }

 if (abs(sides)==2) val=val*2.;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::BinomialPvalue(Int_t k,Int_t n,Double_t p,Int_t sides,Int_t sigma,Int_t mode) const
{
// Computation of the P-value for a certain specified number of successes k
// for a Binomial distribution with n trials and success probability p.
//
// The P-value for a certain number of successes k corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield a number of successes at least (at most) the value k for an
// upper (lower) tail test in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <K>=n*p  Var(K)=n*p*(1-p)
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input k value w.r.t. <K> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference k-<K> expressed in units of sigma
//             Note : This difference may be negative.
//
// mode = 0 : Incomplete Beta function will be used to calculate the tail content.
//        1 : Straightforward summation of the Binomial terms is used.
//
// The Incomplete Beta function in general provides the most accurate values.
//
// The default values are sides=0, sigma=0 and mode=0.
//
//--- NvE 24-may-2005 Utrecht University

 Double_t mean=double(n)*p;

 if (!sides) // Automatic one-sided test
 {
  sides=1;
  if (k<mean) sides=-1;
 }

 if (sides==3) // Automatic two-sided test
 {
  sides=2;
  if (k<mean) sides=-2;
 }

 Double_t val=0;

 if (sigma) // P-value in units of sigma
 {
  Double_t s=sqrt(double(n)*p*(1.-p));
  val=(double(k)-mean)/s;
 }
 else // P-value from tail contents
 {
  if (sides>0) // Upper tail
  {
   if (!mode) // Use Incomplete Beta function
   {
    val=TMath::BetaIncomplete(p,k,n-k+1);
   }
   else // Use straightforward summation
   {
    for (Int_t i=k; i<=n; i++)
    {
     val+=TMath::Binomial(n,i)*pow(p,i)*pow(1.-p,n-i);
    }
   }
  }
  else // Lower tail
  {
   if (!mode) // Use Incomplete Beta function
   {
    val=1.-TMath::BetaIncomplete(p,k+1,n-k);
   }
   else
   {
    for (Int_t j=0; j<=k; j++)
    {
     val+=TMath::Binomial(n,j)*pow(p,j)*pow(1.-p,n-j);
    }
   }
  }
 }

 if (abs(sides)==2) val=val*2.;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PoissonPvalue(Int_t k,Double_t mu,Int_t sides,Int_t sigma) const
{
// Computation of the P-value for a certain specified number of occurrences k
// for a Poisson distribution with a given average number (in time or space)
// of mu occurrences.
//
// The P-value for a certain number of occurrences k corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield a number of occurrences at least (at most) the value k for an
// upper (lower) tail test in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <K>=mu  Var(K)=mu
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input k value w.r.t. <K> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference k-<K> expressed in units of sigma
//             Note : This difference may be negative.
//
// The default values are sides=0 and sigma=0.
//
// Note : The tail contents are given by the incomplete Gamma function P(a,x).
//        Lower tail contents = 1-P(k,mu)
//        Upper tail contents = P(k,mu)
//
//--- NvE 24-may-2005 Utrecht University

 Double_t mean=mu;

 if (!sides) // Automatic one-sided test
 {
  sides=1;
  if (k<mean) sides=-1;
 }

 if (sides==3) // Automatic two-sided test
 {
  sides=2;
  if (k<mean) sides=-2;
 }

 Double_t val=0;

 if (sigma) // P-value in units of sigma
 {
  Double_t s=sqrt(mu);
  val=(double(k)-mean)/s;
 }
 else // P-value from tail contents
 {
  if (sides>0) // Upper tail
  {
   val=Gamma(k-1,mu);
  }
  else // Lower tail
  {
   val=1.-Gamma(k,mu);
  }
 }

 if (abs(sides)==2) val=val*2.;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PoissonPvalue(Int_t k,Double_t r,Double_t dt,Int_t sides,Int_t sigma) const
{
// Computation of the P-value for a certain specified number of occurrences k
// for a Poisson distribution with a given average rate r (in time or space)
// of occurrences and a (time or space) interval dt.
//
// The P-value for a certain number of occurrences k corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield a number of occurrences at least (at most) the value k for an
// upper (lower) tail test in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <K>=rdt  Var(K)=rdt
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input k value w.r.t. <K> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference k-<K> expressed in units of sigma
//             Note : This difference may be negative.
//
// The default values are sides=0 and sigma=0.
//
// Note : The tail contents are given by the incomplete Gamma function P(a,x).
//        Lower tail contents = 1-P(k,mu)
//        Upper tail contents = P(k,mu)
//
//--- NvE 14-oct-2014 IIHE-VUB, Brussel

 Double_t mu=r*dt;
 Double_t val=PoissonPvalue(k,mu,sides,sigma);
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PoissonDtPvalue(Double_t dt,Double_t r,Int_t n,Int_t sides,Int_t sigma) const
{
// Computation of the P-value for a certain specified time (or space) interval dt
// for a Poisson related distribution with a given average rate r (in time or space)
// of occurrences and an observed number n of events.
//
// The P-value for a certain time (or space) interval dt corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield an interval at least (at most) the value dt for an upper (lower) tail test
// in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
//
// Note : <dt>=n/r  Var(K)=n/(r*r)
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input dt value w.r.t. <dt> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference dt-<dt> expressed in units of sigma
//             Note : This difference may be negative.
//
// The default values are sides=0 and sigma=0.
//
// Note : The tail contents are given by the incomplete Gamma function P(a,x).
//        Lower tail content = P(n,r*dt)
//        Upper tail content = 1-P(n,r*dt)
//
// In case of inconsistent input the value -1 is returned.
//
//--- NvE 15-oct-2014 IIHE-VUB, Brussel
 
 Double_t val=-1;

 if (n<=0 || r<=0) return val;

 Double_t mean=double(n)/r;
 Double_t sig=sqrt(double(n)/(r*r));

 if (!sides) // Automatic one-sided test
 {
  sides=1;
  if (dt<mean) sides=-1;
 }

 if (sides==3) // Automatic two-sided test
 {
  sides=2;
  if (dt<mean) sides=-2;
 }

 val=0;

 if (sigma) // P-value in units of sigma
 {
  val=(dt-mean)/sig;
 }
 else // P-value from tail contents
 {
  if (sides>0) // Upper tail
  {
   val=1.-Gamma(n,r*dt);
  }
  else // Lower tail
  {
   val=Gamma(n,r*dt);
  }
 }

 if (abs(sides)==2) val=val*2.;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::NegBinomialnPvalue(Int_t n,Int_t k,Double_t p,Int_t sides,Int_t sigma,Int_t mode) const
{
// Computation of the P-value for a certain specified number of trials n
// for a Negative Binomial distribution where exactly k successes are to
// be reached which have each a probability p.
//
// p(N|k,p) = probability for the number of needed trials N to reach k successes
//            given the probability p of success. 
//
// Note : <N>=k/p  Var(N)=k*(1-p)/(p*p) 
//
// The P-value for a certain number of trials n corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield a number of trials at least (at most) the value n for an
// upper (lower) tail test in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input n value w.r.t. <N> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference n-<N> expressed in units of sigma
//             Note : This difference may be negative.
//
// mode = 0 : Incomplete Beta function will be used to calculate the tail content.
//        1 : Straightforward summation of the Negative Binomial terms is used.
//
// The Incomplete Beta function in general provides the most accurate values.
//
// The default values are sides=0, sigma=0 and mode=0.
//
//--- NvE 24-may-2005 Utrecht University

 Double_t mean=double(k)/p;

 if (!sides) // Automatic one-sided test
 {
  sides=1;
  if (n<mean) sides=-1;
 }

 if (sides==3) // Automatic two-sided test
 {
  sides=2;
  if (n<mean) sides=-2;
 }

 Double_t val=0;

 if (sigma) // P-value in units of sigma
 {
  Double_t s=sqrt(double(k)*(1.-p)/(p*p));
  val=(double(n)-mean)/s;
 }
 else // P-value from tail contents
 {
  if (sides>0) // Upper tail
  {
   if (!mode) // Use Incomplete Beta function
   {
    TF1 cdf=NegBinomialnCDF(k,p);
    val=cdf.Eval(double(n-1));
   }
   else // Use straightforward summation
   {
    val=0;
    for (Int_t i=1; i<n; i++)
    {
     val+=TMath::Binomial(i-1,k-1)*pow(p,k)*pow(1.-p,i-k);
    }
   }
   val=1.-val;
  }
  else // Lower tail
  {
   if (!mode) // Use Incomplete Beta function
   {
    TF1 cdf=NegBinomialnCDF(k,p);
    val=cdf.Eval(double(n));
   }
   else // Use straightforward summation
   {
    val=0;
    for (Int_t j=1; j<=n; j++)
    {
     val+=TMath::Binomial(j-1,k-1)*pow(p,k)*pow(1.-p,j-k);
    }
   }
  }
 }

 if (abs(sides)==2) val=val*2.;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::NegBinomialxPvalue(Int_t x,Int_t k,Double_t p,Int_t sides,Int_t sigma,Int_t mode) const
{
// Computation of the P-value for a certain specified number of failures x
// for a Negative Binomial distribution where exactly k successes are to
// be reached which have each a probability p.
//
// p(X|k,p) = probability for the number of failures X before k successes are reached
//            given the probability p of success. 
//
// In case k=1 the function p(X|1,p) is known as the Geometric PDF.
//
// Note : <X>=k*(1-p)/p  Var(X)=k*(1-p)/(p*p) 
//
// The P-value for a certain number of failures x corresponds to the fraction of
// repeatedly drawn equivalent samples from a certain population, which is expected
// to yield a number of failures at least (at most) the value x for an
// upper (lower) tail test in case a certain hypothesis is true.
//
// Further details can be found in the excellent textbook of Phil Gregory
// "Bayesian Logical Data Analysis for the Physical Sciences".
// 
// With the "sides" parameter a one-sided or two-sided test can be selected
// using either the upper or lower tail contents.
// In case of automatic upper/lower selection the decision is made on basis
// of the location of the input x value w.r.t. <X> of the distribution. 
//
// sides =  1 : One-sided test using the upper tail contents
//          2 : Two-sided test using the upper tail contents
//         -1 : One-sided test using the lower tail contents
//         -2 : Two-sided test using the lower tail contents
//          0 : One-sided test using the auto-selected upper or lower tail contents
//          3 : Two-sided test using the auto-selected upper or lower tail contents
//
// The argument "sigma" allows for the following return values :
//
// sigma = 0 : P-value is returned as the above specified fraction
//         1 : The difference x-<X> expressed in units of sigma
//             Note : This difference may be negative.
//
// mode = 0 : Incomplete Beta function will be used to calculate the tail content.
//        1 : Straightforward summation of the Negative Binomial terms is used.
//
// The Incomplete Beta function in general provides the most accurate values.
//
// The default values are sides=0, sigma=0 and mode=0.
//
//--- NvE 24-may-2005 Utrecht University

 Double_t mean=double(x)*(1.-p)/p;

 if (!sides) // Automatic one-sided test
 {
  sides=1;
  if (x<mean) sides=-1;
 }

 if (sides==3) // Automatic two-sided test
 {
  sides=2;
  if (x<mean) sides=-2;
 }

 Double_t val=0;

 if (sigma) // P-value in units of sigma
 {
  Double_t s=sqrt(double(k)*(1.-p)/(p*p));
  val=(double(x)-mean)/s;
 }
 else // P-value from tail contents
 {
  if (sides>0) // Upper tail
  {
   if (!mode) // Use Incomplete Beta function
   {
    TF1 cdf=NegBinomialxCDF(k,p);
    val=cdf.Eval(double(x-1));
   }
   else // Use straightforward summation
   {
    val=0;
    for (Int_t i=0; i<x; i++)
    {
     val+=TMath::Binomial(i+k-1,k-1)*pow(p,k)*pow(1.-p,i);
    }
   }
   val=1.-val;
  }
  else // Lower tail
  {
   if (!mode) // Use Incomplete Beta function
   {
    TF1 cdf=NegBinomialxCDF(k,p);
    val=cdf.Eval(double(x));
   }
   else // Use straightforward summation
   {
    val=0;
    for (Int_t j=0; j<=x; j++)
    {
     val+=TMath::Binomial(j+k-1,k-1)*pow(p,k)*pow(1.-p,j);
    }
   }
  }
 }

 if (abs(sides)==2) val=val*2.;

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Log(Double_t B,Double_t x) const
{
// Compute log_B(x) with base B.
//
// In case of inconsistent input the value 0 is returned.

 Double_t val=0;

 if (B<=1 || x<=0) return val;

 val=log(x)/log(B);
 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Nfac(Int_t n,Int_t mode) const
{
// Compute n!.
// The algorithm can be selected by the "mode" input argument.
//
// mode : 0 ==> Calculation by means of straightforward multiplication
//      : 1 ==> Calculation by means of Stirling's approximation
//      : 2 ==> Calculation by means of n!=Gamma(n+1)
//
// For large n the calculation modes 1 and 2 will in general be faster.
// By default mode=0 is used.
// For n<0 the value 0 will be returned.
//
// Note : Because of Double_t value overflow the maximum value is n=170.
//
//--- NvE 20-jan-2007 Utrecht University

 if (n<0) return 0;
 if (n==0 || n==1) return 1;

 Double_t twopi=2.*acos(-1.);
 Double_t z=0;
 Double_t nfac=1;
 Int_t i=n;
 
 switch (mode)
 {
  case 0: // Straightforward multiplication
   while (i>1)
   {
    nfac*=Double_t(i);
    i--;
   }
   break;

  case 1: // Stirling's approximation 
   z=n;
   nfac=sqrt(twopi)*pow(z,z+0.5)*exp(-z)*(1.+1./(12.*z));
   break;

  case 2: // Use of Gamma(n+1)
   z=n+1;
   nfac=Gamma(z);
   break;

  default:
   nfac=0;
   break;
 }

 return nfac;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::LnNfac(Int_t n,Int_t mode) const
{
// Compute ln(n!).
// The algorithm can be selected by the "mode" input argument.
//
// mode : 0 ==>  Calculation via evaluation of n! followed by taking ln(n!)
//      : 1 ==>  Calculation via Stirling's approximation ln(n!)=0.5*ln(2*pi)+(n+0.5)*ln(n)-n+1/(12*n)
//      : 2 ==>  Calculation by means of ln(n!)=LnGamma(n+1)
//
// Note : Because of Double_t value overflow the maximum value is n=170 for mode=0.
//
// For mode=2 rather accurate results are obtained for both small and large n.
// By default mode=2 is used.
// For n<1 the value 0 will be returned.
//
//--- NvE 20-jan-2007 Utrecht University

 if (n<=1) return 0;

 Double_t twopi=2.*acos(-1.);
 Double_t z=0;
 Double_t lognfac=0;
 
 switch (mode)
 {
  case 0: // Straightforward ln(n!)
   z=Nfac(n);
   lognfac=log(z);
   break;

  case 1: // Stirling's approximation 
   z=n;
   lognfac=0.5*log(twopi)+(z+0.5)*log(z)-z+1./(12.*z);
   break;

  case 2: // Use of LnGamma(n+1)
   z=n+1;
   lognfac=LnGamma(z);
   break;

  default:
   lognfac=0;
   break;
 }

 return lognfac;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::LogNfac(Int_t n,Int_t mode) const
{
// Compute log_10(n!).
// First ln(n!) is evaluated via invokation of LnNfac(n,mode).
// Then the algorithm log_10(z)=ln(z)*log_10(e) is used.
//
// For n<1 the value 0 will be returned.
//
//--- NvE 20-jan-2007 Utrecht University

 if (n<=1) return 0;

 Double_t e=exp(1.);

 Double_t val=LnNfac(n,mode);
 val*=log10(e);

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Rfac(Double_t r) const
{
// Compute r! for a fractional value r.
// The algorithm used is : r!=Gamma(r+1).
//
// For r<0 the value 0 will be returned.
//
// Note : Because of Double_t value overflow the maximum value is about r=170.
//
//--- NvE 17-jul-2008 Utrecht University

 if (r<0) return 0;
 if (r==0 || r==1) return 1;

 Double_t z=r+1.;
 return Gamma(z);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::LnRfac(Double_t r) const
{
// Compute ln(r!) for a fractional value r.
// The algorithm used is : ln(r!)=LnGamma(r+1).
//
// For r<0 the value 0 will be returned.
//
//--- NvE 17-jul-2008 Utrecht University

 if (r<=0 || r==1) return 0;

 Double_t z=r+1.;
 return LnGamma(z);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::LogRfac(Double_t r) const
{
// Compute log_10(r!) for a fractional value r.
// First ln(r!) is evaluated via invokation of LnRfac(r).
// Then the algorithm log_10(z)=ln(z)*log_10(e) is used.
//
// For r<0 the value 0 will be returned.
//
//--- NvE 17-jul-2008 Utrecht University

 if (r<=0 || r==1) return 0;

 Double_t e=exp(1.);

 Double_t val=LnRfac(r);
 val*=log10(e);

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PsiValue(Int_t m,Int_t* n,Double_t* p,Int_t f) const
{
// Provide the Bayesian Psi value of observations of a counting experiment
// w.r.t. a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
//
// The Psi value provides (in dB scale) the amount of support that the
// data can maximally give to any Bernoulli class hypothesis different
// from the currently specified B_m.
//
// To be specific : Psi=-10*log[p(D|B_m I)]
//
// where p(D|B_m I) represents the likelihood of the data D under the condition
// that B_m and some prior I are true.
//
// A Psi value of zero indicates a perfect match between the observations
// and the specified hypothesis.
// Further mathematical details can be found in astro-ph/0702029.
//
// m : The number of different possible outcomes of the counting experiment
// n : The observed numbers of occurrences of the different outcomes
// p : The probabilities of the different outcomes according to the hypothesis
// f : Flag to indicate the use of a frequentist (Stirling) approximation (f=1)
//     or the exact Bayesian expression (f=0).
//
// Note : Both the arrays "n" and (when provided) "p" should be of dimension "m".
//
// In case no probabilities are given (i.e. p=0), a uniform distribution
// is assumed.
//
// The default values are p=0 and f=0.
//
// In the case of inconsistent input, a Psi value of -1 is returned.
//
//--- NvE 03-oct-2007 Utrecht University

 Double_t psi=-1;

 if (m<=0 || !n) return psi;

 Int_t ntot=0;
 for (Int_t j=0; j<m; j++)
 {
  if (n[j]>0) ntot+=n[j];
 }

 psi=0;
 Double_t pk=1./float(m); // Prob. of getting outcome k for a uniform distr.
 for (Int_t i=0; i<m; i++)
 {
  if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
  if (n[i]>0 && pk>0)
  {
   if (!f) // Exact Bayesian expression
   {
    psi+=double(n[i])*log10(pk)-LogNfac(n[i]);
   }
   else // Frequentist (Stirling) approximation
   {
    if (ntot>0) psi+=double(n[i])*log10(double(n[i])/(double(ntot)*pk));
   }
  }
 }

 if (!f) // Exact Bayesian expression
 {
  psi+=LogNfac(ntot);
  psi*=-10.;
 }
 else // Frequentist (Stirling) approximation
 {
  psi*=10.;
 }

 return psi;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PsiValue(Int_t m,Double_t* n,Double_t* p,Int_t f) const
{
// Provide the Bayesian Psi value of observations of a counting experiment
// w.r.t. a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
//
// Note : The observed numbers of occurrences of the different outcomes
//        may be fractional numbers for this memberfunction.
//        This mainly serves to investigate predicted background matches
//        via histogram input.
//
// The Psi value provides (in dB scale) the amount of support that the
// data can maximally give to any Bernoulli class hypothesis different
// from the currently specified B_m.
//
// To be specific : Psi=-10*log[p(D|B_m I)]
//
// where p(D|B_m I) represents the likelihood of the data D under the condition
// that B_m and some prior I are true.
//
// A Psi value of zero indicates a perfect match between the observations
// and the specified hypothesis.
// Further mathematical details can be found in astro-ph/0702029.
//
// m : The number of different possible outcomes of the counting experiment
// n : The observed numbers of occurrences of the different outcomes
// p : The probabilities of the different outcomes according to the hypothesis
// f : Flag to indicate the use of a frequentist (Stirling) approximation (f=1)
//     or the exact Bayesian expression (f=0).
//
// Note : Both the arrays "n" and (when provided) "p" should be of dimension "m".
//
// In case no probabilities are given (i.e. p=0), a uniform distribution
// is assumed.
//
// The default values are p=0 and f=0.
//
// In the case of inconsistent input, a Psi value of -1 is returned.
//
//--- NvE 25-jul-2008 Utrecht University

 Double_t psi=-1;

 if (m<=0 || !n) return psi;

 Double_t ntot=0;
 for (Int_t j=0; j<m; j++)
 {
  if (n[j]>0) ntot+=n[j];
 }

 psi=0;
 Double_t pk=1./float(m); // Prob. of getting outcome k for a uniform distr.
 for (Int_t i=0; i<m; i++)
 {
  if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
  if (n[i]>0 && pk>0)
  {
   if (!f) // Exact Bayesian expression
   {
    psi+=n[i]*log10(pk)-LogRfac(n[i]);
   }
   else // Frequentist (Stirling) approximation
   {
    if (ntot>0) psi+=n[i]*log10(n[i]/(ntot*pk));
   }
  }
 }

 if (!f) // Exact Bayesian expression
 {
  psi+=LogRfac(ntot);
  psi*=-10.;
 }
 else // Frequentist (Stirling) approximation
 {
  psi*=10.;
 }

 return psi;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PsiValue(TH1* his,TH1* hyp,TF1* pdf,Int_t f) const
{
// Provide the Bayesian Psi value of observations of a counting experiment
// (in histogram format) w.r.t. a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
//
// The specification of a hypothesis B_m can be provided either in
// histogram format (hyp) or via a probability distribution function (pdf),
// as outlined below.
// Note : The histogram "hyp" or the function "pdf" do not need to be normalised.
//
// The Psi value provides (in dB scale) the amount of support that the
// data can maximally give to any Bernoulli class hypothesis different
// from the currently specified B_m.
//
// To be specific : Psi=-10*log[p(D|B_m I)]
//
// where p(D|B_m I) represents the likelihood of the data D under the condition
// that B_m and some prior I are true.
//
// A Psi value of zero indicates a perfect match between the observations
// and the specified hypothesis.
// Further mathematical details can be found in astro-ph/0702029.
//
// his : The experimental observations in histogram format
//       Note that Underflow and/or Overflow entries are not taken into account
// hyp : Hypothetical observations according to some hypothesis
// pdf : Probability distribution function for the hypothesis
// f   : Flag to indicate the use of a frequentist (Stirling) approximation (f=1)
//       or the exact Bayesian expression (f=0).
//
// In case no hypothesis is specified (i.e. hyp=0 and pdf=0), a uniform
// background distribution is assumed.
//
// Default values are : hyp=0, pdf=0 and f=0.
//
// In the case of inconsistent input, a Psi value of -1 is returned.
//
//--- NvE 03-oct-2007 Utrecht University

 Double_t psi=-1;

 if (!his) return psi;

 TAxis* xaxis=his->GetXaxis();
 Double_t xmin=xaxis->GetXmin();
 Double_t xmax=xaxis->GetXmax();
 Double_t range=xmax-xmin;
 Int_t nbins=his->GetNbinsX();
 Double_t nensig=his->GetSumOfWeights();

 if (nbins<=0 || nensig<=0 || range<=0) return psi;

 Double_t* n=new Double_t[nbins];
 Double_t* p=new Double_t[nbins]; 
 Double_t nk;
 Double_t pk;

 // Uniform hypothesis distribution
 if (!hyp && !pdf)
 {
  for (Int_t i=1; i<=nbins; i++)
  {
   nk=his->GetBinContent(i);
   pk=his->GetBinWidth(i)/range;
   n[i-1]=0;
   p[i-1]=0;
   if (nk>0) n[i-1]=nk;
   if (pk>0) p[i-1]=pk;
  }
  psi=PsiValue(nbins,n,p,f);
 }

 // Hypothesis specified via a pdf
 if (pdf && !hyp)
 {
  pdf->SetRange(xmin,xmax);
  Double_t ftot=pdf->Integral(xmin,xmax);
  if (ftot>0)
  {
   Double_t x1,x2;
   for (Int_t ipdf=1; ipdf<=nbins; ipdf++)
   {
    nk=his->GetBinContent(ipdf);
    x1=his->GetBinLowEdge(ipdf);
    x2=x1+his->GetBinWidth(ipdf);
    pk=pdf->Integral(x1,x2)/ftot;
    n[ipdf-1]=0;
    p[ipdf-1]=0;
    if (nk>0) n[ipdf-1]=nk;
    if (pk>0) p[ipdf-1]=pk;
   }
   psi=PsiValue(nbins,n,p,f);
  }
 }

 // Hypothesis specified via a histogram
 if (hyp && !pdf)
 {
  TH1* href=(TH1*)his->Clone("href");
  href->Reset();
  Double_t nenhyp=0;
  Double_t x,y;
  for (Int_t ihyp=1; ihyp<=hyp->GetNbinsX(); ihyp++)
  {
   x=hyp->GetBinCenter(ihyp);
   y=hyp->GetBinContent(ihyp);
   href->Fill(x,y);
   nenhyp+=y;
  }
  for (Int_t j=1; j<=nbins; j++)
  {
   nk=his->GetBinContent(j);
   pk=href->GetBinContent(j)/nenhyp;
   n[j-1]=0;
   p[j-1]=0;
   if (nk>0) n[j-1]=nk;
   if (pk>0) p[j-1]=pk;
  }
  psi=PsiValue(nbins,n,p,f);
  delete href;
 }

 delete [] n;
 delete [] p;

 return psi;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PsiExtreme(Double_t n,Int_t m,Double_t* p,Int_t k) const
{
// Provide extreme Bayesian Psi values for a certain number of trials
// w.r.t. a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
//
// Note : An outcome k is only taken into account if its probability pk>0.
//
// The Psi value provides (in dB scale) the amount of support that the
// data can maximally give to any Bernoulli class hypothesis different
// from the currently specified B_m.
//
// To be specific : Psi=-10*log[p(D|B_m I)]
//
// where p(D|B_m I) represents the likelihood of the data D under the condition
// that B_m and some prior I are true.
//
// A Psi value of zero indicates a perfect match between the observations
// and the specified hypothesis.
// However, due to the finite amount of trials a perfect match is never obtained.
// This gives rise to extreme Psi values given a certain amount of trials.
// The extreme Psi values which may be retrieved by this memberfunction reflect
// either the situation where all trials yield the same outcome k (i.e. nk=n)
// or the case that the observed numbers of occurrences of the different outcomes
// match the predictions (i.e. nk=n*pk).
// In the latter case fractional values of nk are allowed (e.g. for weighted event samples),
// unless the user explicitly requested a discrete situation (via k=-2) where all the nk
// will be integer values.
// A discrete situation gives a more accurate description in the case of a low statistics
// counting experiment where the various outcomes are obtained only a few times (if at all).
// In such a case the fractional nk values (which may be <1) don't reflect realistic outcomes.
//
// Further mathematical details can be found in astro-ph/0702029.
//
// n : The total number of trials
// m : The number of different possible outcomes of the counting experiment
// p : The probabilities of the different outcomes according to the hypothesis
// k : The specified (k=1,2,..,m) fixed outcome which is obtained at every trial.
//     Note : k= 0 implies the best match of the outcomes with the predictions (i.e. nk=n*pk).
//            k=-1 implies the worst match of the outcomes with the predictions,
//                 where all trials yield the outcome with the lowest probability.
//            k=-2 implies the best match of the outcomes with integer nk values.
//
// In case no probabilities are given (i.e. p=0), a uniform distribution is assumed.
//
// The default values are p=0 and k=0.
//
// In the case of inconsistent input, a Psi value of -1 is returned.
//
//--- Nick van Eijndhoven 17-jul-2008 NCFS

 Double_t psi=-1;

 if (m<=0 || n<=0 || k<-2 || k>m) return psi;

 psi=LogRfac(n);
 Double_t pk=1./double(m); // Prob. of getting outcome k for a uniform distr.

 ///////////////////////////////////////////
 // The user specified fixed outcome case //
 ///////////////////////////////////////////
 if (k>0)
 {
  if (p) pk=p[k-1];
  if (pk>0)
  {
   psi+=n*log10(pk)-LogRfac(n);
   psi*=-10.;
  }
  else 
  {
   psi=-1;
  }
  return psi;
 }

 ////////////////////////////////////////////////////////////////////////////////
 // Determine the minimal and maximal probability of all the possible outcomes //
 ////////////////////////////////////////////////////////////////////////////////
 Double_t pmin=999;
 Double_t pmax=-1;
 Int_t jmin=-1;
 Int_t jmax=-1;
 if (!p) // Uniform distribution
 {
  pmin=pk;
  pmax=pk;
  jmin=0;
  jmax=0;
 }
 else // User specified distribution
 {
  for (Int_t j=0; j<m; j++)
  {
   if (p[j]>0 && p[j]<pmin)
   {
    pmin=p[j];
    jmin=j;
   }
   if (p[j]>0 && p[j]>pmax)
   {
    pmax=p[j];
    jmax=j;
   }
  }
 }

 // Check for validity of the encountered pmin and pmax
 if (jmin<0 || jmax<0) return psi;

 /////////////////////////////
 // The worst matching case //
 /////////////////////////////
 if (k==-1)
 {
  if (pmin>0)
  {
   psi+=n*log10(pmin)-LogRfac(n);
   psi*=-10.;
  }
  else
  {
   psi=-1;
  }
  return psi;
 }

 Double_t nk=0;

 /////////////////////////////////////////////////////
 // The best matching case for fractional nk values //
 /////////////////////////////////////////////////////
 if (k==0)
 {
  for (Int_t i=0; i<m; i++)
  {
   if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
   if (pk>0)
   {
    nk=n*pk;
    psi+=nk*log10(pk)-LogRfac(nk);
   }
  }
  psi*=-10.;
  return psi;
 }

 //////////////////////////////////////////////////
 // The best matching case for integer nk values //
 //////////////////////////////////////////////////
 if (k==-2)
 {
  // Determine the best matching discrete distribution
  // by starting from the fractional nk=n*pk distribution
  Double_t* narr=new Double_t[m];
  Double_t ndisc=0;
  Int_t ndiff=0;
  for (Int_t i=0; i<m; i++)
  {
   narr[i]=0;
   if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
   if (pk>0) narr[i]=n*pk;
   ndisc+=int(narr[i]);
  }

  // Check the (integer) number of entries
  ndiff=int(n-ndisc);

  // Continue to complete the best matching discrete distribution
  // in case the integer number of entries doesn't match the original "n". 
  while (ndiff>0)
  {
   // For a uniform distribution just fill the "m bins" one after the other
   // with the remaining entries
   if (jmin==jmax) // Signature for a uniform distr.
   {
    Int_t im=0;
    for (Int_t ient=0; ient<ndiff; ient++)
    {
     narr[im]++;
     im++;
     if (im==m) im=0;
    }
    ndiff=0;
   }
   else // Increase the various "bin contents" until the total number of entries fits 
   {
    ndisc=0;
    for (Int_t i=0; i<m; i++)
    {
     if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
     if (pk>0) narr[i]+=pk;
     ndisc+=int(narr[i]);
    }
    ndiff=int(n-ndisc);
    // Only 1 entry left to fill --> Put it at the max. probability
    if (ndiff==1)
    {
     narr[jmax]+=1;
     ndiff=0;
    }
   }  
  } // End of while loop

  // In case too many entries have been filled, remove (one by one) the ones
  // with the lowest probability.
  while (ndiff<0)
  {
   // Determine the filled bin with minimal pk
   pmin=999;
   jmin=0;
   for (Int_t i=0; i<m; i++)
   {
    if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
    if (int(narr[i])>0 && pk<pmin)
    {
     pmin=p[i];
     jmin=i;
    }
   }
   // Remove one entry
   narr[jmin]-=1;
   ndiff++;
  }

  // The best matching discrete distr. is complete now
  for (Int_t i=0; i<m; i++)
  {
   if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
   if (pk>0)
   {
    nk=int(narr[i]);
    psi+=nk*log10(pk)-LogRfac(nk);
   }
  }
  psi*=-10.;
  
  delete [] narr;
 }
 return psi;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PsiExtreme(TH1* his,TH1* hyp,TF1* pdf,Int_t k) const
{
// Provide extreme Bayesian Psi values based on observations in histogram
// format w.r.t. a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
//
// The specification of a hypothesis B_m can be provided either in
// histogram format (hyp) or via a probability distribution function (pdf),
// as outlined below.
// Note : The pdf does not need to be normalised.
//
// The Psi value provides (in dB scale) the amount of support that the
// data can maximally give to any Bernoulli class hypothesis different
// from the currently specified B_m.
//
// To be specific : Psi=-10*log[p(D|B_m I)]
//
// where p(D|B_m I) represents the likelihood of the data D under the condition
// that B_m and some prior I are true.
//
// A Psi value of zero indicates a perfect match between the observations
// and the specified hypothesis.
// However, due to the finite amount of trials a perfect match is never obtained.
// This gives rise to extreme Psi values given a certain amount of trials.
// The extreme Psi values which may be retrieved by this memberfunction reflect
// either the situation where all trials yield the same outcome k (i.e. nk=n)
// or the case that the observed numbers of occurrences of the different outcomes
// match the predictions (i.e. nk=n*pk).
// In the latter case fractional values of nk are allowed (e.g. for weighted event samples),
// unless the user explicitly requested a discrete situation (via k=-2) where all the nk
// will be integer values.
// A discrete situation gives a more accurate description in the case of a low statistics
// counting experiment where the various outcomes are obtained only a few times (if at all).
// In such a case the fractional nk values (which may be <1) don't reflect realistic outcomes.
//
// Further mathematical details can be found in astro-ph/0702029.
//
// his : The experimental observations in histogram format
//       Note that Underflow and/or Overflow entries are not taken into account
// hyp : Hypothetical observations in histogram format according to some hypothesis
// pdf : Probability distribution function for the hypothesis
// k   : The specified (k=1,2,..,m) fixed outcome (bin) which is obtained at every trial.
//       Note : k= 0 implies the best match of the outcomes with the predictions (i.e. nk=n*pk).
//              k=-1 implies the worst match of the outcomes with the predictions,
//                   where all trials yield the outcome with the lowest probability
//              k=-2 implies the best match of the outcomes with integer nk values.
//
// In case no hypothesis is specified (i.e. hyp=0 and pdf=0), a uniform
// background distribution is assumed.
//
// Default values are : hyp=0, pdf=0 and k=0.
//
// In the case of inconsistent input, a Psi value of -1 is returned.
//
//--- Nick van Eijndhoven 18-jul-2008 NCFS

 Double_t psi=-1;

 if (!his) return psi;

 TAxis* xaxis=his->GetXaxis();
 Double_t xmin=xaxis->GetXmin();
 Double_t xmax=xaxis->GetXmax();
 Double_t range=xmax-xmin;
 Int_t nbins=his->GetNbinsX();
 Double_t nensig=his->GetSumOfWeights();

 if (nbins<=0 || nensig<=0 || range<=0) return psi;

 Double_t* p=new Double_t[nbins]; 
 Double_t pk;

 // Uniform hypothesis distribution
 if (!hyp && !pdf)
 {
  for (Int_t i=1; i<=nbins; i++)
  {
   pk=his->GetBinWidth(i)/range;
   p[i-1]=0;
   if (pk>0) p[i-1]=pk;
  }
  psi=PsiExtreme(nensig,nbins,p,k);
 }

 // Hypothesis specified via a pdf
 if (pdf && !hyp)
 {
  pdf->SetRange(xmin,xmax);
  Double_t ftot=pdf->Integral(xmin,xmax);
  if (ftot>0)
  {
   Double_t x1,x2;
   for (Int_t ipdf=1; ipdf<=nbins; ipdf++)
   {
    x1=his->GetBinLowEdge(ipdf);
    x2=x1+his->GetBinWidth(ipdf);
    pk=pdf->Integral(x1,x2)/ftot;
    p[ipdf-1]=0;
    if (pk>0) p[ipdf-1]=pk;
   }
   psi=PsiExtreme(nensig,nbins,p,k);
  }
 }

 // Hypothesis specified via a histogram
 if (hyp && !pdf)
 {
  TH1* href=(TH1*)his->Clone("href");
  href->Reset();
  Double_t nenhyp=0;
  Double_t x=0;
  Double_t y=0;
  for (Int_t ihyp=1; ihyp<=hyp->GetNbinsX(); ihyp++)
  {
   x=hyp->GetBinCenter(ihyp);
   y=hyp->GetBinContent(ihyp);
   href->Fill(x,y);
   nenhyp+=y;
  }
  for (Int_t j=1; j<=nbins; j++)
  {
   pk=href->GetBinContent(j)/nenhyp;
   p[j-1]=0;
   if (pk>0) p[j-1]=pk;
  }
  psi=PsiExtreme(nensig,nbins,p,k);
  delete href;
 }

 delete [] p;

 return psi;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PsiPvalue(Double_t psi0,Double_t nr,Double_t n,Int_t m,Double_t* p,Int_t f,Double_t* na,TH1F* psih,Int_t ncut,Double_t* nrx,Int_t mark)
{
// Provide the statistical P-value (i.e. the fraction of recorded psi values
// with psi>=psi0) for the specified psi0 based on "nr" repetitions of a
// counting experiment corresponding to a Bernoulli class hypothesis B_m
// with "n" independent random trials.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
//
// The Psi value of n trials of B_m provides (in dB scale) the amount of support
// that the data can maximally give to any Bernoulli class hypothesis different
// from the currently specified B_m.
//
// To be specific : Psi=-10*log[p(D|B_m I)]
//
// where p(D|B_m I) represents the likelihood of the data D under the condition
// that B_m and some prior I are true.
//
// A Psi value of zero indicates a perfect match between the observations
// and the specified hypothesis.
// Further mathematical details can be found in the publication
// N. van Eijndhoven, Astropart. Phys. 28 (2008) 540 (astro-ph/0702029).
//
// The arguments of this memberfunction :
// --------------------------------------
// psi0 : A user specified threshold psi value to provide the corresponding P-value
// nr   : The number of repetitions (see note 4) of the counting experiment with n independent random trials
// n    : The number of independent random trials of each counting experiment
// m    : The number of different possible outcomes of the counting experiment
// p    : The probabilities of the different outcomes according to the hypothesis
// f    : Flag to indicate the use of a frequentist (Stirling) approximation (f=1)
//        or the exact Bayesian expression (f=0).
// na   : Array with the signal c.q. (cumulative) observed numbers of occurrences of the different outcomes
// psih : Histogram with observed psi values (see notes 2, 4 and 5).
// ncut : Number of psi>=psi0 values to be obtained to trigger an early stop of the number of repetitions.
//        In case ncut=0 all the specified "nr" repetitions will be performed. 
// nrx  : Returned number of actually performed repetitions (only if a non-zero "nrx" value was also provided on input).
// mark : Flag to activate (mark=1) the marking of the threshold psi value (psi0) by a vertical line in the "psih" histogram.
//        Also the corresponding P-value will be mentioned in the legend.
//
// Notes :
// -------
// 1) When provided, the array "na" may be used to specify a specific configuration
//    representing a signal on invokation of this memberfunction.
//    This will allow the investigation of P-values for possible additional signals after
//    one or more signal configurations have already been established.
//    A provided signal configuration will be stored internally and after taking the "n"
//    independent random (background) trials, the signal configuration will be superimposed
//    "as is" on the resulting outcome of each B_m repetition. So, the signal configuration
//    itself will not be randomised.
//    After the randomisation procedure, the array "na" will contain the (cumulative) statistics
//    of the observations of the signal configuration and the randomised background. 
//    Obviously, in the case of a specified signal configuration all returned statistics will
//    be determined for the total number of "n+nsig" trials.
// 
// 2) When provided, the arrays "p" and "na" should be of dimension "m".
//    A way of retrieving the observed psi values is via the user defined histogram "psih",
//
// 3) The number of repetitions "nr", random trials "n" and the signal configuration
//    c.q. observed numbers of occurrences of the different outcomes "na" are of
//    type "double" to allow for large numbers.
//    Obviously all these variables are meant to represent only integer counts.
//
// 4) In case a non-zero input argument "ncut" is provided, the number of repetitions will be stopped
//    as soon as "ncut" values of psi>=psi0 are obtained.
//    When a large number of repetitions "nr" was specified, this allows an "early stop" and as such
//    a significant reduction of the CPU time. In case the number "ncut" was not reached, the repetition
//    of the counting experiment wil stop as soon as "nr" repetitions have been performed.
//    However, when nr=0 was specified the counting experiment will be repeated until the number "ncut"
//    is reached or when the number of repetitions has reached the maximum allowed value of 1e19.
//    In case a non-zero input argument "nrx" is provided the number of actually performed repetitions
//    will be returned via this same argument "nrx".
//
// 5) In case a histogram "psih" is provided, this function will set the axes titles (and a legend if "mark" is activated).
//
// For practical reasons the maximum values of "nr" and "n" have been limited
// to 1e19, which is about the corresponding maximum value of an unsigned 64-bit integer.
//
// In case no probabilities are given (i.e. p=0), a uniform distribution is assumed.
//
// The default values are p=0, f=0, na=0, psih=0, ncut=0, nrx=0 and mark=1.
//
// In the case of inconsistent input, a value of -1 is returned.
//
//--- NvE 17-nov-2008 NCFS

 if (psi0<0 || nr<0 || (nr>0 && nr<2)) return -1;

 Double_t pval=-1;

 NcRandom q;
 if (na) n=-n;
 Double_t nrused;
 Double_t val=q.RanBm(nr,n,m,p,na,0,psi0,f,psih,ncut,&nrused);
 if (val>=0) pval=val/nrused;
 if (nrx) (*nrx)=nrused;

 // Set axes titles for the "psih" histogram
 if (psih)
 {
  TString title="#psi value in dB";
  TString s="Counts after ";
  s+=nrused;
  s+=" randomisations";

  psih->GetXaxis()->SetTitle(title.Data());
  psih->GetYaxis()->SetTitle(s.Data());

  // Mark the psi0 value by a vertical line in the "psih" histogram
  // Also the corresponding P-value is mentioned in the legend
  if (mark)
  {
   Float_t x=psi0;
   Float_t ymin=0;
   Float_t ymax=psih->GetMaximum();

   TLine* vline=new TLine(x,ymin,x,ymax);
   vline->SetLineStyle(2); // Dashed line
   vline->SetLineWidth(2);
   vline->SetLineColor(4); // Blue color

   title="P-value : %-10.3g";
   s=title.Format(title.Data(),pval);

   TLegend* leg=new TLegend(0.6,0.8,0.8,0.9);
   leg->SetFillColor(0);
   leg->SetHeader(s.Data());
   leg->AddEntry(vline,"Observed #psi","L");

   TList* hlist=psih->GetListOfFunctions();
   hlist->Add(vline);
   hlist->Add(leg);
  }
 }


 return pval;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::PsiPvalue(Double_t psi0,Double_t nr,TH1* his,TH1* hyp,TF1* pdf,Int_t f,Double_t* na,TH1F* psih,Int_t ncut,Double_t* nrx,Int_t mark)
{
// Provide the statistical P-value (i.e. the fraction of recorded psi values
// with psi>=psi0) for the specified psi0 based on "nr" repetitions of a
// counting experiment (specified by the observed histogram "his") corresponding
// to a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
// The number of independent random trials "n" of each counting experiment
// is determined from the number of entries of the input histogram "his".
//
// The specification of a hypothesis B_m can be provided either in
// histogram format (hyp) or via a probability distribution function (pdf),
// as outlined below.
// Note : The histogram "hyp" or the function "pdf" do not need to be normalised.
//
// The Psi value provides (in dB scale) the amount of support that the
// data can maximally give to any Bernoulli class hypothesis different
// from the currently specified B_m.
//
// To be specific : Psi=-10*log[p(D|B_m I)]
//
// where p(D|B_m I) represents the likelihood of the data D under the condition
// that B_m and some prior I are true.
//
// A Psi value of zero indicates a perfect match between the observations
// and the specified hypothesis.
// Further mathematical details can be found in the publication
// N. van Eijndhoven, Astropart. Phys. 28 (2008) 540 (astro-ph/0702029).
//
//
// The arguments of this memberfunction :
// --------------------------------------
// psi0 : A user specified threshold psi value to provide the corresponding P-value.
//        If psi0<0 the corresponding psi value of the input histogram "his" will be taken as psi0.
// nr   : The number of repetitions (see note 4) of the counting experiment with n independent random trials.
// his  : The experimental observations of the different B_m outcomes in histogram format.
//        Note that Underflow and/or Overflow entries are not taken into account.
// hyp  : Hypothetical observations (in histogram format) according to some B_m hypothesis.
// pdf  : Probability distribution function for some B_m hypothesis.
// f    : Flag to indicate the use of a frequentist (Stirling) approximation (f=1)
//        or the exact Bayesian expression (f=0).
// na   : Array with the signal c.q. (cumulative) observed numbers of occurrences of the different outcomes.
// psih : Histogram with observed psi values (see notes 2, 4 and 5).
// ncut : Number of psi>=psi0 values to be obtained to trigger an early stop of the number of repetitions.
//        In case ncut=0 all the specified "nr" repetitions will be performed. 
// nrx  : Returned number of actually performed repetitions (only if a non-zero "nrx" value was also provided on input).
// mark : Flag to activate (mark=1) the marking of the threshold psi value (psi0) by a vertical line in the "psih" histogram.
//        Also the corresponding P-value will be mentioned in the legend.
//
// Notes :
// -------
// 1) When provided, the array "na" may be used to specify a specific configuration
//    representing a signal on invokation of this memberfunction.
//    This will allow the investigation of P-values for possible additional signals after
//    one or more signal configurations have already been established.
//    A provided signal configuration will be stored internally and after taking the "n"
//    independent random (background) trials, the signal configuration will be superimposed
//    "as is" on the resulting outcome of each B_m repetition. So, the signal configuration
//    itself will not be randomised.
//    After the randomisation procedure, the array "na" will contain the (cumulative) statistics
//    of the observations of the signal configuration and the randomised background. 
//    Obviously, in the case of a specified signal configuration all returned statistics will
//    be determined for the total number of "n+nsig" trials.
// 
// 2) When provided, the array "na" should be of dimension "m", being the number of bins
//    of the input histogram "his".
//    A way of retrieving the observed psi values is via the user defined histogram "psih",
//
// 3) The number of repetitions "nr" and the signal configuration c.q. observed numbers of
//    occurrences of the different outcomes "na" are of type "double" to allow for large numbers.
//    Obviously all these variables are meant to represent only integer counts.
//
// 4) In case a non-zero input argument "ncut" is provided, the number of repetitions will be stopped
//    as soon as "ncut" values of psi>=psi0 are obtained.
//    When a large number of repetitions "nr" was specified, this allows an "early stop" and as such
//    a significant reduction of the CPU time. In case the number "ncut" was not reached, the repetition
//    of the counting experiment wil stop as soon as "nr" repetitions have been performed.
//    However, when nr=0 was specified the counting experiment will be repeated until the number "ncut"
//    is reached or when the number of repetitions has reached the maximum allowed value of 1e19.
//    In case a non-zero input argument "nrx" is provided the number of actually performed repetitions
//    will be returned via this same argument "nrx".
//
// 5) In case a histogram "psih" is provided, this function will set the axes titles (and a legend if "mark" is activated).
//
// For practical reasons the maximum value of "nr" has been limited to 1e19, which is about
// the corresponding maximum value of an unsigned 64-bit integer.
//
// In case no hypothesis is specified (i.e. hyp=0 and pdf=0), a uniform
// background distribution is assumed.
//
// Default values are : hyp=0, pdf=0 f=0, na=0, psih=0, ncut=0, nrx=0 and mark=1.
//
// In the case of inconsistent input, a P-value of -1 is returned.
//
//--- Nick van Eijndhoven 05-may-2011 IIHE Brussel

 Double_t pval=-1;

 if (!his) return pval;

 TAxis* xaxis=his->GetXaxis();
 Double_t xmin=xaxis->GetXmin();
 Double_t xmax=xaxis->GetXmax();
 Double_t range=xmax-xmin;
 Int_t nbins=his->GetNbinsX();
 Double_t nensig=his->GetSumOfWeights();

 if (nbins<=0 || nensig<=0 || range<=0) return pval;

 if (psi0<0) psi0=PsiValue(his,hyp,pdf,f);

 Double_t* n=new Double_t[nbins];
 Double_t* p=new Double_t[nbins]; 
 Double_t nk;
 Double_t pk;

 // Uniform hypothesis distribution
 if (!hyp && !pdf)
 {
  for (Int_t i=1; i<=nbins; i++)
  {
   nk=his->GetBinContent(i);
   pk=his->GetBinWidth(i)/range;
   n[i-1]=0;
   p[i-1]=0;
   if (nk>0) n[i-1]=nk;
   if (pk>0) p[i-1]=pk;
  }
  pval=PsiPvalue(psi0,nr,nensig,nbins,p,f,na,psih,ncut,nrx,mark);
 }

 // Hypothesis specified via a pdf
 if (pdf && !hyp)
 {
  pdf->SetRange(xmin,xmax);
  Double_t ftot=pdf->Integral(xmin,xmax);
  if (ftot>0)
  {
   Double_t x1,x2;
   for (Int_t ipdf=1; ipdf<=nbins; ipdf++)
   {
    nk=his->GetBinContent(ipdf);
    x1=his->GetBinLowEdge(ipdf);
    x2=x1+his->GetBinWidth(ipdf);
    pk=pdf->Integral(x1,x2)/ftot;
    n[ipdf-1]=0;
    p[ipdf-1]=0;
    if (nk>0) n[ipdf-1]=nk;
    if (pk>0) p[ipdf-1]=pk;
   }
   pval=PsiPvalue(psi0,nr,nensig,nbins,p,f,na,psih,ncut,nrx,mark);
  }
 }

 // Hypothesis specified via a histogram
 if (hyp && !pdf)
 {
  TH1* href=(TH1*)his->Clone("href");
  href->Reset();
  Double_t nenhyp=0;
  Double_t x,y;
  for (Int_t ihyp=1; ihyp<=hyp->GetNbinsX(); ihyp++)
  {
   x=hyp->GetBinCenter(ihyp);
   y=hyp->GetBinContent(ihyp);
   href->Fill(x,y);
   nenhyp+=y;
  }
  for (Int_t j=1; j<=nbins; j++)
  {
   nk=his->GetBinContent(j);
   pk=href->GetBinContent(j)/nenhyp;
   n[j-1]=0;
   p[j-1]=0;
   if (nk>0) n[j-1]=nk;
   if (pk>0) p[j-1]=pk;
  }
  pval=PsiPvalue(psi0,nr,nensig,nbins,p,f,na,psih,ncut,nrx,mark);
  delete href;
 }

 delete [] n;
 delete [] p;

 return pval;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Chi2Value(Int_t m,Int_t* n,Double_t* p,Int_t* ndf) const
{
// Provide the frequentist chi-squared value of observations of a counting
// experiment w.r.t. a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
//
// Further mathematical details can be found in astro-ph/0702029.
//
// m   : The number of different possible outcomes of the counting experiment
// n   : The observed number of different outcomes
// p   : The probabilities of the different outcomes according to the hypothesis
// ndf : The returned number of degrees of freedom
//
// Note : Both the arrays "n" and (when provided) "p" should be of dimension "m".
//
// In case no probabilities are given (i.e. pk=0), a uniform distribution
// is assumed.
//
// The default values are pk=0 and ndf=0.
//
// In the case of inconsistent input, a chi-squared and ndf value of -1 is returned.
//
//--- Nick van Eijndhoven 03-oct-2007 NCFS

 Double_t chi=-1;
 if (ndf) *ndf=-1;

 if (m<=0 || !n) return chi;

 Int_t ntot=0;
 for (Int_t j=0; j<m; j++)
 {
  if (n[j]>0) ntot+=n[j];
 }

 chi=0;
 Double_t pk=1./float(m); // Prob. of getting outcome k for a uniform distr.
 for (Int_t i=0; i<m; i++)
 {
  if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
  if (n[i]>0 && pk>0 && ntot>0)
  {
   chi+=pow(double(n[i])-double(ntot)*pk,2)/(double(ntot)*pk);
   if (ndf) (*ndf)=(*ndf)+1;
  }
 }

 return chi;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Chi2Value(Int_t m,Double_t* n,Double_t* p,Int_t* ndf) const
{
// Provide the frequentist chi-squared value of observations of a counting
// experiment w.r.t. a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
//
// Note : The observed numbers of occurrences of the different outcomes
//        may be fractional numbers for this memberfunction.
//        This mainly serves to investigate predicted background matches
//        via histogram input.
//
// Further mathematical details can be found in astro-ph/0702029.
//
// m   : The number of different possible outcomes of the counting experiment
// n   : The observed number of different outcomes
// p   : The probabilities of the different outcomes according to the hypothesis
// ndf : The returned number of degrees of freedom
//
// Note : Both the arrays "n" and (when provided) "p" should be of dimension "m".
//
// In case no probabilities are given (i.e. pk=0), a uniform distribution
// is assumed.
//
// The default values are pk=0 and ndf=0.
//
// In the case of inconsistent input, a chi-squared and ndf value of -1 is returned.
//
//--- Nick van Eijndhoven 25-jul-2008 NCFS

 Double_t chi=-1;
 if (ndf) *ndf=-1;

 if (m<=0 || !n) return chi;

 Double_t ntot=0;
 for (Int_t j=0; j<m; j++)
 {
  if (n[j]>0) ntot+=n[j];
 }

 chi=0;
 Double_t pk=1./float(m); // Prob. of getting outcome k for a uniform distr.
 for (Int_t i=0; i<m; i++)
 {
  if (p) pk=p[i]; // Probabilities from a specific B_m hypothesis
  if (n[i]>0 && pk>0 && ntot>0)
  {
   chi+=pow(n[i]-ntot*pk,2)/(ntot*pk);
   if (ndf) (*ndf)=(*ndf)+1;
  }
 }

 return chi;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::Chi2Value(TH1* his,TH1* hyp,TF1* pdf,Int_t* ndf) const
{
// Provide the frequentist chi-squared value of observations of a counting
// experiment (in histogram format) w.r.t. a Bernoulli class hypothesis B_m.
// The hypothesis B_m represents a counting experiment with m different
// possible outcomes and is completely defined by the probabilities
// of the various outcomes (and the requirement that the sum of all these
// probabilities equals 1).
// The specification of a hypothesis B_m can be provided either in
// histogram format (hyp) or via a probability distribution function (pdf),
// as outlined below.
// Note : The pdf does not need to be normalised.
//
// Further mathematical details can be found in astro-ph/0702029.
//
// his : The experimental observations in histogram format
//       Note that Underflow and/or Overflow entries are not taken into account
// hyp : Hypothetical observations according to some hypothesis
// pdf : Probability distribution function for the hypothesis
// ndf : The returned number of degrees of freedom
//
// In case no hypothesis is specified (i.e. hyp=0 and pdf=0), a uniform
// background distribution is assumed.
//
// Default values are : hyp=0, pdf=0 and ndf=0.
//
// In the case of inconsistent input, a chi-squared and ndf value of -1 is returned.
//
//--- Nick van Eijndhoven 03-oct-2007 NCFS

 Double_t chi=-1;

 if (!his) return chi;

 TAxis* xaxis=his->GetXaxis();
 Double_t xmin=xaxis->GetXmin();
 Double_t xmax=xaxis->GetXmax();
 Double_t range=xmax-xmin;
 Int_t nbins=his->GetNbinsX();
 Double_t nensig=his->GetSumOfWeights();

 if (nbins<=0 || nensig<=0 || range<=0) return chi;

 Double_t* n=new Double_t[nbins];
 Double_t* p=new Double_t[nbins]; 
 Double_t nk;
 Double_t pk;

 // Uniform hypothesis distribution
 if (!hyp && !pdf)
 {
  for (Int_t i=1; i<=nbins; i++)
  {
   nk=his->GetBinContent(i);
   pk=his->GetBinWidth(i)/range;
   n[i-1]=0;
   p[i-1]=0;
   if (nk>0) n[i-1]=nk;
   if (pk>0) p[i-1]=pk;
  }
  chi=Chi2Value(nbins,n,p,ndf);
 }

 // Hypothesis specified via a pdf
 if (pdf && !hyp)
 {
  pdf->SetRange(xmin,xmax);
  Double_t ftot=pdf->Integral(xmin,xmax);
  if (ftot>0)
  {
   Double_t x1,x2;
   for (Int_t ipdf=1; ipdf<=nbins; ipdf++)
   {
    nk=his->GetBinContent(ipdf);
    x1=his->GetBinLowEdge(ipdf);
    x2=x1+his->GetBinWidth(ipdf);
    pk=pdf->Integral(x1,x2)/ftot;
    n[ipdf-1]=0;
    p[ipdf-1]=0;
    if (nk>0) n[ipdf-1]=nk;
    if (pk>0) p[ipdf-1]=pk;
   }
   chi=Chi2Value(nbins,n,p,ndf);
  }
 }

 // Hypothesis specified via a histogram
 if (hyp && !pdf)
 {
  TH1* href=(TH1*)his->Clone("href");
  href->Reset();
  Double_t nenhyp=0;
  Double_t x,y;
  for (Int_t ihyp=1; ihyp<=hyp->GetNbinsX(); ihyp++)
  {
   x=hyp->GetBinCenter(ihyp);
   y=hyp->GetBinContent(ihyp);
   href->Fill(x,y);
   nenhyp+=y;
  }
  for (Int_t j=1; j<=nbins; j++)
  {
   nk=his->GetBinContent(j);
   pk=href->GetBinContent(j)/nenhyp;
   n[j-1]=0;
   p[j-1]=0;
   if (nk>0) n[j-1]=nk;
   if (pk>0) p[j-1]=pk;
  }
  chi=Chi2Value(nbins,n,p,ndf);
  delete href;
 }

 delete [] n;
 delete [] p;

 return chi;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::MeanMu(Double_t cl,Double_t nbkg,Int_t mode,TF1* fw,TFeldmanCousins* fc,Int_t nmax) const
{
// Provide the Feldman-Cousins average upper/lower limit corresponding to
// the confidence level "cl", some background expectation and weight function.
//
// Input arguments :
// -----------------
// cl   : The required confidence level (e.g. 0.95).
// nbkg : The expected number of background events.
// mode : Flag to request the average lowerlimit (1) or upperlimit (2).
// fw   : Weight function, evaluated for each "nobs" value in the cumulative summation.
// fc   : Specific Feldman-Cousins function to be used (optional).
// nmax : The maximum "nobs" value until which the cumulative summation is performed.
//
// Notes :
// -------
// 1) If w=0 a Poisson pdf will be used as weight function.
// 2) In case f=0 the standard TFeldmanCousins settings are used
//    (apart from the specified "cl" value, which is always taken).
// 3) In case nmax=0, the cumulative summation will run until nobs=nbkg+10*sqrt(nbkg),
//    which corresponds to 10 sigma above the expectation value for a Poisson pdf.
//
// Default values are w=0, f=0 an nmax=0.

 TFeldmanCousins* f=fc;
 if (!f) f=new TFeldmanCousins();

 f->SetCL(cl);

 if (!nmax) nmax=int(nbkg+10.*sqrt(nbkg));

 Double_t mu;
 Double_t muav=0;

 // Cumulative summation for the number of observed events
 for (Int_t nobs=0; nobs<=nmax; nobs++)
 {
  if (mode==1)
  {
   mu=f->CalculateLowerLimit(nobs,nbkg);
  }
  else
  {
   mu=f->CalculateUpperLimit(nobs,nbkg);
  }
  if (fw)
  {
   muav+=mu*fw->Eval(nobs);
  }
  else // Poisson pdf weight
  {
   muav+=mu*pow(nbkg,nobs)*exp(-nbkg)/Rfac(nobs);
  }
 }

 if (!fc) delete f;
 return muav;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcMath::LiMaSignificance(Double_t Non,Double_t Ton,Double_t Noff,Double_t Toff,Double_t Ra,Double_t Re) const
{
// Provide the significance in terms of the amount of standard deviations 
// of a certain "on source" and "off source" observation according to the
// procedure outlined by T.Li and Y.Ma in Astrophysical Journal 271 (1983) 317.
//
// In case of non-physical situations the value -1 is returned.
//
// Input arguments :
// -----------------
// Non  : The number of observed "on source" events
// Ton  : The "on source" exposure time
// Noff : The number of observed "off source" events 
// Toff : The "off source" exposure time
// Ra   : The ratio (on source area)/(off source area)
// Re   : The ratio (on source detection efficiency)/(off source detection efficiency)
//
// Notes :
// -------
// 1) The exposure times Ton and Toff may be given in any units (sec, min, hours, ...)
//    provided that for both the same units are used.
// 2) The resulting significance is most reliable for Non>10 and Noff>10.
//
// The default values are Ra=1 and Re=1.

 Double_t s=-1;

 if (Non<=0 || Noff<=0 || Ton<=0 || Toff<=0 || Ra<=0 || Re<=0) return -1;

 Double_t sum=Non+Noff;

 Double_t a=Ra*Re*Ton/Toff;

 s=2.*(Non*log((1.+a)*Non/(a*sum))+Noff*log((1.+a)*Noff/sum));
 s=sqrt(s);
 return s;
}
///////////////////////////////////////////////////////////////////////////
