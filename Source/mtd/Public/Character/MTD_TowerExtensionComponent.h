#pragma once

#include "mtd.h"
#include "MTD_PawnComponent.h"

#include "MTD_TowerExtensionComponent.generated.h"

class UMTD_TowerData;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_TowerExtensionComponent : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    UMTD_TowerExtensionComponent();

    UFUNCTION(BlueprintPure, Category="MTD|Pawn")
    static UMTD_TowerExtensionComponent *FindTowerExtensionComponent(const AActor *Actor);

    void SetTowerData(const UMTD_TowerData *InTowerData);

    virtual bool IsPawnComponentReadyToInitialize() const override;

protected:
    virtual void OnRegister() override;

public:
    template <class T>
    const T *GetTowerData() const;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MTD|Tower", meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_TowerData> TowerData = nullptr;
};

inline UMTD_TowerExtensionComponent *UMTD_TowerExtensionComponent::FindTowerExtensionComponent(const AActor *Actor)
{
    return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_TowerExtensionComponent>()) : (nullptr);
}

template <class T>
inline const T *UMTD_TowerExtensionComponent::GetTowerData() const
{
    return Cast<T>(TowerData);
}
