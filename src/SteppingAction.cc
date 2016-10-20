#include "Analysis.hh"

#include "SteppingAction.hh"
#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4SteppingManager.hh"
#include "G4VTouchable.hh"
#include "G4VPhysicalVolume.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

SteppingAction::SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void SteppingAction::UserSteppingAction(const G4Step* step)
{ 
 //std::cout << "s" << step->GetTrack()->GetTrackID() << " ";
 G4double flagParticle=0.;
 G4double flagProcess=0.;
 G4double x,y,z;
 G4double x2,y2,z2;
 
 G4String Particle = step->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName();
 
 if (step->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "e-")
 	flagParticle = 1;    
 else if (step->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "proton") 
	flagParticle = 2;
 else if (step->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "GenericIon")
 	flagParticle = 3;
 else {
 	G4cout  << G4endl << "Unknown particle!!!" << G4endl;
 }
 
 G4String Process = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
 //std::cout << "Process:" << Process << " ";
 
 if (Process =="msc")
 	flagProcess =30;
 else if (Process=="e-_G4MicroElecElastic")
 	flagProcess =21;
 else if (Process=="e-_G4MicroElecInelastic")
 	flagProcess =22;
 else if (Process=="eCapture")
 	flagProcess =11;
 else if (Process=="p_G4MicroElecInelastic")
 	flagProcess =12;
 else if (Process=="ion_G4MicroElecInelastic")	
 	flagProcess =13;
 else if (Process=="hIoni")
 	flagProcess =14;
 else if (Process=="eIoni")
 	flagProcess =40;
 	
 	
 if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="Transportation")
 {  
   x=step->GetPreStepPoint()->GetPosition().x()/nanometer;
   y=step->GetPreStepPoint()->GetPosition().y()/nanometer;
   z=step->GetPreStepPoint()->GetPosition().z()/nanometer;

   x2=step->GetPostStepPoint()->GetPosition().x()/nanometer;
   y2=step->GetPostStepPoint()->GetPosition().y()/nanometer;
   z2=step->GetPostStepPoint()->GetPosition().z()/nanometer;

   
   // get analysis manager
   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

   // fill ntuple
   analysisManager->FillNtupleDColumn(0, flagParticle);
   analysisManager->FillNtupleDColumn(1, flagProcess);
   analysisManager->FillNtupleDColumn(2, step->GetTrack()->GetTrackID());
   analysisManager->FillNtupleDColumn(3, step->GetTrack()->GetParentID());
   analysisManager->FillNtupleDColumn(4, x);
   analysisManager->FillNtupleDColumn(5, y);
   analysisManager->FillNtupleDColumn(6, z);
   analysisManager->FillNtupleDColumn(7, (step->GetPreStepPoint()->GetKineticEnergy() - step->GetPostStepPoint()->GetKineticEnergy())/eV );
   //analysisManager->FillNtupleDColumn(8, step->GetNonIonizingEnergyDeposit()/eV);
   //analysisManager->FillNtupleDColumn(9, step->GetTotalEnergyDeposit()/eV);
   //Total energy deposited during the step - this is the sum of
   //    the energy deposited by the energy loss process, and
   //    the energy lost by secondaries which have NOT been generated because each of their energies was below the cut threshold
   // Поскольку NonIonizingEnergyDeposit всегда 0 (по опыту), вся энергия из TED идет на генерацию ЭДП

   analysisManager->FillNtupleDColumn(8, step->GetPreStepPoint()->GetKineticEnergy()/eV);
   analysisManager->FillNtupleDColumn(9, x2);
   analysisManager->FillNtupleDColumn(10, y2);
   analysisManager->FillNtupleDColumn(11, z2);
   analysisManager->AddNtupleRow();      
 }
 
 
}    
