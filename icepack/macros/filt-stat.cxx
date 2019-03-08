//////////////////////////////////////////////////////////////////////////////
// Code (ROOT) to perform IceCube event filter analysis within IcePack.     //
//                                                                          //
//--- Nick van Eijndhoven 22-jan-2014 IIHE-VUB, Brussel.                    //
//////////////////////////////////////////////////////////////////////////////

#include <iomanip>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TArrayI.h"
#include "TMatrixF.h"

#include "IceEvent.h"

void user()
{
 // Number of events to be processed (0=all)
 Int_t nevents=0;

 // Print frequency to provide an output line every "ipfreq" events
 Int_t ipfreq=1000;

 // Access to the input data
 TChain* data=new TChain("T");
 data->Add("*.icepack");

 data->Print();


 // Define a pointer for an event
 IceEvent* evt=0;

 // Branch in the tree for the event input
 data->SetBranchAddress("IceEvent",&evt);

 Int_t nen=(int)data->GetEntries();
 cout << endl;
 cout << " Number of entries available : " << nen << endl;
 cout << endl;

 if (nevents && nevents<nen) nen=nevents;
 cout << " *** Number of entries to be processed : " << nen << endl;

 Int_t nevt=0; // Number of events that were filtered
 Int_t nevtcomb=0; // Number of events with selected (combined) filters

 NcSignal conds;
 conds.SetNameTitle("Filtstats","IceCube filter condition statistics");
 TArrayI* aconds=0;
 TMatrixF* mconds=0;
 NcSignal writes;
 writes.SetNameTitle("Filtstats","IceCube filter write (prescale) statistics");
 TArrayI* awrites=0;
 TMatrixF* mwrites=0;
 NcSignal combis;
 combis.SetNameTitle("Filtstats","IceCube combined condition*write filter statistics");
 TArrayI* acombis=0;
 TMatrixF* mcombis=0;

 Int_t nfilters=0;
 Int_t jrun=0;
 Int_t jevt=0;
 Int_t perc=0;
 TString name;
 IceEvent* first=0;
 IceEvent* last=0;
 Int_t condition=0;
 Int_t prescale=0;
 Int_t combflag=0;
 Int_t ival1=0;
 Int_t ival2=0;
 Int_t ival3=0;
 Int_t ival4=0;

 for (Int_t ient=0; ient<nen; ient++)
 {
  data->GetEntry(ient);
  if (!evt) continue;

  nfilters=0;
  NcDevice* filtdev=(NcDevice*)evt->GetDevice("Filter");
  if (!filtdev) continue;

  nfilters=filtdev->GetNhits();

  if (!aconds) aconds=new TArrayI(nfilters);
  if (!awrites) awrites=new TArrayI(nfilters);
  if (!acombis) acombis=new TArrayI(nfilters);

  aconds->Reset();
  awrites->Reset();
  acombis->Reset();

  if (!mconds) mconds=new TMatrixF(nfilters,nfilters);
  if (!mwrites) mwrites=new TMatrixF(nfilters,nfilters);
  if (!mcombis) mcombis=new TMatrixF(nfilters,nfilters);

  jrun=evt->GetRunNumber();
  jevt=evt->GetEventNumber();

  // Determine the first and last recorded event
  if (!first) first=new IceEvent(*evt);
  if (!last) last=new IceEvent(*evt);
  if (first->GetDifference(evt,"s")<0)
  {
   delete first;
   first=new IceEvent(*evt);
  }
  if (last->GetDifference(evt,"s")>0)
  {
   delete last;
   last=new IceEvent(*evt);
  }

  nevt++;

  combflag=0;
  for (Int_t ifilt=1; ifilt<=nfilters; ifilt++)
  {
   NcSignal* sx=filtdev->GetHit(ifilt);
   if (!sx) continue;
   name=sx->GetName();
   condition=int(sx->GetSignal("condition")+0.1); // Flag to indicate if the filter condition is satisfied (1) or not (0)
   prescale=int(sx->GetSignal("prescale")+0.1);   // Flag to indicate if this filter would write the event (1) or not (0)

   // Update stats for this filter name
   conds.AddNamedSlot(name);
   writes.AddNamedSlot(name);
   combis.AddNamedSlot(name);
   conds.AddSignal(condition,name);
   aconds->AddAt(condition,ifilt-1);
   writes.AddSignal(prescale,name);
   awrites->AddAt(prescale,ifilt-1);
   combis.AddSignal(condition*prescale,name);
   acombis->AddAt(condition*prescale,ifilt-1);

   //////////////////////////////////////////////////////////////////////////
   // De-activate some filters for the combined condition*write statistics //
   // which will enable to study background reduction for some analyses    //
   //////////////////////////////////////////////////////////////////////////

   if (name.Contains("IceTop") || name.Contains("GC") || name.Contains("Moon") || name.Contains("Sun") || name.Contains("Slop") ||
       name.Contains("MinBias") || name.Contains("FSS") || name.Contains("FixedRate") || name.Contains("VEF"))
   {
    combis.SetDead(name);
   }
   else
   {
    if (condition*prescale) combflag=1;
   }
  } // End of filter loop

  if (combflag) nevtcomb++;

  // Update the various filter matrices for this event
  for (Int_t irow=0; irow<nfilters; irow++)
  {
   ival1=aconds->At(irow);
   ival2=awrites->At(irow);
   ival3=acombis->At(irow);
   if (!ival1 && !ival2 && !ival3) continue;
   for (Int_t icol=0; icol<nfilters; icol++)
   {
    ival4=aconds->At(icol);
    if (ival1 && ival4) (*mconds)(irow,icol)+=float(ival4);
    ival4=awrites->At(icol);
    if (ival2 && ival4) (*mwrites)(irow,icol)+=float(ival4);
    ival4=acombis->At(icol);
    if (ival3 && ival4) (*mcombis)(irow,icol)+=float(ival4);
   }
  }  

  if (!(ient%ipfreq))
  {
   perc=100*(ient+1)/nen;
   cout << " *** Processed input entry : " << ient << " run : " << jrun << " event : " << jevt << " (" << perc << "%)" << endl;
  }

 } // End of event loop

 // The final statistics
 cout << " Total number of filtered events : " << nevt << endl;
 cout << endl;
 if (nevt)
 {
  cout << " ===== First filtered event =====" << endl;
  first->HeaderData();
  cout << endl;

  cout << " ===== Last filtered event =====" << endl;
  last->HeaderData();
  cout << endl;

  Double_t dt=first->GetDifference(last,"s");
  cout << " Elapsed timespan (sec.) : " << dt << endl;
  if (dt<=0) cout << " *** Incorrect timespan. No rates calculated. ***" << endl;

  // The various filter rates
  if (dt>0)
  {
   Double_t ratetot=double(nevt)/dt;
   cout << " Total filtered event rate (Hz) : " << ratetot << endl; 
   Double_t ratecombitot=double(nevtcomb)/dt;
   cout << " Selective filter suppressed (see below) event rate (Hz) : " << ratecombitot << endl;

   // Rate per filter channel
   Double_t ratecond=0;
   Double_t ratewrite=0;
   Double_t ratio=0;
   cout << endl;
   cout << " ===== Total filter rates (Hz) for the various channels =====" << endl;
   for (Int_t i=1; i<=conds.GetNslots(); i++)
   {
    name=conds.GetSlotName(i);
    ratecond=conds.GetSignal(i)/dt;
    ratewrite=writes.GetSignal(i)/dt;
    ratio=1;
    if (ratecond>0) ratio=ratewrite/ratecond;
    cout << " " << name.Data() << " condition : " << ratecond << " write (prescale) : " << ratewrite 
         << " ===> write/cond : " << ratio << endl;
   }
  }

  // Overview of the various Filter counts
  cout << endl;
  cout << " ===== Total filter condition passing statistics =====" << endl;
  conds.Data();
  cout << endl;
  cout << " The filter condition matrix (empty rows/columns suppressed) : " << endl;
  Int_t ndigits=6;
  cout << setfill(' ');
  for (Int_t irow=-1; irow<nfilters; irow++)
  {
   // Skip empty rows to reduce printed matrix size
   if (irow>-1)
   {
    if (conds.GetSignal(irow+1)<1) continue;
   }
   cout << " " << setw(2) << (irow+1);
   for (Int_t icol=0; icol<nfilters; icol++)
   {
    // Skip empty columns to reduce printed matrix size
    if (conds.GetSignal(icol+1)<1) continue;
    if (irow==-1)
    {
     ival1=icol+1;
    }
    else
    {
     ival1=int((*mconds)(irow,icol));
    }
    cout << " " << setw(ndigits) << ival1;
   }
   cout << endl;
  }  
  cout << endl;
  cout << " ===== Total filter event writing (prescale) statistics =====" << endl;
  writes.Data();
  cout << endl;
  cout << " The filter event writing (prescale) matrix (empty rows/columns suppressed) : " << endl;
  for (Int_t irow=-1; irow<nfilters; irow++)
  {
   // Skip empty rows to reduce printed matrix size
   if (irow>-1)
   {
    if (writes.GetSignal(irow+1)<1) continue;
   }
   cout << " " << setw(2) << (irow+1);
   for (Int_t icol=0; icol<nfilters; icol++)
   {
    // Skip empty columns to reduce printed matrix size
    if (writes.GetSignal(icol+1)<1) continue;
    if (irow==-1)
    {
     ival1=icol+1;
    }
    else
    {
     ival1=int((*mwrites)(irow,icol));
    }
    cout << " " << setw(ndigits) << ival1;
   }
   cout << endl;
  }  
  cout << endl;
  cout << " ===== Combined condition and event writing (prescale) filter statistics =====" << endl;
  cout << " ===== with user selected de-activations to study background reduction   =====" << endl;
  combis.Data();
  cout << endl;
  cout << " The combined condition and event writing (prescale) filter matrix (empty rows/columns suppressed) : " << endl;
  for (Int_t irow=-1; irow<nfilters; irow++)
  {
   // Skip empty rows to reduce printed matrix size
   if (irow>-1)
   {
    if (combis.GetSignal(irow+1)<1) continue;
   }
   cout << " " << setw(2) << (irow+1);
   for (Int_t icol=0; icol<nfilters; icol++)
   {
    // Skip empty columns to reduce printed matrix size
    if (combis.GetSignal(icol+1)<1) continue;
    if (irow==-1)
    {
     ival1=icol+1;
    }
    else
    {
     ival1=int((*mcombis)(irow,icol));
    }
    cout << " " << setw(ndigits) << ival1;
   }
   cout << endl;
  }  
 }
}
