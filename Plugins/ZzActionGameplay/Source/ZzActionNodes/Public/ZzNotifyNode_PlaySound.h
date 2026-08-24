// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/ZzActionNotifyTrack.h"
#include "ZzNotifyNode_PlaySound.generated.h"

UCLASS(DisplayName="Play Sound")
class ZZACTIONNODES_API UZzNotifyNode_PlaySound : public UZzActionNotifyNode
{
	GENERATED_BODY()
public:
	UZzNotifyNode_PlaySound();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NotifyNode")
	TObjectPtr<class USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NotifyNode")
	float VolumeMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NotifyNode")
	float PitchMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NotifyNode")
	uint32 bFollow:1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NotifyNode", meta=(EditCondition="bFollow", ExposeOnSpawn = true))
	FName AttachName;
	
	virtual FLinearColor GetNotifyColor_Implementation() const override;
	virtual FText GetNotifyName_Implementation() const override;
	virtual void OnBegin(const FZzActionNodeContext& Context) const override;
};
