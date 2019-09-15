#ifndef NcSample_h
#define NcSample_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcSample.h 69 2012-10-05 16:31:56Z nickve $

#include <math.h>

#include "Rtypes.h"
#include "TArrayD.h"
#include "TH1.h"
 
class NcSample
{
 public:
  NcSample();                                      // Default constructor
  virtual ~NcSample();                             // Default destructor
  void Reset();                                    // Reset complete statistics
  void Enter(Double_t x);                          // Enter value for 1-dim. sample
  void Remove(Double_t x);                         // Remove value from 1-dim. sample
  void Enter(Double_t x, Double_t y);              // Enter value for 2-dim. sample
  void Remove(Double_t x, Double_t y);             // Remove value from 2-dim. sample
  void Enter(Double_t x, Double_t y, Double_t z);  // Enter value for 3-dim. sample
  void Remove(Double_t x, Double_t y, Double_t z); // Remove value from 3-dim. sample
  Int_t GetDimension() const;                      // Provide dimension of the sample
  Int_t GetN() const;                              // Provide the number of entries
  Double_t GetSum(Int_t i) const;                  // Provide sum for i-th variable
  Double_t GetMean(Int_t i) const;                 // Provide mean for i-th variable
  Double_t GetVar(Int_t i) const;                  // Provide variance for i-th variable
  Double_t GetSigma(Int_t i) const;                // Standard deviation for i-th variable
  Double_t GetCov(Int_t i, Int_t j) const;         // Covariance for i-th and j-th variable
  Double_t GetCor(Int_t i, Int_t j) const;         // Correlation for i-th and j-th variable
  Double_t GetQuantile(Double_t f,Int_t i);        // Quantile value of i-th variable for fraction f
  Double_t GetMedian(Int_t i);                     // Provide median for i-th variable
  Double_t GetSpread(Int_t i);                     // Provide spread w.r.t. the median for i-th variable
  Double_t GetMinimum(Int_t i) const;              // Provide the minimum value for i-th variable
  Double_t GetMaximum(Int_t i) const;              // Provide the maximum value for i-th variable
  void Data();                                     // Stat. info for the complete sample
  void Data(Int_t i);                              // Stat. info for the i-th variable
  void Data(Int_t i, Int_t j) const;               // Stat. info for i-th and j-th variable
  void SetStoreMode(Int_t mode=1);                 // Set mode for storage of all entered data
  Int_t GetStoreMode() const;                      // Provide storage mode of all entered data
  Double_t GetQuantile(Double_t f,TH1* histo,Int_t mode=0) const; // Quantile value for fraction f from the specified 1D histogram
  Double_t GetMedian(TH1* histo,Int_t mode=0) const; // Provide median from the specified 1D histogram
  Double_t GetSpread(TH1* histo,Int_t mode=0) const; // Provide spread w.r.t. the median from the 1D histo
  Double_t GetEntry(Int_t i,Int_t j) const;        // Provide the value of the i-th entry for the j-th variable
 
 private:
  Int_t fDim;                       // Dimension of the sample
  Int_t fN;                         // Number of entries of the sample
  enum  {fMaxdim=3};                // Maximum supported dimension
  char  fNames[fMaxdim];            // Variable names i.e. X,Y,Z
  Double_t fSum[fMaxdim];           // Total sum for each variable
  Double_t fSum2[fMaxdim][fMaxdim]; // Total sum**2 for each variable
  void  Compute();                  // Compute the various quantities
  Double_t fMean[fMaxdim];          // Mean for each variable
  Double_t fVar[fMaxdim];           // Variation for each variable
  Double_t fSigma[fMaxdim];         // Standard deviation for each variable
  Double_t fCov[fMaxdim][fMaxdim];  // Covariances of pairs of variables
  Double_t fCor[fMaxdim][fMaxdim];  // Correlations of pairs of variables
  Double_t fMin[fMaxdim];           // Minimum value for each variable
  Double_t fMax[fMaxdim];           // Maximum value for each variable
  Int_t fRemove;                    // Flag to indicate that some entry has been removed
  Int_t fStore;                     // Flag to denote storage of all entered data 
  TArrayD* fX;                      // Storage array for the 1st variable (e.g. X)
  TArrayD* fY;                      // Storage array for the 2nd variable (e.g. Y)
  TArrayD* fZ;                      // Storage array for the 3rd variable (e.g. Z)
  TArrayD* fArr;                    // Temp. storage array for ordering

 ClassDef(NcSample,0) // Statistics tools for various multi-dimensional data samples.
};
#endif
