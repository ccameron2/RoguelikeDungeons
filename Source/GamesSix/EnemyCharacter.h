// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h" 
#include "ResourcePickup.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class GAMESSIX_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	UFUNCTION()
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void OverlappingEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void AttackComplete();

	UFUNCTION()
	void DeathComplete();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		float HealthPoints = 100.0f;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AResourcePickup> ResourceClass;

	UPROPERTY(EditAnywhere)
		int NumResourcesDropped = 50;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
		float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool PlayerVisibility = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool PlayerInRange = false;

	UPROPERTY(EditAnywhere)
		USphereComponent* AttackSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Attacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackDistance = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float KnockbackDistance = 80.0f;

	UPROPERTY(EditAnywhere)
		USoundCue* DamageSound;

	UPROPERTY(EditAnywhere)
		USoundCue* DeathSound;

	UPROPERTY(EditAnywhere)
		USoundCue* AttackSound;

	AActor* CurrOverlappedEnemy = nullptr;
	bool IsOverlapping = false;
	bool OnCooldown = false;

	FTimerHandle AttackTimer;
	bool AttackTimerStarted = false;

	FTimerHandle DeathTimer;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool Dead = false;
};
