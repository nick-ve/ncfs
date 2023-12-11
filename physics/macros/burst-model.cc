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
// $ root -b -q burst-model.cc                                   //
//                                                               //
// Several standard histograms, depending on the user selections,//
// are automatically generated and may be written into a ROOT    //
// output file for later inspection, as indicated below.         //
//                                                               //
//--- Nick van Eijndhoven, IIHE-VUB, Brussel, March 2, 2020      //
///////////////////////////////////////////////////////////////////
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

 // User defined parameter settings
 Float_t bkgrate=-0.003/(2.*pi);
 lab.SetBurstParameter("Bkgrate",bkgrate);
 lab.SetBurstParameter("Declmin",5);
 lab.SetBurstParameter("Declmax",85);
 lab.SetBurstParameter("Dtnu",60);
 lab.SetBurstParameter("Datype",1);
 lab.SetBurstParameter("Dawin",2);

 // Use experimental distributions instead of parametrizations
/// lab.MakeBurstZdist("../grbweb/GRB-z-Swift.root","T","z",200,0,20);
/// lab.MakeBurstT90dist("../grbweb/GRB-t90-Fermi.root","T","t90");
/// lab.MakeBurstSigmaPosdist("../grbweb/GRBweb.root","T","sigmapos","deg");
/// lab.MakeBurstEnergydist(-1,"IC86*data.root","tree","logE","dec","rad",200,1e7,1000);
/// lab.MakeBurstRecoAngresdist("IC86*data.root","tree","logE","angErr","rad","dec","rad",200,1e7);

 // Obtain burst locations, durations etc.

/*************
 // Use this code to load observed GCN data
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
 lab.LoadInputData(src,"../grbweb/GRBweb.root","T");
**************/

 // Use this statement to generate fictative burst GCN data
 lab.GenBurstGCNdata(500,"GRB");

 // Provide a listing of the first 10 stored burst positions
 cout << endl;
 lab.ListSignals("equ","J",1,"T",10);
 cout << endl;

 // Generate burst signals according to the user defined parameters
 lab.GenBurstSignals();

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
 Int_t mode=1;
 Int_t fact=1;
 lab.GetBurstBayesianPsiStatistics("time",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("angle",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("cosa",nr,ncut);
 lab.GetBurstBayesianPsiStatistics("dt",nr,ncut,ndt,mode,fact);

 // Produce a ROOT output file with the produced (standard) histograms
 lab.WriteBurstHistograms("burst-model.root");
}
