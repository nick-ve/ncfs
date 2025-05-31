/**  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
~~~
 * Copyright(c) 1996 NCFS/IIHE, All Rights Reserved.                           *
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
/** @class NcSample
~~~
// Class NcSample
// Perform statistics on various multi-dimensional data samples.
//
// A data sample can be filled using the "Enter" and/or "Remove" functions,
// or by importing the data from a TGraph or TGraph2D via the "Load" function.
// The "Reset" function resets the complete sample to 'empty'.
//
// The member function SetStoreMode() allows to store the data of each entry,
// or limit the number of stored entries via a so-called dynamic buffer,
// which removes (after a selected ordering) the first entry after reaching
// a specified maximum number of entries.
// Please refer to the documentation of SetStoreMode() for further details. 
//
// The info which can be extracted from a certain data sample are the
// minimum, maximum, sum, mean, variance, sigma, median, spread, SNR, CV,
// RMS, covariance and correlation.
// The "spread" of the sample represents the average deviation w.r.t.
// some reference value, i.e. spread=<|v_i-v_ref|>.
// The "Data" function provides all statistics data for a certain sample.
// The variables for which these statistical parameters have to be calculated
// are indicated either by the name or the index of the variable which is
// passed as an argument to the various member functions.
// The index convention for a data point (x,y,z,t) is : x=1  y=2  z=3  t=4.
// The member function SetNames() allows the user to specify different names
// to replace the default ("X","Y","Z","T") naming.
//
// To develop c.q. improve a data acquisition system (DAQ), the functions
// SampleAndHold(), SampleAndSum() and Transmit() may be used to
// investigate the performance of a sampling device, like for instance
// a Switched Capacitor Array (SCA) and an Analog to Digital Converter (ADC).  
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
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, January 14, 2025  13:09Z
~~~
**/
///////////////////////////////////////////////////////////////////////////

#include "NcSample.h"
#include "Riostream.h"
 
ClassImp(NcSample); // Class implementation to enable ROOT I/O
 
///////////////////////////////////////////////////////////////////////////
NcSample::NcSample(const char* name,const char* title) : TNamed(name,title)
{
/**
~~~
// Creation of an NcSample object and resetting the statistics values.
// The dimension is initialised to maximum
~~~
**/

 fDim=fMaxdim;
 fN=0;
 fRemove=0;
 fStore=0;
 fNmax=0;
 fMoveVariable=0;
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
/**
~~~
// Default destructor.
~~~
**/

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
/**
~~~
// Copy constructor.
~~~
**/

 for (Int_t i=0; i<s.fMaxdim; i++)
 {
  fNames[i]=s.fNames[i];
 }

 fDim=s.fDim;
 fStore=s.fStore;
 fNmax=s.fNmax;
 fMoveVariable=s.fMoveVariable;
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
/**
~~~
// Resetting the statistics values for this NcSample object.
// Also the variable names are reset to their (X,Y,Z,T) defaults.
// Dimension and storage parameters are NOT changed.
~~~
**/

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
/**
~~~
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
~~~
**/

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
/**
~~~
// Entering a value into a 1-dim. sample.
// In case of first entry the dimension is set to 1.
~~~
**/

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

 // Remove (after re-ordering) the first entry if needed
 if (fNmax && fN==fNmax) RemoveEntry(1,fMoveVariable,abs(fMoveVariable));

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
/**
~~~
// Removing a value from a 1-dim. sample.
~~~
**/

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
/**
~~~
// Entering a pair (x,y) into a 2-dim. sample.
// In case of first entry the dimension is set to 2.
~~~
**/

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

 // Remove (after re-ordering) the first entry if needed
 if (fNmax && fN==fNmax) RemoveEntry(1,fMoveVariable,abs(fMoveVariable));

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
/**
~~~
// Removing a pair (x,y) from a 2-dim. sample.
~~~
**/

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
/**
~~~
// Entering a set (x,y,z) into a 3-dim. sample.
// In case of first entry the dimension is set to 3.
~~~
**/

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

 // Remove (after re-ordering) the first entry if needed
 if (fNmax && fN==fNmax) RemoveEntry(1,fMoveVariable,abs(fMoveVariable));

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
/**
~~~
// Removing a set (x,y,z) from a 3-dim. sample.
~~~
**/

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
/**
~~~
// Entering a set (x,y,z,t) into a 4-dim. sample.
// In case of first entry the dimension is set to 4.
~~~
**/

 if (!fN) fDim=4;

 if (fDim != 4)
 {
  cout << " *NcSample::Enter* Error : Not a 4-dim sample." << endl;
  return;
 }

 // Remove (after re-ordering) the first entry if needed
 if (fNmax && fN==fNmax) RemoveEntry(1,fMoveVariable,abs(fMoveVariable));

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
/**
~~~
// Removing a set (x,y,z,t) from a 4-dim. sample.
~~~
**/

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
/**
~~~
// Remove the full data entry at the index "i" (1=first) after ordering
// w.r.t. the j-th variable (1=first).
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// Warning :
// ---------
// After invokation of this member function, the indexing has changed.
// Always check the current indexing before removing 1 or more entries.
// To remove a range of consecutive entries from the index interval [m,n],
// please refer to the corresponding other RemoveEntry() member function.
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of "j" is irrelevant
~~~
**/

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
/**
~~~
// Remove the full data entry at the index "i" (1=first) after ordering
// w.r.t. the variable with the specified name.
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// Warning :
// ---------
// After invokation of this member function, the indexing has changed.
// Always check the current indexing before removing 1 or more entries.
// To remove a range of consecutive entries from the index interval [m,n],
// please refer to the corresponding other RemoveEntry() member function.
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of name is irrelevant
~~~
**/

 Int_t j=GetIndex(name);
 RemoveEntry(i,mode,j);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::RemoveEntry(Int_t i,Int_t j,Int_t mode,Int_t k)
{
/**
~~~
// Remove the full data entries at the index interval [i,j] (1=first) after ordering
// w.r.t. the k-th variable (1=first).
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of "k" is irrelevant
~~~
**/

 for (Int_t m=i; m<=j; m++)
 {
  RemoveEntry(i,mode,k);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::RemoveEntry(Int_t i,Int_t j,Int_t mode,TString name)
{
/**
~~~
// Remove the full data entry at the index interval [i,j] (1=first) after ordering
// w.r.t. the variable with the specified name.
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of name is irrelevant
~~~
**/

 for (Int_t m=i; m<=j; m++)
 {
  RemoveEntry(i,mode,name);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Order(Int_t mode,Int_t i)
{
/**
~~~
// Internal member function to order the entries according to the i-th variable (first is i=1).
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of "i" is irrelevant, but i=1 will be used to obtain
//        a correct setting of the ordering status word.
~~~
**/

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

 // Use i=1 for mode=0 to obtain a correct setting of the ordering status word.
 if (!mode) i=1;

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
/**
~~~
// Provide the index (1=first) of the specified variable name.
// In case of no match, the value 0 is returned.
~~~
**/

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
/**
~~~
// Internal member function to compute the various statistical values
// after each entering or removing action on a certain sample.
~~~
**/

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
/**
~~~
// Provide the dimension of this sample.
~~~
**/

 return fDim;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcSample::GetN() const
{
/**
~~~
// Provide the number of entries of a certain sample.
~~~
**/

 return fN;
}
///////////////////////////////////////////////////////////////////////////
TString NcSample::GetVariableName(Int_t i) const
{
/**
~~~
// Provide the name of the i-th variable (first is i=1).
~~~
**/

 TString name="";

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetVariableName* Error : Invalid index " << i << endl;
 }
 else
 {
  name=fNames[i-1];
 }

 return name;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSum(Int_t i) const
{
/**
~~~
// Provide the sum of the i-th variable (first is i=1).
~~~
**/

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
/**
~~~
// Provide the sum of the variable with the specified name.
~~~
**/

 Int_t i=GetIndex(name);
 return GetSum(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMean(Int_t i) const
{
/**
~~~
// Provide the mean of the i-th variable (first is i=1).
~~~
**/

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
/**
~~~
// Provide the mean of the variable with the specified name.
~~~
**/

 Int_t i=GetIndex(name);
 return GetMean(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetRMS(Int_t i) const
{
/**
~~~
// Provide the Root Mean Square of the i-th variable (first is i=1).
//
// Note :
// ------
// This is NOT the RMS deviation defined as sqrt(variance).
// Use the memberfunction GetSigma() to obtain the RMS deviation.
~~~
**/

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
/**
~~~
// Provide the Root Mean Square of the variable with the specified name.
//
// Note :
// ------
// This is NOT the RMS deviation defined as sqrt(variance).
// Use the memberfunction GetSigma() to obtain the RMS deviation.
~~~
**/

 Int_t i=GetIndex(name);
 return GetRMS(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetVar(Int_t i,Int_t model) const
{
/**
~~~
// Provide the variance of the i-th variable (first is i=1)
// as (rms-deviation)^2 when model=0 or as sigma^2 when model=1.
//
// The default value is model=0.
~~~
**/

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
/**
~~~
// Provide the variance of the variable with the specified name.
// as (rms-deviation)^2 when model=0 or as sigma^2 when model=1.
//
// The default value is model=0.
~~~
**/

 Int_t i=GetIndex(name);
 return GetVar(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSigma(Int_t i,Int_t model) const
{
/**
~~~
// Provide the rms-deviation (model=0) or standard deviation sigma of the
// underlying parent distribution (model=1) of the i-th variable.
// The first variable corresponds to i=1.
//
// The default value is model=0.
~~~
**/

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
/**
~~~
// Provide the rms-deviation (model=0) or standard deviation sigma (model=1)
// of the variable with the specified name.
//
// The default value is model=0.
~~~
**/

 Int_t i=GetIndex(name);
 return GetSigma(i,model);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCov(Int_t i,Int_t j) const
{
/**
~~~
// Provide the covariance between variables i and j.
// The index of the first variable is 1.
~~~
**/

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
/**
~~~
// Provide the covariance between the variables with the specified names.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return GetCov(i,j);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCor(Int_t i,Int_t j) const
{
/**
~~~
// Provide the correlation coefficient between variables i and j.
// The index of the first variable is 1.
~~~
**/

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
/**
~~~
// Provide the correlation coefficient the variables with the specified names.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return GetCor(i,j);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Data(Int_t i,Int_t j)
{
/**
~~~
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
~~~
**/

 const char* name=GetName();
 const char* title=GetTitle();

 cout << " *" << ClassName() << "::Data*";
 if (strlen(name))  cout << " Info for sample Name : " << name;
 if (strlen(title)) cout << " Title : " << title;
 cout << endl;
 cout << " Access to the individual data entries is";
 if (!fStore) cout << " not";
 cout << " possible (StoreMode=" << fStore << ")" << endl;

 if (fStore && fNmax)
 {
  if (fNmax==fN && fRemove)
  {
   cout << " *** At entering data the dynamic FIFO storage was limited to " << fNmax << " entries";
  }
  else
  {
   cout << " *** At entering new data the dynamic FIFO storage will be limited to " << fNmax << " entries";
  }
  if (!fMoveVariable) cout << " using the order of original entering.";
  if (fMoveVariable>0) cout << " after increasing";
  if (fMoveVariable<0) cout << " after decreasing";
  if (fMoveVariable) cout << " ordering w.r.t. variable " << abs(fMoveVariable);
  cout << endl;
 }

 if (i<0 || i>fDim || j<0 || j>fDim)
 {
  cout << " Inconsistent input i=" << i << " and j=" << j << " for dimension " << fDim << endl;
  return;
 }
 if (!i && !j) cout << " Statistics and correlations of all variables";
 if (i && !j)  cout << " Statistics of variable " << i;
 if (!i && j)  cout << " Statistics of variable " << j;
 if (i && j)   cout << " Correlation statistics of the variables " << i << " and " << j;
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
/**
~~~
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
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Data(i,j);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::ListOrdered(Int_t mode,Int_t i)
{
/**
~~~
// Provide a listing of all stored entries according to the specified ordering
// mode of the i-th variable (first is i=1). 
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the value of "i" is irrelevant. 
~~~
**/

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

 const char* name=GetName();
 const char* title=GetTitle();

 cout << " *NcSample::ListOrdered* Ordered listing for the sample";
 if (strlen(name))  cout << " Name : " << name;
 if (strlen(title)) cout << " Title : " << title;
 cout << endl;
 if (fNmax==fN && fRemove)
 {
  cout << " *** At entering data the dynamic FIFO storage was limited to " << fNmax << " entries";
  if (!fMoveVariable) cout << " using the order of original entering.";
  if (fMoveVariable>0) cout << " after increasing";
  if (fMoveVariable<0) cout << " after decreasing";
  if (fMoveVariable) cout << " ordering w.r.t. variable " << abs(fMoveVariable);
  cout << endl;
 }
 cout << " Listing of the stored entries in";
 if (!mode) cout << " order of original entering." << endl;
 if (mode<0) cout << " decreasing order of variable : " << i << " (" << fNames[i-1] << ")" << endl;
 if (mode>0) cout << " increasing order of variable : " << i << " (" << fNames[i-1] << ")" << endl;
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
/**
~~~
// Provide a listing of all stored entries according to the specified ordering
// mode of the variable with the specified name.
//
// mode : <0 --> Order in decreasing order
//         0 --> Order in the way the entries were entered
//        >0 --> Order in increasing order
//
// For this functionality the storage mode has to be activated.
//
// Note : If mode=0 the provided name is irrelevant.
~~~
**/

 Int_t i=GetIndex(name);
 ListOrdered(mode,i);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::List(Int_t i)
{
/**
~~~
// Internal member function to list the statistics of i-th variable.
// The index of the first variable is 1.
~~~
**/

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
/**
~~~
// Internal member function to list the covariance and correlation coefficient between variables i and j.
// The index of the first variable is 1.
~~~
**/

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
void NcSample::SetStoreMode(Int_t mode,Int_t nmax,Int_t i)
{
/**
~~~
// Set storage mode for entered data.
// The input parameter "nmax" allows to limit the number of stored entries,
// by removing every first entry after ordering w.r.t. the i-th variable (1=first)
// once a new entry is entered that would exceed "nmax". 
//
// mode = 0 : Entered data will not be stored, but at each entry the statistics will be updated.
//        1 : Data will be stored as entered.
// nmax = 0 : No maximum is set for the number of stored entries.
//      > 0 : Entries will be stored up to a maximum of "nmax" entries.
// i    > 0 : Re-order the entries w.r.t. the i-th variable in increasing order before removing the first (=lowest value).
//      = 0 : Re-order the entries in the order they were entered before removing the first one.
//      < 0 : Re-order the entries w.r.t. the i-th variable in decreasing order before removing the first (=highest value).
//            When nmax=0 the value of "i" is irrelevant.
//
// Notes :
// -------
// 1) For mode=0 or inconsistent input the values of "nmax" and "i" are both set to 0.
// 2) Specification of i=0 results in a regular First In First Out (FIFO) buffer performance,
//    whereas for other "i" values it reflects a FIFO behaviour of removing the first entry
//    after the requested ordering.
//
// By default the values of mode, nmax and i are set to 0 in the constructor of this class.
// The default at invokation of this memberfunction is mode=1, nmax=0 and i=0.
//
// For normal statistics evaluation (e.g. mean, sigma, covariance etc...)
// storage of entered data is not needed. This is the default mode
// of operation and is the most efficient w.r.t. cpu time and memory.
// However, when calculation of a median, minimum or maximum is required,
// then the data storage mode has be activated, unless the statistics
// are obtained from a specified input histogram.
//
// Note : Specification of storage mode can only be performed before the
//        first data item is entered or after invokation of Reset(). 
~~~
**/

 if (fN)
 {
  cout << "  Storage mode can only be set before first data." << endl;
 }
 else if ((!mode && nmax) || mode<0 || mode>1 || nmax<0 || abs(i)>fDim)
 {
  cout << "  Inconsistent input : mode=" << mode << " nmax=" << nmax << " i=" << i << endl;
  fNmax=0;
  fMoveVariable=0;
 }
 else
 {
  fStore=mode;
  fNmax=nmax;
  fMoveVariable=i;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t NcSample::GetStoreMode() const
{
/**
~~~
// Provide the storage mode.
~~~
**/

 return fStore;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcSample::GetStoreNmax() const
{
/**
~~~
// Provide the maximum number of entries for storage.
~~~
**/

 return fNmax;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetQuantile(Double_t f,Int_t i)
{
/**
~~~
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
~~~
**/

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
/**
~~~
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
~~~
**/

 Int_t i=GetIndex(name);
 return GetQuantile(f,i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMedian(Int_t i)
{
/**
~~~
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
~~~
**/

 Double_t median=GetQuantile(0.5,i);
 return median;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMedian(TString name)
{
/**
~~~
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
~~~
**/

 Double_t median=GetQuantile(0.5,name);
 return median;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSpread(Int_t i,Int_t model,Double_t vref)
{
/**
~~~
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
~~~
**/

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
/**
~~~
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
~~~
**/

 Int_t i=GetIndex(name);
 return GetSpread(i,model,vref);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMinimum(Int_t i) const
{
/**
~~~
// Provide the minimum value of the i-th variable (first is i=1).
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.
~~~
**/

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
/**
~~~
// Provide the minimum value of the variable with the specified name.
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.
~~~
**/

 Int_t i=GetIndex(name);
 return GetMinimum(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetMaximum(Int_t i) const
{
/**
~~~
// Provide the maximum value of the i-th variable (first is i=1).
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.
~~~
**/

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
/**
~~~
// Provide the maximum value of the variable with the specified name.
// In case entries have been removed from the sample, a correct value can
// only be obtained if the storage mode has been activated.
~~~
**/

 Int_t i=GetIndex(name);
 return GetMaximum(i);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetQuantile(Double_t f,TH1* histo,Int_t mode) const
{
/**
~~~
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
~~~
**/

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
/**
~~~
// Provide the median of the variable X or Y from the specified 1D histogram.
// For this functionality it is not needed to activate the storage mode.
//
// In the case of incompatible data the value 0 is returned.
//
// The median is determined by invokation of GetQuantile(0.5,histo,mode).
// Please refer to the documentation of the corresponding GetQuantile()
// memberfunction for further details.
~~~
**/

 Double_t median=GetQuantile(0.5,histo,mode);
 return median;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSpread(TH1* histo,Int_t mode,Int_t model,Double_t vref) const
{
/**
~~~
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
~~~
**/

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
/**
~~~
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
~~~
**/

 TString name=GetName();

 if (!fStore)
 {
  cout << " *NcSample::GetEntry* Error : Storage mode not activated." << endl;
  return 0;
 }

 if (i<1 || i>fN)
 {
  cout << " *NcSample::GetEntry* Error : Invalid index number i=" << i;
  if (name!="") printf(" for NcSample %-s",name.Data());
  printf("\n");
  return 0;
 }

 if (j<1 || j>fDim)
 {
  cout << " *NcSample::GetEntry* Error : Invalid variable number j=" << j;
  if (name!="") printf(" for NcSample %-s",name.Data());
  printf("\n");
  return 0;
 }

 if (mode && (k<1 || k>fDim))
 {
  cout << " *NcSample::GetEntry* Error : Invalid argument k=" << k;
  if (name!="") printf(" for NcSample %-s",name.Data());
  printf("\n");
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
/**
~~~
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
~~~
**/

 Int_t j=GetIndex(nameA);
 Int_t k=GetIndex(nameB);
 return GetEntry(i,j,mode,k);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::GetSubset(NcSample* s,Int_t ifirst,Int_t ilast,Int_t mode,Int_t k)
{
/**
~~~
// Provide the [ifirst,ilast] entries, after ordering w.r.t. the k-th variable, via the output NcSample "s".
// The first entry is indicated by the index ifirst=1 and the first variable is k=1.
//
// Note : All previously stored data in the output NcSample "s" will be lost.
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
~~~
**/

 if (!s)
 {
  cout << " *NcSample::GetSubset* Error : Pointer for output NcSample object is zero." << endl;
  return;
 }

 s->Reset();

 if (!fStore)
 {
  cout << " *NcSample::GetSubset* Error : Storage mode not activated." << endl;
  return;
 }

 if (ifirst<1 || ifirst>fN || ilast>fN || ilast<ifirst)
 {
  cout << " *NcSample::GetSubset* Error : Invalid input ifirst=" << ifirst << " ilast=" << ilast << endl;
  return;
 }

 if (mode && (k<1 || k>fDim))
 {
  cout << " *NcSample::GetSubset* Error : Invalid argument k=" << k << endl;
  return;
 }

 s->SetStoreMode();
 s->SetNames(fNames[0],fNames[1],fNames[2],fNames[3]);

 Double_t* values=new Double_t[fDim];
 for (Int_t i=ifirst; i<=ilast; i++)
 {
  for (Int_t j=1; j<=fDim; j++)
  {
   values[j-1]=GetEntry(i,j,mode,k);
  }
  if (fDim==1) s->Enter(values[0]);
  if (fDim==2) s->Enter(values[0],values[1]);
  if (fDim==3) s->Enter(values[0],values[1],values[2]);
  if (fDim==4) s->Enter(values[0],values[1],values[2],values[3]);
 }

 delete [] values;
}
///////////////////////////////////////////////////////////////////////////
NcSample NcSample::GetDtSample(Int_t i,Int_t nc,Int_t store,Int_t nmax,Int_t order)
{
/**
~~~
// The i-th variable of the current sample is regarded as the recording of event times,
// and this memberfunction samples the time intervals (dt) between a certain fixed
// amount of time ordered consecutive entries for this i-th variable.
//
// Returned object : A single-variable NcSample object containing the corresponding dt values,
//                   with "Dt" as variable name.
//
// For this functionality the storage mode of the current sample has to be activated.
//
// Input arguments :
// -----------------
// i         : The requested variable (1=first) of the current sample.
// nc        : The step count to arrive at the required consecutive entry (see example below).
// store = 0 : The resulting dt values will not be stored, but the dt statistics will be recorded.
//         1 : In addition to recording the dt statistics, also the obtained dt values will be stored.
// nmax  = 0 : No maximum is set for the number of stored dt entries.
//       > 0 : The dt entries will be stored up to a maximum of "nmax" entries.
// order > 0 : Re-order the dt entries in increasing order before removing the first (=lowest value).
//       = 0 : Re-order the dt entries in the order they were obtained before removing the first one.
//       < 0 : Re-order the dt entries in decreasing order before removing the first (=highest value).
//             When nmax=0 the value of "order" is irrelevant.
//
// Note :
// ------
// Specification of order=0 results in a regular First In First Out (FIFO) buffer performance,
// whereas for other "order" values it reflects a FIFO behaviour of removing the first entry
// after the requested ordering.
//
// The default values are store=1, nmax=0 and order=0.
//
// Example :
// ---------
// Specifying "nc=1" will provide the NcSample with the dt intervals between each consecutive event,
// i.e. the distribution of time intervals between the events (1,2), (2,3), (3,4) etc.
// Specifying "nc=2" will provide the NcSample with the dt intervals between every 2nd consecutive event,
// i.e. the distribution of time intervals between the events (1,3), (2,4), (3,5) etc.
// In case the input sample contained Poissonian distributed event times, the produced dt values represent
// so called Erlang distributions (see also class NcMath), for which each time interval contains
// exactly "nc" events of which the last event occurs right at the end of the interval.
//
// Note :
// ------
// In the text above the values of the current sample are regarded as observed event times,
// but actually any sampling of an observable with a well defined ordering can be used.
~~~
**/

 // Make "order" value to comply with a single variable sample
 if (order>0) order=1;
 if (order<0) order=-1; 

 NcSample sdt;
 sdt.SetStoreMode(store,nmax,order);
 sdt.SetName("DtSample");
 sdt.SetNames("Dt");

 if (i<1 || i>GetDimension()) return sdt;

 if (nc<1) return sdt;
 
 Int_t nen=GetN();
 if (nen<=nc) return sdt;

 TString varname=GetVariableName(i);
 TString str;
 str.Form("Interval sampling for variable %-s between %-i consecutive entries (nc=%-i)",varname.Data(),nc+1,nc);
 sdt.SetNameTitle("DtSample",str);

 // Fill the dt sample
 Double_t t1=0;
 Double_t t2=0;
 Double_t deltat=0;
 for (Int_t ien=1; ien<=nen; ien++)
 {
  if (ien+nc>nen) break;

  t1=GetEntry(ien,i,1,i);
  t2=GetEntry(ien+nc,i,1,i);
  deltat=fabs(t2-t1);

  sdt.Enter(deltat);
 }

 return sdt;
}
///////////////////////////////////////////////////////////////////////////
NcSample NcSample::GetDtSample(TString name,Int_t nc,Int_t store,Int_t nmax,Int_t order)
{
/**
~~~
// The variable with the specified name of the current sample is regarded as the recording of event times,
// and this memberfunction samples the time intervals (dt) between a certain fixed amount of
// time ordered consecutive entries for the specified variable.
//
// Returned object : A single-variable NcSample object containing the corresponding dt values,
//                   with "Dt" as variable name.
//
// For this functionality the storage mode of the current sample has to be activated.
//
// Input arguments :
// -----------------
// name      : The name of the requested variable of the current sample.
// nc        : The step count to arrive at the required consecutive entry (see example below).
// store = 0 : The resulting dt values will not be stored, but the dt statistics will be recorded.
//         1 : In addition to recording the dt statistics, also the obtained dt values will be stored.
// nmax  = 0 : No maximum is set for the number of stored dt entries.
//       > 0 : The dt entries will be stored up to a maximum of "nmax" entries.
// order > 0 : Re-order the dt entries in increasing order before removing the first (=lowest value).
//       = 0 : Re-order the dt entries in the order they were obtained before removing the first one.
//       < 0 : Re-order the dt entries in decreasing order before removing the first (=highest value).
//             When nmax=0 the value of "order" is irrelevant.
//
// Note :
// ------
// Specification of order=0 results in a regular First In First Out (FIFO) buffer performance,
// whereas for other "order" values it reflects a FIFO behaviour of removing the first entry
// after the requested ordering.
//
// The default values are store=1, nmax=0 and order=0.
//
// Example :
// ---------
// Specifying "nc=1" will provide the NcSample with the dt intervals between each consecutive event,
// i.e. the distribution of time intervals between the events (1,2), (2,3), (3,4) etc.
// Specifying "nc=2" will provide the NcSample with the dt intervals between every 2nd consecutive event,
// i.e. the distribution of time intervals between the events (1,3), (2,4), (3,5) etc.
// In case the input sample contained Poissonian distributed event times, the produced dt values represent
// so called Erlang distributions (see also class NcMath), for which each time interval contains
// exactly "nc" events of which the last event occurs right at the end of the interval.
//
// Note :
// ------
// In the text above the values of the current sample are regarded as observed event times,
// but actually any sampling of an observable with a well defined ordering can be used.
~~~
**/

 Int_t i=GetIndex(name);
 NcSample sdt=GetDtSample(i,nc,store,nmax,order);

 return sdt;
}
///////////////////////////////////////////////////////////////////////////
TH1D NcSample::GetSamplingHistogram(Int_t i,TF1* f)
{
/**
~~~
// Provide a TH1D histogram with : X-axis=sampling entry number and Y-axis=variable i.
// In case the 1-dimensional function "f" is specified, then the
// function value of variable i will be used as at the Y-axis.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default value is f=0.
~~~
**/

 TString s="Sampling Histogram for NcSample ";
 s+=GetName();
 s+=";Sampling number;";
 if (f)
 {
  TString sf=f->GetExpFormula("p");
  sf.ReplaceAll("x",fNames[i-1]);   
  s+=sf;
 }
 else
 {
  s+=fNames[i-1];
 }

 TH1D hist("",s,fN,0,fN);

 if (!fStore)
 {
  cout << " *NcSample::GetSamplingHistogram* Error : Storage mode has not been activated." << endl;
  return hist;
 }

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetSamplingHistogram* Error : Invalid index encountered i=" << i << endl;
  return hist;
 }

 Double_t y=0;
 for (Int_t ip=1; ip<=fN; ip++)
 {
  y=GetEntry(ip,i);
  if (f) y=f->Eval(y);
  hist.SetBinContent(ip,y);
 }

 return hist;
}
///////////////////////////////////////////////////////////////////////////
TH1D NcSample::GetSamplingHistogram(Int_t i,TString f)
{
/**
~~~
// Provide a TH1D with : X-axis=sampling entry number and Y-axis=f(variable i).
// The 1D function "f" has to be specified following the TF1 string format convention,
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
~~~
**/

 TF1 func("func",f);

 return GetSamplingHistogram(i,&func);
}
///////////////////////////////////////////////////////////////////////////
TH1D NcSample::GetSamplingHistogram(TString nameA,TF1* f)
{
/**
~~~
// Provide a TH1D with : X-axis=sampling entry number and Y-axis=variable with nameA.
// In case the 1-dimensional function "f" is specified, then the
// function value of nameA will be used as at the Y-axis.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default value is f=0.
~~~
**/

 Int_t i=GetIndex(nameA);
 return GetSamplingHistogram(i,f);
}
///////////////////////////////////////////////////////////////////////////
TH1D NcSample::GetSamplingHistogram(TString nameA,TString f)
{
/**
~~~
// Provide a TH1D with : X-axis=sampling entry number and Y-axis=f(variable with nameA).
// The 1D function "f" has to be specified following the TF1 string format convention,
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
~~~
**/

 TF1 func("func",f);

 return GetSamplingHistogram(nameA,&func);
}
///////////////////////////////////////////////////////////////////////////
TH1D NcSample::Get1DHistogram(Int_t i,Int_t j,Bool_t sumw2,Int_t nbx,TF1* f)
{
/**
~~~
// Provide a TH1D histogram with the values of variable i.
// If j>0 the corresponding value of variable j will be used as a weight.
// In case j>0 and also the 1-dimensional function "f" is specified,
// then the function value of variable j will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input argument "nbx" defines the number of bins on the X-axis.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are j=0, sumw2=kFALSE, nbx=100 and f=0.
~~~
**/

 TString s="1D Histogram for NcSample ";
 s+=GetName();
 s+=";";
 s+=fNames[i-1];
 s+=";Counts";
 if (j>0)
 {
  s+=" weighted by : ";
  if (f)
  {
   TString sf=f->GetExpFormula("p");
   sf.ReplaceAll("x",fNames[j-1]);   
   s+=sf;
  }
  else
  {
   s+=fNames[j-1];
  }
 }

 Double_t xlow=GetMinimum(i);
 Double_t xup=GetMaximum(i);

 Double_t dx=0;
 if (nbx) dx=(xup-xlow)/double(nbx);

 // Extension to include the maximum value
 xup+=1e-6*fabs(dx);

 TH1D hist("",s.Data(),nbx,xlow,xup);
 hist.Sumw2(sumw2);

 if (!fStore)
 {
  cout << " *NcSample::Get1DHistogram* Error : Storage mode has not been activated." << endl;
  return hist;
 }

 if (i<1 || i>fDim || j>fDim)
 {
  cout << " *NcSample::Get1DHistogram* Error : Invalid index encountered i=" << i << " j=" << j << endl;
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
   if (f) y=f->Eval(y);
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
TH1D NcSample::Get1DHistogram(TString nameA,TString nameB,Bool_t sumw2,Int_t nbx,TF1* f)
{
/**
~~~
// Provide a TH1D histogram for the variable specified with nameA.
// If nameB is valid, the corresponding value of that variable will be used as a weight.
// In case nameB is valid and also the 1-dimensional function "f" is specified,
// then the function value of nameB will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input argument "nbx" defines the number of bins on the X-axis.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are nameB="-", sumw2=kFALSE, nbx=100 and f=0.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return Get1DHistogram(i,j,sumw2,nbx,f);
}
///////////////////////////////////////////////////////////////////////////
TH2D NcSample::Get2DHistogram(Int_t i,Int_t j,Int_t k,Bool_t sumw2,Int_t nbx,Int_t nby,TF1* f)
{
/**
~~~
// Provide a TH2D histogram for the values of variables i and j.
// If k>0 the corresponding value of variable k will be used as a weight.
// In case k>0 and also the 1-dimensional function "f" is specified,
// then the function value of variable k will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input arguments "nbx" and "nby" define the number of bins on the X-axis
// and Y-axis, respectively.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are k=0, sumw2=kFALSE, nbx=100, nby=100 and f=0.
~~~
**/

 TString s="2D Histogram for NcSample ";
 s+=GetName();
 s+=";";
 s+=fNames[i-1];
 s+=";";
 s+=fNames[j-1];
 s+=";Counts";
 if (k>0)
 {
  s+=" weighted by : ";
  if (f)
  {
   TString sf=f->GetExpFormula("p");
   sf.ReplaceAll("x",fNames[k-1]);
   s+=sf;
  }
  else
  {
   s+=fNames[k-1];
  }
 }

 Double_t xlow=GetMinimum(i);
 Double_t xup=GetMaximum(i);
 Double_t ylow=GetMinimum(j);
 Double_t yup=GetMaximum(j);

 Double_t dx=0;
 if (nbx) dx=(xup-xlow)/double(nbx);
 Double_t dy=0;
 if (nby) dy=(yup-ylow)/double(nby);

 // Extension to include the maximum values
 xup+=1e-6*fabs(dx);
 yup+=1e-6*fabs(dy);

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
  cout << " *NcSample::Get2DHistogram* Error : Invalid index encountered i=" << i << " j=" << j << " k=" << k << endl;
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
   if (f) z=f->Eval(z);
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
TH2D NcSample::Get2DHistogram(TString nameA,TString nameB,TString nameC,Bool_t sumw2,Int_t nbx,Int_t nby,TF1* f)
{
/**
~~~
// Provide a TH2D histogram for the variables specified with nameA and nameB.
// If nameC is valid, the corresponding value of that variable will be used as a weight.
// In case nameC is valid and also the 1-dimensional function "f" is specified,
// then the function value of nameC will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input arguments "nbx" and "nby" define the number of bins on the X-axis
// and Y-axis, respectively.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are nameC="-", sumw2=kFALSE, nbx=100, nby=100 and f=0.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 return Get2DHistogram(i,j,k,sumw2,nbx,nby,f);
}
///////////////////////////////////////////////////////////////////////////
TH3D NcSample::Get3DHistogram(Int_t i,Int_t j,Int_t k,Int_t m,Bool_t sumw2,Int_t nbx,Int_t nby,Int_t nbz,TF1* f)
{
/**
~~~
// Provide a TH3D histogram for the values of variables i, j and k.
// If m>0 the corresponding value of variable m will be used as a weight.
// In case m>0 and also the 1-dimensional function "f" is specified,
// then the function value of variable m will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input arguments "nbx", "nby" and "nbz" define the number of bins on the X-axis,
// Y-axis and Z-axis, respectively.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are m=0, sumw2=kFALSE, nbx=100, nby=100, nbz=100 and f=0.
~~~
**/

 TString s="3D Histogram for NcSample ";
 s+=GetName();
 if (m>0)
 {
  s+=" with ";
  if (f)
  {
   TString sf=f->GetExpFormula("p");
   sf.ReplaceAll("x",fNames[m-1]);
   s+=sf;
  }
  else
  {
   s+=fNames[m-1];
  }
  s+=" as weight";
 }
 s+=";";
 s+=fNames[i-1];
 s+=";";
 s+=fNames[j-1];
 s+=";";
 s+=fNames[k-1];

 Double_t xlow=GetMinimum(i);
 Double_t xup=GetMaximum(i);
 Double_t ylow=GetMinimum(j);
 Double_t yup=GetMaximum(j);
 Double_t zlow=GetMinimum(k);
 Double_t zup=GetMaximum(k);

 Double_t dx=0;
 if (nbx) dx=(xup-xlow)/double(nbx);
 Double_t dy=0;
 if (nby) dy=(yup-ylow)/double(nby);
 Double_t dz=0;
 if (nbz) dz=(zup-zlow)/double(nbz);

 // Extension to include the maximum values
 xup+=1e-6*fabs(dx);
 yup+=1e-6*fabs(dy);
 zup+=1e-6*fabs(dz);

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
  cout << " *NcSample::Get2DHistogram* Error : Invalid index encountered i=" << i << " j=" << j << " k=" << k << " m=" << m << endl;
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
   if (f) t=f->Eval(t);
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
TH3D NcSample::Get3DHistogram(TString nameA,TString nameB,TString nameC,TString nameD,Bool_t sumw2,Int_t nbx,Int_t nby,Int_t nbz,TF1* f)
{
/**
~~~
// Provide a TH3D histogram for the variables specified with nameA, nameB and nameC.
// If nameD is valid, the corresponding value of that variable will be used as a weight.
// In case nameD is valid and also the 1-dimensional function "f" is specified,
// then the function value of nameD will be used as a weight.
// The input argument "sumw2" activates (kTRUE) TH1::Sumw2() option or not (kFALSE).
// The input arguments "nbx", "nby" and "nbz" define the number of bins on the X-axis,
// Y-axis and Z-axis, respectively.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are nameD="-", sumw2=kFALSE, nbx=100, nby=100, nbz=100 and f=0.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 Int_t m=GetIndex(nameD);
 return Get3DHistogram(i,j,k,m,sumw2,nbx,nby,nbz,f);
}
///////////////////////////////////////////////////////////////////////////
TGraph NcSample::GetGraph(Int_t i,TF1* f)
{
/**
~~~
// Provide a TGraph with : X-axis=sampling entry number and Y-axis=variable i.
// In case the 1-dimensional function "f" is specified, then the
// function value of variable i will be used as at the Y-axis.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default value is f=0.
~~~
**/

 TGraph gr;
 gr.SetName("NcSample");

 if (!fStore)
 {
  cout << " *NcSample::GetGraph* Error : Storage mode has not been activated." << endl;
  return gr;
 }

 if (i<1 || i>fDim)
 {
  cout << " *NcSample::GetGraph* Error : Invalid index encountered i=" << i << endl;
  return gr;
 }

 Double_t x=0;
 Double_t y=0;
 for (Int_t ip=0; ip<fN; ip++)
 {
  x=ip+1;
  y=GetEntry(ip+1,i);
  if (f) y=f->Eval(y);
  gr.SetPoint(ip,x,y);
 }

 TString s="TGraph for NcSample ";
 s+=GetName();
 s+=" : X-axis=Sampling number ";
 s+="  Y-axis=";
 TString sf=fNames[i-1];
 if (f)
 {
  sf=f->GetExpFormula("p");
  sf.ReplaceAll("x",fNames[i-1]);
 }
 s+=sf;
 s+=";Sampling number;";
 s+=sf;

 gr.SetTitle(s.Data());

 gr.SetMarkerStyle(20);
 gr.SetMarkerSize(1);
 gr.SetDrawOption("AP");

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph NcSample::GetGraph(Int_t i,TString f)
{
/**
~~~
// Provide a TGraph with : X-axis=sampling entry number and Y-axis=f(variable i).
// The 1D function "f" has to be specified following the TF1 string format convention,
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
~~~
**/

 TF1 func("func",f);

 return GetGraph(i,&func);
}
///////////////////////////////////////////////////////////////////////////
TGraph NcSample::GetGraph(TString nameA,TF1* f)
{
/**
~~~
// Provide a TGraph with : X-axis=sampling entry number and Y-axis=variable with nameA.
// In case the 1-dimensional function "f" is specified, then the
// function value of nameA will be used as at the Y-axis.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default value is f=0.
~~~
**/

 Int_t i=GetIndex(nameA);
 return GetGraph(i,f);
}
///////////////////////////////////////////////////////////////////////////
TGraph NcSample::GetGraph(Int_t i,Int_t j,TF1* f)
{
/**
~~~
// Provide a TGraph with : X-axis=variable i and Y-axis=variable j.
// In case the 1-dimensional function "f" is specified, then the
// function value of variable j will be used as at the Y-axis.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default value is f=0.
~~~
**/

 TGraph gr;
 gr.SetName("NcSample");

 if (!fStore)
 {
  cout << " *NcSample::GetGraph* Error : Storage mode has not been activated." << endl;
  return gr;
 }

 if (i<1 || i>fDim || j<1 || j>fDim)
 {
  cout << " *NcSample::GetGraph* Error : Invalid index encountered i=" << i << " j=" << j << endl;
  return gr;
 }

 Double_t x=0;
 Double_t y=0;
 for (Int_t ip=0; ip<fN; ip++)
 {
  x=GetEntry(ip+1,i);
  y=GetEntry(ip+1,j);
  if (f) y=f->Eval(y);
  gr.SetPoint(ip,x,y);
 }

 TString s="TGraph for NcSample ";
 s+=GetName();
 s+=" : X-axis=";
 s+=fNames[i-1];
 s+="  Y-axis=";
 TString sf=fNames[j-1];
 if (f)
 {
  sf=f->GetExpFormula("p");
  sf.ReplaceAll("x",fNames[j-1]);
 }
 s+=sf;
 s+=";";
 s+=fNames[i-1];
 s+=";";
 s+=sf;

 gr.SetTitle(s.Data());

 gr.SetMarkerStyle(20);
 gr.SetMarkerSize(1);
 gr.SetDrawOption("AP");

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph NcSample::GetGraph(Int_t i,Int_t j,TString f)
{
/**
~~~
// Provide a TGraph with : X-axis=variable i and Y-axis=f(variable j).
// The 1D function "f" has to be specified following the TF1 string format convention,
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
~~~
**/

 TF1 func("func",f);

 return GetGraph(i,j,&func);
}
///////////////////////////////////////////////////////////////////////////
TGraph NcSample::GetGraph(TString nameA,TString nameB,TF1* f)
{
/**
~~~
// Provide a TGraph with : X-axis=variable with nameA and Y-axis=variable with nameB.
// In case the 1-dimensional function "f" is specified, then the
// function value of nameB will be used as at the Y-axis.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default value is f=0.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return GetGraph(i,j,f);
}
///////////////////////////////////////////////////////////////////////////
TGraphErrors NcSample::GetGraphErrors(TGraph* gr,Int_t ix,Int_t iy,TF1* fx,TF1* fy)
{
/**
~~~
// Provide a Graph with new c.q. modified errors.
// The returned object is a TGraphErrors and the provided input Graph is not modified.
//
// Input arguments :
// -----------------
// gr : Pointer to a TGraph or TGraphErrors object to provide the (X,Y) data points
// ix : Index of the variable of this NcSample object of which the (fx function) values will be used to set the X-errors 
// iy : Index of the variable of this NcSample object of which the (fy function) values will be used to set the Y-errors 
// fx : 1-dimensional function to specify the X-errors as a function of either the actual X-value or specified "ix variable" 
// fy : 1-dimensional function to specify the Y-errors as a function of either the actual Y-value or specified "iy variable"
//
// Notes :
// -------
// 1) The error will always be stored as the absolute value of the specified variable or function value.
// 2) If the variable index and corresponding function pointer are both zero, the corresponding errors are all set to 0.
// 3) In case a non-zero value of ix or iy is specified, the storage mode has to be active.
// 4) In case a non-zero value of ix or iy is specified, the entries of this NcSample and Graph should sequentially match.
//
// Examples :
// ----------
// 1) ix=3, fx=0, iy=4, fy=0 
//    The X-errors will be set equal to the absolute values of the 3rd variable of this NcSample object
//    and the Y-errors will be set to the absolute values of the 4th variable of this NcSample object.
// 2) ix=3, fx=0, iy=0, fy="sqrt(x)"
//    The X-errors will be set equal to the absolute values of the 3rd variable of this NcSample object
//    and the Y-errors will be set to the square root of the corresponding Y-value of the input graph.
// 3) ix=3, fx="0.1*x", iy=4, fy="sqrt(x)" 
//    The X-errors will be set equal to the absolute value of 0.1 times the values of the 3rd variable of this NcSample object
//    and the Y-errors will be set to the square root of the values of the 4th variable of this NcSample object.
// 4) ix=0, fx=0, iy=4, fy="sqrt(abs(x))" 
//    The X-errors will be set to 0 and the Y-errors will be set to the square root of the absolute values of the 4th variable
//    of this NcSample object.
// 5) ix=0, fx="sqrt(x)", iy=0, fy="0.1*x" 
//    The X-errors will be set to the square root of the corresponding X-value of the input graph and the Y-errors will be set to
//    the absolute value of 0.1 times the corresponding Y-value of the input graph.
//
// The defaults are ix=0, iy=0, fx=0 and fy=0.
~~~
**/

 TGraphErrors gre;

 if (!gr)
 {
  cout << " *NcSample::GetGraphErrors* Error : No input Graph is specified." << endl;
  return gre;
 }

 TString name=gr->GetName();
 name+=".E";

 gre.SetName(name);

 if ((ix || iy) && !fStore)
 {
  cout << " *NcSample::GetGraphErrors* Error : Storage mode has not been activated." << endl;
  return gre;
 }

 if (ix<0 || ix>fDim || iy<0 || iy>fDim)
 {
  cout << " *NcSample::GetGraphErros* Error : Invalid index encountered ix=" << ix << " iy=" << iy << endl;
  return gre;
 }

 // The number of data points of the input Graph
 Int_t np=gr->GetN();

 if (!np)
 {
  cout << " *NcSample::GetGraphErrors* Error : Input Graph does not contain any data points." << endl;
  return gre;
 }

 if ((ix || iy) && np!=fN)
 {
  cout << " *NcSample::GetGraphErrors* Error : Entries don't match Nsample=" << fN << " Ngraph=" << np << endl;
  return gre;
 }

 Double_t x=0;
 Double_t y=0;
 Double_t ex=0;
 Double_t ey=0;
 for (Int_t ip=0; ip<np; ip++)
 {
  gr->GetPoint(ip,x,y);
  if (ix) ex=GetEntry(ip+1,ix);
  if (fx)
  {
   if (!ix)
   {
    ex=fx->Eval(x);
   }
   else
   {
    ex=fx->Eval(ex);
   }
  }
  if (iy) ey=GetEntry(ip+1,iy);
  if (fy)
  {
   if (!iy)
   {
    ey=fy->Eval(y);
   }
   else
   {
    ey=fy->Eval(ey);
   }
  }
  ex=fabs(ex);
  ey=fabs(ey);
  gre.SetPoint(ip,x,y);
  gre.SetPointError(ip,ex,ey);
 }

 TString sfx="0";
 if (ix || fx)
 {
  sfx="|";
  if (!fx)
  {
   sfx+=fNames[ix-1];
  }
  else
  {
   sfx+=fx->GetExpFormula("p");
   if (ix) sfx.ReplaceAll("x",fNames[ix-1]);
  }
  sfx+="|";
 }

 TString sfy="0";
 if (iy || fy)
 {
  sfy="|";
  if (!fy)
  {
   sfy+=fNames[iy-1];
  }
  else
  {
   sfy+=fy->GetExpFormula("p");
   if (iy)
   {
    sfy.ReplaceAll("x",fNames[iy-1]);
   }
   else
   {
    sfy.ReplaceAll("x","y");
   }
  }
  sfy+="|";
 }

 TString s="TGraphErrors for Graph : ";
 s+=gr->GetName();
 s+="   X-error=";
 s+=sfx;
 s+=" Y-error=";
 s+=sfy;

 // Copy the axes titles of the input Graph (if present)
 TAxis* axis=0;
 axis=gr->GetXaxis();
 if (axis)
 {
  s+=";";
  s+=axis->GetTitle();
 } 
 axis=gr->GetYaxis();
 if (axis)
 {
  s+=";";
  s+=axis->GetTitle();
 } 

 gre.SetTitle(s.Data());

 gre.SetMarkerStyle(20);
 gre.SetMarkerSize(0.5);
 gre.SetDrawOption("AP");

 return gre;
}
///////////////////////////////////////////////////////////////////////////
TGraphErrors NcSample::GetGraphErrors(TGraph* gr,TString nameA,TString nameB,TF1* fx,TF1* fy)
{
/**
~~~
// Provide a Graph with new c.q. modified errors.
// The returned object is a TGraphErrors and the provided input Graph is not modified.
//
// Input arguments :
// -----------------
// gr    : Pointer to a TGraph or TGraphErrors object to provide the (X,Y) data points
// nameA : Name of the variable of this NcSample object of which the (fx function) values will be used to set the X-errors 
// nameB : Name of the variable of this NcSample object of which the (fy function) values will be used to set the Y-errors 
// fx    : 1-dimensional function to specify the X-errors as a function of either the actual X-value or specified "nameA variable" 
// fy    : 1-dimensional function to specify the Y-errors as a function of either the actual Y-value or specified "nameB variable"
//
// Notes :
// -------
// 1) The error will always be stored as the absolute value of the specified variable or function value.
// 2) If the variable name doesn't exist and the corresponding function pointer is zero, the corresponding errors are all set to 0.
// 3) In case an existing variable name is specified, the storage mode has to be active.
// 4) In case an existing variable name is specified, the entries of this NcSample and Graph should sequentially match.
//
// Examples :
// ----------
// 1) nameA="Time", fx=0, nameB="Amp", fy=0 
//    The X-errors will be set equal to the absolute values of the variable "Time" of this NcSample object
//    and the Y-errors will be set to the absolute values of the variable "Amp" of this NcSample object.
// 2) nameA="Time", fx=0, nameB="-", fy="sqrt(x)"
//    The X-errors will be set equal to the absolute values of the variable "Time" of this NcSample object
//    and the Y-errors will be set to the square root of the corresponding Y-value of the input graph.
// 3) nameA="Time", fx="0.1*x", iy="Amp", fy="sqrt(x)" 
//    The X-errors will be set equal to the absolute value of 0.1 times the values of the "Time" variable of this NcSample object
//    and the Y-errors will be set to the square root of the values of the "Amp" variable of this NcSample object.
// 4) nameA="-", fx=0, nameB="Amp", fy="sqrt(abs(x))" 
//    The X-errors will be set to 0 and the Y-errors will be set to the square root of the absolute values of the "Amp" variable
//    of this NcSample object.
// 5) nameA="-", fx="sqrt(x)", nameB="-", fy="0.1*x" 
//    The X-errors will be set to the square root of the corresponding X-value of the input graph and the Y-errors will be set to
//    the absolute value of 0.1 times the corresponding Y-value of the input graph.
//
// The defaults are fx=0 and fy=0.
~~~
**/

 Int_t ix=GetIndex(nameA);
 Int_t iy=GetIndex(nameB);

 return GetGraphErrors(gr,ix,iy,fx,fy);
}
///////////////////////////////////////////////////////////////////////////
TGraphTime* NcSample::GetGraph(Int_t i,Int_t j,Int_t mode,Int_t k,Bool_t smp)
{
/**
~~~
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
~~~
**/

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
  cout << " *NcSample::GetGraph* Error : Invalid index encountered i=" << i << " j=" << j << " k=" << k << endl;
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
 fGraphT->SetName("NcSample");

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
 s+=";";
 s+=fNames[i-1];
 s+=";";
 s+=fNames[j-1];
 fGraphT->SetNameTitle("",s.Data());

 return fGraphT;
}
///////////////////////////////////////////////////////////////////////////
TGraphTime* NcSample::GetGraph(TString nameA,TString nameB,Int_t mode,TString nameC,Bool_t smp)
{
/**
~~~
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
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 return GetGraph(i,j,mode,k,smp);
}
///////////////////////////////////////////////////////////////////////////
TGraph2D NcSample::GetGraph(Int_t i,Int_t j,Int_t k,TF1* f)
{
/**
~~~
// Provide a TGraph with : X-axis=variable i, Y-axis=variable j and Z-axis=variable k.
// In case the 1-dimensional function "f" is specified, the function value of variable k
// will be used as at the Z-axis.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default value is f=0.
~~~
**/

 TGraph2D gr;
 gr.SetName("NcSample");

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
  cout << " *NcSample::GetGraph* Error : Invalid index encountered i=" << i << " j=" << j << " k=" << k << endl;
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
  if (f) z=f->Eval(z);
  gr.SetPoint(ip,x,y,z);
 }

 TString s="TGraph2D for NcSample ";
 s+=GetName();
 s+=" : X-axis=";
 s+=fNames[i-1];
 s+="  Y-axis=";
 s+=fNames[j-1];
 s+="  Z-axis=";
 TString sf=fNames[k-1];
 if (f)
 {
  sf=f->GetExpFormula("p");
  sf.ReplaceAll("x",fNames[k-1]);
 }
 s+=sf;
 s+=";";
 s+=fNames[i-1];
 s+=";";
 s+=fNames[j-1];
 s+=";";
 s+=sf;

 gr.SetTitle(s.Data());

 gr.SetMarkerStyle(20);
 gr.SetMarkerSize(1);
 gr.SetDrawOption("P");

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TGraph2D NcSample::GetGraph(Int_t i,Int_t j,Int_t k,TString f)
{
/**
~~~
// Provide a TGraph with : X-axis=variable i, Y-axis=variable j and Z-axis=f(variable k).
// The 1D function "f" has to be specified following the TF1 string format convention.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
~~~
**/

 TF1 func("func",f);

 return GetGraph(i,j,k,&func);
}
///////////////////////////////////////////////////////////////////////////
TGraph2D NcSample::GetGraph(TString nameA,TString nameB,TString nameC,TF1* f)
{
/**
~~~
// Provide a TGraph with : X-axis=variable nameA, Y-axis=variable nameB and Z-axis=variable nameC.
// In case the 1-dimensional function "f" is specified, then the function value of nameC
// will be used as at the Z-axis.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default value is f=0.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 return GetGraph(i,j,k,f);
}
///////////////////////////////////////////////////////////////////////////
TGraph2D NcSample::GetGraph(TString nameA,TString nameB,TString nameC,TString f)
{
/**
~~~
// Provide a TGraph with : X-axis=variable nameA, Y-axis=variable nameB and Z-axis=f(variable nameC).
// The 1D function "f" has to be specified following the TF1 string format convention,
//
// Note : This facility is only available if the storage mode has been activated.
~~~
**/

 TF1 func("func",f);

 return GetGraph(nameA,nameB,nameC,&func);
}
///////////////////////////////////////////////////////////////////////////
TGraph2DErrors NcSample::GetGraph2DErrors(TGraph2D* gr,Int_t ix,Int_t iy,Int_t iz,TF1* fx,TF1* fy,TF1* fz)
{
/**
~~~
// Provide a Graph2D with new c.q. modified errors.
// The returned object is a TGraph2DErrors and the provided input Graph2D is not modified.
//
// Input arguments :
// -----------------
// gr : Pointer to a TGraph2D or TGraph2DErrors object to provide the (X,Y,Z) data points
// ix : Index of the variable of this NcSample object of which the (fx function) values will be used to set the X-errors 
// iy : Index of the variable of this NcSample object of which the (fy function) values will be used to set the Y-errors 
// iz : Index of the variable of this NcSample object of which the (fy function) values will be used to set the Y-errors 
// fx : 1-dimensional function to specify the X-errors as a function of either the actual X-value or specified "ix variable" 
// fy : 1-dimensional function to specify the Y-errors as a function of either the actual Y-value or specified "iy variable"
// fz : 1-dimensional function to specify the Z-errors as a function of either the actual Z-value or specified "iz variable"
//
// Notes :
// -------
// 1) The error will always be stored as the absolute value of the specified variable or function value.
// 2) If the variable index and corresponding function pointer are both zero, the corresponding errors are all set to 0.
// 3) In case a non-zero value of ix, iy or iz is specified, the storage mode has to be active.
// 4) In case a non-zero value of ix,iy or iz is specified, the entries of this NcSample and Graph should sequentially match.
//
// Examples :
// ----------
// 1) ix=3, fx=0, iy=4, fy=0, iz=2, fz=0 
//    The X-errors will be set equal to the absolute values of the 3rd variable of this NcSample object,
//    the Y-errors will be set to the absolute values of the 4th variable of this NcSample object
//    and the Z-errors will be set to the absolute values of the 2nd variable of this NcSample object.
// 2) ix=3, fx=0, iy=0, fy="sqrt(x)", iz=2, fz=0
//    The X-errors will be set equal to the absolute values of the 3rd variable of this NcSample object,
//    the Y-errors will be set to the square root of the corresponding Y-values of the input graph
//    and the Z-errors will be set to the absolute values of the 2nd variable of this NcSample object.
// 3) ix=3, fx="0.1*x", iy=4, fy="sqrt(x)", iz=0, fz="sqrt(x)" 
//    The X-errors will be set equal to the absolute value of 0.1 times the values of the 3rd variable of this NcSample object,
//    the Y-errors will be set to the square root of the values of the 4th variable of this NcSample object
//    and the Z-errors will be set to the square root of the corresponding Z-values of the input graph.
// 4) ix=0, fx=0, iy=4, fy="sqrt(abs(x))", iz=1, fz=0 
//    The X-errors will be set to 0, the Y-errors will be set to the square root of the absolute values of the 4th variable
//    of this NcSample object, and the Z-errors will be set to the absolute values of the 1st variable of this NcSample object. 
// 5) ix=0, fx="sqrt(x)", iy=0, fy="0.1*x", iz=0, fz=0
//    The X-errors will be set to the square root of the corresponding X-values of the input graph, the Y-errors will be set to
//    the absolute value of 0.1 times the corresponding Y-values of the input graph and the Z-errors will be set to 0.
//
// The defaults are ix=0, iy=0, iz=0, fx=0, fy=0 and fz=0.
~~~
**/

 TGraph2DErrors gre;

 if (!gr)
 {
  cout << " *NcSample::GetGraph2DErrors* Error : No input Graph is specified." << endl;
  return gre;
 }

 TString name=gr->GetName();
 name+=".E";

 gre.SetName(name);

 if ((ix || iy || iz) && !fStore)
 {
  cout << " *NcSample::GetGraph2DErrors* Error : Storage mode has not been activated." << endl;
  return gre;
 }

 if (ix<0 || ix>fDim || iy<0 || iy>fDim || iz<0 || iz>fDim)
 {
  cout << " *NcSample::GetGraph2DErrors* Error : Invalid index encountered ix=" << ix << " iy=" << iy << " iz=" << iz << endl;
  return gre;
 }

 // The number of data points of the input Graph
 Int_t np=gr->GetN();

 if (!np)
 {
  cout << " *NcSample::GetGraph2DErrors* Error : Input Graph does not contain any data points." << endl;
  return gre;
 }

 if ((ix || iy || iz) && np!=fN)
 {
  cout << " *NcSample::GetGraph2DErrors* Error : Entries don't match Nsample=" << fN << " Ngraph=" << np << endl;
  return gre;
 }

 Double_t* xarr=gr->GetX();
 Double_t* yarr=gr->GetY();
 Double_t* zarr=gr->GetZ();
 Double_t x=0;
 Double_t y=0;
 Double_t z=0;
 Double_t ex=0;
 Double_t ey=0;
 Double_t ez=0;
 for (Int_t ip=0; ip<np; ip++)
 {
  x=xarr[ip];
  y=yarr[ip];
  z=zarr[ip];
  if (ix) ex=GetEntry(ip+1,ix);
  if (fx)
  {
   if (!ix)
   {
    ex=fx->Eval(x);
   }
   else
   {
    ex=fx->Eval(ex);
   }
  }
  if (iy) ey=GetEntry(ip+1,iy);
  if (fy)
  {
   if (!iy)
   {
    ey=fy->Eval(y);
   }
   else
   {
    ey=fy->Eval(ey);
   }
  }
  if (iz) ez=GetEntry(ip+1,iz);
  if (fz)
  {
   if (!iz)
   {
    ez=fz->Eval(z);
   }
   else
   {
    ez=fz->Eval(ez);
   }
  }
  ex=fabs(ex);
  ey=fabs(ey);
  ez=fabs(ez);
  gre.SetPoint(ip,x,y,z);
  gre.SetPointError(ip,ex,ey,ez);
 }

 TString sfx="0";
 if (ix || fx)
 {
  sfx="|";
  if (!fx)
  {
   sfx+=fNames[ix-1];
  }
  else
  {
   sfx+=fx->GetExpFormula("p");
   if (ix) sfx.ReplaceAll("x",fNames[ix-1]);
  }
  sfx+="|";
 }

 TString sfy="0";
 if (iy || fy)
 {
  sfy="|";
  if (!fy)
  {
   sfy+=fNames[iy-1];
  }
  else
  {
   sfy+=fy->GetExpFormula("p");
   if (iy)
   {
    sfy.ReplaceAll("x",fNames[iy-1]);
   }
   else
   {
    sfy.ReplaceAll("x","y");
   }
  }
  sfy+="|";
 }

 TString sfz="0";
 if (iz || fz)
 {
  sfz="|";
  if (!fz)
  {
   sfz+=fNames[iz-1];
  }
  else
  {
   sfz+=fz->GetExpFormula("p");
   if (iz)
   {
    sfz.ReplaceAll("x",fNames[iz-1]);
   }
   else
   {
    sfz.ReplaceAll("x","z");
   }
  }
  sfz+="|";
 }

 TString s="TGraphErrors for Graph : ";
 s+=gr->GetName();
 s+="   X-error=";
 s+=sfx;
 s+=" Y-error=";
 s+=sfy;
 s+=" Z-error=";
 s+=sfz;

 // Copy the axes titles of the input Graph (if present)
 TAxis* axis=0;
 axis=gr->GetXaxis();
 if (axis)
 {
  s+=";";
  s+=axis->GetTitle();
 } 
 axis=gr->GetYaxis();
 if (axis)
 {
  s+=";";
  s+=axis->GetTitle();
 } 
 axis=gr->GetZaxis();
 if (axis)
 {
  s+=";";
  s+=axis->GetTitle();
 } 

 gre.SetTitle(s.Data());

 gre.SetMarkerStyle(20);
 gre.SetMarkerSize(0.5);
 gre.SetDrawOption("err P");

 return gre;
}
///////////////////////////////////////////////////////////////////////////
TGraph2DErrors NcSample::GetGraph2DErrors(TGraph2D* gr,TString nameA,TString nameB,TString nameC,TF1* fx,TF1* fy,TF1* fz)
{
/**
~~~
// Provide a Graph2D with new c.q. modified errors.
// The returned object is a TGraph2DErrors and the provided input Graph2D is not modified.
//
// Input arguments :
// -----------------
// gr    : Pointer to a TGraph or TGraphErrors object to provide the (X,Y) data points
// nameA : Name of the variable of this NcSample object of which the (fx function) values will be used to set the X-errors 
// nameB : Name of the variable of this NcSample object of which the (fy function) values will be used to set the Y-errors 
// nameC : Name of the variable of this NcSample object of which the (fz function) values will be used to set the Z-errors 
// fx    : 1-dimensional function to specify the X-errors as a function of either the actual X-value or specified "nameA variable" 
// fy    : 1-dimensional function to specify the Y-errors as a function of either the actual Y-value or specified "nameB variable"
// fz    : 1-dimensional function to specify the Z-errors as a function of either the actual Z-value or specified "nameC variable"
//
// Notes :
// -------
// 1) The error will always be stored as the absolute value of the specified variable or function value.
// 2) If the variable name doesn't exist and the corresponding function pointer is zero, the corresponding errors are all set to 0.
// 3) In case an existing variable name is specified, the storage mode has to be active.
// 4) In case an existing variable name is specified, the entries of this NcSample and Graph should sequentially match.
//
// Examples :
// ----------
// 1) nameA="Time", fx=0, nameB="Amp", fy=0, nameC="ADC", fz=0
//    The X-errors will be set equal to the absolute values of the variable "Time" of this NcSample object,
//    the Y-errors will be set to the absolute values of the variable "Amp" of this NcSample object
//    and the Z-errors will be set to the absolute values of the variable "ADC" of this NcSample object
// 2) nameA="Time", fx=0, nameB="-", fy="sqrt(x)", nameC="ADC", fz=0
//    The X-errors will be set equal to the absolute values of the variable "Time" of this NcSample object,
//    the Y-errors will be set to the square root of the corresponding Y-values of the input graph
//    and the Z-errors will be set to the absolute values of the variable "ADC" of this NcSample object
// 3) nameA="Time", fx="0.1*x", iy="Amp", fy="sqrt(x)", iz=0, fz="sqrt(x)"
//    The X-errors will be set equal to the absolute value of 0.1 times the values of the "Time" variable of this NcSample object,
//    the Y-errors will be set to the square root of the values of the "Amp" variable of this NcSample object
//    and the Z-errors will be set to the square root of the corresponding Z-values of the input graph.
// 4) nameA="-", fx=0, nameB="Amp", fy="sqrt(abs(x))", nameC="ADC", fz=0
//    The X-errors will be set to 0, the Y-errors will be set to the square root of the absolute values of the "Amp" variable
//    of this NcSample object, and the Z-values will be set to the absolute values of the "ADC" variable of this NcSample object.
// 5) nameA="-", fx="sqrt(x)", nameB="-", fy="0.1*x", nameC="-", fz=0
//    The X-errors will be set to the square root of the corresponding X-values of the input graph, the Y-errors will be set to
//    the absolute value of 0.1 times the corresponding Y-values of the input graph and the Z-errors will be set to 0.
//
// The defaults are fx=0, fy=0 and fz=0.
~~~
**/

 Int_t ix=GetIndex(nameA);
 Int_t iy=GetIndex(nameB);
 Int_t iz=GetIndex(nameC);

 return GetGraph2DErrors(gr,ix,iy,iz,fx,fy,fz);
}
///////////////////////////////////////////////////////////////////////////
TGraphQQ NcSample::GetQQplot(Int_t i,Int_t j,TF1* f)
{
/**
~~~
// Provide a QQ-plot (TGraphQQ) for the values of the variables i and j.
// The first variable has index 1.
// In case the function "f" is specified, it will replace the role of variable j.
// When the function "f" is specified, the value of "j" is irrelevant.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default is f=0.
~~~
**/

 TGraphQQ gr;
 gr.SetName("NcSample");

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
  cout << " *NcSample::GetQQplot* Error : Invalid index encountered i=" << i << " j=" << j << endl;
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
  s+=" of ";
  s+=fNames[i-1];
  s+=" versus f(a)=";
  TString sf=f->GetExpFormula("p");
  sf.ReplaceAll("x","a");
  s+=sf;
 }
 else
 {
  s+=";";
  s+=fNames[i-1];
  s+=";";
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
/**
~~~
// Provide a QQ-plot (TGraphQQ) for the the variables specified by nameA and nameB.
// In case the function "f" is specified, it will replace the role of variable nameB.
// When the function "f" is specified, the value of nameB is irrelevant.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default is f=0.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 return GetQQplot(i,j,f);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Load(TGraph* g,Int_t clr)
{
/**
~~~
// Load the data points of a TGraph object as 2-dimensional (x,y) data.
//
// Input arguments :
// -----------------
// g   : Pointer to the TGraph object.
// clr : Flag to denote to first clear (1) the data storage or not (0).
//
// The default value is clr=1.
~~~
**/
 
 if (!g)
 {
  printf(" *%-s::Load* Error : No TGraph specified ==> No action taken. \n",ClassName());
  return;
 }
 
 if (!clr && fDim!=2)
 {
  printf(" *%-s::Load* Error : Dimension %-i does not match a TGraph. \n",ClassName(),fDim);
  return;
 }

 if (clr) Reset();

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
/**
~~~
// Load the data points of a TGraph2D object as 3-dimensional (x,y,z) data.
//
// Input arguments :
// -----------------
// g   : Pointer to the TGraph2D object.
// clr : Flag to denote to first clear (1) the data storage or not (0).
//
// The default value is clr=1.
~~~
**/
 
 if (!g)
 {
  printf(" *%-s::Load* Error : No TGraph2D specified ==> No action taken. \n",ClassName());
  return;
 }
 
 if (!clr && fDim!=3)
 {
  printf(" *%-s::Load* Error : Dimension %-i does not match a TGraph2D. \n",ClassName(),fDim);
  return;
 }

 if (clr) Reset();
 
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
void NcSample::Load(TH1* h,Int_t clr)
{
/**
~~~
// Load the data of a 1-dimensional histogram.
//
// Input arguments :
// -----------------
// h   : Pointer to the 1-dimensional histogram.
// clr : Flag to denote to first clear (1) the data storage or not (0).
//
// The default value is clr=1.
~~~
**/
 
 if (!h)
 {
  printf(" *%-s::Load* Error : No histogram specified ==> No action taken. \n",ClassName());
  return;
 }

 Int_t hdim=h->GetDimension();
 
 if (hdim!=1)
 {
  printf(" *%-s::Load* Error : Histograms of dimension %-i are not supported. \n",ClassName(),hdim);
  return;
 }
 
 if (!clr && fDim!=1)
 {
  printf(" *%-s::Load* Error : Dimension %-i does not match a 1-dimensional histogram. \n",ClassName(),fDim);
  return;
 }

 if (clr) Reset();

 Int_t nbins=h->GetNbinsX();

 Double_t val=0;
 for (Int_t i=1; i<=nbins; i++)
 {
  val=h->GetBinContent(i);
  Enter(val);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Load(TArray* arr,Int_t clr)
{
/**
~~~
// Load the data of a 1-dimensional data array.
//
// Input arguments :
// -----------------
// arr : Pointer to the 1-dimensional data array.
// clr : Flag to denote to first clear (1) the data storage or not (0).
//
// The default value is clr=1.
~~~
**/
 
 if (!arr)
 {
  printf(" *%-s::Load* Error : No data array specified ==> No action taken. \n",ClassName());
  return;
 }
 
 if (!clr && fDim!=1)
 {
  printf(" *%-s::Load* Error : Dimension %-i does not match a 1-dimensional data array. \n",ClassName(),fDim);
  return;
 }

 if (clr) Reset();

 Int_t n=arr->GetSize();

 Double_t val=0;
 for (Int_t i=0; i<n; i++)
 {
  val=arr->GetAt(i);
  Enter(val);
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetSNR(Int_t i,Int_t mode,Bool_t dB) const
{
/**
~~~
// Provide the Signal to Noise Ratio (SNR) of the i-th variable.
// The first variable corresponds to i=1.
//
// For a transient c.q. pulse signal, the SNR is defined as the ratio between
// the largest amplitude difference and the standard deviation of the sample.
// This is obtained via input argument "mode" with abs(mode)=3 or abs(mode)=4,
// as outlined below.
//
// For a continuous signal, the definition used here is SNR=(signal power)/(noise power).
// This implies that when the values of the i-th variable represent amplitudes related
// to a continuous signal, the squares of the (rms) values should be used.
// This can be specified via the input argument "mode" where abs(mode)=2 will
// invoke a conversion of amplitudes into power, whereas abs(mode)=1 will use
// the values of the i-th variable "as is", i.e. reflecting a direct power measurement.
//
// Furthermore, the input argument "mode" also provides a selection to use
// the variance of the sample (mode>0) or the standard deviation, sigma, of the underlying
// parent distribution (mode<0), as indicated below.  
//
// Input arguments :
// -----------------
// mode :  1 --> Continuous (nearly) stable signal with data reflecting power and SNR=abs(mean/rms-deviation)
//        -1 --> Continuous (nearly) stable signal with data reflecting power and SNR=abs(mean/sigma)
//         2 --> Continuous (nearly) stable signal with data reflecting amplitudes and SNR=(mean*mean)/variance
//        -2 --> Continuous (nearly) stable signal with data reflecting amplitudes and SNR=(mean*mean)/(sigma*sigma)
//         3 --> Transient signal with data reflecting power or RMS amplitudes and SNR=(max-min)/rms-deviation
//        -3 --> Transient signal with data reflecting power or RMS amplitudes and SNR=(max-min)/sigma
//         4 --> Transient signal with data reflecting bipolar amplitudes and SNR=(max-min)/(2*rms-deviation)
//        -4 --> Transient signal with data reflecting bipolar amplitudes and SNR=(max-min)/(2*sigma)
// dB   : kFALSE Provide the SNR as the above straight ratio
//        kTRUE  Provide the SNR in Decibel
//
// The default values are mode=2 and dB=kTRUE for backward compatibility.
//
// In case of inconsistent data, the value -9999 is returned.
//
// Derivation of the applied formulas for a continuous (nearly) stable signal :
// ----------------------------------------------------------------------------
// For each recorded sampling k we assume an amplitude contribution Sk of the signal and Nk of the noise.
//
// This leads to the expression :
//
//   SNR=(average signal power)/(average noise power)=<Sk^2>/<Nk^2>  (1)
//
// Using variance(y)=<y^2>-<y>^2 we can express the above formula in terms of the
// signal variance var(Sk), signal mean <Sk>, noise variance var(Nk) and noise mean <Nk> as follows :
//
//   SNR=(var(Sk)+<Sk>^2)/(var(Nk)+<Nk>^2)  (2)  
//
// Furthermore, the mean of all the recorded samplings Mu is given by : Mu=<Sk+Nk>=<Sk>+<Nk>  (3)
//
// In most cases we will have <Nk>=0, which implies that Mu=<Sk>  (4)
//
// In other words : The mean of the observed samplings is equal to the signal mean,
// which implies that we can determine <Sk> directly from our data.
//
// Using Eq.(4) and <Nk>=0 in Eq.(2) leads us to : SNR=(var(Sk)+Mu^2)/var(Nk)  (5)
//
// In case of a (nearly) stable signal, we have approximately var(Sk)=0,
// which implies that the total observed sampling variance S=var(Nk).
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
~~~
**/

 if (i<1 || i>fDim || !mode || abs(mode)>4) return -9999;

 Double_t snr=-1;
 Double_t ymax=GetMaximum(i);
 Double_t ymin=GetMinimum(i);
 Double_t mean=GetMean(i);
 Double_t var=GetVar(i);
 Double_t rmsdev=sqrt(var);
 Double_t sigma=GetSigma(i,1);
 Double_t stdev=rmsdev;
 if (mode<0) stdev=sigma;

 if (stdev<=0) return -9999;

 ///////////////////////////////
 // SNR of a transient signal //
 ///////////////////////////////

 if (abs(mode)>2)
 {
  if (abs(mode)==3) // Data are power or RMS amplitudes
  {
   snr=(ymax-ymin)/stdev;
  }
  if (abs(mode)==4) // Data are bipolar amplitudes
  {
   snr=(ymax-ymin)/(2.*stdev);
  }

  if (dB && snr>0) snr=10.*log10(snr);

  return snr;
 }

 ////////////////////////////////
 // SNR of a continuous signal //
 ////////////////////////////////

 // Treat the values as observed amplitudes and work in dB scale
 Double_t psignal=mean*mean;
 Double_t pnoise=stdev*stdev;

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
/**
~~~
// Provide the Signal to Noise Ratio (SNR) of the variable with the specified name.
//
// For a transient c.q. pulse signal, the SNR is defined as the ratio between
// the largest amplitude difference and the standard deviation of the sample.
// This is obtained via input argument "mode" with abs(mode)=3 or abs(mode)=4,
// as outlined below.
//
// For a continuous signal, the definition used here is SNR=(signal power)/(noise power).
// This implies that when the values of the i-th variable represent amplitudes related
// to a continuous signal, the squares of the (rms) values should be used.
// This can be specified via the input argument "mode" where abs(mode)=2 will
// invoke a conversion of amplitudes into power, whereas abs(mode)=1 will use
// the values of the i-th variable "as is", i.e. reflecting a direct power measurement.
//
// Furthermore, the input argument "mode" also provides a selection to use
// the variance of the sample (mode>0) or the standard deviation, sigma, of the underlying
// parent distribution (mode<0), as indicated below.  
//
// Input arguments :
// -----------------
// mode :  1 --> Continuous (nearly) stable signal with data reflecting power and SNR=abs(mean/rms-deviation)
//        -1 --> Continuous (nearly) stable signal with data reflecting power and SNR=abs(mean/sigma)
//         2 --> Continuous (nearly) stable signal with data reflecting amplitudes and SNR=(mean*mean)/variance
//        -2 --> Continuous (nearly) stable signal with data reflecting amplitudes and SNR=(mean*mean)/(sigma*sigma)
//         3 --> Transient signal with data reflecting power or RMS amplitudes and SNR=(max-min)/rms-deviation
//        -3 --> Transient signal with data reflecting power or RMS amplitudes and SNR=(max-min)/sigma
//         4 --> Transient signal with data reflecting bipolar amplitudes and SNR=(max-min)/(2*rms-deviation)
//        -4 --> Transient signal with data reflecting bipolar amplitudes and SNR=(max-min)/(2*sigma)
// dB   : kFALSE Provide the SNR as the above straight ratio
//        kTRUE  Provide the SNR in Decibel
//
// The default values are mode=2 and dB=kTRUE for backward compatibility.
//
// In case of inconsistent data, the value -9999 is returned.
//
// Derivation of the applied formulas for a continuous (nearly) stable signal :
// ----------------------------------------------------------------------------
// For each recorded sampling k we assume an amplitude contribution Sk of the signal and Nk of the noise.
//
// This leads to the expression :
//
//   SNR=(average signal power)/(average noise power)=<Sk^2>/<Nk^2>  (1)
//
// Using variance(y)=<y^2>-<y>^2 we can express the above formula in terms of the
// signal variance var(Sk), signal mean <Sk>, noise variance var(Nk) and noise mean <Nk> as follows :
//
//   SNR=(var(Sk)+<Sk>^2)/(var(Nk)+<Nk>^2)  (2)  
//
// Furthermore, the mean of all the recorded samplings Mu is given by : Mu=<Sk+Nk>=<Sk>+<Nk>  (3)
//
// In most cases we will have <Nk>=0, which implies that Mu=<Sk>  (4)
//
// In other words : The mean of the observed samplings is equal to the signal mean,
// which implies that we can determine <Sk> directly from our data.
//
// Using Eq.(4) and <Nk>=0 in Eq.(2) leads us to : SNR=(var(Sk)+Mu^2)/var(Nk)  (5)
//
// In case of a (nearly) stable signal, we have approximately var(Sk)=0,
// which implies that the total observed sampling variance S=var(Nk).
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
~~~
**/

 Int_t i=GetIndex(name);
 return GetSNR(i,mode,dB);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCV(Int_t i,Int_t mode) const
{
/**
~~~
// Provide the Coefficient of Variation (CV) of the i-th variable.
// The first variable corresponds to i=1.
//
// As indicated below, the input argument "mode" allows to select use of
// the mean of the sample or the difference between the maximum and minimum value.
// The latter is best suited for bipolar transient signals, since their mean value
// may become (close to) zero, irrespective of the signal amplitude.
//
// Furthermore, the input argument "mode" also provides a selection to use
// the RMSdeviation of the sample (mode>0) or the standard deviation, sigma,
// of the underlying parent distribution (mode<0), as indicated below.  
//
// mode :  1 --> CV=abs(RMSdeviation/mean)
//        -1 --> CV=abs(sigma/mean)
//         2 --> CV=abs(2*RMSdeviation/(max-min))
//        -2 --> CV=abs(2*sigma/(max-min))
//
// The default value is mode=1 for backward compatibility.
//
// In case of inconsistent data, the value -1 is returned. 
~~~
**/

 if (i<1 || i>fDim || abs(mode)<1 || abs(mode)>2) return -1;

 Double_t vmax=GetMaximum(i);
 Double_t vmin=GetMinimum(i);
 Double_t value=GetMean(i);
 if (abs(mode)==2) value=(vmax-vmin)/2.;
 Double_t stdev=GetSigma(i,0);
 if (mode<0) stdev=GetSigma(i,1);

 Double_t cv=-1;

 if (value) cv=fabs(stdev/value);

 return cv;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::GetCV(TString name,Int_t mode) const
{
/**
~~~
// Provide the Coefficient of Variation (CV) of the variable with the specified name.
//
// As indicated below, the input argument "mode" allows to select use of
// the mean of the sample or the difference between the maximum and minimum value.
// The latter is best suited for bipolar transient signals, since their mean value
// may become (close to) zero, irrespective of the signal amplitude.
//
// Furthermore, the input argument "mode" also provides a selection to use
// the RMSdeviation of the sample (mode>0) or the standard deviation, sigma,
// of the underlying parent distribution (mode<0), as indicated below.  
//
// mode :  1 --> CV=abs(RMSdeviation/mean)
//        -1 --> CV=abs(sigma/mean)
//         2 --> CV=abs(2*RMSdeviation/(max-min))
//        -2 --> CV=abs(2*sigma/(max-min))
//
// The default value is mode=1 for backward compatibility.
//
// In case of inconsistent data, the value -1 is returned.
~~~
**/

 Int_t i=GetIndex(name);
 return GetCV(i,mode);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Animation(Int_t i,Int_t j,Int_t mode,Int_t k,Int_t delay,TString opt)
{
/**
~~~
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
~~~
**/

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
/**
~~~
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
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 Animation(i,j,mode,k,delay,opt);
}
///////////////////////////////////////////////////////////////////////////
void NcSample::Animation(Int_t i,Int_t j,Int_t k,Int_t mode,Int_t m,Int_t delay,TString opt)
{
/**
~~~
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
~~~
**/

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
/**
~~~
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
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);
 Int_t k=GetIndex(nameC);
 Int_t m=GetIndex(nameD);
 Animation(i,j,k,mode,m,delay,opt);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::Digitize(Int_t i,Int_t nbits,Double_t vcal,Int_t mode)
{
/**
~~~
// Digitize the values of the i-th variable according to an "nbits" ADC.
//
// **************************************************************************************
// *** This function has become obsolete and is only kept for backward compatibility. ***
// *** Please refer to the new, more flexible memberfunction Transmit().              ***
// **************************************************************************************
//
// Input arguments :
// -----------------
// i        : Digitization of the i-th variable (first is i=1) will be performed.
//            In case the value of "i" is out of range for this sample, just the specs
//            of the specified ADC performance will be printed, but no digitization is performed.
//            This allows to test various scenarios without modifying the data.
// nbits >0 : Digitization of the values will be performed using nbits.
//            The digitized result (digval) will be stored to replace the original value.
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
// Return argument :
// -----------------
// mode=0 or mode=1 --> The value of "step size" is returned.
// mode=2 or mode=3 --> The value of "scale" is returned.
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
//         
// The maximum number of bits that is supported is 60 to guarantee identical functioning
// on all machines.
//
// Warning :
// ---------
// This member function will change the contents of this sample
// concerning the values of the i-th variable.
// In case access to the original values is required, one should
// make a copy of the original sample before the digitization process.
//
// This facility is only available if the storage mode has been activated.
//
// In case of inconsistent input parameters, no digitization is performed and the value 0 is returned.
~~~
**/

 if (mode<0 || mode>3)
 {
  cout << " *NcSample::Digitize* Inconsistent input mode=" << mode << endl;
  return 0;
 }

 if (!nbits || nbits>60 || nbits<-60 || fabs(vcal)<=0)
 {
  cout << " *NcSample::Digitize* Inconsistent input nbits=" << nbits << " vcal=" << vcal << endl;
  return 0;
 }

 if ((mode==1 || mode==3) && nbits==1)
 {
  cout << " *NcSample::Digitize* Inconsistent input nbits=" << nbits << " mode=" << mode << endl;
  return 0;
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

 if (step<=0) return 0;

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

 cout << " *NcSample::Digitize* Parameters of the " << nbits << "-bits";
 if (logmode) cout << " Log10";
 cout << " ADC digitization";
 if (i>0 && i<=fDim) cout << " of variable : " << i << " (" << fNames[i-1] << ")"; 
 cout << endl;
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

 Double_t retval=step;
 if (mode==2)
 {
  if (vcal<0) retval=digvalmin;
  if (vcal>0) retval=digvalmax;
 }
 if (mode==3) retval=digvalmax;

 if (!fStore)
 {
  cout << endl;
  cout << " *NcSample::Digitize* Error : Storage mode has not been activated." << endl;
  return retval;
 }

 Double_t minval=1;
 if (i>0 && i<=fDim) minval=GetMinimum(i);

 if (logmode && minval<=0)
 {
  cout << endl;
  cout << " *NcSample::Digitize* Non-positive input value(s) for Log10 ADC i=" << i << " minimum=" << minval << endl;
  return retval;
 }
 
 if (i<1 || i>fDim)
 {
  cout << endl;
  cout << " *NcSample::Digitize* Value i=" << i << " is outside range. Only listing of ADC specs." << endl;
  return retval;
 }

 Double_t val=0;
 Long_t nsteps=0;
 Double_t digval=0;
 for (Int_t j=1; j<=fN; j++)
 {
  val=GetEntry(j,i);

  if (logmode) val=log10(val);

  nsteps=val/step;

  if (nsteps<nstepsmin) nsteps=nstepsmin;
  if (nsteps>nstepsmax) nsteps=nstepsmax;

  digval=double(nsteps)*step;

  if (logmode) digval=pow(10,digval);

  if (i==1) fX->AddAt(digval,j-1);
  if (i==2) fY->AddAt(digval,j-1);
  if (i==3) fZ->AddAt(digval,j-1);
  if (i==4) fT->AddAt(digval,j-1);
 }

 return retval;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcSample::Digitize(TString name,Int_t nbits,Double_t vcal,Int_t mode)
{
/**
~~~
// Digitize the values of the variable with the specified name according to an "nbits" ADC.
//
// **************************************************************************************
// *** This function has become obsolete and is only kept for backward compatibility. ***
// *** Please refer to the new, more flexible memberfunction Transmit().              ***
// **************************************************************************************
//
// Input arguments :
// -----------------
// name     : Digitization of the variable with this name will be performed.
//            In case the name is non-existent for this sample, just the specs
//            of the specified ADC performance will be printed, but no digitization is performed.
//            This allows to test various scenarios without modifying the data.
// nbits >0 : Digitization of the values will be performed using nbits.
//            The digitized result (digval) will be stored to replace the original value.
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
// Return argument :
// -----------------
// mode=0 or mode=1 --> The value of "step size" is returned.
// mode=2 or mode=3 --> The value of "scale" is returned.
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
//         
// The maximum number of bits that is supported is 60 to guarantee identical functioning
// on all machines.
//
// Warning :
// ---------
// This member function will change the contents of this sample
// concerning the values of the variable with the specified name.
// In case access to the original values is required, one should
// make a copy of the original sample before the digitization process.
//
// This facility is only available if the storage mode has been activated.
//
// In case of inconsistent input parameters, no digitization is performed and the value 0 is returned.
~~~
**/

 Int_t i=GetIndex(name);

 return Digitize(i,nbits,vcal,mode);
}
///////////////////////////////////////////////////////////////////////////
TArrayL64 NcSample::ADC(Int_t nbits,Double_t range,Double_t Vbias,TArray* Vsig,TH1* hist,Int_t B,Int_t C) const
{
/**
~~~
// Internal member function for processing of an Analog to Digital Converter (ADC). 
// Construct from analog input signals the discrete quantized data values of an "nbits" ADC,
// based on the "range" for the analog signal and a bias voltage "Vbias" (see below).
// The analog input signals are provided by the TArray "Vsig".
// The resulting (integer) quantized ADC values are returned in a TArrayL64 object.
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
// Vsig     : Array containing the analog input signals.
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
// 1) In case no "Vsig" array is provided, just the ADC specs will be printed
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

 Int_t nVsig=0;
 if (Vsig) nVsig=Vsig->GetSize();

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
  title.Form("%-s %-i-bits %-s ADC with Vfs=%-g LSB=%-g;Sample number;ADC counts",ClassName(),nbits,mode.Data(),Vfs,LSB);
  hist->SetBins(nVsig,0,nVsig);
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
TArrayD NcSample::DAC(Int_t nbits,Double_t range,Double_t Vbias,TArray* adcs,TArray* peds,TH1* hist,Int_t B,Int_t C) const
{
/**
~~~
// Internal member function for processing of a Digital to Analog Converter (DAC). 
// Reconstruct the analog signals based on the discrete quantized digital data from an "nbits" ADC,
// based on the "range" for the analog signal and a bias voltage "Vbias" or array "peds" of pedestal values (see below).
// The digital input signals are provided by the TArray "adcs".
// The resulting analog values are returned in a TArrayD object.
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
// adcs     : Array to contain the digital input signals.
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
// 1) In case no "adcs" array is provided, just the DAC specs will be printed
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

 Int_t nadcs=0;
 if (adcs) nadcs=adcs->GetSize();

 Int_t npeds=0;
 if (peds) npeds=peds->GetSize();

 if (nbits<=0 || nbits>60 || !range || fabs(Vbias)>fabs(range) || (peds && npeds<nadcs) || B<0 || (B && C<1))
 {
  printf("\n *%-s::DAC* Inconsistent input nbits=%-i range=%-g Vbias=%-g nadcs=%-i npeds=%-i B=%-i C=%-i \n",ClassName(),nbits,range,Vbias,nadcs,npeds,B,C);
  return arrdac;
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
  title.Form("%-s %-i-bits %-s DAC with Vfs=%-g LSB=%-g;Sample number;Amplitude",ClassName(),nbits,mode.Data(),Vfs,LSB);
  hist->SetBins(nadcs,0,nadcs);
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
Long64_t NcSample::Transmit(Int_t i,Int_t nbits,Double_t range,Double_t Vbias,TArray* peds,TH1* hist,Int_t B,Int_t C)
{
/**
~~~
// Mimic signal transmission according to an "nbits" ADC-DAC chain.
// The values of the i-th variable are digitized via the discrete quantization levels of an "nbits" ADC,
// based on the "range" for the analog signal and a bias voltage "Vbias" or array "peds" of pedestal values (see below).
// After digitization, the digital signals are converted into analog signals via the corresponding "nbits" DAC.
// In this way the effect of digitization on the original input signals can be investigated, and can provide
// a guideline in selecting the most suitable ADC-DAC system for data transmission.
//
// Note :
// ------
// The resulting analog values will replace the corresponding values of the i-th variable.
// For investigation of the performance of various ADC-DAC scenarios without modification
// of the current NcSample data, the user can invoke the Transmit() or ADC() and DAC()
// processors of the class NcDSP.
//
// In case the index "i" is out of range for this sample or no data is available, just the specs
// of the specified ADC performance will be printed and no digitization is performed, but the
// return argument will contain the adc value corresponding to Vbias.
// This allows to test various scenarios without modifying the data.
// In all other cases the unphysical value -1 is returned.
//
// Note : Make sure to use the same units for "range" and "Vbias" as those of the i-th variable.
//
// For further details, please refer to the documentation of the memberfunctions ADC() and DAC() of class NcDSP.
//
// Warning :
// ---------
// This member function will change the contents of this sample concerning the values of the i-th variable.
// In case access to the original values is required, one should make a copy of the original sample
// before the digitization process or use the corresponding member function Transmit() of class NcDSP.
//
// This facility is only available if the storage mode has been activated.
//
// In case of inconsistent input parameters, no digitization is performed.
//
// Input arguments :
// -----------------
// i        : Digitization of the i-th variable (first is i=1) will be performed.
// nbits    : Digital quantization was performed using an nbits ADC.
// range >0 : The full scale voltage range (Vfs) of the analog signal that corresponds to adc=N-1. 
//       <0 : |range| is the reference voltage (Vref) of the analog signal that corresponds to the hypothetical adc=N.
// Vbias    : The bias voltage that was added to the analog input signal before digitization.
//            If specified, the resulting analog signals will be corrected for the bias voltage.
//            For a linear DAC the correction via Vbias will only be performed if no pedestal array "peds"
//            is specified (see below). If the array "peds" is specified, Vbias will be ignored for a linear DAC.
//            For a Log DAC the pedestal values will never be used and bias correction may only be obtained via Vbias.
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
// The maximum number of bits that is supported is 60 to guarantee identical functioning on all machines.
//
// In case of inconsistent input parameters, no processing is performed.
//
// The default values are Vbias=0, peds=0, hist=0, B=0 and C=3.
~~~
**/

 TArrayL64 adcarr;
 TArrayD dacarr;
 if (hist) hist->Reset();

 if (!fStore)
 {
  printf("\n *%-s::Transmit* Error : Storage mode has not been activated. \n",ClassName());
  printf(" No digitization is performed. \n");
  return -1;
 }

 if (i<1 || i>fDim)
 {
  printf("\n *%-s::Transmit* Invalid variable name or index (i=%-i) --> Only listing of ADC specs. \n",ClassName(),i);
  adcarr=ADC(nbits,range,Vbias,0,0,B,C);
  return adcarr[0];
 }
 
 if (!fN)
 {
  printf("\n *%-s::Transmit* No data available for variable %-s (index=%-i) --> Only listing of ADC specs. \n",ClassName(),fNames[i-1].Data(),i);
  adcarr=ADC(nbits,range,Vbias,0,0,B,C);
  return adcarr[0];
 }

 Double_t minval=GetMinimum(i);
 Double_t Vin=minval+Vbias;

 if (B && Vin<=0)
 {
  TString mode="Log_e";
  if (B>1)
  {
   mode="Log_";
   mode+=B;
  }
  printf("\n *%-s::Transmit* Non-positive input value(s) for %-s ADC --> No digitization is performed. \n",ClassName(),mode.Data());
  printf(" Minimum+Vbias=%-g+%-g=%-g for variable with name %-s (index i=%-i). \n",minval,Vbias,Vin,fNames[i-1].Data(),i);
  return -1;
 }

 // Get the corresponding data array
 TArrayD* arrin=0;
 if (i==1) arrin=fX;
 if (i==2) arrin=fY;
 if (i==3) arrin=fZ;
 if (i==4) arrin=fT;

 // Perform the digitization via the ADC processeor
 adcarr=ADC(nbits,range,Vbias,arrin,0,B,C);

 // Convert the digital data into analog signals via the DAC processor
 dacarr=DAC(nbits,range,Vbias,&adcarr,peds,hist,B,C);

 if (hist)
 {
  TString title=hist->GetTitle();
  title.ReplaceAll("DAC","ADC-DAC (Transmit)");
  hist->SetTitle(title);
 }

 Int_t ndac=dacarr.GetSize();
 if (ndac<fN)
 {
  printf("\n *%-s::Transmit* Inconsistent number of entries : Nsamplings=%-i Ndac=%-i --> No digitization is performed. \n",ClassName(),fN,ndac);
  return -1;
 }

 // Replace the values of the i-th variable
 Double_t val=0;
 for (Int_t j=0; j<fN; j++)
 {
  val=dacarr[j];
  arrin->AddAt(val,j);
 }

 return -1;
}
///////////////////////////////////////////////////////////////////////////
Long64_t NcSample::Transmit(TString name,Int_t nbits,Double_t range,Double_t Vbias,TArray* peds,TH1* hist,Int_t B,Int_t C)
{
/**
~~~
// Mimic signal transmission according to an "nbits" ADC-DAC chain.
// The values of the variable "name" are digitized via the discrete quantization levels of an "nbits" ADC,
// based on the "range" for the analog signal and a bias voltage "Vbias" or array "peds" of pedestal values (see below).
// After digitization, the digital signals are converted into analog signals via the corresponding "nbits" DAC.
// In this way the effect of digitization on the original input signals can be investigated, and can provide
// a guideline in selecting the most suitable ADC-DAC system for data transmission.
//
// Note :
// ------
// The resulting analog values will replace the corresponding values of the variable "name".
// For investigation of the performance of various ADC-DAC scenarios, the user can also   
// invoke the Transmit() or ADC() and DAC() processors of the class NcDSP.
// This will not modify the current NcSample data, and actually these NcDSP processors
// are used here to perform the various calculations.
//
// In case the variable "name" does not exist for this sample or no data is available, just the specs
// of the specified ADC performance will be printed and no digitization is performed, but the
// return argument will contain the adc value corresponding to Vbias.
// This allows to test various scenarios without modifying the data.
// In all other cases the unphysical value -1 is returned.
//
// Note : Make sure to use the same units for "range" and "Vbias" as those of the i-th variable.
//
// For further details, please refer to the documentation of the memberfunctions ADC() and DAC() of class NcDSP.
//
// Warning :
// ---------
// This member function will change the contents of this sample concerning the values of variable "name".
// In case access to the original values is required, one should make a copy of the original sample
// before the digitization process or use the corresponding member function Transmit() of class NcDSP.
//
// This facility is only available if the storage mode has been activated.
//
// In case of inconsistent input parameters, no digitization is performed.
//
// Input arguments :
// -----------------
// name     : Name of the variable for which the digitization will be performed.
// nbits    : Digital quantization was performed using an nbits ADC.
// range >0 : The full scale voltage range (Vfs) of the analog signal that corresponds to adc=N-1. 
//       <0 : |range| is the reference voltage (Vref) of the analog signal that corresponds to the hypothetical adc=N.
// Vbias    : The bias voltage that was added to the analog input signal before digitization.
//            If specified, the resulting analog signals will be corrected for the bias voltage.
//            For a linear DAC the correction via Vbias will only be performed if no pedestal array "peds"
//            is specified (see below). If the array "peds" is specified, Vbias will be ignored for a linear DAC.
//            For a Log DAC the pedestal values will never be used and bias correction may only be obtained via Vbias.
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
// The maximum number of bits that is supported is 60 to guarantee identical functioning on all machines.
//
// In case of inconsistent input parameters, no processing is performed.
//
// The default values are Vbias=0, peds=0, hist=0, B=0 and C=3.
~~~
**/

 Int_t i=GetIndex(name);

 return Transmit(i,nbits,range,Vbias,peds,hist,B,C);
}
///////////////////////////////////////////////////////////////////////////
NcSample NcSample::SampleAndHold(TF1 f,Double_t step,Double_t vmin,Double_t vmax,Int_t loc) const
{
/**
~~~
// Perform a Sample-And-Hold operation on the specified function "f"
// in the interval [vmin,vmax], using "step" as the sampling step size.
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
// The default value is loc=-1.
~~~
**/

 NcSample s("SampleAndHold","Error occurred in SampleAndHold");
 s.SetStoreMode(1);

 if (step<=0 || vmax<=vmin)
 {
  cout << " *NcSample::SampleAndHold* Error : Invalid input step=" << step << " vmin=" << vmin << " vmax=" << vmax << endl;
  return s;
 }
 
 TString str="For Function ";
 str+=f.GetExpFormula("p");
 str+=" with step=%-10.3g";
 TString str2=str.Format(str.Data(),step);
 s.SetTitle(str2.Data());

 // Enter the sampled data into the new sample
 Double_t xlow=vmin;
 Double_t xval=0;
 Double_t yval=0;
 while (xlow<=vmax)
 {
  yval=f.Eval(xlow);
  if (loc<0) xval=xlow;
  if (!loc) xval=xlow+step/2.;
  if (loc>0) xval=xlow+step;
  if (xval>vmax) xval=vmax;
  s.Enter(xval,yval);

  xlow+=step;
 }

 return s;
}
///////////////////////////////////////////////////////////////////////////
NcSample NcSample::SampleAndSum(Int_t i,Double_t step,Int_t loc,Int_t j,Double_t vmin,Double_t vmax)
{
/**
~~~
// Perform a Sample-And-Sum operation on the values of the i-th variable
// in the interval [vmin,vmax], using "step" as the sampling step size.
// If vmax<=vmin the minimum and maximum values of the i-th variable will be used.
//
// This procedure resembles a Sample-And-Hold operation, but instead of locking
// the data recording during the stepsize "step", the data that appear within "step"
// are summed.
//
// If the i-th variable can be regarded as sampling in time, this mimics a 
// Switched Capacitor Array (SCA) with a time gate of "step" time units,
// or equivalently a sampling frequency of 1/step.
//
// The input argument "loc" determines whether the new values of the i-th variable
// will be recorded at the begin (loc<0), center (loc=0) or end (loc>0)
// of the sampling step size.
//
// If j>0 the corresponding value of variable j will be used as a weight.
// The first variable has index 1.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are loc=0, j=0, vmin=0 and vmax=-1.
~~~
**/

 NcSample s("SampleAndSum","Error occurred in SampleAndSum");
 s.SetStoreMode(1);

 if (!fStore)
 {
  cout << " *NcSample::SampleAndSum* Error : Storage mode has not been activated." << endl;
  return s;
 }

 if (i<1 || i>fDim || j>fDim)
 {
  cout << " *NcSample::SampleAndSum* Error : Invalid index encountered i=" << i << " j=" << j << endl;
  return s;
 }

 if (step<=0)
 {
  cout << " *NcSample::SampleAndSum* Error : Invalid step size step=" << step << endl;
  return s;
 }

 TString xname=fNames[i-1];
 TString yname="Counts";
 if (j>0) yname=fNames[j-1];

 s.SetNames(xname,yname);

 TString str="For Variable ";
 str+=xname;
 str+=" with step=%-10.3g";
 TString str2=str.Format(str.Data(),step);
 s.SetTitle(str2.Data());

 // Define a histogram with "step" as binsize
 Double_t xmin=GetMinimum(i);
 Double_t xmax=GetMaximum(i);

 if (vmax>vmin)
 {
  xmin=vmin;
  xmax=vmax;
 }

 Int_t nbx=(xmax-xmin)/step;
 nbx++;

 Double_t* bins=new Double_t[nbx+1];
 Double_t xlow=xmin;
 for (Int_t idx=0; idx<=nbx; idx++)
 {
  bins[idx]=xlow;
  xlow+=step;
 }

 TH1D hist("","",nbx,bins);

 // Fill the histogram with data of the original sample
 Double_t xval=0;
 Double_t yval=1;
 for (Int_t ient=1; ient<=fN; ient++)
 {
  xval=GetEntry(ient,i);
  if (j>0) yval=GetEntry(ient,j);
  hist.Fill(xval,yval);
 }

 // Enter the histgram data into the new sample
 Int_t nbins=hist.GetNbinsX();
 Double_t binwidth=hist.GetBinWidth(1);
 for (Int_t k=1; k<=nbins; k++)
 {
  if (loc) xval=hist.GetBinLowEdge(k);
  if (!loc) xval=hist.GetBinCenter(k);
  if (loc>0) xval+=binwidth;
  yval=hist.GetBinContent(k);
  s.Enter(xval,yval);
 }

 delete [] bins;
 return s;
}
///////////////////////////////////////////////////////////////////////////
NcSample NcSample::SampleAndSum(TString nameA,Double_t step,Int_t loc,TString nameB,Double_t vmin,Double_t vmax)
{
/**
~~~
// Perform a Sample-And-Sum operation on the values of the variable specified with nameA
// in the interval [vmin,vmax], using "step" as the sampling step size.
// If vmax<=vmin the minimum and maximum values of the variable with nameA will be used.
//
// This procedure resembles a Sample-And-Hold operation, but instead of locking
// the data recording during the stepsize "step", the data that appear within "step"
// are summed.
//
// If the variable with nameA can be regarded as sampling in time, this mimics a 
// Switched Capacitor Array (SCA) with a time gate of "step" time units,
// or equivalently a sampling frequency of 1/step.
//
// The input argument "loc" determines whether the new values of the variable with nameA
// will be recorded at the begin (loc<0), center (loc=0) or end (loc>0)
// of the sampling step size.
//
// If nameB is valid, the corresponding value of that variable will be used as a weight.
//
// Note : This facility is only available if the storage mode has been activated.
//
// The default values are loc=0, nameB="-", vmin=0 and vmax=-1.
~~~
**/

 Int_t i=GetIndex(nameA);
 Int_t j=GetIndex(nameB);

 return SampleAndSum(i,step,loc,j,vmin,vmax);
}
///////////////////////////////////////////////////////////////////////////
TObject* NcSample::Clone(const char* name) const
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

 NcSample* q=new NcSample(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
