// Fill out your copyright notice in the Description page of Project Settings.
#include "RoomManager.h"
#include "External/delaunator.hpp"
#include "Kismet/KismetSystemLibrary.h"

typedef ARoom::Direction Direction;

// Sets default values
ARoomManager::ARoomManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARoomManager::BeginPlay()
{
	Super::BeginPlay();
	MakeNewLevel();
}

void ARoomManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
}

void ARoomManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetActorLocation(FVector{ 0,0,0 });
}

// Called every frame
void ARoomManager::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

void ARoomManager::Destroyed()
{
	Super::Destroyed();
	if (!Rooms.IsEmpty())
	{
		for (auto room : Rooms)
		{
			room->Destroy();
		}
		Rooms.Empty();
	}
}

void ARoomManager::SpawnNewRooms(ARoom* node, int level, FastNoise* noise)
{
	int numRooms = FMath::RandRange(1, 3);

	//if(numRooms == 3 || numRooms == 2) numRooms = FMath::RandRange(1, 3);

	for (int i = 0; i < numRooms; i++)
	{
		FTransform transform;

		Direction direction = static_cast<Direction>(FMath::RandRange(0, 3));
		while (node->UsedDirections.Contains(direction))
		{
			direction = static_cast<Direction>(FMath::RandRange(0, 3));
		}
		FVector location;

		switch (direction)
		{
		case Direction::North:
			location = node->GetActorLocation() + FVector{ 0,float(node->SizeY * node->Scale) - node->Scale,0 };
			break;
		case Direction::East:
			location = node->GetActorLocation() + FVector{ float(node->SizeX * node->Scale) - node->Scale,0,0 };
			break;
		case Direction::South:
			location = node->GetActorLocation() - FVector{0, float(node->SizeY * node->Scale) - node->Scale,0 };
			break;
		case Direction::West:
			location = node->GetActorLocation() - FVector{ float(node->SizeX * node->Scale) - node->Scale,0,0 };
			break;
		default:
			break;
		}

		FHitResult Hit;
		FVector Start = location + FVector{0,0,float(node->Scale * node->WallSizeZ)};
		FVector End = location - FVector{ 0,0,float(node->Scale * node->WallSizeZ)};
		ECollisionChannel Channel = ECC_Visibility;
		FCollisionQueryParams Params;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Channel, Params);

		//DrawDebugLine(GetWorld(), Start, End, Hit.bBlockingHit ? FColor::Red : FColor::Blue, true, 5.0f, 0, 10.0f);

		auto hitActor = Cast<ARoom>(Hit.GetActor());
		if (!hitActor)
		{
			node->UsedDirections.Push(direction);

			transform.SetLocation(location);
			ARoom* room = GetWorld()->SpawnActor<ARoom>(RoomClass, transform);
			node->Connections.Push(room);
			Rooms.Push(room);
			room->GenerateMesh(noise);

			auto nodeDirection = Direction::North;
			if (direction == Direction::North) nodeDirection = Direction::South;
			if (direction == Direction::East) nodeDirection = Direction::West;
			if (direction == Direction::South) nodeDirection = Direction::North;
			if (direction == Direction::West) nodeDirection = Direction::East;
			room->UsedDirections.Push(nodeDirection);
		}		
	}

	if (level < MaxRecursionLevel)
	{
		for (auto& subnode : node->Connections)
		{
			SpawnNewRooms(subnode, ++level, noise);
		}
	}
	
}

void ARoomManager::MakeNewLevel()
{
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
	if (Rooms.Num() > 0)
	{
		for (auto& room : Rooms)
		{
			room->ProcMesh->ClearAllMeshSections();
			room->Destroy();
		}
		Rooms.Empty();
	}

	FastNoise noise;
	noise.SetNoiseType(FastNoise::SimplexFractal);
	noise.SetSeed(FMath::Rand());

	FTransform transform;
	auto location = FVector{ 0,0,0 };
	transform.SetLocation(location);

	ARoom* root = GetWorld()->SpawnActor<ARoom>(RoomClass, transform);
	root->GenerateMesh(&noise);
	Rooms.Push(root);

	SpawnNewRooms(root,0,&noise);
	//DrawDebugConnections();
	MakeWalls(&noise);
}

void ARoomManager::MakeWalls(FastNoise* noise)
{
	int index = 0;

	for (auto& room : Rooms)
	{
		room->MakeWalls(noise);
		room->PlaceObjects();
	}
}

void ARoomManager::DrawDebugConnections()
{
	// iterate over each room and add its connections to the arrays
	for (auto& room : Rooms)
	{	
		for (auto& connection : room->Connections)
		{
			DrawDebugLine(GetWorld(), room->GetActorLocation(), connection->GetActorLocation(), FColor{ 255,0,255 }, true, 5.0f, 0, 10.0f);
		}
	}
}
