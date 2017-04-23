// Copyright (C) 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Components/SceneComponent.h"
#include "RunebergVR_CustomGravity.h"
#include "RunebergVR_Climb.generated.h"


UCLASS( ClassGroup=(VR), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_Climb : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URunebergVR_Climb();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Tags that can be used to stop gravity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	TArray<FName> ClimbTags;

	// Gravity origin when gravity direction is set to RELATIVE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FVector InitialLocation = FVector::ZeroVector;

	// Whether this actor is currently falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool IsClimbing = false;
	
	// Whether this actor is currently falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	URunebergVR_CustomGravity* CustomGravity = nullptr;

	/** Enable climb mode */
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Climb();

	/** Deactivate climb mode */
	UFUNCTION(BlueprintCallable, Category = "VR")
	void LetGo();

private:
	void StartClimb(USceneComponent* ClimbingReference);
};
