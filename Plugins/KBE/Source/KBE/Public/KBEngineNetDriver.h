// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Core.h"
#include "Engine.h"
#include "Engine/NetDriver.h"
#include "Mailbox.h"
#include "Entity.h"
#include "KBENetConnect.h"
#include "KBEngineNetDriver.generated.h"

USTRUCT()
struct FLoadEntityType {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere)
	FString EntityName;
	UPROPERTY(EditAnywhere)
	UClass* ClassEntity;
};
UCLASS()
class KBE_API URegisterLoadEntityClass : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FLoadEntityType> items;
};

/**
 * 
 */
UCLASS()
class KBE_API UKBEngineNetDriver : public UNetDriver 
{
	GENERATED_BODY()
	

public:

	UKBEngineNetDriver();

	virtual void BeginDestroy();

	virtual void Tick(float DeltaTime);
	bool Login(const FString &Ip,
		uint16 Port, 
		const FString &LoginName, 
		const FString &Password);

	bool EnterLobby(const FString &LoginName);

	void Logoff(const FString &LoginName);

	int GetConnectStatu(const FString &LoginName);

	void LeaveWorld(const FString &LoginName);

	AActor *GetPlayer(const FString &LoginName);

	UEntity *GetPlayerEntity(const FString &LoginName);

	void RemoveEntityExceptPlayer(const FString &LoginName);

	UPROPERTY()
	UKBENetConnect* ServerConnect;

	UPROPERTY()
	TMap< FString, UKBENetConnect*> ServerConnects;

	TMap<int, FString> EntityClassIndex;

	//登陆回调
	DECLARE_DELEGATE_ThreeParams(FKBELoginDelegate, const FString&, int, const FString&);
	FKBELoginDelegate KBELoginDelegate;

	//玩家进入场景回调
	DECLARE_DELEGATE_OneParam(FKBEEntityEnterWorldDelegate, int);
	FKBEEntityEnterWorldDelegate KBEPlayerEnterWorldDelegate;

	//Entity进入场景回调
	FKBEEntityEnterWorldDelegate KBEEntityEnterWorldDelegate;

};

