#include "Characters/MTD_BaseEnemy.h"

#include "Characters/MTD_PlayerCharacter.h"
#include "Components/MTD_HealthComponent.h"
#include "Components/MTD_WeaponComponent.h"
#include "Components/MTD_PathFollowingComponent.h"
#include "Controllers/MTD_EnemyController.h"
#include "Utilities/MTD_Utilities.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AMTD_BaseEnemy::AMTD_BaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(35.f, 80.f);
	GetCapsuleComponent()->SetCollisionProfileName(EnemyCollisionProfileName);

	GetMesh()->SetCollisionProfileName("NoCollision");

	ACharacter::GetCharacterMovement()->bOrientRotationToMovement = true;
	ACharacter::GetCharacterMovement()->MaxWalkSpeed = 300.f;
	
	SightSphere =
		CreateDefaultSubobject<USphereComponent>(TEXT("Sight Sphere"));
	SightSphere->SetupAttachment(GetRootComponent());
	SightSphere->InitSphereRadius(100.f);

	SightSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SightSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SightSphere->SetCollisionResponseToAllChannels(
		ECollisionResponse::ECR_Ignore);
	SightSphere->SetCollisionResponseToChannel(
		AllyCollisionChannel, ECollisionResponse::ECR_Overlap);
	
	LoseSightSphere =
		CreateDefaultSubobject<USphereComponent>(TEXT("Lose Sight Sphere"));
	LoseSightSphere->SetupAttachment(GetRootComponent());
	LoseSightSphere->InitSphereRadius(150.f);
	
	LoseSightSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LoseSightSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LoseSightSphere->SetCollisionResponseToAllChannels(
		ECollisionResponse::ECR_Ignore);
	LoseSightSphere->SetCollisionResponseToChannel(
		AllyCollisionChannel, ECollisionResponse::ECR_Overlap);

	Health =
		CreateDefaultSubobject<UMTD_HealthComponent>(
			TEXT("MTD Health Component"));
	
	Weapon =
		CreateDefaultSubobject<UMTD_WeaponComponent>(
			TEXT("MTD Weapon Component"));
}

void AMTD_BaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	check(Health);
	check(Weapon);
	check(SightSphere);
	check(LoseSightSphere);

	if (!IsValid(Controller))
	{
		MTDS_WARN("Controller is invalid");
	}
	if (!IsValid(DeathAnimMontage))
	{
		MTDS_WARN("Death animation montage is invalid");
	}

	InitDelegates();
}

void AMTD_BaseEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(StopForceTargettingTimerHandle);
	GetWorldTimerManager().ClearTimer(TryChaneTargetTimerHandle);
}

void AMTD_BaseEnemy::InitDelegates()
{
	Health->OnDamagedDelegate.AddUObject(
		this, &AMTD_BaseEnemy::OnDamaged);
	Health->OnDeathDelegate.AddUObject(this, &AMTD_BaseEnemy::OnDeath);
	
	SightSphere->OnComponentBeginOverlap.AddDynamic(
		this, &AMTD_BaseEnemy::OnSightSphereBeginOverlap);
	LoseSightSphere->OnComponentEndOverlap.AddDynamic(
		this, &AMTD_BaseEnemy::OnLoseSightSphereEndOverlap);

	auto *EnemyController = Cast<AMTD_EnemyController>(Controller);
	if (IsValid(EnemyController))
	{
		auto *PathFollowingComp =
			FMTD_Utilities::GetActorComponent<UMTD_PathFollowingComponent>(
				EnemyController);
		if (IsValid(PathFollowingComp))
		{
			PathFollowingComp->OnDestinationDelegate.AddUObject(
				this, &AMTD_BaseEnemy::OnDestination);
		}
		else
		{
			MTDS_WARN("%s has an invalid path following component", 
				*Controller->GetName());
		}
	}
	else
	{
		MTDS_WARN("Failed to cast %s to AMTD_EnemyController",
			*Controller->GetName());
	}
}

void AMTD_BaseEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// We don't bother about the vision if we're forced to target
	if (bForceTarget)
		return;

	// Check out if we still see the target
	if (IsValid(AttackTarget))
	{
		// Keep targetting the seen actor
		if (SeesAttackTarget())
		{
			return; // Still see the target
		}
		else
		{
			// Target has left the range or is hidden by some other object, 
			// so we're going to search for a new target
			MTDS_VVERBOSE("%s is not seen anymore", *AttackTarget->GetName());
		}
	}

	// Clear target that has been invalidated since last frame
	CleanAttackTargets();

	// Look around and find a target
	AActor *NewAttackTarget = SelectAttackTarget();
	SetNewAttackTarget(NewAttackTarget);
}

void AMTD_BaseEnemy::OnDeath_Implementation()
{
	MTDS_VVERBOSE("Death");

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	
	PlayDeathAnimation();
}

int32 AMTD_BaseEnemy::CleanAttackTargets()
{
	// Check what's invalid and remove it from the list afterwards
	TArray<TObjectPtr<AActor>> ActorsToRemove;
	for (AActor *Item : DetectedTargets)
	{
		if (!IsValid(Item))
			ActorsToRemove.Add(Item);
	}

	for (AActor *Item : ActorsToRemove)
	{
		ActorsToRemove.Remove(Item);
	}
	
	return ActorsToRemove.Num();
}

AActor *AMTD_BaseEnemy::SelectAttackTarget()
{
	const UWorld *World = GetWorld();
	if (!IsValid(World))
		return nullptr;

	// Return the first visible target
	for (AActor *Item : DetectedTargets)
	{
		const FVector Location = Item->GetActorLocation();
		FHitResult HitResult = GetTraceResultByLookingAt(Location);
		
		if (HitResult.bBlockingHit &&
			HitResult.GetComponent()->GetCollisionObjectType() ==
				AllyCollisionChannel)
		{
			return Item;
		}
	}

	return nullptr;
}

FHitResult AMTD_BaseEnemy::GetTraceResultByLookingAt(
	FVector TargetLocation) const
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetPawnViewLocation(),
		TargetLocation,
		AllyTraceChannel,
		FCollisionQueryParams(TEXT(""), false, this));

	return HitResult;
}

bool AMTD_BaseEnemy::SeesAttackTarget() const
{
	if (!IsValid(AttackTarget))
		return false;
	
	const FVector Direction = GetDirectionToAttackTarget();
	const float Range = LoseSightSphere->GetScaledSphereRadius();
	const FHitResult HitResult =
		GetTraceResultByLookingAt(GetPawnViewLocation() + Direction * Range);
	
	return HitResult.bBlockingHit && HitResult.GetActor() == AttackTarget;
}

void AMTD_BaseEnemy::OnTargetNonReacheable()
{
	if (IsValid(AttackTarget))
		MTDS_WARN("%s cannot be reached", *AttackTarget->GetName());
	
	OnAttackTargetIgnore();
}

void AMTD_BaseEnemy::OnAttackTargetDied()
{
	if (IsValid(AttackTarget))
		MTDS_WARN("Attack target %s has died", *AttackTarget->GetName());
	
	SetNewAttackTarget(nullptr);

	// Cannot force target at a dead actor
	GetWorldTimerManager().ClearTimer(StopForceTargettingTimerHandle);
	OnStopForceTargetting();
}

void AMTD_BaseEnemy::OnAttackTargetIgnore(bool bOverrideAttackTarget)
{
	if (IsValid(AttackTarget))
	{
		MTDS_VVERBOSE("%s is being ignored", *AttackTarget->GetName());
		DetectedTargets.Remove(AttackTarget); // Ignore target
		IgnoredActor = AttackTarget;
	}
	
	if (bOverrideAttackTarget)
		SetNewAttackTarget(nullptr);
}

void AMTD_BaseEnemy::OnIgnoreTargetClear()
{
	if (IsValid(IgnoredActor))
		MTDS_VVERBOSE("%s is not ignored anymore", *IgnoredActor->GetName());
	IgnoredActor = nullptr;
}

void AMTD_BaseEnemy::OnSightSphereBeginOverlap(
	UPrimitiveComponent *OverlappedComponent,
	AActor *OtherActor,
	UPrimitiveComponent *OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult &SweepResult)
{
	if (OtherActor == IgnoredActor)
		return;

	if (DetectedTargets.Find(OtherActor) == INDEX_NONE)
	{
		DetectedTargets.Add(OtherActor);
		if (IsValid(OtherActor))
		{
			MTDS_VVERBOSE("%s entered into our vision", *OtherActor->GetName());
		}
	}
}

void AMTD_BaseEnemy::OnLoseSightSphereEndOverlap(
	UPrimitiveComponent *OverlappedComponent,
	AActor *OtherActor,
	UPrimitiveComponent *OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == IgnoredActor)
		OnIgnoreTargetClear();
	
	if (DetectedTargets.Remove(OtherActor) > 0)
	{
		if (IsValid(OtherActor))
			MTDS_VVERBOSE("%s left our vision", *OtherActor->GetName());
	}
}

void AMTD_BaseEnemy::OnDestination(bool bSuccess)
{
	// If we're stuck in a crowd then by trying to reach both waypoint and
	// an enemy, then we'll start ignoring an enemy (only one) to try go by him,
	// but if it isn't possible, we need to agro back on the enemy
	if (!bSuccess)
	{
		OnIgnoreTargetClear();
	}
}

void AMTD_BaseEnemy::OnDamaged(AActor *DamageCauser, int32 Damage)
{
	if (!IsValid(DamageCauser))
		return;
	
	MTDS_VVERBOSE("%s has dealt %d damage",
		*DamageCauser->GetName(), Damage);
	
	// TODO: Check if player is close enough navmesh wise, ignore if too far
	// ...

	const bool bIsDamagerIgnored = IgnoredActor == DamageCauser;
	if (bIsDamagerIgnored)
		OnIgnoreTargetClear();
		
	if (!GetWorldTimerManager().IsTimerActive(TryChaneTargetTimerHandle) &&
		(DamageCauser->IsA(AMTD_PlayerCharacter::StaticClass()) ||
		(IsValid(AttackTarget) && AttackTarget->IsA(AMTD_PlayerCharacter::StaticClass()))))
	{
		MTDS_WARN("Setup timer to try to change target");
		GetWorldTimerManager().SetTimer(
			TryChaneTargetTimerHandle,
			this,
			&AMTD_BaseEnemy::OnTryChangeTarget,
			SecondsToWaitBeforeTryingToChangeTarget);
	}
	LastDamageDealers.FindOrAdd(DamageCauser) += Damage;

	if (!bIsDamagerIgnored)
	{
		if (!IsValid(AttackTarget) || AttackTarget == DamageCauser)
		{
			SetNewAttackTarget(DamageCauser, true);
			return;
		}
		// Prioritize players over anything
		else if (DamageCauser->IsA(AMTD_PlayerCharacter::StaticClass()))
		{
			SetNewAttackTarget(DamageCauser, true);
			return;
		}
	}
}

void AMTD_BaseEnemy::SetNewAttackTarget(AActor *Actor, bool bForceNewTarget)
{
	if (Actor != AttackTarget) // Don't trigger much stuff if nothing changed
	{
		if (IsValid(AttackTarget))
		{
			auto *OtherHealth =
				FMTD_Utilities::GetActorComponent<UMTD_HealthComponent>(
					AttackTarget);
			
			if (IsValid(OtherHealth))
			{
				OtherHealth->OnDeathDelegate.Remove(TargetDiedDelegateHandle);
			}
		}

		AttackTarget = Actor;
		OnNewAttackTargetSpottedDelegate.Broadcast(AttackTarget);

		if (IsValid(AttackTarget))
		{
			MTDS_VVERBOSE("%s has become the new target", *Actor->GetName());
			
			auto *OtherHealth =
				FMTD_Utilities::GetActorComponent<UMTD_HealthComponent>(
					AttackTarget);
			
			if (IsValid(OtherHealth))
			{
				TargetDiedDelegateHandle =
					OtherHealth->OnDeathDelegate.AddUObject(
						this, &AMTD_BaseEnemy::OnAttackTargetDied);
			}
		}
		else
		{
			MTDS_VVERBOSE("No attack target");
		}
	}

	if (bForceNewTarget)
	{
		OnStartForceTargetting(SecondsToForceTarget);
	}


}

void AMTD_BaseEnemy::OnStartForceTargetting(float Duration)
{
	if (!IsValid(AttackTarget))
	{
		MTDS_ERROR("Attack target is invalid. Force targetting is impossible");
		return;
	}
	
	MTDS_VVERBOSE("Force target %s for %f", *AttackTarget->GetName(), Duration);
	bForceTarget = true;

	GetWorldTimerManager().SetTimer(
		StopForceTargettingTimerHandle,
		this,
		&AMTD_BaseEnemy::OnStopForceTargetting,
		Duration);
}

void AMTD_BaseEnemy::OnStopForceTargetting()
{
	MTDS_VVERBOSE("Force target effect has expired");
	bForceTarget = false;
}

AActor *FindMostDealingDamager(const TMap<TObjectPtr<AActor>, int32> Dealers)
{
	if (Dealers.IsEmpty())
		return nullptr;

	auto MostDealing = *Dealers.begin();
	for (auto &It : Dealers)
	{
		if (It.Value > MostDealing.Value)
			MostDealing = It;
	}
	return MostDealing.Key;
}

void AMTD_BaseEnemy::OnTryChangeTarget()
{
	MTD_WARN("Timer has been hit");
	if (LastDamageDealers.IsEmpty())
		return;

	if (!IsValid(AttackTarget) ||
		!AttackTarget.IsA(AMTD_PlayerCharacter::StaticClass()))
	{
		LastDamageDealers.Empty();
		return;
	}

	const int32 *Found = LastDamageDealers.Find(AttackTarget);
	AActor *MostDealing = FindMostDealingDamager(LastDamageDealers);
	LastDamageDealers.Empty();
	
	if (!Found)
	{
		MTDS_VVERBOSE("Player didn't deal any damage for sometime, %s will be "
			"targeted instead", MostDealing ?
			*MostDealing->GetName() : TEXT("no one"));
		
		OnAttackTargetIgnore(false);
		if (IsValid(MostDealing))
			SetNewAttackTarget(MostDealing, true);
		else
			SetNewAttackTarget(nullptr);
	}
	else if (MostDealing != AttackTarget)
	{
		MTDS_VVERBOSE("Player didn't deal the most amount of damage for "
			"sometime, %s will be targeted instead", *MostDealing->GetName());

		OnAttackTargetIgnore(false);
		SetNewAttackTarget(MostDealing, true);
	}
}

void AMTD_BaseEnemy::PlayDeathAnimation() const
{
	if (!IsValid(DeathAnimMontage))
		return;
	
	UAnimInstance *MyAnimInstance = GetMesh()->GetAnimInstance();
	MyAnimInstance->Montage_Play(DeathAnimMontage);
}

FVector AMTD_BaseEnemy::GetDirectionToAttackTarget() const
{
	const FVector Distance =
		AttackTarget->GetActorLocation() - GetPawnViewLocation();
	return Distance / Distance.Size();
}
