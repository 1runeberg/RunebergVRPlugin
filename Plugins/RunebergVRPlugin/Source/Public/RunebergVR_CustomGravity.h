// Copyright (C) 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Components/ActorComponent.h"
#include "RunebergVR_CustomGravity.generated.h"

UENUM(BlueprintType)		
enum class EGravityDirection : uint8
{
	DIR_DOWN		UMETA(DisplayName = "Down"),
	DIR_UP			UMETA(DisplayName = "Up"),
	DIR_LEFT		UMETA(DisplayName = "Left"),
	DIR_RIGHT		UMETA(DisplayName = "Right"),
	DIR_FORWARD		UMETA(DisplayName = "Forward"),
	DIR_BACK		UMETA(DisplayName = "Back"),
	DIR_RELATIVE	UMETA(DisplayName = "Relative")
};

UCLASS( ClassGroup=(VR), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_CustomGravity : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URunebergVR_CustomGravity();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Whether this actor is currently falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	EGravityDirection GravityDirection = EGravityDirection::DIR_DOWN;

	// Rate of gravity's acceleration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float GravityStrength = 1.0f;

	// Gravity origin when gravity direction is set to RELATIVE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FVector GravityOrigin = FVector::ZeroVector;

	// Tags that can be used to stop gravity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	TArray<FName> StopTags;

	// Tags that can be used to start gravity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	TArray<FName> StartTags;

	// Whether this actor is currently falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool IsGravityActive = true;

	/** Stop or Start Gravity if collided component or actor has the corresponding start/stop tag */
	UFUNCTION(BlueprintCallable, Category = "VR")
	void ProcessTags(AActor* OtherActor, UPrimitiveComponent* OtherComp);
};
