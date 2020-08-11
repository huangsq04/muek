#include "Message.h"
FMessage::FMessage()
{
};
FMessage::FMessage(const FString &InName) :
	MessageID(0), Name(InName), ArgNum(-1)
{
};
FMessage::FMessage(uint16 Id, const FString &InName, int16 Num) :
	MessageID(Id), Name(InName), ArgNum(Num)
{ 
};
void FMessage::Process(FMemoryStream *Stream)
{
	if (NetMessageDelegate.IsBound())
	{
		NetMessageDelegate.Execute(Stream);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FMessage DefaultProcess..[%s][%d]..."), *Name, MessageID);
	}
}