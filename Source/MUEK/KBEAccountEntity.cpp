// Fill out your copyright notice in the Description page of Project Settings.


#include "KBEAccountEntity.h"
#include "Kismet/KismetMathLibrary.h"
#include "MUEKGameMode.h"

#pragma optimize("", off)
void UKBEAccountEntity::RespCreatePlayer_Implementation(const int32 ID, const FString &RoleName)
{
	UE_LOG(LogTemp, Warning, TEXT("RespCreatePlayer_Imp....."));
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

void UKBEAccountEntity::RespGetRoleInfo_Implementation(const FROLEINFO &arg0)
{
	UE_LOG(LogTemp, Warning, TEXT("RespGetRoleInfo_Imp....."));
	AMUEKGameMode *GameMode = Cast<AMUEKGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (bIsAutoEnterWorld)
	{
		if (RoleList.Items.Num() > 0)
		{
			Base->PlayerEnter(RoleList.Items[0].Id, RoleList.Items[0].Map);
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
void UKBEAccountEntity::CreateRole(const FString &Name)
{
	if (RoleList.Items.Num() >= 4)
	{
		FString Msg = *FString::Printf(TEXT("%s"), TEXT("Role number exceed 4 ...."));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *Msg);
		return;
	}

	FString RoleName = Name;
	if (!IsRunningDedicatedServer())
	{
		RoleName = *FString::Printf(TEXT("%s%d"), *RoleName, UKismetMathLibrary::RandomIntegerInRange(100, 900));
	}
	Base->OnCreatePlayer(RoleName);
}
const FROLEITEM *UKBEAccountEntity::GetRoleInfo(const FString &Name)
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
