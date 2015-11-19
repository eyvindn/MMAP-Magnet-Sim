//
/*
 *Dark Photon Action Initialization 
 *!!!History 
 *   CJC 6.15.14 created
 * 
 *
 *file: ActionInitialization.cc
 *
 * Description:
 * Initiates necessary classes
 */

#include "ActionInitialization.hh"
#include "H02PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"

ActionInitialization::ActionInitialization()
  : G4VUserActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

// Creates instances of the necessary classes for running
void ActionInitialization::BuildForMaster() const
{
 
  SetUserAction(new RunAction);
}


//Creates instances of the necessary classes for Geant
void ActionInitialization::Build() const
{

  SetUserAction(new H02PrimaryGeneratorAction); //CHANGED AUG 27th to add manual madgraph importing! SetUserAction(new PrimaryGeneratorAction);
  SetUserAction(new RunAction);
  SetUserAction(new EventAction);

  G4cout << "Action initializer complete" << G4endl;
}
