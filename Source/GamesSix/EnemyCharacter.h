// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimMontage.h" 

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

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		float HealthPoints = 100.0f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
		float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool PlayerVisibility = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool PlayerInRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimMontage* AttackAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimMontage* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Attacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackDistance = 200.0f;

	FTimerHandle AttackTimer;
	bool AttackTimerStarted = false;

	FTimerHandle DeathTimer;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool Dead = false;
};
