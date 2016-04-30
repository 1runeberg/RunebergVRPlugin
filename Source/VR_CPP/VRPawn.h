// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/* 
GNU GPLv3 - http://www.gnu.org/licenses/gpl.txt

VR Pawn with C++ & Blueprint common interface functions
Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

UCLASS(Blueprintable)
class VR_CPP_API AVRPawn : public APawn
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	AVRPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

private:
	UInputComponent* InputComponent = nullptr;

};
