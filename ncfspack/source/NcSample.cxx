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

///////////////////////////////////////////////////////////////////////////
// Class NcSample
// Perform statistics on various multi-dimensional data samples.
// A data sample can be filled using the "Enter" and/or "Remove" functions,
// or by importing the data from a TGraph or TGraph2D via the "Load" function.
// The "Reset" function resets the complete sample to 'empty'.
// The info which can be extracted from a certain data sample are the
// minimum, maximum, sum, mean, variance, sigma, median, spread, SNR, CV,
// RMS, covariance and correlation.
// The "spread" of the sample represents the average deviation w.r.t.
// some reference value
// The "Data" function provides all statistics data for a certain sample.
// The variables for which these statistical parameters have to be calculated
// are indicated either by the name or the index of the variable which is
// passed as an argument to the various member functions.
// The index convention for a data point (x,y,z,t) is : x=1  y=2  z=3  t=4.
// The member function SetNames() allows the user to specify different names
// to replace the default ("X","Y","Z","T") naming.
//
// Interfaces with various graphics facilities are provided like for instance
// GetGraph(), Get1DHistogram(), Animation() etc...
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
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel April 10, 2021  22:37Z
///////////////////////////////////////////////////////////////////////////

#include "NcSample.h"
#include "Riostream.h"
 
ClassImp(NcSample) // Class implementation to enable ROOT I/O
 
NcSample::NcSample(const char* name,const char* title) : TNamed(name,title)
{
// Creation of an NcSample object and resetting the statistics values
// The dimension is initialised to maximum

 fDim=fMaxdim;
 fN=0;
 fRemove=0;
 fStore=0;
 fX=0;
 fY=0;
 fZ=0;
 fT=0;
 fArr=0;
 fIndices=0;
 fOrdered=0;
 fGraphT=0;
 fCanvas=0;
 fAnimObject=0;
 Reset();
 SetNames();
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
 if (fT)
 {
  delete fT;
  fT=0;
 }
 if (fArr)
 {
  delete fArr;
  fArr=0;
 }
 if (fIndices)
 {
  delete fIndices;
  fIndices=0;
 }
 if (fGraphT)
 {
  delete fGraphT;
  fGraphT=0;
 }
 if (fCanvas)
 {
  delete fCanvas;
  fCanvas=0;
 }
 if (fAnimObject)
 {
  delete fAnimObject;
  fAnimObject=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcSample::NcSample(const NcSample& s) : TNamed(s)
{
// Copy constructor.

 for (Int_t i=0; i<s.fMaxdim; i++)
 {
  fNames[i]=s.fNames[i];
 }

 fDim=s.fDim;
 fStore=s.fStore;
 fN=s.fN;
 fRemove=s.fRemove;

 fX=0;
 fY=0;
 fZ=0;
 fT=0;
 fArr=0;
 fIndices=0;
 fOrdered=0;
 fGraphT=0;
 fCanvas=0;
 fAnimObject=0;

 if (s.fX) fX=new TArrayD(fN);
 if (s.fY) fY=new TArrayD(fN);
 if (s.fZ) fZ=new TArrayD(fN);
 if (s.fT) fT=new TArrayD(fN);

 Double_t val=0;
 for (Int_t i=0; i<fDim; i++)
 {
  fSum[i]=s.fSum[i];
  fMean[i]=s.fMean[i];
  fRMSdev[i]=s.fRMSdev[i];
  fSigma[i]=s.fSigma[i];
  fMin[i]=s.fMin[i];
  fMax[i]=s.fMax[i];
  for (Int_t j=0; j<fDim; j++)
  {
   fSum2[i][j]=s.fSum2[i][j];
   fCov[i][j]=s.fCov[i][j];
   fCor[i][j]=s.fCor[i][j];
  }
 }

 if (!fStore) return;

 for (Int_t i=0; i<fN; i++)
 {
  if (fX)
  {
   val=s.fX->At(i);
   fX->AddAt(val,i);
  }
  if (fY)
  {
   val=s.fY->At(i);
   fY->AddAt(val,i);
  }
  if (fZ)
  {
   val=s.fZ->At(i);
   fZ->AddAt(val,i);
  }
  if (fT)
  {
   val=s.fT->At(i);
   fT->AddAt(val,i);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Reset()
{
// Resetting the statistics values for this NcSample object.
// Also the variable names are reset to their (X,Y,Z,T) defaults.
// Dimension and storage flag are NOT changed.

 fN=0;
 fRemove=0;
 fOrdered=0;
 for (Int_t i=0; i<fDim; i++)
 {
  fSum[i]=0;
  fMean[i]=0;
  fRMSdev[i]=0;
  fSigma[i]=0;
  fMin[i]=0;
  fMax[i]=0;
  for (Int_t j=0; j<fDim; j++)
  {
   fSum2[i][j]=0;
   fCov[i][j]=0;
   fCor[i][j]=0;
  }
 }

 // Set storage arrays to initial size
 if (fX) fX->Set(10);
 if (fY) fY->Set(10);
 if (fZ) fZ->Set(10);
 if (fT) fT->Set(10);

 // Resetting the variable names to their defaults
 SetNames();

 // Delete the temp. storage arrays for ordering
 if (fArr)
 {
  delete fArr;
  fArr=0;
 }
 if (fIndices)
 {
  delete fIndices;
  fIndices=0;
 }

 // Delete the temp. TGraphTime pointer
 if (fGraphT)
 {
  delete fGraphT;
  fGraphT=0;
 }

 // Delete the multi-purpose canvas
 if (fCanvas)
 {
  delete fCanvas;
  fCanvas=0;
 }

 // Delete the multi-purpose animation object
 if (fAnimObject)
 {
  delete fAnimObject;
  fAnimObject=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::SetNames(TString name1,TString name2,TString name3,TString name4)
{
// Specification of the names of the various variables.
//
// The user specified names will only be stored before the first entry has been entered.
// So, setting (new) names during a sampling process will have no effect.
// However, after invokation of Reset() the variables names can again be specified.
// 
//
// Note : Specification of name="" and name="-" should not be used.
//
// The default values are : name1="X",  name2="Y",  name3="Z" and name4="T".

 if (!fN)
 {
  fNames[0]=name1;
  fNames[1]=name2;
  fNames[2]=name3;
  fNames[3]=name4;
 }
 else
 {
  cout << " *NcSample::SetNames* Names not modified, since data were already present." << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Enter(Double_t x)
{
// Entering a value into a 1-dim. sample
// In case of first entry the dimension is set to 1

 if (!fN)
 {
  fDim=1;
  fNames[1]="";
  fNames[2]="";
  fNames[3]="";
 }

 if (fDim != 1)
 {
  cout << " *NcSample::Enter* Error : Not a 1-dim sample." << endl;
  return;
 }

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
///////////////////////////////////////////////////////////////////////////
void NcSample::Remove(Double_t x)
{
// Removing a value from a 1-dim. sample

 if (!fN) return;

 if (fDim != 1)
 {
  cout << " *NcSample::Remove* Error : Not a 1-dim sample." << endl;
  return;
 }

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
///////////////////////////////////////////////////////////////////////////
void NcSample::Enter(Double_t x,Double_t y)
{
// Entering a pair (x,y) into a 2-dim. sample
// In case of first entry the dimension is set to 2

 if (!fN)
 {
  fDim=2;
  fNames[2]="";
  fNames[3]="";
 }

 if (fDim != 2)
 {
  cout << " *NcSample::Enter* Error : Not a 2-dim sample." << endl;
  return;
 }

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
///////////////////////////////////////////////////////////////////////////
void NcSample::Remove(Double_t x,Double_t y)
{
// Removing a pair (x,y) from a 2-dim. sample

 if (!fN) return;

 if (fDim != 2)
 {
  cout << " *NcSample::Remove* Error : Not a 2-dim sample." << endl;
  return;
 }

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
///////////////////////////////////////////////////////////////////////////
void NcSample::Enter(Double_t x,Double_t y,Double_t z)
{
// Entering a set (x,y,z) into a 3-dim. sample
// In case of first entry the dimension is set to 3

 if (!fN)
 {
  fDim=3;
  fNames[3]="";
 }

 if (fDim != 3)
 {
  cout << " *NcSample::Enter* Error : Not a 3-dim sample." << endl;
  return;
 }

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
///////////////////////////////////////////////////////////////////////////
void NcSample::Remove(Double_t x,Double_t y,Double_t z)
{
// Removing a set (x,y,z) from a 3-dim. sample

 if (!fN) return;

 if (fDim != 3)
 {
  cout << " *NcSample::Remove* Error : Not a 3-dim sample." << endl;
  return;
 }

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
///////////////////////////////////////////////////////////////////////////
void NcSample::Enter(Double_t x,Double_t y,Double_t z,Double_t t)
{
// Entering a set (x,y,z,t) into a 4-dim. sample
// In case of first entry the dimension is set to 4

 if (!fN) fDim=4;

 if (fDim != 4)
 {
  cout << " *NcSample::Enter* Error : Not a 4-dim sample." << endl;
  return;
 }

 fN+=1;
 fSum[0]+=x;
 fSum[1]+=y;
 fSum[2]+=z;
 fSum[3]+=t;
 fSum2[0][0]+=x*x;
 fSum2[0][1]+=x*y;
 fSum2[0][2]+=x*z;
 fSum2[0][3]+=x*t;
 fSum2[1][0]+=y*x;
 fSum2[1][1]+=y*y;
 fSum2[1][2]+=y*z;
 fSum2[1][3]+=y*t;
 fSum2[2][0]+=z*x;
 fSum2[2][1]+=z*y;
 fSum2[2][2]+=z*z;
 fSum2[2][3]+=z*t;
 fSum2[3][0]+=t*x;
 fSum2[3][1]+=t*y;
 fSum2[3][2]+=t*z;
 fSum2[3][3]+=t*t;

 if (fN==1)
 {
  fMin[0]=x;
  fMax[0]=x;
  fMin[1]=y;
  fMax[1]=y;
  fMin[2]=z;
  fMax[2]=z;
  fMin[3]=t;
  fMax[3]=t;
 }
 else
 {
  if (x<fMin[0]) fMin[0]=x;
  if (x>fMax[0]) fMax[0]=x;
  if (y<fMin[1]) fMin[1]=y;
  if (y>fMax[1]) fMax[1]=y;
  if (z<fMin[2]) fMin[2]=z;
  if (z>fMax[2]) fMax[2]=z;
  if (t<fMin[3]) fMin[3]=t;
  if (t>fMax[3]) fMax[3]=t;
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
  if (!fT) fT=new TArrayD(10);
  if (fT->GetSize() < fN) fT->Set(fN+10);
  fT->AddAt(t,fN-1);
 }

 // Compute the various statistics
 Compute();
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Remove(Double_t x,Double_t y,Double_t z,Double_t t)
{
// Removing a set (x,y,z,t) from a 4-dim. sample

 if (!fN) return;

 if (fDim != 4)
 {
  cout << " *NcSample::Remove* Error : Not a 4-dim sample." << endl;
  return;
 }

 fRemove=1;
 fN-=1;
 fSum[0]-=x;
 fSum[1]-=y;
 fSum[2]-=z;
 fSum[3]-=t;
 fSum2[0][0]-=x*x;
 fSum2[0][1]-=x*y;
 fSum2[0][2]-=x*z;
 fSum2[0][3]-=x*t;
 fSum2[1][0]-=y*x;
 fSum2[1][1]-=y*y;
 fSum2[1][2]-=y*z;
 fSum2[1][3]-=y*t;
 fSum2[2][0]-=z*x;
 fSum2[2][1]-=z*y;
 fSum2[2][2]-=z*z;
 fSum2[2][3]-=z*t;
 fSum2[3][0]-=t*x;
 fSum2[3][1]-=t*y;
 fSum2[3][2]-=t*z;
 fSum2[3][3]-=t*t;

 // Remove data entry from the storage
 if (fStore && fX && fY && fZ && fT)
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
   val=fT->At(i);
   if (fabs(t-val)>1.e-10) continue;

   for (Int_t j=i+1; j<=fN; j++)
   {
    val=fX->At(j);
    fX->AddAt(val,j-1);
    val=fY->At(j);
    fY->AddAt(val,j-1);
    val=fZ->At(j);
    fZ->AddAt(val,j-1);
    val=fT->At(j);
    fT->AddAt(val,j-1);
   }
  }
 }

 // Compute the various statistics
 Compute();
}
///////////////////////////////////////////////////////////////////////////
void NcSample::RemoveEntry(Int_t i,Int_t mode,Int_t j)
{
// Remove the full data entry at the index "i" (1=first) after ordering
// w.r.t. the j-th variable (1=first).
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of "j" is irrelevant

 if (fDim<1 || fN<=0 || i<1 || i>fN) return;

 if (mode && (j<1 || j>fDim))
 {
  cout << " *NcSample::RemoveEntry* Error : Invalide argument j=" << j << endl;
  return;
 }

 if (!fStore)
 {
  cout << " *NcSample::RemoveEntry* Error : Storage of data entries was not activated." << endl;
  return;
 }

 Order(mode,j);

 // Get the corresponding original entry index
 Int_t idx=fIndices->At(i-1);

 Double_t x=0;
 Double_t y=0;
 Double_t z=0;
 Double_t t=0;

 if (fX) x=GetEntry(idx+1,1);
 if (fY) y=GetEntry(idx+1,2);
 if (fZ) z=GetEntry(idx+1,3);
 if (fT) t=GetEntry(idx+1,4);

 if (fDim==1) Remove(x);
 if (fDim==2) Remove(x,y);
 if (fDim==3) Remove(x,y,z);
 if (fDim==4) Remove(x,y,z,t);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::RemoveEntry(Int_t i,Int_t mode,TString name)
{
// Remove the full data entry at the index "i" (1=first) after ordering
// w.r.t. the variable with the specified name.
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of name is irrelevant

 Int_t j=GetIndex(name);
 RemoveEntry(i,mode,j);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Order(Int_t mode,Int_t i)
{
// Internal member function to order the entries according to the i-th variable (first is i=1).
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of "i" is irrelevant

 if (mode && (i<1 || i>fDim))
 {
  cout << " *NcSample::Order* Error : Invalide argument i=" << i << endl;
  return;
 }

 if (fDim<1)
 {
  cout << " *NcSample::Order* Error : Dimension less than 1." << endl;
  return;
 }

 if (!fStore)
 {
  cout << " *NcSample::Order* Error : Storage of data entries was not activated." << endl;
  return;
 }

 if (fN<=0)
 {
  fOrdered=0;
  return;
 }

 // Set the corresponding ordering status word
 Int_t iword=10*abs(mode)+i;
 if (mode<0) iword*=-1;

 // No new ordering needed if the ordering status word hasn't changed
 if (iword==fOrdered) return;

 // Store the new ordering status word
 fOrdered=iword;

 // Prepare temp. array to hold the ordered values
 if (!fArr)
 {
  fArr=new TArrayD(fN);
 }
 else
 {
  if (fArr->GetSize() < fN) fArr->Set(fN);
 }

 // Prepare temp. array to hold the ordered indices
 if (!fIndices)
 {
  fIndices=new TArrayI(fN);
 }
 else
 {
  if (fIndices->GetSize() < fN) fIndices->Set(fN);
 }

 Double_t val=0;

 // Just the order in which the entries were entered
 if (!mode)
 {
  for (Int_t j=0; j<fN; j++)
  {
   if (i==1) val=fX->At(j);
   if (i==2) val=fY->At(j);
   if (i==3) val=fZ->At(j);
   if (i==4) val=fT->At(j);
   fArr->AddAt(val,j);
   fIndices->AddAt(j,j);
  }
  return;
 }

 // Order the values of the specified variable
 Int_t iadd=0;
 for (Int_t j=0; j<fN; j++)
 {
  if (i==1) val=fX->At(j);
  if (i==2) val=fY->At(j);
  if (i==3) val=fZ->At(j);
  if (i==4) val=fT->At(j);

  iadd=0;
  if (j==0)
  {
   fArr->AddAt(val,j);
   fIndices->AddAt(j,j);
   iadd=1;
  }
  else
  {
   for (Int_t k=0; k<j; k++)
   {
    if (mode>0 && val>=fArr->At(k)) continue; // Increasing ordering
    if (mode<0 && val<=fArr->At(k)) continue; // Decreasing ordering
    // Put value in between the existing ones
    for (Int_t m=j-1; m>=k; m--)
    {
     fArr->AddAt(fArr->At(m),m+1);
     fIndices->AddAt(fIndices->At(m),m+1);
    }
    fArr->AddAt(val,k);
    fIndices->AddAt(j,k);
    iadd=1;
    break;
   }

   if (!iadd)
   {
    fArr->AddAt(val,j);
    fIndices->AddAt(j,j);
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcSample::GetIndex(TString name) const
{
// Internal member function to provide the index (1=first) of the specified variable name.
// In case of no match, the value 0 is returned.

 Int_t idx=0;

 for (Int_t i=0; i<fMaxdim; i++)
 {
  if (fNames[i]==name) idx=i+1;
 }

 return idx;
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Compute()
{
// Internal member function to compute the various statistical values
// after each entering or removing action on a certain sample.

 // Reset the ordering status word
 fOrdered=0;

 if (fN<=0) return;

 Double_t rn=fN;
 Double_t rn1=fN-1;
 Double_t var=0;
 Double_t rmsij;
 for (Int_t k=0; k<fDim; k++)
 {
  fMean[k]=fSum[k]/rn;
  var=(fSum2[k][k]/rn)-(fMean[k]*fMean[k]);
  if (var<0) var=0;
  fRMSdev[k]=sqrt(var);
  var=0;
  if (rn1)
  {
   var=(fSum2[k][k]-(rn*fMean[k]*fMean[k]))/rn1;
   if (var<0) var=0;
  }
  fSigma[k]=sqrt(var);
 }
 for (Int_t i=0; i<fDim; i++)
 {
  for (Int_t j=0; j<fDim; j++)
  {
   fCov[i][j]=(fSum2[i][j]/rn)-(fMean[i]*fMean[j]);
   fCor[i][j]=0;
   rmsij=fRMSdev[i]*fRMSdev[j];
   if (rmsij != 0) fCor[i][j]=fCov[i][j]/rmsij;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcSample::GetDimension() const
{
// Provide the dimension of this sample
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
// Provide the sum of the i-th variable (first is i=1)

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetSum* Error : Invalid index " << i << endl;
  return 0;
 }
 else
 {
  return fSum[i-1];
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSum(TString name) const
{
// Provide the sum of the variable with the specified name.

 Int_t i=GetIndex(name);
 return GetSum(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMean(Int_t i) const
{
// Provide the mean of the i-th variable (first is i=1)

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetMean* Error : Invalid index " << i << endl;
  return 0;
 }

 return fMean[i-1];
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMean(TString name) const
{
// Provide the mean of the variable with the specified name.

 Int_t i=GetIndex(name);
 return GetMean(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetRMS(Int_t i) const
{
// Provide the Root Mean Square of the i-th variable (first is i=1)
//
// Note :
// ------
// This is NOT the RMS deviation defined as sqrt(variance).
// Use the memberfunction GetSigma() to obtain the RMS deviation.

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetRMS* Error : Invalid index " << i << endl;
  return 0;
 }

 Double_t rms=0;
 Double_t val=fSum2[i-1][i-1]/float(fN);
 if (val>=0) rms=sqrt(val);

 return rms;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetRMS(TString name) const
{
// Provide the Root Mean Square of the variable with the specified name.
//
// Note :
// ------
// This is NOT the RMS deviation defined as sqrt(variance).
// Use the memberfunction GetSigma() to obtain the RMS deviation.

 Int_t i=GetIndex(name);
 return GetRMS(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetVar(Int_t i,Int_t model) const
{
// Provide the variance of the i-th variable (first is i=1)
// as (rms-deviation)^2 when model=0 or as sigma^2 when model=1.
//
// The default value is model=0.

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetVar* Error : Invalid index " << i << endl;
  return 0;
 }

 Double_t var=fRMSdev[i-1]*fRMSdev[i-1];
 if (model) var=fSigma[i-1]*fSigma[i-1];
 
 return var;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetVar(TString name,Int_t model) const
{
// Provide the variance of the variable with the specified name.
// as (rms-deviation)^2 when model=0 or as sigma^2 when model=1.
//
// The default value is model=0.

 Int_t i=GetIndex(name);
 return GetVar(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSigma(Int_t i,Int_t model) const
{
// Provide the rms-deviation (model=0) or standard deviation (model=1) of the i-th variable.
// The first variable corresponds to i=1.
//
// The default value is model=0.

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetSigma* Error : Invalid index " << i << endl;
  return 0;
 }

 Double_t val=fRMSdev[i-1];
 if (model) val=fSigma[i-1];

 return val;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSigma(TString name,Int_t model) const
{
// Provide the rms-deviation (model=0) or standard deviation (model=1)
// of the variable with the specified name.
//
// The default value is model=0.

 Int_t i=GetIndex(name);
 return GetSigma(i,model);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCov(Int_t i,Int_t j) const
{
// Provide the covariance between variables i and j.
// The index of the first variable is 1.

 if (i<1 || i>fDim || j<1 || j>fDim)
 {
  cout << " *NcSample::GetCov* Error : Invalid index encountered i=" << i << " j=" << j << endl;
  return 0;
 }

 return fCov[i-1][j-1];
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCov(TString nameA, TString nameB) const
{
// Provide the covariance between the variables with the specified names.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return GetCov(i,j);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCor(Int_t i,Int_t j) const
{
// Provide the correlation coefficient between variables i and j
// The index of the first variable is 1.

 if (i<1 || i>fDim || j<1 || j>fDim)
 {
  cout << " *NcSample::GetCor* Error : Invalid index encountered i=" << i << " j=" << j << endl;
  return 0;
 }

  return fCor[i-1][j-1];
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCor(TString nameA, TString nameB) const
{
// Provide the correlation coefficient the variables with the specified names.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return GetCor(i,j);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Data(Int_t i,Int_t j)
{
// Listing of statistics of all or selected variables.
//
// Meaning of the input arguments (i,j) :
// --------------------------------------
// (0,0) : List all statistics and correlations of all variables 
// (i,0) : List statistics of the i-th variable (first is i=1)
// (i,j) : List correlation statistics of the variables i and j
//
// Note : The input (0,k) provides the same listing as (k,0).
//
// The default values are i=0 and j=0.

 const char* name=GetName();
 const char* title=GetTitle();

 cout << " *" << ClassName() << "::Data*";
 if (i<0 || i>fDim || j<0 || j>fDim)
 {
  cout << " Inconsistent input i=" << i << " and j=" << j << " for dimension " << fDim << endl;
  return;
 }
 if (!i && !j) cout << " Statistics and correlations of all variables";
 if (i && !j)  cout << " Statistics of variable " << i;
 if (!i && j)  cout << " Statistics of variable " << j;
 if (i && j)   cout << " Correlation statistics of the variables " << i << " and " << j;
 if (strlen(name))  cout << " for sample Name : " << name;
 if (strlen(title)) cout << " Title : " << title;
 cout << endl;

 if (!fN)
 {
  cout << " No data has been entered." << endl;
  return;
 }

 // Statistics and correlations of all variables
 if (!i && !j)
 {
  // Statistics of individual variables
  for (Int_t iv=1; iv<=fDim; iv++)
  {
   List(iv);
  }

  if (fDim<2) return;

  // Covariance(s) and correlation coefficient(s)
  for (Int_t iv=1; iv<=fDim; iv++)
  {
   for (Int_t jv=iv+1;jv<=fDim; jv++)
   {
    List(iv,jv);
   }
  }
 }

 // Statistics of the i-th variable
 if (i && !j) List(i);

 // Statistics of the j-th variable
 if (!i && j) List(j);

 // Correlation statistics of the variables i and j
 if (i && j) List(i,j);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Data(TString nameA,TString nameB)
{
// Listing of statistics of all or selected variables.
//
// Input arguments :
// -----------------
// nameA : Name of a variable to be selected
// nameB : Name of a variable to be selected
//
// Meaning of the input arguments (nameA,nameB) :
// ----------------------------------------------
// ("-","-") : List all statistics and correlations of all variables 
// ("R","-") : List statistics of the variable with the name "R"
// ("Q","M") : List correlation statistics of the variables with names "Q" and "M"
//
// Notes :
// -------
// 1) The specification of names is case sensitive.
// 2) The input ("-","Time") provides the same listing as ("Time","-").
// 3) In case no match is found for a specified name, it is treated as "-".
// 4) Invokation of Data() corresponds to Data("-","-").
//
// The default value is nameB="-".

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Data(i,j);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::ListOrdered(Int_t mode,Int_t i)
{
// Provide a listing of all stored entries according to the specified ordering
// mode of the i-th variable (first is i=1). 
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of "i" is irrelvant. 

 if (!fStore)
 {
  cout << " *NcSample::ListOrdered* Error : Storage of data entries was not activated." << endl;
  return;
 }

 if (fN<=0)
 {
  cout << " *NcSample::ListOrdered* No entries were stored." << endl;
  return;
 }

 if (fDim<1)
 {
  cout << " *NcSample::ListOrdered* Error : Dimension less than 1." << endl;
  return;
 }

 if (mode && (i<1 || i>fDim))
 {
  cout << " *NcSample::ListOrdered* Error : Invalid argument i=" << i << endl;
  return;
 }

 Order(mode,i);

 TString s=fNames[i-1];

 cout << " *NcSample::ListOrdered* Listing of the stored entries in";
 if (!mode) cout << " order of original entering." << endl;
 if (mode<0) cout << " decreasing order of variable : " << i << " (" << s << ")" << endl;
 if (mode>0) cout << " increasing order of variable : " << i << " (" << s << ")" << endl;
 if (mode) cout << " The number between brackets indicates the original data entry number." << endl; 

 Int_t index=0;
 for (Int_t j=0; j<fN; j++)
 {
  index=fIndices->At(j);
  
  if (index<0 || index>=fN) continue;

  cout << " Index : " << (j+1);
  if (mode) cout << " (" << (index+1) << ") ";
  cout << " " << fNames[0] << "=" << fX->At(index);
  if (fDim>1) cout << " " << fNames[1] << "=" << fY->At(index);
  if (fDim>2) cout << " " << fNames[2] << "=" << fZ->At(index);
  if (fDim>3) cout << " " << fNames[3] << "=" << fT->At(index);
  cout << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::ListOrdered(Int_t mode,TString name)
{
// Provide a listing of all stored entries according to the specified ordering
// mode of the variable with the specified name.
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the provided name is irrelvant.

 Int_t i=GetIndex(name);
 ListOrdered(mode,i);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::List(Int_t i)
{
// Internal member function to list the statistics of i-th variable
// The index of the first variable is 1.

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::List(i)* Error : Invalid index " << i << endl;
 }
 else
 {
  cout << " Variable " << fNames[i-1] << " : N=" << fN;
  cout << " Sum=" << fSum[i-1] << " Mean=" << fMean[i-1];
  cout << " Deviation(rms)=" << fRMSdev[i-1] << " Sigma=" << fSigma[i-1];
  if (!fRemove || fStore)
  {
   cout << endl;
   cout << "          Minimum=" << GetMinimum(i) << " Maximum=" << GetMaximum(i);
  }
  if (fStore)
  {
   cout << " Median=" << GetMedian(i);
   cout << " Spread(w.r.t. median)=" << GetSpread(i,0) << " Spread(w.r.t. mean)=" << GetSpread(i,1);
  }
  cout << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::List(Int_t i,Int_t j) const
{
// Internal member function to list the covariance and correlation coefficient between variables i and j.
// The index of the first variable is 1.

 if (i<1 || i>fDim || j<1 || j>fDim)
 {
  cout << " *NcSample::List(i,j)* Error : Invalid index encountered i=" << i << " j=" << j << endl;
 }
 else
 {
  cout << " " << fNames[i-1] << "-" << fNames[j-1];
  cout << " Correlation(coefficient)=" << fCor[i-1][j-1] << " Covariance=" << fCov[i-1][j-1] << endl;
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
// Provide the value of the i-th variable (first is i=1) that marks the
// quantile with fraction "f" of the sample.
// By definition "f" belongs to the interval [0,1] where f=0.5 indicates
// the median of the specified variable.
//
// For this functionality the storage mode has to be activated.
//
// In the case of incompatible data the value 0 is returned.
//
// Note : For large datasets it is more efficient to determine the quantile
//        via the specification of a histogram. 
//        See the other GetQuantile memberfunction for details.

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetQuantile* Error : Invalid index " << i << endl;
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
  if (i==4) quantile=fT->At(0);
  return quantile;
 }

 if (f==0) return GetMinimum(i);
 if (f==1) return GetMaximum(i);

 // Order the values of the i-th variable in increasing order
 Order(1,i);

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
Double_t NcSample::GetQuantile(Double_t f,TString name)
{
// Provide the value of the variable with the specified name that marks the
// quantile with fraction "f" of the sample.
// By definition "f" belongs to the interval [0,1] where f=0.5 indicates
// the median of the specified variable.
//
// For this functionality the storage mode has to be activated.
//
// In the case of incompatible data the value 0 is returned.
//
// Note : For large datasets it is more efficient to determine the quantile
//        via the specification of a histogram. 
//        See the other GetQuantile memberfunction for details.

 Int_t i=GetIndex(name);
 return GetQuantile(f,i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMedian(Int_t i)
{
// Provide the median of the i-th variable (first is i=1).
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
Double_t NcSample::GetMedian(TString name)
{
// Provide the median of the variable with the specified name.
// For this functionality the storage mode has to be activated.
//
// In the case of incompatible data the value 0 is returned.
//
// The median is determined by invokation of GetQuantile(0.5,name).
// Please refer to the documentation of the corresponding GetQuantile()
// memberfunction for further details.
//
// Note : For large datasets it is more efficient to determine the median
//        via the specification of a histogram. 
//        See the other GetMedian memberfunction for details.

 Double_t median=GetQuantile(0.5,name);
 return median;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSpread(Int_t i,Int_t model,Double_t vref)
{
// Provide the spread w.r.t. some reference value of the i-th variable (first is i=1).
// The spread is defined as the average of |median-val(i)| when model=0,
// the average of |mean-val(i)| when model=1, or the average of |vref-val(i)| when model=2.
//
// In case model=0 or model=1, the value of "vref" is irrelevant.
// 
// For this functionality the storage mode has to be activated.
//
// Note : For large datasets it is more efficient to determine the spread
//        via the specification of a histogram. 
//        See the other GetSpread memberfunction for details.
// 
// The default values are model=0 and vref=0 for backward compatibility.
//
// In case of inconsistent data, the value -1 is returned. 

 if (model<0 || model>2)
 {
  cout << " *NcSample::GetSpread* Error : Unsupported parameter model=" << model << endl;
  return -1;
 }

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetSpread* Error : Invalid index " << i << endl;
  return -1;
 }

 if (!fStore)
 {
  cout << " *NcSample::GetSpread* Error : Storage of data entries was not activated." << endl;
  return -1;
 }

 if (fN<=1) return -1;

 Double_t central=GetMedian(i);
 if (model==1) central=GetMean(i);
 if (model==2) central=vref;

 Double_t spread=0;
 for (Int_t j=0; j<fN; j++)
 {
  spread+=fabs(central-(fArr->At(j)));
 }

 spread=spread/float(fN);

 return spread;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSpread(TString name,Int_t model,Double_t vref)
{
// Provide the spread w.r.t. some reference value of the variable with the specified name.
// The spread is defined as the average of |median-val(i)| when model=0,
// the average of |mean-val(i)| when model=1, or the average of |vref-val(i)| when model=2.
//
// In case model=0 or model=1, the value of "vref" is irrelevant.
// 
// For this functionality the storage mode has to be activated.
//
// Note : For large datasets it is more efficient to determine the spread
//        via the specification of a histogram. 
//        See the other GetSpread memberfunction for details.
// 
// The default values are model=0 and vref=0 for backward compatibility.
//
// In case of inconsistent data, the value -1 is returned. 

 Int_t i=GetIndex(name);
 return GetSpread(i,model,vref);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMinimum(Int_t i) const
{
// Provide the minimum value of the i-th variable (first is i=1).
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetMinimum* Error : Invalid index " << i << endl;
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
Double_t NcSample::GetMinimum(TString name) const
{
// Provide the minimum value of the variable with the specified name.
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.

 Int_t i=GetIndex(name);
 return GetMinimum(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMaximum(Int_t i) const
{
// Provide the maximum value of the i-th variable (first is i=1).
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetMaximum* Error : Invalid index " << i << endl;
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
Double_t NcSample::GetMaximum(TString name) const
{
// Provide the maximum value of the variable with the specified name.
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.

 Int_t i=GetIndex(name);
 return GetMaximum(i);
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

 Double_t xlow=0;
 Double_t xup=0;
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
  // Take the average of two quantiles closely around f
  // This will enhance the precision for low statistics
  Double_t q[2];
  Double_t p[2];
  p[0]=f-0.01;
  p[1]=f+0.01;
  if (p[0]<0) p[0]=0;
  if (p[0]>1) p[0]=1;
  histo->ComputeIntegral();
  Int_t nq=histo->GetQuantiles(2,q,p);

  if (!nq) return 0;

  xlow=q[0];
  xup=q[1];
  if (mode==1)
  {
   Int_t mbin=histo->FindBin(q[0]);
   xlow=histo->GetBinCenter(mbin);
   mbin=histo->FindBin(q[1]);
   xup=histo->GetBinCenter(mbin);
  }
 }

 quantile=(xlow+xup)/2.;

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
Double_t NcSample::GetSpread(TH1* histo,Int_t mode,Int_t model,Double_t vref) const
{
// Provide the spread w.r.t. some X or Y reference value for the specified 1D histogram.
// The spread is defined as the average of |median-val(i)| when model=0,
// the average of |mean-val(i)| when model=1, or the average of |vref-val(i)| when model=2.
//
// In case model=0 or model=1, the value of "vref" is irrelevant.
//
// For this functionality it is not needed to activate the storage mode.
//
// In the case of incompatible data the value -1 is returned.
//
// In case of X-spread w.r.t. the median, this facility uses TH1::GetQuantiles to determine
// the X-median, which provides a median value which in general is different
// from any of the central bin X values. The user may force the used X-median
// to be the corresponding central bin X value via the "mode" input argument.
//
// mode = 0 ==> The pure TH1::GetQuantiles X-median value is used when model=0
//        1 ==> The corresponding central bin X value is used as X-median when model=0
//        2 ==> The spread in Y-values will be provided
//
// By default mode=0 will be used to agree with standard ROOT processing.
//
// The default values are mode=0, model=0 and vref=0 for backward compatibility.

 if (model<0 || model>2)
 {
  cout << " *NcSample::GetSpread* Error : Unsupported parameter model=" << model << endl;
  return -1;
 }

 if (!histo) return -1;

 Int_t nbins=histo->GetNbinsX();

 if (nbins<1) return -1;

 Double_t spread=0;

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
  spread=temp.GetSpread(1,model,vref);
 }
 else // Spread in X values
 {
  Double_t central=GetMedian(histo,mode);
  if (model==1) central=histo->GetMean();
  if (model==2) central=vref;
  Double_t x=0;
  Double_t y=0;
  Double_t ysum=0;
  for (Int_t jbin=1; jbin<=nbins; jbin++)
  {
   x=histo->GetBinCenter(jbin);
   y=histo->GetBinContent(jbin);
   if (y>0)
   {
    spread+=fabs(x-central)*y;
    ysum+=y;
   }
  }
  if (ysum>0) spread=spread/ysum;
 }

 return spread;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetEntry(Int_t i,Int_t j,Int_t mode,Int_t k)
{
// Access the data entry at index "i" and provide the value of the j-th variable,
// after ordering w.r.t. the k-th variable.
// The first entry is indicated by the index i=1 and the first variable is j=1.
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// This facility is only available if the storage mode has been activated.
//
// Note : If mode=0 the value of "k" is irrelevant.
//
// The default values are mode=0 and k=0.

 if (!fStore)
 {
  cout << " *NcSample::GetEntry* Error : Storage mode not activated." << endl;
  return 0;
 }

 if (i<1 || i>fN)
 {
  cout << " *NcSample::GetEntry* Error : Invalid index number i=" << i << endl;
  return 0;
 }

 if (j<1 || j>fDim)
 {
  cout << " *NcSample::GetEntry* Error : Invalid variable number j=" << j << endl;
  return 0;
 }

 if (mode && (k<1 || k>fDim))
 {
  cout << " *NcSample::GetEntry* Error : Invalid argument k=" << k << endl;
  return 0;
 }

 Double_t value=0;

 // Determine the entry index in the main arrays.
 Int_t index=i-1;

 if (mode)
 {
  Order(mode,k);
  index=fIndices->At(i-1);
 }

 if (j==1) value=fX->At(index);
 if (j==2) value=fY->At(index);
 if (j==3) value=fZ->At(index);
 if (j==4) value=fT->At(index);

 return value;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetEntry(Int_t i,TString nameA,Int_t mode,TString nameB)
{
// Access the data entry at index "i" and provide the value of the variable 
// specified with nameA, after ordering w.r.t. the variable specified with nameB.
// The first entry is indicated by the index i=1.
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// This facility is only available if the storage mode has been activated.
//
// Note : If mode=0 the value of nameB is irrelevant.
//
// The default values are mode=0 and nameB="-".

 Int_t j=GetIndex(nameA);
 Int_t k=GetIndex(nameB);
 return GetEntry(i,j,mode,k);
}
///////////////////////////////////////////////////////////////////////////
TH1D NcSample::Get1DHistogram(Int_t i,Int_t j,Bool_t sumw2,Int_t nbx)
{
// Provide a TH1D histogram with the values of variable i.
// If j>0 the corresponding value of variable j will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input argument "nbx" defines the number of bins on the X-axis.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are j=0 , sumw2=kFALSE and nbx=100.

 TString s="1D Histogram for NcSample ";
 s+=GetName();
 s+=";Variable ";
 s+=fNames[i-1];
 s+=";Counts";
 if (j>0)
 {
  s+=" weighted with Variable ";
  s+=fNames[j-1];
 }

 Double_t xlow=GetMinimum(i);
 Double_t xup=GetMaximum(i);

 // Extension to include the maximum value
 xup+=0.001*fabs(xup);

 TH1D hist("",s.Data(),nbx,xlow,xup);
 hist.Sumw2(sumw2);

 if (!fStore)
 {
  cout << " *NcSample::Get1DHistogram* Error : Storage mode has not been activated." << endl;
  return hist;
 }

 if (i<1 || i>fDim || j>fDim)
 {
  cout << " *NcSample::Get1DHistogram* Error : Invalid index encountered i=" << " j=" << j << endl;
  return hist;
 }

 Double_t x=0;
 Double_t y=0;
 for (Int_t ip=1; ip<=fN; ip++)
 {
  x=GetEntry(ip,i);
  if (j>0)
  {
   y=GetEntry(ip,j);
   hist.Fill(x,y);
  }
  else
  {
   hist.Fill(x);
  }
 }

 return hist;
}
///////////////////////////////////////////////////////////////////////////
TH1D NcSample::Get1DHistogram(TString nameA,TString nameB,Bool_t sumw2,Int_t nbx)
{
// Provide a TH1D histogram for the variable specified with nameA.
// If nameB is valid, the corresponding value of that variable will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input argument "nbx" defines the number of bins on the X-axis.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are nameB="-", sumw2=kFALSE and nbx=100.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return Get1DHistogram(i,j,sumw2,nbx);
}
///////////////////////////////////////////////////////////////////////////
TH2D NcSample::Get2DHistogram(Int_t i,Int_t j,Int_t k,Bool_t sumw2,Int_t nbx,Int_t nby)
{
// Provide a TH2D histogram for the values of variables i and j.
// If k>0 the corresponding value of variable k will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input arguments "nbx" and "nby" define the number of bins on the X-axis
// and Y-axis, respectively.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are k=0, sumw2=kFALSE, nbx=100 and nby=100.

 TString s="2D Histogram for NcSample ";
 s+=GetName();
 s+=";Variable ";
 s+=fNames[i-1];
 s+=";Variable ";
 s+=fNames[j-1];
 s+=";Counts";
 if (k>0)
 {
  s+=" weighted with Variable ";
  s+=fNames[k-1];
 }

 Double_t xlow=GetMinimum(i);
 Double_t xup=GetMaximum(i);
 Double_t ylow=GetMinimum(j);
 Double_t yup=GetMaximum(j);

 // Extension to include the maximum values
 xup+=0.001*fabs(xup);
 yup+=0.001*fabs(yup);

 TH2D hist("",s.Data(),nbx,xlow,xup,nby,ylow,yup);
 hist.Sumw2(sumw2);
 hist.SetMarkerStyle(20);
 hist.SetMarkerSize(1);

 if (!fStore)
 {
  cout << " *NcSample::Get2DHistogram* Error : Storage mode has not been activated." << endl;
  return hist;
 }

 if (i<1 || i>fDim || j<1 || j>fDim || k>fDim)
 {
  cout << " *NcSample::Get2DHistogram* Error : Invalid index encountered i=" << " j=" << j << " k=" << k << endl;
  return hist;
 }

 Double_t x=0;
 Double_t y=0;
 Double_t z=0;
 for (Int_t ip=1; ip<=fN; ip++)
 {
  x=GetEntry(ip,i);
  y=GetEntry(ip,j);
  if (k>0)
  {
   z=GetEntry(ip,k);
   hist.Fill(x,y,z);
  }
  else
  {
   hist.Fill(x,y);
  }
 }

 return hist;
}
///////////////////////////////////////////////////////////////////////////
TH2D NcSample::Get2DHistogram(TString nameA,TString nameB,TString nameC,Bool_t sumw2,Int_t nbx,Int_t nby)
{
// Provide a TH2D histogram for the variables specified with nameA and nameB.
// If nameC is valid, the corresponding value of that variable will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input arguments "nbx" and "nby" define the number of bins on the X-axis
// and Y-axis, respectively.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are nameC="-", sumw2=kFALSE, nbx=100 and nby=100.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 return Get2DHistogram(i,j,k,sumw2,nbx,nby);
}
///////////////////////////////////////////////////////////////////////////
TH3D NcSample::Get3DHistogram(Int_t i,Int_t j,Int_t k,Int_t m,Bool_t sumw2,Int_t nbx,Int_t nby,Int_t nbz)
{
// Provide a TH3D histogram for the values of variables i, j and k.
// If m>0 the corresponding value of variable m will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input arguments "nbx", "nby" and "nbz" define the number of bins on the X-axis,
// Y-axis and Z-axis, respectively.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are m=0, sumw2=kFALSE, nbx=100, nby=100 and nbz=100.

 TString s="3D Histogram for NcSample ";
 s+=GetName();
 if (m>0)
 {
  s+=" with Variable ";
  s+=fNames[m-1];
  s+=" as weight";
 }
 s+=";Variable ";
 s+=fNames[i-1];
 s+=";Variable ";
 s+=fNames[j-1];
 s+=";Variable ";
 s+=fNames[k-1];

 Double_t xlow=GetMinimum(i);
 Double_t xup=GetMaximum(i);
 Double_t ylow=GetMinimum(j);
 Double_t yup=GetMaximum(j);
 Double_t zlow=GetMinimum(k);
 Double_t zup=GetMaximum(k);

 // Extension to include the maximum values
 xup+=0.001*fabs(xup);
 yup+=0.001*fabs(yup);
 zup+=0.001*fabs(zup);

 TH3D hist("",s.Data(),nbx,xlow,xup,nby,ylow,yup,nbz,zlow,zup);
 hist.Sumw2(sumw2);
 hist.SetMarkerStyle(20);
 hist.SetMarkerSize(1);

 if (!fStore)
 {
  cout << " *NcSample::Get3DHistogram* Error : Storage mode has not been activated." << endl;
  return hist;
 }

 if (i<1 || i>fDim || j<1 || j>fDim || k<1 || k>fDim || m>fDim)
 {
  cout << " *NcSample::Get2DHistogram* Error : Invalid index encountered i=" << " j=" << j << " k=" << k << " m=" << m << endl;
  return hist;
 }

 Double_t x=0;
 Double_t y=0;
 Double_t z=0;
 Double_t t=0;
 for (Int_t ip=1; ip<=fN; ip++)
 {
  x=GetEntry(ip,i);
  y=GetEntry(ip,j);
  z=GetEntry(ip,k);
  if (m>0)
  {
   t=GetEntry(ip,m);
   hist.Fill(x,y,z,t);
  }
  else
  {
   hist.Fill(x,y,z);
  }
 }

 return hist;
}
///////////////////////////////////////////////////////////////////////////
TH3D NcSample::Get3DHistogram(TString nameA,TString nameB,TString nameC,TString nameD,Bool_t sumw2,Int_t nbx,Int_t nby,Int_t nbz)
{
// Provide a TH3D histogram for the variables specified with nameA, nameB and nameC.
// If nameD is valid, the corresponding value of that variable will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input arguments "nbx", "nby" and "nbz" define the number of bins on the X-axis,
// Y-axis and Z-axis, respectively.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are nameD="-", sumw2=kFALSE, nbx=100, nby=100 and nbz=100.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 Int_t m=GetIndex(nameD);
 return Get3DHistogram(i,j,k,m,sumw2,nbx,nby,nbz);
}
///////////////////////////////////////////////////////////////////////////
TGraph NcSample::GetGraph(Int_t i,Int_t j)
{
// Provide a TGraph with : X-axis=variable i and Y-axis=variable j.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.

 TGraph gr;

 if (!fStore)
 {
  cout << " *NcSample::GetGraph* Error : Storage mode has not been activated." << endl;
  return gr;
 }

 if (i<1 || i>fDim || j<1 || j>fDim)
 {
  cout << " *NcSample::GetGraph* Error : Invalid index encountered i=" << " j=" << j << endl;
  return gr;
 }

 Double_t x=0;
 Double_t y=0;
 for (Int_t ip=0; ip<fN; ip++)
 {
  x=GetEntry(ip+1,i);
  y=GetEntry(ip+1,j);
  gr.SetPoint(ip,x,y);
 }

 TString s="TGraph for NcSample ";
 s+=GetName();
 s+=" : X-axis=variable ";
 s+=fNames[i-1];
 s+="  Y-axis=variable ";
 s+=fNames[j-1];
 gr.SetTitle(s.Data());

 gr.SetMarkerStyle(20);
 gr.SetMarkerSize(1);
 gr.SetDrawOption("AP");

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph NcSample::GetGraph(TString nameA,TString nameB)
{
// Provide a TGraph with : X-axis=variable with nameA and Y-axis=variable with nameB.
//
// Note : This facility is only available if the storage mode has been activated.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return GetGraph(i,j);
}
///////////////////////////////////////////////////////////////////////////
TGraphTime* NcSample::GetGraph(Int_t i,Int_t j,Int_t mode,Int_t k,Bool_t smp)
{
// Provide a pointer to a TGraphTime with : X-axis=variable i and Y-axis=variable j.
// The first variable has index 1.
//
// Note :
// ------
// At every invokation of this member function, the existing TGraphTime will be deleted.
// In case you want to keep the produced graph(s) fur further analysis, you have to make
// a local copy (e.g. via the Clone() facility) of the produced TGraphTime object(s).
//
// Every data entry is considered to occur at a step in time, and the TGraphTime allows
// to display an animated time development of the sampling of the specified 2D data points.
// This can be achieved by setting the time step delay via TGraphTime::SetSleepTime()
// followed by TGraphTime::Draw().
//
// The time development of the various entries can be displayed in cumulative sampling mode
// (smp=kTRUE), which reflects an animation of the building up of the total data sample.
// When smp=kFALSE, only every single entry is displayed, which reflects an animation
// of a single point moving in time in 2 dimensions.
// The animations can be saved in an animated GIF file via TGraphTime::SaveAnimatedGif()
// after TGraphTime::Draw() has been invoked.
//
// The time flow can be controlled by ordering the various entries according to
// the specified k-th variable and the input argument "mode".
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// Note : If mode=0 the value of "k" is irrelevant.
//
// This facility is only available if the storage mode has been activated.
//
// In case of inconsistent data, the value 0 is returned.
//
// The default value is smp=kTRUE.

 if (!fStore)
 {
  cout << " *NcSample::GetGraph* Error : Storage mode has not been activated." << endl;
  return 0;
 }

 if (fN<1)
 {
  cout << " *NcSample::GetGraph* Error : No data entries are stored." << endl;
  return 0;
 }

 if (i<1 || i>fDim || j<1 || j>fDim || (mode && (k<1 || k>fDim)))
 {
  cout << " *NcSample::GetGraph* Error : Invalid index encountered i=" << " j=" << j << " k=" << k << endl;
  return 0;
 }

 Double_t xlow=GetMinimum(i);
 Double_t xup=GetMaximum(i);
 Double_t ylow=GetMinimum(j);
 Double_t yup=GetMaximum(j);

 // Extensions to display the points well within the frame
 xlow-=0.1*fabs(xlow);
 xup+=0.1*fabs(xup);
 ylow-=0.1*fabs(ylow);
 yup+=0.1*fabs(yup);

 if (fGraphT)
 {
  delete fGraphT;
  fGraphT=0;
 }

 fGraphT=new TGraphTime(fN,xlow,ylow,xup,yup);

 Double_t x=0;
 Double_t y=0;
 TMarker* m=0;
 Int_t jpstart=0;
 for (Int_t istep=0; istep<fN; istep++)
 {
  if (!smp) jpstart=istep;
  for (Int_t jp=jpstart; jp<=istep; jp++)
  {
   x=GetEntry(jp+1,i,mode,k);
   y=GetEntry(jp+1,j,mode,k);
   m=new TMarker(x,y,20);
   m->SetMarkerSize(1);
   fGraphT->Add(m,istep);
  }  
 }

 TString s="TGraphTime for NcSample ";
 s+=GetName();
 s+=";Variable ";
 s+=fNames[i-1];
 s+=";Variable ";
 s+=fNames[j-1];
 fGraphT->SetNameTitle("",s.Data());

 return fGraphT;
}
///////////////////////////////////////////////////////////////////////////
TGraphTime* NcSample::GetGraph(TString nameA,TString nameB,Int_t mode,TString nameC,Bool_t smp)
{
// Provide a pointer to a TGraphTime with : X-axis=variable nameA and Y-axis=variable nameB.
//
// Note :
// ------
// At every invokation of this member function, the existing TGraphTime will be deleted.
// In case you want to keep the produced graph(s) fur further analysis, you have to make
// a local copy (e.g. via the Clone() facility) of the produced TGraphTime object(s).
//
// Every data entry is considered to occur at a step in time, and the TGraphTime allows
// to display an animated time development of the sampling of the specified 2D data points.
// This can be achieved by setting the time step delay via TGraphTime::SetSleepTime()
// followed by TGraphTime::Draw().
//
// The time development of the various entries can be displayed in cumulative sampling mode
// (smp=kTRUE), which reflects an animation of the building up of the total data sample.
// When smp=kFALSE, only every single entry is displayed, which reflects an animation
// of a single point moving in time in 2 dimensions.
// The animations can be saved in an animated GIF file via TGraphTime::SaveAnimatedGif()
// after TGraphTime::Draw() has been invoked.
//
// The time flow can be controlled by ordering the various entries according to
// the variable specified with nameC and the input argument "mode".
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// Note : If mode=0 the value of nameC is irrelevant.
//
// This facility is only available if the storage mode has been activated.
//
// In case of inconsistent data, the value 0 is returned.
//
// The default value is smp=kTRUE.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 return GetGraph(i,j,mode,k,smp);
}
///////////////////////////////////////////////////////////////////////////
TGraph2D NcSample::GetGraph(Int_t i,Int_t j,Int_t k)
{
// Provide a TGraph with : X-axis=variable i, Y-axis=variable j and Z-axis=variable k.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.

 TGraph2D gr;

 if (!fStore)
 {
  cout << " *NcSample::GetGraph* Error : Storage mode has not been activated." << endl;
  return gr;
 }

 if (fN<1)
 {
  cout << " *NcSample::GetGraph* Error : No data entries are stored." << endl;
  return gr;
 }

 if (i<1 || i>fDim || j<1 || j>fDim || k<1 || k>fDim)
 {
  cout << " *NcSample::GetGraph* Error : Invalid index encountered i=" << " j=" << j << " k=" << k << endl;
  return gr;
 }

 Double_t x=0;
 Double_t y=0;
 Double_t z=0;
 for (Int_t ip=0; ip<fN; ip++)
 {
  x=GetEntry(ip+1,i);
  y=GetEntry(ip+1,j);
  z=GetEntry(ip+1,k);
  gr.SetPoint(ip,x,y,z);
 }

 TString s="TGraph2D for NcSample ";
 s+=GetName();
 s+=" : X-axis=variable ";
 s+=fNames[i-1];
 s+="  Y-axis=variable ";
 s+=fNames[j-1];
 s+="  Z-axis=variable ";
 s+=fNames[k-1];
 gr.SetTitle(s.Data());

 gr.SetMarkerStyle(20);
 gr.SetMarkerSize(1);
 gr.SetDrawOption("P");

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph2D NcSample::GetGraph(TString nameA,TString nameB,TString nameC)
{
// Provide a TGraph with : X-axis=variable nameA, Y-axis=variable nameB and Z-axis=variable nameC.
//
// Note : This facility is only available if the storage mode has been activated.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 return GetGraph(i,j,k);
}
///////////////////////////////////////////////////////////////////////////
TGraphQQ NcSample::GetQQplot(Int_t i,Int_t j,TF1* f)
{
// Provide a QQ-plot (TGraphQQ) for the values of the variables i and j.
// The first variable has index 1.
// In case the function "f" is specified, it will replace the role of variable j.
// When the function "f" is specified, the value of "j" is irrelevant.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default is f=0.

 TGraphQQ gr;

 if (!f && (j<1 || j>fDim)) return gr;

 if (!fStore)
 {
  cout << " *NcSample::GetQQplot* Error : Storage mode has not been activated." << endl;
  return gr;
 }

 if (fN<1)
 {
  cout << " *NcSample::GetQQplot* Error : No data entries are stored." << endl;
  return gr;
 }

 if (i<1 || i>fDim || (!f && (j<1 || j>fDim)))
 {
  cout << " *NcSample::GetQQplot* Error : Invalid index encountered i=" << " j=" << j << endl;
  return gr;
 }

 Double_t* arri=0;
 if (i==1) arri=fX->GetArray();
 if (i==2) arri=fY->GetArray();
 if (i==3) arri=fZ->GetArray();
 if (i==4) arri=fT->GetArray();

 if (!f)
 {
  Double_t* arrj=0;
  if (j==1) arrj=fX->GetArray();
  if (j==2) arrj=fY->GetArray();
  if (j==3) arrj=fZ->GetArray();
  if (j==4) arrj=fT->GetArray();

  if (arri && arrj) gr=TGraphQQ(fN,arri,fN,arrj);
 }
 else
 {
  if (arri) gr=TGraphQQ(fN,arri,f);
 }

 TString s="QQ-plot (TGraphQQ) for NcSample ";
 s+=GetName();
 if (f)
 {
  s+=" of Variable ";
  s+=fNames[i-1];
  s+=" versus Function ";
  s+=f->GetExpFormula("p");
 }
 else
 {
  s+=";Variable ";
  s+=fNames[i-1];
  s+=";Variable ";
  s+=fNames[j-1];
 }

 gr.SetTitle(s.Data());

 gr.SetMarkerStyle(20);
 gr.SetMarkerSize(1);

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraphQQ NcSample::GetQQplot(TString nameA,TString nameB,TF1* f)
{
// Provide a QQ-plot (TGraphQQ) for the the variables specified by nameA and nameB.
// In case the function "f" is specified, it will replace the role of variable nameB.
// When the function "f" is specified, the value of nameB is irrelevant.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default is f=0.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return GetQQplot(i,j,f);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Load(TGraph* g,Int_t clr)
{
// Load the data points of a TGraph object as 2-dimensional (x,y) data.
//
// Input arguments :
// -----------------
// g   : Pointer to the TGraph object.
// clr : Flag to denote to first clear (1) the data storage or not (0).
//
// The default value is clr=1.
 
 if (!clr && fDim!=2)
 {
  cout << " *NcSample::Load* Error : Dimension " << fDim << " does not match a TGraph." << endl;
  return;
 }

 if (clr) Reset();
 
 if (!g) Reset();

 Int_t npoints=g->GetN();
 Double_t* x=g->GetX();
 Double_t* y=g->GetY();

 if (!x || !y || npoints<1) return;

 for (Int_t i=0; i<npoints; i++)
 {
  Enter(x[i],y[i]);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Load(TGraph2D* g,Int_t clr)
{
// Load the data points of a TGraph2D object as 3-dimensional (x,y,z) data.
//
// Input arguments :
// -----------------
// g   : Pointer to the TGraph2D object.
// clr : Flag to denote to first clear (1) the data storage or not (0).
//
// The default value is clr=1.
 
 if (!clr && fDim!=3)
 {
  cout << " *NcSample::Load* Error : Dimension " << fDim << " does not match a TGraph2D." << endl;
  return;
 }

 if (clr) Reset();
 
 if (!g) Reset();

 Int_t npoints=g->GetN();
 Double_t* x=g->GetX();
 Double_t* y=g->GetY();
 Double_t* z=g->GetZ();

 if (!x || !y || !z || npoints<1) return;

 for (Int_t i=0; i<npoints; i++)
 {
  Enter(x[i],y[i],z[i]);
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSNR(Int_t i,Int_t mode,Bool_t dB) const
{
// Provide the Signal to Noise Ratio (SNR) of the i-th variable.
// The first variable corresponds to i=1.
//
// The definition used here is SNR=(signal power)/(noise power).
// This implies that when the values of the i-th variable represent amplitudes,
// the squares of the (rms) values should be used.
// This can be specified via the input argument "mode" where abs(mode)=2 will
// invoke a conversion of amplitudes into power, whereas abs(mode)=1 will use
// the values of the i-th variable "as is", i.e. reflecting a direct power measurement.
// Furthermore, the input argument "mode" also provides a selection to use
// the variance of the sample or the standard deviation of the underlying parent distribution,
// as indicated below.  
//
// Input arguments :
// -----------------
// mode :  2 --> SNR=(mean*mean)/variance
//         1 --> SNR=abs(mean/rms-deviation)
//        -2 --> SNR=(mean*mean)/(sigma*sigma)
//        -1 --> SNR=abs(mean/sigma)
// dB   : kFALSE Provide the SNR as the above straight ratio
//        kTRUE  Provide the SNR in Decibel
//
// The default values are model=2 and dB=kTRUE.
//
// In case of inconsistent data, the value -9999 is returned.
//
// Derivation of the applied formulas :
// ------------------------------------
// For each recorded sampling k we assume a signal contribution Sk and a noise contribution Nk.
//
// This leads to the expression :
//
//   SNR=(average signal power)/(average noise power)=<Sk^2>/<Nk^2>  (1)
//
// Using variance=<x^2>-<x>^2 we can express the above formula in terms of the
// signal variance (Svar), signal mean (Smu), noise variance (Nvar) and noise mean (Nmu) as follows :
//
//   SNR=(Svar+Smu^2)/(Nvar+Nmu^2)  (2)  
//
// Furthermore, the mean of the recorded samplings Mu is given by : Mu=<Sk+Nk>=<Sk>+<Nk>=Smu+Nmu  (3)
//
// In most cases we will have <Nk>=Nmu=0, which implies that Mu=Smu  (4)
//
// In other words : The mean of the observed samplings is equal to the signal mean,
// which implies that we can determine Smu directly from our data.
//
// Using Eq.(4) and Nmu=0 in Eq.(2) leads us to : SNR=(Svar+Mu^2)/Nvar  (5)
//
// In case of a (nearly) stable signal, we have approximately Svar=0,
// which implies that the total observed sampling variance S=Nvar.
//
// This leads to the final expression for SNR in terms of observed sampling amplitudes : 
//
//  SNR=Mu^2/S  (6)
//
// The above is based on the recorded sampling statistics.
//
// In terms of the standard deviation (sigma) of the underlying parent distribution,
// the above expression becomes :
//
//  SNR=Mu^2/(sigma^2)  (7)
//
// In case of a direct power measurement, the sqrt() of expression (6) or (7) should be used. 

 if (i<1 || i>fDim || !mode || abs(mode)>2) return -9999;

 Double_t snr=-1;
 Double_t mean=GetMean(i);
 Double_t var=GetVar(i);
 Double_t sigma=GetSigma(i,1);

 // Treat the values as observed amplitudes and work in dB scale
 Double_t psignal=mean*mean;
 Double_t pnoise=var;
 if (mode<0) pnoise=sigma*sigma;

 if (psignal>0 && pnoise>0)
 {
  snr=10.*(log10(psignal)-log10(pnoise));
  if (abs(mode)==1) snr=snr/2.; // Treat values as observed power
  if (!dB) // Convert to the straight ratio
  {
   snr=snr/10.;
   snr=pow(10,snr);
  }
 }
 
 return snr;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSNR(TString name,Int_t mode,Bool_t dB) const
{
// Provide the Signal to Noise Ratio (SNR) of the variable with the specified name.
//
// The definition used here is SNR=(signal power)/(noise power).
// This implies that when the values of the specified variable represent amplitudes,
// the squares of the (rms) values should be used.
// This can be specified via the input argument "mode" where abs(mode)=2 will
// invoke a conversion of amplitudes into power, whereas abs(mode)=1 will use
// the values of the specified variable "as is", i.e. reflecting a direct power measurement.
// Furthermore, the input argument "mode" also provides a selection to use
// the variance of the sample or the standard deviation of the underlying parent distribution,
// as indicated below.  
//
// Input arguments :
// -----------------
// mode :  2 --> SNR=(mean*mean)/variance
//         1 --> SNR=abs(mean/rms-deviation)
//        -2 --> SNR=(mean*mean)/(sigma*sigma)
//        -1 --> SNR=abs(mean/sigma)
// dB   : kFALSE Provide the SNR as the above straight ratio
//        kTRUE  Provide the SNR in Decibel
//
// The default values are model=2 and dB=kTRUE.
//
// In case of inconsistent data, the value -9999 is returned.
//
// Derivation of the applied formulas :
// ------------------------------------
// For each recorded sampling k we assume a signal contribution Sk and a noise contribution Nk.
//
// This leads to the expression :
//
//   SNR=(average signal power)/(average noise power)=<Sk^2>/<Nk^2>  (1)
//
// Using variance=<x^2>-<x>^2 we can express the above formula in terms of the
// signal variance (Svar), signal mean (Smu), noise variance (Nvar) and noise mean (Nmu) as follows :
//
//   SNR=(Svar+Smu^2)/(Nvar+Nmu^2)  (2)  
//
// Furthermore, the mean of the recorded samplings Mu is given by : Mu=<Sk+Nk>=<Sk>+<Nk>=Smu+Nmu  (3)
//
// In most cases we will have <Nk>=Nmu=0, which implies that Mu=Smu  (4)
//
// In other words : The mean of the observed samplings is equal to the signal mean,
// which implies that we can determine Smu directly from our data.
//
// Using Eq.(4) and Nmu=0 in Eq.(2) leads us to : SNR=(Svar+Mu^2)/Nvar  (5)
//
// In case of a (nearly) stable signal, we have approximately Svar=0,
// which implies that the total observed sampling variance S=Nvar.
//
// This leads to the final expression for SNR in terms of observed sampling amplitudes : 
//
//  SNR=Mu^2/S  (6)
//
// The above is based on the recorded sampling statistics.
//
// In terms of the standard deviation (sigma) of the underlying parent distribution,
// the above expression becomes :
//
//  SNR=Mu^2/(sigma^2)  (7)
//
// In case of a direct power measurement, the sqrt() of expression (6) or (7) should be used. 

 Int_t i=GetIndex(name);
 return GetSNR(i,mode,dB);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCV(Int_t i,Int_t model) const
{
// Provide the Coefficient of Variation (CV) of the i-th variable.
// The first variable corresponds to i=1.
//
// The definition used here is CV=abs(sigma/mean).
//
// Input arguments :
// -----------------
// model : 0 --> sigma represents the rms-deviation
//         1 --> sigma represents the standard deviation
//
// The default value is model=0.
//
// In case of inconsistent data, the value -1 is returned. 

 if (i<1 || i>fDim || model<0 || model>1) return -1;

 Double_t cv=-1;
 Double_t mean=GetMean(i);
 Double_t sigma=GetSigma(i,model);

 if (mean) cv=fabs(sigma/mean);

 return cv;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCV(TString name,Int_t model) const
{
// Provide the Coefficient of Variation (CV) of the variable with the specified name.
//
// The definition used here is CV=abs(sigma/mean).
//
// Input arguments :
// -----------------
// model : 0 --> sigma represents the rms-deviation
//         1 --> sigma represents the standard deviation
//
// The default value is model=0.
//
// In case of inconsistent data, the value -1 is returned.

 Int_t i=GetIndex(name);
 return GetCV(i,model);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Animation(Int_t i,Int_t j,Int_t mode,Int_t k,Int_t delay,TString opt)
{
// Animation of an (ordered) sampling with : X-axis=variable i and Y-axis=variable j.
// The first variable has index 1.
//
// Every data entry is considered to occur at a step in time, and this member function
// displays an animated time development of the sampling of the specified 2D data points.
// The input argument "delay" represents the pause time (in ms) between every step.
//
// The time flow can be controlled by ordering the various entries according to
// the specified k-th variable and the input argument "mode".
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// Note : If mode=0 the value of "k" is irrelevant.
//
// The input argument "opt" represents the drawing option(s) for a TGraph.
//
// The default value is opt="AP".
//
// This facility is only available if the storage mode has been activated.

 if (!fStore || fN<1 || i<1 || i>fDim || j<1 || j>fDim || (mode && (k<1 || k>fDim)) || delay<0)
 {
  cout << " *NcSample::Animation* Inconsistent input data." << endl;
  return;
 }

 if (fCanvas)
 {
  delete fCanvas;
  fCanvas=0;
 }

 if (fAnimObject)
 {
  delete fAnimObject;
  fAnimObject=0;
 }

 fCanvas=new TCanvas("fCanvas","Sampling animation");

 TGraph* gr=new TGraph();
 fAnimObject=gr;

 TString s="Sampling animation for NcSample ";
 s+=GetName();
 gr->SetTitle(s.Data());
 gr->SetMarkerStyle(20);
 gr->SetMarkerSize(1);

 Double_t x=0;
 Double_t y=0;
 for (Int_t ip=0; ip<fN; ip++)
 {
  x=GetEntry(ip+1,i,mode,k);
  y=GetEntry(ip+1,j,mode,k);
  gr->SetPoint(ip,x,y);

  if (!ip) gr->Draw(opt.Data());

  s="Variable ";
  s+=fNames[i-1];
  gr->GetXaxis()->SetTitle(s.Data());
  s="Variable ";
  s+=fNames[j-1];
  gr->GetYaxis()->SetTitle(s.Data());
  
  fCanvas->Modified();
  fCanvas->Update();

  gSystem->Sleep(delay);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Animation(TString nameA,TString nameB,Int_t mode,TString nameC,Int_t delay,TString opt)
{
// Animation of an (ordered) sampling with : X-axis=variable nameA and Y-axis=variable nameB.
//
// Every data entry is considered to occur at a step in time, and this member function
// displays an animated time development of the sampling of the specified 2D data points.
// The input argument "delay" represents the pause time (in ms) between every step.
//
// The time flow can be controlled by ordering the various entries according to
// the variable specified with nameC and the input argument "mode".
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// Note : If mode=0 the value of nameC is irrelevant.
//
// The input argument "opt" represents the drawing option(s) for a TGraph.
//
// The default value is opt="AP".
//
// This facility is only available if the storage mode has been activated.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 Animation(i,j,mode,k,delay,opt);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Animation(Int_t i,Int_t j,Int_t k,Int_t mode,Int_t m,Int_t delay,TString opt)
{
// Animation of an (ordered) sampling with : X-axis=variable i, Y-axis=variable j and Z-axis=variable k.
// The first variable has index 1.
//
// Every data entry is considered to occur at a step in time, and this member function
// displays an animated time development of the sampling of the specified 2D data points.
// The input argument "delay" represents the pause time (in ms) between every step.
//
// The time flow can be controlled by ordering the various entries according to
// the specified m-th variable and the input argument "mode".
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// Note : If mode=0 the value of "m" is irrelevant.
//
// The input argument "opt" represents the drawing option(s) for a TGraph2D.
//
// The default value is opt="PFB".
//
// This facility is only available if the storage mode has been activated.

 if (!fStore || fN<1 || i<1 || i>fDim || j<1 || j>fDim || k<1 || k>fDim || (mode && (m<1 || m>fDim)) || delay<0)
 {
  cout << " *NcSample::Animation* Inconsistent input data." << endl;
  return;
 }

 if (fCanvas)
 {
  delete fCanvas;
  fCanvas=0;
 }

 if (fAnimObject)
 {
  delete fAnimObject;
  fAnimObject=0;
 }

 fCanvas=new TCanvas("fCanvas","Sampling animation");

 TGraph2D* gr=new TGraph2D(fN);
 fAnimObject=gr;

 TString s="Sampling animation for NcSample ";
 s+=GetName();
 gr->SetTitle(s.Data());
 gr->SetMarkerStyle(20);
 gr->SetMarkerSize(1);

 Double_t x=0;
 Double_t y=0;
 Double_t z=0;
 for (Int_t ip=0; ip<fN; ip++)
 {
  x=GetEntry(ip+1,i,mode,m);
  y=GetEntry(ip+1,j,mode,m);
  z=GetEntry(ip+1,k,mode,m);
  gr->SetPoint(ip,x,y,z);

  if (!ip) gr->Draw(opt.Data());

  s="Variable ";
  s+=fNames[i-1];
  gr->GetXaxis()->SetTitle(s.Data());
  s="Variable ";
  s+=fNames[j-1];
  gr->GetYaxis()->SetTitle(s.Data());
  s="Variable ";
  s+=fNames[k-1];
  gr->GetZaxis()->SetTitle(s.Data());
  
  fCanvas->Modified();
  fCanvas->Update();

  gSystem->Sleep(delay);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Animation(TString nameA,TString nameB,TString nameC,Int_t mode,TString nameD,Int_t delay,TString opt)
{
// Animation of an (ordered) sampling with : X-axis=variable nameA, Y-axis=variable nameB and Z-axis=variable nameC.
//
// Every data entry is considered to occur at a step in time, and this member function
// displays an animated time development of the sampling of the specified 2D data points.
// The input argument "delay" represents the pause time (in ms) between every step.
//
// The time flow can be controlled by ordering the various entries according to
// the variable specified by nameD and the input argument "mode".
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// Note : If mode=0 the value of nameD is irrelevant.
//
// The input argument "opt" represents the drawing option(s) for a TGraph2D.
//
// The default value is opt="PFB".
//
// This facility is only available if the storage mode has been activated.

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 Int_t m=GetIndex(nameD);
 Animation(i,j,k,mode,m,delay,opt);
}
///////////////////////////////////////////////////////////////////////////
TObject* NcSample::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.

 NcSample* q=new NcSample(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
