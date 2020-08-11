#include "Mailbox.h"
FMailbox::FMailbox()
{
}

FMailbox::FMailbox(int32 EntityID, FSocket *Net, FMessage *Message) :
	Id(EntityID)
{
	Socket = Net;
	MessageID = Message->MessageID;
	MessageArg = Message->ArgNum;
}

FBundle *FMailbox::NewMail()
{
	if (BundlePtr == NULL)
		BundlePtr = new FBundle();

	if (Socket != nullptr)
	{
		BundlePtr->NewMessage(MessageID, MessageArg);
	}

	BundlePtr->WriteInt32(Id);

	return BundlePtr;
}

void FMailbox::PostMail(FBundle *InBundle)
{
	if (InBundle == nullptr)
		InBundle = BundlePtr;

	if (Socket != nullptr)
		InBundle->Send(Socket);

	if (InBundle == BundlePtr)
		BundlePtr = nullptr;
}