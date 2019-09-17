#ifndef NcEvent_h
#define NcEvent_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcEvent.h 132 2016-09-01 16:50:25Z nickve $

#include <math.h>
 
#include "TObject.h"
#include "TObjArray.h"
#include "TDatime.h"
#include "TMath.h"
 
#include "NcVertex.h"
#include "NcDevice.h"
#include "NcTimestamp.h"
#include "NcSample.h"
 
class NcEvent : public NcVertex,public NcTimestamp
{
 public:
  NcEvent();                              // Default constructor
  NcEvent(Int_t n);                       // Create an event to hold initially n tracks
  virtual ~NcEvent();                     // Default destructor
  NcEvent(const NcEvent& evt);            // Copy constructor
  virtual TObject* Clone(const char* name="") const; // Make a deep copy and provide its pointer
  virtual void SetOwner(Bool_t own=kTRUE);// Set ownership of all added objects
  void SetDayTime(TTimeStamp& stamp);     // (Obsolete, see docs) Set date/time exactly as specified (1ns accuracy)
  void SetDayTime(TDatime& stamp);        // (Obsolete, see docs) Set date/time interpreted as local time (1s accuracy)
  void SetRunNumber(Int_t run);           // Set the run number
  void SetEventNumber(Int_t evt);         // Set the event number
  void SetProjectile(Int_t a,Int_t z,Double_t pnuc,Int_t id=0); // Set projectile A, Z, momentum per nucleon and id
  void SetProjectile(Int_t a,Int_t z,Nc3Vector& p,Int_t id=0);  // Set projectile A, Z, 3-momentum per nucleon and id
  Int_t GetProjectileA() const;           // Provide A value of the projectile
  Int_t GetProjectileZ() const;           // Provide Z value of the projectile
  Double_t GetProjectilePnuc() const;     // Provide the projectile momentum value per nucleon
  Int_t GetProjectileId() const;          // Provide the user defined particle ID of the projectile
  void SetTarget(Int_t a,Int_t z,Double_t pnuc,Int_t id=0); // Set target A, Z, momentum per nucleon and id
  void SetTarget(Int_t a,Int_t z,Nc3Vector& p,Int_t id=0);  // Set target A, Z, 3-momentum per nucleon and id
  Int_t GetTargetA() const;               // Provide A value of the target
  Int_t GetTargetZ() const;               // Provide Z value of the target
  Double_t GetTargetPnuc() const;         // Provide the target momentum value per nucleon
  Int_t GetTargetId() const;              // Provide the user defined particle ID of the target
  virtual void Reset();                   // Reset all values
  TTimeStamp GetDayTime() const;          // (Obsolete, see docs) Provide the date and time stamp
  Int_t GetRunNumber() const;             // Provide the run number
  Int_t GetEventNumber() const;           // Provide the event number
  virtual void HeaderData();              // Print the event header information
  using NcVertex::Data;
  virtual void Data(TString f="car",TString u="rad"); // Print the event info within frame f and ang units u
  void SetDevCopy(Int_t j);               // (De)activate creation of private copies of the devices
  Int_t GetDevCopy() const;               // Provide DevCopy flag value      
  void AddDevice(NcDevice& d);            // Add a device to the event
  void AddDevice(NcDevice* d) { if (d) AddDevice(*d); }
  void RemoveDevice(NcDevice* d);         // Remove the specified device from the event
  Int_t GetNdevices() const;              // Provide the number of devices
  Int_t GetNdevices(const char* classname,TObjArray* hits=0) const; // Provide the number of selected devices
  void ShowDevices(Int_t mode=1) const;   // Provide on overview of the available devices
  void ShowDevices(const char* classname,Int_t mode=1) const; // Overview of selected devices
  TObjArray* GetDevices(const char* classname,TObjArray* devices=0); // Provide references to the devices derived from the specified class
  NcDevice* GetDevice(Int_t i) const;     // Provide i-th device of the event
  NcDevice* GetDevice(TString name) const;// Provide the device with name "name"
  NcDevice* GetIdDevice(Int_t id, TObjArray* devs=0) const; // Get device with identifier "id"
  NcDevice* GetIdDevice(Int_t id, const char* classname) const; // Get device with identifier "id" of specified class
  Int_t GetNhits(const char* classname);  // Provide number of hits for the specified device class
  TObjArray* GetHits(const char* classname,TObjArray* hits=0,TString name="none",Int_t mode=0,Int_t opt=0); // Provide refs to all hits of the specified device class and selections 
  NcSignal* GetIdHit(Int_t id,const char* classname); // Provide hit with unique "id" for the specified device class
  TObjArray* SortHits(const char* classname,TString name,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0); // Sort hits by named signal
  TObjArray* SortHits(const char* classname,Int_t idx=1,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);  // Sort hits by indexed signal
  Nc3Vector GetHitPath(TObjArray* hits,Int_t pos=0) const; // Provide the average direction of specified hit pattern
  NcPosition GetCOG(TObjArray* hits,Int_t pos=0,TString slotname="none",Int_t mode=0) const; // Provide the Center Of Gravity of the specified hits
  Double_t GetCVAL(TObjArray* hits,TString obsname,TString weightname="none",Int_t mode=0,Int_t type=1) const; // Provide the Central Value of "obsname" of the specified hits
  void GetExtremes(const char* classname,Float_t& vmin,Float_t& vmax,Int_t idx=1,Int_t mode=1,Int_t deadcheck=1); // min and max signal
  void GetExtremes(const char* classname,Float_t& vmin,Float_t& vmax,TString name,Int_t mode=1,Int_t deadcheck=1);// min and max signal
  void DisplayHits(const char* classname,TString name,Float_t scale=-1,Int_t dp=0,Int_t mode=1,Int_t mcol=4);
  void DisplayHits(const char* classname,Int_t idx=1,Float_t scale=-1,Int_t dp=0,Int_t mode=1,Int_t mcol=4);
  void ShowHits(const char* classname,Int_t mode=1,TString f="car",TString u="rad"); // Overview of all hits of selected devices
  TObjArray* SortDevices(const char* classname,TString name,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0); // Sort devices by signal
  TObjArray* SortDevices(const char* classname,Int_t idx=1,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);  // Sort devices by signal
  TObjArray* SortDevices(TObjArray* hits,TString name,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);       // Sort devices by signal
  TObjArray* SortDevices(TObjArray* hits,Int_t idx=1,Int_t mode=-1,Int_t mcal=1,Int_t deadcheck=1,TObjArray* ordered=0);        // Sort devices by signal
  void SetWeight(Double_t weight); // Set the event weight      
  Double_t GetWeight() const;      // Provide the event weight      

 protected:
  Int_t fRun;                           // The run number
  Int_t fEvent;                         // The event number
  Double_t fWeight;                     // The event weight
  TObjArray* fDevices;                  // Array to hold the pointers to the various devices
  Int_t fDevCopy;                       // Flag to denote creation of private copies of the devices
  void LoadHits(const char* classname,TObjArray* hits=0); // Load references to the hits registered to the specified device class
  TObjArray* fHits;                     //! Temp. array to hold references to the registered NcDevice hits
  TObjArray* fOrdered;                  //! Temp. array to hold references to various ordered objects
  TObject* fDisplay;                    //! Temp. pointer to hold objects which serve event displays
  TObjArray* fDevs;                     //! Temp. array to hold references to user selected devices

 ClassDef(NcEvent,10) // Creation and investigation of an NCFS generic event structure.
};
#endif
