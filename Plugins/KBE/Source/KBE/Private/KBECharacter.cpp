#include "KBECharacter.h"
#include "DrawDebugHelpers.h"
#include "KBE.h"
#pragma optimize("", off)
AKBECharacter::AKBECharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Entity = nullptr;
}
void AKBECharacter::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent *CharacterMovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->MovementMode = EMovementMode::MOVE_Walking;
	}

}
void AKBECharacter::KBESetRemoteRole(const ENetRole InRemoteRole)
{
	SetRemoteRoleForBackwardsCompat(InRemoteRole);
}

void AKBECharacter::BeginDestroy()
{
	Super::BeginDestroy();
}

void AKBECharacter::GetFloorPoint(const FVector &Location, FHitResult &OutHit, float Radius)
{
	FCollisionQueryParams Params(SCENE_QUERY_STAT(UpdateMovement), true, this);
	FVector Start = Location + FVector(0, 0, 50.0);
	FVector End = Location + FVector(0, 0, -500.0);

	//bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, Params);
	bool bHit = GetWorld()->SweepSingleByChannel(OutHit, Start
		, End, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(Radius), Params);

}
void AKBECharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	check(ComponentEntity());
	if (Role == ROLE_SimulatedProxy  &&
		ComponentEntity()->GetKBENetConnect() != nullptr && 
		RepMovements.Num() != 0)
	{ 
		while (RepMovements.Num())
		{
			ReplicatedMovement = RepMovements.Pop();
		}
		OnRep_ReplicatedMovement();
	}
}

void AKBECharacter::OnUpdateEntityMovement()
{
	check(ComponentEntity());
	if (ComponentEntity()->GetKBENetConnect() == nullptr)
	{
		return;
	}
	FVector AdjPoint = ComponentEntity()->KBESyncLocation;
	TEnumAsByte<enum EMovementMode>  &MovementMode = GetCharacterMovement()->MovementMode;
	float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	FVector ActorLocation = OldSyncLocation;
	FHitResult TargetHits;
	FHitResult LocationHits;

	GetFloorPoint(ActorLocation, LocationHits, Radius);
	static float ScaledHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	static float Height = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	FVector LinearVelocity;

	if (MovementMode == MOVE_Walking)
	{
		GetFloorPoint(AdjPoint, TargetHits, Radius);

		AdjPoint = TargetHits.ImpactPoint + FVector(0, 0, Height);

		LinearVelocity = TargetHits.ImpactPoint - LocationHits.ImpactPoint;
		//DrawDebugSphere(GetWorld(), TargetHits.ImpactPoint, 10, 30, FColor::Red, false, 2.0);
	}
	else
	{
		AdjPoint = ComponentEntity()->KBESyncLocation;
		LinearVelocity = AdjPoint - OldSyncLocation;
	}

	FRotator Rotator(0, ComponentEntity()->KBESyncDirection.Y, 0);
	float Speed = GetMovementComponent()->GetMaxSpeed() * MaxSpeedPercentage;


	if (MovementMode == MOVE_Walking)
	{
		LinearVelocity.Z = 0.0;
	}
	LinearVelocity.Normalize();
	LinearVelocity = LinearVelocity * Speed;

	if (GetRemoteRole() == ROLE_AutonomousProxy)
	{
		SetActorLocation(AdjPoint);
		SetActorRotation(Rotator);
		//PostNetReceiveVelocity(LinearVelocity);
	}
	else if (Role == ROLE_SimulatedProxy)
	{
		FRepMovement  RepMovement = ReplicatedMovement;

		RepMovement.Location = AdjPoint;
		RepMovement.LinearVelocity = LinearVelocity;
		RepMovement.Rotation = Rotator;

		RepMovements.Push(RepMovement);
		OldSyncLocation = ComponentEntity()->KBESyncLocation;
	}
}
UEntity *AKBECharacter::ComponentEntity()
{ 
	if (!Entity->IsValidLowLevel())
	{
		TInlineComponentArray<UEntity*> ComponentArray;
		GetComponents<UEntity>(ComponentArray);

		if (ComponentArray.Num() == 1)
		{
			Entity = ComponentArray[0];
		}
	}
	return Entity;
}
float AKBECharacter::GetSpeedPercentage()
{
	FVector Velocity = GetVelocity();
	float MaxSpeed = GetMovementComponent()->GetMaxSpeed();
	return (Velocity.Size() / MaxSpeed);
}
void AKBECharacter::SetSpeedPercentage(float Percentage)
{
	MaxSpeedPercentage = Percentage;
}
void AKBECharacter::KBEReplicatedBasedMovement()
{
	ReplicatedBasedMovement.MovementBase = nullptr;

	ReplicatedMovementMode = MOVE_Falling;
}
#pragma optimize("", on)