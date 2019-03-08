//////////////////////////////////////////////////////////////
// Definition of the data structures and event loop processor
// for the investigation of hit statistics from IcePack data.
//
// This code is loaded and executed via the ROOT macro
// hit-stat.cc where additional doc information is provided. 
//
//--- NvE 30-nov-2010 IIHE Brussel
//////////////////////////////////////////////////////////////

#include <vector>

#include "TChain.h"
#include "NcJob.h"
#include "IceEvent.h"
#include "IceDOM.h"

class Hit
{
// Definition of the basic hit data
 public:
  Float_t adc;
  Float_t le;
  Float_t tot;
  Int_t deadadc;
  Int_t deadle;
  Int_t deadtot;
  Int_t slc;
};
////////////////////////////////////////////////////////////////////////////////
class Readout
{
// Storage of all hit data for a certain readout system (e.g. ATWD0, FADC,...) 
 public:
  Int_t nhits;
  std::vector<Hit> hits; //[nhits]

  Readout() {nhits=0;}
};
////////////////////////////////////////////////////////////////////////////////
class Dom
{
// Definition of all data of a certain DOM
 public:
  Int_t domid;
  Float_t x;
  Float_t y;
  Float_t z;
  Readout calatwd;
  Readout calfadc;
  Readout rawatwd0;
  Readout rawatwd1;
  Readout rawatwd2;
  Readout rawatwd3;
  Readout rawfadc;
  Readout qstamp;
  Readout dst;
};
////////////////////////////////////////////////////////////////////////////////
class Detector
{
// Definition of some overall detector statistics
 public :
 Float_t trigtime;
 Int_t ndoms;
 Int_t nhitsgood;
 Int_t nhitsbad;
 Float_t adcsumgood;
 Float_t adcsumbad;
};
////////////////////////////////////////////////////////////////////////////////
class Event
{
// Defintion of the total event data
 public :
  Int_t jrun;
  Int_t jevt;
  Int_t mjd;
  Int_t sec;
  Int_t nsec;
  Detector icetop;
  Detector inice;
  Detector icecube;
  Detector deepcore;
  Int_t ndoms;
  std::vector<Dom> doms; //[ndoms]

  Event() {ndoms=0;}
};
////////////////////////////////////////////////////////////////////////////////
// Pragma definitions necessary for the various std::vector arrays
#ifdef __MAKECINT__
#pragma link C++ class vector<Hit>+;
#pragma link C++ class vector<Dom>+;
#endif
////////////////////////////////////////////////////////////////////////////////
void run(TChain* data,IceEvent* evt,NcJob* job,TTree* otree,Long64_t nen)
{
// The main event loop processor module

 Int_t slc;         // Soft Local Coincidence flag
 Float_t adc;       // Total hit amplitude (=integral of waveform pulse)
 Float_t le;        // Leading edge time value in nanoseconds
 Float_t tot;       // Pulse duration (=integrated time interval) in nanoseconds
 Int_t deadadc;     // Flag to denote rejected adc value by IceCleanHits
 Int_t deadle;      // Flag to denote rejected le value by IceCleanHits
 Int_t deadtot;     // Flag to denote rejected tot value by IceCleanHits

 // The event structure for the output tree
 Event event;

 Event* pevent=&event;
 otree->Branch("event.",&pevent);

 Int_t ndoms;
 Int_t idom;
 Int_t nh;
 Int_t ihit;
 Int_t domid;

 for (Long64_t ien=0; ien<nen; ien++)
 {
  data->GetEntry(ien);

  if (!evt) continue;

  job->ProcessObject(evt);

  if (!(ien%10)) cout << " Processing entry " << ien << endl;

  event.jrun=evt->GetRunNumber();
  event.jevt=evt->GetEventNumber();
  evt->GetMJD(event.mjd,event.sec,event.nsec);

  NcDevice* dev=(NcDevice*)evt->GetDevice("IceCleanHits");
  event.icetop.trigtime=0;
  event.icecube.trigtime=0;
  event.deepcore.trigtime=0;
  if (dev)
  {
   event.icetop.trigtime=fabs(dev->GetSignal("TtimT"));
   event.icecube.trigtime=fabs(dev->GetSignal("TtimIC"));
   event.deepcore.trigtime=fabs(dev->GetSignal("TtimDC"));
  }
  TObjArray* idoms=evt->GetDevices("IceIDOM");
  event.inice.trigtime=evt->GetTriggerTime("user",idoms);

  event.icetop.ndoms=evt->GetNdevices("IceTDOM");
  event.inice.ndoms=evt->GetNdevices("IceIDOM");
  event.icecube.ndoms=evt->GetNdevices("IceICDOM");
  event.deepcore.ndoms=evt->GetNdevices("IceDCDOM");

  // Fill the DOM based hit structures and update total event hit counters
  event.icetop.nhitsgood=0;
  event.icetop.nhitsbad=0;
  event.icetop.adcsumgood=0;
  event.icetop.adcsumbad=0;
  event.inice.nhitsgood=0;
  event.inice.nhitsbad=0;
  event.inice.adcsumgood=0;
  event.inice.adcsumbad=0;
  event.icecube.nhitsgood=0;
  event.icecube.nhitsbad=0;
  event.icecube.adcsumgood=0;
  event.icecube.adcsumbad=0;
  event.deepcore.nhitsgood=0;
  event.deepcore.nhitsbad=0;
  event.deepcore.adcsumgood=0;
  event.deepcore.adcsumbad=0;

  TObjArray* doms=evt->GetDevices("IceDOM");
  ndoms=0;
  if (doms) ndoms=doms->GetEntries();

  event.doms.resize(ndoms);
  event.ndoms=0;
  Int_t stored=0;
  for (Int_t id=0; id<ndoms; id++)
  {
   IceDOM* domx=(IceDOM*)doms->At(id);
   if (!domx) continue;
   domid=domx->GetUniqueID();
   event.ndoms+=1;
   idom=event.ndoms-1;
   event.doms[idom].domid=domid;
   event.doms[idom].x=domx->GetX(1,"car");
   event.doms[idom].y=domx->GetX(2,"car");
   event.doms[idom].z=domx->GetX(3,"car");

   nh=domx->GetNhits("CAL-ATWD",2);
   event.doms[idom].calatwd.hits.resize(nh);
   event.doms[idom].calatwd.nhits=0;
   nh=domx->GetNhits("CAL-FADC",2);
   event.doms[idom].calfadc.hits.resize(nh);
   event.doms[idom].calfadc.nhits=0;
   nh=domx->GetNhits("RAW-ATWD0",2);
   event.doms[idom].rawatwd0.hits.resize(nh);
   event.doms[idom].rawatwd0.nhits=0;
   nh=domx->GetNhits("RAW-ATWD1",2);
   event.doms[idom].rawatwd1.hits.resize(nh);
   event.doms[idom].rawatwd1.nhits=0;
   nh=domx->GetNhits("RAW-ATWD2",2);
   event.doms[idom].rawatwd2.hits.resize(nh);
   event.doms[idom].rawatwd2.nhits=0;
   nh=domx->GetNhits("RAW-ATWD3",2);
   event.doms[idom].rawatwd3.hits.resize(nh);
   event.doms[idom].rawatwd3.nhits=0;
   nh=domx->GetNhits("RAW-FADC",2);
   event.doms[idom].rawfadc.hits.resize(nh);
   event.doms[idom].rawfadc.nhits=0;
   nh=domx->GetNhits("Q-STAMP",2);
   event.doms[idom].qstamp.hits.resize(nh);
   event.doms[idom].qstamp.nhits=0;
   nh=domx->GetNhits();
   event.doms[idom].dst.hits.resize(nh);
   event.doms[idom].dst.nhits=0;

   // Sort all hits of this DOM with increasing hit time
   TObjArray* hits=domx->SortHits("LE",1,0,0,0);
   nh=0;
   if (hits) nh=hits->GetEntries();
   if (!nh) continue;

   for (Int_t ih=0; ih<nh; ih++)
   {
    NcSignal* sigx=(NcSignal*)hits->At(ih);
    if (!sigx) continue;
    slc=sigx->GetSignal("SLC");
    adc=sigx->GetSignal("ADC");
    le=sigx->GetSignal("LE");
    tot=sigx->GetSignal("TOT");
    deadadc=sigx->GetDeadValue("ADC");
    deadle=sigx->GetDeadValue("LE");
    deadtot=sigx->GetDeadValue("TOT");

    if (domid%100 > 60) // IceTop DOM
    {
     if (deadadc || deadle || deadtot)
     {
      event.icetop.nhitsbad+=1;
      event.icetop.adcsumbad+=adc;
     }
     else
     {
      event.icetop.nhitsgood+=1;
      event.icetop.adcsumgood+=adc;
     }
    }
    else // InIce DOM
    {
     if (deadadc || deadle || deadtot)
     {
      event.inice.nhitsbad+=1;
      event.inice.adcsumbad+=adc;
     }
     else
     {
      event.inice.nhitsgood+=1;
      event.inice.adcsumgood+=adc;
     }
     if (domid%10 < 79) // Standard IceCube DOM
     {
      if (deadadc || deadle || deadtot)
      {
       event.icecube.nhitsbad+=1;
       event.icecube.adcsumbad+=adc;
      }
      else
      {
       event.icecube.nhitsgood+=1;
       event.icecube.adcsumgood+=adc;
      }
     }
     else // DeepCore DOM
     {
      if (deadadc || deadle || deadtot)
      {
       event.deepcore.nhitsbad+=1;
       event.deepcore.adcsumbad+=adc;
      }
      else
      {
       event.deepcore.nhitsgood+=1;
       event.deepcore.adcsumgood+=adc;
      }
     }
    }

    stored=0;
    if (sigx->GetSignal("CAL-ATWD"))
    {
     event.doms[idom].calatwd.nhits+=1;
     ihit=event.doms[idom].calatwd.nhits-1;
     event.doms[idom].calatwd.hits[ihit].adc=adc;
     event.doms[idom].calatwd.hits[ihit].le=le;
     event.doms[idom].calatwd.hits[ihit].tot=tot;
     event.doms[idom].calatwd.hits[ihit].deadadc=deadadc;
     event.doms[idom].calatwd.hits[ihit].deadle=deadle;
     event.doms[idom].calatwd.hits[ihit].deadtot=deadtot;
     event.doms[idom].calatwd.hits[ihit].slc=slc;
     stored=1;
    }
    if (sigx->GetSignal("CAL-FADC"))
    {
     event.doms[idom].calfadc.nhits+=1;
     ihit=event.doms[idom].calfadc.nhits-1;
     event.doms[idom].calfadc.hits[ihit].adc=adc;
     event.doms[idom].calfadc.hits[ihit].le=le;
     event.doms[idom].calfadc.hits[ihit].tot=tot;
     event.doms[idom].calfadc.hits[ihit].deadadc=deadadc;
     event.doms[idom].calfadc.hits[ihit].deadle=deadle;
     event.doms[idom].calfadc.hits[ihit].deadtot=deadtot;
     event.doms[idom].calfadc.hits[ihit].slc=slc;
     stored=1;
    }
    if (sigx->GetSignal("RAW-ATWD0"))
    {
     event.doms[idom].rawatwd0.nhits+=1;
     ihit=event.doms[idom].rawatwd0.nhits-1;
     event.doms[idom].rawatwd0.hits[ihit].adc=adc;
     event.doms[idom].rawatwd0.hits[ihit].le=le;
     event.doms[idom].rawatwd0.hits[ihit].tot=tot;
     event.doms[idom].rawatwd0.hits[ihit].deadadc=deadadc;
     event.doms[idom].rawatwd0.hits[ihit].deadle=deadle;
     event.doms[idom].rawatwd0.hits[ihit].deadtot=deadtot;
     event.doms[idom].rawatwd0.hits[ihit].slc=slc;
     stored=1;
    }
    if (sigx->GetSignal("RAW-ATWD1"))
    {
     event.doms[idom].rawatwd1.nhits+=1;
     ihit=event.doms[idom].rawatwd1.nhits-1;
     event.doms[idom].rawatwd1.hits[ihit].adc=adc;
     event.doms[idom].rawatwd1.hits[ihit].le=le;
     event.doms[idom].rawatwd1.hits[ihit].tot=tot;
     event.doms[idom].rawatwd1.hits[ihit].deadadc=deadadc;
     event.doms[idom].rawatwd1.hits[ihit].deadle=deadle;
     event.doms[idom].rawatwd1.hits[ihit].deadtot=deadtot;
     event.doms[idom].rawatwd1.hits[ihit].slc=slc;
     stored=1;
    }
    if (sigx->GetSignal("RAW-ATWD2"))
    {
     event.doms[idom].rawatwd2.nhits+=1;
     ihit=event.doms[idom].rawatwd2.nhits-1;
     event.doms[idom].rawatwd2.hits[ihit].adc=adc;
     event.doms[idom].rawatwd2.hits[ihit].le=le;
     event.doms[idom].rawatwd2.hits[ihit].tot=tot;
     event.doms[idom].rawatwd2.hits[ihit].deadadc=deadadc;
     event.doms[idom].rawatwd2.hits[ihit].deadle=deadle;
     event.doms[idom].rawatwd2.hits[ihit].deadtot=deadtot;
     event.doms[idom].rawatwd2.hits[ihit].slc=slc;
     stored=1;
    }
    if (sigx->GetSignal("RAW-ATWD3"))
    {
     event.doms[idom].rawatwd3.nhits+=1;
     ihit=event.doms[idom].rawatwd3.nhits-1;
     event.doms[idom].rawatwd3.hits[ihit].adc=adc;
     event.doms[idom].rawatwd3.hits[ihit].le=le;
     event.doms[idom].rawatwd3.hits[ihit].tot=tot;
     event.doms[idom].rawatwd3.hits[ihit].deadadc=deadadc;
     event.doms[idom].rawatwd3.hits[ihit].deadle=deadle;
     event.doms[idom].rawatwd3.hits[ihit].deadtot=deadtot;
     event.doms[idom].rawatwd3.hits[ihit].slc=slc;
     stored=1;
    }
    if (sigx->GetSignal("RAW-FADC"))
    {
     event.doms[idom].rawfadc.nhits+=1;
     ihit=event.doms[idom].rawfadc.nhits-1;
     event.doms[idom].rawfadc.hits[ihit].adc=adc;
     event.doms[idom].rawfadc.hits[ihit].le=le;
     event.doms[idom].rawfadc.hits[ihit].tot=tot;
     event.doms[idom].rawfadc.hits[ihit].deadadc=deadadc;
     event.doms[idom].rawfadc.hits[ihit].deadle=deadle;
     event.doms[idom].rawfadc.hits[ihit].deadtot=deadtot;
     event.doms[idom].rawfadc.hits[ihit].slc=slc;
     stored=1;
    }
    if (sigx->GetSignal("Q-STAMP"))
    {
     event.doms[idom].qstamp.nhits+=1;
     ihit=event.doms[idom].qstamp.nhits-1;
     event.doms[idom].qstamp.hits[ihit].adc=adc;
     event.doms[idom].qstamp.hits[ihit].le=le;
     event.doms[idom].qstamp.hits[ihit].tot=tot;
     event.doms[idom].qstamp.hits[ihit].deadadc=deadadc;
     event.doms[idom].qstamp.hits[ihit].deadle=deadle;
     event.doms[idom].qstamp.hits[ihit].deadtot=deadtot;
     event.doms[idom].qstamp.hits[ihit].slc=slc;
     stored=1;
    }
    if (!stored)
    {
     event.doms[idom].dst.nhits+=1;
     ihit=event.doms[idom].dst.nhits-1;
     event.doms[idom].dst.hits[ihit].adc=adc;
     event.doms[idom].dst.hits[ihit].le=le;
     event.doms[idom].dst.hits[ihit].tot=tot;
     event.doms[idom].dst.hits[ihit].deadadc=deadadc;
     event.doms[idom].dst.hits[ihit].deadle=deadle;
     event.doms[idom].dst.hits[ihit].deadtot=deadtot;
     event.doms[idom].dst.hits[ihit].slc=slc;
     stored=1;
    }
   }
  } // End of loop over DOMs

  otree->Fill();
 } // End of loop over entries
}
