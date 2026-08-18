#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpartaPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class MYPROJECTCH2_API ASpartaPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASpartaPlayerController();
    // EditAnywhere, BlueprintReadWrite을 쓰면 디테일 패널에서 기본값을 자유롭게 수정하고
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* InputMappingContext;
    //동시에 블루프린트 그래프 안에서 노드로 가져다쓰거나 바꿀수도 있다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* SprintAction;

protected:
    virtual void BeginPlay() override;
};