// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OPPoolableActorComponent.generated.h"


class AOPPoolableActor;

UENUM()
enum class EOPGetPooledActorErrorMode : uint8
{
	Ignore,
	LogAndReturnNull,
	Assert
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OBJECTPOOL_API UOPPoolableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPoolInitialized);
	DECLARE_MULTICAST_DELEGATE(FOnPoolInitializedNative);

	/**
	 * Triggered whenever this pool is fully initialized.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPoolInitialized OnPoolInitialized;
	FOnPoolInitializedNative OnPoolInitializedNative;
	
	// Sets default values for this component's properties
	UOPPoolableActorComponent();

	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void InitializePool();
	
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	AOPPoolableActor* GetPooledActor(EOPGetPooledActorErrorMode InErrorMode = EOPGetPooledActorErrorMode::Ignore);

	template<typename TAsType> requires TIsDerivedFrom<TAsType, AOPPoolableActor>::Value
	TAsType* GetPooledActor(EOPGetPooledActorErrorMode InErrorMode = EOPGetPooledActorErrorMode::Ignore) noexcept
	{
		return Cast<TAsType>(GetPooledActor(InErrorMode));
	}

	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void ReleaseActor(AOPPoolableActor* ActorToRelease);

	/**
	 * Adjusts the size of the pool possibly
	 * shrinking of expanding the pool.
	 *
	 * @note: used actors are not going to be shrinked,
	 * however they will be destroyed once released.
	 * @param InInitialSize Initial size
	 * @param InMaxSize Max size
	 */
	UFUNCTION(BlueprintCallable, Category="Object Pool")
	void AdjustPoolSize(const int32 InInitialSize, const int32 InMaxSize) noexcept;

	/**
	 * Ultimately destroyed all invactive objects
	 */
	UFUNCTION(BlueprintCallable, Category="Object Pool")
	void ForceShrinkPool() noexcept;

	UFUNCTION(BlueprintPure, Category="Object Pool")
	FORCEINLINE bool IsInitialized() const noexcept
	{
		return bPoolInitialized;
	}

	void LogCurrentPoolableActors();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	TSubclassOf<AOPPoolableActor> PooledActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	int32 InitialPoolSize = 10;

	/**
	 * Should this pool be dynamically expandable
	 * when amount of pooled actors is over initial size?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	bool bDynamicallyExpandable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	int32 MaxPoolSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	float ShrinkCheckInterval = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	float InactivityThreshold = 120.0f;

	/**
	 * Whether this pool was successfully initialized
	 */
	UPROPERTY(Transient, BlueprintReadOnly)
	bool bPoolInitialized = false;

private:

	UPROPERTY(Transient)
	TArray<AOPPoolableActor*> PooledActors;
	
    
	
	void InitiateShrinkTimer();

	/**
	 * Removes any destroyed actors
	 */
	void ValidatePool();
	AOPPoolableActor* CreatePooledActor();
	void ShrinkPool();

	UPROPERTY(Transient)
	TMap<AOPPoolableActor*, FDateTime> LastReleaseTimes;
	FTimerHandle ShrinkTimerHandle;
};
