// Fill out your copyright notice in the Description page of Project Settings.


#include "KBEMonster.h"
#include "KBEBlueprintFunctionLibrary.h"
#include "TroopsComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DefineEntity.h"
#include "Engine/World.h"

#pragma optimize("", off)
AKBEMonster::AKBEMonster()
{
	TroopsComponent = CreateDefaultSubobject<UTroopsComponent>(TEXT("TroopsComponent"));
}
void AKBEMonster::BeginPlay()
{
	Super::BeginPlay();
	if (!IsRunningDedicatedServer() && UKBEBlueprintFunctionLibrary::GetPlayer() == nullptr)
	{
		TroopsComponent->CreateFollowCharacter();
	}
}
void AKBEMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AKBEMonster::ComeHere()
{
	TroopsComponent->ComeHere();
}
void AKBEMonster::CallKBEComeHere()
{
	GetController()->StopMovement();
	UMonsterEntity *MonsterEntity = Cast<UMonsterEntity>(ComponentEntity());
	if (MonsterEntity->GetCell() != nullptr && IsRunningDedicatedServer())
	{
		//通知其他客户端
		MonsterEntity->Cell->CellComeHere();
	}
	else if (HasAuthority())
	{
		TroopsComponent->ComeHere();
	}
}

void AKBEMonster::FollowMe()
{
	TroopsComponent->FollowMe();
}
void AKBEMonster::CallKBEFollowMe()
{
	UMonsterEntity *MonsterEntity = Cast<UMonsterEntity>(ComponentEntity());
	if (MonsterEntity->GetCell() != nullptr && IsRunningDedicatedServer())
	{
		//通知其他客户端
		MonsterEntity->Cell->CellFollowMe();
	}
	else if (HasAuthority())
	{
		TroopsComponent->FollowMe();
	}
}

void AKBEMonster::StopFollowMe()
{
	TroopsComponent->StopFollowMe();
}
void AKBEMonster::CallKBEStopFollowMe()
{
	UMonsterEntity *MonsterEntity = Cast<UMonsterEntity>(ComponentEntity());
	if (MonsterEntity->GetCell() != nullptr && IsRunningDedicatedServer())
	{
		//通知其他客户端
		MonsterEntity->Cell->CellStopFollowMe();
	}
	else if (HasAuthority())
	{
		TroopsComponent->StopFollowMe();
	}
}

FVector &AKBEMonster::RandomTargetPoint(float Radius)
{
	FVector T = UKismetMathLibrary::RandomUnitVector() * Radius;

	T.Z = 0.0;

	TargetPoint = StartPoint + T;

	return TargetPoint;
}
#pragma optimize("", on)
