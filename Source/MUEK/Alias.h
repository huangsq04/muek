#pragma once
#include "Entity.h"
#include "Alias.generated.h"
USTRUCT(BlueprintType)
struct FCREATEMONSTERARG{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ClassName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PlayerName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Position;
public: 
    void CreateFromStream(FMemoryStream* Stream){
        ClassName= Stream->ReadString();
        PlayerName= Stream->ReadString();
        Position= Stream->ReadFVector();
    }

    void AddToStream(FBundle *Stream) const{
        Stream->WriteString(ClassName);
        Stream->WriteString(PlayerName);
        Stream->WriteFVector(Position);
    }
 
}; 
USTRUCT(BlueprintType)
struct FCREATEMONSTERARGS{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FCREATEMONSTERARG> Items;
public: 
    void CreateFromStream(FMemoryStream* Stream){
        Items.Empty();
        int ArrayItemsNum = Stream->ReadUint32();
        while (ArrayItemsNum > 0){
            ArrayItemsNum--;
            FCREATEMONSTERARG Value;
            Value.CreateFromStream(Stream);
            Items.Add(Value);
        }
    }

    void AddToStream(FBundle *Stream) const{
        Stream->WriteUint32(Items.Num());
        for (int32 i = 0; i < Items.Num(); i++) {
            Items[i].AddToStream(Stream);
        }
    }
 
}; 
USTRUCT(BlueprintType)
struct FITEMINFO{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Amount;
public: 
    void CreateFromStream(FMemoryStream* Stream){
        Id= Stream->ReadInt32();
        Amount= Stream->ReadInt32();
    }

    void AddToStream(FBundle *Stream) const{
        Stream->WriteInt32(Id);
        Stream->WriteInt32(Amount);
    }
 
}; 
USTRUCT(BlueprintType)
struct FROLEITEM{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Map;
public: 
    void CreateFromStream(FMemoryStream* Stream){
        Id= Stream->ReadInt32();
        Name= Stream->ReadString();
        Map= Stream->ReadString();
    }

    void AddToStream(FBundle *Stream) const{
        Stream->WriteInt32(Id);
        Stream->WriteString(Name);
        Stream->WriteString(Map);
    }
 
}; 
USTRUCT(BlueprintType)
struct FROLEINFO{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FROLEITEM> Items;
public: 
    void CreateFromStream(FMemoryStream* Stream){
        Items.Empty();
        int ArrayItemsNum = Stream->ReadUint32();
        while (ArrayItemsNum > 0){
            ArrayItemsNum--;
            FROLEITEM Value;
            Value.CreateFromStream(Stream);
            Items.Add(Value);
        }
    }

    void AddToStream(FBundle *Stream) const{
        Stream->WriteUint32(Items.Num());
        for (int32 i = 0; i < Items.Num(); i++) {
            Items[i].AddToStream(Stream);
        }
    }
 
}; 
USTRUCT(BlueprintType)
struct FROLEITEMS{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FITEMINFO> Items;
public: 
    void CreateFromStream(FMemoryStream* Stream){
        Items.Empty();
        int ArrayItemsNum = Stream->ReadUint32();
        while (ArrayItemsNum > 0){
            ArrayItemsNum--;
            FITEMINFO Value;
            Value.CreateFromStream(Stream);
            Items.Add(Value);
        }
    }

    void AddToStream(FBundle *Stream) const{
        Stream->WriteUint32(Items.Num());
        for (int32 i = 0; i < Items.Num(); i++) {
            Items[i].AddToStream(Stream);
        }
    }
 
}; 
