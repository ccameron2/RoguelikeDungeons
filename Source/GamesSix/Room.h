// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <ProceduralMeshComponent.h>
#include "KismetProceduralMeshLibrary.h"
#include "External/FastNoise.h"

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
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Geometry")
		UProceduralMeshComponent* ProcMesh;

	UPROPERTY(VisibleAnywhere, Category = "Geometry")
		UMaterialInstance* Material;

	UPROPERTY(EditAnywhere, Category = "Geometry")
		int SizeX = 25;

	UPROPERTY(EditAnywhere, Category = "Geometry")
		int SizeY = 25;

	UPROPERTY(EditAnywhere, Category = "Geometry")
		int WallSizeZ = 20;

	UPROPERTY(EditAnywhere, Category = "Geometry")
		int Scale = 30;
	
	UPROPERTY(VisibleAnywhere)
		int ID = 0;

	TArray<ARoom*> Connections;

	UFUNCTION(CallInEditor)
		void GenerateRandomMesh();

	void MakeWalls(FastNoise* noise);

	void GenerateMesh(FastNoise* noise);

	enum Direction
	{
		North,
		East,
		South,
		West
	};
	TArray<Direction> UsedDirections;
	void GenerateTriangles(int SizeX, int SizeY, TArray<int32>& Triangles);
	void CalculateNormals(TArray<FVector>& normals, TArray<FVector> vertices, TArray<int32> triangles);
private:
	void CreateFloor(FastNoise* noise);
	void CreateTop(FastNoise* noise);

	bool MeshGenerated = false;
	int WallHeight = 10;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColours;
	TArray<FProcMeshTangent> Tangents;

	TArray<FVector> TopVertices;
	TArray<int32> TopTriangles;
	TArray<FVector> TopNormals;

	TArray<FVector> WallVertices;
	TArray<int32> WallTriangles;
	TArray<FVector> WallNormals;
};