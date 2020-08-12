// Fill out your copyright notice in the Description page of Project Settings.


#include "KBEAgentEntity.h"

#pragma optimize("", off)
void UKBEAgentEntity::OnInitPropertyFinish_Implementation()
{
	if (!IsRunningDedicatedServer())
	{
		return;
	}
	if (!ByControll)
	{
		return;
	}
	//设置位置
	for (TActorIterator<AActor> Itor(GetWorld()); Itor; ++Itor)
	{
		TInlineComponentArray<UKBEAgentEntity*> ComponentArray;
		(*Itor)->GetComponents<UKBEAgentEntity>(ComponentArray);
		if (ComponentArray.Num() != 1)
		{
			continue;
		}

		if (ComponentArray[0]->EntityID != 0)
		{
			continue;
		}

		if (ComponentArray[0]->PlayerName == PlayerName)
		{
			const FVector &Pos = (*Itor)->GetActorLocation();
			GetOwner()->SetActorLocation(Pos);
			Cell->SetAgentLocation(Unreal2KBEnginePosition(Pos));
			(*Itor)->Destroy();
			break;
		}
	}
}
void UKBEAgentEntity::OnSetAgentLocation_Implementation()
{
	if (!IsRunningDedicatedServer())
	{
		return;
	}

	if (!ByControll)
	{
		return;
	}
	TArray<FCREATEMONSTERARG> Arg;
	//获取要控制的怪物
	for (TActorIterator<AActor> Itor(GetWorld()); Itor; ++Itor)
	{
		TInlineComponentArray<UMonsterEntity*> ComponentArray;
		(*Itor)->GetComponents<UMonsterEntity>(ComponentArray);
		if (ComponentArray.Num() != 1)
		{
			continue;
		}

		if (ComponentArray[0]->EntityID != 0)
		{
			continue;
		}

		float Range = ControlRange * ControlRange;
		if(((*Itor)->GetActorLocation() - GetOwner()->GetActorLocation()).SizeSquared2D() < Range)
		{
			AgentActor.Add(ComponentArray[0]->PlayerName, (*Itor));
			Arg.Add({ComponentArray[0]->EntityClassName, ComponentArray[0]->PlayerName, Unreal2KBEnginePosition((*Itor)->GetActorLocation())});
		}
	}

	//通知服务器创建和控制实体
	Cell->CreateControllMonster(Arg);
}
#pragma optimize("", on)
