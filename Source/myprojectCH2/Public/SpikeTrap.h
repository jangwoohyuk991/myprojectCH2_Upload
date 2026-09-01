#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;

UCLASS()
class MYPROJECTCH2_API ASpikeTrap : public AActor
{
	GENERATED_BODY()

public:
	ASpikeTrap();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SpikeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DamageBox;

	// ★ 가시 솟구칠 때 재생할 효과음 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Settings")
	USoundBase* SpikeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Settings")
	float SpikeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Settings")
	float ToggleInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Settings")
	float SpikeUpHeight;

	FTimerHandle SpikeTimerHandle;
	bool bIsSpikeUp;
	FVector DownPosition;
	FVector UpPosition;

	void ToggleSpike();
	void CheckAndApplyDamage();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};