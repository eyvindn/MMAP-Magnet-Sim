
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
#include "G4PropagatorInField.hh"


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
    fMagnetMaterial(NULL),
    CLEObool(false)
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

  fScintillatorMaterial = nistManager->FindOrBuildMaterial("G4_LUCITE"); //G4_PLASTIC_SC_SOMETHING_SORRY_LOOKITUP

  fMagnetMaterial = nistManager->FindOrBuildMaterial("G4_Fe"); //Iron for magnet

  fWallMaterial = nistManager->FindOrBuildMaterial("G4_CONCRETE");
  
  density = 2.700*g/cm3;
  a = 26.98*g/mole;
  fBeamDumpMaterial = new G4Material(name="Aluminum", z=13., a, density);
  fBeamLineMaterial = new G4Material(name="Aluminum", z=13., a, density);
  // fBeamLineMaterial = nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  


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

  //Beryllium for target
  //New Target is going to be 1/2 inch plate of Be 
  //CJC 11/15/15
  a = 9.01218*g/mole;
  G4Element* ele_Be = new G4Element(
				    name="Beryllium", 
				    symbol="Be", 
				    z=4., 
				    a);

  density = 1.85*g/cm3;
  G4Material* solidBe = new G4Material(name="Beryllium", density, nComp=1);
  solidBe->AddElement(ele_Be, nAtom=1);
  fTargetMaterial=solidBe; //Target is made of beryllium
  


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
  //Updated for new vacuum chamber 
  //CJC 11/15/15

  G4double targetLength = .5*2.54*cm; // target is 1/2 inch
  G4double targetFace = 10.0*cm; //lengths of sides of face of target

  G4double crystalLength = 2.54*12.0*cm; // 1 ft long xtals

  G4double calorSpacing = 10.*m; //distance from target to calorimeter
  G4double targetPos = -(.5*calorSpacing); //position of Z coordinate of target
  G4double magnetLength= .98*m;
  G4double magnetFace = .1*m;
  G4double calorDist = .5*calorSpacing + .5*targetLength;
  G4double spacing = 1.*cm;
  G4double frontSpace = .56*m;
  G4double chamberLength = (calorSpacing-3*spacing-frontSpace-magnetLength)/3;
  G4double toDumpLength = 35.*cm;



  G4double worldLength = 150.*ft;

  //Geometry upgrades

  //wall
  G4double wallTotarget = 6.8*ft;
  G4double wallL = 4.*ft;
  G4double wallW = 2.*ft;
  G4double wallH = 8.*ft;

  //beamline
  G4double blL = (worldLength/2-targetPos-15*cm-targetLength/2)/2;

  //beamdump
  G4double beamdumpL = 8.*ft;
  G4double beamdumpW = 8.*inch;


  //floor and ceiling
  G4double floorW = 20.*ft;
  G4double ceilingW = 20.*ft;
  G4double worldW = 30.*ft;


  

 


  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);


  G4cout << "Computed tolerance = "
	 << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
	 << " mm" << G4endl;


  //World

  /* !!!! DECIDE WHAT SPACE MATERIAL YOU ARE TESTING !!! */
  fSpaceMaterial = fVacuumMaterial; 
  fWorldMaterial = fVacuumMaterial;
  //fBeamLineMaterial = fVacuumMaterial;
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
 
 /*
 new G4PVPlacement(0, // no rotation
		   positionTarget, // at (x,y,z)
		   fLogicTarget, // logical volume
		   "Target", //name
		   worldLV, //mother volume
		   false, //no booleans
		   0, // copy number
		   fCheckOverlaps); //true
 */
 
 

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
    
     if ((i>46 && i<58) || (i>79 && i<95) || (i>112 && i<132) ||
	 (i>146 && i<168) || (i>180 && i<204) || (i>214 && i<240) ||
	 (i>248 && i<276) || (i>282 && i<274) || (i>282 && i<312) ||
	 (i>317 && i<347) || (i>351 && i<383) || (i>386 && i<400) ||
	 (i>404 && i<418) || (i>420 && i<434) || (i>440 && i<454) ||
	 (i>455 && i<468) || (i>476 && i<489) || (i>490 && i<502) ||
	 (i>512 && i<524) || (i>525 && i<537) || (i>547 && i<559) ||
	 (i>560 && i<571) || (i>583 && i<594) || (i>595 && i<606) ||
	 (i>618 && i<629) || (i>630 && i<641) || (i>653 && i<664) ||
	 (i>665 && i<677) || (i>687 && i<699) || (i>700 && i<712) || 
	 (i>722 && i<734) || (i>735 && i<748) || (i>756 && i<769) || 
	 (i>770 && i<784) || (i>790 && i<804) || (i>806 && i<820) ||
	 (i>824 && i<838) || (i>841 && i<873) || (i>877 && i<907) ||
	 (i>912 && i<942) || (i>948 && i<976) || (i>984 && i<1010) ||
	 (i>1020 && i<1044) || (i>1056 && i<1078) || (i>1092 && i<1112) ||
	 (i>1129 && i<1145) || (i>1166 && i<1178))
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
   new G4LogicalVolume(ceiling, fVacuumMaterial, "ceilingLV"); //fWallMaterial
 G4LogicalVolume * floorLV = 
   new G4LogicalVolume(floor, fVacuumMaterial, "floorLV"); //fWallMaterial


     //Wall blocks
     G4VSolid* wall = new G4Box("wallBlock", wallW/2, wallH/2, wallL/2);
     G4LogicalVolume* wallLV = 
       new G4LogicalVolume(wall, fVacuumMaterial, "WallLV"); //fVacuumMaterial

 




//beamline

 G4VSolid * beamline = 
   new G4Tubs("beamline", 2.*inch, 2.*inch+3.*mm, 
	     blL/2, 0.*deg, 360.*deg);

 G4LogicalVolume* beamlineLV = 
   new G4LogicalVolume(
		       beamline, 
		       fBeamLineMaterial, 
		       "beamlineLV");

	     

//beamVoid

 G4VSolid * beamVoid = new G4Tubs("beamvoid", 0.*inch, 2.*inch,
				  blL/2, 0.*deg, 360.*deg); 
 G4LogicalVolume* beamVoidLV = 
   new G4LogicalVolume(
		       beamVoid, 
		       fVacuumMaterial, 
		       "beamVoidLV");


 //Chambers & Magnet
 //CJC 11/15/15

 G4VSolid * magnet1 = new G4Box("magnet1", magnetFace/2, magnetFace/2, magnetLength/2);
 G4LogicalVolume * magnet1LV = new G4LogicalVolume(magnet1, fMagnetMaterial, "magnet1LV");

 G4VSolid * beamPipe0 = new G4Tubs("beamPipe0",14.5*cm, 14.71*cm, (frontSpace+magnetLength+spacing)/2, 
				   0.*deg, 360.*deg); //MODIFIED THICKNESS
 G4LogicalVolume * beamPipe0LV = new G4LogicalVolume(beamPipe0, fBeamLineMaterial, "beampipe0LV");

 G4VSolid * barrier0 = new G4Tubs("barrier0", 14.71*cm, 35.21*cm, .3*cm, 0.*deg, 360.*deg); //MODIFIED THICKNESS
 G4LogicalVolume * barrier0LV = new G4LogicalVolume(barrier0, fBeamLineMaterial, "barrier0LV");

 G4VSolid * beamPipe1 = new G4Tubs("beamPipe1", 35.*cm, 35.21*cm, chamberLength/2, 0.*deg, 360.*deg); //MODIFIED THICKNESS
 G4LogicalVolume * beamPipe1LV = new G4LogicalVolume(beamPipe1, fBeamLineMaterial, "beampipe1LV");

 G4VSolid * beamPipe2 = new G4Tubs("beamPipe2", 61.*cm, 61.21*cm, chamberLength/2, 0.*deg, 360.*deg); //MODIFIED THICKNESS
 G4LogicalVolume * beamPipe2LV = new G4LogicalVolume(beamPipe2, fBeamLineMaterial, "beampipe2LV");

 G4VSolid * beamPipe3 = new G4Tubs("beamPipe3", 87.*cm, 87.21*cm, chamberLength/2-1.*cm, 0.*deg, 360.*deg); //MODIFIED THICKNESS
 G4LogicalVolume * beamPipe3LV = new G4LogicalVolume(beamPipe3, fBeamLineMaterial, "beampipe3LV");

 G4VSolid * barrier1 = new G4Tubs("barrier1", 35.21*cm, 61.21*cm, .3*cm, 0.*deg, 360.*deg); //MODIFIED THICKNESS
 G4LogicalVolume * barrier1LV = new G4LogicalVolume(barrier1, fBeamLineMaterial, "barrier1LV");

 G4VSolid * barrier2 = new G4Tubs("barrier2", 61.21*cm, 87.21*cm, .3*cm, 0.*deg, 360.*deg); //MODIFIED THICKNESS
 G4LogicalVolume * barrier2LV = new G4LogicalVolume(barrier2, fBeamLineMaterial, "barrier2LV");


 //interface with Calorimeter

 G4VSolid * barrier3 = new G4Tubs("barrier3", 25.*cm, 87.*cm, .1*cm, 0.*deg, 360.*deg);

 G4LogicalVolume * barrier3LV = new G4LogicalVolume(barrier3, fBeamLineMaterial, "barrier3LV"); 
 
 G4VSolid * beamPipe4 = new G4Tubs("beamPipe4", 21.*cm, 21.21*cm, toDumpLength/2, 0.*deg, 360.*deg); //MODIFIED THICKNESS

 G4LogicalVolume * beamPipe4LV = new G4LogicalVolume(beamPipe4, fBeamLineMaterial, "beamPipe4LV");
 

		      
 //beamvoid2

 G4VSolid * beamVoid2 = new G4Cons("beamvoid2", 0.*mm, 2.*inch+5.*mm-5.*cm,
				   0.*mm, 90.*cm,  9.9/2.*m, 
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

 //Magnet
 new G4PVPlacement(0, 
		 G4ThreeVector(35.*cm+magnetFace/2, 0., frontSpace+targetPos+magnetLength/2), 
		 magnet1LV, 
		 "Magnet", 
		 worldLV, 
		 false, 
		 0, 
		 fCheckOverlaps);

 new G4PVPlacement(0, 
		 G4ThreeVector(-35.*cm-magnetFace/2, 0., frontSpace+targetPos+magnetLength/2), 
		 magnet1LV, 
		 "Magnet", 
		 worldLV, 
		 false, 
		 0, 
		 fCheckOverlaps);

 //Modified vacuum chamber
 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., targetPos+(frontSpace+targetLength+magnetLength)/2), 
		   beamPipe0LV, 
		   "beampipe0", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
 
 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., targetPos+frontSpace+targetLength/2+magnetLength+chamberLength/2),
		   beamPipe1LV, 
		   "beampipe1", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., targetPos+frontSpace+targetLength/2+magnetLength+chamberLength*3/2), 
		   beamPipe2LV, 
		   "beampipe2", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., frontSpace+targetPos+targetLength/2+magnetLength+chamberLength*5/2), 
		   beamPipe3LV, 
		   "beampipe3", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., frontSpace+targetPos+targetLength/2+magnetLength-.3*cm), 
		   barrier0LV, 
		   "barrier0", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., frontSpace+targetPos+targetLength/2+magnetLength+chamberLength-.3*cm), 
		   barrier1LV, 
		   "barrier1", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., frontSpace+targetPos+targetLength/2+magnetLength+chamberLength*2-.3*cm), 
		   barrier2LV, 
		   "barrier2", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., frontSpace+targetPos+targetLength/2+magnetLength+3*chamberLength-.1*cm), 
		   barrier3LV, 
		   "barrier3", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);

 new G4PVPlacement(0, 
		   G4ThreeVector(0., 0., magnetLength+frontSpace+targetPos+targetLength/2+spacing+3*chamberLength+toDumpLength/2), 
		   beamPipe4LV, 
		   "beamPipe4", 
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
    

 
     //beamline


     new G4PVPlacement(
		   0, 
		   G4ThreeVector(0., 0., targetPos-blL/2-targetLength/2), 
		   beamlineLV, 
		   "Beamline", 
		   worldLV, 
		   false, 
		   0, 
		   fCheckOverlaps);
     //beamVoid
     
     new G4PVPlacement(
		   0, 
		   G4ThreeVector(0., 0., targetPos-blL/2-targetLength),
		   beamVoidLV, 
		   "BeamVoid", 
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
 

 G4VSolid* liningO = new G4Tubs("liningO", 90.2*cm, 90.7*cm, crystalLength/2, 
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

     

									  
    
    


     

   
 


 //Visualization

 G4VisAttributes* color  = new G4VisAttributes(G4Colour(0.9, 0.7, 0.2));

 //worldLV->SetVisAttributes(new G4VisAttributes(G4Colour(1.0,1.0,1.0)));
 fLogicTarget->SetVisAttributes(color);
 beamlineLV->SetVisAttributes(new G4VisAttributes(G4Colour(0., 1., 0.)));

 //liningHLV->SetVisAttributes(new G4VisAttributes(G4Colour(.7, .7, .7)));
 //liningLV->SetVisAttributes(new G4VisAttributes(G4Colour(.7, .7, .7)));
 
 wallLV->SetVisAttributes(new G4VisAttributes(G4Colour(.4, .4, .4)));
 beamdumpLV->SetVisAttributes(new G4VisAttributes(G4Colour(1, .64, 0.)));
 beamVoidLV->SetVisAttributes(new G4VisAttributes(G4Colour(1., 0., 0.))); 
 beamPipe3LV->SetVisAttributes(new G4VisAttributes(G4Colour(1., 0., 0.)));
 beamPipe2LV->SetVisAttributes(new G4VisAttributes(G4Colour(1., 0., 0.)));
 beamPipe1LV->SetVisAttributes(new G4VisAttributes(G4Colour(1., 0., 0.)));
 omniLV->SetVisAttributes(new G4VisAttributes(G4Colour(0., 1., 0.)));
 magnet1LV->SetVisAttributes(new G4VisAttributes(G4Colour(0., 0., 1.)));
 barrier1LV->SetVisAttributes(new G4VisAttributes(G4Colour(1., 0., 0.)));
 barrier2LV->SetVisAttributes(new G4VisAttributes(G4Colour(1., 0., 0.)));
 barrier3LV->SetVisAttributes(new G4VisAttributes(G4Colour(0., 0., 1.)));
 beamPipe4LV->SetVisAttributes(new G4VisAttributes(G4Colour(0., 1., 0.)));



 color->SetVisibility(true);

 //Setting user Limits

 G4double maxStep = 1.0*cm;
 fStepLimit = new G4UserLimits(maxStep);



 return worldPV;

   }


void DetectorConstruction::ConstructSDandField()
{
  //!!!
  //Create a sensitive detector and put it with logical volumes
  G4SDManager* sdMan = G4SDManager::GetSDMpointer();
  G4String SDname;

    /*Add Adjustment Magnet Field*/
    AdjustmentField* aField = new AdjustmentField;
    G4FieldManager* fieldMgr
            = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDetectorField(aField);
    fieldMgr->CreateChordFinder(aField);

    G4PropagatorInField* propMgr = G4TransportationManager::GetTransportationManager()->GetPropagatorInField();

    //Largest acceptable Step is 1km by default
    propMgr->SetLargestAcceptableStep( 10*cm);

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
