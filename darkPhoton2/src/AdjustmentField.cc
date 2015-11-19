#include "G4SystemOfUnits.hh"
#include "AdjustmentField.hh"
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
AdjustmentField::AdjustmentField()
{

  std::ifstream file("../six_map.table");
  x_prec = 0.5;
  y_prec = 0.5;
  z_prec = 2.0;

  x_zero = -9.0;
  y_zero = -9.0;
  z_zero = -60.0;

  std::string line;

  center.x = 0;
  center.y = 0;
  center.z = -(.5*10.*m) + 0.55*m + 0.25*m + .5*m;

  int i = 0;
  while(std::getline(file, line))
  {
    std::stringstream linestream(line);

    if(i == 0){
      linestream >> DEPTH >> WIDTH >> HEIGHT;
      DEPTH = DEPTH; // This is the number of entries in each direction
      HEIGHT = HEIGHT;
      WIDTH = WIDTH;
      //printf("Pos is H %i W %i D %i\n",HEIGHT, WIDTH, DEPTH);
      //Set up sizes. (HEIGHT x WIDTH)
      magField.resize(HEIGHT);
      for (int k = 0; k < HEIGHT; ++k) {
        magField[k].resize(WIDTH);
        for (int j = 0; j < WIDTH; ++j){
          magField[k][j].resize(DEPTH);
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
      posX = (posX - x_zero)/x_prec;// + (HEIGHT-1)/2;
      posY = (posY - y_zero)/y_prec;// + (WIDTH-1)/2;
      posZ = (posZ - z_zero)/z_prec;// + //(DEPTH-1)/2;

      printf("Pos is %f, %f, %f, H %i W %i D %i\n", posX, posY, posZ, HEIGHT, WIDTH, DEPTH);


      //Save into integer sized chunks.

      magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].x = bX;
      magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].y = bY;
      magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].z = bZ;
//        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ+1)].x = bX;
//        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ+1)].y = bY;
//        magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ+1)].z = bZ;

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

  G4double posX = ((Point[0]-center.x)/cm - x_zero)/x_prec;
  G4double posY = ((Point[1]-center.y)/cm - y_zero)/y_prec;
  G4double posZ = ((Point[2]-center.z)/cm - z_zero)/z_prec;

  //Translate to wherever the magnet is.
//  posX += (HEIGHT-1)/2;
//  posY += (WIDTH-1)/2;
//  posZ += (DEPTH-1)/2;

  //printf("Calcpos is %f, %f, %f\n", posX, posY, posZ);

  if( posX <= HEIGHT && posX >= 0 &&
      posY <= WIDTH && posY >= 0 &&
      posZ <= DEPTH && posZ >= 0){

    //We are within the magnetic region - extrapolate

    Bfield[0] = constFactor*magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].x*gauss;
    Bfield[1] = constFactor*magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].y*gauss;
    Bfield[2] = constFactor*magField[(int)floor(posX)][(int)floor(posY)][(int)floor(posZ)].z*gauss;
    printf("FOUND %f, %f, %f WITH STRENGTH %f, %f, %f\n", posX, posY, posZ, Bfield[0]/gauss, Bfield[1]/gauss, Bfield[2]/gauss);
    printf("ORIGINAL WAS %f, %f, %f\n", (Point[0]-center.x)/cm, (Point[1]-center.y)/cm, (Point[2]-center.z)/cm);

  } else {
    Bfield[0] = 0*gauss;
    Bfield[1] = 0*gauss;
    Bfield[2] = 0*gauss;
  }

}
