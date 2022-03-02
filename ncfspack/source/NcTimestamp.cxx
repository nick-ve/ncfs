/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.                     *
 *                                                                             *
 * Authors: The Netherlands Center for Fundamental Studies (NCFS).             *
 *          The Inter-university Institute for High Energies (IIHE).           *                 
 *                    Website : http://www.iihe.ac.be                          *
 *            Contact : Nick van Eijndhoven (nickve.nl@gmail.com)              *
 *                                                                             *
 * Contributors are mentioned in the code where appropriate.                   *
 *                                                                             * 
 * No part of this software may be used, copied, modified or distributed       *
 * by any means nor transmitted or translated into machine language for        *
 * commercial purposes without written permission by the IIHE representative.  *
 * Permission to use the software strictly for non-commercial purposes         *
 * is hereby granted without fee, provided that the above copyright notice     *
 * appears in all copies and that both the copyright notice and this           *
 * permission notice appear in the supporting documentation.                   *
 * This software is provided "as is" without express or implied warranty.      *
 * The authors make no claims that this software is free of error, or is       *
 * consistent with any particular standard of merchantability, or that it      *
 * will meet your requirements for any particular application, other than      *
 * indicated in the corresponding documentation.                               *
 * This software should not be relied on for solving a problem whose           *
 * incorrect solution could result in injury to a person or loss of property.  *
 * If you do use this software in such a manner, it is at your own risk.       *
 * The authors disclaim all liability for direct or consequential damage       *
 * resulting from your use of this software.                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///////////////////////////////////////////////////////////////////////////
// Class NcTimestamp
// Handling of timestamps for (astro)particle physics reserach.
// This facility supports picosecond timing precision.
//
// This class is derived from TTimeStamp and provides additional
// facilities (e.g. Julian date) which are commonly used in the
// field of (astro)particle physics.
//
// The basis of the used timing is the "day".
//
// 1 day=24 hours  1 hour=60 minutes  1 minute=60 seconds.
//
// So, 1 day consists of 86400 seconds.
//
// The absolute duration of a day is defined by the time scale that is used.
// The two main time scales for time recording are Universal Time (UT)
// and International Atomic Time (TAI), as outlined below.
// Here it should be noted that UT may represent Coordinated Universal Time (UTC)
// as well as UT1, which is a more exact time keeping for astronomical observations.
// The UT1 timing is obtained by daily observations of the slight variations in the
// meridian transitions of (extra galactic) cosmic objects, due to irregularities
// in the Earth rotation. This results in daily updates w.r.t. UTC expressed as dUT=UT1-UTC.
// In case the value of dUT=UT1-UTC is known (see below), the timing of UT1 will be used
// as the main reference time instead of UTC, in order to provide the best accuracy
// for linking observations with astrophysical phenomena.
// The UT time scale is always provided and the TAI (and its derived) time scale(s)
// may be activated by the various Set() functions, as outlined in the corresponding docs.
// For precise time keeping it is important to work within 1 single time scale and indicate
// which time scale is being used.
// Time information for the various time scales may be obtained via the Date() and the
// various Get() memberfunctions.
//
// Notes :
// -------
// 1) Both the UT and TAI based time scales provide picosecond precision, so from that
//    respect there is no preference for one or the other.
//    However, when timing is received from an external clock (e.g. GPS), it might be
//    convenient to choose a specific time scale when setting dates and times depending
//    on the format in which the clock information is broadcast.
// 2) As explained below, the UT and TAI clocks do not "tick" at the same pace.
//    This implies that when two timestamps are compared (e.g. via the GetDifference()
//    member function) both timestamps should be using the same time scale.
// 3) For most of the time broadcasting the Coordinated Universal Time (UTC) is used.
//    This UTC time runs at the pace of TAI, but is kept close (within 0.9 sec) to UT1
//    by the introduction of Leap Seconds when abs(UT1-UTC) exceeds 0.9 sec.
//    This time synchronisation is coordinated by the International Earth Rotation and
//    Reference Systems Service (IERS) via a daily monitoring of the Earth Orientation
//    Parameters (EOP). 
//    Depending on dUT=UT1-UTC, these Leap Seconds can be positive or negative.
//    The introduction of Leap Seconds into UTC started at 01-jan-1972 00:00:00 UT.
//
//    An overview of the history of introduced Leap seconds (TAI-UTC) is online available at :
//                 https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
//
//    The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
//    The accuracy of dUT=UT1-UTC is about 10 microseconds, so in case of accurate astronomical
//    timing the user is advised to specify dUT in the various Set() facilities or use SetUT().
//    An automatic setting of dUT is provided based on the IERS data files. 
//    Please refer to the member function LoadUTCparameterFiles() for further details.
//
// 4) In some cases Unix Time (also called POSIX Time or UNIX Epoch Time) is used.
//    Unix Time is closely releated to UTC and represents the (fractional) elapsed second count
//    since the start of the Unix Epoch, in which every Unix Time day contains exactly 86400 seconds.
//    This implies that for Unix Time the UTC leap seconds should be ignored, as explained below.
//    The UNIX Time EPOCH starts at 01-jan-1970 00:00:00 UTC which corresponds to JD=2440587.5
//    (i.e. the start of MJD=40587, as explained in the discussion on Julian Date below).
//    Synchronization with UTC is obtained by continuing the second count when a UTC leap second
//    occurs and then the Unix Time count jumps up or down 1 second at the beginning of the new
//    day after the UTC leap second has expired.
//    In case of a negative UTC leap second (which has not occurred so far) this would result
//    in a jump forward of 1 second in Unix Time, introducing a "gap" of 1 second in the timing
//    at the start of the new day.
//    In case of a positive UTC leap second this results in a jump back of 1 second in Unix Time
//    at the start of a new day.
//    In this case there exist 2 ambiguous Unix Times over a 1 second time interval,
//    namely at the beginning of the introduction of the UTC leap second and at the moment that
//    the UTC leap second expires, i.e. the start of the new day.
//    This NcTimestamp facility provides setting and retrieval of Unix Time, but for accurate
//    timing the user is advised to use one of the time scales mentioned below. 
// 
// Supported time scales :
// ----------------------
// UT  : Universal Time.
//       1 day is the time span between two successive solar meridian transitions.
//       This implies that UT is based on the actual rotation of the Earth and as such suited
//       for (synchronization of) astronomical observations at various locations on Earth.
//       The reference time is defined by transitions over the meridian at Greenwich.
// ST  : Siderial Time (see also the details below).
//       1 day is the time span between two successive stellar meridian transitions.
//       This implies that ST is based on the actual rotation of the Earth and its rotation
//       around the Sun. Siderial Time is very well suited for astronomical observations.
//       The reference time is defined by transitions over the meridian at Greenwich.
//       Because of the stability of the rotation of the Earth around the Sun, the Siderial Time
//       is derived from the Universal Time and as such not treated as a separate time scale.
// TAI : International Atomic Time (Temps Atomique International).
//       1 day is the time span of 86400 standard atomic (SI) seconds at average sea level.
//       The standard atomic (SI) second is derived from a set of Cs atomic clocks.
//       This implies that TAI is not related to the actual rotation of the Earth and as such
//       will not run at the same pace as UT.
//       The start epoch of TAI is 01-jan-1958 00:00:00 UT, at which time UT1-UTC was about 0.
// GPS : Global Positioning System time.
//       This satellite based timing system is based on TAI and broadcast via a satellite network.
//       The start epoch of GPS is 06-jan-1980 00:00:00 UTC, at which the number of Leap Seconds was 19.
//       This implies that at that time TAI=UTC+19 sec. and consequently we always have TAI=GPS+19 sec.
//       GPS is broadcast in two formats : (w,sow) and (w,dow,sod) with
//       w   = week number after the start epoch.   1 week=7 days or 1 week=604800 seconds.
//       sow = (fractional) second count in the current week
//       dow = day number in the current week (sunday=0, monday=1 etc.)
//       sod = (fractional) second count in the current day
//       Early implementations used to reset the week count after a cycle of 1024 weeks and
//       provided the corresponding cycle count. Please refer to the SetGPS() memberfunctions
//       for further details.
// TT  : Terrestrial Time.
//       This timing system is based on TAI and provides the date/time at average sea level.
//       It has been introduced for observations from the surface of the Earth, to be consistent
//       with General Relativity. Since planetary orbits are very stable in time and not related
//       to the rotation of the Earth, TT is mainly used for observations of the Solar system.
//       TT was synchronized with TAI at 01-jan-1977 00:00:00 TAI, to indicate 00:00:32.184 in
//       order to provide a continuation of its (now obsolete) predecessor Ephemeris Time (ET).
//       This implies that always TT=TAI+32.184 sec.
//
// All Gregorian dates/times (e.g. 12-aug-1982 13:20:12) are treated on basis of
// the time scales mentioned above. So, there is no need for leap second treatment
// like this is the case for Coordinated Universal Time (UTC).  
//
// In order to enable a precise measurement of elapsed time over (very) long periods,
// a continuous day counting system has been introduced, called the Julian Date.
// The Julian Date (JD) indicates the number of days since noon (12:00:00) on
// 01 jan -4712 (i.e. noon 01 jan 4713 BC), being day 0 of the Julian calendar.
// It is custom to couple the Julian Date to UT so that it serves astronomical observations.
// However, this NcTimestamp facility allows to use a continuous day counting system
// with all the supported time scales mentioned above.
//
// The Modified Julian Date (MJD) indicates the number of days since midnight (00:00:00)
// on 17-nov-1858, which corresponds to 2400000.5 days after day 0 of the
// Julian calendar.
//
// The Truncated Julian Date (TJD) corresponds to 2440000.5 days after day 0
// of the Julian calendar and consequently TJD=MJD-40000.
// This TJD date indication was used by the Vela and CGRO satellite missions in
// view of Gamma Ray Burst investigations.
//
// The Julian Epoch (JE) indicates the fractional elapsed Julian year count
// since the start of the Gregorian year count.
// A Julian year is defined to be 365.25 days and starts at 01-jan 12:00:00.
// As such, the integer part of JE corresponds to the usual Gregorian year count,
// apart from 01-jan before 12:00:00.
// So, 01-jan-1965 12:00:00 UT corresponds to JE=1965.0
//
// The Besselian Epoch (BE) indicates the fractional elapsed Besselian year count
// since the start of the Gregorian year count.
// A Besselian (or tropical) year is defined to be 365.242198781 days.
// The date 31-dec-1949 22:09:46.862 UT corresponds to BE=1950.0
//
// The Besselian and Julian epochs are used in astronomical catalogs
// to denote values of time varying observables like e.g. right ascension.
//
// Because of the fact that the Julian date indicators are all w.r.t. UT or TAI derived
// time scales, they provide an absolute time scale irrespective of timezone or daylight
// saving time (DST).
//
// In view of astronomical observations and positioning it is convenient
// to have also a UT equivalent related to stellar meridian transitions.
// This is achieved by the Greenwich Sidereal Time (GST).
// The GST is defined as the right ascension of the objects passing
// the meridian over Greenwich.
// Due to the rotation of the Earth around the Sun, a sidereal day
// lasts 86164.09 seconds (23h 56m 04.09s) compared to the mean solar
// day of 86400 seconds (24h).
// Furthermore, precession of the earth's spin axis results in the fact
// that the zero point of right ascension (vernal equinox) gradually 
// moves along the celestial equator.
// In addition, tidal friction and ocean and atmospheric effects will
// induce seasonal variations in the earth's spin rate and polar motion
// of the earth's spin axis.
// Taking the above effects into account leads to what is called
// the Greenwich Mean Sidereal Time (GMST).
// In case also the nutation of the earth's spin axis is taken into
// account we speak of the Greenwich Apparent Sidereal Time (GAST).
//
// This NcTimestamp facility allows for picosecond precision, in view
// of time of flight analyses for particle physics experiments.
// For normal date/time indication the standard nanosecond precision
// will in general be sufficient.
// Picosecond precision can be obtained by invokation of GetPs() or GetDifference().
// Note that when the fractional JD, MJD and TJD counts are used instead
// of the integer (days,sec,ns) specification, the nanosecond precision
// may be lost due to computer accuracy w.r.t. floating point operations.
//
// The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00
// which corresponds to JD=2440587.5 or the start of MJD=40587 or TJD=587.
// Using the corresponding MJD of this EPOCH allows construction of
// the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input (M/T)JD and time.
// Obviously this TTimeStamp implementation would prevent usage of values
// smaller than JD=2440587.5 or MJD=40587 or TJD=587.
// Furthermore, due to a limitation on the "seconds since the EPOCH start" count
// in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08.
// However, this NcTimestamp facility provides support for the full range
// of (M/T)JD values, but the setting of the corresponding TTimeStamp parameters
// is restricted to the values allowed by the TTimeStamp implementation.
// For these earlier/later (M/T)JD values, the standard TTimeStamp parameters will
// be set corresponding to the start of the TTimeStamp EPOCH.
// This implies that for these earlier/later (M/T)JD values the TTimeStamp parameters
// do not match the Julian parameters of NcTimestamp.
// As such the standard TTimeStamp parameters do not appear on the print output
// when invoking the Date() memberfunction for these earlier/later (M/T)JD values.  
//
// Examples :
// ==========
//
// Note : All TTimeStamp functionality is available as well.
//
// NcTimestamp t;
//
// t.Date();
//
// // Set a specific Date/Time in Universal Time (UT1)
// // without recording UTC nor the corresponding International Atomic Time (TAI)
// t.SetUT("22-08-2016","14:03:29.7358",0,"U");
//
// // Set a specific Date/Time in Universal Time (UTC)
// // without recording UT1 nor the corresponding International Atomic Time (TAI)
// t.SetUT("22-08-2016","14:03:29.7358",0,"N");
//
// // Set a specific Date/Time in Universal Time (UTC)
// // and also record UT1 and the corresponding International Atomic Time (TAI)
// // using manual "leap" and "dut" settings
// Int_t leap=34;
// Double_t dut=-0.14;
// t.SetUT("17-05-2011","05:43:18.31468",0,"M",leap,dut);
//
// // Set a specific Date/Time in Universal Time (UTC)
// // and also record UT1 and the corresponding International Atomic Time (TAI)
// // using automatic "leap" and "dut" settings from the IERS data files
// t.LoadUTCparameterFiles("leap.txt","dut.txt");
// t.SetUT("17-05-2011","05:43:18.31468",0,"A");
//
// // Set a specific Date/Time in Global Positioning System time (GPS)
// // and also record the corresponding Universal Times (UTC and UT1) and TAI
// // using automatic "leap" and "dut" settings from the IERS data files
// t.LoadUTCparameterFiles("leap.txt","dut.txt");
// t.SetTAI("GPS","17-05-2011","05:43:18.31468",0,"A",0,0);
// 
// // Retrieve Julian Date to ns precision
// Int_t jd,jsec,jns;
// t.GetJD(jd,jsec,jns);
// // Get the remaining ps precision
// Int_t ps=GetPs();
//
// // Retrieve fractional Truncated Julian Date
// Double_t tjd=t.GetTJD();
//
// // Retrieve fractional Julian Epoch
// Double_t je=t.GetJE();
//
// // Set to a specific Modified Julian Date representing UTC
// // without recording the corresponding International Atomic Time (TAI)
// Int_t mjd=50537;
// Int_t mjsec=1528;
// Int_t mjns=185643;
// Int_t mjps=35;
// t.SetMJD(mjd,mjsec,mjns,mjps,"N");
//
// t.Date();
//
// // Set to a specific Modified Julian Date representing UTC
// // and also record the corresponding International Atomic Time (TAI)
// // using automatic "leap" and "dut" settings from the IERS data files
// Int_t mjd=58457;
// Int_t mjsec=1528;
// Int_t mjns=185643;
// Int_t mjps=35;
// t.LoadUTCparameterFiles("leap.txt","dut.txt");
// t.SetMJD(mjd,mjsec,mjns,mjps,"A");
//
// t.Date();
//
// // Set to a specific Modified Julian Date representing UT1
// // and also record the corresponding International Atomic Time (TAI)
// // using automatic "leap" and "dut" settings from the IERS data files
// Int_t mjd=58457;
// Int_t mjsec=1528;
// Int_t mjns=185643;
// Int_t mjps=35;
// t.LoadUTCparameterFiles("leap.txt","dut.txt");
// t.SetMJD(mjd,mjsec,mjns,mjps,"U");
//
// t.Date();
//
// // Time intervals for e.g. Trigger or Time Of Flight analysis
// NcEvent evt;
// NcTrack* tx=evt.GetTrack(5);
// NcTimestamp* timex=tx->GetTimestamp();
// Double_t dt=evt.GetDifference(timex,"ps");
// NcTimestamp trig((NcTimestamp)evt);
// trig.Add(0,0,2,173);
// NcSignal* sx=evt.GetHit(23);
// NcTimestamp* timex=sx->GetTimestamp();
// Double_t dt=trig.GetDifference(timex,"ps");
// Int_t d,s,ns,ps;
// trig.GetDifference(timex,d,s,ns,ps);
//
// // Some practical conversion facilities
// // Note : They don't influence the actual date/time settings
// //        and as such can also be invoked as NcTimestamp::Convert(...) etc...
// Int_t y=1921;
// Int_t m=7;
// Int_t d=21;
// Int_t hh=15;
// Int_t mm=23;
// Int_t ss=47;
// Int_t ns=811743;
// Double_t jdate=t.GetJD(y,m,d,hh,mm,ss,ns);
//
// Int_t days,secs,nsecs;
// Double_t date=421.1949327;
// t.Convert(date,days,secs,nsecs);
//
// days=875;
// secs=23;
// nsecs=9118483;
// date=t.Convert(days,secs,nsecs);
//
// Double_t mjdate=40563.823744;
// Double_t epoch=t.GetJE(mjdate,"mjd");
//
//--- Author: Nick van Eijndhoven 28-jan-2005 Utrecht University
//- Modified: Nick van Eijndhoven, IIHE-VUB Brussel, March 2, 2022  13:30Z
///////////////////////////////////////////////////////////////////////////

#include "NcTimestamp.h"
#include "Riostream.h"

ClassImp(NcTimestamp) // Class implementation to enable ROOT I/O
 
NcTimestamp::NcTimestamp() : TTimeStamp()
{
// Default constructor
// Creation of an NcTimestamp object and initialisation of parameters.
// All attributes are initialised to the current UTC date/time as specified
// in the docs of TTimeStamp.

 FillJulian();
 fJps=0;
 fUtc=0;
 fLeap=0;
 fDut=0;
 fTmjd=0;
 fTsec=0;
 fTns=0;
 fTps=0;
 fUTCdata=0;
}
///////////////////////////////////////////////////////////////////////////
NcTimestamp::NcTimestamp(TTimeStamp& t) : TTimeStamp(t)
{
// Creation of an NcTimestamp object and initialisation of parameters.
// All attributes are initialised to the values of the input TTimeStamp.

 FillJulian();
 fJps=0;
 fUtc=0;
 fLeap=0;
 fDut=0;
 fTmjd=0;
 fTsec=0;
 fTns=0;
 fTps=0;
 fUTCdata=0;
}
///////////////////////////////////////////////////////////////////////////
NcTimestamp::~NcTimestamp()
{
// Destructor to delete dynamically allocated memory.
 if (fUTCdata)
 {
  delete fUTCdata;
  fUTCdata=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcTimestamp::NcTimestamp(const NcTimestamp& t) : TTimeStamp(t)
{
// Copy constructor

 fMJD=t.fMJD;
 fJsec=t.fJsec;
 fJns=t.fJns;
 fJps=t.fJps;
 fCalcs=t.fCalcs;
 fCalcns=t.fCalcns;
 fUtc=t.fUtc;
 fLeap=t.fLeap;
 fDut=t.fDut;
 fTmjd=t.fTmjd;
 fTsec=t.fTsec;
 fTns=t.fTns;
 fTps=t.fTps;
 fUTCdata=0;
 TTree* tx=t.fUTCdata;
 if (tx) fUTCdata=(TTree*)tx->Clone();
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::Date(Int_t mode,Double_t offset)
{
// Print date/time info.
//
// mode = 1 ==> Only the date/time info for UT and GMST and the Equation of Time (EoT) and Equinoxes (EoX) is listed
//        2 ==> Only the Julian parameter info (incl. TAI MJD, if available) is listed
//        3 ==> Both UT, GMST, EoT, EoX and Julian parameter info (incl. TAI MJD, if available) is listed
//        4 ==> Only the UTC and TAI related date/time is listed
//       -1 ==> Only the date/time info for UT, GMST, EoT, EoX, GAT and GAST is listed
//       -3 ==> Both UT, GMST, EoT, EoX, GAT, GAST and Julian parameter info (incl. TAI MJD, if available) is listed
//
// offset : Local time offset from UT (and also GMST) in fractional hours.
//
// When an offset value is specified, the corresponding local times
// LMT and LMST (or LAT and LAST) are printed as well.
//
// The default values are mode=3 and offset=0.
//
// Note : In case the (M/T)JD falls outside the TTimeStamp range,
//        the yy-mm-dd info will be omitted.

 Int_t mjd,mjsec,mjns;
 GetMJD(mjd,mjsec,mjns);

 TString month[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
 TString day[7]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
 UInt_t y,m,d,wd;
 Int_t hh,mm,ss,ns,ps;
 Double_t gat,gast;
 Bool_t date=kFALSE;
 
 // The UT date and time
 if (abs(mode)==1 || abs(mode)==3)
 {
  if (mjd>=40587 && (mjd<65442 || (mjd==65442 && mjsec<8047)))
  {
   GetDate(kTRUE,0,&y,&m,&d);
   wd=GetDayOfWeek(kTRUE,0);
   cout << " " << day[wd-1].Data() << ", " << setfill('0') << setw(2) << d << " "
        << setfill(' ') << month[m-1].Data() << " " << y << " ";
   date=kTRUE;
  }
  else
  {
   cout << " Time ";
   date=kFALSE;
  }
  GetUT(hh,mm,ss,ns,ps);
  cout << setfill('0') << setw(2) << hh << ":"
       << setw(2) << mm << ":" << setw(2) << ss << "."
       << setw(9) << ns << setw(3) << ps;
  if (!fUtc)
  {
   cout << " (UTC) ";
  }
  else
  {
   cout << " (UT1) ";
  }

  // The GMST time information
  GetGMST(hh,mm,ss,ns,ps);
  cout << setfill('0') << setw(2) << hh << ":"
       << setw(2) << mm << ":" << setw(2) << ss << "."
       << setw(9) << ns << setw(3) << ps << " (GMST)" << endl;

  // Equation of Time and Equation of Equinoxes
  Double_t eot;
  Double_t eox=Almanac(0,0,0,0,"Sun",0,0,0,&eot,10);
  // Convert to fractional hours
  eox/=3600.;
  eot/=3600.;
  if (date)
  {
   cout << "                 ";
  }
  else
  {
   cout << "     ";
  }
  if (eot>=0) cout << " ";
  PrintTime(eot,3); cout << " (LAT-LMT)";
  cout << "     ";
  if (eox>=0) cout << " ";
  PrintTime(eox,3); cout << " (LAST-LMST)"; cout << endl;

  // Greenwich apparent time information
  if (mode<0)
  {
   gat=GetLT(eot); // Obtain GAT via the Equation of Time as offset
   gast=GetGAST();
   if (date)
   {
    cout << "                  ";
   }
   else
   {
    cout << "      ";
   }
   PrintTime(gat,12); cout << " (GAT) ";
   PrintTime(gast,12); cout << " (GAST)" << endl;
  }

  // Local time information
  if (offset)
  {
   // Determine the new date by including the offset
   NcTimestamp t2(*this);
   t2.Add(offset);
   Int_t mjd2,mjsec2,mjns2;
   t2.GetMJD(mjd2,mjsec2,mjns2);
   if (mjd2>=40587 && (mjd2<65442 || (mjd2==65442 && mjsec2<8047)))
   {
    t2.GetDate(kTRUE,0,&y,&m,&d);
    wd=t2.GetDayOfWeek(kTRUE,0);
    cout << " " << day[wd-1].Data() << ", " << setfill('0') << setw(2) << d << " "
         << setfill(' ') << month[m-1].Data() << " " << y << " ";
    date=kTRUE;
   }
   else
   {
    cout << " Time ";
    date=kFALSE;
   }
   // Determine the local time by including the offset w.r.t. the original timestamp
   Double_t hlt=0;
   Double_t hlst=0;
   if (mode>0)
   {
    hlt=GetLT(offset);
    hlst=GetLMST(offset);
    PrintTime(hlt,12);  cout << " (LMT) ";
    PrintTime(hlst,12); cout << " (LMST)" << endl;
   }
   else
   {
    hlt=GetLT(offset+eot); // Obtain LAT via the Equation of Time as extra offset
    hlst=GetLAST(offset);
    PrintTime(hlt,12);  cout << " (LAT) ";
    PrintTime(hlst,12); cout << " (LAST)" << endl;
   }
  }
 }

 // Julian parameter information
 if (abs(mode)==2 || abs(mode)==3)
 {
  Int_t jd,jsec,jns;
  GetJD(jd,jsec,jns);
  Int_t tjd,tjsec,tjns;
  GetTJD(tjd,tjsec,tjns);
  cout << " Julian Epoch : " << setprecision(25) << GetJE()
       << " Besselian Epoch : " << setprecision(25) << GetBE() << endl;
  cout << " JD : " << jd << " sec : " << jsec << " ns : " << jns << " ps : " << fJps
       << " Fractional : " << setprecision(25) << GetJD() << endl;
  cout << " MJD : " << mjd << "  sec : " << mjsec << " ns : " << mjns << " ps : " << fJps
       << " Fractional : " << setprecision(25) << GetMJD() << endl;
  cout << " TJD : " << tjd << "  sec : " << tjsec << " ns : " << tjns << " ps : " << fJps
       << " Fractional : " << setprecision(25) << GetTJD() << endl;
  if (fUtc && fUtc!=-3)
  {
   cout << " TAI : " << fTmjd << "  sec : " << fTsec << " ns : " << fTns << " ps : " << fTps
        << " Fractional : " << setprecision(25) << GetTAI() << endl;
  }
 }

 // TAI related information
 if (mode==4 && fUtc && fUtc!=-3)
 {
  cout << " Cumulated (TAI-UTC) leap seconds: " << setfill(' ') << setw(3) << fLeap 
       << " UT1-UTC : " << setprecision(6) << fDut << " sec.";
  if (fUtc<0) cout << " (IERS database)" << endl;
  if (fUtc>0) cout << " (Manual setting)" << endl;
 
  // A dummy timestamp is used to obtain the TAI corresponding date indicator
  NcTimestamp tx;
  tx.SetMJD(fTmjd,fTsec,fTns,fTps);
  if (fTmjd>=40587 && (fTmjd<65442 || (fTmjd==65442 && fTsec<8047)))
  {
   tx.GetDate(kTRUE,0,&y,&m,&d);
   wd=tx.GetDayOfWeek(kTRUE,0);
   cout << " " << day[wd-1].Data() << ", " << setfill('0') << setw(2) << d << " "
        << setfill(' ') << month[m-1].Data() << " " << y << " ";
   date=kTRUE;
  }
  else
  {
   cout << " Time ";
   date=kFALSE;
  }

  // Determine the TAI derived times
  GetTAI(hh,mm,ss,ns,ps,"TAI");
  cout << setfill('0') << setw(2) << hh << ":"
       << setw(2) << mm << ":" << setw(2) << ss << "."
       << setw(9) << ns << setw(3) << ps << " (TAI) ";

  GetTAI(hh,mm,ss,ns,ps,"UTC");
  cout << setfill('0') << setw(2) << hh << ":"
       << setw(2) << mm << ":" << setw(2) << ss << "."
       << setw(9) << ns << setw(3) << ps << " (UTC)" << endl;

  GetTAI(hh,mm,ss,ns,ps,"GPS");
  if (!date)
  {
    cout << "      ";
  }
  else
  {
   cout << "                  ";
  }
  cout << setfill('0') << setw(2) << hh << ":"
       << setw(2) << mm << ":" << setw(2) << ss << "."
       << setw(9) << ns << setw(3) << ps << " (GPS) ";

  GetTAI(hh,mm,ss,ns,ps,"TT");
  cout << setfill('0') << setw(2) << hh << ":"
       << setw(2) << mm << ":" << setw(2) << ss << "."
       << setw(9) << ns << setw(3) << ps << " (TT)" << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetJD(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns) const
{
// Provide the (fractional) Julian Date (JD) corresponding to the UT date
// and time in the Gregorian calendar as specified by the input arguments.
//
// The input arguments represent the following :
// y  : year in UT (e.g. 1952, 2003 etc...)
// m  : month in UT (1=jan  2=feb etc...)
// d  : day in UT (1-31)
// hh : elapsed hours in UT (0-23) 
// mm : elapsed minutes in UT (0-59)
// ss : elapsed seconds in UT (0-59)
// ns : remaining fractional elapsed second of UT in nanosecond
//
// This algorithm is valid for all AD dates in the Gregorian calendar
// following the recipe of R.W. Sinnott Sky & Telescope 82, (aug. 1991) 183.
// See also http://scienceworld.wolfram.com/astronomy/JulianDate.html
//
// In case of invalid input, a value of -1 is returned.
//
// Note :
// ------
// This memberfunction only provides the JD corresponding to the
// UT input arguments. It does NOT set the corresponding Julian parameters
// for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp. 

 if (y<0 || m<1 || m>12 || d<1 || d>31) return -1;
 if (hh<0 || hh>23 || mm<0 || mm>59 || ss<0 || ss>59 || ns<0 || ns>1e9) return -1;

 // The UT daytime in fractional hours
 Double_t ut=double(hh)+double(mm)/60.+(double(ss)+double(ns)*1.e-9)/3600.;

 Double_t JD=0;
 
 JD=367*y-int(7*(y+int((m+9)/12))/4)
    -int(3*(int((y+(m-9)/7)/100)+1)/4)
    +int(275*m/9)+d+1721028.5+ut/24.;

 return JD;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetMJD(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns) const
{
// Provide the (fractional) Modified Julian Date corresponding to the UT
// date and time in the Gregorian calendar as specified by the input arguments.
//
// The input arguments represent the following :
// y  : year in UT (e.g. 1952, 2003 etc...)
// m  : month in UT (1=jan  2=feb etc...)
// d  : day in UT (1-31)
// hh : elapsed hours in UT (0-23) 
// mm : elapsed minutes in UT (0-59)
// ss : elapsed seconds in UT (0-59)
// ns : remaining fractional elapsed second of UT in nanosecond
//
// This algorithm is valid for all AD dates in the Gregorian calendar
// following the recipe of R.W. Sinnott Sky & Telescope 82, (aug. 1991) 183.
// See also http://scienceworld.wolfram.com/astronomy/JulianDate.html
//
// In case of invalid input, a value of -1 is returned.
//
// Note :
// ------
// This memberfunction only provides the MJD corresponding to the
// UT input arguments. It does NOT set the corresponding Julian parameters
// for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp.

 Double_t JD=GetJD(y,m,d,hh,mm,ss,ns);

 if (JD<0) return JD;

 Double_t MJD=JD-2400000.5;

 return MJD; 
} 
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetTJD(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns) const
{
// Provide the (fractional) Truncated Julian Date corresponding to the UT
// date and time in the Gregorian calendar as specified by the input arguments.
//
// The input arguments represent the following :
// y  : year in UT (e.g. 1952, 2003 etc...)
// m  : month in UT (1=jan  2=feb etc...)
// d  : day in UT (1-31)
// hh : elapsed hours in UT (0-23) 
// mm : elapsed minutes in UT (0-59)
// ss : elapsed seconds in UT (0-59)
// ns : remaining fractional elapsed second of UT in nanosecond
//
// This algorithm is valid for all AD dates in the Gregorian calendar
// following the recipe of R.W. Sinnott Sky & Telescope 82, (aug. 1991) 183.
// See also http://scienceworld.wolfram.com/astronomy/JulianDate.html
//
// In case of invalid input, a value of -1 is returned.
//
// Note :
// ------
// This memberfunction only provides the TJD corresponding to the
// UT input arguments. It does NOT set the corresponding Julian parameters
// for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp.

 Double_t JD=GetJD(y,m,d,hh,mm,ss,ns);

 if (JD<0) return JD;

 Double_t TJD=JD-2440000.5;

 return TJD; 
} 
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetJE(Double_t date,TString mode) const
{
// Provide the Julian Epoch (JE) corresponding to the specified date.
// The argument "mode" indicates the type of the argument "date".
//
// Available modes are :
// mode = "jd"  ==> date represents the Julian Date
//      = "mjd" ==> date represents the Modified Julian Date
//      = "tjd" ==> date represents the Truncated Julian Date
//
// The default is mode="jd".
//
// In case of invalid input, a value of -99999 is returned.
//
// Note :
// ------
// This memberfunction only provides the JE corresponding to the
// input arguments. It does NOT set the corresponding Julian parameters
// for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp.

 if ((mode != "jd") && (mode != "mjd") && (mode != "tjd")) return -99999;

 Double_t jd=date;
 if (mode=="mjd") jd=date+2400000.5;
 if (mode=="tjd") jd=date+2440000.5;

 Double_t je=2000.+(jd-2451545.)/365.25;

 return je;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetBE(Double_t date,TString mode) const
{
// Provide the Besselian Epoch (JE) corresponding to the specified date.
// The argument "mode" indicates the type of the argument "date".
//
// Available modes are :
// mode = "jd"  ==> date represents the Julian Date
//      = "mjd" ==> date represents the Modified Julian Date
//      = "tjd" ==> date represents the Truncated Julian Date
//
// The default is mode="jd".
//
// In case of invalid input, a value of -99999 is returned.
//
// Note :
// ------
// This memberfunction only provides the BE corresponding to the
// input arguments. It does NOT set the corresponding Julian parameters
// for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp.

 if ((mode != "jd") && (mode != "mjd") && (mode != "tjd")) return -99999;

 Double_t jd=date;
 if (mode=="mjd") jd=date+2400000.5;
 if (mode=="tjd") jd=date+2440000.5;

 Double_t be=1900.+(jd-2415020.31352)/365.242198781;

 return be;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::Convert(Double_t date,Int_t& days,Int_t& secs,Int_t& ns) const
{
// Convert date as fractional day count into integer days, secs and ns.
//
// Note : Due to computer accuracy the ns value may become inaccurate.
//
// The arguments represent the following :
// date : The input date as fractional day count
// days : Number of elapsed days
// secs : Remaining number of elapsed seconds
// ns   : Remaining fractional elapsed second in nanoseconds
//
// Note :
// ------
// This memberfunction only converts the input date into the corresponding
// integer parameters. It does NOT set the corresponding Julian parameters
// for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp.
 
 days=int(date);
 date=date-double(days);
 Int_t daysecs=24*3600;
 date=date*double(daysecs);
 secs=int(date);
 date=date-double(secs);
 ns=int(date*1.e9);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::Convert(Int_t days,Int_t secs,Int_t ns) const
{
// Convert date in integer days, secs and ns into fractional day count. 
//
// Note : Due to computer accuracy the ns precision may be lost.
//
// The input arguments represent the following :
// days : Number of elapsed days
// secs : Remaining number of elapsed seconds
// ns   : Remaining fractional elapsed second in nanoseconds
//
// Note :
// ------
// This memberfunction only converts the input integer parameters into the
// corresponding fractional day count. It does NOT set the corresponding
// Julian parameters for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp.

 Double_t frac=double(secs)+double(ns)*1.e-9;
 Int_t daysecs=24*3600;
 frac=frac/double(daysecs);
 Double_t date=double(days)+frac;
 return date;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::Convert(Double_t h,Int_t& hh,Int_t& mm,Int_t& ss,Int_t& ns,Int_t& ps) const
{
// Convert fractional hour count h into hh:mm:ss:ns:ps.
// The sign of the input value will be neglected, so h<0 will result in
// the same output values as h>0.
//
// Note : Due to computer accuracy the ps value may become inaccurate.
//
// Note :
// ------
// This memberfunction only converts the input "h" into the corresponding
// integer parameters. It does NOT set the corresponding Julian parameters
// for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp.
 
 // Neglect sign of h
 h=fabs(h);

 hh=int(h);
 h=h-double(hh);
 h=h*60.;
 mm=int(h);
 h=h-double(mm);
 h=h*60.;
 ss=int(h);
 h=h-double(ss);
 h=h*1.e9;
 ns=int(h);
 h=h-double(ns);
 h=h*1000.;
 ps=int(h);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::Convert(Double_t h,Int_t& hh,Int_t& mm,Double_t& ss) const
{
// Convert fractional hour count h into hh:mm:ss.s.
// The sign of the input value will be neglected, so h<0 will result in
// the same output values as h>0.
//
// Notes :
// -------
// 1) This memberfunction only converts the input "h" into the corresponding
//    hh:mm:ss.s values. It does NOT set the corresponding Julian parameters
//    for the current NcTimestamp instance.
//    As such the TTimeStamp limitations do NOT apply to this memberfunction.
//    To set the Julian parameters for the current NcTimestamp instance,
//    please use the corresponding SET() memberfunctions of either NcTimestamp
//    or TTimeStamp.
// 2) This facility can also be used to convert degrees in arcminutes etc...
 
 // Neglect sign of h
 h=fabs(h);
 
 hh=int(h);
 h=h-double(hh);
 h=h*60.;
 mm=int(h);
 h=h-double(mm);
 ss=h*60.;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::Convert(Int_t hh,Int_t mm,Int_t ss,Int_t ns,Int_t ps) const
{
// Convert hh:mm:ss:ns:ps into fractional hour count. 
// The sign of the input values will be neglected, so the output value
// will always correspond to a positive hh:mm:ss:ns:ps specification.
//
// Note : Due to computer accuracy the ps precision may be lost.
//
// Note :
// ------
// This memberfunction only converts the input integer parameters into the
// corresponding fractional hour count. It does NOT set the corresponding
// Julian parameters for the current NcTimestamp instance.
// As such the TTimeStamp limitations do NOT apply to this memberfunction.
// To set the Julian parameters for the current NcTimestamp instance,
// please use the corresponding SET() memberfunctions of either NcTimestamp
// or TTimeStamp.

 // Neglect the sign of the input values
 hh=abs(hh);
 mm=abs(mm);
 ss=abs(ss);
 ns=abs(ns);
 ps=abs(ps);

 Double_t h=hh;
 h+=double(mm)/60.+(double(ss)+double(ns)*1.e-9+double(ps)*1.e-12)/3600.;

 return h;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::Convert(Int_t hh,Int_t mm,Double_t ss) const
{
// Convert hh:mm:ss.s into fractional hour count. 
// The sign of the input values will be neglected, so the output value
// will always correspond to a positive hh:mm:ss.s specification.
//
// Notes :
// -------
// 1) This memberfunction only converts the input hh:mm:ss.s data into the
//    corresponding fractional hour count. It does NOT set the corresponding
//    Julian parameters for the current NcTimestamp instance.
//    As such the TTimeStamp limitations do NOT apply to this memberfunction.
//    To set the Julian parameters for the current NcTimestamp instance,
//    please use the corresponding SET() memberfunctions of either NcTimestamp
//    or TTimeStamp.
// 2) This facility can also be used to convert ddd:mm:ss.s into fractional degrees.

 // Neglect the sign of the input values
 hh=abs(hh);
 mm=abs(mm);
 ss=fabs(ss);

 Double_t h=hh;
 h+=double(mm)/60.+ss/3600.;

 return h;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::PrintTime(Double_t h,Int_t ndig) const
{
// Print a fractional hour count in hh:mm:ss.ssss format.
// The range of the printed hour value is : -24 < hh < 24.
// The argument "ndig" specifies the number of digits for the fractional
// seconds (e.g. ndig=6 corresponds to microsecond precision).
// No rounding will be performed, so a second count of 3.473 with ndig=1
// will appear as 03.4 on the output.
// Due to computer accuracy, precision on the picosecond level may get lost.
//
// The default is ndig=1.
//
// Note : The time info is printed without additional spaces or "endline".
//        This allows the print to be included in various composite output formats.

 Int_t hh,mm,ss;
 ULong64_t sfrac;
 Double_t s;

 while (h<-24)
 {
  h+=24.;
 }
 while (h>24)
 {
  h-=24.;
 }
   
 Convert(h,hh,mm,s);
 ss=Int_t(s);
 s-=Double_t(ss);
 s*=pow(10.,ndig);
 sfrac=ULong64_t(s);

 if (h<0) cout << "-";
 cout << setfill('0')
      << setw(2) << hh << ":" << setw(2) << mm << ":"
      << setw(2) << ss << "." << setw(ndig) << sfrac;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::FillJulian()
{
// Internal memberfunction for calculation and setting of the Julian date/time parameters
// corresponding to the current TTimeStamp UTC date/time parameters.

 UInt_t y,m,d,hh,mm,ss;

 GetDate(kTRUE,0,&y,&m,&d);
 GetTime(kTRUE,0,&hh,&mm,&ss);
 Int_t ns=GetNanoSec();

 Double_t mjd=GetMJD(y,m,d,hh,mm,ss,ns);

 fMJD=int(mjd);
 fJsec=GetSec()%(24*3600); // Daytime in elapsed seconds
 fJns=ns;                  // Remaining fractional elapsed second in nanoseconds

 // Store the TTimeStamp seconds and nanoseconds values
 // for which this Julian calculation was performed.
 fCalcs=GetSec();
 fCalcns=GetNanoSec();
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::GetMJD(Int_t& mjd,Int_t& sec,Int_t& ns)
{
// Provide the Modified Julian Date (MJD) and time corresponding to the
// currently stored NcTimestamp date/time parameters.
//
// The returned arguments represent the following :
// mjd : The modified Julian date.
// sec : The number of seconds elapsed within the MJD.
// ns  : The remaining fractional number of seconds (in ns) elapsed within the MJD.

 if (fCalcs != GetSec() || fCalcns != GetNanoSec())
 {
  FillJulian();
  SetUTCparameters("A",0,0);
 }

 mjd=fMJD;
 sec=fJsec;
 ns=fJns;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetMJD()
{
// Provide the (fractional) Modified Julian Date (MJD) corresponding to the
// currently stored NcTimestamp date/time parameters.
//
// Due to computer accuracy the ns precision may be lost.
// It is advised to use the (mjd,sec,ns) getter instead.

 Int_t mjd=0;
 Int_t sec=0;
 Int_t ns=0;
 GetMJD(mjd,sec,ns);

 Double_t date=Convert(mjd,sec,ns);

 return date;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::GetTJD(Int_t& tjd,Int_t& sec,Int_t& ns)
{
// Provide the Truncated Julian Date (TJD) and time corresponding to the
// currently stored NcTimestamp date/time parameters.
//
// The returned arguments represent the following :
// tjd : The truncated Julian date.
// sec : The number of seconds elapsed within the TJD.
// ns  : The remaining fractional number of seconds (in ns) elapsed within the TJD.

 Int_t mjd=0;
 GetMJD(mjd,sec,ns);

 tjd=mjd-40000;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetTJD()
{
// Provide the (fractional) Truncated Julian Date (TJD) corresponding to the
// currently stored NcTimestamp date/time parameters.
//
// Due to computer accuracy the ns precision may be lost.
// It is advised to use the (tjd,sec,ns) getter instead.

 Int_t tjd=0;
 Int_t sec=0;
 Int_t ns=0;
 GetTJD(tjd,sec,ns);

 Double_t date=Convert(tjd,sec,ns);

 return date;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::GetTAI(Int_t& d,Int_t& sec,Int_t& ns,Int_t& ps,Bool_t tmjd)
{
// Provide the TAI day count and time corresponding to the currently stored
// NcTimestamp date/time parameters.
// The return value indicates whether the TAI and UTC parameters were actually
// set manually (1), automatically (-1) or not (0).
// In the latter case only UT time recording is available.
//
// The (returned) arguments represent the following :
// d    : The number of TAI days elapsed.
// sec  : The number of seconds elapsed within the TAI day.
// ns   : The remaining fractional number of seconds (in ns) elapsed within the TAI day.
// ps   : The remaining fractional number of nanoseconds (in picoseconds) elapsed within the TAI day.
// tmjd : kTRUE  ==> Provide the MJD equivalent TAI day count.
//        kFALSE ==> Count the elapsed days since the TAI start epoch 01-jan-1958 00:00:00 UT (MJD=36204).
//
// The default value is mjd=kTRUE.
//
// Notes :
// -------
// 1) In case TAI recording was not activated, all returned values will be zero.
// 2) In case tmjd=kFALSE and the current timestamp lies before the TAI start epoch,
//    a negative day count is returned.

 // Make sure to have the updated parameters
 GetMJD(d,sec,ns);
 FillTAI();

 d=0;
 sec=0;
 ns=0;
 ps=0;

 if (!fUtc) return 0;

 d=fTmjd;
 sec=fTsec;
 ns=fTns;
 ps=fTps;

 if (!tmjd) d-=36204;

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetTAI(Bool_t tmjd)
{
// Provide the (fractional) number of elapsed TAI days corresponding to the
// currently stored NcTimestamp date/time parameters.
//
// Due to computer accuracy the ns precision may be lost.
// It is advised to use the (d,sec,ns,ps) getter instead.
//
// The input argument :
// tmjd : kTRUE  ==> Provide the MJD equivalent TAI day count.
//        kFALSE ==> Count the elapsed days since the TAI start epoch 01-jan-1958 00:00:00 UT (MJD=36204).
//
// The default value is tmjd=kTRUE.
//
// Notes :
// -------
// 1) In case TAI recording was not activated, the value 0 will be returned.
// 2) In case tmjd=kFALSE and the current timestamp lies before the TAI start epoch,
//    a negative value is returned.

 if (!fUtc) return 0;

 Int_t d=0;
 Int_t s=0;
 Int_t ns=0;
 Int_t ps=0;
 GetTAI(d,s,ns,ps,tmjd);

 Double_t days=Convert(d,s,ns);

 return days;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::GetTAI(Int_t& hh,Int_t& mm,Int_t& ss,Int_t& ns,Int_t& ps,TString type)
{
// Provide the corrresponding TAI time as hh:mm:ss:ns:ps.
// The return value indicates whether the TAI and UTC parameters were actually
// set manually (1), automatically (-1) or not (0).
// In the latter case only UT time recording is available.
//
// The "type" input argument represents the following :
// type : TAI ==> Time provided is the International Atomic Time
//        UTC ==> Time provided is the Coordinated Universal Time
//        GPS ==> Time provided is the Global Positioning System time
//        TT  ==> Time provided is the Terrestrial Time
//
// The default value is type="TAI".
//
// Notes :
// -------
// 1) In case TAI recording was not activated or the specified "type" is unknown,
//    all returned values will be zero.
// 3) This facility is based on the TAI day count, so the TTimeStamp limitations
//    do not apply here.

 hh=0;
 mm=0;
 ss=0;
 ns=0;
 ps=0;

 if (type!="TAI" && type!="UTC" && type!="GPS" && type!="TT") return 0;

 Int_t d,sec,nsec,psec;

 // Use a dummy timestamp to easily correct for the various offsets
 NcTimestamp tx=(*this);
 if (type=="UTC") tx.Add(0,-fLeap,0,0);
 if (type=="GPS") tx.Add(0,-19,0,0);
 if (type=="TT") tx.Add(0,32,184000000,0);

 tx.GetTAI(d,sec,nsec,psec);

 hh=sec/3600;
 sec=sec%3600;
 mm=sec/60;
 ss=sec%60;
 ns=nsec;
 ps=psec;

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetUnixTime()
{
// Provide the Unix time.
// Unix Time is also called POSIX Time or UNIX Epoch Time and represents
// the (fractional) elapsed second count since the start of the Unix Epoch. 
//
// Unix Time is closely releated to UTC and represents the (fractional) elapsed second count
// since the start of the Unix Epoch, in which every Unix Time day contains exactly 86400 seconds.
// This implies that for Unix Time the UTC leap seconds should be ignored as explained below.
// The UNIX Time EPOCH starts at 01-jan-1970 00:00:00 UTC which corresponds to JD=2440587.5
// (i.e. the start of MJD=40587).
// Synchronization with UTC is obtained by continuing the second count when a UTC leap second
// occurs and then the Unix Time count jumps up or down 1 second at the beginning of the new
// day after the UTC leap second has expired.
// In case of a negative UTC leap second (which has not occurred so far) this would result
// in a jump forward of 1 second in Unix Time, introducing a "gap" of 1 second in the timing
// at the start of the new day.
// In case of a positive UTC leap second this results in a jump back of 1 second in Unix Time
// at the start of a new day.
// In this case there exist 2 ambiguous Unix Times over a 1 second time interval,
// namely at the beginning of the introduction of the UTC leap second and at the moment that
// the UTC leap second expires, i.e. the start of the new day.
//
// Due to a limitation on the "seconds since the EPOCH start" count,
// the latest accessible date/time is 19-jan-2038 02:14:08 UT.
//
// Due to computer accuracy the ns precision may be lost.
// For better precision it is advised to use the other date/time Get() facilities instead.

 Double_t t=0;

 Int_t days=0;
 Int_t secs=0;
 Int_t ns=0;
 Int_t mjd=0;
 GetMJD(mjd,secs,ns);
 Int_t ps=GetPs();

 // Get accurate UTC from UT via UTC=UT1-dUT
 if (fUtc)
 {
  NcTimestamp tx;
  tx.SetMJD(mjd,secs,ns,ps);
  tx.AddSec(-fDut);
  tx.GetMJD(mjd,secs,ns);
  ps=tx.GetPs();
 }

 days=mjd-40587;

 t=(days*86400)+secs;
 t+=double(ns)*1.e-9+double(ps)*1.e-12;

 return t;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::GetJD(Int_t& jd,Int_t& sec,Int_t& ns)
{
// Provide the Julian Date (JD) and time corresponding to the currently
// stored NcTimestamp date/time parameters.
//
// The returned arguments represent the following :
// jd  : The Julian date.
// sec : The number of seconds elapsed within the JD.
// ns  : The remaining fractional number of seconds (in ns) elapsed within the JD.

 Int_t mjd=0;
 GetMJD(mjd,sec,ns);

 jd=mjd+2400000;
 sec+=12*3600;
 if (sec >= 24*3600)
 {
  sec-=24*3600;
  jd+=1;
 }
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetJD()
{
// Provide the (fractional) Julian Date (JD) corresponding to the currently
// stored NcTimestamp date/time parameters.
//
// Due to computer accuracy the ns precision may be lost.
// It is advised to use the (jd,sec,ns) getter instead.

 Int_t jd=0;
 Int_t sec=0;
 Int_t ns=0;
 GetJD(jd,sec,ns);

 Double_t date=Convert(jd,sec,ns);

 return date;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetJE()
{
// Provide the Julian Epoch (JE) corresponding to the currently stored
// NcTimestamp date/time parameters.

 Double_t jd=GetJD();
 Double_t je=GetJE(jd);
 return je;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetBE()
{
// Provide the Besselian Epoch (BE) corresponding to the currently stored
// NcTimestamp date/time parameters.

 Double_t jd=GetJD();
 Double_t be=GetBE(jd);
 return be;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetMJD(Int_t mjd,Int_t sec,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut)
{
// Set the Modified Julian Date (MJD) and time and update the TTimeStamp
// parameters accordingly (if possible).
// The provided UTC parameters (see below) will not affect the MJD setting.
//
// Note :
// ------
// The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00 UT
// which corresponds to the start of MJD=40587.
// Using the corresponding MJD of this EPOCH allows construction of
// the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input MJD and time.
// Obviously this TTimeStamp implementation would prevent usage of MJD values
// smaller than 40587.
// Furthermore, due to a limitation on the "seconds since the EPOCH start" count
// in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08 UT.
// However, this NcTimestamp facility provides support for the full range
// of (M)JD values, but the setting of the corresponding TTimeStamp parameters
// is restricted to the values allowed by the TTimeStamp implementation.
// For these earlier/later MJD values, the standard TTimeStamp parameters will
// be set corresponding to the start of the TTimeStamp EPOCH.  
// This implies that for these earlier/later MJD values the TTimeStamp parameters
// do not match the Julian parameters of NcTimestamp.  
//
// The input arguments represent the following :
// mjd  : The modified Julian date.
// sec  : The number of seconds elapsed within the MJD.
// ns   : The remaining fractional number of seconds (in ns) elapsed within the MJD.
// ps   : The remaining fractional number of nanoseconds (in ps) elapsed within the MJD.
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The date/time that was specified represents UT1, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                This mode is equivalent to utc="A" apart from the fact that the reference time represents UT1.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 2) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The defaults are ps=0, utc="U", leap=0 and dut=0.

 if (sec<0 || sec>=24*3600 || ns<0 || ns>=1e9 || ps<0 || ps>=1000)
 {
  cout << " *NcTimestamp::SetMJD* Invalid input."
       << " sec : " << sec << " ns : " << ns << " ps : " << ps << endl; 
  return;
 }

 fMJD=mjd;
 fJsec=sec;
 fJns=ns;
 fJps=ps;

 Int_t epoch=40587; // MJD of the start of the epoch
 Int_t limit=65442; // MJD of the latest possible TTimeStamp date/time
 
 Int_t date,time;
 if (mjd<epoch || mjd>limit || (mjd==limit && sec>=8047))
 {
  Set(0,kFALSE,0,kFALSE);
  date=GetDate();
  time=GetTime();
  Set(date,time,0,kTRUE,0);
 }
 else
 {
  // The elapsed time since start of EPOCH
  Int_t days=mjd-epoch;
  UInt_t secs=days*24*3600;
  secs+=sec;
  Set(secs,kFALSE,0,kFALSE);
  date=GetDate();
  time=GetTime();
  Set(date,time,ns,kTRUE,0);
 }

 // Denote that the Julian and TTimeStamp parameters are synchronised,
 // even in the case the MJD falls outside the TTimeStamp validity range.
 // The latter still allows retrieval of Julian parameters for these
 // earlier times.
 fCalcs=GetSec();
 fCalcns=GetNanoSec();

 // Update the UTC parameters and corresonding TAI time recording
 SetUTCparameters(utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetMJD(Double_t mjd,TString utc,Int_t leap,Double_t dut)
{
// Set the Modified Julian Date (MJD) and time and update the TTimeStamp
// parameters accordingly (if possible).
// The provided UTC parameters (see below) will not affect the MJD setting.
//
// Note :
// ------
// The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00 UT
// which corresponds to the start of MJD=40587.
// Using the corresponding MJD of this EPOCH allows construction of
// the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input MJD and time.
// Obviously this TTimeStamp implementation would prevent usage of MJD values
// smaller than 40587.
// Furthermore, due to a limitation on the "seconds since the EPOCH start" count
// in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08 UT.
// However, this NcTimestamp facility provides support for the full range
// of (M)JD values, but the setting of the corresponding TTimeStamp parameters
// is restricted to the values allowed by the TTimeStamp implementation.
// For these earlier/later MJD values, the standard TTimeStamp parameters will
// be set corresponding to the start of the TTimeStamp EPOCH.  
// This implies that for these earlier/later MJD values the TTimeStamp parameters
// do not match the Julian parameters of NcTimestamp.  
//
// Due to computer accuracy the ns precision may be lost.
// It is advised to use the (mjd,sec,ns) setting instead.
//
// The input arguments represent the following :
// mjd  : The modified Julian date as fractional day count.
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The date/time that was specified represents UT1, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                This mode is equivalent to utc="A" apart from the fact that the reference time represents UT1.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 2) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are utc="U", leap=0 and dut=0.

 Int_t days=0;
 Int_t secs=0;
 Int_t ns=0;
 Convert(mjd,days,secs,ns);
 SetMJD(days,secs,ns,0,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetJD(Int_t jd,Int_t sec,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut)
{
// Set the Julian Date (JD) and time and update the TTimeStamp
// parameters accordingly (if possible).
// The provided UTC parameters (see below) will not affect the JD setting.
//
// Note :
// ------
// The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00 UT
// which corresponds to JD=2440587.5 or the start of MJD=40587.
// Using the corresponding MJD of this EPOCH allows construction of
// the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input MJD and time.
// Obviously this TTimeStamp implementation would prevent usage of values
// smaller than JD=2440587.5.
// Furthermore, due to a limitation on the "seconds since the EPOCH start" count
// in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08 UT.
// However, this NcTimestamp facility provides support for the full range
// of (M)JD values, but the setting of the corresponding TTimeStamp parameters
// is restricted to the values allowed by the TTimeStamp implementation.
// For these earlier/later JD values, the standard TTimeStamp parameters will
// be set corresponding to the start of the TTimeStamp EPOCH.  
// This implies that for these earlier/later (M)JD values the TTimeStamp parameters
// do not match the Julian parameters of NcTimestamp.  
//
// The input arguments represent the following :
// jd   : The Julian date.
// sec  : The number of seconds elapsed within the JD.
// ns   : The remaining fractional number of seconds (in ns) elapsed within the JD.
// ps   : The remaining fractional number of nanoseconds (in ps) elapsed within the JD.
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The date/time that was specified represents UT1, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                This mode is equivalent to utc="A" apart from the fact that the reference time represents UT1.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 2) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are ps=0, utc="U", leap=0 and dut=0.

 Int_t mjd=jd-2400000;
 sec-=12*3600;
 if (sec<0)
 {
  sec+=24*3600;
  mjd-=1;
 }

 SetMJD(mjd,sec,ns,ps,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetJD(Double_t jd,TString utc,Int_t leap,Double_t dut)
{
// Set the Julian Date (JD) and time and update the TTimeStamp
// parameters accordingly (if possible).
// The provided UTC parameters (see below) will not affect the JD setting.
//
// Note :
// ------
// The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00 UT
// which corresponds to JD=2440587.5 or the start of MJD=40587.
// Using the corresponding MJD of this EPOCH allows construction of
// the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input MJD and time.
// Obviously this TTimeStamp implementation would prevent usage of values
// smaller than JD=2440587.5.
// Furthermore, due to a limitation on the "seconds since the EPOCH start" count
// in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08 UT.
// However, this NcTimestamp facility provides support for the full range
// of (M)JD values, but the setting of the corresponding TTimeStamp parameters
// is restricted to the values allowed by the TTimeStamp implementation.
// For these earlier/later JD values, the standard TTimeStamp parameters will
// be set corresponding to the start of the TTimeStamp EPOCH.  
// This implies that for these earlier/later (M)JD values the TTimeStamp parameters
// do not match the Julian parameters of NcTimestamp.  
//
// Due to computer accuracy the ns precision may be lost.
// It is advised to use the (jd,sec,ns) setting instead.
//
// The input arguments represent the following :
// jd   : The Julian date as fractional day count.
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The date/time that was specified represents UT1, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                This mode is equivalent to utc="A" apart from the fact that the reference time represents UT1.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 2) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are utc="U", leap=0 and dut=0.

 Int_t days=0;
 Int_t secs=0;
 Int_t ns=0;
 Convert(jd,days,secs,ns);

 SetJD(days,secs,ns,0,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetTJD(Int_t tjd,Int_t sec,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut)
{
// Set the Truncated Julian Date (TJD) and time and update the TTimeStamp
// parameters accordingly (if possible).
// The provided UTC parameters (see below) will not affect the TJD setting.
//
// Note :
// ------
// The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00 UT
// which corresponds to JD=2440587.5 or the start of TJD=587.
// Using the corresponding MJD of this EPOCH allows construction of
// the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input MJD and time.
// Obviously this TTimeStamp implementation would prevent usage of values
// smaller than TJD=587.
// Furthermore, due to a limitation on the "seconds since the EPOCH start" count
// in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08 UT.
// However, this NcTimestamp facility provides support for the full range
// of (T)JD values, but the setting of the corresponding TTimeStamp parameters
// is restricted to the values allowed by the TTimeStamp implementation.
// For these earlier/later JD values, the standard TTimeStamp parameters will
// be set corresponding to the start of the TTimeStamp EPOCH.  
// This implies that for these earlier/later (T)JD values the TTimeStamp parameters
// do not match the Julian parameters of NcTimestamp.  
//
// The input arguments represent the following :
// tjd  : The Truncated Julian date.
// sec  : The number of seconds elapsed within the JD.
// ns   : The remaining fractional number of seconds (in ns) elapsed within the JD.
// ps   : The remaining fractional number of nanoseconds (in ps) elapsed within the JD.
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The date/time that was specified represents UT1, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                This mode is equivalent to utc="A" apart from the fact that the reference time represents UT1.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 2) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are ps=0, utc="U", leap=0 and dut=0.

 Int_t mjd=tjd+40000;

 SetMJD(mjd,sec,ns,ps,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetTJD(Double_t tjd,TString utc,Int_t leap,Double_t dut)
{
// Set the Truncated Julian Date (TJD) and time and update the TTimeStamp
// parameters accordingly (if possible).
// The provided UTC parameters (see below) will not affect the TJD setting.
//
// Note :
// ------
// The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00 UT
// which corresponds to JD=2440587.5 or the start of TJD=587.
// Using the corresponding MJD of this EPOCH allows construction of
// the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input MJD and time.
// Obviously this TTimeStamp implementation would prevent usage of values
// smaller than TJD=587.
// Furthermore, due to a limitation on the "seconds since the EPOCH start" count
// in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08 UT.
// However, this NcTimestamp facility provides support for the full range
// of (T)JD values, but the setting of the corresponding TTimeStamp parameters
// is restricted to the values allowed by the TTimeStamp implementation.
// For these earlier/later JD values, the standard TTimeStamp parameters will
// be set corresponding to the start of the TTimeStamp EPOCH.  
// This implies that for these earlier/later (T)JD values the TTimeStamp parameters
// do not match the Julian parameters of NcTimestamp.  
//
// Due to computer accuracy the ns precision may be lost.
// It is advised to use the (jd,sec,ns) setting instead.
//
// The input arguments represent the following :
// tjd  : The Truncated Julian date as fractional day count.
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The date/time that was specified represents UTC, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The date/time that was specified represents UT1, and serves as the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                This mode is equivalent to utc="A" apart from the fact that the reference time represents UT1.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 2) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are utc="U", leap=0, dut=0.

 Int_t days=0;
 Int_t secs=0;
 Int_t ns=0;
 Convert(tjd,days,secs,ns);

 SetTJD(days,secs,ns,0,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::FillTAI()
{
// Internal memberfunction to calculate and set the TAI day count parameters
// corresponding to the current NcTimestamp parameters.
// A separate MJD counting system is recorded for the TAI timing system. 

 if (!fUtc || fUtc==-3)
 {
  fTmjd=0;
  fTsec=0;
  fTns=0;
  fTps=0;
  return;
 }

 // Use memberfunction to ensure most recent values 
 GetMJD(fTmjd,fTsec,fTns);
 fTps=GetPs();

 // Dummy timestamp to easily obtain TAI based day etc. counts
 // It is essential not to use UTC parameters here in order to prevent an infinite loop
 NcTimestamp tx;
 tx.SetMJD(fTmjd,fTsec,fTns,fTps,"N");

 tx.AddCalc(0,fLeap,0,0,kFALSE); // Account for the leap seconds
 tx.AddSecCalc(-fDut,kFALSE);    // Account for dUT=UT1-UTC

 // Retrieve the corresponding TAI day etc. count
 tx.GetMJD(fTmjd,fTsec,fTns);
 fTps=tx.GetPs();
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::SetTAI(TString type,TString date,TString time,Int_t mode,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the specified TAI based date and time
// in the Gregorian calendar as specified by the input arguments.
// Based on the specified accumulated number of Leap Seconds ("leap") and the UT1-UTC value ("dut")
// also the UT will be set.
// The return value indicates whether the date/time and UTC parameters are actually
// set Manually (1), Automatically (-1) or Failed (0).
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetTAI() or SetUT() memberfunctions.
//
// The input arguments represent the following :
// type : UTC ==> Date/time provided by the Coordinated Universal Time broadcast
//        GPS ==> Date/time provided by the Global Positioning System broadcast
//        TAI ==> Date/time provided by the International Atomic Time recording
//        TT  ==> Date/time provided by the Terrestrial Time recording
// date : The date in the format dd-mm-yyyy or dd/mm/yyyy or ddmmyyyy (mode=0)
// date : The date in the format yyyy-mm-dd or yyyy/mm/dd or yyyymmdd (mode=1)
// date : The date in the format mm-dd-yyyy or mm/dd/yyyy or mmddyyyy (mode=2)
// date : The date in the format yyyy-dd-mm or yyyy/dd/mm or yyyyddmm (mode=3)
// time : The time in the format hh:mm:ss.sss
// mode : Date format specifier (see above)
// utc  : Flag to denote which UTC parameter values "leap" and "dut" (see below) should be used.
//        "M" ==> Manually provided UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatically provided UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// The default value is dut=0, but if <1 sec precision is required, the actual dut value should be provided.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
//    Also JD=0 will be set to indicate that something went wrong.
//    The same holds in case utc="A" and no data files have been loaded, or no
//    IERS information for the provided date/time is available.
// 2) For the available IERS information please refer to the docs of LoadUTCparameterFiles().
// 3) In case a date/time setting outside the range of the IERS information is required,
//    the manual setting of "leap" and "dut" has to be invoked.  

 Int_t ibad=0;

 if (type!="UTC" && type!="GPS" && type!="TAI" && type!="TT") ibad=1;

 if (utc!="M" && utc!="A") ibad=1;

 if (utc=="M" && fabs(dut)>0.9) ibad=1;

 if (utc=="A" && !fUTCdata) ibad=1;

 // In case utc="A" check whether the corresponding IERS database info is available
 if (utc=="A")
 {
  NcTimestamp tx; // Dummy timestamp for easy MJD retrieval
  tx.SetUT(date,time,mode,utc);
  Int_t ien=GetUTCparameters(tx.fMJD,leap,dut);
  if (ien<0) ibad=1;
 }

 if (ibad)
 {
  SetJD(0,"N");
  return fUtc;
 }

 SetUT(date,time,mode,utc,leap,dut);
 if (type != "UTC") Add(0,-fLeap,0,0);      // Account for the leap seconds
 if (type == "GPS") Add(0,19,0,0);          // Account for TAI-GPS=19 sec.
 if (type == "TT") Add(0,-32,-184000000,0); // Account for TAI-TT=-32.184 sec.

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::SetTAI(Int_t d,Int_t sec,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut,Bool_t tmjd)
{
// Set the International Atomic Time (TAI) date and time and update the TTimeStamp
// parameters accordingly (if possible).
// The return value indicates whether the date/time and UTC parameters are actually
// set Manually (1), Automatically (-1) or Failed (0).
//
// The input arguments represent the following :
// d    : The TAI day count according to the specified "tmjd" parameter (see below).
// sec  : The number of seconds elapsed within the TAI day.
// ns   : The remaining fractional number of seconds (in ns) elapsed within the TAI day.
// ps   : The remaining fractional number of nanoseconds (in ps) elapsed within the TAI day.
// utc  : Flag to denote which UTC parameter values "leap" and "dut" (see below) should be used.
//        "M" ==> Manually provided UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatically provided UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
// tmjd : kTRUE  ==> The MJD equivalent TAI day count.
//        kFALSE ==> The TAI day count since the TAI start epoch 01-jan-1958 00:00:00 UT (MJD=36204).
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// The default values are dut=0 and tmjd=kFALSE.
// However, if <1 sec precision is required, the actual dut value should be provided.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
//    Also JD=0 will be set to indicate that something went wrong.
//    The same holds in case utc="A" and no data files have been loaded, or no
//    IERS information for the provided date/time is available.
// 2) For the available IERS information please refer to the docs of LoadUTCparameterFiles().
// 3) In case a date/time setting outside the range of the IERS information is required,
//    the manual setting of "leap" and "dut" has to be invoked.  
// 4) The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00 UT
//    which corresponds to JD=2440587.5 (i.e. the start of MJD=40587) or the
//    start of TAI=4383 w.r.t. the TAI start epoch 01-jan-1958 00:00:00 UT.
//    Using the corresponding TAI of this EPOCH allows construction of
//    the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input TAI day and time.
//    Obviously this TTimeStamp implementation would prevent usage of values
//    smaller than TAI=4383 for tmjd=kFALSE or TAI=40587 for tmjd=kTRUE.
//    Furthermore, due to a limitation on the "seconds since the EPOCH start" count
//    in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08 UT.
//    However, this NcTimestamp facility provides support for the full range
//    of TAI values, but the setting of the corresponding TTimeStamp parameters
//    is restricted to the values allowed by the TTimeStamp implementation.
//    For these earlier/later TAI values, the standard TTimeStamp parameters will
//    be set corresponding to the start of the TTimeStamp EPOCH.  
//    This implies that for these earlier/later TAI values the TTimeStamp parameters
//    do not match the TAI c.q. Julian parameters of NcTimestamp.  

 Int_t ibad=0;

 if (sec<0 || sec>86400 || ns<0 || ns>999999999 || ps<0 || ps>999) ibad=1;

 if (utc!="M" && utc!="A") ibad=1;

 if (utc=="M" && fabs(dut)>0.9) ibad=1;

 if (utc=="A" && !fUTCdata) ibad=1;

 // Set the corresponding MJD
 Int_t mjd=d;
 if (!tmjd) mjd+=36204;

 // In case utc="A" check whether the corresponding IERS database info is available
 if (utc=="A")
 {
  Int_t ien=GetUTCparameters(mjd,leap,dut);
  if (ien<0) ibad=1;
 }

 if (ibad)
 {
  SetJD(0,"N");
  return fUtc;
 }

 SetMJD(mjd,sec,ns,ps,utc,leap,dut);
 Add(0,-fLeap,0,0); // Account for the leap seconds
 AddSec(fDut);      // Account for dUT=UT1-UTC

 // Set the corresponding TAI day count etc.
 FillTAI();

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::SetTAI(Double_t tai,TString utc,Int_t leap,Double_t dut,Bool_t tmjd)
{
// Set the International Atomic Time (TAI) date and time and update the TTimeStamp
// parameters accordingly (if possible).
// The return value indicates whether the date/time and UTC parameters are actually
// set Manually (1), Automatically (-1) or Failed (0).
//
// Due to computer accuracy the ns precision may be lost.
// It is advised to use the (d,sec,ns,ps) setting instead.
//
// The input arguments represent the following :
// tai  : The TAI fractional day count according to the specified "tmjd" parameter (see below).
// utc  : Flag to denote which UTC parameter values "leap" and "dut" (see below) should be used.
//        "M" ==> Manually provided UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatically provided UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT-UTC in seconds.
// tmjd : kTRUE  ==> The MJD equivalent TAI day count.
//        kFALSE ==> The TAI day count since the TAI start epoch 01-jan-1958 00:00:00 UT (MJD=36204).
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// The default values are dut=0 and tmjd=kFALSE.
// However, if <1 sec precision is required, the actual dut value should be provided.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
//    Also JD=0 will be set to indicate that something went wrong.
//    The same holds in case utc="A" and no data files have been loaded, or no
//    IERS information for the provided date/time is available.
// 2) For the available IERS information please refer to the docs of LoadUTCparameterFiles().
// 3) In case a date/time setting outside the range of the IERS information is required,
//    the manual setting of "leap" and "dut" has to be invoked.  
// 4) The TTimeStamp EPOCH starts at 01-jan-1970 00:00:00 UT
//    which corresponds to JD=2440587.5 (i.e. the start of MJD=40587) or the
//    start of TAI=4383 w.r.t. the TAI start epoch 01-jan-1958 00:00:00 UT.
//    Using the corresponding TAI of this EPOCH allows construction of
//    the yy-mm-dd hh:mm:ss:ns TTimeStamp from a given input TAI day and time.
//    Obviously this TTimeStamp implementation would prevent usage of values
//    smaller than TAI=4383 for tmjd=kFALSE or TAI=40587 for tmjd=kTRUE.
//    Furthermore, due to a limitation on the "seconds since the EPOCH start" count
//    in TTimeStamp, the latest accessible date/time is 19-jan-2038 02:14:08 UT.
//    However, this NcTimestamp facility provides support for the full range
//    of TAI values, but the setting of the corresponding TTimeStamp parameters
//    is restricted to the values allowed by the TTimeStamp implementation.
//    For these earlier/later TAI values, the standard TTimeStamp parameters will
//    be set corresponding to the start of the TTimeStamp EPOCH.  
//    This implies that for these earlier/later TAI values the TTimeStamp parameters
//    do not match the TAI c.q. Julian parameters of NcTimestamp.  

 Int_t days=0;
 Int_t secs=0;
 Int_t ns=0;
 Convert(tai,days,secs,ns);

 SetTAI(days,secs,ns,0,utc,leap,dut,tmjd);

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::SetGPS(Int_t w,Int_t sow,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut,Int_t icycle)
{
// Set the date and time from Global Positioning System (GPS) broadcast data.
// The return value indicates whether the date/time and UTC parameters are actually
// set Manually (1), Automatically (-1) or Failed (0).
//
// The input arguments represent the following :
// ---------------------------------------------
// w      : The GPS week count since the GPS epoch start at 06-jan-1980 00:00:00 UTC which
//          corresponds to 06-jan-1980 00:00:19 TAI representing a TAI day count of 8040.
//          1 week corresponds to 7 days.
// sow    : The number of seconds elapsed within the GPS week (Seconds Of Week).
// ns     : The remaining fractional number of seconds (in ns) elapsed within the GPS week.
// ps     : The remaining fractional number of nanoseconds (in ps) elapsed within the GPS week.
// utc    : Flag to denote which UTC parameter values "leap" and "dut" (see below) should be used.
//          "M" ==> Manually provided UTC parameters as specified by "leap" and "dut".
//          "A" ==> Automatically provided UTC parameters from the loaded IERS data files.
//                  In this case the specified values of "leap" and "dut" are irrelevant.
//                  For further details see the memberfunction SetUTCparameters().
// leap   : The accumulated number of Leap Seconds corresponding to this date/time.
// dut    : The monitored time difference UT-UTC in seconds.
// icycle : The GPS cycle count (to support previous GPS broadcasts).
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// In older GPS clock implementations the week count was reset after 1024 weeks,
// which was called a "cycle", and the cycle count was increased by 1.
// So, week=2030 and icycle=0 is equivalent with week=1006 and icycle=1. 
//
// The default values are dut=0 and icycle=0.
// However, if <1 sec precision is required, the actual dut value should be provided.
//
// Notes :
// -------
// 1) In case of invalid input arguments the GPS (i.e. TAI) related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
//    Also JD=0 will be set to indicate that something went wrong.
//    The same holds in case utc="A" and no data files have been loaded, or no
//    IERS information for the provided date/time is available.
// 2) For the available IERS information please refer to the docs of LoadUTCparameterFiles().
// 3) In case a date/time setting outside the range of the IERS information is required,
//    the manual setting of "leap" and "dut" has to be invoked.  

 if (w<0 || sow<0 || sow>604800 || ns<0 || ns>999999999 || ps<0 || ps>999 || icycle<0)
 {
  SetJD(0,"N");
  return fUtc;
 }

 // Correct the week count for the cycle number if needed
 if (icycle) w+=icycle*1024;

 Int_t days=8040+w*7;
 sow+=19;
 Int_t daysecs=24*3600;
 Int_t days2=sow/daysecs;
 days+=days2;
 Int_t secs=sow%daysecs;

 SetTAI(days,secs,ns,ps,utc,leap,dut);

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::SetGPS(Int_t w,Int_t dow,Int_t sod,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut,Int_t icycle)
{
// Set the date and time from Global Positioning System (GPS) broadcast data.
// The return value indicates whether the date/time and UTC parameters are actually
// set Manually (1), Automatically (-1) or Failed (0).
//
// The input arguments represent the following :
// ---------------------------------------------
// w      : The GPS week count since the GPS epoch start at 06-jan-1980 00:00:00 UTC which
//          corresponds to 06-jan-1980 00:00:19 TAI representing a TAI day count of 8040.
//          1 week corresponds to 7 days.
// dow    : The day count within the GPS week (Day Of Week).
// sod    : The number of seconds elapsed within the GPS day (Seconds Of Day).
// ns     : The remaining fractional number of seconds (in ns) elapsed within the GPS day.
// ps     : The remaining fractional number of nanoseconds (in ps) elapsed within the GPS day.
// utc    : Flag to denote which UTC parameter values "leap" and "dut" (see below) should be used.
//          "M" ==> Manually provided UTC parameters as specified by "leap" and "dut".
//          "A" ==> Automatically provided UTC parameters from the loaded IERS data files.
//                  In this case the specified values of "leap" and "dut" are irrelevant.
//                  For further details see the memberfunction SetUTCparameters().
// leap   : The accumulated number of Leap Seconds corresponding to this date/time.
// dut    : The monitored time difference UT-UTC in seconds.
// icycle : The GPS cycle count (to support previous GPS broadcasts).
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// In older GPS clock implementations the week count was reset after 1024 weeks,
// which was called a "cycle", and the cycle count was increased by 1.
// So, week=2030 and icycle=0 is equivalent with week=1006 and icycle=1. 
//
// The default values are dut=0 and icycle=0.
// However, if <1 sec precision is required, the actual dut value should be provided.
//
// Notes :
// -------
// 1) In case of invalid input arguments the GPS (i.e. TAI) related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
//    Also JD=0 will be set to indicate that something went wrong.
//    The same holds in case utc="A" and no data files have been loaded, or no
//    IERS information for the provided date/time is available.
// 2) For the available IERS information please refer to the docs of LoadUTCparameterFiles().
// 3) In case a date/time setting outside the range of the IERS information is required,
//    the manual setting of "leap" and "dut" has to be invoked.  

 if (w<0 || dow<0 || dow>7 || sod<0 || sod>86400 || ns<0 || ns>999999999 || ps<0 || ps>999 || icycle<0)
 {
  SetJD(0,"N");
  return fUtc;
 }

 // Correct the week count for the cycle number if needed
 if (icycle) w+=icycle*1024;

 Int_t days=8040+w*7+dow;
 sod+=19;

 SetTAI(days,sod,ns,ps,utc,leap,dut);

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::SetUnixTime(Double_t sec,TString utc,Int_t leap,Double_t dut)
{
// Set the Unix date and time and update the TTimeStamp parameters accordingly (if possible).
// Unix Time is also called POSIX Time or UNIX Epoch Time.
//
// Unix Time is closely releated to UTC and represents the (fractional) elapsed second count
// since the start of the Unix Epoch, in which every Unix Time day contains exactly 86400 seconds.
// This implies that for Unix Time the UTC leap seconds should be ignored as explained below.
// The UNIX Time EPOCH starts at 01-jan-1970 00:00:00 UTC which corresponds to JD=2440587.5
// (i.e. the start of MJD=40587).
// Synchronization with UTC is obtained by continuing the second count when a UTC leap second
// occurs and then the Unix Time count jumps up or down 1 second at the beginning of the new
// day after the UTC leap second has expired.
// In case of a negative UTC leap second (which has not occurred so far) this would result
// in a jump forward of 1 second in Unix Time, introducing a "gap" of 1 second in the timing
// at the start of the new day.
// In case of a positive UTC leap second this results in a jump back of 1 second in Unix Time
// at the start of a new day.
// In this case there exist 2 ambiguous Unix Times over a 1 second time interval,
// namely at the beginning of the introduction of the UTC leap second and at the moment that
// the UTC leap second expires, i.e. the start of the new day.
//
// For accurate timing the user is advised to use one of the other supported time scales. 
//
// Due to a limitation on the "seconds since the EPOCH start" count,
// the latest accessible date/time is 19-jan-2038 02:14:08 UT.
//
// The return value indicates whether the date/time and UTC parameters are actually
// set Manually (1), Automatically (-1) or Failed (0).
//
// Due to computer accuracy the ns precision may be lost.
// For better precision it is advised to use the other date/time Set() facilities instead.
//
// The input arguments represent the following :
// ---------------------------------------------
// sec  : The (fractional) second count elapsed since the start of the Unix Epoch.
// utc  : Flag to denote which UTC parameter values "leap" and "dut" (see below) should be used.
//        "N" ==> No UTC parameters will be stored.
//                The TAI related time recording is disabled and the values of
//                the Leap Seconds and dut=UT-UTC will be set to zero.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//        "M" ==> Manually provided UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatically provided UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// The default values are utc="A", leap=0, dut=0.
// However, if <1 sec precision is required, the actual dut value should be provided.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
// 2) In case utc="A" and no data files have been loaded, or no information is available,
//    the utc="N" mode will be invoked.

 // Determine the fractional day count since the start of the Unix Epoch
 Double_t tday=sec/86400.;

 Int_t days=0;
 Int_t s=0;
 Int_t ns=0;
 Convert(tday,days,s,ns);

 // Determine the remaining elapsed picoseconds
 Int_t iword=int(sec);
 sec=sec-double(iword);
 sec*=1e9;
 iword=int(sec);
 sec=sec-double(iword);
 Int_t ps=int(sec*1000.);
 
 SetMJD(40587,0,0,0,utc,leap,dut); // Start of the Unix Epoch
 Add(days,s,ns,ps);      // Add the elapsed time
 if (fUtc) AddSec(fDut); // Correct for dUT=UT1-UTC

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetNs(Int_t ns)
{
// Set the remaining fractional number of seconds in nanosecond precision.
// Notes :
// -------
// 1) The allowed range for the argument "ns" is [0,999999999].
//    Outside that range no action is performed.
// 2) The ns fraction can also be entered directly via SetMJD() etc...
// 3) For additional accuracy see SetPs().

 if (ns>=0 && ns<=999999999) fJns=ns; 
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::GetNs() const
{
// Provide the remaining fractional number of seconds in nanosecond precision.
// This function allows trigger/timing analysis for (astro)particle physics
// experiments.
// Note : For additional accuracy see also GetPs().

 return fJns; 
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetPs(Int_t ps)
{
// Set the remaining fractional number of nanoseconds in picoseconds.
// Notes :
// -------
// 1) The allowed range for the argument "ps" is [0,999].
//    Outside that range no action is performed.
// 2) The ps fraction can also be entered directly via SetMJD() etc...

 if (ps>=0 && ps<=999) fJps=ps; 
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::GetPs() const
{
// Provide remaining fractional number of nanoseconds in picoseconds.
// This function allows time of flight analysis for particle physics
// experiments.

 return fJps; 
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::GetUTCparameters(Int_t& leap,Double_t& dut) const
{
// Provide the values of the UTC parameters (i.e. Leap Seconds and dut=UT1-UTC).
//
// The return value indicates whether the UTC parameters are actually set or not,
// and has the following conventions :
// 
//  1 --> The UTC parameters were set manually and used to determine UT1 from UTC.
//  0 --> No UTC parameters were provided and the stored reference time is UTC.
// -1 --> The UTC parameters were retrieved from the IERS data files and used to determine UT1 from UTC.
// -2 --> The UTC parameters were retrieved from the IERS data files and used to determine all TAI times from UT1.
// -3 --> No UTC parameters were provided and the stored reference time is UT1.
//
// Notes :
// -------
// 1) For the return values 0 and -3 only the UTC (0) or UT1 (-3) time recording is available,
//    depending on the input argument "utc", as outlined above.
// 2) A return value of -2 or -3 indicates that the time which was entered via SetUT()
//    or via one of the Julian Date facilities was actually UT1.

 leap=fLeap;
 dut=fDut;

 return fUtc; 
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::GetUTCparameters(Int_t mjd,Int_t& leap,Double_t& dut) const
{
// Provide the values of the UTC parameters (i.e. Leap Seconds and dut=UT1-UTC)
// from the daily IERS data for the specified MJD.
// The return value indicates the entry (0=first) in the IERS data TTree at which
// the corresponding data was stored.
// In case the corresponding data could not be found, the return value is -1
// and "leap" and "dut" are set to 0.

 leap=0;
 dut=0;

 if (!fUTCdata) return -1;

 Int_t nen=fUTCdata->GetEntries();

 if (!nen) return -1;

 Int_t dbmjd=0;
 Int_t dbleap=0;
 Double_t dbdut=0;

 fUTCdata->SetBranchAddress("mjd",&dbmjd);
 fUTCdata->SetBranchAddress("lsec",&dbleap);
 fUTCdata->SetBranchAddress("dut",&dbdut);

 // Data of the first entry
 fUTCdata->GetEntry(0);
 Int_t ien=mjd-dbmjd;

 if (ien<0 || ien>=nen) return -1; // Specified mjd not in range of database

 fUTCdata->GetEntry(ien);
 if (dbmjd==mjd) // Specified mjd is found in database
 {
  leap=dbleap;
  dut=dbdut;
 }
 else
 {
  ien=-1;
  leap=0;
  dut=0;
 }

 return ien;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::SetUTCparameters(TString utc,Int_t leap,Double_t dut)
{
// Internal memberfunction to set the UTC parameters (i.e. Leap Seconds and dut=UT1-UTC).
// The TAI time recording is updated accordingly, depending on the input argument "utc", as outlined below.
//
// The return value indicates whether the UTC parameters are actually set or not,
// and has the following conventions :
// 
//  1 --> The UTC parameters were set manually and used to determine UT1 from UTC.
//  0 --> No UTC parameters were provided and the stored reference time is UTC.
// -1 --> The UTC parameters were retrieved from the IERS data files and used to determine UT1 from UTC.
// -2 --> The UTC parameters were retrieved from the IERS data files and used to determine all TAI times from UT1.
// -3 --> No UTC parameters were provided and the stored reference time is UT1.
//        
// The input arguments represent the following :
// ---------------------------------------------
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified c.q. stored represents UTC, which serves as reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The time that was specified c.q. stored represents UTC, and the provided UTC parameters
//                may be used by the calling function to determine UT1, which may then become the reference time.  
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified c.q. stored represents UTC, and the retrieved UTC parameters
//                may be used by the calling function to determine UT1, which may then become the reference time.  
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunction LoadUTCparameterFiles().
//        "U" ==> The time that was specified c.q. stored represents UT1, which will remain the reference time.
//                This mode is equivalent to utc="A" apart from the fact that UT1 will not be modified.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 2) For the return values 0 and -3 only the UTC (0) or UT1 (-3) time recording is available,
//    depending on the input argument "utc", as outlined above.
// 3) A return value of -2 or -3 indicates that the time which was entered via SetUT()
//    or via one of the Julian Date facilities was actually UT1.

 fUtc=0;
 if (utc=="U") fUtc=-3;
 fLeap=0;
 fDut=0;

 Int_t ibad=0;

 if (utc!="N" && utc!="M" && utc!="A" && utc!="U") ibad=1;

 if (utc=="N" || ((utc=="A" || utc=="U") && !fUTCdata)) ibad=1;

 if (utc=="M" && fabs(dut)>0.9) ibad=1;

 if (ibad)
 {
  FillTAI();
  return fUtc;
 }

 // From here only utc="M", utc="A" or utc="U"

 if (utc=="M")
 {
  fUtc=1;
  fLeap=leap;
  fDut=dut;

  FillTAI();
  return fUtc;
 }

 // Automatic setting of the UTC parameters from the loaded data files
 Int_t nen=fUTCdata->GetEntries();

 if (nen<=0) // No entries in the IERS data TTree
 {
  FillTAI();
  return fUtc;
 }

 Int_t mjd=0;

 fUTCdata->SetBranchAddress("mjd",&mjd);
 fUTCdata->SetBranchAddress("lsec",&leap);
 fUTCdata->SetBranchAddress("dut",&dut);

 // Get the starting mjd of the IERS daily data
 // and determine the entry for the current MJD info
 fUTCdata->GetEntry(0);
 Int_t ien=fMJD-mjd;
 if (ien>=0 && ien<nen)
 {
  fUTCdata->GetEntry(ien);
  if (mjd==fMJD)
  {
   fUtc=-1;
   if (utc=="U") fUtc=-2;
   fLeap=leap;
   fDut=dut;
  }
 }

 FillTAI();

 return fUtc;
}
///////////////////////////////////////////////////////////////////////////
TTree* NcTimestamp::LoadUTCparameterFiles(TString leapfile,TString dutfile)
{
// Load the IERS data for automatic setting of Leap Seconds and dUT=UT1-UTC.
//
// For the current timestamp the corresponding Leap seconds and dUT are set.
// If different settings are required, please invoke SetUTCparameters() after
// the invokation of this memberfunction. 
//
// Input arguments :
// -----------------
// leapfile : The name of a copy of the (most recent) IERS ascii file
//            https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//            This file contains the archival list of accumulated Leap Seconds.
// dutfile  : The name of a copy of the (most recent) IERS the ascii file
//            https://hpiers.obspm.fr/iers/series/opa/eopc04
//            This file contains the archival list of the daily dUT=UT-UTC monitoring.
//
// Environment variables may be used as $(...) in the filennames for convenience.
// For example "$(HOME)/my-iers/leap.txt".
//
// The default file names are :
// ----------------------------
// leapfile="$(NCFS)/IERS/leap.txt"
// dutfile="$(NCFS)/IERS/dut.txt"
//
// The corresponding daily values of the accumulated Leap Seconds and dUT=UT1-UTC
// are stored in an internal ROOT TTree.
// The return argument provides a pointer to the corresponding TTree to enable
// the user to investigate or store the corresponding data.
//
// The data stored in each TTree entry are the following :
// Int_t mjd    : The corresponding MJD of the entry.
// Int_t lsec   : The accumulated leap seconds of the corresponding mjd.
// Double_t dut : The dUT=UT1-UTC value (in seconds) of the corresponding mjd.
//
// Accurate daily monitoring dUT=UT1-UTC data, with about 10 microsecond precision,
// is available since MJD=37665 (01-jan-1962 00:00:00).
// Leap Seconds were introduced into UTC on MJD=41317 (01-jan-1972 00:00:00).
// For the period MJD=37665 until MJD=41317 the accumulated Leap Second count is set to 0. 
//
// Note : In case of an error or inconsistency, no ROOT TTree will be created
//        and the returned pointer will be zero.

 // Expand the input file pathnames 
 leapfile=gSystem->ExpandPathName(leapfile.Data());
 dutfile=gSystem->ExpandPathName(dutfile.Data());
  
 if (fUTCdata)
 {
  delete fUTCdata;
  fUTCdata=0;
 }

 // The Leap Second input data file 
 ifstream fleap;
 fleap.clear();
 fleap.open(leapfile.Data());
 if (!fleap.good())
 {
  cout << " *NcTimestamp::LoadUTCparameterFiles* Data file for Leap Seconds not found ***" << endl;
  cout << " File name provided was : " << leapfile.Data() << endl;
  return 0;
 }

 // The dUT=UT1-UTC input data file 
 ifstream fdut;
 fdut.clear();
 fdut.open(dutfile.Data());
 if (!fdut.good())
 {
  cout << " *NcTimestamp::LoadUTCparameterFiles* Data file for dUT=UT-UTC not found ***" << endl;
  cout << " File name provided was : " << dutfile.Data() << endl;
  return 0;
 }

 // Determine the number of characters in the Leap Second input file
 // to reserve sufficient array storage of all Leap Second entries
 fleap.seekg(0,fleap.end); // Position at end of file
 Int_t ndim=fleap.tellg();

 // The storage arrays for the Leap second data
 Int_t* lmjd=new Int_t[ndim];
 Int_t* leap=new Int_t[ndim];

 ///////////////////////////////
 // Read the Leap Second data //
 ///////////////////////////////

 fleap.seekg(0); // Position at begin of file

 // Read title lines until the first data line is found
 string line;
 Int_t i=0;
 while (getline(fleap,line))
 {
  if (line.find("1972")!=line.npos) break;
  i++;
 }

 // Go to the beginning of the file and skip the title lines preceding the data lines
 fleap.seekg(0);
 for (Int_t j=0; j<i; j++)
 {
  getline(fleap,line);
 }
 
 // Read the data
 Float_t rmjd=0;
 Int_t lsec=0;
 Float_t x; // Dummy variable for skipping non-requested data columns
 i=0;
 while (fleap >> rmjd >> x >> x >> x >> lsec)
 {
  lmjd[i]=int(rmjd);
  leap[i]=lsec;
  i++;
 }

 // The number of actual Leap Second entries
 Int_t nleap=i;

 //////////////////////////
 // Read the dUT=UT1-UTC //
 //////////////////////////

 fdut.seekg(0); // Position at begin of file

 // Read title lines until the first data line is found
 i=0;
 while (getline(fdut,line))
 {
  if (line.find("1962")!=line.npos) break;
  i++;
 }

 // Go to the beginning of the file and skip the title lines preceding the data lines
 fdut.seekg(0);
 for (Int_t j=0; j<i; j++)
 {
  getline(fdut,line);
 }
 
 // Read the dUT daily data and fill the TTree structure
 Int_t mjd=0;
 Double_t dut=0;

 // The produced output structure
 fUTCdata=new TTree("T","Daily UTC leap second and dUT=UT-UTC parameter data");

 // The output variables for the Tree
 fUTCdata->Branch("mjd",&mjd,"mjd/I");
 fUTCdata->Branch("lsec",&lsec,"lsec/I");
 fUTCdata->Branch("dut",&dut,"dut/D");

 while (fdut >> x >> x >> x >> mjd >> x >> x >> dut >> x >> x >> x >> x >> x >> x >> x >> x >> x)
 {
  lsec=0;
  // Retrieve the corresponding Leap Second info
  for (Int_t j=nleap-1; j>=0; j--)
  {
   if (mjd>=lmjd[j])
   {
    lsec=leap[j];
    break;
   }
  }
  fUTCdata->Fill();
 }

 delete[] lmjd;
 delete[] leap; 

 // Retrieve the UTC parameters for the current timestamp
 if (!fUtc) // Reference time is UTC
 {
  SetUTCparameters("A",0,0);
  AddSec(fDut);
 }
 else // Reference time is UT1
 {
  SetUTCparameters("U",0,0);
 }

 return fUTCdata;
}
///////////////////////////////////////////////////////////////////////////
TTree* NcTimestamp::GetIERSdatabase() const
{
// Provide the pointer to the internal IERS database TTree.
// In case no database TTree exists the pointer value 0 will be returned.
// For details please refer to the member function LoadUTCparameterFiles().

 return fUTCdata;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::Add(Int_t d,Int_t s,Int_t ns,Int_t ps)
{
// Add (or subtract) a certain time difference to the current timestamp.
// Subtraction can be achieved by entering negative values as input arguments.
//
// The UTC parameters (i.e. Leap Seconds and dut=UT1-UTC) will be updated accordingly.
//
// The time difference is entered via the following input arguments :
//
// d  : elapsed number of days
// s  : (remaining) elapsed number of seconds
// ns : (remaining) elapsed number of nanoseconds
// ps : (remaining) elapsed number of picoseconds
//
// The specified d, s, ns and ps values will be used in an additive
// way to determine the time difference.
// So, specification of d=1, s=100, ns=0, ps=0 will result in the
// same time difference addition as d=0, s=24*3600+100, ns=0, ps=0.
// However, by making use of the latter the user should take care
// of possible integer overflow problems in the input arguments,
// which obviously will provide incorrect results. 
//
// Note : ps=0 is the default value.

 AddCalc(d,s,ns,ps);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::Add(Double_t hours)
{
// Add (or subtract) a certain time difference to the current timestamp.
// The time difference is specified as a (fractional) number of hours.
// Subtraction can be achieved by entering a negative value as input argument.
//
// Note : For small time differences a better accuracy may be obtained by using
//        the memberfunction AddSec() or Add(d,s,ns,ps).
//
// The UTC parameters (i.e. Leap Seconds and dut=UT1-UTC) will be updated accordingly.

 AddCalc(hours);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::AddSec(Double_t seconds)
{
// Add (or subtract) a certain time difference to the current timestamp.
// The time difference is specified as a (fractional) number of seconds.
// Subtraction can be achieved by entering a negative value as input argument.
//
// Note : For very small time differences a better accuracy may be obtained by
//        using the memberfunction Add(d,s,ns,ps).
//
// The UTC parameters (i.e. Leap Seconds and dut=UT1-UTC) will be updated accordingly.

 AddSecCalc(seconds);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::AddCalc(Int_t d,Int_t s,Int_t ns,Int_t ps,Bool_t utcpar)
{
// Internal memberfunction to actually add (or subtract) a certain time difference
// to the current timestamp and (optionally) update also the UTC parameters.
// Subtraction can be achieved by entering negative values as input arguments.
//
// The time difference is entered via the following input arguments :
//
// d  : elapsed number of days
// s  : (remaining) elapsed number of seconds
// ns : (remaining) elapsed number of nanoseconds
// ps : (remaining) elapsed number of picoseconds
//
// The specified d, s, ns and ps values will be used in an additive
// way to determine the time difference.
// So, specification of d=1, s=100, ns=0, ps=0 will result in the
// same time difference addition as d=0, s=24*3600+100, ns=0, ps=0.
// However, by making use of the latter the user should take care
// of possible integer overflow problems in the input arguments,
// which obviously will provide incorrect results. 
//
// The argument "utcpar" controls whether the UTC parameters are updated (kTRUE) or not (kFALSE)
// via invokation of SetUTCparameters() after the addition (or subtraction) of the time difference.
// In general utcpar=kTRUE should obviously be used, but utcpar=kFALSE allows to
// add (or subtract) time differences in FillTAI() without creating an infinite loop.
//
// The default values are ps=0 and utcpar=kTRUE.

 Int_t days=0;
 Int_t secs=0;
 Int_t nsec=0;
 // Use Get functions to ensure updated Julian parameters. 
 GetMJD(days,secs,nsec);
 Int_t psec=GetPs();

 psec+=ps%1000;
 nsec+=ps/1000;
 while (psec<0)
 {
  nsec-=1;
  psec+=1000;
 }
 while (psec>999)
 {
  nsec+=1;
  psec-=1000;
 }

 nsec+=ns%1000000000;
 secs+=ns/1000000000;
 while (nsec<0)
 {
  secs-=1;
  nsec+=1000000000;
 }
 while (nsec>999999999)
 {
  secs+=1;
  nsec-=1000000000;
 }

 secs+=s%(24*3600);
 days+=s/(24*3600);
 while (secs<0)
 {
  days-=1;
  secs+=24*3600;
 }
 while (secs>=24*3600)
 {
  days+=1;
  secs-=24*3600;
 }

 days+=d;

 TString utc="N";
 if (fUtc==1) utc="M";
 if (fUtc==-1) utc="A";
 if (fUtc==-2) utc="U";
 if (fUtc==-3) utc="U";
 SetMJD(days,secs,nsec,psec,utc,fLeap,fDut);

 if (utcpar) SetUTCparameters(utc,fLeap,fDut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::AddCalc(Double_t hours,Bool_t utcpar)
{
// Internal memberfunction to actually add (or subtract) a certain time difference
// to the current timestamp and (optionally) update also the UTC parameters.
// The time difference is specified as a (fractional) number of hours.
// Subtraction can be achieved by entering a negative value as input argument.
//
// Note : For small time differences a better accuracy may be obtained by using
//        the memberfunction AddSec() or Add(d,s,ns,ps).
//
// The argument "utcpar" controls whether the UTC parameters are updated (kTRUE) or not (kFALSE)
// via invokation of SetUTCparameters() after the addition (or subtraction) of the time difference.
// In general utcpar=kTRUE should obviously be used, but utcpar=kFALSE allows to
// add (or subtract) time differences in FillTAI() without creating an infinite loop.
//
// The default value is utcpar=kTRUE.

 Int_t d,s,ns,ps;
 Double_t h=fabs(hours);
 d=int(h/24.);
 h-=double(d)*24.;
 h*=3600.;
 s=int(h);
 h-=double(s);
 h*=1.e9;
 ns=int(h);
 h-=double(ns);
 ps=int(h*1000.);
 if (hours>0) AddCalc(d,s,ns,ps,utcpar);
 if (hours<0) AddCalc(-d,-s,-ns,-ps,utcpar);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::AddSecCalc(Double_t seconds,Bool_t utcpar)
{
// Internal memberfunction to actually add (or subtract) a certain time difference
// to the current timestamp and (optionally) update also the UTC parameters.
// The time difference is specified as a (fractional) number of seconds.
// Subtraction can be achieved by entering a negative value as input argument.
//
// Note : For very small time differences a better accuracy may be obtained by
//        using the memberfunction Add(d,s,ns,ps).
//
// The argument "utcpar" controls whether the UTC parameters are updated (kTRUE) or not (kFALSE)
// via invokation of SetUTCparameters() after the addition (or subtraction) of the time difference.
// In general utcpar=kTRUE should obviously be used, but utcpar=kFALSE allows to
// add (or subtract) time differences in FillTAI() without creating an infinite loop.
//
// The default value is utcpar=kTRUE.

 Int_t s,ns,ps;
 Double_t a=fabs(seconds);
 s=int(a);
 a-=double(s);
 a*=1.e9;
 ns=int(a);
 a-=double(ns);
 ps=int(a*1000.);
 if (seconds>0) AddCalc(0,s,ns,ps,utcpar);
 if (seconds<0) AddCalc(0,-s,-ns,-ps,utcpar);
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::GetDifference(NcTimestamp* t,Int_t& d,Int_t& s,Int_t& ns,Int_t& ps,TString type)
{
// Provide the time difference w.r.t the NcTimestamp specified on the input.
// This memberfunction supports both very small (i.e. time of flight analysis
// for particle physics experiments) and very long (i.e. investigation of
// astrophysical phenomena) time intervals.
//
// The input argument "type" allows to specify the time recording system
// type : "UT"  ==> Time difference is provided according to the UT day counting
//        "TAI" ==> Time difference is provided according to the TAI day counting
//
// Note :
// ------
// In case type="TAI" and the TAI recording was not activated for one (or both) of the timestamps,
// the time difference will be set to 0.
// The UT recording is always available, and in case of a mix of UT1 and UTC as reference times
// for the two timestamps, the time difference will be determined on basis of UTC.
// Obviously, the time difference based on UTC will be less accurate than the time difference
// determined on basis of UT1 for both timestamps.
//
// The default value is type="UT", which is to be used for accurate astrophysical observations.
//
// Full picosecond accuracy is automatically obtained by using the same "type" as with which
// both the timestamps have been set (i.e. SetTAI() or SetGPS() and using type="TAI" 
// or via the SetUT() related facilities and using type="UT").
// Otherwise, the time difference accuracy amounts to the one of dUT=UT-UTC.
// For timestamps that have the same UTC parameters (i.e. Leap Seconds and dUT=UT-UTC),
// both the "UT" and "TAI" systems provide the same time difference (within the dUT accuracy).
//
// The time difference is returned via the following output arguments :
// d  : elapsed number of days
// s  : remaining elapsed number of seconds
// ns : remaining elapsed number of nanoseconds
// ps : remaining elapsed number of picoseconds
//
// Note :
// ------
// The calculated time difference is the absolute value of the time interval.
// This implies that the values of d, s, ns and ps are always positive or zero.
//
// The integer return argument indicates whether the NcTimestamp specified
// on the input argument occurred earlier (-1), simultaneously (0) or later (1).
//
// In case of inconsistent input the time difference and the return argument will be set to 0.

 d=0;
 s=0;
 ns=0;
 ps=0;

 if (!t || (type!="UT" && type!="TAI")) return 0;

 Int_t tUtc=t->fUtc;
 if (type=="TAI" && (!fUtc || !tUtc || fUtc==-3 || tUtc==-3)) return 0;

 Int_t d1=0;
 Int_t s1=0;
 Int_t ns1=0;
 Int_t ps1=0;

 Int_t d2=0;
 Int_t s2=0;
 Int_t ns2=0;
 Int_t ps2=0;

 NcTimestamp ts1(*t);
 NcTimestamp ts2(*this);

 // Check for a mixed use of UT1 and UTC
 if (type=="UT")
 {
  Double_t dut=0;
  if (!fUtc && tUtc) // This timestamp is in UTC and the other in UT1
  {
   dut=t->fDut;
   ts1.AddSec(-dut);
  }
  if (fUtc && !tUtc) // This timestamp is in UT1 and the other in UTC
  {
   ts2.AddSec(-fDut);
  }
 }

 // Use Get functions to ensure updated Julian and TAI parameters. 
 if (type=="UT")
 {
  ts1.GetMJD(d1,s1,ns1);
  ps1=ts1.GetPs();
  ts2.GetMJD(d2,s2,ns2);
  ps2=ts2.GetPs();
 }
 if (type=="TAI")
 {
  t->GetTAI(d1,s1,ns1,ps1);
  GetTAI(d2,s2,ns2,ps2);
 }

 d=d1-d2;
 s=s1-s2;
 ns=ns1-ns2;
 ps=ps1-ps2;

 if (!d && !s && !ns && !ps) return 0;

 Int_t sign=0;

 if (d>0) sign=1;
 if (d<0) sign=-1;

 if (!sign && s>0) sign=1;
 if (!sign && s<0) sign=-1;

 if (!sign && ns>0) sign=1; 
 if (!sign && ns<0) sign=-1;

 if (!sign && ps>0) sign=1; 
 if (!sign && ps<0) sign=-1;

 // In case the input stamp was earlier, take the reverse difference
 // to simplify the algebra.
 if (sign<0)
 {
  d=-d;
  s=-s;
  ns=-ns;
  ps=-ps;
 }

 // Here we always have a positive time difference
 // and can now unambiguously correct for other negative values.
 if (ps<0)
 {
  ns-=1;
  ps+=1000;
 }

 if (ns<0)
 {
  s-=1;
  ns+=1000000000;
 }

 if (s<0)
 {
  d-=1;
  s+=24*3600;
 }

 return sign;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcTimestamp::GetDifference(NcTimestamp& t,Int_t& d,Int_t& s,Int_t& ns,Int_t& ps,TString type)
{
// Provide the time difference w.r.t the NcTimestamp specified on the input.
// This memberfunction supports both very small (i.e. time of flight analysis
// for particle physics experiments) and very long (i.e. investigation of
// astrophysical phenomena) time intervals.
//
// The input argument "type" allows to specify the time recording system
// type : "UT"  ==> Time difference is provided according to the UT day counting
//        "TAI" ==> Time difference is provided according to the TAI day counting
//
// Note :
// ------
// In case type="TAI" and the TAI recording was not activated for one (or both) of the timestamps,
// the time difference will be set to 0.
// The UT recording is always available, and in case of a mix of UT1 and UTC as reference times
// for the two timestamps, the time difference will be determined on basis of UTC.
// Obviously, the time difference based on UTC will be less accurate than the time difference
// determined on basis of UT1 for both timestamps.
//
// The default value is type="UT", which is to be used for accurate astrophysical observations.
//
// Full picosecond accuracy is automatically obtained by using the same "type" as with which
// both the timestamps have been set (i.e. SetTAI() or SetGPS() and using type="TAI" 
// or via the SetUT() related facilities and using type="UT").
// Otherwise, the time difference accuracy amounts to the one of dUT=UT-UTC.
// For timestamps that have the same UTC parameters (i.e. Leap Seconds and dUT=UT-UTC),
// both the "UT" and "TAI" systems provide the same time difference (within the dUT accuracy).
//
// The time difference is returned via the following output arguments :
// d  : elapsed number of days
// s  : remaining elapsed number of seconds
// ns : remaining elapsed number of nanoseconds
// ps : remaining elapsed number of picoseconds
//
// Note :
// ------
// The calculated time difference is the absolute value of the time interval.
// This implies that the values of d, s, ns and ps are always positive or zero.
//
// The integer return argument indicates whether the NcTimestamp specified
// on the input argument occurred earlier (-1), simultaneously (0) or later (1).
//
// In case of inconsistent input the time difference and the return argument will be set to 0.

 return GetDifference(&t,d,s,ns,ps,type);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetDifference(NcTimestamp* t,TString u,Int_t mode,TString type)
{
// Provide the time difference w.r.t the NcTimestamp specified on the input
// in the units as specified by the TString "u" argument.
// A positive return value means that the NcTimestamp specified on the input
// argument occurred later, whereas a negative return value indicates an
// earlier occurence. 
//
// The input argument "type" allows to specify the time recording system
// type : "UT"  ==> Time difference is provided according to the UT day counting
//        "TAI" ==> Time difference is provided according to the TAI day counting
//
// Note :
// ------
// In case type="TAI" and the TAI recording was not activated for one (or both) of the timestamps,
// the time difference will be set to 0.
// The UT recording is always available, and in case of a mix of UT1 and UTC as reference times
// for the two timestamps, the time difference will be determined on basis of UTC.
// Obviously, the time difference based on UTC will be less accurate than the time difference
// determined on basis of UT1 for both timestamps.
//
// The default value is type="UT", which is to be used for accurate astrophysical observations.
//
// Full picosecond accuracy is automatically obtained by using the same "type" as with which
// both the timestamps have been set (i.e. SetTAI() or SetGPS() and using type="TAI" 
// or via the SetUT() related facilities and using type="UT").
// Otherwise, the time difference accuracy amounts to the one of dUT=UT-UTC.
// For timestamps that have the same UTC parameters (i.e. Leap Seconds and dUT=UT-UTC),
// both the "UT" and "TAI" systems provide the same time difference (within the dUT accuracy).
//
// The units may be specified as :
// u = "d"  ==> Time difference returned as (fractional) day count
//     "s"  ==> Time difference returned as (fractional) second count
//     "ns" ==> Time difference returned as (fractional) nanosecond count
//     "ps" ==> Time difference returned as picosecond count
//
// It may be clear that for a time difference of several days, the picosecond
// and even the nanosecond accuracy may be lost.
// To cope with this, the "mode" argument has been introduced to allow 
// timestamp comparison on only the specified units.
//
// The following operation modes are supported :
// mode = 1 : Full time difference is returned in specified units
//        2 : Time difference is returned in specified units by
//            neglecting the elapsed time for the larger units than the
//            ones specified.
//        3 : Time difference is returned in specified units by only
//            comparing the timestamps on the level of the specified units.
//
// Example :
// ---------
// NcTimestamp t1; // Corresponding to days=3, secs=501, ns=31, ps=7 
// NcTimestamp t2; // Corresponding to days=5, secs=535, ns=12, ps=15
//
// The statement : Double_t val=t1.GetDifference(t2,....)
// would return the following values :
// val=(2*24*3600)+34-(19*1e-9)+(8*1e-12) for u="s" and mode=1
// val=34-(19*1e-9)+(8*1e-12)             for u="s" and mode=2
// val=34                                 for u="s" and mode=3
// val=-19                                for u="ns" and mode=3
//
// The default is mode=1.
//
// In case of inconsistent input the time difference will be set to 0.

 if (!t || mode<1 || mode>3 || (type!="UT" && type!="TAI")) return 0;

 if (u!="d" && u!="s" && u!="ns" && u!="ps") return 0;


 Int_t tUtc=t->fUtc;
 if (type=="TAI" && (!fUtc || !tUtc || fUtc==-3 || tUtc==-3)) return 0;

 Double_t dt=0;

 Int_t d1=0;
 Int_t s1=0;
 Int_t ns1=0;
 Int_t ps1=0;

 Int_t d2=0;
 Int_t s2=0;
 Int_t ns2=0;
 Int_t ps2=0;

 NcTimestamp ts1(*t);
 NcTimestamp ts2(*this);

 // Check for a mixed use of UT1 and UTC
 if (type=="UT")
 {
  Double_t dut=0;
  if (!fUtc && tUtc) // This timestamp is in UTC and the other in UT1
  {
   dut=t->fDut;
   ts1.AddSec(-dut);
  }
  if (fUtc && !tUtc) // This timestamp is in UT1 and the other in UTC
  {
   ts2.AddSec(-fDut);
  }
 }

 // Use Get functions to ensure updated Julian and TAI parameters. 
 if (type=="UT")
 {
  ts1.GetMJD(d1,s1,ns1);
  ps1=ts1.GetPs();
  ts2.GetMJD(d2,s2,ns2);
  ps2=ts2.GetPs();
 }
 if (type=="TAI")
 {
  t->GetTAI(d1,s1,ns1,ps1);
  GetTAI(d2,s2,ns2,ps2);
 }

 Int_t dd=d1-d2;
 Int_t ds=s1-s2;
 Int_t dns=ns1-ns2;
 Int_t dps=ps1-ps2;

 // Time difference for the specified units only
 if (mode==3)
 {
  if (u=="d") dt=dd;
  if (u=="s") dt=ds;
  if (u=="ns") dt=dns;
  if (u=="ps") dt=dps;
  return dt;
 }

 // Suppress elapsed time for the larger units than specified
 if (mode==2)
 {
  if (u=="s") dd=0;
  if (u=="ns")
  {
   dd=0;
   ds=0;
  }
  if (u=="ps")
  {
   dd=0;
   ds=0;
   dns=0;
  }
 }

 // Compute the time difference as requested 
 if (u=="s" || u=="d")
 {
  // The time difference in (fractional) seconds
  dt=double(dd*24*3600+ds)+(double(dns)*1e-9)+(double(dps)*1e-12);
  if (u=="d") dt=dt/double(24*3600);
 }
 if (u=="ns") dt=(double(dd*24*3600+ds)*1e9)+double(dns)+(double(dps)*1e-3);
 if (u=="ps") dt=(double(dd*24*3600+ds)*1e12)+(double(dns)*1e3)+double(dps);

 return dt;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetDifference(NcTimestamp& t,TString u,Int_t mode,TString type)
{
// Provide the time difference w.r.t the NcTimestamp specified on the input
// in the units as specified by the TString "u" argument.
// A positive return value means that the NcTimestamp specified on the input
// argument occurred later, whereas a negative return value indicates an
// earlier occurence. 
//
// The input argument "type" allows to specify the time recording system
// type : "UT"  ==> Time difference is provided according to the UT day counting
//        "TAI" ==> Time difference is provided according to the TAI day counting
//
// Note :
// ------
// In case type="TAI" and the TAI recording was not activated for one (or both) of the timestamps,
// the time difference will be set to 0.
// The UT recording is always available, and in case of a mix of UT1 and UTC as reference times
// for the two timestamps, the time difference will be determined on basis of UTC.
// Obviously, the time difference based on UTC will be less accurate than the time difference
// determined on basis of UT1 for both timestamps.
//
// The default value is type="UT", which is to be used for accurate astrophysical observations.
//
// Full picosecond accuracy is automatically obtained by using the same "type" as with which
// both the timestamps have been set (i.e. SetTAI() or SetGPS() and using type="TAI" 
// or via the SetUT() related facilities and using type="UT").
// Otherwise, the time difference accuracy amounts to the one of dUT=UT-UTC.
// For timestamps that have the same UTC parameters (i.e. Leap Seconds and dUT=UT-UTC),
// both the "UT" and "TAI" systems provide the same time difference (within the dUT accuracy).
//  
// The units may be specified as :
// u = "d"  ==> Time difference returned as (fractional) day count
//     "s"  ==> Time difference returned as (fractional) second count
//     "ns" ==> Time difference returned as (fractional) nanosecond count
//     "ps" ==> Time difference returned as picosecond count
//
// It may be clear that for a time difference of several days, the picosecond
// and even the nanosecond accuracy may be lost.
// To cope with this, the "mode" argument has been introduced to allow 
// timestamp comparison on only the specified units.
//
// The following operation modes are supported :
// mode = 1 : Full time difference is returned in specified units
//        2 : Time difference is returned in specified units by
//            neglecting the elapsed time for the larger units than the
//            ones specified.
//        3 : Time difference is returned in specified units by only
//            comparing the timestamps on the level of the specified units.
//
// Example :
// ---------
// NcTimestamp t1; // Corresponding to days=3, secs=501, ns=31, ps=7 
// NcTimestamp t2; // Corresponding to days=5, secs=535, ns=12, ps=15
//
// The statement : Double_t val=t1.GetDifference(t2,....)
// would return the following values :
// val=(2*24*3600)+34-(19*1e-9)+(8*1e-12) for u="s" and mode=1
// val=34-(19*1e-9)+(8*1e-12)             for u="s" and mode=2
// val=34                                 for u="s" and mode=3
// val=-19                                for u="ns" and mode=3
//
// The default is mode=1.
//
// In case of inconsistent input the time difference will be set to 0.

 return GetDifference(&t,u,mode,type);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetUT(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the UT date and time
// in the Gregorian calendar as specified by the input arguments.
// Note that UT may represent either UT1 or UTC as specified by the
// arguments "utc", "leap" and "dut", as outlined below.
//
// This facility is exact upto picosecond precision and as such is
// for scientific observations preferable above the corresponding
// Set function(s) of TTimestamp.
// The latter has a random spread in the sub-second part, which
// might be of use in generating distinguishable timestamps while
// still keeping second precision.
//
// The input arguments represent the following :
// y    : year in UT (e.g. 1952, 2003 etc...)
// m    : month in UT (1=jan  2=feb etc...)
// d    : day in UT (1-31)
// hh   : elapsed hours in UT (0-23) 
// mm   : elapsed minutes in UT (0-59)
// ss   : elapsed seconds in UT (0-59)
// ns   : remaining fractional elapsed second of UT in nanosecond
// ps   : remaining fractional elapsed nanosecond of UT in picosecond
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, which will serve as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The time that was specified represents UTC, and the provided UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.  
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, and the retrieved UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The time that was specified represents UT1, which will become the reference time.
//                This mode is equivalent to utc="A" apart from the fact that UT1 will not be modified.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid date and/or time arguments, JD=0 is set to indicate that something went wrong.
// 2) In case of invalid "utc" input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 3) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are ns=0, ps=0, utc="A", leap=0 and dut=0.
//
// This facility first determines the elapsed days, seconds etc...
// since the beginning of the specified UT year on basis of the
// input arguments. Subsequently it invokes the SetUT memberfunction
// for the elapsed timespan.
// As such this facility is valid for all AD dates in the Gregorian
// calendar with picosecond precision.

 if (d<1 || d>31 || m<1 || m>12 || hh<0 || hh>23 || mm<0 || mm>59 || ss<0 || ss>59 || ns<0 || ns>999999999 || ps<0 || ps>999)
 {
  cout << " *NcTimestamp::SetUT* Incompatible argument(s) Day=" << d << " Month=" << m << " Year=" << y
       << " hour=" << hh << " min=" << mm << " sec=" << ss << " ns=" << ns << " ps=" << ps << endl;
  cout << " ==> TAI related time recording is disabled and JD=0 has been set." << endl;
  SetJD(0,"N");
  return;
 }

 Int_t day=GetDayOfYear(d,m,y);
 Int_t secs=hh*3600+mm*60+ss;
 SetUT(y,day-1,secs,ns,ps,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetUT(Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Double_t s,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the UT date and time
// in the Gregorian calendar as specified by the input arguments.
// Note that UT may represent either UT1 or UTC as specified by the
// arguments "utc", "leap" and "dut", as outlined below.
//
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetUT() memberfunctions.
//
// The input arguments represent the following :
// y    : year in UT (e.g. 1952, 2003 etc...)
// m    : month in UT (1=jan  2=feb etc...)
// d    : day in UT (1-31)
// hh   : elapsed hours in UT (0-23) 
// mm   : elapsed minutes in UT (0-59)
// s    : elapsed (fractional) seconds in UT (0-59.999...)
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, which will serve as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The time that was specified represents UTC, and the provided UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.  
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, and the retrieved UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The time that was specified represents UT1, which will become the reference time.
//                This mode is equivalent to utc="A" apart from the fact that UT1 will not be modified.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid date and/or time arguments, JD=0 is set to indicate that something went wrong.
// 2) In case of invalid "utc" input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 3) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are utc="A", leap=0 and dut=0.

 Int_t ss=int(s);
 s-=double(ss);
 Int_t ns=s*1.e9;
 s-=double(ns)*1.e-9;
 Int_t ps=s*1.e12;
 SetUT(y,m,d,hh,mm,ss,ns,ps,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetUT(Int_t y,Int_t m,Int_t d,TString time,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the UT date and time
// in the Gregorian calendar as specified by the input arguments.
// Note that UT may represent either UT1 or UTC as specified by the
// arguments "utc", "leap" and "dut", as outlined below.
//
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetUT() memberfunctions.
//
// The input arguments represent the following :
// y    : year in UT (e.g. 1952, 2003 etc...)
// m    : month in UT (1=jan  2=feb etc...)
// d    : day in UT (1-31)
// time : The UT time in the format hh:mm:ss.sss
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, which will serve as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The time that was specified represents UTC, and the provided UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.  
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, and the retrieved UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The time that was specified represents UT1, which will become the reference time.
//                This mode is equivalent to utc="A" apart from the fact that UT1 will not be modified.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid date and/or time arguments, JD=0 is set to indicate that something went wrong.
// 2) In case of invalid "utc" input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 3) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are utc="A", leap=0 and dut=0.

 Int_t iword=0;
 Double_t uttime=0;
 Int_t hh=0;
 Int_t mm=0;
 Double_t s=0;
 time.ReplaceAll(":"," ");
 uttime=time.Atof();
 iword=int(uttime);
 hh=iword/10000;
 iword=iword%10000;
 mm=iword/100;
 s=uttime-double(hh*10000+mm*100);
 SetUT(y,m,d,hh,mm,s,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetUT(TString date,TString time,Int_t mode,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the UT date and time
// in the Gregorian calendar as specified by the input arguments.
// Note that UT may represent either UT1 or UTC as specified by the
// arguments "utc", "leap" and "dut", as outlined below.
//
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetUT() memberfunctions.
//
// The input arguments represent the following :
// date : The UT date in the format dd-mm-yyyy or dd/mm/yyyy or ddmmyyyy (mode=0)
// date : The UT date in the format yyyy-mm-dd or yyyy/mm/dd or yyyymmdd (mode=1)
// date : The UT date in the format mm-dd-yyyy or mm/dd/yyyy or mmddyyyy (mode=2)
// date : The UT date in the format yyyy-dd-mm or yyyy/dd/mm or yyyyddmm (mode=3)
// time : The UT time in the format hh:mm:ss.sss
// mode : Date format specifier (see above)
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, which will serve as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The time that was specified represents UTC, and the provided UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.  
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, and the retrieved UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The time that was specified represents UT1, which will become the reference time.
//                This mode is equivalent to utc="A" apart from the fact that UT1 will not be modified.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid date and/or time arguments, JD=0 is set to indicate that something went wrong.
// 2) In case of invalid "utc" input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 3) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The default values are utc="A", leap=0 and dut=0.
//
// Note : In case of incompatible argument(s) the current UT date/time will not be modified.

 Int_t iword=0;
 Int_t utdate=0;
 Int_t year=0;
 Int_t month=0;
 Int_t day=0;
 TString datex=date;
 datex.ReplaceAll("-"," ");
 datex.ReplaceAll("/"," ");
 utdate=datex.Atoi();
 iword=utdate;
 if (mode==0)
 {
  day=iword/1000000;
  iword=iword%1000000;
  month=iword/10000;
  iword=iword%10000;
  year=iword;
 }
 if (mode==1)
 {
  year=iword/10000;
  iword=iword%10000;
  month=iword/100;
  iword=iword%100;
  day=iword;
 }
 if (mode==2)
 {
  month=iword/1000000;
  iword=iword%1000000;
  day=iword/10000;
  iword=iword%10000;
  year=iword;
 }
 if (mode==3)
 {
  year=iword/10000;
  iword=iword%10000;
  day=iword/100;
  iword=iword%100;
  month=iword;
 }

 if (day<1 || day>31 || month<1 || month>12)
 {
  cout << " *NcTimestamp::SetUT* Incompatible argument(s) Date: " << date << " Time: " << time << " mode: " << mode << endl;
  cout << " ==> TAI related time recording is disabled and JD=0 has been set." << endl;
  SetJD(0,"N");
 }
 else
 {
  SetUT(year,month,day,time,utc,leap,dut);
 }
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetUT(Int_t y,Int_t d,Int_t s,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the specified elapsed
// timespan since the beginning of the new UT year.
// Note that UT may represent either UT1 or UTC as specified by the
// arguments "utc", "leap" and "dut", as outlined below.
//
// This facility is exact upto picosecond precision and as such is
// for scientific observations preferable above the corresponding
// Set function(s) of TTimestamp.
// The latter has a random spread in the sub-second part, which
// might be of use in generating distinguishable timestamps while
// still keeping second precision.
//
// The UT year and elapsed time span is entered via the following input arguments :
//
// y    : year in UT (e.g. 1952, 2003 etc...)
// d    : elapsed number of days 
// s    : (remaining) elapsed number of seconds
// ns   : (remaining) elapsed number of nanoseconds
// ps   : (remaining) elapsed number of picoseconds
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, which will serve as the reference time.
//                Since no Leap Second info is available, the TAI related time recording is disabled
//                and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//                The time that was specified represents UTC, and the provided UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.  
//                The provided UTC parameters will serve to determine the TAI related time recording.
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                The time that was specified represents UTC, and the retrieved UTC parameters
//                will be used to determine UT1, which will from now on be the reference time.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
//                In case the corresponding UTC parameters are not found, this mode is equivalent to utc="N".
//                For further details see the memberfunctions LoadUTCparameterFiles() and GetUTCparameters().
//        "U" ==> The time that was specified represents UT1, which will become the reference time.
//                This mode is equivalent to utc="A" apart from the fact that UT1 will not be modified.
//                The retrieved UTC parameters will serve to determine the TAI related time recording.
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT1-UTC in seconds.
//
// The value of UT1-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT1-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT1-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid date and/or time arguments, JD=0 is set to indicate that something went wrong.
// 2) In case of invalid "utc" input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT1-UTC will be set to zero.
// 3) In case utc="A" or utc="U" and no data files have been loaded, or no information is available,
//    the TAI related time recording is disabled and the values of the Leap Seconds and dut=UT1-UTC
//    will be set to zero.
//
// The specified d, s, ns and ps values will be used in an additive
// way to determine the elapsed timespan.
// So, specification of d=1, s=100, ns=0, ps=0 will result in the
// same elapsed time span as d=0, s=24*3600+100, ns=0, ps=0.
// However, by making use of the latter the user should take care
// of possible integer overflow problems in the input arguments,
// which obviously will provide incorrect results. 
//
// Note : ns=0, ps=0, utc="A", leap=0 and dut=0  are the default values.
//
// This facility first sets the (M)JD corresponding to the start (01-jan 00:00:00)
// of the specified UT year following the recipe of R.W. Sinnott
// Sky & Telescope 82, (aug. 1991) 183.
// Subsequently the day and (sub)second parts are added to the NcTimestamp.
// As such this facility is valid for all AD dates in the Gregorian calendar.

 Double_t jd=GetJD(y,1,1,0,0,0,0);
 SetJD(jd);

 Int_t mjd,sec,nsec;
 GetMJD(mjd,sec,nsec);
 SetMJD(mjd,0,0,0,utc,leap,dut);
 Add(d,s,ns,ps);

 // Determine UT1 using dut=UT1-UTC if UTC was provided as input
 if (utc!="U") AddSec(fDut);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::GetUT(Int_t& hh,Int_t& mm,Int_t& ss,Int_t& ns,Int_t& ps)
{
// Provide the corresponding UT reference time as hh:mm:ss:ns:ps.
// This facility is based on the MJD, so the TTimeStamp limitations
// do not apply here.

 Int_t mjd,sec,nsec,psec;

 GetMJD(mjd,sec,nsec);
 psec=GetPs();

 hh=sec/3600;
 sec=sec%3600;
 mm=sec/60;
 ss=sec%60;
 ns=nsec;
 ps=psec;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetUT()
{
// Provide the corresponding UT reference time in fractional hours.
// This facility is based on the MJD, so the TTimeStamp limitations
// do not apply here.

 Int_t hh,mm,ss,ns,ps;

 GetUT(hh,mm,ss,ns,ps);

 Double_t ut=Convert(hh,mm,ss,ns,ps);

 return ut;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::GetGMST(Int_t& hh,Int_t& mm,Int_t& ss,Int_t& ns,Int_t& ps)
{
// Provide the corrresponding Greenwich Mean Sideral Time (GMST).
// The algorithm used is the one described at p. 83 of the book
// Astronomy Methods by Hale Bradt.
// This facility is based on the MJD, so the TTimeStamp limitations
// do not apply here.

 Int_t mjd,sec,nsec,psec;

 // The current UT based timestamp data
 GetMJD(mjd,sec,nsec);
 psec=fJps;

 // The basis for the daily corrections in units of Julian centuries w.r.t. J2000.
 // Note : Epoch J2000 starts at 01-jan-2000 12:00:00 UT.
 Double_t tau=(GetJD()-2451545.)/36525.;

 // Syncronise sidereal time with current timestamp
 NcTimestamp sid;
 sid.SetMJD(mjd,sec,nsec,psec);

 // Add offset for GMST start value defined as 06:41:50.54841 at 01-jan 00:00:00 UT
 sec=6*3600+41*60+50;
 nsec=548410000;
 psec=0;
 sid.Add(0,sec,nsec,psec);

 // Daily correction for precession and polar motion 
 Double_t addsec=8640184.812866*tau+0.093104*pow(tau,2)-6.2e-6*pow(tau,3);
 sec=int(addsec);
 addsec-=double(sec);
 nsec=int(addsec*1.e9);
 addsec-=double(nsec)*1.e-9;
 psec=int(addsec*1.e12);
 sid.Add(0,sec,nsec,psec);

 sid.GetMJD(mjd,sec,nsec);
 psec=sid.GetPs();

 hh=sec/3600;
 sec=sec%3600;
 mm=sec/60;
 ss=sec%60;
 ns=nsec;
 ps=psec;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetGMST()
{
// Provide the corrresponding Greenwich Mean Sideral Time (GMST)
// in fractional hours.
// This facility is based on the MJD, so the TTimeStamp limitations
// do not apply here.

 Int_t hh,mm,ss,ns,ps;

 GetGMST(hh,mm,ss,ns,ps);

 Double_t gst=Convert(hh,mm,ss,ns,ps);

 return gst;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetGAST()
{
// Provide the corrresponding Greenwich Apparent Sideral Time (GAST)
// in fractional hours.
// In case a hh:mm:ss.sss format is needed, please invoke the Convert()
// memberfunction for conversion of the provided fractional hour value.
//
// The GAST is the GMST corrected for the shift of the vernal equinox
// due to nutation. The right ascension component of the nutation correction
// of the vernal equinox is called the "equation of the equinoxes".
// So we have :
//
//      GAST = GMST + (equation of the equinoxes)
//
// The equation of the equinoxes is determined via the Almanac() memberfunction.
// 
// Since GMST is based on the MJD, the TTimeStamp limitations do not apply here.

 Double_t da=Almanac();

 // Convert to fractional hours
 da/=3600.;

 Double_t gast=GetGMST()+da;

 while (gast<0)
 {
  gast+=24.;
 }
 while (gast>24.)
 {
  gast-=24.;
 }

 return gast;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetLT(Double_t offset)
{
// Provide the corresponding local time in fractional hours.
// The "offset" denotes the time difference in (fractional) hours w.r.t. UT.
// A mean solar day lasts 24h (i.e. 86400s).
//
// In case a hh:mm:ss format is needed, please use the Convert() facility. 

 // Current UT time in fractional hours
 Double_t h=GetUT();
 
 h+=offset;

 while (h<0)
 {
  h+=24.;
 }
 while (h>24)
 {
  h-=24.;
 }

 return h;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetLMST(Double_t offset)
{
// Provide the corresponding Local Mean Sidereal Time (LMST) in fractional hours.
// The "offset" denotes the time difference in (fractional) hours w.r.t. GMST.
// A sidereal day corresponds to 23h 56m 04.09s (i.e. 86164.09s) mean solar time.
// The definition of GMST is such that a sidereal clock corresponds with
// 24 sidereal hours per revolution of the Earth.
// As such, local time offsets w.r.t. UT and GMST can be treated similarly. 
//
// In case a hh:mm:ss format is needed, please use the Convert() facility. 

 // Current GMST time in fractional hours
 Double_t h=GetGMST();

 h+=offset;

 while (h<0)
 {
  h+=24.;
 }
 while (h>24)
 {
  h-=24.;
 }

 return h;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetLAST(Double_t offset)
{
// Provide the corresponding Local Apparent Sidereal Time (LAST) in fractional hours.
// The "offset" denotes the time difference in (fractional) hours w.r.t. GAST.
// A sidereal day corresponds to 23h 56m 04.09s (i.e. 86164.09s) mean solar time.
// The definition of GMST and GAST is such that a sidereal clock corresponds with
// 24 sidereal hours per revolution of the Earth.
// As such, local time offsets w.r.t. UT, GMST and GAST can be treated similarly. 
//
// In case a hh:mm:ss.sss format is needed, please use the Convert() facility. 

 // Current GAST time in fractional hours
 Double_t h=GetGAST();

 h+=offset;

 while (h<0)
 {
  h+=24.;
 }
 while (h>24)
 {
  h-=24.;
 }

 return h;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetLT(Double_t dt,Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Int_t ss,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the LT date and time
// in the Gregorian calendar as specified by the input arguments.
// This facility is exact upto picosecond precision and as such is
// for scientific observations preferable above the corresponding
// Set function(s) of TTimestamp.
// The latter has a random spread in the sub-second part, which
// might be of use in generating distinguishable timestamps while
// still keeping second precision.
// Optionally the user can also provide the corresponding UTC parameters 
// (i.e. cumulated number of Leap Seconds and dut=UT-UTC) to enable to report next 
// to UT also UTC and TAI derived time information.
// These UTC parameters will not affect the LT setting.
//
// The input arguments represent the following :
//
// dt   : the local time offset in fractional hours w.r.t. UT.
// y    : year in LT (e.g. 1952, 2003 etc...)
// m    : month in LT (1=jan  2=feb etc...)
// d    : day in LT (1-31)
// hh   : elapsed hours in LT (0-23) 
// mm   : elapsed minutes in LT (0-59)
// ss   : elapsed seconds in LT (0-59)
// ns   : remaining fractional elapsed second of LT in nanosecond
// ps   : remaining fractional elapsed nanosecond of LT in picosecond
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                The TAI related time recording is disabled and the values of
//                the Leap Seconds and dut=UT-UTC will be set to zero.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT-UTC in seconds.
//
// The value of UT-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
// 2) In case utc="A" and no data files have been loaded, or no information is available,
//    the utc="N" mode will be invoked.
//
// The default values are ns=0, ps=0, utc="A", leap=0 and dut=0.
//
// This facility first sets the UT as specified by the input arguments
// and then corrects the UT by subtracting the local time offset w.r.t. UT.
// As such this facility is valid for all AD dates in the Gregorian
// calendar with picosecond precision.

 SetUT(y,m,d,hh,mm,ss,ns,ps,utc,leap,dut);
 Add(-dt);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetLT(Double_t dt,Int_t y,Int_t m,Int_t d,Int_t hh,Int_t mm,Double_t s,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the LT date and time
// in the Gregorian calendar as specified by the input arguments.
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetLT() memberfunctions.
// Optionally the user can also provide the corresponding UTC parameters 
// (i.e. cumulated number of Leap Seconds and dut=UT-UTC) to enable to report next 
// to UT also UTC and TAI derived time information.
// These UTC parameters will not affect the LT setting.
//
// The input arguments represent the following :
// dt   : the local time offset in fractional hours w.r.t. UT
// y    : year in LT (e.g. 1952, 2003 etc...)
// m    : month in LT (1=jan  2=feb etc...)
// d    : day in LT (1-31)
// hh   : elapsed hours in LT (0-23) 
// mm   : elapsed minutes in LT (0-59)
// s    : elapsed (fractional) seconds in LT (0-59.999...)
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                The TAI related time recording is disabled and the values of
//                the Leap Seconds and dut=UT-UTC will be set to zero.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT-UTC in seconds.
//
// The value of UT-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
// 2) In case utc="A" and no data files have been loaded, or no information is available,
//    the utc="N" mode will be invoked.
//
// The default values are utc="A", leap=0 and dut=0.
//
// This facility first sets the UT as specified by the input arguments
// and then corrects the UT by subtracting the local time offset w.r.t. UT.
// As such this facility is valid for all AD dates in the Gregorian calendar.

 SetUT(y,m,d,hh,mm,s,utc,leap,dut);
 Add(-dt);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetLT(Double_t dt,Int_t y,Int_t m,Int_t d,TString time,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the LT date and time
// in the Gregorian calendar as specified by the input arguments.
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetLT() memberfunctions.
// Optionally the user can also provide the corresponding UTC parameters 
// (i.e. cumulated number of Leap Seconds and dut=UT-UTC) to enable to report next 
// to UT also UTC and TAI derived time information.
// These UTC parameters will not affect the LT setting.
//
// The input arguments represent the following :
// dt   : the local time offset in fractional hours w.r.t. UT
// y    : year in LT (e.g. 1952, 2003 etc...)
// m    : month in LT (1=jan  2=feb etc...)
// d    : day in LT (1-31)
// time : The LT time in the format hh:mm:ss.sss
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                The TAI related time recording is disabled and the values of
//                the Leap Seconds and dut=UT-UTC will be set to zero.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT-UTC in seconds.
//
// The value of UT-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
// 2) In case utc="A" and no data files have been loaded, or no information is available,
//    the utc="N" mode will be invoked.
//
// The default values are utc="A", leap=0 and dut=0.
//
// This facility first sets the UT as specified by the input arguments
// and then corrects the UT by subtracting the local time offset w.r.t. UT.
// As such this facility is valid for all AD dates in the Gregorian calendar.

 SetUT(y,m,d,time,utc,leap,dut);
 Add(-dt);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetLT(Double_t dt,TString date,TString time,Int_t mode,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the LT date and time
// in the Gregorian calendar as specified by the input arguments.
// Due to rounding errors the highest (i.e. picosecond) accuracy might not be reached.
// For a guaranteed picosecond precision please refer to the other SetLT() memberfunctions.
// Optionally the user can also provide the corresponding UTC parameters 
// (i.e. cumulated number of Leap Seconds and dut=UT-UTC) to enable to report next 
// to UT also UTC and TAI derived time information.
// These UTC parameters will not affect the LT setting.
//
// The input arguments represent the following :
// dt   : the local time offset in fractional hours w.r.t. UT
// date : The LT date in the format dd-mm-yyyy or dd/mm/yyyy or ddmmyyyy (mode=0)
// date : The LT date in the format yyyy-mm-dd or yyyy/mm/dd or yyyymmdd (mode=1)
// date : The LT date in the format mm-dd-yyyy or mm/dd/yyyy or mmddyyyy (mode=2)
// date : The LT date in the format yyyy-dd-mm or yyyy/dd/mm or yyyyddmm (mode=3)
// time : The LT time in the format hh:mm:ss.sss
// mode : Date format specifier (see above)
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                The TAI related time recording is disabled and the values of
//                the Leap Seconds and dut=UT-UTC will be set to zero.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT-UTC in seconds.
//
// The value of UT-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
// 2) In case utc="A" and no data files have been loaded, or no information is available,
//    the utc="N" mode will be invoked.
//
// The default values are utc="A", leap=0 and dut=0.
//
// This facility first sets the UT as specified by the input arguments
// and then corrects the UT by subtracting the local time offset w.r.t. UT.
// As such this facility is valid for all AD dates in the Gregorian calendar.

 SetUT(date,time,mode,utc,leap,dut);
 Add(-dt);
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetLT(Double_t dt,Int_t y,Int_t d,Int_t s,Int_t ns,Int_t ps,TString utc,Int_t leap,Double_t dut)
{
// Set the NcTimestamp parameters corresponding to the specified elapsed
// timespan since the beginning of the new LT year.
// This facility is exact upto picosecond precision and as such is
// for scientific observations preferable above the corresponding
// Set function(s) of TTimestamp.
// The latter has a random spread in the sub-second part, which
// might be of use in generating distinguishable timestamps while
// still keeping second precision.
// Optionally the user can also provide the corresponding UTC parameters 
// (i.e. cumulated number of Leap Seconds and dut=UT-UTC) to enable to report next 
// to UT also UTC and TAI derived time information.
// These UTC parameters will not affect the LT setting.
//
// The LT year and elapsed time span is entered via the following input arguments :
//
// dt   : the local time offset in fractional hours w.r.t. UT.
// y    : year in LT (e.g. 1952, 2003 etc...)
// d    : elapsed number of days 
// s    : (remaining) elapsed number of seconds
// ns   : (remaining) elapsed number of nanoseconds
// ps   : (remaining) elapsed number of picoseconds
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                The TAI related time recording is disabled and the values of
//                the Leap Seconds and dut=UT-UTC will be set to zero.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT-UTC in seconds.
//
// The value of UT-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
// 2) In case utc="A" and no data files have been loaded, or no information is available,
//    the utc="N" mode will be invoked.
//
// The specified d, s, ns and ps values will be used in an additive
// way to determine the elapsed timespan.
// So, specification of d=1, s=100, ns=0, ps=0 will result in the
// same elapsed time span as d=0, s=24*3600+100, ns=0, ps=0.
// However, by making use of the latter the user should take care
// of possible integer overflow problems in the input arguments,
// which obviously will provide incorrect results. 
//
// Note : ns=0, ps=0, utc="A", leap=0 and dut=0 are the default values.
//
// This facility first sets the UT as specified by the input arguments
// and then corrects the UT by subtracting the local time offset w.r.t. UT.
// As such this facility is valid for all AD dates in the Gregorian calendar.

 SetUT(y,d,s,ns,ps,utc,leap,dut);
 Add(-dt);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetJD(Double_t e,TString mode) const
{
// Provide the fractional Julian Date from epoch e.
// The sort of epoch may be specified via the "mode" parameter.
//
// mode = "J" ==> Julian epoch
//        "B" ==> Besselian epoch
//
// The default value is mode="J".

 Double_t jd=0;

 if (mode=="J" || mode=="j") jd=(e-2000.0)*365.25+2451545.0;

 if (mode=="B" || mode=="b") jd=(e-1900.0)*365.242198781+2415020.31352;

 return jd;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetMJD(Double_t e,TString mode) const
{
// Provide the fractional Modified Julian Date from epoch e.
// The sort of epoch may be specified via the "mode" parameter.
//
// mode = "J" ==> Julian epoch
//        "B" ==> Besselian epoch
//
// The default value is mode="J".

 Double_t mjd=GetJD(e,mode)-2400000.5;

 return mjd;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetTJD(Double_t e,TString mode) const
{
// Provide the fractional Truncated Julian Date from epoch e.
// The sort of epoch may be specified via the "mode" parameter.
//
// mode = "J" ==> Julian epoch
//        "B" ==> Besselian epoch
//
// The default value is mode="J".

 Double_t tjd=GetJD(e,mode)-2440000.5;

 return tjd;
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::Almanac(Double_t* dpsi,Double_t* deps,Double_t* eps,Double_t* dl,TString name,Double_t* el,Double_t* eb,Double_t* dr,Double_t* value,Int_t j)
{
// Determination of various astronomical observables which may be needed
// for further calculations like e.g. precession of coordinates.
//
// The standard returned value is the "equation of the equinoxes" 
// (i.e. the nutational shift of the RA of the vernal equinox) in seconds.
// The memberfunction arguments provide the possibility of retrieving
// optional returned values. The corresponding output observables are :
//
// dpsi    : Nutational shift in ecliptic longitude in arcseconds
// deps    : Nutational shift in ecliptic obliquity in arcseconds
// eps     : Mean obliquity of the ecliptic in arcseconds
// dl      : General precession in ecliptic longitude in arcseconds
// el      : Ecliptic longitude (in degrees) of the selected solar system body w.r.t. the equinox of date
// eb      : Ecliptic latitude (in degrees) of the selected solar system body w.r.t. the equinox of date
// dr      : Distance of the selected solar system body w.r.t. the selected origin
// value   : One of the additional observable values mentioned below
// j       : Index to select an observable value such that value=obs[j] as mentioned below
//
// Apart from some generic parameters, the user may select via the input argument "name"
// a specific solar system body for which various astronomical data are retrieved.
//
// Currently the following names are supported :
// ---------------------------------------------
// "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Sun", "Moon".
//
// By specifying one of the above names, the geocentric ecliptic coordinates (el,eb,dr)
// of the specified body are provided w.r.t. the equinox of date.
// These values include the effect of precession, but not the effect due to nutation.
// In view of the accuracy of the used algorithms, the effects of light-time and aberration are
// not taken into account.
// By adding a "*" to the name, e.g. "Mars*", the corresponding heliocentric ecliptic coordinates are provided.
// In case "Moon*" is selected, the corresponding "Earth*" values are provided since the differences
// are negligible in view of the accuracy of the used algorithms.
//
// The distance "dr" is provided in Astronomical Units (AU), except for the selection "Moon" in which case
// the Earth-Moon distance is provided in kilometers.
//
// To obtain the ecliptic longitude for the J2000 equinox, the user may retrieve "dl" and subtract
// (after conversion to degrees) this value from "el". 
// The user may correct the obtained ecliptic longitude for nutation by also retrieving "dpsi"
// and adding (after conversion to degrees) this value to the obtained "el".
//
// In addition to the ecliptic coordinates, also some heliocentric orbital parameters of the
// selected solar system body will be provided w.r.t. the equinox of date.
// These additional parameters are only provided for the major planets.
//
// Available additional observable values :
// ---------------------------------------- 
// obs[0]  : Semi major axis (in AU) of the orbit
// obs[1]  : Eccentricity of the orbit
// obs[2]  : Inclination (in degrees) of the orbit with the ecliptic
// obs[3]  : Mean ecliptic longitude (in degrees) of the ascending node
// obs[4]  : Mean orbital longitude (in degrees) of the perihelion
// obs[5]  : Mean orbital longitude (in degrees) of the body
// obs[6]  : Orbital argument (in degrees) of the perihelion
// obs[7]  : Mean orbital anomaly (in degrees) of the body
// obs[8]  : Equation of the center (in degrees)
// obs[9]  : True anomaly (in degrees) of the body
// obs[10] : Equation of Time (in seconds)
//
// All generic shifts are determined for the current timestamp with
// J2000.0 (i.e. 01-jan-2000 12:00:00 UT) as the reference epoch.
//
// Invokation example :
// --------------------
// NcTimestamp t;
// Double_t da,dpsi,deps,eps;
// da=t.Almanac(&dpsi,&deps,&eps); 
//
// Notes :
// -------
// 1) The default values are dpsi=0, deps=0, eps=0, dl=0, name="", el=0, eb=0, dr=0, value=0 and j=0,
//    which implies that the default invokation Almanac() will only return the equation of the equinoxes in sec.
// 2) The definitions and expressions are the ones used in the book of Jean Meeus
//    "Astronomical Algorithms" (2nd edition of August 2009), esp. chapters 31-33.
//
// The nutation model used is the new one as documented in :
//   "The IAU Resolutions on Astronomical Reference Systems,
//    Time Scales and Earth Rotation Models".
// This document is freely available as Circular 179 (2005) of the
// United States Naval Observatory (USNO).
// (See : http://aa.usno.navy.mil/publications/docs).
//
// The change in ecliptic longitude (dpsi) and ecliptic obliquity (deps)
// are evaluated using the IAU 2000A nutation series expansion
// as provided in the USNO Circular 179.
// The new expression for the equation of the equinoxes is based on a series
// expansion and is the most accurate one known to date.
// The components are documented on p.17 of the USNO Circular 179.
//
// In the current implementation only the first 28 terms of the nutation series
// are used. This provides an accuracy of about 0.01 arcsec corresponding to 0.001 sec.
// In case a better accuracy is required, the series can be extended.
// The total series expansion consists of 1365 terms.
// 
// Since all calculations are based on the JD, the TTimeStamp limitations
// do not apply here.

 Double_t pi=acos(-1.);

 Double_t td;      // Time difference in fractional Julian days w.r.t. the start of J2000.
 Double_t tc;      // Time difference in fractional Julian centuries w.r.t. the start of J2000.
 Double_t tm;      // Time difference in fractional Julian millennia w.r.t. the start of J2000.
 const Int_t nvals=11;
 Double_t val[nvals]; // Array to hold the additional (orbital) parameters
 
 td=GetJD()-2451545.0;
 tc=td/36525.;
 tm=tc/10.;

 // Fundamental solar system variables (in arcseconds) w.r.t. the J2000.0 equinox.
 // The expressions are taken from the USNO circular 179.
 // epsilon : Mean obliquity of the ecliptic
 // l       : Mean anomaly of the Moon
 // lp      : Mean anomaly of the Sun
 // f       : Mean argument of latitude of the moon
 // d       : Mean elongation of the Moon from the Sun
 // om      : Mean longitude of the Moon's mean ascending node
 Double_t epsilon=84381.406-46.836769*tc-0.0001831*pow(tc,2)+0.00200340*pow(tc,3)-0.000000576*pow(tc,4)-0.0000000434*pow(tc,5);
 Double_t l=485868.249036+1717915923.2178*tc+31.8792*pow(tc,2)+0.051635*pow(tc,3)-0.00024470*pow(tc,4);
 Double_t lp=1287104.79305+129596581.0481*tc-0.5532*pow(tc,2)+0.000136*pow(tc,3)-0.00001149*pow(tc,4);
 Double_t f=335779.526232+1739527262.8478*tc-12.7512*pow(tc,2)-0.001037*pow(tc,3)+0.00000417*pow(tc,4);
 Double_t d=1072260.70369+1602961601.2090*tc-6.3706*pow(tc,2)+0.006593*pow(tc,3)-0.00003169*pow(tc,4);
 Double_t om=450160.398036-6962890.5431*tc+7.4722*pow(tc,2)+0.007702*pow(tc,3)-0.00005939*pow(tc,4);

 // General precession in longitude (in arcseconds) w.r.t. J2000.0
 // according to Jean Meeus Ch.21 (page 136)
 Double_t prec=5029.0966*tc+1.11113*pow(tc,2)-0.000006*pow(tc,3);

 if (eps) *eps=epsilon;
 if (dl) *dl=prec;

 // Convert to radians for use with goniometric functions
 Double_t fac=pi/(180.*3600.);
 epsilon*=fac;
 l*=fac;
 lp*=fac;
 f*=fac;
 d*=fac;
 om*=fac;

 //The IAU 2000A nutation series expansion.
 Double_t phi[28]={om,2.*(f-d+om),2.*(f+om),2.*om,lp,lp+2.*(f-d+om),l,
                   2.*f+om,l+2.*(f+om),2.*(f-d+om)-lp,2.*(f-d)+om,2.*(f+om)-l,2.*d-l,l+om,
                   om-l,2.*(f+d+om)-l,l+2.*f+om,2.*(f-l)+om,2.*d,2.*(f+d+om),2.*(f-d+om-lp),
                   2.*(d-l),2.*(l+d+om),l+2.*(f-d+om),2.*f+om-l,2.*l,2.*f,lp+om};
 Double_t s[28]={-17.2064161,-1.3170907,-0.2276413, 0.2074554, 0.1475877,-0.0516821, 0.0711159,
                  -0.0387298,-0.0301461, 0.0215829, 0.0128227, 0.0123457, 0.0156994, 0.0063110,
                  -0.0057976,-0.0059641,-0.0051613, 0.0045893, 0.0063384,-0.0038571, 0.0032481,
                  -0.0047722,-0.0031046, 0.0028593, 0.0020441, 0.0029243, 0.0025887,-0.0014053};
 Double_t sd[28]={-0.0174666,-0.0001675,-0.0000234, 0.0000207,-0.0003633, 0.0001226, 0.0000073,
                  -0.0000367,-0.0000036,-0.0000494, 0.0000137, 0.0000011, 0.0000010, 0.0000063,
                  -0.0000063,-0.0000011,-0.0000042, 0.0000050, 0.0000011,-0.0000001, 0.0000000,
                   0.0000000,-0.0000001, 0.0000000, 0.0000021, 0.0000000, 0.0000000,-0.0000025};
 Double_t cp[28]={ 0.0033386,-0.0013696, 0.0002796,-0.0000698, 0.0011817,-0.0000524,-0.0000872,
                   0.0000380, 0.0000816, 0.0000111, 0.0000181, 0.0000019,-0.0000168, 0.0000027,
                  -0.0000189, 0.0000149, 0.0000129, 0.0000031,-0.0000150, 0.0000158, 0.0000000,
                  -0.0000018, 0.0000131,-0.0000001, 0.0000010,-0.0000074,-0.0000066, 0.0000079};
 Double_t c[28]= { 9.2052331, 0.5730336, 0.0978459,-0.0897492, 0.0073871, 0.0224386,-0.0006750,
                   0.0200728, 0.0129025,-0.0095929,-0.0068982,-0.0053311,-0.0001235,-0.0033228,
                   0.0031429, 0.0025543, 0.0026366,-0.0024236,-0.0001220, 0.0016452,-0.0013870,
                   0.0000477, 0.0013238,-0.0012338,-0.0010758,-0.0000609,-0.0000550, 0.0008551};
 Double_t cd[28]={ 0.0009086,-0.0003015,-0.0000485, 0.0000470,-0.0000184,-0.0000677, 0.0000000,
                   0.0000018,-0.0000063, 0.0000299,-0.0000009, 0.0000032, 0.0000000, 0.0000000,
                   0.0000000,-0.0000011, 0.0000000,-0.0000010, 0.0000000,-0.0000011, 0.0000000,
                   0.0000000,-0.0000011, 0.0000010, 0.0000000, 0.0000000, 0.0000000,-0.0000002};
 Double_t sp[28]={ 0.0015377,-0.0004587, 0.0001374,-0.0000291,-0.0001924,-0.0000174, 0.0000358,
                   0.0000318, 0.0000367, 0.0000132, 0.0000039,-0.0000004, 0.0000082,-0.0000009,
                  -0.0000075, 0.0000066, 0.0000078, 0.0000020, 0.0000029, 0.0000068, 0.0000000,
                  -0.0000025, 0.0000059,-0.0000003,-0.0000003, 0.0000013, 0.0000011,-0.0000045};
 
 Double_t dp=0,de=0,da=0;
 for (Int_t i=0; i<28; i++)
 {
  dp+=(s[i]+sd[i]*tc)*sin(phi[i])+cp[i]*cos(phi[i]);
  de+=(c[i]+cd[i]*tc)*cos(phi[i])+sp[i]*sin(phi[i]);
 }

 da=dp*cos(epsilon)+0.00264096*sin(om)+0.00006352*sin(2.*om)
     +0.00001175*sin(2.*f-2.*d+3.*om)+0.00001121*sin(2.*f-2.*d+om)
     -0.00000455*sin(2.*f-2.*d+2.*om)+0.00000202*sin(2.*f+3.*om)+0.00000198*sin(2.*f+om)
     -0.00000172*sin(3.*om)-0.00000087*tc*sin(om);

 if (dpsi) *dpsi=dp;
 if (deps) *deps=de;

 // Convert to seconds
 da/=15.;

 //////////////////////////////////////////////////////////////////////////////
 // Determination of the mean orbital elements and true ecliptic coordinates //
 // of a requested solar system body, for the mean equinox of the date.      //
 //////////////////////////////////////////////////////////////////////////////

 // The definitions and expressions are the ones used in the book of Jean Meeus
 // "Astronomical Algorithms" (2nd edition of August 2009), esp. chapters 31-33.

 // The various observables are :
 // a      : Semi major axis (in AU) of the orbit
 // e      : Eccentricity of the orbit
 // inc    : Inclination (in degrees) of the orbit with the ecliptic
 // omega  : Mean ecliptic longitude (in degrees) of the ascending node
 // lp     : Mean orbital longitude (in degrees) of the perihelion
 // l      : Mean orbital longitude (in degrees) of the body
 // omega2 : Orbital argument (in degrees) of the perihelion
 // m      : Mean orbital anomaly (in degrees) of the body
 // ec     : Equation of the center (in degrees)
 // nu     : True anomaly (in degrees) of the body
 // ltrue  : Heliocentric longitude of the body
 // btrue  : Heliocentric latitude of the body
 // r      : Distance between the body and the sun
 // lambda : Geocentric ecliptic longitude
 // beta   : Geocentric ecliptic latitude

 if (value) *value=0;
 for (Int_t i=0; i<nvals; i++)
 {
  val[i]=0;
 }

 Double_t a=0;
 Double_t e=0;
 Double_t inc=0;
 Double_t omega=0;
 Double_t omega2=0;
 Double_t m=0;
 Double_t ec=0;
 Double_t nu=0;
 Double_t ltrue=0;
 Double_t btrue=0;
 Double_t r=0;
 Double_t lambda=0;
 Double_t beta=0;

 // Polynomial coefficients for a (in AU)of the 8 major planets 
 Double_t aa0[8]={0.387098310,0.723329820,1.000001018,1.523679342,5.202603209,9.554909192,19.218446062,30.110386869};
 Double_t aa1[8]={0,0,0,0,0.0000001913,-0.0000021390,-0.0000000372,-0.0000001663};
 Double_t aa2[8]={0,0,0,0,0,0.000000004,0.00000000098,0.00000000069};
 Double_t aa3[8]={0,0,0,0,0,0,0,0};
 // Polynomial coefficients for e of the 8 major planets 
 Double_t ea0[8]={0.20563175,0.00677192,0.01670863,0.09340065,0.04849793,0.05554814,0.04638122,0.00945575};
 Double_t ea1[8]={0.000020407,-0.000047765,-0.000042037,0.000090484,0.000163225,-0.000346641,-0.000027293,0.000006033};
 Double_t ea2[8]={-0.0000000283,0.0000000981,-0.0000001267,-0.0000000806,-0.0000004714,-0.0000006436,0.0000000789,0};
 Double_t ea3[8]={-0.00000000018,0.00000000046,0.00000000014,-0.00000000025,-0.00000000201,0.00000000340,0.00000000024,-0.00000000005};
 // Polynomial coefficients for inc (in degrees) of the 8 major planets 
 Double_t ia0[8]={7.004986,3.394662,0,1.849726,1.303267,2.488879,0.773197,1.769953};
 Double_t ia1[8]={0.0018215,0.0010037,0,-0.0006011,-0.0054965,-0.0037362,0.0007744,-0.0093082};
 Double_t ia2[8]={-0.00001810,-0.00000088,0,0.00001276,0.00000466,-0.00001519,0.00003749,-0.00000708};
 Double_t ia3[8]={0.000000056,-0.000000007,0,-0.000000007,-0.000000002,0.000000087,-0.000000092,0.000000027};
 // Polynomial coefficients for omega (in degrees) of the 8 major planets
 Double_t oa0[8]={48.330893,76.679920,0,49.558093,100.464407,113.665503,74.005957,131.784057};
 Double_t oa1[8]={1.1861883,0.9011206,0,0.7720959,1.0209774,0.8770880,0.5211278,1.1022039};
 Double_t oa2[8]={0.00017542,0.00040618,0,0.00001557,0.00040315,-0.00012176,0.00133947,0.00025952};
 Double_t oa3[8]={0.000000215,-0.000000093,0,0.000002267,0.000000404,-0.000002249,0.000018484,-0.000000637};
 // Polynomial coefficients for lp (in degrees) of the 8 major planets
 Double_t pa0[8]={77.456119,131.563703,102.937348,336.060234,14.331207,93.057237,173.005291,48.120276};
 Double_t pa1[8]={1.5564776,1.4022288,1.7195366,1.8410449,1.6126352,1.9637613,1.4863790,1.4262957};
 Double_t pa2[8]={0.00029544,-0.00107618,0.00045688,0.00013477,0.00103042,0.00083753,0.00021406,0.00038434};
 Double_t pa3[8]={0.000000009,-0.000005678,-0.000000018,0.000000536,-0.000004464,0.000004928,0.000000434,0.000000020};
 // Polynomial coefficients for l (in degrees) of the 8 major planets
 Double_t la0[8]={252.250906,181.979801,100.466457,355.433000,34.351519,50.077444,314.055005,304.348665};
 Double_t la1[8]={149474.0722491,58519.2130302,36000.7698278,19141.6964471,3036.3027748,1223.5110686,429.8640561,219.8833092};
 Double_t la2[8]={0.00030350,0.00031014,0.00030322,0.00031052,0.00022330,0.00051908,0.00030390,0.00030882};
 Double_t la3[8]={0.000000018,0.000000015,0.000000020,0.000000016,0.000000037,-0.000000030,0.000000026,0.000000018};

 TString names[10]={"Mercury","Venus","Earth","Mars","Jupiter","Saturn","Uranus","Neptune","Sun","Moon"};

 Int_t k=-1;
 Int_t geo=1;
 for (Int_t jbody=0; jbody<10; jbody++)
 {
  if (name.Contains("*")) geo=0;
  if (name.Contains(names[jbody].Data()))
  {
   k=jbody;
   break;
  }
 }

 if (k<0) return da; // Non-supported solar system body

 if (!geo && k==8) return da; // Request for heliocentric data of the Sun itself

 if (geo && k==2) return da; // Request for geocentric data of the Earth itself

 // In case geocentric data for the Sun are requested, the heliocentric data
 // of the Earth are used to construct the corresponding data for the Sun.
 Int_t sun=0;
 if (k==8)
 {
  k=2;
  sun=1;
 }

 // In case heliocentric data for the Moon are requested, the corresponding data
 // for the Earth are provided in view of negligible differences within the
 // accuracy of the algorithms used here.
 Int_t moon=0;
 if (k==9) moon=1;
 if (!geo && k==9) k=2;  

 lambda=0;
 beta=0;

 ///////////////////////////////////////////////////////
 // Determination of the geocentric data for the Moon //
 ///////////////////////////////////////////////////////

 if (geo && k==9)
 {
  // Low-precision geocentric ecliptic coordinates (in degrees) of the Moon.
  // Source : Astronomical Alamanac 2012 page D22.
  // Maximal errors : 0.3 degr. in lambda, 0.2 degr. in beta, 0.003 degr. in plax and 0.2 R_Earth in r
  // A more accurate method is the series expansion given in the book of Jean Meeus.
  lambda=218.32+481267.881*tc
         +6.29*sin((135.+477198.87*tc)*pi/180.)-1.27*sin((259.3-413335.36*tc)*pi/180.)
         +0.66*sin((235.7+890534.22*tc)*pi/180.)+0.21*sin((269.9+954397.74*tc)*pi/180.)
         -0.19*sin((357.5+35999.05*tc)*pi/180.)-0.11*sin((186.5+966404.03*tc)*pi/180.);
  beta=5.13*sin((93.3+483202.02*tc)*pi/180.)+0.28*sin((228.2+960400.89*tc)*pi/180.)
       -0.28*sin((318.3+6003.15*tc)*pi/180.)-0.17*sin((217.6-407332.21*tc)*pi/180.);
  Double_t plax=0.9508
                +0.0518*cos((135.+477198.87*tc)*pi/180.)+0.0095*cos((259.3-413335.36*tc)*pi/180.)
                +0.0078*cos((235.7+890534.22*tc)*pi/180.)+0.0028*cos((269.9+954397.74*tc)*pi/180.);
  r=1./sin(plax*pi/180.);

 // Convert r into km using an average Earth radius of 6367.45 km
 r*=6367.45;

 while (lambda<0) { lambda+=360.; }
 while (lambda>360) { lambda-=360.; }

 if (el) *el=lambda;
 if (eb) *eb=beta;
 if (dr) *dr=r;

 return da;
 }

 ////////////////////////////////////////////////////////////////////////////////
 // Determination of the heliocentric data for the requested solar system body //
 ////////////////////////////////////////////////////////////////////////////////

 a=0;
 e=0;
 inc=0;
 omega=0;
 l=0;
 lp=0;

 a=aa0[k]+aa1[k]*tc+aa2[k]*pow(tc,2)+aa3[k]*pow(tc,3);
 e=ea0[k]+ea1[k]*tc+ea2[k]*pow(tc,2)+ea3[k]*pow(tc,3);
 inc=ia0[k]+ia1[k]*tc+ia2[k]*pow(tc,2)+ia3[k]*pow(tc,3);
 omega=oa0[k]+oa1[k]*tc+oa2[k]*pow(tc,2)+oa3[k]*pow(tc,3);
 lp=pa0[k]+pa1[k]*tc+pa2[k]*pow(tc,2)+pa3[k]*pow(tc,3);
 l=la0[k]+la1[k]*tc+la2[k]*pow(tc,2)+la3[k]*pow(tc,3);

 while (omega<0) { omega+=360.; }
 while (omega>360) { omega-=360.; }

 m=l-lp;

 while (m<0) { m+=360.; }
 while (m>360) { m-=360.; }

 ec=(2.*e-(pow(e,3)/4.)+(5.*pow(e,5)/96.))*sin(m*pi/180.)+((5.*pow(e,2)/4.)-(11.*pow(e,4)/24.))*sin(2.*m*pi/180.)
    +((13.*pow(e,3)/12.)-(43.*pow(e,5)/64.))*sin(3.*m*pi/180.)+(103.*pow(e,4)*sin(4.*m*pi/180.)/96.)
    +(1097.*pow(e,5)*sin(5.*m*pi/180.)/960.);
 ec*=180./pi;

 omega2=lp-omega;

 while (omega2<0) { omega2+=360.; }
 while (omega2>360) { omega2-=360.; }

 while (lp<0) { lp+=360.; }
 while (lp>360) { lp-=360.; }

 while (l<0) { l+=360.; }
 while (l>360) { l-=360.; }

 nu=m+ec;

 while (nu<0) { nu+=360.; }
 while (nu>360) { nu-=360.; }

 // Store the orbital parameters in the additional values array
 if (!sun && !moon)
 {
  val[0]=a;
  val[1]=e;
  val[2]=inc;
  val[3]=omega;
  val[4]=lp;
  val[5]=l;
  val[6]=omega2;
  val[7]=m;
  val[8]=ec;
  val[9]=nu;
 }

 // Determination of the Equation of Time via a recursive invokation
 if (j==10)
 {
  Double_t xdpsi,xdeps,xeps,xlambda,xbeta;
  Almanac(&xdpsi,&xdeps,&xeps,0,"Sun",&xlambda,&xbeta);

  // Convert from arcsec to degrees
  xdpsi/=3600.;
  xdeps/=3600.;
  xeps/=3600.;

  // Correct for nutation to get the true values
  xeps+=xdeps;
  xlambda+=xdpsi;

  while (xlambda<0) { xlambda+=360.; }
  while (xlambda>360) { xlambda-=360.; }

  // Convert to radians for gonio
  Double_t epsr=xeps*pi/180.;
  Double_t lambdar=xlambda*pi/180.;
  Double_t betar=xbeta*pi/180.;

  // True Right Ascension of the Sun (see Ch.13 p.93 of J. Meeus) 
  Double_t x=sin(lambdar)*cos(epsr)-tan(betar)*sin(epsr);
  Double_t y=cos(lambdar);
  Double_t alpha=0;
  if (x || y) alpha=atan2(x,y)*180./pi;

  while (alpha<0) { alpha+=360.; }
  while (alpha>360) { alpha-=360.; }

  // Mean longitude of the Sun (see Ch.28 p.183 of J. Meeus)
  Double_t L0=280.4664567+360007.6982779*tm+0.03032028*pow(tm,2)+pow(tm,3)/49931.-pow(tm,4)/15300.-pow(tm,5)/2000000.;

  while (L0<0) { L0+=360.; }
  while (L0>360) { L0-=360.; }

  // Determine the Equation of Time in degrees (see Ch.28 p.183 of J. Meeus)
  Double_t eot=L0-0.0057183-alpha+xdpsi*cos(xeps*pi/180.);

  // The Equation of Time never exceeds 20 minutes (= 5 degrees)
  while (eot<-5) { eot+=360.; }
  while (eot>5) { eot-=360.; }

  // Convert the Equation of Time from degrees to seconds
  eot*=240.;

  val[10]=eot;
 }
 
 // Make requested value available
 if (value)
 {
  *value=0;
  if (j>=0 && j<nvals) *value=val[j];
 }
 
 r=a*(1.-e*e)/(1.+e*cos(nu*pi/180.));

 // Use sine rule to obtain the latitude in radians
 Double_t sinb=sin(inc*pi/180.)*sin((l-omega+ec)*pi/180.);
 btrue=asin(sinb);

 ltrue=omega;
 // Use Neper's rule to obtain the extra term of the longitude
 Double_t arg=l-omega+ec;
 while (arg<0) { arg+=360.; }
 while (arg>360) { arg-=360.; }
 Double_t extra=0;
 if (cos(btrue))
 {
  Double_t cosl=cos(arg*pi/180.)/cos(btrue);
  extra=acos(cosl)*180./pi;
  if (arg>180) extra=-extra;
 }

 btrue*=180./pi;
 ltrue+=extra;

 // Convert heliocentric Earth data into geocentric Sun data if requested
 if (sun)
 {
  btrue=-btrue;
  ltrue+=180.;
 }

 while (ltrue<0) { ltrue+=360.; }
 while (ltrue>360) { ltrue-=360.; } 

 if (el) *el=ltrue;
 if (eb) *eb=btrue;
 if (dr) *dr=r;

 if (!geo || sun) return da; // Heliocentric (or geocentric Sun) coordinates were requested

 //////////////////////////////////////////////////
 // Convert into geocentric ecliptic coordinates //
 //////////////////////////////////////////////////

 // The algorithm used here is the one outlined in Ch. 33 of the book of Jean Meeus.
 // In view of the accuracy of the current algorithm, the effects of light-time
 // and aberration are not taken into account here.

 // Determine the heliocentric coordinates of the Earth
 // via recursive invokation of this memberfunction
 Double_t l0,b0,r0;
 Almanac(0,0,0,0,"Earth*",&l0,&b0,&r0);

 Double_t x=r*cos(btrue*pi/180.)*cos(ltrue*pi/180.)-r0*cos(b0*pi/180.)*cos(l0*pi/180.);
 Double_t y=r*cos(btrue*pi/180.)*sin(ltrue*pi/180.)-r0*cos(b0*pi/180.)*sin(l0*pi/180.);
 Double_t z=r*sin(btrue*pi/180.)-r0*sin(b0*pi/180.);

 lambda=atan2(y,x)*180./pi;
 beta=atan2(z,sqrt(x*x+y*y))*180./pi;
 r=sqrt(x*x+y*y+z*z);

 while (lambda<0) { lambda+=360.; }
 while (lambda>360) { lambda-=360.; }

 if (el) *el=lambda;
 if (eb) *eb=beta;
 if (dr) *dr=r;

 return da;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetEpoch(Double_t e,TString mode,TString utc,Int_t leap,Double_t dut)
{
// Set the timestamp parameters according to the specified epoch.
//
// Optionally the user can also provide the corresponding UTC parameters 
// (i.e. cumulated number of Leap Seconds and dut=UT-UTC) to enable to report next 
// to UT also UTC and TAI derived time information.
// These UTC parameters will not affect the epoch setting.
//
// The input arguments represent the following :
// e    : The epoch specification
// mode : "B" ==> Besselian epoch
//        "J" ==> Julian epoch
// utc  : Flag to denote whether the UTC parameters "leap" and "dut" (see below) are provided or not.
//        "N" ==> No UTC parameters will be stored.
//                The TAI related time recording is disabled and the values of
//                the Leap Seconds and dut=UT-UTC will be set to zero.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//        "M" ==> Manual setting of the UTC parameters as specified by "leap" and "dut".
//        "A" ==> Automatic setting of the UTC parameters from the loaded IERS data files.
//                In this case the specified values of "leap" and "dut" are irrelevant.
//                For further details see the memberfunction SetUTCparameters().
// leap : The accumulated number of Leap Seconds corresponding to this date/time.
// dut  : The monitored time difference UT-UTC in seconds.
//
// The value of UT-UTC is kept within 0.9 sec. by the introduction of Leap Seconds into UTC.
// An overview of the accumulated leap seconds is available at :
//                https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
//                     or  http://maia.usno.navy.mil/ser7/tai-utc.dat
// 
// The time difference dUT=UT-UTC is monitored on a daily basis and the data are available at :
//                 https://hpiers.obspm.fr/iers/series/opa/eopc04
//                     or  http://maia.usno.navy.mil/ser7/ser7.dat
//
// The accuracy of the dUT=UT-UTC monitoring is about 10 microseconds.
//
// Notes :
// -------
// 1) In case of invalid input arguments the TAI related time recording is disabled
//    and the values of the Leap Seconds and dut=UT-UTC will be set to zero.
// 2) In case utc="A" and no data files have been loaded, or no information is available,
//    the utc="N" mode will be invoked.
//
// The default values are utc="A", leap=0 and dut=0.

 Double_t jd=GetJD(e,mode);
 SetJD(jd,utc,leap,dut);
}
///////////////////////////////////////////////////////////////////////////
Double_t NcTimestamp::GetEpoch(TString mode)
{
// Provide the corresponding epoch value.
// Via the input argument "mode" the user can specify the type of epoch
//
// mode = "B" ==> Besselian epoch
//        "J" ==> Julian epoch

 Double_t e=0;
 if (mode=="B" || mode=="b") e=GetBE();
 if (mode=="J" || mode=="j") e=GetJE();
 return e;
}
///////////////////////////////////////////////////////////////////////////
TString NcTimestamp::GetDayTimeString(TString mode,Int_t ndig,Double_t offset)
{
// Get a string with date and time info for the date/time system specified via "mode".
//
// The argument "mode" my have the values :
// "UT", "UT1", "UTC", "GAT", "GMST", "GAST", "GPS", "TAI", "TT", "LMT", "LAT", "LMST" or "LAST"
//
// Note that mode="UT" will refer to the main reference time currently in use,
// which is either "UT1" or "UTC".
// For further details see the documentation of SetUT(). 
//
// The argument "ndig" specifies the number of digits for the fractional
// seconds (e.g. ndig=6 corresponds to microsecond precision).
// No rounding will be performed, so a second count of 3.473 with ndig=1
// will appear as 03.4 on the output.
// Due to computer accuracy, precision on the picosecond level may get lost.
//
// offset : Local time offset from UT (and also GMST) in fractional hours.
//
// When an offset value is specified, the corresponding local times
// LMT and LMST (or LAT and LAST) are available as well.
//
// The default values are ndig=0 and offset=0.
//
// Notes :
// -------
// 1) TAI related date/time info (i.e. TAI, UTC, GPS and TT) is only available if
//    UTC parameters were provided.
//    Please refer to the memberfunctions LoadUTCparameterFiles() and GetUTCparameters() for details.
// 2) In case the MJD falls outside the TTimeStamp range, the date info will be omitted.

 Bool_t set=kFALSE;

 TString daytime=mode;
 daytime+=" information unavailable";

 TString month[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
 TString day[7]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

 UInt_t y=0;
 UInt_t m=0;
 UInt_t d=0;
 UInt_t wd=0;
 Int_t hh,mm,ss,ns,ps;
 Double_t s=0;
 ULong64_t sfrac=0;
 Double_t gat=0;
 Double_t gast=0;
 Bool_t date=kFALSE;

 // Equation of Time and Equation of Equinoxes
 Double_t eot; // LAT-LMT
 Double_t eox; // LAST-LMST
 eox=Almanac(0,0,0,0,"Sun",0,0,0,&eot,10);
 // Convert to fractional hours
 eox/=3600.;
 eot/=3600.;

 Int_t mjd,mjsec,mjns;
 GetMJD(mjd,mjsec,mjns);

 // Check whether UT1 is the main reference
 if (mode=="UT1" && fUtc) mode="UT";

 // Check whether UTC is the main reference
 if (mode=="UTC" && !fUtc) mode="UT";

 // UT related information :  UT, GAT, GMST and GAST date and time
 if (mode=="UT" || mode=="GAT" || mode=="GMST" || mode=="GAST")
 {
  if (mjd>=40587 && (mjd<65442 || (mjd==65442 && mjsec<8047)))
  {
   GetDate(kTRUE,0,&y,&m,&d);
   wd=GetDayOfWeek(kTRUE,0);
   date=kTRUE;
  }
  if (mode=="UT") GetUT(hh,mm,ss,ns,ps);
  if (mode=="GMST") GetGMST(hh,mm,ss,ns,ps);
  if (mode=="GAT")
  {
   // Obtain GAT via the Equation of Time as offset
   gat=GetLT(eot);
   Convert(gat,hh,mm,ss,ns,ps);
  }
  if (mode=="GAST")
  {
   gast=GetGAST();
   Convert(gast,hh,mm,ss,ns,ps);
  }

  set=kTRUE;
 }

 // TAI related information : TAI, UTC, GPS and TT date and time
 if (mode=="TAI" || mode=="UTC" || mode=="GPS" || mode=="TT")
 {
  if (fUtc && fUtc!=-3)
  {
   // A dummy timestamp is used to obtain the TAI corresponding date indicator
   NcTimestamp tx;
   tx.SetMJD(fTmjd,fTsec,fTns,fTps);
   if (fTmjd>=40587 && (fTmjd<65442 || (fTmjd==65442 && fTsec<8047)))
   {
    tx.GetDate(kTRUE,0,&y,&m,&d);
    wd=tx.GetDayOfWeek(kTRUE,0);
    date=kTRUE;
   }
   GetTAI(hh,mm,ss,ns,ps,mode);
 
   set=kTRUE;
  }
 }

 // Local time information
 if (mode=="LMT" || mode=="LAT" || mode=="LMST" || mode=="LAST")
 {
  // Determine the new date by including the offset
  NcTimestamp t2(*this);
  t2.Add(offset);
  Int_t mjd2,mjsec2,mjns2;
  t2.GetMJD(mjd2,mjsec2,mjns2);
  if (mjd2>=40587 && (mjd2<65442 || (mjd2==65442 && mjsec2<8047)))
  {
   t2.GetDate(kTRUE,0,&y,&m,&d);
   wd=t2.GetDayOfWeek(kTRUE,0);
   date=kTRUE;
  }
  // Determine the local time by including the offset w.r.t. the original timestamp
  Double_t hlt=0;
  if (mode=="LMT") hlt=GetLT(offset);
  if (mode=="LAT") hlt=GetLT(offset+eot); // Obtain LAT via the Equation of Time as extra offset
  if (mode=="LMST") hlt=GetLMST(offset);
  if (mode=="LAST") hlt=GetLAST(offset);
  Convert(hlt,hh,mm,ss,ns,ps);

  set=kTRUE;
 }

 // No match found with requested date/time system
 if (!set) return daytime;

 // Create the date and time info string
 daytime="";

 // The date string
 if (date)
 {
  daytime=day[wd-1];
  daytime+=" ";
  if (d<10) daytime+="0";
  daytime+=d;
  daytime+="-";
  daytime+=month[m-1];
  daytime+="-";
  daytime+=y;
  daytime+=" ";
 }

 // The time string
 if (hh<10) daytime+="0";
 daytime+=hh;
 daytime+=":";
 if (mm<10) daytime+="0";
 daytime+=mm;
 daytime+=":";
 if (ss<10) daytime+="0";
 daytime+=ss;
 if (ndig>0)
 {
  daytime+=".";
  s=double(ns)*1e-9+double(ps)*1e-12;
  s*=pow(10.,ndig);
  sfrac=ULong64_t(s);
  ULong64_t isfrac=pow(10,ndig-1);
  while (sfrac<isfrac)
  {
   daytime+="0";
   isfrac/=10;
  }
  daytime+=sfrac;
 }
 // Add the time system indicator
 if (mode=="UT")
 {
  mode="UT1";
  if (!fUtc) mode="UTC";
 }
 daytime+=" ";
 daytime+=mode;

 return daytime;
}
///////////////////////////////////////////////////////////////////////////
void NcTimestamp::SetSystemTime()
{
// Set UTC to the current time of the system clock.
//
// In case no database value of dUT=UT1-UTC is available,
// the system clock time is used as the main (UTC) reference.

 // Retrieve the current system clock time
 Set();

 // Store this time as UTC
 FillJulian();
 Int_t ret=SetUTCparameters("A",0,0);

 // Use the UTC parameters (if any) to determine UT1
 if (ret && ret!=-3)
 {
  AddSec(fDut); // Correct UTC for dUT=UT1-UTC to obtain UT1 as main reference
 }
 else
 {
  SetUTCparameters("N",0,0); // Keep the system clock time as main (UTC) reference
 }
 FillTAI();
}
///////////////////////////////////////////////////////////////////////////
Bool_t NcTimestamp::IsUT1() const
{
// Indicate whether the reference time is UT1 (kTRUE) or UTC (kFALSE).

 Bool_t ut1=kTRUE;
 if (!fUtc) ut1=kFALSE;

 return ut1;
}
///////////////////////////////////////////////////////////////////////////
