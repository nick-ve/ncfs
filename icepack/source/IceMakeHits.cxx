/*******************************************************************************
 * Copyright(c) 2003, IceCube Experiment at the South Pole. All rights reserved.
 *
 * Author: The IceCube NCFS-based Offline Project.
 * Contributors are mentioned in the code where appropriate.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation strictly for non-commercial purposes is hereby granted
 * without fee, provided that the above copyright notice appears in all
 * copies and that both the copyright notice and this permission notice
 * appear in the supporting documentation.
 * The authors make no claims about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *******************************************************************************/

// $Id: IceMakeHits.cxx 57 2012-02-07 10:05:54Z nickve $

///////////////////////////////////////////////////////////////////////////
// Class IceMakeHits
// TTask derived class to perform hit extraction from waveforms.
//
// In case an event has been rejected by an NcEventSelector (based) processor,
// this task (and its sub-tasks) is not executed.
//
// The code in this processor is based on the algorithms as developed by
// Nick van Eijndhoven and Garmt de Vries-Uiterweerd (Utrecht University, The Netherlands).
// The parameters have been tuned for IceCube waveforms by Dennis Diederix (IIHE, Brussels).
// Further tuning and automatic progression of parameters introduced by Nick van Eijndhoven (IIHE, Brussels). 
//
// Procedure applied for Amanda TWR data :
// ---------------------------------------
//
// 1) The waveform is fed to a TSpectrum object, and the peak locations 
//    are determined with the TSpectrum::Search() function.
//
// 2) The waveform is divided into regions corresponding to the peaks found by 
//    TSpectrum. The region boundary between two peaks is at the location of 
//    the minimum between the two peaks. 
//
// 3) For each region the "effective baseline" (used in e.g. the
//    evaluation of the leading edge value) is determined as :
//    AMANDA  : effective baseline = fBasefracXXX * value at lower region boundary
//    IceCube : effective baseline = previous baseline + fBasefracXXX * (value at lower region boundary - previous baseline).
//              In case the "value at lower region boundary" is below the overall baseline, the pulses are obviously
//              well separated and the effective baseline is set to the overall baseline again.
//    This takes into account the effect from the previous pulse.
//    For the first pulse, the effective baseline is equal to the overall 
//    baseline.
//
// 4) For each region, the point of steepest rise between the lower region
//    boundary and the peak location is determined. The tangent at this point
//    is extrapolated to the effective baseline. The point of intersection yields the
//    leading edge.
//
// 5) For each region the range of charge integration is determined as :
//    - Start of integration at the lower region boundary or at the leading edge,
//      whichever comes last.
//    - End of integration at the upper region boundary or at the point where the
//      signal drops below the overall baseline, whichever comes first.
//
// 6) For each region the integrated charge is determined as :
//    Sum over bins in integration range of (value in bin - overall baseline).
//    Note : For IceCube the effective baseline is used in the charge integration.
//
// 7) For each pulse the quality is evaluated by requiring that :
//    peak location - lower region boundary > lower region boundary - leading edge.
//    For a too shallow steepest rise, the leading edge value is unreliable, in 
//    which case the pulse is merged with the previous pulse. 
// 
// 8) Each pulse is checked for saturation and discarded if necessary.
//    Note : For IceCube only the RAW waveforms are checked for saturation.
//
// 9) TSpectrum needs a minimum number of bins for its Search function, otherwise
//    the clipping window is too large, which causes an error. If a waveform does
//    not contain enough bins, the following alternative approach is used : 
//    - A loop over all bins is performed.
//    - As soon as the signal exceeds a given threshold, a pulse is started.
//    - The pulse ends when the signal drops below the threshold again.
//    - While looping, the charge is integrated for each pulse.
//
// The defaults of the various parameters can be changed by the corresponding
// Set memberfunctions.
//
// Procedure applied for IceCube waveform (ATWD and/or FADC) data :
// ----------------------------------------------------------------
//
// The procedure for ATWD/FADC data is essentially the same as for TWR data.
// However, an extension for IceCube is that the baseline of the waveform is
// determined automatically from the waveform itself via the median of all the
// signals below 20% of the maximum signal w.r.t. a first approximation of the baseline.
// The latter is obtained by just taking the median of all the signals in the waveform.
// This baseline value is also stored as a slot value in the corresponding DOM device.
// All relevant parameters can be set independently.
// 
// Information about the actual parameter settings can be found in the event
// structure itself via the device named "IceMakeHits".
//
//--- Author: Nick van Eijndhoven and Garmt de Vries-Uiterweerd 15-jan-2007 Utrecht University
//- Modified: NvE $Date: 2012-02-07 11:05:54 +0100 (Tue, 07 Feb 2012) $ NCFS
///////////////////////////////////////////////////////////////////////////
 
#include "IceMakeHits.h"
#include "Riostream.h"

#include "TCanvas.h"

ClassImp(IceMakeHits) // Class implementation to enable ROOT I/O

IceMakeHits::IceMakeHits(const char* name,const char* title) : TTask(name,title)
{
// Default constructor.
 fEvt=0;
 // Parameters for Amanda TWR hit extraction
 fBasefracA=0.5;
 fSigmaA=1.5;
 fMaxPeaksA=10;
 fMinPulseHeightA=50;
 fThresholdA=0.2;
 // Parameters for IceCube ATWD/FADC hit extraction
 fBasefracI=0.5;
 fSigmaI=1.+1.e-6;
 fMaxPeaksI=100;
 fPeakAcceptanceLevelI=5;
 fMinPulseHeightI=1.e-12;
 fThresholdI=0.2;
 fUseNamesI=0;
}
///////////////////////////////////////////////////////////////////////////
IceMakeHits::~IceMakeHits()
{
// Default destructor.
 if (fUseNamesI)
 {
  delete fUseNamesI;
  fUseNamesI=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetBasefracA(Float_t val)
{
// Set baseline fractional update for Amanda TWR extraction.
// The default as set in the constructor of this class is 0.5.
 fBasefracA=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetSigmaA(Float_t val)
{
// Set clipping window width for Amanda TWR extraction.
// The default as set in the constructor of this class is 1.5.
 fSigmaA=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetMaxPeaksA(Int_t val)
{
// Set maximum number of peaks in a waveform for Amanda TWR extraction.
// The default as set in the constructor of this class is 10.
 fMaxPeaksA=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetMinPulseHeightA(Float_t val)
{
// Set minimum required pulse height for Amanda TWR extraction.
// This is used only for narrow pulses that cannot be handled with TSpectrum.
// The default as set in the constructor of this class is 50.
 fMinPulseHeightA=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetThresholdA(Float_t val)
{
// Set threshold for use in analysis of narrow pulses for Amanda TWR extraction.
// A peak is assumed to start when the signal rises above threshold*maxval,
// where maxval is the maximum value found in the waveform.
// The default as set in the constructor of this class is 0.2.
 fThresholdA=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetBasefracI(Float_t val)
{
// Set baseline fractional update for IceCube waveform hit extraction.
// The default as set in the constructor of this class is 0.5.
 fBasefracI=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetSigmaI(Float_t val)
{
// Set clipping window width for IceCube waveform hit extraction.
// The default as set in the constructor of this class is 2.
 fSigmaI=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetMaxPeaksI(Int_t val)
{
// Set maximum number of peaks in a waveform for IceCube hit extraction.
// The default as set in the constructor of this class is 10.
 fMaxPeaksI=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetPeakAcceptanceLevelI(Float_t val)
{
// Set the IceCube minimum height of a peak above the baseline, in terms of baseline spread.
// The default as set in the constructor of this class is 5.
 fPeakAcceptanceLevelI=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetMinPulseHeightI(Float_t val)
{
// Set minimum required pulse height for IceCube waveform hit extraction.
// This is used only for narrow pulses that cannot be handled with TSpectrum.
// The default as set in the constructor of this class is 0.
 fMinPulseHeightI=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetThresholdI(Float_t val)
{
// Set threshold for use in analysis of narrow pulses for IceCube waveform hit extraction.
// A peak is assumed to start when the signal rises above threshold*maxval,
// where maxval is the maximum value found in the waveform.
// The default as set in the constructor of this class is 0.2.
 fThresholdI=val;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::SetWaveformNameI(const char* name)
{
// Set the name of a specific IceCube waveform to be analysed.
//
// Notes :
// -------
// 1) Invokation of this memberfunction allows to only specify 1 single 
//    waveform name (pattern).
//    Previously specified name patterns will be overwritten.
// 2) This memberfunction is kept for backward compatibility.
//    You are advised to use UseWaveformNameI() instead for more flexibility.
// 3) The search for the corresponding waveform will be performed on basis of
//    a pattern name matching, so the functionality is like searching with "wildcards".
//    However, the pattern needs to match exactly, i.e. the search is case sensitive.

 if (fUseNamesI)
 {
  delete fUseNamesI;
  fUseNamesI=0;
 }
 
 TString s=name;
 UseWaveformNameI(s);
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::UseWaveformNameI(TString name)
{
// Specification of the name(s) of the IceCube waveforms to be analysed.
//
// Consecutive invokations of this memberfunction with different names
// will result in an incremental effect.
//
// Example :
// ---------
// UseWaveformNameI("CAL-ATWD");
// UseWaveformNameI("CAL-FADC");
// UseWaveFormNameI("RAW-ATWD0");
//
// This will analyse the waveforms "CAL-ATWD", "CAL-FADC" and "RAW-ATWD0" for each
// DOM encountered in the event structure.
//
// Note :
// ------
// The search for the corresponding waveform will be performed on basis of
// a pattern name matching, so the functionality is like searching with "wildcards".
// For example, UseWaveformNameI("ATWD") will select all the waveforms which have the
// pattern "ATWD" in their name.
// However, the pattern needs to match exactly, i.e. the search is case sensitive.

 if (!fUseNamesI)
 {
  fUseNamesI=new TObjArray();
  fUseNamesI->SetOwner();
 }
 
 // Check if this name (or pattern) has already been specified before.
 // In case the new name pattern is more generic than matching existing ones
 // it will replace these already existing patterns.
 TString s;
 Int_t nen=fUseNamesI->GetEntries();
 for (Int_t i=0; i<nen; i++)
 {
  TObjString* sx=(TObjString*)fUseNamesI->At(i);
  if (!sx) continue;
  s=sx->GetString();

  // Exact match or new name is less generic than existing one --> Keep existing one
  if (name==s || name.Contains(s)) return;

  // New name (pattern) is more generic than existing one --> Remove existing one
  if (s.Contains(name))
  {
   sx=(TObjString*)fUseNamesI->RemoveAt(i);
   if (sx) delete sx;
  }
 }

 // New name (pattern) to be added into the storage
 fUseNamesI->Compress(); 
 nen=fUseNamesI->GetEntries();
 if (nen >= fUseNamesI->GetSize()) fUseNamesI->Expand(nen+1);
 
 TObjString* newname=new TObjString();
 newname->SetString(name.Data());
 fUseNamesI->Add(newname);
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::PrintWaveformNamesI() const
{
// Print the name (pattern) of all the InIce waveforms which will be analysed.

 cout << " *IceMakeHits::PrintWaveformNamesI* InIce waveform names (or patterns) that will be analysed." << endl;
 if (!fUseNamesI)
 {
  cout << "  No name (pattern) has been specified." << endl;
 }
 else
 {
  TString s;
  Int_t nen=fUseNamesI->GetEntries();
  for (Int_t i=0; i<nen; i++)
  {
   TObjString* sx=(TObjString*)fUseNamesI->At(i);
   if (!sx) continue;
   s=sx->GetString();
   cout << "  " << s.Data() << endl;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::Exec(Option_t* opt)
{
// Implementation of the feature extraction.

 TString name=opt;
 NcJob* parent=(NcJob*)(gROOT->GetListOfTasks()->FindObject(name.Data()));

 if (!parent) return;

 fEvt=(IceEvent*)parent->GetObject("IceEvent");
 if (!fEvt) return;

 // Only process accepted events
 NcDevice* seldev=(NcDevice*)fEvt->GetDevice("NcEventSelector");
 if (seldev)
 {
  if (seldev->GetSignal("Select") < 0.1) return;
 }

 // Storage of the used parameters in the IceMakeHits device
 NcDevice params;
 params.SetNameTitle("IceMakeHits","IceMakeHits processor parameters");

 // Amanda hit extraction
 params.AddNamedSlot("BasefracA");
 params.AddNamedSlot("SigmaA");
 params.AddNamedSlot("MaxPeaksA");
 params.AddNamedSlot("MinPulseHeightA");
 params.AddNamedSlot("ThresholdA");
 params.SetSignal(fBasefracA,"BasefracA");
 params.SetSignal(fSigmaA,"SigmaA");
 params.SetSignal(fMaxPeaksA,"MaxPeaksA");
 params.SetSignal(fMinPulseHeightA,"MinPulseHeightA");
 params.SetSignal(fThresholdA,"ThresholdA");

 // IceCube hit extraction
 params.AddNamedSlot("BasefracI");
 params.AddNamedSlot("SigmaI");
 params.AddNamedSlot("MaxPeaksI");
 params.AddNamedSlot("MinPulseHeightI");
 params.AddNamedSlot("ThresholdI");
 params.SetSignal(fBasefracI,"BasefracI");
 params.SetSignal(fSigmaI,"SigmaI");
 params.SetSignal(fMaxPeaksI,"MaxPeaksI");
 params.SetSignal(fMinPulseHeightI,"MinPulseHeightI");
 params.SetSignal(fThresholdI,"ThresholdI");

 fEvt->AddDevice(params);

 Int_t oldlevel=gErrorIgnoreLevel;
 gErrorIgnoreLevel=kFatal; // Suppress all (TSpectrum) error and warning messages

 Amanda();
 IceCube();
// InIce();
// DeepCore();
// IceTop();

 gErrorIgnoreLevel=oldlevel; // Re-activate previous info level

}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::Amanda()
{
// Hit extraction from the Amanda TWR data.

 // All Amanda OMs with a signal
 TObjArray* aoms=fEvt->GetDevices("IceAOM");
 if (!aoms) return;

 // Arrays for storing info
 Float_t* baseline=new Float_t[fMaxPeaksA];
 Int_t* lowend=new Int_t[fMaxPeaksA];
 Int_t* upend=new Int_t[fMaxPeaksA];
 Int_t* startcharge=new Int_t[fMaxPeaksA];
 Int_t* stopcharge=new Int_t[fMaxPeaksA];
 Int_t* status=new Int_t[fMaxPeaksA]; // 0=OK, 1=rejected, 2=saturation
 Float_t* leadingedge=new Float_t[fMaxPeaksA];
 Float_t* charge=new Float_t[fMaxPeaksA];
 Float_t* tot=new Float_t[fMaxPeaksA];

 // Some objects and variables we will need
 TH1F htemp, diff;
 TSpectrum spec(fMaxPeaksA);
 Int_t nrIterations=(Int_t)(7*fSigmaA+0.5); // Number of iterations used in TSpectrum::SearchHighRes()
 Int_t npeaks=0, ibin=0, lookforsteepestuntilbin=0, steep=0;
 Float_t maxval=0, rise=0, rc=0, yyy=0;
 Bool_t pulsegoingon=false;
 Int_t* index=new Int_t[fMaxPeaksA];

 // OM, waveform and hit
 IceAOM* omx=0;
 TH1F* wf=0;
 NcSignal hit;
 hit.SetSlotName("ADC",1);
 hit.SetSlotName("LE",2);
 hit.SetSlotName("TOT",3);

 // Loop over all fired OMs and extract the hit info
 for (Int_t iom=0; iom<aoms->GetEntries(); iom++)
 {
  omx=(IceAOM*)aoms->At(iom);
  if (!omx) continue;
  // Remove all existing hits of this OM 
  omx->RemoveHits();
  // Reset (de)calibration functions to indicate uncalibrated data
  omx->SetCalFunction(0,"ADC");
  omx->SetCalFunction(0,"LE");
  omx->SetCalFunction(0,"TOT");
  omx->SetDecalFunction(0,"ADC");
  omx->SetDecalFunction(0,"LE");
  omx->SetDecalFunction(0,"TOT");
  // Should we skip OMs that we know from the dbase to have problems ?
////  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;

  // Investigate all waveforms for this OM
  for (Int_t iwf=1; iwf<=omx->GetNwaveforms(); iwf++)
  {
   wf=omx->GetWaveform(iwf);
   if (!wf) continue; 
   maxval=wf->GetMaximum();
   // Check if clipping window is not too large
   if(wf->GetNbinsX() > 2*nrIterations+1)
   {
    // Find peaks with TSpectrum
    npeaks=spec.Search(wf,fSigmaA,"goff");
    // Discard waveform if no or too many peaks found
    if(npeaks<1 || npeaks>fMaxPeaksA) continue;

    // Get differential of WF
    diff=*wf;
    for(ibin=2;ibin<diff.GetNbinsX();ibin++)
    {
     diff.SetBinContent(ibin,wf->GetBinContent(ibin)-wf->GetBinContent(ibin-1));
    }
    diff.SetBinContent(1,0);
    // Set baseline and lower end for first peak,
    baseline[0]=0;
    lowend[0]=1;

    // Sort peaks in time
    TMath::Sort(npeaks,spec.GetPositionX(),index,false);
    // For each of the peaks,
    for(Int_t ipeak=0; ipeak<npeaks; ipeak++)
    {
     // Find baseline and region around peak
     htemp=*wf;
     // (Second and later peaks: lower edge = upper edge previous peak,
     // baseline is average of previous baseline and minimum value between two
     // peaks)
     if(ipeak>0)
     {
      lowend[ipeak]=upend[ipeak-1]+1;
      baseline[ipeak]=fBasefracA*htemp.GetBinContent(lowend[ipeak]);
     }
     // (Upper edge range is minimum between this and next peak)
     if(ipeak<npeaks-1)
     {
      htemp.SetAxisRange(spec.GetPositionX()[index[ipeak]],spec.GetPositionX()[index[ipeak+1]]);
      upend[ipeak]=htemp.GetMinimumBin()-1;
     }
     // (Last peak: upper edge is end of histo)
     else
     {
      upend[ipeak]=wf->GetNbinsX();
     }
     // Find steepest rise
     lookforsteepestuntilbin=wf->FindBin(spec.GetPositionX()[index[ipeak]]);
     htemp=diff;
     // Look for steepest rise between lower edge and peak position
     htemp.SetAxisRange(wf->GetBinCenter(lowend[ipeak]),wf->GetBinCenter(lookforsteepestuntilbin));
     steep=htemp.GetMaximumBin();
     rise=htemp.GetBinContent(steep);

     // Extrapolate tangent to find leading edge
     yyy=wf->GetBinContent(steep)-baseline[ipeak];
     rc=rise/htemp.GetBinWidth(steep);
     if(rc>0) leadingedge[ipeak]=wf->GetBinCenter(steep)-yyy/rc; else leadingedge[ipeak]=0;

     // Determine peak status
     status[ipeak]=0;
     // Check for saturation
     if(rc<0.1 && wf->GetBinContent(wf->FindBin(spec.GetPositionX()[index[ipeak]])) == maxval)
     {
      status[ipeak]=2;
     }
     // Check quality: LE should not be too far below lower edge
     // Otherwise, ignore this peak and set baseline back to what it was
     else if(wf->GetBinLowEdge(lowend[ipeak]) - leadingedge[ipeak] > spec.GetPositionX()[index[ipeak]] - wf->GetBinLowEdge(lowend[ipeak]))
     {
      status[ipeak]=1;
      if(ipeak>0) baseline[ipeak]=baseline[ipeak-1];
     }
 
     // Start charge integration at LE, or at lower edge of range
     startcharge[ipeak]=wf->FindBin(leadingedge[ipeak]);
     if(lowend[ipeak]>startcharge[ipeak]) startcharge[ipeak]=lowend[ipeak];
 
     // Integrate charge until pulse drop below baseline, or else until edge of range
     stopcharge[ipeak]=upend[ipeak];
     for(ibin=wf->FindBin(spec.GetPositionX()[index[ipeak]]); ibin<=upend[ipeak]; ibin++)
     {
      if(wf->GetBinContent(ibin)<0)
      {
       stopcharge[ipeak]=ibin-1;
       break;
      }
     }

     // Determine time over threshold
     tot[ipeak]=wf->GetBinLowEdge(stopcharge[ipeak]+1)-wf->GetBinLowEdge(startcharge[ipeak]);
 
     // Determine charge
     charge[ipeak]=0;
     for(ibin=startcharge[ipeak]; ibin<=stopcharge[ipeak]; ibin++)
     {
      charge[ipeak]+=wf->GetBinContent(ibin);
     }

    } // end loop over peaks
 
    // Check all peaks, from latest to earliest
    for(int ipeak=npeaks-1; ipeak>=0; ipeak--)
    {

     // If this peak was rejected, add charge and TOT to previous peak (if there is one)
     if(status[ipeak]==1 && ipeak>0)
     {
      charge[ipeak-1]+=charge[ipeak];
      charge[ipeak]=0;
      tot[ipeak-1]+=tot[ipeak];
      tot[ipeak]=0;
     }

     // If this peak is OK, add hit info
     if(status[ipeak]==0)
     {
      hit.Reset();
      hit.SetSignal(charge[ipeak],"ADC");
      hit.SetSignal(leadingedge[ipeak],"LE");
      hit.SetSignal(tot[ipeak],"TOT");
      omx->AddHit(hit);
     }

    } // end loop over peaks
   // If number of bins too small, use different method
   }
   else
   {
    // If maximum value high enough to suspect presence of peak,
    if(maxval>fMinPulseHeightA)
    {
     // Loop over bins
     pulsegoingon=false;
     npeaks=0;
     for(ibin=1; ibin<=wf->GetNbinsX(); ibin++)
     {
      // If bin content above threshold, start pulse
      if(wf->GetBinContent(ibin)>fThresholdA*maxval){
       if(!pulsegoingon)
       {
	// Pulse starts here
        pulsegoingon=true;
        leadingedge[npeaks]=wf->GetBinLowEdge(ibin);
        charge[npeaks]=wf->GetBinContent(ibin);
       }
       else
       {
	// Pulse continues       
        charge[npeaks]+=wf->GetBinContent(ibin);
       }
      }
      else
      {
       if(pulsegoingon)
       {
	// Pulse ends here
        tot[npeaks]=wf->GetBinLowEdge(ibin)-leadingedge[npeaks];

	// Store pulse information
        hit.Reset();
        hit.SetSignal(charge[npeaks],"ADC");
        hit.SetSignal(leadingedge[npeaks],"LE");
        hit.SetSignal(tot[npeaks],"TOT");
        omx->AddHit(hit);

        // Get ready for next pulse
	pulsegoingon=false;
        npeaks++;
       }
      }

     } // End of loop over bins
    } 

   } // End of alternative method for narrow pulses

  } // End of WF loop
 } // End of OM loop

 // Clean up
 delete[] baseline;
 delete[] lowend;
 delete[] upend;
 delete[] startcharge;
 delete[] stopcharge;
 delete[] status;
 delete[] leadingedge;
 delete[] charge;
 delete[] tot;
 delete[] index;
}
///////////////////////////////////////////////////////////////////////////
void IceMakeHits::IceCube()
{
// Hit extraction from all IceCube (incl. IceTop) waveform (ATWD and/or FADC) data.

 // All IceCube (incl. IceTop)  DOMs with a signal
 TObjArray* idoms=fEvt->GetDevices("IceDOM");
 if (!idoms) return;

 // The number of different waveform names (or patterns) to be analysed 
 Int_t npatterns=0;
 if (fUseNamesI) npatterns=fUseNamesI->GetEntries();
 

 // Arrays for storing individual peak info
 // Peak status indicator :
 // 0=OK, 1=LE<peak window low edge, 2=saturation, 3=not high enough, 5=adjacent to later higher peak
 Float_t* baseline=new Float_t[fMaxPeaksI];
 Int_t* lowend=new Int_t[fMaxPeaksI];
 Int_t* upend=new Int_t[fMaxPeaksI];
 Int_t* startcharge=new Int_t[fMaxPeaksI];
 Int_t* stopcharge=new Int_t[fMaxPeaksI];
 Int_t* status=new Int_t[fMaxPeaksI];
 Float_t* leadingedge=new Float_t[fMaxPeaksI];
 Float_t* charge=new Float_t[fMaxPeaksI];
 Float_t* tot=new Float_t[fMaxPeaksI];
 Int_t* index=new Int_t[fMaxPeaksI];

 // Some objects and variables we will need
 TH1F htemp, diff;
 TSpectrum spec(fMaxPeaksI);
 spec.SetDeconIterations(50);
 Int_t nrIterations=(Int_t)(7*fSigmaI+0.5); // Number of iterations used in TSpectrum::SearchHighRes()
 Int_t npeaks=0, ibin=0, lookforsteepestuntilbin=0, steep=0;
 Float_t maxval=0, base1=0, rise=0, rc=0, yyy=0;
 Bool_t pulsegoingon=false;
 TString slotname;
 TString wfname;
 TString wfnamepattern;
 Int_t patternmatch=0;

 // OM, waveform and hit
 IceDOM* omx=0;
 TH1F* wf=0;
 Int_t nbinswf=0;
 Float_t wfbinvalue=0;
 NcSample amplitudes;
 amplitudes.SetStoreMode(1);
 Float_t basemedian=0;
 Float_t basespread=0;
 Float_t cutlevel=0;
 TString hitclass="none"; // Name of the hit class (e.g. CAL-ATWD, SLC etc...)
 NcSignal hit;
 hit.SetSlotName("ADC",1);
 hit.SetSlotName("LE",2);
 hit.SetSlotName("TOT",3);
 hit.SetSlotName(hitclass,4);
 hit.SetSlotName("SLC",5);

 // Loop over all fired OMs and extract the hit info
 Int_t nwf=0;
 Int_t slc=0;
 for (Int_t iom=0; iom<idoms->GetEntries(); iom++)
 {
  omx=(IceIDOM*)idoms->At(iom);
  if (!omx) continue;

  // Remove all existing hits of this OM 
  omx->RemoveHits();

  // Reset (de)calibration functions to indicate uncalibrated data
  omx->SetCalFunction(0,"ADC");
  omx->SetCalFunction(0,"LE");
  omx->SetCalFunction(0,"TOT");
  omx->SetDecalFunction(0,"ADC");
  omx->SetDecalFunction(0,"LE");
  omx->SetDecalFunction(0,"TOT");

  // Should we skip OMs that we know from the dbase to have problems ?
////  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;

  // Investigate all requested waveforms for this OM
  nwf=omx->GetNwaveforms();
  for (Int_t iwf=1; iwf<=nwf; iwf++)
  {
   wf=omx->GetWaveform(iwf);
   if (!wf) continue;
   wfname=wf->GetName();

   // Check whether the waveform name matches any of the specified name patterns
   patternmatch=0;
   for (Int_t ipat=0; ipat<npatterns; ipat++)
   {
    TObjString* sx=(TObjString*)fUseNamesI->At(ipat);
    if (!sx) continue;
    wfnamepattern=sx->GetString();
    if (wfname.Contains(wfnamepattern))
    {
     patternmatch=1;
     break;
    }
   }

   if (!patternmatch) continue;

   // Determine the name of the hitclass for this waveform
   hitclass="none";
   slc=0;
   if (wfname.Contains("CAL-ATWD")) hitclass="CAL-ATWD";
   if (wfname.Contains("CAL-FADC")) hitclass="CAL-FADC";
   if (wfname.Contains("RAW-ATWD0")) hitclass="RAW-ATWD0";
   if (wfname.Contains("RAW-ATWD1")) hitclass="RAW-ATWD1";
   if (wfname.Contains("RAW-ATWD2")) hitclass="RAW-ATWD2";
   if (wfname.Contains("RAW-ATWD3")) hitclass="RAW-ATWD3";
   if (wfname.Contains("RAW-FADC")) hitclass="RAW-FADC";
   if (wfname.Contains("STAMP"))
   {
    hitclass="Q-STAMP";
    if (nwf==1) slc=1;
   }

   // Determination of the baseline and cutlevel for the minimal peak height in this waveform
   maxval=wf->GetMaximum();
   base1=amplitudes.GetMedian(wf,2);  // First approximation of baseline
   cutlevel=base1+0.2*(maxval-base1); // Cut level for entries contributing to new baseline sampling

   // RAW waveforms consist of integer counts, so the minimal cutlevel is set
   // slightly higher than 1 count above the base1 for these RAW waveforms to get a better sampling of the baseline
   if (hitclass.Contains("RAW") && (cutlevel-base1)<1.1) cutlevel=base1+1.1;

   // Determination of the baseline and its spread for this waveform
   nbinswf=wf->GetNbinsX();
   amplitudes.Reset();
   for (Int_t jbin=1; jbin<=nbinswf; jbin++)
   {
    wfbinvalue=wf->GetBinContent(jbin);
    if (wfbinvalue<cutlevel) amplitudes.Enter(wfbinvalue);
   }
   basemedian=amplitudes.GetMedian(1);
   basespread=amplitudes.GetSpread(1);

   // Determination of the cutlevel to require a minimal peak height
   // In view of the fact that the CAL-ATWD may have a very large dynamic range because it may be
   // built out of several RAW-ATWD channels with different gains, the cutlevel will be limited for CAL-ATWD
   // in order not to remove good peaks in the presence of very high ones.
   // For CAL-ATWD a 1 PE signal corresponds roughly to 5 mV, so the cutlevel will be limited to 1 mV,
   // corresponding to about 0.2 PE.
   // RAW waveforms consist of integer counts, so the minimal cutlevel will be set slightly higher than 1 count
   // above the baseline for these RAW waveforms in order to prevent construction of fake peaks.
   cutlevel=basemedian+fPeakAcceptanceLevelI*basespread;
   if (hitclass.Contains("CAL-ATWD") && cutlevel>1) cutlevel=1;
   if (hitclass.Contains("RAW") && (cutlevel-basemedian)<1.1) cutlevel=basemedian+1.1;

   // Store the baseline and its spread in the DOM attributes
   slotname="BASELINE-WF";
   slotname+=iwf;
   omx->AddNamedSlot(slotname);
   omx->SetSignal(basemedian,slotname);
   omx->SetSignalError(basespread,slotname);

   // Check if clipping window is not too large
   if(nbinswf > 2*nrIterations+1)
   {
    // Find peaks with TSpectrum
    if ((cutlevel-basemedian) && (maxval-basemedian))
    {
     npeaks=spec.Search(wf,fSigmaI,"goff",fabs((cutlevel-basemedian)/(maxval-basemedian)));
    }
    else
    {
     npeaks=spec.Search(wf,fSigmaI,"goff",0.002);
    }
    // Discard waveform if no or too many peaks found
    if(npeaks<1 || npeaks>fMaxPeaksI) continue;

    // Get differential of WF
    diff=*wf;
    for(ibin=2;ibin<diff.GetNbinsX();ibin++)
    {
     diff.SetBinContent(ibin,wf->GetBinContent(ibin)-wf->GetBinContent(ibin-1));
    }
    diff.SetBinContent(1,0);
    // Set baseline and lower end for first peak,
    baseline[0]=basemedian;
    lowend[0]=1;

    // Sort peaks in time in increasing order
    TMath::Sort(npeaks,spec.GetPositionX(),index,false);

    // Analyse each individual peak
    for(Int_t ipeak=0; ipeak<npeaks; ipeak++)
    {
     // Find baseline and region around peak
     // Second and later peaks: lower edge = upper edge previous peak,
     // baseline is a predefined fraction between the previous baseline and minimum value between two peaks
     // In case the "minimum value between the peaks" is below the overall baseline, the pulses are obviously
     // well separated and the effective baseline is set to the overall baseline again.
     htemp=*wf;
     if(ipeak>0)
     {
      lowend[ipeak]=upend[ipeak-1]+1;
      baseline[ipeak]=baseline[ipeak-1]+fBasefracI*(htemp.GetBinContent(lowend[ipeak])-baseline[ipeak-1]);
      if (htemp.GetBinContent(lowend[ipeak])<=basemedian) baseline[ipeak]=basemedian;
     }
     if(ipeak<npeaks-1) // Upper edge range is minimum between this and next peak
     {
      htemp.SetAxisRange(spec.GetPositionX()[index[ipeak]],spec.GetPositionX()[index[ipeak+1]]);
      upend[ipeak]=htemp.GetMinimumBin()-1;
     }
     else // Last peak: upper edge is end of histo
     {
      upend[ipeak]=wf->GetNbinsX();
     }

     // Find steepest rise
     lookforsteepestuntilbin=wf->FindBin(spec.GetPositionX()[index[ipeak]]);
     htemp=diff;
     // Look for steepest rise between lower edge and peak position
     htemp.SetAxisRange(wf->GetBinCenter(lowend[ipeak]),wf->GetBinCenter(lookforsteepestuntilbin));
     steep=htemp.GetMaximumBin();
     rise=htemp.GetBinContent(steep);

     // Extrapolate tangent to find leading edge
     yyy=wf->GetBinContent(steep)-baseline[ipeak];
     rc=rise/htemp.GetBinWidth(steep);
     if (rc>0)
     {
      leadingedge[ipeak]=wf->GetBinCenter(steep)-yyy/rc;
     }
     else
     {
      leadingedge[ipeak]=0;
     }

     // Determine peak status
     status[ipeak]=0;
     // Check for saturation in RAW waveform data
     if ((hitclass.Contains("RAW") || hitclass.Contains("STAMP")) &&
         rc<1./wf->GetBinWidth(1) && wf->GetBinContent(wf->FindBin(spec.GetPositionX()[index[ipeak]])) == maxval)
     {
      status[ipeak]=2;
     }

     // Check quality: LE should not be too far below lower edge
     // Otherwise, ignore this peak and set baseline back to what it was
     if (wf->GetBinLowEdge(lowend[ipeak])-leadingedge[ipeak]>spec.GetPositionX()[index[ipeak]]-wf->GetBinLowEdge(lowend[ipeak]) &&
         ipeak>0 && status[ipeak-1]!=5)
     {
      status[ipeak]=1;
      if(ipeak>0) baseline[ipeak]=baseline[ipeak-1];
     }

     // Check if peak is adjacent to a higher, later peak.
     // If this is the case, flag it with status 5.
     // The smaller bump will then lateron be merged with the larger adjacent peak.
     if (upend[ipeak]-wf->FindBin(spec.GetPositionX()[index[ipeak]])<0) status[ipeak]=5;

     // Check if peak is high enough
     if(wf->GetBinContent(lookforsteepestuntilbin) < cutlevel) status[ipeak]=3;

     // Start charge integration at LE, or at lower edge of range
     startcharge[ipeak]=wf->FindBin(leadingedge[ipeak]);
     if(lowend[ipeak]>startcharge[ipeak]) startcharge[ipeak]=lowend[ipeak];
 
     // Integrate charge until pulse drops below its baseline, or else until edge of range
     stopcharge[ipeak]=upend[ipeak];
     for (ibin=wf->FindBin(spec.GetPositionX()[index[ipeak]]); ibin<=upend[ipeak]; ibin++)
     {
      if(wf->GetBinContent(ibin)<=baseline[ipeak])
      {
       stopcharge[ipeak]=ibin-1;
       break;
      }
     }

     // Determine time over threshold
     tot[ipeak]=wf->GetBinLowEdge(stopcharge[ipeak]+1)-wf->GetBinLowEdge(startcharge[ipeak]);
 
     // Determine the charge corrected for the baseline
     charge[ipeak]=0;
     for(ibin=startcharge[ipeak]; ibin<=stopcharge[ipeak]; ibin++)
     {
      charge[ipeak]+=(wf->GetBinContent(ibin))-baseline[ipeak];
     }

    } // end loop over peaks
 
    // Check all peaks, from latest to earliest
    for(int ipeak=npeaks-1; ipeak>=0; ipeak--)
    {
     // If this peak was rejected, add charge and TOT to previous peak (if there is one)
     if(status[ipeak]==1 && ipeak>0)
     {
      charge[ipeak-1]+=charge[ipeak];
      charge[ipeak]=0;
      tot[ipeak-1]+=tot[ipeak];
      tot[ipeak]=0;
     }

     // If earlier peak was rejected by status 5, add charge, TOT and set LE of that peak to this peak
     if (status[ipeak-1]==5)
     {
      charge[ipeak]+=charge[ipeak-1];
      charge[ipeak-1]=0;
      tot[ipeak]+=tot[ipeak-1];
      tot[ipeak-1]=0;
      leadingedge[ipeak]=leadingedge[ipeak-1];
      leadingedge[ipeak-1]=0;
     }

     // If this peak is OK, add hit info
     if(status[ipeak]==0 && charge[ipeak]>0)
     {
      hit.Reset();
      hit.SetSignal(charge[ipeak],"ADC");
      hit.SetSignal(leadingedge[ipeak],"LE");
      hit.SetSignal(tot[ipeak],"TOT");
      hit.SetSlotName(hitclass,4);
      hit.SetSignal(1,hitclass);
      if (slc)
      {
       hit.SetSignal(1,"SLC");
      }
      else
      {
       hit.SetSignal(0,"SLC");
      }
      omx->AddHit(hit);
     }

    } // end loop over peaks
   }
   else // If number of bins too small, use different method
   {
    // If maximum value high enough to suspect presence of peak,
    if(maxval>fMinPulseHeightI)
    {
     // Loop over bins
     pulsegoingon=false;
     npeaks=0;
     for(ibin=1; ibin<=wf->GetNbinsX(); ibin++)
     {
      // If bin content above threshold, start pulse
      if(wf->GetBinContent(ibin)>fThresholdI*maxval){
       if(!pulsegoingon)
       {
	// Pulse starts here
        pulsegoingon=true;
        leadingedge[npeaks]=wf->GetBinLowEdge(ibin);
        charge[npeaks]=(wf->GetBinContent(ibin))-basemedian;
       }
       else
       {
	// Pulse continues       
        charge[npeaks]+=(wf->GetBinContent(ibin))-basemedian;
       }
      }
      else
      {
       if(pulsegoingon)
       {
	// Pulse ends here
        tot[npeaks]=wf->GetBinLowEdge(ibin)-leadingedge[npeaks];

	// Store pulse information
        hit.Reset();
        hit.SetSignal(charge[npeaks],"ADC");
        hit.SetSignal(leadingedge[npeaks],"LE");
        hit.SetSignal(tot[npeaks],"TOT");
        hit.SetSlotName(hitclass,4);
        hit.SetSignal(1,hitclass);
        if (slc)
        {
         hit.SetSignal(1,"SLC");
        }
        else
        {
         hit.SetSignal(0,"SLC");
        }
        omx->AddHit(hit);

        // Get ready for next pulse
	pulsegoingon=false;
        npeaks++;
       }
      }

     } // End of loop over bins
    } 

   } // End of alternative method for narrow pulses

  } // End of WF loop
 } // End of OM loop

 // Clean up
 delete[] baseline;
 delete[] lowend;
 delete[] upend;
 delete[] startcharge;
 delete[] stopcharge;
 delete[] status;
 delete[] leadingedge;
 delete[] charge;
 delete[] tot;
 delete[] index;
}
///////////////////////////////////////////////////////////////////////////
