#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class MYPROJECTCH2_API AMovingPlatform : public AActor
{
    GENERATED_BODY()

public:
    AMovingPlatform();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform|Components")
    USceneComponent* SceneRoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform|Components")
    UStaticMeshComponent* StaticMeshComp;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform|Properties")
    float MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform|Properties")
    float MaxRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform|Properties")
    FVector MoveDirection;

    FVector StartLocation;

    // --- 스폰 및 타이머 관련 멤버 변수 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform|Properties")
    bool bIsOriginal = false; // 원본 액터인지 확인하는 변수

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform|Properties")
    TSubclassOf<AMovingPlatform> PlatformSpawnClass; // 동적 스폰할 블루프린트 클래스

    FTimerHandle DirectionChangeTimerHandle;
    FTimerHandle DisappearTimerHandle;

    UFUNCTION()
    void ChangeDirection();

    void DestroyPlatform();
};