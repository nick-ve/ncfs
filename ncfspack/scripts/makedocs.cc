////////////////////////////////////////////////////////////////////////////////
// ROOT macro to produce the documentation of all NCFS classes
//
//--- Author : Nick van Eijndhoven 03-dec-2008 NCFS
////////////////////////////////////////////////////////////////////////////////
{
 gSystem->Load("ncfs");

 gEnv->SetValue("Root.Html.Description","///////////");

 THtml q;
 q.SetOutputDir("./htmldoc");
 q.SetSourceDir("..");
 q.SetAuthorTag("//--- Author:");
 q.SetLastUpdateTag("//- Modified:");
 q.SetCopyrightTag("// Copyright");

 q.MakeAll();
}