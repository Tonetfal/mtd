#pragma once

#include "mtd.h"
#include "MTD_PawnComponent.h"

#include "MTD_PlayerExtensionComponent.generated.h"

class UMTD_PlayerData;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_PlayerExtensionComponent : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    UMTD_PlayerExtensionComponent();

    UFUNCTION(BlueprintPure, Category="MTD|Pawn")
    static UMTD_PlayerExtensionComponent *FindPlayerExtensionComponent(const AActor *Actor);

    void SetPlayerData(const UMTD_PlayerData *InPlayerData);

    virtual bool IsPawnComponentReadyToInitialize() const override;

protected:
    virtual void OnRegister() override;

public:
    template <class T>
    const T *GetPlayerData() const;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MTD|Player", meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_PlayerData> PlayerData = nullptr;
};

inline UMTD_PlayerExtensionComponent *UMTD_PlayerExtensionComponent::FindPlayerExtensionComponent(const AActor *Actor)
{
    return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_PlayerExtensionComponent>()) : (nullptr);
}

template <class T>
inline const T *UMTD_PlayerExtensionComponent::GetPlayerData() const
{
    return Cast<T>(PlayerData);
}
