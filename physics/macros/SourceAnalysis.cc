///////////////////////////////////////////////////////////////////
// ROOT macro to study the behaviour of signal detection related //
// to astrophysical steady sources.                              //
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
// $ root -b -q SourceAnalysis.cc                                //
//                                                               //
// Several standard histograms, depending on the user selections,//
// are automatically generated and may be written into a ROOT    //
// output file for later inspection, as indicated below.         //
//                                                               //
//--- Nick van Eijndhoven, IIHE-VUB, Brussel, February 5, 2026   //
///////////////////////////////////////////////////////////////////
void SourceAnalysis()
{
 gSystem->Load("ncfspack");

 NcVersion version;
 version.Data();

 // The virtual lab for a cosmic transient phenomena study
 NcAstrolab lab;

 lab.SetExperiment("IceCube");
 lab.SetUT("11-04-2020","12:00:00.0",0); // Fixed fictative analysis date to get same random seed
 lab.SetRandomiser(-1); // Use the UT timestamp to generate a seed

 // Use position scrambling to obtain off-source data
 Int_t pmode=-3;
 Float_t dmin=0;
 Float_t dmax=0;
 Float_t thmin=0;
 Float_t thmax=0;
 Float_t phimin=30;
 Float_t phimax=150;
 lab.SetPositionScramble(pmode,dmin,dmax,0,thmin,thmax,0,phimin,phimax);

 lab.Data();

 Float_t pi=acos(-1.);

 /////////////////////////////////////
 // User defined parameter settings //
 /////////////////////////////////////
 Float_t bkgrate=-0.003/(2.*pi);
 lab.SetBurstParameter("Nbkg",2);
 lab.SetBurstParameter("Nmaxsrc",-1);
 lab.SetBurstParameter("Nmaxevt",1000);
 lab.SetBurstParameter("Grbnu",0);
 lab.SetBurstParameter("Bkgrate",bkgrate);
 lab.SetBurstParameter("Declmin",-1);
 lab.SetBurstParameter("Declmax",1);
 lab.SetBurstParameter("Tunits",0);
 lab.SetBurstParameter("Tmin",1);
 lab.SetBurstParameter("Tmax",0);
 lab.SetBurstParameter("Tbint90",0);
 lab.SetBurstParameter("Tbin",1);
 lab.SetBurstParameter("Datype",2);
 lab.SetBurstParameter("Dawin",5);
 lab.SetBurstParameter("Abin",0.1);
 lab.SetBurstParameter("Sigmamax",5);
 lab.SetBurstParameter("Angresmax",1);
 lab.SetBurstParameter("Emin",pow(10,3.3));

 ///////////////////////////
 // Enter the source data //
 ///////////////////////////
 NcSignal* sx=lab.SetSignal(1,24240.771,"hms",47.84,"dms","equ",0,-1,"J","NGC1068",0);
 sx->AddNamedSlot("z");
 sx->AddNamedSlot("csigma");
 sx->SetSignal(0.0038,"z");
 sx->SetSignal(1e-3,"csigma");

 // Obtain the effective area data
 lab.LoadAeffData("$(NCFS)/icepack/Aeff/IC86-PS-Aeff.root","hAeff");

 // Obtain observed event data

 lab.InitDataNames(1,"equ");
 lab.SetDataNames("Run","run");
 lab.SetDataNames("Event","event");
 lab.SetDataNames("Tobs","time","MJD");
 lab.SetDataNames("a","ra","rad");
 lab.SetDataNames("b","dec","rad");
 lab.SetDataNames("csigma","angErr","rad");
 lab.SetDataNames("E","logE","1","Log");

 lab.ListDataNames();

 Bool_t src=kFALSE;
 lab.LoadInputData(src,"IC86_2017_data.root","tree"); // Load from observed event data

 // Provide a listing of the first 5 stored entries for both the sources (c.q. burst) and the observed events
 cout << endl;
 lab.ListSignals("equ","J",3,"M",5);
 cout << endl;

 ////////////////////////////////
 // Match signals with sources //
 ////////////////////////////////
 NcDevice matches;
 lab.MatchBurstData(matches,"NGC1068");

 matches.Data();

 // Provide a listing of all relevant parameters
 cout << endl;
 lab.ListBurstParameters();

 //////////////////////////////////////////
 // Perform various statistical analyses //
 //////////////////////////////////////////
 Double_t rlow,rup;
 lab.GetBurstBayesianSignalRate(90,rlow,rup);

 lab.GetBurstLiMaSignificance();

 Double_t nr=1e4;
 Int_t ncut=10;
 Int_t ndt=2;
 lab.GetBurstBayesianPsiStatistics("angle",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("cosa",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("time",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("dt",nr,ncut,ndt);
 lab.GetBurstBayesianPsiStatistics("BBrat",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("time",nr,ncut,ndt,kTRUE);
 lab.GetBurstBayesianPsiStatistics("dt",nr,ncut,ndt,kTRUE);
 lab.GetBurstBayesianPsiStatistics("BBrat",nr,ncut,ndt,kTRUE);

 // Produce a ROOT output file with the produced (standard) histograms
 lab.WriteBurstHistograms("SourceAnalysis.root");
}
