#ifndef NcSample_h
#define NcSample_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <math.h>

#include "TNamed.h"
#include "TArrayD.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TGraphTime.h"
#include "TMarker.h"
 
class NcSample : public TNamed
{
 public:
  NcSample(const char* name="",const char* title=""); // Default constructor
  virtual ~NcSample();                                // Default destructor
  NcSample(const NcSample& s);                        // Copy constructor
  virtual TObject* Clone(const char* name="") const;  // Make a deep copy and provide its pointer
  void Reset();                                       // Reset complete statistics
  void Enter(Double_t x);                             // Enter value for 1-dim. sample
  void Remove(Double_t x);                            // Remove value from 1-dim. sample
  void Enter(Double_t x,Double_t y);                  // Enter values for 2-dim. sample
  void Remove(Double_t x,Double_t y);                 // Remove values from 2-dim. sample
  void Enter(Double_t x,Double_t y,Double_t z);       // Enter values for 3-dim. sample
  void Remove(Double_t x,Double_t y,Double_t z);      // Remove values from 3-dim. sample
  void Enter(Double_t x,Double_t y,Double_t z,Double_t t);  // Enter values for 4-dim. sample
  void Remove(Double_t x,Double_t y,Double_t z,Double_t t); // Remove values from 4-dim. sample
  void RemoveEntry(Int_t i,Int_t mode,Int_t j);       // Remove the i-th entry after ordering according to the j-th variable
  void Order(Int_t mode,Int_t i);                     // Order the entries according to the i-th variable
  Int_t GetDimension() const;                         // Provide dimension of the sample
  Int_t GetN() const;                                 // Provide the number of entries
  Double_t GetSum(Int_t i) const;                     // Provide sum for i-th variable
  Double_t GetMean(Int_t i) const;                    // Provide mean for i-th variable
  Double_t GetRMS(Int_t i) const;                     // Provide Root Mean Square for i-th variable
  Double_t GetVar(Int_t i,Int_t model=0) const;       // Provide variance for i-th variable
  Double_t GetSigma(Int_t i,Int_t model=0) const;     // RMS or standard deviation for i-th variable
  Double_t GetCov(Int_t i,Int_t j) const;             // Covariance for i-th and j-th variable
  Double_t GetCor(Int_t i,Int_t j) const;             // Correlation for i-th and j-th variable
  Double_t GetQuantile(Double_t f,Int_t i);           // Quantile value of i-th variable for fraction f
  Double_t GetMedian(Int_t i);                        // Provide median for i-th variable
  Double_t GetSpread(Int_t i,Int_t model=0,Double_t vref=0); // Provide spread w.r.t. some reference value for i-th variable
  Double_t GetMinimum(Int_t i) const;                 // Provide the minimum value for i-th variable
  Double_t GetMaximum(Int_t i) const;                 // Provide the maximum value for i-th variable
  void Data(Int_t i=0,Int_t j=0);                     // Statistics info for the sample
  void ListOrdered(Int_t mode,Int_t i);               // Provide a listing of the stored entries ordered w.r.t. the i-th variable
  void SetStoreMode(Int_t mode=1);                    // Set mode for storage of all entered data
  Int_t GetStoreMode() const;                         // Provide storage mode of all entered data
  Double_t GetQuantile(Double_t f,TH1* histo,Int_t mode=0) const; // Quantile value for fraction f from the specified 1D histogram
  Double_t GetMedian(TH1* histo,Int_t mode=0) const;  // Provide median from the specified 1D histogram
  Double_t GetSpread(TH1* histo,Int_t mode=0,Int_t model=0,Double_t vref=0) const; // Provide spread w.r.t. some reference value from the 1D histo
  Double_t GetEntry(Int_t i,Int_t j,Int_t mode=0,Int_t k=-1); // Access the i-th (ordered) entry and provide the value of the j-th variable
  TH1D Get1DHistogram(Int_t i,Int_t j=0,Bool_t sumw2=kFALSE,Int_t nbx=100); // Provide a TH1D with values of variable i (weighted with variable j)
  TH2D Get2DHistogram(Int_t i,Int_t j,Int_t k=0,Bool_t sumw2=kFALSE,Int_t nbx=100,Int_t nby=100); // Provide a TH2D with values of variables i and j (weighted with variable k)
  TH3D Get3DHistogram(Int_t i,Int_t j,Int_t k,Int_t m=0,Bool_t sumw2=kFALSE,Int_t nbx=100,Int_t nby=100,Int_t nbz=100); // Provide a TH3D with values of variables i, j and k (weighted with variable m)
  TGraph GetGraph(Int_t i,Int_t j);                   // Provide a TGraph with values of variables i and j
  TGraphTime* GetGraph(Int_t i,Int_t j,Int_t mode,Int_t k,Bool_t smp=kTRUE); // Provide a TGraphTime with (ordered) values of variables i and j
  TGraph2D GetGraph(Int_t i,Int_t j,Int_t k);         // Provide a TGraph2D with values of variables i, j and k
  void Load(TGraph* g,Int_t clr=1);                   // Load the data points of a TGraph object as 2-dimensional (x,y) data.
  void Load(TGraph2D* g,Int_t clr=1);                 // Load the data points of a TGraph2D object as 3-dimensional (x,y,z) data.
  Double_t GetSNR(Int_t i,Int_t mode=2,Bool_t db=kTRUE) const; // Provide the Signal to Noise ratio for the i-th variable
  Double_t GetCV(Int_t i,Int_t model=0) const;        // Provide the Coefficient of Variation for the i-th variable
 
 protected:
  Int_t fDim;                       // Dimension of the sample
  Int_t fN;                         // Number of entries of the sample
  enum  {fMaxdim=4};                // Maximum supported dimension
  char  fNames[fMaxdim];            // Variable names i.e. X,Y,Z,T
  Double_t fSum[fMaxdim];           // Total sum for each variable
  Double_t fSum2[fMaxdim][fMaxdim]; // Total sum**2 for each variable
  void  Compute();                  // Compute the various quantities
  Double_t fMean[fMaxdim];          // Mean for each variable
  Double_t fRMSdev[fMaxdim];        // Root Mean Square deviation for each variable
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
  TArrayD* fT;                      // Storage array for the 4th variable (e.g. T)
  TArrayD* fArr;                    //! Temp. storage array for ordered values
  TArrayI* fIndices;                //! Temp. storage array for the indices of the ordered entries
  Int_t fOrdered;                   //! Indicator of the status of the current ordering
  TGraphTime* fGraphT;              //! Temp. pointer to return a TGraphTime object

  void List(Int_t i);               // Statistics info for the i-th variable
  void List(Int_t i,Int_t j) const; // Correlation statistics info for i-th and j-th variable

 ClassDef(NcSample,2) // Statistics tools for various multi-dimensional data samples.
};
#endif
