/*
 * Dark Photon Run Action
 * !!!History 
 *    CJC 6.14.15 created
 *
 * file RunAction.cc
 * Description: 
 * Run Action is called for every run. Consider what data should be collected
 * over long runs
 */



#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

#include "Analysis.hh"
#include "G4SystemOfUnits.hh"


//!!!
//Initialize analysis storing i.e. ntuples or histograms
RunAction::RunAction()
  : G4UserRunAction()
{
  //set printing event number for each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // Data storage and analysis
  G4AnalysisManager* analysisMan = G4AnalysisManager::Instance();

  analysisMan->SetVerboseLevel(1);
  
  analysisMan->CreateNtuple("Signal", "SignalTest");

      for (G4int j = 0; j<1225; j++)
	{
	  std::stringstream ss2; 
	  ss2 << j; 
	  G4String str = ss2.str();
	  analysisMan->CreateNtupleDColumn("Crystal_"+str);
	}
      //These next 3 columns are for testing code only
      analysisMan->CreateNtupleDColumn("EventTime");
      analysisMan->CreateNtupleDColumn("EventEnergy");
      analysisMan->CreateNtupleIColumn("ID");
      analysisMan->CreateNtupleIColumn("EventID");


      analysisMan->CreateNtupleIColumn("hits");
      analysisMan->FinishNtuple(); 

      analysisMan->CreateNtuple("Check", "Check");
 
      analysisMan->CreateNtupleDColumn("Energy");
      //analysisMan->CreateNtupleDColumn("Theta");
      //analysisMan->CreateNtupleDColumn("Phi");
      analysisMan->CreateNtupleDColumn("Charge");
      //analysisMan->CreateNtupleIColumn("EventID");
      analysisMan->CreateNtupleDColumn("StartPosX");
      analysisMan->CreateNtupleDColumn("StartPosY");
      analysisMan->CreateNtupleDColumn("StartPosZ");
      analysisMan->CreateNtupleDColumn("TarX");
      analysisMan->CreateNtupleDColumn("TarY");
      analysisMan->CreateNtupleDColumn("TarZ");
      //analysisMan->CreateNtupleIColumn("TrackID");
      analysisMan->FinishNtuple();

      analysisMan->CreateNtuple("True", "True");
      analysisMan->CreateNtupleDColumn("Energy");
      analysisMan->CreateNtupleDColumn("Theta");
      analysisMan->CreateNtupleDColumn("Phi");
      

      analysisMan->FinishNtuple();
      
}



//delete your analysis manager
RunAction::~RunAction()
{
  delete G4AnalysisManager::Instance();
}

//!!!
//Data storage
void RunAction::BeginOfRunAction(const G4Run*)
{

  //Save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  
  // Data storage and analysis
  G4AnalysisManager* analysisMan = G4AnalysisManager::Instance();
 
   analysisMan->OpenFile("Signal");

}

//!!!
//Finish and close data file
void RunAction::EndOfRunAction(const G4Run* )
{
  
  G4AnalysisManager* analysisMan = G4AnalysisManager::Instance();
  analysisMan->Write();
  analysisMan->CloseFile();
}
