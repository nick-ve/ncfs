#ifndef NcAstrolab_h
#define NcAstrolab_h
// Copyright(c) 1997, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <math.h>

#include "TROOT.h"
#include "TTask.h"
#include "TObjString.h"
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
#include "TStyle.h"
#include "TFile.h"
#include "TChain.h"
#include "TLeaf.h"
#include "TLeafC.h"

#include "TApplication.h"
#include "TGFrame.h"
#include "TGComboBox.h"
#include "TGNumberEntry.h"
#include "TGButtonGroup.h"

#include "NcTimestamp.h"
#include "NcPosition.h"
#include "NcSignal.h"
#include "NcDevice.h"
#include "NcRandom.h"
#include "NcMath.h"
#include "NcSample.h"
#include "NcBlocks.h"

class NcAstrolab : public TTask,public NcTimestamp
{
 public:
  NcAstrolab(const char* name="User",const char* title="Virtual Lab for general use"); // Constructor
  virtual ~NcAstrolab();                                              // Destructor
  NcAstrolab(const NcAstrolab& t);                                    // Copy constructor
  virtual TObject* Clone(const char* name="") const;                  // Make a deep copy and provide its pointer
  void Data(Int_t mode=1,TString u="deg",Bool_t utc=kTRUE);           // Lab info in angular units u
  void SetLabPosition(Nc3Vector& r);                                  // Set lab position in terrestrial frame
  void SetLabPosition(Double_t l,Double_t b,TString u="deg");         // Set lab terrestrial position
  void SetExperiment(TString name,Int_t id=0);                        // Set position and local frame for the specified experiment
  void SetLabTimeOffset(Double_t dt);                                 // Set the lab time offset w.r.t. UT
  NcPosition GetLabPosition() const;                                  // Provide the lab terrestrial position 
  void GetLabPosition(Double_t& l,Double_t& b,TString u="deg") const; // Provide the lab terrestrial position
  TString GetExperiment() const;                                      // Provide the name of the experiment site
  Int_t GetLabDetectorId() const;                                     // Provide the detector system identifier
  Double_t GetLabTimeOffset() const;                                  // Provide the lab time offset w.r.t. UT
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
  void PrintAngle(Double_t a,TString in,TString out,Int_t ndig=1,Bool_t align=kFALSE) const;// Print angle in various formats
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
  Int_t SetSourceAttributes(NcSignal* s,Double_t sigmapos,TString u,Double_t z=-999,Double_t T90=-999);
  Double_t GetSourceAttributes(NcSignal* s,Float_t* z=0,Float_t* T90=0);
  Int_t RemoveRefSignal(Int_t j,Int_t compress);                    // Remove a stored reference signal object
  Int_t RemoveRefSignal(TString name,Int_t compress);               // Remove a stored reference signal object
  Int_t RemoveSignal(Int_t j,Int_t type,Int_t compress);            // Remove the signal object stored at index j
  Int_t RemoveSignal(TString name,Int_t type,Int_t compress);       // Remove the signal object stored with the provided name
  Int_t RemoveSignals(TString name,Int_t type,Int_t compress);      // Remove all signal objects that match the provided name pattern
  void PrintSignal(TString frame,TString mode,NcTimestamp* ts,Int_t ndig,Int_t jref=0,TString emode="T",Int_t type=0,Bool_t align=kFALSE); // Print stored signal data
  void PrintSignal(TString frame,TString mode,NcTimestamp* ts,Int_t ndig,TString name,TString emode="T",Int_t type=0,Bool_t align=kFALSE); // Print stored signal data
  void ListSignals(TString frame,TString mode,Int_t ndig=1,TString emode="T",Int_t nmax=10,Int_t j=-1,Int_t type=-1,NcTimestamp* ts=0,TString name="*"); // List stored signals
  Int_t GetSignalIndex(TString name,Int_t type=0); // Provide storage index of the signal with the specified name
  Int_t GetSignalIndex(NcSignal* s,Int_t type);    // Provide storage index of the specified signal
  Double_t GetHourAngle(TString mode,NcTimestamp* ts,Int_t jref=0,Int_t type=0);// Provide the Local Hour Angle in degrees

  void GeoToHeliocentric(Double_t& R,Double_t& B,Double_t& L,NcTimestamp* ts,TString Bu="deg",TString Lu="deg"); // Convert Geocentric ecliptic coordinates into Heliocentric
  void HelioToGeocentric(Double_t& R,Double_t& B,Double_t& L,NcTimestamp* ts,TString Bu="deg",TString Lu="deg"); // Convert Heliocentric ecliptic coordinates into Geocentric

  void SetLocalFrame(Double_t t1,Double_t p1,Double_t t2,Double_t p2,Double_t t3,Double_t p3); // Define local coordinate frame
  void GetLocalFrame(Float_t arr[6]); // Provide the axes orientations of the local coordinate frame
  using NcTimestamp::GetDifference;
  Double_t GetDifference(Int_t jref,TString au,Double_t& dt,TString tu,Int_t mode=1,Int_t* ia=0,Int_t* it=0); // Provide space and time difference
  Double_t GetDifference(TString name,TString au,Double_t& dt,TString tu,Int_t mode=1);// Provide space and time difference
  Double_t GetSeparation(TString name1,TString name2,TString au,Double_t* dt=0,TString tu="s",Int_t mode=1,Double_t* diftheta=0,Double_t* difphi=0); // Provide space and time separation
  TArrayI* MatchRefSignal(Double_t da,TString au,Double_t dt,TString tu,Int_t mode=1); // Provide space and time matching reference signals
  void MatchSignals(NcDevice& matches,Double_t da,TString au,Double_t dt,TString tu,Int_t mode=1,Int_t i1=1,Int_t i2=0,Int_t itype=0,Int_t j1=1,Int_t j2=0,Int_t jtype=1); // Provide space and time matching info of signals
  void MatchSignals(NcDevice& matches,TString name,Double_t da,TString au,Double_t dt,TString tu,Int_t mode=1,Int_t itype=0,Int_t j1=1,Int_t j2=0,Int_t jtype=1); // Provide space and time matching info of signals
  void SetTimeScramble(Int_t mode,Double_t tmin,Double_t tmax,TF1* frndm=0); // Set time scrambling parameters
  Int_t GetTimeScramble(Double_t* tmin=0,Double_t* tmax=0,TF1* frndm=0); // Provide time scrambling parameters
  void SetPositionScramble(Int_t mode,Double_t dmin,Double_t dmax,TF1* df=0,Double_t thmin=0,Double_t thmax=0,TF1* thf=0,Double_t phimin=0,Double_t phimax=0,TF1* phif=0); // Set position scrambling parameters
  Int_t GetPositionScramble(Double_t* dmin=0,Double_t* dmax=0,TF1* df=0,Double_t* thmin=0,Double_t* thmax=0,TF1* thf=0,Double_t* phimin=0,Double_t* phimax=0,TF1* phif=0); // Get position scrambling parameters
  void SetMaxDt(Double_t s); // Set maximum time difference (in sec) for GetSignal 
  Double_t GetMaxDt() const; // Provide maximum time difference (in sec) for GetSignal 
  void DisplaySignal(TString frame,TString mode,NcTimestamp* ts,Int_t j=-1,TString proj="ham",Int_t clr=0,TString name=""); // Display stored signal
  void DisplaySignal(TString frame,TString mode,NcTimestamp* ts,TString name,TString proj="ham",Int_t clr=0,Int_t type=0);  // Display stored signal
  void DisplaySignals(TString frame,TString mode,NcTimestamp* ts,TString proj="ham",Int_t clr=0,Int_t nmax=-1,Int_t j=-1,Int_t type=-1,TString name="*"); // Display all stored signals
  void SetMarkerSize(Float_t size,Int_t type); // Set size for the marker symbols of the skymaps and related histograms 
  void SetMarkerStyle(Int_t style,Int_t type); // Set style for the marker symbols of the skymaps and related histograms 
  void SetMarkerColor(Int_t color,Int_t type); // Set color for the marker symbols of the skymaps and related histograms 
  void SetCentralMeridian(Int_t mode=0,Double_t phi=0,TString u="deg");  // Set central meridian and orientation for the sky display
  void SetPhysicalParameter(TString name,Double_t value); // Facility to modify physical parameter values
  Double_t GetPhysicalParameter(TString name) const;      // Provide the (modified) value of a physical parameter
  Double_t GetPhysicalDistance(Double_t z,TString u="Mpc",Int_t t=1) const; // Provide physical distance of an object with redshift z in a flat universe
  Double_t GetProperDistance(Double_t z,TString u="Mpc",Int_t t=1) const;   // Provide proper distance of an object with redshift z in a flat universe
  Double_t GetComovingDistance(Double_t z,TString u="Mpc") const;           // Provide comoving coordinate distance of an object with redshift z in a flat universe
  Double_t GetLuminosityDistance(Double_t z,TString u="Mpc") const;         // Provide luminosity distance of an object with redshift z in a flat universe
  Double_t GetLightTravelDistance(Double_t z,TString u="Mpc") const;        // Provide light-travel distance of an object with redshift z in a flat universe
  Double_t GetLightTravelTime(Double_t z) const;                            // Provide light-travel time of an object with redshift z in a flat universe
  Double_t GetHubbleParameter(Double_t z,TString u="Mpc") const;            // Provide the Hubble parameter for a certain redshift z in a flat universe
  Double_t GetNuclearMass(Int_t Z,Int_t N,Int_t mode=1) const;              // Provide the nuclear mass or binding energy for a nucleus (Z,N)
  Double_t GetRadiationLength(Double_t Z,Double_t A,Double_t rho=-1) const; // Provide the radiation length for a medium consisting of (averaged) nuclei (Z,A) 
  Double_t GetMeanFreePath(Double_t sigma,Double_t rho,Int_t mode) const;   // Provide the mean free path of a high-energy particle passing through a certain medium
  Double_t GetInteractionProbability(Double_t x,Double_t lambda) const;                        // Provide the interaction probability of a high-energy particle traveling a distance x in a certain medium
  Double_t GetInteractionProbability(Double_t x,Double_t sigma,Double_t rho,Int_t mode) const; // Provide the interaction probability of a high-energy particle traveling a distance x in a certain medium
  Double_t GetSurvivalProbability(Double_t x,Double_t lambda) const;                           // Provide the survival probability of a high-energy particle traveling a distance x in a certain medium
  Double_t GetSurvivalProbability(Double_t x,Double_t sigma,Double_t rho,Int_t mode) const;    // Provide the survival probability of a high-energy particle traveling a distance x in a certain medium
  Double_t GetShieldingThickness(Double_t prob,Double_t lambda) const;                         // Provide the shielding thickness to obtain the specified survival probability for a high-energy particle
  Double_t GetShieldingThickness(Double_t prob,Double_t sigma,Double_t rho,Int_t mode) const;  // Provide the shielding thickness to obtain the specified survival probability for a high-energy particle
  Double_t GetTargetThickness(Double_t prob,Double_t lambda) const;                            // Provide the target thickness to obtain the specified interaction probability for a high-energy particle
  Double_t GetTargetThickness(Double_t prob,Double_t sigma,Double_t rho,Int_t mode) const;     // Provide the target thickness to obtain the specified interaction probability for a high-energy particle

  Double_t GetNeutrinoXsection(Int_t mode,Int_t type,Double_t egev,Double_t xscale=1,Double_t* eprimgev=0,Double_t* alpha=0) const; // Provide neutrino cross section
  Double_t GetNeutrinoAngle(Double_t E,TString u,Int_t mode,TF1* f=0); // Provide the kinematic opening angle between a neutrino and the CC produced corresponding lepton
  void RandomPosition(Nc3Vector& v,Double_t thetamin,Double_t thetamax,Double_t phimin,Double_t phimax); // Random angular position according to an isotropic solid angle distribution
  void SmearPosition(Nc3Vector& v,Double_t sigma); // Smear angular position according to the specified angular accuracy
  void ShiftPosition(Nc3Vector& v,Double_t angle); // Shift the position with the specified angular offset to a random location on a cone around the original position
  TH1F GetDxHistogram(TH1* hx,Int_t nc,Double_t dxbin=-1,Double_t dxmin=-1,Double_t dxmax=-1,Int_t mode=1,Double_t fact=1); // Provide dx distribution of histogram entries 
  TH1F GetDifHistogram(TH1* hin,Int_t mode,TString s="",TF1* f=0) const;  // Construct the differential dy/dx vs. x histogram from a 1D y vs. x histogram
  TH1F GetCountsHistogram(TF1& spec,Int_t nbins,Double_t xmin, Double_t xmax,Int_t mode,TString s="") const;  // Construct the counts (N) vs. x histogram from a 1D differential spectrum dN/dx
  TH1F GetCountsHistogram(TH1& hin,Int_t mode,TString s="",TF1* fscale=0) const;  // Construct the counts (N) vs. x histogram from a 1D differential distribution dN/dx
  TH1F GetLogHistogram(TH1* hin,Int_t mode,TString s="") const; // Construct the Log10(y) or Ln(y) vs. x histogram from a 1D y vs. x histogram
  TF1 GetBackgroundRatePDF(Double_t Noff,Double_t Toff,Double_t bmax=-1,Double_t prec=709); // Posterior Bayesian PDF for a background rate
  TF1 GetSignalRatePDF(Double_t Non,Double_t Ton,Double_t Noff,Double_t Toff,Double_t Ra=1,Double_t Re=1,Double_t smax=-1,Double_t bmax=-1,Double_t prec=709); // Posterior Bayesian PDF for a source signal rate
  Double_t GetUpperLimit(TF1 pdf,Double_t p);  // Provide the "p%" upper limit for the specified PDF
  Double_t GetUpperLimit(TH1* pdf,Double_t p); // Provide the "p%" upper limit for the specified histogram
  Double_t GetCredibleInterval(TF1 pdf,Double_t p,Double_t& xlow,Double_t& xup,Int_t n=1000); // Provide the "p%" credible interval for the specified PDF
  Double_t GetCredibleInterval(TF1 pdf,Double_t p,Float_t& xlow,Float_t& xup,Int_t n=1000);   // Provide the "p%" credible interval for the specified PDF
  Double_t GetCredibleInterval(TH1* his,Double_t p,Double_t& xlow,Double_t& xup); // Provide the "p%" credible interval for the specified histogram
  Double_t GetCredibleInterval(TH1* his,Double_t p,Float_t& xlow,Float_t& xup);   // Provide the "p%" credible interval for the specified histogram
  Double_t KolmogorovTest(TString mode,TH1* h1,TH1* h2=0,TF1* pdf=0,Double_t nr=1000,TH1F* ksh=0,Int_t ncut=0,Double_t* nrx=0,Int_t mark=1); // Perform a K-S test
  TH1F GetCumulHistogram(TH1* h,TString name,TString mode="F") const; // Provide the Cumulative Distribution Histogram from an input histogram
  TH1F GetCumulHistogram(TF1* f,TString name,Int_t nbins,Double_t xmin,Double_t xmax,TString mode="F") const; // Provide the Cumulative Distribution Histogram from an input function
  void InitDataNames(Int_t dir,TString frame,TString mode="J"); // Initialisation of the input data variable names correspondence table
  void SetDataNames(TString obsname,TString varname,TString units="1",TString func="none"); // Specification of the input data variable names correspondence table
  void ListDataNames(); // Listing of the input data variable names correspondence table

  // Facilities for transient burst investigation
  void SetBurstParameter(TString name,Double_t value); // Specification of a certain transient burst parameter setting
  NcDevice* GetBurstParameters();    // Provide the device containing all the burst parameter settings
  void ListBurstParameters() const;  // Listing of all the burst parameter settings
  void LoadInputData(Bool_t src,TString file,TString tree,Int_t date1=0,Int_t date2=0,Int_t nmax=-1,TString type="-");  // Load source (c.q. burst) or observed event data
  void GenBurstGCNdata(Int_t n,TString name="GRB",Bool_t scale=kFALSE); // Generate fictative burst GCN data
  void GenBurstSignals(); // Generate detector signals from the stored transient bursts
  void MatchBurstData(NcDevice& matches,Int_t i1=1,Int_t i2=0,Int_t itype=0,Int_t j1=1,Int_t j2=0,Int_t jtype=1); // Provide Burst and Event data space and time matching info
  void MatchBurstData(NcDevice& matches,TString name,Int_t itype=0,Int_t j1=1,Int_t j2=0,Int_t jtype=1); // Provide Burst and Event data space and time matching info
  void ListBurstHistograms() const;            // Provide a list of all the stored transient burst histograms
  void WriteBurstHistograms(TString filename); // Write all stored transient burst histograms to a ROOT output file
  void MakeBurstZdist(TString file,TString tree,TString name,Int_t nb=200,Float_t zmin=0,Float_t zmax=20); // Make transient burst observed redshift distribution
  void MakeBurstT90dist(TString file,TString tree,TString name,Int_t nb=50,Float_t xmin=-5,Float_t xmax=5); // Make transient burst observed T90 distribution
  void MakeBurstSigmaPosdist(TString file,TString tree,TString name,TString u,Int_t nb=900,Float_t xmin=0,Float_t xmax=90); // Make burst observed position uncertainty dist.
  void MakeBurstEnergydist(Int_t mode,TString file,TString tree,TString name1,TString name2,TString u,Double_t Emin,Double_t Emax,Int_t nb=1000);
  void MakeBurstEnergydist(Int_t mode,TF1& spec,Double_t Emin,Double_t Emax,Int_t nbins=1000);
  void MakeBurstEnergydist(Int_t mode,Double_t alpha,Double_t Emin,Double_t Emax,Int_t nbins=1000);
  void MakeBurstRecoAngresdist(TString file,TString tree,TString name1,TString name2,TString ua,TString name3,TString ud,Double_t Emin,Double_t Emax,Int_t nbe=100,Int_t nba=1000);
  Double_t GetBurstSignalEnergy(Double_t Emin=-1,Double_t Emax=-1) const;
  Double_t GetBurstBackgroundEnergy(Double_t Emin=-1,Double_t Emax=-1) const;
  Double_t GetBurstRecoAngres(Double_t Emin=-1,Double_t Emax=-1,Double_t Amin=0,Double_t Amax=999) const;
  TH1* GetBurstBayesianSignalRate(Double_t p,Double_t& rlow,Double_t& rup,Int_t n=1000); // Provide transient burst Bayesian signal rate and credible interval 
  Double_t GetBurstLiMaSignificance() const; // Provide the transient burst Li-Ma signal significance
  void GetBurstBayesianPsiStatistics(TString type,Double_t nr=-1,Int_t ncut=10,Int_t ndt=2,Bool_t zcor=kFALSE,Int_t freq=0); // Provide transient burst Bayesian Psi statistics
  void GetBurstChi2Statistics(TString type,Int_t ndt=2,Bool_t zcor=kFALSE); // Provide the transient burst Chi-squared statistics

  // Facilities for the SkyMapPanel GUI
  virtual void SkyMapPanel();
  void MapLocl(const char* text);
  void MapLocb(const char* text);
  void MapUloc(Int_t i);
  void MapExperiment(Int_t i);
  void MapLocId(const char* text);
  void MapLocEnter();
  void MapDateTime(const char* text);
  void MapTimeType(Int_t i);
  void MapLabTS();
  void MapLabLframeEnter();
  void MapCinfo(Int_t);
  void MapTinfo(Int_t);
  void MapUinfo(Int_t);
  void MapIname(const char* text);
  void MapInfo();
  void MapEa(const char* text);
  void MapUa(Int_t i);
  void MapEb(const char* text);
  void MapUb(Int_t i);
  void MapEcoord(Int_t i);
  void MapEtype(Int_t i);
  void MapEname(const char* text);
  void MapEnter();
  void MapRemove();
  void MapDcoord(Int_t i);
  void MapProj(Int_t i);
  void MapMerMode(Int_t i);
  void MapMerC(const char* text);
  void MapMerUc(Int_t i);
  void MapDoptions(Int_t i);
  void MapNmax(const char* text);
  void MapNdigs(const char* text);
  void MapDname(const char* text);
  void MapMarkSize(const char* text);
  void MapMarkStyle(Int_t i);
  void MapMarkColor(Int_t i);
  void MapMarkType(Int_t i);
  void MapSolar(Int_t i);
  void MapEnterSolar();
  void MapRemoveSolar();
  void MapList();
  void MapDraw();
  void MapClose();
  void MapExit();

 protected:
  NcPosition fLabPos;    // Position of the lab in the terrestrial longitude-latitude frame
  TString fExperiment;   // The name of the experimental site
  Int_t fLabId;          // The identifier of the detector element 
  Double_t fToffset;     // Lab time offset in fractional hours w.r.t. UT
  Double_t fAxes[6];     // Orientation angles of the local reference frame axes w.r.t. the MRS reference frame
  TObjArray* fRefs;      // Array holding the reference signals
  TObjArray* fSigs;      // Array holding the measured signals
  ULong64_t fNen[2];     // Counters to record every storage of an unnamed reference c.q. measurement signal
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
  NcSignal* SetSignal(Nc3Vector* r,TString frame,TString mode,NcTimestamp* ts,Int_t jref,TString name,Int_t type); // Generic signal storage
  NcSignal* GetSignal(Nc3Vector& r,TString frame,TString mode,NcTimestamp* ts,Int_t jref,Int_t type); // Provide stored signal data
  Int_t SetSolarSystem(TString name,NcTimestamp* ts,Int_t type=0); // Set c.q. update coordinates for solar system objects
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
  Double_t GetSeparation(Int_t i,Int_t j,TString au,Double_t& dt,TString tu,Int_t mode,Int_t bkgpatch,Double_t* diftheta=0,Double_t* difphi=0); // Provide space and time separation

  // The skymap display facilities
  Int_t fUsMeridian;      // Flag to denote the (user) selection of the central meridian and display mode
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

  // The variables for the various SkyMapPanel GUI facilities
  TGMainFrame* fSkyMapPanel;            //! The main frame for the SkyMapPanel GUI 
  TGNumberEntryField* fMapLabLBI[3];    //! The GUI number entries for the Lab location specification
  TGComboBox* fMapLabU;                 //! The GUI Lab location angular unit selection box
  TGComboBox* fMapLabE;                 //! The GUI Lab experiment site selection box
  Double_t fMapLabLocL;                 //! The GUI entered Lab longitude
  Double_t fMapLabLocB;                 //! The GUI entered Lab latitude
  TString fMapLabLocU;                  //! The GUI entered Lab location angular units
  TString fMapLabExpName;               //! The GUI entered Lab experimental site
  Int_t fMapLabId;                      //! The GUI entered Lab detector Id
  TGTextEntry* fMapTSdatetime;          //! The GUI TS date/time specification
  TGComboBox* fMapTStimetype;           //! The GUI TS time type selection box
  TString fMapDate;                     //! The GUI entered date
  TString fMapTime;                     //! The GUI entered time
  TString fMapTimeType;                 //! The GUI entered time type
  TString fMapDateTime;                 //! The GUI entered datetime 
  NcTimestamp fMapTS;                   //! The GUI entered timestamp to be used for the List/Map
  Bool_t fMapLabTS;                     //! The GUI selection to use the Lab timestamp for the List/Map
  TGNumberEntryField* fMapLabLframe[6]; //! The GUI number entries for the local frame specification
  TString fMapCinfo;                    //! The GUI selected info category 
  Int_t fMapTinfo;                      //! The GUI selected mode for the timestamp info
  TString fMapUinfo;                    //! The GUI selected angular units for the Lab info 
  TString fMapIname;                    //! The GUI selected entry name for the info
  Double_t fMapEa;                      //! The GUI entered a coordinate of an entry
  TString fMapEua;                      //! The GUI entered angular units of a
  Double_t fMapEb;                      //! The GUI entered b coordinate of an entry
  TString fMapEub;                      //! The GUI entered angular units of b
  Int_t fMapEtype;                      //! The GUI entered entry type
  TString fMapEcoord;                   //! The GUI entered coordinate system of the entry
  TString fMapEmode;                    //! The GUI entered coordinate system mode of the entry
  TString fMapEname;                    //! The GUI entered name of the entry
  TString fMapDcoord;                   //! The GUI selected coordinate system for the Map/List
  TString fMapProj;                     //! The GUI selected projection for the Map
  TString fMapDmode;                    //! The GUI selected coordinate system mode for the Map/List
  Bool_t fMapDoptions[5];               //! The GUI Map/List options (histo, clr, ref, meas, refTS)
  Int_t fMapNmax;                       //! The GUI selected max. number of signals of each type to Map/List
  Int_t fMapNdigs;                      //! The GUI selected number of digits for the List output
  TString fMapDname;                    //! The GUI entered name pattern for entries to be shown in the Map/List
  Bool_t fMapSolar[10];                 //! The GUI selection of solar system objects
  Int_t fMapMerMode;                    //! The GUI selected meridian orientation for the Map
  Double_t fMapMerC;                    //! The GUI entered central meridian location for the Map
  TString fMapMerUc;                    //! The GUI selected angular units for the central meridian location
  Double_t fMapMarkSize;                //! The GUI entered marker size for the Map
  Int_t fMapMarkStyle;                  //! The GUI selected marker style for the Map
  Int_t fMapMarkColor;                  //! The GUI selected marker color for the Map
  Int_t fMapMarkType;                   //! The GUI selected entry type to apply the marker attributes on

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
  Double_t fGn;      // The nominal value of the gravitational acceleration at the Earth surface in m s^-2
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

  // Function to parametrize the Neutrino-Lepton kinematic opening angle
  TF1* fNuAngle;

  // Actual calculation of the posterior Bayesian probability for background c.q. source signal rates
  Double_t GetBackgroundRateProb(Double_t* vars,Double_t* pars); // Posterior Bayesian probability for a background rate "b"
  Double_t GetSignalRateProb(Double_t* vars,Double_t* pars); // Posterior Bayesian probability for a source signal rate "s"

  // Specifications of the data from a ROOT input Tree  
  Int_t fDataDir;         // Indicator for arrival (1) or moving (-1) directions
  TString fDataFrame;     // The frame in which coordinates are provided
  TString fDataMode;      // Indicator for equatorial coordinates mode (Mean, True, B1950 or J2000)
  NcObjMatrix fDataNames; // The correspondence table between physical observables and variable names

  // Storage for transient burst investigations
  NcDevice* fBurstParameters; // Various parameters describing the transient burst
  TObjArray fBurstHistos;     // Storage of all the produced transient burst histograms
  NcSample fBurstOnReco;      // The On-source reconstructed track data
  NcSample fBurstOnMatch;     // The On-source matching data
  NcSample fBurstSigReco;     // The simulated signal reconstructed track data
  NcSample fBurstSignal;      // The simulated signal events
  NcSample fBurstOffReco;     // The Off-source reconstructed track data
  NcSample fBurstOffMatch;    // The Off-source matching data

  // Internal functions for transient burst investigations
  void BurstCompensate(Int_t& nmugrb);
  void InitBurstHistograms(Int_t mode);
  TH1* GetBurstZdist(TString name,TString type);
  TH1* GetBurstT90dist(TString name,TString type);
  TH1* GetBurstSigmaPosdist(TString name,TString type);
  void MakeBurstDataStats(Int_t mode,Int_t nmugrb=0);
  void GetBurstDtDistributions(Int_t ndt,TH1F& hisdtOn,TF1& pdfdtOn,TH1F& hisdtOff,TF1& pdfdtOff,Bool_t zcor);

  // Internal functions for the composition of the various sub-panels of the SkyMapPanel GUI
  virtual void LabLocationPanel(TGCompositeFrame* frame);
  virtual void TimestampPanel(TGCompositeFrame* frame);
  virtual void LabLocalFramePanel(TGCompositeFrame* frame);
  virtual void InfoPanel(TGCompositeFrame* frame);
  virtual void EntriesPanel(TGCompositeFrame* frame);
  virtual void MapListOptionsPanel(TGCompositeFrame* frame);
  virtual void CommandPanel(TGCompositeFrame* frame);
  void SetMapTS();
 
 ClassDef(NcAstrolab,43) // Virtual lab to provide (astro)physical parameters, treat data and relate observations with astrophysical phenomena
};
#endif
