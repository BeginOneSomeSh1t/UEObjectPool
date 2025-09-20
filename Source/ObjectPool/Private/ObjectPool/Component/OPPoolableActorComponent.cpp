// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/Component/OPPoolableActorComponent.h"

#include "ObjectPool/Actors/OPPoolableActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogPoolableActorComponent, Log, All);

// Sets default values for this component's properties
UOPPoolableActorComponent::UOPPoolableActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UOPPoolableActorComponent::InitializePool()
{
    if (!PooledActorClass)
    {
        UE_LOG(LogPoolableActorComponent, Error, TEXT("UOPPoolableActorComponent::InitializePool PooledActorClass is not set!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        UE_LOG(LogPoolableActorComponent, Error, TEXT("UOPPoolableActorComponent::InitializePool World is not valid!"));
        return;
    }

    PooledActors.Reserve(InitialPoolSize);
    
    for (int32 i = 0; i < InitialPoolSize; ++i)
    {
        AOPPoolableActor* PooledActor = CreatePooledActor();
        if (!IsValid(PooledActor))
        {
            UE_LOG(LogPoolableActorComponent, Error, TEXT("UOPPoolableActorComponent::InitializePool CreatePooledActor failed!"));
            return;
        }
        
        PooledActor->SetActorHiddenInGame(true);
        PooledActor->SetActorEnableCollision(false);
        PooledActor->SetActorTickEnabled(false);
    }

    UE_LOG(LogPoolableActorComponent, Log, TEXT("Pool initialized with %d actors"), PooledActors.Num());
    bPoolInitialized = true;
    OnPoolInitialized.Broadcast();
    OnPoolInitializedNative.Broadcast();

    InitiateShrinkTimer();
}

void UOPPoolableActorComponent::InitiateShrinkTimer()
{
    GetWorld()->GetTimerManager().SetTimer(ShrinkTimerHandle, [This = TWeakObjectPtr<UOPPoolableActorComponent>(this)]
        
    {
        if (This.IsValid())
        {
            This->ShrinkPool();
        }
    }, ShrinkCheckInterval, true);
}

void UOPPoolableActorComponent::ValidatePool()
{
    for (auto It = PooledActors.CreateIterator(); It; ++It)
    {
        if (!It) continue;

        auto LActor = *It;
        if (!IsValid(LActor))
        {
            It.RemoveCurrent();
            UE_LOG(LogPoolableActorComponent, Warning, TEXT("Removed one hung actor during validation!"));
        }
    }
}

AOPPoolableActor* UOPPoolableActorComponent::GetPooledActor(EOPGetPooledActorErrorMode InErrorMode)
{
    ValidatePool();
    
    for (AOPPoolableActor* Actor : PooledActors)
    {
        if (!IsValid(Actor)) continue;
        
        if (!Actor->IsActive())
        {
            Actor->SetIsActive(true);
            Actor->OnAcquired();

            UE_LOG(LogPoolableActorComponent, Log, TEXT("Pool acquired actor: %s"), *GetNameSafe(Actor));
            return Actor;
        }
    }

    if (bDynamicallyExpandable && PooledActors.Num() < MaxPoolSize)
    {
        auto LActor = CreatePooledActor();
        if (!IsValid(LActor))
        {
            UE_LOG(LogPoolableActorComponent, Error, TEXT("UOPPoolableActorComponent::GetPooledActor Failed to dynamically create a pooled actor!"));
            return nullptr;
        }
        
        LActor->SetIsActive(true);
        LActor->OnAcquired();

        UE_LOG(LogPoolableActorComponent, Log, TEXT("Pool dynamically expanded to %d!"), PooledActors.Num());
        return LActor;
    }
    
    switch (InErrorMode)
    {
    case EOPGetPooledActorErrorMode::Ignore:
        break;
    case EOPGetPooledActorErrorMode::LogAndReturnNull:
        UE_LOG(LogPoolableActorComponent, Error, TEXT("Pool is full!"))
        break;
    case EOPGetPooledActorErrorMode::Assert:
        check(bDynamicallyExpandable);
        break;
    default: ;
    }
    
    return nullptr;
}

AOPPoolableActor* UOPPoolableActorComponent::CreatePooledActor()
{
    UWorld* World = GetWorld();
    if (!World || !PooledActorClass)
    {
        UE_LOG(LogPoolableActorComponent, Error, TEXT("UOPPoolableActorComponent::CreatePooledActor World or PooledActorClass is not valid!"));
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = GetOwner()->GetInstigator();

    AOPPoolableActor* SpawnedActor = World->SpawnActor<AOPPoolableActor>(
        PooledActorClass,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (IsValid(SpawnedActor))
    {
        SpawnedActor->SetPoolComponent(this);
        PooledActors.Add(SpawnedActor);
    }

    UE_LOG(LogPoolableActorComponent, Log, TEXT("Pool created new actor: %s"), *GetNameSafe(SpawnedActor));
    return SpawnedActor;
}

void UOPPoolableActorComponent::ShrinkPool()
{
    TArray<AOPPoolableActor*> LCandidates;
    const FDateTime ThresholdTime = FDateTime::Now() - FTimespan::FromSeconds(InactivityThreshold);
    
    // Identify candidates for removal
    for(auto& LActor : PooledActors)
    {
        if(!LActor->IsActive() && 
           LastReleaseTimes.Contains(LActor) &&
           LastReleaseTimes[LActor] < ThresholdTime)
        {
            LCandidates.Add(LActor);
        }
    }

    // Keep minimum initial size, remove excess
    const int32 RemoveCount = FMath::Min(
        LCandidates.Num(), 
        PooledActors.Num() - InitialPoolSize
    );
    
    for(int32 i = 0; i < RemoveCount; i++)
    {
        auto LActor = LCandidates[i];
        PooledActors.Remove(LActor);
        LastReleaseTimes.Remove(LActor);
        LActor->Destroy();
    }

    UE_LOG(LogPoolableActorComponent, Log, TEXT("Pool shrunk to %d actors. Total removed actors: %d"), PooledActors.Num(), RemoveCount);
}

void UOPPoolableActorComponent::ReleaseActor(AOPPoolableActor* ActorToRelease)
{
    if (!ActorToRelease || !PooledActors.Contains(ActorToRelease)) return;

    ActorToRelease->SetIsActive(false);
    ActorToRelease->OnReleased();
    ActorToRelease->SetActorHiddenInGame(true);
    ActorToRelease->SetActorEnableCollision(false);
    ActorToRelease->SetActorTickEnabled(false);
    LastReleaseTimes.Add(ActorToRelease, FDateTime::Now());

    UE_LOG(LogPoolableActorComponent, Log, TEXT("Pool released actor: %s"), *GetNameSafe(ActorToRelease));
}

void UOPPoolableActorComponent::AdjustPoolSize(const int32 InInitialSize, const int32 InMaxSize) noexcept
{
    InitialPoolSize = InInitialSize;
    MaxPoolSize = InMaxSize;

    if (PooledActors.Num() > InitialPoolSize)
    {
        ShrinkPool();
    }
}

void UOPPoolableActorComponent::ForceShrinkPool() noexcept
{
    for (auto It = PooledActors.CreateIterator(); It; ++It)
    {
        auto LActor = *It;
        if (IsValid(LActor) && !LActor->IsActive())
        {
            LActor->OnReleased();
            LActor->Destroy();
            It.RemoveCurrent();
        }
    }
}

void UOPPoolableActorComponent::LogCurrentPoolableActors()
{
    for (auto& LActor : PooledActors)
    {
        if (ensure(IsValid(LActor)))
            UE_LOG(LogPoolableActorComponent, Log, TEXT("Pool actor: %s, active: %d"), *GetNameSafe(LActor), LActor->IsActive());
    }
}
