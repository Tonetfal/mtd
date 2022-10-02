#include "Characters/MTD_PlayerCharacter.h"

#include "Components/MTD_HealthComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AMTD_PlayerCharacter::AMTD_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->InitCapsuleSize(35.f, 80.f);
	GetCapsuleComponent()->SetCollisionProfileName(AllyCollisionProfileName);
	
	GetMesh()->SetCollisionProfileName("NoCollision");

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(
		TEXT("Spring Arm Component"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("Camera Component"));

	CameraBoom->SetupAttachment(GetRootComponent());
	FollowCamera->SetupAttachment(
		CameraBoom, USpringArmComponent::SocketName);

	CameraBoom->TargetArmLength = 550.f;
	CameraBoom->TargetOffset.Z = 150.f;
	FollowCamera->AddRelativeRotation(FRotator(0.f, -15.f, 0.f));
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera->bUsePawnControlRotation = false;

	// Camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	Weapon =
		CreateDefaultSubobject<UMTD_WeaponComponent>(
			TEXT("MTD Weapon Component"));
	
	Health =
		CreateDefaultSubobject<UMTD_HealthComponent>(
			TEXT("MTD Health Component"));
}

void AMTD_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(Weapon);
	check(Health);
	
	if (!IsValid(DeathAnimMontage))
	{
		MTD_WARN("%s does not have a death animation montage", *GetName());
	}
	
	InitDelegates();
}

void AMTD_PlayerCharacter::InitDelegates()
{
	Health->OnDeathDelegate.AddUObject(this, &AMTD_PlayerCharacter::OnDeath);
}

void AMTD_PlayerCharacter::SetupPlayerInputComponent(
	UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Mouse movements
	PlayerInputComponent->BindAxis(
		"Turn Right", this, &AMTD_PlayerCharacter::TurnRight);
	PlayerInputComponent->BindAxis(
		"Look Up", this, &AMTD_PlayerCharacter::LookUp);
	
	// Keyboard movements
	PlayerInputComponent->BindAxis(
		"Move Forward", this, &AMTD_PlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(
		"Move Right", this, &AMTD_PlayerCharacter::MoveRight);
	
	PlayerInputComponent->BindAction(
		"Jump", IE_Pressed, this, &AMTD_PlayerCharacter::Jump);
	PlayerInputComponent->BindAction(
		"Attack", IE_Pressed, this, &AMTD_PlayerCharacter::StartAttacking);
	PlayerInputComponent->BindAction(
		"Attack", IE_Released, this, &AMTD_PlayerCharacter::StopAttacking);
}

float AMTD_PlayerCharacter::ConsumeTurnRatio()
{
	const float Tmp = TurnRatio;
	bIsTurning = false;
	TurnRatio = 0.f;
	return Tmp;
}

float AMTD_PlayerCharacter::GetMovementDirectionAngle() const
{
	const FVector Velocity = GetCharacterMovement()->Velocity;
	if (Velocity.IsZero())
		return 0.f;

	const FVector ActorForward = GetActorForwardVector();
	const FVector MovVectorNorm = Velocity.GetSafeNormal();

	const float AngleBetween =
		FMath::Acos(FVector::DotProduct(ActorForward, MovVectorNorm));
	const FVector CrossProduct =
		FVector::CrossProduct(ActorForward, MovVectorNorm);
	const float Degrees = FMath::RadiansToDegrees(AngleBetween);

	return CrossProduct.IsZero() ?
		Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void AMTD_PlayerCharacter::LookUp(float Ratio)
{
	if (Ratio != 0.f)
	{
		// TODO: Store ratio to make look up/down animations
		AddControllerPitchInput(Ratio);
	}
}

void AMTD_PlayerCharacter::TurnRight(float Ratio)
{
	if (Ratio != 0.f)
	{
		bIsTurning = true;
		TurnRatio = Ratio;
		AddControllerYawInput(Ratio);
	}
}

void AMTD_PlayerCharacter::MoveForward(float Ratio)
{
	if (IsValid(Controller) && Ratio != 0.f)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// Get forward vector
		const FVector Direction =
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Ratio);
	}
}

void AMTD_PlayerCharacter::MoveRight(float Ratio)
{
	if (IsValid(Controller) && Ratio != 0.f)
	{
		// Find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// Get right vector
		const FVector Direction =
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Ratio);
	}
}

void AMTD_PlayerCharacter::StartAttacking()
{
	Weapon->StartAttacking();
}

void AMTD_PlayerCharacter::StopAttacking()
{
	Weapon->StopAttacking();
}

void AMTD_PlayerCharacter::OnDeath_Implementation()
{
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(
		ECollisionResponse::ECR_Ignore);
	
	PlayDeathAnimation();

	AController *ActorController = GetController();
	if (!IsValid(ActorController))
		return;
	
	APlayerController *Pc = Cast<APlayerController>(ActorController);
	if (IsValid(Pc))
	{
		DisableInput(Pc);
		Controller->ChangeState(NAME_Spectating);
	}
}

void AMTD_PlayerCharacter::PlayDeathAnimation() const
{
	if (!IsValid(DeathAnimMontage))
		return;
	
	UAnimInstance *MyAnimInstance = GetMesh()->GetAnimInstance();
	MyAnimInstance->Montage_Play(DeathAnimMontage);
}
