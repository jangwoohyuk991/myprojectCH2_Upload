#include "SpartaPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "SpartaPlayerController.h"
#include "InputActionValue.h"

ASpartaPawn::ASpartaPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    RootComponent = CapsuleComp;
    CapsuleComp->InitCapsuleSize(34.0f, 88.0f);
    CapsuleComp->SetSimulatePhysics(false);

    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(CapsuleComp);
    MeshComp->SetSimulatePhysics(false);
    MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(CapsuleComp);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;
}

void ASpartaPawn::BeginPlay()
{
    Super::BeginPlay();
}

void ASpartaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (ASpartaPlayerController* PC = Cast<ASpartaPlayerController>(GetController()))
        {
            if (PC->MoveAction)
            {
                EnhancedInput->BindAction(PC->MoveAction, ETriggerEvent::Triggered, this, &ASpartaPawn::Move);
                EnhancedInput->BindAction(PC->MoveAction, ETriggerEvent::Completed, this, &ASpartaPawn::Move);
            }
            if (PC->LookAction)
            {
                EnhancedInput->BindAction(PC->LookAction, ETriggerEvent::Triggered, this, &ASpartaPawn::Look);
            }
        }
    }
}

void ASpartaPawn::Move(const FInputActionValue& Value)
{
    MoveInput = Value.Get<FVector2D>();
}

void ASpartaPawn::Look(const FInputActionValue& Value)
{
    LookInput = Value.Get<FVector2D>();
}

void ASpartaPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 1. 마우스 회전 입력 처리 (컨트롤러 회전 변경)
    if (!LookInput.IsZero())
    {
        AddControllerYawInput(LookInput.X);
        AddControllerPitchInput(LookInput.Y);
        LookInput = FVector2D::ZeroVector; // 입력 초기화
    }

    // 2. 이동 처리 (벽 충돌 방지 Sweep 적용)
    if (!MoveInput.IsZero())
    {
        const FRotator ControlRot = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;
        const FRotator YawRot(0.0f, ControlRot.Yaw, 0.0f);

        const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

        FVector MoveDirection = (ForwardDir * MoveInput.X + RightDir * MoveInput.Y);
        MoveDirection.Z = 0.0f;
        MoveDirection.Normalize();

        // 이동할 실제 거리 계산
        FVector DeltaLocation = MoveDirection * MoveSpeed * DeltaTime;
        AddActorWorldOffset(DeltaLocation, true); // true = 벽에 부딪힘 (막힘)

        // 캐릭터 몸통을 이동 방향으로 자연스럽게 회전
        if (!MoveDirection.IsZero())
        {
            FRotator TargetRotation = MoveDirection.Rotation();
            FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);
            SetActorRotation(NewRotation);
        }

        // 움직이고 있으므로 현재 속도를 MoveSpeed로 설정
        CurrentSpeed = MoveSpeed;
    }
    else
    {
        // 키를 떼었을 때 속도를 0으로 만들어 멈춤 상태로 전환
        CurrentSpeed = 0.0f;
    }
}