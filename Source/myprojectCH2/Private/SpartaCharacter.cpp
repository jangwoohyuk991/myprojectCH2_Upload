#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h" // ProgressBar 연동을 위해 추가
#include "Blueprint/UserWidget.h"

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

	// 머리 위 3D 위젯 생성 및 메쉬에 부착
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen); // 화면을 정면으로 바라보도록 설정
	OverheadWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 190.0f)); // 머리 위 높이 수정 (190.0f)
	OverheadWidget->SetDrawSize(FVector2D(150.0f, 20.0f));             // 3D 위젯 가로/세로 크기 설정

	// 스프린트 및 이동 속도 초기화
	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// 초기 체력 설정
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 시작 시 머리 위 체력 UI 초기화
	UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Look);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopSprint);
			}
		}
	}
}

// === 기존 Move() 내부 입력 벡터에 조작 반전 처리 추가 ===
void ASpartaCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D MoveInput = Value.Get<FVector2D>();

	// 조작 반전 디버프 상태라면 입력 값의 축을 반대로(-1) 곱함
	if (bIsReverseControl)
	{
		MoveInput *= -1.0f;
	}

	if (!FMath::IsNearlyZero(MoveInput.X)) AddMovementInput(GetActorForwardVector(), MoveInput.X);
	if (!FMath::IsNearlyZero(MoveInput.Y)) AddMovementInput(GetActorRightVector(), MoveInput.Y);
}

void ASpartaCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) Jump();
}

void ASpartaCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>()) StopJumping();
}

void ASpartaCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

// === 감속 디버프 중 스프린트 속도 조절 ===
void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		float TargetSpeed = SprintSpeed;
		if (bIsSlowed)
		{
			TargetSpeed *= CurrentSlowRatio;
		}
		GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
	}
}

// === 감속 디버프 중 스프린트 종료 속도 ===
void ASpartaCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		float TargetSpeed = NormalSpeed;
		if (bIsSlowed)
		{
			TargetSpeed *= CurrentSlowRatio;
		}
		GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
	}
}

int32 ASpartaCharacter::GetHealth() const
{
	return FMath::RoundToInt(Health);
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health increased to: %f"), Health);

	// 회복 시 머리 위 체력 UI 갱신
	UpdateOverheadHP();
}

// === 디버프 적용 및 해제 함수 (스프린트 연동 추가) ===
void ASpartaCharacter::ApplySlow(float SlowRatio, float Duration)
{
	bIsSlowed = true;
	CurrentSlowRatio = SlowRatio;

	if (GetCharacterMovement())
	{
		// 현재 속도(걷기/달리기 상태)에 감속 비율을 곱함
		GetCharacterMovement()->MaxWalkSpeed *= CurrentSlowRatio;
	}

	// 기존 타이머가 작동 중이면 재설정(지속시간 갱신)
	GetWorldTimerManager().SetTimer(
		SlowTimerHandle,
		this,
		&ASpartaCharacter::ResetSlow,
		Duration,
		false
	);
}

void ASpartaCharacter::ResetSlow()
{
	bIsSlowed = false;
	CurrentSlowRatio = 1.0f;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void ASpartaCharacter::ApplyReverseControl(float Duration)
{
	bIsReverseControl = true;

	GetWorldTimerManager().SetTimer(
		ReverseTimerHandle,
		this,
		&ASpartaCharacter::ResetReverseControl,
		Duration,
		false
	);
}

void ASpartaCharacter::ResetReverseControl()
{
	bIsReverseControl = false;
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), Health);

	// 피격 시 머리 위 체력 UI 갱신
	UpdateOverheadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	UE_LOG(LogTemp, Error, TEXT("Character is Dead!"));

	// 사망 시 GameState에 게임 오버 처리 요청
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

// 머리 위 체력 텍스트 갱신 구현
void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	// 기존 OverHeadHP 텍스트 갱신
	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), Health, MaxHealth)));
	}

	// 3D 위젯의 HPBar(ProgressBar) 비율 갱신 (명시적 float 연산 적용)
	if (UProgressBar* HPBar = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName(TEXT("HPBar"))))
	{
		float HealthRatio = (MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f;
		HPBar->SetPercent(HealthRatio);
	}
}