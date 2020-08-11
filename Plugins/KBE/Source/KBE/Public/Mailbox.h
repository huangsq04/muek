#pragma once
#include "Bundle.h"
#include "Message.h"
class KBE_API FMailbox
{
public:

	int32 Id = 0;

	FBundle *BundlePtr = nullptr;

	FMailbox();

	FMailbox(int32 EntityID, FSocket *Net, FMessage *Message);

	FBundle *NewMail();
	void PostMail(FBundle *InBundle);

private:
	uint16 MessageID = 0;
	int16  MessageArg = -1;

	FSocket *Socket = nullptr;
};