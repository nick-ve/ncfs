//////////////////////////////////////////////////////////
// ROOT macro to model the GRB neutrino analysis method.
//
// ********************************************************
// ***         This code has become obsolete            ***
// *** Please use the more generic macro burst-model.cc ***
// ********************************************************
//
// To run this macro, it is necessary to have the ROOT
// framework (http://root.cern.ch) installed.
// In addition the NCFS-Pack (astro)physics analysis
// framework (http://sites.google.com/site/nickveweb)
// has to be installed as well.
// Both the ROOT and NCFS packages are freely available.
//
// To run the macro and obtain the output in a file called
// "output.log" just issue the command ($ means cmd prompt):
//
// $root -b -q grbmodel.cc >output.log
//
// Author   : Nick van Eijndhoven 17-feb-2006 Utrecht University
// Modified : NvE 01-feb-2007 Utrecht University
// Modified : NvE 14-feb-2007 Utrecht University
// Modified : NvE 26-jul-2007 Utrecht University
// Modified : NvE 07-oct-2007 Utrecht University
//            Variable bin size option implemented.
// Modified : NvE 18-jul-2008 Utrecht University
//            Flat background Psi expectation calculated
//            via the new PsiExtreme facility.
// Modified : NvE 30-mar-2010 IIHE/NCFS
//            Adopted the NCFS-Pack namings.
// Modified : NvE 08-nov-2010 IIHE/NCFS
//            Explicit Gaussian smearing on theta and phi
//            separately instead of only on opening angle
//            to incorporate detector resolution.
// Modified : NvE 22-apr-2011 IIHE/NCFS
//            Various functionalities in the macro more
//            clearly separated and comments added.
//            Attempt to judge Psi quality by using the
//            new features of NcMath::PsiExtreme.            
// Modified : NvE 06-sep-2012 IIHE/NCFS
//            Fix to prevent statistical overfluctuation
//            of number of GRB neutrinos in case no 
//            stat. fluctuations are requested (grbnu<0).
//            Thanks to Lionel Brayeur for identifying
//            this problem.
//////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");

 /////////////////////////////////////////////////////
 // User settings to reflect the physical situation //
 /////////////////////////////////////////////////////

 Int_t ngrbs=100;         // Number of fictative SWIFT GRB triggers in Northern hemisphere
 Float_t grbnu=-0.10;     // Number of detectable neutrinos per GRB in full IceCube (<0 : no stat. fluct.)
 Float_t grbz=2;          // Average observed GRB redshift
 Float_t grbt90=30;       // Mean observed GRB duration in seconds
 Float_t dtnu=grbt90;     // Mean time difference (in sec) between gammas and nus of the GRB
 Float_t dtnus=grbt90;    // Sigma of time difference (in sec) between gammas and nus of the GRB
 Float_t detfrac=1;       // Fraction of detector effective area w.r.t. full IceCube (0.01 for Ama II)
 Float_t thetares=1;      // Detector angular resolution on polar angle theta (degrees)
 Float_t phires=1;        // Detector angular resolution on azimuth angle phi (degrees)
 Float_t timres=1e-5;     // Detector time resolution (sec)
 Float_t nmupday=300;     // Mean number of upgoing bkg muons per day in full IceCube
 Float_t dtwin=7200.;     // Total search time window (in sec) centered at GRB trigger
 Float_t dawin=5;         // Ang. search circle (<0 is decl. band) in degrees around (above/below) GRB position
 Float_t nbkg=0.5;        // Mean number of bkg counts/bin in case of auto-binning
 Float_t tbin=5.*dtnus;   // Time bin size in sec (0=variable bins  <0 will result in a mean n bkg counts/bin)
 Float_t vartbin=10;      // Size (in sec) of the first time bin in case of variable time bins
 Float_t abin=1;          // Angular bin size in degrees (<0 will result in a mean n bkg counts/bin)
 Int_t freq=0;            // Use frequentist's approximation (1) or exact Bayesian expression (0)
 Int_t npsi=-250;         // Number of psi entries for bkg psi-value distributions (<0 : time shuffling)
 Int_t usetott=1;         // Use the observed tott number of entries in case of time shuffling 
 Int_t grbpos=1;          // Use the original grb locations (1) or random ones (0) for bkg studies
 Double_t nrandom=1e6;    // Number of randomised configurations for direct psi P-value determination

 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 //////////////////////////////////////////////
 // The implementation of the actual program //
 //////////////////////////////////////////////

 Float_t pi=acos(-1.);

 // Take detector effective area into account
 grbnu*=detfrac;
 nmupday*=detfrac;

 cout << endl;
 cout << " ============================ User provided settings ==================================" << endl;
 cout << " Number of GRBs in Northern hemisphere : " << ngrbs << endl;
 cout << " Average observed GRB redshift : " << grbz << endl;
 cout << " Mean time diff. (in sec) between GRB gammas and nus : " << dtnu << endl;
 cout << " Sigma of mean time diff. (in sec) between GRB gammas and nus : " << dtnus << endl;
 cout << " Fraction of the detector effective area w.r.t. full IceCube : " << detfrac << endl;
 cout << " Number (<0 is without stat. fluct.) of detectable neutrinos per GRB : " << grbnu << endl;
 cout << " Angular resolution in polar angle theta (degrees) of the detector : " << thetares << endl;
 cout << " Angular resolution in azimuth angle phi (degrees) of the detector : " << phires << endl;
 cout << " Time resolution (sec) of the detector : " << timres << endl;
 cout << " Mean number of upgoing bkg muons per day in the detector : " << nmupday << endl;
 cout << " Total search time window (in sec) centered at GRB trigger : " << dtwin << endl;
 cout << " Ang. search circle (<0 is decl. band) in degrees around (above/below) GRB position : " << dawin << endl;
 cout << " Mean number of bkg counts/bin in case of auto-binning : " << nbkg << endl;
 cout << " Time bin size in sec (0=variable bins  <0 will result in the above mean counts/bin) : " << tbin << endl;
 cout << " Size (in sec) of the first time bin in case of variable time bins : " << vartbin << endl;
 cout << " Ang. bin size in degrees (<0 will result in the above mean counts/bin) : " << abin << endl;
 cout << " Use Frequentist's approximation for psi determination : " << freq << endl;
 cout << " Number of psi entries for bkg psi-value distributions (<0 means time shuffling) : " << npsi << endl;
 cout << " Usage of the observed tott number of entries in case of time shuffling : " << usetott << endl; 
 cout << " Usage of actually observed GRB positions for bkg studies : " << grbpos << endl;
 cout << " Number of randomised configurations for direct psi P-value determination : " << nrandom << endl;
 cout << " ======================================================================================" << endl;
 cout << endl;

 Float_t danglow=0;    // Lower value (in degrees) of angular difference histo
 Float_t dangup=dawin; // Upper value (in degrees) of angular difference histo
 if (dangup<0 || dangup>180) dangup=180;

 Float_t mup=nmupday*dtwin/(3600.*24.); // Mean no. of upgoing bkg muons in search time window

 //////////////////////////////////////////////////////////////////////////
 // Automatic definition of the various signal and background histograms //
 // based on the provided user settings                                  //
 //////////////////////////////////////////////////////////////////////////

 Int_t ntbins=0;
 Double_t* binarr=0;
 if (fabs(tbin)>0) // Fixed time bins
 {
  if (tbin>0) ntbins=int(dtwin/tbin);
  if (tbin<0) ntbins=int(mup*float(ngrbs)/nbkg);
 }
 else // Variable time bins
 {
  Int_t nbx=int(dtwin/vartbin);
  Float_t gamma=grbz+1.;
  Float_t* bins=new Float_t[nbx];
  ntbins=0;
  Float_t xlow=0,xup=0,size=vartbin;
  for (Int_t i=0; i<nbx-1; i++)
  {
   xup=xlow+size;
   if (xup>dtwin/2.) // Store the last lowerbound
   {
    bins[i]=xlow;
    ntbins++;
    break;
   }
   bins[i]=xlow;
   ntbins++;
   xlow=xup;
   size=xlow*gamma;
  }
  binarr=new Double_t[2*ntbins-1];
  for (Int_t j=ntbins; j>0; j--)
  {
   binarr[ntbins-j]=-bins[j-1];
   binarr[ntbins+j-2]=bins[j-1];
  }
  ntbins=2*ntbins-2;
 }

 Int_t nabins=int((dangup-danglow)/abin);
 if (abin<0) nabins=int(((dangup-danglow)/180.)*mup*float(ngrbs)/nbkg);
 if (ntbins<2) ntbins=2;
 if (nabins<2) nabins=2;

 gStyle->SetOptStat("e"); // Only display number of entries in stats box

 TH1F* bkgt=0;
 TH1F* tott=0;
 TH2F* bkgt2=0;
 TH2F* tot2=0;
 if (fabs(tbin)>0) // Fixed time bins
 {
  bkgt=new TH1F("bkgt","t of bkg mu-up in twindow",ntbins,-dtwin/2.,dtwin/2.);
  tott=new TH1F("tott","t of all mu-up in twindow",ntbins,-dtwin/2.,dtwin/2.);
  bkg2=new TH2F("bkg2","t vs. dang of bkg mu-up in twindow",
                nabins/10,danglow,dangup,ntbins,-dtwin/2.,dtwin/2.);
  tot2=new TH2F("tot2","t vs. dang of all mu-up in twindow",
                nabins/10,danglow,dangup,ntbins,-dtwin/2.,dtwin/2.);
 }
 else // Variable time bins
 {
  bkgt=new TH1F("bkgt","t of bkg mu-up in twindow",ntbins,binarr);
  tott=new TH1F("tott","t of all mu-up in twindow",ntbins,binarr);
  bkg2=new TH2F("bkg2","t vs. dang of bkg mu-up in twindow",
                nabins/10,danglow,dangup,ntbins,binarr);
  tot2=new TH2F("tot2","t vs. dang of all mu-up in twindow",
                nabins/10,danglow,dangup,ntbins,binarr);
 }
 TH1F* bkga=new TH1F("bkga","dang of bkg mu-up in twindow",nabins,danglow,dangup);
 TH1F* tota=new TH1F("tota","dang of all mu-up in twindow",nabins,danglow,dangup);

 TString s;
 Int_t itbin=int(tbin);
 if (tbin<0) itbin=int(dtwin/float(ntbins));
 s="Counts per ";
 if (fabs(tbin)>0)
 {
  s+=itbin;
  s+=" seconds";
 }
 else
 {
  s+="time bin";
 }
 bkgt->GetXaxis()->SetTitle("Upgoing #mu arrival time (in sec) w.r.t. GRB #gamma trigger");
 bkgt->GetYaxis()->SetTitle(s.Data());
 tott->GetXaxis()->SetTitle("Upgoing #mu arrival time (in sec) w.r.t. GRB #gamma trigger");
 tott->GetYaxis()->SetTitle(s.Data());

 //////////////////////////////////////////////////////////
 // Generation of the signal and background observations //
 // based on the provided user settings                  //
 //////////////////////////////////////////////////////////

 Int_t nmup;
 Float_t thetagrb,phigrb,thetamu,phimu,cost;
 NcRandom rndm;
 Float_t dt=0;
 Float_t vec[3]={1,0,0};
 NcPosition rgrb;
 NcPosition rmu;
 Float_t dang;
 Float_t ranlow,ranup;
 TObjArray* grblocs=new TObjArray();
 Int_t nmugrb=0;
 // Generate the fictative GRB space-time positions in the Northern hemisphere
 for (Int_t igrb=0; igrb<ngrbs; igrb++)
 {
  ranlow=0;
  ranup=360;
  phigrb=rndm.Uniform(ranlow,ranup);
  ranlow=-1;
  ranup=0;
  cost=rndm.Uniform(ranlow,ranup);
  thetagrb=acos(cost);
  thetagrb*=180./pi;
  vec[1]=thetagrb;
  vec[2]=phigrb;
  rgrb.SetPosition(vec,"sph","deg");

  grblocs->Add(new NcPositionObj(rgrb));

  // Generate the upgoing bkg muons in the search window
  nmup=int(rndm.Poisson(mup));
  for (Int_t imup=0; imup<nmup; imup++)
  {
   ranlow=-dtwin/2.;
   ranup=dtwin/2.;
   dt=rndm.Uniform(ranlow,ranup);
   // Smear the time difference with the Gaussian time resolution
   if (timres>0) dt=rndm.Gauss(dt,timres);
   ranlow=0;
   ranup=360;
   phimu=rndm.Uniform(ranlow,ranup);
   ranlow=-1;
   ranup=0;
   cost=rndm.Uniform(ranlow,ranup);
   thetamu=acos(cost);
   thetamu*=180./pi;
   // Smear the direction of the upgoing bkg muon according to  the detector resolution
   if (thetares>0) thetamu=rndm.Gauss(thetamu,thetares);
   if (phires>0) phimu=rndm.Gauss(phimu,phires);
   vec[1]=thetamu;
   vec[2]=phimu;
   rmu.SetPosition(vec,"sph","deg");

   if (dawin<0) // Declination band
   {
    dang=fabs(thetagrb-thetamu);
   }
   else // Circle around GRB position
   {
    dang=rgrb.GetOpeningAngle(rmu,"deg");
   }

   if (dang>fabs(dawin)) continue;

   bkgt->Fill(dt);
   tott->Fill(dt);
   bkga->Fill(dang);
   tota->Fill(dang);
   bkg2->Fill(dang,dt);
   tot2->Fill(dang,dt);
  }

  // Generate the GRB related upgoing muon(s) in the search window

  // Prevent statistical overfluctuation in number of GRB muons if requested by grbnu<0
  if (grbnu<0 && nmugrb>=int(fabs(grbnu)*float(ngrbs))) continue;

  nmup=int(fabs(grbnu));
  if (!nmup && rndm.Uniform()<fabs(grbnu)) nmup=1;
  for (Int_t imup2=0; imup2<nmup; imup2++)
  {
   nmugrb++;
   dt=rndm.Gauss(dtnu,dtnus);
   if (timres>0) dt=rndm.Gauss(dt,timres);
   thetamu=thetagrb;
   phimu=phigrb;
   // Smear the direction of the upgoing bkg muon according to  the detector resolution
   if (thetares>0) thetamu=rndm.Gauss(thetamu,thetares);
   if (phires>0) phimu=rndm.Gauss(phimu,phires);
   vec[1]=thetamu;
   vec[2]=phimu;
   rmu.SetPosition(vec,"sph","deg");

   dang=rgrb.GetOpeningAngle(rmu,"deg");

   if (dang>fabs(dawin)) continue;

   tott->Fill(dt);
   tota->Fill(dang);
   tot2->Fill(dang,dt);
  }
 }

 // Compensate statistical underfluctuation in number of GRB muons if requested by grbnu<0
 if (grbnu<0)
 {
  nmup=int(fabs(grbnu)*float(ngrbs));
  while (nmugrb<nmup)
  {
   nmugrb++;
   dt=rndm.Gauss(dtnu,dtnus);
   if (timres>0) dt=rndm.Gauss(dt,timres);
   thetamu=thetagrb;
   phimu=phigrb;
   // Smear the direction of the upgoing bkg muon according to  the detector resolution
   if (thetares>0) thetamu=rndm.Gauss(thetamu,thetares);
   if (phires>0) phimu=rndm.Gauss(phimu,phires);
   vec[1]=thetamu;
   vec[2]=phimu;
   rmu.SetPosition(vec,"sph","deg");

   dang=rgrb.GetOpeningAngle(rmu,"deg");

   if (dang>fabs(dawin)) continue;

   tott->Fill(dt);
   tota->Fill(dang);
   tot2->Fill(dang,dt);
  }
 } 

 ////////////////////////////////////////////////////////////////////////////////
 // Statistical evaluation of the generated signal and background observations //
 //                                                                            //
 // Determination of the Bayesian psi value for the time and angular histos    //
 // under the assumption that there is no GRB signal.                          //
 // This corresponds to searching out the Bernoulli class B_m                  //
 // with m=nbins of the histogram.                                             //
 // An orthodox chi-squared analysis is also performed.                        //
 ////////////////////////////////////////////////////////////////////////////////

 NcMath math;

 Int_t nbt=tott->GetNbinsX();
 Int_t nba=tota->GetNbinsX();
 Float_t nentot=tott->GetEntries();
 Float_t nenbkg=bkgt->GetEntries();

 // Statistics of the event samples
 cout << " *** Statistics of the observed event samples ***" << endl;
 cout << " Number of entries : " << nentot << " Number of time bins : " << nbt << " Number of angular bins : " << nba << endl;
 cout << " --- (Unknown) Number of bkg entries : " << nenbkg << " ==> Number of signal entries : " << (nentot-nenbkg) << endl;
 cout << endl; 

 // Time histo Bayesian statistics
 Double_t psitott=math.PsiValue(tott,0,0,freq);
 Double_t psibkgt=math.PsiValue(bkgt,0,0,freq);

 // Angular histo Bayesian statistics
 TF1 pdfa("pdfa","sin(x*acos(-1.)/180.)");
 Double_t psitota=math.PsiValue(tota,0,&pdfa,freq);
 Double_t psibkga=math.PsiValue(bkga,0,&pdfa,freq);

 Float_t psitdif=psitott-psibkgt;
 Float_t psiadif=psitota-psibkga;

 // Direct determination of the P-value of the observed psi values
 Double_t pvaluet=math.PsiPvalue(-1,nrandom,tott,0,0,freq);
 Double_t pvaluea=math.PsiPvalue(-1,nrandom,tota,0,&pdfa,freq);

 cout << " *** Observed Bayesian psi values (in dB) for the hyp. of no GRB signal ***" << endl;
 cout << " psi for tott : " << psitott << " tota : " << psitota << endl;
 cout << " ==> P-value of the observed tott psi : " << pvaluet << " Used # of randomisations : " << nrandom << endl;
 cout << " ==> P-value of the observed tota psi : " << pvaluea << " Used # of randomisations : " << nrandom << endl;
 cout << " --- (Unknown) Corresponding psi values (in dB) for the bkg upgoing muons ---" << endl;
 cout << " psi for bkgt : " << psibkgt << " bkga : " << psibkga << endl;
 cout << " --- (Unknown) Difference between observed and bkg psi values (in dB) ---" << endl;
 cout << " Delta psi for tott-bkgt : " << psitdif << " tota-bkga : " << psiadif << endl;

 // Extreme Psi values of pure background for the recorded time and angular entries
 Float_t psimintott=math.PsiExtreme(tott,0,0,-2);
 Float_t psimaxtott=math.PsiExtreme(tott,0,0,-1);
 Float_t psifractott=(psimaxtott-psitott)/(psimaxtott-psimintott);
 Float_t psiminbkgt=math.PsiExtreme(bkgt,0,0,-2);
 Float_t psimaxbkgt=math.PsiExtreme(bkgt,0,0,-1);
 Float_t psifracbkgt=(psimaxbkgt-psibkgt)/(psimaxbkgt-psiminbkgt);
 Float_t psimintota=math.PsiExtreme(tota,0,&pdfa,-2);
 Float_t psimaxtota=math.PsiExtreme(tota,0,&pdfa,-1);
 Float_t psifractota=(psimaxtota-psitota)/(psimaxtota-psimintota);
 Float_t psiminbkga=math.PsiExtreme(bkga,0,&pdfa,-2);
 Float_t psimaxbkga=math.PsiExtreme(bkga,0,&pdfa,-1);
 Float_t psifracbkga=(psimaxbkga-psibkga)/(psimaxbkga-psiminbkga);
 cout << " === Extreme Psi values for the case of pure background ===" << endl;
 cout << " *** tott psimin : " << psimintott << " psimax : " << psimaxtott << " (psimax-psi)/range : " << psifractott << endl;
 cout << " --- (Unknown) bkgt psimin : " << psiminbkgt << " psimax : " << psimaxbkgt << " (psimax-psi)/range : " << psifracbkgt << endl;
 cout << " *** tota psimin : " << psimintota << " psimax : " << psimaxtota << " (psimax-psi)/range : " << psifractota << endl;
 cout << " --- (Unknown) bkga psimin : " << psiminbkga << " psimax : " << psimaxbkga << " (psimax-psi)/range : " << psifracbkga << endl;
 cout << endl;

 // The time conventional chi-squared evaluation
 Int_t ndftott;
 Float_t chitott=math.Chi2Value(tott,0,0,&ndftott);
 Int_t ndfbkgt;
 Float_t chibkgt=math.Chi2Value(bkgt,0,0,&ndfbkgt);

 // The angular conventional chi-squared evaluation
 Int_t ndftota;
 Float_t chitota=math.Chi2Value(tota,0,&pdfa,&ndftota);
 Int_t ndfbkga;
 Float_t chibkga=math.Chi2Value(bkga,0,&pdfa,&ndfbkga);

 cout << endl;
 cout << " *** Observed chi-squared values for the hypothesis of no GRB signal ***" << endl;
 cout << " chi2 for tott : " << chitott << " ndf : " << ndftott
      << " P-value : " << math.Chi2Pvalue(chitott,ndftott)
      << " (" << math.Chi2Pvalue(chitott,ndftott,0,1) << " * sigma)" << endl;
 cout << " chi2 for tota : " << chitota << " ndf : " << ndftota
      << " P-value : " << math.Chi2Pvalue(chitota,ndftota)
      << " (" << math.Chi2Pvalue(chitota,ndftota,0,1) << " * sigma)" << endl;
 cout << endl;
 cout << " --- (Unknown) Corresponding chi-squared values for the bkg upgoing muons ---" << endl;
 cout << " chi2 for bkgt : " << chibkgt << " ndf : " << ndfbkgt
      << " P-value : " << math.Chi2Pvalue(chibkgt,ndfbkgt)
      << " (" << math.Chi2Pvalue(chibkgt,ndfbkgt,0,1) << " * sigma)" << endl;
 cout << " chi2 for bkga : " << chibkga << " ndf : " << ndfbkga
      << " P-value : " << math.Chi2Pvalue(chibkga,ndfbkga)
      << " (" << math.Chi2Pvalue(chibkga,ndfbkga,0,1) << " * sigma)" << endl;

 // Statistics of bkg psi-value distribution
 if (npsi)
 {
  cout << endl;
  cout << " +++ Simulating GRB background measurements +++" << endl;
  if (npsi>0)
  {
   cout << " The above analysis will be repeated (off-burst) " << npsi << " times." << endl;
  }
  else
  {
   if (!usetott)
   {
    cout << " The above GRB samples will be taken (off-burst) only once more." << endl;
    cout << " By random re-filling the obtained bkg time entries we construct " << abs(npsi) << " bkg samples." << endl;
   }
   else
   {
    cout << " By random re-filling the original tott time entries we construct " << abs(npsi) << " bkg samples." << endl;
   }
  }
  cout << endl;

  TH1F* hpsibkgt=0;
  if (fabs(tbin)>0)
  {
   hpsibkgt=new TH1F("hpsibkgt","t of bkg mu-up in twindow",ntbins,-dtwin/2.,dtwin/2.);
  }
  else
  {
   hpsibkgt=new TH1F("hpsibkgt","t of bkg mu-up in twindow",ntbins,binarr);
  }
  TH1F* hpsibkga=new TH1F("hpsibkga","dang of bkg mu-up in twindow",nabins,danglow,dangup);
  TH1F* hpsit=new TH1F("hpsit","time bkg psi-value distribution",100,0,2.*psitott);
  TH1F* hpsia=new TH1F("hpsia","angular bkg psi-value distribution",100,0,2.*psitota);
  TObjArray* bkgthists=new TObjArray();
  TObjArray* bkgahists=new TObjArray();
  Float_t bkgpsit=0,bkgchit=0;
  Float_t bkgpsia=0,bkgchia=0;
  NcSample psit,psia,chit,chia;
  psit.SetStoreMode(1);
  psia.SetStoreMode(1);
  chit.SetStoreMode(1);
  chia.SetStoreMode(1);
  Int_t nloop=npsi,nshuffle=1;
  if (npsi<0)
  {
   nloop=1;
   nshuffle=abs(npsi);
  }
  for (Int_t ipsi=0; ipsi<nloop; ipsi++)
  {
   hpsibkgt->Reset();
   hpsibkga->Reset();

   // Generate the fictative GRB space-time positions in the Northern hemisphere
   // In case the grbpos flag was activated, the above SWIFT GRB positions will be used 
   for (Int_t jgrb=0; jgrb<ngrbs; jgrb++)
   {
    if (grbpos)
    {
     NcPositionObj* rx=(NcPositionObj*)grblocs->At(jgrb);
     rgrb=rx->GetPosition();
    }
    else
    {
     ranlow=0;
     ranup=360;
     phigrb=rndm.Uniform(ranlow,ranup);
     ranlow=-1;
     ranup=0;
     cost=rndm.Uniform(ranlow,ranup);
     thetagrb=acos(cost);
     thetagrb*=180./pi;
     vec[1]=thetagrb;
     vec[2]=phigrb;
     rgrb.SetPosition(vec,"sph","deg");
    }

    // Generate the upgoing bkg muons in the search window
    nmup=int(rndm.Poisson(mup));
    for (Int_t jmup=0; jmup<nmup; jmup++)
    {
     ranlow=-dtwin/2.;
     ranup=dtwin/2.;
     dt=rndm.Uniform(ranlow,ranup);
     // Smear the time difference with the Gaussian time resolution
     if (timres>0) dt=rndm.Gauss(dt,timres);

     ranlow=0;
     ranup=360;
     phimu=rndm.Uniform(ranlow,ranup);
     ranlow=-1;
     ranup=0;
     cost=rndm.Uniform(ranlow,ranup);
     thetamu=acos(cost);
     thetamu*=180./pi;
     // Smear the direction of the upgoing bkg muon according to the detector resolution
     if (thetares>0) thetamu=rndm.Gauss(thetamu,thetares);
     if (phires>0) phimu=rndm.Gauss(phimu,phires);
     vec[1]=thetamu;
     vec[2]=phimu;
     rmu.SetPosition(vec,"sph","deg");

     if (dawin<0) // Declination band
     {
      dang=fabs(thetagrb-thetamu);
     }
     else // Circle around GRB position
     {
      dang=rgrb.GetOpeningAngle(rmu,"deg");
     }

     if (dang>fabs(dawin)) continue;

     hpsibkgt->Fill(dt);
     hpsibkga->Fill(dang);
    }
   } // End of nburst loop

   // Get the corresponding psi values for this bkg simulation

   // The angular location histos
   bkgpsia=math.PsiValue(hpsibkga,0,&pdfa,freq);
   bkgchia=math.Chi2Value(hpsibkga,0,&pdfa);

   bkgahists->Add(new TH1F(*hpsibkga));

   // The arrival time histos

   // Refill the time histo by a random re-distribution
   // of the same number of entries as tott
   if (usetott && npsi<0)
   {
    hpsibkgt->Reset();
    nenbkg=tott->GetEntries();
    for (Int_t ient=0; ient<nenbkg; ient++)
    {
     ranlow=-dtwin/2.;
     ranup=dtwin/2.;
     dt=rndm.Uniform(ranlow,ranup);
     hpsibkgt->Fill(dt);
    }
   }

   for (Int_t ishuffle=0; ishuffle<nshuffle; ishuffle++)
   {
    bkgpsit=math.PsiValue(hpsibkgt,0,0,freq);
    bkgchit=math.Chi2Value(hpsibkgt);

    bkgthists->Add(new TH1F(*hpsibkgt));
         
    psit.Enter(bkgpsit);
    chit.Enter(bkgchit);
    hpsit->Fill(bkgpsit);

    // Refill the time histo by a random re-distribution of the same number of entries
    nenbkg=hpsibkgt->GetEntries();
    hpsibkgt->Reset();
    for (Int_t ient=0; ient<nenbkg; ient++)
    {
     ranlow=-dtwin/2.;
     ranup=dtwin/2.;
     dt=rndm.Uniform(ranlow,ranup);
     hpsibkgt->Fill(dt);
    }
   } // End of shuffle loop

   psia.Enter(bkgpsia);
   chia.Enter(bkgchia);
   hpsia->Fill(bkgpsia);
  } // End of psi loop

  Float_t psitmean=psit.GetMean(1);
  Float_t psitsigma=psit.GetSigma(1);
  Float_t psitmedian=psit.GetMedian(1);
  Float_t psitspread=psit.GetSpread(1);
  Float_t psitdiff=psitott-psitmean;
  Float_t psitdiff2=psitott-psitmedian;
  Float_t psiamean=psia.GetMean(1);
  Float_t psiasigma=psia.GetSigma(1);
  Float_t psiamedian=psia.GetMedian(1);
  Float_t psiaspread=psia.GetSpread(1);
  Float_t psiadiff=psitota-psiamean;
  Float_t psiadiff2=psitota-psiamedian;
  cout << " *** Observed Bayesian bkg psi-value (in dB) statistics for " << abs(npsi) << " entries ***" << endl;
  cout << " Time bkg psi distr. Mean : " << psitmean << " Sigma : " << psitsigma
       << " Median : " << psitmedian << " Spread : " << psitspread << endl;
  if (npsi>0)
  {
   cout << " Ang. bkg psi distr. Mean : " << psiamean << " Sigma : " << psiasigma
        << " Median : " << psiamedian << " Spread : " << psiaspread << endl;
  }
  cout << " *** Comparison with GRB observed psi-values (in dB) ***" << endl;
  cout << " Time psi-psimean : " << psitdiff;
  if (psitdiff && psitsigma>0) cout << " (" << fabs(psitdiff/psitsigma) << " * sigma)";
  cout << " psi-psimedian : " << psitdiff2;
  if (psitdiff2 && psitspread>0) cout << " (" << fabs(psitdiff2/psitspread) << " * spread)";
  cout << endl;
  if (psitdiff && psitsigma>0)
  {
   cout << " ===> Two sided Gaussian P-value of psi w.r.t. bkg psimean  : " << math.GaussPvalue(fabs(psitdiff/psitsigma)) << endl;
  }
  if (npsi>0)
  {
   cout << " Ang. psi-psimean : " << psiadiff;
   if (psiadiff && psiasigma>0) cout << " (" << fabs(psiadiff/psiasigma) << " * sigma)";
   cout << " psi-psimedian : " << psiadiff2;
   if (psiadiff2 && psiaspread>0) cout << " (" << fabs(psiadiff2/psiaspread) << " * spread)";
   cout << endl;
   if (psiadiff && psiasigma>0)
   {
    cout << " ===> Two sided Gaussian P-value of psi w.r.t. bkg psimean  : " << math.GaussPvalue(fabs(psiadiff/psiasigma)) << endl;
   }
  }

  Float_t chitmean=chit.GetMean(1);
  Float_t chitsigma=chit.GetSigma(1);
  Float_t chitmedian=chit.GetMedian(1);
  Float_t chitspread=chit.GetSpread(1);
  Float_t chitdiff=chitott-chitmean;
  Float_t chitdiff2=chitott-chitmedian;
  Float_t chiamean=chia.GetMean(1);
  Float_t chiasigma=chia.GetSigma(1);
  Float_t chiamedian=chia.GetMedian(1);
  Float_t chiaspread=chia.GetSpread(1);
  Float_t chiadiff=chitota-chiamean;
  Float_t chiadiff2=chitota-chiamedian;
  cout << " *** Observed bkg chi-squared statistics for " << abs(npsi) << " entries ***" << endl;
  cout << " Time bkg chi-squared values Mean : " << chitmean << " Sigma : " << chitsigma
       << " Median : " << chitmedian << " Spread : " << chitspread << endl;
  if (npsi>0)
  {
   cout << " Ang. bkg chi-squared values Mean : " << chiamean << " Sigma : " << chiasigma
        << " Median : " << chiamedian << " Spread : " << chiaspread << endl;
  }
  cout << " *** Comparison with GRB observed chi-squared values ***" << endl;
  cout << " Time chi-chimean : " << chitdiff;
  if (chitdiff && chitsigma>0) cout << " (" << fabs(chitdiff/chitsigma) << " * sigma)";
  cout << " chi-chimedian : " << chitdiff2;
  if (chitdiff2 && chitspread>0) cout << " (" << fabs(chitdiff2/chitspread) << " * spread)";
  cout << endl;
  if (npsi>0)
  {
   cout << " Ang. chi-chimean : " << chiadiff;
   if (chiadiff && chiasigma>0) cout << " (" << fabs(chiadiff/chiasigma) << " * sigma)";
   cout << " chi-chimedian : " << chiadiff2;
   if (chiadiff2 && chiaspread>0) cout << " (" << fabs(chiadiff2/chiaspread) << " * spread)";
   cout << endl;
  }

  Float_t psibkgtdiff=psibkgt-psitmean;
  Float_t psibkgtdiff2=psibkgt-psitmedian;
  Float_t psibkgadiff=psibkga-psiamean;
  Float_t psibkgadiff2=psibkga-psiamedian;
  Float_t chibkgtdiff=chibkgt-chitmean;
  Float_t chibkgtdiff2=chibkgt-chitmedian;
  Float_t chibkgadiff=chibkga-chiamean;
  Float_t chibkgadiff2=chibkga-chiamedian;
  cout << endl;
  cout << " --- Comparison with (unknown) GRB bkg psi-values (in dB) ---" << endl;
  cout << " Time psibkg-psimean : " << psibkgtdiff;
  if (psibkgtdiff && psitsigma>0) cout << " (" << fabs(psibkgtdiff/psitsigma) << " * sigma)";
  cout << " psibkg-psimedian : " << psibkgtdiff2;
  if (psibkgtdiff2 && psitspread>0) cout << " (" << fabs(psibkgtdiff2/psitspread) << " * spread)";
  cout << endl;
  if (npsi>0)
  {
   cout << " Ang. psibkg-psimean : " << psibkgadiff;
   if (psibkgadiff && psiasigma>0) cout << " (" << fabs(psibkgadiff/psiasigma) << " * sigma)";
   cout << " psibkg-psimedian : " << psibkgadiff2;
   if (psibkgadiff2 && psiaspread>0) cout << " (" << fabs(psibkgadiff2/psiaspread) << " * spread)";
   cout << endl;
  }
  cout << " --- Comparison with (unknown) GRB bkg chi-squared values ---" << endl;
  cout << " Time chi-chimean : " << chibkgtdiff;
  if (chibkgtdiff && chitsigma>0) cout << " (" << fabs(chibkgtdiff/chitsigma) << " * sigma)";
  cout << " chi-chimedian : " << chibkgtdiff2;
  if (chibkgtdiff2 && chitspread>0) cout << " (" << fabs(chibkgtdiff2/chitspread) << " * spread)";
  cout << endl;
  if (npsi>0)
  {
   cout << " Ang. chi-chimean : " << chibkgadiff;
   if (chibkgadiff && chiasigma>0) cout << " (" << fabs(chibkgadiff/chiasigma) << " * sigma)";
   cout << " chi-chimedian : " << chibkgadiff2;
   if (chibkgadiff2 && chiaspread>0) cout << " (" << fabs(chibkgadiff2/chiaspread) << " * spread)";
   cout << endl;
  }
 }

  cout << endl;
  cout << " ====== Histos generated ======" << endl;
  cout << " Observations and (unknown) background : tott tota tot2 bkgt bkga bkg2" << endl;  
  if (npsi)
  {
   cout << " Background studies : hpsibkgt hpsibkga hpsit hpsia" << endl;
   cout << " and all hpsibkgt and hpsibkga histos in the TObjArrays bkgthists and bkgahists" << endl;
  }
}
