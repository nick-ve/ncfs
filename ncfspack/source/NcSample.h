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
#include "TGraphErrors.h"
#include "TGraph2DErrors.h"
#include "TGraphTime.h"
#include "TF1.h"
#include "TGraphQQ.h"
#include "TMarker.h"
#include "TCanvas.h"
#include "TSystem.h"
 
class NcSample : public TNamed
{
 public:
  NcSample(const char* name="",const char* title=""); // Default constructor
  virtual ~NcSample();                                // Default destructor
  NcSample(const NcSample& s);                        // Copy constructor
  virtual TObject* Clone(const char* name="") const;  // Make a deep copy and provide its pointer
  void Reset();                                       // Reset complete statistics
  void SetNames(TString name1="X",TString name2="Y",TString name3="Z",TString name4="T"); // Set the variable names
  void Enter(Double_t x);                             // Enter value for 1-dim. sample
  void Remove(Double_t x);                            // Remove value from 1-dim. sample
  void Enter(Double_t x,Double_t y);                  // Enter values for 2-dim. sample
  void Remove(Double_t x,Double_t y);                 // Remove values from 2-dim. sample
  void Enter(Double_t x,Double_t y,Double_t z);       // Enter values for 3-dim. sample
  void Remove(Double_t x,Double_t y,Double_t z);      // Remove values from 3-dim. sample
  void Enter(Double_t x,Double_t y,Double_t z,Double_t t);  // Enter values for 4-dim. sample
  void Remove(Double_t x,Double_t y,Double_t z,Double_t t); // Remove values from 4-dim. sample
  void RemoveEntry(Int_t i,Int_t mode,Int_t j);       // Remove the i-th entry after ordering according to the j-th variable
  void RemoveEntry(Int_t i,Int_t mode,TString name);  // Remove the i-th entry after ordering according to the variable with the specified name
  void RemoveEntry(Int_t i,Int_t j,Int_t mode,Int_t k);      // Remove the entries in the interval [i,j] after ordering according to the k-th variable
  void RemoveEntry(Int_t i,Int_t j,Int_t mode,TString name); // Remove the entries in the interval [i,j] after ordering according to the variable with the specified name
  Int_t GetDimension() const;                         // Provide dimension of the sample
  Int_t GetN() const;                                 // Provide the number of entries
  TString GetVariableName(Int_t i) const;             // Provide the name of the i-th variable
  Int_t GetIndex(TString name) const;                 // Provide the index of the specified variable name
  Double_t GetSum(Int_t i) const;                     // Provide sum for i-th variable
  Double_t GetSum(TString name) const;                // Provide sum for the variable with the specified name
  Double_t GetMean(Int_t i) const;                    // Provide mean for i-th variable
  Double_t GetMean(TString name) const;               // Provide mean for the variable with the specified name
  Double_t GetRMS(Int_t i) const;                     // Provide Root Mean Square for i-th variable
  Double_t GetRMS(TString name) const;                // Provide Root Mean Square for the variable with the specified name
  Double_t GetVar(Int_t i,Int_t model=0) const;       // Provide variance for i-th variable
  Double_t GetVar(TString name,Int_t model=0) const;  // Provide variance for the variable with the specified name
  Double_t GetSigma(Int_t i,Int_t model=0) const;     // RMS or standard deviation for i-th variable
  Double_t GetSigma(TString name,Int_t model=0) const; // RMS or standard deviation for variable with the specified name
  Double_t GetCov(Int_t i,Int_t j) const;             // Covariance for i-th and j-th variable
  Double_t GetCov(TString nameA,TString nameB) const; // Covariance for variables with the specified names
  Double_t GetCor(Int_t i,Int_t j) const;             // Correlation for i-th and j-th variable
  Double_t GetCor(TString nameA,TString nameB) const; // Correlation for variables with the specified names
  Double_t GetQuantile(Double_t f,Int_t i);           // Quantile value of the i-th variable for the fraction f
  Double_t GetQuantile(Double_t f,TString name);      // Quantile value of variable with the specified name for the fraction f
  Double_t GetMedian(Int_t i);                        // Provide median for the i-th variable
  Double_t GetMedian(TString name);                   // Provide median for the variable with the specified name
  Double_t GetSpread(Int_t i,Int_t model=0,Double_t vref=0); // Provide spread w.r.t. some reference value for the i-th variable
  Double_t GetSpread(TString name,Int_t model=0,Double_t vref=0); // Provide spread w.r.t. some reference value for the variable with the specified name
  Double_t GetMinimum(Int_t i) const;                 // Provide the minimum value for the i-th variable
  Double_t GetMinimum(TString name) const;            // Provide the minimum value for the variable with the specified name
  Double_t GetMaximum(Int_t i) const;                 // Provide the maximum value for the i-th variable
  Double_t GetMaximum(TString name) const;            // Provide the maximum value for the variable with the specified name
  void Data(Int_t i=0,Int_t j=0);                     // Statistics info for the sample
  void Data(TString nameA,TString nameB="-");         // Statistics info for the sample
  void ListOrdered(Int_t mode,Int_t i);               // Provide a listing of the stored entries ordered w.r.t. the i-th variable
  void ListOrdered(Int_t mode,TString name);          // Provide a listing of the stored entries ordered w.r.t. the variable with the specified name
  void SetStoreMode(Int_t mode=1,Int_t nmax=0,Int_t i=0); // Set mode for storage of entered data
  Int_t GetStoreMode() const;                         // Provide storage mode of entered data
  Int_t GetStoreNmax() const;                         // Provide the maximum number of entries for storage
  Double_t GetQuantile(Double_t f,TH1* histo,Int_t mode=0) const; // Quantile value for fraction f from the specified 1D histogram
  Double_t GetMedian(TH1* histo,Int_t mode=0) const;  // Provide median from the specified 1D histogram
  Double_t GetSpread(TH1* histo,Int_t mode=0,Int_t model=0,Double_t vref=0) const; // Provide spread w.r.t. some reference value from the 1D histo
  Double_t GetEntry(Int_t i,Int_t j,Int_t mode=0,Int_t k=0); // Access the i-th (ordered) entry and provide the value of the j-th variable
  Double_t GetEntry(Int_t i,TString nameA,Int_t mode=0,TString nameB="-"); // Access the i-th (ordered) entry and provide the value of the variable with nameA
  void GetSubset(NcSample* s,Int_t ifirst,Int_t ilast,Int_t mode=0,Int_t k=0); // Provide the [ifirst,ilast] (ordered) entries via NcSample "s"
  TH1D Get1DHistogram(Int_t i,Int_t j=0,Bool_t sumw2=kFALSE,Int_t nbx=100,TF1* f=0); // Provide a TH1D for variable i (weighted with (a function of) variable j)
  TH1D Get1DHistogram(TString nameA,TString nameB="-",Bool_t sumw2=kFALSE,Int_t nbx=100,TF1* f=0); // Provide a TH1D for variable nameA (weighted with (a function of) variable nameB)
  TH2D Get2DHistogram(Int_t i,Int_t j,Int_t k=0,Bool_t sumw2=kFALSE,Int_t nbx=100,Int_t nby=100,TF1* f=0); // Provide a TH2D for variables i and j (weighted with (a function of) variable k)
  TH2D Get2DHistogram(TString nameA,TString nameB,TString nameC="-",Bool_t sumw2=kFALSE,Int_t nbx=100,Int_t nby=100,TF1* f=0); // Provide a TH2D for variables nameA and nameB (weighted with (a function of) variable nameC)
  TH3D Get3DHistogram(Int_t i,Int_t j,Int_t k,Int_t m=0,Bool_t sumw2=kFALSE,Int_t nbx=100,Int_t nby=100,Int_t nbz=100,TF1* f=0); // Provide a TH3D with values of variables i, j and k (weighted with (a function of) variable m)
  TH3D Get3DHistogram(TString nameA,TString nameB,TString nameC,TString nameD="-",Bool_t sumw2=kFALSE,Int_t nbx=100,Int_t nby=100,Int_t nbz=100,TF1* f=0); // Provide a TH3D for variables nameA, nameB and nameC (weighted with (a function of) variable nameD)
  TGraph GetGraph(Int_t i,TF1* f=0);                     // Provide a TGraph with values of (a function of) variable i vs. the sampling entry number
  TGraph GetGraph(TString nameA,TF1* f=0);               // Provide a TGraph for (a function of) variable nameA vs. the sampling entry number
  TGraph GetGraph(Int_t i,Int_t j,TF1* f=0);             // Provide a TGraph with values of (a function of) variable j vs. variable i
  TGraph GetGraph(TString nameA,TString nameB,TF1* f=0); // Provide a TGraph for (a function of) variable nameB vs. variable nameA
  TGraphErrors GetGraphErrors(TGraph* g,Int_t ix=0,Int_t iy=0,TF1* fx=0,TF1* fy=0);       // Provide a Graph with new c.q. modified errors
  TGraphErrors GetGraphErrors(TGraph* g,TString nameA,TString nameB,TF1* fx=0,TF1* fy=0); // Provide a Graph with new c.q. modified errors
  TGraphTime* GetGraph(Int_t i,Int_t j,Int_t mode,Int_t k,Bool_t smp=kTRUE);                   // Provide a TGraphTime with (ordered) values of variables i and j
  TGraphTime* GetGraph(TString nameA,TString nameB,Int_t mode,TString nameC,Bool_t smp=kTRUE); // Provide a TGraphTime with (ordered) values of variables nameA and nameB
  TGraph2D GetGraph(Int_t i,Int_t j,Int_t k,TF1* f=0);                   // Provide a TGraph2D with values of (a function of) variable k vs. i and j
  TGraph2D GetGraph(TString nameA,TString nameB,TString nameC,TF1* f=0); // Provide a TGraph2D for (a function of) variable nameC vs. nameA and nameB
  TGraph2DErrors GetGraph2DErrors(TGraph2D* g,Int_t ix=0,Int_t iy=0,Int_t iz=0,TF1* fx=0,TF1* fy=0,TF1* fz=0);          // Provide a Graph2D with new c.q. modified errors
  TGraph2DErrors GetGraph2DErrors(TGraph2D* g,TString nameA,TString nameB,TString nameC,TF1* fx=0,TF1* fy=0,TF1* fz=0); // Provide a Graph2D with new c.q. modified errors
  TGraphQQ GetQQplot(Int_t i,Int_t j,TF1* f=0);             // Provide a TGraphQQ for the values of variables i and j or the function f replacing j 
  TGraphQQ GetQQplot(TString nameA,TString nameB,TF1* f=0); // Provide a TGraphQQ for variables nameA and nameB or the function f replacing nameB
  void Load(TGraph* g,Int_t clr=1);                   // Load the data points of a TGraph object as 2-dimensional (x,y) data.
  void Load(TGraph2D* g,Int_t clr=1);                 // Load the data points of a TGraph2D object as 3-dimensional (x,y,z) data.
  Double_t GetSNR(Int_t i,Int_t mode=2,Bool_t db=kTRUE) const; // Provide the Signal to Noise ratio for the i-th variable
  Double_t GetSNR(TString name,Int_t mode=2,Bool_t db=kTRUE) const; // Provide the Signal to Noise ratio for the variable with the specified name
  Double_t GetCV(Int_t i,Int_t model=0) const;        // Provide the Coefficient of Variation for the i-th variable
  Double_t GetCV(TString name,Int_t model=0) const;   // Provide the Coefficient of Variation for the variable with the specified name
  void Animation(Int_t i,Int_t j,Int_t mode,Int_t k,Int_t delay,TString opt="AP"); // Animation of an (ordered) sampling of the values of variables i and j
  void Animation(TString nameA,TString nameB,Int_t mode,TString nameC,Int_t delay,TString opt="AP"); // Animation of an (ordered) sampling of the values of variables nameA and nameB
  void Animation(Int_t i,Int_t j,Int_t k,Int_t mode,Int_t m,Int_t delay,TString opt="PFB"); // Animation of an (ordered) sampling of the values of variables i, j and k
  void Animation(TString nameA,TString nameB,TString nameC,Int_t mode,TString nameD,Int_t delay,TString opt="PFB"); // Animation of an (ordered) sampling of the values of variables nameA, nameB and nameC
  Double_t Digitize(Int_t i,Int_t nbits,Double_t vcal,Int_t mode); // Digitize the values of the i-th variable with an "nbits" ADC
  Double_t Digitize(TString name,Int_t nbits,Double_t vcal,Int_t mode); // Digitize the values of the specified variable with an "nbits" ADC
  NcSample SampleAndHold(TF1 f,Double_t step,Double_t vmin,Double_t vmax,Int_t loc=-1) const; // Perform a Sample-And-Hold operation on the specified function
  NcSample SampleAndSum(Int_t i,Double_t step,Int_t loc=0,Int_t j=0,Double_t vmin=0,Double_t vmax=-1); // Perform a Sample-And-Hold operation on the values of the i-th variable
  NcSample SampleAndSum(TString nameA,Double_t step,Int_t loc=0,TString nameB="-",Double_t vmin=0,Double_t vmax=-1); // Perform a Sample-And-Hold operation on the values of the variable nameA
 
 protected:
  Int_t fDim;                       // Dimension of the sample
  Int_t fN;                         // Number of entries of the sample
  enum  {fMaxdim=4};                // Maximum supported dimension
  TString fNames[fMaxdim];          // Variable names e.g. X,Y,Z,T
  Double_t fSum[fMaxdim];           // Total sum for each variable
  Double_t fSum2[fMaxdim][fMaxdim]; // Total sum**2 for each variable
  Double_t fMean[fMaxdim];          // Mean for each variable
  Double_t fRMSdev[fMaxdim];        // Root Mean Square deviation for each variable
  Double_t fSigma[fMaxdim];         // Standard deviation for each variable
  Double_t fCov[fMaxdim][fMaxdim];  // Covariances of pairs of variables
  Double_t fCor[fMaxdim][fMaxdim];  // Correlations of pairs of variables
  Double_t fMin[fMaxdim];           // Minimum value for each variable
  Double_t fMax[fMaxdim];           // Maximum value for each variable
  Int_t fRemove;                    // Flag to indicate that some entry has been removed
  Int_t fStore;                     // Flag to denote storage of entered data 
  Int_t fNmax;                      // Maximum number of entries to be stored before moving some out
  Int_t fMoveVariable;              // Variable to order before moving out entries  
  TArrayD* fX;                      // Storage array for the 1st variable (e.g. X)
  TArrayD* fY;                      // Storage array for the 2nd variable (e.g. Y)
  TArrayD* fZ;                      // Storage array for the 3rd variable (e.g. Z)
  TArrayD* fT;                      // Storage array for the 4th variable (e.g. T)
  TArrayD* fArr;                    //! Temp. storage array for ordered values
  TArrayI* fIndices;                //! Temp. storage array for the indices of the ordered entries
  Int_t fOrdered;                   //! Indicator of the status of the current ordering
  TGraphTime* fGraphT;              //! Temp. pointer to return a TGraphTime object
  TCanvas* fCanvas;                 //! Multi-purpose canvas for e.g. animation displays
  TObject* fAnimObject;             //! Multi-purpose pointer for animation objects

  void  Compute();                    // Compute the various quantities
  void Order(Int_t mode,Int_t i);     // Order the entries according to the i-th variable
  void List(Int_t i);                 // Statistics info for the i-th variable
  void List(Int_t i,Int_t j) const;   // Correlation statistics info for i-th and j-th variable

 ClassDef(NcSample,12) // Sampling and statistics tools for various multi-dimensional data samples.
};
#endif
