#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealingItem.generated.h"

UCLASS()
class MYPROJECTCH2_API AHealingItem : public ABaseItem
{
    GENERATED_BODY()
public:
    AHealingItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 HealAmount; // È¸º¹·® (20)

    virtual void ActivateItem(AActor* Activator) override;
};

