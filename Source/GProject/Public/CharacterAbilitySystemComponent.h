// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h" // for FGameplayAbilitySpecHandle
#include "EnhancedInputComponent.h" // for FInputBindingHandle
#include "CharacterAbilitySystemComponent.generated.h"

class UInputAction;

USTRUCT()
struct FAbilityInputBinding
{
	GENERATED_BODY()

	int32  InputID = 0;
	uint32 OnPressedHandle = 0;
	uint32 OnReleasedHandle = 0;
	TArray<FGameplayAbilitySpecHandle> BoundAbilitiesStack;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UCharacterAbilitySystemComponent*, sourceASC, float, UnmitigatedDamage, float, MitigatedDamage);
/**
 *
 */
UCLASS(Blueprintable, BlueprintType, Category = "AbilityInput", meta = (BlueprintSpawnableComponent))
class GPROJECT_API UCharacterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:


	bool CharacterAbilitiesGiven = false;
	bool StartupEffectsApplied = false;
	FReceivedDamageDelegate ReceivedDamage;

	UPROPERTY()
	TArray<FGameplayAbilitySpec> specArray;

	int maxSpell = 2;


	virtual void ReceiveDamage(UCharacterAbilitySystemComponent* sourceASC, float UnmitigatedDamage, float MitigatedDamage);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void ClearAbilityBindings(UInputAction* InputAction);

	UFUNCTION()
	FGameplayTag AbilityContainsTags(FGameplayTagContainer AbilityTags, FGameplayTagContainer InputAbilityTags);

	UFUNCTION()
	void BindInputComponent(UInputComponent* inputComponent);

private:
	UPROPERTY()
	UInputAction* inputAction;
	UPROPERTY()
	FAbilityInputBinding abilityInputBinding;
	void OnAbilityInputPressed(UInputAction* InputAction);

	void OnAbilityInputReleased(UInputAction* InputAction);


	void RemoveEntry(UInputAction* InputAction);

	void TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding);

	FGameplayAbilitySpec* FindAbilitySpec(FGameplayAbilitySpecHandle Handle);

	virtual void BeginPlay() override;

	UPROPERTY(transient)
	TMap<UInputAction*, FAbilityInputBinding> MappedAbilities;

	UPROPERTY(transient)
	UEnhancedInputComponent* InputComponent;
	UFUNCTION()
	APawn* GetPawnFromASC();

	UPROPERTY(EditAnywhere)
	UInputComponent* AbilityAction1;

	UPROPERTY(EditAnywhere)
	UInputComponent* AbilityAction2;
	
};

