#ifndef NcHelix_h
#define NcHelix_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcHelix.h 5 2010-03-19 10:10:02Z nickve $

#include "THelix.h"
#include "TObjArray.h"
#include "TPolyMarker3D.h"

#include "Nc3Vector.h"
#include "NcTrack.h"
#include "NcEvent.h"
 
class NcHelix : public THelix
{
 public:
  NcHelix();                   // Default constructor
  virtual ~NcHelix();          // Destructor
  NcHelix(const NcHelix& h);   // Copy constructor
  void SetB(Nc3Vector& b);     // Set the magnetic field vector in Tesla
  Nc3Vector& GetB();           // Provide the magnetic field vector in Tesla
  void SetTofmax(Float_t tof); // Set maximum time of flight
  Float_t GetTofmax() const;   // Provide the maximum time of flight
  void Display(NcTrack* t,Double_t* range=0,Int_t iaxis=3,Double_t scale=-1);   // Show curve for this track
  void Display(NcEvent* e,Double_t* range=0,Int_t iaxis=3,Double_t scale=-1);   // Show curves for this event
  void Display(TObjArray* a,Double_t* range=0,Int_t iaxis=3,Double_t scale=-1); // Show curves for this track array
  void Refresh(Int_t mode=0);  // Refresh the view before drawing the next one
  NcPosition* Extrapolate(NcTrack* t,Double_t* pars=0,Double_t scale=-1); // Extrapolate this track
  void MakeCurve(NcTrack* t,Double_t* range=0,Int_t iaxis=3,Double_t scale=-1); // Helix curve for this track
  void SetMarker(Int_t marker=8,Float_t size=0.2,Int_t color=-1); // Set marker for track starting point 
  void UseEndPoint(Int_t mode=1); // (De)Select (0/1) usage of track endpoint for drawing and extrapolation

 protected:
  Nc3Vector fB;        // The magnetic field vector in Tesla
  Float_t fTofmax;     // The maximum time of flight
  Int_t fRefresh;      // Auto-refresh flag for drawings
  Int_t fMstyle;       // The marker style to indicate the track starting point
  Float_t fMsize;      // The size of the marker
  Int_t fMcol;         // The colour of the marker
  Int_t fEnduse;       // Flag to denote tracks endpoint usage
  TObjArray* fCurves;  //! Temp. storage for the curves on the drawing
  NcPosition* fExt;    //! The extrapolation result
 
 ClassDef(NcHelix,1) // Representation and extrapolation of NcTracks in a magnetic field.
};
#endif
