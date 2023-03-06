// Fill out your copyright notice in the Description page of Project Settings.
#include "Room.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Room Mesh"));
	SetRootComponent(ProcMesh);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> forestMaterial(TEXT("M_Material'/Game/Materials/M_Material.M_Material'"));
	Material = forestMaterial.Object;

}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
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
	ProcMesh->ClearMeshSection(0);
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
			Vertices[SizeX * indexX + indexY] = newVector;
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
		Vertices[i].Z += result1 * 15;
		auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);
		Vertices[i].Z += result2 * 5;
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
			TopVertices[SizeX * indexX + indexY] = newVector;
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
		TopVertices[i].Z += result1 * 15;
		auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);
		TopVertices[i].Z += result2 * 5;
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
