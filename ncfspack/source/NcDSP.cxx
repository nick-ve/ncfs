/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 2021-2023, NCFS/IIHE, All Rights Reserved.                     *
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
// In addition to the above transformations, also convolution, correlation
// and digitization processors are provided.
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
// hx.Draw("P");
// c.cd(2);
// hh.SetMarkerStyle(20);
// hh.Draw("P");
// c.cd(3);
// hy.Draw("P");
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
// hx.Draw("P");
// c.cd(2);
// hh.SetMarkerStyle(20);
// hh.Draw("P");
// c.cd(3);
// hy.Draw("P");
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, October 19, 2021  09:42Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, February 3, 2023  14:17Z
///////////////////////////////////////////////////////////////////////////

#include "NcDSP.h"
#include "Riostream.h"
 
ClassImp(NcDSP) // Class implementation to enable ROOT I/O
 
NcDSP::NcDSP(const char* name,const char* title) : TNamed(name,title)
{
// Default constructor.

 Reset();
 fSample=0;
}
///////////////////////////////////////////////////////////////////////////
NcDSP::~NcDSP()
{
// Default destructor.

 if (fProc)
 {
  delete fProc;
  fProc=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcDSP::NcDSP(const NcDSP& q) : TNamed(q)
{
// Copy constructor.

 fProc=0;
 fN=q.fN;
 fNwf=q.fNwf;
 fReIn=q.fReIn;
 fImIn=q.fImIn;
 fReOut=q.fReOut;
 fImOut=q.fImOut;
 fSample=q.fSample;
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::Reset()
{
// Internal member function to reset all data and the FFTW processor. 

 fProc=0;
 fN=0;
 fNwf=0;
 fReIn.Set(0);
 fImIn.Set(0);
 fReOut.Set(0);
 fImOut.Set(0);
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetSamplingFrequency(Float_t f)
{
// Set the actual DAQ sampling frequency in Hz.
// This sampling frequency may be overwritten by invokation of
// one of the various Load() member functions.
// The sampling frequency is set to 0 in the constructor of this class.

 fSample=f;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcDSP::GetSamplingFrequency() const
{
// Provide the current value of the DAQ sampling frequency in Hz.

 return fSample;
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::Load(Int_t n,Double_t* re,Double_t* im,Float_t f)
{
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
// Load the current transformation result as new input data in order to enable
// inverse transformations acting on previous transformation results.
//
// Notes :
// -------
// 1) Invokation of one of the other Load() memberfunctions will reset the internal storage,
//    by which previously obtained results will internally be lost.
//    Use the GetData() memberfunction to retrieve data that might be needed later again. 
// 2) The (optional) waveform data stored via SetWaveform() will not be modified.

 fReIn=fReOut;
 fImIn=fImOut;
 fReOut.Set(0);
 fImOut.Set(0);
}
///////////////////////////////////////////////////////////////////////////
void NcDSP::SetWaveform(Int_t n,Double_t* h,Float_t f)
{
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
// Provide the number of data elements (to be) processed.
//
// mode : 0 --> Provide the number of input data elements entered via Load()
//        1 --> Provide the number of input data elements entered via SetWaveform()
//
// The default value is mode=0 for backward compatibility.

 Int_t n=fN;

 if (mode==1) n=fNwf;

 return n;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::GetData(TString sel) const
{
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
//        "Wave" --> The values of the stored (system response) waveform data are provided
//
// Examples :
// ----------
// sel="AMP out" will provide all the N amplitudes of the resulting data after transformation.
// sel="RE in" will provide all the N real components of the input data.
// sel="Wave" will provide all the amplitudes of the stored (system response) waveform

 if (sel.Contains("RE") && sel.Contains("in")) return fReIn;
 if (sel.Contains("IM") && sel.Contains("in")) return fImIn;
 if (sel.Contains("RE") && sel.Contains("out")) return fReOut;
 if (sel.Contains("IM") && sel.Contains("out")) return fImOut;
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
void NcDSP::Hartley(Int_t mode,TH1* hist,TString sel)
{
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
void NcDSP::Cosine(Int_t type,TH1* hist,TString sel)
{
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
void NcDSP::Sine(Int_t type,TH1* hist,TString sel)
{
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
TArrayD NcDSP::Convolve(TH1* hist,Int_t* i1,Int_t* i2)
{
// Convolve the loaded input data x[] with the data contained in the (system response)
// waveform h[] and return the resulting data y[] in a TArrayD object.
// The input data x[] have to be entered as real numbers by one of the Load() member functions,
// whereas the (system response) waveform h[] has to be specified by SetWaveform().
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
// input (signal) data.
// These values i1 and i2 (if requested) are indicated by vertical
// dashed blue lines in the histogram.
//
// Note : The sampling (rate) of h[] has to be the same as for the input data x[].
//
// The default values are hist=0, i1=0 and i2=0.

 TArrayD y(0);
 if (hist) hist->Reset();
 Int_t nx=fReIn.GetSize();
 Int_t nh=fWaveform.GetSize();

 if (nh<1 || nx<1)
 {
  printf(" *%-s::Convolve* Input or Waveform data are missing. Ninput=%-i Nwaveform=%-i \n",ClassName(),nx,nh);
  return y;
 }

 Int_t ny=nx+nh-1;
 y.Set(ny);

 for (Int_t i=0; i<ny; i++)
 {
  y[i]=0;
 }

 Double_t* x=fReIn.GetArray();
 Double_t* h=fWaveform.GetArray();

 // Convolution from the input signal viewpoint
 for (Int_t ix=0; ix<nx; ix++)
 {
  for (Int_t ih=0; ih<nh; ih++)
  {
   y[ix+ih]=y[ix+ih]+x[ix]*h[ih];
  }
 }

 if (i1) *i1=nh-1;
 if (i2) *i2=ny-nh;

 if (hist)
 {
  TString title;
  if (fSample>0)
  {
  title.Form("NcDSP Convolution result (%-g samples/sec);Time in seconds;Value",fSample);
  hist->SetBins(ny,0,double(ny)/fSample);
  }
  else
  {
  title.Form("NcDSP Convolution result;Sample number;Value");
  hist->SetBins(ny,0,ny);
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

 return y;
}
///////////////////////////////////////////////////////////////////////////
TArrayD NcDSP::Correlate(TH1* hist,Int_t* i1,Int_t* i2)
{
// (Cross) Correlate the data contained in the waveform h[] with the loaded input data x[]
// and return the resulting data y[] in a TArrayD object.
// The input data x[] have to be entered as real numbers by one of the Load() member functions,
// whereas the waveform h[] has to be specified by SetWaveform().
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
// Note : The sampling (rate) of h has to be the same as for the loaded input data x[].
//
// The default values are hist=0, i1=0 and i2=0.

 TArrayD y(0);
 if (hist) hist->Reset();
 Int_t nx=fReIn.GetSize();
 Int_t nh=fWaveform.GetSize();

 if (nh<1 || nx<1)
 {
  printf(" *%-s::Correlate* Input or Waveform data are missing. Ninput=%-i Nwaveform=%-i \n",ClassName(),nx,nh);
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
 y=Convolve(hist,i1,i2);

 // Put the correct histogram title
 if (hist)
 {
  TString title;
  if (fSample>0)
  {
   title.Form("NcDSP Correlation result (%-g samples/sec)",fSample);
  }
  else
  {
   title.Form("NcDSP Correlation result");
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
// Digitize the values of the stored waveform according to an "nbits" ADC.
// The resulting digitized values are returned in a TArrayD object.
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
TArrayD NcDSP::SampleAndHold(TF1 f,Double_t step,Double_t vmin,Double_t vmax,TH1* hist,Int_t loc) const
{
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
// Perform a Sample-And-Hold operation on the data contained in the stored waveform
// over the sampled interval [jmin,jmax], using "n" original samples as the new sampling step size.
// By convention, the first sample is at j=0.
// The result is returned in a TArrayD object and (optionally) in the histogram "hist".
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
// Perform a Sample-And-Sum operation on the data contained in the stored waveform
// over the sampled interval [jmin,jmax], using "n" original samples as the new sampling step size.
// By convention, the first sample is at j=0.
// The result is returned in a TArrayD object and (optionally) in the histogram "hist".
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
TObject* NcDSP::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.

 NcDSP* q=new NcDSP(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
