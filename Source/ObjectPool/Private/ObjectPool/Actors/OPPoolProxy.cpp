// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/Actors/OPPoolProxy.h"

#include "ObjectPool/Component/OPPoolableActorComponent.h"
#include "Settings/OPObjectPoolSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogPoolProxy, Log, All);

// Sets default values
AOPPoolProxy::AOPPoolProxy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PoolableActorComponent = CreateDefaultSubobject<UOPPoolableActorComponent>(TEXT("PoolableActorComponent"));
}

AOPPoolProxy* AOPPoolProxy::SpawnPoolProxy(UObject* WorldContextObject, TSubclassOf<AOPPoolProxy> InProxyClass /* = nullptr*/) noexcept
{
	auto LWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(LWorld))
	{
		return nullptr;
	}

	const auto LConfig = GetDefault<UOPObjectPoolSettings>();
	if (InProxyClass == nullptr)
	{
		InProxyClass = LConfig->PoolProxyDefaultClass;
	}

	if (!ensureMsgf(InProxyClass, TEXT("Proxy class must've been at least some valid class!")))
	{
		return nullptr;
	}

	FActorSpawnParameters LParams;
	LParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	return LWorld->SpawnActor<AOPPoolProxy>(InProxyClass, LParams);
}

void AOPPoolProxy::LogCurrentPoolableActors() noexcept
{
	if (IsValid(PoolableActorComponent))
	{
		PoolableActorComponent->LogCurrentPoolableActors();
	}
}

// Called when the game starts or when spawned
void AOPPoolProxy::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOPPoolProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

