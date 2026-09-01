#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent; // 3D 위젯 컴포넌트 전방 선언
struct FInputActionValue;

UCLASS()
class MYPROJECTCH2_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	// 현재 체력을 반환하는 함수 (HUD UI 연동용)
	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth() const;

	// 체력을 회복시키는 함수
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	// === [추가] 디버프 상태 변수 및 타이머 핸들 ===
	bool bIsReverseControl = false;
	FTimerHandle SlowTimerHandle;
	FTimerHandle ReverseTimerHandle;

	// === [추가] 스프린트 중 감속 연동을 위한 상태 변수 ===
	bool bIsSlowed = false;
	float CurrentSlowRatio = 1.0f;

	// === [추가] 디버프 적용/해제 함수 ===
	void ApplySlow(float SlowRatio, float Duration);
	void ResetSlow();
	void ApplyReverseControl(float Duration);
	void ResetReverseControl();

protected:
	virtual void BeginPlay() override; // 시작 시 HP UI 갱신용

	// 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// 3D 머리 위 위젯 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	// 스프린트 및 이동 속도 속성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	// 체력 속성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	// 입력 컴포넌트 바인딩 설정
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 입력 액션 바인딩 함수들
	UFUNCTION()
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

	// 데미지 수신 및 사망 처리
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void OnDeath();

	// 머리 위 체력 텍스트 갱신 함수
	void UpdateOverheadHP();
};