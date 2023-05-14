///////////////////////////////////////////////////////////////////
// ROOT macro to analyse the data Tree generated by jet-gen.cc,  //
// which simulates an (obscured) astrophysical Jet.              //
// Both the p+p interactions of the jet with the dust and the    //
// p+gamma interactions within the jet have been simulated.      //
//                                                               //
// To execute the code, just invoke ($ = prompt)                 //
// $ root jet-ana.cc                                             //
//                                                               //
// Nick van Eijndhoven, IIHE-VUB, Brussels.                      //
// UTC March 28, 2023  07:19                                     //
///////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfspack");
 
 NcAstrolab lab;

 // Access to the input data
 TChain* data=new TChain("Data");
 data->Add("spectra*.root");

 // Setting up the basic histograms
 Int_t mode=1; // Flag to denote linear(=0)/log10(=1)/Ln(=2) scale for the X-axis
 Int_t nbins=100;
 Double_t xmin=-5;
 Double_t xmax=8;
 TString title="dN/dE=E^{-2} proton beam spectrum within 0.2-10 PeV on 10 keV gamma;^{10}Log(E_{#nu}/GeV);Number of neutrinos";

 // The p+p "beamdump" data
 TH1F hdump("hdump",title.Data(),nbins,xmin,xmax);
 data->Project("hdump","log10(nu.p)","evt.jrun>0");

 // The p+gamma "jet" data
 TH1F hjet("hjet","p+gamma data",nbins,xmin,xmax);
 data->Project("hjet","log10(nu.p)","evt.jrun<0");

 // The straight counts histograms
 TCanvas counts("counts","counts");
 hdump.Draw();
 hjet.Draw("same");
 counts.SetLogy(1);

 // The dN/dE histograms
 TH1F hdiffdump=lab.GetDifHistogram(&hdump,mode,"dN_{#nu}/dE");
 TH1F hdiffjet=lab.GetDifHistogram(&hjet,mode,"dN_{#nu}/dE");
 TCanvas cdiff("cdiff","cdiff");
 hdiffdump.Draw();
 hdiffjet.Draw("same");
 cdiff.SetLogy(1);

 // The E^2 dN/dE histograms
 TF1 f("f","pow(x,2.)");
 TH1F hdiffdump2=lab.GetDifHistogram(&hdump,mode,"E^{2}dN_{#nu}/dE",&f);
 TH1F hdiffjet2=lab.GetDifHistogram(&hjet,mode,"E^{2}dN_{#nu}/dE",&f);
 TCanvas cdiff2("cdiff2","cdiff2");
 hdiffdump2.Draw();
 hdiffjet2.Draw("same");
 cdiff2.SetLogy(1);

 // Straight counts histograms with log10 Y-axis
 TH1F hlogdump=lab.GetLogHistogram(&hdump,1);
 TH1F hlogjet=lab.GetLogHistogram(&hjet,1);
 TCanvas clog("clog","clog");
 hlogdump.Draw();
 hlogjet.Draw("same");

 // dN/dE histograms with log10 Y-axis
 TH1F hlogdiffdump=lab.GetLogHistogram(&hdiffdump,1);
 TH1F hlogdiffjet=lab.GetLogHistogram(&hdiffjet,1);
 TCanvas clogdiff("clogdiff","clogdiff");
 hlogdiffdump.Draw();
 hlogdiffjet.Draw("same");

 // E^2 dN/dE histograms with log10 Y-axis
 TH1F hlogdiffdump2=lab.GetLogHistogram(&hdiffdump2,1,"^{10}Log(E^{2}dN_{#nu}/dE) in GeV");
 TH1F hlogdiffjet2=lab.GetLogHistogram(&hdiffjet2,1,"^{10}Log(E^{2}dN_{#nu}/dE) in GeV");
 TCanvas clogdiff2("clogdiff2","clogdiff2");
 hlogdiffdump2.Draw();
 hlogdiffjet2.Draw("same");
}