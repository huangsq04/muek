// Fill out your copyright notice in the Description page of Project Settings.


#include "KBEBlueprintFunctionLibrary.h"
#include "KBE.h"

#pragma optimize("", off)
AActor *UKBEBlueprintFunctionLibrary::GetPlayer()
{
	FKBEModule &KBE = FModuleManager::LoadModuleChecked<FKBEModule>(TEXT("KBE"));
	if(KBE.GetKKBEngine()->ServerConnect)
	{
		return  KBE.GetKKBEngine()->ServerConnect->Player();
	}
	return nullptr;
}

UEntity *UKBEBlueprintFunctionLibrary::GetPlayerEntity()
{
	FKBEModule &KBE = FModuleManager::LoadModuleChecked<FKBEModule>(TEXT("KBE"));
	if (KBE.GetKKBEngine()->ServerConnect)
	{
		return  KBE.GetKKBEngine()->ServerConnect->GetPlayerEntity();
	}
	return nullptr;
}
UKBEngineNetDriver *UKBEBlueprintFunctionLibrary::GetKBEngineNetDriver()
{
	FKBEModule &KBE = FModuleManager::LoadModuleChecked<FKBEModule>(TEXT("KBE"));
	return KBE.GetKKBEngine();
}
#pragma optimize("", on)
