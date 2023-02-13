#pragma once

#include "Components/ActorComponent.h"
#include "mtd.h"
#include "MTD_CoreTypes.h"

#include "MTD_TeamComponent.generated.h"

/**
 * Component to manage owning actor's team.
 */
UCLASS(ClassGroup="Pawn", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_TeamComponent
    : public UActorComponent
    , public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    UMTD_TeamComponent();

    /**
     * Find team component on a given actor.
     * @param   Actor: actor to search for team component in.
     * @result  Team component residing on the given actor. Can be nullptr.
     */
    static UMTD_TeamComponent *FindTeamComponent(const AActor *Actor);

    /**
     * Set MTD team ID.
     * @param   Id: MTD team ID to set.
     */
    void SetMtdTeamId(EMTD_TeamId Id);

    /**
     * Get MTD team ID.
     * @return  MTD team ID.
     */
    EMTD_TeamId GetMtdTeamId() const;

    //~IGenericTeamAgentInterface Interface
    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId &Id) override;
    virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor &OtherActor) const override;
    //~End of IGenericTeamAgentInterface Interface

protected:
    //~UActorComponent interface
    virtual void BeginPlay() override;
    //~End of UActorComponent interface

private:
    /** MTD team ID that the owner is associated with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Team Component", meta=(AllowPrivateAccess="true"))
    EMTD_TeamId TeamId = EMTD_TeamId::Unknown;
};

inline UMTD_TeamComponent *UMTD_TeamComponent::FindTeamComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_TeamComponent>()) : (nullptr));
}
