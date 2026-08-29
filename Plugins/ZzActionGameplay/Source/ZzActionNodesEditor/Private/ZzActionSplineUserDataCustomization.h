// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Spline/ZzActionSplineUserData.h"
#include "Misc/EngineVersionComparison.h"


class UZzActionSplineUserData;

class FZzActionSplineUserDataCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FZzActionSplineUserDataCustomization);
	}
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& Utils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& Utils) override;

private:
	TSharedPtr<IPropertyHandle> SplinePropertyHandle;
	
	UZzActionSplineUserData* GetSplineUserData() const;
	
	USplineComponent* GetLevelSelectedSplineComponent() const;
	
	bool HasSelectedSpline() const;
	
	FReply HandleSaveSplineData();
	
	FReply ExportToLevel();
	
	FReply ImportFromLevel();
	
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 7, 0)
	FReply HandleSelectAllSplinePoints();
	
	static class ISplineDetailsProvider* GetSplineDetailsProvider(const USplineComponent* SplineComponent);
#endif
};
