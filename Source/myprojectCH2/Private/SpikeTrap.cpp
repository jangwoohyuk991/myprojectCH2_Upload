#include "SpikeTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
	SpikeMesh->SetupAttachment(RootComp);

	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
	DamageBox->SetupAttachment(SpikeMesh);

	SpikeDamage = 15.0f;
	ToggleInterval = 2.0f;
	SpikeUpHeight = 120.0f;
	bIsSpikeUp = false;
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();

	DownPosition = FVector(0.0f, 0.0f, -100.0f);
	UpPosition = FVector(0.0f, 0.0f, 0.0f);

	SpikeMesh->SetRelativeLocation(DownPosition);

	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnOverlapBegin);

	GetWorldTimerManager().SetTimer(
		SpikeTimerHandle,
		this,
		&ASpikeTrap::ToggleSpike,
		ToggleInterval,
		true
	);
}

void ASpikeTrap::ToggleSpike()
{
	bIsSpikeUp = !bIsSpikeUp;

	if (bIsSpikeUp)
	{
		SpikeMesh->SetRelativeLocation(UpPosition);

		// ★ 가시가 솟구칠 때 효과음 재생
		if (SpikeSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				SpikeSound,
				GetActorLocation()
			);
		}

		CheckAndApplyDamage();
	}
	else
	{
		SpikeMesh->SetRelativeLocation(DownPosition);
	}
}

void ASpikeTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsSpikeUp && OtherActor)
	{
		APawn* PlayerPawn = Cast<APawn>(OtherActor);
		if (PlayerPawn)
		{
			UGameplayStatics::ApplyDamage(
				PlayerPawn,
				SpikeDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

void ASpikeTrap::CheckAndApplyDamage()
{
	TArray<AActor*> OverlappingActors;
	DamageBox->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		APawn* PlayerPawn = Cast<APawn>(Actor);
		if (PlayerPawn)
		{
			UGameplayStatics::ApplyDamage(
				PlayerPawn,
				SpikeDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}
}