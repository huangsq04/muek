#pragma once

#include "Core.h"
#include <vector>
#include <string>

class KBE_API FMemoryStream : public FBitReader
{
public:
	const static size_t BUFFER_MAX = 1460 * 4;

	union PackFloatXType
	{
		float FV;
		uint32 UV;
		int32 IV;
	};

	FMemoryStream() {	}
	FMemoryStream(uint8* Src, int64 CountBits)
		:FBitReader(Src, CountBits)
	{
	}
	virtual ~FMemoryStream() {}
	template <typename T> T Read()
	{
		T Value;
		SerializeBits(&Value, sizeof(T) * 8);
		return Value;
	}

	int8 ReadInt8() { return Read<int8>(); }
	int16 ReadInt16() { return Read<int16>(); }
	int32 ReadInt32() { return Read<int32>(); }
	int64 ReadInt64() { return Read<int64>(); }
	uint8 ReadUint8() { return Read<uint8>(); }
	uint16 ReadUint16() { return Read<uint16>(); }
	uint32 ReadUint32() { return Read<uint32>(); }
	uint64 ReadUint64() { return Read<uint64>(); }
	float ReadFloat() { return Read<float>(); }
	double ReadDouble() { return Read<double>(); }
	TArray<uint8> ReadBlob()
	{
		TArray<uint8> Bytes;
		ReadBlob(Bytes);
		return Bytes;
	}

	FString ReadString()
	{
		FBitWriter String(0, true);
		uint8 Char;
		while (1)
		{
			Char = ReadUint8();
			if (Char == 0)
				break;
			String.SerializeBits(&Char, sizeof(uint8) * 8);
		}
		String.SerializeBits(&Char, sizeof(uint8) * 8);
		//KBE在传参数的字符串会添加\"b\"
		if (String.GetData()[0] == 34 && String.GetData()[1] == 98 && String.GetData()[2] == 39)
		{
			FString RetString = FString(String.GetNumBytes() - 6, (char *)String.GetData() + 3);
			return RetString;
		}

		FString RetString = FString((char *)String.GetData(), String.GetNumBytes() - 1);
		return RetString;
	}

	FString ReadUTF8()
	{
		uint32 ReadSize = ReadUint32();
		if (ReadSize > 0)
		{
			FBitReader Datas;
			Datas.SetData(*this, ReadSize * 8);
			FUTF8ToTCHAR ToString((const ANSICHAR *)(Datas.GetData()), ReadSize);
			return FString(ToString.Get(), ToString.Length());
		}
		return FString();
	}
	uint32 ReadBlob(std::string &OutDatas)
	{
		uint32 ReadSize = ReadUint32();
		if (ReadSize > 0)
		{
			FBitReader Datas;
			Datas.SetData(*this, ReadSize * 8);
			OutDatas.assign((char*)(Datas.GetData()), ReadSize);
		}
		return ReadSize;
	}
	uint32 ReadBlob(TArray<uint8> &Bytes)
	{
		uint32 ReadSize = ReadUint32();

		if (ReadSize > 0)
		{
			Bytes.SetNumUninitialized(ReadSize);
			SerializeBits((char *)Bytes.GetData(), ReadSize * 8);
		}
		return ReadSize;
	}
	void ReadPackXYZ(float &X, float &Y, float &Z, float Minf = -256.f)
	{
		uint32 Packed = ReadUint32();
		X = ((Packed & 0x7FF) << 21 >> 21) * 0.25f;
		Z = ((((Packed >> 11) & 0x7FF) << 21) >> 21) * 0.25f;
		Y = ((Packed >> 22 << 22) >> 22) * 0.25f;

		X += Minf;
		Y += Minf / 2.f;
		Z += Minf;
	}
	void ReadPackXZ(float& X, float& Z)
	{
		PackFloatXType & PackDataX = (PackFloatXType&)X;
		PackFloatXType & PackDataZ = (PackFloatXType&)Z;
		PackDataX.UV = 0x40000000;
		PackDataZ.UV = 0x40000000;
		uint8 Tv;
		uint32 Data = 0;

		Tv = ReadUint8();
		Data |= (Tv << 16);

		Tv = ReadUint8();
		Data |= (Tv << 8);

		Tv = ReadUint8();
		Data |= Tv;

		// 复制指数和尾数
		PackDataX.UV |= (Data & 0x7ff000) << 3;
		PackDataZ.UV |= (Data & 0x0007ff) << 15;

		PackDataX.FV -= 2.0f;
		PackDataZ.FV -= 2.0f;

		// 设置标记位
		PackDataX.UV |= (Data & 0x800000) << 8;
		PackDataZ.UV |= (Data & 0x000800) << 20;
	}
	void ReadPackY(float& Y)
	{
		PackFloatXType PackDataY;
		PackDataY.UV = 0x40000000;

		uint16 Data = ReadUint16();
		PackDataY.UV |= (Data & 0x7fff) << 12;
		PackDataY.FV -= 2.f;
		PackDataY.UV |= (Data & 0x8000) << 16;
		Y = PackDataY.FV;
	}
	FVector2D ReadPackXZ()
	{
		FVector2D Vector;
		ReadPackXZ(Vector.X, Vector.Y);
		return Vector;
	}
	FVector ReadFVector()
	{
		FVector Vector;
		Vector.X = ReadFloat();
		Vector.Y = ReadFloat();
		Vector.Z = ReadFloat();
		return Vector;
	}
	float ReadPackY()
	{
		float Value;
		ReadPackY(Value);
		return Value;
	}

	virtual size_t Length() const
	{
		return Num - Pos;
	}
};

