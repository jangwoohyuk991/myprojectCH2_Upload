#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class MYPROJECTCH2_API ACoinItem : public ABaseItem
{
    GENERATED_BODY()
public:
    ACoinItem();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 PointValue; // 코인 점수

    virtual void ActivateItem(AActor* Activator) override;
};