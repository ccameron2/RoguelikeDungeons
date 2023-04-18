// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/PointLightComponent.h" 

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Campfire.generated.h"

UCLASS()
class GAMESSIX_API ACampfire : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACampfire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UPointLightComponent* PointLight;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* CampfireMesh;

	UPROPERTY(EditAnywhere)
		float LightDisableDistance = 3500;
};
