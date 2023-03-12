// Fill out your copyright notice in the Description page of Project Settings.
#include "Room.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Room Mesh"));
	SetRootComponent(ProcMesh);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> material(TEXT("M_Material'/Game/Materials/M_Material.M_Material'"));
	Material = material.Object;
}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
}

void ARoom::OnConstruction(const FTransform& Transform)
{

}


void ARoom::Destroyed()
{
	Super::Destroyed();
	if (!Torches.IsEmpty())
	{
		for (auto torch : Torches)
		{
			torch->Destroy();
		}
		Torches.Empty();
	}
}
// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARoom::GenerateMesh(FastNoise* noise)
{

	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	VertexColours.Empty();
	Tangents.Empty();
	ProcMesh->ClearAllMeshSections();

	//SizeX = FMath::RandRange(5, 25);
	//SizeY = FMath::RandRange(5, 25);

	CreateFloor(noise);
	CreateTop(noise);
}

void ARoom::CreateFloor(FastNoise* noise)
{
	Vertices.Init({ 0,0,0 }, SizeX * SizeY);

	// Create a grid of vertices
	int indexX = 0;
	for (int i = 0; i < SizeX; i++)
	{
		int indexY = 0;
		for (int j = 0; j < SizeY; j++)
		{
			FVector newVector = FVector{ float(i * Scale), float(j * Scale),0 };
			Vertices[SizeY * indexX + indexY] = newVector;
			indexY++;
		}
		indexX++;
	}

	// Generate triangles for grid of vertices
	GenerateTriangles(SizeX, SizeY, Triangles);

	for (auto& vertex : Vertices)
	{
		vertex.X -= (SizeX * Scale) / 2;
		vertex.Y -= (SizeY * Scale) / 2;
	}

	// For each vertex, get 2 different noise values and apply them to vertex hight at different scales.
	for (int i = 0; i < Vertices.Num(); i++)
	{
		// Get input vector from vertex list and sample noise at different levels
		auto input = Vertices[i] + GetActorLocation();
		auto result1 = noise->GetNoise(input.X / 0.9, input.Y / 0.9, input.Z / 0.9);
		Vertices[i].Z += result1 * 8;
		auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);
		Vertices[i].Z += result2 * 2;
	}

	Normals.Init({ 0,0,0 }, Vertices.Num());

	// Faster normals
	CalculateNormals(Normals,Vertices,Triangles);

	// Create mesh section
	ProcMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColours, Tangents, true);
	ProcMesh->SetMaterial(0, Material);
}

void ARoom::CreateTop(FastNoise* noise)
{
	TopVertices.Init({ 0,0,0 }, SizeX * SizeY);

	// Create a grid of vertices
	int indexX = 0;
	for (int i = 0; i < SizeX; i++)
	{
		int indexY = 0;
		for (int j = 0; j < SizeY; j++)
		{
			FVector newVector = FVector{ float(i * Scale), float(j * Scale),0 };
			TopVertices[SizeY * indexX + indexY] = newVector;
			indexY++;
		}
		indexX++;
	}

	// Generate triangles for grid of vertices
	UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeX, SizeY, true, TopTriangles);

	for (auto& vertex : TopVertices)
	{
		vertex.X -= (SizeX * Scale) / 2;
		vertex.Y -= (SizeY * Scale) / 2;
		vertex.Z += ((WallSizeZ * Scale) / 2)-Scale;
	}

	// For each vertex, get 2 different noise values and apply them to vertex hight at different scales.
	for (int i = 0; i < TopVertices.Num(); i++)
	{
		// Get input vector from vertex list and sample noise at different levels
		auto input = TopVertices[i] + GetActorLocation();
		auto result1 = noise->GetNoise(input.X / 0.9, input.Y / 0.9, input.Z / 0.9);
		TopVertices[i].Z += result1 * 8;
		auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);
		TopVertices[i].Z += result2 * 2;
	}

	TopNormals.Init({ 0,0,0 }, TopVertices.Num());

	// Faster normals
	CalculateNormals(TopNormals, TopVertices, TopTriangles);

	// Create mesh section
	ProcMesh->CreateMeshSection(1, TopVertices, TopTriangles, TopNormals, UVs, VertexColours, Tangents, true);
	ProcMesh->SetMaterial(1, Material);
}

void ARoom::GenerateTriangles(int sizeX, int sizeY, TArray<int32>& triangles)
{
	const int NumTriangles = 2 * (sizeX - 1) * (sizeY - 1);
	triangles.SetNum(NumTriangles * 3);

	int TriangleIndex = 0;
	for (int i = 0; i < sizeX - 1; i++)
	{
		for (int j = 0; j < sizeY - 1; j++)
		{
			int VertexIndex = i * sizeY + j;

			// Create two triangles for each grid cell
			triangles[TriangleIndex++] = VertexIndex;
			triangles[TriangleIndex++] = VertexIndex + 1;
			triangles[TriangleIndex++] = VertexIndex + sizeY;
			
			triangles[TriangleIndex++] = VertexIndex + sizeY;
			triangles[TriangleIndex++] = VertexIndex + 1;
			triangles[TriangleIndex++] = VertexIndex + sizeY + 1;
		}
	}
}

void ARoom::GenerateRandomMesh()
{
	FastNoise noise;
	noise.SetNoiseType(FastNoise::SimplexFractal);
	noise.SetSeed(FMath::Rand());
	GenerateMesh(&noise);
}

void ARoom::MakeWalls(FastNoise* noise)
{
	FTransform transform;
	int index = 2;
	for (int i = 0; i < 4; i++)
	{
		auto direction = static_cast<Direction>(i);
		if (!UsedDirections.Contains(direction))
		{
			FVector location = GetActorLocation();
			TArray<FVector> vertices;
			TArray<FVector> normals;
			TArray<int32> triangles;

			switch (direction)
			{
			case Direction::North:
				for (int j = 0; j < SizeX; j++)
				{
					for (int k = 0; k < WallHeight; k++)
					{
						vertices.Push(FVector{ float(j * Scale),0,float(k * Scale) });
					}
				}
				for (auto& vertex : vertices)
				{
					vertex.X -= (SizeX * Scale) / 2;
					vertex.Y += (SizeY * Scale / 2) - Scale;
				}
				UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeX, WallHeight, false, triangles);

				transform.SetLocation((GetActorLocation() + FVector{ 0,float((SizeY * Scale / 2) - 2 * Scale),float(WallSizeZ * Scale) / 2 - 4 * Scale }));
				transform.SetRotation(FVector{ 0,0,0 }.ToOrientationQuat());
				break;
			case Direction::South:
				for (int j = 0; j < SizeX; j++)
				{
					for (int k = 0; k < WallHeight; k++)
					{
						vertices.Push(FVector{ float(j * Scale),0,float(k * Scale) });
					}
				}
				for (auto& vertex : vertices)
				{
					vertex.X -= (SizeX * Scale) / 2;
					vertex.Y -= (SizeY * Scale / 2);
				}
				UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeX, WallHeight, true, triangles);
				transform.SetLocation(GetActorLocation() + FVector{ 0,float(-SizeY * Scale + Scale) / 2,float(WallSizeZ * Scale) / 2 - 4 * Scale });
				transform.SetRotation(FVector{ 0,0,0 }.ToOrientationQuat());
				break;
			case Direction::East:
				for (int j = 0; j < SizeY; j++)
				{
					for (int k = 0; k < WallHeight; k++)
					{
						vertices.Push(FVector{ 0,float(j * Scale),float(k * Scale) });
					}
				}
				for (auto& vertex : vertices)
				{
					vertex.X += (SizeX * Scale / 2) - Scale;
					vertex.Y -= (SizeY * Scale / 2);
				}
				UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeY, WallHeight, true, triangles);
				transform.SetLocation((GetActorLocation() + FVector{ float((SizeX * Scale / 2) - 2 * Scale),0,float(WallSizeZ * Scale) / 2 - 4 * Scale }));
				transform.SetRotation(FVector{ 0,0,0 }.ToOrientationQuat());
				break;
			case Direction::West:
				for (int j = 0; j < SizeY; j++)
				{
					for (int k = 0; k < WallHeight; k++)
					{
						vertices.Push(FVector{ 0,float(j * Scale),float(k * Scale) });
					}
				}
				for (auto& vertex : vertices)
				{
					vertex.X -= (SizeX * Scale) / 2;
					vertex.Y -= (SizeY * Scale / 2);
				}
				UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeY, WallHeight, false, triangles);
				transform.SetLocation((GetActorLocation() + FVector{ float(( -SizeX * Scale  / 2) + 2 * Scale) ,0,float(WallSizeZ * Scale) / 2 - 4 * Scale}));
				transform.SetRotation(FVector{ 0,90,0 }.ToOrientationQuat());
				break;
			}

			// For each vertex, get 2 different noise values and apply them to vertex hight at different scales.
			for (auto& vertex : vertices)
			{
				// Get input vector from vertex list and sample noise at different levels
				auto input = vertex + GetActorLocation();
				auto result1 = noise->GetNoise(input.X / 0.9, input.Y / 0.9, input.Z / 0.9);
				auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);

				if (vertex.Z == 0)
				{
					vertex.Z += result1 * 8;
					vertex.Z += result2 * 2;
				}
				else if (vertex.Z == WallHeight * Scale - Scale)
				{
					vertex.Z += result1 * 8;
					vertex.Z += result2 * 2;
				}			 
				else
				{
					vertex.Y += result1 * 15;
					vertex.Y += result2 * 5;
					vertex.X += result1 * 15;
					vertex.X += result2 * 5;							
				}
			}

			normals.Init({ 0,0,0 }, vertices.Num());
			CalculateNormals(normals, vertices, triangles);

			ProcMesh->CreateMeshSection(index, vertices, triangles, normals, UVs, VertexColours, Tangents, true);
			ProcMesh->SetMaterial(index, Material);

			// Place torches randomly
			if(Torches.IsEmpty())
			{
				auto torch = GetWorld()->SpawnActor<ATorch>(TorchClass, transform);
				torch->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				torch->SetOwner(this);
				Torches.Push(torch);
			}
			/*if (Torches.Num() < 2)
			{
				if (FMath::RandRange(0, 6) == 0)
				{
					auto torch = GetWorld()->SpawnActor<ATorch>(TorchClass, transform);
					torch->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					torch->SetOwner(this);
					Torches.Push(torch);
				}
			}*/
		}
		index++;
	}
}

void ARoom::CalculateNormals(TArray<FVector>& normals,TArray<FVector> vertices, TArray<int32> triangles)
{
	for (int i = 0; i < triangles.Num() - 3; i += 3)
	{
		auto a = vertices[triangles[i]];
		auto b = vertices[triangles[i + 1]];
		auto c = vertices[triangles[i + 2]];

		auto v1 = a - b;
		auto v2 = c - b;
		auto n = v1 ^ v2;
		n.Normalize();

		normals[triangles[i]] += n;
		normals[triangles[i + 1]] += n;
		normals[triangles[i + 2]] += n;
	}

	for (auto& normal : normals)
	{
		normal.Normalize();
	}
}
