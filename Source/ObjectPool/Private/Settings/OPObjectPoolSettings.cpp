// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/OPObjectPoolSettings.h"

#include "ObjectPool/Actors/OPPoolProxy.h"

UOPObjectPoolSettings::UOPObjectPoolSettings(const FObjectInitializer& CDO)
{
	PoolProxyDefaultClass = AOPPoolProxy::StaticClass();
}
