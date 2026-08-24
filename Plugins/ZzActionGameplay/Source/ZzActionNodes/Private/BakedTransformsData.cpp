// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "BakedTransformsData.h"

#include "Nodes/Anim/ZzActionAnimData.h"
#include "Nodes/Anim/ZzActionAnimFunctionLibrary.h"


void FZzBakedTransformsData::Bake(UAnimMontage* InAnimMontage, double InBeginTime, double InEndTime)
{
	BakedTimeRange = {InBeginTime, InEndTime};
	BakedTransformsWithRootMotion.Empty();
	BakedRootTransforms.Empty();
	
	if (!ensure(InAnimMontage))
	{
		return;
	}

	const FName RootBoneName = TEXT("root");
	
	double CurTime = InBeginTime;
	const double SubDeltaTime = 1.0 / FMath::Max(BakeFPS, 10);

	if (CurTime < InEndTime && SubDeltaTime > 0.0)
	{
		// The animation may not start at the origin, so preserve and compensate for the initial offset.
		const FTransform RootMotionOffset = UZzActionAnimFunctionLibrary::ExtractBoneTransform(InAnimMontage, RootBoneName, 0.f, false);
		const FTransform RootMotionWorldToLocal = RootMotionOffset.Inverse();
		
		while (CurTime <= InEndTime)
		{
			auto RootTF = UZzActionAnimFunctionLibrary::ExtractBoneTransform(InAnimMontage, RootBoneName, CurTime, false);
			auto BoneTFWithRootMotion = UZzActionAnimFunctionLibrary::ExtractBoneTransform(InAnimMontage, Socket, CurTime, false);

			BakedRootTransforms.Add(RootTF * RootMotionWorldToLocal);
			BakedTransformsWithRootMotion.Add(BoneTFWithRootMotion * RootMotionWorldToLocal);

			CurTime += SubDeltaTime;
		}
	}
}

void FZzBakedTransformsData::Bake(const UZzActionTimeline* InTimeline, double InBeginTime, double InEndTime)
{
	UAnimMontage* Montage = nullptr;
	if (InTimeline)
	{
		if (auto AnimData = UZzActionAnimData::GetAnimData(InTimeline))
		{
			Montage = AnimData->GetDefaultMontage();
		}
	}
	Bake(Montage, InBeginTime, InEndTime);
}

bool FZzBakedTransformsData::HasBakedTransforms() const
{
	if (BakedTransformsWithRootMotion.Num() == BakedRootTransforms.Num())
	{
		return !BakedTransformsWithRootMotion.IsEmpty() && BakedTimeRange.Size() > UE_KINDA_SMALL_NUMBER;
	}
	return false;
}

FTransform FZzBakedTransformsData::GetBakedTransform(float InCurrentTime, float InSubstepTime) const
{
	if (BakedRootTransforms.IsEmpty() || BakedTransformsWithRootMotion.Num() != BakedRootTransforms.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("FZzBakedTransformsData::GetBakedTransform: No valid baked transforms available."));
		return FTransform::Identity;
	}
	
	const int32 BakedNum = BakedRootTransforms.Num();

	auto GetInx = [&](float t)
	{
		const float Alpha = FMath::GetMappedRangeValueClamped(BakedTimeRange, FVector2D(0.0, 1.0), t);
		return FMath::FloorToInt((BakedNum - 1) * Alpha);
	};
	
	auto BoneTF = BakedTransformsWithRootMotion[GetInx(InSubstepTime)];
	auto RootMotionTF = BakedRootTransforms[GetInx(InCurrentTime)];
	return BoneTF * RootMotionTF.Inverse();
}

void FZzBakedTransformsData::DrawDebug(const UWorld* World) const
{
	FTransform PreviewMeshTF = FTransform::Identity;
	PreviewMeshTF.SetRotation(FRotator(0, -90, 0).Quaternion());

	for (int32 i = 0; i < BakedTransformsWithRootMotion.Num(); i++)
	{
		auto BoneTF = BakedTransformsWithRootMotion[i];
		BoneTF *= PreviewMeshTF;
		DrawDebugCoordinateSystem(World, BoneTF.GetLocation(), BoneTF.GetRotation().Rotator(), 10);
	}
}
