#ifndef MACRO_G4MAGNET_C
#define MACRO_G4MAGNET_C

#include <GlobalVariables.C>

#include <g4detectors/PHG4CylinderSubsystem.h>

#include <Geant4/G4Types.hh>  // for G4double, G4int
#include <Geant4/G4Material.hh>
#include <Geant4/G4MaterialPropertiesTable.hh>  // for G4MaterialProperties...
#include <Geant4/G4MaterialPropertyVector.hh>   // for G4MaterialPropertyVector
#include <Geant4/G4NistManager.hh>

void DefineMaterials()
{
  
  G4double density;
  G4int ncomponents;

  // Material for CORE copper testing... 
  G4Material *CoilCORE = new G4Material("Copper_Magnet", density = 9.0 * g / cm3, ncomponents = 3);
  CoilCORE->AddElement(G4NistManager::Instance()->FindOrBuildElement("Cu"), 0.722);
  CoilCORE->AddElement(G4NistManager::Instance()->FindOrBuildElement("Nb"), 0.139);
  CoilCORE->AddElement(G4NistManager::Instance()->FindOrBuildElement("Ti"), 0.139);
}

R__LOAD_LIBRARY(libg4detectors.so)

namespace Enable
{
  bool MAGNET = false;
  bool MAGNET_ABSORBER = false;
  bool MAGNET_OVERLAPCHECK = false;
  int MAGNET_VERBOSITY = 0;
}  // namespace Enable

namespace G4MAGNET
{
  // Outer cylindrical wall
  double magnet_outer_cryostat_wall_radius = 130.;
  double magnet_outer_cryostat_wall_thickness = 10.;
  double magnet_length = 250.;
}  // namespace G4MAGNET

void MagnetFieldInit()
{
  if (!isfinite(G4MAGNET::magfield_rescale))
  {
    G4MAGNET::magfield_rescale = 1.;
  }
  if (G4MAGNET::magfield.empty())
  {
    G4MAGNET::magfield = string(getenv("CALIBRATIONROOT")) + string("/Field/Map/sPHENIX.2d.root");
  }
}

void MagnetInit()
{
  MagnetFieldInit();
  BlackHoleGeometry::max_radius = std::max(BlackHoleGeometry::max_radius, G4MAGNET::magnet_outer_cryostat_wall_radius + G4MAGNET::magnet_outer_cryostat_wall_thickness);
  BlackHoleGeometry::max_z = std::max(BlackHoleGeometry::max_z, G4MAGNET::magnet_length / 2.);
  BlackHoleGeometry::min_z = std::min(BlackHoleGeometry::min_z, -G4MAGNET::magnet_length / 2.);
 BlackHoleGeometry::visible = true;
}

double Magnet(PHG4Reco* g4Reco, double radius)
{
  DefineMaterials();

  bool AbsorberActive = Enable::ABSORBER || Enable::MAGNET_ABSORBER;
  bool OverlapCheck = Enable::OVERLAPCHECK || Enable::MAGNET_OVERLAPCHECK;
  int verbosity = std::max(Enable::VERBOSITY, Enable::MAGNET_VERBOSITY);

  // Inner cylindrical wall
  double magnet_inner_cryostat_wall_radius = 100;
  double magnet_inner_cryostat_wall_thickness = 4;

  // Coil
  double magnet_coil_radius = 115;
  double magnet_coil_thickness = 4.;
  double magnet_length = 250.;

  // Coil support ring
  double magnet_coil_support_radius = 119;
  double magnet_coil_support_thickness = 10;

  // Radiation shield
  double magnet_radiation_shield_radius = 125; // NEED TO UPDATE!
  double magnet_radiation_shield_thickness = 0.1;

  double coil_length = 250.;
  if (radius > magnet_inner_cryostat_wall_radius)
  {
    cout << "inconsistency: radius: " << radius
         << " larger than Magnet inner radius: " << magnet_inner_cryostat_wall_radius << endl;
    gSystem->Exit(-1);
  }

  radius = magnet_inner_cryostat_wall_radius;
  
  // Change the material for the magnet
   string material = "Al5083";
  //string material = "Copper_Magnet";  
  cout << "G4_Magnet...Magnet Material: " << material << endl;


  // Inner cylindrical wall
  PHG4CylinderSubsystem* cyl = new PHG4CylinderSubsystem("MAGNET", 0);
  cyl->set_color(1.,0.,0.,1.);
  cyl->set_double_param("radius", magnet_inner_cryostat_wall_radius);
  cyl->set_int_param("lengthviarapidity", 0);
  cyl->set_double_param("length", G4MAGNET::magnet_length);
  cyl->set_double_param("thickness", magnet_inner_cryostat_wall_thickness);
  cyl->set_string_param("material", material);  // use 1 radiation length Al for magnet thickness
  cyl->SuperDetector("MAGNET");
  if (AbsorberActive) cyl->SetActive();
  cyl->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(cyl);

  // Coil
  cyl = new PHG4CylinderSubsystem("MAGNET", 1);
  cyl->set_color(1.,0.,0.,1.);
  cyl->set_double_param("radius", magnet_coil_radius);
  cyl->set_int_param("lengthviarapidity", 0);
  cyl->set_double_param("length", coil_length);
  cyl->set_double_param("thickness", magnet_coil_thickness);
  cyl->set_string_param("material", material);  // use 1 radiation length Al for magnet thickness
  cyl->SuperDetector("MAGNET");
  if (AbsorberActive) cyl->SetActive();
  cyl->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(cyl);
 
  // Coil support ring
  cyl = new PHG4CylinderSubsystem("MAGNET", 2);
  cyl->set_color(1.,0.,0.,1.);
  cyl->set_double_param("radius", magnet_coil_support_radius);
  cyl->set_int_param("lengthviarapidity", 0);
  cyl->set_double_param("length", coil_length);
  cyl->set_double_param("thickness", magnet_coil_support_thickness);
  cyl->set_string_param("material", material);  // use 1 radiation length Al for magnet thickness
  cyl->SuperDetector("MAGNET");
  if (AbsorberActive) cyl->SetActive();
  cyl->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(cyl);

  // Radiation shield
  cyl = new PHG4CylinderSubsystem("MAGNET", 3);
  cyl->set_color(1.,0.,0.,1.);
  cyl->set_double_param("radius", magnet_radiation_shield_radius);
  cyl->set_int_param("lengthviarapidity", 0);
  cyl->set_double_param("length", G4MAGNET::magnet_length);
  cyl->set_double_param("thickness", magnet_radiation_shield_thickness);
  cyl->set_string_param("material", material);  // use 1 radiation length Al for magnet thickness
  cyl->SuperDetector("MAGNET");
  if (AbsorberActive) cyl->SetActive();
  cyl->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(cyl);

  // Outer cylindrical wall
  cyl = new PHG4CylinderSubsystem("MAGNET", 4);
  cyl->set_color(1.,0.,0.,1.);
  cyl->set_double_param("radius", G4MAGNET::magnet_outer_cryostat_wall_radius);
  cyl->set_int_param("lengthviarapidity", 0);
  cyl->set_double_param("length", G4MAGNET::magnet_length);
  cyl->set_double_param("thickness", G4MAGNET::magnet_outer_cryostat_wall_thickness);
  cyl->set_string_param("material", material);  // use 1 radiation length Al for magnet thickness
  cyl->SuperDetector("MAGNET");
  if (AbsorberActive) cyl->SetActive();
  cyl->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(cyl);


  radius = G4MAGNET::magnet_outer_cryostat_wall_radius + G4MAGNET::magnet_outer_cryostat_wall_thickness;  // outside of magnet

  if (verbosity > 0)
  {
    cout << "========================= G4_Magnet.C::Magnet() ===========================" << endl;
    cout << " MAGNET Material Description:" << endl;
    cout << "  inner radius = " << magnet_inner_cryostat_wall_radius << " cm" << endl;
    cout << "  outer radius = " << G4MAGNET::magnet_outer_cryostat_wall_radius + G4MAGNET::magnet_outer_cryostat_wall_thickness << " cm" << endl;
    cout << "  length = " << G4MAGNET::magnet_length << " cm" << endl;
    cout << "===========================================================================" << endl;
  }

  radius += no_overlapp;

  return radius;
}
#endif
