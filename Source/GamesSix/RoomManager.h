// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Room.h"
#include "EnemyCharacter.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomManager.generated.h"

UCLASS()
class GAMESSIX_API ARoomManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Destroyed() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Room class to spawn
	UPROPERTY(EditAnywhere)
		TSubclassOf<ARoom> RoomClass;
	
	// Enemy class to spawn
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AEnemyCharacter>> EnemyClasses;

	// Array of rooms and characters
	TArray<ARoom*> Rooms;
	TArray<AEnemyCharacter*> EnemyCharacters;

	// Draw debug connections between rooms to show generation
	void DrawDebugConnections();

	// Spawn enemies in rooms
	void SpawnEnemies(TArray<ARoom*> rooms);

	// Find and save furthest room
	ARoom* FurthestRoom;
	void FindFurthestRoom();

	// Max recursions to use when placing rooms
	int MaxRecursionLevel = 12;

	// Place new rooms in the world
	void SpawnNewRooms(ARoom* node, int level, FastNoise* noise);

	UFUNCTION(CallInEditor)
	void MakeNewLevel();

	// Call make walls on all rooms
	void MakeWalls(FastNoise* noise);

};
