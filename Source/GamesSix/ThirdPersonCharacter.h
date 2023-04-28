// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "ResourcePickup.h"
#include "Torch.h"
#include "EnemyCharacter.h"
#include "Sound/SoundCue.h" 
#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

UCLASS()
class GAMESSIX_API AThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AThirdPersonCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void MoveForward(float AxisValue);
	void Strafe(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);
	void Attack();
	void Jump();
	void SwapCamera();
	void ToggleSprint();
	void EndAttack();
	void EndSpamPrevention();
	void GetGeneratedTorches();
	bool Walking = false;
	float AttackTime = 0.8f;
	FTimerHandle AttackTimer;
	FTimerHandle DamageCooldownTimer;
	FTimerHandle SpamPreventionTimer;
	bool SpamPrevention = false;

	UPROPERTY(EditAnywhere)
		float DamageCooldownTime = 1.0f;

	UPROPERTY(BlueprintReadWrite)
		bool IsRolling = false;

	UPROPERTY(BlueprintReadWrite)
		bool IsAttacking = false;

	UPROPERTY(EditAnywhere)
		float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere)
		float RotationSpeed = 500.0f;

	UPROPERTY(EditAnywhere)
		float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere)
		float MaxEnergy = 100.0f;

	UPROPERTY(EditAnywhere)
		float MaxExp = 100.0f;

	UPROPERTY(EditAnywhere)
		float MaxMana = 100.0f;

	UPROPERTY(EditAnywhere)
		float MaxGold = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool Dead = false;

	UFUNCTION(BlueprintCallable)
		float GetMaxHealth() { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
		float GetMaxEnergy() { return MaxEnergy; }

	UFUNCTION(BlueprintCallable)
		float GetMaxMana() { return MaxMana; }

	UFUNCTION(BlueprintCallable)
		float GetMaxGold() { return MaxGold; }

	UFUNCTION(BlueprintCallable)
		float GetMaxExperience() { return MaxExp; }

	UFUNCTION(BlueprintCallable)
		float GetHealth() { return HealthPoints; }

	UFUNCTION(BlueprintCallable)
		float GetEnergy() { return Energy; }

	UFUNCTION(BlueprintCallable)
		float GetExperience() { return ExpPoints; }

	UFUNCTION(BlueprintCallable)
		float GetLives() { return Lives; }

	UFUNCTION(BlueprintCallable)
		float GetPlayerLevel() { return Level; }

	UFUNCTION(BlueprintCallable)
		void LevelUp();

	UFUNCTION()
		void DeathComplete();

	UPROPERTY(VisibleAnywhere)
		bool LevelWon = false;

private:
	UPROPERTY(VisibleAnywhere)
		float Damage = 35.0f;

	UPROPERTY(VisibleAnywhere)
		float HealthPoints = MaxHealth;

	UPROPERTY(VisibleAnywhere)
		float ExpPoints = 0.0f;

	UPROPERTY(VisibleAnywhere)
		float Energy = MaxEnergy;

	UPROPERTY(VisibleAnywhere)
		int Level = 1;

	UPROPERTY(VisibleAnywhere)
		int Lives = 1;

	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
		void OverlappingEnemy();

	UFUNCTION()
		void DamageCooldown();

	bool OnCooldown = false;

	UPROPERTY(EditAnywhere)
		UCameraComponent* ThirdPersonCamera;

	UPROPERTY(EditAnywhere)
		UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ATorch> TorchClass;

	UPROPERTY(EditAnywhere)
		USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)
		USoundCue* DamageSound;

	UPROPERTY(EditAnywhere)
		USoundCue* DeathSound;

	UPROPERTY(EditAnywhere)
		USoundCue* AttackSound;

	UPROPERTY(EditAnywhere)
		USoundCue* LevelUpSound;

	UPROPERTY(EditAnywhere)
		USoundCue* PickupItemSound;

	FTimerHandle DeathTimer;

	TArray<ATorch*> Torches;

	bool IsOverlapping = false;
	TArray<AEnemyCharacter*> CurrOverlappedEnemies;
};
