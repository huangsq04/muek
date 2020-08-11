#pragma once
#include "MUEK.h"
#include "Entity.h"
#include "Alias.h"
#include "DefineEntity.generated.h"
#pragma optimize("", off)

UCLASS(BlueprintType, Blueprintable)
class MUEK_API UAccountCellMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
};

UCLASS(BlueprintType, Blueprintable)
class MUEK_API UAccountBaseMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Entity | Base")
    void GetRoleInfo()
    {
        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();
        BundlePtr->WriteUint16(4);
        Entity->BaseMailbox()->PostMail(NULL);
    }
    UFUNCTION(BlueprintCallable, Category = "Entity | Base")
    void Logoff()
    {
        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();
        BundlePtr->WriteUint16(3);
        Entity->BaseMailbox()->PostMail(NULL);
    }
    UFUNCTION(BlueprintCallable, Category = "Entity | Base")
    void OnCreatePlayer(const FString &arg0)
    {
        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();
        BundlePtr->WriteUint16(2);
        BundlePtr->WriteString(arg0);
        Entity->BaseMailbox()->PostMail(NULL);
    }
    UFUNCTION(BlueprintCallable, Category = "Entity | Base")
    void PlayerEnter(const int32 ID,const FString &MapName)
    {
        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();
        BundlePtr->WriteUint16(1);
        BundlePtr->WriteInt32(ID);
        BundlePtr->WriteString(MapName);
        Entity->BaseMailbox()->PostMail(NULL);
    }
};
UCLASS(BlueprintType, Blueprintable)
class MUEK_API  UAccountEntity: public UEntity
{
    GENERATED_BODY()
public:
    virtual void Init()
    {
    	Cell = NewObject<UAccountCellMethod>();
    	Cell->Entity = this;
    	Base = NewObject<UAccountBaseMethod>();
    	Base->Entity = this;
    	SetClassName(TEXT("Account"));
    }
    virtual bool UsePropertyDescrAlias() { return 1; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    UAccountBaseMethod *Base;
    virtual UObject *GetBase() { return Base; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    UAccountCellMethod *Cell;
    virtual UObject *GetCell() { return Cell; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    int32 AuthorityType;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    FROLEINFO RoleList;
    virtual FString OnUpdateProperty(uint16 UType, FMemoryStream& Stream)
    {
        FString Command = TEXT("");
        switch(UType){
            case 0:
                __SET_ENTITY_POSITION();
                 break;
            case 1:
                __SET_ENTITY_DIRECTION();
                break;
            case 2:
                SpaceID = Stream.ReadInt32();
                break;
            case 3:
                AuthorityType= Stream.ReadInt32();
                Command = FString::Printf(TEXT("OnRep_AuthorityType"));
                break;
            case 4:
                RoleList.CreateFromStream(&Stream);
                Command = FString::Printf(TEXT("OnRep_RoleList"));
                break;
        }
        return Command;
    }

    UFUNCTION(BlueprintNativeEvent, Category = "Entity | Client")
    void RespCreatePlayer(const int32 ID,const FString &RoleName);
    virtual void RespCreatePlayer_Implementation(const int32 ID,const FString &RoleName){};
    UFUNCTION(BlueprintNativeEvent, Category = "Entity | Client")
    void RespGetRoleInfo(const FROLEINFO &arg0);
    virtual void RespGetRoleInfo_Implementation(const FROLEINFO &arg0){};
    virtual void OnRemoteMethodCall(FMemoryStream& Stream)
    {
    	    uint16 AliasID = Stream.ReadUint8();
            switch(AliasID){
            case 0xFFFF:
                 break;
            case 0:
            {
               int32 ID;
               ID= Stream.ReadInt32();
               FString RoleName;
               RoleName= Stream.ReadString();
               RespCreatePlayer(ID,RoleName);
               break;
            }
            case 1:
            {
               FROLEINFO arg0;
               arg0.CreateFromStream(&Stream);
               RespGetRoleInfo(arg0);
               break;
            }
        }
      }

};


UCLASS(BlueprintType, Blueprintable)
class MUEK_API URoleCellMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Entity | Cell")
    void CreateMonster(const FString &PlayerName,const FVector Position)
    {
        FBundle* BundlePtr = Entity->CellMailbox()->NewMail();
        BundlePtr->WriteUint16(8);
        BundlePtr->WriteString(PlayerName);
        BundlePtr->WriteFloat(Position.X);
        BundlePtr->WriteFloat(Position.Y);
        BundlePtr->WriteFloat(Position.Z);
        Entity->CellMailbox()->PostMail(NULL);
    }
    UFUNCTION(BlueprintCallable, Category = "Entity | Cell")
    void LevelTeleport(const FString &MapName,const FVector Position)
    {
        FBundle* BundlePtr = Entity->CellMailbox()->NewMail();
        BundlePtr->WriteUint16(7);
        BundlePtr->WriteString(MapName);
        BundlePtr->WriteFloat(Position.X);
        BundlePtr->WriteFloat(Position.Y);
        BundlePtr->WriteFloat(Position.Z);
        Entity->CellMailbox()->PostMail(NULL);
    }
};

UCLASS(BlueprintType, Blueprintable)
class MUEK_API URoleBaseMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Entity | Base")
    void OnLeaveWorld()
    {
        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();
        BundlePtr->WriteUint16(9);
        Entity->BaseMailbox()->PostMail(NULL);
    }
    UFUNCTION(BlueprintCallable, Category = "Entity | Base")
    void logoff()
    {
        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();
        BundlePtr->WriteUint16(10);
        Entity->BaseMailbox()->PostMail(NULL);
    }
};
UCLASS(BlueprintType, Blueprintable)
class MUEK_API  URoleEntity: public UEntity
{
    GENERATED_BODY()
public:
    virtual void Init()
    {
    	Cell = NewObject<URoleCellMethod>();
    	Cell->Entity = this;
    	Base = NewObject<URoleBaseMethod>();
    	Base->Entity = this;
    	SetClassName(TEXT("Role"));
    }
    virtual bool UsePropertyDescrAlias() { return 1; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    URoleBaseMethod *Base;
    virtual UObject *GetBase() { return Base; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    URoleCellMethod *Cell;
    virtual UObject *GetCell() { return Cell; };
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    int32 EXP;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    int32 HP;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    FROLEITEMS ItemList;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    FString Map;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    int32 ParentDBID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    FString PlayerName;
    bool IsSupportedForNetworking() const	{ return true;	}
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(URoleEntity, EXP);
        DOREPLIFETIME(URoleEntity, HP);
        DOREPLIFETIME(URoleEntity, ItemList);
        DOREPLIFETIME(URoleEntity, Map);
        DOREPLIFETIME(URoleEntity, ParentDBID);
        DOREPLIFETIME(URoleEntity, PlayerName);
    }
    virtual FString OnUpdateProperty(uint16 UType, FMemoryStream& Stream)
    {
        FString Command = TEXT("");
        switch(UType){
            case 0:
                __SET_ENTITY_POSITION();
                 break;
            case 1:
                __SET_ENTITY_DIRECTION();
                break;
            case 2:
                SpaceID = Stream.ReadInt32();
                break;
            case 3:
                EXP= Stream.ReadInt32();
                Command = FString::Printf(TEXT("OnRep_EXP"));
                break;
            case 4:
                HP= Stream.ReadInt32();
                Command = FString::Printf(TEXT("OnRep_HP"));
                break;
            case 5:
                ItemList.CreateFromStream(&Stream);
                Command = FString::Printf(TEXT("OnRep_ItemList"));
                break;
            case 6:
                Map= Stream.ReadString();
                Command = FString::Printf(TEXT("OnRep_Map"));
                break;
            case 7:
                ParentDBID= Stream.ReadInt32();
                Command = FString::Printf(TEXT("OnRep_ParentDBID"));
                break;
            case 8:
                PlayerName= Stream.ReadString();
                Command = FString::Printf(TEXT("OnRep_PlayerName"));
                break;
        }
        return Command;
    }

    UFUNCTION(BlueprintNativeEvent, Category = "Entity | Client")
    void OnFire(const FVector &Position,const FVector &Rotation);
    virtual void OnFire_Implementation(const FVector &Position,const FVector &Rotation){};
    UFUNCTION(BlueprintNativeEvent, Category = "Entity | Client")
    void ReqFire(const FVector &Position,const FVector &Rotation);
    virtual void ReqFire_Implementation(const FVector &Position,const FVector &Rotation){};
    virtual void OnRemoteMethodCall(FMemoryStream& Stream)
    {
    	    uint16 AliasID = Stream.ReadUint8();
            switch(AliasID){
            case 0xFFFF:
                 break;
            case 0:
            {
               FVector Position;
               Position= Stream.ReadFVector();
               FVector Rotation;
               Rotation= Stream.ReadFVector();
               OnFire(Position,Rotation);
               break;
            }
            case 1:
            {
               FVector Position;
               Position= Stream.ReadFVector();
               FVector Rotation;
               Rotation= Stream.ReadFVector();
               ReqFire(Position,Rotation);
               break;
            }
        }
      }

};


UCLASS(BlueprintType, Blueprintable)
class MUEK_API UAgentCellMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Entity | Cell")
    void CreateControllMonster(const TArray<FCREATEMONSTERARG> &arg0)
    {
        FBundle* BundlePtr = Entity->CellMailbox()->NewMail();
        BundlePtr->WriteUint16(15);
        BundlePtr->WriteInt32(arg0.Num());
        for(int i = 0; i < arg0.Num(); i++)
            arg0[i].AddToStream(BundlePtr);
        Entity->CellMailbox()->PostMail(NULL);
    }
    UFUNCTION(BlueprintCallable, Category = "Entity | Cell")
    void CreateMonster(const FString &EntityClass,const FString &PlayerName,const FVector Position)
    {
        FBundle* BundlePtr = Entity->CellMailbox()->NewMail();
        BundlePtr->WriteUint16(13);
        BundlePtr->WriteString(EntityClass);
        BundlePtr->WriteString(PlayerName);
        BundlePtr->WriteFloat(Position.X);
        BundlePtr->WriteFloat(Position.Y);
        BundlePtr->WriteFloat(Position.Z);
        Entity->CellMailbox()->PostMail(NULL);
    }
    UFUNCTION(BlueprintCallable, Category = "Entity | Cell")
    void SetAgentLocation(const FVector Position)
    {
        FBundle* BundlePtr = Entity->CellMailbox()->NewMail();
        BundlePtr->WriteUint16(14);
        BundlePtr->WriteFloat(Position.X);
        BundlePtr->WriteFloat(Position.Y);
        BundlePtr->WriteFloat(Position.Z);
        Entity->CellMailbox()->PostMail(NULL);
    }
};

UCLASS(BlueprintType, Blueprintable)
class MUEK_API UAgentBaseMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Entity | Base")
    void Logoff()
    {
        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();
        BundlePtr->WriteUint16(17);
        Entity->BaseMailbox()->PostMail(NULL);
    }
    UFUNCTION(BlueprintCallable, Category = "Entity | Base")
    void OnLeaveWorld()
    {
        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();
        BundlePtr->WriteUint16(16);
        Entity->BaseMailbox()->PostMail(NULL);
    }
};
UCLASS(BlueprintType, Blueprintable)
class MUEK_API  UAgentEntity: public UEntity
{
    GENERATED_BODY()
public:
    virtual void Init()
    {
    	Cell = NewObject<UAgentCellMethod>();
    	Cell->Entity = this;
    	Base = NewObject<UAgentBaseMethod>();
    	Base->Entity = this;
    	SetClassName(TEXT("Agent"));
    }
    virtual bool UsePropertyDescrAlias() { return 1; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    UAgentBaseMethod *Base;
    virtual UObject *GetBase() { return Base; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    UAgentCellMethod *Cell;
    virtual UObject *GetCell() { return Cell; };
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    FString Map;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    int32 ParentDBID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    FString PlayerName;
    bool IsSupportedForNetworking() const	{ return true;	}
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(UAgentEntity, Map);
        DOREPLIFETIME(UAgentEntity, ParentDBID);
        DOREPLIFETIME(UAgentEntity, PlayerName);
    }
    virtual FString OnUpdateProperty(uint16 UType, FMemoryStream& Stream)
    {
        FString Command = TEXT("");
        switch(UType){
            case 0:
                __SET_ENTITY_POSITION();
                 break;
            case 1:
                __SET_ENTITY_DIRECTION();
                break;
            case 2:
                SpaceID = Stream.ReadInt32();
                break;
            case 3:
                Map= Stream.ReadString();
                Command = FString::Printf(TEXT("OnRep_Map"));
                break;
            case 4:
                ParentDBID= Stream.ReadInt32();
                Command = FString::Printf(TEXT("OnRep_ParentDBID"));
                break;
            case 5:
                PlayerName= Stream.ReadString();
                Command = FString::Printf(TEXT("OnRep_PlayerName"));
                break;
        }
        return Command;
    }

    UFUNCTION(BlueprintNativeEvent, Category = "Entity | Client")
    void OnSetAgentLocation();
    virtual void OnSetAgentLocation_Implementation(){};
    virtual void OnRemoteMethodCall(FMemoryStream& Stream)
    {
    	    uint16 AliasID = Stream.ReadUint8();
            switch(AliasID){
            case 0xFFFF:
                 break;
            case 0:
            {
               OnSetAgentLocation();
               break;
            }
        }
      }

};


UCLASS(BlueprintType, Blueprintable)
class MUEK_API UMonsterCellMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Entity | Cell")
    void ControllMonster(const int32 ID)
    {
        FBundle* BundlePtr = Entity->CellMailbox()->NewMail();
        BundlePtr->WriteUint16(19);
        BundlePtr->WriteInt32(ID);
        Entity->CellMailbox()->PostMail(NULL);
    }
};

UCLASS(BlueprintType, Blueprintable)
class MUEK_API UMonsterBaseMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
};
UCLASS(BlueprintType, Blueprintable)
class MUEK_API  UMonsterEntity: public UEntity
{
    GENERATED_BODY()
public:
    virtual void Init()
    {
    	Cell = NewObject<UMonsterCellMethod>();
    	Cell->Entity = this;
    	Base = NewObject<UMonsterBaseMethod>();
    	Base->Entity = this;
    	SetClassName(TEXT("Monster"));
    }
    virtual bool UsePropertyDescrAlias() { return 1; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    UMonsterBaseMethod *Base;
    virtual UObject *GetBase() { return Base; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    UMonsterCellMethod *Cell;
    virtual UObject *GetCell() { return Cell; };
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    uint8 AnimState;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    float MoveSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    FString PlayerName;
    bool IsSupportedForNetworking() const	{ return true;	}
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(UMonsterEntity, AnimState);
        DOREPLIFETIME(UMonsterEntity, MoveSpeed);
        DOREPLIFETIME(UMonsterEntity, PlayerName);
    }
    virtual FString OnUpdateProperty(uint16 UType, FMemoryStream& Stream)
    {
        FString Command = TEXT("");
        switch(UType){
            case 0:
                __SET_ENTITY_POSITION();
                 break;
            case 1:
                __SET_ENTITY_DIRECTION();
                break;
            case 2:
                SpaceID = Stream.ReadInt32();
                break;
            case 3:
                AnimState= Stream.ReadUint8();
                Command = FString::Printf(TEXT("OnRep_AnimState"));
                break;
            case 4:
                MoveSpeed= Stream.ReadFloat();
                Command = FString::Printf(TEXT("OnRep_MoveSpeed"));
                break;
            case 5:
                PlayerName= Stream.ReadString();
                Command = FString::Printf(TEXT("OnRep_PlayerName"));
                break;
        }
        return Command;
    }

    UFUNCTION(BlueprintNativeEvent, Category = "Entity | Client")
    void OnFire(const FVector &Position,const FVector &Rotation);
    virtual void OnFire_Implementation(const FVector &Position,const FVector &Rotation){};
    virtual void OnRemoteMethodCall(FMemoryStream& Stream)
    {
    	    uint16 AliasID = Stream.ReadUint8();
            switch(AliasID){
            case 0xFFFF:
                 break;
            case 0:
            {
               FVector Position;
               Position= Stream.ReadFVector();
               FVector Rotation;
               Rotation= Stream.ReadFVector();
               OnFire(Position,Rotation);
               break;
            }
        }
      }

};


UCLASS(BlueprintType, Blueprintable)
class MUEK_API UMapManagerCellMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
};

UCLASS(BlueprintType, Blueprintable)
class MUEK_API UMapManagerBaseMethod: public UEntityServerMethod
{
    GENERATED_BODY()
public:
};
UCLASS(BlueprintType, Blueprintable)
class MUEK_API  UMapManagerEntity: public UEntity
{
    GENERATED_BODY()
public:
    virtual void Init()
    {
    	Cell = NewObject<UMapManagerCellMethod>();
    	Cell->Entity = this;
    	Base = NewObject<UMapManagerBaseMethod>();
    	Base->Entity = this;
    	SetClassName(TEXT("MapManager"));
    }
    virtual bool UsePropertyDescrAlias() { return 1; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    UMapManagerBaseMethod *Base;
    virtual UObject *GetBase() { return Base; };
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
    UMapManagerCellMethod *Cell;
    virtual UObject *GetCell() { return Cell; };
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Entity")
    FString mapType;
    bool IsSupportedForNetworking() const	{ return true;	}
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(UMapManagerEntity, mapType);
    }
    virtual FString OnUpdateProperty(uint16 UType, FMemoryStream& Stream)
    {
        FString Command = TEXT("");
        switch(UType){
            case 0:
                __SET_ENTITY_POSITION();
                 break;
            case 1:
                __SET_ENTITY_DIRECTION();
                break;
            case 2:
                SpaceID = Stream.ReadInt32();
                break;
            case 3:
                mapType= Stream.ReadString();
                Command = FString::Printf(TEXT("OnRep_mapType"));
                break;
        }
        return Command;
    }

    virtual void OnRemoteMethodCall(FMemoryStream& Stream)
    {
    	    uint16 AliasID = Stream.ReadUint8();
            switch(AliasID){
            case 0xFFFF:
                 break;
        }
      }

};

inline void GetClassNameFromType(TMap<int,FString> &EntityClassIndex)
 {
    EntityClassIndex.Add(1, TEXT("Account"));
    EntityClassIndex.Add(2, TEXT("Role"));
    EntityClassIndex.Add(3, TEXT("Agent"));
    EntityClassIndex.Add(4, TEXT("Monster"));
    EntityClassIndex.Add(5, TEXT("MapManager"));
 }

#pragma optimize("", on)
