#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
	PrimaryGeneratorAction();
    ~PrimaryGeneratorAction();

    // methods
    virtual void GeneratePrimaries(G4Event*);

  private:
    // static data members
    static const G4String fgkDefaultParticleName;
    static const G4double fgkDefaultEnergy;

    // data members
    G4GeneralParticleSource*  fGeneralParticleSource; //pointer a to G4 service class
};

#endif
