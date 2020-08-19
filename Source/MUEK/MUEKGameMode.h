// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KBEngineNetDriver.h"
#include "GameFramework/GameModeBase.h"
#include "MUEKGameMode.generated.h"

UCLASS(minimalapi)
class AMUEKGameMode : public AGameModeBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateRoleListDelegate);
public:
	AMUEKGameMode();

	virtual void BeginPlay();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList);

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void KBELogin(const FString& Name, const FString& Password, const FString& ServerIP);

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void KBEEnterLobby();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	AActor *GetPlayer();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	UEntity *GetPlayerEntity();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	bool IsKBEConnect();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	FString GetLoginName();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void KBECreateRole();

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void KBEEnterSpace(const FString &Name);

	UFUNCTION(BlueprintImplementableEvent, Category = "MUEK")
	void LoginCallBP(const FString &Name, int type, const FString &Error);

	UFUNCTION(BlueprintImplementableEvent, Category = "MUEK")
	void OnPlayerEnterWorld(int Type); //type 是否是以服务器登陆

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void KBELevelTeleport(const FString &Map, const FVector Position);

	UFUNCTION(BlueprintCallable, Category = "MUEK")
	void KBELeaveWorld();

	void KBELoginCallback(const FString &Name, int type, const FString &Error);

	void KBEPlayerEnterWorldCallback(int Id);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	FString LoginIp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	int LoginPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	int LoginType; //1:客户端自动登陆 2:作为服务器登陆`

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUEK")
	FString LoginName;

	//通知界面玩家角色刷新
	UPROPERTY(BlueprintAssignable)
	FUpdateRoleListDelegate UpdateRoleListDelegate;
	void NotifyRoleListChange();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	AActor *TravelActor;

private:

	FStreamableManager	StreamableManager;

	TMap<FString, FString> Maps;

	UKBEngineNetDriver *KBENetDriver;
};



