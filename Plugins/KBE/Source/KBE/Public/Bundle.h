#pragma once
#include "Core.h"
#include "MemoryStream.h"
#include "Networking.h"

class  KBE_API FBundle : public FBitWriter
{
public:
	FBundle();
	~FBundle();

	void NewMessage(uint16 Idx, int16 ArgNumb = -1);

	void FinishMessageBody();

	void Send(FSocket *Socket);

	void WriteInt8(int8 Value)
	{
		SerializeBits(&Value, sizeof(int8) * 8);
	}
	void WriteInt16(int16 Value)
	{
		SerializeBits(&Value, sizeof(int16) * 8);
	}
	void WriteInt32(int32 Value)
	{
		SerializeBits(&Value, sizeof(int32) * 8);
	}
	void WriteInt64(int64 Value)
	{
		SerializeBits(&Value, sizeof(int64) * 8);
	}
	void WriteUint8(uint8 Value)
	{
		SerializeBits(&Value, sizeof(uint8) * 8);
	}
	void WriteUint16(uint16 Value)
	{
		SerializeBits(&Value, sizeof(uint16) * 8);
	}
	void WriteUint32(uint32 Value)
	{
		SerializeBits(&Value, sizeof(uint32) * 8);
	}
	void WriteUint64(uint64 Value)
	{
		SerializeBits(&Value, sizeof(uint64) * 8);
	}
	void WriteFloat(float Value)
	{
		SerializeBits(&Value, sizeof(float) * 8);
	}
	void WriteDouble(double Value)
	{
		SerializeBits(&Value, sizeof(double) * 8);
	}
	void WriteFVector(FVector Value)
	{
		WriteFloat(Value.X);
		WriteFloat(Value.Y);
		WriteFloat(Value.Z);
	}
	void WriteStdString(const std::string &Value)
	{
		SerializeBits((void *)Value.data(), Value.length() * 8);
		int8 t = 0;
		SerializeBits(&t, sizeof(int8) * 8);
	}

	void WriteString(const FString &Value)
	{
		auto String = StringCast<ANSICHAR>(*Value);
		SerializeBits((void *)String.Get(), String.Length() * 8);
		int8 t = 0;
		SerializeBits(&t, sizeof(int8) * 8);
	}

	void WriteUTF8(const FString &Value)
	{
		FTCHARToUTF8 utf8(*Value);

		//                 |- 长度  -| |- 数据流 ->
		// BLOB数据流格式：00 00 00 00 xx xx xx xx xx ...
		// 所以需要加4个字节长度
		SerializeBits((void *)utf8.Get(), utf8.Length() * 8);
	}

	void WriteBlob(const char *Buf, uint32 Len)
	{
		SerializeBits((void *)Buf, Len * 8);
	}

	void WriteBlob(const TArray<uint8> &Bytes)
	{
		uint32 Len = (uint32)Bytes.Num();
		SerializeBits((void *)&Len, sizeof(uint32) * 8);
		SerializeBits((void *)Bytes.GetData(), Len * 8);
	}

	int DataLength() {	return this->GetNumBytes() - MessageHeadLength;	}

protected:

	int MessageHeadLength = 0;
};



