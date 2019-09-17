#ifndef NcMath_h
#define NcMath_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcMath.h 107 2016-02-07 18:20:26Z nickve $

#include <math.h>
 
#include "TObject.h"
#include "TF1.h"
#include "TString.h"
#include "TMath.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TList.h"
#include "TLine.h"
#include "TLegend.h"
#include "TFeldmanCousins.h"

#include "NcRandom.h"
 
class NcMath : public TObject
{
 public:
  NcMath();                                                  // Default constructor
  virtual ~NcMath();                                         // Destructor
  NcMath(const NcMath& m);                                   // Copy constructor
  Double_t Zeta(Double_t x,Int_t nterms=100000) const;       // The Riemann Zeta function for x>1.
  Double_t Gamma(Double_t z) const;                          // Standard gamma function Gamma(z)
  Double_t Gamma(Double_t a,Double_t x,Int_t mode=0) const;  // Incomplete gamma function P(a,x) or gamma(a,x)
  Double_t LnGamma(Double_t z) const;                        // Compute ln[Gamma(z)]
  Double_t LnGamma(Double_t a,Double_t x,Int_t mode=0) const;// Compute ln of the incomplete gamma function P(a,x) or gamma(a,x)
  Double_t Erf(Double_t x) const;                            // Error function erf(x)
  Double_t Erfc(Double_t x) const;                           // Complementary error function erfc(x)
  Double_t Prob(Double_t chi2,Int_t ndf,Int_t mode=1) const; // Compute Chi-squared probability
  Double_t BesselI(Int_t n,Double_t x) const;                // Compute integer order mod. Bessel function I_n(x)
  Double_t BesselK(Int_t n,Double_t x) const;                // Compute integer order mod. Bessel function K_n(x)
  TF1 Chi2Dist(Int_t ndf) const;                             // Provide the Chi-squared distribution function
  TF1 Chi2CDF(Int_t ndf) const;                              // Provide the Chi-squared cumulative distribution function
  TF1 StudentDist(Double_t ndf) const;                       // Provide the Student's T distribution function
  TF1 StudentCDF(Double_t ndf) const;                        // Provide the Student's T cumulative distribution function
  TF1 FratioDist(Int_t ndf1,Int_t ndf2) const;               // Provide the Fratio distribution function
  TF1 FratioCDF(Int_t ndf1,Int_t ndf2) const;                // Provide the Fratio cumulative distribution function
  TF1 BinomialDist(Int_t n,Double_t p) const;                // Provide the Binomial PDF p(k|n,p) for k successes
  TF1 BinomialCDF(Int_t n,Double_t p) const;                 // Provide the Binomial CDF for p(k|n,p)
  TF1 NegBinomialnDist(Int_t k,Double_t p) const;            // Provide the Negative Binomial PDF p(n|k,p) for n trials
  TF1 NegBinomialnCDF(Int_t k,Double_t p) const;             // Provide the Negative Binomial CDF for p(n|k,p)
  TF1 NegBinomialxDist(Int_t k,Double_t p) const;            // Provide the Negative Binomial PDF p(x|k,p) for x failures
  TF1 NegBinomialxCDF(Int_t k,Double_t p) const;             // Provide the Negative Binomial CDF for p(x|k,p)
  TF1 PoissonDist(Double_t mu) const;                        // Provide the Poisson p(n|mu) pdf for mean mu
  TF1 PoissonCDF(Double_t mu) const;                         // Provide the Poisson p(n|mu) CDF for mean mu
  TF1 PoissonDist(Double_t r,Double_t dt) const;             // Provide the Poisson p(n|r,dt) pdf for rate r and time interval dt
  TF1 PoissonCDF(Double_t r,Double_t dt) const;              // Provide the Poisson p(n|r,dt) CDF for rate r and time interval dt
  TF1 PoissonDtDist(Double_t r,Int_t n) const;               // Provide the Poisson related p(dt|r,n) pdf for rate r and number of events n
  TF1 PoissonDtCDF(Double_t r,Int_t n) const;                // Provide the Poisson related p(dt|r,n) CDF for rate r and number of events n
  TF1 GammaDtDist(Double_t r,Double_t z) const;              // Provide the p(dt|r,z) pdf for given rate r and observed occurrences z
  TF1 GaussDist(Double_t mu,Double_t sigma) const;           // Provide the Gauss p(x|mu,sigma) pdf for mean mu and std dev. sigma
  TF1 GaussCDF(Double_t mu,Double_t sigma) const;            // Provide the Gauss p(x|mu,sigma) CDF for mean mu and std dev. sigma
  Double_t GaussProb(Double_t q,Double_t mean=0,Double_t sigma=1,Int_t isig=0) const; // P(|x-mean|<=|q-mean|) for Gauss pdf
  Double_t GaussPvalue(Double_t q,Double_t mean=0,Double_t sigma=1,Int_t sides=2,Int_t isig=0) const; // P-value of q for Gauss pdf
  Double_t Chi2Pvalue(Double_t chi2,Int_t ndf,Int_t sides=0,Int_t sigma=0,Int_t mode=1) const; // Chi-squared P-value
  Double_t StudentPvalue(Double_t t,Double_t ndf,Int_t sides=0,Int_t sigma=0) const; // Student's P-value
  Double_t FratioPvalue(Double_t f,Int_t ndf1,Int_t ndf2,Int_t sides=0,Int_t sigma=0) const; // F ratio P-value
  Double_t BinomialPvalue(Int_t k,Int_t n,Double_t p,Int_t sides=0,Int_t sigma=0,Int_t mode=0) const; // Bin. P-value
  Double_t PoissonPvalue(Int_t k,Double_t mu,Int_t sides=0,Int_t sigma=0) const; // Poisson P-value
  Double_t PoissonPvalue(Int_t k,Double_t r,Double_t dt,Int_t sides=0,Int_t sigma=0) const; // Poisson P-value
  Double_t PoissonDtPvalue(Double_t dt,Double_t r,Int_t n,Int_t sides=0,Int_t sigma=0) const; // Poisson dt P-value
  Double_t NegBinomialnPvalue(Int_t n,Int_t k,Double_t p,Int_t sides=0,Int_t sigma=0,Int_t mode=0) const; // NegBin. P-value for n trials
  Double_t NegBinomialxPvalue(Int_t x,Int_t k,Double_t p,Int_t sides=0,Int_t sigma=0,Int_t mode=0) const; // NegBin. P-value for x failures
  Double_t Nfac(Int_t n,Int_t mode=0) const;    // Compute n!
  Double_t LnNfac(Int_t n,Int_t mode=2) const;  // Compute ln(n!) 
  Double_t LogNfac(Int_t n,Int_t mode=2) const; // Compute log_10(n!) 
  Double_t Rfac(Double_t r) const;              // Compute r! for a fractional value r
  Double_t LnRfac(Double_t r) const;            // Compute ln(r!) for a fractional value r 
  Double_t LogRfac(Double_t r) const;           // Compute log_10(r!) for a fractional value r 
  Double_t PsiValue(Int_t m,Int_t* n,Double_t* p=0,Int_t f=0) const;     // Bayesian Psi value of a counting exp. w.r.t. hypothesis
  Double_t PsiValue(Int_t m,Double_t* n,Double_t* p=0,Int_t f=0) const;  // Bayesian Psi value of a counting exp. w.r.t. hypothesis
  Double_t PsiValue(TH1* his,TH1* hyp=0,TF1* pdf=0,Int_t f=0) const;     // Bayesian Psi value of a counting exp. w.r.t. hypothesis
  Double_t PsiExtreme(Double_t n,Int_t m,Double_t* p=0,Int_t k=0) const; // Extreme Bayesian Psi values w.r.t. some hypothesis
  Double_t PsiExtreme(TH1* his,TH1* hyp=0,TF1* pdf=0,Int_t k=0) const;   // Extreme Bayesian Psi values w.r.t. some hypothesis
  Double_t PsiPvalue(Double_t psi0,Double_t nr,Double_t n,Int_t m,Double_t* p=0,Int_t f=0,Double_t* na=0,TH1F* psih=0,Int_t ncut=0,Double_t* nrx=0,Int_t mark=1); // P-value of psi0 for specified B_m exp.
  Double_t PsiPvalue(Double_t psi0,Double_t nr,TH1* his,TH1* hyp=0,TF1* pdf=0,Int_t f=0,Double_t* na=0,TH1F* psih=0,Int_t ncut=0,Double_t* nrx=0,Int_t mark=1);   // P-value of psi0 for specified B_m exp.
  Double_t Chi2Value(Int_t m,Int_t* n,Double_t* p=0,Int_t* ndf=0) const;    // Frequentist Chi2 value of a counting exp. w.r.t. hypothesis
  Double_t Chi2Value(Int_t m,Double_t* n,Double_t* p=0,Int_t* ndf=0) const; // Frequentist Chi2 value of a counting exp. w.r.t. hypothesis
  Double_t Chi2Value(TH1* his,TH1* hyp=0,TF1* pdf=0,Int_t* ndf=0) const;    // Frequentist Chi2 value of a counting exp. w.r.t. hypothesis
  Double_t MeanMu(Double_t cl,Double_t nbkg,Int_t mode,TF1* w=0,TFeldmanCousins* f=0,Int_t nmax=0) const; // Average Feldman-Cousins upper/lower limit 
  Double_t LiMaSignificance(Int_t Non,Double_t Ton,Int_t Noff,Double_t Toff,Double_t Ra=1,Double_t Re=1) const; // Li and Ma significance

 protected:
  Double_t GamSer(Double_t a,Double_t x) const; // Compute P(a,x) via serial representation
  Double_t GamCf(Double_t a,Double_t x) const;  // Compute P(a,x) via continued fractions
  Double_t BesselI0(Double_t x) const;          // Compute modified Bessel function I_0(x)
  Double_t BesselK0(Double_t x) const;          // Compute modified Bessel function K_0(x)
  Double_t BesselI1(Double_t x) const;          // Compute modified Bessel function I_1(x)
  Double_t BesselK1(Double_t x) const;          // Compute modified Bessel function K_1(x)
 
 ClassDef(NcMath,10) // Various mathematical tools for scientific analysis.
 
};
#endif
