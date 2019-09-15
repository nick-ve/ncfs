#ifndef NcVersion_h
#define NcVersion_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcVersion.h 5 2010-03-19 10:10:02Z nickve $

#include "TObject.h"
#include "TString.h"
 
class NcVersion : public TObject
{
 public:
  NcVersion();                                  // Default constructor
  virtual ~NcVersion();                         // Destructor
  NcVersion(const NcVersion& v);                // Copy constructor
  NcVersion& operator=(const NcVersion& q);     // Set equal to another NcVersion instance
  virtual void Data() const;                    // Provide version information

 protected:
  TString fVersion; // The actual version number of this release
  TString fRev;     // SVN Revision of this version
  TString fDate;    // Creation date of this version

 ClassDef(NcVersion,1) // Facility to provide version information
};
#endif
