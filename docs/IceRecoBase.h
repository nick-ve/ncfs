#ifndef IceRecoBase_h
#define IceRecoBase_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceRecoBase.h 83 2013-03-15 15:22:45Z nickve $

#include "TROOT.h"
#include "TTask.h"
#include "TString.h"
#include "TObjString.h"

#include "NcJob.h"
#include "IceEvent.h"
#include "IceGOM.h"

class IceRecoBase : public TTask
{
 public :
  IceRecoBase(const char* name="IceRecoBase",const char* title="Base class for IceCube reconstruction tasks"); // Constructor
  virtual ~IceRecoBase();                    // Destructor
  virtual void Exec(Option_t* opt);          // Base for the actual reconstruction processor
  void SetCleaned(Int_t flag,TString s);     // Set flag to select usage of only cleaned hits or all hits
  void SetMaxMod(Int_t nmax,TString s);      // Set max. number of good fired (D)OMs for events to be processed
  void SetMinMod(Int_t nmin,TString s);      // Set min. number of good fired (D)OMs for events to be processed
  void SetMaxHits(Int_t nmax,TString s);     // Set max. number of good hits per (D)OM to be processed
  void SetSingleHit(Int_t ndoms,TString s,Int_t ndoms1=0);  // Set minimum number of good firing (D)OMs at which only the first hit will be used
  void SetMinAhits(Int_t nmin,TString s);    // Set min. number of associated hits to produce a reconstructed track
  void SetMinAmods(Int_t nmin,TString s);    // Set min. number of associated (D)OMs to produce a reconstructed track
  void SetSLChitUsage(Int_t flag,TString s); // (De)activate usage of Soft Local Coincidence hits
  void SetFlipAngles(Float_t thetatrk,Float_t thetahits); // Polar angle thresholds for track direction flipping
  void SetScatteringLength(Float_t lambda,TString s);  // Set average photon scattering length
  void SetAbsorptionLength(Float_t lambda,TString s);  // Set average photon absorbtion length
  void SetTimeJitter(Float_t sigma,TString s);         // Set average PMT time jitter in ns.
  void SetTrackName(TString s);              // Set (alternative) name for the produced reconstructed tracks
  void SetCharge(Float_t charge);            // Set user defined charge for the produced reconstructed tracks
  void UseTracks(TString classname,Int_t n=-1); // Specify input tracks to be used for track based reconstruction
  void SetVgroupUsage(Int_t flag,TString s);    // (De)activate usage of distinct phase and group velocities

 protected :
  Int_t fFirst;         // Flag to denote first invokation of the processor
  IceEvent* fEvt;       // Pointer to the current event
  Int_t fCleanA;        // Amanda flag to indicate usage of only cleaned hits
  Int_t fCleanI;        // InIce flag to indicate usage of only cleaned hits
  Int_t fCleanIC;       // Standard IceCube InIce flag to indicate usage of only cleaned hits
  Int_t fCleanDC;       // DeepCore flag to indicate usage of only cleaned hits
  Int_t fMaxmodA;       // The max. number of good fired Amanda OMs for events to get processed
  Int_t fMaxmodI;       // The max. number of good fired InIce DOMs for events to get processed
  Int_t fMaxmodIC;      // The max. number of good fired standard IceCube InIce DOMs for events to get processed
  Int_t fMaxmodDC;      // The max. number of good fired DeepCore DOMs for events to get processed
  Int_t fMinmodA;       // The min. number of good fired Amanda OMs for events to get processed
  Int_t fMinmodI;       // The min. number of good fired InIce DOMs for events to get processed
  Int_t fMinmodIC;      // The min. number of good fired standard IceCube InIce DOMs for events to get processed
  Int_t fMinmodDC;      // The min. number of good fired DeepCore DOMs for events to get processed
  Int_t fMaxhitsA;      // The maximum number of good hits per Amanda OM to be processed
  Int_t fMaxhitsI;      // The maximum number of good hits per InIce DOM to be processed
  Int_t fMaxhitsIC;     // The maximum number of good hits per standard IceCube InIce DOM to be processed
  Int_t fMaxhitsDC;     // The maximum number of good hits per DeepCore DOM to be processed
  Int_t fSingleA;       // Minimum number of good firing Amanda OMs at which only first hits will be used
  Int_t fSingleIC;      // Minimum number of good firing standard IceCube InIce DOMs at which only first hits will be used
  Int_t fSingleI;       // Minimum number of good firing InIce DOMs at which only first hits will be used
  Int_t fSingleDC;      // Minimum number of good firing DeepCore DOMs at which only first hits will be used
  Int_t fSingle1A;      // Minimum number of good firing Amanda OMs on a single string at which only first hits will be used
  Int_t fSingle1IC;     // Minimum number of good firing standard IceCube InIce DOMs on a single string at which only first hits will be used
  Int_t fSingle1I;      // Minimum number of good firing InIce DOMs on a single string at which only first hits will be used
  Int_t fSingle1DC;     // Minimum number of good firing DeepCore DOMs on a single string at which only first hits will be used
  Int_t fMinahitsA;     // The minimum number of associated hits to produce an Amanda reconstructed track
  Int_t fMinahitsI;     // The minimum number of associated hits to produce a hybrid InIce (I) reconstructed track
  Int_t fMinahitsIC;    // The minimum number of associated hits to produce a standard IceCube (IC) reconstructed track
  Int_t fMinahitsDC;    // The minimum number of associated hits to produce a DeepCore (DC) reconstructed track
  Int_t fMinamodsA;     // The minimum number of associated OMs to produce an Amanda reconstructed track
  Int_t fMinamodsI;     // The minimum number of associated DOMs to produce a hybrid InIce (I) reconstructed track
  Int_t fMinamodsIC;    // The minimum number of associated DOMs to produce a standard IceCube (IC) reconstructed track
  Int_t fMinamodsDC;    // The minimum number of associated DOMs to produce a DeepCore (DC) reconstructed track
  Int_t fSlcI;          // InIce flag to indicate usage of SLC hits
  Int_t fSlcIC;         // Standard IceCube InIce flag to indicate usage of SLC hits
  Int_t fSlcDC;         // DeepCore flag to indicate usage of SLC hits
  Float_t fThetatrk;    // Track polar angle threshold (in degrees) for track direction flipping
  Float_t fThetahits;   // Hit path polar angle threshold (in degrees) for track direction flipping
  Float_t fLambdaA;     // Average photon scattering length (in meter) for Amanda OMs
  Float_t fLambdaUD;    // Average photon scattering length (in meter) for InIce DOMs in the Upper Detector above the dust layer
  Float_t fLambdaDL;    // Average photon scattering length (in meter) for InIce DOMs in the Dust Layer
  Float_t fLambdaLD;    // Average photon scattering length (in meter) for InIce DOMs in the Lower Detector below the dust layer
  Float_t fLabsA;       // Average photon absorption length (in meter) for Amanda OMs
  Float_t fLabsUD;      // Average photon absorption length (in meter) for InIce DOMs in the Upper Detector above the dust layer
  Float_t fLabsDL;      // Average photon absorption length (in meter) for InIce DOMs in the Dust Layer
  Float_t fLabsLD;      // Average photon absorption length (in meter) for InIce DOMs in the Lower Detector below the dust layer
  Float_t fTsigmaA;     // PMT jitter (in ns) for Amanda OMs
  Float_t fTsigmaIC;    // PMT jitter (in ns) for standard IceCube DOMs
  Float_t fTsigmaDC;    // PMT jitter (in ns) for DeepCore DOMs
  Int_t fVgroupA;       // Amanda flag to indicate usage of distinct phase and group velocities
  Int_t fVgroupI;       // InIce flag to indicate usage of distinct phase and group velocities
  Int_t fVgroupIC ;     // Standard IceCube InIce flag to indicate usage of distinct phase and group velocities
  Int_t fVgroupDC ;     // DeepCore flag to indicate usage of distinct phase and group velocities
  TString fTrackname;   // The name identifier for the produced first guess tracks
  Float_t fCharge;      // User defined charge of the produced first guess tracks
  TObjArray* fUseNames; // The first guess classnames to be used 
  TArrayI* fUseNtk;     // The max. numbers of the various first guess tracks to be used
  NcDevice fParams;     // Device to store the parameters for this processor

  virtual void FlipTrack(NcTrack* t) const; // Flip the track direction if needed

 ClassDef(IceRecoBase,3) // TTask derived base class for the various IceCube reconstruction tasks
};
#endif
