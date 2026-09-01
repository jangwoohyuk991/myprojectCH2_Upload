// 컴포넌트 및 필수 헤더 포함
#include "spartaPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AspartaPawn::AspartaPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 루트 캡슐 컴포넌트 생성
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    SetRootComponent(CapsuleComp);
    CapsuleComp->SetSimulatePhysics(false);

    // 2. 스켈레탈 메시 컴포넌트 생성
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetSimulatePhysics(false);

    // 3. 스프링암 컴포넌트 생성
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;

    // 4. 카메라 컴포넌트 생성
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
}

void AspartaPawn::BeginPlay()
{
    Super::BeginPlay();

    // Enhanced Input Context 활성화
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (InputMappingContext)
                {
                    Subsystem->AddMappingContext(InputMappingContext, 0);
                }
            }
        }
    }
}

void AspartaPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 1. 이동 처리 (DeltaTime 적용, AddActorLocalOffset 사용)
    if (!CurrentMoveInput.IsNearlyZero())
    {
        FVector MoveDelta = FVector(CurrentMoveInput.X, CurrentMoveInput.Y, 0.0f) * MoveSpeed * DeltaTime;
        AddActorLocalOffset(MoveDelta, true);
    }

    // 2. 회전 처리 (DeltaTime 적용, AddActorLocalRotation 및 SpringArm 사용)
    if (!CurrentLookInput.IsNearlyZero())
    {
        float YawDelta = CurrentLookInput.X * RotationSpeed * DeltaTime;
        float PitchDelta = CurrentLookInput.Y * RotationSpeed * DeltaTime;

        // Yaw 회전 (Pawn 본체 회전)
        FRotator NewRotation = FRotator(0.0f, YawDelta, 0.0f);
        AddActorLocalRotation(NewRotation);

        // Pitch 회전 (카메라 스프링암 회전)
        if (SpringArmComp)
        {
            FRotator ArmRotation = SpringArmComp->GetRelativeRotation();
            ArmRotation.Pitch = FMath::Clamp(ArmRotation.Pitch + PitchDelta, -80.0f, 80.0f);
            SpringArmComp->SetRelativeRotation(ArmRotation);
        }
    }
}

void AspartaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced Input 바인딩 (Triggered: 누르고 있을 때, Completed: 손을 뗐을 때)
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AspartaPawn::Move);
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AspartaPawn::Move);
        }

        if (LookAction)
        {
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AspartaPawn::Look);
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Completed, this, &AspartaPawn::Look);
        }
    }
}

void AspartaPawn::Move(const FInputActionValue& Value)
{
    CurrentMoveInput = Value.Get<FVector2D>();
}

void AspartaPawn::Look(const FInputActionValue& Value)
{
    CurrentLookInput = Value.Get<FVector2D>();
}