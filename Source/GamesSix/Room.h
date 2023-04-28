// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <ProceduralMeshComponent.h>
#include "KismetProceduralMeshLibrary.h"
#include "Components/InstancedStaticMeshComponent.h" 
#include "External/FastNoise.h"
#include "InteractableChest.h"
#include "Archway.h"

#include "Torch.h"
#include "Campfire.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

UCLASS()
class GAMESSIX_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Destroyed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Geometry")
		UProceduralMeshComponent* ProcMesh;

	UPROPERTY(VisibleAnywhere, Category = "Geometry")
		UMaterialInstance* Material;

	UPROPERTY(EditAnywhere, Category = "Geometry")
		float SizeX = 25;

	UPROPERTY(EditAnywhere, Category = "Geometry")
		float SizeY = 15;

	UPROPERTY(EditAnywhere, Category = "Geometry")
		int WallSizeZ = 20;

	UPROPERTY(EditAnywhere, Category = "Geometry")
		int Scale = 40;
	
	UPROPERTY(VisibleAnywhere)
		int ID = 0;

	UPROPERTY(VisibleAnywhere)
		int NumWalls = 0;

	// Actor classes to spawn
	UPROPERTY(EditAnywhere)
		TSubclassOf<ATorch> TorchClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<ACampfire> CampfireClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AArchway> ArchwayClass;

	// Archway actor
	AArchway* Archway;

	// Chest actors
	UPROPERTY(EditAnywhere)
		TSubclassOf<AInteractableChest> ChestClass;

	// Room object meshes
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* PillarMesh;
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* CobwebMesh;
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* TableMesh;	
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* BigTableMesh;
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* BarrelMesh;
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* ChairMesh;
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* HorseMesh;
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* HorsePedestalMesh;

	// Torch actors
	UPROPERTY(EditAnywhere)
		TArray<ATorch*> Torches;

	// Fire actors
	UPROPERTY(EditAnywhere)
		TArray<ACampfire*> Campfires;

	// Chest actors
	UPROPERTY(EditAnywhere)
		TArray<AInteractableChest*> Chests;

	TArray<ARoom*> Connections;

	UFUNCTION(CallInEditor)
		void GenerateRandomMesh();

	enum Direction
	{
		North,
		East,
		South,
		West
	};

	// Generate room and walls
	void MakeWalls(FastNoise* noise);
	void GenerateMesh(FastNoise* noise);

	void PlaceObjects();
	void PlaceCobwebs();

	// Load meshes
	void LoadMeshes();

	// Spawn archway in final room
	void SpawnNextArchway();

	// Geometry functions
	void GenerateTriangles(int SizeX, int SizeY, TArray<int32>& Triangles);
	void CalculateNormals(TArray<FVector>& normals, TArray<FVector> vertices, TArray<int32> triangles);

	// Get the vertex on the side of the room that doesnt have a wall
	Direction GetUnusedNSVertex(FVector& vertex);

	// Array of directions that have walls
	TArray<Direction> UsedDirections;

private:
	// Create floor and top
	void CreateFloor(FastNoise* noise);
	void CreateTop(FastNoise* noise);
	
	// Place static mesh pillars on corners
	void PlacePillars();

	// Has mesh been generated
	bool MeshGenerated = false;

	// Height of the walls
	int WallHeight = 11;

	// Room geometry
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColours;
	TArray<FProcMeshTangent> Tangents;

	// Roof geometry
	TArray<FVector> TopVertices;
	TArray<int32> TopTriangles;
	TArray<FVector> TopNormals;

	// Wall geometry
	TArray<FVector> WallVertices;
	TArray<int32> WallTriangles;
	TArray<FVector> WallNormals;

};
