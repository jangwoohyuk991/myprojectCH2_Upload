#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMeshComp->SetupAttachment(SceneRoot);

    // FMath::RandRange를 사용해 매번 생성될 때마다 랜덤한 속도와 범위 부여
    MoveSpeed = FMath::RandRange(150.0f, 400.0f);
    MaxRange = FMath::RandRange(300.0f, 600.0f);

    MoveDirection = FVector(1.0f, 0.0f, 0.0f);
}

void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    // 게임 시작 시점의 위치를 '절대적인 시작 기준점'으로 고정
    StartLocation = GetActorLocation();

    // 3초마다 반복해서 ChangeDirection 함수를 호출하는 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(
        DirectionChangeTimerHandle,
        this,
        &AMovingPlatform::ChangeDirection,
        3.0f, true, 3.0f
    );

    // 20초 뒤에 DestroyPlatform 함수를 호출하여 발판 소멸
    GetWorld()->GetTimerManager().SetTimer(
        DisappearTimerHandle,
        this,
        &AMovingPlatform::DestroyPlatform,
        7.0f, false
    );

    // 게임 시작 시 SpawnActor를 통해 무작위 위치에 플랫폼 5개 동적 스폰
    if (bIsOriginal)
    {
        UWorld* World = GetWorld();
        if (World && PlatformSpawnClass) // 스폰할 클래스가 에디터에서 지정되었는지 확인
        {
            for (int32 i = 0; i < 5; ++i)
            {
                // 랜덤한 위치 오프셋 생성
                FVector SpawnOffset = FVector(
                    FMath::RandRange(-400.0f, 400.0f),
                    FMath::RandRange(-400.0f, 400.0f),
                    FMath::RandRange(50.0f, 200.0f)
                );
                FVector SpawnLocation = StartLocation + SpawnOffset;
                FRotator SpawnRotation = FRotator::ZeroRotator;

                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                // SpawnActor로 생성된 복제본(SpawnedPlatform)을 받아옵니다.
                AMovingPlatform* SpawnedPlatform = World->SpawnActor<AMovingPlatform>(PlatformSpawnClass, SpawnLocation, SpawnRotation, SpawnParams);

                if (SpawnedPlatform)
                {
                    // [ 원본의 현재 속도와 범위를 복제본에게 그대로 전달하여 동기화합니다!
                    SpawnedPlatform->MoveSpeed = this->MoveSpeed;
                    SpawnedPlatform->MaxRange = this->MaxRange;
                    SpawnedPlatform->bIsOriginal = false; // 복제본은 원본이 아니므로 false
                }
            }
        }
    }
}

void AMovingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 현재 위치를 이동 방향과 속도에 맞춰 이동
    FVector CurrentLocation = GetActorLocation();
    CurrentLocation += MoveDirection.GetSafeNormal() * MoveSpeed * DeltaTime;
    SetActorLocation(CurrentLocation);

    // 시작 위치로부터의 직선 거리 측정
    float DistanceMoved = FVector::Dist(StartLocation, CurrentLocation);

    // MaxRange 도달 시 방향 반전 및 보정
    if (DistanceMoved >= MaxRange)
    {
        FVector Overshoot = MoveDirection.GetSafeNormal() * (DistanceMoved - MaxRange);
        SetActorLocation(CurrentLocation - Overshoot);

        MoveDirection *= -1.0f;
    }
}

// 타이머가 발동할 때마다 실행되는 함수
void AMovingPlatform::ChangeDirection()
{
    MoveDirection *= -1.0f;
}

// 20초 뒤 실행되는 함수 (발판 파괴)
void AMovingPlatform::DestroyPlatform()
{
    Destroy(); // 액터 파괴
}