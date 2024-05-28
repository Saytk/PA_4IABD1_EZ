// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectPlayerController.h"
#include "AbilitySystemComponent.h"
#include "GProjectCharacter.h"
#include "GProjectPlayerState.h"

 AGProjectPlayerController::AGProjectPlayerController() 
 {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}
void AGProjectPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AGProjectPlayerState* PS = GetPlayerState<AGProjectPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void AGProjectPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Hero is possessed.
	//CreateHUD();
}

