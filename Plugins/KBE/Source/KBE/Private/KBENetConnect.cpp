// Fill out your copyright notice in the Description page of Project Settings.

#include "KBENetConnect.h"
#include "KBE.h"
#include "KBEngineNetDriver.h"
#include "GameFramework/DefaultPawn.h"
#include "KBECharacter.h"
#pragma optimize("", off)
static inline float INT8_TO_ANGLE(int8 Angle, bool Half = false)
{
	return float(Angle) * float((PI / (Half ? 254.f : 128.f)));
}
void UKBENetConnect::Init(UNetDriver *NetDriver)
{
	KBENetDriver = NetDriver;
	//登记服务器调用的客户端方法
	NEW_CLIENT_MESSAGE(TEXT("Client_onHelloCB"), &UKBENetConnect::OnHelloCB);
	NEW_CLIENT_MESSAGE(TEXT("Client_onScriptVersionNotMatch"), &UKBENetConnect::OnScriptVersionNotMatch);
	NEW_CLIENT_MESSAGE(TEXT("Client_onVersionNotMatch"), &UKBENetConnect::OnVersionNotMatch);
	NEW_CLIENT_MESSAGE(TEXT("Client_onImportClientMessages"), &UKBENetConnect::OnImportClientMessages);

	NEW_CLIENT_MESSAGE(TEXT("Client_onLoginFailed"), &UKBENetConnect::OnLoginFailed);
	NEW_CLIENT_MESSAGE(TEXT("Client_onLoginSuccessfully"), &UKBENetConnect::OnLoginSuccessfully);
	NEW_CLIENT_MESSAGE(TEXT("Client_onAppActiveTickCB"), &UKBENetConnect::OnAppActiveTickCB);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdatePropertys"), &UKBENetConnect::OnUpdatePropertys);
	NEW_CLIENT_MESSAGE(TEXT("Client_onCreatedProxies"), &UKBENetConnect::OnCreatedProxies);
	NEW_CLIENT_MESSAGE(TEXT("Client_onEntityDestroyed"), &UKBENetConnect::OnEntityDestroyed);
	NEW_CLIENT_MESSAGE(TEXT("Client_onEntityEnterWorld"), &UKBENetConnect::OnEntityEnterWorld);
	NEW_CLIENT_MESSAGE(TEXT("Client_initSpaceData"), &UKBENetConnect::OnInitSpaceData);
	NEW_CLIENT_MESSAGE(TEXT("Client_onSetEntityPosAndDir"), &UKBENetConnect::OnSetEntityPosAndDir);
	NEW_CLIENT_MESSAGE(TEXT("Client_onEntityEnterSpace"), &UKBENetConnect::OnEntityEnterSpace);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdateBasePosXZ"), &UKBENetConnect::OnUpdateBasePosXZ);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdateData_xyz"), &UKBENetConnect::OnUpdateData_xyz);
	NEW_CLIENT_MESSAGE(TEXT("Client_onEntityLeaveWorldOptimized"), &UKBENetConnect::OnEntityLeaveWorld);
	NEW_CLIENT_MESSAGE(TEXT("Client_onControlEntity"), &UKBENetConnect::OnControlEntity);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdateData_xz_ypr"), &UKBENetConnect::OnUpdateData_xz_ypr);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdateData_xz"), &UKBENetConnect::OnUpdateData_xz);
	NEW_CLIENT_MESSAGE(TEXT("Client_onRemoteMethodCallOptimized"), &UKBENetConnect::OnRemoteMethodCallOptimized);
	NEW_CLIENT_MESSAGE(TEXT("Client_onRemoteMethodCall"), &UKBENetConnect::OnRemoteMethodCall);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdatePropertysOptimized"), &UKBENetConnect::OnUpdatePropertysOptimized);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdateData_ypr"), &UKBENetConnect::OnUpdateData_ypr);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdateData_xyz_ypr"), &UKBENetConnect::OnUpdateData_xyz_ypr);
	NEW_CLIENT_MESSAGE(TEXT("Client_onEntityLeaveSpace"), &UKBENetConnect::OnEntityLeaveSpace);
	NEW_CLIENT_MESSAGE(TEXT("Client_onUpdateBasePos"), &UKBENetConnect::OnUpdateBasePos);

	//绑定信息ID到方法
	BindEvent(521, TEXT("Client_onHelloCB"), -1);
	BindEvent(522, TEXT("Client_onScriptVersionNotMatch"), -1);
	BindEvent(523, TEXT("Client_onVersionNotMatch"), -1);
	BindEvent(518, TEXT("Client_onImportClientMessages"), -1);

	//登记Loginapp的服务器消息
	LoginAppMessage.Add(TEXT("Loginapp_importClientMessages"), new FMessage(5, TEXT("Loginapp_importClientMessages"), 0));
	LoginAppMessage.Add(TEXT("Loginapp_hello"), new FMessage(4, TEXT("Loginapp_hello"), -1));

	//登记Baseapp的服务器消息
	BaseAppMessage.Add(TEXT("Baseapp_importClientMessages"), new FMessage(207, TEXT("Baseapp_importClientMessages"), 0));
	BaseAppMessage.Add(TEXT("Baseapp_importClientEntityDef"), new FMessage(208, TEXT("Baseapp_importClientEntityDef"), 0));
	BaseAppMessage.Add(TEXT("Baseapp_hello"), new FMessage(200, TEXT("Baseapp_hello"), -1));

	RecvBuffer = new uint8[UKBENetConnect::TCP_PACKET_MAX];

	ConnectStatus = -1;

	InitSocket();
}
void UKBENetConnect::BeginDestroy()
{
	Super::BeginDestroy();

	for (TPair<int32, UEntity*>& Element : Entities)
	{
		if (Element.Value->IsValidLowLevel())
		{
			Element.Value->OnDestroy();
		}
	}
	Entities.Empty();
}
//保持到服务器的心跳
void UKBENetConnect::KeepAlive()
{
	FMessage **MessagePtr = nullptr;

	//loginapp
	if (ConnectStatus == 0)
	{
		MessagePtr = LoginAppMessage.Find(TEXT("Loginapp_onClientActiveTick"));
	}
	else if (ConnectStatus == 1)
	{
		MessagePtr = BaseAppMessage.Find(TEXT("Baseapp_onClientActiveTick"));
	}

	if (!MessagePtr)return;

	FBundle* BundlePtr = new FBundle();
	BundlePtr->NewMessage((*MessagePtr)->MessageID, (*MessagePtr)->ArgNum);
	BundlePtr->Send(Socket);
	delete BundlePtr;
}
bool UKBENetConnect::InitSocket(uint32 ReceiveBufferSize, uint32 SendBufferSize)
{
	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	if (SocketSubsystem != nullptr)
	{
		FString descript = TEXT("KBEngine");
		Socket = SocketSubsystem->CreateSocket(NAME_Stream, descript, true);

		if (Socket != nullptr)
		{
			bool Error = !Socket->SetReuseAddr(true) ||
				!Socket->SetLinger(false, 0) ||
				!Socket->SetRecvErr();

			if (!Error)
			{
				Error = !Socket->SetNonBlocking(false);
			}
			if (!Error)
			{
				int32 OutNewSize;

				if (ReceiveBufferSize > 0)
				{
					Socket->SetReceiveBufferSize(ReceiveBufferSize, OutNewSize);
				}
				if (SendBufferSize > 0)
				{
					Socket->SetSendBufferSize(SendBufferSize, OutNewSize);
				}
			}

			if (Error)
			{
				SocketSubsystem->DestroySocket(Socket);

				Socket = nullptr;
				return false;
			}
		}
	}
	return true;
}
bool UKBENetConnect::ConnectToLoginApp(FString Ip, uint16 Port, const FString &Name, const FString &InPassword)
{
	auto Addr = SocketSubsystem->CreateInternetAddr(0, Port);
	bool bIsValid;

	Addr->SetIp(*Ip, bIsValid);
	if (Socket->Connect(*Addr) == false)
	{
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
		SocketSubsystem = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Error can not connect login server .....[%s][%d]..."), *Ip, Port);
		if (Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.IsBound())
		{
			Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.Execute(UserName, 0, TEXT("Error can not connect login server ."));
		}
		return false;
	}

	ConnectStatus = 0;

	UserName = Name;

	Password = InPassword;

	FMessage **MessagePtr = nullptr;
	MessagePtr = LoginAppMessage.Find(TEXT("Loginapp_hello"));
	if (!MessagePtr)return false;
	FBundle* BundlePtr = new FBundle();
	BundlePtr->NewMessage((*MessagePtr)->MessageID, (*MessagePtr)->ArgNum);
	BundlePtr->WriteString(ClientVersion);
	BundlePtr->WriteString(ClientScriptVersion);
	BundlePtr->WriteBlob(EncryptedKey);
	BundlePtr->Send(Socket);
	delete BundlePtr;

	LoginIP = Ip;
	return true;
}
bool UKBENetConnect::Send(uint8* Datas, int32 Length)
{
	int32 BytesSent = 0;
	Socket->Send(Datas, Length, BytesSent);
	return true;
}

void UKBENetConnect::Process(uint8* Datas, uint32 Offset, uint32 Length)
{
	uint32 TotalLen = Offset;
	uint16 MsgLen = 0;
	FBitWriter Temp(0, true);

	if (ReadBuffer.GetNumBytes() > 0)
	{
		Temp.SerializeBits(ReadBuffer.GetData(), ReadBuffer.GetBytesLeft());
	}

	Temp.SerializeBits(Datas + Offset, Length * 8);
	FBitReader Reader(Temp.GetData(), Temp.GetNumBits());

	while (Length > 0 && ExpectSize > 0)
	{
		if (Length < ExpectSize)
		{
			FBitReader UnFinishReader(Reader.GetData() + Reader.GetPosBits() / 8, Reader.GetBitsLeft());
			ReadBuffer.SetData(UnFinishReader, UnFinishReader.GetNumBits());
			ExpectSize -= Length;
			break;
		}
		switch (State)
		{
		case  READ_STATE::READ_STATE_MSGID:
		{
			Reader.SerializeBits((void *)&MsgID, sizeof(uint16) * 8);
			TotalLen += ExpectSize;
			Length -= ExpectSize;

			FMessage *MessagePtr = GetHandleMessage(MsgID);
			if (MessagePtr == nullptr)	return;
			if (MessagePtr->ArgNum == -1)
			{
				State = READ_STATE::READ_STATE_MSGLEN;
				ExpectSize = 2;
			}
			else if (MessagePtr->ArgNum == 0)
			{
				// 如果是0个参数的消息，那么没有后续内容可读了，处理本条消息并且直接跳到下一条消息
				MessagePtr->Process(NULL);
				State = READ_STATE::READ_STATE_MSGID;
				ExpectSize = 2;
			}
			else
			{
				ExpectSize = (uint32)MessagePtr->ArgNum;
				State = READ_STATE::READ_STATE_BODY;
			}
			break;
		}
		case   READ_STATE::READ_STATE_MSGLEN:
		{
			TotalLen += ExpectSize;
			Length -= ExpectSize;
			Reader.SerializeBits((void *)&MsgLen, sizeof(uint16) * 8);

			// 长度扩展
			if (MsgLen >= 65535)
			{
				State = READ_STATE::READ_STATE_MSGLEN_EX;
				ExpectSize = 4;
			}
			else
			{
				State = READ_STATE::READ_STATE_BODY;
				ExpectSize = MsgLen;
			}
			break;
		}
		case   READ_STATE::READ_STATE_MSGLEN_EX:
		{
			Reader.SerializeBits((void *)&MsgLen, sizeof(uint32) * 8);
			Length -= ExpectSize;

			Reader.SerializeBits((void *)&MsgLen, sizeof(uint32) * 8);
			State = READ_STATE::READ_STATE_BODY;
			break;
		}
		case   READ_STATE::READ_STATE_BODY:
		{
			FMemoryStream body;
			body.SetData(Reader, ExpectSize * 8);
			TotalLen += ExpectSize;
			Length -= ExpectSize;

			FMessage *msg = GetHandleMessage(MsgID);
			if (msg == nullptr)	return;
			msg->Process(&body);
			State = READ_STATE::READ_STATE_MSGID;
			ExpectSize = 2;
			break;
		}
		default:
			break;
		}
	}
}
void UKBENetConnect::Tick(float DeltaTime)
{
	KeepAliveCount -= DeltaTime;
	if (KeepAliveCount < 0.0f)
	{
		KeepAlive();
		KeepAliveCount = 5.0f;
	}
	uint32 DatagramSize = 0;
	if (Socket->HasPendingData(DatagramSize))
	{
		Socket->Recv(RecvBuffer, UKBENetConnect::TCP_PACKET_MAX, BytesRead);
		if (BytesRead == -1)
		{
		}
		else if (BytesRead == 0)
		{
		}
		else
		{
			Process(RecvBuffer, 0, BytesRead);
		}
	}
	UpdateDataToKBEServer(DeltaTime);
}
void UKBENetConnect::UpdatePlayerLocationToKBEServer()
{
	FreqUpdatePlayer = -0.1;
	UpdateDataToKBEServer(1.0);
}
void UKBENetConnect::UpdateDataToKBEServer(float DeltaTime)
{
	if (FreqUpdatePlayer > 0.0)
	{
		FreqUpdatePlayer = FreqUpdatePlayer - DeltaTime;
		return;
	}

	UEntity** EntityPtr = Entities.Find(ProxyEntityID);
	if (!EntityPtr)return;
	UEntity* PlayerEntity = Cast<UEntity>(*EntityPtr);

	if (PlayerEntity == NULL)
		return;
	if (!PlayerEntity->IsValidLowLevel())
		return;
	if (!PlayerEntity->GetOwner()->IsValidLowLevel())
		return;
	if (PlayerEntity->GetInWorld() == false)
		return;

	if (PlayerEntity->NeedSyncEntityInfo())
	{
		const FSyncEntityInfo *SyncEntityInfo = PlayerEntity->GetSyncEntityInfo();
		const FVector &Position = SyncEntityInfo->Position;
		AckPlayerPosition = SyncEntityInfo->Position;
		const FVector &Direction = Unreal2KBEngineDirection(SyncEntityInfo->Direction);
		float Percentage = SyncEntityInfo->Percentage;
		auto KBEPosition = FVector(Position[0] * 0.01, Position[2] * 0.01, Position[1] * 0.01);

		FMessage **MessagePtr = nullptr;
		MessagePtr = BaseAppMessage.Find(TEXT("Baseapp_onUpdateDataFromClient"));
		if (!MessagePtr)return;

		FBundle* BundlePtr = new FBundle();
		BundlePtr->NewMessage((*MessagePtr)->MessageID, (*MessagePtr)->ArgNum);
		BundlePtr->WriteFloat(KBEPosition.X);
		BundlePtr->WriteFloat(KBEPosition.Y);
		BundlePtr->WriteFloat(KBEPosition.Z);

		BundlePtr->WriteFloat(Percentage);//速度
		BundlePtr->WriteFloat(Direction.Y);
		BundlePtr->WriteFloat(Direction.Z); //旋转
		BundlePtr->WriteUint8((uint8)(PlayerEntity->IsOnGround()));
		BundlePtr->WriteUint32(SpaceID);

		BundlePtr->Send(Socket);
		delete BundlePtr;
		FreqUpdatePlayer = 0.01f;
		AckPlayerPosition = Position;
	}
	FreqUpdatePlayer -= DeltaTime;
	FMessage **CEMessagePtr = nullptr;
	CEMessagePtr = BaseAppMessage.Find(TEXT("Baseapp_onUpdateDataFromClientForControlledEntity"));
	if (!CEMessagePtr)return;

	//开始同步被控制了的entity的位置每次

	if (SyncControlledEntities.Num() == 0)
	{
		SyncControlledEntities = ControlledEntities;
	}

	for(int32 i = 0; i < MaxSyncEntitiesNum; i++)
	{
		if (SyncControlledEntities.Num() == 0) break;
		int32 EID = SyncControlledEntities.Pop();
		UEntity* Entity = GetEntity(EID);
		if (!Entity->IsValidLowLevel()) continue;
		if (!Entity->NeedSyncEntityInfo())continue;

		const FSyncEntityInfo *SyncEntityInfo = Entity->GetSyncEntityInfo();
		const FVector &Position = SyncEntityInfo->Position;
		const FVector &Direction = Unreal2KBEngineDirection(SyncEntityInfo->Direction);
		float Percentage = SyncEntityInfo->Percentage;
		int EntityId = Entity->GetID();

		FVector KBEPos = FVector(Position[0] * 0.01, Position[2] * 0.01, Position[1] * 0.01);
		FBundle* CEBbundle = new FBundle();
		CEBbundle->NewMessage((*CEMessagePtr)->MessageID, (*CEMessagePtr)->ArgNum);
		CEBbundle->WriteInt32(EntityId);
		CEBbundle->WriteFloat(KBEPos.X);
		CEBbundle->WriteFloat(KBEPos.Y);
		CEBbundle->WriteFloat(KBEPos.Z);
		CEBbundle->WriteFloat(Percentage);
		CEBbundle->WriteFloat(Direction.Y);
		CEBbundle->WriteFloat(Direction.Z);
		CEBbundle->WriteUint8((uint8)(Entity->IsOnGround()));
		CEBbundle->WriteUint32(SpaceID);
		CEBbundle->Send(Socket);
		delete CEBbundle;
	}
}
int32 UKBENetConnect::GetAoiEntityIDFromStream(FMemoryStream *Stream)
{
	int32 Id = 0;
	if (EntityIDAliasIDList.Num() > 255)
	{
		Id = Stream->ReadInt32();
	}
	else
	{
		uint8 AliasID = Stream->ReadUint8();

		Id = EntityIDAliasIDList[AliasID];
	}

	return Id;
}

bool UKBENetConnect::BindEvent(uint16 Id, FString Name, int16 ArgNumb)
{
	for (auto& Msg : Messages)
	{
		if (Msg->Name == Name)
		{
			Msg->MessageID = Id;
			Msg->ArgNum = ArgNumb;
			ClientMessage.Add(Id, Msg);
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Not find %s[%d] in Process Event!"), *Name, Id);

	return false;

}
void UKBENetConnect::OnHelloCB(FMemoryStream *Stream)
{
	FString ServerVersion = Stream->ReadString();
	FString ServerScriptVersion = Stream->ReadString();
	FString ServerProtocolMD5 = Stream->ReadString();
	FString ServerEntitydefMD5 = Stream->ReadString();
	int32 Type = Stream->ReadInt32();

	FMessage **MessagePtr = nullptr;
	if (ConnectStatus == 0)
	{
		MessagePtr = LoginAppMessage.Find(TEXT("Loginapp_importClientMessages"));
	}
	else if (ConnectStatus == 1)
	{
		MessagePtr = BaseAppMessage.Find(TEXT("Baseapp_importClientMessages"));
	}

	if (!MessagePtr)return;

	FBundle* BundlePtr = new FBundle();
	BundlePtr->NewMessage((*MessagePtr)->MessageID, (*MessagePtr)->ArgNum);
	BundlePtr->Send(Socket);
	delete BundlePtr;
}
FMessage *UKBENetConnect::GetHandleMessage(uint16 Idx)
{
	FMessage **MessagePtr = ClientMessage.Find(Idx);
	check(MessagePtr);
	if (!MessagePtr)
	{
		return nullptr;
	}
	return  *MessagePtr;
}
void UKBENetConnect::UpdateEntityMovement(int32 InId, const FVector &InPos, int GroundType )
{
	UEntity *EntityPtr = GetEntity(InId);
	if (EntityPtr)
	{
		if (EntityPtr->IsValidLowLevel())
		{
			FVector Pos = InPos;
			if (InPos == FVector::ZeroVector)
			{
				Pos = EntityPtr->GetOwner()->GetActorLocation();
			}
			if (GroundType == 0)
			{
				EntityPtr->SetGround(false);
			}
			else if (GroundType == 1)
			{
				EntityPtr->SetGround(true);
				Pos.Z = EntityPtr->GetOwner()->GetActorLocation().Z;
			}
			EntityPtr->OnUpdateEntityMovement(Pos);
		}
	}
}
void UKBENetConnect::UpdateEntityDirection(int32 InId, const FVector &InDirection)
{
	UEntity** EntityPtr = Entities.Find(InId);

	if (!EntityPtr)return;

	UEntity* Entity = *EntityPtr;

	if (Entity->IsValidLowLevel())
	{
		Entity->OnUpdateEntityDirection(KBEngine2UnrealDirection(InDirection));
	}
}
void UKBENetConnect::UpdateEntitySpeed(int32 InId, float Percentage)
{
	UEntity** EntityPtr = Entities.Find(InId);
	if (!EntityPtr)return;
	UEntity* Entity = *EntityPtr;
	Entity->SetSpeedPercentage(Percentage);
}
void UKBENetConnect::OnImportClientMessages(FMemoryStream *Stream)
{
	uint16 MsgCount = Stream->ReadUint16();
	while (MsgCount > 0)
	{
		MsgCount--;
		uint16 MsgId = Stream->ReadUint16();
		int16 MsgLen = Stream->ReadInt16();
		FString MsgName = Stream->ReadString();
		int8 ArgsType = Stream->ReadInt8();
		uint8 ArgSize = Stream->ReadUint8();
		TArray<uint8> ArgsTypes;

		for (uint8 i = 0; i < ArgSize; i++)
		{
			ArgsTypes.Add(Stream->ReadUint8());
		}

		bool IsClientMethod = MsgName.Contains("Client_");

		if (MsgName.Len() > 0)
		{
			if (IsClientMethod)
			{
				if (!BindEvent(MsgId, MsgName, MsgLen))
				{
					FMessage *Msg = new FMessage(MsgName);
					Messages.Add(Msg);
					BindEvent(MsgId, MsgName, MsgLen);
				}
			}
			else
			{
				if (ConnectStatus == 0)
				{
					LoginAppMessage.Add(MsgName, new FMessage(MsgId, MsgName, MsgLen));
				}
				else
				{
					BaseAppMessage.Add(MsgName, new FMessage(MsgId, MsgName, MsgLen));
				}
			}
		}
	}
	ImportClientMessagesCompleted();
}
void UKBENetConnect::ImportClientMessagesCompleted()
{
	FMessage **MessagePtr = nullptr;
	if (ConnectStatus == 0)
	{
		MessagePtr = LoginAppMessage.Find(TEXT("Loginapp_login"));
	}
	else if (ConnectStatus == 1)
	{
		MessagePtr = BaseAppMessage.Find(TEXT("Baseapp_loginBaseapp"));
	}

	if (!MessagePtr)return;

	FBundle* BundlePtr = new FBundle();
	BundlePtr->NewMessage((*MessagePtr)->MessageID, (*MessagePtr)->ArgNum);
	if (ConnectStatus == 0)
	{
		BundlePtr->WriteInt8((int8)CLIENT_TYPE_WIN);
		TArray<uint8> ClientDatas;
		BundlePtr->WriteBlob(ClientDatas);
	}

	BundlePtr->WriteString(UserName);
	BundlePtr->WriteString(Password);

	BundlePtr->Send(Socket);
	delete BundlePtr;
}
void UKBENetConnect::OnLoginFailed(FMemoryStream *Stream)
{
	FString Error = Stream->ReadString();
	if (Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.IsBound())
	{
		Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.Execute(UserName, 0, Error);
	}
	if (ConnectStatus == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login error  .....[%s][%s]..."), *UserName, *Error);
	}		
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Base error .....[%s][%s]..."), *UserName, *Error);
	}
		
}
/**
 * 服务器通知登陆成功
 */
void UKBENetConnect::OnLoginSuccessfully(FMemoryStream *Stream)
{
	UserName = Stream->ReadString();
	BaseappIP = Stream->ReadString();
	BaseappPort = Stream->ReadUint16();
	Stream->ReadBlob(ServerDatas);

	if (Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.IsBound())
	{
		Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.Execute(UserName, 1, TEXT(""));
	}
}

bool UKBENetConnect::OnEnterLobby()
{
	bool Ret = ConnectToBaseApp(BaseappIP, BaseappPort, UserName, Password);
	if (!Ret)
	{
		Ret = ConnectToBaseApp(LoginIP, BaseappPort, UserName, Password);
	}

	if (!Ret)
	{
		if (Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.IsBound())
		{
			Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.Execute(UserName, 3, TEXT(""));
		}
	}
	return Ret;
}
bool UKBENetConnect::ConnectToBaseApp(FString Ip, uint16 Port, const FString &Name, const FString &InPassword)
{
	InitSocket();
	auto Addr = SocketSubsystem->CreateInternetAddr(0, Port);
	bool bIsValid;

	Addr->SetIp(*Ip, bIsValid);
	if (Socket->Connect(*Addr) == false)
	{
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
		SocketSubsystem = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Error can not connect base .[%s][%d]..."), *Ip, Port);
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("Connect base ok .[%s][%d]..."), *Ip, Port);
	ConnectStatus = 1;

	FMessage **MessagePtr = nullptr;
	MessagePtr = BaseAppMessage.Find(TEXT("Baseapp_hello"));
	if (!MessagePtr)return false;

	FBundle* BundlePtr = new FBundle();
	BundlePtr->NewMessage((*MessagePtr)->MessageID, (*MessagePtr)->ArgNum);
	BundlePtr->WriteString(ClientVersion);
	BundlePtr->WriteString(ClientScriptVersion);
	BundlePtr->WriteBlob(EncryptedKey);
	BundlePtr->Send(Socket);
	delete BundlePtr;
	return true;
}
void UKBENetConnect::OnAppActiveTickCB(FMemoryStream *Stream)
{

}
void UKBENetConnect::OnUpdatePropertys(FMemoryStream *Stream)
{
	int32 Eid = Stream->ReadInt32();
	UE_LOG(LogTemp, Warning, TEXT("OnUpdatePropertys..[%d]..."), Eid);
	UEntity* EntityPtr = GetEntity(Eid);
	if (!EntityPtr)
	{
		FMemoryStream* EntityMessage = NULL;
		if (TryGetValue(BufferedCreateEntityMessage, Eid, EntityMessage))
		{
			return;
		}

		FMemoryStream* Stream1 = new FMemoryStream(Stream->GetData(), Stream->GetNumBits());
		BufferedCreateEntityMessage.Add(Eid, Stream1);
		return;
	}
	EntityPtr->OnUpdatePropertys(*Stream);
}

/**
 * 创建玩家对象entity 
 */
void UKBENetConnect::OnCreatedProxies(FMemoryStream *Stream)
{
	uint64 rndUUID = Stream->ReadUint64();
	int32 Eid = Stream->ReadInt32();
	UE_LOG(LogTemp, Warning, TEXT("OnCreatedProxies..[%d]..."), Eid);
	AActor *Actor = nullptr;
	FString EntityType = Stream->ReadString();
	ProxyEntityID = Eid;

	Actor = UEntity::CreateEntityObject(EntityType);

	TInlineComponentArray<UEntity*> ComponentArray;
	Actor->GetComponents<UEntity>(ComponentArray);

	UEntity *Entity = ComponentArray[0];
	Entity->SetID(Eid);
	Entity->SetClassName(EntityType);
	Entity->SetBaseMailbox(new FMailbox(Eid, Socket, BaseAppMessage["Entity_onRemoteMethodCall"]));
	Entity->SetKBENetConnect(this);
	AddEntity(Eid, Entity);


	FMemoryStream* EntityMessage = NULL;
	TryGetValue(BufferedCreateEntityMessage, Eid, EntityMessage);
	if (EntityMessage != NULL)
	{
		OnUpdatePropertys(EntityMessage);
		BufferedCreateEntityMessage.Remove(Eid);
	}

	if (EntityType == TEXT("Account"))
	{
		//登陆完成
		if (Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.IsBound())
		{
			Cast<UKBEngineNetDriver>(KBENetDriver)->KBELoginDelegate.Execute(UserName, 2, "");
		}
	}
}
void UKBENetConnect::OnEntityDestroyed(FMemoryStream *Stream)
{
	int32 Eid = Stream->ReadInt32();
	UE_LOG(LogTemp, Warning, TEXT("OnEntityDestroyed..[%d]..."), Eid);
	UEntity *EntityPtr = GetEntity(Eid);
	if (EntityPtr)
	{
		RemoveEntity(Eid);
	}
}
void UKBENetConnect::OnEntityEnterWorld(FMemoryStream *Stream)
{
	int32 Eid = Stream->ReadInt32();
	UE_LOG(LogTemp, Warning, TEXT("OnEntityEnterWorld..[%d]..."), Eid);
	//当定义的entity超过255的时候这里需要改成ReadInt16
	uint16 EntityType = Stream->ReadInt8();

	if (ProxyEntityID > 0 && ProxyEntityID != Eid)
	{
		EntityIDAliasIDList.Add(Eid);
	}

	if (Stream->Length() > 0)
	{
		int8 IsOnGround = Stream->ReadUint8();
	}

	UEntity* Entity = nullptr;

	FString *SType = Cast<UKBEngineNetDriver>(KBENetDriver)->EntityClassIndex.Find(EntityType);
	if (!SType)
	{
		return;
	}

	FString UEntityType = *SType ;

	if (!TryGetValue(Entities, Eid, Entity))
	{
		//重新创建
		AActor *Actor = UEntity::CreateEntityObject(UEntityType);
		TInlineComponentArray<UEntity*> ComponentArray;
		Actor->GetComponents<UEntity>(ComponentArray);

		Entity = ComponentArray[0];
		if (Entity != nullptr)
		{
			Entity->SetKBENetConnect(this);
			Entity->SetID(Eid);
			Entity->SetClassName(UEntityType);
			AddEntity(Eid, Entity);
		}
	}

	FMemoryStream* EntityMessage = NULL;
	TryGetValue(BufferedCreateEntityMessage, Eid, EntityMessage);
	if (EntityMessage != NULL)
	{
		OnUpdatePropertys(EntityMessage);
		BufferedCreateEntityMessage.Remove(Eid);
	}

	auto *MailBox = new FMailbox(Eid, Socket, BaseAppMessage["Baseapp_onRemoteCallCellMethodFromClient"]);
	Entity->SetCellMailbox(MailBox);
	Entity->SetInWorld(true);

	if (ProxyEntityID == Entity->EntityID) 
	{
		Entity->SetByControll(true);

		AckPlayerPosition = Entity->KBESyncLocation;
	}
	else if (ControlledEntities.Contains(Entity->EntityID))
	{
		Entity->SetByControll(true);
	}
	else
	{
		Entity->SetByControll(false);
	}

	if (Cast<UKBEngineNetDriver>(KBENetDriver)->KBEEntityEnterWorldDelegate.IsBound())
	{
		Cast<UKBEngineNetDriver>(KBENetDriver)->KBEEntityEnterWorldDelegate.Execute(Eid);
	}

	Entity->InitPropertiesFinish();
}
void UKBENetConnect::OnInitSpaceData(FMemoryStream *Stream)
{
	SpaceID = Stream->ReadUint32();
	while (Stream->Length() > 0)
	{
		FString key = Stream->ReadString();
		FString val = Stream->ReadString();
	}
}
void UKBENetConnect::OnSetEntityPosAndDir(FMemoryStream *Stream)
{
	int32 Eid = Stream->ReadUint32();
	UEntity *EntityPtr = GetEntity(Eid);
	if (EntityPtr)
	{
		FVector Position(0.0);
		FVector Direction(0.0);

		Position.X = Stream->ReadFloat();
		Position.Y = Stream->ReadFloat();
		Position.Z = Stream->ReadFloat();

		EntityPtr->SetPosition(KBEngine2UnrealPosition(Position));

		Direction.X = Stream->ReadFloat();
		Direction.Y = Stream->ReadFloat();
		Direction.Z = Stream->ReadFloat();
		EntityPtr->SetDirection(KBEngine2UnrealDirection(Direction));
	}
}
//玩家进入世界
void UKBENetConnect::OnEntityEnterSpace(FMemoryStream *Stream)
{
	int32 Eid = Stream->ReadUint32();
	UEntity *EntityPtr = GetEntity(Eid);
	UE_LOG(LogTemp, Warning, TEXT("OnEntityEnterSpace..[%d]..."), Eid);
	if (!EntityPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not found player."));
		return;
	}
	SpaceID = Stream->ReadUint32();
	if (Stream->Length() > 0)
	{
		int8 IsOnGround = Stream->ReadInt8();
	}
		
	if (Cast<UKBEngineNetDriver>(KBENetDriver)->KBEPlayerEnterWorldDelegate.IsBound())
	{
		Cast<UKBEngineNetDriver>(KBENetDriver)->KBEPlayerEnterWorldDelegate.Execute(Eid);
	}
}
//更新玩家坐标
void UKBENetConnect::OnUpdateBasePosXZ(FMemoryStream *Stream)
{
	float X = Stream->ReadFloat();
	float Y = Stream->ReadFloat();
	UpdateEntityMovement(ProxyEntityID, AckPlayerPosition + KBEngine2UnrealPosition(FVector(X, Y, 0)), 1);
}
//不在地面的位置更新
void UKBENetConnect::OnUpdateData_xyz(FMemoryStream *Stream)
{
	int32 Eid = GetAoiEntityIDFromStream(Stream);
	FVector2D XZ = Stream->ReadPackXZ();
	float Y = Stream->ReadPackY();
	UpdateEntityMovement(Eid, AckPlayerPosition + KBEngine2UnrealPosition(FVector(XZ[0], Y, XZ[1])), 0);
}
void UKBENetConnect::OnEntityLeaveWorld(FMemoryStream *Stream)
{
	int32 Eid = GetAoiEntityIDFromStream(Stream);

	UE_LOG(LogTemp, Warning, TEXT("OnEntityLeaveWorld..[%d]..."), Eid);
	EntityIDAliasIDList.Remove(Eid);

	if (ProxyEntityID != Eid)
	{
		UEntity *EntityPtr = GetEntity(Eid);
		if (EntityPtr)
		{
			if (EntityPtr->GetByControll())
			{
				ControlledEntities.Remove(Eid);
			}
		}
		RemoveEntity(Eid);
	}
}
/**
 * 响应控制实体消息
 */
void UKBENetConnect::OnControlEntity(FMemoryStream *Stream)
{
	int32 Eid = Stream->ReadInt32();
	uint8 IsControlled = Stream->ReadInt8();
	UEntity *EntityPtr = GetEntity(Eid);
	bool IsCont = IsControlled != 0;
	if (IsCont)
	{
		//实体被当前客户端控制
		ControlledEntities.Add(Eid);
	}
	else
	{
		//实体失去控制
		ControlledEntities.Remove(Eid);
	}
	if (EntityPtr)
	{
		EntityPtr->SetByControll(IsCont);
	}

}
void UKBENetConnect::OnRemoteMethodCallOptimized(FMemoryStream *Stream)
{
	int32 Eid = GetAoiEntityIDFromStream(Stream);
	UEntity*EntityPtr = GetEntity(Eid);
	if (EntityPtr)
	{
		EntityPtr->OnRemoteMethodCall(*Stream);
	}
}
void UKBENetConnect::OnRemoteMethodCall(FMemoryStream *Stream)
{
	int32 Eid = Stream->ReadInt32();
	UEntity *EntityPtr = GetEntity(Eid);
	if (EntityPtr)
	{
		EntityPtr->OnRemoteMethodCall(*Stream);
	}
}
AActor *UKBENetConnect::Player()
{
	UEntity *Entity = GetEntity(ProxyEntityID);
	if (Entity) return Entity->GetOwner();
	return nullptr;
}
UEntity* UKBENetConnect::GetPlayerEntity()
{
	return GetEntity(ProxyEntityID);
}
UEntity *UKBENetConnect::GetEntity(int Id)
{
	UEntity** EntityPtr = Entities.Find(Id);
	if (!EntityPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetEntity not found [%d]..."), Id);
		return nullptr;
	}
	UEntity *Entity = *EntityPtr;
	if (!Entity->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetEntity entity not valid [%d]..."), Id);
		return nullptr;
	}
	return  Entity;
}

void UKBENetConnect::OnUpdatePropertysOptimized(FMemoryStream *Stream)
{
	int32 Eid = GetAoiEntityIDFromStream(Stream);
	UEntity* EntityPtr = GetEntity(Eid);
	if (EntityPtr)
	{
		EntityPtr->OnUpdatePropertys(*Stream);
	}
}
bool UKBENetConnect::AddEntity(int32 Idx, UEntity* Entity)
{
	Entities.Add(Idx, Entity);
	return true;
}

bool UKBENetConnect::RemoveEntity(int32 Idx)
{	
	if (Entities.Contains(Idx))
	{
		UEntity *EntityPtr = GetEntity(Idx);
		Entities.Remove(Idx);
		if (EntityPtr)
		{
			EntityPtr->OnDestroy();
		}
	}
	return true;
}
void UKBENetConnect::OnUpdateData_xz_ypr(FMemoryStream *Stream)
{
	int32 Eid = GetAoiEntityIDFromStream(Stream);
	FVector2D XZ = Stream->ReadPackXZ();

	float Y = INT8_TO_ANGLE(Stream->ReadInt8());
	float P = INT8_TO_ANGLE(Stream->ReadInt8());
	float R = INT8_TO_ANGLE(Stream->ReadInt8());

	UpdateEntitySpeed(Eid, R);
	UpdateEntityDirection(Eid, FVector(0, 0, Y));
	UpdateEntityMovement(Eid, AckPlayerPosition + KBEngine2UnrealPosition(FVector(XZ[0], 0.0f, XZ[1])), 1);
}
void UKBENetConnect::OnUpdateData_xz(FMemoryStream *Stream)
{
	int32 Eid = GetAoiEntityIDFromStream(Stream);
	FVector2D XZ = Stream->ReadPackXZ();
	UpdateEntityMovement(Eid, AckPlayerPosition + KBEngine2UnrealPosition(FVector(XZ[0], 0.0f, XZ[1])), 1);
}
//方向更新
void UKBENetConnect::OnUpdateData_ypr(FMemoryStream *Stream)
{
	int32 Eid = GetAoiEntityIDFromStream(Stream);

	float Y = INT8_TO_ANGLE(Stream->ReadInt8());
	float P = INT8_TO_ANGLE(Stream->ReadInt8());
	float R = INT8_TO_ANGLE(Stream->ReadInt8());

	UpdateEntityDirection(Eid, FVector(0, 0, Y));
	UpdateEntitySpeed(Eid, R);
	UpdateEntityMovement(Eid);
}
void UKBENetConnect::OnUpdateData_xyz_ypr(FMemoryStream *Stream)
{
	int32 Eid = GetAoiEntityIDFromStream(Stream);
	FVector2D XZ = Stream->ReadPackXZ();
	float Y = Stream->ReadPackY();

	float Yaw = INT8_TO_ANGLE(Stream->ReadInt8());
	float P = INT8_TO_ANGLE(Stream->ReadInt8());
	float R = INT8_TO_ANGLE(Stream->ReadInt8());

	UpdateEntityDirection(Eid, FVector(0, 0, Yaw));
	UpdateEntitySpeed(Eid, R);
	UpdateEntityMovement(Eid, AckPlayerPosition + KBEngine2UnrealPosition(FVector(XZ[0], Y, XZ[1])), 0);
}
int32 UKBENetConnect::GetProxyEntityID()
{
	return ProxyEntityID;
}
void UKBENetConnect::OnEntityLeaveSpace(FMemoryStream *Stream)
{
	int32 Eid = Stream->ReadInt32();

	UE_LOG(LogTemp, Warning, TEXT("OnEntityLeaveSpace [%d]..."), Eid);

	RemoveEntityExceptPlayer();

}
void UKBENetConnect::OnUpdateBasePos(FMemoryStream *Stream)
{
	float X = Stream->ReadFloat();
	float Y = Stream->ReadFloat();
	float Z = Stream->ReadFloat();

	UE_LOG(LogTemp, Warning, TEXT("OnUpdateBasePos [%f,%f,%f]..."), X, Y, Z);
}
void UKBENetConnect::RemoveEntityExceptPlayer()
{
	UEntity *Entity = GetPlayerEntity();
	for (TPair<int32, UEntity*>& Element : Entities)
	{
		if (Element.Value->IsValidLowLevel())
		{
			if (Element.Value->GetID() != ProxyEntityID)
			{
				Element.Value->OnDestroy();
			}
		}
	}
	Entities.Empty();
	Entities.Add(ProxyEntityID, Entity);
}
#pragma optimize("", on)