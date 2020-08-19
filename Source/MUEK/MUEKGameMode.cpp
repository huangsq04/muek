// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MUEKGameMode.h"
#include "MUEKCharacter.h"
#include "KBE.h"
#include "DefineEntity.h"
#include "KBEAccountEntity.h"
#include "UObject/ConstructorHelpers.h"

#pragma optimize("", off)
AMUEKGameMode::AMUEKGameMode()
{
	Maps = { 
		{ TEXT("MapA"), TEXT("/Game/Maps/MapA") },
		{ TEXT("MapB"), TEXT("/Game/Maps/MapB") },
	};
	LoginType = 0;

	GConfig->GetString(TEXT("MUEK"), TEXT("LoginIp"), LoginIp, GGameIni);
	GConfig->GetInt(TEXT("MUEK"), TEXT("LoginPort"), LoginPort, GGameIni);

	FString DefaultLoginName;
	if (FParse::Value(FCommandLine::Get(), TEXT("LoginName="), DefaultLoginName))
	{
		//自动登陆
		LoginType = 1;
		LoginName = DefaultLoginName;
	}

	FParse::Value(FCommandLine::Get(), TEXT("LoginIp="), LoginIp);

	bUseSeamlessTravel = true;
}

void AMUEKGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
	ActorList.Add(TravelActor);
}

void AMUEKGameMode::BeginPlay()
{
	Super::BeginPlay();

	FName MapName = GetWorld()->GetFName();

	FKBEModule &KBE = FModuleManager::LoadModuleChecked<FKBEModule>(TEXT("KBE"));
	KBENetDriver = KBE.GetKKBEngine();

	GetClassNameFromType(KBENetDriver->EntityClassIndex);
	KBE.World = GetWorld();

	KBENetDriver->KBELoginDelegate.BindUObject(this, &AMUEKGameMode::KBELoginCallback);
	KBENetDriver->KBEPlayerEnterWorldDelegate.BindUObject(this, &AMUEKGameMode::KBEPlayerEnterWorldCallback);

	if (LoginType == 1)
	{
		//自动登陆
		KBELogin(LoginName, LoginName, TEXT(""));
	}
	else if (LoginType == 0)
	{
		if (MapName == TEXT("LobbyMap"))
		{
			//选择角色界面
			LoginName = UGameplayStatics::ParseOption(OptionsString, TEXT("LoginName"));
			LoginType = UGameplayStatics::GetIntOption(OptionsString, TEXT("LoginType"), 0);

			//判断当前状态可能是从登陆界面过来也可能是从游戏地图过来
			switch (KBENetDriver->GetConnectStatu(LoginName))
			{
			case 0:

				UE_LOG(LogTemp, Warning, TEXT("Connect base ....."));
				KBEEnterLobby();
				break;
			case 1:
				Cast<URoleEntity>(GetPlayerEntity())->Base->OnLeaveWorld();
				break;
			default:
				break;
			}
		}
		else
		{
			//进入游戏地图
			LoginName = UGameplayStatics::ParseOption(OptionsString, TEXT("LoginName"));
			LoginType = UGameplayStatics::GetIntOption(OptionsString, TEXT("LoginType"), 0);

			UAccountEntity *Account = Cast<UAccountEntity>(GetPlayerEntity());
			//从角色选择界面进入地图
			if (Account)
			{
				int Id  = UGameplayStatics::GetIntOption(OptionsString, TEXT("Id"), 0);
				Account->Base->PlayerEnter(Id, MapName.ToString());
			}

			//其他地图传送进入地图
			URoleEntity *RoleEntity = Cast<URoleEntity>(GetPlayerEntity());
			if (RoleEntity)
			{
				RoleEntity->Cell->LevelTeleport(MapName.ToString(), FVector(1.0, 5.0, 3.0));
				TravelActor = GetPlayer();
			}
		}
	}
}
void AMUEKGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason != EEndPlayReason::Type::LevelTransition)
	{
		FKBEModule &KBE = FModuleManager::LoadModuleChecked<FKBEModule>(TEXT("KBE"));
		KBE.GetKKBEngine()->Logoff(LoginName);
	}
}

/**
 * 
 */
void AMUEKGameMode::KBELogin(const FString& Name, const FString& Password, const FString& ServerIP)
{
	if (ServerIP != TEXT(""))
	{
		LoginIp = ServerIP;
	}

	LoginName = Name;

	FString MsgString = *FString::Printf(TEXT("LoginName=%s?LoginIP=%s"), *LoginName, *LoginIp);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, MsgString);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *MsgString);
	KBENetDriver->Login(LoginIp, LoginPort, LoginName, Password);
}

/**
 * 登陆消息处理
 */
void AMUEKGameMode::KBELoginCallback(const FString &Name, int type, const FString &Error)
{
	//先调用蓝图重载函数
	LoginCallBP(Name, type, Error);
	if (type == 0)
	{
		//loginapp验证失败
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *Error);
	}
	else if (type == 1)
	{
		//loginapp验证成功加载游戏大厅
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Login ok:") + Name);
		UE_LOG(LogTemp, Warning, TEXT("Login ok:%s"), *Name);

		//显示角色选择地图
		if (LoginType == 0)
		{
			FString OptionString = *FString::Printf(TEXT("LoginName=%s?LoginType=%d"), *LoginName, LoginType);
			UGameplayStatics::OpenLevel((UObject*)this, FName(TEXT("LobbyMap")), true, OptionString);
		}
		else
		{
			KBEEnterLobby();
		}
	}
	else if (type == 2)
	{
		//大厅地图
		UKBEAccountEntity *Entity = Cast<UKBEAccountEntity>(KBENetDriver->GetPlayerEntity(Name));
		if (Entity)
		{
			FString Msg = *FString::Printf(TEXT("%s"), TEXT("Base connect ok ."));
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *Msg);
			UE_LOG(LogTemp, Warning, TEXT("Base connect ok ."));
			//加载玩家角色
			Entity->Base->GetRoleInfo();
			//设置自动登陆
			if (LoginType == 1)
			{
				Entity->bIsAutoEnterWorld = true;
			}
			//设置跨关卡Actor
			TravelActor = GetPlayer();
		}
	}
	else if (type == 3)
	{
		FString Msg = *FString::Printf(TEXT("%s"), TEXT("Base connect failed ."));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *Msg);
		UE_LOG(LogTemp, Warning, TEXT("Base connect failed ."));

		UGameplayStatics::OpenLevel((UObject*)this, FName(TEXT("LoginMap")), true, TEXT(""));
	}
}
//链接base
void AMUEKGameMode::KBEEnterLobby()
{
	KBENetDriver->EnterLobby(LoginName);
}

AActor *AMUEKGameMode::GetPlayer()
{
	if (KBENetDriver)
		return KBENetDriver->GetPlayer(LoginName);

	return nullptr;
}

UEntity *AMUEKGameMode::GetPlayerEntity()
{
	return KBENetDriver->GetPlayerEntity(LoginName);
}

bool AMUEKGameMode::IsKBEConnect()
{
	if (KBENetDriver->GetConnectStatu(LoginName) == -1)
		return false;

	return true;
}
/**
 * 角色进入地图
 */
void AMUEKGameMode::KBEPlayerEnterWorldCallback(int Id)
{
	UE_LOG(LogTemp, Warning, TEXT("KBEPlayerEnterWorldCallback....."));

	APawn *Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AActor *Actor = KBENetDriver->GetPlayer(LoginName);

	if (Pawn != Actor)
	{
		//从登陆界面进入地图
		APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			PlayerController->Possess(Cast<APawn>(Actor));
		}
		if (Pawn)
		{
			Pawn->Destroy();
		}
	}

	TravelActor = GetPlayer();

	OnPlayerEnterWorld(IsRunningDedicatedServer());
}
FString AMUEKGameMode::GetLoginName()
{
	return LoginName;
}
void AMUEKGameMode::KBECreateRole()
{
	UKBEAccountEntity *Entity = Cast<UKBEAccountEntity>(KBENetDriver->GetPlayerEntity(LoginName));
	if (Entity == nullptr)
	{
		FString Msg = *FString::Printf(TEXT("%s"), TEXT("Current Player is not Account ...."));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *Msg);
		return;
	}

	Entity->CreateRole(LoginName);
}

/**
 * 玩家进入游戏地图
 */
void AMUEKGameMode::KBEEnterSpace(const FString &Name)
{
	UE_LOG(LogTemp, Warning, TEXT("KBEEnterSpace [%s]....."), *Name);

	UKBEAccountEntity *Entity = Cast<UKBEAccountEntity>(KBENetDriver->GetPlayerEntity(LoginName));
	if (Entity == nullptr)
	{
		FString Msg = *FString::Printf(TEXT("%s"), TEXT("Current Player is not Account ...."));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *Msg);
		return;
	}
	const FROLEITEM *RoleItem = Entity->GetRoleInfo(Name);

	if (RoleItem == nullptr)
	{
		FString Msg = *FString::Printf(TEXT("%s"), TEXT("Current Player is not role ...."));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *Msg);
		return;
	}

	if (RoleItem->Map == TEXT(""))
	{
		FString Msg = *FString::Printf(TEXT("Not map select %s"), *Name);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *Msg);
		return;
	}

	FString OptionString = *FString::Printf(TEXT("%s?LoginName=%s?LoginType=%d?Id=%d"), *RoleItem->Map, *LoginName, LoginType, RoleItem->Id);
	GetWorld()->SeamlessTravel(OptionString);
}

void AMUEKGameMode::NotifyRoleListChange()
{
	UpdateRoleListDelegate.Broadcast();
}

/**
 * 玩家切换地图
 */
void AMUEKGameMode::KBELevelTeleport(const FString &Map, const FVector Position)
{
	UE_LOG(LogTemp, Warning, TEXT("KBELevelTeleport [%s]....."), *Map);

	FName MapName = GetWorld()->GetFName();
	if (MapName == *Map)
	{
		return;
	}

	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->UnPossess();
	}

	//SeamlessTravel 会把TheActor->Role < ROLE_Authority 的Actor带过去，所以要在发送命令到服务器前
	//删除这些服务器怪物和其他玩家。
	FString OptionString = *FString::Printf(TEXT("%s?LoginName=%s?LoginType=%d"), *Map, *LoginName, LoginType);
	GetWorld()->SeamlessTravel(OptionString);
}
void AMUEKGameMode::KBELeaveWorld()
{
	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->UnPossess();
	}

	FString OptionString = *FString::Printf(TEXT("LobbyMap?LoginName=%s?LoginType=%d"), *LoginName, LoginType);
	GetWorld()->SeamlessTravel(OptionString);
}
#pragma optimize("", on)
