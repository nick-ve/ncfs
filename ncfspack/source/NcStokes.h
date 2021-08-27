#ifndef NcStokes_h
#define NcStokes_h

// Copyright(c) 1997-2021, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "TString.h"
#include "TMath.h"

class NcStokes
{
 public:
  NcStokes();                  // Default constructor
  virtual ~NcStokes();         // Default destructor
  NcStokes(const NcStokes& q); // Copy constructor
  void SetStokesParameters(Double_t I,Double_t Q,Double_t U,Double_t V);                          // Set the values of the Stokes parameters
  void SetEllipseParameters(Double_t a,Double_t b,Double_t theta,TString u,Int_t h,Double_t p=1); // Set the parameters of the polarisation ellipse
  void SetJonesVector(Double_t A1,Double_t A2,Double_t phi,TString u,Double_t p=1);               // Set the Jones vector (E1,E2) with phase difference phi
  Double_t GetParameter(TString name,TString u="rad");                                            // Provide the value of the parameter with the specified name
  void Data(TString u="rad");                                                                     // Provide all polarisation related information

 protected:
  Double_t fI; // Stokes parameter I (also called S0)
  Double_t fQ; // Stokes parameter Q (also called S1)
  Double_t fU; // Stokes parameter U (also called S2)
  Double_t fV; // Stokes parameter V (also called S3)

 ClassDef(NcStokes,1) // Treatment of Stokes parameters for EM polarisation studies
};
#endif
