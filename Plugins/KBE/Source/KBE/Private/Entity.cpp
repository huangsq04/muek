#include "Entity.h"
#include "KBE.h"
#include "KBECharacter.h"
#include "AIController.h"
#include "KBEngineNetDriver.h"
#pragma optimize("", off)

void UEntity::PostInitProperties()
{
	Super::PostInitProperties();
	//Init();
}

void UEntity::BeginDestroy()
{
	Super::BeginDestroy();
}

void UEntity::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
}
void UEntity::SetByControll(bool Value)
{
	ByControll = Value;
	if (ByControll)
	{
		GetOwner()->Role = ROLE_Authority;
		AKBECharacter *Character = Cast<AKBECharacter>(GetOwner());
		if (Character)
		{
			//Character->KBESetRemoteRole(ROLE_Authority);
		}
	}
	else
	{
		if (IsRunningDedicatedServer())
		{
			GetOwner()->Role = ROLE_Authority;
			AKBECharacter *Character = Cast<AKBECharacter>(GetOwner());
			if (Character)
			{
				Character->KBESetRemoteRole(ROLE_AutonomousProxy);
				//Character->GetCapsuleComponent()->SetEnableGravity(false);
			}
		}
		else
		{
			GetOwner()->Role = ROLE_SimulatedProxy;
		}
	}
	if (OnByControllDelegate.IsBound())
	{
		OnByControllDelegate.Broadcast(Value);
	}
}
void UEntity::OnDestroy()
{
	if (SyncEntityInfo != nullptr)
	{
		delete SyncEntityInfo;
		SyncEntityInfo = nullptr;
	}

	GetOwner()->ConditionalBeginDestroy();
}
FVector UEntity::GetPosition()
{
	return GetOwner()->GetActorLocation();
}
FVector UEntity::GetDirection()
{
	AKBECharacter *EntityActor = Cast<AKBECharacter>(GetOwner());
	if (EntityActor)
	{
		return EntityActor->GetActorRotation().Euler();
	}
	return FVector::ZeroVector;
}
void UEntity::SetPosition(const FVector &Position)
{
	AKBECharacter *EntityActor = Cast<AKBECharacter>(GetOwner());
	if (EntityActor)
	{
		EntityActor->SetActorLocation(Position);
	}
}
void UEntity::SetDirection(const FVector &Direction)
{

}
float UEntity::GetSpeedPercentage()
{
	AKBECharacter *EntityActor = Cast<AKBECharacter>(GetOwner());
	if (EntityActor)
	{
		return  EntityActor->GetSpeedPercentage();;
	}	
	return 0.0;
}
void UEntity::SetSpeedPercentage(float Percentage)
{
	AKBECharacter *EntityActor = Cast<AKBECharacter>(GetOwner());
	if (EntityActor)
	{
		EntityActor->SetSpeedPercentage(Percentage);
	}
}
 void UEntity::OnUpdateEntityDirection(const FVector &Direction)
{
	 FVector dir =  Direction * 360.0 / (2 * PI);
	 KBESyncDirection = dir;

}
 void UEntity::OnUpdateEntityMovement(const FVector &Pos)
 {
	 KBESyncLocation = Pos;
	 AKBECharacter *Character = Cast<AKBECharacter>(GetOwner());
	 if (Character)
	 {
		 Character->OnUpdateEntityMovement();
	 }
 }
void UEntity::RedirectCallFunction(const TArray<uint8> &Arg)
{
	FMemoryStream Stream((uint8 *)Arg.GetData(), Arg.Num() * 8);
	KBENetConnect->OnRemoteMethodCall(&Stream);
}
AActor * UEntity::CreateEntityObject(const FString &Name, UObject* Outer)
{
	FKBEModule &KBE = FModuleManager::LoadModuleChecked<FKBEModule>(TEXT("KBE"));
	URegisterLoadEntityClass *ObjectAssets = LoadObject<URegisterLoadEntityClass>(NULL, TEXT("/Game/BluePrints/Datas/LoadEntityDataAsset.LoadEntityDataAsset"), NULL, LOAD_None, NULL);
	if (ObjectAssets == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Error URegisterLoadEntityClass::Load[/Game/BluePrints/Datas/LoadEntityDataAsset.LoadEntityDataAsset] !"));
		return nullptr;
	}
	for (auto& Item : ObjectAssets->items) {
		if (Item.EntityName == Name) {
			if (Item.ClassEntity == nullptr) return nullptr;
			if (!Item.ClassEntity->IsValidLowLevelFast()) return nullptr;
			FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor *Actor = KBE.World->SpawnActor<AActor>(Item.ClassEntity, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
			TInlineComponentArray<UEntity*> ComponentArray;
			Actor->GetComponents<UEntity>(ComponentArray);

			if (ComponentArray.Num() != 1)
			{
				return nullptr;
			}

			ComponentArray[0]->Init();

			return Actor;
		}
	}
	return nullptr;

}
bool UEntity::IsOnGround()
{
	AKBECharacter *Actor = Cast<AKBECharacter>(GetOwner());
	if (Actor)
	{
		bool Falling = Actor->GetCharacterMovement()->IsFalling();
		return !Falling;
	}
	return true;
}
void UEntity::SetGround(bool Ground)
{
	AKBECharacter *Actor = Cast<AKBECharacter>(GetOwner());
	if (Actor)
	{
		if (Ground)
		{
			if(Actor->GetCharacterMovement()->IsFalling())
			{ 
				Actor->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
		}
		else
		{
			if (Actor->GetCharacterMovement()->IsMovingOnGround())
			{
				Actor->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
				Actor->KBEReplicatedBasedMovement();
				Actor->OnRep_ReplicatedBasedMovement();
				Actor->PostNetReceive();
			}
		}
	}
}
/**
 * 更新所有属性
 */
void UEntity::OnUpdatePropertys(FMemoryStream& Stream) {

	while (Stream.Length() > 0) {		
		
		uint16 TypeId = 0;
		if (UsePropertyDescrAlias())
		{
			TypeId = Stream.ReadUint8();
		}
		else
		{
			TypeId = Stream.ReadUint16();
		}
		FOutputDeviceNull Ar;
		FString Command = OnUpdateProperty(TypeId, Stream);

		//执行改变响应函数
		if (Command != TEXT(""))
		{
			CallFunctionByNameWithArguments(*Command, Ar, NULL, true);
		}
	}
}
bool UEntity::NeedSyncEntityInfo()
{
	if (SyncEntityInfo == nullptr)
	{
		SyncEntityInfo = new FSyncEntityInfo();
		SyncEntityInfo->Position = GetPosition();
		SyncEntityInfo->Direction = GetDirection();
		SyncEntityInfo->Percentage = GetSpeedPercentage();
	}

	bool Need = SyncEntityInfo->Position != GetPosition() || 
		SyncEntityInfo->Direction != GetDirection() || 
		SyncEntityInfo->Percentage != GetSpeedPercentage();

	if (Need)
	{
		SyncEntityInfo = new FSyncEntityInfo();
		SyncEntityInfo->Position = GetPosition();
		SyncEntityInfo->Direction = GetDirection();
		SyncEntityInfo->Percentage = GetSpeedPercentage();
	}

	return Need;
}

bool UEntity::IsKBEConnect()
{
	if (KBENetConnect == nullptr) return false;
	return true;
}
void UEntity::InitPropertiesFinish()
{
	OnInitPropertyFinish();

	OnInitPropertiesFinishDelegate.Broadcast();
}
void UEntity::SetPlayerNameProperty(const FString &Value)
{
	UProperty* Property = GetClass()->FindPropertyByName(TEXT("PlayerName"));

	if (UStrProperty* StringProperty = Cast<UStrProperty>(Property))
	{
		StringProperty->SetPropertyValue(Property->ContainerPtrToValuePtr<FString>(this), Value);
	}
}
#pragma optimize("", on)