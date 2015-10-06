#include "G4SystemOfUnits.hh"
#include "AdjustmentField.hh"
#include <vector>
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
AdjustmentField::AdjustmentField()
{
  Bz = 3.0*tesla;
  rmax_sq = sqr(50.*cm);
  zmax = 100.*cm;
  std::vector<std::vector<std::vector<double> > > magField;

  // Put some values in
  // array3D[1][2][5] = 6.0;
  // array3D[3][1][4] = 5.5;

  std::ifstream file("../magMap.table");
  std::string   line;


  int i = 0;
  while(std::getline(file, line))
  {
      std::stringstream   linestream(line);

      if(i == 0){
        int HEIGHT;
        int WIDTH;
        int DEPTH;
        linestream >> HEIGHT >> WIDTH >> DEPTH;

        // Set up sizes. (HEIGHT x WIDTH)
        magField.resize(HEIGHT*2);
        for (int i = 0; i < HEIGHT*2; ++i) {
          magField[i].resize(WIDTH*2);

          for (int j = 0; j < WIDTH*2; ++j)
            magField[i][j].resize(DEPTH*2);
        }
      } else if (i>7) {
        
        double i;
        double j;
        double k;

        double bX;
        double bY;
        double bZ;

        // If you have truly tab delimited data use getline() with third parameter.
        // If your data is just white space separated data
        // then the operator >> will do (it reads a space separated word into a string).
        //std::getline(linestream, data, '\t');  // read up-to the first tab (discard tab).

        // Read the integers using the operator >>
        linestream >> i >> j >> k >> bX >> bY >> bZ;
        magField[(int)floor(i)][(int)floor(j)][(int)floor(k)] = bX;
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

  Bfield[0] = 1000.0*tesla;
  Bfield[1] = 1000.0*tesla;
  Bfield[2] = 1000.0*tesla;
}