// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Core.h"
#include "Engine.h"
#include "Mailbox.h"
#include "Entity.h"
#include "KBENetConnect.generated.h"
/**
 *  ����KBE��������
 */
UCLASS()
class KBE_API UKBENetConnect : public UObject
{
	GENERATED_BODY()
	
public:
	enum COMPONENT_CLIENT_TYPE
	{
		UNKNOWN_CLIENT_COMPONENT_TYPE = 0,

		// �ƶ��࣬�ֻ���ƽ�����
		// Mobile, Phone, Pad
		CLIENT_TYPE_MOBILE = 1,

		// ������WindowsӦ�ó���
		// Windows Application program
		CLIENT_TYPE_WIN = 2,

		// ������LinuxӦ�ó���
		// Linux Application program
		CLIENT_TYPE_LINUX = 3,

		// MacӦ�ó���
		// Mac Application program
		CLIENT_TYPE_MAC = 4,

		// Web, HTML5, Flash
		CLIENT_TYPE_BROWSER = 5,

		// bots
		CLIENT_TYPE_BOTS = 6,

		// �����
		CLIENT_TYPE_MINI = 7,

		// End
		CLIENT_TYPE_END = 8
	};


	const FString ClientScriptVersion = "0.1.0";
	const FString ClientVersion = "1.2.2";

	void Init(UNetDriver *NetDriver);

	enum READ_STATE
	{
		// ��ϢID
		READ_STATE_MSGID = 0,
		// ��Ϣ�ĳ���65535����
		READ_STATE_MSGLEN = 1,
		// ���������Ϣ���ȶ��޷�����Ҫ��ʱʹ����չ����
		// uint32
		READ_STATE_MSGLEN_EX = 2,
		// ��Ϣ������
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

	//��������ٶ�,�����Ǵﵽ����ٶȵ�����ֵ
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
	//����ʵ������
	void OnUpdatePropertys(FMemoryStream *Stream);
	//����ͨ�Ŵ���
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
	//�������
	void OnUpdateData_ypr(FMemoryStream *Stream);
	//����λ�ø���
	void OnUpdateData_xyz_ypr(FMemoryStream *Stream);


	void RemoveEntityExceptPlayer();

	FSocket *GetSocket() { return Socket; }

	UFUNCTION(BlueprintCallable, Category = "Entity")
	int32 GetProxyEntityID();
	
	//���Ƶ�ʵ��ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	int32 ProxyEntityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	int32 SpaceID;

private:

	FBitReader ReadBuffer;

	//��������
	uint8 *RecvBuffer;
	//ÿtick��ȡ�Ĵ�С
	int32 BytesRead = 0;

	FSocket *Socket = nullptr;

	ISocketSubsystem *SocketSubsystem = nullptr;

	uint16 MsgID = 0;

	uint32 ExpectSize = 2;

	READ_STATE State = READ_STATE::READ_STATE_MSGID;

	FString UserName;

	FString Password;

	//����״̬-1�������ӣ�0��loginapp 1��baseapp
	int ConnectStatus = -1;

	//����ʱ��5��
	float KeepAliveCount = 5.0f;

	float FreqUpdatePlayer = 0.01f;

	TMap<uint16, FMessage *> ClientMessage;

	TArray< FMessage *> Messages;

	TMap<FString, FMessage *> LoginAppMessage;

	TMap<FString, FMessage *> BaseAppMessage;

	TArray<uint8> EncryptedKey;

	//���е�ʵ��
	TMap<int32, UEntity*> Entities;

	//����ǰ�ͻ��˿��Ƶ�ʵ��
	TArray<int32> ControlledEntities;
	TArray<int32> SyncControlledEntities;
	int MaxSyncEntitiesNum = 20;

	//������������
	TMap<int32, FMemoryStream*> BufferedCreateEntityMessage;

	TArray<int32> EntityIDAliasIDList;

	//ͬ���������������λ��
	FVector AckPlayerPosition;

	FString LoginIP;

	FString BaseappIP;

	uint16 BaseappPort;

	TArray<uint8> ServerDatas;
	 
	UNetDriver *KBENetDriver;
};
