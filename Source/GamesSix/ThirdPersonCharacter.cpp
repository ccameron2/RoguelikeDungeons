// Fill out your copyright notice in the Description page of Project Settings.
#include "ThirdPersonCharacter.h"
#include "GameFramework/PawnMovementComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create spring arm and attach to the root component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	// Spring Arm settings
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	SpringArm->SetRelativeLocation(FVector(-361.0f, -305.0f, 113.0f));
	SpringArm->SetupAttachment(RootComponent);

	// Create third person camera and attach to SpringArm
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Third Person Camera"));
	ThirdPersonCamera->SetupAttachment(SpringArm);

	// Create first person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), "head");
	FirstPersonCamera->SetRelativeRotation(FRotator(0, 0, 0));
	FirstPersonCamera->SetRelativeLocation(FVector{ 0,10,0 });
	FirstPersonCamera->bUsePawnControlRotation = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Sphere"));
	SphereComponent->SetSphereRadius(50.0f);
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->SetupAttachment(GetMesh());

	//Possess player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	ThirdPersonCamera->SetActive(true);
	FirstPersonCamera->SetActive(false);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AThirdPersonCharacter::OnOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AThirdPersonCharacter::OnOverlapEnd);
}

// Called every frame
void AThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If health depleted
	if (HealthPoints <= 0)
	{
		if (Dead) return;
		GetCharacterMovement()->DisableMovement();

		// Start death timer
		GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &AThirdPersonCharacter::DeathComplete, 2.0f, false);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
		Dead = true;
	}

	// Regenerate energy for jump
	if(Energy < MaxEnergy){ Energy++; }
	
	// Track XP for level up
	if (ExpPoints >= MaxExp) { LevelUp(); }

	OverlappingEnemy();
}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AThirdPersonCharacter::OnConstruction(const FTransform& Transform)
{
	FirstPersonCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "head");
}

void AThirdPersonCharacter::MoveForward(float AxisValue)
{
	//Move character forwards
	if (Controller != nullptr && AxisValue != 0)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if(!Walking) AddMovementInput(Direction, AxisValue);
		else AddMovementInput(Direction, AxisValue / 2);
	}
}

void AThirdPersonCharacter::Strafe(float AxisValue)
{
	//Strafe character left and right
	if (Controller != nullptr && AxisValue != 0)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		if (!Walking) AddMovementInput(Direction, AxisValue);
		else AddMovementInput(Direction, AxisValue / 2);
	}
}

void AThirdPersonCharacter::Turn(float AxisValue)
{
	//Turn character left and right
	AddControllerYawInput(AxisValue);
}

void AThirdPersonCharacter::LookUp(float AxisValue)
{
	//Rotate view up and down
	AddControllerPitchInput(AxisValue);
}

void AThirdPersonCharacter::Attack()
{
	if (!SpamPrevention)
	{
		// Start timer to prevent spamming attack
		GetWorld()->GetTimerManager().SetTimer(SpamPreventionTimer, this, &AThirdPersonCharacter::EndSpamPrevention, 1.0f, false);
		SpamPrevention = true;

		if (!IsAttacking)
		{
			// Set character to attacking and start cooldown timer
			IsAttacking = true;
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation());
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AThirdPersonCharacter::EndAttack, AttackTime, false);
		}
	}
}

void AThirdPersonCharacter::Jump()
{
	Super::Jump();
	auto MovementComponent = GetMovementComponent();

	if (Energy > 50 && !MovementComponent->IsFalling())
	{
		Energy -= 75;
	}	
}

void AThirdPersonCharacter::SwapCamera()
{
	ThirdPersonCamera->ToggleActive();
	FirstPersonCamera->ToggleActive();
}

void AThirdPersonCharacter::ToggleSprint()
{
	Walking = !Walking;
}

void AThirdPersonCharacter::EndAttack()
{
	AttackTimer.Invalidate();
	IsAttacking = false;
}

void AThirdPersonCharacter::EndSpamPrevention()
{
	SpamPrevention = false;
}

void AThirdPersonCharacter::GetGeneratedTorches()
{
	TArray<AActor*> torchActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TorchClass, torchActors);

	for (auto torch : torchActors)
	{
		Torches.Push(Cast<ATorch>(torch));
	}
}

void AThirdPersonCharacter::LevelUp()
{
	// Increase stats on level up
	Level++; 
	ExpPoints = 0.0f; 
	MaxExp += 5;
	MaxHealth += 5;
	Damage += 5;
	LevelWon = false;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), LevelUpSound, GetActorLocation());
}

void AThirdPersonCharacter::DeathComplete()
{
	Destroy();
	UGameplayStatics::OpenLevel(GetWorld(), FName("TitleScreen"));
}

float AThirdPersonCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Take Damage"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSound, GetActorLocation());
	HealthPoints -= DamageAmount;
	return 0.0f;
}

void AThirdPersonCharacter::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{		

	}
}

void AThirdPersonCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		AResourcePickup* resourcePickup = Cast<AResourcePickup>(OtherActor);
		if (resourcePickup)
		{
			if (resourcePickup->Type == 0) { if(HealthPoints < MaxHealth ){ HealthPoints++; }}
			if (resourcePickup->Type == 1) { if(ExpPoints	 < MaxExp	 ){ ExpPoints++; }}
			OtherActor->Destroy();
		}
		AEnemyCharacter* enemyCharacter = Cast<AEnemyCharacter>(OtherActor);
		if (enemyCharacter)
		{
			IsOverlapping = true;
			if(!CurrOverlappedEnemies.Contains(enemyCharacter)) CurrOverlappedEnemies.Push(enemyCharacter);
		}
	}
}

void AThirdPersonCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		IsOverlapping = false;
		auto enemy = Cast<AEnemyCharacter>(OtherActor);
		CurrOverlappedEnemies.Remove(enemy);
	}
}

void AThirdPersonCharacter::OverlappingEnemy()
{
	if (IsOverlapping)
	{
		if (IsAttacking)
		{
			if (OnCooldown) return;
			FDamageEvent DamageEvent;
			if (!CurrOverlappedEnemies.IsEmpty()) 
			{
				for (auto& enemy : CurrOverlappedEnemies)
				{
					enemy->TakeDamage(Damage, DamageEvent, GetController(), this);
				}
			}
			GetWorld()->GetTimerManager().SetTimer(DamageCooldownTimer, this, &AThirdPersonCharacter::DamageCooldown, DamageCooldownTime, false);
			OnCooldown = true;
		}
	}
}

void AThirdPersonCharacter::DamageCooldown()
{
	OnCooldown = false;
}
