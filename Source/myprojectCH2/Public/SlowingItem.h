#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowingItem.generated.h"

UCLASS()
class MYPROJECTCH2_API ASlowingItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ASlowingItem();

	// 50% 감속 (0.5)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff")
	float SlowRatio = 0.5f;

	// 5초간 디버프 유지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Debuff")
	float Duration = 5.0f;

	// BaseItem의 ActivateItem 오버라이드
	virtual void ActivateItem(AActor* Activator) override;
};