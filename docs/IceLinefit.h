#ifndef IceLinefit_h
#define IceLinefit_h

// Copyright(c) 2003, IceCube Experiment at the South Pole, All rights reserved.
// See cxx source for full Copyright notice.

// $Id: IceLinefit.h 72 2013-01-21 13:13:40Z nickve $

#include "IceRecoBase.h"

class IceLinefit : public IceRecoBase
{
 public :
  IceLinefit(const char* name="IceLinefit",const char* title="Linefit reconstruction"); // Constructor
  virtual ~IceLinefit();                                // Destructor
  virtual void Exec(Option_t* opt);                     // Linefit reconstruction

 protected :
  Int_t Amanda();   // Linefit complete event reconstruction for Amanda OMs
  Int_t InIce();    // Linefit complete event reconstruction for all InIce DOMs
  Int_t IceCube();  // Linefit complete reconstruction for standard IceCube InIce DOMs
  Int_t DeepCore(); // Linefit complete event reconstruction for DeepCore DOMs
  Int_t Tracks();   // Linefit track based associated hit reconstruction
  NcTrack* Reconstruct(TObjArray* doms,Int_t cln,Int_t minmod,Int_t maxmod,Int_t maxhits,Int_t minahits,Int_t minamods,Int_t slc); // The actual reconstruction algorithm

 ClassDef(IceLinefit,9) // IceRecoBase derived TTask processor to perform linefit reconstruction
};
#endif
