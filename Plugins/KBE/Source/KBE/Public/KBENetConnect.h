// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Core.h"
#include "Engine.h"
#include "Mailbox.h"
#include "Entity.h"
#include "KBENetConnect.generated.h"
/**
 *  链接KBE服务器类
 */
UCLASS()
class KBE_API UKBENetConnect : public UObject
{
	GENERATED_BODY()
	
public:
	enum COMPONENT_CLIENT_TYPE
	{
		UNKNOWN_CLIENT_COMPONENT_TYPE = 0,

		// 移动类，手机，平板电脑
		// Mobile, Phone, Pad
		CLIENT_TYPE_MOBILE = 1,

		// 独立的Windows应用程序
		// Windows Application program
		CLIENT_TYPE_WIN = 2,

		// 独立的Linux应用程序
		// Linux Application program
		CLIENT_TYPE_LINUX = 3,

		// Mac应用程序
		// Mac Application program
		CLIENT_TYPE_MAC = 4,

		// Web, HTML5, Flash
		CLIENT_TYPE_BROWSER = 5,

		// bots
		CLIENT_TYPE_BOTS = 6,

		// 轻端类
		CLIENT_TYPE_MINI = 7,

		// End
		CLIENT_TYPE_END = 8
	};


	const FString ClientScriptVersion = "0.1.0";
	const FString ClientVersion = "1.2.2";

	void Init(UNetDriver *NetDriver);

	enum READ_STATE
	{
		// 消息ID
		READ_STATE_MSGID = 0,
		// 消息的长度65535以内
		READ_STATE_MSGLEN = 1,
		// 当上面的消息长度都无法到达要求时使用扩展长度
		// uint32
		READ_STATE_MSGLEN_EX = 2,
		// 消息的内容
		READ_STATE_BODY = 3
	};
	static const int TCP_PACKET_MAX = 65535;

	template<class K, class V>
	bool TryGetValue(const TMap< K, V> &Map, const K &Key, V &Value)
	{
		if (auto pValue = Map.Find(Key))
		{
			Value = *pValue;
			return true;
		}
		return false;
	}

	bool InitSocket(uint32 ReceiveBufferSize = 0, uint32 SendBufferSize = 0);

	virtual void BeginDestroy();

	bool ConnectToLoginApp(FString Ip, uint16 Port, const FString &Name, const FString &InPassword);

	bool OnEnterLobby();

	bool ConnectToBaseApp(FString Ip, uint16 Port, const FString &Name, const FString &InPassword);

	bool Send(uint8* Datas, int32 Length);

	bool AddEntity(int32 Idx, UEntity* Entity);

	bool RemoveEntity(int32 Idx);

	void Tick(float DeltaTime);

	void KeepAlive();

	AActor* Player();

	UEntity* GetPlayerEntity();

	UEntity* GetEntity(int Id);

	void Process(uint8* Datas, uint32 Offset, uint32 Length);

	bool BindEvent(uint16 Id, FString Name, int16 ArgNumb);

	void ImportClientMessagesCompleted();

	void UpdateDataToKBEServer(float DeltaTime);

	void UpdatePlayerLocationToKBEServer();

	void UpdateEntityMovement(int32 InId, const FVector &InPos = FVector::ZeroVector, int GroundType = -1);

	void UpdateEntityDirection(int32 InId, const FVector &InDirection);

	//更新玩家速度,传的是达到最大速度的缩放值
	void UpdateEntitySpeed(int32 InId, float Speed);

	FMessage *GetHandleMessage(uint16 Idx);

	int32 GetAoiEntityIDFromStream(FMemoryStream *Stream);

	int GetConnectStatu() { return ConnectStatus; }

	void OnVersionNotMatch(FMemoryStream *Stream) {}
	void OnScriptVersionNotMatch(FMemoryStream *Stream) {}
	void OnImportClientMessages(FMemoryStream *Stream);
	void OnHelloCB(FMemoryStream *Stream);
	void OnLoginFailed(FMemoryStream *Stream);
	void OnLoginSuccessfully(FMemoryStream *Stream);
	void OnAppActiveTickCB(FMemoryStream *Stream);
	//更新实体属性
	void OnUpdatePropertys(FMemoryStream *Stream);
	//创建通信代理
	void OnCreatedProxies(FMemoryStream *Stream);
	void OnEntityDestroyed(FMemoryStream *Stream);
	void OnEntityEnterWorld(FMemoryStream *Stream);
	void OnInitSpaceData(FMemoryStream *Stream);
	void OnSetEntityPosAndDir(FMemoryStream *Stream);
	void OnEntityEnterSpace(FMemoryStream *Stream);
	void OnUpdateBasePosXZ(FMemoryStream *Stream);
	void OnUpdateData_xyz(FMemoryStream *Stream);
	void OnEntityLeaveWorld(FMemoryStream *Stream);
	void OnControlEntity(FMemoryStream *Stream);
	void OnUpdateData_xz_ypr(FMemoryStream *Stream);
	void OnUpdateData_xz(FMemoryStream *Stream);
	void OnRemoteMethodCallOptimized(FMemoryStream *Stream);
	void OnRemoteMethodCall(FMemoryStream *Stream);
	void OnUpdatePropertysOptimized(FMemoryStream *Stream);
	void OnEntityLeaveSpace(FMemoryStream *Stream);
	void OnUpdateBasePos(FMemoryStream *Stream);
	//方向更新
	void OnUpdateData_ypr(FMemoryStream *Stream);
	//空中位置更新
	void OnUpdateData_xyz_ypr(FMemoryStream *Stream);


	void RemoveEntityExceptPlayer();

	FSocket *GetSocket() { return Socket; }

	UFUNCTION(BlueprintCallable, Category = "Entity")
	int32 GetProxyEntityID();
	
	//控制的实体ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	int32 ProxyEntityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	int32 SpaceID;

private:

	FBitReader ReadBuffer;

	//网络数据
	uint8 *RecvBuffer;
	//每tick读取的大小
	int32 BytesRead = 0;

	FSocket *Socket = nullptr;

	ISocketSubsystem *SocketSubsystem = nullptr;

	uint16 MsgID = 0;

	uint32 ExpectSize = 2;

	READ_STATE State = READ_STATE::READ_STATE_MSGID;

	FString UserName;

	FString Password;

	//连接状态-1：无连接，0：loginapp 1：baseapp
	int ConnectStatus = -1;

	//心跳时间5秒
	float KeepAliveCount = 5.0f;

	float FreqUpdatePlayer = 0.01f;

	TMap<uint16, FMessage *> ClientMessage;

	TArray< FMessage *> Messages;

	TMap<FString, FMessage *> LoginAppMessage;

	TMap<FString, FMessage *> BaseAppMessage;

	TArray<uint8> EncryptedKey;

	//所有的实体
	TMap<int32, UEntity*> Entities;

	//被当前客户端控制的实体
	TArray<int32> ControlledEntities;
	TArray<int32> SyncControlledEntities;
	int MaxSyncEntitiesNum = 20;

	//缓存网络数据
	TMap<int32, FMemoryStream*> BufferedCreateEntityMessage;

	TArray<int32> EntityIDAliasIDList;

	//同步到服务器的玩家位置
	FVector AckPlayerPosition;

	FString LoginIP;

	FString BaseappIP;

	uint16 BaseappPort;

	TArray<uint8> ServerDatas;
	 
	UNetDriver *KBENetDriver;
};
