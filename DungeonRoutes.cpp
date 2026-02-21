#include "DungeonRoutes.h"
#include "Log.h"

namespace BotAI {

DungeonRouteManager* DungeonRouteManager::instance_ = nullptr;

DungeonRouteManager* DungeonRouteManager::instance() {
    if (!instance_) {
        instance_ = new DungeonRouteManager();
    }
    return instance_;
}

void DungeonRouteManager::Initialize() {
    LOG_INFO("module", "Initializing ALL dungeon routes...");
    
    CreateShadowfangKeepRoute();
    CreateDeadminesRoute();
    CreateWailingCavernsRoute();
    CreateBlackfathomDeepsRoute();
    CreateUldamanRoute();
    CreateGnomereganRoute();
    CreateSunkenTempleRoute();
    CreateRazorfenDownsRoute();
    CreateScarletMonasteryRoute();
    CreateZulfarrakRoute();
    CreateBlackrockDepthsRoute();
    CreateTheBlackMorassRoute();
    CreateScholomanceRoute();
    CreateZulgurubRoute();
    CreateStratholmeRoute();
    CreateMaraudonRoute();
    CreateRagefireChasmRoute();
    CreateMoltenCoreRoute();
    CreateDireMaulRoute();
    CreateBlackwingLairRoute();
    CreateRuinsOfAhnqirajRoute();
    CreateTempleOfAhnqirajRoute();
    CreateKarazhanRoute();
    CreateNaxxramasRoute();
    CreateHyjalRoute();
    CreateShatteredHallsRoute();
    CreateBloodFurnaceRoute();
    CreateHellfireRampartsRoute();
    CreateMagtheridonsLairRoute();
    CreateSteamVaultRoute();
    CreateTheUnderbogRoute();
    CreateTheSlavePensRoute();
    CreateSerpentShrineRoute();
    CreateTheEyeRoute();
    CreateArcatrazRoute();
    CreateTheBotanicaRoute();
    CreateMechanarRoute();
    CreateShadowLabyrinthRoute();
    CreateSethekkHallsRoute();
    CreateManaTombsRoute();
    CreateAuchenaiCryptsRoute();
    CreateOldHillsbradRoute();
    CreateBlackTempleRoute();
    CreateGruulsLairRoute();
    CreateZulamanRoute();
    CreateUtgardeKeepRoute();
    CreateUtgardePinnacleRoute();
    CreateNexusRoute();
    CreateOculusRoute();
    CreateSunwellPlateauRoute();
    CreateMagistersTerraceRoute();
    CreateCullingOfStratholmeRoute();
    CreateHallsOfStoneRoute();
    CreateDrakTharonKeepRoute();
    CreateAzjolNerubRoute();
    CreateHallsOfLightningRoute();
    CreateUlduarRoute();
    CreateGundrakRoute();
    CreateVioletHoldRoute();
    CreateObsidianSanctumRoute();
    CreateEyeOfEternityRoute();
    CreateAhnkahetRoute();
    CreateVaultOfArchavonRoute();
    CreateIcecrownCitadelRoute();
    CreateForgeOfSoulsRoute();
    CreateTrialOfTheCrusaderRoute();
    CreateTrialOfTheChampionRoute();
    CreatePitOfSaronRoute();
    CreateHallsOfReflectionRoute();
    CreateRubySanctumRoute();
    
    LOG_INFO("module", "Loaded {} dungeon routes", routes_.size());
}

DungeonRoute* DungeonRouteManager::GetRoute(uint32 mapId) {
    auto it = routes_.find(mapId);
    if (it != routes_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool DungeonRouteManager::HasRoute(uint32 mapId) {
    return routes_.find(mapId) != routes_.end();
}

// ============================================================================
// SHADOWFANG KEEP (Map 33)
// ============================================================================
void DungeonRouteManager::CreateShadowfangKeepRoute() {
    DungeonRoute route;
    route.mapId = 33;
    route.dungeonName = "Shadowfang Keep";
    
    
    routes_[33] = route;
    LOG_INFO("module", "Loaded Shadowfang Keep route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// DEADMINES (Map 36)
// ============================================================================
void DungeonRouteManager::CreateDeadminesRoute() {
    DungeonRoute route;
    route.mapId = 36;
    route.dungeonName = "Deadmines";
    
    route.waypoints.push_back(RouteWaypoint(-289.45f, -513.01f, 49.68f, "Sneed's Shredder", true, true));
    route.waypoints.push_back(RouteWaypoint(-192.92f, -448.21f, 54.43f, "Rhahk'Zor", true, true));
    route.waypoints.push_back(RouteWaypoint(-177.36f, -574.46f, 19.31f, "Gilnid", true, true));
    route.waypoints.push_back(RouteWaypoint(-87.37f, -819.9f, 39.3f, "Edwin VanCleef", true, true));
    route.waypoints.push_back(RouteWaypoint(-59.62f, -820.13f, 41.61f, "Captain Greenskin", true, true));
    route.waypoints.push_back(RouteWaypoint(-22.85f, -797.28f, 20.37f, "Mr. Smite", true, true));
    
    routes_[36] = route;
    LOG_INFO("module", "Loaded Deadmines route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// WAILING CAVERNS (Map 43)
// ============================================================================
void DungeonRouteManager::CreateWailingCavernsRoute() {
    DungeonRoute route;
    route.mapId = 43;
    route.dungeonName = "Wailing Caverns";
    
    
    routes_[43] = route;
    LOG_INFO("module", "Loaded Wailing Caverns route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// BLACKFATHOM DEEPS (Map 48)
// ============================================================================
void DungeonRouteManager::CreateBlackfathomDeepsRoute() {
    DungeonRoute route;
    route.mapId = 48;
    route.dungeonName = "Blackfathom Deeps";
    
    
    routes_[48] = route;
    LOG_INFO("module", "Loaded Blackfathom Deeps route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// ULDAMAN (Map 70)
// ============================================================================
void DungeonRouteManager::CreateUldamanRoute() {
    DungeonRoute route;
    route.mapId = 70;
    route.dungeonName = "Uldaman";
    
    
    routes_[70] = route;
    LOG_INFO("module", "Loaded Uldaman route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// GNOMEREGAN (Map 90)
// ============================================================================
void DungeonRouteManager::CreateGnomereganRoute() {
    DungeonRoute route;
    route.mapId = 90;
    route.dungeonName = "Gnomeregan";
    
    
    routes_[90] = route;
    LOG_INFO("module", "Loaded Gnomeregan route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// SUNKEN TEMPLE (Map 109)
// ============================================================================
void DungeonRouteManager::CreateSunkenTempleRoute() {
    DungeonRoute route;
    route.mapId = 109;
    route.dungeonName = "Sunken Temple";
    
    
    routes_[109] = route;
    LOG_INFO("module", "Loaded Sunken Temple route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// RAZORFEN DOWNS (Map 129)
// ============================================================================
void DungeonRouteManager::CreateRazorfenDownsRoute() {
    DungeonRoute route;
    route.mapId = 129;
    route.dungeonName = "Razorfen Downs";
    
    
    routes_[129] = route;
    LOG_INFO("module", "Loaded Razorfen Downs route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// SCARLET MONASTERY (Map 189)
// ============================================================================
void DungeonRouteManager::CreateScarletMonasteryRoute() {
    DungeonRoute route;
    route.mapId = 189;
    route.dungeonName = "Scarlet Monastery";
    
    
    routes_[189] = route;
    LOG_INFO("module", "Loaded Scarlet Monastery route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// ZULFARRAK (Map 209)
// ============================================================================
void DungeonRouteManager::CreateZulfarrakRoute() {
    DungeonRoute route;
    route.mapId = 209;
    route.dungeonName = "Zulfarrak";
    
    
    routes_[209] = route;
    LOG_INFO("module", "Loaded Zulfarrak route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// BLACKROCK DEPTHS (Map 230)
// ============================================================================
void DungeonRouteManager::CreateBlackrockDepthsRoute() {
    DungeonRoute route;
    route.mapId = 230;
    route.dungeonName = "Blackrock Depths";
    
    
    routes_[230] = route;
    LOG_INFO("module", "Loaded Blackrock Depths route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// THE BLACK MORASS (Map 269)
// ============================================================================
void DungeonRouteManager::CreateTheBlackMorassRoute() {
    DungeonRoute route;
    route.mapId = 269;
    route.dungeonName = "The Black Morass";
    
    route.waypoints.push_back(RouteWaypoint(-2025.3f, 7119.58f, 22.75f, "Medivh", true, true));
    
    routes_[269] = route;
    LOG_INFO("module", "Loaded The Black Morass route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// SCHOLOMANCE (Map 289)
// ============================================================================
void DungeonRouteManager::CreateScholomanceRoute() {
    DungeonRoute route;
    route.mapId = 289;
    route.dungeonName = "Scholomance";
    
    
    routes_[289] = route;
    LOG_INFO("module", "Loaded Scholomance route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// ZULGURUB (Map 309)
// ============================================================================
void DungeonRouteManager::CreateZulgurubRoute() {
    DungeonRoute route;
    route.mapId = 309;
    route.dungeonName = "Zulgurub";
    
    route.waypoints.push_back(RouteWaypoint(-12326.5f, -1577.11f, 133.59f, "High Priestess Mar'li", true, true));
    route.waypoints.push_back(RouteWaypoint(-12291.9f, -1380.08f, 144.9f, "High Priestess Jeklik", true, true));
    route.waypoints.push_back(RouteWaypoint(-12167.8f, -1927.25f, 153.73f, "Bloodlord Mandokir", true, true));
    route.waypoints.push_back(RouteWaypoint(-12029.8f, -1707.93f, 39.41f, "High Priest Venoxis", true, true));
    route.waypoints.push_back(RouteWaypoint(-11787.4f, -1649.34f, 54.1f, "Hakkar", true, true));
    route.waypoints.push_back(RouteWaypoint(-11700.8f, -1998.94f, 62.42f, "High Priest Thekal", true, true));
    route.waypoints.push_back(RouteWaypoint(-11515.8f, -1275.48f, 79.66f, "Jin'do the Hexxer", true, true));
    
    routes_[309] = route;
    LOG_INFO("module", "Loaded Zulgurub route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// STRATHOLME (Map 329)
// ============================================================================
void DungeonRouteManager::CreateStratholmeRoute() {
    DungeonRoute route;
    route.mapId = 329;
    route.dungeonName = "Stratholme";
    
    
    routes_[329] = route;
    LOG_INFO("module", "Loaded Stratholme route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// MARAUDON (Map 349)
// ============================================================================
void DungeonRouteManager::CreateMaraudonRoute() {
    DungeonRoute route;
    route.mapId = 349;
    route.dungeonName = "Maraudon";
    
    
    routes_[349] = route;
    LOG_INFO("module", "Loaded Maraudon route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// RAGEFIRE CHASM (Map 389)
// ============================================================================
void DungeonRouteManager::CreateRagefireChasmRoute() {
    DungeonRoute route;
    route.mapId = 389;
    route.dungeonName = "Ragefire Chasm";
    
    
    routes_[389] = route;
    LOG_INFO("module", "Loaded Ragefire Chasm route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// MOLTEN CORE (Map 409)
// ============================================================================
void DungeonRouteManager::CreateMoltenCoreRoute() {
    DungeonRoute route;
    route.mapId = 409;
    route.dungeonName = "Molten Core";
    
    route.waypoints.push_back(RouteWaypoint(587.3f, -801.97f, -205.16f, "Shazzrah", true, true));
    route.waypoints.push_back(RouteWaypoint(601.09f, -1179.11f, -195.88f, "Sulfuron Harbinger", true, true));
    route.waypoints.push_back(RouteWaypoint(691.02f, -498.39f, -214.27f, "Garr", true, true));
    route.waypoints.push_back(RouteWaypoint(747.55f, -981.68f, -178.4f, "Baron Geddon", true, true));
    route.waypoints.push_back(RouteWaypoint(793.25f, -998.27f, -206.76f, "Golemagg the Incinerator", true, true));
    route.waypoints.push_back(RouteWaypoint(898.07f, -545.87f, -203.4f, "Gehennas", true, true));
    route.waypoints.push_back(RouteWaypoint(1037.02f, -986.34f, -181.52f, "Lucifron", true, true));
    route.waypoints.push_back(RouteWaypoint(1144.63f, -1020.48f, -185.66f, "Magmadar", true, true));
    
    routes_[409] = route;
    LOG_INFO("module", "Loaded Molten Core route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// DIRE MAUL (Map 429)
// ============================================================================
void DungeonRouteManager::CreateDireMaulRoute() {
    DungeonRoute route;
    route.mapId = 429;
    route.dungeonName = "Dire Maul";
    
    
    routes_[429] = route;
    LOG_INFO("module", "Loaded Dire Maul route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// BLACKWING LAIR (Map 469)
// ============================================================================
void DungeonRouteManager::CreateBlackwingLairRoute() {
    DungeonRoute route;
    route.mapId = 469;
    route.dungeonName = "Blackwing Lair";
    
    route.waypoints.push_back(RouteWaypoint(-7587.76f, -1261.43f, 482.21f, "Lord Victor Nefarius - Lord of Blackrock", true, true));
    route.waypoints.push_back(RouteWaypoint(-7573.97f, -1034.39f, 449.34f, "Broodlord Lashlayer", true, true));
    route.waypoints.push_back(RouteWaypoint(-7570.64f, -1090.02f, 413.56f, "Razorgore the Untamed", true, true));
    route.waypoints.push_back(RouteWaypoint(-7520.24f, -1025.77f, 449.09f, "Firemaw", true, true));
    route.waypoints.push_back(RouteWaypoint(-7515.34f, -1029.62f, 476.73f, "Chromaggus", true, true));
    route.waypoints.push_back(RouteWaypoint(-7483.79f, -1015.99f, 408.65f, "Vaelastrasz the Corrupt", true, true));
    route.waypoints.push_back(RouteWaypoint(-7407.94f, -1031.04f, 477.35f, "Flamegor", true, true));
    route.waypoints.push_back(RouteWaypoint(-7358.19f, -994.32f, 477.35f, "Ebonroc", true, true));
    
    routes_[469] = route;
    LOG_INFO("module", "Loaded Blackwing Lair route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// RUINS OF AHNQIRAJ (Map 509)
// ============================================================================
void DungeonRouteManager::CreateRuinsOfAhnqirajRoute() {
    DungeonRoute route;
    route.mapId = 509;
    route.dungeonName = "Ruins Of Ahnqiraj";
    
    route.waypoints.push_back(RouteWaypoint(-9719.61f, 1514.75f, 27.55f, "Ayamiss the Hunter", true, true));
    route.waypoints.push_back(RouteWaypoint(-9502.8f, 2042.65f, 105.31f, "Ossirian the Unscarred", true, true));
    route.waypoints.push_back(RouteWaypoint(-9235.98f, 1234.07f, -63.66f, "Buru the Gorger", true, true));
    route.waypoints.push_back(RouteWaypoint(-9129.73f, 1602.5f, 26.54f, "General Rajaxx", true, true));
    route.waypoints.push_back(RouteWaypoint(-8845.52f, 2260.14f, 21.47f, "Moam", true, true));
    route.waypoints.push_back(RouteWaypoint(-8822.2f, 1625.82f, 19.81f, "Kurinnaxx", true, true));
    
    routes_[509] = route;
    LOG_INFO("module", "Loaded Ruins Of Ahnqiraj route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// TEMPLE OF AHNQIRAJ (Map 531)
// ============================================================================
void DungeonRouteManager::CreateTempleOfAhnqirajRoute() {
    DungeonRoute route;
    route.mapId = 531;
    route.dungeonName = "Temple Of Ahnqiraj";
    
    route.waypoints.push_back(RouteWaypoint(-9023.67f, 1176.24f, -104.23f, "Emperor Vek'nilash", true, true));
    route.waypoints.push_back(RouteWaypoint(-8953.3f, 1233.64f, -99.72f, "The Master's Eye", true, true));
    route.waypoints.push_back(RouteWaypoint(-8868.31f, 1205.97f, -104.23f, "Emperor Vek'lor", true, true));
    route.waypoints.push_back(RouteWaypoint(-8663.46f, 2029.89f, 108.66f, "Arygos", true, true));
    route.waypoints.push_back(RouteWaypoint(-8660.59f, 2022.34f, 108.66f, "Caelestrasz", true, true));
    route.waypoints.push_back(RouteWaypoint(-8652.15f, 2020.91f, 108.66f, "Merithra of the Dream", true, true));
    route.waypoints.push_back(RouteWaypoint(-8598.54f, 2165.3f, -4.02f, "Vem", true, true));
    route.waypoints.push_back(RouteWaypoint(-8587.96f, 2174.21f, -4.15f, "Princess Yauj", true, true));
    route.waypoints.push_back(RouteWaypoint(-8578.79f, 1986.18f, 100.3f, "Eye of C'Thun", true, true));
    route.waypoints.push_back(RouteWaypoint(-8578.65f, 1985.85f, 100.3f, "C'Thun", true, true));
    route.waypoints.push_back(RouteWaypoint(-8566.88f, 2177.17f, -3.98f, "Lord Kri", true, true));
    route.waypoints.push_back(RouteWaypoint(-8515.8f, 1693.65f, -90.48f, "Princess Huhuran", true, true));
    route.waypoints.push_back(RouteWaypoint(-8346.05f, 2080.98f, 125.73f, "The Prophet Skeram", true, true));
    route.waypoints.push_back(RouteWaypoint(-8302.48f, 1657.66f, -29.81f, "Battleguard Sartura", true, true));
    route.waypoints.push_back(RouteWaypoint(-8085.39f, 1196.72f, -91.97f, "Fankriss the Unyielding", true, true));
    route.waypoints.push_back(RouteWaypoint(-7992.36f, 908.19f, -52.62f, "Viscidus", true, true));
    
    routes_[531] = route;
    LOG_INFO("module", "Loaded Temple Of Ahnqiraj route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// KARAZHAN (Map 532)
// ============================================================================
void DungeonRouteManager::CreateKarazhanRoute() {
    DungeonRoute route;
    route.mapId = 532;
    route.dungeonName = "Karazhan";
    
    route.waypoints.push_back(RouteWaypoint(-11240.6f, -1704.27f, 179.32f, "Terestian Illhoof", true, true));
    route.waypoints.push_back(RouteWaypoint(-11179.1f, -1903.99f, 232.0f, "Blizzard (Shade of Aran)", true, true));
    route.waypoints.push_back(RouteWaypoint(-11169.1f, -1908.56f, 165.76f, "The Curator", true, true));
    route.waypoints.push_back(RouteWaypoint(-11165.5f, -1911.71f, 232.01f, "Shade of Aran", true, true));
    route.waypoints.push_back(RouteWaypoint(-11134.0f, -1582.82f, 278.79f, "Netherspite", true, true));
    route.waypoints.push_back(RouteWaypoint(-11126.3f, -1929.11f, 49.33f, "Midnight", true, true));
    route.waypoints.push_back(RouteWaypoint(-11098.8f, -1853.56f, 221.15f, "Echo of Medivh", true, true));
    route.waypoints.push_back(RouteWaypoint(-11003.7f, -1760.19f, 140.25f, "Nightbane", true, true));
    route.waypoints.push_back(RouteWaypoint(-10982.7f, -1877.93f, 81.81f, "Moroes - Tower Steward", true, true));
    route.waypoints.push_back(RouteWaypoint(-10962.2f, -2018.73f, 275.44f, "Prince Malchezaar", true, true));
    route.waypoints.push_back(RouteWaypoint(-10945.9f, -2103.79f, 92.79f, "Maiden of Virtue", true, true));
    
    routes_[532] = route;
    LOG_INFO("module", "Loaded Karazhan route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// NAXXRAMAS (Map 533)
// ============================================================================
void DungeonRouteManager::CreateNaxxramasRoute() {
    DungeonRoute route;
    route.mapId = 533;
    route.dungeonName = "Naxxramas";
    
    route.waypoints.push_back(RouteWaypoint(2517.62f, -2959.38f, 245.64f, "Lady Blaumeux", true, true));
    route.waypoints.push_back(RouteWaypoint(2520.5f, -2955.38f, 245.63f, "Thane Korth'azz", true, true));
    route.waypoints.push_back(RouteWaypoint(2524.32f, -2951.28f, 245.63f, "Baron Rivendare", true, true));
    route.waypoints.push_back(RouteWaypoint(2528.79f, -2948.58f, 245.63f, "Sir Zeliek", true, true));
    route.waypoints.push_back(RouteWaypoint(2642.14f, -3386.96f, 285.49f, "Gothik the Harvester", true, true));
    route.waypoints.push_back(RouteWaypoint(2671.65f, -3489.11f, 261.38f, "Noth the Plaguebringer", true, true));
    route.waypoints.push_back(RouteWaypoint(2758.93f, -3107.12f, 267.68f, "Instructor Razuvious", true, true));
    route.waypoints.push_back(RouteWaypoint(2793.86f, -3707.38f, 276.63f, "Heigan the Unclean", true, true));
    route.waypoints.push_back(RouteWaypoint(2909.0f, -3997.41f, 274.19f, "Loatheb", true, true));
    route.waypoints.push_back(RouteWaypoint(3227.58f, -3378.3f, 311.33f, "Grobbulus", true, true));
    route.waypoints.push_back(RouteWaypoint(3256.36f, -3230.33f, 294.06f, "Patchwerk", true, true));
    route.waypoints.push_back(RouteWaypoint(3283.09f, -3156.96f, 297.79f, "Gluth", true, true));
    route.waypoints.push_back(RouteWaypoint(3308.59f, -3476.29f, 287.16f, "Anub'Rekhan", true, true));
    route.waypoints.push_back(RouteWaypoint(3353.25f, -3620.1f, 261.08f, "Grand Widow Faerlina", true, true));
    route.waypoints.push_back(RouteWaypoint(3511.38f, -3921.58f, 299.51f, "Maexxna", true, true));
    route.waypoints.push_back(RouteWaypoint(3513.84f, -2926.55f, 302.91f, "Thaddius", true, true));
    route.waypoints.push_back(RouteWaypoint(3522.39f, -5236.78f, 137.71f, "Sapphiron", true, true));
    route.waypoints.push_back(RouteWaypoint(3749.68f, -5114.06f, 142.12f, "Kel'Thuzad", true, true));
    
    routes_[533] = route;
    LOG_INFO("module", "Loaded Naxxramas route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// HYJAL (Map 534)
// ============================================================================
void DungeonRouteManager::CreateHyjalRoute() {
    DungeonRoute route;
    route.mapId = 534;
    route.dungeonName = "Hyjal";
    
    route.waypoints.push_back(RouteWaypoint(5084.07f, -1789.03f, 1322.65f, "Lady Jaina Proudmoore", true, true));
    route.waypoints.push_back(RouteWaypoint(5184.97f, -3383.56f, 1638.26f, "Tyrande Whisperwind - High Priestess of Elune", true, true));
    route.waypoints.push_back(RouteWaypoint(5450.54f, -2723.97f, 1485.59f, "Thrall - Warchief", true, true));
    route.waypoints.push_back(RouteWaypoint(5601.94f, -3446.28f, 1577.49f, "Archimonde", true, true));
    
    routes_[534] = route;
    LOG_INFO("module", "Loaded Hyjal route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// SHATTERED HALLS (Map 540)
// ============================================================================
void DungeonRouteManager::CreateShatteredHallsRoute() {
    DungeonRoute route;
    route.mapId = 540;
    route.dungeonName = "Shattered Halls";
    
    
    routes_[540] = route;
    LOG_INFO("module", "Loaded Shattered Halls route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// BLOOD FURNACE (Map 542)
// ============================================================================
void DungeonRouteManager::CreateBloodFurnaceRoute() {
    DungeonRoute route;
    route.mapId = 542;
    route.dungeonName = "Blood Furnace";
    
    route.waypoints.push_back(RouteWaypoint(274.13f, -87.16f, -137.37f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(307.78f, -31.85f, -137.37f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(308.2f, -141.77f, -137.37f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(320.91f, -83.06f, -133.31f, "Magtheridon", true, true));
    route.waypoints.push_back(RouteWaypoint(369.15f, -55.57f, -137.37f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(369.46f, -118.76f, -137.37f, "Hellfire Channeler", true, true));
    
    routes_[542] = route;
    LOG_INFO("module", "Loaded Blood Furnace route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// HELLFIRE RAMPARTS (Map 543)
// ============================================================================
void DungeonRouteManager::CreateHellfireRampartsRoute() {
    DungeonRoute route;
    route.mapId = 543;
    route.dungeonName = "Hellfire Ramparts";
    
    
    routes_[543] = route;
    LOG_INFO("module", "Loaded Hellfire Ramparts route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// MAGTHERIDONS LAIR (Map 544)
// ============================================================================
void DungeonRouteManager::CreateMagtheridonsLairRoute() {
    DungeonRoute route;
    route.mapId = 544;
    route.dungeonName = "Magtheridons Lair";
    
    route.waypoints.push_back(RouteWaypoint(-73.73f, 47.36f, -0.39f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(-73.73f, 47.36f, -0.39f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(-73.73f, 47.36f, -0.39f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(-55.69f, 2.17f, 0.71f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(-31.95f, 40.22f, 0.71f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(-31.76f, -35.84f, 0.71f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(-18.7f, 2.24f, -0.32f, "Magtheridon", true, true));
    route.waypoints.push_back(RouteWaypoint(-8.91f, 68.38f, 22.66f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(-0.18f, 67.19f, 22.83f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(2.0f, -64.4f, 22.86f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(2.7f, -60.94f, 22.9f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(5.55f, -64.08f, 22.92f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(6.13f, 69.07f, 22.91f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(10.34f, -19.79f, 0.71f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(10.48f, 23.99f, 0.71f, "Hellfire Channeler", true, true));
    route.waypoints.push_back(RouteWaypoint(34.31f, 6.96f, 45.12f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(34.59f, -3.04f, 45.12f, "Hellfire Warder", true, true));
    route.waypoints.push_back(RouteWaypoint(36.45f, 2.03f, 45.14f, "Hellfire Warder", true, true));
    
    routes_[544] = route;
    LOG_INFO("module", "Loaded Magtheridons Lair route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// STEAM VAULT (Map 545)
// ============================================================================
void DungeonRouteManager::CreateSteamVaultRoute() {
    DungeonRoute route;
    route.mapId = 545;
    route.dungeonName = "Steam Vault";
    
    
    routes_[545] = route;
    LOG_INFO("module", "Loaded Steam Vault route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// THE UNDERBOG (Map 546)
// ============================================================================
void DungeonRouteManager::CreateTheUnderbogRoute() {
    DungeonRoute route;
    route.mapId = 546;
    route.dungeonName = "The Underbog";
    
    
    routes_[546] = route;
    LOG_INFO("module", "Loaded The Underbog route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// THE SLAVE PENS (Map 547)
// ============================================================================
void DungeonRouteManager::CreateTheSlavePensRoute() {
    DungeonRoute route;
    route.mapId = 547;
    route.dungeonName = "The Slave Pens";
    
    
    routes_[547] = route;
    LOG_INFO("module", "Loaded The Slave Pens route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// SERPENT SHRINE (Map 548)
// ============================================================================
void DungeonRouteManager::CreateSerpentShrineRoute() {
    DungeonRoute route;
    route.mapId = 548;
    route.dungeonName = "Serpent Shrine";
    
    route.waypoints.push_back(RouteWaypoint(-239.84f, -366.49f, -0.74f, "Hydross the Unstable - Duke of Currents", true, true));
    route.waypoints.push_back(RouteWaypoint(29.99f, -922.41f, 42.99f, "Lady Vashj - Coilfang Matron", true, true));
    route.waypoints.push_back(RouteWaypoint(370.83f, -723.94f, -14.01f, "Morogrim Tidewalker", true, true));
    route.waypoints.push_back(RouteWaypoint(376.54f, -438.63f, 29.61f, "Leotheras the Blind", true, true));
    route.waypoints.push_back(RouteWaypoint(456.87f, -539.74f, -7.46f, "Fathom-Lord Karathress", true, true));
    
    routes_[548] = route;
    LOG_INFO("module", "Loaded Serpent Shrine route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// THE EYE (Map 550)
// ============================================================================
void DungeonRouteManager::CreateTheEyeRoute() {
    DungeonRoute route;
    route.mapId = 550;
    route.dungeonName = "The Eye";
    
    route.waypoints.push_back(RouteWaypoint(377.72f, -18.54f, 42.87f, "Al'ar - Phoenix God", true, true));
    route.waypoints.push_back(RouteWaypoint(424.23f, 403.48f, 14.98f, "Void Reaver", true, true));
    route.waypoints.push_back(RouteWaypoint(432.74f, -373.64f, 18.01f, "High Astromancer Solarian", true, true));
    route.waypoints.push_back(RouteWaypoint(785.78f, -20.4f, 48.81f, "Lord Sanguinar - The Blood Hammer", true, true));
    route.waypoints.push_back(RouteWaypoint(785.81f, 19.49f, 48.81f, "Thaladred the Darkener - Advisor to Kael'thas", true, true));
    route.waypoints.push_back(RouteWaypoint(792.41f, -13.24f, 48.81f, "Grand Astromancer Capernian - Advisor to Kael'thas", true, true));
    route.waypoints.push_back(RouteWaypoint(792.72f, 12.78f, 48.81f, "Master Engineer Telonicus - Advisor to Kael'thas", true, true));
    route.waypoints.push_back(RouteWaypoint(795.41f, -1.6f, 48.81f, "Kael'thas Sunstrider - Lord of the Blood Elves", true, true));
    
    routes_[550] = route;
    LOG_INFO("module", "Loaded The Eye route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// ARCATRAZ (Map 552)
// ============================================================================
void DungeonRouteManager::CreateArcatrazRoute() {
    DungeonRoute route;
    route.mapId = 552;
    route.dungeonName = "Arcatraz";
    
    
    routes_[552] = route;
    LOG_INFO("module", "Loaded Arcatraz route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// THE BOTANICA (Map 553)
// ============================================================================
void DungeonRouteManager::CreateTheBotanicaRoute() {
    DungeonRoute route;
    route.mapId = 553;
    route.dungeonName = "The Botanica";
    
    
    routes_[553] = route;
    LOG_INFO("module", "Loaded The Botanica route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// MECHANAR (Map 554)
// ============================================================================
void DungeonRouteManager::CreateMechanarRoute() {
    DungeonRoute route;
    route.mapId = 554;
    route.dungeonName = "Mechanar";
    
    
    routes_[554] = route;
    LOG_INFO("module", "Loaded Mechanar route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// SHADOW LABYRINTH (Map 555)
// ============================================================================
void DungeonRouteManager::CreateShadowLabyrinthRoute() {
    DungeonRoute route;
    route.mapId = 555;
    route.dungeonName = "Shadow Labyrinth";
    
    
    routes_[555] = route;
    LOG_INFO("module", "Loaded Shadow Labyrinth route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// SETHEKK HALLS (Map 556)
// ============================================================================
void DungeonRouteManager::CreateSethekkHallsRoute() {
    DungeonRoute route;
    route.mapId = 556;
    route.dungeonName = "Sethekk Halls";
    
    
    routes_[556] = route;
    LOG_INFO("module", "Loaded Sethekk Halls route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// MANA TOMBS (Map 557)
// ============================================================================
void DungeonRouteManager::CreateManaTombsRoute() {
    DungeonRoute route;
    route.mapId = 557;
    route.dungeonName = "Mana Tombs";
    
    
    routes_[557] = route;
    LOG_INFO("module", "Loaded Mana Tombs route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// AUCHENAI CRYPTS (Map 558)
// ============================================================================
void DungeonRouteManager::CreateAuchenaiCryptsRoute() {
    DungeonRoute route;
    route.mapId = 558;
    route.dungeonName = "Auchenai Crypts";
    
    
    routes_[558] = route;
    LOG_INFO("module", "Loaded Auchenai Crypts route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// OLD HILLSBRAD (Map 560)
// ============================================================================
void DungeonRouteManager::CreateOldHillsbradRoute() {
    DungeonRoute route;
    route.mapId = 560;
    route.dungeonName = "Old Hillsbrad";
    
    
    routes_[560] = route;
    LOG_INFO("module", "Loaded Old Hillsbrad route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// BLACK TEMPLE (Map 564)
// ============================================================================
void DungeonRouteManager::CreateBlackTempleRoute() {
    DungeonRoute route;
    route.mapId = 564;
    route.dungeonName = "Black Temple";
    
    route.waypoints.push_back(RouteWaypoint(434.94f, 739.25f, 15.07f, "High Warlord Naj'entus", true, true));
    route.waypoints.push_back(RouteWaypoint(449.6f, 401.19f, 118.62f, "Shade of Akama", true, true));
    route.waypoints.push_back(RouteWaypoint(497.77f, 184.17f, 94.6f, "Reliquary of the Lost", true, true));
    route.waypoints.push_back(RouteWaypoint(547.54f, 400.19f, 112.87f, "Akama", true, true));
    route.waypoints.push_back(RouteWaypoint(606.64f, 402.17f, 187.17f, "Teron Gorefiend", true, true));
    route.waypoints.push_back(RouteWaypoint(697.41f, 299.38f, 277.53f, "Lady Malande", true, true));
    route.waypoints.push_back(RouteWaypoint(697.49f, 310.65f, 277.53f, "Veras Darkshadow", true, true));
    route.waypoints.push_back(RouteWaypoint(700.74f, 302.72f, 277.53f, "Gathios the Shatterer", true, true));
    route.waypoints.push_back(RouteWaypoint(700.79f, 307.19f, 277.53f, "High Nethermancer Zerevor", true, true));
    route.waypoints.push_back(RouteWaypoint(702.14f, 650.72f, 75.05f, "Supremus", true, true));
    route.waypoints.push_back(RouteWaypoint(705.73f, 304.99f, 353.92f, "Illidan Stormrage - The Betrayer", true, true));
    route.waypoints.push_back(RouteWaypoint(744.3f, 277.08f, 63.84f, "Gurtogg Bloodboil", true, true));
    route.waypoints.push_back(RouteWaypoint(945.31f, 149.08f, 197.16f, "Mother Shahraz", true, true));
    
    routes_[564] = route;
    LOG_INFO("module", "Loaded Black Temple route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// GRUULS LAIR (Map 565)
// ============================================================================
void DungeonRouteManager::CreateGruulsLairRoute() {
    DungeonRoute route;
    route.mapId = 565;
    route.dungeonName = "Gruuls Lair";
    
    route.waypoints.push_back(RouteWaypoint(139.09f, 204.21f, -9.33f, "Kiggler the Crazed", true, true));
    route.waypoints.push_back(RouteWaypoint(142.02f, 198.36f, -10.35f, "Blindeye the Seer", true, true));
    route.waypoints.push_back(RouteWaypoint(145.79f, 190.98f, -11.54f, "High King Maulgar - Lord of the Ogres", true, true));
    route.waypoints.push_back(RouteWaypoint(146.23f, 174.24f, -10.79f, "Krosh Firehand", true, true));
    route.waypoints.push_back(RouteWaypoint(147.27f, 182.56f, -10.86f, "Olm the Summoner", true, true));
    route.waypoints.push_back(RouteWaypoint(239.92f, 359.06f, -4.65f, "Gruul the Dragonkiller", true, true));
    
    routes_[565] = route;
    LOG_INFO("module", "Loaded Gruuls Lair route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// ZULAMAN (Map 568)
// ============================================================================
void DungeonRouteManager::CreateZulamanRoute() {
    DungeonRoute route;
    route.mapId = 568;
    route.dungeonName = "Zulaman";
    
    route.waypoints.push_back(RouteWaypoint(-18.64f, 1150.46f, 18.79f, "Jan'alai - Dragonhawk Avatar", true, true));
    route.waypoints.push_back(RouteWaypoint(16.96f, 1414.6f, 11.93f, "Nalorakk - Bear Avatar", true, true));
    route.waypoints.push_back(RouteWaypoint(117.36f, 923.57f, 33.97f, "Hex Lord Malacrass", true, true));
    route.waypoints.push_back(RouteWaypoint(119.98f, 674.24f, 51.82f, "Zul'jin", true, true));
    route.waypoints.push_back(RouteWaypoint(370.26f, 1135.74f, 6.55f, "Halazzi - Lynx Avatar", true, true));
    route.waypoints.push_back(RouteWaypoint(376.35f, 1407.34f, 75.53f, "Akil'zon - Eagle Avatar", true, true));
    
    routes_[568] = route;
    LOG_INFO("module", "Loaded Zulaman route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// UTGARDE KEEP (Map 574)
// ============================================================================
void DungeonRouteManager::CreateUtgardeKeepRoute() {
    DungeonRoute route;
    route.mapId = 574;
    route.dungeonName = "Utgarde Keep";
    
    
    routes_[574] = route;
    LOG_INFO("module", "Loaded Utgarde Keep route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// UTGARDE PINNACLE (Map 575)
// ============================================================================
void DungeonRouteManager::CreateUtgardePinnacleRoute() {
    DungeonRoute route;
    route.mapId = 575;
    route.dungeonName = "Utgarde Pinnacle";
    
    route.waypoints.push_back(RouteWaypoint(370.12f, -334.9f, 107.28f, "King Tor", true, true));
    route.waypoints.push_back(RouteWaypoint(370.64f, -314.33f, 107.3f, "King Bjorn", true, true));
    route.waypoints.push_back(RouteWaypoint(413.57f, -335.5f, 107.3f, "King Ranulf", true, true));
    route.waypoints.push_back(RouteWaypoint(413.96f, -314.94f, 107.29f, "King Haldor", true, true));
    
    routes_[575] = route;
    LOG_INFO("module", "Loaded Utgarde Pinnacle route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// NEXUS (Map 576)
// ============================================================================
void DungeonRouteManager::CreateNexusRoute() {
    DungeonRoute route;
    route.mapId = 576;
    route.dungeonName = "Nexus";
    
    
    routes_[576] = route;
    LOG_INFO("module", "Loaded Nexus route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// OCULUS (Map 578)
// ============================================================================
void DungeonRouteManager::CreateOculusRoute() {
    DungeonRoute route;
    route.mapId = 578;
    route.dungeonName = "Oculus";
    
    
    routes_[578] = route;
    LOG_INFO("module", "Loaded Oculus route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// SUNWELL PLATEAU (Map 580)
// ============================================================================
void DungeonRouteManager::CreateSunwellPlateauRoute() {
    DungeonRoute route;
    route.mapId = 580;
    route.dungeonName = "Sunwell Plateau";
    
    route.waypoints.push_back(RouteWaypoint(1463.83f, 611.51f, 21.57f, "Brutallus", true, true));
    route.waypoints.push_back(RouteWaypoint(1477.94f, 643.22f, 21.21f, "Madrigosa", true, true));
    route.waypoints.push_back(RouteWaypoint(1662.56f, 622.76f, 28.05f, "Grand Magister Rommath", true, true));
    route.waypoints.push_back(RouteWaypoint(1666.48f, 621.0f, 28.05f, "Lor'themar Theron - Regent Lord of Quel'Thalas", true, true));
    route.waypoints.push_back(RouteWaypoint(1704.22f, 924.76f, 53.16f, "Kalecgos", true, true));
    route.waypoints.push_back(RouteWaypoint(1813.98f, 625.91f, 33.4f, "Lady Sacrolash", true, true));
    route.waypoints.push_back(RouteWaypoint(1816.25f, 625.48f, 69.6f, "M'uru", true, true));
    route.waypoints.push_back(RouteWaypoint(1819.18f, 625.54f, 33.4f, "Grand Warlock Alythess", true, true));
    
    routes_[580] = route;
    LOG_INFO("module", "Loaded Sunwell Plateau route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// MAGISTERS TERRACE (Map 585)
// ============================================================================
void DungeonRouteManager::CreateMagistersTerraceRoute() {
    DungeonRoute route;
    route.mapId = 585;
    route.dungeonName = "Magisters Terrace";
    
    route.waypoints.push_back(RouteWaypoint(-24.61f, -482.02f, -22.13f, "M'uru Sunwell", true, true));
    
    routes_[585] = route;
    LOG_INFO("module", "Loaded Magisters Terrace route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// CULLING OF STRATHOLME (Map 595)
// ============================================================================
void DungeonRouteManager::CreateCullingOfStratholmeRoute() {
    DungeonRoute route;
    route.mapId = 595;
    route.dungeonName = "Culling Of Stratholme";
    
    
    routes_[595] = route;
    LOG_INFO("module", "Loaded Culling Of Stratholme route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// HALLS OF STONE (Map 599)
// ============================================================================
void DungeonRouteManager::CreateHallsOfStoneRoute() {
    DungeonRoute route;
    route.mapId = 599;
    route.dungeonName = "Halls Of Stone";
    
    
    routes_[599] = route;
    LOG_INFO("module", "Loaded Halls Of Stone route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// DRAK THARON KEEP (Map 600)
// ============================================================================
void DungeonRouteManager::CreateDrakTharonKeepRoute() {
    DungeonRoute route;
    route.mapId = 600;
    route.dungeonName = "Drak Tharon Keep";
    
    
    routes_[600] = route;
    LOG_INFO("module", "Loaded Drak Tharon Keep route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// AZJOL NERUB (Map 601)
// ============================================================================
void DungeonRouteManager::CreateAzjolNerubRoute() {
    DungeonRoute route;
    route.mapId = 601;
    route.dungeonName = "Azjol Nerub";
    
    
    routes_[601] = route;
    LOG_INFO("module", "Loaded Azjol Nerub route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// HALLS OF LIGHTNING (Map 602)
// ============================================================================
void DungeonRouteManager::CreateHallsOfLightningRoute() {
    DungeonRoute route;
    route.mapId = 602;
    route.dungeonName = "Halls Of Lightning";
    
    
    routes_[602] = route;
    LOG_INFO("module", "Loaded Halls Of Lightning route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// ULDUAR (Map 603)
// ============================================================================
void DungeonRouteManager::CreateUlduarRoute() {
    DungeonRoute route;
    route.mapId = 603;
    route.dungeonName = "Ulduar";
    
    route.waypoints.push_back(RouteWaypoint(-223.93f, -279.22f, 396.08f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(-204.52f, -297.85f, 396.08f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(-99.03f, 90.03f, 461.17f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(-77.99f, 108.99f, 461.17f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(-59.5f, 88.47f, 461.17f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(69.21f, -379.87f, 437.59f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(96.24f, -379.45f, 437.59f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(326.15f, 332.42f, 436.52f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(326.45f, 304.61f, 436.52f, "Ironwork Cannon", true, true));
    route.waypoints.push_back(RouteWaypoint(420.73f, -15.37f, 409.8f, "Flame Leviathan", true, true));
    route.waypoints.push_back(RouteWaypoint(586.54f, 378.8f, 360.92f, "Ignis the Furnace Master", true, true));
    route.waypoints.push_back(RouteWaypoint(588.0f, -178.0f, 490.0f, "Razorscale", true, true));
    route.waypoints.push_back(RouteWaypoint(886.28f, -12.05f, 409.6f, "XT-002 Deconstructor", true, true));
    route.waypoints.push_back(RouteWaypoint(1567.86f, 129.13f, 427.27f, "Stormcaller Brundir", true, true));
    route.waypoints.push_back(RouteWaypoint(1587.18f, 121.03f, 427.27f, "Steelbreaker", true, true));
    route.waypoints.push_back(RouteWaypoint(1589.54f, 106.99f, 427.42f, "Runemaster Molgeim", true, true));
    route.waypoints.push_back(RouteWaypoint(1797.15f, -24.4f, 448.74f, "Kologarn", true, true));
    route.waypoints.push_back(RouteWaypoint(1852.78f, 81.39f, 342.46f, "General Vezax", true, true));
    route.waypoints.push_back(RouteWaypoint(1956.2f, 49.32f, 411.36f, "Auriaya", true, true));
    route.waypoints.push_back(RouteWaypoint(1980.28f, -25.59f, 329.4f, "Sara", true, true));
    
    routes_[603] = route;
    LOG_INFO("module", "Loaded Ulduar route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// GUNDRAK (Map 604)
// ============================================================================
void DungeonRouteManager::CreateGundrakRoute() {
    DungeonRoute route;
    route.mapId = 604;
    route.dungeonName = "Gundrak";
    
    
    routes_[604] = route;
    LOG_INFO("module", "Loaded Gundrak route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// VIOLET HOLD (Map 608)
// ============================================================================
void DungeonRouteManager::CreateVioletHoldRoute() {
    DungeonRoute route;
    route.mapId = 608;
    route.dungeonName = "Violet Hold";
    
    
    routes_[608] = route;
    LOG_INFO("module", "Loaded Violet Hold route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// OBSIDIAN SANCTUM (Map 615)
// ============================================================================
void DungeonRouteManager::CreateObsidianSanctumRoute() {
    DungeonRoute route;
    route.mapId = 615;
    route.dungeonName = "Obsidian Sanctum";
    
    route.waypoints.push_back(RouteWaypoint(3145.68f, 520.71f, 89.7f, "Vesperon", true, true));
    route.waypoints.push_back(RouteWaypoint(3239.07f, 657.23f, 86.88f, "Tenebron", true, true));
    route.waypoints.push_back(RouteWaypoint(3246.57f, 551.26f, 58.62f, "Sartharion - The Onyx Guardian", true, true));
    route.waypoints.push_back(RouteWaypoint(3363.06f, 525.28f, 98.36f, "Shadron", true, true));
    
    routes_[615] = route;
    LOG_INFO("module", "Loaded Obsidian Sanctum route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// EYE OF ETERNITY (Map 616)
// ============================================================================
void DungeonRouteManager::CreateEyeOfEternityRoute() {
    DungeonRoute route;
    route.mapId = 616;
    route.dungeonName = "Eye Of Eternity";
    
    route.waypoints.push_back(RouteWaypoint(747.61f, 1393.43f, 295.97f, "Malygos", true, true));
    
    routes_[616] = route;
    LOG_INFO("module", "Loaded Eye Of Eternity route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// AHNKAHET (Map 619)
// ============================================================================
void DungeonRouteManager::CreateAhnkahetRoute() {
    DungeonRoute route;
    route.mapId = 619;
    route.dungeonName = "Ahnkahet";
    
    
    routes_[619] = route;
    LOG_INFO("module", "Loaded Ahnkahet route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// VAULT OF ARCHAVON (Map 624)
// ============================================================================
void DungeonRouteManager::CreateVaultOfArchavonRoute() {
    DungeonRoute route;
    route.mapId = 624;
    route.dungeonName = "Vault Of Archavon";
    
    route.waypoints.push_back(RouteWaypoint(-218.73f, -288.57f, 91.55f, "Emalon the Storm Watcher", true, true));
    route.waypoints.push_back(RouteWaypoint(-218.52f, 104.39f, 91.83f, "Koralon the Flame Watcher", true, true));
    route.waypoints.push_back(RouteWaypoint(-43.33f, -288.71f, 91.25f, "Toravon the Ice Watcher", true, true));
    route.waypoints.push_back(RouteWaypoint(138.88f, -101.53f, 91.4f, "Archavon the Stone Watcher", true, true));
    
    routes_[624] = route;
    LOG_INFO("module", "Loaded Vault Of Archavon route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// ICECROWN CITADEL (Map 631)
// ============================================================================
void DungeonRouteManager::CreateIcecrownCitadelRoute() {
    DungeonRoute route;
    route.mapId = 631;
    route.dungeonName = "Icecrown Citadel";
    
    route.waypoints.push_back(RouteWaypoint(-634.69f, 2211.39f, 51.98f, "Lady Deathwhisper", true, true));
    route.waypoints.push_back(RouteWaypoint(-556.01f, 2211.42f, 539.37f, "High Overlord Saurfang", true, true));
    route.waypoints.push_back(RouteWaypoint(-461.51f, 2211.11f, 541.2f, "Deathbringer Saurfang", true, true));
    route.waypoints.push_back(RouteWaypoint(-401.37f, 2211.14f, 42.08f, "Lord Marrowgar", true, true));
    route.waypoints.push_back(RouteWaypoint(-126.29f, 2211.68f, 82.23f, "Highlord Bolvar Fordragon", true, true));
    route.waypoints.push_back(RouteWaypoint(-69.69f, 2155.64f, 30.74f, "Highlord Darion Mograine", true, true));
    route.waypoints.push_back(RouteWaypoint(-49.22f, 2211.06f, 27.9f, "High Overlord Saurfang", true, true));
    route.waypoints.push_back(RouteWaypoint(-48.92f, 2213.55f, 27.99f, "Highlord Tirion Fordring", true, true));
    route.waypoints.push_back(RouteWaypoint(428.6f, -2123.88f, 864.96f, "The Lich King", true, true));
    route.waypoints.push_back(RouteWaypoint(4202.85f, 2484.92f, 383.84f, "Valithria Dreamwalker", true, true));
    route.waypoints.push_back(RouteWaypoint(4203.65f, 2483.89f, 364.96f, "Valithria Dreamwalker", true, true));
    route.waypoints.push_back(RouteWaypoint(4203.65f, 2483.89f, 364.96f, "Valithria Dreamwalker", true, true));
    route.waypoints.push_back(RouteWaypoint(4210.31f, 2484.53f, 364.88f, "Valithria Dreamwalker", true, true));
    route.waypoints.push_back(RouteWaypoint(4267.92f, 3137.26f, 360.59f, "Festergut", true, true));
    route.waypoints.push_back(RouteWaypoint(4356.19f, 3262.9f, 389.48f, "Professor Putricide", true, true));
    route.waypoints.push_back(RouteWaypoint(4445.87f, 3137.31f, 360.59f, "Rotface", true, true));
    route.waypoints.push_back(RouteWaypoint(4624.88f, 2768.31f, 402.19f, "Blood-Queen Lana'thel", true, true));
    route.waypoints.push_back(RouteWaypoint(4680.29f, 2769.24f, 364.17f, "Prince Valanar", true, true));
    route.waypoints.push_back(RouteWaypoint(4682.73f, 2783.42f, 364.17f, "Prince Keleseth", true, true));
    route.waypoints.push_back(RouteWaypoint(4682.89f, 2755.11f, 364.17f, "Prince Taldaram", true, true));
    
    routes_[631] = route;
    LOG_INFO("module", "Loaded Icecrown Citadel route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// FORGE OF SOULS (Map 632)
// ============================================================================
void DungeonRouteManager::CreateForgeOfSoulsRoute() {
    DungeonRoute route;
    route.mapId = 632;
    route.dungeonName = "Forge Of Souls";
    
    route.waypoints.push_back(RouteWaypoint(4899.98f, 2208.16f, 638.82f, "Lady Sylvanas Windrunner - Banshee Queen", true, true));
    
    routes_[632] = route;
    LOG_INFO("module", "Loaded Forge Of Souls route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// TRIAL OF THE CRUSADER (Map 649)
// ============================================================================
void DungeonRouteManager::CreateTrialOfTheCrusaderRoute() {
    DungeonRoute route;
    route.mapId = 649;
    route.dungeonName = "Trial Of The Crusader";
    
    route.waypoints.push_back(RouteWaypoint(503.25f, 137.23f, 418.3f, "Thrall - Warchief", true, true));
    route.waypoints.push_back(RouteWaypoint(503.93f, 141.74f, 418.3f, "Garrosh Hellscream - Overlord of the Warsong Offensive", true, true));
    route.waypoints.push_back(RouteWaypoint(624.14f, 140.84f, 418.3f, "King Varian Wrynn - King of Stormwind", true, true));
    route.waypoints.push_back(RouteWaypoint(624.35f, 137.65f, 418.29f, "Lady Jaina Proudmoore - Ruler of Theramore", true, true));
    
    routes_[649] = route;
    LOG_INFO("module", "Loaded Trial Of The Crusader route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// TRIAL OF THE CHAMPION (Map 650)
// ============================================================================
void DungeonRouteManager::CreateTrialOfTheChampionRoute() {
    DungeonRoute route;
    route.mapId = 650;
    route.dungeonName = "Trial Of The Champion";
    
    route.waypoints.push_back(RouteWaypoint(746.42f, 557.54f, 435.4f, "Highlord Tirion Fordring", true, true));
    
    routes_[650] = route;
    LOG_INFO("module", "Loaded Trial Of The Champion route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// PIT OF SARON (Map 658)
// ============================================================================
void DungeonRouteManager::CreatePitOfSaronRoute() {
    DungeonRoute route;
    route.mapId = 658;
    route.dungeonName = "Pit Of Saron";
    
    route.waypoints.push_back(RouteWaypoint(424.46f, 212.16f, 528.8f, "Lady Sylvanas Windrunner - Banshee Queen", true, true));
    
    routes_[658] = route;
    LOG_INFO("module", "Loaded Pit Of Saron route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// HALLS OF REFLECTION (Map 668)
// ============================================================================
void DungeonRouteManager::CreateHallsOfReflectionRoute() {
    DungeonRoute route;
    route.mapId = 668;
    route.dungeonName = "Halls Of Reflection";
    
    route.waypoints.push_back(RouteWaypoint(5236.67f, 1929.91f, 707.78f, "Lady Sylvanas Windrunner - Banshee Queen", true, true));
    route.waypoints.push_back(RouteWaypoint(5362.46f, 2062.69f, 707.78f, "The Lich King", true, true));
    route.waypoints.push_back(RouteWaypoint(5552.77f, 2262.57f, 733.01f, "The Lich King", true, true));
    
    routes_[668] = route;
    LOG_INFO("module", "Loaded Halls Of Reflection route with {} waypoints", route.waypoints.size());
}

// ============================================================================
// RUBY SANCTUM (Map 724)
// ============================================================================
void DungeonRouteManager::CreateRubySanctumRoute() {
    DungeonRoute route;
    route.mapId = 724;
    route.dungeonName = "Ruby Sanctum";
    
    route.waypoints.push_back(RouteWaypoint(3049.7f, 528.12f, 89.52f, "General Zarithrian", true, true));
    route.waypoints.push_back(RouteWaypoint(3151.39f, 636.85f, 78.74f, "Saviana Ragefire", true, true));
    route.waypoints.push_back(RouteWaypoint(3153.06f, 389.49f, 86.26f, "Baltharus the Warborn", true, true));
    route.waypoints.push_back(RouteWaypoint(3156.0f, 533.81f, 72.99f, "Halion - The Twilight Destroyer", true, true));
    
    routes_[724] = route;
    LOG_INFO("module", "Loaded Ruby Sanctum route with {} waypoints", route.waypoints.size());
}

} // namespace BotAI
