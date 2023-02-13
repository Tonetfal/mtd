#pragma once

#include "CoreMinimal.h"
#include "MTD_Log.h"

const FName AllyCollisionProfileName = "Ally";
const FName PlayerCollisionProfileName = "Player";
const FName TowerCollisionProfileName = "Tower";
const FName FoeCollisionProfileName = "Foe";
const FName FloatingTokenCollisionProfileName = "FloatingToken";
const FName AllyAttackCollisionProfileName = "AllyAttack";
const FName FoeAttackCollisionProfileName = "FoeAttack";
const FName AllyProjectileCollisionProfileName = "AllyProjectile";
const FName FoeProjectileCollisionProfileName = "FoeProjectile";
const FName SightSphereCollisionProfileName = "SightSphere";

constexpr ECollisionChannel AllyProjectileCollisionChannel = ECollisionChannel::ECC_GameTraceChannel1;
constexpr ECollisionChannel AttackCollisionChannel = ECollisionChannel::ECC_GameTraceChannel2;
constexpr ECollisionChannel FoeCollisionChannel = ECollisionChannel::ECC_GameTraceChannel3;
constexpr ECollisionChannel PlayerCollisionChannel = ECollisionChannel::ECC_GameTraceChannel4;
constexpr ECollisionChannel FoeProjectileCollisionChannel = ECollisionChannel::ECC_GameTraceChannel5;
constexpr ECollisionChannel TowerCollisionChannel = ECollisionChannel::ECC_GameTraceChannel6;

constexpr ECollisionChannel FoeTraceChannel = ECollisionChannel::ECC_GameTraceChannel7;
constexpr ECollisionChannel PlayerTraceChannel = ECollisionChannel::ECC_GameTraceChannel8;
constexpr ECollisionChannel TowerTraceChannel = ECollisionChannel::ECC_GameTraceChannel9;
constexpr ECollisionChannel ItemTraceChannel = ECollisionChannel::ECC_GameTraceChannel10;

constexpr EObjectTypeQuery FoeQuery = EObjectTypeQuery::ObjectTypeQuery9;
constexpr EObjectTypeQuery PlayerQuery = EObjectTypeQuery::ObjectTypeQuery10;
constexpr EObjectTypeQuery TowerQuery = EObjectTypeQuery::ObjectTypeQuery12;
