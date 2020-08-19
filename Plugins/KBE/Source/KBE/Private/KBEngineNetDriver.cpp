// Fill out your copyright notice in the Description page of Project Settings.

#include "KBEngineNetDriver.h"
#pragma optimize("", off)

UKBEngineNetDriver::UKBEngineNetDriver()
{
}

void UKBEngineNetDriver::BeginDestroy()
{
	Super::BeginDestroy();
}
void UKBEngineNetDriver::Tick(float DeltaTime)
{
	TArray<FString> FailedConnects;
	for (TPair<FString, UKBENetConnect*>& Element : ServerConnects)
	{
		if (Element.Value)
		{
			if (Element.Value->GetSocket() != nullptr)
			{
				Element.Value->Tick(DeltaTime);
			}
			else
			{
				FailedConnects.Add(Element.Key);
				if (ServerConnect == Element.Value)
				{
					ServerConnect = nullptr;
				}
			}
		}		
	}

	for (const FString &Item : FailedConnects)
	{
		ServerConnects.Remove(Item);
	}
}
bool UKBEngineNetDriver::Login(const FString &Ip,
	uint16 Port,
	const FString &LoginName,
	const FString &Password)
{
	ServerConnect = NewObject<UKBENetConnect>();
	ServerConnect->AddToRoot();
	ServerConnect->Init(this);
	if (!ServerConnect->ConnectToLoginApp(Ip, Port, LoginName, Password))
	{
		FString Message = *FString::Printf(TEXT("Connect loginapp faild ..[%s:%d][%s]..."), *Ip, Port, *LoginName);
		if (KBELoginDelegate.IsBound())
		{
			KBELoginDelegate.Execute(LoginName, 0, Message);
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

		ServerConnect->ConditionalBeginDestroy();
		ServerConnect = nullptr;
		return false;
	}
	else
	{
		ServerConnects.Add(LoginName, ServerConnect);
		return true;
	}
	
}

bool UKBEngineNetDriver::EnterLobby(const FString &LoginName)
{
	UKBENetConnect **Connect = ServerConnects.Find(LoginName);

	if (Connect != nullptr)
	{
		return (*Connect)->OnEnterLobby();
	}

	return false;
}
void UKBEngineNetDriver::Logoff(const FString &LoginName)
{
	UKBENetConnect **Connect = ServerConnects.Find(LoginName);

	if (Connect != nullptr)
	{
		UEntity *AccountEntity = (*Connect)->GetPlayerEntity();
		if (AccountEntity == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), TEXT("connect not account .."));
			return;
		}

		UObject *Base = AccountEntity->GetBase();
		if (Base != nullptr)
		{
			UFunction* Function = Base->GetClass()->FindFunctionByName(TEXT("logoff"));
			if (Function != nullptr)
			{
				FOutputDeviceNull ar;
				FString FunctionName = *FString::Printf(TEXT("logoff"));
				Base->CallFunctionByNameWithArguments(*FunctionName, ar, NULL, true);
			}
		}
		(*Connect)->RemoveFromRoot();
		(*Connect)->ConditionalBeginDestroy();
		ServerConnects.Remove(LoginName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), TEXT("not found connect .."));
	}

}

void UKBEngineNetDriver::LeaveWorld(const FString &LoginName)
{
	UKBENetConnect **Connect = ServerConnects.Find(LoginName);
	if (Connect != nullptr)
	{
		UEntity *Entity = (*Connect)->GetPlayerEntity();
		UObject *Base = Entity->GetBase();
		if (Base != nullptr)
		{
			UFunction* Function = Base->GetClass()->FindFunctionByName(TEXT("OnLeaveWorld"));
			if (Function != nullptr)
			{
				FOutputDeviceNull ar;
				FString FunctionName = *FString::Printf(TEXT("OnLeaveWorld"));
				Base->CallFunctionByNameWithArguments(*FunctionName, ar, NULL, true);
			}
		}
	}
	else
	{

	}
}
int UKBEngineNetDriver::GetConnectStatu(const FString &LoginName)
{
	UKBENetConnect **Connect = ServerConnects.Find(LoginName);
	if (Connect != nullptr)
	{
		return (*Connect)->GetConnectStatu();
	}
	else
	{
		return -1;
	}

};
AActor * UKBEngineNetDriver::GetPlayer(const FString &LoginName)
{
	UKBENetConnect **Connect = ServerConnects.Find(LoginName);
	if (Connect != nullptr)
	{
		return  (*Connect)->Player();
	}
	return nullptr;
}

UEntity *UKBEngineNetDriver::GetPlayerEntity(const FString &LoginName)
{
	UKBENetConnect **Connect = ServerConnects.Find(LoginName);
	if (Connect != nullptr)
	{
		return (*Connect)->GetPlayerEntity();
	}
	else
	{
		return nullptr;
	}
}
void UKBEngineNetDriver::RemoveEntityExceptPlayer(const FString &LoginName)
{
	UKBENetConnect **Connect = ServerConnects.Find(LoginName);
	if (Connect != nullptr)
	{
		(*Connect)->RemoveEntityExceptPlayer();
	}
}
#pragma optimize("", on)
