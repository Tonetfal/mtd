//void AMTD_BaseEnemy::OnHealthChanged(
//	UMTD_HealthComponent *InHealthComponent, 
//	float OldValue,
//	float NewValue,
//	AActor *InInstigator)
//{
//	const float Damage = NewValue - OldValue;
//	if (Damage <= 0.f)
//		return;
//	
//	if (!IsValid(InInstigator))
//		return;
//	
//	MTDS_VVERBOSE("%s has dealt %f damage", *InInstigator->GetName(), Damage);
//	
//	// Check if player is close enough navmesh wise, ignore if too far
//	// ...
//
//	const bool bIsDamagerIgnored = IgnoredActor == InInstigator;
//	if (bIsDamagerIgnored)
//		OnIgnoreTargetClear();
//		
//	if (!GetWorldTimerManager().IsTimerActive(TryChaneTargetTimerHandle) &&
//		(InInstigator->IsA(AMTD_PlayerCharacter::StaticClass()) ||
//		(IsValid(AttackTarget) && AttackTarget->IsA(AMTD_PlayerCharacter::StaticClass()))))
//	{
//		MTDS_WARN("Setup timer to try to change target");
//		GetWorldTimerManager().SetTimer(
//			TryChaneTargetTimerHandle,
//			this,
//			&AMTD_BaseEnemy::OnTryChangeTarget,
//			SecondsToWaitBeforeTryingToChangeTarget);
//	}
//	LastDamageDealers.FindOrAdd(InInstigator) += Damage;
//
//	if (!bIsDamagerIgnored)
//	{
//		if (!IsValid(AttackTarget) || AttackTarget == InInstigator)
//		{
//			SetNewAttackTarget(InInstigator, true);
//			return;
//		}
//		// Prioritize players over anything
//		else if (InInstigator->IsA(AMTD_PlayerCharacter::StaticClass()))
//		{
//			SetNewAttackTarget(InInstigator, true);
//			return;
//		}
//	}
//}
//
//AActor *FindMostDealingDamager(const TMap<TObjectPtr<AActor>, int32> Dealers)
//{
//	if (Dealers.IsEmpty())
//		return nullptr;
//
//	auto MostDealing = *Dealers.begin();
//	for (auto &It : Dealers)
//	{
//		if (It.Value > MostDealing.Value)
//			MostDealing = It;
//	}
//	return MostDealing.Key;
//}
//
//void AMTD_BaseEnemy::OnTryChangeTarget()
//{
//	MTD_WARN("Timer has been hit");
//	if (LastDamageDealers.IsEmpty())
//		return;
//
//	if (!IsValid(AttackTarget) ||
//		!AttackTarget.IsA(AMTD_PlayerCharacter::StaticClass()))
//	{
//		LastDamageDealers.Empty();
//		return;
//	}
//
//	const int32 *Found = LastDamageDealers.Find(AttackTarget);
//	AActor *MostDealing = FindMostDealingDamager(LastDamageDealers);
//	LastDamageDealers.Empty();
//	
//	if (!Found)
//	{
//		MTDS_VVERBOSE("Player didn't deal any damage for sometime, %s will be "
//			"targeted instead", MostDealing ?
//			*MostDealing->GetName() : TEXT("no one"));
//		
//		OnAttackTargetIgnore(false);
//		if (IsValid(MostDealing))
//			SetNewAttackTarget(MostDealing, true);
//		else
//			SetNewAttackTarget(nullptr);
//	}
//	else if (MostDealing != AttackTarget)
//	{
//		MTDS_VVERBOSE("Player didn't deal the most amount of damage for "
//			"sometime, %s will be targeted instead", *MostDealing->GetName());
//
//		OnAttackTargetIgnore(false);
//		SetNewAttackTarget(MostDealing, true);
//	}
//}
//