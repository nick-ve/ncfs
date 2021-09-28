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
// Class for Bayesian Block treatment of sequential data.
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
// For each Data Mode a dedicated analysis procedure is provided via the various
// GetBlocks() member functions.
//
// Local variability in the sequential data stream is indicated by so called Change Points,
// at which a step is introduced in e.g. the event rate, event count or measured value.
//
// Since the data treatment is inherently sequential, it is also possible to
// trigger c.q. stop after the occurrence of a certain number of Change Points.
// For instance, an online trigger system may be obtained by specifying to stop at the
// first occurrence of a Change Point.
// Please refer to the documentation of the GetBlocks() member functions for further details. 
//
// Details of the various algorithms can be found in the publication :
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
// TH1F h("h","Example 2: Binned data;Time in sec.;Weighted counts",100,0,100);
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
// q.GetBlocks(&h,fpr,&hblock);
//
// TCanvas c("c","c");
// c.Divide(1,2);
// c.cd(1);
// h.SetLineWidth(2);
// h.Draw();
// hblock.Draw("same");
// c.cd(2);
// hblock.Draw();
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
// q.GetBlocks(gr,fpr,&hblock);
//
// TCanvas c("c","c");
// c.Divide(1,2);
// c.cd(1);
// gr.Draw("A*");
// hblock.Draw("same");
// c.cd(2);
// hblock.Draw();
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
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, September 25, 2021  09:11Z
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
  TString title,s;
  title="Bayesian Block representation with fpr= %-10.3g";
  s=title.Format(title.Data(),fpr);
  hout->SetTitle(s.Data());
 }

 TArrayD best(n);    // Array with optimal fitness values
 TArrayI last(n);    // Array with indices of optimal last change points
 TArrayD lengths(n); // Array with the lengths of the optimal partition blocks
 TArrayD counts(n);  // Array with the event counts of the optimal partition blocks

 Float_t blen=0;   // Length of a certain block
 Float_t bcount=0; // Event count in a certain block
 Double_t prior=0; // Contribution of each block to the prior fitness for a certain partition
 Double_t bfit=0;  // Fitness of a certain block
 Double_t pfit=0;  // Fitness of a certain partition

 Float_t xlow=0;
 Float_t xup=0;
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
 Float_t x=0;
 Float_t y=0;
 ncp=0;
 TArrayF xarr(ncells+1);
 TArrayF yarr(ncells+1);
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
 title+=" with FPR= %-10.3g";
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
  title="Requested trigger at : %-10.3g";
  str=title.Format(title.Data(),xtrig);

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
// Note : The Store Mode of the NcSample must be activated. 
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
  cout << " *NcBlock::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 Int_t n=s.GetN();
 Int_t store=s.GetStoreMode();
 Int_t dim=s.GetDimension();

 if (n<2 || !store || dim<1 || i<1 || i>dim || fpr<0 || fpr>1)
 {
  cout << " *NcBlock::GetBlocks* Inconsistent input for NcSample treatment." << endl;
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
 title+=" with fpr= %-10.3g";
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
  cout << " *NcBlock::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 if (!arr || n<2 || fpr<0 || fpr>1)
 {
  cout << " *NcBlock::GetBlocks* Inconsistent input for array treatment." << endl;
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
 TString title,str;
 title="Bayesian Block representation for unbinned array data with fpr= %-10.3g";
 title+=";Recordings (e.g. time);Count rate";
 str=title.Format(title.Data(),fpr);
 hout->SetTitle(str.Data());

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
  cout << " *NcBlock::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 if (!arr || n<2 || fpr<0 || fpr>1)
 {
  cout << " *NcBlock::GetBlocks* Inconsistent input for array treatment." << endl;
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
 TString title,str;
 title="Bayesian Block representation for unbinned array data with fpr= %-10.3g";
 title+=";Recordings (e.g. time);Count rate";
 str=title.Format(title.Data(),fpr);
 hout->SetTitle(str.Data());

 return xtrig;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcBlocks::GetBlocks(TGraphErrors gr,Double_t fpr,TH1* hout,Int_t ntrig)
{
// Get the Bayesian Block partitions for measurements of an observable (Data Mode 3)
// with a false positive rate "fpr", and provide the results in 1-D histogram "hout".
//
// Note :
// ------
// It is essential that the errors on the y-values are provided, since they are
// used as weights in the statistical analysis.
// The errors on the x-values may be omitted, since they are not used in the process.
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
  cout << " *NcBlock::GetBlocks* Error : Output histogram not specified." << endl;
  return 0;
 }

 Int_t n=gr.GetN();

 if (n<2 || fpr<0 || fpr>1)
 {
  cout << " *NcBlock::GetBlocks* Inconsistent input for TGraphErrors treatment." << endl;
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
 for (Int_t i=0; i<n; i++)
 {
  gr.GetPoint(i,x,y);
  err=fabs(gr.GetErrorX(i));
  xbins[i]=x-err;
 }
 // Add an extra bin to contain the last measurement
 err=fabs(gr.GetErrorX(n-1));
 if (!err) err=fabs(1e-6*xbins[n-1]);
 xbins[n]=xbins[n-1]+2.*err;

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
 title="Bayesian Block representation for TGraphErrors ";
 title+=gr.GetName();
 title+=" with fpr= %-10.3g";
 title+=";Samplings (e.g. time);Measured value";
 str=title.Format(title.Data(),fpr);
 hout->SetTitle(str.Data());

 delete [] xbins;

 return xtrig;
}
///////////////////////////////////////////////////////////////////////////
