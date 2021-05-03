#ifndef MACRO_G4PLUGDOOREIC_C
#define MACRO_G4PLUGDOOREIC_C

#include <GlobalVariables.C>

#include <g4detectors/PHG4CylinderSubsystem.h>

#include <g4main/PHG4Reco.h>

R__LOAD_LIBRARY(libg4detectors.so)

namespace Enable
{
  bool PLUGDOOR = false;
  bool PLUGDOOR_ABSORBER = false;
  bool PLUGDOOR_OVERLAPCHECK = false;
}  // namespace Enable

namespace G4PLUGDOOR
{
  // sPHENIX forward flux return(s)
  // define via four corners in the engineering drawing
  double z_1 = -300.;
  double z_2 = -200.;
  double z_3 =  330.;
  double z_4 =  440.;

  double r_1 = 20.;
  double r_2 = 250.;
  double r_3 = 160.;

  double length_1 = z_2 - z_1;
  double place_z1 = (z_1 + z_2) / 2.;

  double length_2 = z_3 - z_2;
  double place_z2 = (z_2 + z_3) / 2.;

  double length_3 = z_4 - z_3;
  double place_z3 = (z_3 + z_4) / 2.;

}  // namespace G4PLUGDOOR

void PlugDoorInit()
{
  BlackHoleGeometry::max_radius = std::max(BlackHoleGeometry::max_radius, G4PLUGDOOR::r_2);
  BlackHoleGeometry::min_z = std::min(BlackHoleGeometry::min_z, G4PLUGDOOR::z_1);
  BlackHoleGeometry::max_z = std::min(BlackHoleGeometry::max_z, G4PLUGDOOR::z_4);
}

void PlugDoor(PHG4Reco *g4Reco)
{
  //----------------------------------------
  bool OverlapCheck = Enable::OVERLAPCHECK || Enable::PLUGDOOR_OVERLAPCHECK;
  const bool flux_door_active = Enable::ABSORBER || Enable::PLUGDOOR_ABSORBER;

  const string material("Steel_1006");

  PHG4CylinderSubsystem *flux_return_minus = new PHG4CylinderSubsystem("FLUXRET_ETA_MINUS", 0);
  flux_return_minus->set_double_param("length", G4PLUGDOOR::length_1);
  flux_return_minus->set_double_param("radius", G4PLUGDOOR::r_1);
  flux_return_minus->set_double_param("place_z", G4PLUGDOOR::place_z1);
  flux_return_minus->set_double_param("thickness", G4PLUGDOOR::r_2 - G4PLUGDOOR::r_1);
  flux_return_minus->set_string_param("material", material);
  flux_return_minus->SetActive(flux_door_active);
  flux_return_minus->SuperDetector("FLUXRET");
  flux_return_minus->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(flux_return_minus);

  PHG4CylinderSubsystem *flux_return_central = new PHG4CylinderSubsystem("FLUXRET_ETA_CENTRAL", 1);
  flux_return_central->set_double_param("length", G4PLUGDOOR::length_2);
  flux_return_central->set_double_param("radius", G4PLUGDOOR::r_3);
  flux_return_central->set_double_param("place_z", G4PLUGDOOR::place_z2);
  flux_return_central->set_double_param("thickness", G4PLUGDOOR::r_2 - G4PLUGDOOR::r_3);
  flux_return_central->set_string_param("material", material);
  flux_return_central->SetActive(flux_door_active);
  flux_return_central->SuperDetector("FLUXRET");
  flux_return_central->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(flux_return_central);

  PHG4CylinderSubsystem *flux_return_plus = new PHG4CylinderSubsystem("FLUXRET_ETA_PLUS", 2);
  flux_return_plus->set_double_param("length", G4PLUGDOOR::length_3);
  flux_return_plus->set_double_param("radius", G4PLUGDOOR::r_1);
  flux_return_plus->set_double_param("place_z", G4PLUGDOOR::place_z3);
  flux_return_plus->set_double_param("thickness", G4PLUGDOOR::r_2 - G4PLUGDOOR::r_1);
  flux_return_plus->set_string_param("material", material);
  flux_return_plus->SetActive(flux_door_active);
  flux_return_plus->SuperDetector("FLUXRET");
  flux_return_plus->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(flux_return_plus);

  return;
}
#endif
