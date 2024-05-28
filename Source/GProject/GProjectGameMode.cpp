// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectGameMode.h"
#include "GProjectPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AGProjectGameMode::AGProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AGProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	/*static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}*/
}
//bool AGProjectGameMode::ShouldSpawnAtStartSpot(AController* Player)
//{
//	return false;
//}
//void AGProjectGameMode::PostLogin(APlayerController* NewPlayer)
//{
//	Super::PostLogin(NewPlayer);
//	ConnectedPlayers.Add(NewPlayer);
//
//	// Check if all expected players are connected and spawn them.
//	CheckAndSpawnPlayers();
//}
//void AGProjectGameMode::CheckAndSpawnPlayers()
//{
//	if (ConnectedPlayers.Num() == MAX_PLAYERS) // Or however many players you expect
//	{
//		for (APlayerController* PlayerController : ConnectedPlayers)
//		{
//			FTransform SpawnTransform=FTransform::Identity; // Set this to wherever you want the player to spawn
//			SpawnTransform.TransformPosition(FVector(0, 0, 50));
//			*  SpawnedPlayer= GetWorld()->SpawnActor<AGProjectCharacter>(PlayerController->GetClass(), SpawnTransform);
//			PlayerController->Possess(SpawnedPlayer); // Make sure the player controller possesses the new pawn
//		}
//
//		ConnectedPlayers.Empty(); // Clear the list as players have been spawned
//	}
//}