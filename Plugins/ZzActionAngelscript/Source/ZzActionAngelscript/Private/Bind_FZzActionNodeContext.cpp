// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "as_scriptengine.h"
#include "AngelscriptBinds.h"
#include "AngelscriptDocs.h"
#include "AngelscriptManager.h"

#include "ZzActionType.h"
#include "Node/ZzActionNode.h"


AS_FORCE_LINK const FAngelscriptBinds::FBind Bind_FZzActionNodeContext(FAngelscriptBinds::EOrder::Late, []
{
	auto FZzActionNodeContext_ = FAngelscriptBinds::ExistingClass("FZzActionNodeContext");

	FZzActionNodeContext_.Method("bool IsValid() const", METHOD_TRIVIAL(FZzActionNodeContext, IsValid));
	
	FZzActionNodeContext_.Method("UZzActionTimeline GetTimeline() const", METHOD_TRIVIAL(FZzActionNodeContext, GetTimeline));
	FZzActionNodeContext_.Method("UZzActionInstance GetActionInstance() const", METHOD_TRIVIAL(FZzActionNodeContext, GetActionInstance));
	FZzActionNodeContext_.Method("UZzActionComponent GetActionComponent() const", METHOD_TRIVIAL(FZzActionNodeContext, GetActionComponent));
	FZzActionNodeContext_.Method("UZzActionAnimRuntime GetAnimRuntime(bool bCreateIfNone = false) const", METHOD_TRIVIAL(FZzActionNodeContext, GetAnimRuntime));
	FZzActionNodeContext_.Method("double GetCurrentTime() const", METHOD_TRIVIAL(FZzActionNodeContext, GetCurrentTime));
	FZzActionNodeContext_.Method("AActor GetOwnedActor() const", METHOD_TRIVIAL(FZzActionNodeContext, GetOwnedActor));
	FZzActionNodeContext_.Method("int32 GetNodeUniqueID() const", METHOD_TRIVIAL(FZzActionNodeContext, GetNodeUniqueID));
	FZzActionNodeContext_.Method("const FZzActionData& GetActionData() const", METHOD_TRIVIAL(FZzActionNodeContext, GetActionData));
	FZzActionNodeContext_.Method("USkeletalMeshComponent TryGetSkeletonComponent() const", METHOD_TRIVIAL(FZzActionNodeContext, TryGetSkeletonComponent));
	FZzActionNodeContext_.Method("bool IsPreview() const", METHOD_TRIVIAL(FZzActionNodeContext, IsPreview));
	FZzActionNodeContext_.Method("float32 GetPlayRate() const", METHOD_TRIVIAL(FZzActionNodeContext, GetPlayRate));
	FZzActionNodeContext_.Method("FString GetAssetName() const", METHOD_TRIVIAL(FZzActionNodeContext, GetAssetName));
	FZzActionNodeContext_.Method("UZzActionNode GetNode() const", METHOD_TRIVIAL(FZzActionNodeContext, GetNode));
	
	FZzActionNodeContext_.Method("UZzActionNode GetNodeAs(const TSubclassOf<UZzActionNode>& Class) const", [](const FZzActionNodeContext* Self, const TSubclassOf<UZzActionNode>& Class) -> UZzActionNode*
	{
		if (auto Node = Self->GetNode())
		{
			if (Class == nullptr || Node->IsA(Class))
			{
				return Node;
			}
		}
		return nullptr;
	});
	FAngelscriptBinds::SetPreviousBindArgumentDeterminesOutputType(0);
	
	FZzActionNodeContext_.Method("bool NodeIsA(const UClass InNodeClass) const", METHOD_TRIVIAL(FZzActionNodeContext, NodeIsA));
	
	FZzActionNodeContext_.Method("void AddFloat(FName Name, float32 Value) const", METHOD_TRIVIAL(FZzActionNodeContext, AddFloat));
	FZzActionNodeContext_.Method("void AddInt(FName Name, int32 Value) const", METHOD_TRIVIAL(FZzActionNodeContext, AddInt));
	FZzActionNodeContext_.Method("void AddVector(FName Name, FVector Value) const", METHOD_TRIVIAL(FZzActionNodeContext, AddVector));
	FZzActionNodeContext_.Method("void AddTransform(FName Name, FTransform Value) const", METHOD_TRIVIAL(FZzActionNodeContext, AddTransform));
	FZzActionNodeContext_.Method("void AddObject(FName Name, UObject Value) const", METHOD_TRIVIAL(FZzActionNodeContext, AddObject));
});
