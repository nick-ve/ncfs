# ncfs
The NCFSPack framework with its extensions.

Documentation of all the contained software tools is available at
http://www.iihe.ac.be/ice3/ncfsdoc

In this repository the software development is performed on the
"main" branch (called "trunk" in SVN jargon), whereas various specific
releases (and the initial "import" from the original SVN repository)
are stored in different named branches, which are protected against
modifications and/or deletion.
Note however, that all branches (including "main") always contain a
fully functional package which is 100% backward compatible with previous
releases.  

//*CMZ :  1.00/01 04/07/97  16.43.13  by  Nick van Eijndhoven (UU/CERN)
//*-- Author :    Nick van Eijndhoven (UU/CERN)   04/07/97
 
This project was started to provide an Object Oriented framework,
consisting of C++ classes, in which event reconstruction of the ALICE
detector data at the CERN LHC could be performed.
In switching to Object Oriented programming, I myself have started to
perform the WA93 and WA98 data analysis within the ROOT [1] framework
and the corresponding ALICE software package was called RALICE.
Having seen the great advantages of this system I have started to make my
C++ classes more general in order to use them as an onset for a generic
reconstruction and (astro)physics analysis toolbox fully integrated within
the ROOT framework.
The basic generic classes providing various tools for detector signal handling,
track storage, event building, physics analysis, statistics and even the simulation
of particle collisions are contained in the "ncfspack" directory.
In addition to that, various detector specific packages can be added to extend
the functionality tailored to different experiments, like the "icepack" directory
for the analysis of IceCube data.  
The complete package can be compiled on all platforms which support ROOT,
using for instance the GNU G++ compiler.
Being embedded within the ROOT framework provides sophisticated analysis
capabilities (e.g. histogramming, fitting, graphics etc...).
In addition, the high level of interactivity of the ROOT/CINT system allows
program development without the time consuming compile/link/load/execute cycle,
whereas also the ROOT tree output format provides a completely machine
independent data format providing efficient and easy to use data access
capable to cope with the most complex data analyses programs.
 
Only the C++ ANSI standard is used in the source code and as such the package
is fully compatible with all standard C++ compilers as well as with
the ROOT/CINT interpreting system.
 
The comments in the source code are placed in the locations proposed
in the ROOT manual pages [1] such that the automatic source code
documentation system of ROOT can be used directly from the source code.
This has turned out to be very convenient (time saving) and guarantees
always updated documentation compatible with the current source code.
 
Coding conventions :
--------------------
In order not to clash with the (class) names of the ROOT framework
and (future) packages of other experiments, a few rules concerning names
of classes, (member)functions and variables have to be obeyed.
The rules are the following :
 
 1) Only ANSI standard C++ is allowed, with an even stricter
    requirement that the code should compile without any warnings
    under the GNU g++, msvc++ and the native C++ compilers of HP,
    Mac, AMD and DECAlpha machines.
    This will assure the programs to run on all standard research platforms.
 2) The generic "ncfspack"  class names start with "Nc" followed by an
    uppercase character, like for example : NcEvent.
    In this way the "ncfspack" class names will NEVER clash with the ones
    of ROOT whereas the probability of a clash with the class names of
    other group's code (e.g. IceCube) is minimised by using for instance
    "Ice" as the start of class names related to IceCube specific software.
    To prevent name clashes within the various (future) detector packages,
    please refer to the general note at the end.
 3) Class headers should be under the control of "#ifndef" and the name
    should consist of "classname_h".
    Example : #ifndef NcTrack_h
              #define NcTrack_h
    In this way also the ifdefs will be unique and prevents the danger
    of having the name of an ifdef being the same as a Classname.
 4) The private area in the class header has to be defined as the last item.
    Macros, like the ROOT ClassDef() statement (if needed) must be put
    to appear at the right location, i.e. just before the "};" of the
    class definition.
 5) Names of member functions should start with a capital character
    and should NOT contain underscores (which drop out with HTML).
    From the name it should be clear what the functionality is and
    capital characters should be used to indicate various "words".
    Example : NcTrack::Set3Momentum(...)
 6) The declaration of variables should adopt the ROOT type definitions
    like for instance Int_t, Float_t, Double_t etc.
    This will assure the most compact data format and correct type conversion
    across various platforms.
 7) Names of datamembers of a class should start with a lowercase "f"
    and the next character has to be uppercase.
    Example : Float_t fEnergy
    This will allow directly identification of datamembers in the code.
    The names of all other local variables may be chosen freely by the
    author.
    Note : It is recommended to use only lowercase characters
           for local variables.
 8) Names of global variables should start with "g" and the next
    characters have to be the detector specific character string
    used as the start for the detector specific class names.
    Example : gIcePandel
    This will allow directly identification of global variables in the
    code and will not clash with the existing ROOT globals like
    for instance gDirectory etc...
    Note : Usage of global variables should be avoided as much as
           possible. Most of the data transfer should go via the classes
           and their member functions (data hiding).
 9) Comments should be placed at the positions as outlined in the ROOT docs.
    This will enable the automatic HTML machinery of ROOT.
10) Each class should contain a short description of the class functionality,
    including some examples, at the appropriate location for the ROOT
    automatic documentation system.
 
General note :
--------------
Within a certain software pool it may happen that e.g. in simulation
applications one wants to define for instance a Track class which
contains as data members some additional information (e.g. which was
the corresponding parent particle) compared to some generic MyTrack class.
Since objects reconstructed from real data will always contain the
minimal amount of information compared to e.g. objects from simulation,
it is in the above case then necessary to introduce a new class
MySTrack (simulation track).
Obviously such a newly defined object (MySTrack) can be derived from
the reconstruction object (MyTrack) and just have some data members
and/or memberfunctions added to it.
In such a way maximum flexibility is provided within every (future)
detector project, whereas all produced data can always be analysed using
the generic detector tools.
 
Installation :
--------------
The various shared libraries may be automatically installed using the provided shell
scripts in the "scripts" directory of the various packages.
It is essential that one first installs ROOT (including the TPythia6 package
in view of the NcCollider physics event generator) and run the provided installation
scripts within the proper "NCFS environment".
The proper "NCFS environment" may be obtained automatically by invoking the following
scripts which are provided in this "ncfs" top directory :

ncfs.bat : To initialise the proper "NCFS environment" for Windows.
ncfs.sh  : To initialise the proper "NCFS environment" for bash shells.

Note that after invokation of one of the above "environment setting scripts", the
command prompt should be "ncfs>" to indicate that the environment has been set correctly.

Once ROOT is installed and the "NCFS environment" is set correctly, the first shared library
to be created is "ncfspack", after which all the detector specific shared libraries can be created.
 
Invoking the various tools :
----------------------------
The functionality of a certain package is obtained by loading the needed shared
libraries into the ROOT system. In doing so, one should always first load the
generic "ncfspack" library, followed by the desired detector specific package(s).

Example : To create the IceCube environment one should load the following libraries

Root> gSystem->Load("ncfspack");
Root> gSystem->Load("icepack");
 
[1] http://root.cern.ch
 
 
 
                                           Nick van Eijndhoven
                                           Inter-university Institute for High Energies (IIHE)
                                           Vrije Universiteit Brussel (VUB)
                                           Pleinlaan 2
                                           B-1050 Brussel
                                           Belgium
                                           Email: nick@icecube.wisc.edu
                                           WWW: http://www.iihe.ac.be

