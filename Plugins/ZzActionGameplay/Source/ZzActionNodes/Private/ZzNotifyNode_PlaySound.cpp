// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzNotifyNode_PlaySound.h"

#include "ZzActionBlueprint.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/SkeletalMeshComponent.h"

UZzNotifyNode_PlaySound::UZzNotifyNode_PlaySound()
{
	bPoint = true;
	InstancingPolicy = EZzActionNodeInstancingPolicy::NonInstanced;
	Tags.AddUnique(TEXT("Sound"));
	
	VolumeMultiplier = 1.f;
	PitchMultiplier = 1.f;

	bFollow = false;
}

FLinearColor UZzNotifyNode_PlaySound::GetNotifyColor_Implementation() const
{
	return FColor(196, 142, 255, 255);
}

FText UZzNotifyNode_PlaySound::GetNotifyName_Implementation() const
{
	if (Sound)
	{
		return FText::FromString(Sound->GetName());
	}
	
	return Super::GetNotifyName_Implementation();
}

void UZzNotifyNode_PlaySound::OnBegin(const FZzActionNodeContext& Context) const
{
	auto SkMesh = Context.TryGetSkeletonComponent();
	if (Sound && SkMesh)
	{
		if (!Sound->IsOneShot())
		{
			UE_LOG(LogTemp, Warning, TEXT("PlaySound notify: ZzAction Asset %s tried to play a sound asset which is not a one-shot: '%s'. Spawning suppressed."), *Context.GetAssetName(), *GetNameSafe(Sound));
			return;
		}
		
		if (bFollow)
		{
			UGameplayStatics::SpawnSoundAttached(Sound, SkMesh, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(SkMesh, Sound, SkMesh->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
		}
		
	}
	
	Super::OnBegin(Context);
}