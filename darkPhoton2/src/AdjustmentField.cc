#include "G4SystemOfUnits.hh"
#include "AdjustmentField.hh"
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
AdjustmentField::AdjustmentField()
{

  std::ifstream file("../magMap.table");
  std::string line;

  center.x = 0;
  center.y = 0;
  center.z = -(.5*10.*m) + 0.55*m + 0.25*m;

  int i = 0;
  while(std::getline(file, line))
  {
      std::stringstream linestream(line);

      if(i == 0){
        linestream >> DEPTH >> WIDTH >> HEIGHT;
        DEPTH = (DEPTH-1)*2;
        HEIGHT = HEIGHT-1;
        WIDTH = WIDTH-1;
        //printf("Pos is H %i W %i D %i\n",HEIGHT, WIDTH, DEPTH);
        //Set up sizes. (HEIGHT x WIDTH)
        magField.resize(HEIGHT*2);
        for (int k = 0; k < HEIGHT*2; ++k) {
          magField[k].resize(WIDTH*2);
          for (int j = 0; j < WIDTH*2; ++j){
            magField[k][j].resize(DEPTH*2);
          }
        }
      } else if (i>7) {
        
        double posX;
        double posY;
        double posZ;

        double bX;
        double bY;
        double bZ;

        // If you have truly tab delimited data use getline() with third parameter.
        // If your data is just white space separated data
        // then the operator >> will do (it reads a space separated word into a string).

        // Read the integers using the operator >>
        linestream >> posX >> posY >> posZ >> bX >> bY >> bZ;

        //non-zero!
        posX = posX + HEIGHT/2;
        posY = posY + WIDTH/2;
        posZ = posZ + DEPTH/2;

        printf("Pos is %f, %f, %f, H %i W %i D %i\n", posX, posY, posZ, HEIGHT, WIDTH, DEPTH);


        //Save into integer sized chunks.

        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].x = bX;
        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].y = bY;
        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].z = bZ;
        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ+1)].x = bX;
        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ+1)].y = bY;
        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ+1)].z = bZ;

    }
    i++;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
AdjustmentField::~AdjustmentField()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void AdjustmentField::GetFieldValue(const double Point[3],double *Bfield) const
{

  G4double constFactor = 1.0;

  //printf("Evaluating magfield at %f, %f, %f\n", Point[0], Point[1], Point[2]);

  G4double posX = (Point[0]-center.x)/cm;
  G4double posY = (Point[1]-center.y)/cm;
  G4double posZ = (Point[2]-center.z)/cm;

  //Translate to wherever the magnet is.
  posX += HEIGHT/2;
  posY += WIDTH/2;
  posZ += DEPTH/2;

  //printf("Calcpos is %f, %f, %f\n", posX, posY, posZ);

  if( posX <= HEIGHT*2 && posX >= 0 &&
      posY <= WIDTH*2 && posY >= 0 &&
      posZ <= DEPTH*2 && posZ >= 0){

    //We are within the magnetic region - extrapolate

    Bfield[0] = constFactor*magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].x*gauss;
    Bfield[1] = constFactor*magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].y*gauss;
    Bfield[2] = constFactor*magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].z*gauss;
    //printf("FOUND %f, %f, %f WITH STRENGTH %f, %f, %f\n", posX, posY, posZ, Bfield[0]/gauss, Bfield[1]/gauss, Bfield[2]/gauss);

  } else {
    Bfield[0] = 0*gauss;
    Bfield[1] = 0*gauss;
    Bfield[2] = 0*gauss;
  }

}
