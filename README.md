# ![logo](https://community.trinitycore.org/public/style_images/1_trinitycore.png) TrinityCore Single (3.3.5)

## Last update

Release: TDB 335.64

Month:   October 2018 (last commit: [Add rotation to .gob info](https://github.com/TrinityCore/TrinityCore/commit/257ae44a20ed40469dd821845de8e27c0af838c7))

## Introduction

TrinityCore Single is a personal fork of [TrinityCore](https://www.trinitycore.org) I made it on my freetime, as an attempt to play World of Warcraft as a Single-Player game (and also as a technical challenge!).

## Additions/Changes from TrinityCore

Here is the list of changes I made to the core:

- SpeedGame: increase or reduce the time speed in the game.
- NoCastTime: ignore casting time.
- NoCooldown: ignore cooldowns (with NoCastTime, leads to fireball burst shooting!).
- HurtInRealTime: disable auto-attack, 1 click = 1 attack.
- AttackSpeedForPlayer: increase or reduce auto-attack speed for the player.
- AttackSpeedforMobs: same than above but for the bad guys.
- FastFishing: catch instantly all fishes!
- GainHonorOnGuardKill: gain honor by killing guards.
- GainHonorOnEliteKill: gain honor by killing Elite mobs.
- RespawnSpeed: increase or reduce the respawn time of the defeated mobs.
- AutoCompleteQuestDelay: on quest completion, the quest giver will be summoned on your position to allow you to terminate the quest. The delay is the time in seconds until when the NPC will depop.
- SmartQuests System: quest items can be looted (from mobs and/or containers in the world) even if you don't own the quest. That means you can cross a land, kill monsters (they will be counted if later you catch some quest requiring you to kill them!), get the quest loot, and when someone gives you a quest for those items, you won't have to go killing the same monsters again.
- LootOnlyForPlayer: only loot weapons and armors that fit to the player (no more hammer for a thief!).
- MinimumRates for loot: set a minimum chance for the loot (some really rare weapons have a blizzlike chance of 0.000001%, which means nothing on a Single-Player game).
- AutoMount: some actions requires you to get off your mount (like mining or opening a chest), and it's boring. So with this mod, you'll automatically summon your mount when you finish looting.

## NPCBots

I'm using NPCBots (credit to [livingsacrifice](https://github.com/livingsacrifice/TrinityCore/tree/npcbots)) updated for the last TDB release and with some customizations:
- Reduced damages: sometimes the bots were dealing way too much damages. I fixed that.
- No hostility: bots are not hostile against mobs. That means if you're a thief, you can sneak around in peace!
- More control to bots: new commands allowing to stop bots following you or even taking part to the fight (can be useful for some quests). 

## Additional notes

Feel free to use my fork and open issues if you see strange things related to my work.
