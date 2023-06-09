﻿#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "ViewModelSkeletalMeshComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4GUNFOV_API UViewModelSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Mesh)
	float DesiredHorizontalFov;

	UPROPERTY(EditAnywhere, Category=Mesh)
	bool EnableCustomRenderMatrix = true;

protected:
	// The calculation to render an object with a camera that has the World FOV is
	// M . V . P = T
	// where
	//  - M: The model matrix of the object
	//  - V: The view matrix of the camera
	//  - P: The projection matrix with a World FOV
	//  - The resulting transform of the object in NDC space.
	// M is what is returned by GetRenderMatrix(), so this is the matrix we can adjust to get
	// us to an object rendered with the viewmodel FOV.
	// see forum post:
	// https://forums.unrealengine.com/t/custom-mesh-fov/279782/5
	// in short, the goal is to cancel the existing (V . P) component in the MVP matrix
	// M . V . P' . (P . V)^-1
	// then when it's used to transform the vertices (combined with V . P), it becomes
	// M . V . P' . (P . V)^-1 . (P . V) = M . V . P'
	// where P' is the new projection matrix with the desired FOV
	virtual FMatrix GetRenderMatrix() const override;

private:
	void AddDebugMessage(FString&& Message) const;
};
