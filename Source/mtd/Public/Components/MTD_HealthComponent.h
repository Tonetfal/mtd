#pragma once

#include "mtd.h"
#include "Components/ActorComponent.h"

#include "MTD_HealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnHealthChangedSignature, int32 NewHealth, int32 HealthDelta);
DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnDamagedSignature, AActor* DamageCauser, int32 Damage);
DECLARE_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMTD_HealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	void IncreaseHealth(int32 Value);
	void DecreaseHealth(int32 Value);
	void SetHealth(int32 Value);
	int32 GetHealth() const
		{ return Health; }
	int32 GetMaxHealth() const
		{ return MaxHealth; }
	float GetHealthRatio() const
		{ return static_cast<float>(Health) / static_cast<float>(MaxHealth); }
	bool IsDead() const
		{ return Health == 0; }

protected:
	UFUNCTION()
	virtual void OnTakeAnyDamage(
		AActor *DamagedActor,
		float Damage,
		const UDamageType *DamageType,
		AController *IntigatedBy,
		AActor *DamageCauser);

public:
	FOnHealthChangedSignature OnHealthChangedDelegate;
	FOnDamagedSignature OnDamagedDelegate;
	FOnDeathSignature OnDeathDelegate;
		
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Health Component",
		meta=(AllowPrivateAccess="true"))
	bool bImmortal = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Health Component",
		meta=(AllowPrivateAccess="true", EditCondition="!bImmortal"))
	int32 MaxHealth = 100;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Health Component|Runtime",
		meta=(AllowPrivateAccess="true"))
	int32 Health = 0;

	// Resists
};
