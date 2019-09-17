#ifndef IceDwalkx_h
#define IceDwalkx_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceDwalkx.h 63 2012-07-13 14:17:43Z nickve $

#include "TArrayI.h"

#include "NcSample.h"

#include "IceLinefit.h"

class IceDwalkx : public IceLinefit
{
 public :
  IceDwalkx(const char* name="IceDwalkx",const char* title="Direct walk reconstruction"); // Constructor
  virtual ~IceDwalkx();                                // Destructor
  virtual void Exec(Option_t* opt);                    // Direct walk reconstruction
  void SetDmin(Float_t d,TString s);                   // Set minimum hit distance to form a track element
  void SetDtmarg(Int_t dt,TString s);                  // Set maximum hit time difference margin for track elements
  void SetMaxDhit(Float_t d,TString s);                // Set maximum distance (in scat. length) for hit association 
  void SetTangmax(Float_t ang,TString s);              // Set max. angular separation for track candidate clustering into jets
  void SetTdistmax(Float_t d,TString s,Int_t invol=1); // Set maximum track distance for track candidate clustering
  void SetJangmax(Float_t ang,TString s,Int_t iter=1); // Set max. angular separation for jet merging into 1 single track
  void SetJdistmax(Float_t d,TString s,Int_t invol=1); // Set maximum jet distance for jet merging
  void SetVgroupUsage(Int_t flag,TString s);           // (De)activate usage of distinct phase and group velocities
  void SetAsType(Int_t flag,TString s);                // Select # assoc. hits or strings for quality indicator
  void SetScatteringLength(Float_t lambda,TString s);  // Set average photon scattering length
  void SetConditionalReco(Int_t flag);                 // (De)activate conditional reconstruction for the various detector combinations

 protected :
  IceEvent* fEvt;      // Pointer to the event structure
  Float_t fDminA;      // Minimum Amanda OM hit distance (in m) to form a track element 
  Float_t fDminI;      // Minimum InIce DOM hit distance (in m) to form a track element 
  Float_t fDminIC;     // Minimum standard IceCube InIce DOM hit distance (in m) to form a track element 
  Float_t fDminDC;     // Minimum DeepCore DOM hit distance (in m) to form a track element 
  Int_t fDtmargA;      // Maximum Amanda OM hit time difference margin (in ns) for track elements
  Int_t fDtmargI;      // Maximum InIce DOM hit time difference margin (in ns) for track elements
  Int_t fDtmargIC;     // Maximum standard IceCube InIce DOM hit time difference margin (in ns) for track elements
  Int_t fDtmargDC;     // Maximum DeepCore DOM hit time difference margin (in ns) for track elements
  Float_t fMaxdhitA;   // Maximum Amanda OM hit distance (in scat. length) for hit association
  Float_t fMaxdhitI;   // Maximum InIce DOM hit distance (in scat. length) for hit association
  Float_t fMaxdhitIC;  // Maximum standard IceCube InIce DOM hit distance (in scat. length) for hit association
  Float_t fMaxdhitDC;  // Maximum DeepCore DOM hit distance (in scat. length) for hit association
  Float_t fTangmaxA;   // Amanda angular separation (in deg) within which track candidates are clustered in a jet
  Float_t fTangmaxI;   // InIce angular separation (in deg) within which track candidates are clustered in a jet
  Float_t fTangmaxIC;  // Standard IceCube InIce angular separation (in deg) within which track candidates are clustered in a jet
  Float_t fTangmaxDC;  // DeepCore angular separation (in deg) within which track candidates are clustered in a jet
  Float_t fTdistmaxA;  // Maximum Amanda track distance (in m) for track candidate clustering
  Float_t fTdistmaxI;  // Maximum InIce track distance (in m) for track candidate clustering
  Float_t fTdistmaxIC; // Maximum standard IceCube InIce track distance (in m) for track candidate clustering
  Float_t fTdistmaxDC; // Maximum DeepCore track distance (in m) for track candidate clustering
  Int_t fTinvolA;      // Amanda flag to denote maximum track distance testing inside/outside detector volume
  Int_t fTinvolI;      // InIce flag to denote maximum track distance testing inside/outside detector volume
  Int_t fTinvolIC;     // Standard IceCube InIce flag to denote maximum track distance testing inside/outside detector volume
  Int_t fTinvolDC;     // DeepCore flag to denote maximum track distance testing inside/outside detector volume
  Float_t fJangmaxA;   // Amanda angular separation (in deg) within which jets are merged into 1 single track
  Float_t fJangmaxI;   // InIce angular separation (in deg) within which jets are merged into 1 single track
  Float_t fJangmaxIC;  // Standard IceCube InIce angular separation (in deg) within which jets are merged into 1 single track
  Float_t fJangmaxDC;  // DeepCore angular separation (in deg) within which jets are merged into 1 single track
  Int_t fJiterateA;    // Amanda flag to indicate iteration in the jet merging process
  Int_t fJiterateI;    // InIce flag to indicate iteration in the jet merging process
  Int_t fJiterateIC;   // Standard IceCube InIce flag to indicate iteration in the jet merging process
  Int_t fJiterateDC;   // DeepCore flag to indicate iteration in the jet merging process
  Float_t fJdistmaxA;  // Amanda maximum jet distance (in m) for jet merging
  Float_t fJdistmaxI;  // InIce maximum jet distance (in m) for jet merging
  Float_t fJdistmaxIC; // Standard IceCube InIce maximum jet distance (in m) for jet merging
  Float_t fJdistmaxDC; // DeepCore maximum jet distance (in m) for jet merging
  Int_t fJinvolA;      // Amanda flag to denote maximum jet distance testing inside/outside detector volume
  Int_t fJinvolI;      // InIce flag to denote maximum jet distance testing inside/outside detector volume
  Int_t fJinvolIC;     // Standard IceCube InIce flag to denote maximum jet distance testing inside/outside detector volume
  Int_t fJinvolDC;     // DeepCore flag to denote maximum jet distance testing inside/outside detector volume
  Int_t fVgroupA;      // Amanda flag to indicate usage of distinct phase and group velocities
  Int_t fVgroupI;      // InIce flag to indicate usage of distinct phase and group velocities
  Int_t fVgroupIC;     // Standard IceCube InIce flag to indicate usage of distinct phase and group velocities
  Int_t fVgroupDC;     // DeepCore flag to indicate usage of distinct phase and group velocities
  Int_t fAsTypeA;      // Amanda flag to indicate usage of # assoc. hits or strings for quality
  Int_t fAsTypeI;      // InIce flag to indicate usage of # assoc. hits or strings for quality
  Int_t fAsTypeIC;     // Standard IceCube InIce flag to indicate usage of # assoc. hits or strings for quality
  Int_t fAsTypeDC;     // DeepCore flag to indicate usage of # assoc. hits or strings for quality
  Float_t fLambdaA;    // Average photon scattering length (in meter) for Amanda OMs
  Float_t fLambdaI;    // Average photon scattering length (in meter) for InIce DOMs
  Float_t fLambdaIC;   // Average photon scattering length (in meter) for standard IceCube InIce DOMs
  Float_t fLambdaDC;   // Average photon scattering length (in meter) for DeepCore DOMs
  Int_t fConditional;  // Flag to indicate conditional reconstruction of the various detector combinations

  virtual void Amanda();    // Direct walk reconstruction for Amanda OM signals
  virtual Int_t InIce();    // Direct walk reconstruction for all InIce DOM signals
  virtual Int_t IceCube();  // Direct walk reconstruction for the standard IceCube InIce DOM signals
  virtual Int_t DeepCore(); // Direct walk reconstruction for the DeepCore DOM signals
  virtual void AssociateHits(TObjArray& tes,TObjArray& hits,Int_t vgroup,Float_t maxdhit,Int_t astype,Float_t& qmax,Float_t lambda);// Hit association
  virtual void SelectQvalue(TObjArray& tes,Int_t astype,Float_t qmax); // TC selection via Q-value
  virtual void ClusterTracks(TObjArray& tes,TObjArray& jets,Float_t tangmax,Int_t tinvol,Float_t tdistmax,Int_t astype,Float_t qmax);// Track clustering  
  virtual void MergeJets(TObjArray& jets,Float_t jangmax,Float_t jdistmax,Int_t jinvol,Int_t jiterate,Int_t astype);// Jet Merging
  virtual void StoreTracks(TObjArray& jets,Float_t jangmax,TString name,TString title); // Final track storage

 ClassDef(IceDwalkx,3) // TTask derived class to perform (improved) direct walk reconstruction
};
#endif
