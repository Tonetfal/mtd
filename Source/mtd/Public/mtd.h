#pragma once

#include "CoreMinimal.h"
#include "MTD_Log.h"

const FName AllyCollisionProfileName = TEXT("Ally");
const FName PlayerCollisionProfileName = TEXT("Player");
const FName TowerCollisionProfileName = TEXT("Tower");
const FName EnemyCollisionProfileName = TEXT("Enemy");
const FName FloatingTokenCollisionProfileName = TEXT("FloatingToken");
const FName AllyAttackCollisionProfileName = TEXT("AllyAttack");
const FName EnemyAttackCollisionProfileName = TEXT("EnemyAttack");
const FName AllyProjectileCollisionProfileName = TEXT("AllyProjectile");
const FName EnemyProjectileCollisionProfileName = TEXT("EnemyProjectile");

constexpr ECollisionChannel AllyProjectileCollisionChannel = ECollisionChannel::ECC_GameTraceChannel1;
constexpr ECollisionChannel AttackCollisionChannel = ECollisionChannel::ECC_GameTraceChannel2;
constexpr ECollisionChannel EnemyCollisionChannel = ECollisionChannel::ECC_GameTraceChannel3;
constexpr ECollisionChannel PlayerCollisionChannel = ECollisionChannel::ECC_GameTraceChannel4;
constexpr ECollisionChannel EnemyProjectileCollisionChannel = ECollisionChannel::ECC_GameTraceChannel5;
constexpr ECollisionChannel TowerCollisionChannel = ECollisionChannel::ECC_GameTraceChannel6;

constexpr ECollisionChannel EnemyTraceChannel = ECollisionChannel::ECC_GameTraceChannel7;
constexpr ECollisionChannel PlayerTraceChannel = ECollisionChannel::ECC_GameTraceChannel8;
constexpr ECollisionChannel TowerTraceChannel = ECollisionChannel::ECC_GameTraceChannel9;
