#ifndef NcVersion_h
#define NcVersion_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

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
  TString GetVersion() const;                   // Provide the version string

 protected:
  TString fVersion; // The actual version number of this release
  TString fRev;     // Revision identifier of this version
  TString fDate;    // Creation date of this version

 ClassDef(NcVersion,3) // Facility to provide version information
};
#endif
