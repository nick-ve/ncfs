/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2009, NCFS, All Rights Reserved.                          *
 *                                                                             *
 * Author: The Netherlands Center for Fundamental Studies (NCFS).              *
 *         http://sites.google.com/site/ncfsweb ncfs.nl@gmail.com              *
 *                                                                             *
 * Contributors are mentioned in the code where appropriate.                   *
 *                                                                             * 
 * No part of this software may be used, copied, modified or distributed       *
 * by any means nor transmitted or translated into machine language without    *
 * written permission by the NCFS.                                             *
 * Permission to use the documentation strictly for non-commercial purposes    *
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

// $Id: NcPsiDistrib.cxx 5 2010-03-19 10:10:02Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcPsiDistrib
// Tool to analyse the distribution of possibly observed psi values
// (see NcMath::PsiValue()) in the case of random background only
// or in the presence of a user-specified signal.
//
// Example: comparing two psi distributions for 60 observed events:
// 1) Under the hypothesis of background only
// 2) Under the hypothesis of a signal of 10 events in one bin
//
// Int_t ntrials=10000;
// Int_t nbins=1000;
// Int_t nsig=10;
// Int_t nbg=50;
// 
// NcPsiDistrib pd;
// pd.SetNtrials(ntrials);
// pd.SetNoutcomes(nbins);
//
// // BG only
// pd.SetNevents(nbg+nsig);
// Int_t sig[nbins];
// for(Int_t i=0; i<nbins; i++) { sig[i]=0; }
// pd.SetSignal(sig);
// pd.Distribute(1);
// TH1D bgonly;
// pd.GetPsiHisto()->Copy(bgonly);
// Double_t meanbgonly=pd.GetPsiSample()->GetMean(1);
// Double_t spreadbgonly=pd.GetPsiSample()->GetSigma(1);
// Double_t medianbgonly=pd.GetPsiSample()->GetMedian(1);
// 
// // Signal
// pd.SetNevents(nbg);
// sig[0]=nsig; // Add signal events
// pd.SetSignal(sig);
// pd.Distribute(1);
// TH1D bgplussig;
// pd.GetPsiHisto()->Copy(bgplussig);
// Double_t meansig=pd.GetPsiSample()->GetMean(1);
// Double_t spreadsig=pd.GetPsiSample()->GetSigma(1);
// Double_t mediansig=pd.GetPsiSample()->GetMedian(1);
// 
// // Output
// cout << nbg << " background events, " << nsig << " signal events" << endl;
// cout << "Background only: psi = " << meanbgonly << " +/- " << spreadbgonly << ", median = " << medianbgonly << endl;
// cout << "Signal: psi = " << meansig << " +/- " << spreadsig << ", median = " << mediansig << endl;
// 
// // Draw histograms
// bgonly.Draw();
// bgplussig.Draw("same");
// 
//--- Author: Garmt de Vries-Uiterweerd 15-jun-2008 Utrecht University
//- Modified: NvE $Date: 2010-03-19 11:10:02 +0100 (Fri, 19 Mar 2010) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "NcPsiDistrib.h"
#include "Riostream.h"

ClassImp(NcPsiDistrib) // Class implementation to enable ROOT I/O

NcPsiDistrib::NcPsiDistrib()
{
// Default constructor.
 fNtrials=10000;
 fNevents=100;
 fNoutcomes=100;
 fProbs=new Double_t[fNoutcomes];
 fSignal=new Int_t[fNoutcomes];
 for(Int_t i=0; i<fNoutcomes; i++)
 {
  fProbs[i]=1./fNoutcomes;
  fSignal[i]=0;
 }
 fRefPsi=0;
 fNbelow=0;
 fNabove=0;
 fPrintFreq=0;

 fRangeSet=kFALSE;
 fPsiHisto=new TH1D("psi","psi distribution",100,0,1);
 fPsiHisto->SetDirectory(0);
 fPsiHisto->SetXTitle("psi");
 fPsiHisto->SetFillColor(4);

 fSample=new NcSample();
// fSample->SetStoreMode(1);
}
///////////////////////////////////////////////////////////////////////////
NcPsiDistrib::~NcPsiDistrib()
{
// Default destructor.
 if(fProbs)
 {
  delete[] fProbs;
  fProbs=0;
 }
 if(fSignal)
 {
  delete[] fSignal;
  fSignal=0;
 }
 if(fPsiHisto)
 {
  delete fPsiHisto;
  fPsiHisto=0;
 }
 if(fSample)
 {
  delete fSample;
  fSample=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::SetNtrials(Long_t n)
{
// Set number of trials.
 fNtrials=n;
}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::SetNevents(Int_t n)
{
// Set number of background events to simulate.
 fNevents=n;
}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::SetNoutcomes(Int_t n, Double_t* p)
{
// Set number of possible outcomes.
// The optional argument p is an array containing the respective probabilities
// of the outcomes. Its length must be equal to the number of outcomes. In 
// case no probabilities are specified, a flat distribution is assumed.
// Setting the number of outcomes resets any signal that may be present.
 fNoutcomes=n;
 if(fProbs)
 {
  delete[] fProbs;
 }
 fProbs=new Double_t[fNoutcomes];

 SetProbabilities(p);

 if(fSignal)
 {
  delete[] fSignal;
  fSignal=new Int_t[fNoutcomes];
  for(Int_t i=0; i<fNoutcomes; i++){ fSignal[i]=0; }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::SetProbabilities(Double_t* p)
{
// Set probabilities of the possible outcomes.
// The length of the array passed as an argument must be equal to the number 
// of outcomes. In case p=0, a flat distribution is made.
 if(p)
 {
  Double_t sum=0;
  for(Int_t i=0; i<fNoutcomes; i++) { sum+=p[i]; }
  if(sum<=0)
  {
   cout << "NcPsiDistrib: Sum of probabilities is not positive" << endl;
   sum=1;
  }
  for(Int_t i=0; i<fNoutcomes; i++) { fProbs[i]=p[i]/sum; }
 }
 else
 {
  for(Int_t i=0; i<fNoutcomes; i++){ fProbs[i]=1./fNoutcomes; }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::SetSignal(Int_t* s)
{
// Set signal counts for each outcome.
// The length of the array passed as an argument must be equal to the number of outcomes.
// In case s=0, the signal is set to 0 for all outcomes.
 if(s)
 {
  for(Int_t i=0; i<fNoutcomes; i++)
  {
   fSignal[i]=s[i];
  }
 }
 else
 {
  for(Int_t i=0; i<fNoutcomes; i++)
  {
   fSignal[i]=0;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::SetPsiRange(Int_t nb, Float_t low, Float_t high)
{
// Set number of bins and range of the psi histogram.
 fPsiHisto->SetBins(nb,low,high);
 fRangeSet=kTRUE;
}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::SetReferencePsi(Double_t ref)
{
// Set reference psi.
 fRefPsi=ref;
}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::SetPrintFreq(Int_t freq)
{
// Set print frequency.
 fPrintFreq=freq;
}
///////////////////////////////////////////////////////////////////////////
//void NcPsiDistrib::SetSampleStoreMode(Int_t mode)
//{
// Set store mode of the psi sample (see NcSample::SetStoreMode()).
// This resets the sample, and 
// fSample->SetStoreMode(mode);
//}
///////////////////////////////////////////////////////////////////////////
void NcPsiDistrib::Distribute(Int_t storemode)
{
// Make distribution.
// The parameter storemode is the mode to be used for the NcSample of the 
// psi distribution. In case storemode=1, entered data will be stored in the 
// sample, allowing the user to find the median of the psi distribution.
// Setting mode=1 can be memory-intensive, and is not advised when dealing
// with large number of trials. The default is storemode=0, meaning no storage
// of entered data.

 // Variables we need
 Int_t* data=new Int_t[fNoutcomes];
 NcMath math;
 Double_t psi=0;
 char title[100];

 // Reference histo with probabilities
 TH1D ref("ref","ref",fNoutcomes,0,1);
 for(Int_t i=0; i<fNoutcomes; i++)
 {
  ref.SetBinContent(i+1,fProbs[i]);
 }

 // Make histogram for events
 TH1D histo("histo","events",fNoutcomes,0,1);

 // Reset stuff
 fSample->Reset();
 fSample->SetStoreMode(storemode);
 fPsiHisto->Reset();
 sprintf(title,"psi_%d_%d",fNevents,fNoutcomes);
 fPsiHisto->SetName(title);
 fNbelow=0;
 fNabove=0;

 // Set range for psi histogram
 if(!fRangeSet){
  fPsiHisto->SetBins(100,0,1.1*FindMaxPsi());
 }

 // Loop over trials
 for(Int_t itrial=0; itrial<fNtrials; itrial++){
  if(fPrintFreq>0 && itrial%fPrintFreq==0) cout << "Trial " << itrial << endl;

  // Fill event histogram
  histo.Reset();
  histo.FillRandom(&ref, fNevents);

  // Add signal if prescribed
  for(Int_t i=0; i<fNoutcomes; i++)
  {
   histo.SetBinContent(i+1, histo.GetBinContent(i+1)+fSignal[i]);
  }

  // Calculate psi
  for(Int_t i=0; i<fNoutcomes; i++)
  {
   data[i]=int(histo.GetBinContent(i+1));
  }
  psi=math.PsiValue(fNoutcomes, data, fProbs);
  fSample->Enter(psi);
  fPsiHisto->Fill(psi);

  // Keep track of # below/above ref psi
  if(fRefPsi)
  {
   if(psi<fRefPsi) fNbelow++;
   if(psi>fRefPsi) fNabove++;
  }
 }

 delete[] data;
 
 // Set psi distribution histo name
 sprintf(title,"psi distribution (%d events, %d time bins): mean %.2f, sigma %.2e",fNevents,fNoutcomes,fSample->GetMean(1),fSample->GetSigma(1));
 fPsiHisto->SetTitle(title);
}
///////////////////////////////////////////////////////////////////////////
TH1D* NcPsiDistrib::GetPsiHisto()
{
// Get histogram with psi distribution.
 return fPsiHisto;
}
///////////////////////////////////////////////////////////////////////////
NcSample* NcPsiDistrib::GetPsiSample()
{
// Get the sample containing information on the psi distribution.
 return fSample;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcPsiDistrib::GetFracBelow()
{
// Get fraction of trials in which a psi smaller than the reference psi is observed.
 return ((Double_t)fNbelow)/((Double_t)fNtrials);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcPsiDistrib::GetFracAbove()
{
// Get fraction of trials in which a psi larger than the reference psi is observed.
 return ((Double_t)fNabove)/((Double_t)fNtrials);
}
///////////////////////////////////////////////////////////////////////////
Float_t NcPsiDistrib::FindMaxPsi()
{
// Find the maximum possible psi value with the given signal, probabilities and number of events.

 // Make data array and initialise with signal
 Int_t* data=new Int_t[fNoutcomes];
 for(Int_t i=0; i<fNoutcomes; i++)
 {
  data[i]=fSignal[i];
 }

 // Try putting background events in each bin consecutively
 Float_t maxpsi=0, psi=0;
 NcMath m;
 for(Int_t i=0; i<fNoutcomes; i++)
 {
  data[i]+=fNevents;
  psi=m.PsiValue(fNoutcomes, data, fProbs);
  if(psi>maxpsi) maxpsi=psi;
  data[i]-=fNevents;
 }

 // Delete data array
 delete[] data;

 return maxpsi;
}
///////////////////////////////////////////////////////////////////////////
