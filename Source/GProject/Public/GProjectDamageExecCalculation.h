// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GProjectDamageExecCalculation.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGProjectDamageExecCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public :

	UGProjectDamageExecCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
