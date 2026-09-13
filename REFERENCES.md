# External references

Community data sources and tools for MuffinCore work. **Links and notes only; don't copy third-party data into this repo** unless its license allows it. Use them locally to cross-check generated data and fill design gaps.

Status: `new` = not yet evaluated · `evaluated` = see notes · `in use` = actively used by a task

| Source | What it is (first look) | License | Likely use | Status |
|---|---|---|---|---|
| [jealous-sound/azerothcore-wotlk-coa](https://github.com/jealous-sound/azerothcore-wotlk-coa) | Upstream server source for the CoA Repack (AzerothCore fork + mod-ascension-compat) | AGPL-3.0 (AzerothCore) — module license to confirm | Update source; issue tracker for #17, #21–#35 | evaluating (hosting research) |
| [f3rr311/CoA-Databank](https://github.com/f3rr311/CoA-Databank) | Archive created 2026-09-12: coabuildhub.com scrape (captured 2026-07-31), client MPQ/DBC palette (captured 2026-07-29), provenance manifests with SHA-256. 21 classes, 3,612 talent nodes, 2,909 skills, 128 builds, 238,888 spells with dependencies, 5,269 creatures, 119,084 DBC rows. Compressed JSONL/JSON. Made for the "ByteCOA" rebuild | **None stated** (all rights reserved by default); underlying data is Ascension's | Cross-check `AscensionCoATalentData.h` (3,618 entries vs 3,612 nodes); spell dependencies for classes without dedicated code (Stormbringer, Chronomancer, Bloodmage) | new |
| [AscensionDB archive](https://ascension-db.ascension-archive.workers.dev/) | Community preservation archive ("not an official or live Ascension service"): items, spells, quests; searchable by name/ID; keeps conflicting captures separate. Sources: client captures, Exiles DB, BisBeard addon, Wayback. Open data said to be on GitHub | See its GitHub | Look up spell/item/talent details when fixing class mechanics | new |
| [azerothhub.com](https://azerothhub.com/) | not yet evaluated | ? | ? | new |
| [Tareksoh/Worldforged-data](https://github.com/Tareksoh/Worldforged-data) | not yet evaluated | ? | ? | new |
| [windymindy/wowchat_ascension — HandshakeAscension.scala](https://github.com/windymindy/wowchat_ascension/blob/main/src/main/scala/wowchat/realm/HandshakeAscension.scala) | Chat-bot client implementing Ascension's realm (logon) handshake | ? | Likely documents Ascension's encrypted auth challenge. Relevant to hosting (F1): the server currently can't read usernames from unpatched Ascension clients and maps them all to account `LOCAL` (`AuthSession.cpp:312`) | new — high priority |
| [synthalorian/open_ascension](https://github.com/synthalorian/open_ascension) | not yet evaluated (name suggests an open Ascension client/server reimplementation) | ? | ? | new |
| [ascension.help](https://ascension.help/) | not yet evaluated | ? | ? | new |
| [Nighthawk42/ascension-db](https://github.com/Nighthawk42/ascension-db) | not yet evaluated (possibly the open data behind the AscensionDB archive) | ? | ? | new |
| [Duff-SPP/AcensionOfflineDatabase](https://github.com/Duff-SPP/AcensionOfflineDatabase) | not yet evaluated (offline Ascension database) | ? | ? | new |
| [DustinHendrickson/mod-classless-wildcard](https://github.com/DustinHendrickson/mod-classless-wildcard) | AzerothCore module for classless / wildcard play (to evaluate: approach, license, AzerothCore version) | ? | U1: classless CoA mode (currently mapped to a Warrior shell) | new — batch 2 |
| [coatavern.com/items](https://coatavern.com/items) | not yet evaluated (CoA item database) | ? | Item data cross-check | new |
