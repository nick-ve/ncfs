/*******************************************************************************
 * Copyright(c) 2021, RNO-G Experiment at Summit Station. All rights reserved.
 *
 * Author: The RNO-G NCFS-based Offline Project.
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

///////////////////////////////////////////////////////////////////////////
// Class RnoStation
// Handling of RNO-G event data.
// Basically this class provides an RNO-G tailored user interface
// to the functionality of the class NcDetectorUnit.
//
// Notes :
// ------
// * In the event structure MC tracks are labeled with a negative track ID,
//   whereas reconstructed tracks are labeled with a positive track ID.
//   This allows for a direct selection of either MC or Reco tracks via the
//   GetIdTrack() facility.
// * The particle codes used for the various tracks are the PDG ones.
//   In the need of RNO-G specific "particle" types the PDG database
//   may be extended in the usual way.
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, June 22, 2021  08:23Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, February 3, 2023  16:30Z
///////////////////////////////////////////////////////////////////////////

#include "RnoStation.h"
#include "Riostream.h"
 
ClassImp(RnoStation) // Class implementation to enable ROOT I/O
 
RnoStation::RnoStation(const char* name,const char* title) : NcDetectorUnit(name,title)
{
// Default constructor.

 fCanvas=0;
}
///////////////////////////////////////////////////////////////////////////
RnoStation::~RnoStation()
{
// Default destructor.

 if (fCanvas)
 {
  if (gROOT->GetListOfCanvases()->FindObject(ClassName())) delete fCanvas;
  fCanvas=0;
 }
}
///////////////////////////////////////////////////////////////////////////
RnoStation::RnoStation(const RnoStation& q) : NcDetectorUnit(q)
{
// Copy constructor.

 fCanvas=0;
}
///////////////////////////////////////////////////////////////////////////
TGraph* RnoStation::DisplaySampling(Int_t ich,Int_t j)
{
// Display the sampling of the j-th sampled observable (1=first) for the selected channel number "ich".
// The graph will display the values of the j-th observable versus the sample entry number.
//
// The returned argument is the pointer to the created graph.
// For extended functionality, please refer to the (inherited) memberfunction DisplaySample().
//
// The default value is j=1.

 if (fCanvas)
 {
  if (gROOT->GetListOfCanvases()->FindObject(ClassName())) delete fCanvas;
  fCanvas=0;
 }

 if (ich<0 || j<1) return 0;

 fCanvas=new TCanvas();
 TVirtualPad* pad=fCanvas->cd();
 gROOT->SetSelectedPad(pad);

 TString devname="Ch";
 devname+=ich;

 Int_t id=GetUniqueID(); // The station ID
 TString staname="Station";
 staname+=id;

 TString title="";

 title=staname;
 title+=" ";
 title+=devname;
 fCanvas->SetName(ClassName());
 fCanvas->SetTitle(title);
 fCanvas->SetGrid();

 GetSamplingGraph(ich,j).DrawClone("AL");

 // Obtain the pointer to the displayed graph
 TGraph* gr=(TGraph*)fCanvas->FindObject("NcSample");

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TCanvas* RnoStation::DisplaySamplings(Int_t j)
{
// Display the sampling of the j-th sampled observable (1=first) for all channels.
// The graph will display the values of the j-th observable versus the sample entry number.
//
// The returned argument is the pointer to the created canvas.
// For extended functionality, please refer to the (inherited) memberfunction DisplaySample().
//
// The default value is j=1.

 if (fCanvas)
 {
  if (gROOT->GetListOfCanvases()->FindObject(ClassName())) delete fCanvas;
  fCanvas=0;
 }

 if (j<1) return 0;

 fCanvas=new TCanvas();

 Int_t id=GetUniqueID(); // The station ID
 TString staname="Station";
 staname+=id;

 TString title="";
 TString devname="";

 title=staname;
 fCanvas->SetName(ClassName());
 fCanvas->SetTitle(title);
 fCanvas->Divide(4,6);

 TVirtualPad* pad=0;
 for (Int_t jch=0; jch<24; jch++)
 {
  pad=fCanvas->cd(jch+1);

  if (!pad) continue;

  gROOT->SetSelectedPad(pad);
  pad->SetGrid();

  GetSamplingGraph(jch,j).DrawClone("AL");
 } // End of loop over the channels
 
 return fCanvas;
}
///////////////////////////////////////////////////////////////////////////
TGraph RnoStation::GetSamplingGraph(Int_t ich,Int_t j)
{
// Provide the sampling graph of the j-th sampled observable (1=first) for the selected channel number "ich".
// The graph contains the values of the j-th observable versus the sample entry number.
//
// The default value is j=1.

 TGraph gr;

 if (ich<0 || j<1) return gr;

 TString devname="Ch";
 devname+=ich;

 Int_t id=GetUniqueID(); // The station ID
 TString staname="Station";
 staname+=id;

 NcDevice* daq=GetDevice("DAQ",kFALSE);
 Double_t fsample=0;
 if (daq) fsample=daq->GetSignal("Sampling-rate");

 TString rate="";
 rate.Form(" (DAQ: %-.3g Samples/sec)",fsample);

 TString title=staname;
 title+=" ";
 title+=devname;
 title+=rate;

 RnoGANT* ant=(RnoGANT*)GetDevice(devname,kTRUE);
 NcSample* sx=ant->GetSample(1);

 if (!sx) return gr;

 Int_t ndim=sx->GetDimension();
 if (j>ndim) return gr;

 gr=sx->GetGraph(j);

 TString xname="Sample";
 TString yname=sx->GetVariableName(j);

 gr.SetTitle(title);
 gr.GetXaxis()->SetTitle(xname);
 gr.GetYaxis()->SetTitle(yname);

 return gr;
}
///////////////////////////////////////////////////////////////////////////
TH1F RnoStation::GetSamplingDFT(Int_t ich,TString sel,Int_t j)
{
// Provide the Discrete Fourier Transform (DFT) of the j-th sampled observable (1=first)
// for the selected channel number "ich".
//
// sel     : String to specify the contents and representation of the result histogram
//           "RE"   --> Y-axis shows the values of the real (re) components
//           "IM"   --> Y-axis shows the values of the imaginary (im) components
//           "AMP"  --> Y-axis shows the values of the amplitudes, i.e. sqrt(re*re+im*im)
//           "PHIR" --> Y-axis shows the values of the phases, i.e. arctan(im/re), in radians
//           "PHID" --> Y-axis shows the values of the phases, i.e. arctan(im/re), in degrees
//           "k"    --> X-axis represents the index k in the frequency domain  
//           "f"    --> X-axis represents the fraction f of the sampling rate in the frequency domain
//           "Hz"   --> X-axis represents the actual frequency in Hz in the frequency domain
//           "n"    --> X-axis represents the index n in the time domain  
//           "t"    --> X-axis represents the actual time in seconds in the time domain
//           "2"    --> X-axis spans the full number of data points, instead of the usual (N/2)+1
//
// Examples :
// ----------
// sel="AMP f" will show the (N/2)+1 amplitudes as a function of the fractional sampling rate.
// sel="RE k 2" will show all N real components as a function of the index k in the frequency domain.
//
// The default values are sel="AMP Hz" and j=1.

 TH1F his;

 TGraph gr=GetSamplingGraph(ich,j);

 NcDevice* daq=GetDevice("DAQ",kFALSE);
 Double_t fsample=0;
 if (daq) fsample=daq->GetSignal("Sampling-rate");

 NcDSP q;
 q.Load(&gr,fsample);
 q.Fourier("R2C",&his,sel);

 TString title=gr.GetTitle();

 his.SetTitle(title);
 his.SetStats(0);

 return his;
}
///////////////////////////////////////////////////////////////////////////
TObject* RnoStation::Clone(const char* name) const
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers like NcEvent when adding objects in case the
// container owns the objects.

 RnoStation* q=new RnoStation(*this);
 if (name)
 {
  if (strlen(name)) q->SetName(name);
 }
 return q;
}
///////////////////////////////////////////////////////////////////////////
