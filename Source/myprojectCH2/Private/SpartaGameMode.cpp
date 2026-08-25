#include "SpartaGameMode.h"
#include "SpartaPlayerController.h"
#include "SpartaCharacter.h"
#include "SpartaGameState.h"

ASpartaGameMode::ASpartaGameMode()
{
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
	DefaultPawnClass = ASpartaCharacter::StaticClass();

	// 커스텀 GameState 클래스 할당
	GameStateClass = ASpartaGameState::StaticClass();
}