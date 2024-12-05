/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2021, NCFS/IIHE, All Rights Reserved.                     *
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
// Class NcBlocks
// Class for (Bayesian) Block treatment of sequential data.
//
// This class provides a tool set to detect and characterize local variability 
// in sequential data.
//
// A very common case are observations in the form of time series, and as such
// this will be used for illustration in the documentation. However, the various
// procedures provided in this class apply to any form of sequential data.
//
// Time series can be divided in three categories, called Data Modes :
// 
// 1) Recorded times of events, so called Time-Tagged Event (TTE) data.
// 2) Counts of events in time bins.
// 3) Measurements of a (quasi) continuous observable at a sequence of points in time.
//
// A Bayesian Block analysis provides a procedure to automatically detect local
// variability in the data stream by a dynamic partitioning of the dataset, resulting
// in data blocks of different length over which the event rate, event count or
// measured value can be regarded as constant.
//
// For each Data Mode a dedicated Bayesian Block analysis procedure is provided via
// the various GetBlocks() member functions.
//
// On the other hand, this class also provides GetBlocks() member functions which allow
// the user to form blocks defined by a fixed number of "n" samplings c.q. bins.
// In this case the height of each block is defined by the mean (or median) value of the
// recorded values within each set of "n" samplings.
// These member functions provide an efficient way to identify (pedestal) variations in case
// these might be expected to occur after recording "n" samples due to the specific DAQ design.
// In principle a Bayesian Block analysis would also be able to identify these variations
// over the sets of "n" samplings, but the analysis over a fixed number of "n" samplings
// is much faster than the dynamic Bayesian Block analysis. 
//
// In a Bayesian Block analysis, local variability in the sequential data stream is indicated
// by so called Change Points, at which a step is introduced in e.g. the event rate, event count
// or measured value.
//
// Since the data treatment is inherently sequential, it is also possible to
// trigger c.q. stop after the occurrence of a certain number of Change Points.
// For instance, an online trigger system may be obtained by specifying to stop at the
// first occurrence of a Change Point.
// Please refer to the documentation of the GetBlocks() member functions for further details. 
//
// Details of the various Bayesian Block algorithms can be found in the publication :
//
// J.D. Scargle et al., The Astrophysical Journal 764 (2013) 167. (ArXiv:1207.5578).
//
//
// Some coding examples:
// =====================
//
//
// Example 1: Unbinned data (Data Mode 1)
// --------------------------------------
//
// gSystem->Load("ncfspack");
//
// NcBlocks q;
//
// NcSample s("Event times");
// s.SetStoreMode();
// s.SetNames("Time");
//
// // Construct and store artificial event times
// Int_t nevt=100;
// Float_t t=0;
// Int_t low=nevt/4;
// Int_t mid=nevt/2;
// Int_t up=3*nevt/4;
// Float_t add=0;
// for (Int_t i=1; i<=nevt; i++)
// {
//  if (i<low)
//  {
//   add=1;
//  }
//  else if (i<mid)
//  {
//   add=0.1;
//  }
//  else if (i<up)
//  {
//   add=1;
//  }
//  else
//  {
//   add=0.2;
//  }
//  t+=add;
//  s.Enter(t);
// }
//
// Float_t fpr=0.1;
// TH1F hblock;
// hblock.SetName("hblock");
// q.GetBlocks(s,1,fpr,&hblock);
//
// TCanvas c("c","c");
// c.Divide(1,2);
// c.cd(1);
// TH1D hs=s.Get1DHistogram(1,0,kFALSE,1000);
// hs.Draw();
// c.cd(2);
// hblock.Draw();
//
//
// Example 2: Binned data (Data Mode 2)
// ------------------------------------
//
// gSystem->Load("ncfspack");
//
// NcBlocks q;
//
// TH1F h("h","Example 2: Binned data (Data Mode 2);Time in sec.;Weighted counts",100,0,100);
//
// h.Fill(0,1.5);
// h.Fill(1,1.0);
// h.Fill(2,1.3);
// h.Fill(3,0.9);
// h.Fill(5,1.1);
// h.Fill(6,1.2);
// h.Fill(7,1.1);
// h.Fill(8,1.3);
// h.Fill(9,2);
// h.Fill(10,2.5);
// h.Fill(11,3.3);
// h.Fill(12,5);
// h.Fill(13,6);
// h.Fill(14,6);
// h.Fill(30,8.1);
// h.Fill(31,8.3);
// h.Fill(32,8.5);
// h.Fill(33,8.2);
// h.Fill(40,2);
// h.Fill(41,2);
// h.Fill(42,2);
// h.Fill(43,2);
// h.Fill(60,1);
// h.Fill(61,1);
// h.Fill(62,1);
// h.Fill(63,1);
// h.Fill(80,-3);
// h.Fill(81,-5.5);
// h.Fill(82,-8);
// h.Fill(83,-3.2);
// h.Fill(90,1);
// h.Fill(91,1);
// h.Fill(92,1);
// h.Fill(93,1);
//
// Float_t fpr=0.1;
// TH1F hblock;
// hblock.SetName("hblock");
// q.GetBlocks(&h,fpr,&hblock);
//
// // Subtract the Block values from the original histogram bin contents
// // without scaling w.r.t. the bin size
// TH1F hdif;
// q.Add(&h,&hblock,&hdif,0,-1);
//
// TCanvas c("c","c");
// c.Divide(1,3);
// c.cd(1);
// h.SetLineWidth(2);
// h.Draw();
// hblock.Draw("same");
// c.cd(2);
// hblock.Draw();
// c.cd(3);
// hdif.SetLineWidth(2);
// hdif.Draw();
//
//
// Example 3: Measurement data (Data Mode 3)
// -----------------------------------------
//
// gSystem->Load("ncfspack");
//
// NcBlocks q;
//
// NcRandom ran;
//
// TGraphErrors gr;
// gr.SetName("Cosine");
// gr.SetTitle("Example 3 : Measurement data (Data Mode 3);x in radians;cos(x)");
//
// // Create and store some artificial measurements
// Double_t pi=acos(-1.);
// Float_t x=0;
// Float_t y=0;
// Float_t ex=0;
// Float_t ey=0;
// Int_t nloop=100;
// Int_t jpoint=0;
// Double_t step=2.*pi/float(nloop);
// Int_t itest=0;
// for (Int_t i=0; i<=nloop; i++)
// {
//  y=cos(x);
//  ex=0.1;
//  ey=fabs(0.1*y);
//  y+=ey*ran.Uniform(-1.,1.);
//  itest=i%5;
//  if (!itest)
//  {
//   itest=i;
//   gr.SetPoint(jpoint,x,y);
//   gr.SetPointError(jpoint,ex,ey);
//   jpoint++;
//  }
//  x+=step;
// }
//
// Float_t fpr=0.1;
// TH1F hblock;
// hblock.SetName("hblock");
// q.GetBlocks(gr,fpr,&hblock);
//
// // Subtract the Block values from the original graph
// TGraphErrors gdif;
// q.Add(&gr,&hblock,&gdif,-1);
//
// TCanvas c("c","c");
// c.Divide(1,3);
// c.cd(1);
// gr.Draw("A*");
// hblock.Draw("same");
// c.cd(2);
// hblock.Draw();
// c.cd(3);
// gdif.Draw("A*");
//
//
// Example 4: Triggering
// ---------------------
//
// gSystem->Load("ncfspack");
//
// NcBlocks q;
//
// TH1F h("h","Example 4: Triggering;Time in sec.;Weighted counts",100,0,100);
//
// h.Fill(0,1.5);
// h.Fill(1,1);
// h.Fill(2,1.3);
// h.Fill(3,0.9);
// h.Fill(5,1.1);
// h.Fill(6,1.2);
// h.Fill(7,1.1);
// h.Fill(8,1.3);
// h.Fill(9,2);
// h.Fill(10,2.5);
// h.Fill(11,3.3);
// h.Fill(12,5);
// h.Fill(13,6);
// h.Fill(14,6);
// h.Fill(30,8.1);
// h.Fill(31,8.3);
// h.Fill(32,8.5);
// h.Fill(33,8.2);
// h.Fill(40,2);
// h.Fill(41,2);
// h.Fill(42,2);
// h.Fill(43,2);
// h.Fill(60,1);
// h.Fill(61,1);
// h.Fill(62,1);
// h.Fill(63,1);
// h.Fill(80,-3);
// h.Fill(81,-5.5);
// h.Fill(82,-8);
// h.Fill(83,-3.2);
// h.Fill(90,1);
// h.Fill(91,1);
// h.Fill(92,1);
// h.Fill(93,1);
// h.Fill(97,5);
// h.Fill(98,7);
// h.Fill(99,10);
//
// // Trigger on the first rising edge by regarding
// // the histogram data as a realtime incoming data stream
// Float_t fpr=0.1;
// Int_t ntrig=1;
// TH1F hblock;
// hblock.SetName("hblock");
// Double_t xtrig=q.GetBlocks(&h,fpr,&hblock,ntrig);
//
// cout << " Data stream triggered at : " << xtrig << endl;
//
// TCanvas c("c","c");
// h.SetLineWidth(2);
// h.Draw();
// hblock.Draw("same");
//
//
// Example 5: Real time animation
// ------------------------------
//
// gSystem->Load("ncfspack");
//
// NcBlocks q;
//
// Float_t fpr=0.1;
// Int_t nevt=100;
// Float_t arr[nevt];
//
// Float_t t=0;
// Int_t low=nevt/4;
// Int_t mid=nevt/2;
// Int_t up=3*nevt/4;
// Float_t add=0;
// TH1F hblock;
// hblock.SetName("hblock");
// TCanvas c("c","c");
// // Construct and store artificial event times
// for (Int_t i=1; i<=nevt; i++)
// {
//  if (i<low)
//  {
//   add=1;
//  }
//  else if (i<mid)
//  {
//   add=0.1;
//  }
//  else if (i<up)
//  {
//   add=1;
//  }
//  else
//  {
//   add=0.2;
//  }
//  t+=add;
//  arr[i-1]=t;
//
//  // Display the Block histogram step by step
//  if (i>1)
//  {
//   q.GetBlocks(i,arr,fpr,&hblock);
//   hblock.Draw();
//   c.Update();
//   gSystem->Sleep(25);
//  }
// }
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, September 7, 2021  08:06Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, December 5, 2024  14:54Z
///////////////////////////////////////////////////////////////////////////

#include "NcBlocks.h"
#include "Riostream.h"
 
ClassImp(NcBlocks) // Class implementation to enable ROOT I/O
 
NcBlocks::NcBlocks()
{
// Default constructor.

 fMode=0;
}
///////////////////////////////////////////////////////////////////////////
NcBlocks::~NcBlocks()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
NcBlocks::NcBlocks(const NcBlocks& q)
{
// Copy constructor.

 fMode=0;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetPrior(Int_t n,Double_t fpr)
{
// Internal member function to provide the prior fitness value for each individual block.
// The returned value corresponds to -(ncp_prior) of the article of J.D. Scargle et al.
//
// Input arguments :
// -----------------
// n    : The number of Data Cells
// fpr  : The requested false positive rate
//
// Note :
// ------
// For Data Mode 1 and 2, the result is rather accurate on the full fpr interval [0,1].
// For Data Mode 3 the fit for fpr=0.05 was given in the article of J.D. Scargle et al.
// in the form : prior=c+s*log10(n) with c=1.32 and s=0.577.
// For the fpr values of 0.01, 0.1, 0.2, 0.3, 0.4 and 0.5 the data were read off from
// Fig. 6 in the article of J.D. Scargle et al. and fits of prior vs. N were made to provide
// the parameters c and s for each fpr value.
// These data sets of c and s were then used to provide fits of c vs. fpr and s vs. fpr,
// which allows to determine the parameter values of c and s for a certain fpr value,
// and subsequently the value of the corresponding prior.
//
// In case of inconsistent input, the value 0 is returned.

 Double_t prior=0;

 if (fMode<1 || fMode>3 || n<1 || fpr<0 || fpr>1)
 {
  cout << " *NcBlocks::GetPrior* Inconsistent input : mode=" << fMode << " n=" << n << " fpr=" << fpr << endl;
  return 0;
 }

 Double_t rn=n;

 if (fMode==3)
 {
  Double_t c=0;
  Double_t s=0;
  if (fpr>0.045 && fpr<0.055) // Fit of J.D. Scargle for fpr=0.05
  {
   c=1.32;
   s=0.577;
  }
  else
  {
   c=51.29*TMath::Landau(fpr,-0.152,0.03167);
   s=0.5807+0.2317*fpr;
  }
  prior=c+s*log10(rn);
 }
 else // Data Mode 1 and 2
 {
  prior=4.-log(73.53*fpr*pow(rn,-0.478));
 }

 prior=-prior;

 return prior;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlockFitness(Double_t n,Double_t len)
{
// Internal member function to provide the fitness value for a certain block content
// in the case of Data Mode 1 or 2.
//
// Input arguments :
// -----------------
// n     : The (weighted) number of events in the block
// len   : The length of the block
//
// The length of the block represents the summed time span of the contained Data Cells.
//
// In case of inconsistent input, the value 0 is returned.

 Double_t fb=0;

 if ((fMode!=1 && fMode!=2) || (fMode==1 && n<1) || len<=0)
 {
  cout << " *NcBlocks::GetBlockFitness* Inconsistent input : mode=" << fMode << " n=" << n << " len=" << len << endl;
  return 0;
 }

 if (n<=0) return 0;

 fb=n*log(n/len);

 return fb;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(TH1* hin,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for the binned data (Data Mode 2) of histogram "hin"
// with a false positive rate "fpr", and provide the results in the 1-D histogram "hout".
//
// Note : Both "hin" and "hout" must be existing 1-dimensional histograms.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the bin contents of the input histogram "hin".
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 if (!hin || !hout) return 0;

 Int_t n=hin->GetNbinsX();

 if (!n) return 0;

 // Set the Data Mode for binned data, if it was not set already
 // Also initialize the output histogram title for binned data
 if (!fMode)
 {
  fMode=2;
  TString title;
  title.Form("Bayesian Block representation with FPR=%-.3g",fpr);
  hout->SetTitle(title);
 }

 TArrayD best(n);    // Array with optimal fitness values
 TArrayI last(n);    // Array with indices of optimal last change points
 TArrayD lengths(n); // Array with the lengths of the optimal partition blocks
 TArrayD counts(n);  // Array with the event counts of the optimal partition blocks

 Double_t blen=0;   // Length of a certain block
 Double_t bcount=0; // Event count in a certain block
 Double_t prior=0; // Contribution of each block to the prior fitness for a certain partition
 Double_t bfit=0;  // Fitness of a certain block
 Double_t pfit=0;  // Fitness of a certain partition

 Double_t xlow=0;
 Double_t xup=0;
 Int_t index=0;

 // Variables for measurement of an observable (i.e. Data Mode 3)
 Double_t yk=0;   // Measured value
 Double_t sigk=0; // Error on the measured value
 Double_t a=0; 
 Double_t b=0;

 // The parameters for the optimal partition
 Double_t optfit=0;
 Int_t optj=0;
 Double_t optlen=0;
 Double_t optcount=0;

 // Parameters and counter for trigger mode
 Int_t oldoptj=0;
 Double_t oldoptlen=0;
 Int_t ncp=0;
 Double_t xtrig=0;
 Double_t ytrig=0;
 Double_t oldytrig=0;

 // Add Data Cells one by one to the sample to be partioned
 Int_t ncells=0;
 Int_t first=1;
 for (Int_t i=1; i<=n; i++)
 {
  ncells=i;
  prior=GetPrior(i,fpr);
  xup=hin->GetBinLowEdge(i)+hin->GetBinWidth(i);
  // Loop over all possible block partitions for this Data Cell sample
  first=1;
  for (Int_t j=1; j<=i; j++)
  {
   xlow=hin->GetBinLowEdge(j);
   blen=xup-xlow;
   bcount=0;
   bfit=0;
   a=0;
   b=0;
   if (fMode==1 || fMode==2)
   {
    bcount=hin->Integral(j,i);
    bfit=GetBlockFitness(fabs(bcount),blen);
   }
   if (fMode==3)
   {
    for (Int_t k=j; k<=i; k++)
    {
     yk=hin->GetBinContent(k);
     sigk=hin->GetBinError(k);
     sigk=sigk*sigk;
     if (sigk)
     {
      a+=1./(sigk);
      b+=yk/sigk;
     }
    }
    if (a)
    {
     bcount=b/a; // Weighted mean of the y-values in the block
     bfit=b*b/(2.*a); 
    }
   }
   pfit=prior+bfit;
   index=j-1; // Array index reduced by 1 because of C++ array index convention
   if (index>0) pfit+=best.At(index-1);

   // Record attributes for the optimal partition 
   if (first)
   {
    optfit=pfit;
    optj=j;
    optlen=blen;
    optcount=bcount;
    first=0;
   }
   else
   {
    if (pfit>optfit)
    {
     optfit=pfit;
     optj=j;
     optlen=blen;
     optcount=bcount;
    }
   }
  } // End of loop over possible block partitions

  // Store the attributes of the optimal partition
  best.SetAt(optfit,i-1);
  last.SetAt(optj,i-1);
  lengths.SetAt(optlen,i-1);
  counts.SetAt(optcount,i-1);

  if (!ntrig || optj==1) continue;

  // Check for triggering on a new change point
  oldoptj=last.At(i-2);
  oldoptlen=lengths.At(i-2);
  oldytrig=0;
  if ((fMode==1 || fMode==2) && oldoptlen) oldytrig=counts.At(i-2)/oldoptlen;
  if (fMode==3) oldytrig=counts.At(i-2);
  ytrig=0;
  if ((fMode==1 || fMode==2) && optlen) ytrig=counts.At(i-1)/optlen;
  if (fMode==3) ytrig=counts.At(i-1);

  if (optj>oldoptj && ((ntrig>0 && ytrig>oldytrig) || (ntrig<0 && ytrig<oldytrig)))
  {
   ncp++;
   xtrig=hin->GetBinLowEdge(optj);
  }

  // Stop when the requested number of triggers has been reached
  if (ncp>=abs(ntrig)) break;

 } // End of adding Data Cells one by one

 // Obtain the change points and corresponding partition information
 Int_t jcp=0;
 index=ncells;
 Double_t x=0;
 Double_t y=0;
 ncp=0;
 TArrayD xarr(ncells+1);
 TArrayD yarr(ncells+1);
 while (index>0)
 {
  index--; // Reduce array index by 1 because of C++ array convention
  jcp=last.At(index);
  ncp++;
  x=hin->GetBinLowEdge(jcp)+lengths.At(index);
  y=0;
  if (fMode==1 || fMode==2)
  {
   if (lengths.At(index)) y=counts.At(index)/lengths.At(index);
  }
  if (fMode==3) y=counts.At(index);

  xarr.SetAt(x,ncp-1);
  yarr.SetAt(y,ncp-1);

  // Also  mark the start of the first bin
  if (jcp==1) xarr.SetAt(hin->GetBinLowEdge(jcp),ncp);

  index=jcp-1;  
 }

 // Create the corresponding variable binned histogram
 Double_t* xbins=new Double_t[ncp+1];
 Double_t* yvals=new Double_t[ncp+1];

 for (Int_t i=0; i<=ncp; i++)
 {
  xbins[i]=xarr.At(ncp-i);
  yvals[i]=yarr.At(ncp-i);
 }

 hout->SetBins(ncp,xbins);
 for (Int_t i=1; i<=ncp; i++)
 {
  hout->SetBinContent(i,yvals[i]);
 }

 // Determine the first trigger point after the full sample analysis
 if (!ntrig) xtrig=xbins[1];

 hout->SetLineWidth(2);
 hout->SetLineColor(kBlue);
 hout->SetStats(kFALSE);

 // Set the output histogram and axes titles
 TString title,str;
 title="Bayesian Block representation for histogram ";
 title+=hin->GetName();
 title+=" with FPR= %-.3g";
 title+=";";
 str=hin->GetXaxis()->GetTitle();
 if (str=="") str="Recordings (e.g. time)";
 title+=str;
 title+=";";
 str=hin->GetYaxis()->GetTitle();
 if (str=="") str="Counts";
 title+=str;
 str=title.Format(title.Data(),fpr);
 hout->SetTitle(str.Data());

 // Indicate the requested trigger in a legend
 if (ntrig)
 {
  str.Form("Requested trigger at : %-.3g",xtrig);

  TLegend* leg=new TLegend(0.5,0.85,0.7,0.9,str);
  leg->SetFillColor(0);
  leg->SetTextColor(kBlue);
  leg->SetTextAlign(22);

  TList* hlist=hout->GetListOfFunctions();
  hlist->Add(leg);
 }

 // Reset the Data Mode for subsequent invokations
 fMode=0;

 delete [] xbins;
 delete [] yvals;

 return xtrig;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(NcSample s,Int_t i,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for the (Data Mode 1) i-th variable of NcSample "s"
// with a false positive rate "fpr", and provide the results in 1-D histogram "hout".
// A common case is where the NcSample contains recorded event times.
//
// Notes :
// -------
// 1) The Store Mode of the NcSample must be activated.
// 2) "hout" must be an existing 1-dimensional histogram.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the event rate based on the recordings in the provided NcSample.
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 Int_t n=s.GetN();
 Int_t store=s.GetStoreMode();
 Int_t dim=s.GetDimension();

 if (n<2 || !store || dim<1 || i<1 || i>dim || fpr<0 || fpr>1)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input for NcSample treatment." << endl;
  cout << " Store Mode:" << store << " Entries:" << n << " Dimension:" << dim << " i:" << i << " fpr:" << fpr << endl; 
  return 0;
 }

 // Set Data Mode for unbinned event data
 fMode=1;

 // Represent each observation as 1 count in a variable binned histogram
 Double_t* xbins=new Double_t[n];
 Double_t val=0;
 for (Int_t idx=1; idx<=n; idx++)
 {
  val=s.GetEntry(idx,i,1,i);
  xbins[idx-1]=val;
 }

 TH1F hin("","",n-1,xbins);
 for (Int_t j=1; j<n; j++)
 {
  hin.SetBinContent(j,1);
 }

 Double_t xtrig=GetBlocks(&hin,fpr,hout,ntrig);

 // Set the output histogram and axes titles
 TString title,str;
 title="Bayesian Block representation for NcSample ";
 title+=s.GetName();
 title+=" with FPR=%-.3g";
 title+=";Recordings of variable ";
 title+=i;
 title+=" (";
 title+=s.GetVariableName(i);
 title+=")";
 title+=";Count rate";
 str=title.Format(title.Data(),fpr);
 hout->SetTitle(str.Data());

 delete [] xbins;

 return xtrig;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(NcSample s,TString name,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for the (Data Mode 1) named variable of NcSample "s"
// with a false positive rate "fpr", and provide the results in 1-D histogram "hout".
// A common case is where the NcSample contains recorded event times.
//
// Notes :
// -------
// 1) The Store Mode of the NcSample must be activated.
// 2) "hout" must be an existing 1-dimensional histogram.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the event rate based on the recordings in the provided NcSample.
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 Int_t i=s.GetIndex(name);

 Double_t xtrig=GetBlocks(s,i,fpr,hout,ntrig);

 return xtrig;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(Int_t n,Double_t* arr,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for the "n" data recordings (Data Mode 1)
// contained in the data array "arr" with a false positive rate "fpr",
// and provide the results in the 1-D histogram "hout".
// A common case is where the array "arr" contains recorded event times.
//
// Notes :
// -------
// 1) The data in array "arr" are interpreted as Data Mode 1.
//    For Data Mode 2 or Data Mode 3 treatment, please use the corresponding GetBlocks() function.
// 2) The data elements in the array "arr" do not need to be ordered.
// 3) "hout" must be an existing 1-dimensional histogram.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the event rate based on the recordings in the provided array.
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 if (!arr || n<2 || fpr<0 || fpr>1)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input for array treatment." << endl;
  if (!arr)
  {
   cout << " Array pointer is 0." << endl;
  }
  else
  {
   cout << " Entries:" << n <<  " fpr:" << fpr << endl;
  }
  return 0;
 }

 NcSample s;
 s.SetStoreMode();

 for (Int_t i=0; i<n; i++)
 {
  s.Enter(arr[i]);
 }

 Double_t xtrig=GetBlocks(s,1,fpr,hout,ntrig);

 // Set the output histogram and axes titles
 TString title;
 title.Form("Bayesian Block representation for unbinned array data with FPR=%-.3g",fpr);
 title+=";Recordings (e.g. time);Count rate";
 hout->SetTitle(title);

 return xtrig;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(Int_t n,Float_t* arr,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for the "n" data recordings (Data Mode 1)
// contained in the data array "arr" with a false positive rate "fpr",
// and provide the results in the 1-D histogram "hout".
// A common case is where the array "arr" contains recorded event times.
//
// Notes :
// -------
// 1) The data in array "arr" are interpreted as Data Mode 1.
//    For Data Mode 2 or Data Mode 3 treatment, please use the corresponding GetBlocks() function.
// 2) The data elements in the array "arr" do not need to be ordered.
// 3) "hout" must be an existing 1-dimensional histogram.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the event rate based on the recordings in the provided array.
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 if (!arr || n<2 || fpr<0 || fpr>1)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input for array treatment." << endl;
  if (!arr)
  {
   cout << " Array pointer is 0." << endl;
  }
  else
  {
   cout << " Entries:" << n <<  " fpr:" << fpr << endl;
  }
  return 0;
 }

 NcSample s;
 s.SetStoreMode();

 for (Int_t i=0; i<n; i++)
 {
  s.Enter(arr[i]);
 }

 Double_t xtrig=GetBlocks(s,1,fpr,hout,ntrig);

 // Set the output histogram and axes titles
 TString title;
 title.Form("Bayesian Block representation for unbinned array data with FPR=%-.3g",fpr);
 title+=";Recordings (e.g. time);Count rate";
 hout->SetTitle(title);

 return xtrig;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(TGraphErrors gr,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for measurements of an observable (Data Mode 3)
// with a false positive rate "fpr", and provide the results in 1-D histogram "hout".
//
// Notes :
// -------
// 1) It is essential that the errors on the y-values are provided, since they are
//    used as weights in the statistical analysis.
//    The errors on the x-values may be omitted, since they are not used in the process.
// 2) "hout" must be an existing 1-dimenstional histogram.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the measured value based on the recordings in the provided TGraphErrors object.
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 Int_t n=gr.GetN();

 if (n<2 || fpr<0 || fpr>1)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input for TGraphErrors treatment." << endl;
  cout << " Entries:" << n << " fpr:" << fpr << endl; 
  return 0;
 }

 // Set Data Mode for measurements of a continuous observable
 fMode=3;

 // Sort the data points with increasing x-value
 gr.Sort();

 // Represent each observation as a value in a variable binned histogram
 Double_t* xbins=new Double_t[n+1];
 Double_t x=0;
 Double_t y=0;
 Double_t err=0;
 Double_t dist=0;
 Double_t dmin=-1;
 for (Int_t i=0; i<n; i++)
 {
  gr.GetPoint(i,x,y);
  err=fabs(gr.GetErrorX(i));
  xbins[i]=x-err;
  if (i>0)
  {
   dist=xbins[i]-xbins[i-1];
   if (dmin<0 || dist<dmin) dmin=dist;
  }
 }
 // Add an extra bin to contain the last measurement
 xbins[n]=xbins[n-1]+dmin;

 TH1F hin("","",n,xbins);
 for (Int_t j=1; j<=n; j++)
 {
  gr.GetPoint(j-1,x,y);
  err=fabs(gr.GetErrorY(j-1));
  hin.SetBinContent(j,y);
  hin.SetBinError(j,err);
 }

 Double_t xtrig=GetBlocks(&hin,fpr,hout,ntrig);

 // Set the output histogram and axes titles
 TString title,str;
 TString xtitle="Samplings (e.g. time)";
 TString ytitle="Measured value";
 TAxis* ax=gr.GetXaxis();
 if (ax)
 {
  str=ax->GetTitle();
  if (str != "") xtitle=str;
 }
 ax=gr.GetYaxis();
 if (ax)
 {
  str=ax->GetTitle();
  if (str != "") ytitle=str;
 }
 title="Bayesian Block representation for TGraphErrors ";
 title+=gr.GetName();
 title+=" with FPR=%-.3g;";
 title+=xtitle;
 title+=";";
 title+=ytitle;
 str=title.Format(title,fpr);
 hout->SetTitle(str);

 delete [] xbins;

 return xtrig;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(TGraph gr,TF1 f,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for measurements of an observable (Data Mode 3)
// with a false positive rate "fpr", and provide the results in 1-D histogram "hout".
//
// Notes :
// -------
// 1) "hout" must be an existing 1-dimensional histogram.
// 2) The other member function with TGraphErrors input provides more flexibilty.
//
// The error of each y-value is determined by the absolute value of f(y).
// This provides an easy way to perform quickly a Bayesian Block analysis directly
// on a TGraph object instead of having to convert it first into a TGraphErrors object.
// However, the error setting facility provided here is rather limited, so it
// is intended to serve only for simple situations.
// For more flexibility and accuracy, please refer to the other member function that
// takes a TGraphErrors object as input, or refer to the class NcSample which
// contains a facility to extend TGraph objects into TGraphError objects. 
//
// Note :
// ------
// It is essential that the errors on the y-values are provided, since they are
// used as weights in the statistical analysis.
// The errors on the x-values are omitted, since they are not used in the process.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the measured value based on the recordings in the provided TGraphErrors object.
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 NcSample s;
 TGraphErrors gre=s.GetGraphErrors(&gr,0,0,0,&f);

 Double_t xtrig=GetBlocks(gre,fpr,hout,ntrig);

 TString str=" and input errors : ";
 str+=f.GetExpFormula("p");
 str.ReplaceAll("x","y");

 TString title=hout->GetTitle();
 title+=str;
 hout->SetTitle(title);

 return xtrig; 
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(TGraph gr,TString f,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for measurements of an observable (Data Mode 3)
// with a false positive rate "fpr", and provide the results in 1-D histogram "hout".
//
// Notes :
// -------
// 1) "hout" must be an existing 1-dimensional histogram.
// 2) The other member function with TGraphErrors input provides more flexibilty.
//
// The error of each y-value is determined by the absolute value of f(y), where the
// function is described by the input argument "f" following the TF1 string format convention.
// This provides an easy way to perform quickly a Bayesian Block analysis directly
// on a TGraph object instead of having to convert it first into a TGraphErrors object.
// However, the error setting facility provided here is rather limited, so it
// is intended to serve only for simple situations.
// For more flexibility and accuracy, please refer to the other member function that
// takes a TGraphErrors object as input, or refer to the class NcSample which
// contains a facility to extend TGraph objects into TGraphError objects. 
//
// Note :
// ------
// It is essential that the errors on the y-values are provided, since they are
// used as weights in the statistical analysis.
// The errors on the x-values are omitted, since they are not used in the process.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the measured value based on the recordings in the provided TGraphErrors object.
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 TF1 func("func",f);
 NcSample s;
 TGraphErrors gre=s.GetGraphErrors(&gr,0,0,0,&func);

 Double_t xtrig=GetBlocks(gre,fpr,hout,ntrig);

 TString str=" and input errors : ";
 str+=func.GetExpFormula("p");
 str.ReplaceAll("x","y");

 TString title=hout->GetTitle();
 title+=str;
 hout->SetTitle(title);

 return xtrig; 
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(TGraph gr,Double_t nrms,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for measurements of an observable (Data Mode 3)
// with a false positive rate "fpr", and provide the results in 1-D histogram "hout".
//
// Notes :
// -------
// 1) "hout" must be an existing 1-dimensional histogram.
// 2) The other member function with TGraphErrors input provides more flexibilty.
//
// The error of each y-value is determined by "nrms" times the RMS deviation of all the y-values,
// which often provides an efficient way to ignore noise variations of a background pedestal.
// This provides an easy way to perform quickly a Bayesian Block analysis directly
// on a TGraph object instead of having to convert it first into a TGraphErrors object.
// However, the error setting facility provided here is rather limited, so it
// is intended to serve only for simple situations.
// For more flexibility and accuracy, please refer to the other member function that
// takes a TGraphErrors object as input, or refer to the class NcSample which
// contains a facility to extend TGraph objects into TGraphError objects. 
//
// Note :
// ------
// It is essential that the errors on the y-values are provided, since they are
// used as weights in the statistical analysis.
// The errors on the x-values are omitted, since they are not used in the process.
//
// Each new block is started at a so called Change Point, to indicate a significant change
// in the measured value based on the recordings in the provided TGraphErrors object.
// The various Change Points represent the start of the various (variable) sized bins
// of the resulting histogram "hout".
// Since the data treatment is inherently sequential, the user may use the argument "ntrig"
// to trigger c.q. stop after the occurrence of ntrig Change Points.
// For instance, an online trigger system may be obtained by specifying ntrig=1.
//
// Meaning of the input argument "ntrig" :
// ---------------------------------------
// ntrig >0 : Only consider Change Points indicating a rising edge
//       <0 : Only consider Change Points indicating a falling edge
//       =0 : No triggering c.q. early stopping of the data processing will be performed
//
// The default value is ntrig=0.
//
// The returned value is the "X-value" of the selected Change Point, e.g. trigger time.
// In case ntrig=0 the return value corresponds to the 1st Change Point, irrespective
// whether that represents a rising or falling edge.
//
// In case of inconsistent input, the value 0 is returned.

 // Obtain the RMS deviation of all the y-values
 Double_t rms=gr.GetRMS(2);

 // Determine the error for each y-value and convert into a function format
 Double_t err=fabs(nrms*rms);

 TString f;
 f.Form("%-.5g",err);

 Double_t xtrig=GetBlocks(gr,f,fpr,hout,ntrig);

 TString str;
 str.Form(" from nrms=%-.3g",fabs(nrms));

 TString title=hout->GetTitle();
 title+=str;
 hout->SetTitle(title);

 return xtrig; 
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::GetBlocks(TH1* hin,TH1* hout,Int_t n,Int_t mode)
{
// Get the block partitions consisting of "n" consecutive bins for binned data
// contained in histogram "hin" with as block height the mean, median or RMS value
// of the contained "n" bins and provide the results in the 1-D histogram "hout".
//
// This facility provides an efficient way to identify pedestal variations in case these
// might be expected to occur after recording "n" samples due to the specific DAQ design.
//
// mode = 0 --> Use the mean as average value
//        1 --> Use the median as average value
//        2 --> Use the Root Mean Square (RMS) as average value
//              Note : This is NOT the RMS deviation defined as sqrt(variance)
//
// The default value is mode=0;
//
// Notes :
// -------
// 1) "hout" must be an existing 1-dimensional histogram.
// 2) The blocks represent the bins of "hout", which may have variable length
//    in case the sampling was not continuous.
//    As such, the structure of the output histogram is comparable to the result
//    of a Bayesian Block analysis.  
//
// The returned value is the number of produced blocks. 
// In case of inconsistent input, the value 0 is returned.

 if (!hin)
 {
  cout << " *NcBlocks::GetBlocks* Error : Input histogram not specified." << endl;
  return 0;
 }

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 Int_t nbins=hin->GetNbinsX();

 if (!nbins || n<1 || n>nbins || mode<0 || mode>2)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input nbins=" << nbins << " n=" << n << " mode=" << mode << endl;
  return 0;
 }

 // Retrieve the various sets of "n" bins from the input histogram
 Int_t jbin=0;
 Double_t x=0;
 Double_t y=0;
 Double_t xlow=0;
 Double_t xup=0;
 Double_t binwidth=0;
 NcSample s;
 if (mode==1) s.SetStoreMode(1);
 Int_t nblocks=0;
 Double_t average=0;
 TArrayD xarr(nbins);
 TArrayD yarr(nbins);
 while (jbin<nbins)
 {
  for (Int_t i=0; i<n; i++)
  {
   jbin++;

   if (jbin>nbins) break;

   x=hin->GetBinCenter(jbin);
   y=hin->GetBinContent(jbin);
   binwidth=hin->GetBinWidth(jbin);
   if (i==0) xlow=hin->GetBinLowEdge(jbin);
   xup=x+0.5*binwidth;
   s.Enter(x,y);
  }
  if (mode==0) average=s.GetMean(2);
  if (mode==1) average=s.GetMedian(2);
  if (mode==2) average=s.GetRMS(2);
  nblocks++;
  xarr.SetAt(xlow,nblocks-1);
  yarr.SetAt(average,nblocks-1);
  s.Reset();
 } // End of while loop

 // Create the corresponding variable binned output histogram
 Double_t* xbins=new Double_t[nblocks+1];
 Double_t* yvals=new Double_t[nblocks+1];

 for (Int_t i=0; i<nblocks; i++)
 {
  xbins[i]=xarr.At(i);
  yvals[i]=yarr.At(i);
 }
 // Add an extra bin to contain the last data
 xbins[nblocks]=(1.+1e-6)*xup;
 yvals[nblocks]=yvals[nblocks-1];

 hout->SetBins(nblocks,xbins);
 for (Int_t i=1; i<=nblocks; i++)
 {
  hout->SetBinContent(i,yvals[i-1]);
 }

 hout->SetLineWidth(2);
 hout->SetLineColor(kBlue);
 hout->SetStats(kFALSE);

 // Set the output histogram and axes titles
 TString title,str;
 title="Block representation for histogram ";
 title+=hin->GetName();
 title+=" grouped in %-d consecutive bins";
 title+=";";
 str=hin->GetXaxis()->GetTitle();
 if (str=="") str="Recordings (e.g. time)";
 title+=str;
 if (mode==0) title+=";Mean ";
 if (mode==1) title+=";Median ";
 if (mode==2) title+=";RMS ";
 str=hin->GetYaxis()->GetTitle();
 if (str=="") str="Counts";
 title+=str;
 str=title.Format(title.Data(),n);
 hout->SetTitle(str.Data());

 delete [] xbins;
 delete [] yvals;

 return nblocks;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::GetBlocks(NcSample s,Int_t i,TH1* hout,Int_t n,Int_t mode)
{
// Get the block partitions consisting of "n" consecutive samplings for the
// i-th variable of NcSample "s", with as block height the mean, median or RMS value
// of the contained "n" samples and provide the results in the 1-D histogram "hout".
//
// This facility provides an efficient way to identify pedestal variations in case these
// might be expected to occur after recording "n" samples due to the specific DAQ design.
//
// mode = 0 --> Use the mean as average value
//        1 --> Use the median as average value
//        2 --> Use the Root Mean Square (RMS) as average value
//              Note : This is NOT the RMS deviation defined as sqrt(variance)
//
// The default value is mode=0;
//
// Notes :
// -------
// 1) The Store Mode of the NcSample must be activated.
// 2) "hout" must be an existing 1-dimensional histogram.
// 3) The blocks represent the bins of "hout", which may have variable length
//    in case the sampling was not continuous.
//    As such, the structure of the output histogram is comparable to the result
//    of a Bayesian Block analysis.  
//
// The returned value is the number of produced blocks. 
// In case of inconsistent input, the value 0 is returned.

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 Int_t nen=s.GetN();
 Int_t store=s.GetStoreMode();
 Int_t dim=s.GetDimension();

 if (!store || dim<1 || i<1 || i>dim || n<1 || n>nen || mode<0 || mode>2)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input for NcSample treatment." << endl;
  cout << " Store Mode:" << store << " Entries:" << nen << " Dimension:" << dim << " i:" << i << " n:" << n << " mode:" << mode << endl; 
  return 0;
 }

 TGraph gr=s.GetGraph(i);

 Int_t nblocks=GetBlocks(&gr,hout,n,mode);

 // Set the output histogram and axes titles
 TString title,str;
 title="Block representation for NcSample ";
 title+=s.GetName();
 title+=" grouped in %-d consecutive samples";
 title+=";Sampling number";
 if (mode==0) title+=";Mean ";
 if (mode==1) title+=";Median ";
 if (mode==2) title+=";RMS ";
 title+="of variable ";
 title+=i;
 title+=" (";
 title+=s.GetVariableName(i);
 title+=")";
 str=title.Format(title.Data(),n);
 hout->SetTitle(str.Data());

 return nblocks;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::GetBlocks(NcSample s,TString name,TH1* hout,Int_t n,Int_t mode)
{
// Get the block partitions consisting of "n" consecutive samplings for the
// named variable of NcSample "s", with as block height the mean, median or RMS value
// of the contained "n" samples and provide the results in the 1-D histogram "hout".
//
// This facility provides an efficient way to identify pedestal variations in case these
// might be expected to occur after recording "n" samples due to the specific DAQ design.
//
// mode = 0 --> Use the mean as average value
//        1 --> Use the median as average value
//        2 --> Use the Root Mean Square (RMS) as average value
//              Note : This is NOT the RMS deviation defined as sqrt(variance)
//
// The default value is mode=0;
//
// Notes :
// -------
// 1) The Store Mode of the NcSample must be activated.
// 2) "hout" must be an existing 1-dimensional histogram.
// 3) The blocks represent the bins of "hout", which may have variable length
//    in case the sampling was not continuous.
//    As such, the structure of the output histogram is comparable to the result
//    of a Bayesian Block analysis.  
//
// The returned value is the number of produced blocks. 
// In case of inconsistent input, the value 0 is returned.

 Int_t i=s.GetIndex(name);

 Int_t nblocks=GetBlocks(s,i,hout,n,mode);

 return nblocks;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::GetBlocks(Int_t nr,Double_t* arr,TH1* hout,Int_t n,Int_t mode)
{
// Get the block partitions consisting of "n" consecutive elements of the "nr" data recordings
// contained in the data array "arr", and provide the results in the 1-D histogram "hout".
//
// This facility provides an efficient way to identify pedestal variations in case these
// might be expected to occur after recording "n" samples due to the specific DAQ design.
//
// mode = 0 --> Use the mean as average value
//        1 --> Use the median as average value
//        2 --> Use the Root Mean Square (RMS) as average value
//              Note : This is NOT the RMS deviation defined as sqrt(variance)
//
// The default value is mode=0;
//
// Notes :
// -------
// 1) The data elements in the array "arr" do not need to be ordered.
// 2) "hout" must be an existing 1-dimensional histogram.
// 3) The blocks represent the bins of "hout", which may have variable length
//    in case the sampling was not continuous.
//    As such, the structure of the output histogram is comparable to the result
//    of a Bayesian Block analysis.  
//
// The returned value is the number of produced blocks. 
// In case of inconsistent input, the value 0 is returned.

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 if (!arr || n<1 || n>nr || mode<0 || mode>2)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input for array treatment." << endl;
  if (!arr)
  {
   cout << " Array pointer is 0." << endl;
  }
  else
  {
   cout << " Entries:" << nr << " n:" << n << " mode:" << mode << endl;
  }
  return 0;
 }

 NcSample s;
 s.SetStoreMode();

 for (Int_t i=0; i<nr; i++)
 {
  s.Enter(arr[i]);
 }

 Int_t nblocks=GetBlocks(s,1,hout,n,mode);

 // Set the output histogram and axes titles
 TString title;
 title.Form("Block representation for array data grouped in %-d consecutive recordings",n);
 title+=";Sampling number";
 if (mode==0) title+=";Mean ";
 if (mode==1) title+=";Median ";
 if (mode==2) title+=";RMS ";
 title+="value";
 hout->SetTitle(title);

 return nblocks;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::GetBlocks(Int_t nr,Float_t* arr,TH1* hout,Int_t n,Int_t mode)
{
// Get the block partitions consisting of "n" consecutive elements of the "nr" data recordings
// contained in the data array "arr", and provide the results in the 1-D histogram "hout".
//
// This facility provides an efficient way to identify pedestal variations in case these
// might be expected to occur after recording "n" samples due to the specific DAQ design.
//
// mode = 0 --> Use the mean as average value
//        1 --> Use the median as average value
//        2 --> Use the Root Mean Square (RMS) as average value
//              Note : This is NOT the RMS deviation defined as sqrt(variance)
//
// The default value is mode=0;
//
// Notes :
// -------
// 1) The data elements in the array "arr" do not need to be ordered.
// 2) "hout" must be an existing 1-dimensional histogram.
// 3) The blocks represent the bins of "hout", which may have variable length
//    in case the sampling was not continuous.
//    As such, the structure of the output histogram is comparable to the result
//    of a Bayesian Block analysis.  
//
// The returned value is the number of produced blocks. 
// In case of inconsistent input, the value 0 is returned.

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 if (!arr || n<1 || n>nr || mode<0 || mode>2)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input for array treatment." << endl;
  if (!arr)
  {
   cout << " Array pointer is 0." << endl;
  }
  else
  {
   cout << " Entries:" << nr << " n:" << n << " mode:" << mode << endl;
  }
  return 0;
 }

 NcSample s;
 s.SetStoreMode();

 for (Int_t i=0; i<nr; i++)
 {
  s.Enter(arr[i]);
 }

 Int_t nblocks=GetBlocks(s,1,hout,n,mode);

 // Set the output histogram and axes titles
 TString title;
 title.Form("Block representation for array data grouped in %-d consecutive recordings",n);
 title+=";Sampling number";
 if (mode==0) title+=";Mean ";
 if (mode==1) title+=";Median ";
 if (mode==2) title+=";RMS ";
 title+="value";
 hout->SetTitle(title);

 return nblocks;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::GetBlocks(TGraph* gr,TH1* hout,Int_t n,Int_t mode)
{
// Get the block partitions consisting of "n" consecutive samples for measurements
// of an observable with as block height the mean, median or RMS of the values
// of the contained "n" samples and provide the results in the 1-D histogram "hout".
//
// This facility provides an efficient way to identify pedestal variations in case these
// might be expected to occur after recording "n" samples due to the specific DAQ design.
//
// mode = 0 --> Use the mean as average value
//        1 --> Use the median as average value
//        2 --> Use the Root Mean Square (RMS) as average value
//              Note : This is NOT the RMS deviation defined as sqrt(variance)
//
// The default value is mode=0;
//
// Notes :
// -------
// 1) "hout" must be an existing 1-dimensional histogram.
// 2) The blocks represent the bins of "hout", which may have variable length
//    in case the sampling was not continuous.
//    As such, the structure of the output histogram is comparable to the result
//    of a Bayesian Block analysis.  
//
// The returned value is the number of produced blocks. 
// In case of inconsistent input, the value 0 is returned.

 if (!gr)
 {
  cout << " *NcBlocks::GetBlocks* Error : Input TGraph not specified." << endl;
  return 0;
 }

 if (!hout)
 {
  cout << " *NcBlocks::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 if (n<1 || mode<0 || mode>2)
 {
  cout << " *NcBlocks::GetBlocks* Inconsistent input for TGraph treatment : n=" << n << " mode=" << mode << endl;
  return 0;
 }

 Int_t npoints=gr->GetN();

 if (!npoints) return 0;

 // Sort the data points with increasing x-value
 gr->Sort();

 // Represent each observation as a value in a variable binned histogram
 Double_t* xbins=new Double_t[npoints+1];
 Double_t x=0;
 Double_t y=0;
 for (Int_t i=0; i<npoints; i++)
 {
  gr->GetPoint(i,x,y);
  xbins[i]=x;
 }
 // Add an extra bin to contain the last measurement
 xbins[npoints]=(1.+1e-6)*xbins[npoints-1];

 TH1F hin("","",npoints,xbins);
 for (Int_t j=1; j<=npoints; j++)
 {
  gr->GetPoint(j-1,x,y);
  hin.SetBinContent(j,y);
 }

 Int_t nblocks=GetBlocks(&hin,hout,n,mode);

 // Set the output histogram and axes titles
 TString title,str;
 TString xtitle="Samplings (e.g. time)";
 TString ytitle="Measured value";
 TAxis* ax=gr->GetXaxis();
 if (ax)
 {
  str=ax->GetTitle();
  if (str != "") xtitle=str;
 }
 ax=gr->GetYaxis();
 if (ax)
 {
  str=ax->GetTitle();
  if (str != "") ytitle=str;
 }
 title="Block representation for TGraph ";
 title+=gr->GetName();
 title+=" grouped in %-d consecutive samples;";
 title+=xtitle;
 if (mode==0) title+=";Mean ";
 if (mode==1) title+=";Median ";
 if (mode==2) title+=";RMS ";
 title+=ytitle;
 str=title.Format(title,n);
 hout->SetTitle(str);

 delete [] xbins;

 return nblocks;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::Add(TH1* h1,TH1* h2,TH1* hout,Bool_t scale,Double_t c,Double_t d)
{
// Provide the 1-dimensional histogram hout=h1+c*h2+d.
// So, for c=-1 and d=0, the values contained in the histogram "h2" will be 
// subtracted from the corresponding bin values of the input histogram "h1".
// The output histogram "hout" will be given the same binning as the histogram "h1".
//
// Note : All histograms "h1", "h2" and "hout" must be existing 1-dimensional histograms.
//
// This facility is mainly intended for "adding" the variable binned Bayesian Block values
// (contained in "h2") to the corresponding values contained in the bins of "h1".  
// However, it can be applied to other combinations of 1-dimensional histograms as well.
//
// All provided histograms have to be 1-dimensional and histograms with
// variable bin sizes are supported. However, all bin sizes of "h1"
// have to be smaller than or equal to the smallest bin size of "h2".
// Obviously, this is always the case when "h2" contains the Bayesian Block
// partitions of "h1".
//
// The return value indicates whether an error occured (1) or not (0).
//
// The input parameter "scale" allows to scale (kTRUE) or not scale (kFALSE)
// the bin content of "h2" to be "added", to the corresponding bin width of "h1".
//
// Note :
// ------
// Since this facility is mainly intended for "adding" variable binned Bayesian Blocks,
// scaling should *not* be used in case of mode 3 data, whereas for mode 2 data
// the use of scaling will correctly reflect the resulting (event) counts in each bin.
//
// The default value is d=0.

 if (hout) hout->Reset();

 if (!h1 || !h2 || !hout) return 1;

 Int_t ndim1=h1->GetDimension();
 Int_t ndim2=h2->GetDimension();
 Int_t ndimo=hout->GetDimension();

 if (ndim1!=1 || ndim2!=1 || ndimo!=1)
 {
  cout << " *NcBlocks::Add* Error : Histograms should all be 1-dimensional." << endl;
  return 1;
 }

 // Make the X-axis of "hout" identical to the X-axis of "h1"
 TString name=hout->GetName();
 h1->Copy(*hout);
 hout->Reset();

 TString name1=h1->GetName();
 if (name1=="") name1="h1";

 TString name2=h2->GetName();
 if (name2=="") name2="h2";

 TString sc="+";
 if (c<0) sc="-";
 if (fabs(c)!=1)
 {
  sc.Form("%-+.3g",c);
  sc+="*";
 }

 TString sd="";
 sd.Form("%-+.3g",d);

 TString title="Resulting histogram of: ";
 title+=name1;
 if (c)
 {
  title+=sc;
  title+=name2;
 }
 if (d) title+=sd;
 if (scale)
 {
  title+=" (scaled w.r.t. bin size)"; 
 }
 else
 {
  title+=" (not scaled w.r.t. bin size)"; 
 }
 title+=";";
 TAxis* axis1=h1->GetXaxis();
 title+=axis1->GetTitle();
 title+=";";
 axis1=h1->GetYaxis();
 title+=axis1->GetTitle();

 hout->SetName(name);
 hout->SetTitle(title);

 Int_t nb1=h1->GetNbinsX();
 Int_t nb2=h2->GetNbinsX();

 if (!nb1 || !nb2) return 1;

 // Get the largest bin size of "h1"
 Double_t bwidth1=0;
 Double_t bwmax1=0;
 Int_t imax1=0;
 for (Int_t i=1; i<=nb1; i++)
 {
  bwidth1=h1->GetBinWidth(i);
  if (i==1 || bwidth1>bwmax1)
  {
   bwmax1=bwidth1;
   imax1=i;
  }
 }

 // Get the smallest bin size of "h2" 
 Double_t bwidth2=0;
 Double_t bwmin2=0;
 Int_t imin2=0;
 for (Int_t i=1; i<=nb2; i++)
 {
  bwidth2=h2->GetBinWidth(i);
  if (i==1 || bwidth2<bwmin2)
  {
   bwmin2=bwidth2;
   imin2=i;
  }
 }

 Double_t ratio=bwmax1/bwmin2;
 if (ratio>1.001)
 {
  printf(" *NcBlocks::Add* Error : Larger bin size encountered in histogram %-s than in %-s \n",name1.Data(),name2.Data());
  printf(" %-s: binsize=%-g for bin=%-i  %-s: binsize=%-g for bin=%-i \n",name1.Data(),bwmax1,imax1,name2.Data(),bwmin2,imin2);
  return 1;
 }

 // Loop over all the bins of the input histogram h1
 Double_t x1=0;
 Double_t y1=0;
 Int_t i2=0;
 Double_t y2=0;
 Double_t ynew=0;
 TAxis* axis2=h2->GetXaxis();
 for (Int_t i1=1; i1<=nb1; i1++)
 {
  x1=h1->GetBinCenter(i1);
  y1=h1->GetBinContent(i1);
  bwidth1=h1->GetBinWidth(i1);
  i2=axis2->FindFixBin(x1);
  y2=h2->GetBinContent(i2);
  bwidth2=h2->GetBinWidth(i2);

  // Do not consider underflow or overflow bins
  if (i2<1 || i2>nb2) continue;

  if (scale) y2*=bwidth1/bwidth2;
  ynew=y1+c*y2+d;
  hout->SetBinContent(i1,ynew);
 }

 return 0;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::Add(TGraph* gr,TH1* h,TGraph* gout,Double_t c,Double_t d)
{
// Provide the TGraph gout=gr+c*h+d, where "h" is a 1-dimensional histogram.
// So, for c=-1 and d=0, the values contained in the histogram "h" will be 
// subtracted from the corresponding values of the input graph "gr".
//
// Notes :
// -------
// 1) Both graphs "gr" and "gout" as well as histogram "h" must be existing.
// 2) In case both "gr" and "gout" are TGraphErrors objects, the errors
//    of "gout" will be set to the values of the errors of the input graph "gr".
//
// The return value indicates whether an error occured (1) or not (0).
//
// This facility is mainly intended for "adding" the variable binned Bayesian Block values
// corresponding to the mode 3 data contained in the graph "gr".
// However, it can be applied to any combination of input graph "gr" and histogram "h".
//
// The default value is d=0.

 if (gout) gout->Set(0);

 if (!gr || !h || !gout) return 1;

 TString nameg=gr->GetName();
 if (nameg=="") nameg="gr";

 TString nameh=h->GetName();
 if (nameh=="") nameh="h";

 TString sc="+";
 if (c<0) sc="-";
 if (fabs(c)!=1)
 {
  sc.Form("%-+.3g",c);
  sc+="*";
 }

 TString sd="";
 sd.Form("%-+.3g",d);

 TString title="Resulting graph of: ";
 title+=nameg;
 if (c)
 {
  title+=sc;
  title+=nameh;
 }
 if (d) title+=sd;
 title+=";";
 TAxis* axis=0;
 axis=gr->GetXaxis();
 title+=axis->GetTitle();
 title+=";";
 axis=gr->GetYaxis();
 title+=axis->GetTitle();

 gout->SetTitle(title);

 Int_t ndim=h->GetDimension();

 if (ndim!=1)
 {
  cout << " *NcBlocks::Add* Error : Histogram " << nameh << " should be 1-dimensional." << endl;
  return 1;
 }

 Int_t np=gr->GetN();
 Int_t nb=h->GetNbinsX();

 if (!np || !nb) return 1;

 // Loop over all the points in the graph
 Double_t x=0;
 Double_t y=0;
 Double_t ex=0;
 Double_t ey=0;
 Int_t hbin=0;
 Double_t hval=0;
 Double_t ynew=0;
 axis=h->GetXaxis();
 for (Int_t i=0; i<np; i++)
 {
  gr->GetPoint(i,x,y);
  hbin=axis->FindFixBin(x);
  hval=h->GetBinContent(hbin);

  // Do not consider underflow or overflow bins
  if (hbin<1 || hbin>nb) continue;

  ynew=y+c*hval+d;
  gout->SetPoint(i,x,ynew);

  if (gr->InheritsFrom("TGraphErrors") && gout->InheritsFrom("TGraphErrors"))
  {
   TGraphErrors* gre=(TGraphErrors*)gr;
   TGraphErrors* goute=(TGraphErrors*)gout;
   ex=gre->GetErrorX(i);
   ey=gre->GetErrorY(i);
   goute->SetPointError(i,ex,ey);
  }
 }

 return 0;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::Divide(TH1* h1,TH1* h2,TH1* hout,Bool_t scale,Double_t c,Double_t d)
{
// Provide the 1-dimensional histogram hout=d+h1/(c*h2).
// The output histogram "hout" will be given the same binning as the histogram "h1".
//
// Note : All histograms "h1", "h2" and "hout" must be existing 1-dimensional histograms.
//
// This facility is mainly intended for dividing the values contained in the bins of "h1"
// by the corresponding variable binned Bayesian Block values contained in "h2".
// However, it can be applied to other combinations of 1-dimensional histograms as well.
//
// All provided histograms have to be 1-dimensional and histograms with
// variable bin sizes are supported. However, all bin sizes of "h1"
// have to be smaller than or equal to the smallest bin size of "h2".
// Obviously, this is always the case when "h2" contains the Bayesian Block
// partitions of "h1".
//
// The return value indicates whether an error occured (1) or not (0).
//
// The input parameter "scale" allows to scale (kTRUE) or not scale (kFALSE)
// the bin content of "h2" to the corresponding bin width of "h1".
//
// Note :
// ------
// Since this facility is mainly intended for division by variable binned Bayesian Blocks,
// scaling should *not* be used in case of mode 3 data, whereas for mode 2 data
// the use of scaling will correctly reflect the resulting (event) counts in each bin.
//
// The default value is d=0.

 if (hout) hout->Reset();

 if (!c)
 {
  printf(" *NcBlocks::Divide* Error : Invalid value c=0. \n");
  return 1;
 }

 if (!h1 || !h2 || !hout) return 1;

 Int_t ndim1=h1->GetDimension();
 Int_t ndim2=h2->GetDimension();
 Int_t ndimo=hout->GetDimension();

 if (ndim1!=1 || ndim2!=1 || ndimo!=1)
 {
  cout << " *NcBlocks::Divide* Error : Histograms should all be 1-dimensional." << endl;
  return 1;
 }

 // Make the X-axis of "hout" identical to the X-axis of "h1"
 TString name=hout->GetName();
 h1->Copy(*hout);
 hout->Reset();

 TString name1=h1->GetName();
 if (name1=="") name1="h1";

 TString name2=h2->GetName();
 if (name2=="") name2="h2";

 TString sc="/";
 if (fabs(c)!=1) sc.Form("%-.3g*",fabs(c));

 TString sd;
 if (c>0)
 {
  sd="";
  if (d) sd.Form("%-.3g+",d);
 }
 else
 {
  sd="-";
  if (d) sd.Form("%-.3g-",d);
 }

 TString title="Resulting histogram of: ";
 if (sd!="") title+=sd;
 if (c)
 {
  title+=name1;
  if (sc.Contains("*")) title+="/(";
  title+=sc;
  title+=name2;
  if (sc.Contains("*")) title+=")";
 }
 if (scale)
 {
  title+=" (scaled w.r.t. bin size)"; 
 }
 else
 {
  title+=" (not scaled w.r.t. bin size)"; 
 }
 title+=";";
 TAxis* axis1=h1->GetXaxis();
 title+=axis1->GetTitle();
 title+=";";
 axis1=h1->GetYaxis();
 title+=axis1->GetTitle();

 hout->SetName(name);
 hout->SetTitle(title);

 Int_t nb1=h1->GetNbinsX();
 Int_t nb2=h2->GetNbinsX();

 if (!nb1 || !nb2) return 1;

 // Get the largest bin size of "h1"
 Double_t bwidth1=0;
 Double_t bwmax1=0;
 Int_t imax1=0;
 for (Int_t i=1; i<=nb1; i++)
 {
  bwidth1=h1->GetBinWidth(i);
  if (i==1 || bwidth1>bwmax1)
  {
   bwmax1=bwidth1;
   imax1=i;
  }
 }

 // Get the smallest bin size of "h2" 
 Double_t bwidth2=0;
 Double_t bwmin2=0;
 Int_t imin2=0;
 for (Int_t i=1; i<=nb2; i++)
 {
  bwidth2=h2->GetBinWidth(i);
  if (i==1 || bwidth2<bwmin2)
  {
   bwmin2=bwidth2;
   imin2=i;
  }
 }

 Double_t ratio=bwmax1/bwmin2;
 if (ratio>1.001)
 {
  printf(" *NcBlocks::Divide* Error : Larger bin size encountered in histogram %-s than in %-s \n",name1.Data(),name2.Data());
  printf(" %-s: binsize=%-g for bin=%-i  %-s: binsize=%-g for bin=%-i \n",name1.Data(),bwmax1,imax1,name2.Data(),bwmin2,imin2);
  return 1;
 }

 // Loop over all the bins of the input histogram h1
 Double_t x1=0;
 Double_t y1=0;
 Int_t i2=0;
 Double_t y2=0;
 Double_t val=0;
 Double_t ynew=0;
 TAxis* axis2=h2->GetXaxis();
 for (Int_t i1=1; i1<=nb1; i1++)
 {
  x1=h1->GetBinCenter(i1);
  y1=h1->GetBinContent(i1);
  bwidth1=h1->GetBinWidth(i1);
  i2=axis2->FindFixBin(x1);
  y2=h2->GetBinContent(i2);
  bwidth2=h2->GetBinWidth(i2);

  // Do not consider underflow or overflow bins
  if (i2<1 || i2>nb2) continue;

  val=c*y2;
  if (!val) continue;

  if (scale) y2*=bwidth1/bwidth2;
  ynew=d+y1/val;
  hout->SetBinContent(i1,ynew);
 }

 return 0;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::Divide(TGraph* gr,TH1* h,TGraph* gout,Double_t c,Double_t d)
{
// Provide the TGraph gout=d+gr/(c*h), where "h" is a 1-dimensional histogram.
//
// Notes :
// -------
// 1) Both graphs "gr" and "gout" as well as histogram "h" must be existing.
// 2) In case both "gr" and "gout" are TGraphErrors objects, the errors
//    of "gout" will be set to the values of the errors of the input graph "gr".
//
// The return value indicates whether an error occured (1) or not (0).
//
// This facility is mainly intended for dividing the mode 3 data contained in the graph "gr"
// by the corresponding variable binned Bayesian Block values contained in "h".
// However, it can be applied to any combination of input graph "gr" and histogram "h".
//
// The default value is d=0.

 if (gout) gout->Set(0);

 if (!c)
 {
  printf(" *NcBlocks::Divide* Error : Invalid value c=0. \n");
  return 1;
 }

 if (!gr || !h || !gout) return 1;

 TString nameg=gr->GetName();
 if (nameg=="") nameg="gr";

 TString nameh=h->GetName();
 if (nameh=="") nameh="h";

 TString sc="/";
 if (fabs(c)!=1) sc.Form("%-.3g*",fabs(c));

 TString sd;
 if (c>0)
 {
  sd="";
  if (d) sd.Form("%-.3g+",d);
 }
 else
 {
  sd="-";
  if (d) sd.Form("%-.3g-",d);
 }

 TString title="Resulting graph of: ";
 if (sd!="") title+=sd;
 if (c)
 {
  title+=nameg;
  if (sc.Contains("*")) title+="/(";
  title+=sc;
  title+=nameh;
  if (sc.Contains("*")) title+=")";
 }
 title+=";";
 TAxis* axis=0;
 axis=gr->GetXaxis();
 title+=axis->GetTitle();
 title+=";";
 axis=gr->GetYaxis();
 title+=axis->GetTitle();

 gout->SetTitle(title);

 Int_t ndim=h->GetDimension();

 if (ndim!=1)
 {
  cout << " *NcBlocks::Divide* Error : Histogram " << nameh << " should be 1-dimensional." << endl;
  return 1;
 }

 Int_t np=gr->GetN();
 Int_t nb=h->GetNbinsX();

 if (!np || !nb) return 1;

 // Loop over all the points in the graph
 Double_t x=0;
 Double_t y=0;
 Double_t ex=0;
 Double_t ey=0;
 Int_t hbin=0;
 Double_t hval=0;
 Double_t val=0;
 Double_t ynew=0;
 axis=h->GetXaxis();
 Int_t j=0;
 for (Int_t i=0; i<np; i++)
 {
  gr->GetPoint(i,x,y);
  hbin=axis->FindFixBin(x);
  hval=h->GetBinContent(hbin);

  // Do not consider underflow or overflow bins
  if (hbin<1 || hbin>nb) continue;

  val=c*hval;
  if (!val) continue;

  ynew=d+y/val;
  gout->SetPoint(j,x,ynew);

  if (gr->InheritsFrom("TGraphErrors") && gout->InheritsFrom("TGraphErrors"))
  {
   TGraphErrors* gre=(TGraphErrors*)gr;
   TGraphErrors* goute=(TGraphErrors*)gout;
   ex=gre->GetErrorX(i);
   ey=gre->GetErrorY(i);
   goute->SetPointError(j,ex,ey);
  }

  j++;
 }

 return 0;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcBlocks::Rebin(TH1* hin,TH1* hout,Bool_t scale,Int_t nbins,Double_t xmin,Double_t xmax)
{
// Provide the 1-dimensional histogram "hout" as a uniformly binned version of the
// input 1-dimensional histogram "hin" over the interval [xmin,xmax].
// This will for instance allow a Fourier analysis of a Bayesian Block result in the time domain.
//
// Note : Both histograms "hin" and "hout" must be existing 1-dimensional histograms.
//
// The bin size of "hout" has to be smaller than or equal to the smallest bin size of
// "hin" encountered in the interval [xmin,xmax] (see also notes below).
//
// Function arguments :
// --------------------
// hin   : Pointer to the input histogram
// hout  : Pointer to the output histogram
// scale : Scale the bin content to the bin width (kTRUE) or not (kFALSE)
// nbins : The number of bins for the output histogram (see notes below)
// xmin  : The minimal x-value for the output histogram (see notes below)
// xmax  : The maximal x-value for the output histogram (see notes below)
//
// Notes :
// -------
// 1) Since this facility is mainly intended for re-binning variable binned Bayesian Blocks,
//    scaling should *not* be used in case of mode 3 data, whereas for mode 2 data
//    the use of scaling will correctly reflect the number of (event) counts in each bin.
// 2) If nbins<1 the bin size of "hout" will automatically be set to the smallest bin size of
//    "hin" that is encountered in the interval [xmin,xmax].
// 3) In case xmax<xmin, the xmin and xmax values of the input histogram are used.
//
// The return value is the number of bins of the produced output histogram "hout".
//
// The default values are nbins=0, xmin=0 and xmax=-1.

 if (hout) hout->Reset();

 if (!hin || !hout) return 0;

 Int_t ndimi=hin->GetDimension();
 Int_t ndimo=hout->GetDimension();

 if (ndimi!=1 || ndimo!=1)
 {
  cout << " *NcBlocks::Rebin* Error : Histograms should both be 1-dimensional." << endl;
  return 0;
 }

 Int_t nb1=hin->GetNbinsX();

 if (!nb1) return 0;

 TAxis* xaxis=hin->GetXaxis();
 TAxis* yaxis=hin->GetYaxis();

 if (xmax<xmin)
 {
  xmin=xaxis->GetXmin();
  xmax=xaxis->GetXmax();
 }

 Double_t bwidth1=0;
 Double_t xlow1=0;
 Double_t xup1=0;

 // Automatic binwidth setting
 if (nbins<=0)
 {
  Double_t bwmin=-1;
  for (Int_t i=1; i<=nb1; i++)
  {
   bwidth1=hin->GetBinWidth(i);
   xlow1=hin->GetBinLowEdge(i);
   xup1=xlow1+bwidth1;

   if (xlow1>=xmax || xup1<=xmin) continue;

   if (bwmin<0 || bwidth1<bwmin) bwmin=bwidth1;
  }

  if (bwmin<0)
  {
   cout << " *NcBlocks::Rebin* Error : Input histogram had no data in requested interval [xmin,xmax]." << endl;
   return 0;
  }

  Double_t rnbins=(xmax-xmin)/bwmin;
  nbins=int(rnbins);
  Double_t diff=rnbins-double(nbins);
  if (diff) nbins=nbins+1;
 }

 hout->SetBins(nbins,xmin,xmax);
 Double_t bwidth=hout->GetBinWidth(1);

 TString name=hin->GetName();
 if (name=="") name="hin";

 TString snb="";
 snb.Form("   nbins=%-i",nbins);

 TString smin="";
 smin.Form(" xmin=%-.3g",xmin);

 TString smax="";
 smax.Form(" xmax=%-.3g",xmax);

 TString title="Uniformly binned version of histogram: ";
 title+=name;
 title+=snb;
 title+=smin;
 title+=smax;
 if (scale)
 {
  title+=" (scaled w.r.t. bin size)"; 
 }
 else
 {
  title+=" (not scaled w.r.t. bin size)"; 
 }
 title+=";";
 title+=xaxis->GetTitle();
 title+=";";
 title+=yaxis->GetTitle();

 hout->SetTitle(title);

 // Check if the binning of "hout" is not coarser than that of "hin" within [xmin,xmax]
 for (Int_t i=1; i<=nb1; i++)
 {
  bwidth1=hin->GetBinWidth(i);
  xlow1=hin->GetBinLowEdge(i);
  xup1=xlow1+bwidth1;

  if (xlow1>=xmax || xup1<=xmin) continue;

  if (bwidth1<bwidth)
  {
  printf(" *NcBlocks::Rebin* Error : Input histogram had finer binning than output histogram. \n");
  printf(" Input: binwidth=%-g for bin=%-i  Output: uniform binwidth=%-g \n",bwidth1,i,bwidth);
  return 0;
  }
 }

 // Loop over all the bins of the output histogram hout
 Double_t x=0;
 Int_t i1=0;
 Double_t y1=0;
 for (Int_t i=1; i<=nbins; i++)
 {
  x=hout->GetBinCenter(i);
  i1=xaxis->FindFixBin(x);

  // Do not consider underflow or overflow bins
  if (i1<1 || i1>nb1) continue;

  y1=hin->GetBinContent(i1);
  bwidth1=hin->GetBinWidth(i1);
  if (scale) y1=y1*bwidth/bwidth1;
  hout->SetBinContent(i,y1);
 }

 return nbins;
}
///////////////////////////////////////////////////////////////////////////
