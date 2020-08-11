#pragma optimize("", off)
#include "Bundle.h"
FBundle::FBundle():
	FBitWriter(0, true)
{
}
FBundle::~FBundle()
{
}
void FBundle::NewMessage(uint16 Idx, int16 ArgNumb)
{
	WriteUint16(Idx);
	if (ArgNumb == -1)
		WriteUint16(0);
	MessageHeadLength = GetNumBytes();
}
void FBundle::FinishMessageBody()
{
	int MessageLength = GetNumBytes() - MessageHeadLength;
	GetData()[2] = (uint8)(MessageLength & 0xff);
	GetData()[3] = (uint8)(MessageLength >> 8 & 0xff);
}
void FBundle::Send(FSocket *Socket)
{
	FinishMessageBody();
	if (Socket == nullptr)
		return;
	int32 BytesSent = 0;
	Socket->Send(GetData(), GetNumBytes(), BytesSent);
}
#pragma optimize("", on)