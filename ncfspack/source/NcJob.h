#ifndef NcJob_h
#define NcJob_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcJob.h 74 2013-01-21 14:02:42Z nickve $

#include <cstdlib>

#include "TROOT.h"
#include "TTask.h"
#include "TFolder.h"
#include "TObjArray.h"

class NcJob : public TTask
{
 public :
  NcJob(const char* name="NcJob",const char* title="");   // Constructor
  virtual ~NcJob();                                       // Destructor
  void ListEnvironment();                                 // Provide listing of the job environment
  void ExecuteJob(Int_t mode=0);                          // Invokation of the top level processing
  void MakeFolder();                                      // Select creation of the folder structure 
  TFolder* GetFolder() const;                             // Provide pointer to the whiteboard folder 
  TObject* GetMainObject() const;                         // Provide pointer to the main object structure
  void AddObject(TObject* obj);                           // Add an object into the environment
  void AddObjects(TObjArray* arr);                        // Add all array objects into the environment
  void RemoveObject(TObject* obj);                        // Remove an object from the environment
  void RemoveObjects(const char* classname);              // Remove all objects inheriting from "classname"
  TObject* GetObject(const char* classname) const;        // Provide first stored object inheriting from "classname" 
  TObject* GetObject(Int_t j) const;                      // Provide j-th stored object
  TObjArray* GetObjects() const;                          // Provide all stored object pointers
  TObjArray* GetObjects(const char* classname);           // Provide all objects inheriting from "classname" 
  void ProcessObject(TObject* obj);                       // Process all sub-tasks for the specified object

 protected :
  Int_t fMakefolder;    // Flag to indicate creation of the folder structure
  TFolder* fFolder;     // Pointer to the folder which serves as the job's whiteboard
  TObject* fMainObject; // Pointer to the main processing object structure within the job
  TObjArray* fObjects;  // Pointers to the various user-added objects 
  TObjArray* fSelect;   //! Temp. array of pointers to user-selected stored objects 

  void SetMainObject(TObject* obj); // Store pointer to the main object structure

 ClassDef(NcJob,1) // Base class for top level job in a task based procedure 
};
#endif
