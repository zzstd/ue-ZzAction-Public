// Copyright 2024-2026 zz studio. All Rights Reserved.

#include "Niagara/ZzNotifyNode_NiagaraEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraSystemInstanceController.h"
#include "Timeline/ZzActionTimelineUtils.h"

UZzNotifyNode_NiagaraEffect::UZzNotifyNode_NiagaraEffect()
	: bAbsoluteScale(false)
{
	Tags.AddUnique(TEXT("Fx"));
	InstancingPolicy = EZzActionNodeInstancingPolicy::InstancedPerExecution;

	FXComponent = nullptr;
}

FLinearColor UZzNotifyNode_NiagaraEffect::GetNotifyColor_Implementation() const
{
	return FColor(192, 255, 99, 255);
}

FText UZzNotifyNode_NiagaraEffect::GetNotifyName_Implementation() const
{
	if (Template)
	{
		return FText::FromString(Template->GetName());
	}
	
	return Super::GetNotifyName_Implementation();
}

void UZzNotifyNode_NiagaraEffect::OnBegin()
{
	Super::OnBegin();

	if (FXComponent)
	{
		FXComponent->DeactivateImmediate();
	}

	if (!GetNodeContext().IsPreview())
	{
		FXComponent = SpawnEffect(GetNodeContext().TryGetSkeletonComponent());
	}
}

void UZzNotifyNode_NiagaraEffect::OnEnd(bool Abort)
{
	Super::OnEnd(Abort);

	if (FXComponent)
	{
		if (bTimedEffect ||
			(bDestroyPointEffectAtAborted && Abort))
		{
			if (bDestroyAtEnd)
			{
				FXComponent->DestroyComponent();
			}
			else
			{
				FXComponent->Deactivate();
			}
		}
	}
}

bool UZzNotifyNode_NiagaraEffect::IsPoint() const
{
	return !bTimedEffect;
}

#if WITH_EDITOR
namespace ZZ
{
void SetNiagaraDesiredAge(UNiagaraComponent* NiagaraComponent, float DesiredAge)
{
	ENiagaraAgeUpdateMode AgeUpdateMode = ENiagaraAgeUpdateMode::DesiredAge;
	bool bAllowScalability = false;

	NiagaraComponent->SetForceSolo(true);
	NiagaraComponent->SetAgeUpdateMode(AgeUpdateMode);
	NiagaraComponent->SetAllowScalability(bAllowScalability);

	{
		NiagaraComponent->SetSeekDelta(ZZ::TimelineUtils::GetDeltaTime());
		NiagaraComponent->SetLockDesiredAgeDeltaTimeToSeekDelta(false);
	}

	bool bRenderingEnabled = true;
	NiagaraComponent->SetRenderingEnabled(bRenderingEnabled);

	if (DesiredAge >= 0)
	{
		NiagaraComponent->SetDesiredAge(DesiredAge);
	}
}
}

void UZzNotifyNode_NiagaraEffect::OnPreviewEditorTick(float NewPos, bool bPlaying) const
{
	Super::OnPreviewEditorTick(NewPos, bPlaying);

	{
		FString ErrMsg;
		ValidateParameters(GetNodeContext().TryGetSkeletonComponent(), &ErrMsg);
		GEngine->AddOnScreenDebugMessage(8888, 0.0f, FColor(255, 48, 16), *ErrMsg);
	}

	double FxDesiredAge = GetEndTime() - GetBeginTime();
	if (!bTimedEffect)
	{
		// @TODO: Get niagara desired age.
		FxDesiredAge = 10.f; 
	}
	
	const auto PreviewMeshComp = GetNodeContext().TryGetSkeletonComponent();
	
	auto MyOffset = GetSpawnEffectOffset();
	const bool bFxAgeInRange = ZZ::TimelineUtils::TimeInRange(GetBeginTime(), GetBeginTime() + FxDesiredAge, NewPos);
	
	if (EditorPreviewFxComp)
	{
		bool bShouldRespawnFx = !bEditorPreview;
		bShouldRespawnFx |= !bFxAgeInRange;
		bShouldRespawnFx |= EditorPreviewFxComp->GetFXSystemAsset() != Template;
		bShouldRespawnFx |= (EditorPreviewFxComp->GetAttachParent() != nullptr) != bAttached;
		bShouldRespawnFx |= !bAttached && !EditorPreviewCachedSpawnOffset.Equals(MyOffset);
		if (bAttached)
		{
			bShouldRespawnFx |= EditorPreviewFxComp->GetAttachSocketName() != SocketName;
		}
		else
		{
			bShouldRespawnFx |= !EditorPreviewCachedSpawnOffset.Equals(MyOffset);
		}

		if (bShouldRespawnFx)
		{
			EditorPreviewFxComp->DeactivateImmediate();
			EditorPreviewFxComp = nullptr;
		}
	}
	
	if (bFxAgeInRange && bEditorPreview)
	{
		if (!EditorPreviewFxComp)
		{
			EditorPreviewCachedSpawnOffset = MyOffset;
			EditorPreviewFxComp = SpawnEffect(PreviewMeshComp);
		}

		if (EditorPreviewFxComp)
		{
			if (bAttached && !EditorPreviewFxComp->GetRelativeTransform().Equals(MyOffset))
			{
				EditorPreviewFxComp->SetRelativeTransform(MyOffset);
			
				FNiagaraSystemInstanceControllerPtr SystemInstanceController = EditorPreviewFxComp->GetSystemInstanceController();
				if (SystemInstanceController.IsValid())
				{
					SystemInstanceController->Reset(FNiagaraSystemInstance::EResetMode::ResetAll);
				}
			}
			
			ZZ::SetNiagaraDesiredAge(EditorPreviewFxComp, NewPos - GetBeginTime());
		}
	}
}

void UZzNotifyNode_NiagaraEffect::OnPreviewEditorCleanup() const
{
	if (EditorPreviewFxComp)
	{
		EditorPreviewFxComp->DeactivateImmediate();
		EditorPreviewFxComp = nullptr;
	}
}

void UZzNotifyNode_NiagaraEffect::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (!IsPoint() && GetLength() <= KINDA_SMALL_NUMBER)
	{
		SetEndTime(GetBeginTime() + 0.1f);
	}
}
#endif

UNiagaraComponent* UZzNotifyNode_NiagaraEffect::SpawnEffect(USkeletalMeshComponent* MeshComp) const
{
	UNiagaraComponent* ReturnComp = nullptr;
	
	auto MyOffset = GetSpawnEffectOffset();
	if (ValidateParameters(MeshComp))
	{
		if (bAttached)
		{
			ReturnComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				Template
				, MeshComp, SocketName
				, MyOffset.GetLocation()
				, MyOffset.GetRotation().Rotator()
				, EAttachLocation::KeepRelativeOffset
				, !bDestroyAtEnd);
		}
		else
		{
			const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
			ReturnComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				MeshComp, 
				Template, 
				MeshTransform.TransformPosition(MyOffset.GetLocation()), 
				(MeshTransform.GetRotation() * MyOffset.GetRotation()).Rotator(), 
				FVector(1.0f),true);
		}
		
		if (ReturnComp != nullptr)
		{
			ReturnComp->SetUsingAbsoluteScale(bAbsoluteScale);
			ReturnComp->SetRelativeScale3D_Direct(MyOffset.GetScale3D());
		}
	}
	
	return ReturnComp;
}

FTransform UZzNotifyNode_NiagaraEffect::GetSpawnEffectOffset_Implementation() const
{
	return Offset;
}

bool UZzNotifyNode_NiagaraEffect::ValidateParameters(USkeletalMeshComponent* MeshComp, FString* ErrMsg) const
{
	if (!Template)
	{
		if (ErrMsg)
		{
			*ErrMsg = TEXT("Spawn niagara failed: No Set Asset.");
		}
		return false;
	}
	
	if (!MeshComp)
	{
		if (ErrMsg)
		{
			*ErrMsg =TEXT("Spawn niagara failed: No Mesh Component.");
		}
		return false;
	}

	return true;
}
