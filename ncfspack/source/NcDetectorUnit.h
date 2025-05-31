#ifndef NcDetectorUnit_h
#define NcDetectorUnit_h

// Copyright(c) 2021 NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <math.h>
 
#include "TMath.h"

#include "NcDevice.h"
#include "NcSample.h"

/** Handling of a collection of generic devices. **/

class NcDetectorUnit : public NcDevice
{
 public:
  NcDetectorUnit(const char* name="",const char* title=""); // Default constructor
  virtual ~NcDetectorUnit();                                // Default destructor
  NcDetectorUnit(const NcDetectorUnit& q);                  // Copy constructor
  virtual TObject* Clone(const char* name="") const;        // Make a deep copy and provide its pointer
  virtual void SetOwner(Bool_t own=kTRUE);                  // Set ownership of all added objects
  using NcDevice::Data;
  virtual void Data(TString f="car",TString u="rad"); // Print the detector unit info within frame f and ang units u
  void SetDevCopy(Int_t j);               // (De)activate creation of private copies of the devices
  Int_t GetDevCopy() const;               // Provide DevCopy flag value      
  virtual void Reset(Int_t mode=0);       // Reset registered devices, hits and NcSignal attributes
  void AddDevice(NcDevice& d);            // Add a device to the detector unit
  void AddDevice(NcDevice* d) { if (d) AddDevice(*d); }
  void RemoveDevice(NcDevice* d);         // Remove the specified device from the detector unit
  Int_t GetNdevices(Bool_t follow);              // Provide the number of devices
  Int_t GetNdevices(TString classname,Bool_t follow,TObjArray* hits=0); // Provide the number of selected devices
  void ShowDevices(Int_t mode=1,Bool_t header=kTRUE);   // Provide on overview of the available devices
  void ShowDevices(TString classname,Int_t mode=1,Bool_t header=kTRUE); // Overview of selected devices
  NcDevice* GetDevice(Int_t i) const;     // Provide i-th device of the detector unit
  NcDevice* GetDevice(TString name,Bool_t follow);// Provide the device with name "name"
  NcDevice* GetIdDevice(Int_t id,Bool_t follow,TObjArray* devs=0); // Get device with identifier "id"
  NcDevice* GetIdDevice(Int_t id,TString classname,Bool_t follow); // Get device with identifier "id" of specified class
  TObjArray* GetDevices(TString classname,Bool_t follow,TObjArray* devices=0); // Provide references to the devices derived from the specified class
  NcDetectorUnit* GetDetectorUnit(TString name,Bool_t follow);// Provide the detector unit with name "name"
  NcDetectorUnit* GetIdDetectorUnit(Int_t id,Bool_t follow,TObjArray* devs=0); // Get detector unit with identifier "id"
  NcDetectorUnit* GetIdDetectorUnit(Int_t id,TString classname,Bool_t follow); // Get detector unit with identifier "id" of specified class
  Int_t GetNhitsDevices(TString classname,Bool_t follow,Bool_t inc); // Provide number of hits for the specified device class
  TObjArray* GetHitsDevices(TString classname,Bool_t follow,Bool_t inc,TObjArray* hits=0,TString name="none",Int_t mode=0,Int_t opt=0); // Provide refs to all hits of the specified device class and selections 
  NcSignal* GetIdHit(Int_t id,TString classname,Bool_t follow,Bool_t inc); // Provide hit with unique "id" for the specified device class
  void ShowHits(TString classname,Bool_t follow,Bool_t inc,Int_t mode=1,TString f="car",TString u="rad"); // Overview of all hits of selected devices
  using NcDevice::SortHits;
  TObjArray* SortHits(TString classname,Bool_t follow,Bool_t inc,TString name,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0); // Sort hits by named signal
  TObjArray* SortHits(TString classname,Bool_t follow,Bool_t inc,Int_t idx=1,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);  // Sort hits by indexed signal
  void DisplayHits(TString classname,Bool_t follow,Bool_t inc,TString name,Float_t scale=-1,Int_t dp=0,Int_t mode=1,Int_t mcol=4);
  void DisplayHits(TString classname,Bool_t follow,Bool_t inc,Int_t idx=1,Float_t scale=-1,Int_t dp=0,Int_t mode=1,Int_t mcol=4);
  TObjArray* SortDevices(TString classname,Bool_t follow,Bool_t inc,TString name,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0); // Sort devices by signal
  TObjArray* SortDevices(TString classname,Bool_t follow,Bool_t inc,Int_t idx=1,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);  // Sort devices by signal
  TObjArray* SortDevices(TObjArray* hits,TString name,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);       // Sort devices by signal
  TObjArray* SortDevices(TObjArray* hits,Int_t idx=1,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);        // Sort devices by signal

 protected:
  TObjArray* fDevices;                  // Array to hold the pointers to the various devices
  Int_t fDevCopy;                       // Flag to denote creation of private copies of the devices
  TObjArray* fDevs;                     //!< Temp. array to hold references to user selected devices
  TObjArray* fThits;                    //!< Temp. array to hold references to the registered NcDevice hits
  TObjArray* fOrdered;                  //!< Temp. array to hold references to various ordered objects
  TObject* fDisplay;                    //!< Temp. pointer to hold objects which serve event displays

  void LoadHits(TString classname,Bool_t follow,Bool_t inc,TObjArray* hits=0,NcDetectorUnit* unit=0); // Load references to the hits registered to the specified device class
  void LoadDevices(TString classname,Bool_t follow,TObjArray* hits=0,NcDetectorUnit* unit=0); // Load references to the devices derived from the specified device class
  void ShowDevicesTree(TString classname,Int_t mode=1,NcDetectorUnit* unit=0); // Overview of selected devices

 ClassDef(NcDetectorUnit,1) // Handling of a collection of generic devices
};
#endif
