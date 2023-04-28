// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "ThirdPersonCharacter.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableChest.generated.h"

UCLASS()
class GAMESSIX_API AInteractableChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ClosedTopMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* OpenTopMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BottomMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* AttackMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LevelMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* HealthMesh;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ActiveMesh;

	UPROPERTY(EditAnywhere)
		float PickupLifetime = 2.0f;

	UFUNCTION()
		void ClearPickup();

	FTimerHandle TimerHandle;

	bool PickedUp = false;
	bool IsOpen = false;
};
