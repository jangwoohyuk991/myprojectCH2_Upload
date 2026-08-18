#include "SpartaCharacter.h"
#include "SpartaPlayerController.h" // 컨트롤러 액션 접근을 위해 필요
#include "EnhancedInputComponent.h" // 강화된 입력 시스템
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h" // 이동 속도 제어

ASpartaCharacter::ASpartaCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // 카메라 및 스프링 암 설정
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    // 스프린트 관련 변수 초기화
    NormalSpeed = 600.0f;
    SprintSpeedMultiplier = 1.5f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced InputComponent로 캐스팅
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // 컨트롤러 캐스팅
        if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
                EnhancedInput->BindAction
                (PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);

            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction
                (PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartJump);
                EnhancedInput->BindAction
                (PlayerController->JumpAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
            }

            if (PlayerController->LookAction)
                EnhancedInput->BindAction
                (PlayerController->LookAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Look);

            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction
                (PlayerController->SprintAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartSprint);
                EnhancedInput->BindAction
                (PlayerController->SprintAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopSprint);
            }
        }
    }
}
// 기능 구현부 !FMath::IsNearlyZero(MoveInput:항상 0 딱 숫자가 떨어지기가 쉽지 않기에
// 혹시나 가까운 값이 들어왔을 때 작은 오차가 있더라도 일단 0으로 해주자(0이라는 얘기는 키를 안눌렀다는뜻)
void ASpartaCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return; // GetActorForwardVector,GetActorRightVector이걸 가져오려면 !Controller 이걸 한번 체크해줘야한다
    const FVector2D MoveInput = Value.Get<FVector2D>();
    if (!FMath::IsNearlyZero(MoveInput.X)) AddMovementInput(GetActorForwardVector(), MoveInput.X);
    if (!FMath::IsNearlyZero(MoveInput.Y)) AddMovementInput(GetActorRightVector(), MoveInput.Y);
}
// AddMovementInput: 직접 귀찮은걸 구현해야하는걸 이 한줄로 모든걸 해결
void ASpartaCharacter::StartJump(const FInputActionValue& Value) {
    if (Value.Get<bool>()) Jump();
}
void ASpartaCharacter::StopJump(const FInputActionValue& Value) {
    if (!Value.Get<bool>()) StopJumping();
}

//여기선 딱히 안쓰기때문에  !Controller 체크 안해줘도 된다.
void ASpartaCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookInput = Value.Get<FVector2D>();
    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}
void ASpartaCharacter::StopSprint(const FInputActionValue& Value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}