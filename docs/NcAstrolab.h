#ifndef NcAstrolab_h
#define NcAstrolab_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcAstrolab.h 139 2017-09-20 08:30:45Z nickve $

#include <math.h>

#include "TTask.h"
#include "TString.h"
#include "TRotMatrix.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TText.h"
#include "TLatex.h"
#include "TH2.h"
#include "TMarker.h"
#include "TF1.h"
#include "TArrayD.h"

#include "NcTimestamp.h"
#include "NcPosition.h"
#include "NcSignal.h"
#include "NcDevice.h"
#include "NcRandom.h"
#include "NcMath.h"
 
class NcAstrolab : public TTask,public NcTimestamp
{
 public:
  NcAstrolab(const char* name="NcAstrolab",const char* title="Generic lab"); // Constructor
  virtual ~NcAstrolab();                                              // Destructor
  NcAstrolab(const NcAstrolab& t);                                    // Copy constructor
  virtual TObject* Clone(const char* name="") const;                  // Make a deep copy and provide its pointer
  void Data(Int_t mode=1,TString u="deg",Bool_t utc=kTRUE);           // Lab info in angular units u
  void SetLabPosition(Nc3Vector& r);                                  // Set lab position in terrestrial frame
  void SetLabPosition(Double_t l,Double_t b,TString u="deg");         // Set lab terrestrial position
  void SetExperiment(TString name);                                   // Set position and local frame for the specified experiment
  NcPosition GetLabPosition() const;                                  // Provide the lab terrestrial position 
  void GetLabPosition(Double_t& l,Double_t& b,TString u="deg") const; // Provide the lab terrestrial position
  void SetRandomiser(Int_t iseed,Int_t cnt1=0,Int_t cnt2=0,NcTimestamp* ts=0); // (Re)initialise the internal NcRandom randomisation facility 
  NcRandom* GetRandomiser(Int_t& iseed,Int_t& cnt1,Int_t& cnt2) const;         // Provide the current internal NcRandom randomiser parameters
  using NcTimestamp::GetLT;
  Double_t GetLT();  // Provide Local Time (LT) in fractional hours
  using NcTimestamp::GetLMST;
  Double_t GetLMST(); // Provide Local Mean Sidereal Time (LMST) in fractional hours
  using NcTimestamp::GetLAST;
  Double_t GetLAST(); // Provide Local Apparent Sidereal Time (LAST) in fractional hours
  using NcTimestamp::SetLT;
  void SetLT(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns=0,Int_t ps=0); // Set specified Local Time
  void SetLT(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Double_t s); // Set specified Local Time
  void SetLT(Int_t y,Int_t m,Int_t d,TString time);                 // Set specified Local Time
  void SetLT(TString date,TString time,Int_t mode);                 // Set specified Local Time
  void SetLT(Int_t y,Int_t d,Int_t s,Int_t ns=0,Int_t ps=0); // Set Local Time based on elapsed days, secs etc...
  Double_t ConvertAngle(Double_t a,TString in,TString out) const;       // Angular format conversions
  void PrintAngle(Double_t a,TString in,TString out,Int_t ndig=1) const;// Print angle in various formats
  Double_t GetSolidAngle(Double_t thetamin,Double_t thetamax,TString tu,Double_t phimin,Double_t phimax,TString pu) const; // Provide solid angle between the specified boundaries
  NcSignal* SetSignal(Double_t d,Double_t a,TString au,Double_t b,TString bu,TString frame,NcTimestamp* ts,Int_t jref,TString mode="T",TString name="",Int_t type=0); // Store generic signal
  NcSignal* SetSignal(Double_t d,Double_t a,TString au,Double_t b,TString bu,TString frame,TString s,Double_t e,Int_t jref,TString mode,TString name="",Int_t type=0);// Store generic signal
  Int_t GetNRefSignals(Int_t mode=0) const; // Provide the number of stored reference signals
  Int_t GetNsignals(Int_t type,Int_t mode=0) const; // Provide the number of stored signals of type "type"
  NcSignal* GetSignal(Double_t& d,Double_t& a,TString au,Double_t& b,TString bu,TString frame,NcTimestamp* ts,Int_t jref,TString mode="T",Int_t type=0);   // Provide signal data
  NcSignal* GetSignal(Double_t& d,Double_t& a,TString au,Double_t& b,TString bu,TString frame,NcTimestamp* ts,TString name,TString mode="T",Int_t type=0); // Provide signal data
  NcSignal* GetSignal(Double_t& d,Double_t& a,TString au,Double_t& b,TString bu,TString frame,TString s,Double_t e,Int_t jref,TString mode,Int_t type=0);  // Provide signal data
  NcSignal* GetSignal(Double_t& d,Double_t& a,TString au,Double_t& b,TString bu,TString frame,TString s,Double_t e,TString name,TString mode,Int_t type=0);// Provide signal data
  NcSignal* GetSignal(Int_t jref=0,Int_t type=0);                   // Provide pointer to a stored signal object
  NcSignal* GetSignal(TString name,Int_t type=0,NcTimestamp* ts=0); // Provide pointer to a stored signal object
  void RemoveRefSignal(Int_t j,Int_t compress);                     // Remove a stored reference signal object
  void RemoveRefSignal(TString name,Int_t compress);                // Remove a stored reference signal object
  void RemoveSignal(Int_t j,Int_t type,Int_t compress);             // Remove a stored signal object
  void RemoveSignal(TString name,Int_t type,Int_t compress);        // Remove a stored signal object
  void PrintSignal(TString frame,TString mode,NcTimestamp* ts,Int_t ndig,Int_t jref=0,TString emode="T",Int_t type=0); // Print stored signal data
  void PrintSignal(TString frame,TString mode,NcTimestamp* ts,Int_t ndig,TString name,TString emode="T",Int_t type=0); // Print stored signal data
  void ListSignals(TString frame,TString mode,Int_t ndig=1,TString emode="T",Int_t nmax=10,Int_t j=-1,Int_t type=-1); // List stored signals
  Int_t GetSignalIndex(TString name,Int_t type=0); // Provide storage index of the signal with the specified name
  Double_t GetHourAngle(TString mode,NcTimestamp* ts,Int_t jref=0,Int_t type=0);// Provide the Local Hour Angle in degrees
  void SetLocalFrame(Double_t t1,Double_t p1,Double_t t2,Double_t p2,Double_t t3,Double_t p3); // Define local coordinate frame
  using NcTimestamp::GetDifference;
  Double_t GetDifference(Int_t jref,TString au,Double_t& dt,TString tu,Int_t mode=1,Int_t* ia=0,Int_t* it=0); // Provide space and time difference
  Double_t GetDifference(TString name,TString au,Double_t& dt,TString tu,Int_t mode=1);// Provide space and time difference
  TArrayI* MatchRefSignal(Double_t da,TString au,Double_t dt,TString tu,Int_t mode=1); // Provide space and time matching reference signals
  void MatchSignals(NcDevice& matches,Double_t da,TString au,Double_t dt,TString tu,Int_t mode=1,Int_t i1=1,Int_t i2=0,Int_t itype=0,Int_t j1=1,Int_t j2=0,Int_t jtype=1); // Provide space and time matching info of signals
  void MatchSignals(NcDevice& matches,TString name,Double_t da,TString au,Double_t dt,TString tu,Int_t mode=1,Int_t itype=0,Int_t j1=1,Int_t j2=0,Int_t jtype=1); // Provide space and time matching info of signals
  void SetTimeScramble(Int_t mode,Double_t tmin,Double_t tmax,TF1* frndm=0); // Set time scrambling parameters
  Int_t GetTimeScramble(Double_t* tmin=0,Double_t* tmax=0,TF1* frndm=0); // Provide time scrambling parameters
  void SetPositionScramble(Int_t mode,Double_t dmin,Double_t dmax,TF1* df=0,Double_t thmin=0,Double_t thmax=0,TF1* thf=0,Double_t phimin=0,Double_t phimax=0,TF1* phif=0); // Set position scrambling parameters
  Int_t GetPositionScramble(Double_t* dmin=0,Double_t* dmax=0,TF1* df=0,Double_t* thmin=0,Double_t* thmax=0,TF1* thf=0,Double_t* phimin=0,Double_t* phimax=0,TF1* phif=0); // Get position scrambling parameters
  void SetMaxDt(Double_t s); // Set maximum time difference (in sec) for GetSignal 
  Double_t GetMaxDt() const; // Provide maximum time difference (in sec) for GetSignal 
  void DisplaySignal(TString frame,TString mode,NcTimestamp* ts,Int_t j=0,TString proj="ham",Int_t clr=0); // Display stored signal
  void DisplaySignal(TString frame,TString mode,NcTimestamp* ts,TString name,TString proj="ham",Int_t clr=0,Int_t type=0); // Display stored signal
  void DisplaySignals(TString frame,TString mode,NcTimestamp* ts,TString proj="ham",Int_t clr=0,Int_t nmax=-1,Int_t j=-1,Int_t type=-1); // Display all stored signals
  void SetMarkerSize(Float_t size,Int_t type); // Set size for the marker symbols of the skymaps and related histograms 
  void SetCentralMeridian(Double_t phi,TString u="deg");  // Set central meridian for the sky display
  void SetPhysicalParameter(TString name,Double_t value); // Facility to modify physical parameter values
  Double_t GetPhysicalParameter(TString name) const;      // Provide the (modified) value of a physical parameter
  Double_t GetPhysicalDistance(Double_t z,TString u="Mpc",Int_t t=1) const; // Provide physical distance of an object with redshift z in a flat universe
  Double_t GetProperDistance(Double_t z,TString u="Mpc",Int_t t=1) const;   // Provide proper distance of an object with redshift z in a flat universe
  Double_t GetComovingDistance(Double_t z,TString u="Mpc") const;           // Provide comoving coordinate distance of an object with redshift z in a flat universe
  Double_t GetLuminosityDistance(Double_t z,TString u="Mpc") const;         // Provide luminosity distance of an object with redshift z in a flat universe
  Double_t GetLightTravelDistance(Double_t z,TString u="Mpc") const;        // Provide light-travel distance of an object with redshift z in a flat universe
  Double_t GetLightTravelTime(Double_t z) const;                            // Provide light-travel time of an object with redshift z in a flat universe
  Double_t GetHubbleParameter(Double_t z,TString u="Mpc") const; // Provide the Hubble parameter for a certain redshift z in a flat universe
  Double_t GetNuclearMass(Int_t z,Int_t n,Int_t mode=1) const; // Provide the nuclear mass or binding energy for a nucleus (Z,N)
  Double_t GetNeutrinoXsection(Int_t mode,Int_t type,Double_t egev,Double_t xscale=1,Double_t* eprimgev=0,Double_t* alpha=0) const; // Provide neutrino cross section
  void RandomPosition(Nc3Vector& v,Double_t thetamin,Double_t thetamax,Double_t phimin,Double_t phimax); // Random angular position according to an isotropic solid angle distribution
  void SmearPosition(Nc3Vector& v,Double_t sigma); // Smear angular position according to the specified angular accuracy
  TH1F GetDxHistogram(TH1* hx,Int_t nc,Double_t dxbin=-1,Double_t dxmin=-1,Double_t dxmax=-1) const; // Provide dx distribution of histogram entries 
  TH1F GetDifHistogram(TH1* hin,Int_t mode,TString s="",TF1* f=0) const;  // Construct the differential dy/dx vs. x histogram from a 1D y vs. x histogram
  TH1F GetCountsHistogram(TF1& spec,Int_t nbins,Double_t xmin, Double_t xmax,Int_t mode,TString s="") const;  // Construct the counts (N) vs. x histogram from a 1D differential spectrum dN/dx
  TH1F GetLogHistogram(TH1* hin,Int_t mode,TString s="") const; // Construct the Log10(y) or Ln(y) vs. x histogram from a 1D y vs. x histogram
  TF1 GetBackgroundRatePDF(Int_t Noff,Double_t Toff,Double_t bmax=-1,Double_t prec=709); // Posterior Bayesian PDF for a background rate
  TF1 GetSignalRatePDF(Int_t Non,Double_t Ton,Int_t Noff,Double_t Toff,Double_t Ra=1,Double_t Re=1,Double_t smax=-1,Double_t bmax=-1,Double_t prec=709); // Posterior Bayesian PDF for a source signal rate
  Double_t GetUpperLimit(TF1 pdf,Double_t p);  // Provide the "p%" upper limit for the specified PDF
  Double_t GetUpperLimit(TH1* pdf,Double_t p); // Provide the "p%" upper limit for the specified histogram
  Double_t GetCredibleInterval(TF1 pdf,Double_t p,Double_t& xlow,Double_t& xup,Int_t n=1000); // Provide the "p%" credible interval for the specified PDF
  Double_t GetCredibleInterval(TF1 pdf,Double_t p,Float_t& xlow,Float_t& xup,Int_t n=1000);   // Provide the "p%" credible interval for the specified PDF
  Double_t GetCredibleInterval(TH1* his,Double_t p,Double_t& xlow,Double_t& xup); // Provide the "p%" credible interval for the specified histogram
  Double_t GetCredibleInterval(TH1* his,Double_t p,Float_t& xlow,Float_t& xup);   // Provide the "p%" credible interval for the specified histogram
  Double_t KolmogorovTest(TString mode,TH1* h1,TH1* h2=0,TF1* pdf=0,Double_t nr=1000,TH1F* ksh=0,Int_t ncut=0,Double_t* nrx=0,Int_t mark=1); // Perform a K-S test
  TH1F GetCumulHistogram(TH1* h,TString name,TString mode="F") const; // Provide the Cumulative Distribution Histogram from an input histogram
  TH1F GetCumulHistogram(TF1* f,TString name,Int_t nbins,Double_t xmin,Double_t xmax,TString mode="F") const; // Provide the Cumulative Distribution Histogram from an input function
 
 protected:
  NcPosition fLabPos;    // Position of the lab in the terrestrial longitude-latitude frame
  Double_t fToffset;     // Lab time offset in fractional hours w.r.t. UT
  TObjArray* fRefs;      // Array holding the reference signals
  TObjArray* fSigs;      // Array holding the measured signals
  TRotMatrix fB;         //! The frame bias matrix for conversion of ICRS to J2000 coordinates
  Int_t fBias;           //! Initialisation flag for fB values (0=uninitialised  1=initialised)
  TRotMatrix fP;         //! Matrix for precession correction  
  TRotMatrix fN;         //! Matrix for nutation correction  
  TRotMatrix fG;         //! Matrix for conversion of equatorial to galactic coordinates
  Int_t fGal;            //! Type indicator for fG values (0=uninitialised  1=B1950  2=J2000)
  TRotMatrix fE;         //! Matrix for conversion of equatorial to ecliptic coordinates
  TRotMatrix fH;         //! Matrix for conversion of equatorial to horizontal coordinates
  TRotMatrix fL;         // Matrix for conversion of horizontal to local-frame coordinates
  TArrayI* fIndices;     //! Storage indices of the matching reference signals
  Int_t fTscmode;        // Time scrambling mode
  Double_t fTscmin;      // Minimal value (in sec.) for time scrambling
  Double_t fTscmax;      // Maximal value (in sec.) for time scrambling
  TF1* fTscfunc;         // Randomisation function for time scrambling
  Int_t fRscmode;        // Position scrambling mode
  Double_t fDscmin;      // Minimal value for scrambling angular difference (in degrees) or vector length 
  Double_t fDscmax;      // Maximal value for scrambling angular difference (in degrees) or vector length
  TF1* fDscfunc;         // Randomisation function for angular difference (or vector length) scrambling
  Double_t fThetascmin;  // Minimal value (in degrees) for scrambling local theta coordinate
  Double_t fThetascmax;  // Maximal value (in degrees) for scrambling local theta coordinate
  TF1* fThetascfunc;     // Randomisation function for local theta coordinate scrambling
  Double_t fPhiscmin;    // Minimal value (in degrees) for scrambling local phi coordinate
  Double_t fPhiscmax;    // Maximal value (in degrees) for scrambling local phi coordinate
  TF1* fPhiscfunc;       // Randomisation function for local phi coordinate scrambling
  NcRandom* fRan;        // The randomising facility
  Double_t fMaxDt;       // Maximum time difference (in sec) for GetSignal 
  NcSignal* SetSignal(Nc3Vector* r,TString frame,TString mode,NcTimestamp* ts,Int_t jref=0,TString name="",Int_t type=0); // Generic signal storage
  NcSignal* GetSignal(Nc3Vector& r,TString frame,TString mode,NcTimestamp* ts,Int_t jref=0,Int_t type=0); // Provide stored signal data
  void SetSolarSystem(TString name,NcTimestamp* ts,Int_t type=0); // Set c.q. update coordinates for solar system objects
  void SetBmatrix();                 // Set the frame bias matrix
  void SetPmatrix(NcTimestamp* ts);  // Set precession matrix for Julian date jd w.r.t. J2000.
  void SetNmatrix(NcTimestamp* ts);  // Set nutation matrix for Julian date jd w.r.t. J2000.
  void SetGmatrix(TString mode);     // Set the equatorial to galactic conversion matrix
  void SetEmatrix(NcTimestamp* ts);  // Set the equatorial to ecliptic conversion matrix
  void SetHmatrix(NcTimestamp* ts);  // Set the equatorial to horizontal conversion matrix
  void Precess(Nc3Vector& r,NcTimestamp* ts1,NcTimestamp* ts2); // Correct RA and decl. for earth's precession
  void Nutate(Nc3Vector& r,NcTimestamp* ts); // Correct RA and decl. for earth's nutation

  // Internal facility for reference signal and/or measurement matching
  Int_t fSolUpdate; // Flag to update the position of Solar system objects for signal matching  
  Double_t GetDifference(Int_t i,Int_t j,TString au,Double_t& dt,TString tu,Int_t mode=1); // Provide space and time difference

  // The skymap display facilities
  Int_t fUsMeridian;      // Flag to denote that the user has selected the central meridian (1) or not (0)
  Double_t fMeridian;     //! Central meridian (in rad) for the sky display
  TString fProj;          //! Projection which is currently in use
  TCanvas* fCanvas;       //! The canvas for the skymap
  TH2* fHist[2];          //! Temp. histograms for the sky display
  TObjArray* fMarkers;    //! Temp. array to hold the markers for the signal display
  Float_t fMarkerSize[4]; // Size of the marker symbols for the skymaps and related histograms
  Int_t fMarkerStyle[4];  // Style of the marker symbols for the skymaps and related histograms
  Int_t fMarkerColor[4];  // Color of the marker symbols for the skymaps and related histograms
  void Project(Double_t l,Double_t b,TString proj,Double_t& x,Double_t& y);// Projection of (l,b) pair
  void ProjectCylindrical(Double_t l,Double_t b,Double_t& x,Double_t& y);  // Cylindrical projection of (l,b) pair
  void ProjectHammer(Double_t l,Double_t b,Double_t& x,Double_t& y);       // Hammer-Aitoff projection of (l,b) pair
  void ProjectAitoff(Double_t l,Double_t b,Double_t& x,Double_t& y);       // Aitoff projection of (l,b) pair
  void ProjectMercator(Double_t l,Double_t b,Double_t& x,Double_t& y);     // Mercator projection of (l,b) pair

  // Some (astro)physical parameters
  Double_t fSpeedC;  // Speed of light in vacuum in m/s
  Double_t fQe;      // Electron charge in C
  Double_t fMe;      // Electron mass in MeV/c^2
  Double_t fMmu;     // Muon mass in MeV/c^2
  Double_t fMtau;    // Tau mass in MeV/c^2
  Double_t fAmu;     // The unified atomic mass unit in MeV/c^2
  Double_t fMp;      // Proton mass in MeV/c^2
  Double_t fMn;      // Neutron mass in MeV/c^2 
  Double_t fMW;      // W mass in GeV/c^2 
  Double_t fGammaW;  // Full width of W in GeV
  Double_t fMZ;      // Z mass in GeV/c^2 
  Double_t fGammaZ;  // Full width of Z in GeV
  Double_t fAlphaEM; // The electromagnetic fine-structure constant
  Double_t fFermi;   // Fermi coupling constant GF/(hbar*c)^3 in GeV^-2
  Double_t fPlanck;  // The value of the Planck constant (h) in J s
  Double_t fBoltz;   // The value of the Boltzmann constant (k) in J K^-1
  Double_t fNewton;  // The value of the Newton gravitational constant in m^3 kg^-1 s^-2 
  Double_t fAu;      // The Astronomical Unit in m
  Double_t fPc;      // The Parsec in m
  Double_t fHubble;  // The value of the Hubble parameter in km s^-1 Mpc^-1
  Double_t fOmegaM;  // The value of the fractional mass energy density
  Double_t fOmegaR;  // The value of the fractional radiation energy density
  Double_t fOmegaL;  // The value of the fractional vacuum energy density
  Double_t fOmegaB;  // The value of the fractional baryon energy density
  Double_t fOmegaC;  // The value of the fractional cold dark matter energy density

  // Some derived (astro)physical parameters c.q. conversion constants
  Double_t fHbar;    // The value of hbar in MeV s
  Double_t fHbarc;   // The value of the conversion constant hbar*c in MeV fm
  Double_t fHbarc2;  // The value of the conversion constant (hbar*c)^2 in GeV^2 barn

  // Actual calculation of the posterior Bayesian probability for background c.q. source signal rates
  Double_t GetBackgroundRateProb(Double_t* vars,Double_t* pars); // Posterior Bayesian probability for a background rate "b"
  Double_t GetSignalRateProb(Double_t* vars,Double_t* pars); // Posterior Bayesian probability for a source signal rate "s"

 
 ClassDef(NcAstrolab,29) // Virtual lab to provide (astro)physical parameters, treat data and relate observations with astrophysical phenomena
};
#endif
