#include "Player/MTD_EnemyController.h"

#include "Character/MTD_TeamComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/MTD_Utility.h"

AMTD_EnemyController::AMTD_EnemyController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    Team = CreateDefaultSubobject<UMTD_TeamComponent>(TEXT("MTD Team Component"));

    bAttachToPawn = true;
    bWantsPlayerState = true;
}

void AMTD_EnemyController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const AActor *FocusActor = GetFocusActor();
    if ((!IsValid(FocusActor)) && (IsValid(OwnerMovementComponent)) && (!OwnerMovementComponent->Velocity.IsZero()))
    {
        const FRotator DirRot = OwnerMovementComponent->Velocity.GetSafeNormal().ToOrientationRotator();
        SetControlRotation(DirRot);
    }
}

void AMTD_EnemyController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    OwnerMovementComponent = FMTD_Utility::GetActorComponent<UMovementComponent>(InPawn);
    if (!IsValid(OwnerMovementComponent))
    {
        MTDS_WARN("Owner [%s] does not have a Movement Component.", *InPawn->GetName());
        return;
    }
}
