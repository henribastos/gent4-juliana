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

#include "SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

#include <iostream>
#include <fstream>

using namespace std;

SteppingAction::SteppingAction(G4String edepfileName):fEdepName(edepfileName)
{
// User defined option for either writeMode or readMode (environmental variable) 
//	rwMode = "writeMode"; // default initialization to write (usual particle generation)
//	if (getenv( "RW_MODE" )!=NULL) rwMode = getenv( "RW_MODE" );
}

SteppingAction::~SteppingAction()
{;}

void SteppingAction::UserSteppingAction(const G4Step * step)
{

 const G4VProcess* preprocess = step-> GetPreStepPoint()-> GetProcessDefinedStep(); 
 const G4VProcess* postprocess = step-> GetPostStepPoint()-> GetProcessDefinedStep(); 



 if (/*preprocess != 0 && */postprocess != 0){

  if (/*preprocess->GetProcessName() && */postprocess->GetProcessName())
  {
	if (step->GetPostStepPoint()->GetPhysicalVolume()){
		
		PreVolumeName=step->GetPreStepPoint()->GetPhysicalVolume()->GetName();    
//		PreProcessName=s->GetPreStepPoint()-> GetProcessDefinedStep()->GetProcessName();
		PreStepEkin=step->GetPreStepPoint()->GetKineticEnergy();

		xPre=step->GetPreStepPoint()->GetPosition().x();
		yPre=step->GetPreStepPoint()->GetPosition().y();			
		zPre=step->GetPreStepPoint()->GetPosition().z();
		
		PostVolumeName=step->GetPostStepPoint()->GetPhysicalVolume()->GetName();    
		PostProcessName=step->GetPostStepPoint()-> GetProcessDefinedStep()->GetProcessName();
		PostStepEkin=step->GetPostStepPoint()->GetKineticEnergy();        
		
		xPost=step->GetPostStepPoint()->GetPosition().x();
		yPost=step->GetPostStepPoint()->GetPosition().y();		
		zPost=step->GetPostStepPoint()->GetPosition().z();
		
		Z=step->GetTrack()->GetDefinition()->GetAtomicNumber();
		A=step->GetTrack()->GetDefinition()->GetBaryonNumber();
		

		PostPx=step->GetPostStepPoint()->GetMomentumDirection().x();
		PostPy=step->GetPostStepPoint()->GetMomentumDirection().y();
		PostPz=step->GetPostStepPoint()->GetMomentumDirection().z();
	
		theta=step->GetPostStepPoint()->GetMomentumDirection().theta();
		phi=step->GetPostStepPoint()->GetMomentumDirection().phi();

		if (phi < 0)
		{
			//phi2 = phi+360*deg;
			phi=phi+360*deg;
		}

		//else
			//phi2=0;

		rPost = sqrt(xPost*xPost+yPost*yPost);

		ParticleName=step->GetTrack()->GetDefinition()->GetParticleName(); 
		TrackID=step->GetTrack()->GetTrackID();
		StepNo =step->GetTrack()->GetCurrentStepNumber();
		Edep=step->GetTotalEnergyDeposit();
		


// For a particle just entering the PS1, store required info:		

				if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary && PreVolumeName=="fWorld_phys" && PostVolumeName=="PS1_phys"){
					G4cout << "UserInfo PreStepping: Particle -> " << ParticleName 
						<<  " , TrackID -> " << TrackID 
						<< " , PreProcessName -> " << PreProcessName 
						<< " , PreVolumeName -> " << PreVolumeName 
						<< " , z = " << zPre/cm 
						<< " , Energy = " << PreStepEkin/MeV 
						<< G4endl; 
	
					G4cout << "UserInfo PostStepping: Particle -> " << ParticleName 
						<<  " , TrackID -> " << TrackID 
						<< " , PostProcessName -> " << PostProcessName 
						<< " , PostVolumeName -> " << PostVolumeName 
						<< " , z = " << zPost/cm 
						<< " , Energy = " << PostStepEkin/MeV 
						<< G4endl;
					
		        G4double randNoX = G4UniformRand();   
			G4double randNoY = G4UniformRand();   
			G4double randNoZ = G4UniformRand();
			xEdep = step->GetPreStepPoint()->GetPosition().x() + randNoX * (step->GetPostStepPoint()->GetPosition().x() - step->GetPreStepPoint()->GetPosition().x() );//position in the "middle" of the pixel
			yEdep = step->GetPreStepPoint()->GetPosition().y() + randNoY * (step->GetPostStepPoint()->GetPosition().y() - step->GetPreStepPoint()->GetPosition().y() );
			zEdep = step->GetPreStepPoint()->GetPosition().z() + randNoZ * (step->GetPostStepPoint()->GetPosition().z() - step->GetPreStepPoint()->GetPosition().z() );
		
				
					//if (rwMode == "writeMode")
					{
						ofstream file ("/project/med/MAPDOSI/Reserva/Linac_PhaseSpace/build_PhaseSpacelinac/PhaseSpaceP.dat", ios::app);
						if (file.is_open()){
							file << ParticleName << "	"
									<< TrackID << "	"
									<< StepNo << "	"
								//	<< Z << "	"
								//	<< A << "	" 
									<< xPre/cm << "	" 
									<< yPre/cm << "	" 
									<< zPre/cm << "	" 
								//	<< PostPx << "	"
								//	<< PostPy << "	"
								//	<< PostPz << "	"
									<< PreStepEkin/keV << "	" 
									<< PostStepEkin/keV << "	" 

									<< PostProcessName<< "	" 
									<< G4endl;												
							} else G4cout << "PhaseSpaceP.data" << G4endl;
						}	
					}
						 
		
// Following the steps of all particles everywhere so that depth dose profiles can be acquired. 
// Volume selection can also be coded here			
// Randomize the point of Edep along a step
			
			  if ( PostVolumeName=="PS1_phys" ) {
	
				
			
			G4double randNoX = G4UniformRand();   
			G4double randNoY = G4UniformRand();   
			G4double randNoZ = G4UniformRand();
			xEdep = step->GetPreStepPoint()->GetPosition().x() + randNoX * (step->GetPostStepPoint()->GetPosition().x() - step->GetPreStepPoint()->GetPosition().x() );
			yEdep = step->GetPreStepPoint()->GetPosition().y() + randNoY * (step->GetPostStepPoint()->GetPosition().y() - step->GetPreStepPoint()->GetPosition().y() );
			zEdep = step->GetPreStepPoint()->GetPosition().z() + randNoZ * (step->GetPostStepPoint()->GetPosition().z() - step->GetPreStepPoint()->GetPosition().z() );
		
				
			ofstream file (fEdepName, ios::app); 
			if (file.is_open()){
							file << ParticleName << "	"
									//<< theta/deg << "   "
									<< theta/rad << "   "
									//<< phi/deg << "    "
									//<< phi2/deg << "    "
									<< phi/rad << "    "
									//<< TrackID << "	"
									//<< StepNo << "	"
		 							//<< xEdep/cm << "	"
									//<< yEdep/cm << "	"
									//<< zEdep/cm << "	"
									//<< xPre/cm << "	"
									//<< yPre/cm << "	"
									//<< zPre/cm << "	"
									//<< xPost/cm << "	"
									//<< yPost/cm << "	"
									//<< zPost/cm << "	"
									<< rPost/cm << "    "
									//<< PreStepEkin/keV << "	" //PreStepEkin will be = PostStepEkin (because the phsp plane is vacuum and very thin.
									<< PostStepEkin/MeV << "	"
									//<< Edep/MeV << "	"
									//<< PreVolumeName << "	"
									//<< PostVolumeName << "	"
									//<< PostProcessName	<< " 	"
							        //<< PostPx << "     "
								    //<< PostPy	<< " 	"
								 	//<< PostPz << "     "
									<< G4endl;
							} else G4cout << "Unable to open DepthDoseProfile file " << fEdepName << G4endl;						
					
				}
		
		
		} //else { G4cout << "No PostStep Physical Volume" << G4endl; }
    }
		
  
 }

}


