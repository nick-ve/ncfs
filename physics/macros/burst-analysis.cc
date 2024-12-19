///////////////////////////////////////////////////////////////////
// ROOT macro to study the behaviour of signal detection related //
// to (astrophysical) transient phenomena.                       //
// This macro makes use of the generic NcAstrolab facility       //
// of the ROOT based NCFS-Pack framework.                        //
// See https://nick-ve.github.io/ncfs/docs for the documentation.//
//                                                               //
// Note that the corresponding ROOT and NCFS packages have to be //
// correctly installed before one can use this macro.            //
// For information about the installation of these packages,     //
// see https://nick-ve.github.io/ncfs                            //
//                                                               //
// To execute the macro, just invoke ($ = prompt)                //
// $ root -b -q burst-analysis.cc                                //
//                                                               //
// Several standard histograms, depending on the user selections,//
// are automatically generated and may be written into a ROOT    //
// output file for later inspection, as indicated below.         //
//                                                               //
//--- Nick van Eijndhoven, IIHE-VUB, Brussel, January 7, 2022    //
///////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");

 NcVersion version;
 version.Data();

 // The virtual lab for a cosmic transient phenomena study
 NcAstrolab lab;

 lab.SetExperiment("IceCube");
 lab.SetUT("11-04-2020","12:00:00.0",0); // Fixed fictative analysis date to get same random seed
 lab.SetRandomiser(-1); // Use the UT timestamp to generate a seed

 // Use time scrambling to obtain off-source data
 Int_t tmode=-3;
 Float_t tmin=-3600*12;
 Float_t tmax=3600*12;
 lab.SetTimeScramble(tmode,tmin,tmax);

 lab.Data();

 Float_t pi=acos(-1.);

 // User defined parameter settings
 Float_t bkgrate=-1./(30.*24.*3600.*2.*pi); // On average 1 Northern track alert per month
 lab.SetBurstParameter("Nmaxsrc",-1);
 lab.SetBurstParameter("Nmaxevt",-1);
 lab.SetBurstParameter("Bkgrate",bkgrate);
 lab.SetBurstParameter("Declmin",-5);
 lab.SetBurstParameter("Declmax",85);
 lab.SetBurstParameter("Zmin",-1e-6);
 lab.SetBurstParameter("Zmax",20);
 lab.SetBurstParameter("Nbkg",1);
 lab.SetBurstParameter("Tunits",0);
 lab.SetBurstParameter("Tmin",-500);
 lab.SetBurstParameter("Tmax",500);
 lab.SetBurstParameter("Tbint90",0);
 lab.SetBurstParameter("Tbin",1./24.);
 lab.SetBurstParameter("Datype",2);
 lab.SetBurstParameter("Dawin",3);
 lab.SetBurstParameter("Sigmamax",0.1);
 lab.SetBurstParameter("Angresmax",1);
 lab.SetBurstParameter("Abin",0.1);
 lab.SetBurstParameter("Emin",0);

 lab.ListBurstParameters();

 // Use experimental distributions instead of parametrizations
/// lab.MakeBurstZdist("../grbweb/GRB-z-Swift.root","T","z",200,0,20);
/// lab.MakeBurstT90dist("../grbweb/GRB-t90-Fermi.root","T","t90");
/// lab.MakeBurstSigmaPosdist("../grbweb/GRBweb.root","T","sigmapos","deg");
/// lab.MakeBurstEnergydist(-1,"IC86*data.root","tree","logE","dec","rad",200,1e7,1000);
/// lab.MakeBurstRecoAngresdist("IC86*data.root","tree","logE","angErr","rad","dec","rad",200,1e7);

 // Obtain burst locations, durations etc.

 lab.InitDataNames(1,"equ");
 lab.SetDataNames("Name","name");
 lab.SetDataNames("Date","date","yyyymmdd");
 lab.SetDataNames("Tobs","mjdtrig","MJD");
 lab.SetDataNames("a","ra","deg");
 lab.SetDataNames("b","dec","deg");
 lab.SetDataNames("csigma","sigmapos","deg");
 lab.SetDataNames("T90","t90");
 lab.SetDataNames("Tstart","mjdt90start","MJD");
 lab.SetDataNames("T100","t100");
 lab.SetDataNames("S","fluence");
 lab.SetDataNames("z","z");

 lab.ListDataNames();

 Bool_t src=kTRUE;
 lab.LoadInputData(src,"../grbweb/GRBweb.root","T"); // Load from observed GCN data

 // Obtain observed event data

 lab.InitDataNames(1,"equ");
 lab.SetDataNames("Name","name");
 lab.SetDataNames("Run","run");
 lab.SetDataNames("Event","event");
 lab.SetDataNames("VetoLevel","crveto");
 lab.SetDataNames("Tobs","mjd","MJD");
 lab.SetDataNames("a","ra","deg");
 lab.SetDataNames("b","dec","deg");
 lab.SetDataNames("csigma","sigmapos","deg");
 lab.SetDataNames("E","E","1e3");

 lab.ListDataNames();

 Bool_t src=kFALSE;
 lab.LoadInputData(src,"IC-alerts.root","T"); // Load from observed event data

 // Provide a listing of the first 5 stored entries for both the sources (c.q. burst) and the observed events
 cout << endl;
 lab.ListSignals("equ","J",3,"M",5);
 cout << endl;

 // Match signals with sources
 NcDevice matches;
 lab.MatchBurstData(matches); // Match all GRBs and all observed events

 matches.Data();

 // Provide a listing of all relevant parameters
 cout << endl;
 lab.ListBurstParameters();

 // Perform various statistical analyses
 Double_t rlow,rup;
 lab.GetBurstBayesianSignalRate(90,rlow,rup);

 lab.GetBurstLiMaSignificance();

 Double_t nr=1e4;
 Int_t ncut=10;
 Int_t ndt=2;
 lab.GetBurstBayesianPsiStatistics("angle",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("cosa",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("dt",nr,ncut,ndt);
 lab.GetBurstBayesianPsiStatistics("BBrat",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("dt",nr,ncut,ndt,kTRUE);
 lab.GetBurstBayesianPsiStatistics("BBrat",nr,ncut,ndt,kTRUE);

 // Produce a ROOT output file with the produced (standard) histograms
 lab.WriteBurstHistograms("burst-analysis.root");
}
