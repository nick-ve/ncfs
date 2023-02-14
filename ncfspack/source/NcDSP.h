#ifndef NcDSP_h
#define NcDSP_h

// Copyright(c) 2023, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "TArrayD.h"
#include "TVirtualFFT.h"
#include "TList.h"
#include "TLine.h"

#include "NcSample.h"

class NcDSP : public TNamed
{
 public:
  NcDSP(const char* name="",const char* title="");             // Default constructor
  virtual ~NcDSP();                                            // Default destructor
  NcDSP(const NcDSP& q);                                       // Copy constructor
  virtual TObject* Clone(const char* name="") const;           // Make a deep copy and provide its pointer
  void SetSamplingFrequency(Float_t f);                        // Set the actual DAQ sampling frequency in Hz
  Float_t GetSamplingFrequency() const;                        // Provide the current DAQ sampling frequency in Hz
  void Load(Int_t n,Double_t* re,Double_t* im=0,Float_t f=-1); // Provide new input data to be processed
  void Load(TArray* re,TArray* im=0,Float_t f=-1);             // Provide new input data to be processed
  void Load(NcSample* s,Int_t i,Float_t f=-1);                 // Provide new input data to be processed
  void Load(NcSample* s,TString name,Float_t f=-1);            // Provide new input data to be processed
  void Load(TH1* h,Float_t f=-1);                              // Provide new input data to be processed
  void Load(TGraph* gr,Float_t f=-1);                          // Provide new input data to be processed
  void LoadResult();                                           // Load the transformation result as new input data
  void SetWaveform(Int_t n,Double_t* h,Float_t f=-1);          // Set the (system response) waveform for Convolution, Correlation etc.
  void SetWaveform(TArray* h,Float_t f=-1);                    // Set the (system response) waveform for Convolution, Correlation etc.
  void SetWaveform(NcSample* s,Int_t i,Float_t f=-1);          // Set the (system response) waveform for Convolution, Correlation etc.
  void SetWaveform(NcSample* s,TString name,Float_t f=-1);     // Set the (system response) waveform for Convolution, Correlation etc.
  void SetWaveform(TH1* h,Float_t f=-1);                       // Set the (system response) waveform for Convolution, Correlation etc.
  void SetWaveform(TGraph* gr,Float_t f=-1);                   // Set the (system response) waveform for Convolution, Correlation etc.
  void Fourier(TString mode,TH1* hist=0,TString sel="none");   // Perform a 1-dimensional Discrete Fourier Transform (DFT)
  void Hartley(Int_t mode,TH1* hist=0,TString sel="none");     // Perform a 1-dimensional Discrete Hartley Transform (DHT)
  void Cosine(Int_t type,TH1* hist=0,TString sel="none");      // Perform a 1-dimensional Discrete Cosine Transform (DCT)
  void Sine(Int_t type,TH1* hist=0,TString sel="none");        // Perform a 1-dimensional Discrete Sine Transform (DST)
  TArrayD Convolve(TH1* hist=0,Int_t* i1=0,Int_t* i2=0);       // Convolve the loaded data with the stored waveform data
  TArrayD Correlate(TH1* hist=0,Int_t* i1=0,Int_t* i2=0);      // Correlate the stored waveform data with the loaded data
  TArrayD Digitize(Int_t nbits,Double_t vcal,Int_t mode,TH1* hist=0,Double_t* stp=0,Double_t* scale=0) const; // Digitize values according to an "nbits" ADC.
  TArrayD SampleAndHold(TF1 f,Double_t step,Double_t vmin,Double_t vmax,TH1* hist=0,Int_t loc=-1) const; // Perform a Sample-And-Hold operation on function "f"
  TArrayD SampleAndHold(Int_t ns,TH1* hist=0,Int_t loc=-1,Int_t jmin=0,Int_t jmax=-1) const; // Perform a Sample-And-Hold operation on the stored waveform data
  TArrayD SampleAndSum(TF1 f,Double_t step,Double_t vmin,Double_t vmax,TH1* hist=0) const;  // Perform a Sample-And-Sum operation on function "f"
  TArrayD SampleAndSum(Int_t ns,TH1* hist,Int_t jmin=0,Int_t jmax=-1) const; // Perform a Sample-And-Sum operation on the stored waveform data
  TArrayD FilterMovingAverage(Int_t n,TH1* hist=0,Int_t* i1=0,Int_t* i2=0); // Perform a Moving Average filter on the loaded data
  TArrayD FilterLowPass(Double_t fcut,Int_t n,TH1* hisf=0,TH1* hist=0,Int_t* i1=0,Int_t* i2=0);  // Perform a Low Pass filter on the loaded data
  TArrayD FilterHighPass(Double_t fcut,Int_t n,TH1* hisf=0,TH1* hist=0,Int_t* i1=0,Int_t* i2=0); // Perform a High Pass filter on the loaded data
  TArrayD FilterBandPass(Double_t f1,Double_t f2,Int_t n,TH1* hisf=0,TH1* hist=0,Int_t* i1=0,Int_t* i2=0); // Perform a Band Pass filter on the loaded data
  TArrayD FilterBandReject(Double_t f1,Double_t f2,Int_t n,TH1* hisf=0,TH1* hist=0,Int_t* i1=0,Int_t* i2=0); // Perform a Band Reject filter on the loaded data
  TArrayD FilterMultiBand(TArray& freqs,Int_t n,TH1* hisf=0,TH1* hist=0,Int_t* i1=0,Int_t* i2=0); // Perform a Multi Band filter on the loaded data
  Int_t GetN(Int_t mode=0) const;                              // Provide the number of data elements (to be) processed.
  TArrayD GetData(TString mode) const;                         // Provide a selected set of data

 protected:
  TVirtualFFT* fProc; // The FFTW processor
  Int_t fN;           // The number of data elements to be processed as entered via Load()
  Int_t fNwf;         // The number of data elements to be processed as entered via SetWaveform();
  TArrayD fReIn;      // Input array for real components
  TArrayD fImIn;      // Input array for imaginary components
  TArrayD fReOut;     // Output array for real components
  TArrayD fImOut;     // Output array for imaginary components
  TArrayD fWaveform;  // (System response) waveform for Convolution, Correlation etc.
  Float_t fSample;    // The actual data acquisition sampling frequency in Hz

  // Internal member functions
  void Reset();      // Reset all data and the processor
  TArrayD GetMovingAverageKernel(Int_t n);                      // Provide a tine domain Moving Average Filter kernel
  TArrayD GetLowPassKernel(Double_t fcut,Int_t n);              // Provide a time domain Low Pass Filter kernel
  TArrayD GetHighPassKernel(Double_t fcut,Int_t n);             // Provide a time domain High Pass Filter kernel
  TArrayD GetBandPassKernel(Double_t f1,Double_t f2,Int_t n);   // Provide a time domain High Pass Filter kernel
  TArrayD GetBandRejectKernel(Double_t f1,Double_t f2,Int_t n); // Provide a time domain High Pass Filter kernel

 ClassDef(NcDSP,4) // Various Digital Signal Processing (DSP) operations for (sequential) data samples
};
#endif
