// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OPPoolableActor.generated.h"

class UOPPoolableActorComponent;

UCLASS()
class OBJECTPOOL_API AOPPoolableActor : public AActor
{
	GENERATED_BODY()

	friend class UOPPoolableActorComponent;

public:
	// Sets default values for this actor's properties
	AOPPoolableActor();

	UFUNCTION(BlueprintPure, Category = "Object Pool")
	bool IsActive() const { return bIsActive; }


protected:
	// Override these functions to do activation and deactivation of your data
	/**
	 * Called when actor acquired
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	virtual void OnAcquired() {};

	/**
	 * Called when actor released
	 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	virtual void OnReleased() {};

	FORCEINLINE void SetPoolComponent(UOPPoolableActorComponent* PoolComponent) { OwningPool = PoolComponent; }

	FORCEINLINE void SetIsActive(const bool IsActive) noexcept { bIsActive = IsActive; }
	
	UPROPERTY(BlueprintReadOnly, Category = "Object Pool")
	bool bIsActive = false;

protected:
	UPROPERTY()
	UOPPoolableActorComponent* OwningPool = nullptr;
};
