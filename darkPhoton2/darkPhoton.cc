//
/*
 *Dark photon test platform for various known E&M processes
 *
 *!!!History:
 * CJC 6.15.14 Created
 *
 *file: darkPhoton.cc
 */
//

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"


#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4HadronicProcessStore.hh"
#include "G4UImanager.hh"
#include "G4VModularPhysicsList.hh"
#include "G4StepLimiterPhysics.hh"
#include "FTFP_BERT.hh"
#include "Randomize.hh"


//Visualization
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

//user interactive
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc, char** argv)
{

  //Choose the random engine

  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  //construct default run manager

#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager; 
  runManager->SetNumberOfThreads(1);
#else
  G4RunManager * runManager = new G4RunManager;
#endif

  /*Set classes required by Geant. Geant needs for 
// 1. Defining detector construction
// 2. Choosing the physics list
// 3. Setting the action initialization
// 4. Initializing the G4 kernel
   */
  //use specific physics list
  
  G4VModularPhysicsList* physicsList = new FTFP_BERT(0);
  physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  runManager->SetUserInitialization(physicsList);

  // runManager->SetUserInitialization(new DarkPhysicsList);

  G4HadronicProcessStore::Instance()->SetVerbose(0);

  DetectorConstruction* test = new DetectorConstruction();
  runManager->SetUserInitialization(test);
  //!!!
  runManager->SetUserInitialization(new ActionInitialization());

  //Initialize G4 kernel

  runManager->Initialize();

  //Visualization
#ifdef G4VIS_USE
  G4VisManager* visManager = new G4VisExecutive; 
  visManager->Initialize();
#endif

  //Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
 //!!!!
  //Don't want to hear it, Geant.
  UImanager->ApplyCommand("/tracking/verbose 0");
  UImanager->ApplyCommand("/control/verbose 0");
  UImanager->ApplyCommand("/run/verbose 0");
  UImanager->ApplyCommand("/event/verbose 0");
  UImanager->ApplyCommand("/run/particle/verbose 0");
  UImanager->ApplyCommand("/process/verbose 0");
  UImanager->ApplyCommand("/vis/set/verbose 0");
  UImanager->ApplyCommand("/particle/property/verbose 0");



    //batch mode
    if (argc !=1)
      {
	G4String command = "/control/execute ";
	G4String fileName = argv[1]; 
	UImanager->ApplyCommand(command+fileName);
      }
    else
      {   // interactive mode: define UI session
#ifdef G4UI_USE
	G4UIExecutive* ui = new G4UIExecutive(argc, argv);

	//checks if visualization is used
#ifdef G4VIS_USE
	UImanager->ApplyCommand("/control/execute init_vis.mac");

#else
	UImanager->ApplyCommand("/control/execute init.mac");
#endif
	if (ui->IsGUI())
	  UImanager->ApplyCommand("/control/execute gui.mac");
          ui->SessionStart();
	  delete ui;
#endif
      }


  /*
   *Other managers created are deleted
   *along with the deletion of the run manager
   *at the termination of the run.
   */
#ifdef G4VIS_USE
    delete visManager;
#endif

  delete runManager;

  return 0;


}

