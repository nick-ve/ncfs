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
// Class NcVersion
// Facility to provide version information of the current release.
// Instead of using separate branches and tags for the various related
// sub-packages like e.g. IcePack, which may complicate the management
// of the complete NCFS framework, the GIT revision tag identifier of
// the Master branch of the total repository is used as a reference instead.
// Invokation of NcVersion::Data() will always indicate a.o. the version of
// a specific release. In case one wants a previous release, just select
// the desired version (as provided by NcVersion::Data()), check what the
// corresponding GIT revision tag is, and "pull" that specific revision tag
// from the GIT repository for all packages.
// This will always provide you with an integral working environment.
//
// The current version in the master branch may not yet have a GIT revision tag,
// since development may still be going on before a version is frozen and released.
// However, the master branch will always provide an integral working environment.
//
// As usual the version number consists of a major and minor version indicator.
// Major version updates indicate changes that may have impact on the actual
// coding of other (sub)packages, whereas minor version updates only indicate
// improvements or extensions that are fully backward compatible.
//
//--- Author: Nick van Eijndhoven 24-sep-2009 NCFS
//- Modified: Nick van Eijndhoven, IIHE-VUB Brussel, July 1, 2021  21:54Z
///////////////////////////////////////////////////////////////////////////

#include "NcVersion.h"
#include "Riostream.h"
 
ClassImp(NcVersion) // Class implementation to enable ROOT I/O
 
NcVersion::NcVersion() : TObject()
{
// Default constructor to initialise all parameters
 fVersion="4.2";
 fRev="Branch: master";
 fDate="July 11, 2021  01:26 UTC";
}
///////////////////////////////////////////////////////////////////////////
NcVersion::~NcVersion()
{
// Default destructor
}
///////////////////////////////////////////////////////////////////////////
NcVersion::NcVersion(const NcVersion& v) : TObject(v)
{
// Copy constructor
 fVersion=v.fVersion;
 fRev=v.fRev;
 fDate=v.fDate;
}
///////////////////////////////////////////////////////////////////////////
NcVersion& NcVersion::operator=(const NcVersion& q)
{
// Set the current version attributes equal to version instance q.
 fVersion=q.fVersion;
 fRev=q.fRev;
 fDate=q.fDate;
  
 return *this;
}
///////////////////////////////////////////////////////////////////////////
void NcVersion::Data() const
{
// Provide the version information for this release.
 cout << endl;
 cout << " *" << ClassName() << "* Version information." << endl;
 cout << " NCFS-Pack version : " << fVersion << " corresponding to GIT " << fRev << endl;
 cout << " Created at " << fDate << endl;
 cout << endl;
}
///////////////////////////////////////////////////////////////////////////
