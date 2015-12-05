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


//1 background at a time

void scdbgd(int choose){ //0 for 2gamma, 1 for 3gamma, 2 for bhabha, 3 for radBh
  string background = "";
  if (choose < 1)
    background = "2gamma5k.root";
  else if (choose <2)
    background = "3gamma5k.root";
  else if (choose < 3)
    background = "bhabha5k.root";
  else 
    background = "radBhab5k.root";
  
  TFile* file = new TFile("radBhabha5k.root");

  TTree* tree = (TTree *)file->Get("Check");

  int nEvents= tree->GetEntries();
  
  TH1D* charge = new TH1D("charge", "Charge of Particles", 5, -2.5, 2.5);
  TH1D* zpos = new TH1D("zpos", "Z Position of Origin", 110, -1, 10);
  TH1D* zposQ = new TH1D("zposQ", "Z Position of Origin (charged)", 11, -1, 10);
  TH1D* xpos = new TH1D("xpos", "X Position of Origin", 200, -2, 2);
  TH1D* xposQ = new TH1D("xposQ", "X Position of Origin (charged)", 200, -2, 2);
  TH1D* ypos = new TH1D("ypos", "Y Position of Origin", 200, -2, 2);
  TH1D* yposQ = new TH1D("yposQ", "Y Position of Origin (Charged)", 200, -2, 2);
  TH2D* dist = new TH2D("dist", "R v. Z Position of Origin", 110, -1, 10, 100, 0, 1);
  TH2D* distQ = new TH2D("distQ", "R v. Z Position of Origin (Charged)", 110, 
			 -1, 10, 100, 0, 1);

  TH2D* parEng = new TH2D("parEng", "Parent Energy v. Secondary Energy", 250, 0, 500, 250, 0, 500);
  TH2D* parEngQ = new TH2D("parEngQ", "Parent Energy v. Secondary Energy (charged)", 250, 0, 500, 250, 0, 500);
  TH2D* eng = new TH2D("eng", "Energy distribution in Z", 11, -1, 10, 500, 0, 1000);
  TH2D* engQ = new TH2D("engQ", "Energy distribution in Z (Charged)", 11, -1, 10, 500, 0, 1000);
  TH1D* engDist = new TH1D("engDist", "Energy Distribution Of Secondary Particles", 100, 0, 500);
  TH1D* engDistQ = new TH1D("engDistQ", "Energy Distribution Of Charged Secondary Particles", 100, 0, 500);
  TH2D* tar = new TH2D("tar", "Position in Target", 100, -1, 1, 100, -1, 1);
  TH2D* tarQ = new TH2D("tarQ", "Position (charged)in Target", 100, -1, 1, 100, -1, 1);
  TH2D* origin = new TH2D("origin", "Position of Origin", 100, -1, 1, 100, -1, 1);
  TH2D* originQ = new TH2D("originQ", "Position of Origin (Charged)", 100, -1, 1, 100, -1, 1);
  TH2D* originW = new TH2D("originW", "Position of Origin (weighted by z)", 100, -1, 1, 100, -1, 1);
  TH2D* originWQ = new TH2D("originWQ", "Position of Origion (Charged, weighted by z)", 100, -1, 1, 100, -1, 1);

 double q1;
 double x1, y1, z1;
 double eng1 = 0.;
 double tx1, ty1;
 double parentEng;
 

 tree->SetBranchAddress("Charge", &q1);
 tree->SetBranchAddress("StartPosX", &x1);
 tree->SetBranchAddress("StartPosY", &y1);
 tree->SetBranchAddress("StartPosZ", &z1);
 tree->SetBranchAddress("TarX", &tx1);
 tree->SetBranchAddress("TarY", &ty1);
 tree->SetBranchAddress("Energy", &eng1);
 tree->SetBranchAddress("ParentEnergy", &parentEng);

 
 int chargeCount=0;
 int chargeCountE=0;
 int gammaCount=0;
 int gammaCountE=0;
 

 for (int k=0; k<nEvents; k++)
   {
     tree->GetEntry(k);
     //Check for energy threshold
     if (eng1>5){
       //Charged Particles
       if (pow(q1, 2)>.5) {
	 //not from target
	 if(z1>-4993 && z1<5007){
	   charge->Fill(q1);
	   zposQ->Fill(z1/1000+5);
	   xposQ->Fill(x1/1000);
	   yposQ->Fill(y1/1000);
	   originQ->Fill(x1/1000, y1/1000);
	   double R = pow(pow(y1/1000, 2)+pow(x1/1000, 2), .5);
	   distQ->Fill(z1/1000+5, R);
	   engQ->Fill(z1/1000+5, eng1);
	   engDistQ->Fill(eng1);
	   parEngQ->Fill(parentEng, eng1);
	   tarQ->Fill(tx1/1000, ty1/1000);
	   originWQ->Fill(x1/1000, y1/1000, z1/1000+5);
	   chargeCount++;}
       }
       else
       //Uncharged particles
	  if(z1>-4993 && z1<5007){
	   charge->Fill(q1);
	   zpos->Fill(z1/1000+5);
	   xpos->Fill(x1/1000);
	   ypos->Fill(y1/1000);
	   origin->Fill(x1/1000, y1/1000);
	   double R = pow(pow(y1/1000, 2)+pow(x1/1000, 2), .5);
	   dist->Fill(z1/1000+5, R);
	   engDist->Fill(eng1);
	   eng->Fill(z1/1000+5, eng1);
	   parEng->Fill(parentEng, eng1);
	   tar->Fill(tx1/1000, ty1/1000);
	   originW->Fill(x1/1000, y1/1000, z1/1000+5);
	   gammaCount++;}
   }
   }
 
 TPad* p;
 TCanvas* canvas = new TCanvas("canvas", "Charged Particles", 700,700);
 TPad* p1; 
 TCanvas* canvas2 = new TCanvas("canvas2", "Uncharged Particles", 700, 700);

  canvas->Divide(2, 3);
  canvas2->Divide(2, 3);
  
  //origin r v. z
  canvas->cd(1);
  distQ->Draw();
  canvas2->cd(1);
  dist->Draw();

  //origin in x v. y
  canvas->cd(2); 
  originQ->Draw();
  canvas2->cd(2);
  origin->Draw();


  //energy distribution
  canvas->cd(3); 
  engDistQ->Draw();
  canvas2->cd(3);
  engDist->Draw();
  
  //position in target
  canvas->cd(4);
  tarQ->Draw();
  canvas2->cd(4);
  tar->Draw();

  //Scatter plot of parent energy v. secondary energy
  canvas->cd(5); 
  parEngQ->Draw();
  canvas2->cd(5);
  parEng->Draw();

  //Scatter plot of x, y position weighted by z-position
  canvas->cd(6);
  originWQ->Draw("COLZ");
  canvas2->cd(6);
  originW->Draw("COLZ");
  

  

  
  //eng->Draw("BOX");
  /*
  p=(TPad*)canvas->cd(2);
  p->SetLogy();
  p->SetGrid();
  zpos->Draw();

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

  //canvas->cd(3);
  // dist->Draw("BOX");

  /*
  canvas->cd(6);
  tar->Draw("");


  TCanvas* bgd = new TCanvas("bgd", "canvas", 700, 700);
  canvas->Divide(4, 1);
*/
  
  
  
  

   }
