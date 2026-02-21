#ifndef _DUNGEON_ROUTES_H
#define _DUNGEON_ROUTES_H

#include "Define.h"
#include "Position.h"
#include <vector>
#include <map>
#include <string>

namespace BotAI {

struct RouteWaypoint {
    Position position;
    std::string description;
    bool isPullPoint;
    bool isBoss;
    uint32 waitTime;
    
    RouteWaypoint(float x, float y, float z, const std::string& desc, 
                  bool pull = false, bool boss = false, uint32 wait = 0)
        : position(x, y, z, 0.0f), description(desc), 
          isPullPoint(pull), isBoss(boss), waitTime(wait) {}
};

struct DungeonRoute {
    uint32 mapId;
    std::string dungeonName;
    std::vector<RouteWaypoint> waypoints;
    uint32 currentWaypoint;
    
    DungeonRoute() : mapId(0), currentWaypoint(0) {}
    
    bool HasNextWaypoint() const { return currentWaypoint < waypoints.size(); }
    
    RouteWaypoint* GetCurrentWaypoint() {
        if (currentWaypoint < waypoints.size()) {
            return &waypoints[currentWaypoint];
        }
        return nullptr;
    }
    
    void AdvanceWaypoint() {
        if (currentWaypoint < waypoints.size()) currentWaypoint++;
    }
    
    void Reset() { currentWaypoint = 0; }
};

class DungeonRouteManager {
public:
    static DungeonRouteManager* instance();
    
    void Initialize();
    DungeonRoute* GetRoute(uint32 mapId);
    bool HasRoute(uint32 mapId);
    
private:
    DungeonRouteManager() { Initialize(); }
    static DungeonRouteManager* instance_;
    std::map<uint32, DungeonRoute> routes_;
    
    // All 70 dungeon route creation functions
    void CreateShadowfangKeepRoute();
    void CreateDeadminesRoute();
    void CreateWailingCavernsRoute();
    void CreateBlackfathomDeepsRoute();
    void CreateUldamanRoute();
    void CreateGnomereganRoute();
    void CreateSunkenTempleRoute();
    void CreateRazorfenDownsRoute();
    void CreateScarletMonasteryRoute();
    void CreateZulfarrakRoute();
    void CreateBlackrockDepthsRoute();
    void CreateTheBlackMorassRoute();
    void CreateScholomanceRoute();
    void CreateZulgurubRoute();
    void CreateStratholmeRoute();
    void CreateMaraudonRoute();
    void CreateRagefireChasmRoute();
    void CreateMoltenCoreRoute();
    void CreateDireMaulRoute();
    void CreateBlackwingLairRoute();
    void CreateRuinsOfAhnqirajRoute();
    void CreateTempleOfAhnqirajRoute();
    void CreateKarazhanRoute();
    void CreateNaxxramasRoute();
    void CreateHyjalRoute();
    void CreateShatteredHallsRoute();
    void CreateBloodFurnaceRoute();
    void CreateHellfireRampartsRoute();
    void CreateMagtheridonsLairRoute();
    void CreateSteamVaultRoute();
    void CreateTheUnderbogRoute();
    void CreateTheSlavePensRoute();
    void CreateSerpentShrineRoute();
    void CreateTheEyeRoute();
    void CreateArcatrazRoute();
    void CreateTheBotanicaRoute();
    void CreateMechanarRoute();
    void CreateShadowLabyrinthRoute();
    void CreateSethekkHallsRoute();
    void CreateManaTombsRoute();
    void CreateAuchenaiCryptsRoute();
    void CreateOldHillsbradRoute();
    void CreateBlackTempleRoute();
    void CreateGruulsLairRoute();
    void CreateZulamanRoute();
    void CreateUtgardeKeepRoute();
    void CreateUtgardePinnacleRoute();
    void CreateNexusRoute();
    void CreateOculusRoute();
    void CreateSunwellPlateauRoute();
    void CreateMagistersTerraceRoute();
    void CreateCullingOfStratholmeRoute();
    void CreateHallsOfStoneRoute();
    void CreateDrakTharonKeepRoute();
    void CreateAzjolNerubRoute();
    void CreateHallsOfLightningRoute();
    void CreateUlduarRoute();
    void CreateGundrakRoute();
    void CreateVioletHoldRoute();
    void CreateObsidianSanctumRoute();
    void CreateEyeOfEternityRoute();
    void CreateAhnkahetRoute();
    void CreateVaultOfArchavonRoute();
    void CreateIcecrownCitadelRoute();
    void CreateForgeOfSoulsRoute();
    void CreateTrialOfTheCrusaderRoute();
    void CreateTrialOfTheChampionRoute();
    void CreatePitOfSaronRoute();
    void CreateHallsOfReflectionRoute();
    void CreateRubySanctumRoute();
};

} // namespace BotAI

#endif
