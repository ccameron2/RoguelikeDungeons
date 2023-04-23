// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Sphere"));
	AttackSphere->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	HealthPoints = MaxHealth;
	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnOverlapBegin);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnOverlapEnd);
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
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());

		Dead = true;
	}

	
	auto playerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	auto distance = 0.0f;
	if (playerPawn) 
	{
		distance = FVector::Distance(GetActorLocation(), playerPawn->GetActorLocation());

		// Set up the raycast parameters
		FVector rayStart = GetActorLocation();
		FVector rayEnd = playerPawn->GetActorLocation();
		const FCollisionQueryParams RayParams = FCollisionQueryParams::DefaultQueryParam;

		// Perform the raycast and get the hit result
		FHitResult hitResult;
		const bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, rayStart, rayEnd, ECC_Visibility, RayParams);

		if (bHit)
		{
			if (hitResult.GetActor() == playerPawn)
			{
				PlayerVisibility = true;
			}
			else
			{
				PlayerVisibility = false;
			}
		}



	} 

	if (distance < AttackDistance) Attacking = true;
	else { Attacking = false; }

	OverlappingEnemy();

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AttackComplete()
{
	Attacking = false;
	OnCooldown = false;
	AttackTimerStarted = false;
}

void AEnemyCharacter::DeathComplete()
{
	Destroy();
}

void AEnemyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor)
		{
			FString enemyName = OtherActor->GetName();
			auto playerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
			FString playerName;
			if(playerPawn) playerName = playerPawn->GetName();
			if (enemyName == playerName)
			{
				Attacking = true;
				IsOverlapping = true;
				CurrOverlappedEnemy = OtherActor;
			}
		}
	}
}

void AEnemyCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//Attacking = false;
		IsOverlapping = false;
		CurrOverlappedEnemy = nullptr;
	}
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Take Damage"));

	auto damageCauserPos = DamageCauser->GetActorLocation();
	auto pos = GetActorLocation();
	// Subtract the damage-causer's position from the damage position to get the relative position
	FVector relativePos = pos - damageCauserPos;

	// Normalize the relative position vector to get the direction vector
	float length = relativePos.Length();
	FVector direction = { relativePos / length };

	// Scale the direction vector by the distance
	FVector moveOffset = { direction.X * KnockbackDistance,direction.Y * KnockbackDistance,0.0f };

	// Move the actor away from the damage
	SetActorLocation(pos + moveOffset,true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSound, GetActorLocation());

	HealthPoints -= DamageAmount;
	return 0.0f;
}

void AEnemyCharacter::OverlappingEnemy()
{
	if (IsOverlapping)
	{
		if (Attacking)
		{
			if (OnCooldown) return;
			FDamageEvent DamageEvent;
			if (CurrOverlappedEnemy)
			{
				CurrOverlappedEnemy->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
				
			}
			if (!AttackTimerStarted)
			{
				GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEnemyCharacter::AttackComplete, 1.0f, false);
				AttackTimerStarted = true;
				OnCooldown = true;
			}
			
		}
	}
}