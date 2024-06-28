#pragma once

class UReplaceConfig;
class UReplaceTypeDefault;
class UMyClass;



class MATREPLACE_API SMRDetailView : public SCompoundWidget, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SMRDetailView){}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	void ChangeType(TSubclassOf<UReplaceTypeDefault> NewType);
	virtual FString GetReferencerName() const override;

	TObjectPtr<UReplaceConfig> RepType;
	TObjectPtr<UReplaceTypeDefault> DefaultType;
	
	TSharedPtr<IDetailsView> ConfigPanel;
	TSharedPtr<IDetailsView> TypePanel;
};
