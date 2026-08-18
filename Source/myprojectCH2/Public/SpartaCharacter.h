#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue; // 이런 구조체 같은것들은 크기때문에 참조 안하면 객체의 모든 데이터를 복사해서 가져오게됨

UCLASS()
class MYPROJECTCH2_API ASpartaCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASpartaCharacter();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;

    // 스프린트 관련 속성
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float NormalSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintSpeedMultiplier;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    float SprintSpeed;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 입력 액션 처리 함수들 
    UFUNCTION() // 존재만 알려주는 형태로
        void Move(const FInputActionValue& Value);
    UFUNCTION()
    void StartJump(const FInputActionValue& Value);
    UFUNCTION()
    void StopJump(const FInputActionValue& Value);
    UFUNCTION()
    void Look(const FInputActionValue& Value);
    UFUNCTION()
    void StartSprint(const FInputActionValue& Value);
    UFUNCTION()
    void StopSprint(const FInputActionValue& Value);
};