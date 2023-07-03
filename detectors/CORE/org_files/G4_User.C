#ifndef MACRO_G4USER_C
#define MACRO_G4USER_C

#include <fun4all/Fun4AllServer.h>

//#include <fun4all/Fun4AllInputManager.h>
//#include <fun4all/Fun4AllDstInputManager.h>

R__LOAD_LIBRARY(libfun4all.so)

#include "/work/eic/users/tapasig/simulations/fun4all/klm/detectors/CORE/analysismacros/magnetMaterialAnalysis.h"
R__LOAD_LIBRARY(/home/tapasig/install/lib/libdifftaggana.so)

//---
#include <g4detectors/PHG4CylinderSubsystem.h>
#include <g4main/PHG4Reco.h>
R__LOAD_LIBRARY(libg4detectors.so)

class PHG4Reco;

namespace Enable
{
// if you want this to run by default, initialize this to true
// Otherwise you have to use Enable::USER = true; in your macro
  bool USER = false;
  int USER_VERBOSITY = 0;
}

namespace G4USER
{
// here you can set parameters in your macro via
// G4USER::myparam = 1;
// add as many as you need
  int myparam = 0;
}

// This initializes the G4 part if you have a detector implemented
// You need to tell its dimensions to the surrounding black hole
void UserInit()
{
  // set the black hole dimensions surrounding the detector
  // XXX: maximum radius of your detector
  // YYY: maximum extension in z
  // ZZZ: maximum extension in -z (use -ZZZ)
  BlackHoleGeometry::max_radius = std::max(BlackHoleGeometry::max_radius, 90.0);
  BlackHoleGeometry::max_z = std::max(BlackHoleGeometry::max_z, 125.0);
  BlackHoleGeometry::min_z = std::min(BlackHoleGeometry::min_z, -125.0);



}

// If you have a detector - here goes the setup
void UserDetector(PHG4Reco *g4Reco)
{

  //_________________________________
  //First detector
  PHG4CylinderSubsystem *virDetSurface = new PHG4CylinderSubsystem("MAGNET_DET_INNER", 0);
  
  virDetSurface->set_double_param("radius", 99);
  virDetSurface->set_double_param("length", 250);
  virDetSurface->set_double_param("thickness", 0.001);
  virDetSurface->set_int_param("lengthviarapidity", 0);

  // relative position to mother volume...
  virDetSurface->set_double_param("place_x", 0);
  virDetSurface->set_double_param("place_y", 0);
  virDetSurface->set_double_param("place_z", 0);

  virDetSurface->set_double_param("rot_y", 0.);
  // virDetSurface->set_double_param("field_global_rot_y", 0.); // abs. rotation to world for field manager
  
  virDetSurface->set_string_param("material", "G4_Galactic");
  // virDetSurface->set_string_param("material", "coil_core");
  virDetSurface->set_color(0., 1., 0., 1.);
  
  // virDetSurface->BlackHole();
  virDetSurface->SuperDetector("MAGNET_DET_INNER");
  virDetSurface->SetActive();
  virDetSurface->OverlapCheck(true);
  //virDetSurface->Verbosity(verbosity);
    
  g4Reco->registerSubsystem(virDetSurface);
 
  //_________________________________
  //Second detector
  
  PHG4CylinderSubsystem *virDetSurface1 = new PHG4CylinderSubsystem("MAGNET_DET_OUTER", 1);
  
  virDetSurface1->set_double_param("radius", 141);
  virDetSurface1->set_double_param("length", 250);
  virDetSurface1->set_double_param("thickness", 0.001);
  virDetSurface1->set_int_param("lengthviarapidity", 0);

  // relative position to mother volume...
  virDetSurface1->set_double_param("place_x", 0);
  virDetSurface1->set_double_param("place_y", 0);
  virDetSurface1->set_double_param("place_z", 0);

  virDetSurface1->set_double_param("rot_y", 0.);
  // virDetSurface->set_double_param("field_global_rot_y", 0.); // abs. rotation to world for field manager
  
  virDetSurface1->set_string_param("material", "G4_Galactic");
  virDetSurface1->set_color(0., 1., 0., 1.);
  
  virDetSurface1->BlackHole();
  virDetSurface1->SuperDetector("MAGNET_DET_OUTER");
  virDetSurface1->SetActive();
  virDetSurface1->OverlapCheck(true);
  //virDetSurface1->Verbosity(verbosity);
    
  g4Reco->registerSubsystem(virDetSurface1);

  return;
}

// Any analysis goes here (registering your module with Fun4All)
void UserAnalysisInit()
{
  Fun4AllServer* se = Fun4AllServer::instance();
  // diff_tagg_ana *myAnalysisModule = new diff_tagg_ana("diff_tagg_ana","out.root");
  magnetMaterialAnalysis *myAnalysisModule = new magnetMaterialAnalysis("diff_tagg_ana","out.root");
  se->registerSubsystem(myAnalysisModule);
  
  // AnalysisModule *myAnalysisModule = new AnalysisModule();

//  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
//  in->fileopen(fname);
//  se->registerInputManager(in);
//  se->run();
//  se->End();

//  return;
}

#endif
