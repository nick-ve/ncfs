/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2009, NCFS, All Rights Reserved.                          *
 *                                                                             *
 * Author: The Netherlands Center for Fundamental Studies (NCFS).              *
 *         http://sites.google.com/site/ncfsweb ncfs.nl@gmail.com              *
 *                                                                             *
 * Contributors are mentioned in the code where appropriate.                   *
 *                                                                             * 
 * No part of this software may be used, copied, modified or distributed       *
 * by any means nor transmitted or translated into machine language without    *
 * written permission by the NCFS.                                             *
 * Permission to use the documentation strictly for non-commercial purposes    *
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

// $Id: NcVersion.cxx 117 2016-04-12 22:13:22Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcVersion
// Facility to provide version information of the current release.
// Instead of using SVN tags, which may get a bit complex to manage in the
// case of various related sub-packages like e.g. IcePack, the Revision
// of the total repository is used as a reference instead.
// Invokation of NcVersion::Data() will always indicate a.o. the Revision of
// a specific release. In case one wants a previous release, just go back
// to one of the previous versions of NcVersion, check what the corresponding
// Revision is and check-out that specific Revision from the repository for
// all packages.
// This will always provide you with an integral working environment.
//
// As usual the version number consists of a major and minor version indicator.
// Major version updates indicate changes that may have impact on the actual
// coding of other (sub)packages, whereas minor version updates only indicate
// improvements or extensions that are fully backward compatible.
//
//--- Author: Nick van Eijndhoven 24-sep-2009 NCFS
//- Modified: NvE $Date: 2016-04-13 00:13:22 +0200 (Wed, 13 Apr 2016) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "NcVersion.h"
#include "Riostream.h"
 
ClassImp(NcVersion) // Class implementation to enable ROOT I/O
 
NcVersion::NcVersion() : TObject()
{
// Default constructor to initialise all parameters
// Latest update 13-apr-2016 by Nick van Eijndhoven, IIHE-VUB Brussels
 fVersion="2.1";
 fRev="$Revision: 117 $";
 fDate="$Date: 2016-04-13 00:13:22 +0200 (Wed, 13 Apr 2016) $";

 // Remove the $ signs from the SVN keywords
 fRev.ReplaceAll("$","");
 fDate.ReplaceAll("$","");
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
 cout << " *" << ClassName() << "* Version information." << endl;
 cout << " NCFS-Pack version : " << fVersion << " corresponding to SVN " << fRev << endl;
 cout << " Created at " << fDate << endl; 
}
///////////////////////////////////////////////////////////////////////////
