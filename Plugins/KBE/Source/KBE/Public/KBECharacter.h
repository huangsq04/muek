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

	//��ȡ��������
	void GetFloorPoint(const FVector &Location, FHitResult &OutHit, float Radius);

	void KBEReplicatedBasedMovement();

	void OnUpdateEntityMovement();

	void KBESetRemoteRole(const ENetRole InRemoteRole);

private:

	//������ͼ�����ӵ�Entity���
	UEntity *Entity;

	TArray<FRepMovement> RepMovements;

	//����ٶȵİٷֱ�
	float MaxSpeedPercentage;

	//�ϴη�����ͬ��������λ��
	FVector OldSyncLocation;
};