// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/PointLightComponent.h" 
#include "Components/SphereComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Torch.generated.h"

UCLASS()
class GAMESSIX_API ATorch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATorch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UPointLightComponent* PointLight;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* TorchMesh;

	AActor* PlayerPawn;

	UPROPERTY(EditAnywhere)
		float LightDisableDistance = 3000;
};
