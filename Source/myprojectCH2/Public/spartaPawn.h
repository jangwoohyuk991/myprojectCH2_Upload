#pragma once

#include "CoreMinimal.h"// 언리얼에서 자주 사용하는 기본 타입과 매크로를 사용하기 위한 헤더
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "spartaPawn.generated.h"//UHT 생성한 코드를 포함 (Unreal h tool)의 약자

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS() // C++ 클래스를 언리얼의 리플렉션 시스템이 이식하도록 등록
class MYPROJECTCH2_API AspartaPawn : public APawn
{
    GENERATED_BODY()

public:
    AspartaPawn(); // 생성자

protected:
    virtual void BeginPlay() override; // Pawn을 조종하기 시작할때 호출되는 함수를 재정의하겠다고 선언

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    // 컴포넌트 선언
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCapsuleComponent* CapsuleComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComp;

    // Enhanced Input 프로퍼티
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;

    // 이동 및 회전 속도 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RotationSpeed = 100.0f;

private:
    // 입력 데이터를 저장할 변수
    FVector2D CurrentMoveInput;
    FVector2D CurrentLookInput;

    // 입력 처리 함수
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
}; 