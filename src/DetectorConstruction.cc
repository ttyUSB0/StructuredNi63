#include "DetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4UniformMagField.hh"
#include "G4Mag_UsualEqRhs.hh"
//#include "G4EqMagElectricField.hh"
#include "G4HelixHeum.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"

#include <iostream>
#include <fstream>

#include <cstdlib>
#include <sstream>
#include <cstring>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

DetectorConstruction::DetectorConstruction()
:fPhysiWorld(NULL), fLogicWorld(NULL), fSolidWorld(NULL)
{}  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

DetectorConstruction::~DetectorConstruction()
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  DefineMaterials();
  return ConstructDetector();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void DetectorConstruction::DefineMaterials()
{ 

  // Use NIST material database
  G4NistManager * man = G4NistManager::Instance();
  G4Material * Si = man->FindOrBuildMaterial("G4_Si");
  G4Material* Ni = man->FindOrBuildMaterial("G4_Ni");
  G4Material* Air = man->FindOrBuildMaterial("G4_AIR");
  G4Material* SiO2 = man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");


  // Default materials in setup.
  fSiMaterial = Si;
  fNiMaterial = Ni;
  fAirMaterial = Air;
  fSiO2Material = SiO2;
}

G4VPhysicalVolume* DetectorConstruction::ConstructDetector()
{
	// Size vars, in um:
  G4double NiLayerThickness = 0.7*um;
  G4double SiO2LayerThickness = 0.14*um;
  G4double SiLayerThickness = 20*um;
  
  G4double windowLen = 0.7*um;

	// open *.mac file to configure GPS
	std::ofstream file;
	file.open("electrons.mac", std::ios::out);
	file << "# Using General Particle Source" << std::endl
		<< "/gps/source/clear" << std::endl << std::endl;

  // WORLD VOLUME - *************** SiO2 fills all volume ****
  fWorldSizeX  = 100*um; // actual size
  fWorldSizeY  = 100*um;
  fWorldSizeZ  = NiLayerThickness + SiO2LayerThickness + SiLayerThickness; //Ni/SiO2/Si

  fSolidWorld = new G4Box("World", //its name
	fWorldSizeX/2,fWorldSizeY/2,fWorldSizeZ/2);  //its size
  fLogicWorld = new G4LogicalVolume(fSolidWorld,	//its solid
	fSiO2Material,//its material
	"World");//its name
  fPhysiWorld = new G4PVPlacement(0,	//no rotation
	G4ThreeVector(),	//at (0,0,0)
	"World",		//its name
	fLogicWorld,		//its logical volume
	0,			//its mother  volume
	false,			//no boolean operation
	0);			//copy number

  // ------------------------------------ Ni layer
  // Consist of two parts
	G4double NiXSize = fWorldSizeX/2 - windowLen/2;
  // -------- Ni1
	G4Box* solidNickel1 = new G4Box("Ni1",	//its name
		NiXSize/2, fWorldSizeY/2, NiLayerThickness/2);   //its size
	G4LogicalVolume* logicNi1 = new G4LogicalVolume(solidNickel1,       //its solid
		fNiMaterial,	//its material
		"Ni1");		//its name
	new G4PVPlacement(0,  //no rotation
		G4ThreeVector(-fWorldSizeX/2 + NiXSize/2, 0*um, -fWorldSizeZ/2 + NiLayerThickness/2),
		"Ni1",		//its name
		logicNi1,	//its logical volume
		fPhysiWorld,	//its mother  volume
		false,		//no boolean operation
		0);		//copy number
		
	file << "# ------------- Ni1" << std::endl
		<< "/gps/source/add 1" << std::endl
		<< "/gps/particle e-" << std::endl 
		<< "/gps/direction 0 0 1" << std::endl 
		<< "/gps/pos/type Volume" << std::endl 
		<< "/gps/pos/shape Para" << std::endl 
		<< "/gps/pos/halfx " << NiXSize/2/um << " um" << std::endl 
		<< "/gps/pos/halfy " << fWorldSizeY/2/um << " um" << std::endl 
		<< "/gps/pos/halfz " << NiLayerThickness/2/um << " um" << std::endl 
		<< "/gps/pos/centre " << (-fWorldSizeX/2 + NiXSize/2)/um << " 0 " << (-fWorldSizeZ/2 + NiLayerThickness/2)/um <<  " um" << std::endl 
		<< "/gps/ang/type iso" << std::endl 
		<< "/gps/ang/mintheta 90.0 deg" << std::endl
		<< "/gps/ang/maxtheta 180 deg" << std::endl
		<< "/gps/ene/type Exp" << std::endl
		<< "/gps/ene/min 100 eV" << std::endl
		<< "/gps/ene/max 63 keV" << std::endl
		<< "/gps/ene/ezero 0.02" << std::endl
		<< std::endl;
		
  // -------- Ni2
	G4Box* solidNickel2 = new G4Box("Ni2",	//its name
		NiXSize/2, fWorldSizeY/2, NiLayerThickness/2);   //its size
	G4LogicalVolume* logicNi2 = new G4LogicalVolume(solidNickel2,       //its solid
		fNiMaterial,	//its material
		"Ni2");		//its name
	new G4PVPlacement(0,  //no rotation
		G4ThreeVector(fWorldSizeX/2 - NiXSize/2, 0*um, -fWorldSizeZ/2 + NiLayerThickness/2),
		"Ni2",		//its name
		logicNi2,	//its logical volume
		fPhysiWorld,	//its mother  volume
		false,		//no boolean operation
		0);		//copy number
		
	file << "# ------------- Ni2" << std::endl
		<< "/gps/source/add 1" << std::endl
		<< "/gps/particle e-" << std::endl 
		<< "/gps/direction 0 0 1" << std::endl 
		<< "/gps/pos/type Volume" << std::endl 
		<< "/gps/pos/shape Para" << std::endl 
		<< "/gps/pos/halfx " << (NiXSize/2)/um << " um" << std::endl 
		<< "/gps/pos/halfy " << (fWorldSizeY/2)/um << " um" << std::endl 
		<< "/gps/pos/halfz " << (NiLayerThickness/2)/um << " um" << std::endl 
		<< "/gps/pos/centre " << (fWorldSizeX/2 - NiXSize/2)/um << " 0 " << (-fWorldSizeZ/2 + NiLayerThickness/2)/um <<  " um" << std::endl 
		<< "/gps/ang/type iso" << std::endl 
		<< "/gps/ang/mintheta 90.0 deg" << std::endl
		<< "/gps/ang/maxtheta 180 deg" << std::endl
		<< "/gps/ene/type Exp" << std::endl
		<< "/gps/ene/min 100 eV" << std::endl
		<< "/gps/ene/max 63 keV" << std::endl
		<< "/gps/ene/ezero 0.02" << std::endl
		<< std::endl;

  // ------------------------------------ Si layer
	G4Box* solidSi = new G4Box("Si",				     //its name
		fWorldSizeX/2,fWorldSizeY/2,SiLayerThickness/2);   //its size
	G4LogicalVolume* logicSi = new G4LogicalVolume(solidSi,       //its solid
		fSiMaterial,	//its material
		"Si");		//its name
	new G4PVPlacement(0,  //no rotation
		G4ThreeVector(0*um, 0*um, -fWorldSizeZ/2 + NiLayerThickness + SiO2LayerThickness + SiLayerThickness/2),	                               //at (0,0,0)
		"Si",		//its name
		logicSi,	//its logical volume
		fPhysiWorld,		//its mother  volume
		false,		//no boolean operation
		0);			//copy number

  // close file
	file << "# # RUN" << std::endl
		<< "/run/initialize" << std::endl;
	file.close();

  // Visualization attributes
  G4VisAttributes* worldVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  worldVisAtt->SetVisibility(true);
  fLogicWorld->SetVisAttributes(worldVisAtt);

  G4VisAttributes* worldVisAtt2 = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  worldVisAtt2->SetVisibility(true);
  logicNi1->SetVisAttributes(worldVisAtt2);
  logicNi2->SetVisAttributes(worldVisAtt2);


  // Create Target G4Region and add logical volume
  fRegion = new G4Region("Target");
  
  G4ProductionCuts* cuts = new G4ProductionCuts();
  
  G4double defCut = 1*nanometer;
  cuts->SetProductionCut(defCut,"gamma");
  cuts->SetProductionCut(defCut,"e-");
  cuts->SetProductionCut(defCut,"e+");
  cuts->SetProductionCut(defCut,"proton");
  
  fRegion->SetProductionCuts(cuts);
  fRegion->AddRootLogicalVolume(logicSi);

  return fPhysiWorld;
}
