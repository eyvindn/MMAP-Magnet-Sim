/*
 * Dark Photon Event Action
 *!!!History
 *   CJC 6.15.14 created
 *   CJC 6.16.14 analysis added
 *
 * file: EventAction.cc
 *
 * Description: 
 * Event Action method is called after every event. We are using it 
 * to store data from event to event in large runs.
 * Methods to update:
 *
 * BeginOfEventAction
 * {Reset the variables you want to keep track of}
 *
 * EndOfEventAction
 * {fill all of the data storage methods with the analysis manager, 
 * i.e. histograms or ntuples. Also include whatever verbose arguments
 * you want in your final read out.}
 *
 * 
 */

#include "EventAction.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <iomanip>

#include "Analysis.hh"
#include "G4PhysicalConstants.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"

#include "TestHit.hh"
#include "OmniHit.hh"


EventAction::EventAction()
  : G4UserEventAction(), 
    fTHSD(-1), 
    fOHSD(-1)
{
}


EventAction::~EventAction()
{}

//!!!
//Reset your variables
void EventAction::BeginOfEventAction(const G4Event* /* run*/)
{
  if (fTHSD==-1)
    {
      G4SDManager* sdMan = G4SDManager::GetSDMpointer();
      fTHSD = sdMan->GetCollectionID("calorimeterSD/TestHitsCollection");
      fOHSD = sdMan->GetCollectionID("omniSD/OmniHitsCollection");
    }
}


//!!!
//Fill your data analysis
//Add verbosity
void EventAction::EndOfEventAction(const G4Event* event)
{

  G4double pi = 3.14159265358979323;
  
  G4HCofThisEvent* hce = event->GetHCofThisEvent(); //hit collection array allows for several kinds of hits 

  //analysis manager
  G4AnalysisManager* analysisMan = G4AnalysisManager::Instance();
  G4SDManager* fSDM = G4SDManager::GetSDMpointer();

  //get ID for the calorimeter's hit collection
  //G4int collectionID = fSDM->GetCollectionID("TestHitsCollection");

  TestHitsCollection* testHitColl = static_cast<TestHitsCollection*>(hce->GetHC(fTHSD));

  OmniHitsCollection* omniHitColl = static_cast<OmniHitsCollection*>(hce->GetHC(fOHSD));
  
  G4double eDep(0.);
  G4bool hits = false;
  G4double totalEnergy(0.); 
 
  for (int i=0; i<1225; i++)
    {
	  TestHit* hit = (*testHitColl)[i];
	  //if (hit->GetID()==22)
	  //{
	      eDep = hit->GetEnergyDep();
	      if (eDep > 0)
		{
		  analysisMan->FillNtupleDColumn(0, i, eDep);
		  totalEnergy+=eDep;
		  hits = true;
		}
	    //}
    }
  G4double energy(0.);
  int eventID = 
    G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  
  analysisMan->FillNtupleDColumn(0, 1225, 0.);
  analysisMan->FillNtupleDColumn(0, 1226, 100.);
  analysisMan->FillNtupleIColumn(0, 1227, 1);
  analysisMan->FillNtupleIColumn(0, 1228, eventID);

  
  G4int hitNum =  omniHitColl->entries();
  G4cout << "Hit number: " << hitNum << G4endl;
  for (int q=0; q<hitNum; q++)
    {
      OmniHit* hit2 = (*omniHitColl)[q];
      //if (hit2->GetCharge()!=0) {continue;}
      //else {
      //	if (hit2->GetStart().getZ()>-4000) {continue;}
      //else{
	  energy = hit2->GetTotalEnergy(); 
	  
	  double x = hit2->GetPos().getX();
	  double y= hit2->GetPos().getY();
	  G4double theta = std::atan(std::sqrt(std::pow(x, 2.)+std::pow(y, 2.))/10150.);

	    analysisMan->FillNtupleDColumn(1, 0, energy); 	   
	    //analysisMan->FillNtupleDColumn(1, 1, theta*180/pi);
	    //analysisMan->FillNtupleDColumn(1, 2, phi*180/pi);
	    analysisMan->FillNtupleDColumn(1, 1, hit2->GetCharge());
	    //analysisMan->FillNtupleIColumn(1, 4, eventID);
	    analysisMan->FillNtupleDColumn(1, 2, hit2->GetStart().getX());
	    analysisMan->FillNtupleDColumn(1, 3, hit2->GetStart().getY());
	    analysisMan->FillNtupleDColumn(1, 4, hit2->GetStart().getZ());
	    analysisMan->FillNtupleDColumn(1, 5, hit2->GetTar().getX());
	    analysisMan->FillNtupleDColumn(1, 6, hit2->GetTar().getY());
	    analysisMan->FillNtupleDColumn(1, 7, hit2->GetTar().getZ());
	    //analysisMan->FillNtupleIColumn(1, 8, hit2->GetTrack());
	    analysisMan->AddNtupleRow(1);
	    // }
	    //}
    }

 

if (hits) {analysisMan->FillNtupleIColumn(0, 1228, 1);}
analysisMan->AddNtupleRow(0); // now root number of events matches Geant
  
}
