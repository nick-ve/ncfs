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
 lab.SetBurstParameter("Grbnu",-0.03);
 lab.SetBurstParameter("Kinangle",3);

 // Parametrization of the energy spectrum of signal events from the burst
 Double_t emin=100;
 Double_t emax=1e7;
 Int_t nbins=10000;
 Double_t gamma=2;
 lab.MakeBurstEdist(gamma,emin,emax,nbins);

 // Use experimental distributions instead of parametrizations
/// lab.MakeBurstZdist("../grbweb/GRB-z-Swift.root","T","z",200,0,20);
/// lab.MakeBurstT90dist("../grbweb/GRB-t90-Fermi.root","T","t90");
/// lab.MakeBurstBkgEdist("IC86*data.root","tree","logE","dec","rad",200,1e7,1000);

 // Obtain burst locations, durations etc.
/// lab.LoadBurstGCNdata("../grbweb/GRBweb.root","T"); // Load from observed GCN data
  lab.GenBurstGCNdata(100,"GRB"); // Generate fictative burst GCN data

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

 lab.GetBurstBayesianPsiStatistics("time",2,1e4);
 lab.GetBurstBayesianPsiStatistics("angle",2,1e4);
 lab.GetBurstBayesianPsiStatistics("cosa",2,1e4);
 lab.GetBurstBayesianPsiStatistics("dt",2,1e4);

 // Produce a ROOT output file with the produced (standard) histograms
 lab.WriteBurstHistograms("burst-model.root");
}
