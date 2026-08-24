// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "MotionWarping/ZzActionNE_MW_Transform.h"
#include "MotionWarping/ZzNotifyNode_MotionWarping.h"
#include "ZzActionDataFunctionLibrary.h"

#include "MotionWarpingComponent.h"
#include "RootMotionModifier_SkewWarp.h"
#include "ZzActionHelperUtils.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Nodes/Anim/ZzActionAnimFunctionLibrary.h"

namespace ZzActionCVars
{
	static int32 ShowDebugMotionWarpTransform = 0;
	FAutoConsoleVariableRef CVarShowDebugMotionWarpTransform(
		TEXT("z.ShowDebugMotionWarpTransform"),
		ShowDebugMotionWarpTransform,
		TEXT("Show motion warping transform debug info.\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);
}

void UZzActionNE_MW_TransformBase::OnPreBegin_Implementation()
{
	if (GetNodeContext().IsPreview())
	{
		return;
	}
	
	UpdateWarpTarget();
	LastOwnerTransform = GetNodeContext().GetOwnedActor()->GetActorTransform();
}

void UZzActionNE_MW_TransformBase::OnPreTick_Implementation(float DeltaTime)
{
	if (GetNodeContext().IsPreview())
	{
		return;
	}

	if (bTickUpdateTargetTransform || ZzActionCVars::ShowDebugMotionWarpTransform)
	{
		FTransform CurrentTF = GetNodeContext().GetOwnedActor()->GetActorTransform();
		MovingDist += (CurrentTF.GetLocation() - LastOwnerTransform.GetLocation()).Size();
		LastOwnerTransform = CurrentTF;
	}
	
	if (bTickUpdateTargetTransform)
	{
		UpdateWarpTarget();
	}
}

void UZzActionNE_MW_TransformBase::OnEnd_Implementation(bool Abort)
{
	if (GetNodeContext().IsPreview())
	{
		return;
	}
	
	ClearWarpTarget();
	
#if ENABLE_DRAW_DEBUG
	if (ZzActionCVars::ShowDebugMotionWarpTransform && MaxWarpDistance >= 0.f)
	{
		DrawDebugString(GetWorld(), GetNodeContext().GetOwnedActor()->GetActorLocation()
			, FString::Printf(TEXT("Warp Dist: %.2f (%.2f)"), MovingDist, MaxWarpDistance > 0 ? MovingDist / MaxWarpDistance : 0.f), nullptr, FColor::White, 5.f, true);
	}
#endif
}

void UZzActionNE_MW_TransformBase::AlignToPreviewScene()
{
	//todo
}

void UZzActionNE_MW_TransformBase::UpdateWarpTarget()
{
	FTransform TargetTransform;
	if (!GetBaseTransform(TargetTransform))
	{
		ClearWarpTarget();
		return;
	}
	
	const FVector OwnerLocation = GetNodeContext().GetOwnedActor()->GetActorLocation();

	if (!bUseTargetRotationSpace)
	{
		FVector ToTargetDirection = (TargetTransform.GetLocation() - OwnerLocation).GetSafeNormal2D();
		TargetTransform.SetRotation(ToTargetDirection.ToOrientationQuat());
	}

	FTransform DesiredWarpTF = FTransform(RotationOffset, LocationOffset) * TargetTransform;
	
	if (bEnableMinWarpDistance)
	{
		FVector TargetToDes = DesiredWarpTF.GetLocation() - TargetTransform.GetLocation();

		float TargetToOwnerDis = (OwnerLocation - TargetTransform.GetLocation()).Size(); 
		float ToTargetMaxDis = TargetToOwnerDis - FMath::Min(TargetToDes.Size() - TargetToOwnerDis, MinWarpDistance);
		ToTargetMaxDis = FMath::Max(ToTargetMaxDis, 0.f);
		
		DesiredWarpTF.SetLocation(TargetTransform.GetLocation() + TargetToDes.GetClampedToMaxSize(ToTargetMaxDis));
	}
	
	if (MaxWarpDistance > 0)
	{
		FVector ToDesiredDelta = DesiredWarpTF.GetLocation() - OwnerLocation;
		DesiredWarpTF.SetLocation(OwnerLocation + ToDesiredDelta.GetClampedToMaxSize(MaxWarpDistance - MovingDist));
	}

#if ENABLE_DRAW_DEBUG
	if (ZzActionCVars::ShowDebugMotionWarpTransform)
	{
		FVector DrawBoxLocalPos = TargetTransform.InverseTransformPosition(DesiredWarpTF.GetLocation());
		
		DrawDebugBox(GetWorld(), TargetTransform.TransformPosition(DrawBoxLocalPos * 0.5), DrawBoxLocalPos * 0.5, TargetTransform.GetRotation(), FColor::White, false, 5);
		DrawDebugCoordinateSystem(GetWorld(), DesiredWarpTF.GetLocation(), DesiredWarpTF.GetRotation().Rotator(), 50.f, false, 5);
	}
#endif
	
	UpdateWarpTargetFromTransform(DesiredWarpTF);
}

bool UZzActionNE_MW_TransformBase::GetBaseTransform(FTransform& OutTransform) const
{
	checkNoEntry()
	return false;
}

bool UZzActionNE_MW_TransformBase::GetWarpTargetName(FName& OutName) const
{
	if (auto MotionWarpingNode = GetOwnerNode<UZzNotifyNode_MotionWarping>())
	{
		if (auto WarpModifier = Cast<URootMotionModifier_Warp>(MotionWarpingNode->RootMotionModifier))
		{
			OutName = WarpModifier->WarpTargetName;
			return true;
		}
	}

	return false;
}

void UZzActionNE_MW_TransformBase::UpdateWarpTargetFromTransform(const FTransform& TargetTransform)
{
	FName WarpTargetName;
	if (GetWarpTargetName(WarpTargetName))
	{
		if (auto MotionWarpComponent = GetNodeContext().GetOwnedActor()->FindComponentByClass<UMotionWarpingComponent>())
		{
			MotionWarpComponent->AddOrUpdateWarpTargetFromTransform(WarpTargetName, TargetTransform);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No MotionWarpingComponent"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Target Name"))
	}
	
	CachedWarpTargetTransform = TargetTransform;
	UpdateWarpTargetDelegate.Broadcast();
}

void UZzActionNE_MW_TransformBase::ClearWarpTarget()
{
	FName WarpTargetName;
	if (GetWarpTargetName(WarpTargetName))
	{
		if (auto MotionWarpComponent = GetNodeContext().GetOwnedActor()->FindComponentByClass<UMotionWarpingComponent>())
		{
			MotionWarpComponent->RemoveWarpTarget(WarpTargetName);
		}
	}

	CachedWarpTargetTransform.Reset();
	UpdateWarpTargetDelegate.Broadcast();
}

#if WITH_EDITOR
void UZzActionNE_MW_TransformBase::OnPreviewEditorTick(const FZzActionNodePreviewContext& PreviewContext) const
{
	Super::OnPreviewEditorTick(PreviewContext);
	
	FName WarpTargetName;
	if (!GetWarpTargetName(WarpTargetName))
	{
		GEngine->AddOnScreenDebugMessage((uint64)this, 0, FColor::Red, FString::Printf(TEXT("MW_Transform: No valid warp target name")));
		return;
	}
	
	const auto NodeContext = GetNodeContext();
	
	FTransform StartTF = ZZ::Action::ExtractRootMotionWithCharacter(NodeContext.GetTimeline(), 0, NodeContext.GetNode()->GetBeginTime());
	FTransform EndTF = ZZ::Action::ExtractRootMotionWithCharacter(NodeContext.GetTimeline(), 0, NodeContext.GetNode()->GetEndTime());
	{
		auto Dist = (StartTF.GetLocation() - EndTF.GetLocation()).Size();
		GEngine->AddOnScreenDebugMessage((uint64)this + 1, 0, FColor::Orange, FString::Printf(TEXT("Anim Dist: %.2f --- %.2f (%.2f)"), Dist, MaxWarpDistance
			, Dist > 0.f ? MaxWarpDistance / Dist : 0.f));
	}

	float DrawTh = PreviewContext.bSelected ? 1 : 0;
	
#if ENABLE_DRAW_DEBUG
	if (bShowPreviewTransform)
	{
		FTransform LocalToWorld;
		if (ZZ::Action::Helper::GetPreviewStartWorldTransform(NodeContext, false, LocalToWorld))
		{
			FTransform PreviewEndLocalTF(EndTF);
			if (!bUseTargetRotationSpace)
			{
				const FVector ToTargetDirection = (EndTF.GetLocation() - StartTF.GetLocation()).GetSafeNormal2D();
				PreviewEndLocalTF.SetRotation(ToTargetDirection.ToOrientationQuat());
			}
			const FTransform PreviewTargetTF = FTransform(RotationOffset, LocationOffset).Inverse() * PreviewEndLocalTF;
			const FVector PreviewTargetLoc = PreviewTargetTF.GetLocation();
			if (bShowPreviewCapsule)
			{
				// 1) draw character capsule
				constexpr float PreviewCapsuleHalfHeight = 90.f;
				constexpr float PreviewCapsuleRadius = 34.f;
				const FColor PreviewTargetColor = FColor::White;

				auto PreviewCharacterOffset = bUseTargetFeetLocation ? FVector(0, 0, PreviewCapsuleHalfHeight) : FVector::ZeroVector;
				DrawDebugCapsule(GetWorld(), LocalToWorld.TransformPosition(PreviewTargetLoc + PreviewCharacterOffset)
					, PreviewCapsuleHalfHeight, PreviewCapsuleRadius, bUseTargetRotationSpace ? PreviewTargetTF.GetRotation() : FQuat::Identity, PreviewTargetColor);
			}
			{
				// 2) draw offset box
				const FTransform TargetWarpHalfTF = FTransform(RotationOffset, LocationOffset * 0.5).Inverse() * PreviewEndLocalTF * LocalToWorld;
				DrawDebugBox(GetWorld(), TargetWarpHalfTF.GetLocation(), LocationOffset * 0.5, TargetWarpHalfTF.GetRotation(), FColor::White, false, -1, SDPG_Foreground, DrawTh);
				if (bUseTargetRotationSpace)
				{
					DrawDebugDirectionalArrow(GetWorld(), LocalToWorld.TransformPosition(PreviewTargetLoc)
						, LocalToWorld.TransformPosition(PreviewTargetLoc + PreviewTargetTF.GetRotation().GetForwardVector() * 50), 30, FColor::Red, false, -1, SDPG_Foreground, DrawTh);
				}
			}
			const FVector StartLocWP = LocalToWorld.TransformPosition(StartTF.GetLocation());
			{
				// 3) draw start and end point
				const FVector PreviewEndWP = LocalToWorld.TransformPosition(PreviewEndLocalTF.GetLocation());
				
				DrawDebugSphere(GetWorld(), StartLocWP, 10, 6, FColor::White);
				DrawDebugSphere(GetWorld(), PreviewEndWP, 10, 6, FColor::Blue, false, -1, SDPG_Foreground);
				DrawDebugDirectionalArrow(GetWorld(), PreviewEndWP
					, LocalToWorld.TransformPosition(PreviewEndLocalTF.TransformPosition({50, 0 ,0}))
					, 30, FColor::Blue, false, -1, SDPG_Foreground, DrawTh);
			}
			{
				// 4) draw max warp distance
				FVector PreviewEndDirWP = LocalToWorld.TransformRotation(PreviewEndLocalTF.GetRotation()).GetForwardVector();
				
				auto DrawDebugArc = [this](const FVector& Center, float Radius, const FVector& Direction, const FColor& Color)
				{
					constexpr uint8 DepthPriority = SDPG_Foreground;
					
					const FVector LineCenter = Center + Direction * Radius;
					DrawDebugLine(GetWorld(), Center, LineCenter, Color, false, -1, DepthPriority);
					
					if (Radius < 100)
					{
						const FVector LineOffsetA = Direction.RotateAngleAxis(100, FVector::UpVector) * 20;
						const FVector LineOffsetB = Direction.RotateAngleAxis(-100, FVector::UpVector) * 20;
						
						DrawDebugLine(GetWorld(), LineCenter, LineCenter + LineOffsetA, Color, false, -1, DepthPriority);
						DrawDebugLine(GetWorld(), LineCenter, LineCenter + LineOffsetB, Color, false, -1, DepthPriority);
					}
					else
					{
						DrawDebugCircleArc(GetWorld(), Center, Radius, Direction, 0.2f, 5, Color, false, -1, DepthPriority);
					}
				};
				
				if (MaxWarpDistance > 0.f)
				{
					DrawDebugArc(StartLocWP + FVector(0,0,5), MaxWarpDistance, PreviewEndDirWP, FColor::Orange);
				}
				if (bEnableMinWarpDistance)
				{
					DrawDebugArc(StartLocWP + FVector(0,0,5), FMath::Abs(MinWarpDistance), MinWarpDistance < 0 ? -PreviewEndDirWP : PreviewEndDirWP, FColor::Turquoise);
				}
			}
		}
	}
#endif
}
#endif

bool UZzActionNE_MW_SimpleTransform::GetBaseTransform(FTransform& OutTransform) const
{
	FName DataName;
	if (bUseWarpTargetName)
	{
		if (!GetWarpTargetName(DataName))
		{
			UE_LOG(LogTemp, Warning, TEXT("No Warp Target Name"))
			return false;
		}
	}
	else
	{
		DataName = CustomDataName;
	}

	if (DataName.IsNone())
	{
		return false;
	}

	if (bUseTargetFeetLocation)
	{
		if (auto Character = GetNodeContext().GetActionData().GetObject<ACharacter>(DataName))
		{
			OutTransform = Character->GetActorTransform();
			OutTransform.AddToTranslation(FVector(0,0, -Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
			return true;
		}
	}
	
	return UZzActionDataFunctionLibrary::GetActionData__FindTransform(this, DataName, OutTransform);
}


UE_ENABLE_OPTIMIZATION