// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KBECharacter.h"
#include "Components/ActorComponent.h"
#include "TroopsComponent.generated.h"


UCLASS()
class MUEK_API AFollow : public ACharacter
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	float TargetYaw;

	UFUNCTION(BlueprintImplementableEvent, Category = "MUEK")
	void OnGoToTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	int FollowMoveType = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	AActor *FollowActor;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUEK_API UTroopsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTroopsComponent();

	void Rank(const FVector &Location, float Yaw, int Type, TMap<int, FVector> &OutRank);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ComeHere();

	void FollowMe();

	void StopFollowMe();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void CreateFollowCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	TSubclassOf<AFollow> FollowClass;

	UPROPERTY()
	TMap<int, AFollow *> FollowCharacters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	int RankState = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	int RankNum = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	float RankInterval = 300.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	TSubclassOf<AActor> FollowActorClass;
private:

	int FollowMoveType = 0;

	FVector LastPoint;

	float LastYaw;
		
};
