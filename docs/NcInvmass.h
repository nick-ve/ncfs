#ifndef NcInvmass_h
#define NcInvmass_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcInvmass.h 5 2010-03-19 10:10:02Z nickve $

#include <math.h>
 
#include "TObject.h"
#include "TObjArray.h"

#include "NcRandom.h"
#include "NcTrack.h"

class NcInvmass : public TObject
{
 public:
  NcInvmass();                                     // Default constructor
  ~NcInvmass();                                    // Destructor
  void SetStorageMode(Int_t m);                    // Set storage mode (1=single, 2=multiple)
  void SetThetaSwitch(Int_t i=1);                  // Enable (1/0) new theta for comb. bkg. reco.
  void SetPhiSwitch(Int_t i=1);                    // Enable (1/0) new phi for comb. bkg. reco.
  Int_t GetStorageMode() const;                    // Provide storage mode
  Int_t GetThetaSwitch() const;                    // Provide theta switch flag
  Int_t GetPhiSwitch() const;                      // Provide phi switch flag
  TObjArray* Invmass(TObjArray* a1,TObjArray* a2); // Two-particle inv. mass reco.
  TObjArray* CombBkg(TObjArray* a1,TObjArray* a2); // Two-particle comb. background reco.

 protected:
  Double_t fPi;     // Value of pi
  Int_t fMode;      // Storage mode for signal and bkg. results (2=separate arrays)
  Int_t fBkg;       // Flag to denote comb. background processing
  NcRandom fRndm;   // The random number generator for the comb. bkg. reconstruction
  Int_t fNewtheta;  // Flag to denote enabling of switching theta for comb. bkg. reco.
  Int_t fNewphi;    // Flag to denote enabling of switching phi for comb. bkg. reco.
  TObjArray* fMinv; // Array with reconstructed invariant mass 'tracks'
  TObjArray* fMbkg; // Array with reconstructed comb. background 'tracks'

 private:
  void Combine(TObjArray* a1,TObjArray* a2); // Make two-particle combinations

 ClassDef(NcInvmass,1) // Construction of invariant mass and combinatorial background.
};
#endif
