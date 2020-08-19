// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KBECharacter.h"
#include "TroopsComponent.h"
#include "KBEMonster.generated.h"

/**
 * 
 */
UCLASS()
class MUEK_API AKBEMonster : public AKBECharacter
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	AKBEMonster();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	UTroopsComponent *TroopsComponent;

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void ComeHere();
	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void CallKBEComeHere();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void FollowMe();
	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void CallKBEFollowMe();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void StopFollowMe();
	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void CallKBEStopFollowMe();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	FVector &RandomTargetPoint(float Radius = 5000.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	FVector StartPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	FVector TargetPoint;

private:

};
