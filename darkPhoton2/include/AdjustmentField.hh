//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: AdjustmentField.hh 68698 2013-04-05 08:41:22Z gcosmo $
//
/// \file RE05/include/AdjustmentField.hh
/// \brief Definition of the AdjustmentField class
//
using namespace std;
#ifndef AdjustmentField_H
#define AdjustmentField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"
#include <vector>

class AdjustmentField : public G4MagneticField
{
public:
    AdjustmentField();
    virtual ~AdjustmentField();

    virtual void GetFieldValue(const double Point[3],
                               double *Bfield ) const;

private:
    struct position {
        G4double x;
        G4double y;
        G4double z;
    };
    int HEIGHT;
    int WIDTH;
    int DEPTH;
    double x_prec;
    double y_prec;
    double z_prec;

    double x_zero;
    double y_zero;
    double z_zero;

    position center;
    std::vector<std::vector<std::vector<position> > > magField;
};

#endif

