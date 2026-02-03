////////////////////////////////////////////////////////////////////////
// ROOT macro to convert effective area data into plain TTree format. //
//                                                                    //
// The effective area data are read from a file xxx-Aeff.txt,         //
// and the produced ROOT output file is called xxx-Aeff.root,         //
// where "xxx" stands for the sample name (IC86-PS, IC86-GFU,...),    //
// as specified below.                                                //
//                                                                    //
// The histograms contained in the ROOT output file are :             //
// (Aeff=effective area in cm^2 E=energy in GeV, theta=zenith angle)  //
// hAeff    : The 2D Aeff(log(E),cos(theta))                          //
// hAeffE   : The projection of Aeff on the log(E) axis               //
// hAeffZen : The projection of Aeff on the cos(theta) axis           //
//                                                                    //
// To execute the user macro, just invoke ($ = prompt)                //
// $ root -b -q IC-Aeff.cc                                            //
//                                                                    //
//- Nick van Eijndhoven October 14, 2025  12:35Z IIHE-VUB, Brussels.  //
////////////////////////////////////////////////////////////////////////
{
 // The name of the event sample (e.g. IC86-PS, IC86-GFU,...)
 TString sample="IC86-PS";

 // The name of the Aeff input txt file
 TString ifname;
 ifname.Form("%-s-Aeff.txt",sample.Data());

 // Construct the name of the produced ROOT output file
 TString ofname;
 ofname.Form("%-s-Aeff.root",sample.Data());

 // Construct the title of the 2D histogram
 TString title;
 title.Form("%-s Aeff(log(E),cos(#theta)) in cm^{2};log(E) in GeV;Cosine of the Zenith angle (#theta);Aeff in cm^{2}",sample.Data());

 cout << endl;
 cout << " Input  filename : " << ifname << endl;
 cout << " Output filename : " << ofname << endl;

 // Input variables
 Float_t Emin;   // 10log(Emin) in GeV
 Float_t Emax;   // 10log(Emax) in GeV
 Float_t DecMin; // Min. decl. in degrees
 Float_t DecMax; // Max. decl. in degrees
 Float_t ZenMin; // Min. zenith in degrees
 Float_t ZenMax; // Max. zenith in degrees
 Float_t Aeff;   // Aeff in cm^2

 // The input data file for the effective area profile
 FILE* ifile=fopen(ifname.Data(),"r");
 if (!ifile)
 {
  cout << " *** Data file for effective area data not found ***" << endl;
  return;
 }
 rewind(ifile); // Position at begin of file

 // The intermediate data tree
 TTree* treex=new TTree("TX","Aeff data");
 treex->Branch("Emin",&Emin,"log(Emin) in GeV");
 treex->Branch("Emax",&Emax,"log(Emax) in GeV");
 treex->Branch("DecMin",&DecMin,"Min. decl. in deg.");
 treex->Branch("DecMax",&DecMax,"Max. decl. in deg.");
 treex->Branch("ZenMin",&ZenMin,"Min. zenith in deg.");
 treex->Branch("ZenMax",&ZenMax,"Max. zenith in deg.");
 treex->Branch("Aeff",&Aeff,"Aeff in cm^2");

 // "Sink" title line(s) in the input txt file
 fscanf(ifile,"%*100c");

 // Read the data and fill the intermediate data Tree
 Double_t pi=acos(-1.);
 Int_t n=0;
 Float_t xmin,xmax,dx,ymin,ymax,dy;
 while (!feof(ifile))
 {
  fscanf(ifile,"%g %g %g %g %g \n",&Emin,&Emax,&DecMin,&DecMax,&Aeff);

  ZenMin=DecMin+90.;
  ZenMax=DecMax+90.;
  treex->Fill();
  n++;

  if (n==1)
  {
   xmin=Emin;
   xmax=Emax;
   dx=xmax-xmin;
   ymin=cos(ZenMax*pi/180.);
   ymax=cos(ZenMin*pi/180.);
   dy=ymax-ymin;
  }
  else
  {
   if (Emin<xmin) xmin=Emin;
   if (Emax>xmax) xmax=Emax;
   if (cos(ZenMax*pi/180.)<ymin) ymin=cos(ZenMax*pi/180.);
   if (cos(ZenMin*pi/180.)>ymax) ymax=cos(ZenMin*pi/180.);
  }
 }

 Int_t nx=0;
 Int_t ny=0;
 if (dx) nx=TMath::Nint((xmax-xmin)/dx);
 if (dy) ny=TMath::Nint((ymax-ymin)/dy);

 printf(" Number of data entries in the intermediate Tree : %-i \n",n);
 printf(" xmin=%-g xmax=%-g dx=%-g nx=%-i ymin=%-g ymax=%-g dy=%-g ny=%-i \n",xmin,xmax,dx,nx,ymin,ymax,dy,ny);

 // The produced output structure
 Float_t LogE;   // 10log(E) in Gev
 Float_t CosZen; // Cosine of the Zenith angle
 Float_t Zenith; // Zenith angle in degrees
 TFile* ofile=new TFile(ofname.Data(),"RECREATE","Effective area data");
 TTree* otree=new TTree("T","Aeff data");
 otree->Branch("LogE",&LogE,"log(E) in GeV");
 otree->Branch("CosZen",&CosZen,"Cosine of the Zenith angle");
 otree->Branch("Zenith",&Zenith,"Zenith angle in deg.");
 otree->Branch("Aeff",&Aeff,"Aeff in cm^2");

 TH2F* hAeff=new TH2F("hAeff",title,nx,xmin,xmax,ny,ymin,ymax);

 n=treex->GetEntries();
 for (Int_t i=0; i<n; i++)
 {
  treex->GetEntry(i);

  LogE=(Emin+Emax)/2.;
  CosZen=(cos(ZenMin*pi/180.)+cos(ZenMax*pi/180.))/2.;
  Zenith=acos(CosZen)*180./pi;
  
  otree->Fill();
  hAeff->Fill(LogE,CosZen,Aeff);
 }

 TH1D* hAeffE=hAeff->ProjectionX();
 title.Form("%-s Aeff(log(E));log(E) in GeV;Aeff in cm^{2}",sample.Data());
 hAeffE->SetNameTitle("hAeffE",title);
 TH1D* hAeffZen=hAeff->ProjectionY();
 title.Form("%-s Aeff(cos(#theta));Cosine of the Zenith angle (#theta);Aeff in cm^{2}",sample.Data());
 hAeffZen->SetNameTitle("hAeffZen",title);

 // Write the produced structure to the output file
 ofile->Write();
 ofile->Close();
}
