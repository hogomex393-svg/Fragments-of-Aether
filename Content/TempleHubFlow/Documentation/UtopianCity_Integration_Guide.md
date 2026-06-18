# UtopianCity Integration Guide

Imported map:

`/Game/UtopianCity/Maps/UtopianCityDemoMap`

## Hub Entrance

Place or create a Blueprint trigger in `L_Temple_Hub` near the UtopianCity portal.

Recommended settings:

| Property | Value |
| --- | --- |
| Open level target | `/Game/UtopianCity/Maps/UtopianCityDemoMap` |
| Mark island completed | false |

## UtopianCity Exit

Place or create another Blueprint trigger at the city endpoint after the relic collection route.

Recommended settings:

| Property | Value |
| --- | --- |
| Open level target | `/Game/IndianTempleStylized/Levels/L_Temple_Hub` |
| Mark island completed | true |
| `IslandID` | 2 |

This marks the cyber city island as complete before returning the player to the Temple Hub.

The existing progress system already supports this through `BP_TempleGameInstance`.
At the city endpoint, get the current GameInstance, cast to `BP_TempleGameInstance`, call
`MarkIslandCompleted(2)`, then open the Temple Hub map.
