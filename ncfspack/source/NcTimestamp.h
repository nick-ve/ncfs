#ifndef NcTimestamp_h
#define NcTimestamp_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

#include <iostream>
#include <fstream>

#include <cstdlib>
#include <math.h>

#include "TTimeStamp.h"
#include "TString.h"
#include "TTree.h"

class NcTimestamp : public TTimeStamp
{
 public:
  NcTimestamp();                               // Default constructor
  NcTimestamp(TTimeStamp& t);                  // Constructor
  virtual ~NcTimestamp();                      // Destructor
  NcTimestamp(const NcTimestamp& t);           // Copy constructor
  void Date(Int_t mode=3,Double_t offset=0);   // Print date/time info
  Double_t GetJD(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns) const; // Julian Date from input args.
  Double_t GetMJD(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns) const;// Modified JD from input args.
  Double_t GetTJD(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns) const;// Truncated JD from input args.
  Double_t GetJE(Double_t date,TString mode="jd") const; // Julian Epoch corresponding to specified Julian Date
  Double_t GetBE(Double_t date,TString mode="jd") const; // Besselian Epoch corresponding to specified Julian Date
  void Convert(Double_t date,Int_t& days,Int_t& secs,Int_t& ns) const;// Convert frac. day count into days, secs and ns 
  Double_t Convert(Int_t days,Int_t secs,Int_t ns) const;             // Convert days, secs and ns into frac. day count
  void Convert(Double_t h,Int_t& hh,Int_t& mm,Int_t& ss,Int_t& ns,Int_t& ps) const;// Convert frac. hour count
  Double_t Convert(Int_t hh,Int_t mm,Int_t ss,Int_t ns,Int_t ps) const; // Convert hh:mm:ss:ns:ps into frac. hour
  void Convert(Double_t h,Int_t& hh,Int_t& mm,Double_t& ss) const;// Convert frac. hour count
  Double_t Convert(Int_t hh,Int_t mm,Double_t ss) const;          // Convert hh:mm:ss.s into frac. hours
  void PrintTime(Double_t h,Int_t ndig=1) const;// Print time in hh:mm:ss.sss format
  void GetMJD(Int_t& mjd,Int_t& sec,Int_t& ns); // Provide corresponding Modified Julian Date and time
  Double_t GetMJD();                            // Provide corresponding Modified Julian Date in fractional days
  void GetTJD(Int_t& tjd,Int_t& sec,Int_t& ns); // Provide corresponding Truncated Julian Date and time
  Double_t GetTJD();                            // Provide corresponding Truncated Julian Date in fractional days
  void GetJD(Int_t& jd,Int_t& sec,Int_t& ns);   // Provide corresponding Julian Date and time
  Double_t GetJD();                             // Provide corresponding Julian Date in fractional days
  Double_t GetJE();                             // Provide corresponding Julian Epoch
  Double_t GetBE();                             // Provide corresponding Besselian Epoch
  Double_t GetJD(Double_t e,TString mode="J") const;  // Provide fractional Julian Date from Epoch
  Double_t GetMJD(Double_t e,TString mode="J") const; // Provide fractional Modified Julian Date from Epoch
  Double_t GetTJD(Double_t e,TString mode="J") const; // Provide fractional Truncated Julian Date from Epoch
  void SetMJD(Int_t mjd,Int_t sec,Int_t ns,Int_t ps=0,TString utc="A",Int_t leap=0,Double_t dut=0); // Set Modified Julian Date and time
  void SetMJD(Double_t mjd,TString utc="A",Int_t leap=0,Double_t dut=0);                            // Set Modified Julian Date and time
  void SetJD(Int_t jd,Int_t sec,Int_t ns,Int_t ps=0,TString utc="A",Int_t leap=0,Double_t dut=0);   // Set Julian Date and time
  void SetJD(Double_t jd,TString utc="A",Int_t leap=0,Double_t dut=0);                              // Set Julian Date and time
  void SetTJD(Int_t tjd,Int_t sec,Int_t ns,Int_t ps=0,TString utc="A",Int_t leap=0,Double_t dut=0); // Set Truncated Julian Date and time
  void SetTJD(Double_t tjd,TString utc="A",Int_t leap=0,Double_t dut=0);                            // Set Truncated Julian Date and time
  Int_t SetTAI(TString type,TString date,TString time,Int_t mode,TString utc,Int_t leap,Double_t dut=0); // Set specified TAI based date and time
  Int_t SetTAI(Int_t d,Int_t sec,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut=0,Bool_t tmjd=kFALSE);  // Set International Atomic Time (TAI) date and time
  Int_t SetTAI(Double_t tai,TString utc,Int_t leap,Double_t dut=0,Bool_t tmjd=kFALSE); // Set International Atomic Time (TAI) date and time
  Int_t SetGPS(Int_t w,Int_t sow,Int_t ns, Int_t ps,TString utc,Int_t leap,Double_t dut=0,Int_t icycle=0);           // Set the GPS date/time
  Int_t SetGPS(Int_t w,Int_t dow,Int_t sod,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut=0,Int_t icycle=0); // Set the GPS date/time
  Int_t SetUnixTime(Double_t sec,TString utc="A",Int_t leap=0,Double_t dut=0); // Set Unix date and time
  Int_t GetTAI(Int_t& d,Int_t& sec,Int_t& ns,Int_t& ps,Bool_t tmjd=kTRUE); // Provide corresponding TAI day count and time
  Double_t GetTAI(Bool_t tmjd=kTRUE);                                      // Provide the corresponding TAI day count in fractional days
  Int_t GetTAI(Int_t& hh,Int_t& mm,Int_t& ss,Int_t& ns,Int_t& ps,TString type="TAI"); // Provide corresponding TAI time
  Double_t GetUnixTime(); // Provide the corresponding Unix time (fractional) second count
  Int_t GetUTCparameters(Int_t& leap,Double_t& dut) const;           // Provide the UTC parameters
  Int_t GetUTCparameters(Int_t mjd,Int_t& leap,Double_t& dut) const; // Provide the UTC parameters for MJD from the IERS data
  Int_t SetUTCparameters(TString utc,Int_t leap,Double_t dut);       // Setting of the UTC parameters
  TTree* LoadUTCparameterFiles(TString leapfile,TString dutfile); // Load IERS data for automatic setting of Leap Seconds and dUT=UT-UTC
  TTree* GetIERSdatabase() const; // Provide the pointer to the internal IERS database TTree
  void SetNs(Int_t ns);                                 // Set the remaining fractional number of sec in nanoseconds
  Int_t GetNs() const;                                  // Provide remaining fractional number of sec in nanoseconds
  void SetPs(Int_t ps);                                 // Set the remaining fractional number of ns in picoseconds
  Int_t GetPs() const;                                  // Provide remaining fractional number of ns in picoseconds
  using TTimeStamp::Add;
  void Add(Int_t d,Int_t s,Int_t ns,Int_t ps=0);        // Add (or subtract) a certain time difference
  void Add(Double_t hours);                             // Add (or subtract) a certain time difference
  void AddSec(Double_t seconds);                        // Add (or subtract) a certain time difference
  Int_t GetDifference(NcTimestamp* t,Int_t& days,Int_t& sec,Int_t& ns,Int_t& ps,TString type="UT"); // Provide time difference
  Int_t GetDifference(NcTimestamp& t,Int_t& days,Int_t& sec,Int_t& ns,Int_t& ps,TString type="UT"); // Provide time difference
  Double_t GetDifference(NcTimestamp* t,TString u,Int_t mode=1,TString type="UT"); // Provide time diff. in specified units
  Double_t GetDifference(NcTimestamp& t,TString u,Int_t mode=1,TString type="UT"); // Provide time diff. in specified units
  void SetUT(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns=0,Int_t ps=0,TString utc="A",Int_t leap=0,Double_t dut=0); // Set specified UT
  void SetUT(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Double_t s,TString utc="A",Int_t leap=0,Double_t dut=0); // Set specified UT
  void SetUT(Int_t y,Int_t m,Int_t d,TString time,TString utc="A",Int_t leap=0,Double_t dut=0);                 // Set specified UT
  void SetUT(TString date,TString time,Int_t mode,TString utc="A",Int_t leap=0,Double_t dut=0);                 // Set specified UT
  void SetUT(Int_t y,Int_t d,Int_t s,Int_t ns=0,Int_t ps=0,TString utc="A",Int_t leap=0,Double_t dut=0);        // Set UT based on elapsed days, secs etc...
  void GetUT(Int_t& hh,Int_t& mm,Int_t& ss,Int_t& ns,Int_t& ps); // Provide corresponding UT
  Double_t GetUT(); // Provide corresponding UT in fractional hours
  void GetGMST(Int_t& hh,Int_t& mm,Int_t& ss,Int_t& ns,Int_t& ps); // Corresponding Greenwich Mean Sidereal Time (GMST)
  Double_t GetGMST(); // Provide corresponding Greenwich Mean Sidereal Time (GMST) in fractional hours
  Double_t GetGAST(); // Provide corresponding Greenwich Apparent Sidereal Time (GAST) in fractional hours
  Double_t GetLT(Double_t offset);  // Provide corresponding Local Time (LT) in fractional hours
  Double_t GetLMST(Double_t offset); // Provide corresponding Local Mean Sidereal Time (LMST) in fractional hours
  Double_t GetLAST(Double_t offset); // Provide corresponding Local Apparent Sidereal Time (LAST) in fractional hours
  void SetLT(Double_t dt,Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns=0,Int_t ps=0,TString utc="A",Int_t leap=0,Double_t dut=0); // Set data according to LT
  void SetLT(Double_t dt,Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Double_t s,TString utc="A",Int_t leap=0,Double_t dut=0); // Set data according to LT
  void SetLT(Double_t dt,Int_t y,Int_t m,Int_t d,TString time,TString utc="A",Int_t leap=0,Double_t dut=0);                 // Set data according to LT
  void SetLT(Double_t dt,TString date,TString time,Int_t mode,TString utc="A",Int_t leap=0,Double_t dut=0);                 // Set data according to LT
  void SetLT(Double_t dt,Int_t y,Int_t d,Int_t s,Int_t ns=0,Int_t ps=0,TString utc="A",Int_t leap=0,Double_t dut=0); // Set data according to LT based on elapsed days, secs etc...
  Double_t Almanac(Double_t* dpsi=0,Double_t* deps=0,Double_t* eps=0,Double_t* dl=0,TString name="",Double_t* el=0,Double_t* eb=0,Double_t* er=0,Double_t* value=0,Int_t j=0); // Provide astronomical observables
  void SetEpoch(Double_t e,TString mode,TString utc="A",Int_t leap=0,Double_t dut=0); // Set time parameters according to the specified epoch
  Double_t GetEpoch(TString mode); // Provide the requested epoch

 protected:
  Int_t fMJD;      // Modified Julian Date
  Int_t fJsec;     // Number of seconds elapsed within the MJD
  Int_t fJns;      // Remaining fractional number of seconds (in nanoseconds) elapsed within the MJD
  Int_t fJps;      // Remaining fractional number of nanoseconds (in picoseconds) elapsed within the MJD
  Int_t fUtc;      // Flag to denote that the UTC related info below has been provided (-1=auto 0=no 1=yes)
  Int_t fLeap;     // The cumulated number of leap seconds at the moment of the timestamp
  Double_t fDut;   // The value of UT-UTC in seconds at the moment of the timestamp
  Int_t fTmjd;     // Number of elapsed TAI days equivalent to MJD counting
  Int_t fTsec;     // Number of seconds elapsed within the TAI day
  Int_t fTns;      // Remaining fractional number of seconds (in nanoseconds) elapsed within the TAI day
  Int_t fTps;      // Remaining fractional number of nanoseconds (in picoseconds) elapsed within the TAI day
  TTree* fUTCdata; // Internal tree to contain the daily leap second and UT-UTC values

 private:
  void FillJulian(); // Calculation and setting of the corresponding Julian parameters  
  void FillTAI();    // Calculation and setting of the corresponding TAI day etc. count
  Int_t fCalcs;      // The TTimeStamp seconds counter value at Julian parameter calculation
  Int_t fCalcns;     // The TTimeStamp nanoseconds counter value at Julian parameter calculation

 ClassDef(NcTimestamp,8) // Handling of timestamps for (astro)particle physics research.
};
#endif
