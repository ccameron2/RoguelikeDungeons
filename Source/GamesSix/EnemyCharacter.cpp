// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	HealthPoints = MaxHealth;
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HealthPoints <= 0)
	{
		if (Dead) return;
		UE_LOG(LogTemp, Warning, TEXT("Enemy Dead"));
		GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &AEnemyCharacter::DeathComplete, 1.0f, false);
		Dead = true;
	}

	if (Attacking)
	{
		if (!AttackTimerStarted)
		{
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEnemyCharacter::AttackComplete, 1.0f, false);
		}
	}

	auto distance = FVector::Distance(GetActorLocation(), GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
	if (distance > HealthBarDisableDistance) PlayerVisibility = false;
	else PlayerVisibility = true;

	if (distance < AttackDistance) Attacking = true;
	else { Attacking = false; }

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AttackComplete()
{
	Attacking = false;
}

void AEnemyCharacter::DeathComplete()
{
	Destroy();
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Take Damage"));
	HealthPoints -= DamageAmount;
	return 0.0f;
}