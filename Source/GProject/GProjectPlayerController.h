// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GProjectPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class AGProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	

protected:
	
	virtual void OnRep_PlayerState() override;
	AGProjectPlayerController();
	//Server
	void OnPossess(APawn* InPawn);
	

};


