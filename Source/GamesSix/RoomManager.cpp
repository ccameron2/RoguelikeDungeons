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

	WallMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Room Mesh"));
	WallMesh->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> forestMaterial(TEXT("M_Material'/Game/Materials/M_Material.M_Material'"));
	Material = forestMaterial.Object;
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

// Called every frame
void ARoomManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoomManager::SpawnNewRooms(ARoom* node, int level, FastNoise* noise)
{
	int numRooms = FMath::RandRange(1, 3);

	if(numRooms == 3 || numRooms == 2) numRooms = FMath::RandRange(1, 3);
	if(numRooms == 3 || numRooms == 2) numRooms = FMath::RandRange(1, 3);
	if(numRooms == 3 || numRooms == 2) numRooms = FMath::RandRange(1, 3);
	if(numRooms == 3 || numRooms == 2) numRooms = FMath::RandRange(1, 3);

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
	if (Rooms.Num() > 0)
	{
		for (auto& room : Rooms)
		{
			room->ProcMesh->ClearMeshSection(0);
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

	SpawnNewRooms(root,0,&noise);
	DrawDebugConnections();
	MakeWalls(&noise);
}

void ARoomManager::MakeWalls(FastNoise* noise)
{
	int index = 0;

	for (auto& room : Rooms)
	{
		for (int i = 0; i < 4; i++)
		{
			auto direction = static_cast<Direction>(i);
			if (!room->UsedDirections.Contains(direction))
			{
				FVector location = room->GetActorLocation();
				TArray<FVector> vertices;
				TArray<FVector> normals;
				TArray<int32> triangles;

				switch (direction)
				{
				case Direction::North:
					for (int j = 0; j < room->SizeX; j++)
					{
						for (int k = 0; k < WallHeight; k++)
						{
							vertices.Push(FVector{ float(j * room->Scale),0,float(k * room->Scale) });
						}
					}
					for (auto& vertex : vertices)
					{
						vertex.X -= (room->SizeX * room->Scale) / 2;
						vertex.Y += (room->SizeY * room->Scale / 2) - room->Scale;
						vertex += location;
					}
					UKismetProceduralMeshLibrary::CreateGridMeshTriangles(room->SizeX, WallHeight, false, triangles);
					break;
				case Direction::South:
					for (int j = 0; j < room->SizeX; j++)
					{
						for (int k = 0; k < WallHeight; k++)
						{
							vertices.Push(FVector{ float(j * room->Scale),0,float(k * room->Scale) });
						}
					}
					for (auto& vertex : vertices)
					{
						vertex.X -= (room->SizeX * room->Scale) / 2;
						vertex.Y -= (room->SizeY * room->Scale / 2);
						vertex += location;
					}
					UKismetProceduralMeshLibrary::CreateGridMeshTriangles(room->SizeX, WallHeight, true, triangles);
					break;
				case Direction::East:
					for (int j = 0; j < room->SizeY; j++)
					{
						for (int k = 0; k < WallHeight; k++)
						{
							vertices.Push(FVector{ 0,float(j * room->Scale),float(k * room->Scale) });
						}
					}
					for (auto& vertex : vertices)
					{
						vertex.X += (room->SizeX * room->Scale / 2) - room->Scale;
						vertex.Y -= (room->SizeY * room->Scale / 2);
						vertex += location;
					}
					UKismetProceduralMeshLibrary::CreateGridMeshTriangles(room->SizeY, WallHeight, true, triangles);
					break;
				case Direction::West:
					for (int j = 0; j < room->SizeY; j++)
					{
						for (int k = 0; k < WallHeight; k++)
						{
							vertices.Push(FVector{ 0,float(j * room->Scale),float(k * room->Scale) });
						}
					}
					for (auto& vertex : vertices)
					{
						vertex.X -= (room->SizeX * room->Scale) / 2;
						vertex.Y -= (room->SizeY * room->Scale / 2);
						vertex += location;
					}
					UKismetProceduralMeshLibrary::CreateGridMeshTriangles(room->SizeY, WallHeight, false, triangles);
					break;
				}

				// For each vertex, get 2 different noise values and apply them to vertex hight at different scales.
				for (auto& vertex : vertices)
				{
					// Get input vector from vertex list and sample noise at different levels
					auto input = vertex + GetActorLocation();
					auto result1 = noise->GetNoise(input.X / 0.9, input.Y / 0.9, input.Z / 0.9);
					vertex.Z += result1 * 25;
					auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);
					vertex.Z += result2 * 10;

					if (vertex.Z == 0)
					{
						vertex.Z += result1 * 25;
						vertex.Z += result2 * 10;
					}
					else
					{
						vertex.X += result1 * 25;
						vertex.X += result2 * 10;
						
						vertex.Y += result1 * 25;
						vertex.Y += result2 * 10;
					}
				}

				normals.Init({ 0,0,0 }, vertices.Num());
				room->CalculateNormals(normals,vertices,triangles);

				WallMesh->CreateMeshSection(index, vertices, triangles, normals, UVs, VertexColours, Tangents, true);
				WallMesh->SetMaterial(index, Material);
				index++;

			}
		}							
	}
}

void ARoomManager::DrawDebugConnections()
{
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());

	// iterate over each room and add its connections to the arrays
	for (auto& room : Rooms)
	{	
		for (auto& connection : room->Connections)
		{
			DrawDebugLine(GetWorld(), room->GetActorLocation(), connection->GetActorLocation(), FColor{ 255,0,255 }, true, 5.0f, 0, 10.0f);
		}
	}
}
