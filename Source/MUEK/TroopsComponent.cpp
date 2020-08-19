// Fill out your copyright notice in the Description page of Project Settings.
#include "TroopsComponent.h"
#include "KBECharacter.h"
#include "DefineEntity.h"
#include "Kismet/KismetMathLibrary.h"

#pragma optimize("", off)
void AFollow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (FollowMoveType == 1 && 0)
	{
		//¸úËæ
		FVector Point = TargetLocation - GetActorLocation();
		float DstSquared = Point.SizeSquared();
		if (DstSquared > 4000000)
		{
			SetActorLocation(TargetLocation);
		}
	}
}
// Sets default values for this component's properties
UTroopsComponent::UTroopsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTroopsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UTroopsComponent::BeginDestroy()
{
	Super::BeginDestroy();
	for (auto &Item : FollowCharacters)
	{
		Item.Value->ConditionalBeginDestroy();
	}
	FollowCharacters.Empty();
}

// Called every frame
void UTroopsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector Point = GetOwner()->GetActorLocation();
	float Yaw = GetOwner()->GetActorRotation().Yaw;
	if (FollowMoveType == 1 && (Point != LastPoint || Yaw != LastYaw))
	{
		TMap<int, FVector> ToopsRange;
		UTroopsComponent::Rank(Point, Yaw, RankState, ToopsRange);
		for (auto &Item : FollowCharacters)
		{
			Item.Value->TargetLocation = ToopsRange[Item.Key];
		}
		LastPoint = Point;
		LastYaw = Yaw;
	}

	// ...
}

void UTroopsComponent::Rank(const FVector &Location, float Yaw, int Type, TMap<int, FVector> &OutRank)
{
	OutRank.Empty();

	if (Type == 0)
	{
		for (int X = 0; X < 2; X++)
		{
			for (int Y = 0; Y < RankNum * RankNum / 2; Y++)
			{
				FVector  Vector = FVector((X + 1)* RankInterval, (-(RankNum * RankNum / 2.0 - 1.0) / 2.0 + Y) * RankInterval, 0);
				FRotator B(0, 0, 0);
				B.Yaw = Yaw;
				Vector = B.RotateVector(Vector);
				OutRank.Add(OutRank.Num(), Vector + Location);
			}
		}
	}
	else if (Type == 1)
	{
		for (int X = 0; X < 2; X++)
		{
			for (int Y = 0; Y < RankNum * RankNum / 2; Y++)
			{
				FVector  Vector = FVector((-X - 1)* RankInterval, ((RankNum * RankNum / 2.0 - 1.0) / 2.0 - Y) * RankInterval, 0);
				FRotator B(0, 0, 0);
				B.Yaw = Yaw;
				Vector = B.RotateVector(Vector);
				OutRank.Add(OutRank.Num(), Vector + Location);
			}
		}
	}
	else if (Type == 2)
	{
		for (int X = 0; X < RankNum * RankNum; X++)
		{
			for (int Y = 0; Y < 2; Y++)
			{
				FVector  Vector = FVector((-X - 1)* RankInterval, (0.5 - Y) * RankInterval, 0);
				FRotator B(0, 0, 0);
				B.Yaw = Yaw;
				Vector = B.RotateVector(Vector);
				OutRank.Add(OutRank.Num(), Vector + Location);
			}
		}
	}
	if (Type == 3)
	{
		for (int X = 0; X < RankNum; X++)
		{
			for (int Y = 0; Y < RankNum; Y++)
			{
				FVector  Vector = FVector((X + 1)* RankInterval, (-(RankNum - 1.0) / 2.0 + Y) * RankInterval, 0);
				FRotator B(0, 0, 0);
				B.Yaw = Yaw;
				Vector = B.RotateVector(Vector);
				OutRank.Add(OutRank.Num(), Vector + Location);
			}
		}
	}
	else if (Type == 4)
	{
		for (int X = 0; X < RankNum; X++)
		{
			for (int Y = 0; Y < RankNum; Y++)
			{
				FVector  Vector = FVector((-X - 1)* RankInterval, ((RankNum - 1.0) / 2.0 - Y) * RankInterval, 0);
				FRotator B(0, 0, 0);
				B.Yaw = Yaw;
				Vector = B.RotateVector(Vector);
				OutRank.Add(OutRank.Num(), Vector + Location);
			}
		}
	}
}
void UTroopsComponent::CreateFollowCharacter()
{
	TMap<int, FVector> ActorRange;
	TMap<int, FVector> ToopsRange;
	UTroopsComponent::Rank(GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation().Yaw, 0, ToopsRange);
	UTroopsComponent::Rank(FVector(0 ,0, 0), 0.0, 0, ActorRange);

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int Y = 0; Y < RankNum * RankNum; Y++)
	{
		AFollow *Follow = GetWorld()->SpawnActor<AFollow>(FollowClass, ToopsRange[Y], GetOwner()->GetActorRotation(), SpawnParameters);
		AActor *SpawnedActor = GetWorld()->SpawnActor<AActor>(FollowActorClass, ActorRange[Y], GetOwner()->GetActorRotation(), SpawnParameters);
		SpawnedActor->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
		//Follow->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
		Follow->FollowActor = SpawnedActor;
		Follow->TargetLocation = ToopsRange[Y];
		Follow->TargetYaw = GetOwner()->GetActorRotation().Yaw;
		FollowCharacters.Add(Y, Follow);
	}

}
void UTroopsComponent::ComeHere()
{
	//Í£ÒÆ¶¯
	TMap<int, FVector> ToopsRange;
	UTroopsComponent::Rank(GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation().Yaw, RankState, ToopsRange);
	for (auto &Item : FollowCharacters)
	{
		Item.Value->TargetLocation = ToopsRange[Item.Key];
		Item.Value->OnGoToTarget();
	}
}

void UTroopsComponent::FollowMe()
{
	FollowMoveType = 1;
	for (auto &Item : FollowCharacters)
	{
		Item.Value->FollowMoveType = FollowMoveType;
	}
}

void UTroopsComponent::StopFollowMe()
{
	FollowMoveType = 0;
	for (auto &Item : FollowCharacters)
	{
		Item.Value->FollowMoveType = FollowMoveType;
	}
}
#pragma optimize("", on)
