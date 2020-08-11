// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entity.h"
#include "KBEngineNetDriver.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KBEBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class KBE_API UKBEBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable, Category = "MUEK")
	static AActor *GetPlayer();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	static UEntity *GetPlayerEntity();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	static UKBEngineNetDriver *GetKBEngineNetDriver();
};
