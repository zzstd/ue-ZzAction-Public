// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzActionSectionsTrack.h"

#include "ZzActionInstance.h"
#include "Timeline/ZzActionTimeline.h"

UZzActionSectionsNode::UZzActionSectionsNode()
{
	DefaultSection.Name = TEXT("Default");
	DefaultSection.Time = 0.f;
}

void UZzActionSectionsNode::OnBegin()
{
	Super::OnBegin();
	
	CurrentSectionName = DefaultSection.Name;
}

void UZzActionSectionsNode::OnTick(float DeltaTime)
{
	Super::OnTick(DeltaTime);

	if (GetNodeContext().GetCurrentTime() >= GetSection_EndTime(CurrentSectionName))
	{
		JumpToSectionName(GetSection_Link(CurrentSectionName));
	}
}

double UZzActionSectionsNode::GetBeginTime() const
{
	return 0.f;
}

double UZzActionSectionsNode::GetEndTime() const
{
	if (auto Timeline = GetTimelineAsset())
	{
		return Timeline->GetLength();
	}
	
	return 0.f;
}

bool UZzActionSectionsNode::JumpToSectionName(FName SectionName)
{
	if (auto Data = GetSection(SectionName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Jump To %s"), *SectionName.ToString())
		CurrentSectionName = SectionName;
		GetNodeContext().GetActionInstance()->SetTime(Data->Time, true);
		return true;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Failed Jump To %s"), *SectionName.ToString())
	return false;
}

FName UZzActionSectionsNode::GetSection_Link(FName SectionName) const
{
	if (auto Data = GetSection(SectionName))
	{
		return Data->LinkSection;
	}

	return NAME_None;
}

double UZzActionSectionsNode::GetSection_EndTime(FName SectionName) const
{
	auto SortedSections = GetSortedSections();

	for (int32 i = 0; i < SortedSections.Num(); ++i)
	{
		if (SortedSections[i].Name == SectionName)
		{
			if (SortedSections.IsValidIndex(i + 1))
			{
				return SortedSections[i + 1].Time;
			}

			break;
		}
	}
	
	return GetEndTime();
}

int32 UZzActionSectionsNode::GetSectionsNum() const
{
	return Sections.Num() + 1;
}

TArray<FActionNodeSectionData> UZzActionSectionsNode::GetSections(bool HasDefault) const
{
	TArray<FActionNodeSectionData> Result;
	Result.Reserve(Sections.Num() + 1);
	
	if (HasDefault)
	{
		// always index is 0.
		Result.Add(DefaultSection);
	}
	
	Result.Append(Sections);
	return Result;
}

const FActionNodeSectionData* UZzActionSectionsNode::GetSection(FName SectionName) const
{
	if (DefaultSection.Name == SectionName)
	{
		return &DefaultSection;
	}
	
	for (auto& Data : Sections)
	{
		if (Data.Name == SectionName)
		{
			return &Data;
		}
	}

	return nullptr;;
}

TArray<FActionNodeSectionData> UZzActionSectionsNode::GetSortedSections() const
{
	auto SortedSections = GetSections();
	SortedSections.Sort([](const FActionNodeSectionData& A, const FActionNodeSectionData& B){ return A.Time < B.Time; });
	return SortedSections;
}

const FActionNodeSectionData& UZzActionSectionsNode::GetSectionRef(int32 Inx) const
{
	if (Inx == 0)
	{
		return DefaultSection;
	}
	
	check(Sections.IsValidIndex(Inx - 1));
	return Sections[Inx - 1];
}

FActionNodeSectionData& UZzActionSectionsNode::GetSectionRef(int32 Inx)
{
	if (Inx == 0)
	{
		return DefaultSection;
	}

	check(Sections.IsValidIndex(Inx - 1));
	return Sections[Inx - 1];
}

int32 UZzActionSectionsNode::GetNextSectionIndex(int32 Inx) const
{
	auto CurName = GetSectionRef(Inx).LinkSection;
	if (CurName != NAME_None)
	{
		for(int32 i = 0; i < GetSectionsNum(); ++i)
		{
			const auto& OtherSection = GetSectionRef(i);
			if(OtherSection.Name == CurName)
			{
				return i;
			}
		}
	}
	return INDEX_NONE;
}

int32 UZzActionSectionsNode::GetSectionIndexByName(FName SectionName) const
{
	if (DefaultSection.Name == SectionName)
	{
		return 0;
	}
	
	for (int32 i = 0; i < Sections.Num(); ++i)
	{
		if (Sections[i].Name == SectionName)
		{
			return i + 1;
		}
	}

	return INDEX_NONE;
}

int32 UZzActionSectionsNode::GetSectionIndexByTime(double Time) const
{
	auto TempSection = GetSortedSections();
	
	for (int32 i = 0; i < TempSection.Num(); ++i)
	{
		if (TempSection[i].Time <= Time)
		{
			if (!TempSection.IsValidIndex(i + 1))
			{
				return GetSectionIndexByName(TempSection[i].Name);
			}
			if (TempSection[i + 1].Time > Time)
			{
				return GetSectionIndexByName(TempSection[i].Name);
			}
		}
	}

	return INDEX_NONE;
}

void UZzActionSectionsNode::RenameSection(FName OldName, FName NewName)
{
	auto SetNewName = [&](FActionNodeSectionData& Section)
	{
		if (Section.Name == OldName)
		{
			Section.Name = NewName;
		}
		
		if (Section.LinkSection == OldName)
		{
			Section.LinkSection = NewName;
		}
	};
	
	SetNewName(DefaultSection);
	for (auto& Section : Sections)
	{
		SetNewName(Section);
	}
}
