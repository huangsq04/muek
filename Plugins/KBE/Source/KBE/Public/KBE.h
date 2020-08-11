// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KBEngineNetDriver.h"
#include "Modules/ModuleManager.h"

class FKBEModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_EDITORONLY_DATA
	static void OnActorLabelChanged(AActor* ChangedActor);
#endif

	bool Tick(float DeltaTime);

	UKBEngineNetDriver	*GetKKBEngine() { return pKBEngine; }

	UKBEngineNetDriver	*pKBEngine;

	FTickerDelegate TickDelegate;

	FDelegateHandle TickDelegateHandle;
	
	UWorld *World;
};
