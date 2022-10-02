//#include "Characters/MTD_Pawn.h"
//
//#include "Animations/MTD_AnimTerminatedNotify.h"
//#include "Animations/MTD_AttackActivationNotify.h"
//#include "Animations/MTD_AttackDeactivationNotify.h"
//#include "Components/MTD_HealthComponent.h"
//#include "Components/MTD_MovementComponent.h"
//#include "Utilities/MTD_Utilities.h"
//
//#include "Components/ArrowComponent.h"
//#include "Components/CapsuleComponent.h"
//#include "Components/SphereComponent.h"
//
//AMTD_Pawn::AMTD_Pawn()
//{
//	PrimaryActorTick.bCanEverTick = true;
//
//	CollisionCapsule =
//		CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Capsule"));
//	CollisionCapsule->InitCapsuleSize(35.f, 80.f);
//	CollisionCapsule->SetCollisionProfileName(PawnCollisionProfileName);
//	SetRootComponent(CollisionCapsule);
//
//	Mesh =
//		CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
//	Mesh->SetupAttachment(GetRootComponent());
//	Mesh->SetCollisionProfileName("NoCollision");
//	
//	Arrow =
//		CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Component"));
//	Arrow->SetupAttachment(GetRootComponent());
//	Arrow->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
//	
//	AttackArea =
//		CreateDefaultSubobject<USphereComponent>(TEXT("Attack Area"));
//	AttackArea->SetupAttachment(GetRootComponent());
//	AttackArea->SetCollisionProfileName(DeactivatedAttackCollisionProfileName);
//	AttackArea->InitSphereRadius(25.f);
//	AttackArea->SetRelativeLocation(FVector(50.f, 0.f, 0.f));
//
//	Movement =
//		CreateDefaultSubobject<UMTD_MovementComponent>(
//			TEXT("Movement Component"));
//	// Setup movement...
//	
//	Health =
//		CreateDefaultSubobject<UMTD_HealthComponent>(TEXT("Health Component"));
//}
//
//void AMTD_Pawn::BeginPlay()
//{
//	Super::BeginPlay();
//
//	InitDelegates();
//	InitNotifies();
//	
//	OnHealthChanged(Health->GetHealth());
//}
//
//void AMTD_Pawn::InitDelegates()
//{
//	Health->OnHealthChangedDelegate.AddUObject(
//		this, &AMTD_Pawn::OnHealthChanged);
//	Health->OnDeathDelegate.AddUObject(
//		this, &AMTD_Pawn::OnDeath);
//
//	AttackArea->OnComponentBeginOverlap.AddDynamic(
//		this, &AMTD_Pawn::OnAttackBeginOverlap);
//}
//
//void AMTD_Pawn::InitNotifies()
//{
//	if (!IsValid(AttackAnimMontage))
//	{
//		UE_LOG(LogMtdPawn, Warning,
//			TEXT("Attack animation montage is not set!"));
//		return;
//	}
//	
//	UMTD_AttackActivationNotify *AttackActivationNotify =
//		FMTD_Utilities::FindFirstNotifyByClass<UMTD_AttackActivationNotify>(
//			AttackAnimMontage);
//
//	if (IsValid(AttackActivationNotify))
//	{
//		AttackActivationNotify->OnNotifiedDelegate.AddUObject(
//			this, &AMTD_Pawn::OnAttackBegin);
//	}
//	else
//	{
//		UE_LOG(LogMtdPawn, Warning,
//			TEXT("Attack animation montage does not have an attack "
//				"activation notify!"));
//	}
//	
//	UMTD_AttackDeactivationNotify  *AttackDeactivationNotify =
//		FMTD_Utilities::FindFirstNotifyByClass<UMTD_AttackDeactivationNotify>(
//			AttackAnimMontage);
//
//	if (IsValid(AttackDeactivationNotify))
//	{
//		AttackDeactivationNotify->OnNotifiedDelegate.AddUObject(
//			this, &AMTD_Pawn::OnAttackEnd);
//	}
//	else
//	{
//		UE_LOG(LogMtdPawn, Warning,
//			TEXT("Attack animation montage does not have an attack "
//				"deactivation notify!"));
//	}
//	
//	UMTD_AnimTerminatedNotify *AnimationEnd =
//		FMTD_Utilities::FindFirstNotifyByClass<UMTD_AnimTerminatedNotify>(
//			AttackAnimMontage);
//
//	if (IsValid(AnimationEnd))
//	{
//		AnimationEnd->OnNotifiedDelegate.AddUObject(
//			this, &AMTD_Pawn::OnAttackAnimationTerminate);
//	}
//	else
//	{
//		UE_LOG(LogMtdPawn, Warning,
//			TEXT("Attack animation montage does not have an animation "
//				"terminate notify!"));
//	}
//}
//
//void AMTD_Pawn::OnHealthChanged(int32 NewHealth)
//{
//}
//
//void AMTD_Pawn::OnAttack()
//{
//	// Attack animation will fire some notifies which will
//	// be used to handle all the attack logic
//
//	UE_LOG(LogMtdPawn, VeryVerbose, TEXT("Pawn is attacking"));
//	PlayAttackAnimation();
//}
//
//void AMTD_Pawn::OnDeath()
//{
//	UE_LOG(LogMtdPawn, VeryVerbose, TEXT("Pawn has died"));
//	PlayDeathAnimation();
//	// Disable movement
//	CollisionCapsule->SetCollisionResponseToAllChannels(
//		ECollisionResponse::ECR_Ignore);
//}
//
//void AMTD_Pawn::OnAttackBegin(USkeletalMeshComponent*MyMesh)
//{
//	UE_LOG(LogMtdPawn, VeryVerbose, TEXT("An attack has began"));
//	AttackArea->SetCollisionProfileName(ActivatedAttackCollisionProfileName);
//}
//
//void AMTD_Pawn::OnAttackEnd(USkeletalMeshComponent*MyMesh)
//{
//	UE_LOG(LogMtdPawn, VeryVerbose, TEXT("An attack has ended"));
//	AttackArea->SetCollisionProfileName(DeactivatedAttackCollisionProfileName);
//}
//
//void AMTD_Pawn::OnAttackAnimationTerminate(USkeletalMeshComponent*MyMesh)
//{
//	// ...
//}
//
//void AMTD_Pawn::OnDeathAnimationTerminate(USkeletalMeshComponent*MyMesh)
//{
//	// ...
//}
//
//FGenericTeamId AMTD_Pawn::GetGenericTeamId() const
//{
//	return FGenericTeamId::NoTeam;
//}
//
//void AMTD_Pawn::OnAttackBeginOverlap(
//	UPrimitiveComponent *OverlappedComponent,
//	AActor *OtherActor,
//	UPrimitiveComponent *OtherComp,
//	int32 OtherBodyIndex,
//	bool bFromSweep,
//	const FHitResult &SweepResult)
//{
//	// Ignore itself
//	if (OtherActor == this)
//		return;
//	
//	UMTD_HealthComponent *OtherHealth =
//		FMTD_Utilities::GetActorComponent<UMTD_HealthComponent>(OtherActor);
//	
//	if (!IsValid(OtherHealth))
//		return;
//	// OtherHealth->DecreaseHealth(Damage);
//}
//
//void AMTD_Pawn::OnAttackEndOverlap(
//	UPrimitiveComponent *OverlappedComponent,
//	AActor *OtherActor,
//	UPrimitiveComponent *OtherComp,
//	int32 OtherBodyIndex)
//{
//}
//
//void AMTD_Pawn::PlayAttackAnimation()
//{
//	if (!IsValid(AttackAnimMontage))
//	{
//		UE_LOG(LogMtdPawn, Warning,
//			TEXT("Attack animation montage is not set!"));
//		return;
//	}
//	
//	UAnimInstance *MyAnimInstance = Mesh->GetAnimInstance();
//	MyAnimInstance->Montage_Play(AttackAnimMontage);
//}
//
//void AMTD_Pawn::PlayDeathAnimation()
//{
//	if (!IsValid(DeathAnimMontage))
//	{
//		UE_LOG(LogMtdPawn, Warning,
//			TEXT("Death animation montage is not set!"));
//		return;
//	}
//	
//	UAnimInstance *MyAnimInstance = Mesh->GetAnimInstance();
//	MyAnimInstance->Montage_Play(DeathAnimMontage);
//}
//