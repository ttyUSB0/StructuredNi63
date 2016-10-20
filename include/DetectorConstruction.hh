#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4UniformMagField.hh"
#include "G4Mag_UsualEqRhs.hh"
//#include "G4EqMagElectricField.hh"
#include "G4HelixHeum.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class G4Region;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:

  DetectorConstruction();

  ~DetectorConstruction();

  G4VPhysicalVolume* Construct();

  G4Region* GetTargetRegion()  {return fRegion;}
                         
private:
   
  G4double           fWorldSizeX;
  G4double           fWorldSizeY;
  G4double           fWorldSizeZ;

  G4VPhysicalVolume* fPhysiWorld;
  G4LogicalVolume*   fLogicWorld;  
  G4Box*             fSolidWorld;

  G4Material*        fSiMaterial;
  G4Material*        fNiMaterial;
  G4Material*        fAirMaterial;
  G4Material*        fSiO2Material;
  G4Region*          fRegion;

  void DefineMaterials();

  G4VPhysicalVolume* ConstructDetector();     
};
#endif
