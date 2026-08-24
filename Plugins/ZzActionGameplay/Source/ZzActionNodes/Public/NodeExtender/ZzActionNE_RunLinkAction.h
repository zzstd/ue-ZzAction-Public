// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Node/ZzActionNodeExtender.h"
#include "ZzActionNE_RunLinkAction.generated.h"


// simple run link action
UCLASS()
class ZZACTIONNODES_API UZzActionNE_RunLinkAction : public UZzActionNodeExtender
{
	GENERATED_BODY()
public:
	virtual void OnBegin_Implementation() override;
	virtual void OnTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Link Action")
	FName TargetDataName;
	
	virtual UZzActionComponent* GetLinkActionComponent() const;
	
	virtual void RunAction(UZzActionBlueprint* InActionAsset, const FZzActionData& InLinkActionData);
	
	virtual void GetLinkActionData(FZzActionData& OutLinkActionData) const;
protected:
	UFUNCTION()
	void OnSetTime(UZzActionInstance* Instance, double NewTime, bool bTeleport);
	
	UPROPERTY()
	TObjectPtr<UZzActionInstance> LinkActionInstance;
};