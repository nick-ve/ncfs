/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.                     *
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

// $Id: NcSample.cxx 69 2012-10-05 16:31:56Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcSample
// Perform statistics on various multi-dimensional data samples.
// A data sample can be filled using the "Enter" and/or "Remove" functions,
// whereas the "Reset" function resets the complete sample to 'empty'.
// The info which can be extracted from a certain data sample are the
// sum, mean, variance, sigma, covariance and correlation.
// The "Data" function provides all statistics data for a certain sample.
// The variables for which these stat. parameters have to be calculated
// are indicated by the index of the variable which is passed as an
// argument to the various member functions.
// The index convention for a data point (x,y) is : x=1  y=2
//
// Example :
// ---------
// For an NcSample s a data point (x,y) can be entered as s.Enter(x,y) and
// the mean_x can be obtained as s.GetMean(1) whereas the mean_y is obtained
// via s.GetMean(2).
// The correlation between x and y is available via s.GetCor(1,2).
// The x-statistics are obtained via s.Data(1), y-statistics via s.Data(2),
// and the covariance and correlation between x and y via s.Data(1,2).
// All statistics of a sample are obtained via s.Data().
//
//--- Author: Nick van Eijndhoven 30-mar-1996 CERN Geneva
//- Modified: NvE $Date: 2012-10-05 18:31:56 +0200 (vr, 05 okt 2012) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "NcSample.h"
#include "Riostream.h"
 
ClassImp(NcSample) // Class implementation to enable ROOT I/O
 
NcSample::NcSample()
{
// Creation of an Ncample object and resetting the statistics values
// The dimension is initialised to maximum
 fDim=fMaxdim;
 fNames[0]='X';
 fNames[1]='Y';
 fNames[2]='Z';
 fN=0;
 fRemove=0;
 fStore=0;
 fX=0;
 fY=0;
 fZ=0;
 fArr=0;
 Reset();
}
///////////////////////////////////////////////////////////////////////////
NcSample::~NcSample()
{
// Default destructor
 if (fX)
 {
  delete fX;
  fX=0;
 }
 if (fY)
 {
  delete fY;
  fY=0;
 }
 if (fZ)
 {
  delete fZ;
  fZ=0;
 }
 if (fArr)
 {
  delete fArr;
  fArr=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Reset()
{
// Resetting the statistics values for a certain Sample object
// Dimension and storage flag are NOT changed
 fN=0;
 fRemove=0;
 for (Int_t i=0; i<fDim; i++)
 {
  fSum[i]=0.;
  fMean[i]=0.;
  fVar[i]=0.;
  fSigma[i]=0.;
  fMin[i]=0;
  fMax[i]=0;
  for (Int_t j=0; j<fDim; j++)
  {
   fSum2[i][j]=0.;
   fCov[i][j]=0.;
   fCor[i][j]=0.;
  }
 }

 // Set storage arrays to initial size
 if (fX) fX->Set(10);
 if (fY) fY->Set(10);
 if (fZ) fZ->Set(10);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Enter(Double_t x)
{
// Entering a value into a 1-dim. sample
// In case of first entry the dimension is set to 1
 if (fN == 0)
 {
  fDim=1;
  fNames[0]='X';
  fNames[1]='-';
  fNames[2]='-';
 }
 if (fDim != 1)
 {
  cout << " *NcSample::Enter* Error : Not a 1-dim sample." << endl;
 }
 else
 {
  fN+=1;
  fSum[0]+=x;
  fSum2[0][0]+=x*x;

  if (fN==1)
  {
   fMin[0]=x;
   fMax[0]=x;
  }
  else
  {
   if (x<fMin[0]) fMin[0]=x;
   if (x>fMax[0]) fMax[0]=x;
  }

  // Store all entered data when storage mode has been selected 
  if (fStore)
  {
   if (!fX) fX=new TArrayD(10);
   if (fX->GetSize() < fN) fX->Set(fN+10);
   fX->AddAt(x,fN-1);
  }

  // Compute the various statistics
  Compute();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Remove(Double_t x)
{
// Removing a value from a 1-dim. sample

 if (!fN) return;

 if (fDim != 1)
 {
  cout << " *NcSample::Remove* Error : Not a 1-dim sample." << endl;
 }
 else
 {
  fRemove=1;
  fN-=1;
  fSum[0]-=x;
  fSum2[0][0]-=x*x;

  // Remove data entry from the storage
  if (fStore && fX)
  {
   Double_t val=0;
   for (Int_t i=0; i<=fN; i++)
   {
    val=fX->At(i);
    if (fabs(x-val)>1.e-10) continue;

    for (Int_t j=i+1; j<=fN; j++)
    {
     val=fX->At(j);
     fX->AddAt(val,j-1);
    }
   }
  }

  // Compute the various statistics
  Compute();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Enter(Double_t x,Double_t y)
{
// Entering a pair (x,y) into a 2-dim. sample
// In case of first entry the dimension is set to 2
 if (fN == 0)
 {
  fDim=2;
  fNames[0]='X';
  fNames[1]='Y';
  fNames[2]='-';
 }
 if (fDim != 2)
 {
  cout << " *NcSample::Enter* Error : Not a 2-dim sample." << endl;
 }
 else
 {
  fN+=1;
  fSum[0]+=x;
  fSum[1]+=y;
  fSum2[0][0]+=x*x;
  fSum2[0][1]+=x*y;
  fSum2[1][0]+=y*x;
  fSum2[1][1]+=y*y;

  if (fN==1)
  {
   fMin[0]=x;
   fMax[0]=x;
   fMin[1]=y;
   fMax[1]=y;
  }
  else
  {
   if (x<fMin[0]) fMin[0]=x;
   if (x>fMax[0]) fMax[0]=x;
   if (y<fMin[1]) fMin[1]=y;
   if (y>fMax[1]) fMax[1]=y;
  }

  // Store all entered data when storage mode has been selected 
  if (fStore)
  {
   if (!fX) fX=new TArrayD(10);
   if (fX->GetSize() < fN) fX->Set(fN+10);
   fX->AddAt(x,fN-1);
   if (!fY) fY=new TArrayD(10);
   if (fY->GetSize() < fN) fY->Set(fN+10);
   fY->AddAt(y,fN-1);
  }

  // Compute the various statistics
  Compute();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Remove(Double_t x,Double_t y)
{
// Removing a pair (x,y) from a 2-dim. sample

 if (!fN) return;

 if (fDim != 2)
 {
  cout << " *NcSample::Remove* Error : Not a 2-dim sample." << endl;
 }
 else
 {
  fRemove=1;
  fN-=1;
  fSum[0]-=x;
  fSum[1]-=y;
  fSum2[0][0]-=x*x;
  fSum2[0][1]-=x*y;
  fSum2[1][0]-=y*x;
  fSum2[1][1]-=y*y;

  // Remove data entry from the storage
  if (fStore && fX && fY)
  {
   Double_t val=0;
   for (Int_t i=0; i<=fN; i++)
   {
    val=fX->At(i);
    if (fabs(x-val)>1.e-10) continue;
    val=fY->At(i);
    if (fabs(y-val)>1.e-10) continue;

    for (Int_t j=i+1; j<=fN; j++)
    {
     val=fX->At(j);
     fX->AddAt(val,j-1);
     val=fY->At(j);
     fY->AddAt(val,j-1);
    }
   }
  }

  // Compute the various statistics
  Compute();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Enter(Double_t x,Double_t y,Double_t z)
{
// Entering a set (x,y,z) into a 3-dim. sample
// In case of first entry the dimension is set to 3
 if (fN == 0)
 {
  fDim=3;
  fNames[0]='X';
  fNames[1]='Y';
  fNames[2]='Z';
 }
 if (fDim != 3)
 {
  cout << " *NcSample::Enter* Error : Not a 3-dim sample." << endl;
 }
 else
 {
  fN+=1;
  fSum[0]+=x;
  fSum[1]+=y;
  fSum[2]+=z;
  fSum2[0][0]+=x*x;
  fSum2[0][1]+=x*y;
  fSum2[0][2]+=x*z;
  fSum2[1][0]+=y*x;
  fSum2[1][1]+=y*y;
  fSum2[1][2]+=y*z;
  fSum2[2][0]+=z*x;
  fSum2[2][1]+=z*y;
  fSum2[2][2]+=z*z;

  if (fN==1)
  {
   fMin[0]=x;
   fMax[0]=x;
   fMin[1]=y;
   fMax[1]=y;
   fMin[2]=z;
   fMax[2]=z;
  }
  else
  {
   if (x<fMin[0]) fMin[0]=x;
   if (x>fMax[0]) fMax[0]=x;
   if (y<fMin[1]) fMin[1]=y;
   if (y>fMax[1]) fMax[1]=y;
   if (z<fMin[2]) fMin[2]=z;
   if (z>fMax[2]) fMax[2]=z;
  }

  // Store all entered data when storage mode has been selected 
  if (fStore)
  {
   if (!fX) fX=new TArrayD(10);
   if (fX->GetSize() < fN) fX->Set(fN+10);
   fX->AddAt(x,fN-1);
   if (!fY) fY=new TArrayD(10);
   if (fY->GetSize() < fN) fY->Set(fN+10);
   fY->AddAt(y,fN-1);
   if (!fZ) fZ=new TArrayD(10);
   if (fZ->GetSize() < fN) fZ->Set(fN+10);
   fZ->AddAt(z,fN-1);
  }

  // Compute the various statistics
  Compute();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Remove(Double_t x,Double_t y,Double_t z)
{
// Removing a set (x,y,z) from a 3-dim. sample

 if (!fN) return;

 if (fDim != 3)
 {
  cout << " *NcSample::Remove* Error : Not a 3-dim sample." << endl;
 }
 else
 {
  fRemove=1;
  fN-=1;
  fSum[0]-=x;
  fSum[1]-=y;
  fSum[2]-=z;
  fSum2[0][0]-=x*x;
  fSum2[0][1]-=x*y;
  fSum2[0][2]-=x*z;
  fSum2[1][0]-=y*x;
  fSum2[1][1]-=y*y;
  fSum2[1][2]-=y*z;
  fSum2[2][0]-=z*x;
  fSum2[2][1]-=z*y;
  fSum2[2][2]-=z*z;

  // Remove data entry from the storage
  if (fStore && fX && fY && fZ)
  {
   Double_t val=0;
   for (Int_t i=0; i<=fN; i++)
   {
    val=fX->At(i);
    if (fabs(x-val)>1.e-10) continue;
    val=fY->At(i);
    if (fabs(y-val)>1.e-10) continue;
    val=fZ->At(i);
    if (fabs(z-val)>1.e-10) continue;

    for (Int_t j=i+1; j<=fN; j++)
    {
     val=fX->At(j);
     fX->AddAt(val,j-1);
     val=fY->At(j);
     fY->AddAt(val,j-1);
     val=fZ->At(j);
     fZ->AddAt(val,j-1);
    }
   }
  }

  // Compute the various statistics
  Compute();
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Compute()
{
// Computation of the various statistical values
// after each entering or removing action on a certain sample
 Double_t rn=fN;
 for (Int_t k=0; k<fDim; k++)
 {
  fMean[k]=fSum[k]/rn;
  fVar[k]=(fSum2[k][k]/rn)-(fMean[k]*fMean[k]);
  if (fVar[k] < 0.) fVar[k]=0.;
  fSigma[k]=sqrt(fVar[k]);
 }
 for (Int_t i=0; i<fDim; i++)
 {
  for (Int_t j=0; j<fDim; j++)
  {
   fCov[i][j]=(fSum2[i][j]/rn)-(fMean[i]*fMean[j]);
   Double_t sigij=fSigma[i]*fSigma[j];
   if (sigij != 0.) fCor[i][j]=fCov[i][j]/sigij;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcSample::GetDimension() const
{
// Provide the dimension of a certain sample
 return fDim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcSample::GetN() const
{
// Provide the number of entries of a certain sample
 return fN;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSum(Int_t i) const
{
// Provide the sum of a certain variable
 if (fDim < i)
 {
  cout << " *NcSample::GetSum* Error : Dimension less than " << i << endl;
  return 0.;
 }
 else
 {
 return fSum[i-1];
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMean(Int_t i) const
{
// Provide the mean of a certain variable
 if (fDim < i)
 {
  cout << " *NcSample::GetMean* Error : Dimension less than " << i << endl;
  return 0.;
 }
 else
 {
 return fMean[i-1];
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetVar(Int_t i) const
{
// Provide the variance of a certain variable
 if (fDim < i)
 {
  cout << " *NcSample::GetVar* Error : Dimension less than " << i << endl;
  return 0.;
 }
 else
 {
 return fVar[i-1];
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSigma(Int_t i) const
{
// Provide the standard deviation of a certain variable
 if (fDim < i)
 {
  cout << " *NcSample::GetSigma* Error : Dimension less than " << i << endl;
  return 0.;
 }
 else
 {
 return fSigma[i-1];
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCov(Int_t i,Int_t j) const
{
// Provide the covariance between variables i and j
 if ((fDim < i) || (fDim < j))
 {
  Int_t k=i;
  if (j > i) k=j;
  cout << " *NcSample::GetCov* Error : Dimension less than " << k << endl;
  return 0.;
 }
 else
 {
 return fCov[i-1][j-1];
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCor(Int_t i,Int_t j) const
{
// Provide the correlation between variables i and j
 if ((fDim < i) || (fDim < j))
 {
  Int_t k=i;
  if (j > i) k=j;
  cout << " *NcSample::GetCor* Error : Dimension less than " << k << endl;
  return 0.;
 }
 else
 {
 return fCor[i-1][j-1];
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Data()
{
// Printing of statistics of all variables
 for (Int_t i=0; i<fDim; i++)
 {
 cout << " " << fNames[i] << " : N = " << fN;
 cout << " Sum = " << fSum[i] << " Mean = " << fMean[i];
 cout << " Var = " << fVar[i] << " Sigma = " << fSigma[i];
 if (fStore)
 {
  cout << endl;
  cout << "     Minimum = " << GetMinimum(i+1);
  cout << " Maximum = " << GetMaximum(i+1);
  cout << " Median = " << GetMedian(i+1);
 }
 cout << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Data(Int_t i)
{
// Printing of statistics of ith variable
 if (fDim < i)
 {
  cout << " *NcSample::Data(i)* Error : Dimension less than " << i << endl;
 }
 else
 {
  cout << " " << fNames[i-1] << " : N = " << fN;
  cout << " Sum = " << fSum[i-1] << " Mean = " << fMean[i-1];
  cout << " Var = " << fVar[i-1] << " Sigma = " << fSigma[i-1];
  if (fStore)
  {
   cout << endl;
   cout << "     Minimum = " << GetMinimum(i);
   cout << " Maximum = " << GetMaximum(i);
   cout << " Median = " << GetMedian(i);
  }
  cout << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Data(Int_t i,Int_t j) const
{
// Printing of covariance and correlation between variables i and j
 if ((fDim < i) || (fDim < j))
 {
  Int_t k=i;
  if (j > i) k=j;
  cout << " *NcSample::Data(i,j)* Error : Dimension less than " << k << endl;
 }
 else
 {
  cout << " " << fNames[i-1] << "-" << fNames[j-1] << " correlation :";
  cout << " Cov. = " << fCov[i-1][j-1] << " Cor. = " << fCor[i-1][j-1] << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::SetStoreMode(Int_t mode)
{
// Set storage mode for all entered data.
//
// mode = 0 : Entered data will not be stored
//        1 : All data will be stored as entered
//
// By default the storage mode is set to 0 in the constructor of this class.
// The default at invokation of this memberfunction is mode=1.
//
// For normal statistics evaluation (e.g. mean, sigma, covariance etc...)
// storage of entered data is not needed. This is the default mode
// of operation and is the most efficient w.r.t. cpu time and memory.
// However, when calculation of a median, minimum or maximum is required,
// then the data storage mode has be activated, unless the statistics
// are obtained from a specified input histogram.
//
// Note : Activation of storage mode can only be performed before the
//        first data item is entered. 

 if (fN)
 {
  cout << " *NcSample::SetStoreMore* Storage mode can only be set before first data." << endl;
 }
 else
 {
  if (mode==0 || mode==1) fStore=mode;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcSample::GetStoreMode() const
{
// Provide the storage mode
 return fStore;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetQuantile(Double_t f,Int_t i)
{
// Provide the value of the i-th variable that marks the quantile with
// fraction "f" of the sample.
// By definition "f" belongs to the interval [0,1] where f=0.5 indicates
// the median of the specified variable.
// For this functionality the storage mode has to be activated.
//
// In the case of incompatible data the value 0 is returned.
//
// Note : For large datasets it is more efficient to determine the quantile
//        via the specification of a histogram. 
//        See the other GetQuantile memberfunction for details.

 if (fDim < i)
 {
  cout << " *NcSample::GetQuantile* Error : Dimension less than " << i << endl;
  return 0;
 }

 if (!fStore)
 {
  cout << " *NcSample::GetQuantile* Error : Storage of data entries was not activated." << endl;
  return 0;
 }

 if (fN<=0) return 0;

 if (f<0 || f>1) return 0;

 Double_t quantile=0;

 if (fN==1)
 {
  if (i==1) quantile=fX->At(0);
  if (i==2) quantile=fY->At(0);
  if (i==3) quantile=fZ->At(0);
  return quantile;
 }

 if (f==0) return GetMinimum(i);
 if (f==1) return GetMaximum(i);

 // Prepare temp. array to hold the ordered values
 if (!fArr)
 {
  fArr=new TArrayD(fN);
 }
 else
 {
  if (fArr->GetSize() < fN) fArr->Set(fN);
 }

 // Order the values of the specified variable
 Double_t val=0;
 Int_t iadd=0;
 for (Int_t j=0; j<fN; j++)
 {
  if (i==1) val=fX->At(j);
  if (i==2) val=fY->At(j);
  if (i==3) val=fZ->At(j);

  iadd=0;
  if (j==0)
  {
   fArr->AddAt(val,j);
   iadd=1;
  }
  else
  {
   for (Int_t k=0; k<j; k++)
   {
    if (val>=fArr->At(k)) continue;
    // Put value in between the existing ones
    for (Int_t m=j-1; m>=k; m--)
    {
     fArr->AddAt(fArr->At(m),m+1);
    }
    fArr->AddAt(val,k);
    iadd=1;
    break;
   }

   if (!iadd)
   {
    fArr->AddAt(val,j);
   }
  }
 }

 quantile=0;
 Double_t rindex=double(fN)*f;
 Int_t index=int(rindex);
 if (fN%2) // Odd number of entries
 {
  quantile=fArr->At(index);
 }
 else // Even number of entries
 {
  quantile=(fArr->At(index-1)+fArr->At(index))/2.;
 }
 return quantile;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMedian(Int_t i)
{
// Provide the median of the i-th variable.
// For this functionality the storage mode has to be activated.
//
// In the case of incompatible data the value 0 is returned.
//
// The median is determined by invokation of GetQuantile(0.5,i).
// Please refer to the documentation of the corresponding GetQuantile()
// memberfunction for further details.
//
// Note : For large datasets it is more efficient to determine the median
//        via the specification of a histogram. 
//        See the other GetMedian memberfunction for details.

 Double_t median=GetQuantile(0.5,i);
 return median;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSpread(Int_t i)
{
// Provide the spread w.r.t. the median of a certain variable.
// The spread is defined as the average of |median-val(i)|.
// For this functionality the storage mode has to be activated.
//
// Note : For large datasets it is more efficient to determine the spread
//        via the specification of a histogram. 
//        See the other GetSpread memberfunction for details.

 if (fDim < i)
 {
  cout << " *NcSample::GetSpread* Error : Dimension less than " << i << endl;
  return 0;
 }

 if (!fStore)
 {
  cout << " *NcSample::GetSpread* Error : Storage of data entries was not activated." << endl;
  return 0;
 }

 if (fN<=1) return 0;

 Double_t median=GetMedian(i);

 Double_t spread=0;
 for (Int_t j=0; j<fN; j++)
 {
  spread+=fabs(median-(fArr->At(j)));
 }

 spread=spread/float(fN);

 return spread;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMinimum(Int_t i) const
{
// Provide the minimum value of a certain variable.
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.

 if (fDim < i)
 {
  cout << " *NcSample::GetMinimum* Error : Dimension less than " << i << endl;
  return 0;
 }

 if (!fRemove) return fMin[i-1];

 if (!fStore)
 {
  cout << " *NcSample::GetMinimum* Error : Storage of data entries was not activated." << endl;
  return 0;
 }

 if (fN<=0) return 0;

 Double_t min=0;

 if (i==1) min=fX->At(0);
 if (i==2) min=fY->At(0);
 if (i==3) min=fZ->At(0);

 for (Int_t k=1; k<fN; k++)
 {
  if (i==1 && fX->At(k)<min) min=fX->At(k);
  if (i==2 && fY->At(k)<min) min=fY->At(k);
  if (i==3 && fZ->At(k)<min) min=fZ->At(k);
 }

 return min;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMaximum(Int_t i) const
{
// Provide the maxmum value of a certain variable.
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.

 if (fDim < i)
 {
  cout << " *NcSample::GetMaximum* Error : Dimension less than " << i << endl;
  return 0;
 }

 if (!fRemove) return fMax[i-1];

 if (!fStore)
 {
  cout << " *NcSample::GetMaximum* Error : Storage of data entries was not activated." << endl;
  return 0;
 }

 if (fN<=0) return 0;

 Double_t max=0;

 if (i==1) max=fX->At(0);
 if (i==2) max=fY->At(0);
 if (i==3) max=fZ->At(0);

 for (Int_t k=1; k<fN; k++)
 {
  if (i==1 && fX->At(k)>max) max=fX->At(k);
  if (i==2 && fY->At(k)>max) max=fY->At(k);
  if (i==3 && fZ->At(k)>max) max=fZ->At(k);
 }

 return max;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetQuantile(Double_t f,TH1* histo,Int_t mode) const
{
// Provide the value of the variable X or Y from the specified 1D histogram
// that marks the quantile with fraction "f" for the selected variable.
// By definition "f" belongs to the interval [0,1] where f=0.5 indicates
// the median of the specified variable.
// For this functionality it is not needed to activate the storage mode.
//
// In the case of incompatible data the value 0 is returned.
//
// In case of an X-quantile this facility uses TH1::GetQuantiles, which
// provides a value which in general is different from any of the
// central bin X values. The user may force the returned X-value to be
// the corresponding central bin X value via the "mode" input argument.
//
// mode = 0 ==> The pure TH1::GetQuantiles X-quantile value is returned.
//        1 ==> The corresponding central bin X value is returned as X-quantile.
//        2 ==> The Y-quantile value is returned.
//
// By default mode=0 will be used to agree with standard ROOT processing.

 if (!histo) return 0;

 if (f<0 || f>1) return 0;

 Double_t quantile=0;

 if (mode==2) // Quantile of Y values
 {
  NcSample temp;
  temp.SetStoreMode(1);
  Double_t val=0;
  for (Int_t i=1; i<=histo->GetNbinsX(); i++)
  {
   val=histo->GetBinContent(i);
   temp.Enter(val);
  }
  quantile=temp.GetQuantile(f,1);
 }
 else // Quantile of X values
 {
  Double_t q[1];
  Double_t p[1];
  p[0]=f;
  histo->ComputeIntegral();
  Int_t nq=histo->GetQuantiles(1,q,p);

  if (!nq) return 0;

  quantile=q[0];
  if (mode==1)
  {
   Int_t mbin=histo->FindBin(q[0]);
   quantile=histo->GetBinCenter(mbin);
  }
 }

 return quantile;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMedian(TH1* histo,Int_t mode) const
{
// Provide the median of the variable X or Y from the specified 1D histogram.
// For this functionality it is not needed to activate the storage mode.
//
// In the case of incompatible data the value 0 is returned.
//
// The median is determined by invokation of GetQuantile(0.5,histo,mode).
// Please refer to the documentation of the corresponding GetQuantile()
// memberfunction for further details.

 Double_t median=GetQuantile(0.5,histo,mode);
 return median;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSpread(TH1* histo,Int_t mode) const
{
// Provide the spread w.r.t. the X or Y median for the specified 1D histogram.
// The spread is defined as the average of |median-val|.
// For this functionality it is not needed to activate the storage mode.
//
// In the case of incompatible data the value 0 is returned.
//
// In case of X-spread, this facility uses TH1::GetQuantiles to determine
// the X-median, which provides a median value which in general is different
// from any of the central bin X values. The user may force the used X-median
// to be the corresponding central bin X value via the "mode" input argument.
//
// mode = 0 ==> The pure TH1::GetQuantiles X-median value is used
//        1 ==> The corresponding central bin X value is used as X-median
//        2 ==> The spread in Y-values w.r.t. the Y-median will be provided
//
// By default mode=0 will be used to agree with standard ROOT processing.

 if (!histo) return 0;

 Double_t spread=0;

 Int_t nbins=histo->GetNbinsX();

 if (mode==2) // Spread in Y values
 {
  NcSample temp;
  temp.SetStoreMode(1);
  Double_t val=0;
  for (Int_t i=1; i<=nbins; i++)
  {
   val=histo->GetBinContent(i);
   temp.Enter(val);
  }
  spread=temp.GetSpread(1);
 }
 else // Spread in X values
 {
  Double_t median=GetMedian(histo,mode);
  Double_t x=0,y=0,ysum=0;
  for (Int_t jbin=1; jbin<=nbins; jbin++)
  {
   x=histo->GetBinCenter(jbin);
   y=histo->GetBinContent(jbin);
   if (y>0)
   {
    spread+=fabs(x-median)*y;
    ysum+=y;
   }
  }
  if (ysum>0) spread=spread/ysum;
 }

 return spread;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetEntry(Int_t i,Int_t j) const
{
// Provide the value of the i-th entry for the j-th variable.
// The first entry is indicated by i=1 and the first variable is j=1.
//
// Note : This facility is only available if the storage mode has been activated.

 if (fDim < j)
 {
  cout << " *NcSample::GetEntry* Error : Dimension less than " << j << endl;
  return 0;
 }

 if (!fStore)
 {
  cout << " *NcSample::GetEntry* Error : Storage mode not activated." << endl;
  return 0;
 }

 if (i<1 || i>fN)
 {
  cout << " *NcSample::GetEntry* Error : Invalid entry number " << i << endl;
  return 0;
 }

 Double_t value=0;
 if (j==1) value=fX->At(i-1);
 if (j==2) value=fY->At(i-1);
 if (j==3) value=fZ->At(i-1);
 return value;
}
///////////////////////////////////////////////////////////////////////////
