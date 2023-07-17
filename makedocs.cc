////////////////////////////////////////////////////////////////////////
// ROOT macro to produce the documentation of all the classes that are
// contained in the NCFS framework.
//
// To run this macro, just issue the command ($ means command prompt) :
//
// $ root -b -q makedocs.cc >docs.log
//
// The (html) documentation will be located in the ncfs/docs
// directory and the produced log messages in the file docs.log.
//
// The (class) index in the html documentation can be accessed directly
// via the html link index.html in ncfs/docs.
//
// Note : All necessary libraries have to be available for the classes
//        for which the documentation is requested before running
//        this macro.
//
//--- Author: Nick van Eijndhoven 20-oct-2005 NCFS
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 17, 2023  23:37Z
////////////////////////////////////////////////////////////////////////
{
 // Includes needed in view of combined GUI headers in a single file
 #include "TGClient.h"
 #include "TGFrame.h"
 #include "TGButton.h"

 gSystem->Load("ncfspack");
 gSystem->Load("icepack");
 gSystem->Load("iceconvert");
 gSystem->Load("rnopack");

 gEnv->SetValue("Root.Html.Description","///////////");
 gEnv->SetValue("Root.Html.Author","//--- Author:");
 gEnv->SetValue("Root.Html.LastUpdate","//- Modified:");
 gEnv->SetValue("Root.Html.OutputDir","$(NCFS)/docs");

 THtml q;
 q.SetProductName("NCFS");
 q.SetInputDir("$(NCFS)/ncfspack/source:$(NCFS)/icepack/source:$(NCFS)/iceconvert/source:$(NCFS)/rnopack/source");

 q.MakeAll();
}
