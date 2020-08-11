#pragma once
#include "Core.h"
#include "Engine.h"
#include "Mailbox.h"
#include "Entity.generated.h"
class UKBENetConnect;
#define __SET_ENTITY_POSITION() do                   \
{                                                    \
    KBESyncLocation.X = 100.0f * Stream.ReadFloat(); \
    KBESyncLocation.Z = 100.0f * Stream.ReadFloat(); \
    KBESyncLocation.Y = 100.0f * Stream.ReadFloat(); \
	SetPosition(KBESyncLocation);                    \
}while (0)
#define __SET_ENTITY_DIRECTION() do          \
{                                            \
    KBESyncDirection.X = Stream.ReadFloat(); \
    KBESyncDirection.Y = Stream.ReadFloat(); \
    KBESyncDirection.Z = Stream.ReadFloat(); \
	SetDirection(KBESyncDirection);          \
}while (0)


#define NEW_CLIENT_MESSAGE(NAME,FUNCTION) do\
{\
    FMessage *Message = nullptr; \
    Message = new FMessage(NAME); \
	Message->NetMessageDelegate.BindUObject(this, FUNCTION); \
    Messages.Add(Message); \
}while (0)


static inline FVector KBEngine2UnrealPosition(const FVector& KBEnginePoint)
{
	return FVector(KBEnginePoint[0] * 100, KBEnginePoint[2] * 100, KBEnginePoint[1] * 100);
}
static inline FVector Unreal2KBEnginePosition(const FVector& KBEnginePoint)
{
	return FVector(KBEnginePoint[0] / 100.0f, KBEnginePoint[2] / 100.0f, KBEnginePoint[1] / 100.0f);
}
/**
 * 同步到服务器的信息
 */
struct FSyncEntityInfo
{
	FVector Position;
	FVector Direction;
	float Percentage;
	bool operator==(const FSyncEntityInfo& A)
	{
		return Position == A.Position || Direction == A.Direction || Percentage == A.Percentage;
	}
};

UCLASS(BlueprintType, Blueprintable)
class KBE_API UEntity : public UActorComponent {
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnByControllDelegate, bool, IsByControll);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitPropertiesFinishDelegate);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
	int32 EntityID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
	FString EntityClassName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
	int32 SpaceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	FVector KBESyncLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	FVector KBESyncDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	bool ByControll = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	bool InWorld = false;

	UFUNCTION(BlueprintCallable, Category = "Entity")
	UKBENetConnect *GetKBENetConnect() { return KBENetConnect; };

	UFUNCTION(BlueprintCallable, Category = "Entity")
	bool IsKBEConnect();

	UFUNCTION(BlueprintCallable, Category = "Entity")
	const FString & GetClassName() { return EntityClassName; };

	UFUNCTION(BlueprintNativeEvent, Category = "MUEK")
	void OnInitPropertyFinish();
	virtual void OnInitPropertyFinish_Implementation() {};

	virtual void PostInitProperties() override;

	virtual void BeginDestroy() override;

	virtual void DestroyComponent(bool bPromoteChildren = false);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	}


	virtual void SetID(int32 Id) { EntityID = Id; };
	virtual int32 GetID() { return EntityID; };

	void SetClassName(const FString &ClassName) { EntityClassName = ClassName; };

	virtual void SetBaseMailbox(FMailbox *Mailbox) { BaseMailBox = Mailbox; };

	virtual void SetCellMailbox(FMailbox *Mailbox) { CellMailBox = Mailbox; };

	FMailbox * BaseMailbox() { return BaseMailBox; };

	FMailbox * CellMailbox() { return CellMailBox; };

	virtual UObject *GetCell() { return nullptr; };
	virtual UObject *GetBase() { return nullptr; };

	virtual void OnRemoteMethodCall(FMemoryStream& stream) {};

	virtual void OnUpdatePropertys(FMemoryStream& stream);

	virtual void OnUpdateEntityMovement(const FVector &Pos);

	virtual void OnUpdateEntityDirection(const FVector &Direction);

	virtual FString OnUpdateProperty(uint16 utype, FMemoryStream& stream) { return TEXT(""); };

	virtual void Init() {};

	virtual bool UsePropertyDescrAlias() { return false; };

	virtual void OnDestroy();
	virtual FVector GetPosition();
	virtual FVector GetDirection();

	virtual void SetPosition(const FVector &Position);
	virtual void SetDirection(const FVector &Direction);

	virtual void SetByControll(bool Value);
	virtual bool GetByControll() { return ByControll; };

	virtual void SetInWorld(bool Value) { InWorld = Value; };
	virtual bool GetInWorld() { return InWorld; };

	virtual float GetSpeedPercentage();

	virtual void SetSpeedPercentage(float Percentage);

	bool IsOnGround();

	void SetGround(bool Ground);

	void RedirectCallFunction(const TArray<uint8> &Arg);

	void InitPropertiesFinish();

	static AActor * CreateEntityObject(const FString &Name, UObject* Outer = nullptr);

	void SetKBENetConnect(UKBENetConnect *Connect) {	KBENetConnect = Connect;	};

	bool NeedSyncEntityInfo();
	const FSyncEntityInfo *GetSyncEntityInfo() { return SyncEntityInfo; };

	void SetPlayerNameProperty(const FString &Value);

	UPROPERTY(BlueprintAssignable)
	FOnByControllDelegate OnByControllDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInitPropertiesFinishDelegate OnInitPropertiesFinishDelegate;

private:
	FMailbox *BaseMailBox = nullptr;
	FMailbox *CellMailBox = nullptr;
	UKBENetConnect *KBENetConnect = nullptr;

	//已经同步到服务器的信息
	FSyncEntityInfo *SyncEntityInfo = nullptr;

};

UCLASS(BlueprintType, Blueprintable)
class KBE_API UEntityServerMethod : public UObject {
	GENERATED_BODY()
public:
	UEntity *Entity = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class KBE_API UEntityInterface : public UObject {
	GENERATED_BODY()
public:
	UEntity *Entity = nullptr;
};