// Fill out your copyright notice in the Description page of Project Settings.
#include "RoomManager.h"
#include "External/delaunator.hpp"
#include "Kismet/GameplayStatics.h"
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
	SpawnEnemies(Rooms);
	FindFurthestRoom();
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

	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AEnemyCharacter::StaticClass(), outActors);
	for (auto& actor : outActors)
	{
		actor->Destroy();
	}

}

void ARoomManager::SpawnNewRooms(ARoom* node, int level, FastNoise* noise)
{
	// Roll random number of rooms to spawn
	int numRooms = FMath::RandRange(1, 3);

	for (int i = 0; i < numRooms; i++)
	{
		FTransform transform;

		// Roll a new direction
		Direction direction = static_cast<Direction>(FMath::RandRange(0, 3));
		while (node->UsedDirections.Contains(direction))
		{
			direction = static_cast<Direction>(FMath::RandRange(0, 3));
		}
		FVector location;

		// Adjust location of new room to spawn
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

		// Raycast to check if a room has already been spawned
		FHitResult Hit;
		FVector Start = location + FVector{0,0,float(node->Scale * node->WallSizeZ)};
		FVector End = location - FVector{ 0,0,float(node->Scale * node->WallSizeZ)};
		ECollisionChannel Channel = ECC_Visibility;
		FCollisionQueryParams Params;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Channel, Params);

		auto hitActor = Cast<ARoom>(Hit.GetActor());
		if (!hitActor)
		{
			node->UsedDirections.Push(direction);
			transform.SetLocation(location);

			// Spawn a new room and generate mesh
			ARoom* room = GetWorld()->SpawnActor<ARoom>(RoomClass, transform);
			node->Connections.Push(room);
			Rooms.Push(room);
			room->GenerateMesh(noise);

			// Push used direction onto old room
			auto nodeDirection = Direction::North;
			if (direction == Direction::North) nodeDirection = Direction::South;
			if (direction == Direction::East) nodeDirection = Direction::West;
			if (direction == Direction::South) nodeDirection = Direction::North;
			if (direction == Direction::West) nodeDirection = Direction::East;
			room->UsedDirections.Push(nodeDirection);
		}		
	}

	// Recursively place rooms
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

	// Spawn root room
	ARoom* root = GetWorld()->SpawnActor<ARoom>(RoomClass, transform);
	root->GenerateMesh(&noise);
	Rooms.Push(root);

	// Recursively generate rooms
	SpawnNewRooms(root,0,&noise);
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

void ARoomManager::SpawnEnemies(TArray<ARoom*> rooms)
{
	for (ARoom* room : rooms)
	{
		auto rand = FMath::RandRange(0, 3);

		// 50% chance to spawn an enemy in the current room
		if (rand > 2)
		{
			auto enemyClass = EnemyClasses[FMath::RandRange(0, EnemyClasses.Num() - 1)];

			// Spawn an enemy actor at a random location within the room
			FVector roomLocation = room->GetActorLocation();
			auto minX = ((-room->SizeX / 2) * room->Scale) + (2 * room->Scale);
			auto maxX = ( (room->SizeX / 2) * room->Scale)  - (2 * room->Scale);
			auto minY = ((-room->SizeY / 2) * room->Scale) + (2 * room->Scale);
			auto maxY = ( (room->SizeY / 2) * room->Scale) - (2 * room->Scale);
			FVector randomOffset = FVector(FMath::RandRange(minX, maxX), FMath::RandRange(minY, maxY), 100.0f);
			FVector enemyLocation = roomLocation + randomOffset;
			GetWorld()->SpawnActor<AEnemyCharacter>(enemyClass, enemyLocation, FRotator::ZeroRotator);
		}
	}
}

void ARoomManager::FindFurthestRoom()
{
	float maxDistance = INT_MIN;
	for (auto& room : Rooms)
	{
		auto distance = FVector::Distance(room->GetActorLocation(),GetActorLocation());
		if (distance > maxDistance)
		{
			maxDistance = distance;
			FurthestRoom = room;
		}
	}
	if (FurthestRoom)
	{
		FurthestRoom->SpawnNextArchway();
	}
}
