#ifndef NcTransform_h
#define NcTransform_h

// Copyright(c) 1997-2021, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "TArrayD.h"
#include "TVirtualFFT.h"

#include "NcSample.h"

class NcTransform : public TNamed
{
 public:
  NcTransform(const char* name="",const char* title="");       // Default constructor
  virtual ~NcTransform();                                      // Default destructor
  NcTransform(const NcTransform& q);                           // Copy constructor
  virtual TObject* Clone(const char* name="") const;           // Make a deep copy and provide its pointer
  void SetSamplingFrequency(Float_t f);                        // Set the actual DAQ sampling frequency in Hz
  Float_t GetSamplingFrequency() const;                        // Provide the current DAQ sampling frequency in Hz
  void Load(Int_t n,Double_t* re,Double_t* im=0,Float_t f=-1); // Provide new input data to be transformed
  void Load(TArray* re,TArray* im=0,Float_t f=-1);             // Provide new input data to be transformed
  void Load(NcSample* s,Int_t i,Float_t f=-1);                 // Provide new input data to be transformed
  void Load(NcSample* s,TString name,Float_t f=-1);            // Provide new input data to be transformed
  void Load(TH1* h,Float_t f=-1);                              // Provide new input data to be transformed
  void Load(TGraph* gr,Float_t f=-1);                          // Provide new input data to be transformed
  void LoadResult();                                           // Load the transformation result as new input data
  void Fourier(TString mode,TH1* hist=0,TString sel="none");   // Perform a 1-dimensional Discrete Fourier Transform (DFT)
  void Hartley(Int_t mode,TH1* hist=0,TString sel="none");     // Perform a 1-dimensional Discrete Hartley Transform (DHT)
  void Cosine(Int_t type,TH1* hist=0,TString sel="none");      // Perform a 1-dimensional Discrete Cosine Transform (DCT)
  void Sine(Int_t type,TH1* hist=0,TString sel="none");        // Perform a 1-dimensional Discrete Sine Transform (DST)
  Int_t GetN() const;                                          // Provide the number of data elements (to be) processed.
  TArrayD GetData(TString mode) const;                         // Provide a selected set of data

 protected:
  TVirtualFFT* fProc; // The FFTW processor
  Int_t fN;           // The number of data elements to be transformed
  TArrayD fReIn;      // Input array for real components
  TArrayD fImIn;      // Input array for imaginary components
  TArrayD fReOut;     // Output array for real components
  TArrayD fImOut;     // Output array for imaginary components
  Float_t fSample;    // The actual data acquisition sampling frequency in Hz

  // Internal member functions
  void Reset();      // Reset all data and the processor

 ClassDef(NcTransform,1) // Various transformations of (sequential) data samples
};
#endif
