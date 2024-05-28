// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GProjectAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGProjectAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public :
	virtual void StartInitialLoading() override;
};
