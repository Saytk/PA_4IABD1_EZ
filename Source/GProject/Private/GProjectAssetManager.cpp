// Fill out your copyright notice in the Description page of Project Settings.


#include "GProjectAssetManager.h"
#include "AbilitySystemGlobals.h"

void UGProjectAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
	UE_LOG(LogTemp,Warning, TEXT("Hello world okokd"));
}
