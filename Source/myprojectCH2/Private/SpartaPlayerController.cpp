#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h" // 서브시스템 사용을 위한 헤더

ASpartaPlayerController::ASpartaPlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Local Player 플레이어의 입력이나 화면 뷰 같은 것을 관리하는 객체
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer()) //사용자가 하고있는 플레이어의 LocalPlayer 객체를 가져와라
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())//UEnhancedInputLocalPlayerSubsystem:IMC를 추가하고 삭제하는 역할
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(InputMappingContext, 0); // 우선순위 0으로 추가
            }
        }
    }
}