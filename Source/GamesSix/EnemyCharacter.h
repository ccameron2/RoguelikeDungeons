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

	// Attack timer finished
	UFUNCTION()
	void AttackComplete();

	// Death timer finished
	UFUNCTION()
	void DeathComplete();

	// Overlap Delegates
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Current health
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		float HealthPoints = 100.0f;

	// Resource class to drop on death
	UPROPERTY(EditAnywhere)
		TSubclassOf<AResourcePickup> ResourceClass;

	// Num resources to drop
	UPROPERTY(EditAnywhere)
		int NumResourcesDropped = 50;

	// Max health
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
		float MaxHealth = 100.0f;

	// Is player visible
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool PlayerVisibility = false;

	// Is player in range
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool PlayerInRange = false;

	// Attack overlap sphere
	UPROPERTY(EditAnywhere)
		USphereComponent* AttackSphere;

	// Is attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Attacking = false;

	// Damage dealt with an attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackDamage = 10.0f;

	// Distance from player to start attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackDistance = 150.0f;

	// Distance to be knocked back on hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float KnockbackDistance = 80.0f;

	// Unused Sounds
	UPROPERTY(EditAnywhere)
		USoundCue* DamageSound;

	UPROPERTY(EditAnywhere)
		USoundCue* DeathSound;

	UPROPERTY(EditAnywhere)
		USoundCue* AttackSound;

	// Enemy currently overlapping
	AActor* CurrOverlappedEnemy = nullptr;
	bool IsOverlapping = false;
	bool OnCooldown = false;

	// Timers
	FTimerHandle AttackTimer;
	bool AttackTimerStarted = false;
	FTimerHandle DeathTimer;

	// Is dead
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool Dead = false;
};
