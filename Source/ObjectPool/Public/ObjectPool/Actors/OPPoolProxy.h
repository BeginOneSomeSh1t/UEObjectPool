// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OPPoolableActor.h"
#include "GameFramework/Actor.h"
#include "OPPoolProxy.generated.h"

class UOPPoolableActorComponent;

/**
 * Intermediate class
 * for using an object pool mechanic.
 * Useful when pooling is necessary in a subsystem for example.
 */
UCLASS()
class OBJECTPOOL_API AOPPoolProxy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOPPoolProxy();

	UFUNCTION(BlueprintPure, Category = "Object Pool")
	FORCEINLINE UOPPoolableActorComponent* GetPoolableActorComponent() const noexcept { return PoolableActorComponent; }

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"), Category="Object Pool")
	static AOPPoolProxy* SpawnPoolProxy(UObject* WorldContextObject, TSubclassOf<AOPPoolProxy> InProxyClass = nullptr) noexcept;

	UFUNCTION(CallInEditor)
	void LogCurrentPoolableActors() noexcept;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UOPPoolableActorComponent* PoolableActorComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
