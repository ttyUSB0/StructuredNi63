
#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"

const G4String PrimaryGeneratorAction::fgkDefaultParticleName = "e-";
const G4double PrimaryGeneratorAction::fgkDefaultEnergy = 17*keV;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction(),
		fGeneralParticleSource(0)
{
	fGeneralParticleSource  = new G4GeneralParticleSource();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete fGeneralParticleSource;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	// this function is called at the begining of event
	fGeneralParticleSource->GeneratePrimaryVertex(anEvent);
	//std::cout << "     evnt:" << anEvent->GetEventID() << std::endl;
}
