///////////////////////////////////////////////////////////////////////////
// All classes of RnoPack analysis software.
// This class list is used to create the RnoPack dictionary via rootcint.
//
// Note : Headers have also to be entered into the list
//        contained in RNOHeaders.h
//
//--- Author: Nick van Eijndhoven, IIHE-VUB, Brussel, July 9, 2021  16:58Z
//- Modified: Nick van Eijndhoven, IIHE-VUB, Brussel, July 9, 2021  16:58Z
///////////////////////////////////////////////////////////////////////////
 
#ifdef __CINT__
 #pragma link off all globals;
 #pragma link off all classes;
 #pragma link off all functions;

 #pragma link C++ class RnoGANT+;
 #pragma link C++ class RnoDANT+;
 #pragma link C++ class RnoSANT+;
 #pragma link C++ class RnoVPOL+;
 #pragma link C++ class RnoHPOL+;
 #pragma link C++ class RnoLPDA+;
 #pragma link C++ class RnoULPDA+;
 #pragma link C++ class RnoDLPDA+;
 #pragma link C++ class RnoString+;
 #pragma link C++ class RnoStation+;
 #pragma link C++ class RnoDetector+;
 #pragma link C++ class RnoEvent+;
 #pragma link C++ class RnoConvert+;
#endif
 
