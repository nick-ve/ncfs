#ifndef NcCalorimeter_h
#define NcCalorimeter_h
// Copyright(c) 1997-2019, NCFS/IIHE, All Rights Reserved.
// See cxx source for full Copyright notice.

// $Id: NcCalorimeter.h 5 2010-03-19 10:10:02Z nickve $

#include <math.h>
 
#include "TNamed.h"
#include "TObjArray.h"
#include "TH2.h"
#include "TString.h"

#include "NcDevice.h"
#include "NcObjMatrix.h"
#include "NcCalmodule.h"
#include "NcCalcluster.h"
#include "NcPositionObj.h"
#include "NcAttribObj.h"
 
class NcCalorimeter : public NcDevice
{
 public:
  NcCalorimeter();                                       // Default constructor
  NcCalorimeter(Int_t nrow,Int_t ncol);                  // Create a calorimeter matrix
  virtual ~NcCalorimeter();                              // Destructor
  NcCalorimeter(const NcCalorimeter& c);                 // Copy constructor
  virtual TObject* Clone(const char* name="") const;     // Make a deep copy and provide pointer of the copy
  Int_t GetNrows();                                      // Return number of rows of the matrix
  Int_t GetNcolumns();                                   // Return number of columns of the matrix
  using NcDevice::SetSignal;
  void SetSignal(Int_t row,Int_t col,Float_t s);         // Set signal for a certain module
  using NcDevice::AddSignal;
  void AddSignal(Int_t row,Int_t col,Float_t s);         // Add signal to a certain module
  void AddSignal(NcCalmodule* m);                        // Add module signal to current calorimeter
  void Reset(Int_t row,Int_t col);                       // Reset signal for a certain module
  virtual void Reset(Int_t mode=0);                      // Reset the complete calorimeter
  using NcDevice::GetSignal;
  virtual Float_t GetSignal(Int_t row,Int_t col=0) { return GetSignal(row,col,0); }
  Float_t GetSignal(Int_t row,Int_t col,Int_t mode);     // Provide signal of a certain module
  Int_t GetNsignals() const;                             // Return number of modules with a signal
  void Group(Int_t n=1,Int_t mode=1);                    // Group modules into clusters (n rings)
  Int_t GetNclusters() const;                            // Return number of clusters
  Float_t GetClusteredSignal(Int_t row,Int_t col);       // Provide module signal after clustering
  NcCalcluster* GetCluster(Int_t j) const;               // Access to cluster number j
  NcCalmodule* GetModule(Int_t j) const;                 // Access to 'fired' module number j
  NcCalmodule* GetModule(Int_t row,Int_t col);           // Access to module at (row,col)
  using NcDevice::SetEdgeOn;
  void SetEdgeOn(Int_t row,Int_t col);                   // Indicate module as 'edge module'
  using NcDevice::SetEdgeOff;
  void SetEdgeOff(Int_t row,Int_t col);                  // Indicate module as 'non-edge module'
  using NcDevice::GetEdgeValue;
  Int_t GetEdgeValue(Int_t row,Int_t col);               // Provide the edge flag of a module
  using NcDevice::SetDead;
  void SetDead(Int_t row,Int_t col);                     // Indicate module as 'dead module'
  using NcDevice::SetAlive;
  void SetAlive(Int_t row,Int_t col);                    // Indicate module as 'active module'
  using NcDevice::GetDeadValue;
  Int_t GetDeadValue(Int_t row,Int_t col);               // Provide the dead flag of a module
  using NcDevice::SetGain;
  void SetGain(Int_t row,Int_t col,Float_t g);           // Set the gain value for a module
  using NcDevice::SetOffset;
  void SetOffset(Int_t row,Int_t col,Float_t o);         // Set the offset value for a module
  using NcDevice::GetGain;
  Float_t GetGain(Int_t row,Int_t col);                  // Provide the gain value of a module
  using NcDevice::GetGainFlag;
  Int_t GetGainFlag(Int_t row,Int_t col);                // Provide the gain flag value of a module
  using NcDevice::GetOffset;
  Float_t GetOffset(Int_t row,Int_t col);                // Provide the offset value of a module
  using NcDevice::GetOffsetFlag;
  Int_t GetOffsetFlag(Int_t row,Int_t col);              // Provide the offset flag value of a module
  using NcDevice::SetPosition;
  void SetPosition(Int_t row,Int_t col,Float_t* r,TString f); // Set module position
  void SetPosition(Int_t row,Int_t col,Nc3Vector& r);    // Set module position
  using NcDevice::GetPosition;
  void GetPosition(Int_t row,Int_t col,Float_t* r,TString f); // Return module position
  NcPosition* GetPosition(Int_t row,Int_t col);          // Access to module position
  TH2F* DrawModules(Float_t thresh=0.,Int_t mode=0);     // Lego plot of module (corr.) signals above threshold
  TH2F* DrawClusters(Float_t thresh=0.);                 // Lego plot of cluster signals above threshold
  void AddVetoSignal(NcSignal& s);                       // Associate (extrapolated) signal
  void AddVetoSignal(NcSignal* s) { AddVetoSignal(*s); }
  NcSignal* GetVetoSignal(Int_t j) const;                // Access to veto signal number j
  Int_t GetNvetos() const;                               // Provide the number of veto signals
  void SetMatrixSwapMode(Int_t swap=1);                  // Set the swapmode for the storage of the matrices
  Int_t GetMatrixSwapMode() const;                       // Provide the swapmode for the storage of the matrices
 
 protected:
  Int_t fNrows;                              // The number of rows
  Int_t fNcolumns;                           // The number of columns
  NcObjMatrix* fMatrix;                      //! Matrix lookup table of module pointers
  Int_t fSwap;                               // The swapmode for the module and position matrices
  void SortM();                              // Order the modules with decreasing signal (matrix search)
  void SortA();                              // Order the modules with decreasing signal (fired array search)
  TObjArray* fClusters;                      // The array of clusters
  void AddRing(Int_t row,Int_t col,Int_t n); // add signals of n rings around cluster center
  void Ungroup();                            // Restore module matrix as before clustering
  TH2F* fHmodules;                           //! The module 2-D histogram for event display
  TH2F* fHclusters;                          //! The cluster 2-D histogram for event display
  TObjArray* fVetos;                         // The array of associated (extrapolated) veto signals
  TObjArray* fAttributes;                    //! Matrix dbase with module attributes (e.g. gain, offset etc...)
  NcObjMatrix* fPositions;                   //! Matrix dbase of module position pointers
  void LoadMatrix();                         // Loading of matrix lookup table from the linear hit array
 
 ClassDef(NcCalorimeter,1) // Description of a modular calorimeter system.
};
#endif
