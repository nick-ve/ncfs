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

// $Id: NcCalcluster.cxx 5 2010-03-19 10:10:02Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class NcCalcluster
// Description of a cluster of calorimeter modules.
// A 2D (matrix) geometry is assumed in which a cluster center is identified
// by two integer indices (i,j), e.g. row and column indicators.
//
// The 1st signal value is the signal of the complete cluster.
// This is the signal which is provided as default by invoking GetSignal().
//
// In case clustering/grouping of module signals was performed over several
// rings around the center (see e.g. NcCalorimeter::Group), the following
// additional information is provided by the various signal values :
//
// The 2nd signal value is the original signal of the central module.
// The 3rd signal value is the total signal within the 1st (i.e. 3x3) ring of
// modules around the cluster center.
// The 4th signal value is the total signal within the 2nd (i.e. 5x5) ring of
// modules around the cluster center.
// Etc....
//
// Note : In case the cluster consists of only 1 module, then only the
//        1st signal value will be present (for obvious reasons).
//
// Some dispersion info about cluster topology is provided in order
// to enable EM or hadronic cluster identification.
//
//--- Author: Nick van Eijndhoven 13-jun-1997 Utrecht University
//- Modified: NvE $Date: 2010-03-19 11:10:02 +0100 (Fri, 19 Mar 2010) $ NCFS
///////////////////////////////////////////////////////////////////////////

#include "NcCalcluster.h"
#include "Riostream.h"
 
ClassImp(NcCalcluster) // Class implementation to enable ROOT I/O
 
NcCalcluster::NcCalcluster() : NcSignal()
{
// Default constructor, all data is set to 0
 fRow=0;
 fCol=0;
 fNmods=0;
 fRowdisp=0.;
 fColdisp=0.;
 fNvetos=0;
 fVetos=0;
 SetName("NcCalcluster [sig, sig11, sig33, sig55,...]");
}
///////////////////////////////////////////////////////////////////////////
NcCalcluster::~NcCalcluster()
{
// Destructor to delete dynamically allocated memory
 if (fVetos)
 {
  delete fVetos;
  fVetos=0;
 }
}
///////////////////////////////////////////////////////////////////////////
NcCalcluster::NcCalcluster(const NcCalcluster& c) : NcSignal(c)
{
// Copy constructor
 fRow=c.fRow;
 fCol=c.fCol;
 fNmods=c.fNmods;
 fRowdisp=c.fRowdisp;
 fColdisp=c.fColdisp;
 fNvetos=c.fNvetos;

 fVetos=new TObjArray();
 fVetos->SetOwner();

 for (Int_t i=1; i<=fNvetos; i++)
 {
  NcSignal* sx=c.GetVetoSignal(i);
  fVetos->Add(new NcSignal(*sx));
 }
}
///////////////////////////////////////////////////////////////////////////
NcCalcluster::NcCalcluster(NcCalmodule& m) : NcSignal()
{
// Cluster constructor with module m as center.
// Module data is only entered for a module which contains a signal,
// has not been used in a cluster yet, and is not declared dead.
//
// Note :
// It is advised NOT to start a cluster with modules situated at a detector edge.
// This feature is automatically checked when using the built-in clustering
// of NcCalorimeter.  

 Nc3Vector r;

 Float_t sig=m.GetClusteredSignal();

 if (sig>0. && m.GetDeadValue()==0)
 {
  fRow=m.GetRow();
  fCol=m.GetColumn();
  r=m.GetPosition();
  SetPosition(r);
  sig=m.GetSignal(1,1); // Use the gain etc... corrected module signal
  SetSignal(sig);
  fNmods=1;
  fRowdisp=0.;
  fColdisp=0.;
  m.SetClusteredSignal(0.); // mark module as used in cluster
  fNvetos=0;
  fVetos=0;
 }
 else
 {
  fRow=0;
  fCol=0;
  SetPosition(r);
  fNmods=0;
  fRowdisp=0.;
  fColdisp=0.;
  fNvetos=0;
  fVetos=0;
 }
 SetName("NcCalcluster [sig, sig11, sig33, sig55,...]");
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCalcluster::GetRow() const
{
// Provide the row number of the cluster center
 return fRow;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCalcluster::GetColumn() const
{
// Provide the column number of the cluster center
 return fCol;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCalcluster::GetNmodules() const
{
// Provide the number of modules in the cluster
 return fNmods;
}
///////////////////////////////////////////////////////////////////////////
Float_t NcCalcluster::GetRowDispersion() const
{
// Provide the normalised row dispersion of the cluster.
 Float_t sig=GetSignal();
 if (sig > 0.)
 {
  return fRowdisp/sig;
 }
 else
 {
  return 0.;
 }
}
///////////////////////////////////////////////////////////////////////////
Float_t NcCalcluster::GetColumnDispersion() const
{
// Provide the normalised column dispersion of the cluster
 Float_t sig=GetSignal();
 if (sig > 0.)
 {
  return fColdisp/sig;
 }
 else
 {
  return 0.;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcCalcluster::Start(NcCalmodule& m)
{
// Reset cluster data and start with module m.
// A module can only start a cluster when it contains a signal,
// has not been used in a cluster yet, and is not declared dead.
//
// Note :
// It is advised NOT to start a cluster with modules situated at a detector edge.
// This feature is automatically checked when using the built-in clustering
// of NcCalorimeter.  

 NcSignal::Reset();

 Nc3Vector r;

 if (m.GetClusteredSignal()>0. && m.GetDeadValue()==0)
 {
  fRow=m.GetRow();
  fCol=m.GetColumn();
  r=m.GetPosition();
  SetPosition(r);
  SetSignal(m.GetSignal(1,1)); // Use the gain etc... corrected module signal
  fNmods=1;
  fRowdisp=0.;
  fColdisp=0.;
  m.SetClusteredSignal(0.); // mark module as used in cluster
 }
 else
 {
  fRow=0;
  fCol=0;
  SetPosition(r);
  fNmods=0;
  fRowdisp=0.;
  fColdisp=0.;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcCalcluster::Add(NcCalmodule& m)
{
// Add module data to the cluster.
// Dead modules are NOT added to the cluster.
// According to the distance of the module w.r.t. the cluster center
// the various signal values are updated.

 if (fNmods)
 {
  if (m.GetClusteredSignal()>0. && m.GetDeadValue()==0) // only add unused modules
  {
   Float_t sigm=m.GetSignal(1,1); // Use the gain etc... corrected module signal

   Int_t drow=int(fabs(double(GetRow()-m.GetRow())));       // row distance to center
   Int_t dcol=int(fabs(double(GetColumn()-m.GetColumn()))); // column distance to center 

   // Determine the ring index for this module around the cluster center
   Int_t jring=drow;
   if (dcol>drow) jring=dcol;

   Int_t nvalues=GetNvalues();

   if ((jring+2)<=nvalues) // Module within existing ring(s) ==> Add module signal to the enclosing ring(s)
   {
    for (Int_t i=(jring+2); i<=nvalues; i++)
    {
     AddSignal(sigm,i);
    }
   }
   else  // Module outside all existing rings ==> Init. new ring signals with existing enclosed signal(s)
   {
    for (Int_t j=(nvalues+1); j<=(jring+2); j++)
    {
     SetSignal(GetSignal(j-1),j);
    }
    // Add current module signal to the signal value for the corresponding ring
    AddSignal(sigm,(jring+2));
   }

   // Update total cluster signal
   AddSignal(sigm);
 
   fNmods+=1;
   fRowdisp+=sigm*float(drow*drow);
   fColdisp+=sigm*float(dcol*dcol);
   m.SetClusteredSignal(0.); // mark module as used in cluster
  }
 }
 else
 {
  cout << " *NcCalcluster::Add* No action. Cluster should be started first."
       << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void NcCalcluster::AddVetoSignal(NcSignal& s,Int_t extr)
{
// Associate an (extrapolated) NcSignal as veto to the cluster.
// By default a straight line extrapolation is performed which extrapolates
// the signal position until the length of its position vector matches that
// of the position vector of the cluster.
// In this extrapolation procedure the error propagation is performed 
// automatically.  
// Based on the cluster and extrapolated veto signal (x,y) positions and
// position errors the confidence level of association is calculated
// and stored as an additional signal value.
// By means of the GetVetoSignal memberfunction the confidence level of
// association can always be updated by the user.
// In case the user wants to invoke a more detailed extrapolation procedure,
// the automatic extrapolation can be suppressed by setting the argument
// extr=0. In this case it is assumed that the NcSignal as entered via
// the argument contains already the extrapolated position vector and
// corresponding errors. 
// Note : Three additional values are added to the original NcSignal
//        to hold the chi2, ndf and confidence level values of the association. 
 if (!fVetos)
 {
  fNvetos=0;
  fVetos=new TObjArray();
  fVetos->SetOwner();
 } 

 Int_t nvalues=s.GetNvalues();
 NcSignal* sx=new NcSignal(s); // Additional values will be added
 TString name=s.GetName();
 name.Append(" + additional chi2, ndf and CL values");
 sx->SetName(name);

 Double_t vecc[3],vecv[3];
 if (extr)
 {
  // Extrapolate the veto hit position
  Double_t scale=1;
  GetPosition(vecc,"sph");
  s.GetPosition(vecv,"sph"); 
  if (vecv[0]) scale=vecc[0]/vecv[0];
  Nc3Vector r=s*scale;
  sx->SetPosition(r);
 }

 // Calculate the confidence level of association
 GetPosition(vecc,"car");
 sx->GetPosition(vecv,"car"); 
 Double_t dx=vecc[0]-vecv[0];
 Double_t dy=vecc[1]-vecv[1];
 GetPositionErrors(vecc,"car");
 sx->GetPositionErrors(vecv,"car"); 
 Double_t sxc2=vecc[0]*vecc[0];
 Double_t syc2=vecc[1]*vecc[1];
 Double_t sxv2=vecv[0]*vecv[0];
 Double_t syv2=vecv[1]*vecv[1];
 Double_t sumx2=sxc2+sxv2;
 Double_t sumy2=syc2+syv2;
 Double_t chi2=0;
 if (sumx2>0 && sumy2>0) chi2=(dx*dx/sumx2)+(dy*dy/sumy2);
 Int_t ndf=2;
 NcMath m;
 Double_t prob=m.Prob(chi2,ndf);
 if (chi2>0) sx->SetSignal(chi2,nvalues+1);
 if (ndf>0) sx->SetSignal(ndf,nvalues+2);
 if (prob>0) sx->SetSignal(prob,nvalues+3);

 fVetos->Add(sx);
 fNvetos++;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCalcluster::GetNvetos() const
{
// Provide the number of veto signals associated to the cluster
 return fNvetos;
}
///////////////////////////////////////////////////////////////////////////
NcSignal* NcCalcluster::GetVetoSignal(Int_t i) const
{
// Provide access to the i-th veto signal of this cluster.
// Note : The first hit corresponds to i=1.
 if (!fVetos)
 {
  cout << " *NcCalcluster::GetVetoSignal* No veto signals present." << endl;
  return 0;
 }
 else
 {
  if (i>0 && i<=fNvetos)
  {
   return (NcSignal*)fVetos->At(i-1);
  }
  else
  {
   cout << " *NcCalcluster::GetVetoSignal* Signal number " << i << " out of range."
        << " Nvetos = " << fNvetos << endl;
   return 0;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
Float_t NcCalcluster::GetVetoLevel() const
{
// Provide the confidence level of best associated veto signal.
 Float_t cl=0;
 Float_t clmax=0;
 NcSignal* s=0;
 Int_t nvalues=0;
 if (fVetos)
 {
  for (Int_t i=0; i<fNvetos; i++)
  {
   s=((NcSignal*)fVetos->At(i));
   if (s)
   {
    nvalues=s->GetNvalues();
    cl=s->GetSignal(nvalues);
    if (cl>clmax) clmax=cl;
   }
  }
 }
 return clmax;
}
///////////////////////////////////////////////////////////////////////////
Int_t NcCalcluster::HasVetoHit(Double_t cl) const
{
// Investigate if cluster has an associated veto hit with conf. level > cl.
// Returns 1 if there is such an associated veto hit, otherwise returns 0.
// Note : This function is faster than GetVetoLevel().
 NcSignal* s=0;
 Int_t nvalues=0;
 if (fVetos)
 {
  for (Int_t i=0; i<fNvetos; i++)
  {
   s=((NcSignal*)fVetos->At(i));
   if (s)
   {
    nvalues=s->GetNvalues();
    if (s->GetSignal(nvalues) > cl) return 1;
   }
  }
 }
 return 0;
}
///////////////////////////////////////////////////////////////////////////
