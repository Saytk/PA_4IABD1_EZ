// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemInterface.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GProjectCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AGProjectCharacterBase*, character);


UCLASS(Blueprintable)
class GPROJECT_API AGProjectCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGProjectCharacterBase(const class FObjectInitializer& ObjectInitializer);


	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;


	

	
protected:

	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;



	TWeakObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;


	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GProject|Character")
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GProject|Animation")
	UAnimMontage* DeathMontage;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GProject|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GProject|Abilities")
	TArray <TSubclassOf<class UGameplayEffect>> StartupEffects;

	virtual void AddCharacterAbilities();


	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void SetHealth(float health);

	virtual void SetMana(float Mana);

	virtual void BeginPlay() override;

	//GAS
public:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float ProjectileHeight = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GProject|Abilities")
	TArray <TSubclassOf<class UCharacterGameplayAbility>> CharacterAbilities;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintAssignable, Category = "GProject|Character")
	FCharacterDiedDelegate OnCharacterDied;

	UFUNCTION(BlueprintCallable, Category = "GProject|Character")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "GProject|Character")
	virtual int32 GetAbilityLevel(GProjectAbilityID abilityId) const;

	virtual void RemoveCharacterAbilities();

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "GProject|Character")
	virtual void FinishDying();


	UFUNCTION(BlueprintCallable, Category = "GProject|Character|Attribute")
	float GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "GProject|Character|Attribute")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GProject|Character|Attribute")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GProject|Character|Attribute")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "GProject|Character|Attribute")
	float GetMaxMana() const;


};



