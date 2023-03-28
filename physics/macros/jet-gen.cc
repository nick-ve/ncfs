///////////////////////////////////////////////////////////////////
// ROOT macro for simulation of an (obscured) astrophysical Jet. //
// Both the p+p interactions of the jet with the dust            //
// and the p+gamma interactions within the jet are simulated.    //
//                                                               //
// To execute the code, just invoke ($ = prompt)                 //
// $ root -b -q jet-gen.cc                                       //
//                                                               //
// Nick van Eijndhoven, IIHE-VUB, Brussels.                      //
// UTC March 28, 2023  07:19                                     //
///////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");

 NcCollider* gen=new NcCollider();
 gen->SetTitle("Astrophysical (obscured) AGN Jet modeling");

 gen->SetOutputFile("jet-output.root",1);

 Float_t np=10000;                // Number of Jet (beam) protons to be simulated
 Float_t gfrac=0.2;               // Fraction of Jet (beam) protons used for p+gamma interactions
 TString flux="nu,neutron,gamma"; // The final particle species that will be recorded
 Float_t dthmax=-1;               // Variation in target momentum theta direction (in degrees) 
 Int_t nlist=0;                   // Produce an event listing of the first "nlist" events of each sample

 Int_t mult=1;      // Flag to indicate activation of multiple partonic interactions to mimic soft processes
 Int_t elastic=0;   // Flag to indicate activation of Low-Pt, elastic and diffractive interactions

 // Specification of the proton (beam) spectrum
 Double_t pmin=1e5;
 Double_t pmax=1e7;
 TF1 fp("fp","pow(x,-2)");
 gen->SetJetProtonSpectrum(pmin,pmax,&fp);

 // Specification of the gamma (target) spectrum
 pmin=1e-4;
 pmax=1e-3;
 TF1 fg("fg","pow(x,-3)");
 gen->SetJetGammaSpectrum(pmin,pmax,&fg);

 gen->SetPrintFreq(100);
 gen->SetMultiple(mult);
 gen->SetElastic(elastic);
 gen->ProcessJet(np,gfrac,flux,dthmax,nlist);
}
