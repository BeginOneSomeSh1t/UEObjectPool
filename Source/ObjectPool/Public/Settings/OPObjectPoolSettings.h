// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "OPObjectPoolSettings.generated.h"

class AOPPoolProxy;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig)
class OBJECTPOOL_API UOPObjectPoolSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UOPObjectPoolSettings(const FObjectInitializer& CDO);
	
	UPROPERTY(Config, EditAnywhere, Category = "ObjectPool")
	TSubclassOf<AOPPoolProxy> PoolProxyDefaultClass;
	
};
