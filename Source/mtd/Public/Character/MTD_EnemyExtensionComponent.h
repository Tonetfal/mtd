#pragma once

#include "mtd.h"
#include "MTD_PawnComponent.h"

#include "MTD_EnemyExtensionComponent.generated.h"

class UMTD_EnemyData;

UCLASS(Blueprintable, ClassGroup="Pawn", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_EnemyExtensionComponent : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    UMTD_EnemyExtensionComponent();

    UFUNCTION(BlueprintPure, Category="MTD|Enemy")
    static UMTD_EnemyExtensionComponent *FindEnemyExtensionComponent(const AActor *Actor);

    void SetEnemyData(const UMTD_EnemyData *InEnemyData);

    virtual bool IsPawnComponentReadyToInitialize() const override;

protected:
    virtual void OnRegister() override;

public:
    template <class T>
    const T *GetEnemyData() const;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Enemy", meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_EnemyData> EnemyData = nullptr;
};

inline UMTD_EnemyExtensionComponent *UMTD_EnemyExtensionComponent::FindEnemyExtensionComponent(const AActor *Actor)
{
    return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_EnemyExtensionComponent>()) : (nullptr);
}

template <class T>
inline const T *UMTD_EnemyExtensionComponent::GetEnemyData() const
{
    return Cast<T>(EnemyData);
}
