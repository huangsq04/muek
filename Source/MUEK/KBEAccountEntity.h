// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefineEntity.h"
#include "KBEAccountEntity.generated.h"

/**
 * 
 */
UCLASS()
class MUEK_API UKBEAccountEntity : public UAccountEntity
{
	GENERATED_BODY()
public:	
	virtual void RespCreatePlayer_Implementation(const int32 ID, const FString &RoleName) override;

	virtual void RespGetRoleInfo_Implementation(const FROLEINFO &arg0) override;

	void CreateRole(const FString &Name);

	/** 获取角色信息 */
	const FROLEITEM *GetRoleInfo(const FString &Name);

	bool bIsAutoEnterWorld = false;
};
