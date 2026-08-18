#include "SpartaGameMode.h"
#include "SpartaCharacter.h" // 캐릭터 클래스 지정
#include "SpartaPlayerController.h" // 플레이어 컨트롤러 클래스 지정

ASpartaGameMode::ASpartaGameMode()
{
    
    DefaultPawnClass = ASpartaCharacter::StaticClass();

    
    PlayerControllerClass = ASpartaPlayerController::StaticClass();
}