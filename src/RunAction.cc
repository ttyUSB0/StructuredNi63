#include "RunAction.hh"
#include "G4Run.hh"
#include "TrackingAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "Analysis.hh"
#include "G4Threading.hh"

void PrintNParticles(std::map<const G4ParticleDefinition*, int>& container);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

RunAction::RunAction()
{
	fpTrackingAction = 0;
	fInitialized = 0;
	fDebug = true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

RunAction::~RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::BeginOfRunAction(const G4Run* run)
{  
  // In this example, we considered that the same class was
  // used for both master and worker threads.
  // However, in case the run action is long,
  // for better code review, this practice is not recommanded.
  //
  
  if(isMaster) // WARNING : in sequential mode, isMaster == true    
    BeginMaster(run);    
  else 
    BeginWorker(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::EndOfRunAction(const G4Run* run)
{
  if(isMaster)   
    EndMaster(run);
  else	
    EndWorker(run);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::BeginMaster(const G4Run* run)
{
  bool sequential = (G4RunManager::GetRunManager()->GetRunManagerType() == G4RunManager::sequentialRM);
  
  if(fDebug)
    {
	  if(sequential) G4cout << "RunAction::BeginMaster, sequential=true;" << G4endl;
      G4cout << "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°" << G4endl;
      if(!sequential)
	G4cout << "°°°°°°°°°°°°°°°° RunAction::BeginMaster" << G4endl;
      PrintRunInfo(run);
      G4cout << "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°" << G4endl;
    }
  
  if(sequential)
    {
      if(!fInitialized)	
	InitializeWorker(run);
      // Note: fpTrackingAction could be used as a flag for initialization instead      

      CreateHistogram();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::BeginWorker(const G4Run* run)
{
  if(fDebug)
    {
      G4cout << "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°" << G4endl;
      G4cout << "°°°°°°°°°°°°°°°° RunAction::BeginWorker" << G4endl;
      PrintRunInfo(run);
      G4cout << "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°" << G4endl;
    }
  if(!fInitialized)	
    InitializeWorker(run);
  
  CreateHistogram();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::EndMaster(const G4Run* run)
{
  bool sequential = (G4RunManager::GetRunManager()->GetRunManagerType() 
		     == G4RunManager::sequentialRM);
  if(sequential)    
    EndWorker(run);    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::EndWorker(const G4Run* run)
{
  if(fDebug)
    {
      PrintRunInfo(run);
    }
  
  G4int nofEvents = run->GetNumberOfEvent();
  if ( nofEvents == 0 )
    {
      if(fDebug)
	{
	  G4cout << "°°°°°°°°°°°°°°°° NO EVENTS TREATED IN THIS RUN ==> LEAVING RunAction::EndOfRunAction "<< G4endl;
	}
      return;
    }
  
  ///////////////
  // Write Histo
  //
  WriteHistogram();
  
  ///////////////
  // Complete cleanup
  //
  delete G4AnalysisManager::Instance();
  
  ///////////////
  // Printouts
  //
  std::map<const G4ParticleDefinition*, int>&
    particlesCreatedInWorld = fpTrackingAction->GetNParticlesCreatedInWorld();
  
  G4cout << "Number and type of particles created outside region \"Target\" :" << G4endl;
  
  PrintNParticles(particlesCreatedInWorld);
  
  G4cout << "_______________________" << G4endl;
  
  std::map<const G4ParticleDefinition*, int>&
    particlesCreatedInTarget = fpTrackingAction->GetNParticlesCreatedInTarget();
  
  G4cout << "Number and type of particles created in region \"Target\" :" << G4endl;
  
  PrintNParticles(particlesCreatedInTarget);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::InitializeWorker(const G4Run*)
{
	if (fpTrackingAction == 0)
	{
		fpTrackingAction = (TrackingAction*) G4RunManager::GetRunManager()->GetUserTrackingAction();

		if(fpTrackingAction == 0 && isMaster == false)
		{
			G4ExceptionDescription exDescrption ;
			exDescrption << "fpTrackingAction is a null pointer. Has it been correctly initialized ?";
			G4Exception("RunAction::BeginOfRunAction","RunAction001",FatalException, exDescrption);
		}
	}

	fInitialized = true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::CreateHistogram()
{
	// Book histograms, ntuple

	// Create analysis manager
	// The choice of analysis technology is done via selection of a namespace
	// in Analysis.hh

	G4cout << "##### Create analysis manager " << "  " << this << G4endl;
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	G4cout << "Using " << analysisManager->GetType() << " analysis manager" << G4endl;

	// Create directories

	//analysisManager->SetHistoDirectoryName("histograms");
	//analysisManager->SetNtupleDirectoryName("ntuple");
	analysisManager->SetVerboseLevel(1);

	// Open an output file

	G4String fileName = "microelectronics";
	analysisManager->OpenFile(fileName);

	// Creating ntuple

	analysisManager->CreateNtuple("microelectronics", "physics");
	analysisManager->CreateNtupleDColumn("flagParticle");
	analysisManager->CreateNtupleDColumn("flagProcess");
	analysisManager->CreateNtupleDColumn("TrackID");
	analysisManager->CreateNtupleDColumn("ParentID");
	analysisManager->CreateNtupleDColumn("x");
	analysisManager->CreateNtupleDColumn("y");
	analysisManager->CreateNtupleDColumn("z");
	analysisManager->CreateNtupleDColumn("kineticEnergyDifference");
	//analysisManager->CreateNtupleDColumn("NonIonizingEnergyDeposit/eV");
	//analysisManager->CreateNtupleDColumn("GetTotalEnergyDeposit/eV");
	analysisManager->CreateNtupleDColumn("PreStepPointKineticEnergy/eV");
	analysisManager->CreateNtupleDColumn("xPostStepPoint");
	analysisManager->CreateNtupleDColumn("yPostStepPoint");
	analysisManager->CreateNtupleDColumn("zPostStepPoint");
	analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::WriteHistogram()
{
	// print histogram statistics
	//
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// save histograms
	//

	analysisManager->Write();
	analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void RunAction::PrintRunInfo(const G4Run* run)
{
	G4cout << "°°°°°°°°°°°°°°°° Run is = " << run->GetRunID() << G4endl;
	G4cout << "°°°°°°°°°°°°°°°° Run type is = " << G4RunManager::GetRunManager()->GetRunManagerType() << G4endl;
	G4cout << "°°°°°°°°°°°°°°°° Event processed = " << run->GetNumberOfEventToBeProcessed() << G4endl;
	G4cout << "°°°°°°°°°°°°°°°° N° Event = " << run->GetNumberOfEvent() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void PrintNParticles(std::map<const G4ParticleDefinition*, int>& container)
{
    std::map<const G4ParticleDefinition*, int>::iterator it;
    for(it = container.begin() ;
        it != container.end(); it ++)
    {
        G4cout << "N " << it->first->GetParticleName() << " : " << it->second << G4endl;
    }
}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
