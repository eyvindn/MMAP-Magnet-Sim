
/*
 * Dark Photon Detector Construction
 * !!!History:
 *    CJC 6.15.14 created
 *    CJC 6.18.14 changed calorimeter into tube
 *    CJC 7.08.14 completely new set up
 *
 * file: DetectorConstruction.cc
 */

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "TestSD.hh"
#include "OmniSD.hh"
#include "G4SDManager.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SDManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4UnionSolid.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "SquareParameterisation.hh"
#include "G4PVParameterised.hh"
#include "G4SubtractionSolid.hh"

#include "AdjustmentField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

static const G4double inch = 2.54*cm;
static const G4double ft = 12*inch;



//G4ThreadLocal
//G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = 0;

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(),
fLogicCalor(NULL), //logical volume for calorimeter
    fLogicTarget(NULL),
  fTargetMaterial(NULL), //material of target
  fCalorMaterial(NULL), //material of calorimeter
    fWallMaterial(NULL),
    fBeamDumpMaterial(NULL),
    fCLEOMaterial(NULL),
    fWorldMaterial(NULL),
  fStepLimit(NULL), 
    fCheckOverlaps(false),
    fLiningMaterial(NULL), 
    fVacuumMaterial(NULL),
    fBeamLineMaterial(NULL),
    fScintillatorMaterial(NULL),
    CLEObool(true)
    //  fCenterToFront(0.)
{
 fMessenger = new DetectorMessenger(this);
 fLogicCalor = new G4LogicalVolume*[1225];
 CLEObool = true;
}

DetectorConstruction::~DetectorConstruction()
{
  delete fStepLimit;
  delete fMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  //Define the materials
  DefineMaterials();

  //Define volumes
  return DefineVolumes();
}

/*
 * Method used to set the materials of the experiment
 * Liquid hydrogen for the target has to be defined
 * as well as Cesium Iodide for the crystals. Other materials
 * can be found in the NIST directory
 * CJC 6.18.14
 * vacuum for tunnel 
 */

void DetectorConstruction::DefineMaterials()
{

  G4NistManager* nistManager = G4NistManager::Instance();

 
  fWorldMaterial = nistManager->FindOrBuildMaterial("G4_AIR");
 
  G4double z, a, density, pressure, temperature;
  G4String name, symbol;
  G4int nComp, nAtom;

  fScintillatorMaterial = nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  fWallMaterial = nistManager->FindOrBuildMaterial("G4_CONCRETE");
  
  density = 2.700*g/cm3;
  a = 26.98*g/mole;
  fBeamDumpMaterial = new G4Material(name="Aluminum", z=13., a, density);
  fBeamLineMaterial = new G4Material(name="Aluminum", z=13., a, density);


  //Vacuum for chamber
  density = universe_mean_density;
  pressure = 1.e-19*pascal;
  temperature = 0.1*kelvin;
  G4Material* vacuum = new G4Material(name="Vacuum", z=1., a=1.01*g/mole, 
					  density, kStateGas, temperature, 
					  pressure);

  fVacuumMaterial = vacuum;

  //Helium gas for testing
  a = 4.00*g/mole;
  G4Element* ele_He = new G4Element( name = "Helium", symbol = "He", z=2, a);

  density = .000164*g/cm3; 
  G4Material* lhe = new G4Material(name = "Helium Gas", density, nComp =1);
  
  lhe->AddElement(ele_He, nAtom=1);
  fSpaceMaterial = lhe;




  //Liquid hydrogen for the target
  a = 1.01*g/mole;
  G4Element* ele_H = new G4Element(
				   name="Hydrogen", //name
				   symbol="H", //symbol
				   z=1.,//atomic number
				   a); // mass / mole

  density = .07085*g/cm3; //density of Liquid Hydrogen
  density*=100; 
  G4Material* lh2 = new G4Material(name = "Liquid Hydrogen", density, nComp =1);

  lh2->AddElement(ele_H, nAtom=2);
  fTargetMaterial = lh2; // Target material is now liquid hydrogen

  //Cesium Iodide for the crystals

  a = 132.9*g/mole;
  G4Element* ele_Cs = new G4Element(name = "Cesium", symbol = "Cs", z = 55., a);

  a =126.9*g/mole;
  G4Element* ele_I = new G4Element(name="Iodide", symbol = "I", z = 53., a);

  density = 4.51*g/cm3;
  G4Material* CsI = new G4Material(name="Cesium Iodide", density, nComp=2);
  CsI->AddElement(ele_Cs, 1);
  CsI->AddElement(ele_I, 1); 

  fCalorMaterial = CsI;

  density = 2.700*g/cm3;
  a = 26.98*g/mole;
  G4Material* Al = new G4Material(name="Aluminum", z=13., a, density);
  fCLEOMaterial = Al;
  //Lead for lining calorimeter
  a = 207.20*g/mole;
  G4Element* elPb = new G4Element(name="lead",symbol="Pb", z=82., a);

  density = 11.34*g/cm3;
  G4Material* Pb = new G4Material(name="Lead", density, nComp=1);
  Pb->AddElement(elPb, 1);
  fLiningMaterial = Pb;

  //Print Materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;


}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{


  //Sizes and lengths

  G4double targetLength = 10.0*cm; // depth of target
  G4double targetFace = 10.0*cm; //lengths of sides of face of target

  G4double crystalLength = 2.54*12.0*cm; 

  G4double calorSpacing = 10.*m; //distance from target to calorimeter
  G4double targetPos = -(.5*calorSpacing); //position of Z coordinate of target
  G4double calorDist = .5*calorSpacing + .5*targetLength;


  G4double worldLength = 150.*ft;

  //Geometry upgrades

  //wall
  G4double wallTotarget = 6.8*ft;
  G4double wallL = 4.*ft;
  G4double wallW = 2.*ft;
  G4double wallH = 8.*ft;

  //beamline
  G4double blL = (worldLength/2+targetPos-targetLength/2)/2;
  G4double blL2 = 2.8*ft+9.9*m;
  G4double blP2 = .05*m;

  //beamdump
  G4double beamdumpL = 8.*ft;
  G4double beamdumpW = 8.*inch;

  //quadmag
  G4double quadL = 2*ft;
  G4double quadW = 1*ft;

  //dimag
  G4double dipL = 11.2*ft;
  G4double dipW = 1.6*ft;

  //floor and ceiling
  G4double floorW = 20.*ft;
  G4double ceilingW = 20.*ft;
  G4double worldW = 30.*ft;

  //charged particle veto
  G4double scintH = 185.*cm;
  G4double scintW = 1.*inch;
  G4double xtalFace = 5.*cm;

  

 


  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);


  G4cout << "Computed tolerance = "
	 << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
	 << " mm" << G4endl;


  //World

  /* !!!! DECIDE WHAT SPACE MATERIAL YOU ARE TESTING !!! */
  fSpaceMaterial = fVacuumMaterial; 
  //fWorldMaterial = fVacuumMaterial;
  //fSpaceMaterial = fWorldMaterial;

G4Box* worldS = 
  new G4Box("world", worldW/2, (wallH+floorW+ceilingW)/2, worldLength/2);
G4LogicalVolume* worldLV
  = new G4LogicalVolume(
			worldS, // solid
			fWorldMaterial,  // material
			"World"); //logical volume's name

// Place the world

G4VPhysicalVolume* worldPV
  = new G4PVPlacement(
		      0, //no rotation
		      G4ThreeVector(), // at origin
		      worldLV, //logical volume
		      "World", // name
		      0, // no mother volume
		      false, //no booleans
		      0, //copy number
		      fCheckOverlaps); // true
		   
	      
//!!!
//Target



 G4ThreeVector positionTarget = G4ThreeVector(0, 0, targetPos); 

G4Box* targetS = 
  new G4Box("target", targetFace/2, targetFace/2, targetLength/2);

 fLogicTarget = 
   new G4LogicalVolume(targetS, fTargetMaterial, "Target", 0,0,0);
 
//
// new G4PVPlacement(0, // no rotation
//		   positionTarget, // at (x,y,z)
//		   fLogicTarget, // logical volume
//		   "Target", //name
//		   worldLV, //mother volume
//		   false, //no booleans
//		   0, // copy number
//		   fCheckOverlaps); //true



 

 G4cout << "Target is " << targetLength/cm << " cm of " <<
   fTargetMaterial->GetName() << G4endl;



 //!!!
 //Calorimeter 

 G4int crysLength = 5.*cm;

G4VSolid* boxS =
  new G4Box("boxS", crysLength/2, crysLength/2, crystalLength/2);

 G4double xPos[1225] = {};
 G4double yPos[1225] = {};

 G4ThreeVector position = G4ThreeVector(); 

 for (G4int i=0; i<1225; i++)
   {
     xPos[i] = (i%35-17)*crysLength; 
     yPos[i] = (i/35-17)*crysLength;

     position = G4ThreeVector(xPos[i], yPos[i], calorDist+.5*crystalLength);
     fLogicCalor[i] = new G4LogicalVolume(boxS,
					  fCalorMaterial,
					  "CrystalLV", 
					  0, 0, 0);
    
     if ((i>10 && i<24) || (i>43 && i < 61) || (i>77 && i<97) 
	 || (i>110 && i<134) || (i>144 && i<170) || (i>178 && i<206)
	 || (i>213 && i<241) || (i>247 && i<277) || (i>281 && i<313)
	 || (i>316 && i<348) || (i>350 && i<384) || (i>385 && i<400)
	 || (i>404 && i<419)
	 || (i>440 && i<468) || (i>419 && i<434) || (i>440 && i<468)
	 || (i>476 && i<502) || (i>512 && i<537) || (i>547 && i<571)
	 || (i>583 && i<606) || (i>618 && i<641) || (i>653 && i<677) 
	 || (i>687 && i<712) || (i>722 && i<748) || (i>756 && i<784)
	 || (i>790 && i<805) || (i>805 && i<820) || (i>824 && i<839)
	 || (i>840 && i<874) || (i>876 && i<908) || (i>911 && i<943) 
	 || (i>947 && i<977) || (i>983 && i<1011) || (i>1018 && i<1046)
	 || (i>1054 && i<1080) || (i>1090 && i<1114) || (i>1127 && i<1147)
	 || (i>1163 && i<1181) || (i>1200 && i<1214))
       {
	 new G4PVPlacement(0, 
			   position, 
			   fLogicCalor[i], 
			   "ClusterPV", 
			   worldLV, 
			   false, 
			   i, 
			   fCheckOverlaps);
       		
     }			  
   }

 G4VSolid* ceiling = new G4Box("ceiling", worldW/2, ceilingW/2, worldLength/2);
 G4VSolid* floor = new G4Box("floor", worldW/2, floorW/2, worldLength/2);

 G4LogicalVolume * ceilingLV =
   new G4LogicalVolume(ceiling, fWallMaterial, "ceilingLV");
 G4LogicalVolume * floorLV = 
   new G4LogicalVolume(floor, fWallMaterial, "floorLV");


     //Wall blocks
     G4VSolid* wall = new G4Box("wallBlock", wallW/2, wallH/2, wallL/2);
     G4LogicalVolume* wallLV = 
       new G4LogicalVolume(wall, fWallMaterial, "WallLV");

 //Scintillators
 
 G4VSolid * scint35 = new G4Box("Scint", xtalFace*35/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint35LV = 
   new G4LogicalVolume(scint35, 
		       fScintillatorMaterial, 
		       "scintillator35LV");

 G4VSolid * scint33 = new G4Box("Scint", xtalFace*33/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint33LV = 
   new G4LogicalVolume(scint33, 
		       fScintillatorMaterial, 
		       "scintillator33LV");

 G4VSolid * scint31 = new G4Box("Scint", xtalFace*31/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint31LV = 
   new G4LogicalVolume(scint31, 
		       fScintillatorMaterial, 
		       "scintillator31LV");

 G4VSolid * scint29 = new G4Box("Scint", xtalFace*29/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint29LV = 
   new G4LogicalVolume(scint29, 
		       fScintillatorMaterial, 
		       "scintillator29LV");

 G4VSolid * scint27 = new G4Box("Scint", xtalFace*27/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint27LV = 
   new G4LogicalVolume(scint27, 
		       fScintillatorMaterial, 
		       "scintillator27LV");

 G4VSolid * scint25 = new G4Box("Scint", xtalFace*25/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint25LV = 
   new G4LogicalVolume(scint25, 
		       fScintillatorMaterial, 
		       "scintillator25LV");

 G4VSolid * scint23 = new G4Box("Scint", xtalFace*23/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint23LV = 
   new G4LogicalVolume(scint23, 
		       fScintillatorMaterial, 
		       "scintillator23LV");

 G4VSolid * scint21 = new G4Box("Scint", xtalFace*21/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint21LV = 
   new G4LogicalVolume(scint21, 
		       fScintillatorMaterial, 
		       "scintillator21LV");

 G4VSolid * scint19 = new G4Box("Scint", xtalFace*19/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint19LV = 
   new G4LogicalVolume(scint19, 
		       fScintillatorMaterial, 
		       "scintillator19LV");

 G4VSolid * scint17 = new G4Box("Scint", xtalFace*17/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint17LV = 
   new G4LogicalVolume(scint17, 
		       fScintillatorMaterial, 
		       "scintillator17LV");

 G4VSolid * scint13 = new G4Box("Scint", xtalFace*13/2, xtalFace/2, scintW/2);

 G4LogicalVolume * scint13LV = 
   new G4LogicalVolume(scint13, 
		       fScintillatorMaterial, 
		       "scintillator13LV");






//beamline

 G4VSolid * beamline = 
   new G4Tubs("beamline", 2.*inch, 2.*inch+3.*mm, 
	     blL, 0.*deg, 360.*deg);

 G4LogicalVolume* beamlineLV = 
   new G4LogicalVolume(
		       beamline, 
		       fBeamLineMaterial, 
		       "beamlineLV");

 //beamline3s

G4VSolid * beamline3 = 
  new G4Tubs("beamline3", 2.*inch, 2.*inch+3.*mm, blL2/2, 0.*deg, 360.*deg);

G4LogicalVolume* beamline3LV = 
  new G4LogicalVolume(
		      beamline3, 
		      fBeamLineMaterial, 
		      "beamline3LV");

//beamVoid3 

 G4VSolid * beamVoid3 = new G4Tubs("beamvoid3", 0.*inch, 2.*inch, 
				   blL2/2, 0.*deg, 360.*deg);
 G4LogicalVolume * beamVoid3LV = 
   new G4LogicalVolume(
		       beamVoid3, 
		       fVacuumMaterial, 
		       "beamVoidLV3");
		       
	     

//beamVoid

 G4VSolid * beamVoid = new G4Tubs("beamvoid", 0.*inch, 2.*inch,
				  blL, 0.*deg, 360.*deg); 
 G4LogicalVolume* beamVoidLV = 
   new G4LogicalVolume(
		       beamVoid, 
		       fVacuumMaterial, 
		       "beamVoidLV");

 //beamline2

 G4VSolid * beamline2 =
   new G4Cons("beamline2", 2.*inch+5.*mm, 2.*inch+8.*mm, 95.*cm, 95.*cm+3.*mm,
	      9.9/2.*m, 0.*deg, 360.*deg);

 G4LogicalVolume * beamlineLV2 = 
   new G4LogicalVolume(
		       beamline2, 
		       fBeamLineMaterial, 
		       "beamlineLV2");

 //beamvoid2

 G4VSolid * beamVoid2 = new G4Cons("beamvoid2", 0.*mm, 2.*inch+5.*mm,
				   0.*mm, 95.*cm,  9.9/2.*m, 
				   0.*deg, 360.*deg);

 G4LogicalVolume * beamVoid2LV = 
   new G4LogicalVolume(
		       beamVoid2, 
		       fSpaceMaterial, 
		       "beamvoid2LV");
 //pipe to dump
 G4VSolid * pipe = new G4Tubs("pipe", 2.*inch, 2.*inch+3.*mm, 1.4*ft, 
			      0.*deg, 360.*deg); 
 G4LogicalVolume * pipeLV = new G4LogicalVolume(pipe, fBeamLineMaterial, "pipeLV"); 

 G4VSolid * pipeVoid = new G4Tubs("pipeVoid", 0.*inch, 2.*inch, 1.4*ft, 
				  0.*deg, 360.*deg);
G4LogicalVolume * pipeVoidLV = 
  new G4LogicalVolume(pipeVoid, fVacuumMaterial, "pipeVoidLV"); 
			      

 //Beamdump
 
 G4VSolid* beamdump = 
   new G4Box("beamdump", beamdumpW/2, beamdumpW/2, beamdumpL/2);
     G4LogicalVolume* beamdumpLV = 
       new G4LogicalVolume(beamdump, fBeamDumpMaterial, "BeamdumpLV");
     

 //CLEO stuff
 if (CLEObool)
   {
 //Floor and Ceiling

     /* 
   new G4PVPlacement(0, 
		    G4ThreeVector(0., wallH/2+ceilingW/2, 0.),
		    ceilingLV, 
		    "Ceiling", 
		     worldLV, 
		     false, 
		     0, 
		     fCheckOverlaps);
     */

 
 new G4PVPlacement(0,
		   G4ThreeVector(0., -wallH/2-floorW/2, 0.),
		   floorLV,
		   "Floor", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 //walls
     
     for(int w=0; w<3; w++)
       {
       new G4PVPlacement(0, 
			 G4ThreeVector(-2.5*ft-.75*ft*w,0., targetPos+wallTotarget+4*ft*w), 
			 wallLV, 
			 "wallBlock",
			 worldLV, 
			 false, 
			 0, 
			 fCheckOverlaps);
       }

     for(int w=0; w<7; w++)
       {
       new G4PVPlacement(0, 
			 G4ThreeVector(-5*ft,0., targetPos+wallTotarget+4*ft*(w+3)), 
			 wallLV, 
			 "wallBlock",
			 worldLV, 
			 false, 
			 0, 
			 fCheckOverlaps);
       }

 for(int w=0; w<3; w++)
       {
       new G4PVPlacement(0, 
			 G4ThreeVector(3*ft,0., targetPos+wallTotarget-2*ft-4*ft*w), 
			 wallLV, 
			 "wallBlock",
			 worldLV, 
			 false, 
			 0, 
			 fCheckOverlaps);
       }
 for(int w=0; w<4; w++)
       {
       new G4PVPlacement(0, 
			 G4ThreeVector(3*ft-.25*ft*w,0., targetPos+wallTotarget-10*ft-4*ft*w), 
			 wallLV, 
			 "wallBlock",
			 worldLV, 
			 false, 
			 0, 
			 fCheckOverlaps);
       }
    

 
  //Scintillator
 /*
 for (int q=0; q<18; q++)
   {
     if (q==0)
       {
 	 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., calorDist-crystalLength/2-1*inch), 
		   scint35LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
     else if (q<6)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint35LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint35LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
     else if(q<8)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint33LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint33LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
    else if(q<10)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint31LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint31LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }

         else if(q<11)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint29LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint27LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
    else if(q<12)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint27LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint27LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
         else if(q<14)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint25LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint25LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
         else if(q<15)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint23LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint23LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
         else if(q<16)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint19LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint19LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
         else if(q<17)
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint17LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint17LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }
     else
       {
	 new G4PVPlacement(0, 
		   G4ThreeVector(0., q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint13LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

	 new G4PVPlacement(0, 
		   G4ThreeVector(0., -q*xtalFace, calorDist-crystalLength/2-1*inch), 
		   scint13LV, 
		   "ParticleVeto", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
       }




   }
 */
 

 
     //beamline


     new G4PVPlacement(
		   0, 
		   G4ThreeVector(0., 0.,  -blL-5.*m-5.*cm), 
		   beamlineLV, 
		   "Beamline", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
     //beamVoid
     
     new G4PVPlacement(
		   0, 
		   G4ThreeVector(0., 0., -blL-5.*m-5.*cm),
		   beamVoidLV, 
		   "BeamVoid", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
     
 //beamline2
     
 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., 0.), 
		   beamlineLV2, 
		   "Beamline2", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
     
 
 
 //beamvoid2

 
 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., 0.), 
		   beamVoid2LV, 
		   "BeamVoid2", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
 //beam pipe
 //can't place with beamLine3

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., calorDist+crystalLength/2+1.38*ft), 
		   pipeLV, 
		   "PipeLV", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 new G4PVPlacement(0, G4ThreeVector(0., 0., calorDist+crystalLength/2+1.38*ft), 
		   pipeVoidLV, 
		   "PipeVoidLV", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps); 

 
 //
 /*
 //beamline3
 
 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., .05*m+1.4*ft), 
		   beamline3LV, 
		   "Beamline3", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
 

 //beamvoid3

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., .05*m+1.4*ft),
		   beamVoid3LV, 
		   "Beamvoid3", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
    
 
 */

 //beamdump

     new G4PVPlacement(0, 
			 G4ThreeVector(0.,0., calorDist+crystalLength/2+6.8*ft), 
			 beamdumpLV, 
			 "beamdump",
			 worldLV, 
			 false, 
			 0, 
			 fCheckOverlaps);
 
     

     
   }

 //beam guard
 
 G4VSolid* shield = new G4Tubs("sheild", 12.*cm, 95.*cm, 8.*inch, 
			       0.*deg, 360.*cm);

 G4LogicalVolume* shieldLV = new G4LogicalVolume(shield, fLiningMaterial, 
						 "sheildlv");

 new G4PVPlacement(0, 
		G4ThreeVector(0., 0., calorDist+crystalLength+8.*inch+2.*cm),
		   shieldLV, 
		   "Shield", 
		   worldLV,
		   false, 
		   0,
		   fCheckOverlaps);
 

		   



 //Annulus lining inner
 G4VSolid* liningI = new G4Tubs("liningI", 27.7*cm, 28.2*cm, crystalLength/2, 
				0.*deg, 360.*deg);
 G4LogicalVolume* liningILV = new G4LogicalVolume(liningI, fLiningMaterial, 
						  "LiningILV");
 
 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., calorDist+crystalLength/2), 
		   liningILV, 
		   "LiningI",
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
 

 G4VSolid* liningO = new G4Tubs("liningO", 94.2*cm, 94.7*cm, crystalLength/2, 
				0.*deg, 360.*deg);
 G4LogicalVolume* liningOLV = new G4LogicalVolume(liningO, fLiningMaterial, 
						  "LiningOLV");
 
 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., calorDist+crystalLength/2), 
		   liningOLV, 
		   "LiningO",
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

		   
 

 //Omni detector
 G4double crysConst = 5.*cm;
 G4double startRad = 7.5; 

     G4VSolid* omni = 
       new G4Tubs("omniS", .3492*m, .8749*m,
		  .01*cm, 0.*deg, 360.*deg);

     G4LogicalVolume* omniLV = 
       new G4LogicalVolume(omni, fVacuumMaterial, "OmniLV");
     
      new G4PVPlacement(0, 
		   G4ThreeVector(0., 0.,calorDist-.5*mm), 
		   omniLV, 
		   "Omni", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);



									  
    
    


     

     //Quad Magnet
     
     G4VSolid* quadmag = 
       new G4Box("quadmag", quadW/2, quadW/2, quadL/2);
     G4LogicalVolume* quadmagLV = 
       new G4LogicalVolume(quadmag, fWorldMaterial, "quadmagLV");

     new G4PVPlacement(0, 
		       G4ThreeVector(0., 0., targetPos-4.8*ft),
		       quadmagLV, 
		       "QuadMag",
		       worldLV, 
		       false, 
		       0, 
		       fCheckOverlaps);


     //Dipole Magnet

     G4VSolid* dipmag = 
       new G4Box("dipmag", dipW/2, dipW/2, dipL/2);
     G4LogicalVolume* dipmagLV = 
       new G4LogicalVolume(dipmag, fWorldMaterial, "dipmagLV");
     /*
     new G4PVPlacement(0,
		       G4ThreeVector(0., 0., targetPos-12.4*ft),
		       dipmagLV, 
		       "DipMag",
		       worldLV, 
		       false, 
		       0, 
		       fCheckOverlaps);
     
     new G4PVPlacement(0,
		       G4ThreeVector(0., 0., targetPos-24.8*ft),
		       dipmagLV, 
		       "DipMag",
		       worldLV, 
		       false, 
		       0, 
		       fCheckOverlaps);
     */

 


 //Visualization

 G4VisAttributes* color  = new G4VisAttributes(G4Colour(0.9, 0.7, 0.2));

 //worldLV->SetVisAttributes(new G4VisAttributes(G4Colour(1.0,1.0,1.0)));
 fLogicTarget->SetVisAttributes(color);

 //liningHLV->SetVisAttributes(new G4VisAttributes(G4Colour(.7, .7, .7)));
 //liningLV->SetVisAttributes(new G4VisAttributes(G4Colour(.7, .7, .7)));
 
 wallLV->SetVisAttributes(new G4VisAttributes(G4Colour(.4, .4, .4)));
 beamdumpLV->SetVisAttributes(new G4VisAttributes(G4Colour(1, .64, 0.)));
 quadmagLV->SetVisAttributes(new G4VisAttributes(G4Colour(1, 0., .75)));
 dipmagLV->SetVisAttributes(new G4VisAttributes(G4Colour(.20, .80, .20)));

 beamVoidLV->SetVisAttributes(new G4VisAttributes(G4Colour(1., 0., 0.))); 
 beamVoid3LV->SetVisAttributes(new G4VisAttributes(G4Colour(0., 1., 0.)));



 color->SetVisibility(true);

 //Setting user Limits

 G4double maxStep = 1.0*cm;
 fStepLimit = new G4UserLimits(maxStep);



 return worldPV;

   }


void DetectorConstruction::ConstructSDandField()
{

  /*Add Adjustment Magnet Field*/
  AdjustmentField* aField = new AdjustmentField; //"../magMap.table", -5.0, -5.0, -5.0)
  G4FieldManager* fieldMgr
   = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  fieldMgr->SetDetectorField(aField);
  fieldMgr->CreateChordFinder(aField);


  //!!!
  //Create a sensitive detector and put it with logical volumes
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String SDname;


  SDname = "/calorimeterSD";
  TestSD* calorimeterSD =
    new TestSD(SDname, "TestHitsCollection");
  sdMan->AddNewDetector(calorimeterSD);

  SetSensitiveDetector("CrystalLV", calorimeterSD, true); //sets SD to all logical volumes with the name CrystalLV


  SDname = "/omniSD";
  OmniSD* omniSD = 
    new OmniSD(SDname, "OmniHitsCollection");
  sdMan->AddNewDetector(omniSD);
  SetSensitiveDetector("OmniLV", omniSD, true);
  
 
}


void DetectorConstruction::SetTargetMaterial(G4String materialName)
{
  G4NistManager* nistMan = G4NistManager::Instance();

G4Material* pttoMaterial = 
  nistMan->FindOrBuildMaterial(materialName);

 if(fTargetMaterial != pttoMaterial) {
   if ( pttoMaterial) {
     fTargetMaterial = pttoMaterial; 
     if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
     G4cout << "\n-----> The target is made of " << materialName << G4endl;
   }
   else {
     G4cout << "\n --> Warning from SetTargetMaterial: " << 
       materialName << " not found" << G4endl;
   }
 }
}

void DetectorConstruction::SetCalorMaterial(G4String materialName)
{
  G4NistManager* nistMan = G4NistManager::Instance();

G4Material* pttoMaterial = 
  nistMan->FindOrBuildMaterial(materialName);

 if(fCalorMaterial != pttoMaterial) {
   if ( pttoMaterial) {
     fCalorMaterial = pttoMaterial; 
     for (G4int copyNum=0; copyNum<49; copyNum++){
     if (fLogicCalor[copyNum]) fLogicCalor[copyNum]->SetMaterial(fCalorMaterial);
     G4cout << "\n-----> The calorimeter is made of " << materialName << G4endl;
     }
   }
   else {
     G4cout << "\n --> Warning from SetCalorMaterial: " << 
       materialName << " not found" << G4endl;
   }
 }
}


void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
  fCheckOverlaps = checkOverlaps; 
}




