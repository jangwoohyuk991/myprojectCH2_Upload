#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ReverseControlItem.generated.h"

UCLASS()
class MYPROJECTCH2_API AReverseControlItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AReverseControlItem();

	// 5초간 조작 반전 유지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff")
	float Duration = 5.0f;

	// BaseItem의 ActivateItem 오버라이드
	virtual void ActivateItem(AActor* Activator) override;
};