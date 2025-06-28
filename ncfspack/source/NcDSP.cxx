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
/** @class NcDSP
~~~
// Class NcDSP
// Class to perform various Digital Signal Processing (DSP) operations on (sequential) data samples.
//
// For a description of most of the DSP techniques, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// The following discrete transformations (using the FFTW algorithms) are supported :
// Fourier (DFT), Hartley (DHT), Sine (DST) and Cosine (DCT).
//
// All transformation results have been normalized, such that
// the inverse transformation provides the original input spectrum.
//
// In addition to the above transformations, also convolution, correlation, periodogram,
// filter, Analog to Digital Converter (ADC), Digital to Analog Converter (DAC)
// and ADC-DAC chain transmission processors are provided.
//
// For details about the various operations and their options, please refer
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
// NcDSP q;
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
// Usage example for Convolution :
// ------------------------------- 
//
// // The input signal
// const Int_t nx=17;
// Double_t x[nx]={0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0};
// 
// // The system impulse response
// const Int_t nh=10;
// Double_t h[nh]={1,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1};
//
// NcDSP q;
// q.SetSamplingFrequency(1e9); // Sampling frequency in Hz
// q.Load(nx,x);                // Load the signal input data
// q.SetWaveform(nh,h);         // Load the system response data
//
// printf(" Stored elements : Nsignal=%-i  Nwaveform=%-i \n",q.GetN(),q.GetN(1));
//
// // Perform the Convolution
// TH1F hy;
// Int_t smin,smax;
// TArrayD y=q.Convolve(&hy,&smin,&smax);
//
// printf(" Convolution full coverage between sample numbers %-i and %-i \n",smin,smax);
//
// // Plot the various distributions
// TH1F hx("hx","Input signal",nx,0,nx);
// for (Int_t i=1; i<=nx; i++)
// {
//  hx.SetBinContent(i,x[i-1]);
// }
//
// TH1F hh("hh","Impulse response",nh,0,nh);
// for (Int_t i=1; i<=nh; i++)
// {
//  hh.SetBinContent(i,h[i-1]);
// }
//
// TCanvas c("c","Convolution");
// c.Divide(1,3);
// c.cd(1);
// hx.SetMarkerStyle(20);
// hx.Draw("PL");
// c.cd(2);
// hh.SetMarkerStyle(20);
// hh.Draw("PL");
// c.cd(3);
// hy.SetMarkerStyle(20);
// hy.Draw("PL");
//
// Usage example for cross-correlation :
// -------------------------------------
//
// // The input signal
// const Int_t nx=17;
// Double_t x[nx]={0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0};
// 
// // The waveform pattern
// const Int_t nh=10;
// Double_t h[nh]={1,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1};
//
// NcDSP q;
// q.SetSamplingFrequency(1e9); // Sampling frequency in Hz
// q.Load(nx,x);                // Load the signal input data
// q.SetWaveform(nh,h);         // Load the waveform data
//
// printf(" Stored elements : Nsignal=%-i  Nwaveform=%-i \n",q.GetN(),q.GetN(1));
//
// // Perform the Cross-Correlation
// TH1F hy;
// Int_t smin,smax;
// TArrayD y=q.Correlate(&hy,&smin,&smax);
//
// printf(" Correlation full coverage between sample numbers %-i and %-i \n",smin,smax);
//
// // Plot the various distributions
// TH1F hx("hx","Input signal",nx,0,nx);
// for (Int_t i=1; i<=nx; i++)
// {
//  hx.SetBinContent(i,x[i-1]);
// }
//
// TH1F hh("hh","Waveform pattern",nh,0,nh);
// for (Int_t i=1; i<=nh; i++)
// {
//  hh.SetBinContent(i,h[i-1]);
// }
//
// TCanvas c("c","Correlation");
// c.Divide(1,3);
// c.cd(1);
// hx.SetMarkerStyle(20);
// hx.Draw("PL");
// c.cd(2);
// hh.SetMarkerStyle(20);
// hh.Draw("PL");
// c.cd(3);
// hy.SetMarkerStyle(20);
// hy.Draw("PL");
//
// Usage example for a Band Pass filter :
// --------------------------------------
//
// NcDSP q;
// Float_t fsample=1e9; // Sampling frequency in Hz
// q.SetSamplingFrequency(fsample);
// q.Load(nx,x);                    // Load some signal input data
// Double_t f1=200e6/fsample;       // The lower bound (200 MHz) of the frequency band
// Double_t f2=300e6/fsample;       // The upper bound (300 MHz) of the frequency band
// Int_t n=101;     // The number of points describing the filter kernel
// TH1F hisf;       // The frequency domain histogram
// TH1F hist;       // The time domain histogram
// Int_t imin,imax; // The sample number boundaries of the signal for which the kernel is fully embedded
//
// // Perform the filtering
// q.FilterBandPass(f1,f2,n,&hisf,&hist,&imin,&imax);
//
// printf(" Filter kernel full coverage between sample numbers %-i and %-i \n",imin,imax);
//
// TCanvas cf("cf","cf");
// hisf.Draw();
// TCanvas ct("ct","ct");
// hist.Draw();
//
// Usage example for a Multi Band filter :
// ---------------------------------------
//
// NcDSP q;
// Float_t fsample=1e9; // Sampling frequency in Hz
// q.SetSamplingFrequency(fsample);
// q.Load(nx,x);                // Load some signal input data
// Double_t f1=200e6/fsample;   // The lower bound (200 MHz) of the 1st frequency band
// Double_t f2=300e6/fsample;   // The upper bound (300 MHz) of the 1st frequency band
// Double_t f3=450e6/fsample;   // The lower bound (450 MHz) of the 2nd frequency band
// Double_t f4=550e6/fsample;   // The upper bound (550 MHz) of the 2nd frequency band
// TArrayF freqs(4); // Specification of the frequency bands
// freqs[0]=f1;
// freqs[1]=f2;
// freqs[2]=f3;
// freqs[3]=f4;
// Int_t n=101;     // The number of points describing the filter kernel
// TH1F hisf;       // The frequency domain histogram
// TH1F hist;       // The time domain histogram
// Int_t imin,imax; // The sample number boundaries of the signal for which the kernel is fully embedded
//
// // Perform the filtering
// q.FilterMultiBand(freqs,n,&hisf,&hist,&imin,&imax);
//
// printf(" Filter kernel full coverage between sample numbers %-i and %-i \n",imin,imax);
//
// TCanvas cf("cf","cf");
// hisf.Draw();
// TCanvas ct("ct","ct");
// hist.Draw();
//
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, October 19, 2021  09:42Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, June 28, 2025  14:21Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "NcDSP.h"
#include "Riostream.h"
 
ClassImp(NcDSP); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
NcDSP::NcDSP(const char* name,const char* title) : TNamed(name,title)
{
/**
~~~
// Default constructor.
~~~
**/

 fProc=0;
 Reset();
 fSample=0;
 fKeepOutput=kFALSE;
}
///////////////////////////////////////////////////////////////////////////
NcDSP::~NcDSP()
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
NcDSP::NcDSP(const NcDSP& q) : TNamed(q)
{
/**
~~~
// Copy constructor.
~~~
**/

 fProc=0;
 fN=q.fN;
 fNwf=q.fNwf;
 fReIn=q.fReIn;
 fImIn=q.fImIn;
 fReOut=q.fReOut;
 fImOut=q.fImOut;
 fHisto=q.fHisto;
 fWaveform=q.fWaveform;
 fNorm=q.fNorm;
 fSample=q.fSample;
 fKeepOutput=q.fKeepOutput;
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::Reset()
{
/**
~~~
// Internal member function to reset all data and the FFTW processor. 
~~~
**/

 if (fProc)
 {
  delete fProc;
  fProc=0;
 }
 fN=0;
 fNwf=0;
 fReIn.Set(0);
 fImIn.Set(0);
 if (!fKeepOutput)
 {
  fReOut.Set(0);
  fImOut.Set(0);
  fHisto.Set(0);
 }
 fNorm="NONE";
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetSamplingFrequency(Float_t f)
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
Float_t NcDSP::GetSamplingFrequency() const
{
/**
~~~
// Provide the current value of the DAQ sampling frequency in Hz.
~~~
**/

 return fSample;
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::Load(Int_t n,Double_t* re,Double_t* im,Float_t f)
{
/**
~~~
// Provide new input data to be processed and reset the FFTW processor.
//
// Note : The (optional) waveform data stored via SetWaveform() will not be modified.
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
void NcDSP::Load(TArray* re,TArray* im,Float_t f)
{
/**
~~~
// Provide new input data to be processed and reset the FFTW processor.
//
// Note : The (optional) waveform data stored via SetWaveform() will not be modified.
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
void NcDSP::Load(NcSample* s,Int_t i,Float_t f)
{
/**
~~~
// Provide new input data from the NcSample "s" to be processed and reset the FFTW processor.
//
// Note : The (optional) waveform data stored via SetWaveform() will not be modified.
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
void NcDSP::Load(NcSample* s,TString name,Float_t f)
{
/**
~~~
// Provide new input data from the NcSample "s" to be processed and reset the FFTW processor.
//
// Note : The (optional) waveform data stored via SetWaveform() will not be modified.
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
void NcDSP::Load(TH1* h,Float_t f)
{
/**
~~~
// Provide new input data from a 1-Dimensional histogram to be processed and reset the FFTW processor.
//
// Note : The (optional) waveform data stored via SetWaveform() will not be modified.
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
void NcDSP::Load(TGraph* gr,Float_t f)
{
/**
~~~
// Provide new input data from a TGraph object to be processed and reset the FFTW processor.
//
// Note : The (optional) waveform data stored via SetWaveform() will not be modified.
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
void NcDSP::LoadResult()
{
/**
~~~
// Load the current transformation result as new input data in order to enable
// inverse transformations acting on previous transformation results.
//
// Notes :
// -------
// 1) Invokation of this memberfunction will reset all output arrays, so that 
//    previously obtained results will internally be lost.
//    Use the GetData() memberfunction to retrieve data that might be needed later again. 
// 2) Invokation of one of the other Load() memberfunctions will also reset the input arrays.
//    Use the GetData() memberfunction to retrieve data that might be needed later again. 
// 3) The (optional) waveform data stored via SetWaveform() will not be modified.
~~~
**/

 fReIn=fReOut;
 fImIn=fImOut;
 fReOut.Set(0);
 fImOut.Set(0);
 fHisto.Set(0);
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetWaveform(Int_t n,Double_t* h,Float_t f)
{
/**
~~~
// Set the (system response) waveform for Convolution, Correlation etc.
//
// Note : The input data stored via Load() will not be modified.
//
// Input arguments :
// -----------------
// n : The number of data elements
// h : Array with the waveform data
// f : (optional) Actual data acquisition sampling frequency in Hz
//     In case f<0 the current sampling frequency is not modified.
//
// The array size of "h" must be at least of dimension "n".
//
// The default value is f=-1.
~~~
**/

 fWaveform.Set(0);
 fNwf=0;

 if (f>=0) fSample=f;

 if (!h)
 {
  cout << " *" << ClassName() <<"::SetWaveform* No data array specified." << endl;
  return;
 }

 if (n<1) return;

 fWaveform.Set(n);
 fNwf=n;

 for (Int_t i=0; i<n; i++)
 {
  fWaveform[i]=h[i];
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetWaveform(TArray* h,Float_t f)
{
/**
~~~
// Set the (system response) waveform for Convolution, Correlation etc.
//
// Note : The input data stored via Load() will not be modified.
//
// Input arguments :
// -----------------
// h : Array with the waveform data
// f : (optional) Actual data acquisition sampling frequency in Hz
//     In case f<0 the current sampling frequency is not modified.
//
// The default value is f=-1.
~~~
**/

 fWaveform.Set(0);
 fNwf=0;

 if (f>=0) fSample=f;

 if (!h)
 {
  cout << " *" << ClassName() <<"::SetWaveform* No data array specified." << endl;
  return;
 }

 Int_t n=0;
 if (h) n=h->GetSize();

 if (n<1) return;

 fWaveform.Set(n);
 fNwf=n;

 Double_t val=0;
 for (Int_t i=0; i<n; i++)
 {
  val=h->GetAt(i);
  fWaveform[i]=val;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetWaveform(NcSample* s,Int_t i,Float_t f)
{
/**
~~~
// Set the (system response) waveform for Convolution, Correlation etc.
//
// Note : The input data stored via Load() will not be modified.
//
// Input arguments :
// -----------------
// s : NcSample with the waveform data
// i : The data of the i-th variable (1=first) of the NcSample are used
// f : (optional) Actual data acquisition sampling frequency in Hz
//      In case f<0 the current sampling frequency is not modified.
//
// Note : The Store Mode of the NcSample must be activated. 
//
// The default value is f=-1.
~~~
**/

 fWaveform.Set(0);
 fNwf=0;

 if (f>=0) fSample=f;

 if (!s)
 {
  cout << " *" << ClassName() <<"::SetWaveform* No data sample specified." << endl;
  return;
 }

 Int_t n=s->GetN();
 Int_t store=s->GetStoreMode();
 Int_t dim=s->GetDimension();

 if (n<1 || !store || dim<1 || i<1 || i>dim)
 {
  cout << " *" << ClassName() <<"::SetWaveform* Inconsistent input for NcSample treatment." << endl;
  cout << " Store Mode:" << store << " Entries:" << n << " Dimension:" << dim << " i:" << i << " f:" << fSample << endl; 
  return;
 }

 fWaveform.Set(n);
 fNwf=n;

 Double_t val=0;
 for (Int_t idx=1; idx<=n; idx++)
 {
  val=s->GetEntry(idx,i);
  fWaveform[idx-1]=val;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetWaveform(NcSample* s,TString name,Float_t f)
{
/**
~~~
// Set the (system response) waveform for Convolution, Correlation etc.
//
// Note : The input data stored via Load() will not be modified.
//
// Input arguments :
// -----------------
// s    : NcSample with the waveform data
// name : Name of the NcSample variable of which the data are used
// f    : (optional) Actual data acquisition sampling frequency in Hz
//        In case f<0 the current sampling frequency is not modified.
//
// Note : The Store Mode of the NcSample must be activated. 
//
// The default value is f=-1.
~~~
**/

 fWaveform.Set(0);
 fNwf=0;

 if (f>=0) fSample=f;

 if (!s)
 {
  cout << " *" << ClassName() <<"::SetWaveform* No data sample specified." << endl;
  return;
 }

 Int_t n=s->GetN();
 Int_t store=s->GetStoreMode();
 Int_t dim=s->GetDimension();
 Int_t i=s->GetIndex(name);
 

 if (n<1 || !store || dim<1 || !i)
 {
  cout << " *" << ClassName() <<"::SetWaveform* Inconsistent input for NcSample treatment." << endl;
  cout << " Store Mode:" << store << " Entries:" << n << " Dimension:" << dim << " name:" << name << " f:" << fSample << endl; 
  return;
 }

 SetWaveform(s,i,f);
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetWaveform(TH1* h,Float_t f)
{
/**
~~~
// Set the (system response) waveform for Convolution, Correlation etc.
//
// Note : The input data stored via Load() will not be modified.
//
// Input arguments :
// -----------------
// h : Pointer to the 1-D histogram of which the bin contents are used for the waveform data
// f : (optional) Actual data acquisition sampling frequency in Hz
//     In case f<0 the current sampling frequency is not modified.
//
// The default value is f=-1.
~~~
**/

 fWaveform.Set(0);
 fNwf=0;

 if (f>=0) fSample=f;

 if (!h)
 {
  cout << " *" << ClassName() <<"::SetWaveform* No input histogram specified." << endl;
  return;
 }

 Int_t nbins=h->GetNbinsX();
 Double_t nentries=h->GetEntries();

 if (!nbins || !nentries)
 {
  cout << " *" << ClassName() <<"::SetWaveform* Inconsistent input for histogram treatment." << endl;
  cout << " Nbins:" << nbins << " Nentries:" << nentries << " f:" << fSample << endl; 
  return;
 }

 fWaveform.Set(nbins);
 fNwf=nbins;

 Double_t val=0;
 for (Int_t i=1; i<=nbins; i++)
 {
  val=h->GetBinContent(i);
  fWaveform[i-1]=val;;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetWaveform(TGraph* gr,Float_t f)
{
/**
~~~
// Set the (system response) waveform for Convolution, Correlation etc.
//
// Note : The input data stored via Load() will not be modified.
//
// Input arguments :
// -----------------
// gr : Pointer to the TGraph object of which the bin contents are used for the waveform data
// f  : (optional) Actual data acquisition sampling frequency in Hz
//      In case f<0 the current sampling frequency is not modified.
//
// The default value is f=-1.
~~~
**/

 fWaveform.Set(0);
 fNwf=0;

 if (f>=0) fSample=f;

 if (!gr)
 {
  cout << " *" << ClassName() <<"::SetWaveform* No input TGraph object specified." << endl;
  return;
 }

 Int_t n=gr->GetN();

 if (!n)
 {
  cout << " *" << ClassName() <<"::SetWaveform* Inconsistent input for TGraph treatment." << endl;
  cout << " n:" << n << " f:" << fSample << endl; 
  return;
 }

 fWaveform.Set(n);
 fNwf=n;

 gr->Sort();

 Double_t x=0;
 Double_t y=0;
 for (Int_t i=0; i<n; i++)
 {
  gr->GetPoint(i,x,y);
  fWaveform[i]=y;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcDSP::GetN(Int_t mode) const
{
/**
~~~
// Provide the number of data elements (to be) processed.
//
// mode : 0 --> Provide the number of input data elements entered via Load()
//        1 --> Provide the number of input data elements entered via SetWaveform()
//
// The default value is mode=0 for backward compatibility.
~~~
**/

 Int_t n=fN;

 if (mode==1) n=fNwf;

 return n;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::GetData(TString sel) const
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
//        "Hist" --> The contents of the selected transformation histogram (if any) are provided.
//        "Wave" --> The values of the stored (system response) waveform data are provided
//
// Examples :
// ----------
// sel="AMP out" will provide all the N amplitudes of the resulting data after transformation.
// sel="RE in" will provide all the N real components of the input data.
// sel="Wave" will provide all the amplitudes of the stored (system response) waveform
~~~
**/

 if (sel.Contains("RE") && sel.Contains("in")) return fReIn;
 if (sel.Contains("IM") && sel.Contains("in")) return fImIn;
 if (sel.Contains("RE") && sel.Contains("out")) return fReOut;
 if (sel.Contains("IM") && sel.Contains("out")) return fImOut;
 if (sel.Contains("Hist")) return fHisto;
 if (sel.Contains("Wave")) return fWaveform;

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
void NcDSP::Fourier(TString mode,TH1* hist,TString sel)
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
//        "dB"   --> Y-axis shows the values of the amplitudes, i.e. sqrt(re*re+im*im), in decibel
//        "PHIR" --> Y-axis shows the values of the phases, i.e. arctan(im/re), in radians
//        "PHID" --> Y-axis shows the values of the phases, i.e. arctan(im/re), in degrees
//        "CPHI" --> Y-axis shows the values of cos(phase), i.e. cos(arctan(im/re))
//        "SPHI" --> Y-axis shows the values of sin(phase), i.e. sin(arctan(im/re))
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
// sel="AMP f"  will show the (N/2)+1 amplitudes as a function of the fractional sampling rate.
// sel="dB f"   will show the (N/2)+1 amplitudes in dB as a function of the fractional sampling rate.
// sel="RE k 2" will show all N real components as a function of the index k in the frequency domain.
//
// Note : If only the X-axis variable is specified, the Y-axis will show "RE" in the corresponding domain.
// So, sel="t" is equivalent to sel="RE t" etc... 
//
// The default values are hist=0 and sel="none".
~~~
**/

 fReOut.Set(0);
 fImOut.Set(0);
 fHisto.Set(0);

 if (fN<1) return;

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
  fReOut[i]=re;
  fImOut[i]=im;
 }

 Int_t imode=1;
 if (mode=="C2R" || mode=="C2CI") imode=-1;
 TString name;
 name.Form("DFT (%-s)",mode.Data());
 if (imode<0) name.Form("Inverse DFT (%-s)",mode.Data());

 // Check for only domain specification specification
 TString opts[8]={"RE","IM","AMP","dB","PHIR","PHID","CPHI","SPHI"};
 Bool_t found=0;
 for (Int_t i=0; i<8; i++)
 {
  if (sel.Contains(opts[i]))
  {
   found=1;
   break;
  }  
 }

 if (!found)
 {
  if (sel.Contains("n") || sel.Contains("t")) // Time domain
  {
   if (mode=="C2C" || mode=="C2CI") sel+=" RE";
  }
  else // Frequency domain
  {
   sel+=" RE";
  }
 }

 // Fill the requested histogram
 HistogramTrafoResult(name,imode,hist,sel);
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::Hartley(Int_t mode,TH1* hist,TString sel)
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
// sel="f" will show the (N/2)+1 amplitudes as a function of the fractional sampling rate in the frequency domain.
// sel="k 2" will show all N amplitudes as a function of the index k in the frequency domain.
//
// The default values are hist=0 and sel="none"
~~~
**/

 fReOut.Set(0);
 fImOut.Set(0);
 fHisto.Set(0);

 if (!mode || fN<1) return;

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

 // Fill the requested histogram
 TString name="Discrete Hartley Transform (DHT)";
 if (mode<0) name="Inverse Discrete Hartley Transform (IDHT)";

 HistogramTrafoResult(name,mode,hist,sel);
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::Cosine(Int_t type,TH1* hist,TString sel)
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
// Time domain array .... : X[]=X[0],...,X[N-1]
// Frequency domain array : Q[]=Q[0],...,Q[N-1]
//
// Cosine transform type 1 : Q[k]=(1/sqrt(2*(N-1)))*[X[0]+pow(-1,k)*X[N-1]+2*sum(n=1,n=N-2){X[n]*cos(pi*n*k/(N-1))}]
//
// Cosine transform type 2 : Q[k]=(1/sqrt(2N))*2*sum(n=0,n=N-1){X[n]*cos(pi*(n+0.5)*k/N)}]
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
 fHisto.Set(0);

 if (abs(type)<1 || abs(type)>4 || fN<1 || (abs(type)==1 && fN<2)) return;

 // Convert negative type specifications to the corresponding "normal" ones
 Int_t type2=type;
 if (type==-1 || type==-4) type2=abs(type);
 if (type==-2) type2=3;
 if (type==-3) type2=2;

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
  fReOut[i]=re;
 }

 TString name="Discrete Cosine Transform (DCT-";
 if (type<0) name="Inverse Discrete Cosine Transform (IDCT-";
 if (abs(type)==1) name+="I)";
 if (abs(type)==2) name+="II)";
 if (abs(type)==3) name+="III)";
 if (abs(type)==4) name+="IV)";

 // Fill the requested histogram
 HistogramTrafoResult(name,type,hist,sel);
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::Sine(Int_t type,TH1* hist,TString sel)
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
// Time domain array .... : X[]=X[0],...,X[N-1]
// Frequency domain array : Q[]=Q[0],...,Q[N-1]
//
// Sine transform type 1 : Q[k]=(1/sqrt(2*(N+1)))*[2*sum(n=0,n=N-1){X[n]*sin(pi*(n+1)*(k+1)/(N+1))}]
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
 fHisto.Set(0);

 if (abs(type)<1 || abs(type)>4 || fN<1 || (abs(type)==1 && fN<2)) return;

 // Convert negative type specifications to the corresponding "normal" ones
 Int_t type2=type;
 if (type==-1 || type==-4) type2=abs(type);
 if (type==-2) type2=3;
 if (type==-3) type2=2;

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
  fReOut[i]=re;
 }

 TString name="Discrete Sine Transform (DST-";
 if (type<0) name="Inverse Discrete Sine Transform (IDST-";
 if (abs(type)==1) name+="I)";
 if (abs(type)==2) name+="II)";
 if (abs(type)==3) name+="III)";
 if (abs(type)==4) name+="IV)";

 // Fill the requested histogram
 HistogramTrafoResult(name,type,hist,sel);
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::Hilbert(Int_t mode,TH1* hist,TString sel)
{
/**
~~~
// Perform a 1-dimensional Discrete Hilbert Transformation (DHIT).
// Actually, a DHIT is closely related to a Discrete Fourier Transformation (DFT)
// with only real input values, followed by a phase shift and inverse DFT.
// Consequently, the transformation results in a time domain array of only real values.
//
// Practical application :
// -----------------------
// Since the time domain input consists only of real values, the input signal can
// be fully described by cos() terms only.
// Consider in the time domain the following time series (x) that results from
// a carrier wave that is modulated both in amplitude (A) and phase (P) :
//
//             x(t)=A(t)cos(P(t))
//
// The Hilbert transformation allows to extract A(t) and P(t) from the time series x(t)
// via the so called complex analytic signal Z[] as outlined below.
// The values of A(t) provide the so-called the Hilbert envelope, and will always by positive or 0.
// The instantaneous angular frequency omega(t) is obtained via omega(t)=dP(t)/dt, whereas
// the values of cos(P(t)) provide the so-called Temporal Fine Structure (TFS).
//
// Conventions :
// -------------
// N = The number of data elements
// Time domain input array .............................. : X[]=X[0],...,X[N-1]
// Fourier transform array DFT(X) in the frequency domain : Q[]=Q[0],...,Q[N-1]
// Hilbert transform array DHIT(X) in the time domain ... : HT[]=HT[0],...,HT[N-1]
// Analytic signal array in the time domain ............. : Z[]=Z[0],...,Z[N-1]
//
// In this memberfunction, the Hilbert transformation action is executed in the frequency domain,
// after which the time domain result HT[] is obtained via an inverse Fourier transformation.
//
// Indicating in the frequency domain the (intermediate) DHIT data elements by H[k],
// we have the following relations :
//
// H[k] = -i*Q[k] for positive frequencies (which in our convention corresponds to 1<=k<=N/2)
//         0      for frequency 0          (which in our convention corresponds to k=0)
//         i*Q[k] for negative frequencies (which in our convention corresponds to k>N/2)
//
// In other words : If Q[k] is represented as Q[k]=a+b*i then we have :
//
// H[k] =  b-a*i for positive frequencies
//         0     for frequency 0
//        -b+a*i for negative frequencies
//
// From this we obtain via an inverse DFT on H[k] the Hilbert transform HT[n] in the time domain.
//
// The instantaneous amplitude A(t) and phase P(t) can be obtained via the so-called analytic signal Z(t) :
//
// Z[n]=X[n]+i*HT[n] which can be represented as Z[n]=c+d*i
//
// where A[n]=|Z[n]|=sqrt(c*c+d*d) and P[n]=arctan(d/c).
//
// Inverse transformation :
// ------------------------
// From the above procedure it is seen that DHIT(HT[])=-X[], which is used to perform the inverse transformation HT[]->X[].
// For the result of the inverse transformation HT[]->X[] we keep the definition Z[n]=X[n]+i*HT[n], which allows
// to also extract the instantaneous amplitude A(t) and phase P(t) from the inverse transformation result. 
// The inverse Hilbert transformation has been implemented for completeness, but in practice this will not be used often.
// 
// Input arguments :
// -----------------
// mode : >0      --> Perform the forward X[n]->Z[n] Hilbert transformation
//        <0      --> Perform the backward HT[n]->X[n] Hilbert transformation
// hist : (optional)  Histogram with selected results
// sel  : String to specify the representation of the result histogram
//        "X"     --> Y-axis shows the values of the real X[] components
//        "HT"    --> Y-axis shows the values of the real HT[] components
//        "AMP"   --> Y-axis shows the values of the amplitudes, i.e. sqrt(re*re+im*im)
//        "dB"    --> Y-axis shows the values of the amplitudes, i.e. sqrt(re*re+im*im), in decibel
//        "PHIR"  --> Y-axis shows the values of the phases, i.e. arctan(im/re), in radians
//        "PHID"  --> Y-axis shows the values of the phases, i.e. arctan(im/re), in degrees
//        "CPHI"  --> Y axis shows the values of cos(phase), i.e. cos(arctan(im/re))
//                    This is also called the Temporal Fine Structure (TFS).   
//        "OMEGA" --> Y axis shows the values of angular frequency in rad/sec, i.e. d(arctan(im/re))/dt
//        "n"     --> X-axis represents the index n in the time domain  
//        "t"     --> X-axis represents the actual time in seconds in the time domain
//
// Note : The option "t" can only be used if the actual data acquisition sampling rate
//        has been provided via the Load() memberfunction.
//
// Examples :
// ----------
// sel="AMP t" will show the amplitudes as a function of the actual time in seconds.
// sel="PHID n" will show the phases in degrees as a function of the index n in the time domain.
// sel="HT n" will show the values of HT[] as a function of the index n in the time domain.
//
// Note :
// ------
// sel="t" for the forward transformation is equivalent to sel="HT t"
// sel="t" for the inverse transformation is equivalent to sel="X t"
// sel="n" for the forward transformation is equivalent to sel="HT n"
// sel="n" for the inverse transformation is equivalent to sel="X n"
//
// The default values are hist=0 and sel="none"
~~~
**/

 fReOut.Set(0);
 fImOut.Set(0);
 fHisto.Set(0);

 if (!mode || fN<1) return;

 // Save the original input array
 TArrayD temp=fReIn;

 // Perform the DFT on X[] to obtain Q[]
 Fourier("R2C");

 // Perform the Hilbert transformation in the frequency domain
 Double_t a=0;
 Double_t b=0;
 Double_t re=0;
 Double_t im=0;
 for (Int_t k=0; k<fN; k++)
 {
  a=fReOut[k];
  b=fImOut[k];
  re=0;
  im=0;
  if (k && k<=fN/2)
  {
   re=b;
   im=-a;
  }
  if (k>fN/2)
  {
   re=-b;
   im=a;
  }
  fReOut[k]=re;
  fImOut[k]=im;
 }

 // Perform the inverse DFT on H[k] to obtain HT[n]
 TArrayD ReZ=fReIn;  // The real part of the analytic signal
 LoadResult();
 Fourier("C2R");
 TArrayD ImZ=fReOut; // The imaginary part of the analytic signal

 // Provide the analytic signal Z[n]=X[n]+i*HT[n] as intermediate output result
 if (mode>0) // Forward transformation
 {
  fReOut=ReZ;
  fImOut=ImZ;
 }
 else // Backward transformation
 {
  fImOut=ReZ;
  for (Int_t n=0; n<fN; n++)
  {
   fReOut[n]=-ImZ[n];
  } 
 }

 // Restore the original input array
 fReIn=temp;   

 // Check for only "n" or "t" specification
 TString opts[8]={"X","HT","AMP","dB","PHIR","PHID","CPHI","OMEGA"};
 Bool_t found=0;
 for (Int_t i=0; i<8; i++)
 {
  if (sel.Contains(opts[i]))
  {
   found=1;
   break;
  }  
 }

 if (!found)
 {
  if (mode>0) // Forward transformation
  {
   sel+=" HT";
  }
  else // Inverse transformation
  {
   sel+=" X";
  }
 }

 // Convert the "X"and "HT" selections to the corresponding "RE" and "IM'.
 sel.ReplaceAll("X","RE");
 sel.ReplaceAll("HT","IM");

 // Fill the requested histogram
 TString name="Discrete Hilbert Transform (DHIT)";
 if (mode<0) name="Inverse Discrete Hilbert Transform (IDHIT)";

 HistogramTrafoResult(name,mode,hist,sel);

 // Store the correct data array as real output
 if (mode>0) fReOut=ImZ; // Forward transformation
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::HistogramTrafoResult(TString name,Int_t mode,TH1* hist,TString sel)
{
/**
~~~
// Internal memberfunction to provide a histogram with the requested transformation result.
// The histogram contents are stored in the fHisto array, which can be accessed via GetData().
// 
// Input arguments :
// -----------------
// mode : >0      --> Result of a forward transformation
//        <0      --> Result of a backward transformation
// hist :             Histogram with selected results
// sel  : String to specify the representation of the result histogram
//        "RE"    --> Y-axis shows the values of the real (re) components
//        "IM"    --> Y-axis shows the values of the imaginary (im) components
//        "AMP"   --> Y-axis shows the values of the amplitudes, i.e. sqrt(re*re+im*im)
//        "dB"    --> Y-axis shows the values of the amplitudes, i.e. sqrt(re*re+im*im), in decibel
//        "PHIR"  --> Y-axis shows the values of the phases, i.e. arctan(im/re), in radians
//        "PHID"  --> Y-axis shows the values of the phases, i.e. arctan(im/re), in degrees
//        "CPHI"  --> Y axis shows the values of cos(phase), i.e. cos(arctan(im/re))
//        "SPHI"  --> Y axis shows the values of cos(phase), i.e. cos(arctan(im/re))
//        "OMEGA" --> Y axis shows the values of the angular frequency in rad/sec, i.e. d(arctan(im/re))/dt
//        "k"     --> X-axis represents the index k in the frequency domain  
//        "f"     --> X-axis represents the fraction f of the sampling rate in the frequency domain
//        "Hz"    --> X-axis represents the actual frequency in Hz in the frequency domain
//        "n"     --> X-axis represents the index n in the time domain  
//        "t"     --> X-axis represents the actual time in seconds in the time domain
//
// Note : The options "Hz", "t" and "OMEGA" can only be used if the actual data acquisition sampling rate
//        has been provided via the Load() memberfunction.
//
// Examples :
// ----------
// sel="AMP t" will show the amplitudes as a function of the actual time in seconds.
// sel="PHID n" will show the phases in degrees as a function of the index n in the time domain.
~~~
**/

 fHisto.Set(0);

 if (!mode || fN<1 || !hist) return;

 Int_t n=1+fN/2;
 Float_t maxfrac=0.5;
 if (sel.Contains("n") || sel.Contains("t") || sel.Contains("2"))
 {
  n=fN;
  maxfrac=1;
 }

 if ((sel.Contains("Hz") || sel.Contains("t") || sel.Contains("OMEGA")) && fSample<=0) return;

 if (sel.Contains("OMEGA") && !(sel.Contains("n")) && !(sel.Contains("t"))) return; 

 hist->Reset();

 // Initialize the histogram title
 TString title=name;
 title+=" ";
 
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
  title+=";Index k";
  if (sel.Contains("RE")) title+=";Re(Q[k])";
  if (sel.Contains("IM")) title+=";Im(Q[k])";
  if (sel.Contains("AMP")) title+=";Amplitude |Q[k]|";
  if (sel.Contains("dB")) title+=";Amplitude |Q[k]| in dB";
  if (sel.Contains("PHIR")) title+=";Phase #varphi[k] (rad)";
  if (sel.Contains("PHID")) title+=";Phase #varphi[k] (deg)";
  if (sel.Contains("CPHI")) title+=";cos(#varphi[k])";
  if (sel.Contains("SPHI")) title+=";sin(#varphi[k])";
  if (!(title.Contains("[k]")))
  {
   sel+=" RE";
   title+=";Value Q[k]";
  }
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
  title+=";Fraction f of sampling rate";
  if (sel.Contains("RE")) title+=";Re(Q[f])";
  if (sel.Contains("IM")) title+=";Im(Q[f])";
  if (sel.Contains("AMP")) title+=";Amplitude |Q[f]|";
  if (sel.Contains("dB")) title+=";Amplitude |Q[f]| in dB";
  if (sel.Contains("PHIR")) title+=";Phase #varphi[f] (rad)";
  if (sel.Contains("PHID")) title+=";Phase #varphi[f] (deg)";
  if (sel.Contains("CPHI")) title+=";cos(#varphi[f])";
  if (sel.Contains("SPHI")) title+=";sin(#varphi[f])";
  if (!(title.Contains("[f]")))
  {
   sel+=" RE";
   title+=";Value Q[f]";
  }
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
  title+=";Frequency #nu (Hz)";
  if (sel.Contains("RE")) title+=";Re(Q[#nu])";
  if (sel.Contains("IM")) title+=";Im(Q[#nu])";
  if (sel.Contains("AMP")) title+=";Amplitude |Q[#nu]|";
  if (sel.Contains("dB")) title+=";Amplitude |Q[#nu]| in dB";
  if (sel.Contains("PHIR")) title+=";Phase #varphi[#nu] (rad)";
  if (sel.Contains("PHID")) title+=";Phase #varphi[#nu] (deg)";
  if (sel.Contains("CPHI")) title+=";cos(#varphi[#nu])";
  if (sel.Contains("SPHI")) title+=";sin(#varphi[#nu])";
  if (!(title.Contains("[#nu]")))
  {
   sel+=" RE";
   title+=";Value Q[#nu]";
  }
 }

 if (sel.Contains("n"))
 {
  hist->SetBins(fN,0,fN);
  title+="sampling time domain";
  if (mode>0 && !(name.Contains("Hilbert"))) title+=" input";
  if (fSample>0) title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Sample number n";
  if (name.Contains("Hilbert"))
  {
   if (sel.Contains("RE")) title+=";Value X[n]";
   if (sel.Contains("IM")) title+=";Value HT[n]";
   if (sel.Contains("AMP")) title+=";Amplitude |Z[n]|";
   if (sel.Contains("dB")) title+=";Amplitude |Z[n]| in dB";
  }
  else
  {
   if (sel.Contains("RE")) title+=";Re(X[n])";
   if (sel.Contains("IM")) title+=";Im(X[n])";
   if (sel.Contains("AMP")) title+=";Amplitude |X[n]|";
   if (sel.Contains("dB")) title+=";Amplitude |X[n]| in dB";
  }
  if (sel.Contains("PHIR")) title+=";Phase #varphi[n] (rad)";
  if (sel.Contains("PHID")) title+=";Phase #varphi[n] (deg)";
  if (sel.Contains("CPHI")) title+=";cos(#varphi[n])";
  if (sel.Contains("SPHI")) title+=";sin(#varphi[n])";
  if (sel.Contains("OMEGA")) title+=";#omega[n] (rad/s)";
  if (!(title.Contains("[n]")))
  {
   sel+=" RE";
   title+=";Value X[n]";
  }
 }
 if (sel.Contains("t"))
 {
  hist->SetBins(fN,0,double(fN)/fSample);
  title+="actual time domain";
  if (mode>0 && !(name.Contains("Hilbert"))) title+=" input";
  if (fSample>0) title2.Form(" (DAQ: %-.3g samples/sec)",fSample);
  title+=title2;
  title+=";Time t (seconds)";
  if (name.Contains("Hilbert"))
  {
   if (sel.Contains("RE")) title+=";Value X[t]";
   if (sel.Contains("IM")) title+=";Value HT[t]";
   if (sel.Contains("AMP")) title+=";Amplitude |Z[t]|";
   if (sel.Contains("dB")) title+=";Amplitude |Z[t]| in dB";
  }
  else
  {
   if (sel.Contains("RE")) title+=";Re(X[t])";
   if (sel.Contains("IM")) title+=";Im(X[t])";
   if (sel.Contains("AMP")) title+=";Amplitude |X[t]|";
   if (sel.Contains("dB")) title+=";Amplitude |X[t]| in dB";
  }
  if (sel.Contains("PHIR")) title+=";Phase #varphi[t] (rad)";
  if (sel.Contains("PHID")) title+=";Phase #varphi[t] (deg)";
  if (sel.Contains("CPHI")) title+=";cos(#varphi[t])";
  if (sel.Contains("SPHI")) title+=";sin(#varphi[t])";
  if (sel.Contains("OMEGA")) title+=";#omega[t] (rad/s)";
  if (!(title.Contains("[t]")))
  {
   sel+=" RE";
   title+=";Value X[t]";
  }
 }

 hist->SetTitle(title);

 Int_t nReIn=fReIn.GetSize();
 Int_t nImIn=fImIn.GetSize();
 Int_t nReOut=fReOut.GetSize();
 Int_t nImOut=fImOut.GetSize();
 Double_t pi=acos(-1.);
 Double_t amp=0;
 Double_t phi=0;
 Double_t cphi=0;
 Double_t sphi=0;
 Double_t omega=0;
 Double_t re=0;
 Double_t im=0;
 Double_t re2=0;
 Double_t im2=0;
 Double_t phi2=0;
 fHisto.Set(n);

 // Determine stepsize in fractional sampling frequency for the Cosine and Sine transformation
 Double_t fstep=1./(2.*double(fN));
 if (name.Contains("DCT-I") && fN>1) fstep=1./(2.*double(fN-1));
 if (name.Contains("DST-I")) fstep=1./(2.*double(fN+1));

 Double_t x=-1;
 for (Int_t i=0; i<n; i++)
 {
  if (name.Contains("DCT-"))
  {
   x=i;
   if (mode==3 || mode==-2 || abs(mode)==4) x+=0.5;
  }
  if (name.Contains("DST-"))
  {
   x=i+1;
   if (mode==3 || mode==-2 || abs(mode)==4) x-=0.5;
  }
  x*=fstep;
  if (sel.Contains("Hz")) x*=fSample;

  if (sel.Contains("n") || sel.Contains("t")) // Time domain data requested
  {
   if (mode>0) // Forward transformation
   {
    if (name.Contains("Hilbert"))
    {
     if (nReOut)
     {
      re=fReOut[i];
      if (i<n-1) re2=fReOut[i+1];
     }
     if (nImOut)
     {
      im=fImOut[i];
      if (i<n-1) im2=fImOut[i+1];
     }
    }
    else
    {
     if (nReIn) re=fReIn[i];
     if (nImIn) im=fImIn[i];
    }
   }
   if (mode<0) // Inverse transformation
   {
    if (nReOut) re=fReOut[i];
    if (nImOut) im=fImOut[i];
    if (name.Contains("Hilbert") && i<n-1)
    {
     if (nReOut) re2=fReOut[i+1];
     if (nImOut) im2=fImOut[i+1];
    }
   }
  }
  else // Frequency domain data requested
  {
   if (mode<0) // Inverse transformation
   {
    if (nReIn) re=fReIn[i];
    if (nImIn) im=fImIn[i];
   }
   else // Forward transformation
   {
    if (nReOut) re=fReOut[i];
    if (nImOut) im=fImOut[i];
   }
  }
  if (name.Contains("DCT-") || name.Contains("DST-"))
  {
   if (sel.Contains("f") || sel.Contains("Hz"))
   {
    hist->Fill(x,re);
   }
   else
   {
    hist->SetBinContent(i+1,re);
   }
  }
  else
  {
   amp=sqrt(re*re+im*im);
   phi=0;
   if (im || re) phi=atan2(im,re);
   cphi=cos(phi);
   sphi=sin(phi);
   phi2=0;
   if (fSample>0 && i<n-1 && (im2 || re2))
   {
    phi2=atan2(im2,re2);
    if ((phi2*phi)>=0) omega=fabs(phi2-phi)*fSample;
   }
   if (sel.Contains("RE")) hist->SetBinContent(i+1,re);
   if (sel.Contains("IM")) hist->SetBinContent(i+1,im);
   if (sel.Contains("AMP")) hist->SetBinContent(i+1,amp);
   if (amp<=0) amp=hist->GetMinimum();
   if (sel.Contains("dB"))
   {
    // Check for rounding errors
    if (amp<=0)
    {
     amp=hist->GetMinimum();
     hist->SetBinContent(i+1,amp);
    }
    else
    {
     hist->SetBinContent(i+1,20.*log10(amp));
    }
   }
   if (sel.Contains("PHIR")) hist->SetBinContent(i+1,phi);
   if (sel.Contains("PHID")) hist->SetBinContent(i+1,phi*180./pi);
   if (sel.Contains("CPHI")) hist->SetBinContent(i+1,cphi);
   if (sel.Contains("SPHI")) hist->SetBinContent(i+1,sphi);
   if (sel.Contains("OMEGA")) hist->SetBinContent(i+1,omega);
  }
 }

 // Fill the fHisto array with the histogram contents
 for (Int_t i=0; i<n; i++)
 {
  fHisto[i]=hist->GetBinContent(i+1);
 }
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::Convolve(TH1* hist,Int_t* i1,Int_t* i2,Int_t shift)
{
/**
~~~
// Convolve the loaded input data x[] with the data contained in the (system response)
// waveform h[] and return the resulting data y[] in a TArrayD object.
// The input data x[] have to be entered as real numbers by one of the Load() member functions,
// whereas the (system response) waveform h[] has to be specified by SetWaveform().
// The provided data of x[] and h[] are not modified.
//
// In formula : y[]=x[]*h[].
//
// The convolution of two (time) series expresses how the shape of one
// is modified by the other, which makes it a versatile tool to describe
// system responses, digital filtering processing, superposition of various influences
// in physical systems etc.
// The result y[] can be regarded as the weighted sum (or pdf) of X+H, where 
// X and H are two independent random variables with as pdf x[] and h[], respectively.
// Note that here x[]*h[] is identical to h[]*x[].
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// hist : (optional) Histogram with the convolution result
//
// The (optional) arguments i1 and i2 provide the range [i1,i2] in the
// resulting convolved data array for which "h" was fully immersed in the
// input (signal) data x[].
// In other words : The indices range [i1,i2] in the resulting convolved data array y[]
// corresponds to all the y[] elements for which the convolution was completely performed
// by using all the elements of h[].
// So, values of y[j] with j<i1 or j>i2 contain incomplete convolutions, and as such should not
// be considered as reliable, especially when x[] and/or h[] contain large variations.
//
// The argument "shift" determines whether the Xaxis values of the (optional) result histogram
// are shifted or that the array y[] is truncated in order to provide (a view of) the resulting array y[]
// such that it can be directly compared to the original array x[] (see notes 2) and 3) below).
//
// The convention is :
// shift = 0 --> No shift and the full array y[] is returned and shown "as is" in the (optional) histogram.
//         1 --> The full array y[] is returned and shown in the (optional) histogram with y[i1] at the center.
//               This mode is particulary useful for correlation studies between two distributions [see Correlate()],
//               since it will show both negative and positive values at the Xaxis of the (optional) histogram,
//               and the self-correlation peak coincides with the value 0 at the Xaxis.  
//         2 --> The array y[] contains only the modified x[] elements and is shown with y[0] at the start of the (optional) histogram.
//               This mode is particulary useful for digital filtering processes (see the various filtering member functions),
//               since it allows to directly compare the filtered and the unfiltered x[] distribution.
//
// Notes :
// -------
// 1) The sampling (rate) of h[] has to be the same as for the input data x[].
// 2) Array sizes of x[nx] and h[nh] will result in a convolved data array of size y[(nx+nh-1)],
//    except for shift=2 for which the convolved data array has size y[nx]. 
// 3) For an absolute comparison between the x[] and y[] values, one should realize that 
//    the array sizes of x[] and y[] are different and that x[k] should be compared with y[k+i1].
// 4) The values i1 and i2 (if requested) are indicated by vertical dashed blue lines
//    in the time domain histogram.
//
// The default values are hist=0, i1=0, i2=0 and shift=0.
~~~
**/

 TArrayD y(0);
 if (hist) hist->Reset();

 TArrayD x=fReIn;
 TArrayD h=fWaveform;
 Int_t nx=x.GetSize();
 Int_t nh=h.GetSize();

 if (nh<1 || nx<1)
 {
  printf(" *%-s::Convolve* Input or Waveform data are missing. Ninput=%-i Nwaveform=%-i \n",ClassName(),nx,nh);
  return y;
 }

 if (shift<0 || shift>2)
 {
  printf(" *%-s::Convolve* Invalid input argument shift=%-i \n",ClassName(),shift);
  return y;
 }

 Int_t ny=nx+nh-1;
 y.Set(ny);

 // The normalization data for correlation studies
 TArrayD x1(ny);
 TArrayD x2(ny);
 TArrayD h1(ny);
 TArrayD h2(ny);
 TArrayI ns(ny);

 // Convolution from the input signal viewpoint
 for (Int_t ix=0; ix<nx; ix++)
 {
  for (Int_t ih=0; ih<nh; ih++)
  {
   // Perform the unnormalized convolution
   y[ix+ih]=y[ix+ih]+x[ix]*h[ih];

   if (fNorm=="NONE" || fNorm=="GNCC") continue;

   // Gather the normalization data for the "NCC" and "ZNCC" cross-correlation studies
   x1[ix+ih]+=x[ix];
   x2[ix+ih]+=pow(x[ix],2);
   h1[ix+ih]+=h[ih];
   h2[ix+ih]+=pow(h[ih],2);
   ns[ix+ih]+=1;
  }
 }

 // Normalize values for cross-correlation studies if requested
 if (fNorm!="NONE")
 {
  Double_t val=0;
  if (fNorm=="GNCC") // Global vector length normalization
  {
   Double_t xnorm=0;
   Double_t hnorm=0;
   for (Int_t i=0; i<nx; i++)
   {
    xnorm+=pow(x[i],2);
   }
   for (Int_t i=0; i<nh; i++)
   {
    hnorm+=pow(h[i],2);
   }
   xnorm=sqrt(xnorm);
   hnorm=sqrt(hnorm);
   for (Int_t i=0; i<ny; i++)
   {
    val=xnorm*hnorm;
    if (val>0) y[i]=y[i]/val;
   }
  }
  else if (fNorm=="NCC") // NCC normalization
  {
   for (Int_t i=0; i<ny; i++)
   {
    val=sqrt(x2[i]*h2[i]);
    if (val>0) y[i]=y[i]/val;
   }
  }
  else // ZNCC normalization
  {
   y.Reset();
   Double_t x1mean=0;
   Double_t h1mean=0;
   Double_t x2mean=0;
   Double_t h2mean=0;
   Double_t rn=0;
   Double_t sx=0;
   Double_t sh=0;
   Double_t sxsh=0;
   Int_t iy=0;
   for (Int_t ix=0; ix<nx; ix++)
   {
    for (Int_t ih=0; ih<nh; ih++)
    { 
     iy=ix+ih;

     if (!ns[iy]) continue;

     rn=ns[iy];
     x1mean=x1[iy]/rn;
     x2mean=x2[iy]/rn;
     h1mean=h1[iy]/rn;
     h2mean=h2[iy]/rn;
     sx=sqrt(x2mean-pow(x1mean,2));
     sh=sqrt(h2mean-pow(h1mean,2));
     sxsh=sx*sh;
     val=0;
     if (sxsh>0) val=(x[ix]-x1mean)*(h[ih]-h1mean)/(rn*sxsh);
     y[iy]+=val;
    }
   }
  }
 }

 Int_t ilow=nh-1;
 Int_t iup=ny-nh;

 Int_t ioffset=0;
 if (shift==2)
 {
  ioffset=nh/2;
  TArrayD temp(nx);
  for (Int_t i=0; i<nx; i++)
  {
   temp[i]=y[i+ioffset];
  }
  y=temp;
  ny=nx;
  ilow=ilow-ioffset;
  iup=iup-ioffset;
 }

 if (i1) *i1=ilow;
 if (i2) *i2=iup;

 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   title.Form("NcDSP Convolution result (%-g samples/sec);Time in seconds;Value",fSample);
   if (shift==0 || shift==2)
   {
    hist->SetBins(ny,0,double(ny)/fSample);
   }
   else
   {
    hist->SetBins(ny,double(-nh+1)/fSample,double(nx)/fSample);
   }
  }
  else
  {
   title.Form("NcDSP Convolution result;Sample number;Value");
   if (shift==0 || shift==2)
   {
    hist->SetBins(ny,0,ny);
   }
   else
   {
    hist->SetBins(ny,-nh+1,nx);
   }
  }

  hist->SetTitle(title);
  hist->SetMarkerStyle(20);
  for (Int_t ibin=1; ibin<=ny; ibin++)
  {
   hist->SetBinContent(ibin,y[ibin-1]);
  }

  Double_t ymin=hist->GetMinimum();
  Double_t ymax=hist->GetMaximum();

  Double_t xlow=0;
  Double_t xup=0;
  if (i1)  xlow=hist->GetBinLowEdge(ilow+1);
  if (i2)
  {
   xup=hist->GetBinLowEdge(iup+1);
   xup+=hist->GetBinWidth(1);
  }

  TLine* vline1=0;
  TLine* vline2=0;

  if (i1)
  {
   vline1=new TLine(xlow,ymin,xlow,ymax);
   vline1->SetLineStyle(2); // Dashed line
   vline1->SetLineWidth(2);
   vline1->SetLineColor(4); // Blue color
  }
  if (i2)
  {
   vline2=new TLine(xup,ymin,xup,ymax);
   vline2->SetLineStyle(2); // Dashed line
   vline2->SetLineWidth(2);
   vline2->SetLineColor(4); // Blue color
  }

   TList* hlist=hist->GetListOfFunctions();
   if (vline1) hlist->Add(vline1);
   if (vline2) hlist->Add(vline2);
 }

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::Correlate(TH1* hist,Int_t* i1,Int_t* i2,Double_t* peak,TString norm)
{
/**
~~~
// (Cross) Correlate the data contained in the waveform h[] with the loaded input data x[]
// and return the resulting data y[] in a TArrayD object.
// The input data x[] have to be entered as real numbers by one of the Load() member functions,
// whereas the waveform h[] has to be specified by SetWaveform().
// The provided data of x[] and h[] are not modified.
//
// In formula : y[]=h[]*x[].
//
// The cross-correlation is a measure of similarity of two (time) series as a function
// of the displacement of one relative to the other.
// The result y[] can be regarded as the weighted sum (or pdf) of X-H, where 
// X and H are two independent random variables with as pdf x[] and h[], respectively.
// Note that here h[]*x[] is different from x[]*h[].
//
// Mathematically it is seen that the cross-correlation h[]*x[] is equivalent
// to the convolution of h[] and x[], but with the ordering of the elements
// of the distribution h[] reversed.
// 
// In formula : Cross-correlation h[m]*x[n] is equivalent to Convolution h[-m]*x[n].
//
// This feature is used here to centralize the computation in the member function Convolve(),
// so that also this Correlate() processor will automatically profit from possible CPU speed
// improvements in the Convolution processor.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// hist : (optional) Histogram with the correlation result
//
// The (optional) arguments i1 and i2 provide the range [i1,i2] in the
// resulting correlated data array for which "h" was fully immersed in the
// input (signal) data.
// These values i1 and i2 (if requested) are indicated by vertical
// dashed blue lines in the histogram.
//
// The (optional) argument "peak" provides the location of the correlation peak, indicating
// the shift (aka lag) of the h[] pattern w.r.t. the best matching pattern in x[], with the convention
// that the autocorrelation peak falls at 0.
//
// The (optional) argument "norm" allows to obtain the normalized correlation values.
// When normalized, the resulting correlation values will be in the interval [-1,1],
// which facilitates an interpretation of the amount of correlation.
// However, it should be realized that different normalizations may result in different cross-correlation profiles,
// as outlined below.
//
// norm = "NONE" --> No normalization
//        "GNCC" --> Same as norm="NONE" but the resulting correlation values will be divided by (|h|*|x|) of the total vectors h[] and [x] 
//        "NCC"  --> NCC  normalization, i.e. Sum{h[i]*x[i]/(|h|*|x|)}, where |h| and |x| are the vector lengths of the overlapping parts
//        "ZNCC" --> ZNCC normalization, i.e. (1/N)*Sum{(h[i]-hmean)*(x[i]-xmean)/(sx*sh)}, where sx and sh are the STDs of the overlapping parts
//
// The "NCC" and "ZNCC" normalizations are preferred when looking for pattern matches, irrespective of the amplitudes.
// This will provide consistent results for different amplitudes of x[] and/or h[], but may yield incorrect results when
// looking for a (time) lag between two pulses on top of some background noise.
// The "GNCC" normalization is preferred in case amplitude matches, next to pattern matches, are also relevant.
// This is the preferred method when looking for a (time) lag between two pulses on top of some background noise,
// but in case of a bad SNR, the results may become unreliable.
//
// Note : The sampling (rate) of h has to be the same as for the loaded input data x[].
//
// The default values are hist=0, i1=0, i2=0, peak=0 and norm="NONE".
~~~
**/

 TArrayD y(0);
 if (hist) hist->Reset();
 Int_t nx=fReIn.GetSize();
 Int_t nh=fWaveform.GetSize();

 if (nh<1 || nx<1)
 {
  printf(" *%-s::Correlate* Input or Waveform data are missing. Ninput=%-i Nwaveform=%-i \n",ClassName(),nx,nh);
  return y;
 }

 norm.ToUpper(); // Convert norm to uppercase characters
 if (norm!="NONE" && norm!="GNCC" && norm!="NCC" && norm!="ZNCC")
 {
  printf(" *%-s::Correlate* Unsupported normalization mode : norm=%-s \n",ClassName(),norm.Data());
  return y;
 }

 // The temporary "flipped" waveform
 TArrayD store=fWaveform;
 TArrayD temp(nh);
 for (Int_t i=1; i<=nh; i++)
 {
  temp[i-1]=fWaveform[nh-i];
 }

 fWaveform=temp;
 fNorm=norm; // Normalize the correlation values if requested
 y=Convolve(hist,i1,i2,1);
 fNorm="NONE"; // Reset the normalization indicator for convolution studies

 // Get the index of the maximum in the y[] array
 Int_t imax=0;
 Double_t ymax=0;
 Int_t ny=y.GetSize();
 if (ny) ymax=y[0];
 for (Int_t i=1; i<ny; i++)
 {
  if (y[i]>ymax)
  {
   ymax=y[i];
   imax=i;
  }
 }

 // Convert to the index matching the x[] convention
 Int_t idx=imax-nh+1;
 Double_t xpeak=idx;
 if (fSample>0) xpeak=xpeak/fSample;

 if (peak) *peak=xpeak;

 // Put the correct histogram title
 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   if (norm=="NCC")
   {
    title.Form("%-s Normalized Cross-Correlation (NCC): max. at lag=%-g sec. (%-g samples/sec)",ClassName(),xpeak,fSample);
   }
   else if (norm=="ZNCC")
   {
    title.Form("%-s Zero-Normalized Cross-Correlation (ZNCC): max. at lag=%-g sec. (%-g samples/sec)",ClassName(),xpeak,fSample);
   }
   else if (norm=="GNCC")
   {
    title.Form("%-s Globally Normalized Cross-Correlation (GNCC): max. at lag=%-g sec. (%-g samples/sec)",ClassName(),xpeak,fSample);
   }
   else
   {
    title.Form("%-s Unnormalized Cross-Correlation: max. at lag=%-g sec. (%-g samples/sec)",ClassName(),xpeak,fSample);
   }
   title+=";Time lag in seconds;Cross-Correlation value";
  }
  else
  {
   if (norm=="NCC")
   {
    title.Form("%-s Normalized Cross-Correlation (NCC): max. at lag=%-i samplings",ClassName(),idx);
   }
   else if (norm=="ZNCC")
   {
    title.Form("%-s Zero-Normalized Cross-Correlation (ZNCC): max. at lag=%-i samplings",ClassName(),idx);
   }
   else if (norm=="GNCC")
   {
    title.Form("%-s Gobally Normalized Cross-Correlation (GNCC): max. at lag=%-i samplings",ClassName(),idx);
   }
   else
   {
    title.Form("%-s Unnormalized Cross-Correlation: max. at lag=%-i samplings",ClassName(),idx);
   }
   title+=";Lag in samplings;Cross-Correlation value";
  }
  hist->SetTitle(title);
 }

 // Restore the original waveform
 fWaveform=store;

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::Digitize(Int_t nbits,Double_t vcal,Int_t mode,TH1* hist,Double_t* stp,Double_t* scale) const
{
/**
~~~
// **************************************************************************************
// *** This function has become obsolete and is only kept for backward compatibility. ***
// *** Please refer to the new, more flexible memberfunction Transmit().              ***
// *** The user can also invoke the ADC and DAC processors individually by means of   ***
// *** the corresponding memberfunctions ADC() and DAC().                             ***
// **************************************************************************************
// 
// Digitize the values of the stored waveform according to an "nbits" ADC.
// The resulting digitized values are returned in a TArrayD object,
// without modification of the original waveform data.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// nbits >0 : Digitization of the values will be performed using nbits.
//       <0 : Digitization of the Log10 of the values will be performed using |nbits|.
//            After digitization of the Log10 value, the digitized result (digval) is
//            used to store the corresponding linear value via value=pow(10,digval).
//            So, nbits<0 emulates a Log10 ADC to enhance the dynamic range.
//            Note : When nbits<0 all values to be digitized should be positive.
// vcal     : Range calibration value of the ADC according to "mode" as indicated below.
// mode  =0 : Range for the digitized result (digval) will be set to [0,vcal] (or [vcal,0] if vcal<0).
//        1 : Full scale range for the digitized result (digval) will be set to [-|vcal|,|vcal|].
//        2 : A step size of |vcal| is used providing a digval interval of [0,scale] (or [-scale,0] if vcal<0).
//        3 : A step size of |vcal| is used providing a digval interval of [-scale,scale].
//
// Optional output arguments :
// ---------------------------
// hist  : Histogram with the digitized result
// stp   : The value of "step size".
// scale : The value of "scale".
//
// Notes :
// -------
// 1) The step size corresponds closely to the Least Significant Bit (LSB) precision for the
//    digitized result (digval).
//    For an n-bit ADC we have stepsize=range/(-1+2^n), whereas LSB=1/(2^n).
// 2) In case of a Log10 ADC, the value of "vcal" relates to the Log10 values.
//    So, for a Log10 ADC, the "vcal" interval [-2,2] represents linear values [0.01,100].
//    A step size of 0.0315 for an 8-bit Log10 ADC provides a Log10 interval of [-4,4]
//    for mode=3, which corresponds to a linear value interval of [0.0001,10000].
//    For mode=2 this would result in a Log10 interval of [0,8.0325], which corresponds
//    to a linear value interval of [1,1.0777e8].
//    A step size of 0.01 between the Log10 values corresponds to a multiplication factor
//    of 10^0.01 (i.e. about 1.023) for the linear values at each step. 
// 3) In case no waveform is present, just the specs of the specified ADC performance will be printed,
//    but no digitization is performed.
//    This allows to quickly investigate various scenarios without any data treatment.
//         
// The maximum number of bits that is supported is 60 to guarantee identical functioning
// on all machines.
//
// In case of inconsistent input parameters, no digitization is performed and an empty TArrayD is returned.
//
// The default values are hist=0, stp=0 and scale=0.
~~~
**/

 TArrayD arrdig(0);
 if (hist) hist->Reset();

 if (mode<0 || mode>3)
 {
  cout << " *" << ClassName() << "::Digitize* Inconsistent input mode=" << mode << endl;
  return arrdig;
 }

 if (!nbits || nbits>60 || nbits<-60 || fabs(vcal)<=0)
 {
  cout << " *" << ClassName() << "::Digitize* Inconsistent input nbits=" << nbits << " vcal=" << vcal << endl;
  return arrdig;
 }

 if ((mode==1 || mode==3) && nbits==1)
 {
  cout << " *" << ClassName() << "::Digitize* Inconsistent input nbits=" << nbits << " mode=" << mode << endl;
  return arrdig;
 }

 Bool_t logmode=kFALSE;
 if (nbits<0) logmode=kTRUE;

 nbits=abs(nbits);

 Long_t nlevels=pow(2,nbits);
 Double_t range=fabs(vcal);
 if (mode==1 || mode==3) range*=2.;
 Double_t step=fabs(vcal);
 if (mode==0) step=range/double(nlevels-1);
 if (mode==1) step=range/double(nlevels-2);

 if (step<=0) return arrdig;

 Long_t nstepsmin=0;
 Long_t nstepsmax=nlevels-1;
 if ((mode==0 || mode==2) && vcal<0)
 {
  nstepsmin=-nlevels+1;
  nstepsmax=0;
 }
 if (mode==1 || mode==3)
 {
  nstepsmin=-nlevels/2;
  nstepsmax=nlevels/2-1;
 }

 Double_t digvalmin=double(nstepsmin)*step;
 Double_t digvalmax=double(nstepsmax)*step;

 cout << " *" << ClassName() << "::Digitize* Parameters of the " << nbits << "-bits";
 if (logmode) cout << " Log10";
 cout << " ADC digitization." << endl;
 TString s="linear";
 if (logmode) s="Log10";
 if (mode==0 || mode==2)
 {
  cout << " Digitized " << s << " full scale range : [" << digvalmin << "," << digvalmax << "]  Step size : " << step << endl;
 }
 if (mode==1 || mode==3)
 {
  cout << " Digitized " << s << " full scale range : [" << (digvalmin+step) << "," << digvalmax << "]  Step size : " << step;
  cout << "  Actual range : [" << digvalmin << "," << digvalmax << "]" << endl;
 }

 if (logmode)
 {
  Double_t linvalmin=pow(10,digvalmin);
  Double_t linvalmax=pow(10,digvalmax);
  if (mode==0 || mode==2)
  {
   cout << " Digitized linear full scale range : [" << linvalmin << "," << linvalmax << "]" << endl;
  }
  if (mode==1 || mode==3)
  {
   cout << " Digitized linear full scale range : [" << (linvalmin*pow(10,step)) << "," << linvalmax << "]";
   cout << "  Actual range : [" << linvalmin << "," << linvalmax << "]" << endl;
  }
 }

 if (stp) *stp=step;
 if (scale)
 {
  *scale=fabs(vcal);
  if (mode==2)
  {
   if (vcal<0) *scale=digvalmin;
   if (vcal>0) *scale=digvalmax;
  }
  if (mode==3) *scale=digvalmax;
 }
 
 if (fNwf<1)
 {
  printf(" === No waveform data present: Only listing of ADC specs.===\n");
  return arrdig;
 }

 arrdig.Set(fNwf);

 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   hist->SetBins(fNwf,0,double(fNwf)/fSample);
   title.Form("NcDSP Digitize result (%-g samples/sec);Time in seconds;Value",fSample);
  }
  else
  {
   title.Form("NcDSP Digitize result;Sample number;Value");
   hist->SetBins(fNwf,0,fNwf);
  }
  hist->SetMarkerStyle(20);
  hist->SetTitle(title);
 }

 Double_t val=0;
 Long_t nsteps=0;
 Double_t digval=0;
 for (Int_t j=0; j<fNwf; j++)
 {
  val=fWaveform[j];
  
  if (logmode)
  {
   if (val>0)
   {
    val=log10(val);
   }
   else
   {
    cout << endl;
    cout << " *" << ClassName() << "::Digitize* Error: Non-positive input value encountered for Log10 ADC." << endl;
    arrdig.Set(0);
    return arrdig;
   }
  }
  nsteps=val/step;

  if (nsteps<nstepsmin) nsteps=nstepsmin;
  if (nsteps>nstepsmax) nsteps=nstepsmax;

  digval=double(nsteps)*step;

  if (logmode) digval=pow(10,digval);

  arrdig[j]=digval;

  if (hist) hist->SetBinContent(j+1,digval);
 }

 return arrdig;
}
///////////////////////////////////////////////////////////////////////////
TArrayL64 NcDSP::ADC(Int_t nbits,Double_t range,Double_t Vbias,TArray* Vsig,TH1* hist,Int_t B,Int_t C) const
{
/**
~~~
// Processing of an Analog to Digital Converter (ADC). 
// Construct from analog input signals the discrete quantized data values of an "nbits" ADC,
// based on the "range" for the analog signal and a bias voltage "Vbias" (see below).
// The analog input signals may be provided by the (optional) TArray "Vsig".
// In case the array "Vsig" is not provided, the stored waveform is used to provide the analog input signals.
// The resulting (integer) quantized ADC values are returned in a TArrayL64 object,
// without modification of the original waveform data.
//
// Note : Make sure to use the same units for "range", "Vbias" and the analog input signals.
//
// The number of available quantization levels is given by N=2^|nbits|, of which the lowest
// level represents the value 0. This yields for the quantized values (adc) the range [0,N-1].
// The mapping of an analog input voltage (Vin) to one of the quantization levels depends on the
// specified "range" and "Vbias" and whether we have a linear or Log ADC (see below).
//
// As outlined below, the range of the analog input voltage can be specified as the Full Scale
// voltage range (Vfs) corresponding to adc=N-1 or as the Reference voltage (Vref) corresponding to
// the hypothetical adc=N.
// 
// The Least Significant Bit (LSB) represents the smallest analog input voltage interval
// that can reliably be resolved. In other words LSB=Voltage(adc=1)-Voltage(adc=0).
// For a linear ADC we have : LSB=Vfs/(N-1) or equivalently LSB=Vref/N.
// For a Log_B ADC (see below) we have : LSB=Vref*pow(B,-C)*(pow(B,C/N)-1).
//
// The formulas for the Vin->adc mapping with Vin=(Vbias+Vsig) are :
// Linear ADC : adc=Vin/LSB.
// Log_B ADC (see below) : adc=(N/C)*Log_B(pow(B,C)*Vin/Vref).
//
// Which leads to the following relations between Vref and Vfs : 
// Linear ADC : Vref=Vfs+LSB.
// Log_B ADC (see below) : Vfs=Vref*pow(B,-C)*pow(B,(N-1)*C/N).
//
// The Dynamic Range (DR) is defined as the ratio of the largest and smallest input voltages
// that can reliably be resolved.
// Expressed in decibel we have : DR=20*log_10(Vfs/LSB) dB.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// and the publication by Y. Sundarasaradula et al. :
// "A 6-bit, Two-step, Successive Approximation Logarithmic ADC for Biomedical Applications"
// which is online available at https://spiral.imperial.ac.uk/bitstream/10044/1/44156/2/2016_ICECS_LogADC_Camera.pdf.
//
// Input arguments :
// -----------------
// nbits    : Digital quantization of the input values will be performed using nbits.
// range >0 : The full scale voltage range (Vfs) of the analog signal that corresponds to adc=N-1. 
//       <0 : |range| is the reference voltage (Vref) of the analog signal that corresponds to the hypothetical adc=N.
// Vbias    : The bias voltage that will be added to the analog input signal before digitization.
// Vsig     : (Optional) array to contain the analog input signals.
// B     >1 : Base for a Log ADC (e.g. B=10 emulates a Log_10 ADC).
//       =0 : The ADC will be linear
//       =1 : The ADC will be a Log_e ADC.
//            Note : When B>0 all (Vbias+Vsig) input values should be positive.
// C        : Code efficiency factor for a Log ADC.
//            Rule of thumb : pow(B,-C) is about the smallest signal/|range| ratio that can be resolved.
//            So, for a log_10 ADC with C=3, the smallest signal that can be resolved is about |range|/1000.
//            Note : It is required that C>0.
//
// Optional output arguments :
// ---------------------------
// hist  : Histogram with the digitized result
//
// Notes :
// -------
// 1) In case no "Vsig" array is provided and no waveform is present, just the ADC specs will be printed
//    and in the TArrayL64 only the adc value corresponding to Vbias is returned.
//    This allows to quickly investigate various scenarios without any data treatment.
// 2) Providing a "Vsig" array with different small (random) amplitudes allows to mimic variations of the bias or
//    characteristics of the various switched capacitor array elements for the individual samplings.
//    This resembles a so called "pedestal run" in real life Data Acquisition (DAQ) applications.
//    For a linear ADC, the contents of the returned TArrayL64 can be used afterwards for pedestal subtraction.  
//         
// The maximum number of bits that is supported is 60 to guarantee identical functioning
// on all machines.
//
// In case of inconsistent input parameters, no digitization is performed and an empty TArrayL64 is returned.
//
// The default values are Vbias=0, Vsig=0, hist=0, B=0 and C=3.
~~~
**/

 TArrayL64 arradc(0);
 if (hist) hist->Reset();

 if (!Vsig) Vsig=(TArray*)&fWaveform;
 Int_t nVsig=Vsig->GetSize();

 if (nbits<=0 || nbits>60 || !range || fabs(Vbias)>fabs(range) || B<0 || (B && C<1))
 {
  printf("\n *%-s::ADC* Inconsistent input nbits=%-i range=%-g Vbias=%-g B=%-i C=%-i \n",ClassName(),nbits,range,Vbias,B,C);
  return arradc;
 }

 NcMath math;

 Long64_t N=pow(2,nbits); // The number of quantization levels
 Long64_t adcmin=0;
 Long64_t adcmax=N-1;
 Double_t Vref=0;
 Double_t Vfs=0;
 Double_t LSB=0;

 // Floating point version of some parameters
 Double_t rN=N;
 Double_t rB=B;
 if (B==1) rB=exp(1);
 Double_t rC=C;
 Double_t radcmax=adcmax;

 if (range<0) // |range| represents Vref
 {
  Vref=fabs(range);
  LSB=Vref/rN;
  Vfs=Vref-LSB;
 }
 else // range represents Vfs
 {
  Vfs=range;
  LSB=Vfs/radcmax;
  Vref=Vfs+LSB;
 }

 Long64_t ped=0;
 Double_t frac=0;
 if (B) // Log ADC
 {
  if (range<0) Vfs=pow(rB,-rC)*pow(rB,radcmax*rC/rN)*Vref;
  if (range>0) Vref=pow(rB,rC)*pow(rB,-radcmax*rC/rN)*Vfs;
  LSB=Vref*pow(rB,-rC)*(pow(rB,rC/rN)-1.);
  frac=Vbias/Vref;
  ped=0;
  if (frac>0) ped=Long64_t(rN*(math.Log(rB,frac)+rC)/rC);
 }

 if (LSB<=0 || Vfs<=0)
 {
  printf("\n *%-s::ADC* Inconsistent parameters : LSB=%-g Vfs=%-g \n",ClassName(),LSB,Vfs);
  return arradc;
 }

 if (!B) ped=Vbias/LSB; // Pedestal value for a linear ADC

 Double_t DR=20.*log10(Vfs/LSB);

 TString mode="linear";
 if (B==1) mode="Log_e";
 if (B>1)
 {
  mode="Log_";
  mode+=B;
 }

 if (!nVsig)
 {
  printf(" *%-s::ADC* No input data have been provided --> Only the value of adc(Vbias) is returned. \n",ClassName());
  if (!B)
  {
   printf(" Parameters for a %-i-bits %-s ADC with adc=[%-lli,%-lli] : \n",nbits,mode.Data(),adcmin,adcmax);
   printf(" Vref=%-g Vfs=%-g LSB=%-g DR=%-g (dB) Vbias=%-g adc(Vbias)=%-lli \n",Vref,Vfs,LSB,DR,Vbias,ped);
  }
  else
  {
   printf(" Parameters for a %-i-bits %-s ADC with adc=[%-lli,%-lli] and a code efficiency factor of %-i: \n",nbits,mode.Data(),adcmin,adcmax,C);
   printf(" Vref=%-g Vfs=%-g LSB=%-g DR=%-g (dB) Vbias=%-g adc(Vbias)=%-lli \n",Vref,Vfs,LSB,DR,Vbias,ped);
  }
  arradc.Set(1);
  arradc[0]=ped;
  return arradc;
 }

 arradc.Set(nVsig);

 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   hist->SetBins(nVsig,0,double(nVsig)/fSample);
   title.Form("%-s %-i-bits %-s ADC with Vfs=%-g LSB=%-g (%-g samples/sec);Time in seconds;ADC counts",ClassName(),nbits,mode.Data(),Vfs,LSB,fSample);
  }
  else
  {
   title.Form("%-s %-i-bits %-s ADC with Vfs=%-g LSB=%-g;Sample number;ADC counts",ClassName(),nbits,mode.Data(),Vfs,LSB);
   hist->SetBins(nVsig,0,nVsig);
  }
  hist->SetMarkerStyle(20);
  hist->SetTitle(title);
 }

 Double_t val=0;
 Double_t radcval=0;
 Long64_t adcval=0;
 for (Int_t j=0; j<nVsig; j++)
 {
  val=Vbias+Vsig->GetAt(j);
  
  if (B) // Log ADC
  {
   radcval=0;
   frac=val/Vref;
   if (frac>0) radcval=(rN/rC)*(math.Log(rB,frac)+rC);
  }
  else // Linear ADC
  {
   radcval=val/LSB;
  }

  adcval=Long64_t(radcval);

  // Check for saturation
  if (adcval<adcmin) adcval=adcmin;
  if (adcval>adcmax) adcval=adcmax;

  arradc[j]=adcval;

  if (hist) hist->SetBinContent(j+1,adcval);
 }

 return arradc;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::DAC(Int_t nbits,Double_t range,Double_t Vbias,TArray* adcs,TArray* peds,TH1* hist,Int_t B,Int_t C) const
{
/**
~~~
// Processing of a Digital to Analog Converter (DAC). 
// Reconstruct the analog signals based on the discrete quantized digital data from an "nbits" ADC,
// based on the "range" for the analog signal and a bias voltage "Vbias" or array "peds" of pedestal values (see below).
// The digital input signals may be provided by the (optional) TArray "adcs".
// In case the array "adcs" is not provided, the stored waveform is used to provide the digital input signals.
// The resulting analog values are returned in a TArrayD object, without modification of the original waveform data.
//
// Note : Make sure to use the same units for "range" and "Vbias".
//
// The number of digital quantization levels is given by N=2^|nbits|, of which the lowest
// level represents the value 0. This implies a range [0,N-1] for the various digital adc values.
// The correspondance of an analog input voltage (Vin) to one of the quantization levels depends on the
// specified "range" and "Vbias" and whether we have a linear or Log DAC (see below).
//
// As outlined below, the range of the analog input voltage can be specified as the Full Scale
// voltage range (Vfs) corresponding to adc=N-1 or as the Reference voltage (Vref) corresponding to
// the hypothetical adc=N.
// 
// The Least Significant Bit (LSB) represents the smallest analog input voltage interval
// that can reliably be resolved. In other words LSB=Voltage(adc=1)-Voltage(adc=0).
// For a linear DAC we have : LSB=Vfs/(N-1) or equivalently LSB=Vref/N.
// For a Log_B DAC (see below) we have : LSB=Vref*pow(B,-C)*(pow(B,C/N)-1).
//
// The formulas for the adc->Vin mapping with Vin=(Vbias+Vsig) are :
// Linear DAC : Vin=adc*LSB.
// Log_B DAC (see below) : Vin=Vref*pow(B,-C)*pow(B,C*adc/N)
//
// Which implies the following relations between Vref and Vfs : 
// Linear DAC : Vref=Vfs+LSB.
// Log_B DAC (see below) : Vfs=Vref*pow(B,-C)*pow(B,C*(N-1)/N).
//
// The Dynamic Range (DR) is defined as the ratio of the largest and smallest analog voltages
// that can reliably be resolved.
// Expressed in decibel we have : DR=20*log_10(Vfs/LSB) dB.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// and the publication by Y. Sundarasaradula et al. :
// "A 6-bit, Two-step, Successive Approximation Logarithmic ADC for Biomedical Applications"
// which is online available at https://spiral.imperial.ac.uk/bitstream/10044/1/44156/2/2016_ICECS_LogADC_Camera.pdf.
//
// Input arguments :
// -----------------
// nbits    : Digitial quantization was performed using an nbits ADC.
// range >0 : The full scale voltage range (Vfs) of the analog signal that corresponds to adc=N-1. 
//       <0 : |range| is the reference voltage (Vref) of the analog signal that corresponds to the hypothetical adc=N.
// Vbias    : The bias voltage that was added to the analog input signal before digitization.
//            If specified, the resulting analog signals will be corrected for the bias voltage.
//            For a linear DAC the correction via Vbias will only be performed if no pedestal array "peds"
//            is specified (see below). If the array "peds" is specified, Vbias will be ignored for a linear DAC.
//            For a Log DAC the pedestal values will never be used and bias correction may only be obtained via Vbias.
// adcs     : (Optional) array to contain the digital input signals.
// peds     : (Optional) array to contain the pedestal values for the individual digital input signals.
//            The array "peds" must contain (at least) the same number of values as the number of digital input signals.
//            If the array "peds" is specified, the pedestals will be subtracted from the corresponding digital input signals
//            before the conversion to analog signals is performed in case of a linear DAC.
//            For a Log DAC the pedestal values will never be used and bias correction may only be obtained via Vbias.
// B     >1 : Base for a Log DAC (e.g. B=10 emulates a Log_10 DAC).
//       =0 : The DAC will be linear
//       =1 : The DAC will be a Log_e DAC.
// C        : Code efficiency factor that was used for a Log ADC.
//            Rule of thumb : pow(B,-C) is about the smallest signal/|range| ratio that can be resolved.
//            So, for a Log_10 ADC with C=3, the smallest signal that can be resolved is about |range|/1000.
//            Note : It is required that C>0.
//
// Optional output arguments :
// ---------------------------
// hist  : Histogram with the analog result
//
// Notes :
// -------
// 1) In case no "adcs" array is provided and no waveform is present, just the DAC specs will be printed
//    and in the TArrayD only the adc value corresponding to Vbias is returned.
//    This allows to quickly investigate various scenarios without any data treatment.
//         
// The maximum number of bits that is supported is 60 to guarantee identical functioning
// on all machines.
//
// In case of inconsistent input parameters, no processing is performed and an empty TArrayD is returned.
//
// The default values are Vbias=0, adcs=0, peds=0, hist=0, B=0 and C=3.
~~~
**/

 TArrayD arrdac(0);
 if (hist) hist->Reset();

 if (!adcs) adcs=(TArray*)&fWaveform;
 Int_t nadcs=adcs->GetSize();

 Int_t npeds=0;
 if (peds) npeds=peds->GetSize();

 if (nbits<=0 || nbits>60 || !range || fabs(Vbias)>fabs(range) || (peds && npeds<nadcs) || B<0 || (B && C<1))
 {
  printf("\n *%-s::DAC* Inconsistent input nbits=%-i range=%-g Vbias=%-g nadcs=%-i npeds=%-i B=%-i C=%-i \n",ClassName(),nbits,range,Vbias,nadcs,npeds,B,C);
  return arrdac;
 }

 NcMath math;

 nbits=abs(nbits);

 Long64_t N=pow(2,nbits); // The number of quantization levels
 Long64_t adcmin=0;
 Long64_t adcmax=N-1;
 Double_t Vref=0;
 Double_t Vfs=0;
 Double_t LSB=0;

 // Floating point version of some parameters
 Double_t rN=N;
 Double_t rB=B;
 if (B==1) rB=exp(1);
 Double_t rC=C;
 Double_t radcmax=adcmax;

 if (range<0) // |range| represents Vref
 {
  Vref=fabs(range);
  LSB=Vref/rN;
  Vfs=Vref-LSB;
 }
 else // range represents Vfs
 {
  Vfs=range;
  LSB=Vfs/radcmax;
  Vref=Vfs+LSB;
 }

 Long64_t ped=0;
 Double_t frac=0;
 if (B) // Log DAC
 {
  if (range<0) Vfs=pow(rB,-rC)*pow(rB,radcmax*rC/rN)*Vref;
  if (range>0) Vref=pow(rB,rC)*pow(rB,-radcmax*rC/rN)*Vfs;
  LSB=Vref*pow(rB,-rC)*(pow(rB,rC/rN)-1.);
  frac=Vbias/Vref;
  ped=0;
  if (frac>0) ped=Long64_t(rN*(math.Log(rB,frac)+rC)/rC);
 }

 if (LSB<=0 || Vfs<=0)
 {
  printf("\n *%-s::DAC* Inconsistent parameters : LSB=%-g Vfs=%-g \n",ClassName(),LSB,Vfs);
  return arrdac;
 }

 if (!B) ped=Vbias/LSB; // Pedestal value for a linear DAC

 Double_t DR=20.*log10(Vfs/LSB);

 TString mode="linear";
 if (B==1) mode="Log_e";
 if (B>1)
 {
  mode="Log_";
  mode+=B;
 }

 if (!nadcs)
 {
  printf(" *%-s::DAC* No input data have been provided --> Only the value of adc(Vbias) is returned. \n",ClassName());
  if (!B)
  {
   printf(" Parameters for a %-i-bits %-s DAC with adc=[%-lli,%-lli] : \n",nbits,mode.Data(),adcmin,adcmax);
   printf(" Vref=%-g Vfs=%-g LSB=%-g DR=%-g (dB) Vbias=%-g adc(Vbias)=%-lli \n",Vref,Vfs,LSB,DR,Vbias,ped);
  }
  else
  {
   printf(" Parameters for a %-i-bits %-s DAC with adc=[%-lli,%-lli] and a code efficiency factor of %-i: \n",nbits,mode.Data(),adcmin,adcmax,C);
   printf(" Vref=%-g Vfs=%-g LSB=%-g DR=%-g (dB) Vbias=%-g adc(Vbias)=%-lli \n",Vref,Vfs,LSB,DR,Vbias,ped);
  }
  arrdac.Set(1);
  arrdac[0]=ped;
  return arrdac;
 }

 arrdac.Set(nadcs);

 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   hist->SetBins(nadcs,0,double(nadcs)/fSample);
   title.Form("%-s %-i-bits %-s DAC with Vfs=%-g LSB=%-g (%-g samples/sec);Time in seconds;Amplitude",ClassName(),nbits,mode.Data(),Vfs,LSB,fSample);
  }
  else
  {
   title.Form("%-s %-i-bits %-s DAC with Vfs=%-g LSB=%-g;Sample number;Amplitude",ClassName(),nbits,mode.Data(),Vfs,LSB);
   hist->SetBins(nadcs,0,nadcs);
  }
  hist->SetMarkerStyle(20);
  hist->SetTitle(title);
 }

 Long64_t adc=0;
 Double_t radc=0;
 Double_t dacval=0;
 for (Int_t j=0; j<nadcs; j++)
 {
  adc=adcs->GetAt(j);
  radc=adc;

  if (B) // Log DAC
  {
   dacval=Vref*pow(rB,-rC)*pow(rB,radc*rC/rN);
  }
  else // Linear DAC
  {
   if (peds)
   {
    ped=peds->GetAt(j);
    adc=adc-ped;
    radc=adc;
   }
   dacval=radc*LSB;
  }

  // Correct for bias voltage
  if (B || (!B && !peds))
  {
   dacval=dacval-Vbias;
  }

  arrdac[j]=dacval;

  if (hist) hist->SetBinContent(j+1,dacval);
 }

 return arrdac;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::Transmit(Int_t nbits,Double_t range,Double_t Vbias,TArray* Vsig,TArray* peds,TH1* hist,Int_t B,Int_t C) const
{
/**
~~~
// Mimic signal transmission according to an "nbits" ADC-DAC chain.
// Analog input signals are digitized via the discrete quantization levels of an "nbits" ADC,
// based on the "range" for the analog signal and a bias voltage "Vbias" or array "peds" of pedestal values (see below).
// The analog input signals may be provided by the (optional) TArray "Vsig".
// In case the array "Vsig" is not provided, the stored waveform is used to provide the analog input signals.
// After digitization, the digital signals are converted into analog signals via the corresponding "nbits" DAC.
// In this way the effect of digitization on the original input signals can be investigated, and can provide
// a guideline in selecting the most suitable ADC-DAC system for data transmission.
// The resulting analog values are returned in a TArrayD object, without modification of the original waveform data.
//
// Note : Make sure to use the same units for "range", "Vbias" and the analog input signals.
//
// For further details, please refer to the documentation of the memberfunctions ADC() and DAC().
//
// Input arguments :
// -----------------
// nbits    : Digital quantization was performed using an nbits ADC.
// range >0 : The full scale voltage range (Vfs) of the analog signal that corresponds to adc=N-1. 
//       <0 : |range| is the reference voltage (Vref) of the analog signal that corresponds to the hypothetical adc=N.
// Vbias    : The bias voltage that was added to the analog input signal before digitization.
//            If specified, the resulting analog signals will be corrected for the bias voltage.
//            For a linear DAC the correction via Vbias will only be performed if no pedestal array "peds"
//            is specified (see below). If the array "peds" is specified, Vbias will be ignored for a linear DAC.
//            For a Log DAC the pedestal values will never be used and bias correction may only be obtained via Vbias.
// Vsig     : (Optional) array to contain the digital input signals.
// peds     : (Optional) array to contain the pedestal values for the individual digital signals.
//            The array "peds" must contain (at least) the same number of values as the number of analog input signals.
//            If the array "peds" is specified, the pedestals will be subtracted from the corresponding digital signals
//            before the conversion to analog signals is performed in case of a linear DAC.
//            For a Log DAC the pedestal values will never be used and bias correction may only be obtained via Vbias.
// B     >1 : Base for a log ADC (e.g. B=10 emulates a Log_10 ADC).
//       =0 : The ADC will be linear
//       =1 : The ADC will be a Log_e ADC.
//            Note : When B>0 all (Vbias+Vsig) input values should be positive.
// C        : Code efficiency factor that was used for a Log ADC.
//            Rule of thumb : pow(B,-C) is about the smallest signal/|range| ratio that can be resolved.
//            So, for a Log_10 ADC with C=3, the smallest signal that can be resolved is about |range|/1000.
//            Note : It is required that C>0.
//
// Optional output arguments :
// ---------------------------
// hist  : Histogram with the analog result
//
// Note :
// ------
// In case no "Vsig" array is provided and no waveform is present, just the ADC and DAC specs will be printed
// and in the TArrayD only the adc value corresponding to Vbias is returned.
// This allows to quickly investigate various scenarios without any data treatment.
//         
// The maximum number of bits that is supported is 60 to guarantee identical functioning
// on all machines.
//
// In case of inconsistent input parameters, no processing is performed and an empty TArrayD is returned.
//
// The default values are Vbias=0, Vsig=0, peds=0, hist=0, B=0 and C=3.
~~~
**/

 TArrayL64 adcarr;
 TArrayD dacarr;
 if (hist) hist->Reset();

 // Provide the ADC and DAC specs in case no input is provided
 if (!Vsig && fNwf<1)
 {
  printf(" *%-s::Transmit* Specifications for the ADC-DAC transmission chain. \n",ClassName());
  adcarr=ADC(nbits,range,Vbias,0,0,B,C);
  dacarr.Set(1);
  dacarr[0]=adcarr[0];
  return dacarr;
 }

 // Perform the digitization via the ADC processeor
 adcarr=ADC(nbits,range,Vbias,Vsig,hist,B,C);

 // Convert the digital data into analog signals via the DAC processor
 dacarr=DAC(nbits,range,Vbias,&adcarr,peds,hist,B,C);

 if (hist)
 {
  TString title=hist->GetTitle();
  title.ReplaceAll("DAC","ADC-DAC (Transmit)");
  hist->SetTitle(title);
 }

 return dacarr;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::SampleAndHold(TF1 f,Double_t step,Double_t vmin,Double_t vmax,TH1* hist,Int_t loc) const
{
/**
~~~
// Perform a Sample-And-Hold operation on the specified function "f"
// in the interval [vmin,vmax], using "step" as the sampling step size.
// The result is returned in a TArrayD object and (optionally) in the histogram "hist".
//
// If "f" can be regarded as a pulse generator in time, this mimics a 
// sample and hold device with a lock time of "step" time units,
// or equivalently a sampling frequency of 1/step.
//
// The input argument "loc" determines whether the resulting data
// will be recorded at the start (loc<0), center (loc=0) or end (loc>0)
// of the sampling step size.
// However, in case the recording location of the last sampling step would
// exceed "vmax", the data will be recorded at the value of "vmax".
//
// The default values are hist=0 and loc=-1.
~~~
**/

 TArrayD data(0);
 if (hist) hist->Reset();

 if (step<=0 || vmax<=vmin)
 {
  cout << " *" << ClassName() << "::SampleAndHold* Error : Invalid input step=" << step << " vmin=" << vmin << " vmax=" << vmax << endl;
  return data;
 }
 
 // The number of samples
 Int_t n=(vmax-vmin)/step;
 data.Set(n);

 if (hist)
 {
  hist->SetBins(n,vmin,vmax);
  TString sloc="start";
  if (!loc) sloc="center";
  if (loc>0) sloc="end";
  TString title;
  title.Form("NcDSP SampleAndHold for Function %-s in steps of %-.3g;X value;F(x) at the %-s of each step",(f.GetExpFormula("p")).Data(),step,sloc.Data());
  hist->SetTitle(title);
  hist->SetMarkerStyle(20);
 }

 // Enter the sampled data into the output array
 Double_t xlow=vmin;
 Double_t xval=0;
 Double_t yval=0;
 Int_t i=0;
 while (xlow<=vmax)
 {
  if (i>=n) break;

  if (loc<0) xval=xlow;
  if (!loc) xval=xlow+step/2.;
  if (loc>0) xval=xlow+step;
  if (xval>vmax) xval=vmax;

  yval=f.Eval(xval);
  data[i]=yval;

  if (hist) hist->SetBinContent(i+1,data[i]);

  xlow+=step;
  i++;
 }

 return data;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::SampleAndHold(Int_t n,TH1* hist,Int_t loc,Int_t jmin,Int_t jmax) const
{
/**
~~~
// Perform a Sample-And-Hold operation on the data contained in the stored waveform
// over the sampled interval [jmin,jmax], using "n" original samples as the new sampling step size.
// By convention, the first sample is at j=0.
// The result is returned in a TArrayD object and (optionally) in the histogram "hist",
// without modification of the original waveform data.
//
// If the waveform can be regarded as a pulse generator in time, this mimics a 
// sample and hold device with a lock time of "n" time units,
// or equivalently a sampling frequency which is reduced by a factor 1/n.
//
// The input argument "loc" determines whether the resulting data
// will be recorded at the start (loc<0), center (loc=0) or end (loc>0)
// of the new sampling step size.
// However, in case the recording location of the last sampling step would
// exceed "jmax", the data will be recorded at the value of "jmax".
//
// If jmax<=jmin the full data array of the stored waveform will be used.
//
// The default values are hist=0, loc=-1, jmin=0 and jmax=-1.
~~~
**/

 TArrayD data(0);
 if (hist) hist->Reset();

 if (fNwf<1)
 {
  cout << " *" << ClassName() << "::SampleAndHold* Error : No waveform present." << endl;
  return data;
 }

 if (jmax<=jmin)
 {
  jmin=0;
  jmax=fNwf-1;
 }

 if (n<=0 || jmin<0 || jmin>=fNwf || jmax<0 || jmax>=fNwf)
 {
  cout << " *" << ClassName() << "::SampleAndHold* Invalid input n=" << n << " jmin=" << jmin << " jmax=" << jmax << endl;
  return data;
 }

 // Fill the data array
 Int_t ndata=fNwf/n;
 if (fNwf%n) ndata++;
 data.Set(ndata);
 for (Int_t i=0; i<ndata; i++)
 {
  data[i]=0;
 }

 Double_t val=0;
 Int_t j=0;
 Int_t k=0;
 for (Int_t i=0; i<ndata; i++)
 {
  if (j>jmax) break;

  if (loc<0) k=j;
  if (!loc) k=j+n/2;
  if (loc>0) k=j+n;

  if (k<jmin) continue;

  if (k>jmax) k=jmax;

  val=fWaveform[k];
  data[i]=val;
  j+=n;
 }

 if (hist)
 {
  TString sloc="start";
  if (!loc) sloc="center";
  if (loc>0) sloc="end";
  TString title;
  if (fSample>0)
  {
   Double_t fnew=fSample/double(n);
   title.Form("NcDSP SampleAndHold over %-i original samplings (New: %-g samples/sec);Time in seconds;Value at the %-s of each new sample",n,fnew,sloc.Data());
   hist->SetBins(ndata,double(jmin)/fSample,double(jmax)/fSample);
  }
  else
  {
   title.Form("NcDSP SampleAndHold over %-i original samplings;New sample number;Value at the %-s of each new sample",n,sloc.Data());
   hist->SetBins(ndata,jmin,jmax);
  }
  hist->SetTitle(title);
  hist->SetMarkerStyle(20);
  for (Int_t i=1; i<=ndata; i++)
  {
   hist->SetBinContent(i,data[i-1]);
  }
 }

 return data;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::SampleAndSum(TF1 f,Double_t step,Double_t vmin,Double_t vmax,TH1* hist) const
{
/**
~~~
// Perform a Sample-And-Sum operation on the specified function "f"
// in the interval [vmin,vmax], using "step" as the sampling step size.
// The result is returned in a TArrayD object and (optionally) in the histogram "hist".
//
// This procedure resembles a Sample-And-Hold operation, but instead of locking
// the data recording during the stepsize, the data that appear within "step"
// are summed c.q. integrated.
//
// If "f" can be regarded as sampling in time, this mimics a 
// Switched Capacitor Array (SCA) with a time gate of "step" time units,
// or equivalently a sampling frequency which is reduced by a factor 1/step.
//
// The default value is hist=0.
~~~
**/

 TArrayD data(0);
 if (hist) hist->Reset();

 if (step<=0 || vmax<=vmin)
 {
  cout << " *" << ClassName() << "::SampleAndSum* Error : Invalid input step=" << step << " vmin=" << vmin << " vmax=" << vmax << endl;
  return data;
 }
 
 // The number of samples
 Int_t n=(vmax-vmin)/step;
 data.Set(n);
 for (Int_t i=0; i<n; i++)
 {
  data[i]=0;
 }

 if (hist)
 {
  hist->SetBins(n,vmin,vmax);
  TString title;
  title.Form("NcDSP SampleAndSum for Function %-s with sampling steps of %-.3g;X value;Integral over each step",(f.GetExpFormula("p")).Data(),step);
  hist->SetTitle(title);
  hist->SetMarkerStyle(20);
 }

 // Enter the sampled data into the output array
 Double_t xlow=vmin;
 Double_t xup=0;
 Double_t yval=0;
 Int_t i=0;
 while (xlow<=vmax)
 {
  if (i>=n) break;

  xup=xlow+step;
  if (xup>vmax) xup=vmax;

  yval=f.Integral(xlow,xup);
  data[i]=yval;

  if (hist) hist->SetBinContent(i+1,data[i]);

  xlow+=step;
  i++;
 }

 return data;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::SampleAndSum(Int_t n,TH1* hist,Int_t jmin,Int_t jmax) const
{
/**
~~~
// Perform a Sample-And-Sum operation on the data contained in the stored waveform
// over the sampled interval [jmin,jmax], using "n" original samples as the new sampling step size.
// By convention, the first sample is at j=0.
// The result is returned in a TArrayD object and (optionally) in the histogram "hist",
// without modification of the original waveform data.
//
// This procedure resembles a Sample-And-Hold operation, but instead of locking
// the data recording during the stepsize of "n" samplings, the data that appear
// within "n" samplings are summed.
//
// If the waveform data can be regarded as sampling in time, this mimics a 
// Switched Capacitor Array (SCA) with a time gate of "n" time units,
// or equivalently a sampling frequency which is reduced by a factor 1/n.
//
// If jmax<=jmin the full data array of the stored waveform will be used.
//
// The default values are hist=0, jmin=0 and jmax=-1.
~~~
**/

 TArrayD data(0);
 if (hist) hist->Reset();

 if (fNwf<1)
 {
  cout << " *" << ClassName() << "::SampleAndSum* Error : No waveform present." << endl;
  return data;
 }

 if (jmax<=jmin)
 {
  jmin=0;
  jmax=fNwf-1;
 }

 if (n<=0 || jmin<0 || jmin>=fNwf || jmax<0 || jmax>=fNwf)
 {
  cout << " *" << ClassName() << "::SampleAndSum* Invalid input n=" << n << " jmin=" << jmin << " jmax=" << jmax << endl;
  return data;
 }

 // Fill the data array
 Int_t ndata=fNwf/n;
 if (fNwf%n) ndata++;
 data.Set(ndata);
 for (Int_t i=0; i<ndata; i++)
 {
  data[i]=0;
 }

 Double_t sum=0;
 Int_t j=0;
 for (Int_t i=0; i<ndata; i++)
 {
  sum=0;
  for (Int_t k=0; k<n; k++)
  {
   if (j<jmin) continue;
   if (j>jmax) break;
   sum+=fWaveform[j];
   j++;
  }
  data[i]=sum;
 }

 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   Double_t fnew=fSample/double(n);
   title.Form("NcDSP SampleAndSum over %-i original samplings;Time in seconds;Summed value per %-.3g seconds",n,1./fnew);
   hist->SetBins(ndata,double(jmin)/fSample,double(jmax)/fSample);
 }
  else
  {
   title.Form("NcDSP SampleAndSum over %-i original samplings;New sample number;Summed value in each new sample",n);
   hist->SetBins(ndata,jmin,jmax);
  }
  hist->SetTitle(title);
  hist->SetMarkerStyle(20);
  for (Int_t i=1; i<=ndata; i++)
  {
   hist->SetBinContent(i,data[i-1]);
  }
 }

 return data;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::FilterMovingAverage(Int_t n,TString mode,TH1* hist,Int_t* i1,Int_t* i2,TH1* hisf,Bool_t dB)
{
/**
~~~
// Perform a Moving Average filter on the loaded input data x[] with averaging over "n" samples.
// The time domain result is returned in a TArrayD object and (optionally) in the histogram "hist".
// The frequency domain result is returned in the (optional) histogram "hisf", for which
// the amplitudes may be represented in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The original input data x[] are not modified.
// The input data x[] have to be entered as real numbers by one of the Load() member functions.
//
// A Moving Average filter is the optimal time domain filter for reducing random (noise) fluctuations,
// while retaining sharp step responses.
// Large values of "n" will result in a large noise reduction, but the edges of the steps
// become less sharp.
// On the contrary, small values of "n" will result in sharp step edges, but less noise reduction.
//
// Rule of thumb :
// ---------------
// The noise is reduced by a factor of sqrt(n) and the rise or fall of an edge
// is smeared over "n" samples.
//
// This filter may be invoked in two different modes, namely Recursion or Convolution.
// The resulting time domain data are equivalent, but are contained in TArrayD objects
// of different length, as outlined below..
//
// The Recursion mode :
// --------------------
// The straightforward procedure is expressed as :
//
//    y[i]=(1/n)*Sum(j=0,n-1){x[i+j]}  (1)
//
// To reduce rounding errors and computational speed, the following recursion relation is used :
//
//    y[k]=y[k-1]-(x[k-1]/n)+(x[k+n-1]/n)  (2)
//
// whereby the first term y[0] is calulated via equation (1).
//
// In case the number of elements of x[] is m, the number of elements of y[] will "only" be (m-n+1),
// such that the last element of y[] still has been completely computed from all corresponding "n" values of x[].
//
// The Convolution mode :
// ----------------------
// Based on the above, this filter actually represents a convolution with a filter kernel
// that consists of a rectangular pulse of "n" points of height 1/n (i.e. an area of 1).
// In this respect we can regard this filtering as a convolution y[]=x[]*h[] where h[]
// represents the "n" point rectangular pulse filter kernel.
// In this mode, an array x[] with m elements, will result in an array y[] of (m+n-1) elements.
// For further details see the documentation of Convolve().
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// n    : The number of samples that will be averaged over
// mode : To invoke Recursion (mode="rec") or Convolution (mode="conv") processing.
// hist : (optional) Histogram with the filtered result in the time domain
// i1   : Optional argument (see below)
// i2   : Optional argument (see below)
// hisf : (optional) Histogram with the filtered result (amplitudes) in the frequency domain
// dB   : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
//
// The (optional) arguments i1 and i2 provide the range [i1,i2] in the
// resulting filtered data array for which the filter kernel was fully immersed
// in the input (signal) data x[].
// In other words : The indices range [i1,i2] in the resulting filtered data array y[]
// corresponds to all the y[] elements for which the averaging was completely performed
// over all "n" samples.
// So, values of y[j] with j<i1 or j>i2 contain incomplete summations, and as such should not
// be considered as reliable, especially when x[] contains large variations.
// For an absolute comparison between the x[] and y[] values, one should realize that 
// the array sizes of x[] and y[] are different and that x[k] should be compared with y[k+i1].
// The values i1 and i2 (if requested) are indicated by vertical dashed blue lines
// in the time domain histogram.
//
// Notes :
// -------
// 1) The returned TArrayD contains the filtered time domain data,
//    which can be directly used as input for subsequent DSP processing.
// 2) The performance in the frequency domain is very poor, since the Fourier transform
//    of the time domain rectangular pulse filter kernel results in a sinc function.
// 3) A Bayesian Block analysis (see the class NcBlocks) in general provides
//    even better results by allowing "n" to vary dynamically based on the data pattern.
//    However, the computation time involved for a Bayesian Block analysis
//    is vastly larger than for a Moving Average filter.
//
// The default values are hist=0, i1=0 i2=0, hisf=0 and dB=kTRUE.
~~~
**/

 Int_t ny=0;
 TArrayD y(ny);
 if (hist) hist->Reset();
 if (hisf) hisf->Reset();
 Int_t nx=fReIn.GetSize();

 if (nx<1)
 {
  printf(" *%-s::FilterMovingAverage* No loaded input data present. \n",ClassName());
  return y;
 }

 if (n<1 || n>nx || (mode!="rec" && mode!="conv"))
 {
  printf(" *%-s::FilterMovingAverage* Inconsistent input n=%-i for x[%-i] and mode=%-s \n",ClassName(),n,nx,mode.Data());
  return y;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 if (mode=="conv") // Convolution mode
 {
  // The filter kernel
  TArrayD h=GetMovingAverageKernel(n);
  SetWaveform(&h);
 
  // Perform the convolution
  y=Convolve(hist,i1,i2);
 }
 else // Recursive mode
 {
  TArrayD x=fReIn;
  ny=nx-n+1;
  if (i1 || i2) ny=nx;

  if (i1) *i1=0;
  if (i2) *i2=nx-n;

  y.Set(ny);
  y.Reset();

  // Calculate the first y-value summation
  for (Int_t i=0; i<n; i++)
  {
   y[0]+=x[i];
  }

  // The recursive summation
  Double_t add=0;
  for (Int_t k=1; k<ny; k++)
  {
   add=0;
   if ((k+n-1)<nx) add=x[k+n-1];
   y[k]=y[k-1]-x[k-1]+add;
  }

  Double_t rn=n;
  // Calculate the average values
  for (Int_t i=0; i<ny; i++)
  {
   y[i]=y[i]/rn;
  }
 }

 TString title;
 if (hist)
 {
  if (mode=="rec")
  {
   if (fSample>0)
   {
    title.Form("%-s;Time in seconds;Value",ClassName());
    hist->SetBins(ny,0,double(ny)/fSample);
   }
   else
   {
    title.Form("%-s;Sample number;Value",ClassName());
    hist->SetBins(ny,0,ny);
   }

   // Set histogram axis labels and provisonal title
   hist->SetTitle(title);

   // Fill the histogram
   for (Int_t ibin=1; ibin<=ny; ibin++)
   {
    hist->SetBinContent(ibin,y[ibin-1]);
   }

   // Indicate the values of i1 and i2 (if requested) by vertical blue dashed lines
   if (i1 || i2)
   {
    Double_t ymin=hist->GetMinimum();
    Double_t ymax=hist->GetMaximum();

    Double_t xlow=0;
    Double_t xup=0;
    if (i1)  xlow=hist->GetBinLowEdge(*i1+1);
    if (i2)
    {
     xup=hist->GetBinLowEdge(*i2+1);
     xup+=hist->GetBinWidth(1);
    }

    TLine* vline1=0;
    TLine* vline2=0;

    if (i1)
    {
     vline1=new TLine(xlow,ymin,xlow,ymax);
     vline1->SetLineStyle(2); // Dashed line
     vline1->SetLineWidth(2);
     vline1->SetLineColor(4); // Blue color
    }
    if (i2)
    {
     vline2=new TLine(xup,ymin,xup,ymax);
     vline2->SetLineStyle(2); // Dashed line
     vline2->SetLineWidth(2);
     vline2->SetLineColor(4); // Blue color
    }

     TList* hlist=hist->GetListOfFunctions();
     if (vline1) hlist->Add(vline1);
     if (vline2) hlist->Add(vline2);
   }
  }

  // Set the appropriate histogram title
  title.Form("%-s Moving Average (%-s mode) Filter: Time domain result averaged over %-i samples",ClassName(),mode.Data(),n);
  hist->SetTitle(title);
 }

 // Fill the filtered frequency domain histogram
 if (hisf)
 {
  // Obtain the frequency domain histogram
  HistogramFilterFFT(&y,hisf,dB,kFALSE);

  // Set the appropriate histogram title
  title.Form("%-s Moving Average (%-s mode) Filter: Frequency domain result (%-i sample averaging in time domain)",ClassName(),mode.Data(),n);
  hisf->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::FilterLowPass(Double_t fcut,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Int_t* i1,Int_t* i2,Bool_t adaptn)
{
/**
~~~
// Perform a Low Pass filter on the loaded input data x[] with a frequency cut-off
// specified by "fcut" (see below) and a filter kernel consisting of "n" points (see below).
// The time domain result is returned in a TArrayD object and (optionally) in the histogram "hist",
// whereas the frequency domain result is returned in the (optional) histogram "hisf".
// The "hisf" amplitudes may be represented in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The original input data x[] are not modified.
// The input data x[] have to be entered as real numbers by one of the Load() member functions.
//
// Note :
// ------
// When the input data x[] contains en even (odd) number of samples, then an odd (even) value of "n"
// will result again in an even (odd) number of samples for the filtered data in the time domain.
//
// The implementation here is based on the Blackman windowed-sinc filtering procedure.
//
// A Blackman Low Pass filter is an excellent frequency domain filter for seperating
// one band of frequencies from another.
// Large values of "n" will result in a sharp transition between the pass band and the stop band,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in a less sharp transition between the
// pass band and the stop band, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) between the pass band and the stop band (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) The returned TArrayD contains the filtered time domain data,
//    which can be directly used as input for subsequent DSP processing.
// 2) Repeated application of this filter will further decrease the amplitudes in the stop band.
// 3) Actually this filter represents a convolution with a time domain filter kernel
//    that consists of a sinc pulse window of "n" points and an area of 1.
// 4) The performance in the time domain is very poor, since the (inverse) Fourier transform
//    of the frequency domain step pulse filter kernel results in a sinc function.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// fcut   : The cut-off frequency expressed as a fraction of the sampling frequency
//          For proper functionality one should choose 0<fcut<0.5, because of the
//          underlying Fourier transform.
// n      : The number of values in the filter kernel
//          For best functionality this must be an odd integer (see adaptn below)
// hisf   : (optional) Histogram with the filtered result (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filtered result in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The (optional) arguments i1 and i2 provide the range [i1,i2] in the
// resulting filtered data array for which the filter kernel was fully immersed
// in the input (signal) data.
// These values i1 and i2 (if requested) are indicated by vertical
// dashed blue lines in the time domain histogram.
//
// The default values are hisf=0, dB-kTRUE, hist=0, i1=0, i2=0 and adaptn=kTRUE.
~~~
**/

 TArrayD y(0);
 if (hisf) hisf->Reset();
 if (hist) hist->Reset();
 Int_t nx=fReIn.GetSize();

 if (nx<1)
 {
  printf(" *%-s::FilterLowPass* No loaded input data present. \n",ClassName());
  return y;
 }

 if (n<1 || fcut<=0 || fcut>0.5)
 {
  printf(" *%-s::FilterLowPass* Invalid input fcut=%-g n=%-i \n",ClassName(),fcut,n);
  return y;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 // The filter kernel
 TArrayD h=GetLowPassKernel(fcut,n,0,0,0,adaptn);
 
 // Perform the convolution
 SetWaveform(&h);
 y=Convolve(hist,i1,i2,2);

 // Set title for the filtered time domain histogram
 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   Float_t nucut=fcut*fSample;
   title.Form("%-s Low Pass Filter: Time domain result with #nu-cut=%-.3g Hz (%-i-point kernel)",ClassName(),nucut,n);
  }
  else
  {
   title.Form("%-s Low Pass Filter: Time domain result with #nu-cut/#nu-sample=%-.3g (%-i-point kernel)",ClassName(),fcut,n);
  }
  hist->SetTitle(title);
 }

 // Fill the filtered frequency domain histogram
 if (hisf)
 {
  // Obtain the frequency domain histogram
  HistogramFilterFFT(&y,hisf,dB,kFALSE);

  // Set the appropriate histogram title
  TString title;
  if (fSample>0)
  {
   Float_t nucut=fcut*fSample;
   title.Form("%-s Low Pass Filter: Frequency domain result with #nu-cut=%-.3g Hz (%-i-point time domain kernel)",ClassName(),nucut,n);
  }
  else
  {
   title.Form("%-s Low Pass Filter: Frequency domain result with #nu-cut/#nu-sample=%-.3g (%-i-point time domain kernel)",ClassName(),fcut,n);
  }
  hisf->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::FilterHighPass(Double_t fcut,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Int_t* i1,Int_t* i2,Bool_t adaptn)
{
/**
~~~
// Perform a High Pass filter on the loaded input data x[] with a frequency cut-off
// specified by "fcut" (see below) and a filter kernel consisting of "n" points (see below).
// The time domain result is returned in a TArrayD object and (optionally) in the histogram "hist",
// whereas the frequency domain result is returned in the (optional) histogram "hisf".
// The "hisf" amplitudes may be represented in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The original input data x[] are not modified.
// The input data x[] have to be entered as real numbers by one of the Load() member functions.
//
// Note :
// ------
// When the input data x[] contains en even (odd) number of samples, then an odd (even) value of "n"
// will result again in an even (odd) number of samples for the filtered data in the time domain.
//
// The implementation here is based on a spectrally inverted Blackman windowed-sinc Low Pass filter.
//
// A Blackman High Pass filter is an excellent frequency domain filter for seperating
// one band of frequencies from another.
// Large values of "n" will result in a sharp transition between the pass band and the stop band,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in a less sharp transition between the
// pass band and the stop band, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) between the pass band and the stop band (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) The returned TArrayD contains the filtered time domain data,
//    which can be directly used as input for subsequent DSP processing.
// 2) Repeated application of this filter will further decrease the amplitudes in the stop band.
// 3) Actually this filter represents a convolution with a time domain filter kernel
//    that consists of a sinc pulse window of "n" points and an area of 1.
// 4) The performance in the time domain is very poor, since the (inverse) Fourier transform
//    of the frequency domain step pulse filter kernel results in a sinc function.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// fcut   : The cut-off frequency expressed as a fraction of the sampling frequency
//          For proper functionality one should choose 0<fcut<0.5, because of the
//          underlying Fourier transform.
// n      : The number of values in the filter kernel
//          For best functionality this must be an odd integer (see adaptn below).
// hisf   : (optional) Histogram with the filtered result (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filtered result in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The (optional) arguments i1 and i2 provide the range [i1,i2] in the
// resulting filtered data array for which the filter kernel was fully immersed
// in the input (signal) data.
// These values i1 and i2 (if requested) are indicated by vertical
// dashed blue lines in the time domain histogram.
//
// The default values are hisf=0, dB=kTRUE, hist=0, i1=0, i2=0 and adaptn=kTRUE.
~~~
**/

 TArrayD y(0);
 if (hisf) hisf->Reset();
 if (hist) hist->Reset();
 Int_t nx=fReIn.GetSize();

 if (nx<1)
 {
  printf(" *%-s::FilterHighPass* No loaded input data present. \n",ClassName());
  return y;
 }

 if (n<1 || fcut<=0 || fcut>0.5)
 {
  printf(" *%-s::FilterHighPass* Invalid input fcut=%-g n=%-i \n",ClassName(),fcut,n);
  return y;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 // The filter kernel
 TArrayD h=GetHighPassKernel(fcut,n,0,0,0,adaptn);
 
 // Perform the convolution
 SetWaveform(&h);
 y=Convolve(hist,i1,i2,2);

 // Set title for the filtered time domain histogram
 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   Float_t nucut=fcut*fSample;
   title.Form("NcDSP High Pass Filter: Time domain result with #nu-cut=%-.3g Hz (%-i-point kernel)",nucut,n);
  }
  else
  {
   title.Form("NcDSP High Pass Filter: Time domain result with #nu-cut/#nu-sample=%-.3g (%-i-point kernel)",fcut,n);
  }
  hist->SetTitle(title);
 }

 // Fill the filtered frequency domain histogram
 if (hisf)
 {
  // Obtain the frequency domain histogram
  HistogramFilterFFT(&y,hisf,dB,kFALSE);

  // Set the appropriate histogram title
  TString title;
  if (fSample>0)
  {
   Float_t nucut=fcut*fSample;
   title.Form("NcDSP High Pass Filter: Frequency domain result with #nu-cut=%-.3g Hz (%-i-point time domain kernel)",nucut,n);
  }
  else
  {
   title.Form("NcDSP High Pass Filter: Frequency domain result with #nu-cut/#nu-sample=%-.3g (%-i-point time domain kernel)",fcut,n);
  }
  hisf->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::FilterBandPass(Double_t f1,Double_t f2,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Int_t* i1,Int_t* i2,Bool_t adaptn)
{
/**
~~~
// Perform a Band Pass filter on the loaded input data x[] in the frequency band
// specified by "f1" and "f2" (see below) and a filter kernel consisting of "n" points (see below).
// The time domain result is returned in a TArrayD object and (optionally) in the histogram "hist",
// whereas the frequency domain result is returned in the (optional) histogram "hisf".
// The "hisf" amplitudes may be represented in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The original input data x[] are not modified.
// The input data x[] have to be entered as real numbers by one of the Load() member functions.
//
// Note :
// ------
// When the input data x[] contains en even (odd) number of samples, then an odd (even) value of "n"
// will result again in an even (odd) number of samples for the filtered data in the time domain.
//
// The implementation here is based on a combination of Blackman windowed-sinc Low Pass and High Pass filters,
// which is spectrally inverted.
//
// A Blackman Band Pass filter is an excellent frequency domain filter for passing
// only a certain band of frequencies.
// Large values of "n" will result in sharp transitions at the edges of the pass band,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in less sharp transitions at the edges
// of the pass band, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) at the edges of the pass band (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) The returned TArrayD contains the filtered time domain data,
//    which can be directly used as input for subsequent DSP processing.
// 2) Repeated application of this filter will further decrease the amplitudes outside the selected band.
// 3) Actually this filter represents a convolution with a time domain filter kernel
//    that consists of a sinc pulse window of "n" points and an area of 1.
// 4) The performance in the time domain is very poor, since the (inverse) Fourier transform
//    of a frequency domain rectangular pulse filter kernel results in a sinc function.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// f1     : The lower bound of the frequency band expressed as a fraction of the sampling frequency
// f2     : The upper bound of the frequency band expressed as a fraction of the sampling frequency
//          For proper functionality one should choose 0<f1<0.5 and 0<f2<0.5, because of the
//          underlying Fourier transform.
// n      : The number of values in the filter kernel
//          For best functionality this must be an odd integer (see adaptn below).
// hisf   : (optional) Histogram with the filtered result (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filtered result in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The (optional) arguments i1 and i2 provide the range [i1,i2] in the
// resulting filtered data array for which the filter kernel was fully immersed
// in the input (signal) data.
// These values i1 and i2 (if requested) are indicated by vertical
// dashed blue lines in the time domain histogram.
//
// The default values are hisf=0, dB=kTRUE, hist=0, i1=0, i2=0 and adaptn=TRUE.
~~~
**/

 TArrayD y(0);
 if (hisf) hisf->Reset();
 if (hist) hist->Reset();
 Int_t nx=fReIn.GetSize();

 if (nx<1)
 {
  printf(" *%-s::FilterBandPass* No loaded input data present. \n",ClassName());
  return y;
 }

 if (n<1 || f1<=0 || f1>0.5 || f2<=0 || f2>0.5 || f2<=f1)
 {
  printf(" *%-s::FilterBandPass* Invalid input f1=%-g f2=%-g n=%-i \n",ClassName(),f1,f2,n);
  return y;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 // The filter kernel
 TArrayD h=GetBandPassKernel(f1,f2,n,0,0,0,adaptn);
 
 // Perform the convolution
 SetWaveform(&h);
 y=Convolve(hist,i1,i2,2);

 // Set title for the filtered time domain histogram
 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   Float_t nu1=f1*fSample;
   Float_t nu2=f2*fSample;
   title.Form("NcDSP Band Pass Filter: Time domain result for [%-.3g,%-.3g] Hz (%-i-point kernel)",nu1,nu2,n);
  }
  else
  {
   title.Form("NcDSP Band Pass Filter: Time domain result for #nu/#nu-sample=[%-.3g,%-.3g] (%-i-point kernel)",f1,f2,n);
  }
  hist->SetTitle(title);
 }

 // Fill the filtered frequency domain histogram
 if (hisf)
 {
  // Obtain the frequency domain histogram
  HistogramFilterFFT(&y,hisf,dB,kFALSE);

  // Set the appropriate histogram title
  TString title;
  if (fSample>0)
  {
   Float_t nu1=f1*fSample;
   Float_t nu2=f2*fSample;
   title.Form("NcDSP Band Pass Filter: Frequency domain result for [%-.3g,%-.3g] Hz (%-i-point time domain kernel)",nu1,nu2,n);
  }
  else
  {
   title.Form("NcDSP Band Pass Filter: Frequency domain result for #nu/#nu-sample=[%-.3g,%-.3g] (%-i-point time domain kernel)",f1,f2,n);
  }
  hisf->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::FilterBandReject(Double_t f1,Double_t f2,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Int_t* i1,Int_t* i2,Bool_t adaptn)
{
/**
~~~
// Perform a Band Reject filter on the loaded input data x[] in the frequency band
// specified by "f1" and "f2" (see below) and a filter kernel consisting of "n" points (see below).
// The time domain result is returned in a TArrayD object and (optionally) in the histogram "hist",
// whereas the frequency domain result is returned in the (optional) histogram "hisf".
// The "hisf" amplitudes may be represented in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The original input data x[] are not modified.
// The input data x[] have to be entered as real numbers by one of the Load() member functions.
//
// Note :
// ------
// When the input data x[] contains en even (odd) number of samples, then an odd (even) value of "n"
// will result again in an even (odd) number of samples for the filtered data in the time domain.
//
// The implementation here is based on a combination of Blackman windowed-sinc Low Pass and High Pass filters.
//
// A Blackman Band Reject filter is an excellent frequency domain filter for suppressing
// a certain band of frequencies.
// Large values of "n" will result in sharp transitions at the edges of the rejected band,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in less sharp transitions at the edges
// of the rejected band, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) at the edges of the rejected band (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) The returned TArrayD contains the filtered time domain data,
//    which can be directly used as input for subsequent DSP processing.
// 2) Repeated application of this filter will further decrease the amplitudes inside the selected band.
// 3) Actually this filter represents a convolution with a time domain filter kernel
//    that consists of a sinc pulse window of "n" points and an area of 1.
// 4) The performance in the time domain is very poor, since the (inverse) Fourier transform
//    of a frequency domain rectangular pulse filter kernel results in a sinc function.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// f1     : The lower bound of the frequency band expressed as a fraction of the sampling frequency
// f2     : The upper bound of the frequency band expressed as a fraction of the sampling frequency
//          For proper functionality one should choose 0<f1<0.5 and 0<f2<0.5, because of the
//          underlying Fourier transform.
// n      : The number of values in the filter kernel
//          For best functionality this must be an odd integer (see adaptn below).
// hisf   : (optional) Histogram with the filtered result (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filtered result in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The (optional) arguments i1 and i2 provide the range [i1,i2] in the
// resulting filtered data array for which the filter kernel was fully immersed
// in the input (signal) data.
// These values i1 and i2 (if requested) are indicated by vertical
// dashed blue lines in the time domain histogram.
//
// The default values are hisf=0, dB=kTRUE, hist=0, i1=0, i2=0 and adaptn=kTRUE.
~~~
**/

 TArrayD y(0);
 if (hisf) hisf->Reset();
 if (hist) hist->Reset();
 Int_t nx=fReIn.GetSize();

 if (nx<1)
 {
  printf(" *%-s::FilterBandReject* No loaded input data present. \n",ClassName());
  return y;
 }

 if (n<1 || f1<=0 || f1>0.5 || f2<=0 || f2>0.5 || f2<=f1)
 {
  printf(" *%-s::FilterBandReject* Invalid input f1=%-g f2=%-g n=%-i \n",ClassName(),f1,f2,n);
  return y;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 // The filter kernel
 TArrayD h=GetBandRejectKernel(f1,f2,n,0,0,0,adaptn);
 
 // Perform the convolution
 SetWaveform(&h);
 y=Convolve(hist,i1,i2,2);

 // Set title for the filtered time domain histogram
 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   Float_t nu1=f1*fSample;
   Float_t nu2=f2*fSample;
   title.Form("NcDSP Band Reject Filter: Time domain result for [%-.3g,%-.3g] Hz (%-i-point kernel)",nu1,nu2,n);
  }
  else
  {
   title.Form("NcDSP Band Reject Filter: Time domain result for #nu/#nu-sample=[%-.3g,%-.3g] (%-i-point kernel)",f1,f2,n);
  }
  hist->SetTitle(title);
 }

 // Fill the filtered frequency domain histogram
 if (hisf)
 {
  // Obtain the frequency domain histogram
  HistogramFilterFFT(&y,hisf,dB,kFALSE);

  // Set the appropriate histogram title
  TString title;
  if (fSample>0)
  {
   Float_t nu1=f1*fSample;
   Float_t nu2=f2*fSample;
   title.Form("NcDSP Band Reject Filter: Frequency domain result for [%-.3g,%-.3g] Hz (%-i-point time domain kernel)",nu1,nu2,n);
  }
  else
  {
   title.Form("NcDSP Band Reject Filter: Frequency domain result for #nu/#nu-sample=[%-.3g,%-.3g] (%-i-point time domain kernel)",f1,f2,n);
  }
  hisf->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::FilterMultiBand(TArray& freqs,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Int_t* i1,Int_t* i2,Bool_t adaptn)
{
/**
~~~
// Perform a MultiBand filter on the loaded input data x[] in various frequency bands as
// specified by the array "freqs" (see below) and filter kernels consisting of "n" points (see below).
// The time domain result is returned in a TArrayD object and (optionally) in the histogram "hist",
// whereas the frequency domain result is returned in the (optional) histogram "hisf".
// The "hisf" amplitudes may be represented in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The original input data x[] are not modified.
// The input data x[] have to be entered as real numbers by one of the Load() member functions.
//
// Note :
// ------
// When the input data x[] contains en even (odd) number of samples, then an odd (even) value of "n"
// will result again in an even (odd) number of samples for the filtered data in the time domain.
//
// The procedure is based on a convolution of the various provided Blackman
// single Low Pass and/or High Pass and/or Band Pass and/or Band Reject filters.
//
// Large values of "n" will result in sharp transitions at the edges of the specified bands,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in less sharp transitions at the edges
// of the specified bands, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) at the edges of the specified bands (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) The size of the TArrayD "freqs" should match exactly twice the number of
//    frequency bands to be specified (see below).
// 2) The returned TArrayD contains the filtered time domain data,
//    which can be directly used as input for subsequent DSP processing.
// 3) Specifying the same frequency band more than once will further suppress the unwanted frequencies. 
// 4) The performance in the time domain is very poor, since the (inverse) Fourier transform
//    of a frequency domain rectangular pulse filter kernel results in a sinc function.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// freqs  : Array containing the lower and upper bounds of the frequency bands expressed as
//          fractions of the sampling frequency.
//          The array elements represent the various pairs [flow,fup] to define the frequency bands,
//          ordered as (f1low,f1up,f2low,f2up,...).
//          The following conventions are used :
//          flow>0 and fup>0 --> Apply a Band Pass filter over [flow,fup]
//          flow<0 and fup<0 --> Apply a Band Reject filter over [|flow|,|fup|]
//          flow<0 and fup>0 --> Apply a Low Pass filter with fcut=fup
//          flow>0 and fup<0 --> Apply a High Pass filter with fcut=flow
// n      : The number of values in the corresponding filter kernels
//          For best functionality this must be an odd integer (see adaptn below).
// hisf   : (optional) Histogram with the filtered result (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filtered result in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The (optional) arguments i1 and i2 provide the range [i1,i2] in the
// resulting filtered data array for which the filter kernel was fully immersed
// in the input (signal) data.
// These values i1 and i2 (if requested) are indicated by vertical
// dashed blue lines in the time domain histogram.
//
// The default values are hisf=0, dB=kTRUE, hist=0, i1=0, i2=0 and adaptn=kTRUE.
~~~
**/

 TArrayD y(0);
 if (hisf) hisf->Reset();
 if (hist) hist->Reset();
 Int_t nx=fReIn.GetSize();

 if (nx<1)
 {
  printf(" *%-s::FilterMultiBand* No loaded input data present. \n",ClassName());
  return y;
 }

 Int_t arrsize=freqs.GetSize();
 Int_t oddsize=arrsize%2;
 Int_t nbands=arrsize/2;
 if (nbands<1 || n<1 || oddsize)
 {
  printf(" *%-s::FilterMultiBand* Invalid input array size=%-i nbands=%-i n=%-i \n",ClassName(),arrsize,nbands,n);
  return y;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 // The filter kernel
 TArrayD h=GetMultiBandKernel(freqs,n,0,0,0,adaptn);

 // Convolve the composite kernel with the original time domain data
 Load(&xsave);
 SetWaveform(&h);
 y=Convolve(hist,i1,i2,2);

 // Determine the number of actually specified bands
 Double_t flow=0;
 Double_t fup=0;
 Int_t index=0;
 Int_t neff=0; // The number of off actually specified bands
 // Loop over the specified frequency bands
 for (Int_t jband=1; jband<=nbands; jband++)
 {
  index=2*(jband-1);
  flow=freqs.GetAt(index);
  fup=freqs.GetAt(index+1);

  // Skip empty entries in the "freqs" array
  if (!flow || !fup) continue;

  neff++;
 }

 TString title;
 // Set title for the filtered time domain histogram
 if (hist)
 {
  title.Form("%-s MultiBand Filter: Time domain result for %-i bands (%-i-point kernel each)",ClassName(),neff,n);
  hist->SetTitle(title);
 }

 // Fill the filtered frequency domain histogram
 if (hisf)
 {
  // Obtain the frequency domain histogram
  HistogramFilterFFT(&y,hisf,dB,kFALSE);

  // Set the appropriate histogram title
  title.Form("%-s MultiBand Filter: Frequency domain result for %-i bands (%-i-point time domain kernel each)",ClassName(),neff,n);
  hisf->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::GetMovingAverageKernel(Int_t n,TH1* hisf,Bool_t dB,TH1* hist)
{
/**
~~~
// Provide via the returned TArrayD an n-point time domain Moving Average Filter kernel.
// The optional argument "hisf" may be used to obtain a histogram of the frequency domain kernel
// with the amplitude in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The optional argument "hist" may be used to obtain a (zero padded) histogram of the time domain kernel.
//
// A Moving Average filter is the optimal time domain filter for reducing random (noise) fluctuations,
// while retaining sharp step responses.
// Large values of "n" will result in a large noise reduction, but the edges of the steps
// become less sharp.
// On the contrary, small values of "n" will result in sharp step edges, but less noise reduction.
//
// Rule of thumb :
// ---------------
// The noise is reduced by a factor of sqrt(n) and the rise or fall of an edge
// is smeared over "n" samples.
//
// Notes :
// -------
// 1) Actually this filter kernel consists of a time domain rectangular pulse of "n" points
//    and an area of 1.
// 2) The performance in the frequency domain is very poor, since the Fourier transform
//    of the time domain rectangular pulse filter kernel results in a sinc function.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// n    : The number of samples that will be averaged over
// hisf : (optional) Histogram with the filter kernel (amplitudes) in the frequency domain
// dB   : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist : (optional) Histogram with the filter kernel in the time domain
//
// The default values are hisf=0, dB=kTRUE and hist=0.
~~~
**/

 TArrayD h(0);
 if (hisf) hisf->Reset();
 if (hist) hist->Reset();

 if (n<1)
 {
  printf(" *%-s::GetMovingAverageKernel* Invalid input n=%-i \n",ClassName(),n);
  return h;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 h.Set(n);
 for (Int_t i=0; i<n; i++)
 {
  h[i]=1./double(n);
 }

 HistogramFilterFFT(&h,hisf,dB,kTRUE,hist);

 // Set the appropriate histogram titles
 TString title;
 if (hisf)
 {
  title.Form("NcDSP Moving Average Filter: Frequency domain kernel (%-i-point time domain kernel)",n);
  hisf->SetTitle(title);
 }

 if (hist)
 {
  title.Form("NcDSP Moving Average Filter: Time domain kernel (%-i-point zero padded)",n);
  hist->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;
 
 return h;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::GetLowPassKernel(Double_t fcut,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Bool_t adaptn)
{
/**
~~~
// Provide via the returned TArrayD an n-point time domain Low Pass Filter kernel
// with cut-off frequency "fcut" expressed as a fraction of the sampling frequency.
// The optional argument "hisf" may be used to obtain a histogram of the kernel
// in the frequency domain with the amplitude in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The optional argument "hist" may be used to obtain a (zero padded) histogram of the time domain kernel.
//
// The implementation here is based on the Blackman windowed-sinc filtering procedure.
//
// A Blackman Low Pass filter is an excellent frequency domain filter for seperating
// one band of frequencies from another.
// Large values of "n" will result in a sharp transition between the pass band and the stop band,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in a less sharp transition between the
// pass band and the stop band, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) between the pass band and the stop band (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) Repeated application of this filter will further decrease the amplitudes in the stop band.
// 2) Actually this filter kernel consists of a time domain sinc pulse window of "n" points
//    and an area of 1.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// fcut   : The cut-off frequency expressed as a fraction of the sampling frequency
//          For proper functionality one should choose 0<fcut<0.5, because of the
//          underlying Fourier transform.
// n      : The number of values in the filter kernel
//          For best functionality this must be an odd integer (see adaptn below)
// hisf   : (optional) Histogram with the filter kernel (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filter kernel in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The default values are hisf=0, dB=kTRUE, hist=0 and adaptn=kTRUE.
~~~
**/

 TArrayD h(0);
 if (hisf) hisf->Reset();

 if (n<1 || fcut<=0 || fcut>0.5)
 {
  printf(" *%-s::GetLowPassKernel* Invalid input fcut=%-g n=%-i \n",ClassName(),fcut,n);
  return h;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 Double_t twopi=2.*acos(-1.);
 h.Set(n);
 Int_t m=n-1;
 Double_t rm=m;
 Double_t ri=0;
 Double_t sum=0;
 for (Int_t i=0; i<=m; i++)
 {
  if (i==m/2)
  {
   h[i]=twopi*fcut;
  }
  else
  {
   ri=i;
   h[i]=sin(twopi*fcut*double(i-m/2))*(0.42-0.5*cos(twopi*ri/rm)+0.08*cos(2.*twopi*ri/rm))/double(i-m/2);
  }
  sum+=h[i];
 }

 // Normalize the filter kernel to 1
 for (Int_t i=0; i<n; i++)
 {
  h[i]=h[i]/sum;
 }

 HistogramFilterFFT(&h,hisf,dB,kTRUE,hist);

 // Set the appropriate histogram titles
 TString title;
 if (hisf)
 {
  if (fSample>0)
  {
   Float_t nucut=fcut*fSample;
   title.Form("NcDSP Low Pass Filter: Frequency domain kernel with #nu-cut=%-.3g Hz (%-i-point time domain kernel)",nucut,n);
  }
  else
  {
   title.Form("NcDSP Low Pass Filter: Frequency domain kernel with #nu-cut/#nu-sample=%-.3g (%-i-point time domain kenel)",fcut,n);
  }
  hisf->SetTitle(title);
 }

 if (hist)
 {
  if (fSample>0)
  {
   Float_t nucut=fcut*fSample;
   title.Form("NcDSP Low Pass Filter: Time domain kernel (%-i-point zero padded) with #nu-cut=%-.3g Hz",n,nucut);
  }
  else
  {
   title.Form("NcDSP Low Pass Filter: Time domain kernel (%-i-point zero padded) with #nu-cut/#nu-sample=%-.3g",n,fcut);
  }
  hist->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return h;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::GetHighPassKernel(Double_t fcut,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Bool_t adaptn)
{
/**
~~~
// Provide via the returned TArrayD an n-point time domain High Pass Filter kernel
// with cut-off frequency "fcut" expressed as a fraction of the sampling frequency.
// The optional argument "hisf" may be used to obtain a histogram of the kernel
// in the frequency domain with the amplitude in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The optional argument "hist" may be used to obtain a (zero padded) histogram of the time domain kernel.
//
// The implementation here is based on a spectrally inverted Blackman windowed-sinc Low Pass filter.
//
// A Blackman High Pass filter is an excellent frequency domain filter for seperating
// one band of frequencies from another.
// Large values of "n" will result in a sharp transition between the pass band and the stop band,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in a less sharp transition between the
// pass band and the stop band, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) between the pass band and the stop band (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) Repeated application of this filter will further decrease the amplitudes in the stop band.
// 2) Actually this filter kernel consists of a time domain sinc pulse window of "n" points
//    and an area of 1.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// fcut   : The cut-off frequency expressed as a fraction of the sampling frequency
//          For proper functionality one should choose 0<fcut<0.5, because of the
//          underlying Fourier transform.
// n      : The number of values in the filter kernel
//          For best functionality this must be an odd integer (see adaptn below).
// hisf   : (optional) Histogram with the filter kernel (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filter kernel in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The default values are hisf=0, dB=kTRUE, hist=0 and adaptn=kTRUE.
~~~
**/

 TArrayD h(0);
 if (hisf) hisf->Reset();

 if (n<1 || fcut<=0 || fcut>0.5)
 {
  printf(" *%-s::GetHighPassKernel* Invalid input fcut=%-g n=%-i \n",ClassName(),fcut,n);
  return h;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 // The corresponding Low Pass filter kernel
 h=GetLowPassKernel(fcut,n,0,0,0,adaptn);

 // Spectrally invert the filter kernel to obtain the High Pass kernel
 for (Int_t i=0; i<n; i++)
 {
  h[i]=-h[i];
 }
 h[n/2]=h[n/2]+1.;

 HistogramFilterFFT(&h,hisf,dB,kTRUE,hist);

 // Set the appropriate histogram title
 TString title;
 if (hisf)
 {
  if (fSample>0)
  {
   Float_t nucut=fcut*fSample;
   title.Form("NcDSP High Pass Filter: Frequency domain kernel with #nu-cut=%-.3g Hz (%-i-point time domain kernel)",nucut,n);
  }
  else
  {
   title.Form("NcDSP High Pass Filter: Frequency domain kernel with #nu-cut/#nu-sample=%-.3g (%-i-point time domain kernel)",fcut,n);
  }
  hisf->SetTitle(title);
 }

 if (hist)
 {
  if (fSample>0)
  {
   Float_t nucut=fcut*fSample;
   title.Form("NcDSP High Pass Filter: Time domain kernel (%-i-point zero padded) with #nu-cut=%-.3g Hz",n,nucut);
  }
  else
  {
   title.Form("NcDSP High Pass Filter: Time domain kernel (%-i-point zero padded) with #nu-cut/#nu-sample=%-.3g",n,fcut);
  }
  hist->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return h;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::GetBandPassKernel(Double_t f1,Double_t f2,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Bool_t adaptn)
{
/**
~~~
// Provide via the returned TArrayD an n-point time domain Band Pass Filter kernel
// for the frequency band [f1,f2] expressed as fractions of the sampling frequency.
// The optional argument "hisf" may be used to obtain a histogram of the kernel
// in the frequency domain with the amplitude in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The optional argument "hist" may be used to obtain a (zero padded) histogram of the time domain kernel.
//
// The implementation here is based on a combination of Blackman windowed-sinc Low Pass and High Pass filters,
// which is spectrally inverted.
//
// A Blackman Band Pass filter is an excellent frequency domain filter for passing
// only a certain band of frequencies.
// Large values of "n" will result in sharp transitions at the edges of the pass band,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in less sharp transitions at the edges
// of the pass band, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) at the edges of the pass band (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) Repeated application of this filter will further decrease the amplitudes outside the selected band.
// 2) Actually this filter kernel consists of a time domain sinc pulse window of "n" points
//    and an area of 1.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// f1     : The lower bound of the frequency band expressed as a fraction of the sampling frequency
// f2     : The upper bound of the frequency band expressed as a fraction of the sampling frequency
//          For proper functionality one should choose 0<f1<0.5 and 0<f2<0.5, because of the
//          underlying Fourier transform.
// n      : The number of values in the filter kernel
//          For best functionality this must be an odd integer (see adaptn below).
// hisf   : (optional) Histogram with the filter kernel (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filter kernel in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The default values are hisf=0, dB=kTRUE, hist=0 and adaptn=kTRUE.
~~~
**/

 TArrayD h(0);
 if (hisf) hisf->Reset();

 if (n<1 || f1<=0 || f1>0.5 || f2<=0 || f2>0.5 || f2<=f1)
 {
  printf(" *%-s::GetBandPassKernel* Invalid input f1=%-g f2=%-g n=%-i \n",ClassName(),f1,f2,n);
  return h;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 // Get the corresponding Band Reject kernel
 h=GetBandRejectKernel(f1,f2,n,0,0,0,adaptn); 

 // Spectrally invert the Band Reject filter kernel to obtain the Band Pass kernel
 for (Int_t i=0; i<n; i++)
 {
  h[i]=-h[i];
 }
 h[n/2]=h[n/2]+1.;

 HistogramFilterFFT(&h,hisf,dB,kTRUE,hist);

 // Set the appropriate histogram title
 TString title;
 if (hisf)
 {
  if (fSample>0)
  {
   Float_t nu1=f1*fSample;
   Float_t nu2=f2*fSample;
   title.Form("NcDSP Band Pass Filter: Frequency domain kernel for [%-.3g,%-.3g] Hz (%-i-point time domain kernel)",nu1,nu2,n);
  }
  else
  {
   title.Form("NcDSP Band Pass Filter: Frequency domain kernel for #nu/#nu-sample=[%-.3g,%-.3g] (%-i-point time domain kernel)",f1,f2,n);
  }
  hisf->SetTitle(title);
 }

 if (hist)
 {
  if (fSample>0)
  {
   Float_t nu1=f1*fSample;
   Float_t nu2=f2*fSample;
   title.Form("NcDSP Band Pass Filter: Time domain kernel (%-i-point zero padded) for [%-.3g,%-.3g] Hz",n,nu1,nu2);
  }
  else
  {
   title.Form("NcDSP Band Pass Filter: Time domain kernel (%-i-point zero padded) for #nu/#nu-sample=[%-.3g,%-.3g]",n,f1,f2);
  }
  hist->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return h;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::GetBandRejectKernel(Double_t f1,Double_t f2,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Bool_t adaptn)
{
/**
~~~
// Provide via the returned TArrayD an n-point time domain Band Reject Filter kernel
// for the frequency band [f1,f2] expressed as fractions of the sampling frequency.
// The optional argument "hisf" may be used to obtain a histogram of the kernel
// in the frequency domain with the amplitude in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The optional argument "hist" may be used to obtain a (zero padded) histogram of the time domain kernel.
//
// The implementation here is based on a combination of Blackman windowed-sinc Low Pass and High Pass filters.
//
// A Blackman Band Reject filter is an excellent frequency domain filter for suppressing
// a certain band of frequencies.
// Large values of "n" will result in sharp transitions at the edges of the rejected band,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in less sharp transitions at the edges
// of the rejected band, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) at the edges of the rejected band (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) Repeated application of this filter will further decrease the amplitudes inside the selected band.
// 2) Actually this filter kernel consists of a time domain sinc pulse window of "n" points
//    and an area of 1.
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// f1     : The lower bound of the frequency band expressed as a fraction of the sampling frequency
// f2     : The upper bound of the frequency band expressed as a fraction of the sampling frequency
//          For proper functionality one should choose 0<f1<0.5 and 0<f2<0.5, because of the
//          underlying Fourier transform.
// n      : The number of values in the filter kernel
//          For best functionality this must be an odd integer.
// hisf   : (optional) Histogram with the filter kernel (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filter kernel in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The default values are hisf=0, dB=kTRUE, hist=0 and adaptn=kTRUE.
~~~
**/

 TArrayD h(0);
 if (hisf) hisf->Reset();

 if (n<1 || f1<=0 || f1>0.5 || f2<=0 || f2>0.5 || f2<=f1)
 {
  printf(" *%-s::GetBandRejectKernel* Invalid input f1=%-g f2=%-g n=%-i \n",ClassName(),f1,f2,n);
  return h;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 // The Low Pass filter kernel for f1
 TArrayD hlow=GetLowPassKernel(f1,n,0,0,0,adaptn);

 // The High Pass filter kernel for f2
 TArrayD hhigh=GetHighPassKernel(f2,n,0,0,0,adaptn);

 // Add the Low Pass and High Pass kernels to obtain a Band Reject kernel
 h.Set(n);
 for (Int_t i=0; i<n; i++)
 {
  h[i]=hlow[i]+hhigh[i];
 }

 HistogramFilterFFT(&h,hisf,dB,kTRUE,hist);

 // Set the appropriate histogram title
 TString title;
 if (hisf)
 {
  if (fSample>0)
  {
   Float_t nu1=f1*fSample;
   Float_t nu2=f2*fSample;
   title.Form("NcDSP Band Reject Filter: Frequency domain kernel for [%-.3g,%-.3g] Hz (%-i-point time domain kernel)",nu1,nu2,n);
  }
  else
  {
   title.Form("NcDSP Band Reject Filter: Frequency domain kernel for #nu/#nu-sample=[%-.3g,%-.3g] (%-i-point time domain kernel)",f1,f2,n);
  }
  hisf->SetTitle(title);
 }

 if (hist)
 {
  if (fSample>0)
  {
   Float_t nu1=f1*fSample;
   Float_t nu2=f2*fSample;
   title.Form("NcDSP Band Reject Filter: Time domain kernel (%-i-point zero padded) for [%-.3g,%-.3g] Hz",n,nu1,nu2);
  }
  else
  {
   title.Form("NcDSP Band Reject Filter: Time domain kernel (%-i-point zero padded) for #nu/#nu-sample=[%-.3g,%-.3g]",n,f1,f2);
  }
  hist->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return h;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::GetMultiBandKernel(TArray& freqs,Int_t n,TH1* hisf,Bool_t dB,TH1* hist,Bool_t adaptn)
{
/**
~~~
// Provide via the returned TArrayD a time domain Multi Band Filter kernel in various frequency bands
// specified by the array "freqs" (see below) and filter kernels consisting of "n" points each (see below).
// The optional argument "hisf" may be used to obtain a histogram of the kernel
// in the frequency domain with the amplitude in decibel (dB=kTRUE) or linear (dB=kFALSE).
// The optional argument "hist" may be used to obtain a (zero padded) histogram of the time domain kernel.
//
// The procedure is based on a convolution of the various provided Blackman
// single Low Pass and/or High Pass and/or Band Pass and/or Band Reject filters.
//
// Large values of "n" will result in sharp transitions at the edges of the specified bands,
// but may result in long(er) computation times.
// On the contrary, small values of "n" will result in less sharp transitions at the edges
// of the specified bands, but result in short(er) computation times.
//
// Rule of thumb :
// ---------------
// The transition bandwidth (BW) at the edges of the specified bands (aka roll-off)
// may be approximated as BW=4/n, where BW is expressed as a fraction of the sampling frequency.
//
// Notes :
// -------
// 1) The size of the TArrayD "freqs" should match exactly twice the number of
//    frequency bands to be specified (see below).
// 2) Specifying the same frequency band more than once will further suppress the unwanted frequencies. 
//
// For details about the procedure, please refer to the excellent textbook :
// "The Scientist and Engineer's Guide to Digital Signal Processing" by Steven W. Smith,
// which is online available at http://www.dspguide.com/pdfbook.htm
//
// Input arguments :
// -----------------
// freqs  : Array containing the lower and upper bounds of the frequency bands expressed as
//          fractions of the sampling frequency.
//          The array elements represent the various pairs [flow,fup] to define the frequency bands,
//          ordered as (f1low,f1up,f2low,f2up,...).
//          The following conventions are used :
//          flow>0 and fup>0 --> Apply a Band Pass filter over [flow,fup]
//          flow<0 and fup<0 --> Apply a Band Reject filter over [|flow|,|fup|]
//          flow<0 and fup>0 --> Apply a Low Pass filter with fcut=fup
//          flow>0 and fup<0 --> Apply a High Pass filter with fcut=flow
//          In case flow=0 or fup=0 the pair [flow,fup] is neglected.
// n      : The number of points in the corresponding filter kernels
//          For best functionality this must be an odd integer (see adaptn below)
// hisf   : (optional) Histogram with the filter kernel (amplitudes) in the frequency domain
// dB     : Amplitudes of "hisf" are represented in decibel (kTRUE) or linear (kFALSE) 
// hist   : (optional) Histogram with the filter kernel in the time domain
// adaptn : If adaptn=kTRUE an even value of "n" will be increased by 1 to obtain an odd value 
//
// The default values are hisf=0, dB=kTRUE, hist=0 and adaptn=kTRUE.
~~~
**/

 TArrayD h(0); // The convolution of the various filter kernels
 if (hisf) hisf->Reset();

 Int_t arrsize=freqs.GetSize();
 Int_t oddsize=arrsize%2;
 Int_t nbands=arrsize/2;
 if (nbands<1 || n<1 || oddsize)
 {
  printf(" *%-s::GetMultiBandKernel* Invalid input array size=%-i nbands=%-i n=%-i \n",ClassName(),arrsize,nbands,n);
  return h;
 }

 // Save the current stored data
 TArrayD xsave=fReIn;
 TArrayD wfsave=fWaveform;

 // Adapt "n" to an odd value if needed
 Int_t odd=n%2;
 if (!odd && adaptn) n++;

 Double_t flow=0;
 Double_t fup=0;
 Int_t index=0;
 TArrayD hj; // The filter kernel for a single band
 Bool_t first=kTRUE;
 Int_t neff=0; // The number of off actually specified bands
 // Loop over the specified frequency bands
 for (Int_t jband=1; jband<=nbands; jband++)
 {
  index=2*(jband-1);
  flow=freqs.GetAt(index);
  fup=freqs.GetAt(index+1);

  // Skip empty entries in the "freqs" array
  if (!flow || !fup) continue;

  neff++;

  if (flow>0 && fup>0) hj=GetBandPassKernel(flow,fup,n,0,0,0,adaptn);

  if (flow<0 && fup<0) hj=GetBandRejectKernel(fabs(flow),fabs(fup),n,0,0,0,adaptn);

  if (flow<0 && fup>0) hj=GetLowPassKernel(fup,n,0,0,0,adaptn);

  if (flow>0 && fup<0) hj=GetHighPassKernel(flow,n,0,0,0,adaptn);

  SetWaveform(&hj);

  if (first)
  {
   h=hj;
   first=kFALSE;
  }
  else
  {
    Load(&h);
    h=Convolve();
  }
 }

 HistogramFilterFFT(&h,hisf,dB,kTRUE,hist);

 // Set the appropriate histogram titles
 TString title;
 if (hisf)
 {
  title.Form("%-s MultiBand Filter: Frequency domain kernel for %-i bands (%-i-point time domain kernel each)",ClassName(),neff,n);
  hisf->SetTitle(title);
 }

 if (hist)
 {
  title.Form("%-s MultiBand Filter: Time domain kernel (zero padded) for %-i bands (%-i-point kernel each)",ClassName(),neff,n);
  hist->SetTitle(title);
 }

 // Restore the original input data
 Load(&xsave);
 SetWaveform(&wfsave);

 // Let user invokations of Load() reset the output data again
 fKeepOutput=kFALSE;

 return h;
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::HistogramFilterFFT(TArray* h,TH1* hisf,Bool_t dB,Bool_t kernel,TH1* hist)
{
/**
~~~
// Internal member function to provide a filter kernel or filter result histogram in the frequency domain,
// based on the time domain filter kernel or time domain filter result data contained in "h". 
// The amplitude may be represented in decibel (dB=kTRUE) or linear (dB=kFALSE).
// In case of filter kernel data (kernel=kTRUE), the histogram will be rescaled
// such that the maximum value is at 1 for linear amplitudes or 0 for amplitudes in dB. 
// The optional argument "hist" may be used to obtain a (zero padded) histogram of the time domain kernel.
// However, in case kernel=kFALSE, the "hist" histogram will be left empty.
~~~
**/

 if (hisf) hisf->Reset("M");
 if (hist) hist->Reset("M");

 if (!h) return;

 Int_t nh=h->GetSize();

 // Create a temporary array to contain a zero padded copy
 // of the input time domain array "h".
 // This allows to get a better view in the time domain filter kernel
 // and a better resolution in the DFT produced frequency spectrum.
 Int_t npad=0;   // The total number of padded zeros
 Int_t nfront=0; // The number of padded zeros at the front of the array
 Int_t narr=0;   // The length of the zero padded array
 TArrayD arr(narr);

 // The time domain kernel histogram
 if (hist && kernel)
 {
  // The temporary array will be made twice the length of "h"
  // and symmetrically zero padded to display the kernel in the center of the histogram
  narr=2*nh;
  nfront=nh/2;
  arr.Set(narr);
  arr.Reset();
  for (Int_t i=0; i<nh; i++)
  {
   arr[nfront+i]=h->GetAt(i);
  }

  if (fSample>0)
  {
   hist->SetTitle("NcDSP HistogramFilterFFT time domain kernel (zero padded);Time in seconds;Value");
   hist->SetBins(narr,0,double(narr)/fSample);
  }
  else
  {
   hist->SetTitle("NcDSP HistogramFilterFFT time domain kernel (zero padded);Sample number;Value");
   hist->SetBins(narr,0,narr);
  }
  for (Int_t i=1; i<=narr; i++)
  {
   hist->SetBinContent(i,arr[i-1]);
  }
  hist->SetLineWidth(2);
  hist->SetLineColor(kBlue);
 }

 if (!hisf) return;

 // The frequency domain kernel or filter result histogram.
 // Create a temporary zero-padded time domain array with a length of narr=2^k for the FFT,
 // such that the frequency spectrum contains (narr/2)+1 samples.
 // The minimum array length will be 1024 samples.
 Int_t k=log(nh)/log(2);
 k++;
 if (k<10) k=10; // Minimal 1024 samples
 narr=pow(2,k);
 npad=narr-nh;
 nfront=npad/2;

 arr.Set(narr);
 arr.Reset();
 for (Int_t i=0; i<nh; i++)
 {
  arr[nfront+i]=h->GetAt(i);
 }
 
 // Load the temporary zero-padded time domain (kernel) data for Fourier transformation
 Load(&arr);

 TString sel="dB";
 if (!dB) sel="AMP";
 if (fSample>0)
 {
  sel+=" Hz";
 }
 else
 {
  sel+=" f";
 }

 // Perform the Fourier transform
 Fourier("R2C",hisf,sel);

 // Perform the Fourier transform with the original unpadded time domain data
 // in order to store Fourier coefficients that allow retrieving the original
 // unpadded time domain filter result via an inverse Fourier transform from
 // the frequency domain filter result that was produced here.

 // Load the original time domain input data
 Load(h);

 // Perform the Fourier transform
 Fourier("R2C");

 // Prevent following internal Load() invokations to reset the output
 fKeepOutput=kTRUE;

 if (!kernel) return;

 // Normalize the maximum amplitude of "hisf" to 1 (or 0 dB)
 Double_t max=hisf->GetMaximum();
 if (!dB)
 {
  if (max) hisf->Scale(1./fabs(max));
 }
 else
 {
  for (Int_t i=1; i<=hisf->GetNbinsX(); i++)
  {
   hisf->AddBinContent(i,-max);
  }
 }
 hisf->SetLineWidth(2);
 hisf->SetLineColor(kBlue);
}
///////////////////////////////////////////////////////////////////////////
TGraph NcDSP::Periodogram(TString tu,Double_t Tmin,Double_t Tmax,Int_t n,TArray& t,TArray& y,TArray* dy,TF1* Z,NcDevice* results) const
{
/**
~~~
// Provide via the returned TGraph a periodogram of the specified data.
// If the array "dy" is provided, these uncertainties will be used as weights
// in the fitting procedure (see below).
//
// This memberfunction provides a tool to search for periodicity in time series,
// even in case of unequally spaced data.
// The implementation consists of an analytic algorithm known as the
// Generalised Lomb-Scargle (GLS) periodogram and is equivalent to fitting the function
//
//         y(t)=a*Z(t)*cos(2*pi*f*t)+b*Z(t)*sin(2*pi*f*t)+c
//
// where :
// a,b  = Amplitudes
// Z(t) = Amplitude modulation
// c    = Offset
// f    = Frequency
//
// By performing the fit at various frequencies, the corresponding amplitudes can be used
// to provide for each frequency the corresponding power P.
// It is the distribution P(f) that is reflected in the periodogram, where for the implementation
// here the power is normalised to the interval [0,1]. 
//
// Details of the used algorithm can be found at :
//
// M. Zechmeister and M. Kuerster, Astronomy & Astrophysics 496 (2009) 577.
//
// Input arguments :
// -----------------
// tu      : Units of the time recording.
//           Supported are : "ps", "ns", "sec", "hour", "day".
// Tmin    : Lower bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency upper bound of fmax=1/Tmin.
// Tmax    : Upper bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency lower bound of fmin=1/Tmax.
//           Specification of Tmax<=Tmin will result in fmin=0.
// n       : Number of steps to be used for the frequency scan.
//           This will result in n+1 frequency samplings to cover the interval [fmin,fmax].
// t       : Array with the times of the data recordings.
// y       : Array with the observed values y(t).
// dy      : Optional array with the uncertainties of the y-values.
//           dy=0 will result in unweighted treatment of the data.
// Z       : Optional function to describe the amplitude modulation Z(t).
//           Z=0 is equivalent to Z(t)=1, i.e. unmodulated amplitudes.
// results : Optional NcDevice to contain the resulting (f,a,b,c,P) values.
//           Each set (f,a,b,c,P) is stored as a "hit" in the NcDevice, ordered w.r.t. increasing f.
//           Ordering w.r.t. any value can be obtained via the SortHits() facility of NcDevice.
//
// Notes :
// -------
// 1) All provided input arrays should have the same size.
// 2) The values stored in the arrays t, y and (optionally) dy should match 
//    such that (t[i],y[i],dy[i]) represents a single data recording of the time series.
// 3) In case the array dy is provided and some dy[i]=0, the corresponding data recording will be neglected. 
// 4) The settings dy=0 and Z=0 basically result in the original Lomb-Scargle periodogram.
//
// The default values are : dy=0, Z=0 and results=0. 
~~~
**/

  if (results) results->Reset();

  TGraph gr;

  if (tu!="ps" && tu!="ns" && tu!="sec" && tu!="hour" && tu!="day")
  {
   printf("*%-s::Periodogram* Unsupported time unit : %-s \n",ClassName(),tu.Data());
   return gr;
  } 

  if (Tmin<=0 || (Tmax>Tmin && Tmax<=0) || n<1)
  {
   printf("*%-s::Periodogram* Inconsistent input : Tmin=%-g Tmax=%-g n=%-i \n",ClassName(),Tmin,Tmax,n);
   return gr;
  }

  // Check the compatibility of the input array sizes
  Int_t nt=t.GetSize();
  Int_t ny=y.GetSize();
  Int_t ndy=ny;
  if (dy) ndy=dy->GetSize();

  if ((nt+ny+ndy)!=3*nt)
  {
   if (dy)
   {
    printf("*%-s::Periodogram* Incompatible input array sizes : nt=%-i ny=%-i ndy=%-i \n",ClassName(),nt,ny,ndy);
   }
   else
   {
    printf("*%-s::Periodogram* Incompatible input array sizes : nt=%-i ny=%-i \n",ClassName(),nt,ny);
   }
   return gr;
  }

  // Determine the frequency bounds and step size
  Float_t fmax=1./Tmin;
  Float_t fmin=0;
  if (Tmax>Tmin) fmin=1./Tmax;
  Float_t fstep=(fmax-fmin)/float(n);

  // Create compact internal arrays for efficient processing 
  Int_t na=0;
  Float_t ti,yi,dyi2,wi,zi;
  TArrayF tarr(nt);
  TArrayF yarr(nt);
  TArrayF warr(nt);
  TArrayF zarr(nt);
  Float_t Y=0;
  Float_t Y2=0;
  for (Int_t i=0; i<nt; i++)
  {
   ti=t.GetAt(i);
   yi=y.GetAt(i);
   wi=1;
   zi=1;
   if (dy)
   {
    dyi2=pow(dy->GetAt(i),2);
    if (!dyi2) continue;
    wi=1./dyi2;
   }
   if (Z) zi=Z->Eval(ti);
   tarr[na]=ti;
   yarr[na]=yi;
   warr[na]=wi;
   zarr[na]=zi;
   Y+=wi*yi;
   Y2+=wi*pow(yi,2);
   na++;
  }

  tarr.Set(na);
  yarr.Set(na);
  warr.Set(na);
  Float_t W=warr.GetSum();

  if (!na || W<=0)
  {
   printf("*%-s::Periodogram* No valid data available. \n",ClassName());
   return gr;
  }

  Y=Y/W;
  Y2=Y2/W;

  // Loop over the frequencies
  Float_t twopi=2.*acos(-1.);
  Float_t omega=0;
  Float_t zcosi=0;
  Float_t zsini=0;
  Float_t C=0;
  Float_t S=0;
  Float_t CS=0;
  Float_t C2=0;
  Float_t S2=0;
  Float_t YC=0;
  Float_t YS=0;
  Float_t D=0;
  Float_t P=0;
  Float_t a=0;
  Float_t b=0;
  Float_t c=0;
  Int_t ip=0;
  NcSignal sx;
  sx.AddNamedSlot("f");
  sx.AddNamedSlot("a");
  sx.AddNamedSlot("b");
  sx.AddNamedSlot("c");
  sx.AddNamedSlot("P");
  Float_t f=fmin;
  fmax+=0.1*fstep; // To prevent accuracy issues
  while (f<=fmax)
  {
   omega=twopi*f;
   C=0;
   S=0;
   CS=0;
   C2=0;
   S2=0;
   YC=0;
   YS=0;
   // Loop over the data points
   for (Int_t i=0; i<na; i++)
   {
    ti=tarr[i];
    yi=yarr[i];
    wi=warr[i]/W;
    zi=zarr[i];
    zcosi=zi*cos(omega*ti);
    zsini=zi*sin(omega*ti);
    C+=wi*zcosi;
    S+=wi*zsini;
    CS+=wi*zcosi*zsini;
    C2+=wi*pow(zcosi,2);
    S2+=wi*pow(zsini,2);
    YC+=wi*yi*zcosi;
    YS+=wi*yi*zsini;
   }

   CS-=C*S;
   Y2-=pow(Y,2);
   C2-=pow(C,2);
   S2-=pow(S,2);
   YC-=Y*C;
   YS-=Y*S;

   D=C2*S2-pow(CS,2);

   P=0;
   if (Y2*D) P=(S2*pow(YC,2)+C2*pow(YS,2)-2.*CS*YC*YS)/(Y2*D);

   a=0;
   b=0;
   if (D)
   {
    a=(YC*S2-YS*CS)/D;
    b=(YS*C2-YC*CS)/D;
   }
   c=Y-a*C-b*S;

   // Record this (f,a,b,c,P) dataset as a hit in the (optional) NcDevice "results"
   if (results)
   {
    sx.SetSignal(f,"f");
    sx.SetSignal(a,"a");
    sx.SetSignal(b,"b");
    sx.SetSignal(c,"c");
    sx.SetSignal(P,"P");
    results->AddHit(sx);
   }

   // Record this entry in the periodogram graph
   gr.SetPoint(ip,f,P);

   f+=fstep;
   ip++;
  }

 TString unit="Hz";
 if (tu=="ps") unit="THz";
 if (tu=="ns") unit="GHz";
 if (tu=="hour") unit="1/hour";
 if (tu=="day") unit="1/day";
 TString title;
 title.Form("%-s Periodogram;Frequency in %-s;Normalized Power",ClassName(),unit.Data());
 gr.SetNameTitle("Periodogram",title);
 gr.SetMarkerStyle(8);
 gr.SetMarkerSize(0.5);
 gr.SetMarkerColor(kBlue);
 gr.SetLineWidth(2);

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph NcDSP::Periodogram(TString tu,Double_t Tmin,Double_t Tmax,Int_t n,NcSample s,Int_t it,Int_t iy,Int_t idy,TF1* Z,NcDevice* results) const
{
/**
~~~
// Provide via the returned TGraph a periodogram of the specified data as stored in the NcSample object "s".
// Note that the storage mode for the NcSample object has to be activated. 
// If the index "idy" is provided, the corresponding values will be regarded as uncertainties
// and will be used as weights in the fitting procedure (see below).
//
// This memberfunction provides a tool to search for periodicity in time series,
// even in case of unequally spaced data.
// The implementation consists of an analytic algorithm known as the
// Generalised Lomb-Scargle (GLS) periodogram and is equivalent to fitting the function
//
//         y(t)=a*Z(t)*cos(2*pi*f*t)+b*Z(t)*sin(2*pi*f*t)+c
//
// where :
// a,b  = Amplitudes
// Z(t) = Amplitude modulation
// c    = Offset
// f    = Frequency
//
// By performing the fit at various frequencies, the corresponding amplitudes can be used
// to provide for each frequency the corresponding power P.
// It is the distribution P(f) that is reflected in the periodogram, where for the implementation
// here the power is normalised to the interval [0,1]. 
//
// Details of the used algorithm can be found at :
//
// M. Zechmeister and M. Kuerster, Astronomy & Astrophysics 496 (2009) 577.
//
// Input arguments :
// -----------------
// tu      : Units of the time recording.
//           Supported are : "ps", "ns", "sec", "hour", "day".
// Tmin    : Lower bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency upper bound of fmax=1/Tmin.
// Tmax    : Upper bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency lower bound of fmin=1/Tmax.
//           Specification of Tmax<=Tmin will result in fmin=0.
// n       : Number of steps to be used for the frequency scan.
//           This will result in n+1 frequency samplings to cover the interval [fmin,fmax].
// s       : The NcSample object containing the recorded data.
// it      : Index (1=first) of the NcSample variable with the times of the data recordings.
// iy      : Index (1=first) of the NcSample variable with the observed values y(t).
// idy     : Optional index (1=first) of the NcSample varaible with the uncertainties of the y-values.
//           idy=0 will result in unweighted treatment of the data.
// Z       : Optional function to describe the amplitude modulation Z(t).
//           Z=0 is equivalent to Z(t)=1, i.e. unmodulated amplitudes.
// results : Optional NcDevice to contain the resulting (f,a,b,c,P) values.
//           Each set (f,a,b,c,P) is stored as a "hit" in the NcDevice, ordered w.r.t. increasing f.
//           Ordering w.r.t. any value can be obtained via the SortHits() facility of NcDevice.
//
// Notes :
// -------
// 1) Each entry of the provided NcSample object represents a single data recording of the time series.
// 2) In case the index idy>0 is provided but a stored "y uncertainty" is 0, the corresponding NcSample entry will be neglected. 
// 3) The settings idxdy=0 and Z=0 basically result in the original Lomb-Scargle periodogram.
//
// The default values are : idxdy=0, Z=0 and results=0. 
~~~
**/

 TGraph gr;

 if (!s.GetStoreMode())
 {
  printf("*%-s::Periodogram* Error: NcSample storage mode is not activated. \n",ClassName());
  return gr;
 }

 Int_t ndim=s.GetDimension();
 Int_t ns=s.GetN();

 if (ns<1 || it<1 || it>ndim || iy<1 || iy>ndim || idy<0 || idy>ndim)
 {
  printf("*%-s::Periodogram* Inconsistent NcSample input: it=%-i iy=%-i idy=%-i #entries=%-i \n",ClassName(),it,iy,idy,ns);
  return gr;
 }

 TArrayD t(ns);
 TArrayD y(ns);
 TArrayD dy(ns);

 for (Int_t i=1; i<=ns; i++)
 {
  t[i-1]=s.GetEntry(i,it);
  y[i-1]=s.GetEntry(i,iy);
  if (idy) dy[i-1]=s.GetEntry(i,idy);
 }

 if (idy) // Uncertainties will be used as weights
 {
  gr=Periodogram(tu,Tmin,Tmax,n,t,y,&dy,Z,results);
 }
 else // No weights will be used
 {
  gr=Periodogram(tu,Tmin,Tmax,n,t,y,0,Z,results);
 }

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph NcDSP::Periodogram(TString tu,Double_t Tmin,Double_t Tmax,Int_t n,NcSample s,TString namet,TString namey,TString namedy,TF1* Z,NcDevice* results) const
{
/**
~~~
// Provide via the returned TGraph a periodogram of the specified data as stored in the NcSample object "s".
// Note that the storage mode for the NcSample object has to be activated. 
// If a valid "namedy" is provided, the corresponding values will be regarded as uncertainties
// and will be used as weights in the fitting procedure (see below).
//
// This memberfunction provides a tool to search for periodicity in time series,
// even in case of unequally spaced data.
// The implementation consists of an analytic algorithm known as the
// Generalised Lomb-Scargle (GLS) periodogram and is equivalent to fitting the function
//
//         y(t)=a*Z(t)*cos(2*pi*f*t)+b*Z(t)*sin(2*pi*f*t)+c
//
// where :
// a,b  = Amplitudes
// Z(t) = Amplitude modulation
// c    = Offset
// f    = Frequency
//
// By performing the fit at various frequencies, the corresponding amplitudes can be used
// to provide for each frequency the corresponding power P.
// It is the distribution P(f) that is reflected in the periodogram, where for the implementation
// here the power is normalised to the interval [0,1]. 
//
// Details of the used algorithm can be found at :
//
// M. Zechmeister and M. Kuerster, Astronomy & Astrophysics 496 (2009) 577.
//
// Input arguments :
// -----------------
// tu      : Units of the time recording.
//           Supported are : "ps", "ns", "sec", "hour", "day".
// Tmin    : Lower bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency upper bound of fmax=1/Tmin.
// Tmax    : Upper bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency lower bound of fmin=1/Tmax.
//           Specification of Tmax<=Tmin will result in fmin=0.
// n       : Number of steps to be used for the frequency scan.
//           This will result in n+1 frequency samplings to cover the interval [fmin,fmax].
// s       : The NcSample object containing the recorded data.
// namet   : Name of the NcSample variable with the times of the data recordings.
// namey   : Name of the NcSample variable with the observed values y(t).
// namedy  : Name of the optional NcSample variable with the uncertainties of the y-values.
//           namedy="-" will result in unweighted treatment of the data.
// Z       : Optional function to describe the amplitude modulation Z(t).
//           Z=0 is equivalent to Z(t)=1, i.e. unmodulated amplitudes.
// results : Optional NcDevice to contain the resulting (f,a,b,c,P) values.
//           Each set (f,a,b,c,P) is stored as a "hit" in the NcDevice, ordered w.r.t. increasing f.
//           Ordering w.r.t. any value can be obtained via the SortHits() facility of NcDevice.
//
// Notes :
// -------
// 1) Each entry of the provided NcSample object represents a single data recording of the time series.
// 2) In case a valid namedy is provided but a stored "y uncertainty" is 0, the corresponding NcSample entry will be neglected. 
// 3) The settings namedy="-" and Z=0 basically result in the original Lomb-Scargle periodogram.
//
// The default values are : namedy="-", Z=0 and results=0. 
~~~
**/

 TGraph gr;

 // Get the indices corresponding to the specified variable names
 Int_t it=s.GetIndex(namet);
 Int_t iy=s.GetIndex(namey);
 Int_t idy=s.GetIndex(namedy);

 // Check for unsupported namedy
 if (!idy && namedy!="-") idy=-1;

 Int_t ns=s.GetN();

 if (ns<1 || it<1 || iy<1 || idy<0)
 {
  printf("*%-s::Periodogram* Inconsistent NcSample input: namet=%-s namey=%-s namedy=%-s #entries=%-i \n",ClassName(),namet.Data(),namey.Data(),namedy.Data(),ns);
  return gr;
 }

 gr=Periodogram(tu,Tmin,Tmax,n,s,it,iy,idy,Z,results);

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph NcDSP::Periodogram(TString tu,Double_t Tmin,Double_t Tmax,Int_t n,TH1& h,Bool_t err,TF1* Z,NcDevice* results) const
{
/**
~~~
// Provide via the returned TGraph a periodogram of the data contained in the histogram "h".
// If err=kTRUE, the uncertainties of the histogram values will be used as weights in the fitting procedure (see below).
//
// This memberfunction provides a tool to search for periodicity in time series,
// even in case of unequally spaced data.
// The implementation consists of an analytic algorithm known as the
// Generalised Lomb-Scargle (GLS) periodogram and is equivalent to fitting the function
//
//         y(t)=a*Z(t)*cos(2*pi*f*t)+b*Z(t)*sin(2*pi*f*t)+c
//
// where :
// a,b  = Amplitudes
// Z(t) = Amplitude modulation
// c    = Offset
// f    = Frequency
//
// By performing the fit at various frequencies, the corresponding amplitudes can be used
// to provide for each frequency the corresponding power P.
// It is the distribution P(f) that is reflected in the periodogram, where for the implementation
// here the power is normalised to the interval [0,1]. 
//
// Details of the used algorithm can be found at :
//
// M. Zechmeister and M. Kuerster, Astronomy & Astrophysics 496 (2009) 577.
//
// Input arguments :
// -----------------
// tu      : Units of the time recording.
//           Supported are : "ps", "ns", "sec", "hour", "day".
// Tmin    : Lower bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency upper bound of fmax=1/Tmin.
// Tmax    : Upper bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency lower bound of fmin=1/Tmax.
//           Specification of Tmax<=Tmin will result in fmin=0.
// n       : Number of steps to be used for the frequency scan.
//           This will result in n+1 frequency samplings to cover the interval [fmin,fmax].
// h       : The histogram containing the recorded data (t,y).
// err     : Flag to trigger the use of the uncertainties of the y-values (kTRUE) or not (kFALSE).
//           err=kFALSE will result in unweighted treatment of the data.
// Z       : Optional function to describe the amplitude modulation Z(t).
//           Z=0 is equivalent to Z(t)=1, i.e. unmodulated amplitudes.
// results : Optional NcDevice to contain the resulting (f,a,b,c,P) values.
//           Each set (f,a,b,c,P) is stored as a "hit" in the NcDevice, ordered w.r.t. increasing f.
//           Ordering w.r.t. any value can be obtained via the SortHits() facility of NcDevice.
//
// Notes :
// -------
// 1) Each bin of the provided histogram represents a single data recording of the time series.
// 2) In case err=kTRUE but a stored "y uncertainty" is 0, the corresponding bin will be neglected. 
// 3) The settings err=kFALSE and Z=0 basically result in the original Lomb-Scargle periodogram.
//
// The default values are : err=kFALSE, Z=0 and results=0. 
~~~
**/

 TGraph gr;

 Int_t nbins=h.GetNbinsX();
 Int_t nen=h.GetEntries();

 if (nbins<1 || nen<1)
 {
  printf("*%-s::Periodogram* Inconsistent histogram input: nbins=%-i #entries=%-i \n",ClassName(),nbins,nen);
  return gr;
 }

 TArrayD t(nbins);
 TArrayD y(nbins);
 TArrayD dy(nbins);

 for (Int_t i=1; i<=nbins; i++)
 {
  t[i-1]=h.GetBinCenter(i);
  y[i-1]=h.GetBinContent(i);
  if (err) dy[i-1]=h.GetBinError(i);
 }

 if (err) // Uncertainties will be used as weights
 {
  gr=Periodogram(tu,Tmin,Tmax,n,t,y,&dy,Z,results);
 }
 else // No weights will be used
 {
  gr=Periodogram(tu,Tmin,Tmax,n,t,y,0,Z,results);
 }

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph NcDSP::Periodogram(TString tu,Double_t Tmin,Double_t Tmax,Int_t n,TGraph& g,Bool_t err,TF1* Z,NcDevice* results) const
{
/**
~~~
// Provide via the returned TGraph a periodogram of the data contained in the graph "g".
// If err=kTRUE, the uncertainties of the graph values will be used as weights in the fitting procedure (see below).
// Note: Treatment of the uncertainties can only be performed if "g" is a TGraphErrors object. Not for a TGraph.
//
// This memberfunction provides a tool to search for periodicity in time series,
// even in case of unequally spaced data.
// The implementation consists of an analytic algorithm known as the
// Generalised Lomb-Scargle (GLS) periodogram and is equivalent to fitting the function
//
//         y(t)=a*Z(t)*cos(2*pi*f*t)+b*Z(t)*sin(2*pi*f*t)+c
//
// where :
// a,b  = Amplitudes
// Z(t) = Amplitude modulation
// c    = Offset
// f    = Frequency
//
// By performing the fit at various frequencies, the corresponding amplitudes can be used
// to provide for each frequency the corresponding power P.
// It is the distribution P(f) that is reflected in the periodogram, where for the implementation
// here the power is normalised to the interval [0,1]. 
//
// Details of the used algorithm can be found at :
//
// M. Zechmeister and M. Kuerster, Astronomy & Astrophysics 496 (2009) 577.
//
// Input arguments :
// -----------------
// tu      : Units of the time recording.
//           Supported are : "ps", "ns", "sec", "hour", "day".
// Tmin    : Lower bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency upper bound of fmax=1/Tmin.
// Tmax    : Upper bound of the period (in units of "tu") to be investigated.
//           This will lead to a frequency lower bound of fmin=1/Tmax.
//           Specification of Tmax<=Tmin will result in fmin=0.
// n       : Number of steps to be used for the frequency scan.
//           This will result in n+1 frequency samplings to cover the interval [fmin,fmax].
// g       : The graph containing the recorded data (t,y).
// err     : Flag to trigger the use of the uncertainties of the y-values (kTRUE) or not (kFALSE).
//           err=kFALSE will result in unweighted treatment of the data.
// Z       : Optional function to describe the amplitude modulation Z(t).
//           Z=0 is equivalent to Z(t)=1, i.e. unmodulated amplitudes.
// results : Optional NcDevice to contain the resulting (f,a,b,c,P) values.
//           Each set (f,a,b,c,P) is stored as a "hit" in the NcDevice, ordered w.r.t. increasing f.
//           Ordering w.r.t. any value can be obtained via the SortHits() facility of NcDevice.
//
// Notes :
// -------
// 1) Each point of the provided graph represents a single data recording of the time series.
// 2) In case err=kTRUE but a stored "y uncertainty" is 0, the corresponding point will be neglected. 
// 3) The settings err=kFALSE and Z=0 basically result in the original Lomb-Scargle periodogram.
//
// The default values are : err=kFALSE, Z=0 and results=0. 
~~~
**/

 TGraph gr;

 if (err && !(g.InheritsFrom("TGraphErrors")))
 {
  printf("*%-s::Periodogram* Error: For weighted fitting the input graph has to be a TGraphErrors object. \n",ClassName());
  return gr;
 }

 Int_t np=g.GetN();

 if (np<1)
 {
  printf("*%-s::Periodogram* Error: Input graph is empty. \n",ClassName());
  return gr;
 }

 TArrayD t(np);
 TArrayD y(np);
 TArrayD dy(np);

 Double_t gx=0;
 Double_t gy=0;
 for (Int_t i=0; i<np; i++)
 {
  g.GetPoint(i,gx,gy);
  t[i]=gx;
  y[i]=gy;
  if (err) dy[i]=g.GetErrorY(i);
 }

 if (err) // Uncertainties will be used as weights
 {
  gr=Periodogram(tu,Tmin,Tmax,n,t,y,&dy,Z,results);
 }
 else // No weights will be used
 {
  gr=Periodogram(tu,Tmin,Tmax,n,t,y,0,Z,results);
 }

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TObject* NcDSP::Clone(const char* name) const
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

 NcDSP* q=new NcDSP(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
