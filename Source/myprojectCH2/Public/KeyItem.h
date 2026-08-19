#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "KeyItem.generated.h"

UCLASS()
class MYPROJECTCH2_API AKeyItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AKeyItem();

	// 부모(BaseItem)의 3-2 매개변수 형태와 동일하게 맞춥니다.
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void ActivateItem(AActor* Activator) override;
};