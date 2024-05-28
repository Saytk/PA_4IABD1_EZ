// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GProjectCharacterBase.h"
#include "GProjectPlayerState.h"
#include "GProjectCharacter.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputComponentReadyDelegate);

UCLASS()
class GPROJECT_API AGProjectCharacter : public AGProjectCharacterBase
{
	GENERATED_BODY()
	
public : 

	AGProjectCharacter(const class FObjectInitializer& ObjectInitializer);
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	FInputComponentReadyDelegate OnInputComponentReady;

	void TriggerInputComponentReady() {
		OnInputComponentReady.Broadcast();
	}
	virtual void PossessedBy(AController* NewController) override;

	FGameplayAbilitySpecHandle GetAbilityHandle(UGameplayAbility* TargetAbility);


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void FinishDying() override;

	FVector CachedDestination;

	class AGProjectPlayerController* PC;


	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* PlayerMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DrawAction;

	TMap<FGameplayTag, UInputAction*>InputMapHandler;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Ability1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Ability2Action;

	UWorld* world = GetWorld();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UProjectileLauncherComponent* ProjectileLauncherComponent;
	




protected:

	bool ASCInputBound = false;
	FGameplayTag DeadTag;
	virtual void OnRep_PlayerState() override;
	void InitializeBindingFromTxt();
	void BindAbilityToInputUsingTag(FGameplayTag AbilityTag, UInputAction* InputAction);
	void SetUpMapHandler();
	void BindASCInput();

	void InitializeStartingValuess(AGProjectPlayerState* PS);

	/** Called for draw input */
	void Draw(const FInputActionValue& Value);
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look();
	// To add mapping context
	virtual void BeginPlay() override;
};
