// Fill out your copyright notice in the Description page of Project Settings.


#include "KBEAccount.h"
#include "Kismet/KismetMathLibrary.h"
#include "MUEKGameMode.h"

#pragma optimize("", off)
void UKBEAccount::RespCreatePlayer_Implementation(const int32 ID, const FString &RoleName)
{
	if (bIsAutoEnterWorld)
	{
		Base->PlayerEnter(ID, TEXT("MapA"));
	}
	else
	{
		RoleList.Items.Add({ ID, RoleName });
		AMUEKGameMode *GameMode = Cast<AMUEKGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->NotifyRoleListChange();
		}
	}
}

void UKBEAccount::RespGetRoleInfo_Implementation(const FROLEINFO &arg0)
{

	AMUEKGameMode *GameMode = Cast<AMUEKGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (bIsAutoEnterWorld)
	{
		if (RoleList.Items.Num() > 0)
		{
			Base->PlayerEnter(RoleList.Items[0].Id, TEXT("MapA"));
		}
		else
		{
			if (GameMode)
			{
				GameMode->KBECreateRole();
			}
		}
	}
	else
	{
		if (GameMode)
		{
			GameMode->NotifyRoleListChange();
		}
	}
}
void UKBEAccount::CreateRole(const FString &Name)
{
	if (RoleList.Items.Num() >= 4)
	{
		FString Msg = *FString::Printf(TEXT("%s"), TEXT("Role number exceed 4 ...."));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *Msg);
		return;
	}

	FString RoleName = *FString::Printf(TEXT("%s%d"), *Name, UKismetMathLibrary::RandomIntegerInRange(100, 900));
	Base->OnCreatePlayer(RoleName);
}
const FROLEITEM *UKBEAccount::GetRoleInfo(const FString &Name)
{

	for (FROLEITEM &Item : RoleList.Items)
	{
		if (Item.Name == Name)
		{
			return &Item;
		}
	}
	return nullptr;
}

#pragma optimize("", on)
