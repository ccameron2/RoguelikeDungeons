// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Room.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ARoom> RoomClass;

	TArray<ARoom*> Rooms;

	void DrawDebugConnections();

	int MaxRecursionLevel = 5;
	void SpawnNewRooms(ARoom* node, int level, FastNoise* noise);

	UFUNCTION(CallInEditor)
	void MakeNewLevel();

	void MakeWalls(FastNoise* noise);

};
