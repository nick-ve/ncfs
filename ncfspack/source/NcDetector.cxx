/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright(c) 1997-2021, NCFS/IIHE, All Rights Reserved.                     *
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
// Class NcDetector
// Creation and investigation of an NCFS generic detector structure.
//
// This class provides a facility to build up a hierarchical structure
// to store detector signals.
// Detector signals are represented by NcSignal objects that belong to
// NcDevice objects. An Ncsignal object that is related to an NcDevice
// is called a "hit" and can be stored in the NcDevice via the
// generic AddHit memberfunction. 
// A group of NcDevice objects may be combined in an NcDetectorUnit object
// to represent a certain sub-detector system, and several NcDetectorUnit
// objects may finally be combined into an NcDetector object to represent
// the complete experimental setup.
// Combination of these various detector elements is performed by the
// generic AddDevice memberfunction.
//
// Since the goal of recording experimental data is to investigate
// certain (astro)physical phenomena, the occurrence of a sudden
// observation of signals in the detector is called an event.
// To store the event data in an organized way, the class NcEvent is provided,
// in which the various detector structures and their recorded signals
// can be stored via the generic AddDevice memberfunction.
//
// It is good practice to combine the complete detector structure in
// a single NcDetector object and then store the whole detector structure
// in an NcEvent sructure by a single AddDevice invokation.
// This will in general provide the most efficient way to retrieve
// and analyze the recorded data. 
// However, it is also possible to use multiple AddDevice invokations
// of NcEvent to store every NcDevice individually if the user wants
// to do so.
//
// In order to tailor the performance of the various detector elements
// with specific (sub)detector characteristics, the user may extend the
// functionality of the standard classes mentioned above, by introducing
// new classes which are derived from the standard ones.
// See the examples below to illustrate this. 
//
// In addition to the detector structures and their recorded signals,
// the NcEevent class also provides facilities to store reconstructed
// objects like tracks (NcTrack), jets (NcJet) and vertices (NcVertex)
// together with timing information (NcTimestamp).   
//
// Navigation through the detector structures to obtain the contained data
// may be performed via the various memberfunctions of the classes
// NcDetector, NcDetectorUnit, NcDevice, NcSignal and also NcEevent.
//
// Examples :
// ==========
//
// The IceCube Neutrino Observatory at the South Pole :
// ----------------------------------------------------
// The IceCube detector consists of more than 5000 Digital Optical Modules (DOMs)
// that are buried deep in the Antarctic ice in a hexagonal grid.
// At the deepest central part of the detector, additional DOMs have been added
// to form a denser core dubbed DeepCore.
// In addition to this, there are also DOMs located at the ice surface called IceTop.
// For the storage and analysis of the IceCube data, the following hierarchical
// detector structure could be envisaged :
// * Every DOM is represented via an NcDevice.
// * The IceTop DOMs are grouped into an NcDetectorUnit object named "IceTop".
// * All DOMs that are buried under the surface are grouped into an
//   NcDetectorUnit object named "InIce".
// * The DeepCore DOMs are grouped into an NcDetectorUnit object named "DeepCore".
//   Note here that DeepCore DOMs also belong to the InIce part, but this can
//   be handled by the introduction of new classes derived from NcDevice,
//   like for instance IceIDOM (derived from NcDevice) to represent a generic
//   InIce DOM, IceDCDOM (derived from IceIDOM) to represent a DeepCore DOM
//   and IceICDOM (derived from IceIDOM) to represent an InIce DOM not belonging to DeepCore.
// * Finally the IceTop, InIce and DeepCore detector units are grouped
//   into the overall NcDetector named "IceCube".
//
// The IceCube data analysis framework is available in the directory "icepack".
// Note that in this framework also a new class IceEvent (derived from NcEvent)
// has been introduced to provide specific IceCube related functionality.
//
// The Radio Neutrino Observatory in Greenland (RNO-G) :
// -----------------------------------------------------
// The RNO-G detector array is foreseen to consist of 35 so called detector stations,
// each of which consists of 3 vertical strings deep in the ice equipped with antennas.
// These antennas come in two varieties, namely horizontally polarized (HPOL)
// and vertically polarized (VPOL).
// In addition to this, there are also LogPeriodic Dipole Antennas (LPDAs)
// located in 3 trenches at the surface.
// For the storage and analysis of the RNO-G data, the following hierarchical
// detector structure could be envisaged :
// * Every antenna is represented via an NcDevice.
//   To distinguish the different antenna types, new classes (derived from NcDevice)
//   are introduced, namely RnoHPOL, RnoVPOL and RnoLPDA.
// * To represent the various strings, a new class RnoString (derived from NcDetectorUnit)
//   is introduced.
// * To represent the various trenches, a new class RnoTrench (derived from NcDetectorUnit)
//   could be introduced.
//   Although a trench is not an electronically connected entity like a string, and the LPDAs
//   are foreseen to perform as individual devices, this structure may have some benefits
//   in the data analysis phase.
// * All RnoHPOL and RnoVPOL objects are grouped in the corresponding RnoString objects.
// * The RnoLPDA objects are grouped in the corresponding RnoTrench objects.
// * The 3 RnoString objects and the 3 RnoTrench objects are grouped in a RnoStation object
//   that is derived from NcDetectorUnit.
// * Finally all 35 RnoStation objects are grouped into the overall NcDetector named "RNO".
//
// The RNO-G data analysis framework is available in the directory "rnopack".
// Note that in this framework also a new class RnoEvent (derived from NcEvent)
// has been introduced to provide specific RNO-G related functionality.
// 
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, June 25, 2021  07:39Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, June 27, 2021  09:59Z
///////////////////////////////////////////////////////////////////////////

#include "NcDetector.h"
#include "Riostream.h"
 
ClassImp(NcDetector) // Class implementation to enable ROOT I/O
 
NcDetector::NcDetector(const char* name,const char* title) : NcDetectorUnit(name,title)
{
// Default constructor.
}
///////////////////////////////////////////////////////////////////////////
NcDetector::~NcDetector()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
NcDetector::NcDetector(const NcDetector& q) : NcDetectorUnit(q)
{
// Copy constructor.
}
///////////////////////////////////////////////////////////////////////////
TObject* NcDetector::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.

 NcDetector* q=new NcDetector(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
