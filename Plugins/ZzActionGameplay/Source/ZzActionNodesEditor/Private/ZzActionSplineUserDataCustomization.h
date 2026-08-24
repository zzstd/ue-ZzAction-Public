// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"


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
	
	FReply ExportToLevel();
	
	FReply ImportFromLevel();
	
	FReply HandleSelectAllSplinePoints();

	static class ISplineDetailsProvider* GetSplineDetailsProvider(const USplineComponent* SplineComponent);
};
