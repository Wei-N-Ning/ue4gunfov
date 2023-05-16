#include "ViewModelSkeletalMeshComponent.h"
#include "FovUtils.h"
#include "ViewProjectionUtils.h"
#include "DebugHud.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

FMatrix UViewModelSkeletalMeshComponent::GetRenderMatrix() const
{
	// ensure the custom render matrix is enabled
	if (!EnableCustomRenderMatrix)
	{
		return Super::GetRenderMatrix();
	}

	// ensure we have a player controller
	const auto World = GetWorld();
	if (!World)
	{
		return Super::GetRenderMatrix();
	}
	
	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		return Super::GetRenderMatrix();
	}

	// ensure we have a local player
	if (!PlayerController->Player)
	{
		return Super::GetRenderMatrix();
	}
	const auto* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
	if (!LocalPlayer)
	{
		return Super::GetRenderMatrix();
	}

	// ensure we have a viewport
	if (!(LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport))
	{
		return Super::GetRenderMatrix();
	}

	FSceneViewFamilyContext ViewFamily(
		FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags
		).SetRealtimeUpdate(true)
	);

	const auto Matrices = ViewProjectionUtils::GetMatrices(*World);

	const auto ViewportSize = LocalPlayer->ViewportClient->Viewport->GetSizeXY();
	const float Width = ViewportSize.X;
	const float Height = ViewportSize.Y;
	constexpr float TargetHeight = 1080.0f;
	constexpr float TargetWidth = 1920.0f;
	constexpr float FullCircleDegrees = 360.0f;

	// Calculate the new projection matrix based on the desired horizontal FOV
	FMatrix NewProjectionMatrix;
	const auto MaxFittingRes = FovUtils::GetMaxFittingResolution(
		TargetWidth,
		TargetHeight,
		ViewportSize.X,
		ViewportSize.Y
	);
	if (MaxFittingRes.AspectCorrection == FovUtils::ResolutionInformation::EAspectCorrection::PILLAR_BOX)
	{
		// Pillar box: vertical black bars are added to the sides of the image
		const float HorPlusFov = FovUtils::HorFovToHorPlus(DesiredHorizontalFov, TargetWidth, TargetHeight,
		                                                   ViewportSize.X, ViewportSize.Y);
		const float DesiredHalfFovRad = HorPlusFov * PI / FullCircleDegrees;
		NewProjectionMatrix = FReversedZPerspectiveMatrix{
			DesiredHalfFovRad,
			Width,
			Height,
			Matrices.NearClippingPlaneDistance
		};
	}
	else
	{
		// Letter box: horizontal black bars are added to the top and bottom of the image
		const float DesiredHalfFovRad = DesiredHorizontalFov * PI / FullCircleDegrees;
		NewProjectionMatrix = FReversedZPerspectiveMatrix{
			DesiredHalfFovRad,
			TargetWidth,
			TargetHeight,
			Matrices.NearClippingPlaneDistance
		};
	}

	const FTransform ComponentTransform = GetComponentTransform();
	AddDebugMessage(GetName());
	AddDebugMessage(ComponentTransform.ToString());
	const FMatrix NewViewProjectionMatrix = Matrices.ViewMatrix * NewProjectionMatrix;
	const FMatrix InverseOldViewProjectionMatrix = Matrices.InverseViewProjectionMatrix;
	const FMatrix ModelMatrix = ComponentTransform.ToMatrixWithScale();

	// the returned matrix is essentially:
	// M . V . P' . (V . P)^-1
	// then when it's used to transform the vertices during the rendering stage,
	// multiplied by (V . P), the result is
	// M . V . P' . (V . P)^-1 . (V . P) = M . V . P'
	return ModelMatrix * NewViewProjectionMatrix * InverseOldViewProjectionMatrix;
}

void UViewModelSkeletalMeshComponent::AddDebugMessage(FString&& Message) const
{
	if (const auto World = GetWorld())
	{
		if (const auto PlayerController = World->GetFirstPlayerController())
		{
			if (const auto hud = Cast<ADebugHud>(PlayerController->GetHUD()))
			{
				hud->AddDebugMessage(GetTypeHash(GetName()), Message, FColor::Green, 1.25f);
			}
		}
	}
}
