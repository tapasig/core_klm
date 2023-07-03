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

  const string steel("Steel_1006");
  const string scintillator("G4_POLYSTYRENE");

  PHG4CylinderSubsystem *flux_return_minus = new PHG4CylinderSubsystem("FLUXRET_ETA_MINUS", 0);
  flux_return_minus->set_double_param("length", G4PLUGDOOR::length_1);
  flux_return_minus->set_double_param("radius", G4PLUGDOOR::r_1);
  flux_return_minus->set_double_param("place_z", G4PLUGDOOR::place_z1);
  flux_return_minus->set_double_param("thickness", G4PLUGDOOR::r_2 - G4PLUGDOOR::r_1);
  flux_return_minus->set_string_param("material", steel);
  flux_return_minus->SetActive(flux_door_active);
  flux_return_minus->SuperDetector("FLUXRET");
  flux_return_minus->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(flux_return_minus);

  /*PHG4CylinderSubsystem *flux_return_minus_layer_1 = new PHG4CylinderSubsystem("FLUXRET_ETA_MINUS_Layer_1", 1);
  flux_return_minus_layer_1->set_double_param("length", G4PLUGDOOR::length_1/5);
  flux_return_minus_layer_1->set_double_param("radius", G4PLUGDOOR::r_1);
  flux_return_minus_layer_1->set_double_param("place_z", G4PLUGDOOR::place_z1-10.);
  flux_return_minus_layer_1->set_double_param("thickness", G4PLUGDOOR::r_2 - G4PLUGDOOR::r_1);
  flux_return_minus_layer_1->set_string_param("material", material);
  flux_return_minus_layer_1->SetActive(flux_door_active);
  flux_return_minus_layer_1->SuperDetector("FLUXRET");
  flux_return_minus_layer_1->SetMotherSubsystem(flux_return_minus);
  flux_return_minus_layer_1->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(flux_return_minus_layer_1);
*/

 //create a daughter Cell inside the flux return of eta minus

 //define the positions
  const int nCell = 5;
  const double cell_length = G4PLUGDOOR::length_1/nCell;
  const double len_cell[nCell] = {cell_length, cell_length, cell_length, cell_length, cell_length};   
  //const double delta_z  = {20, 20, 20, 20, 20};  
 
  cout << " check 1: " << len_cell[0] <<  endl;
  //cout << " check : 2" <<  G4PLUGDOOR::length_1 << (G4PLUGDOOR::r_2 - G4PLUGDOOR::r_1)/cm  << endl;
 
 //loop over the cells 

 //parameters
 double place_z_cell[nCell] = {0}; 

 //loop to put the cells inside the eta_minus_flux_return
  for (int i = 0; i < nCell; i++ ) {
 
 // add name to the cell if required
 //place_z_cell[i] = -210 - 20*i;// {-210. , -230. , -250, -270., -290.}; 

  place_z_cell[i] = G4PLUGDOOR::place_z1 + (G4PLUGDOOR::length_1)/2 - (2 * i + 1) * len_cell[i]/2 ; 

 cout << " check 2: cell= " << i << " length :" << len_cell[i] << " z = " << place_z_cell[i] << " thickness =" << G4PLUGDOOR::r_2 - G4PLUGDOOR::r_1 << endl ;
  
 //create a new cell subsystem
 string name = "Eta_minus_Cell";// + to_string(i);
 //nw with mother-daughter PHG4CylinderSubsystem *eta_minus_cell = new PHG4CylinderSubsystem(Form("Eta_minus_Cell_%d",i), i);
 PHG4CylinderSubsystem *eta_minus_cell = new PHG4CylinderSubsystem(name, i+1);
  eta_minus_cell->set_double_param("length", len_cell[i]);
  eta_minus_cell->set_double_param("radius", G4PLUGDOOR::r_1);
  eta_minus_cell->set_double_param("place_z", place_z_cell[i]);
  eta_minus_cell->set_double_param("thickness", G4PLUGDOOR::r_2 - G4PLUGDOOR::r_1);
 if ( i%2 == 0 ) {  
	eta_minus_cell->set_string_param("material", scintillator);
	}
 else {
	eta_minus_cell->set_string_param("material", steel);
	}
  // selec only the odd layers as actibe
 if ( i%2 == 0 )  eta_minus_cell->SetActive();
 // eta_minus_cell->SetMotherSubsystem(flux_return_minus);
 //SuperDetector ??  
  eta_minus_cell->SuperDetector("FLUXRET");
  eta_minus_cell->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(eta_minus_cell);
 
 }
  
  PHG4CylinderSubsystem *flux_return_central = new PHG4CylinderSubsystem("FLUXRET_ETA_CENTRAL", 6);
  flux_return_central->set_double_param("length", G4PLUGDOOR::length_2);
  flux_return_central->set_double_param("radius", G4PLUGDOOR::r_3);
  flux_return_central->set_double_param("place_z", G4PLUGDOOR::place_z2);
  flux_return_central->set_double_param("thickness", G4PLUGDOOR::r_2 - G4PLUGDOOR::r_3);
  flux_return_central->set_string_param("material", steel);
  flux_return_central->SetActive(flux_door_active);
  flux_return_central->SuperDetector("FLUXRET");
  flux_return_central->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(flux_return_central);

  PHG4CylinderSubsystem *flux_return_plus = new PHG4CylinderSubsystem("FLUXRET_ETA_PLUS", 7);
  flux_return_plus->set_double_param("length", G4PLUGDOOR::length_3);
  flux_return_plus->set_double_param("radius", G4PLUGDOOR::r_1);
  flux_return_plus->set_double_param("place_z", G4PLUGDOOR::place_z3);
  flux_return_plus->set_double_param("thickness", G4PLUGDOOR::r_2 - G4PLUGDOOR::r_1);
  flux_return_plus->set_string_param("material", steel);
  flux_return_plus->SetActive(flux_door_active);
  flux_return_plus->SuperDetector("FLUXRET");
  flux_return_plus->OverlapCheck(OverlapCheck);
  g4Reco->registerSubsystem(flux_return_plus);

//cout << "check : PHG4CylinderSubsystem *cyl = (PHG4CylinderSubsystem *) g4->getSubsystem(\"" << g4Reco->getSubsystem("FLUXRET_ETA_MINUS") << "\");" << endl;
  return;
}
#endif
