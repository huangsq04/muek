// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "KBE.h"

#define LOCTEXT_NAMESPACE "FKBEModule"

void FKBEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	pKBEngine = NewObject<UKBEngineNetDriver>();
	pKBEngine->AddToRoot();

	TickDelegate = FTickerDelegate::CreateRaw(this, &FKBEModule::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);

#if WITH_EDITORONLY_DATA
	FCoreDelegates::OnActorLabelChanged.AddStatic(&FKBEModule::OnActorLabelChanged);
#endif
}

void FKBEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	pKBEngine->RemoveFromRoot();
}

bool FKBEModule::Tick(float DeltaTime)
{
	if (pKBEngine->IsValidLowLevel())
	{
		pKBEngine->Tick(DeltaTime);
	}
	return true;
}

#if WITH_EDITORONLY_DATA
void FKBEModule::OnActorLabelChanged(AActor* ChangedActor)
{
	FString Lable = ChangedActor->GetActorLabel();
	TInlineComponentArray<UEntity*> ComponentArray;
	ChangedActor->GetComponents<UEntity>(ComponentArray);

	if (ComponentArray.Num() == 1)
	{
		ComponentArray[0]->SetPlayerNameProperty(Lable);
	}
}
#endif
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FKBEModule, KBE)