#pragma once
#include "MemoryStream.h"
#include "Bundle.h"
#include "Networking.h"
#include "Net/DataReplication.h"
#include "Net/UnrealNetwork.h"
#include "UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Net/RepLayout.h"
class KBE_API FMessage
{
public:
	FMessage();
	FMessage(const FString &InName);
	FMessage(uint16 Id, const FString &InName, int16 Num);

	uint16 MessageID = -1;
	FString Name;
	int16 ArgNum = -1;

	DECLARE_DELEGATE_OneParam(FOnNetMessageDelegate, FMemoryStream *);
	FOnNetMessageDelegate NetMessageDelegate;

	void Process(FMemoryStream *Stream);

};