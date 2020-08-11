#pragma once
#include "Core.h"
#include "Engine.h"
#include "Bundle.h"
#include "Networking.h"
//#include "Net/DataReplication.h"
//#include "Net/RepLayout.h"
#include "Message.h"
#include "Mailbox.h"
#include "Entity.h"
#include "KBECharacter.generated.h"

UCLASS(BlueprintType, Blueprintable)
class KBE_API AKBECharacter : public ACharacter
{
	GENERATED_BODY()
public:

	AKBECharacter();

	virtual void Tick(float DeltaTime);

	virtual UEntity *ComponentEntity();

	UFUNCTION(BlueprintImplementableEvent, Category = "Entity")
	void OnInitFinish();

	virtual void BeginDestroy() override;
	virtual void BeginPlay();

	float GetSpeedPercentage();
	void  SetSpeedPercentage(float Percentage);

	//获取地面坐标
	void GetFloorPoint(const FVector &Location, FHitResult &OutHit, float Radius);

	void KBEReplicatedBasedMovement();

	void OnUpdateEntityMovement();

	void KBESetRemoteRole(const ENetRole InRemoteRole);

private:

	//保存蓝图中增加的Entity组件
	UEntity *Entity;

	TArray<FRepMovement> RepMovements;

	//最大速度的百分比
	float MaxSpeedPercentage;

	//上次服务器同步过来的位置
	FVector OldSyncLocation;
};