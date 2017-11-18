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
// $Id: DicomPrimaryGeneratorAction.cc 74809 2013-10-22 09:49:26Z gcosmo $
//
/// \file medical/DICOM/src/DicomPrimaryGeneratorAction.cc
/// \brief Implementation of the DicomPrimaryGeneratorAction class
//
// The code was written by :
//      *Louis Archambault louis.archambault@phy.ulaval.ca,
//      *Luc Beaulieu beaulieu@phy.ulaval.ca
//      +Vincent Hubert-Tremblay at tigre.2@sympatico.ca
//
//
// *Centre Hospitalier Universitaire de Quebec (CHUQ),
// Hotel-Dieu de Quebec, departement de Radio-oncologie
// 11 cote du palais. Quebec, QC, Canada, G1R 2J6
// tel (418) 525-4444 #6720
// fax (418) 691 5268
//
// + Universit� Laval, Qu�bec (QC) Canada
//*******************************************************

#include "DicomPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "DicomRegularDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4IAEAphspReader.hh"
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomPrimaryGeneratorAction::DicomPrimaryGeneratorAction()
{
	   G4cout << " The name of the Elekta source is an input of the UImessenger"
	   <<G4endl;
	     	G4String filename = "GantryData.out";

	#ifdef G4VERBOSE
	     	G4cout << " DicomPrimaryGeneratorAction is opening the gantry file "
	     			<< filename << G4endl;
	          		#endif
	          		std::ifstream finR(filename.c_str(), std::ios_base::in);
	          		if( !finR.is_open() ) {
	          	    G4Exception("DicomPrimaryGeneratorAction has an error",
	          	                "",
	          	                FatalException,
	          	                G4String("File not found " + filename).c_str());
	          		}


	          		G4double angle, FieldX, FieldZ;
	          		G4String OutputFile, fileName;

	          		finR >> angle>>FieldX>>FieldZ>>OutputFile>>fileName;
	          		finR.close();

    G4String TotalName="../../"+fileName;
	std::cout<<"generating particles !!!"<<std::endl;
	theIAEAReader = new G4IAEAphspReader(TotalName);

	//std::cout<<"generating particles !!!"<<std::endl;
	//theIAEAReader = new G4IAEAphspReader(fileName);

    //Phase space plane shift through the z axis
	G4ThreeVector psfShift(0.,0.,-100.*cm);   //phase space at -z, this makes particles travel to +z
	theIAEAReader ->SetGlobalPhspTranslation(psfShift);
	//rotate to  put at Y axis
	theIAEAReader ->SetRotationOrder(132);
	//theIAEAReader ->SetRotationX(180*deg);  //the phsp will be in +z; particles traveling to -z.
	theIAEAReader ->SetRotationZ(angle*deg);


	//theIAEAReader ->SetCollimatorAngle(180.0*deg);



}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
	DicomPrimaryGeneratorAction::~DicomPrimaryGeneratorAction()
	{
	if (theIAEAReader) delete theIAEAReader;
	}
	void DicomPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
	{
	theIAEAReader->GeneratePrimaryVertex(anEvent);
	}


