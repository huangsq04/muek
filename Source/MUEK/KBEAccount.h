// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefineEntity.h"
#include "KBEAccount.generated.h"

/**
 * 
 */
UCLASS()
class MUEK_API UKBEAccount : public UAccountEntity
{
	GENERATED_BODY()
public:

	virtual void RespCreatePlayer_Implementation(const int32 ID, const FString &RoleName);

	virtual void RespGetRoleInfo_Implementation(const FROLEINFO &arg0);

	void CreateRole(const FString &Name);

	/** 获取角色信息 */
	const FROLEITEM *GetRoleInfo(const FString &Name);

	bool bIsAutoEnterWorld = false;
};
