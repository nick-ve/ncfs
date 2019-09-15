#ifndef NcRandom_h
#define NcRandom_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcRandom.h 98 2014-10-27 10:13:13Z nickve $

#include <math.h>
 
#include "TObject.h"

#include "NcMath.h"
#include "NcTimestamp.h"
 
class NcRandom : public TObject
{
 public:
  NcRandom();                                                      // Constructor with default sequence
  NcRandom(Int_t seed,NcTimestamp* ts=0);                          // Constructor with user defined seed
  NcRandom(Int_t seed,Int_t cnt1,Int_t cnt2,NcTimestamp* ts=0);    // User defined starting point
  virtual ~NcRandom();                                             // Destructor
  Int_t GetSeed() const;                                           // Provide current seed value
  Int_t GetCnt1() const;                                           // Provide current counter value cnt1
  Int_t GetCnt2() const;                                           // Provide current counter value cnt2
  void Data() const;                                               // Print current seed, cnt1 and cnt2
  Float_t Uniform();                                               // Uniform dist. within <0,1>
  Float_t Uniform(Float_t a,Float_t b);                            // Uniform dist. within <a,b>
  void Uniform(Float_t* vec,Int_t n);                              // n uniform randoms in <0,1>
  void Uniform(Float_t* vec,Int_t n,Float_t a,Float_t b);          // see above
  Float_t Gauss();                                                 // Gaussian dist. with mean=0 sigma=1
  Float_t Gauss(Float_t mean,Float_t sigma);                       // Gaussian dist. with mean and sigma
  void Gauss(Float_t* vec,Int_t n);                                // n Gaussian randoms mean=0 sigma=1
  void Gauss(Float_t* vec,Int_t n,Float_t mean,Float_t sigma);     // see above
  Float_t Poisson(Float_t mean);                                   // Poisson dist. with certain mean
  void Poisson(Float_t* vec,Int_t n,Float_t mean);                 // n Poisson randoms with mean
  void SetUser(Float_t a,Float_t b,Int_t n,Float_t (*f)(Float_t)); // Specify user dist. f(x)
  void SetUser(Float_t* x,Float_t* y,Int_t n);                     // Specify user dist. arrays
  Float_t User();                                                  // Provide random in [a,b] according to user distribution
  void User(Float_t* vec,Int_t n);                                 // n randoms in [a,b] from user dist.
  Double_t RanBm(Double_t nr,Double_t n,Int_t m,Double_t* p=0,Double_t* na=0,Double_t* psia=0,Double_t psi0=-1,Int_t f=0,TH1* psih=0,Int_t ncut=0,Double_t* nrx=0);// Random B_m
 
 private:
  Int_t   fI,fJ,fSeed,fCnt1,fCnt2,fClip;                        // Indices, seed and counters
  Float_t fU[97],fC,fCd,fCm;                                    // The Fibonacci parameters
  void Start(Int_t seed,Int_t cnt1,Int_t cnt2,NcTimestamp* ts); // Start at certain point
  void Unpack(Int_t seed,Int_t& i,Int_t& j,Int_t& k,Int_t& l);  // Unpack the seed
  void Uniform(Int_t n); // n uniform randoms for quick skipping
  Int_t fNa;             //! The number of bins of the area function
  Float_t* fXa;          //! The binned x values of the area function
  Float_t* fYa;          //! The corresponding y values of the area function
  Float_t fYamin,fYamax; //! The min. and max. y values of the area function
  Int_t* fIbins;         //! The bin numbers of the random x candidates
 
 ClassDef(NcRandom,3) // Generate universal random numbers and sequences on all common machines.
};
#endif
