{
 ///////////////////////////////////////////////////////////////////////////
 // General ROOT logoff facility.
 // More specific logoff functionality can be obtained by providing a
 // customized file "rootlogoff.cc" in the working directory.
 //
 // --- NvE 09-oct-1997 UU-SAP Utrecht
 ///////////////////////////////////////////////////////////////////////////

 cout << endl;
 cout << endl;
 cout << " --- ROOT session ended --- " << endl;
 cout << endl;
 cout << " *** Started at " << session_clock.AsString() << " ***" << endl;

 session_clock.Set();

 cout << endl;
 cout << " *** Stopped at " << session_clock.AsString() << " ***" << endl;
 cout << endl;
}
