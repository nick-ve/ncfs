#ifndef NcDevice_h
#define NcDevice_h
// Copyright(c) 2004 NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include "TPolyMarker3D.h"
#include "TMath.h"

#include "NcSignal.h"

/** Signal (Hit) handling of a generic device. **/

class NcDevice : public NcSignal
{
 public:
  NcDevice(const char* name="",const char* title="");// Default constructor
  virtual ~NcDevice();                               // Default destructor
  NcDevice(const NcDevice& dev);                     // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer
  virtual void SetOwner(Bool_t own=kTRUE);           // Set ownership of all added objects
  void SetStatus(Int_t word);                        // Set the status word (user definable)
  Int_t GetStatus() const;                           // Provide the status word
  void SetHitCopy(Int_t j);                          // (De)activate creation of private copies of hits
  Int_t GetHitCopy() const;                          // Provide HitCopy flag value      
  void AddHit(NcSignal& s);                          // Register an NcSignal object as a hit to this module
  void AddHit(NcSignal* s) { if (s) AddHit(*s); }
  void RemoveHit(NcSignal& s);                       // Remove NcSignal object as hit from this module
  void RemoveHit(NcSignal* s) { if (s) RemoveHit(*s); }
  void RemoveHits();                                 // Remove all NcSignals as hits from this module
  Int_t GetNhits() const;                            // Provide number of registered hits
  Int_t GetNhits(TString name,Int_t mode=0,Int_t opt=0) const; // Provide number of hits registered with the specified hit or slot name
  NcSignal* GetHit(Int_t j) const;                   // Access to the NcSignal registered as hit number j
  NcSignal* GetHit(TString name,Int_t mode=0,Int_t opt=0) const; // Provide the hit with the specified hit or slot name
  NcSignal* GetIdHit(Int_t id) const;                // Provide the hit with unique identifier "id"
  TObjArray* GetHits();                              // Provide the references to all the registered hits
  void GetHits(TObjArray& selected,TString name,Int_t mode=0,Int_t opt=0,TObjArray* hits=0) const; // Provide the references to all the hits with the specified hit or slot name
  virtual void Reset(Int_t mode=0);                  // Reset registered hits and NcSignal attributes
  void ShowHit(Int_t j=0,TString f="car",TString u="rad") const; // Show data of the j-th hit (j=0 means all hits) for frame f and ang units u
  virtual void Data(TString f="car",TString u="rad") const; // Print device/signal info for frame f and ang units u
  void GetExtremes(Float_t& vmin,Float_t& vmax,Int_t idx=1,TObjArray* hits=0,Int_t mode=1,Int_t deadcheck=1) const; // min and max signal
  void GetExtremes(Float_t& vmin,Float_t& vmax,TString name,TObjArray* hits=0,Int_t mode=1,Int_t deadcheck=1) const;// min and max signal
  TObjArray* SortHits(TString name,Int_t mode=-1,TObjArray* hits=0,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);// Sort hits by named signal value
  TObjArray* SortHits(Int_t idx=1,Int_t mode=-1,TObjArray* hits=0,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0); // Sort hits by indexed signal value
  void DisplayHits(TString name,Float_t scale=-1,TObjArray* hits=0,Int_t dp=0,Int_t mode=1,Int_t mcol=4);// Hit disp.
  void DisplayHits(Int_t idx=1,Float_t scale=-1,TObjArray* hits=0,Int_t dp=0,Int_t mode=1,Int_t mcol=4); // Hit disp.
  Double_t SumSignals(Int_t idx,Int_t mode=1,TObjArray* hits=0);    // Summation of selected signal values
  Double_t SumSignals(TString name,Int_t mode=1,TObjArray* hits=0); // Summation of selected signal values
  Double_t SlideWindow(TObjArray* hits,Double_t thres,Double_t swin,TString sname,Int_t smode=0,TString wname="none",Int_t wmode=0,Int_t* i1=0,Int_t* i2=0) const; // Sliding window scan of signals
  Nc3Vector GetHitPath(TObjArray* hits,Int_t pos=0) const; // Provide the average direction of specified hit pattern
  NcPosition GetCOG(TObjArray* hits,Int_t pos=0,TString slotname="none",Int_t mode=0) const; // Provide the Center Of Gravity of the specified hits
  Double_t GetCVAL(TObjArray* hits,TString obsname,TString weightname="none",Int_t mode=0,Int_t type=1) const; // Provide the Central Value of "obsname" of the specified hits

 protected:
  Int_t fStatus;       // User definable status word
  Int_t fHitCopy;      // Flag to denote making private copies of added hits
  TObjArray* fHits;    // Array to hold the registered hits
  TObjArray* fOrdered; ///<! Temp. array to hold the ordered hits
  TObjArray* fMarkers; ///<! Temp. array to hold the 3D markers for the hit display

 ClassDef(NcDevice,13) // Signal (Hit) handling of a generic device.
};
#endif
