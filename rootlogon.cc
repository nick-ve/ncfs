{
 ///////////////////////////////////////////////////////////////////////////
 // General ROOT logon facility.
 // More specific logon functionality can be obtained by providing a
 // customized file "rootlogon.cc" in the working directory.
 //
 // --- NvE 09-oct-1997 UU-SAP Utrecht
 // --- Modified Nick van Eijndhoven, IIHE-VUB, Brussel, July 17, 2023  23:55Z
 ///////////////////////////////////////////////////////////////////////////

 #include <iomanip>

 #include "TGClient.h"
 #include "TGFrame.h"
 #include "TGButton.h"

 // Redefine the interactive ROOT prompt 
 ((TRint*)gROOT->GetApplication())->SetPrompt("Root> ");

// Default white background for all plots
 gStyle->SetCanvasColor(10);
 gStyle->SetStatColor(10);
 gStyle->SetTitleColor(10);
 gStyle->SetPadColor(10);
 gROOT->SetStyle("Plain");
gStyle->SetPalette(1);

// No canvas or pad borders in produced .eps files
 gStyle->SetCanvasBorderMode(0);
 gStyle->SetPadBorderMode(0);

// Settings for statistics information
 gStyle->SetOptFit(1111);
 gStyle->SetOptStat(1111);

// Positioning of axes labels
 gStyle->SetTitleOffset(1.2,"X");
 gStyle->SetTitleOffset(1.2,"Y");

 gROOT->ForceStyle();

 gSystem->Load("libPythia6");
 gSystem->Load("libEG");
 gSystem->Load("libEGPythia6");

 gSystem->Load("libGraf3d");
 gSystem->Load("libMathMore");
 gSystem->Load("libSpectrum");
 gSystem->Load("libPhysics");
 gSystem->Load("libTree");
 gSystem->Load("libMinuit");
 gSystem->Load("libFITSIO");

 TDatime session_clock;

 cout << endl;
 cout << endl;
 cout << " --- Welcome to the ROOT general session --- " << endl;
 cout << endl;
 cout << " === All needed standard ROOT libraries loaded by default on this Unix system === " << endl;
 cout << endl;
 cout << " *** Start at " << session_clock.AsString() << " ***" << endl;
 cout << endl;
}
