#ifndef IceDwalk_h
#define IceDwalk_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceDwalk.h 77 2013-01-24 10:33:57Z nickve $

#include "TArrayI.h"

#include "NcSample.h"

#include "IceRecoBase.h"

class IceDwalk : public IceRecoBase
{
 public :
  IceDwalk(const char* name="IceDwalk",const char* title="Direct walk reconstruction"); // Constructor
  virtual ~IceDwalk();                                 // Destructor
  virtual void Exec(Option_t* opt);                    // Direct walk reconstruction
  void SetDmin(Float_t d,TString s);                   // Set minimum hit distance to form a track element
  void SetDtmarg(Int_t dt,TString s);                  // Set maximum hit time difference margin (in ns) for track elements
  void SetMaxDhit(Float_t d,TString s);                // Set maximum distance (in scat. length) for hit association 
  void SetDthit(Float_t dtmin,Float_t dtmax,TString s);// Set time residual interval (in ns) for hit association
  void SetTangmax(Float_t ang,TString s);              // Set max. angular separation for track candidate clustering into jets
  void SetTdistmax(Float_t d,TString s,Int_t invol=1); // Set maximum track distance for track candidate clustering
  void SetJangmax(Float_t ang,TString s,Int_t iter=1); // Set max. angular separation for jet merging into 1 single track
  void SetJdistmax(Float_t d,TString s,Int_t invol=1); // Set maximum jet distance for jet merging
  void SetAsType(Int_t flag,TString s,Float_t w=-1);   // Select # assoc. hits or strings for quality indicator
  void SetHitWeight(Float_t w);                        // Set weight for associated hit count
  void SetConditionalReco(Int_t flag);                 // (De)activate conditional reconstruction for the various detector combinations
  void SetQvalueCut(Float_t qcut);                     // Set threshold for final track Qvalue selection

 protected :
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
  Float_t fDtminA;     // Minimal Amanda value of time residual (in ns) for hit association
  Float_t fDtmaxA;     // Maximal Amanda value of time residual (in ns) for hit association
  Float_t fDtminI;     // Minimal InIce value of time residual (in ns) for hit association
  Float_t fDtmaxI;     // Maximal InIce value of time residual (in ns) for hit association
  Float_t fDtminIC;    // Minimal standard IceCube InIce value of time residual (in ns) for hit association
  Float_t fDtmaxIC;    // Maximal standard IceCube InIce value of time residual (in ns) for hit association
  Float_t fDtminDC;    // Minimal DeepCore value of time residual (in ns) for hit association
  Float_t fDtmaxDC;    // Maximal DeepCore value of time residual (in ns) for hit association
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
  Int_t fAsTypeA;      // Amanda flag to indicate usage of # assoc. hits or strings for quality
  Int_t fAsTypeI;      // InIce flag to indicate usage of # assoc. hits or strings for quality
  Int_t fAsTypeIC;     // Standard IceCube InIce flag to indicate usage of # assoc. hits or strings for quality
  Int_t fAsTypeDC;     // DeepCore flag to indicate usage of # assoc. hits or strings for quality
  Float_t fWstringA;   // Amanda weight for string count w.r.t. hit count in track quality 
  Float_t fWstringI;   // InIce weight for string count w.r.t. hit count in track quality 
  Float_t fWstringIC;  // Standard IceCube InIce weight for string count w.r.t. hit count in track quality 
  Float_t fWstringDC;  // DeepCore weight for string count w.r.t. hit count in track quality 
  Float_t fHitweight;  // Weight for associated hit counting
  Int_t fConditional;  // Flag to indicate conditional reconstruction of the various detector combinations
  Float_t fQcut;       // Threshold for final track Qvalue selection

  Int_t Amanda();    // Direct walk reconstruction for Amanda OM signals
  Int_t IceCube(TObjArray& hits);  // Direct walk reconstruction with the standard IceCube InIce DOM signals c.q. criteria
  Int_t InIce(TObjArray& hits);    // Direct walk reconstruction with all InIce DOM signals c.q. criteria
  Int_t DeepCore(TObjArray& hits); // Direct walk reconstruction with the DeepCore DOM signals c.q. criteria
  Int_t MakeTEs(Int_t cln,Int_t maxhits,Float_t dmin,Float_t dtmarg,Float_t dtmin,Float_t dtmax,TString domclass,TObjArray& tes,TObjArray& hits,Int_t gethits);// TE creation
  void AssociateHits(TObjArray& tes,TObjArray& hits,Int_t astype,Float_t ws,Float_t dtmin,Float_t dtmax,Float_t maxdhit,Int_t vgroup,Int_t cln,Int_t slc,Float_t& qmax);// Hit association
  void SelectQvalue(TObjArray& tes,Float_t qmax); // TC selection via Q-value
  void ClusterTracks(TObjArray& tes,TObjArray& jets,Float_t tangmax,Int_t tinvol,Float_t tdistmax,Float_t qmax);// Track clustering  
  void MergeJets(TObjArray& jets,Float_t jangmax,Float_t jdistmax,Int_t jinvol,Int_t jiterate,Float_t qmax); // Jet Merging
  Int_t StoreTracks(TObjArray& jets,Int_t minahits,Int_t minamods,Float_t jangmax,TString name,TString title,TObjArray& hits); // Final track storage

 ClassDef(IceDwalk,17) // IceRecoBase derived TTask processor to perform (improved) direct walk reconstruction
};
#endif
