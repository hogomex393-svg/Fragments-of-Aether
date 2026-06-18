# Temple Island Progress GameInstance Guide

`BP_TempleGameInstance` is the central state holder for the three island completion flags.

## Variables

| Variable | Type | Default |
| --- | --- | --- |
| `bIsland01Completed` | Boolean | false |
| `bIsland02Completed` | Boolean | false |
| `bIsland03Completed` | Boolean | false |
| `CompletedIslandCount` | Integer | 0 |
| `bAllIslandsCompleted` | Boolean | false |
| `LastCompletedIslandID` | Integer | 0 |

Island IDs:

| ID | Island |
| --- | --- |
| 1 | Forest mechanism island |
| 2 | Cyber city island |
| 3 | Underground ruins chamber |

## Functions

| Function | Purpose |
| --- | --- |
| `MarkIslandCompleted(IslandID)` | Marks one island complete. Idempotent, so repeated calls do not increase the count again. |
| `IsIslandCompleted(IslandID)` | Returns whether one island is complete. |
| `GetCompletedIslandCount()` | Returns 0-3. |
| `CheckAllIslandsCompleted()` | Updates `bAllIslandsCompleted`. |
| `AreAllIslandsCompleted()` | Returns `bAllIslandsCompleted`. |
| `ResetIslandProgress()` | Debug reset for all island state. |

## Event Dispatchers

| Dispatcher | Trigger |
| --- | --- |
| `OnIslandCompleted(IslandID)` | Fires only when an island is completed for the first time. |
| `OnAllIslandsCompleted` | Fires once when all three islands first become complete. |

## Temporary Test Trigger

`BP_TestIslandCompleteTrigger` is a temporary helper under `Content/TempleHubFlow/Blueprints`.

Place three copies in a test area and set:

| Copy | IslandID |
| --- | --- |
| Trigger 1 | 1 |
| Trigger 2 | 2 |
| Trigger 3 | 3 |

When the player overlaps one trigger, it calls:

`MarkIslandCompleted(IslandID)`

It prints:

`Island X Completed`

When all three are complete, it also prints:

`All Islands Completed - Central Gate Can Open`

## Later Integration

When formal island maps are delivered, each island endpoint should get the current GameInstance, cast to `BP_TempleGameInstance`, then call:

`MarkIslandCompleted(1)`, `MarkIslandCompleted(2)`, or `MarkIslandCompleted(3)`.

The central hub can call `IsIslandCompleted(1/2/3)` to light each rune or energy pillar, and `AreAllIslandsCompleted()` to open the central temple gate.
