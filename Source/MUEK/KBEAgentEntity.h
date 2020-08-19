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

	//������λ�õĻص�
	virtual void OnSetAgentLocation_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
	float ControlRange = 20000; //���Ʒ�Χ100��

private:
};
