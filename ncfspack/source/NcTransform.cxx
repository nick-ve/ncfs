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
/** @class NcTransform
~~~
// Class NcTransform
// Class to perform various discrete transformations of (sequential) data samples.
//
// ********************************************************************************
// *** This class has become obsolete.                                          ***
// *** Please use the class NcDSP instead, which offers extended functionality. ***
// *** This class NcTransform is only kept for backward compatibility.          ***
// ********************************************************************************
// 
//
// The following discrete transformations (using the FFTW algorithms) are supported :
// Fourier (DFT), Hartley (DHT), Sine (DST) and Cosine (DCT).
//
// All transformation results have been normalized, such that
// the inverse transformation provides the original input spectrum. 
//
// For details about the transformations and their options, please refer
// to the documentation in the corresponding member functions.
//
// Usage example for a discrete Fourier transform (DFT) :
// ------------------------------------------------------
//
// Int_t N=2048;          // Number of samples
// Float_t fsample=3.2e9; // Sampling rate in Hz
// Float_t nu=3e8;        // Signal frequency in Hz
//
// Float_t pi=acos(-1.);
// Float_t omega=2.*pi*nu;  // Signal frequency in rad/sec
// Float_t step=1./fsample; // The time step of each sample
//
// // Fill the time domain sampled data
// Double_t tdata[N];
// Float_t t=0;
// Float_t y=0;
// for (Int_t i=0; i<N; i++)
// {
//  y=cos(omega*t)+5.*sin(3.*omega*t);
//  tdata[i]=y;
//  t+=step;
// }
//
// NcTransform q;
// q.SetSamplingFrequency(fsample);
// q.Load(N,tdata);
//
// // Obtain the amplitudes in an array
// q.Fourier("R2C");
// TArrayD arr=q.GetData("AMP out");
//
// // Obtain the amplitudes in a histogram
// TH1F hist;
// q.Fourier("R2C",&hist,"AMP Hz");
//
// // Illustration of forward followed by inverse transformation
//
// // The original time domain input spectrum
// TH1F hist1;
// q.Fourier("R2C",&hist1,"t");
//
// // The forward transformation
// TH1F hist2;
// q.Fourier("R2C",&hist2,"AMP Hz");
//
// // Load the obtained frequency domain data as new input to obtain
// // the original time domain spectrum via the inverse transformation
// q.LoadResult();
//
// // The frequency domain input spectrum
// TH1F hist3;
// q.Fourier("C2R",&hist3,"AMP Hz");
//
// // Obtain the original time domain spectrum via the inverse transformation
// TH1F hist4;
// q.Fourier("C2R",&hist4,"t");
//
// TCanvas c1("c1","Forward");
// c1.Divide(1,2);
// c1.cd(1);
// hist1.Draw();
// c1.cd(2);
// hist2.Draw();
//
// TCanvas c2("c2","Inverse");
// c2.Divide(1,2);
// c2.cd(1);
// hist3.Draw();
// c2.cd(2);
// hist4.Draw();
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, October 19, 2021  09:42Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, December 11, 2021  01:51Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "NcTransform.h"
#include "Riostream.h"
 
ClassImp(NcTransform); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
NcTransform::NcTransform(const char* name,const char* title) : TNamed(name,title)
{
/**
~~~
// Default constructor.
~~~
**/

 Reset();
 fSample=0;
}
///////////////////////////////////////////////////////////////////////////
NcTransform::~NcTransform()
{
/**
~~~
// Default destructor.
~~~
**/

 if (fProc)
 {
  delete fProc;
  fProc=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcTransform::NcTransform(const NcTransform& q) : TNamed(q)
{
/**
~~~
// Copy constructor.
~~~
**/

 fProc=0;
 fN=q.fN;
 fReIn=q.fReIn;
 fImIn=q.fImIn;
 fReOut=q.fReOut;
 fImOut=q.fImOut;
 fSample=q.fSample;
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Reset()
{
/**
~~~
// Internal member function to reset all data and the processor. 
~~~
**/

 fProc=0;
 fN=0;
 fReIn.Set(0);
 fImIn.Set(0);
 fReOut.Set(0);
 fImOut.Set(0);
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::SetSamplingFrequency(Float_t f)
{
/**
~~~
// Set the actual DAQ sampling frequency in Hz.
// This sampling frequency may be overwritten by invokation of
// one of the various Load() member functions.
// The sampling frequency is set to 0 in the constructor of this class.
~~~
**/

 fSample=f;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcTransform::GetSamplingFrequency() const
{
/**
~~~
// Provide the current value of the DAQ sampling frequency in Hz.
~~~
**/

 return fSample;
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Load(Int_t n,Double_t* re,Double_t* im,Float_t f)
{
/**
~~~
// Provide new input data to be transformed and reset the processor.
//
// Input arguments :
// -----------------
// n  : The number of data elements
// re : Array with real data elements
// im : Array with imaginary data elements
// f  : (optional) Actual data acquisition sampling frequency in Hz
//      In case f<0 the current sampling frequency is not modified.
//
// If provided, the array size must be at least of dimension "n".
//
// The defaults are im=0 and f=-1.
~~~
**/

 Reset();

 if (f>=0) fSample=f;

 if (n<1) return;

 fN=n;
 if (re) fReIn.Set(n);
 if (im) fImIn.Set(n);

 for (Int_t i=0; i<n; i++)
 {
  if (re) fReIn.SetAt(re[i],i);
  if (im) fImIn.SetAt(im[i],i);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Load(TArray* re,TArray* im,Float_t f)
{
/**
~~~
// Provide new input data to be transformed and reset the processor.
//
// Input arguments :
// -----------------
// re : Array with real data elements
// im : Array with imaginary data elements
// f  : (optional) Actual data acquisition sampling frequency in Hz
//      In case f<0 the current sampling frequency is not modified.
//
// If both arrays are provided, the size of the smallest non-empty array
// will be used to compose the input data.
//
// The defaults are im=0 and f=-1.
~~~
**/

 Reset();

 if (f>=0) fSample=f;

 Int_t nre=0;
 Int_t nim=0;
 if (re) nre=re->GetSize();
 if (im) nim=im->GetSize();

 Int_t n=nre;
 if (!n) n=nim;
 if (nre && nim>nre) n=nre;
 if (nim && nre>nim) n=nim;

 if (n<1) return;

 fN=n;
 if (nre) fReIn.Set(n);
 if (nim) fImIn.Set(n);

 Double_t valre=0;
 Double_t valim=0;
 for (Int_t i=0; i<n; i++)
 {
  if (nre)
  {
   valre=re->GetAt(i);
   fReIn.SetAt(valre,i);
  }
  if (nim)
  {
   valim=im->GetAt(i);
   fImIn.SetAt(valim,i);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Load(NcSample* s,Int_t i,Float_t f)
{
/**
~~~
// Provide new input data from the NcSample "s" to be transformed and reset the processor.
//
// Input arguments :
// -----------------
// i : The data of the i-th variable (1=first) of the NcSample are used
// f : (optional) Actual data acquisition sampling frequency in Hz
//      In case f<0 the current sampling frequency is not modified.
//
// Note : The Store Mode of the NcSample must be activated. 
//
// The default value is f=-1.
~~~
**/

 Reset();

 if (f>=0) fSample=f;

 if (!s)
 {
  cout << " *" << ClassName() <<"::Load* No input sample specified." << endl;
  return;
 }

 Int_t n=s->GetN();
 Int_t store=s->GetStoreMode();
 Int_t dim=s->GetDimension();

 if (n<1 || !store || dim<1 || i<1 || i>dim)
 {
  cout << " *" << ClassName() <<"::Load* Inconsistent input for NcSample treatment." << endl;
  cout << " Store Mode:" << store << " Entries:" << n << " Dimension:" << dim << " i:" << i << " f:" << fSample << endl; 
  return;
 }

 fN=n;
 fReIn.Set(n);

 Double_t val=0;
 for (Int_t idx=1; idx<=n; idx++)
 {
  val=s->GetEntry(idx,i);
  fReIn.SetAt(val,idx-1);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Load(NcSample* s,TString name,Float_t f)
{
/**
~~~
// Provide new input data from the NcSample "s" to be transformed and reset the processor.
//
// Input arguments :
// -----------------
// name : Name of the NcSample variable of which the data are used
// f    : (optional) Actual data acquisition sampling frequency in Hz
//        In case f<0 the current sampling frequency is not modified.
//
// Note : The Store Mode of the NcSample must be activated. 
//
// The default value is f=-1.
~~~
**/

 Reset();

 if (f>=0) fSample=f;

 if (!s)
 {
  cout << " *" << ClassName() <<"::Load* No input sample specified." << endl;
  return;
 }

 Int_t n=s->GetN();
 Int_t store=s->GetStoreMode();
 Int_t dim=s->GetDimension();
 Int_t i=s->GetIndex(name);
 

 if (n<1 || !store || dim<1 || !i)
 {
  cout << " *" << ClassName() <<"::Load* Inconsistent input for NcSample treatment." << endl;
  cout << " Store Mode:" << store << " Entries:" << n << " Dimension:" << dim << " name:" << name << " f:" << fSample << endl; 
  return;
 }

 Load(s,i,f);
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Load(TH1* h,Float_t f)
{
/**
~~~
// Provide new input data from a 1-Dimensional histogram to be transformed and reset the processor.
//
// Input arguments :
// -----------------
// h : Pointer to the 1-D histogram of which the bin contents are used
// f : (optional) Actual data acquisition sampling frequency in Hz
//     In case f<0 the current sampling frequency is not modified.
//
// The default value is f=-1.
~~~
**/

 Reset();

 if (f>=0) fSample=f;

 if (!h)
 {
  cout << " *" << ClassName() <<"::Load* No input histogram specified." << endl;
  return;
 }

 Int_t nbins=h->GetNbinsX();
 Double_t nentries=h->GetEntries();

 if (!nbins || !nentries)
 {
  cout << " *" << ClassName() <<"::Load* Inconsistent input for histogram treatment." << endl;
  cout << " Nbins:" << nbins << " Nentries:" << nentries << " f:" << fSample << endl; 
  return;
 }

 fN=nbins;
 fReIn.Set(nbins);

 Double_t val=0;
 for (Int_t i=1; i<=nbins; i++)
 {
  val=h->GetBinContent(i);
  fReIn.SetAt(val,i-1);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Load(TGraph* gr,Float_t f)
{
/**
~~~
// Provide new input data from a TGraph object to be transformed and reset the processor.
//
// Input arguments :
// -----------------
// gr : Pointer to the TGraph object of which the bin contents are used
// f  : (optional) Actual data acquisition sampling frequency in Hz
//      In case f<0 the current sampling frequency is not modified.
//
// The default value is f=-1.
~~~
**/

 Reset();

 if (f>=0) fSample=f;

 if (!gr)
 {
  cout << " *" << ClassName() <<"::Load* No input TGraph object specified." << endl;
  return;
 }

 Int_t n=gr->GetN();

 if (!n)
 {
  cout << " *" << ClassName() <<"::Load* Inconsistent input for TGraph treatment." << endl;
  cout << " n:" << n << " f:" << fSample << endl; 
  return;
 }

 fN=n;
 fReIn.Set(n);

 gr->Sort();

 Double_t x=0;
 Double_t y=0;
 for (Int_t i=0; i<n; i++)
 {
  gr->GetPoint(i,x,y);
  fReIn.SetAt(y,i);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::LoadResult()
{
/**
~~~
// Load the transformation result as new input data in order to enable
// inverse transformations acting on previous transformation results.
~~~
**/

 fReIn=fReOut;
 fImIn=fImOut;
 fReOut.Set(0);
 fImOut.Set(0);
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTransform::GetN() const
{
/**
~~~
// Provide the number of data elements (to be) processed.
~~~
**/

 return fN;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcTransform::GetData(TString sel) const
{
/**
~~~
// Provide a selected set of data.
//
// Input argument :
// ----------------
// sel  : String to specify the contents of the provided data array
//        "RE"   --> The values of the real (re) components
//        "IM"   --> The values of the imaginary (im) components
//        "AMP"  --> The amplitudes, i.e. sqrt(re*re+im*im)
//        "PHIR" --> The phases, i.e. arctan(im/re), in radians
//        "PHID" --> The phases, i.e. arctan(im/re), in degrees
//        "in"   --> The values of the input data are provided
//        "out"  --> The values of the output data are provided
//
// Examples :
// ----------
// sel="AMP out" will provide all the N amplitudes of the resulting data after transformation.
// sel="RE in" will provide all the N real components of the input data.
~~~
**/

 if (sel.Contains("RE") && sel.Contains("in")) return fReIn;
 if (sel.Contains("IM") && sel.Contains("in")) return fImIn;
 if (sel.Contains("RE") && sel.Contains("out")) return fReOut;
 if (sel.Contains("IM") && sel.Contains("out")) return fImOut;

 TArrayD data(fN);
 Double_t pi=acos(-1.);
 Double_t re=0;
 Double_t im=0;
 Double_t amp=0;
 Double_t phi=0;
 for (Int_t i=0; i<fN; i++)
 {
  if (sel.Contains("in"))
  {
   re=fReIn.At(i);
   im=fImIn.At(i);
  }
  if (sel.Contains("out"))
  {
   re=fReOut.At(i);
   im=fImOut.At(i);
  }
  amp=sqrt(re*re+im*im);
  phi=0;
  if (im || re) phi=atan2(im,re);

  if (sel.Contains("AMP")) data.SetAt(amp,i);
  if (sel.Contains("PHIR")) data.SetAt(phi,i);
  if (sel.Contains("PHID")) data.SetAt(phi*180./pi,i);
 }

 return data;
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Fourier(TString mode,TH1* hist,TString sel)
{
/**
~~~
// Perform a normalized 1-dimensional Discrete Fourier Transformation (DFT).
//
// Conventions :
// -------------
// N = The number of data elements
// Time domain array : X[]=X[0],...,X[N-1]
// Frequency domain array : Q[]=Q[0],...,Q[N-1]
//
// Fourier transform : Q[k]=(1/sqrt(N))*sum(n=0,n=N-1){X[n]*exp(-i*2pi*(k/N)*n)}
//
// Inverse Fourier transform : X[n]=(1/sqrt(N))*sum(k=0,k=N-1){Q[k]*exp(i*2pi*(n/N)*k)}
//
// Input arguments :
// -----------------
// mode : "R2C"  --> Perform a real-input to complex-output discrete Fourier transformation
//        "C2R"  --> Perform the inverse transformation of "R2C"
//        "C2C"  --> Perform a complex-input to complex-output discrete Fourier transformation
//        "C2CI" --> Perform the inverse of "C2C"
// hist : (optional) Histogram with selected results
// sel  : String to specify the contents and representation of the result histogram
//        "RE"   --> Y-axis shows the values of the real (re) components
//        "IM"   --> Y-axis shows the values of the imaginary (im) components
//        "AMP"  --> Y-axis shows the values of the amplitudes, i.e. sqrt(re*re+im*im)
//        "PHIR" --> Y-axis shows the values of the phases, i.e. arctan(im/re), in radians
//        "PHID" --> Y-axis shows the values of the phases, i.e. arctan(im/re), in degrees
//        "k"    --> X-axis represents the index k in the frequency domain  
//        "f"    --> X-axis represents the fraction f of the sampling rate in the frequency domain
//        "Hz"   --> X-axis represents the actual frequency in Hz in the frequency domain
//        "n"    --> X-axis represents the index n in the time domain  
//        "t"    --> X-axis represents the actual time in seconds in the time domain
//        "2"    --> X-axis spans the full number of data points, instead of the usual (N/2)+1
//
// Note : The options "Hz" and "t" can only be used if the actual data acquisition sampling rate
//        has been provided via the Load() memberfunction.
//
// Examples :
// ----------
// sel="AMP f" will show the (N/2)+1 amplitudes as a function of the fractional sampling rate.
// sel="RE k 2" will show all N real components as a function of the index k in the frequency domain.
//
// The default values are hist=0 and sel="none"
~~~
**/

 fReOut.Set(0);
 fImOut.Set(0);

 if (fN<1) return;

 Int_t n=1+fN/2;
 Float_t maxfrac=0.5;
 if (sel.Contains("n") || sel.Contains("t") || sel.Contains("2"))
 {
  n=fN;
  maxfrac=1;
 }

 // Construct the Fast Fourier Transform (FFT) processor
 TString opt=mode;
 // Comply with the TVirtualFFT conventions
 if (mode=="C2C") opt="C2CFORWARD";
 if (mode=="C2CI") opt="C2CBACKWARD";
 // Set the optimalisation initialisation to "estimate" and create a new processor
 opt+=" ES K";

 if (fProc)
 {
  delete fProc;
  fProc=0;
 }

 fProc=TVirtualFFT::FFT(1,&fN,opt);

 // Enter the input data
 Int_t nReIn=fReIn.GetSize();
 Int_t nImIn=fImIn.GetSize();
 if (mode=="R2C")
 {
  Double_t* data=fReIn.GetArray();
  fProc->SetPoints(data);
 }
 else
 {
  for (Int_t i=0; i<fN; i++)
  {
   if (nReIn && nImIn) fProc->SetPoint(i,fReIn[i],fImIn[i]);
   if (nReIn && !nImIn) fProc->SetPoint(i,fReIn[i],0);
   if (!nReIn && nImIn) fProc->SetPoint(i,0,fImIn[i]);
  }
 }

 // Perform the Fast Fourier Transform
 fProc->Transform();

 Double_t rN=fN;

 // Copy the resulting data in the output arrays
 fReOut.Set(fN);
 fImOut.Set(fN);
 Double_t re=0;
 Double_t im=0;
 for (Int_t i=0; i<fN; i++)
 {
  fProc->GetPointComplex(i,re,im);
  re/=sqrt(rN);
  im/=sqrt(rN);
  fReOut.SetAt(re,i);
  fImOut.SetAt(im,i);
 }

 if (!hist) return;

 if ((sel.Contains("Hz") || sel.Contains("t")) && fSample<=0) return;

 // Initialize the histogram title
 TString title="";
 if (mode=="C2R" || mode=="C2CI") title+="Inverse ";
 title+="DFT (";
 title+=mode;
 title+=") ";
 
 TString title2="";

 // Define and fill the requested result histogram
 if (sel.Contains("k"))
 {
  hist->SetBins(n,0,n-1);
  title+="index frequency domain";
  if (mode=="C2R" || mode=="C2CI")
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Index k";
  if (sel.Contains("RE")) title+=";Re(Q[k])";
  if (sel.Contains("IM")) title+=";Im(Q[k])";
  if (sel.Contains("AMP")) title+=";Amplitude |Q[k]|";
  if (sel.Contains("PHIR")) title+=";Phase #varphi[k] (rad)";
  if (sel.Contains("PHID")) title+=";Phase #varphi[k] (deg)";
 }
 if (sel.Contains("f"))
 {
  hist->SetBins(n,0,maxfrac);
  title+="fractional frequency domain";
  if (mode=="C2R" || mode=="C2CI")
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Fraction f of sampling rate";
  if (sel.Contains("RE")) title+=";Re(Q[f])";
  if (sel.Contains("IM")) title+=";Im(Q[f])";
  if (sel.Contains("AMP")) title+=";Amplitude |Q[f]|";
  if (sel.Contains("PHIR")) title+=";Phase #varphi[f] (rad)";
  if (sel.Contains("PHID")) title+=";Phase #varphi[f] (deg)";
 }
 if (sel.Contains("Hz"))
 {
  hist->SetBins(n,0,maxfrac*fSample);
  title+="actual frequency domain";
  if (mode=="C2R" || mode=="C2CI")
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Frequency #nu (Hz)";
  if (sel.Contains("RE")) title+=";Re(Q[#nu])";
  if (sel.Contains("IM")) title+=";Im(Q[#nu])";
  if (sel.Contains("AMP")) title+=";Amplitude |Q[#nu]|";
  if (sel.Contains("PHIR")) title+=";Phase #varphi[#nu] (rad)";
  if (sel.Contains("PHID")) title+=";Phase #varphi[#nu] (deg)";
 }

 if (sel.Contains("n"))
 {
  hist->SetBins(fN,0,fN);
  title+="sampling time domain";
  if (mode=="R2C" || mode=="C2C") title+=" input";
  if (fSample>0) title2.Form(" (%-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Sample number n";
  if (mode=="R2C" || mode=="C2R")
  {
   title+=";Value X[n]";
  }
  else
  {
   if (sel.Contains("RE")) title+=";Re(X[n])";
   if (sel.Contains("IM")) title+=";Im(X[n])";
   if (sel.Contains("AMP")) title+=";Amplitude |X[n]|";
   if (sel.Contains("PHIR")) title+=";Phase #varphi[n] (rad)";
   if (sel.Contains("PHID")) title+=";Phase #varphi[n] (deg)";
  }
 }
 if (sel.Contains("t"))
 {
  hist->SetBins(fN,0,rN/fSample);
  title+="actual time domain";
  if (mode=="R2C" || mode=="C2C") title+=" input";
  if (fSample>0) title2.Form(" (%-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Time t (seconds)";
  if (mode=="R2C" || mode=="C2R")
  {
   title+=";Value X[t]";
  }
  else
  {
   if (sel.Contains("RE")) title+=";Re(X[t])";
   if (sel.Contains("IM")) title+=";Im(X[t])";
   if (sel.Contains("AMP")) title+=";Amplitude |X[t]|";
   if (sel.Contains("PHIR")) title+=";Phase #varphi[t] (rad)";
   if (sel.Contains("PHID")) title+=";Phase #varphi[t] (deg)";
  }
 }

 hist->SetTitle(title);

 Double_t pi=acos(-1.);
 Double_t amp=0;
 Double_t phi=0;
 re=0;
 im=0;
 for (Int_t i=0; i<n; i++)
 {
  if (sel.Contains("n") || sel.Contains("t")) // Time domain data requested
  {
   if (mode=="R2C")
   {
    if (nReIn) hist->SetBinContent(i+1,fReIn.At(i));
    continue;
   }
   if (mode=="C2R")
   {
    hist->SetBinContent(i+1,fReOut.At(i));
    continue;
   }
   if (mode=="C2C") 
   {
    if (nReIn) re=fReIn.At(i);
    if (nImIn) im=fImIn.At(i);
   }
   if (mode=="C2CI") 
   {
    re=fReOut.At(i);
    im=fImOut.At(i);
   }
  }
  else // Frequency domain data requested
  {
   if (mode=="C2R" || mode=="C2CI") // Inverse transformation
   {
    if (nReIn) re=fReIn.At(i);
    if (nImIn) im=fImIn.At(i);
   }
   else // Forward transformation
   {
    re=fReOut.At(i);
    im=fImOut.At(i);
   }
  }
  amp=sqrt(re*re+im*im);
  phi=0;
  if (im || re) phi=atan2(im,re);

  if (sel.Contains("RE")) hist->SetBinContent(i+1,re);
  if (sel.Contains("IM")) hist->SetBinContent(i+1,im);
  if (sel.Contains("AMP")) hist->SetBinContent(i+1,amp);
  if (sel.Contains("PHIR")) hist->SetBinContent(i+1,phi);
  if (sel.Contains("PHID")) hist->SetBinContent(i+1,phi*180./pi);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Hartley(Int_t mode,TH1* hist,TString sel)
{
/**
~~~
// Perform a normalized 1-dimensional Discrete Hartley Transformation (DHT).
// Actually, a DHT is closely related to a Discrete Fourier Transformation (DFT)
// with only real input values.
// Consequently, the resulting transformed array is also only real.
//
// Indicating in the frequency domain the DHT data elements by H[k] and the 
// DFT data elements by F[k], we have the following relations :
//
// 1) Re(F[k])=(H[k]+H[N-k])/2 and Im(F[k])=(H[N-k]-H[k])/2
// 2) H[k]=Re((1+i)*F[k])
//
// Conventions :
// -------------
// N = The number of data elements
// Time domain array : X[]=X[0],...,X[N-1]
// Frequency domain array : Q[]=Q[0],...,Q[N-1]
//
// Hartley transform : Q[k]=(1/sqrt(N))*sum(n=0,n=N-1){X[n]*[cos(2pi*(k/N)*n)+sin(2pi*(k/N)*n)]}
//
// Inverse Hartley transform : X[n]=(1/sqrt(N))*sum(n=0,k=N-1){Q[k]*[cos(2pi*(n/N)*k)+sin(2pi*(n/N)*k)]}
//
// So, it is seen that the Hartley transform is its own inverse.
//
// Input arguments :
// -----------------
// mode : >0     --> Perform the forward X[n]->Q[k] Hartley transformation
//        <0     --> Perform the backward Q[k]->X[n] Hartley transformation
// hist : (optional) Histogram with selected results
// sel  : String to specify the representation of the result histogram
//        "k"    --> X-axis represents the index k in the frequency domain  
//        "f"    --> X-axis represents the fraction f of the sampling rate in the frequency domain
//        "Hz"   --> X-axis represents the actual frequency in Hz in the frequency domain
//        "n"    --> X-axis represents the index n in the time domain  
//        "t"    --> X-axis represents the actual time in seconds in the time domain
//        "2"    --> X-axis spans the full number of data points, instead of the usual (N/2)+1
//
// Note : The options "Hz" and "t" can only be used if the actual data acquisition sampling rate
//        has been provided via the Load() memberfunction.
//
// Examples :
// ----------
// sel="f" will show the (N/2)+1 amplitudes as a function of the fractional sampling rate.
// sel="k 2" will show all N amplitudes as a function of the index k in the frequency domain.
//
// The default values are hist=0 and sel="none"
~~~
**/

 fReOut.Set(0);
 fImOut.Set(0);

 if (!mode || fN<1) return;

 Int_t n=1+fN/2;
 Float_t maxfrac=0.5;
 if (sel.Contains("n") || sel.Contains("t") || sel.Contains("2"))
 {
  n=fN;
  maxfrac=1;
 }

 // Construct the Fast Fourier Transform (FFT) processor
 if (fProc)
 {
  delete fProc;
  fProc=0;
 }

 fProc=TVirtualFFT::FFT(1,&fN,"DHT ES K");

 // Enter the input data
 Double_t* data=fReIn.GetArray();
 fProc->SetPoints(data);

 // Perform the Discrete Hartley Transform
 fProc->Transform();

 Double_t rN=fN;

 // Copy the resulting data in the output arrays
 fReOut.Set(fN);
 fImOut.Set(0);
 Double_t re=0;
 for (Int_t i=0; i<fN; i++)
 {
  re=fProc->GetPointReal(i,kFALSE);
  re/=sqrt(rN);
  fReOut.SetAt(re,i);
 }

 if (!hist) return;

 if ((sel.Contains("Hz") || sel.Contains("t")) && fSample<=0) return;

 // Initialize the histogram title
 TString title="";
 if (mode<0) title+="Inverse ";
 title+="DHT ";
 
 TString title2="";

 // Define and fill the requested result histogram
 if (sel.Contains("k"))
 {
  hist->SetBins(n,0,n-1);
  title+="index frequency domain";
  if (mode<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Index k;Q[k]";
 }
 if (sel.Contains("f"))
 {
  hist->SetBins(n,0,maxfrac);
  title+="fractional frequency domain";
  if (mode<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Fraction f of sampling rate;Q[f]";
 }
 if (sel.Contains("Hz"))
 {
  hist->SetBins(n,0,maxfrac*fSample);
  title+="actual frequency domain";
  if (mode<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Frequency #nu (Hz);Q[#nu]";
 }

 if (sel.Contains("n"))
 {
  hist->SetBins(fN,0,fN);
  title+="sampling time domain";
  if (mode>0) title+=" input";
  if (fSample>0) title2.Form(" (%-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Sample number n;Value X[n]";
 }
 if (sel.Contains("t"))
 {
  hist->SetBins(fN,0,rN/fSample);
  title+="actual time domain";
  if (mode>0) title+=" input";
  if (fSample>0) title2.Form(" (%-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Time t (seconds);Value X[t]";
 }

 hist->SetTitle(title);

 for (Int_t i=0; i<n; i++)
 {
  if (mode>0) // Forward transform
  {
   if (sel.Contains("n") || sel.Contains("t"))
   {
    hist->SetBinContent(i+1,fReIn.At(i));
   }
   else
   {
    hist->SetBinContent(i+1,fReOut.At(i));
   }
  }
  else // Backward transform
  {
   if (sel.Contains("n") || sel.Contains("t"))
   {
    hist->SetBinContent(i+1,fReOut.At(i));
   }
   else
   {
    hist->SetBinContent(i+1,fReIn.At(i));
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Cosine(Int_t type,TH1* hist,TString sel)
{
/**
~~~
// Perform a normalized 1-dimensional Discrete Cosine Transformation (DCT).
// Actually, this is just a regular Discrete Fourier Transformation (DFT)
// with only real input values which contain an even symmetry.
// Consequently, the resulting transformed array is also only real with even symmetry.
//
// Conventions :
// -------------
// N = The number of data elements
// Time domain array : X[]=X[0],...,X[N-1]
// Frequency domain array : Q[]=Q[0],...,Q[N-1]
//
// Cosine transform type 1 : Q[k]=(1/sqrt(2*(N-1)))*[X[0]+pow(-1,k)*X[N-1]+2*sum(n=1,n=N-2){X[n]*cos(pi*n*k/(N-1))}]
//
// Cosine transform type 2 : Q[k]=(1/sqrt(2N))*2*sum(n=0,n=N-1){X[n]*cos(pi*(n+1)*k/N)}]
//
// Cosine transform type 3 : Q[k]=(1/sqrt(2N))[X[0]+2*sum(n=0,n=N-1){X[n]*cos(pi*n*(k+0.5)/N)}]
//
// Cosine transform type 4 : Q[k]=(1/sqrt(2N))*2*sum(n=0,n=N-1){X[n]*cos(pi*(n+0.5)*(k+0.5)/N)}
//
// Notes :
// -------
// 1) The type 1 transformation is only defined for N>1 and is its own inverse.
// 2) The type 4 transformation is its own inverse.
// 3) The type 3 transformation is the inverse of type 2 (and vice versa).
// 4) The type 2 transformation is often called "the" DCT.
// 5) The type 3 transformation id often called "the" inverse DCT (IDCT).   
//
// Input arguments :
// -----------------
// type : The type of transformation (i.e. 1,2,3 or 4) to be performed
//        The inverse transformations are specified with the corresponding negative type value
// hist : (optional) Histogram with selected results
// sel  : String to specify the representation of the result histogram
//        "k"    --> X-axis represents the index k in the frequency domain  
//        "f"    --> X-axis represents the fraction f of the sampling rate in the frequency domain
//        "Hz"   --> X-axis represents the actual frequency in Hz in the frequency domain
//        "n"    --> X-axis represents the index n in the time domain  
//        "t"    --> X-axis represents the actual time in seconds in the time domain
//        "2"    --> X-axis spans the full number of data points, instead of the usual (N/2)+1
//
// Note : The options "Hz" and "t" can only be used if the actual data acquisition sampling rate
//        has been provided via the Load() memberfunction.
//
// Examples :
// ----------
// sel="f" will show the (N/2)+1 amplitudes as a function of the fractional sampling rate.
// sel="k 2" will show all N amplitudes as a function of the index k in the frequency domain.
//
// The default values are hist=0 and sel="none"
~~~
**/

 fReOut.Set(0);
 fImOut.Set(0);

 if (abs(type)<1 || abs(type)>4 || fN<1 || (abs(type)==1 && fN<2)) return;

 // Convert negative type specifications to the corresponding "normal" ones
 Int_t type2=type;
 if (type==-1 || type==-4) type2=abs(type);
 if (type==-2) type2=3;
 if (type==-3) type2=2;

 Int_t n=1+fN/2;
 Float_t maxfrac=0.5;
 if (sel.Contains("n") || sel.Contains("t") || sel.Contains("2"))
 {
  n=fN;
  maxfrac=1;
 }

 // Comply with the TVirtualFFT conventions
 Int_t kind=type2-1;

 // Construct the Fast Fourier Transform (FFT) processor
 if (fProc)
 {
  delete fProc;
  fProc=0;
 }

 fProc=TVirtualFFT::SineCosine(1,&fN,&kind,"ES");

 // Enter the input data
 Double_t* data=fReIn.GetArray();
 fProc->SetPoints(data);

 // Perform the Discrete Cosine Transform
 fProc->Transform();

 Double_t rN=fN;

 // Copy the resulting data in the output arrays
 fReOut.Set(fN);
 fImOut.Set(0);
 Double_t re=0;
 for (Int_t i=0; i<fN; i++)
 {
  re=fProc->GetPointReal(i,kFALSE);
  if (type2==1)
  {
   re/=sqrt(2.*(rN-1.));
  }
  else
  {
   re/=sqrt(2.*rN);
  }
  fReOut.SetAt(re,i);
 }

 if (!hist) return;

 if ((sel.Contains("Hz") || sel.Contains("t")) && fSample<=0) return;

 // Initialize the histogram title
 TString title="";
 if (type<0) title+="Inverse ";
 title+="DCT-";
 if (abs(type)==1) title+="I";
 if (abs(type)==2) title+="II";
 if (abs(type)==3) title+="III";
 if (abs(type)==4) title+="IV";
 title+=" ";

 TString title2="";

 // Define and fill the requested result histogram
 if (sel.Contains("k"))
 {
  hist->SetBins(n,0,n-1);
  title+="index frequency domain";
  if (type<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Index k;Q[k]";
 }
 if (sel.Contains("f"))
 {
  hist->SetBins(n,0,maxfrac);
  title+="fractional frequency domain";
  if (type<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Fraction f of sampling rate;Q[f]";
 }
 if (sel.Contains("Hz"))
 {
  hist->SetBins(n,0,maxfrac*fSample);
  title+="actual frequency domain";
  if (type<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Frequency #nu (Hz);Q[#nu]";
 }

 if (sel.Contains("n"))
 {
  hist->SetBins(fN,0,fN);
  title+="sampling time domain";
  if (type>0) title+=" input";
  if (fSample>0) title2.Form(" (%-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Sample number n;Value X[n]";
 }
 if (sel.Contains("t"))
 {
  hist->SetBins(fN,0,rN/fSample);
  title+="actual time domain";
  if (type>0) title+=" input";
  if (fSample>0) title2.Form(" (%-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Time t (seconds);Value X[t]";
 }

 hist->SetTitle(title);

 // Determine stepsize in fractional sampling frequency
 Double_t fstep=1./(2.*rN);
 if (type==1) fstep=1./(2.*double(fN-1));

 Double_t x=0;
 for (Int_t i=0; i<n; i++)
 {
  x=i;
  if (type2==3 || type2==4) x+=0.5;
  x*=fstep;

  if (sel.Contains("n") || sel.Contains("t"))
  {
   if (type>0)
   {
    hist->SetBinContent(i+1,fReIn.At(i));
   }
   else
   {
    hist->SetBinContent(i+1,fReOut.At(i));
   }
  }
  else if (sel.Contains("k"))
  {
   if (type>0)
   {
    hist->SetBinContent(i+1,fReOut.At(i));
   }
   else
   {
    hist->SetBinContent(i+1,fReIn.At(i));
   }
  }
  else if (sel.Contains("f"))
  {
   if (type>0)
   {
    hist->Fill(x,fReOut.At(i));
   }
   else
   {
    hist->Fill(x,fReIn.At(i));
   }
  }
  else
  {
   x*=fSample;
   if (type>0)
   {
    hist->Fill(x,fReOut.At(i));
   }
   else
   {
    hist->Fill(x,fReIn.At(i));
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTransform::Sine(Int_t type,TH1* hist,TString sel)
{
/**
~~~
// Perform a normalized 1-dimensional Discrete Sine Transformation (DST).
// Actually, this is just a regular Discrete Fourier Transformation (DFT)
// with only real input values which contain an odd symmetry.
// Consequently, the resulting transformed array is also only real with odd symmetry.
//
// Conventions :
// -------------
// N = The number of data elements
// Time domain array : X[]=X[0],...,X[N-1]
// Frequency domain array : Q[]=Q[0],...,Q[N-1]
//
// Sine transform type 1 : Q[k]=(1/sqrt(2N+1))*2*sum(n=0,n=N-1){X[n]*sin(pi*(n+1)*(k+1)/(N+1))}]
//
// Sine transform type 2 : Q[k]=(1/sqrt(2N))*2*sum(n=0,n=N-1){X[n]*sin(pi*(n+0.5)*(k+1)/N)}
//
// Sine transform type 3 : Q[k]=(1/sqrt(2N))*[pow(-1,k)*X[N-1]+2*sum(n=0,n=N-2){X[n]*sin(pi*(n+1)*(k+0.5)/N)}]
//
// Sine transform type 4 : Q[k]=(1/sqrt(2N))*2*sum(n=0,n=N-1){X[n]*sin(pi*(n+0.5)*(k+0.5)/N)}
//
// Notes :
// -------
// 1) The type 1 transformation is its own inverse.
// 2) The type 4 transformation is its own inverse.
// 3) The type 3 transformation is the inverse of type 2 (and vice versa).
//
// Input arguments :
// -----------------
// type : The type of transformation (i.e. 1,2,3 or 4) to be performed
//        The inverse transformations are specified with the corresponding negative type value
// hist : (optional) Histogram with selected results
// sel  : String to specify the representation of the result histogram
//        "k"    --> X-axis represents the index k in the frequency domain  
//        "f"    --> X-axis represents the fraction f of the sampling rate in the frequency domain
//        "Hz"   --> X-axis represents the actual frequency in Hz in the frequency domain
//        "n"    --> X-axis represents the index n in the time domain  
//        "t"    --> X-axis represents the actual time in seconds in the time domain
//        "2"    --> X-axis spans the full number of data points, instead of the usual (N/2)+1
//
// Note : The options "Hz" and "t" can only be used if the actual data acquisition sampling rate
//        has been provided via the Load() memberfunction.
//
// Examples :
// ----------
// sel="f" will show the (N/2)+1 amplitudes as a function of the fractional sampling rate.
// sel="k 2" will show all N amplitudes as a function of the index k in the frequency domain.
//
// The default values are hist=0 and sel="none"
~~~
**/

 fReOut.Set(0);
 fImOut.Set(0);

 if (abs(type)<1 || abs(type)>4 || fN<1 || (abs(type)==1 && fN<2)) return;

 // Convert negative type specifications to the corresponding "normal" ones
 Int_t type2=type;
 if (type==-1 || type==-4) type2=abs(type);
 if (type==-2) type2=3;
 if (type==-3) type2=2;

 Int_t n=1+fN/2;
 Float_t maxfrac=0.5;
 if (sel.Contains("n") || sel.Contains("t") || sel.Contains("2"))
 {
  n=fN;
  maxfrac=1;
 }

 // Comply with the TVirtualFFT conventions
 Int_t kind=type2+3;

 // Construct the Fast Fourier Transform (FFT) processor
 if (fProc)
 {
  delete fProc;
  fProc=0;
 }

 fProc=TVirtualFFT::SineCosine(1,&fN,&kind,"ES K");

 // Enter the input data
 Double_t* data=fReIn.GetArray();
 fProc->SetPoints(data);

 // Perform the Discrete Cosine Transform
 fProc->Transform();

 Double_t rN=fN;

 // Copy the resulting data in the output arrays
 fReOut.Set(fN);
 fImOut.Set(0);
 Double_t re=0;
 for (Int_t i=0; i<fN; i++)
 {
  re=fProc->GetPointReal(i,kFALSE);
  if (type2==1)
  {
   re/=sqrt(2.*(rN+1.));
  }
  else
  {
   re/=sqrt(2.*rN);
  }
  fReOut.SetAt(re,i);
 }

 if (!hist) return;

 if ((sel.Contains("Hz") || sel.Contains("t")) && fSample<=0) return;

 // Initialize the histogram title
 TString title="";
 if (type<0) title+="Inverse ";
 title+="DST-";
 if (abs(type)==1) title+="I";
 if (abs(type)==2) title+="II";
 if (abs(type)==3) title+="III";
 if (abs(type)==4) title+="IV";
 title+=" ";

 TString title2="";

 // Define and fill the requested result histogram
 if (sel.Contains("k"))
 {
  hist->SetBins(n,0,n-1);
  title+="index frequency domain";
  if (type<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Index k;Q[k]";
 }
 if (sel.Contains("f"))
 {
  hist->SetBins(n,0,maxfrac);
  title+="fractional frequency domain";
  if (type<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Fraction f of sampling rate;Q[f]";
 }
 if (sel.Contains("Hz"))
 {
  hist->SetBins(n,0,maxfrac*fSample);
  title+="actual frequency domain";
  if (type<0)
  {
   title+=" (input)";
  }
  else if (fSample>0)
  {
   title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
   title+=title2;
  }
  title+=";Frequency #nu (Hz);Q[#nu]";
 }

 if (sel.Contains("n"))
 {
  hist->SetBins(fN,0,fN);
  title+="sampling time domain";
  if (type>0) title+=" input";
  if (fSample>0) title2.Form(" (%-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Sample number n;Value X[n]";
 }
 if (sel.Contains("t"))
 {
  hist->SetBins(fN,0,rN/fSample);
  title+="actual time domain";
  if (type>0) title+=" input";
  if (fSample>0) title2.Form(" (%-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Time t (seconds);Value X[t]";
 }

 hist->SetTitle(title);

 // Determine stepsize in fractional sampling frequency
 Double_t fstep=1./(2.*rN);
 if (type==1) fstep=1./(2.*double(fN+1));

 Double_t x=0;
 for (Int_t i=0; i<n; i++)
 {
  x=i+1;
  if (type2==3 || type2==4) x-=0.5;
  x*=fstep;

  if (sel.Contains("n") || sel.Contains("t"))
  {
   if (type>0)
   {
    hist->SetBinContent(i+1,fReIn.At(i));
   }
   else
   {
    hist->SetBinContent(i+1,fReOut.At(i));
   }
  }
  else if (sel.Contains("k"))
  {
   if (type>0)
   {
    hist->SetBinContent(i+1,fReOut.At(i));
   }
   else
   {
    hist->SetBinContent(i+1,fReIn.At(i));
   }
  }
  else if (sel.Contains("f"))
  {
   if (type>0)
   {
    hist->Fill(x,fReOut.At(i));
   }
   else
   {
    hist->Fill(x,fReIn.At(i));
   }
  }
  else
  {
   x*=fSample;
   if (type>0)
   {
    hist->Fill(x,fReOut.At(i));
   }
   else
   {
    hist->Fill(x,fReIn.At(i));
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* NcTransform::Clone(const char* name) const
{
/**
~~~
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.
~~~
**/

 NcTransform* q=new NcTransform(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
