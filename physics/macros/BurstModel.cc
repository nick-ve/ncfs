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
// $ root -b -q BurstModel.cc                                    //
//                                                               //
// Several standard histograms, depending on the user selections,//
// are automatically generated and may be written into a ROOT    //
// output file for later inspection, as indicated below.         //
//                                                               //
//--- Nick van Eijndhoven, IIHE-VUB, Brussel, February, 2026     //
///////////////////////////////////////////////////////////////////
void BurstModel()
{
 gSystem->Load("ncfspack");

 NcVersion version;
 version.Data();

 // The virtual lab for a cosmic transient phenomena study
 NcAstrolab lab;

 lab.SetExperiment("IceCube");
 lab.SetUT("11-04-2020","12:00:00.0",0); // Fixed fictative analysis date
 lab.SetRandomiser(-1); // Use the UT timestamp to generate a seed
 lab.Data();

 Float_t pi=acos(-1.);

 /////////////////////////////////////
 // User defined parameter settings //
 /////////////////////////////////////
 Float_t bkgrate=-0.003/(2.*pi);
 lab.SetBurstParameter("Bkgrate",bkgrate);
 lab.SetBurstParameter("Nbkg",1);
 lab.SetBurstParameter("Grbnu",0.02);
 lab.SetBurstParameter("Sigmamax",0.1);
 lab.SetBurstParameter("Avgrbz",-1);
 lab.SetBurstParameter("Zmin",1e-3);
 lab.SetBurstParameter("Zmax",10);
 lab.SetBurstParameter("Ezcor",1);
 lab.SetBurstParameter("Avgrbt90",-1);
 lab.SetBurstParameter("Declmin",0);
 lab.SetBurstParameter("Declmax",90);
 lab.SetBurstParameter("Dtnu",30);
 lab.SetBurstParameter("Dtnus",30);
 lab.SetBurstParameter("Alphabkg",3.5);
 lab.SetBurstParameter("Alphasig",2);
 lab.SetBurstParameter("Emin",1e2);
 lab.SetBurstParameter("Emax",1e7);
 lab.SetBurstParameter("ESigmin",1e2);
 lab.SetBurstParameter("ESigmax",1e9);
 lab.SetBurstParameter("Angresfix",1);
 lab.SetBurstParameter("Recoangle",0);
 lab.SetBurstParameter("Kinangle",0);
 lab.SetBurstParameter("Datype",0);
 lab.SetBurstParameter("Dawin",5);
 lab.SetBurstParameter("Abin",0.2);
 lab.SetBurstParameter("Tunits",2);
 lab.SetBurstParameter("Tmin",-3600);
 lab.SetBurstParameter("Tmax",3600);
 lab.SetBurstParameter("Tbint90",0);
 lab.SetBurstParameter("Tbin",150);

 ////////////////////////////////////////////////////////////////
 // Use experimental distributions instead of parametrizations //
 ////////////////////////////////////////////////////////////////
/// lab.MakeBurstZdist("../grbweb/GRB-z-Swift.root","T","z",200,0,20);
/// lab.MakeBurstT90dist("../grbweb/GRB-t90-Fermi.root","T","t90");
/// lab.MakeBurstSigmaPosdist("../grbweb/GRBweb.root","T","sigmapos","deg");
/// lab.MakeBurstEnergydist(-1,"IC86*data.root","tree","logE","dec","rad",1000);
/// lab.MakeBurstRecoAngresdist("IC86*data.root","tree","logE","angErr","rad","dec","rad",200,1e7);

 // Obtain the effective area data
 lab.LoadAeffData("$(NCFS)/icepack/Aeff/IC86-GFU-Aeff.root","hAeff");

 // The number of (fictative) bursts when generated
 Int_t ngenburst=500;

 // Detailed specification of signal strength (if wanted)
 Float_t norm;
 TString formula;
 norm=2.221e-5;
 formula.Form("%-.2g*pow(x,-2.11)",norm);
 TF1 sigspec("sigspec",formula);
//@@@ lab.MakeBurstEnergydist("SigF",sigspec,ngenburst);

 // Obtain burst locations, durations etc.

/*********************
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
*******************/

 Bool_t scale=kFALSE;
 lab.GenBurstGCNdata(ngenburst,"GRB",scale); // Generate fictative burst GCN data

 // Provide a listing of the first 10 stored burst positions
 cout << endl;
 lab.ListSignals("equ","J",1,"T",10);
 cout << endl;

 // Generate burst signals according to the user defined parameters
 lab.GenBurstSignals();

 //////////////////////////////////////////
 // Perform various statistical analyses //
 //////////////////////////////////////////

 lab.GetBurstLiMaSignificance();

 Double_t rlow,rup;
 lab.GetBurstBayesianSignalRate(90,rlow,rup);

/***********************
 Double_t nr=1e7;
 Int_t ncut=10;
 Int_t ndt=2;
 lab.GetBurstBayesianPsiStatistics("angle",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("cosa",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("time",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("dt",nr,ncut,ndt);
/// lab.GetBurstBayesianPsiStatistics("BBrat",nr,ncut);

 lab.GetBurstBayesianPsiStatistics("time",nr,ncut,ndt,kTRUE);
 lab.GetBurstBayesianPsiStatistics("dt",nr,ncut,ndt,kTRUE);
/// lab.GetBurstBayesianPsiStatistics("BBrat",nr,ncut,ndt,kTRUE);
****************/

 // Provide a listing of all relevant parameters
 printf("\n");
 lab.ListBurstParameters();

 // Produce a ROOT output file with the produced (standard) histograms
 lab.WriteBurstHistograms("BurstModel.root");
}
