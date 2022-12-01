#include "GameModes/MTD_TowerDefenseMode.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameModes/MTD_Core.h"

AMTD_TowerDefenseMode::AMTD_TowerDefenseMode()
{
}

void AMTD_TowerDefenseMode::BeginPlay()
{
    Super::BeginPlay();

    // Retrieve all cores placed on current level
    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMTD_Core::StaticClass(), OutActors);

    for (AActor *Actor : OutActors)
    {
        auto Core = Cast<AMTD_Core>(Actor);

        // Setup a core and add it to the list
        Core->OnCoreDestroyedDelegate.AddDynamic(this, &ThisClass::OnCoreDestroyed);
        Cores.Add(Core);
    }
}

AActor *AMTD_TowerDefenseMode::GetGameTarget(APawn *Client) const
{
    check(IsValid(Client));
    
    // Store final result in here
    AActor *Result = nullptr;
    float LowestCost = 0.f;

    // Cache calculation data
    UWorld *World = GetWorld();
    const FVector ClientPosition = Client->GetNavAgentLocation();
    const auto ClientController = CastChecked<AAIController>(Client->GetController());
    const auto NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    ANavigationData *NavData = NavSys->GetNavDataForProps(ClientController->GetNavAgentPropertiesRef(), ClientPosition);
    const TSubclassOf<UNavigationQueryFilter> FilterClass = ClientController->GetDefaultNavigationFilterClass();

    for (AMTD_Core *Core : Cores)
    {
        // Shorthand calculation data in variables
        float Cost;
        const FVector CorePosition = Core->GetActorLocation();

        // Check path cost a core
        const ENavigationQueryResult::Type PathResult = NavSys->GetPathCost(
            World, ClientPosition, CorePosition, Cost, NavData, FilterClass);

        // Path result must always be valid in terms of passed data validness
        check(PathResult != ENavigationQueryResult::Error);
        check(PathResult != ENavigationQueryResult::Invalid);

        // If the core is reachable check whether its the most optimal path cost wise
        if (PathResult == ENavigationQueryResult::Success)
        {
            if ((!Result) || (LowestCost > Cost))
            {
                LowestCost = Cost;
                Result = Core;
            }
        }
        // Should never happen. The only reason must be a bad level design that doesn't allow an AI to get all the cores
        else
        {
            MTD_WARN("An actor on [%s] cannot get core [%s]", *ClientPosition.ToString(), *Core->GetName());
        }
    }

    return Result;
}

void AMTD_TowerDefenseMode::OnCoreDestroyed()
{
    TerminateGame(EMTD_GameResult::Lose);
}
