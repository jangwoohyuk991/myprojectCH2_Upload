#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "item.generated.h"

UCLASS()
class MYPROJECTCH2_API AItem : public AActor
{
    GENERATED_BODY()

public:
    AItem();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Components")
    USceneComponent* SceneRoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Components")
    UStaticMeshComponent* StaticMeshComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties")
    float RotationSpeed;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    UFUNCTION(BlueprintCallable, Category = "Item|Actions")
    void ResetActorPosition();

    UFUNCTION(BlueprintPure, Category = "Item|Properties")
    float GetRotationSpeed() const;

    UFUNCTION(BlueprintImplementableEvent, Category = "Item|Event")
    void OnItemPickedUp();
};