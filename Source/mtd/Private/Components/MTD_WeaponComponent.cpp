#include "Components/MTD_WeaponComponent.h"

#include "Animations/MTD_AnimTerminatedNotify.h"
#include "Animations/MTD_AttackActivationNotify.h"
#include "Animations/MTD_AttackDeactivationNotify.h"
#include "Components/MTD_TeamComponent.h"
#include "Utilities/MTD_Utilities.h"

#include "GameFramework/Character.h"

UMTD_WeaponComponent::UMTD_WeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor *ActorOwner = GetOwner();
	if (!IsValid(ActorOwner))
	{
		MTD_WARN("Owner is invalid");
		return;
	}

	auto *OwnerMesh = 
		FMTD_Utilities::GetActorComponent<USkeletalMeshComponent>(ActorOwner);
	if (!IsValid(OwnerMesh))
	{
		MTD_WARN("%s does not have a skeletal mesh", *ActorOwner->GetName());
		return;
	}

	if (!OwnerMesh->DoesSocketExist(WeaponEquipSocketName))
	{
		MTD_WARN("%s does not have socket %s", 
			*OwnerMesh->GetName(), *WeaponEquipSocketName.ToString());
		return;
	}
	
	SpawnWeapon();
	AttachWeaponToSocket(Weapon, OwnerMesh, WeaponEquipSocketName);
	SetWeaponAttackCollisionProfileName();
}

void UMTD_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearTimer(AttackPermissionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(NextAttackTimerHandle);
}

void UMTD_WeaponComponent::StartAttacking()
{
	if (!IsValid(Weapon))
		return;

	bWantsAttacking = true;
	Attack();
}

void UMTD_WeaponComponent::StopAttacking()
{
	if (!IsValid(Weapon))
		return;

	GetWorld()->GetTimerManager().ClearTimer(NextAttackTimerHandle);
	
	bWantsAttacking = false;
}

void UMTD_WeaponComponent::ForceStopAttacking()
{
	Weapon->ForceStopAttack();
}

void UMTD_WeaponComponent::SpawnWeapon()
{
	if (!WeaponData.WeaponClass)
	{
		MTD_WARN("Weapon class is missing");
		return;
	}

	AActor *ActorOwner = GetOwner();
	if (!IsValid(ActorOwner))
	{
		MTD_WARN("Owner is invalid");
		return;
	}

	Weapon = GetWorld()->SpawnActor<AMTD_BaseWeapon>(WeaponData.WeaponClass);
	if (!IsValid(Weapon))
	{
		MTD_WARN("Weapon has failed spawning");
		return;
	}

	Weapon->SetOwner(ActorOwner);
	SetupNotifyDelegates();
}

void UMTD_WeaponComponent::SetupNotifyDelegates()
{
	if (!IsValid(WeaponData.AttackAnimMontage))
	{
		MTD_WARN("Weapon attack animation montage is invalid");
		return;
	}

	auto *AttackActivationNotify = 
		FMTD_Utilities::FindFirstNotifyByClass<UMTD_AttackActivationNotify>(
			WeaponData.AttackAnimMontage);
	if (IsValid(AttackActivationNotify))
	{
		AttackActivationNotify->OnNotifiedDelegate.AddUObject(
			this, &UMTD_WeaponComponent::OnAttackActivation);
	}
	else
	{
		MTD_WARN("%s is missing attack activation notify",
			*WeaponData.AttackAnimMontage->GetName());
	}
	
	UMTD_AttackDeactivationNotify *AttackDeactivationNotify =
		FMTD_Utilities::FindFirstNotifyByClass<UMTD_AttackDeactivationNotify>(
			WeaponData.AttackAnimMontage);
	if (IsValid(AttackDeactivationNotify))
	{
		AttackDeactivationNotify->OnNotifiedDelegate.AddUObject(
			this, &UMTD_WeaponComponent::OnAttackDeactivation);
	}
	else
	{
		MTD_WARN("%s is missing attack deactivation notify",
			*WeaponData.AttackAnimMontage->GetName());
	}
	
	UMTD_AnimTerminatedNotify *AnimTerminatedNotify =
		FMTD_Utilities::FindFirstNotifyByClass<UMTD_AnimTerminatedNotify>(
			WeaponData.AttackAnimMontage);
	if (IsValid(AnimTerminatedNotify))
	{
		AnimTerminatedNotify->OnNotifiedDelegate.AddUObject(
			this, &UMTD_WeaponComponent::OnAttackAnimationTerminated);
	}
	else
	{
		MTD_WARN("%s is missing attack terminated notify",
			*WeaponData.AttackAnimMontage->GetName());
	}
}

void UMTD_WeaponComponent::AttachWeaponToSocket(AMTD_BaseWeapon *WeaponToAttach,
	USceneComponent *AttachTo, FName SocketName)
{
	if (!IsValid(WeaponToAttach))
	{
		MTD_WARN("Weapon is invalid");
		return;
	}
	
	if (!IsValid(AttachTo))
	{
		MTD_WARN("Object that weapon had to be attached to is invalid");
		return;
	}

	const FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget, false);
	WeaponToAttach->AttachToComponent(AttachTo, AttachmentRules, SocketName);
}

void UMTD_WeaponComponent::SetWeaponAttackCollisionProfileName()
{
	if (!IsValid(Weapon))
	{
		MTD_WARN("Weapon is invalid");
		return;
	}

	const FGenericTeamId GenericId =
		FMTD_Utilities::GetMtdGenericTeamId(GetOwner());
	
	switch (FMTD_Utilities::GenericToMtdTeamId(GenericId))
	{
	case EMTD_TeamId::Player:
		Weapon->SetAttackCollisionProfileName(AllyAttackCollisionProfileName);
		break;
		
	case EMTD_TeamId::Tower:
		MTD_WARN("A tower cannot have a weapon component");
		Weapon->SetAttackCollisionProfileName("NoCollision");
		break;
		
	case EMTD_TeamId::Enemy:
		Weapon->SetAttackCollisionProfileName(EnemyAttackCollisionProfileName);
		break;
		
	default:
		Weapon->SetAttackCollisionProfileName("NoCollision");
		break;
	}
}

void UMTD_WeaponComponent::PlayAnimMontage(UAnimMontage *AnimMontage) const
{
	if (!IsValid(AnimMontage))
	{
		MTD_WARN("Animation montage is invalid");
		return;
	}
	
	AActor *ActorOwner = GetOwner();
	if (!IsValid(ActorOwner))
	{
		MTD_WARN("Owner is invalid");
		return;
	}

	ACharacter *CharacterOwner = Cast<ACharacter>(ActorOwner);
	if (!IsValid(CharacterOwner))
	{
		MTD_WARN("Failed to cast %s to ACharacter", *CharacterOwner->GetName());
		return;
	}

	CharacterOwner->PlayAnimMontage(AnimMontage);
}

void UMTD_WeaponComponent::Attack()
{
	if (!IsValid(Weapon) || !bIsAllowedAttacking || !bWantsAttacking)
		return;
	
	PlayAnimMontage(WeaponData.AttackAnimMontage);
	Weapon->Attack();
	bIsAllowedAttacking = false;
}

void UMTD_WeaponComponent::OnAttackActivation(
	USkeletalMeshComponent *TargetMesh)
{
	if (!IsValid(Weapon) || !IsNotifyForUs(TargetMesh))
		return;
	
	Weapon->OnAttackActivation();
}

void UMTD_WeaponComponent::OnAttackDeactivation(
	USkeletalMeshComponent *TargetMesh)
{
	if (!IsValid(Weapon) || !IsNotifyForUs(TargetMesh))
		return;
	
	Weapon->OnAttackDeactivation();
}

void UMTD_WeaponComponent::OnAttackAnimationTerminated(
	USkeletalMeshComponent *TargetMesh)
{
	if (!IsValid(Weapon) || !IsNotifyForUs(TargetMesh))
		return;

	Weapon->OnAttackAnimationTerminated();

	FTimerManager &TimerManager = GetWorld()->GetTimerManager();
	
	if (bWantsAttacking)
	{
		if (TimeBetweenAttacks > 0.f)
		{
			TimerManager.SetTimer(NextAttackTimerHandle,
				this, &UMTD_WeaponComponent::Attack, TimeBetweenAttacks);
		}
	}

	// Must be called after setting timer for next attack!
	if (TimeBetweenAttacks > 0.f)
	{
		TimerManager.SetTimer(AttackPermissionTimerHandle,
			this, &UMTD_WeaponComponent::AllowAttacking, TimeBetweenAttacks);
	}
	else
	{
		AllowAttacking();
	}
}

void UMTD_WeaponComponent::AllowAttacking()
{
	bIsAllowedAttacking = true;
}

bool UMTD_WeaponComponent::IsNotifyForUs(
	const USkeletalMeshComponent *TargetMesh) const
{
	if (!IsValid(TargetMesh))
		return false;
		
	const AActor *ActorOwner = GetOwner();
	if (!IsValid(ActorOwner))
	{
		MTD_WARN("Owner is invalid");
		return false;
	}
		
	return TargetMesh ==
		FMTD_Utilities::GetActorComponent<USkeletalMeshComponent>(ActorOwner);
}
