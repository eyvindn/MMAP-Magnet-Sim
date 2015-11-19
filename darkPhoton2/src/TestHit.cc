/*
 * Dark Photon Calor Hit
 *
 *!!!History
 *   CJC 6.17.14 created
 */

#include "TestHit.hh"

#include "G4THitsCollection.hh"
#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"

#include "G4ios.hh"

G4ThreadLocal G4Allocator<TestHit>* TestHitAllocator;

TestHit::TestHit()
  : G4VHit(), 
    fEnergyDep(0.),
    fPos(G4ThreeVector()), 
    fLogV(0), 
    fCellID(-1),
    fXPos(-1),
    fYPos(-1)
{}

TestHit::~TestHit()
{}

TestHit::TestHit(G4int z)
  : G4VHit(), 
    fEnergyDep(0.), 
    fPos(G4ThreeVector()),
    fLogV(0), 
    fCellID(z),
    fXPos(z%35-17), 
    fYPos(z/35-17)
{}

//Create a hit from another hit
TestHit::TestHit(const TestHit& hit)
  : G4VHit()
{ 
  fEnergyDep = hit.fEnergyDep;
  fPos = hit.fPos;
  fLogV = hit.fLogV;
  fCellID = hit.fCellID;
  fXPos = hit.fXPos;
  fYPos = hit.fYPos;
  
}

const TestHit& TestHit::operator=(const TestHit& hit)
{
  fEnergyDep = hit.fEnergyDep;
  fPos = hit.fPos;
  fLogV = hit.fLogV;
  fCellID = hit.fCellID;
  fXPos = hit.fXPos;
  fYPos = hit.fYPos;

  return *this;

}

G4int TestHit::operator==(const TestHit& hit) const
{
  return (this==&hit) ? 1 : 0;
}


void TestHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
    {
      G4Circle circle(fPos);
      circle.SetScreenSize(4.);
      circle.SetFillStyle(G4Circle::filled);
      if (fEnergyDep > .5*MeV)
	{
	  G4Colour colour(1., 0., 0.);
	  G4VisAttributes attribs(colour);
	  circle.SetVisAttributes(attribs);}
      else
	{
	  G4Colour colour(0., 1., 0.);
	  G4VisAttributes attribs(colour);
	  circle.SetVisAttributes(attribs);}
      pVVisManager->Draw(circle);
    }
}

void TestHit::Print()
{
}




