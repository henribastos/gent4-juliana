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
// $Id: DicomLinac.cc 84839 2014-10-21 13:44:55Z gcosmo $
//
/// \file medical/DICOM/src/DicomLinac.cc
/// \brief Implementation of the DicomLinac class
//

#include "DicomLinac.hh"

#include "G4UIcmdWithAString.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VSolid.hh"
#include "G4tgrSolid.hh"
#include "G4tgbVolume.hh"
#include "G4Material.hh"
#include "G4PhantomParameterisation.hh"
#include "G4PVParameterised.hh"
#include "G4UIcommand.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomLinac::DicomLinac()
 : G4UImessenger()
{

	LinacDirectory= new G4UIdirectory("/linacCommands/");
	LinacDirectory->SetGuidance("linac parameters");

	DefineGantryPrm = new G4UIcmdWithAString("/linacCommands/DefineParameters",this);
    DefineGantryPrm->SetGuidance("It must have the parameters:ANGLE APERTURE_Y1 APERTURE_Y2 OUTPUTFile PhspIAEAFile MLCLeftFile MLCRigthFile");
    DefineGantryPrm->SetParameterName("choice",true);
    DefineGantryPrm->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DicomLinac::~DicomLinac()
{
 if (DefineGantryPrm) delete DefineGantryPrm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DicomLinac::SetNewValue(G4UIcommand * command,
                                             G4String newValues)    
{ 
//	G4AffineTransform theVolumeTransform;

	if( command == DefineGantryPrm ){
		std::vector<G4String> params = GetWordsInString( newValues );
		if( params.size() < 7 ) {
			G4Exception("DefineGantryPrm::SetNewValue",
					" There must be at least 7 parameters: ANGLE APERTURE_Y1 APERTURE_Y2 OUTPUTFile IAEAPhaseSpaceFile MLCLeftFile MLCRightFile",
					FatalErrorInArgument,
					G4String("Number of parameters given = " +
							G4UIcommand::ConvertToString( G4int(params.size()) )).c_str());
		}
		//----- Write phantom header
		G4String thePhantomFileName = "GantryData.out";
		fout.open(thePhantomFileName);
		for( unsigned int ii = 0; ii < params.size(); ii++ )
		    fout << params[ii] << G4endl;
		fout.close();
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
std::vector<G4String> DicomLinac::GetWordsInString(
                                                const G4String& stemp)
{
  std::vector<G4String> wordlist;

  //---------- Read a line of file:
  //----- Clear wordlist
  G4int ii;
  const char* cstr = stemp.c_str();
  int siz = stemp.length();
  int istart = 0;
  int nQuotes = 0;
  bool lastIsBlank = false;
  bool lastIsQuote = false;
  for( ii = 0; ii < siz; ii++ ){
    if(cstr[ii] == '\"' ){
      if( lastIsQuote ){
        G4Exception("GmGenUtils:GetWordsFromString","",FatalException,
            ("There cannot be two quotes together " + stemp).c_str() );
      }
      if( nQuotes%2 == 1 ){
        //close word 
        wordlist.push_back( stemp.substr(istart,ii-istart) );
        //        G4cout << "GetWordsInString new word0 " 
    //<< wordlist[wordlist.size()-1] << " istart " << istart 
        //<< " ii " << ii << G4endl;
      } else {
        istart = ii+1;
      }
      nQuotes++;
      lastIsQuote = true;
      lastIsBlank = false;
    } else if(cstr[ii] == ' ' ){
      //            G4cout << "GetWordsInString blank nQuotes " 
      //<< nQuotes << " lastIsBlank " << lastIsBlank << G4endl;
      if( nQuotes%2 == 0 ){
        if( !lastIsBlank && !lastIsQuote ) {
          wordlist.push_back( stemp.substr(istart,ii-istart) );
          //          G4cout << "GetWordsInString new word1 " 
      //<< wordlist[wordlist.size()-1] << " lastIsBlank " 
          //<< lastIsBlank << G4endl;
        }

        istart = ii+1;
        lastIsQuote = false;
        lastIsBlank = true;
      }
    } else {
      if( ii == siz-1 ) {
        wordlist.push_back( stemp.substr(istart,ii-istart+1) );
        //                G4cout << "GetWordsInString new word2 " 
    //<< wordlist[wordlist.size()-1] << " istart " << istart << G4endl;
      }
      lastIsQuote = false;
      lastIsBlank = false;
    }
  }
  if( nQuotes%2 == 1 ) {
    G4Exception("GmGenUtils:GetWordsFromString","",FatalException, 
        ("unbalanced quotes in line " + stemp).c_str() );
  }


  return wordlist;
}

