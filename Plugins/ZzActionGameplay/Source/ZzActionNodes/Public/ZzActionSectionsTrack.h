// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzActionTrack.h"
#include "Node/ZzActionNode.h"
#include "ZzActionSectionsTrack.generated.h"

USTRUCT(BlueprintType)
struct FActionNodeSectionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sections")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sections")
	double Time = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sections")
	FName LinkSection;
};

UCLASS()
class ZZACTIONNODES_API UZzActionSectionsNode : public UZzActionNode
{
	GENERATED_BODY()
public:
	UZzActionSectionsNode();
	
	virtual void OnBegin() override;
	virtual void OnTick(float DeltaTime) override;
	virtual double GetBeginTime() const override;
	virtual double GetEndTime() const override;
	
	bool JumpToSectionName(FName SectionName);
	
	FName GetSection_Link(FName SectionName) const;
	double GetSection_EndTime(FName SectionName) const;

	int32 GetSectionsNum() const;
	TArray<FActionNodeSectionData> GetSections(bool HasDefault = true) const;;
	const FActionNodeSectionData* GetSection(FName SectionName) const;
	
	TArray<FActionNodeSectionData> GetSortedSections() const;;
	
	const FActionNodeSectionData& GetSectionRef(int32 Inx) const;
	FActionNodeSectionData& GetSectionRef(int32 Inx);
	
	int32 GetNextSectionIndex(int32 Inx) const;
	int32 GetSectionIndexByName(FName SectionName) const;
	int32 GetSectionIndexByTime(double Time) const;

	void RenameSection(FName OldName, FName NewName);
//protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sections")
	FActionNodeSectionData DefaultSection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sections")
	TArray<FActionNodeSectionData> Sections;
	
	FName CurrentSectionName;
};


UCLASS(DisplayName = "Sections Track")
class ZZACTIONNODES_API UZzActionSectionsTrack : public UZzActionTrack
{
	GENERATED_BODY()
public:
	virtual UClass* GetNodeClass() const override { return UZzActionSectionsNode::StaticClass(); }
};

