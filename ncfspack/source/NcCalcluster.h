#ifndef NcCalcluster_h
#define NcCalcluster_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcCalcluster.h 5 2010-03-19 10:10:02Z nickve $

#include <math.h>
 
#include "TObjArray.h"
#include "TString.h"
 
#include "NcCalmodule.h"
#include "NcMath.h"
 
class NcCalcluster : public NcSignal
{
 public:
  NcCalcluster();                          // Default constructor, all data initialised to 0
  virtual ~NcCalcluster();                 // Default destructor
  NcCalcluster(const NcCalcluster& c);     // Copy constructor
  NcCalcluster(NcCalmodule& m);            // Create new cluster starting at module m
  Int_t GetRow() const;                    // Return row number of cluster center
  Int_t GetColumn() const;                 // Return column number of cluster center
  Int_t GetNmodules() const;               // Return number of modules in cluster
  Float_t GetRowDispersion() const;        // Return normalised row dispersion of cluster
  Float_t GetColumnDispersion() const;     // Return normalised column dispersion of cluster
  void Start(NcCalmodule& m);              // Reset cluster data to start with module m
  void Add(NcCalmodule& m);                // Add module data to cluster
  void AddVetoSignal(NcSignal& s,Int_t extr=1); // Associate (extrapolated) signal
  void AddVetoSignal(NcSignal* s,Int_t extr=1) { AddVetoSignal(*s,extr); }
  NcSignal* GetVetoSignal(Int_t j) const; // Access to veto signal number j
  Int_t GetNvetos() const;                 // Provide the number of veto signals
  Float_t GetVetoLevel() const;            // Provide confidence level of best associated veto hit
  Int_t HasVetoHit(Double_t cl) const;     // Check for ass. veto hit with conf. level > cl
 
 protected:
  Int_t fRow;            // Row index of the central module of the cluster
  Int_t fCol;            // Column index of the central module of the cluster
  Int_t fNmods;          // The number of modules in the cluster
  Float_t fRowdisp;      // Row dispersion of cluster (not normalised)
  Float_t fColdisp;      // Column dispersion of cluster (not normalised)
  Int_t fNvetos;         // The number of associated veto signals
  TObjArray* fVetos;     // The array of associated veto signals
 
 ClassDef(NcCalcluster,1) // Description of a cluster of calorimeter modules.
};
#endif
