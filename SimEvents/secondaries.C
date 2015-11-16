 #include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TRandom3.h"
#include <iostream>
#include <sstream>
#include "THStack.h"

void secondaries(){
  TFile* file = new TFile("2gamma5kplast.root");
  TFile* file1 = new TFile("3gamma5kplast.root");
  TFile* file2 = new TFile("radbhabha5kplast.root");
  TFile* file3 = new TFile("bhabha5kplast.root");

  TTree* tree = (TTree *)file->Get("Check");
  TTree* tree1 = (TTree *)file1->Get("Check");
  TTree* tree2 = (TTree *)file2->Get("Check");
  TTree* tree3 = (TTree *)file3->Get("Check");

  int nEvents= tree->GetEntries();
  int nEvents1=tree1->GetEntries();
  int nEvents2=tree2->GetEntries();
  int nEvents3=tree3->GetEntries();
  
  TH1D* charge = new TH1D("charge", "Charge of Particles", 5, -2.5, 2.5);
  TH1D* Zpos1 = new TH1D("zpos1", "Z Position of Origin", 110, -1, 10);
  TH1D* Zpos2 = new TH1D("zpos2", "Z Position of Origin", 110, -1, 10);
  TH1D* Zpos3 = new TH1D("zpos3", "Z Position of Origin", 110, -1, 10);
  TH1D* Zpos4 = new TH1D("zpos4", "Z Position of Origin", 110, -1, 10);
  TH1D* Zpos = new TH1D("zpos", "Z Position of Origin", 110, -1, 10);
  TH1D* xpos = new TH1D("xpos", "X Position of Origin", 200, -2, 2);
  TH1D* ypos = new TH1D("ypos", "Y Position of Origin", 200, -2, 2);
  TH2D* dist = new TH2D("dist", "R v. Z Position of Origin", 110, -1, 10, 100, 0, 1);
  TH2D* eng = new TH2D("eng", "Energy distribution in Z", 11, -1, 10, 500, 0, 1000);
  TH1D* engDist = new TH1D("engDist", "Energy Distribution Of Secondary Particles", 100, 0, 100);
   TH1D* engDist2 = new TH1D("engDist2", "Energy Distribution Of Secondary Particles", 25, 0, 25);
  TH2D* tar = new TH2D("tar", "Target Position", 100, -1, 1, 100, -1, 1);

 engDist->SetStats("nemo");
 engDist->StatOverflows(kTRUE);
 engDist2->StatOverflows(kTRUE);
 engDist2->SetStats("nemo");

  double q1, q2, q3, q4;
  double x1, x2, x3, x4, y1, y2, y3, y4,  z1, z2, z3, z4;
  double eng1, eng2, eng3, eng4;
  double tx1, tx2, ty1, ty2, tx3, ty3, tx4, ty4;
 

  tree->SetBranchAddress("Charge", &q1);
  tree->SetBranchAddress("StartPosX", &x1);
  tree->SetBranchAddress("StartPosY", &y1);
  tree->SetBranchAddress("StartPosZ", &z1);
  tree->SetBranchAddress("TarX", &tx1);
  tree->SetBranchAddress("TarY", &ty1);
  tree->SetBranchAddress("Energy", &eng1);
  tree1->SetBranchAddress("Charge", &q2);
  tree1->SetBranchAddress("StartPosX", &x2);
  tree1->SetBranchAddress("StartPosY", &y2);
  tree1->SetBranchAddress("StartPosZ", &z2);
  tree1->SetBranchAddress("Energy", &eng2);
  tree->SetBranchAddress("TarX", &tx2);
  tree->SetBranchAddress("TarY", &ty2);
  tree2->SetBranchAddress("Charge", &q3);
  tree2->SetBranchAddress("StartPosX", &x3);
  tree2->SetBranchAddress("StartPosY", &y3);
  tree2->SetBranchAddress("StartPosZ", &z3);
  tree2->SetBranchAddress("Energy", &eng3);
  tree->SetBranchAddress("TarX", &tx3);
  tree->SetBranchAddress("TarY", &ty3);
  tree3->SetBranchAddress("Charge", &q4);
  tree3->SetBranchAddress("StartPosX", &x4);
  tree3->SetBranchAddress("StartPosY", &y4);
  tree3->SetBranchAddress("StartPosZ", &z4);
  tree3->SetBranchAddress("Energy", &eng4);
  tree->SetBranchAddress("TarX", &tx4);
  tree->SetBranchAddress("TarY", &ty4);

  int chargeCount=0;
  int chargeCountE=0;
  int gammaCount=0;
  int gammaCountE=0;

  for (int k=0; k<nEvents; k++)
    {
      tree->GetEntry(k);
      if (eng1>5){
      if (pow(q1, 2)>.5) {
      charge->Fill(q1, .0038);
      Zpos1->Fill(z1/1000+5, .0038);
      xpos->Fill(x1/1000, .0038);
      ypos->Fill(y1/1000, .0038);
      double R = pow(pow(y1/1000, 2)+pow(x1/1000, 2), .5);
      dist->Fill(z1/1000+5, R, .0038);
      eng->Fill(z1/1000+5, eng1, .0038);
      tar->Fill(tx1/1000, ty1/1000, .0038);
      if(z1>-4990){
	cout << z1 << endl;
      engDist->Fill(eng1, .0038);
      engDist2->Fill(eng1, .0038);
      chargeCount++;}
      }
      gammaCount++;}
    }

  cout << "2 Gamma charged particles: " << chargeCount << endl;
  cout << "2 Gamma photons: " <<gammaCount << endl;
  chargeCount=0;
  gammaCount=0;

  //Put a cut after the target, and after the calorimeter not count
  
  for (int k=0; k<nEvents1; k++)
    {
      tree1->GetEntry(k);
      if (eng2>5){
      if (pow(q2, 2)>.5) {
      charge->Fill(q2, .001);
      Zpos2->Fill(z2/1000, .001);
      xpos->Fill(x2/1000, .001);
      ypos->Fill(y2/1000, .001);
      double R = pow(pow(y2/1000, 2)+pow(x2/1000, 2), .5);
      dist->Fill(z2/1000+5, R, .001);
      eng->Fill(z2/1000+5, eng2, .001);
      tar->Fill(tx2/1000, ty2/1000, .001);
      if (z2>-4990) {
      engDist->Fill(eng2, .001);
      engDist2->Fill(eng2, .001);
      chargeCount++;}
    }
      gammaCount++;
      }
    }

  cout << "3 Gamma charged particles: " << chargeCount << endl;
  cout << "3 Gamma photons: " << gammaCount << endl;
  chargeCount=0;
  gammaCount=0;

  for (int k=0; k<nEvents2; k++)
    {
      tree2->GetEntry(k);
      if (eng3>5){
      if (pow(q3, 2)>.5) {
      charge->Fill(q3, .214);
      Zpos3->Fill(z3/1000+5, .214);
      xpos->Fill(x3/1000., .214);
      ypos->Fill(y3/1000., .214);
      double R = pow(pow(y3/1000, 2)+pow(x3/1000, 2), .5);
      dist->Fill(z3/1000+5, R, .214);
      eng->Fill(z3/1000+5, eng3, .214);
      tar->Fill(tx3/1000, ty3/1000, .214);
      if (z3>-4900){
      engDist->Fill(eng3, .214);
      engDist2->Fill(eng3, .214);
      chargeCount++;
      }
    }
      gammaCount++;
      }
    }

  cout << "Radiative Bhabha charged particles: " << chargeCount << endl;
  cout << "Gamma count is: " << gammaCount << endl;
  chargeCount=0;
  gammaCount=0;

  for (int k=0; k<nEvents3; k++)
    {
      tree3->GetEntry(k);
      if (eng4>5){
      if (pow(q4, 2)>.5) {
      charge->Fill(q4, .781);
      Zpos4->Fill(z4/1000.+5, .781);
      xpos->Fill(x4/1000, .781);
      ypos->Fill(y4/1000, .781);
      double R = pow(pow(y4/1000, 2)+pow(x4/1000, 2), .5);
      dist->Fill(z4/1000+5, R, .781);
      eng->Fill(z4/1000+5, eng4, .781);
      tar->Fill(tx4/1000, ty4/1000, 7.81);
      if(z4>-4990){
      engDist->Fill(eng4, .781);
      engDist2->Fill(eng4, .781);
      chargeCount++;}
      }
      gammaCount++;
      }
    }

  cout << "Bhabha charged particles: " << chargeCount << endl;
  cout << "Bhabha gamma count: " << gammaCount << endl;
  chargeCount=0;
  gammaCount=0;


  Zpos1->SetFillColor(kBlue);
  Zpos1->SetFillStyle(3001);
  Zpos2->SetFillColor(kRed); 
  Zpos2->SetFillStyle(3001);
  Zpos3->SetFillColor(kGreen);
  Zpos3->SetFillStyle(3001);
  Zpos4->SetFillColor(kMagenta);
  Zpos4->SetFillStyle(3001);

  THStack *hs = new THStack("hs", "Z Position of Origin for Various Backgrounds");
 
  Zpos3->GetYaxis()->SetRange(0, 300);
  Zpos2->GetYaxis()->SetRange(0, 300);
  Zpos1->GetYaxis()->SetRange(0, 300);
  Zpos4->GetYaxis()->SetRange(0, 300);
  hs->Add(Zpos2);
  hs->Add(Zpos1);
  hs->Add(Zpos4);
  hs->Add(Zpos3);
			  

  TPad* p;
  TCanvas* canvas = new TCanvas("canvas", "canvas", 700,700);
  canvas->Divide(1, 3);
    /*
  canvas->cd(1);
  eng->Draw("BOX");*/
  /*
  p=(TPad*)canvas->cd(2);
  p->SetLogy();
  p->SetGrid();
  hs->Draw();*/

  canvas->cd(1);
  engDist->Draw();

  canvas->cd(2);
  engDist2->Draw();

  /*
  p=(TPad*)canvas->cd(3);
  p->SetLogy();
  p->SetGrid();
  xpos->SetFillColor(kBlue);
  xpos->SetFillStyle(3001);
  xpos->Draw();

  p=(TPad*)canvas->cd(4);
  p->SetLogy();
  p->SetGrid();
  ypos->SetFillColor(kBlue);
  ypos->SetFillStyle(3001);
  ypos->Draw();
  */

    canvas->cd(3);
  dist->Draw("BOX");

  /*
  canvas->cd(6);
  tar->Draw("");


  TCanvas* bgd = new TCanvas("bgd", "canvas", 700, 700);
  canvas->Divide(4, 1);
*/
  
  
  
  


  cout << "Total particles: " <<  nEvents*.0038+nEvents1*.001+nEvents2*.214+nEvents3*.781 << endl;
}
