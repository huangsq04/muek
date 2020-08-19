// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefineEntity.h"
#include "KBEAgentEntity.generated.h"

/**
 * 
 */
UCLASS()
class MUEK_API UKBEAgentEntity : public UAgentEntity
{
	GENERATED_BODY()
	
public:

	virtual void OnInitPropertyFinish_Implementation();

	//设置完位置的回调
	virtual void OnSetAgentLocation_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
	float ControlRange = 20000; //控制范围100米

private:
};
