//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// -------------------------------------------------------------------
// $Id: Microelectronics.cc,v 1.5 2010-11-18 11:48:21 allison Exp $
// -------------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*
#ifdef G4MULTITHREADED
	#undef G4MULTITHREADED
#endif // так не работает, где-то еще в одном месте включено в файлах?
*/

#ifdef G4MULTITHREADED
  #include "G4MTRunManager.hh"
#else
  #include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"

#include "G4UIExecutive.hh"

#include "G4VisExecutive.hh"

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"

int main(int argc,char** argv) 
{
  G4UIExecutive* session = NULL;
  if (argc==1)   // Define UI session for interactive mode.
  {
      session = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  runManager->SetNumberOfThreads(4); // Is equal to 2 by default
  // Сколько здесь задаем тредов, столько файлов генерируется.
  //
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Set mandatory user initialization classes
  DetectorConstruction* detector = new DetectorConstruction;
  runManager->SetUserInitialization(detector);
  
  runManager->SetUserInitialization(new PhysicsList);
  
  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization(detector));
  
  // Initialize G4 kernel
  runManager->Initialize();
  
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  
  // Get the pointer to the User Interface manager 
  G4UImanager* UImanager = G4UImanager::GetUIpointer();  


#ifdef G4MULTITHREADED
  G4cout << "G4MULTITHREADED defined.." << G4endl;
#endif

  G4cout << "run this:" << G4endl
  	  	  << "/control/execute microelectronics.mac" << G4endl
		  << "/control/execute electrons.mac" << G4endl
		  << "/run/beamOn " << G4endl;
  //UImanager->ApplyCommand("/control/execute microelectronics.mac");
    session->SessionStart();
    delete session;

  delete visManager;
  delete runManager;

  return 0;
}

