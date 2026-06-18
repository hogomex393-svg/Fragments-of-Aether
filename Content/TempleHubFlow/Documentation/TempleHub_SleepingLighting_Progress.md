# Temple Hub Sleeping Lighting Progress

Date: 2026-05-27

## Accepted State

The central Hub currently shows the accepted pre-completion "sleeping temple" lighting state.

Modified level assets:

- `/Game/IndianTempleStylized/Levels/L_Temple_Hub`
- `/Game/IndianTempleStylized/Levels/Level_Temple`

`L_Temple_Hub` owns the main directional light, sky light, and post-process volume.  
`Level_Temple` is a streamed temple layer used by the Hub and owns the three `RectLight` fill lights.

## Current Sleeping Lighting Values

| Actor | Property | Sleeping value |
| --- | --- | --- |
| `Light Source` | Intensity | `1.35` |
| `Light Source` | Color | Cool blue moonlight |
| `SkyLight` | Intensity | `0.50` |
| `SkyLight` | Color | Cool blue ambience |
| `RectLight`, `RectLight2`, `RectLight3` | Intensity | `0.75` |
| `RectLight`, `RectLight2`, `RectLight3` | Color | Low cool fill light |
| `PostProcessVolume` | Exposure method | Histogram auto exposure override |
| `PostProcessVolume` | Exposure bias | `-0.45` |
| `PostProcessVolume` | Saturation | Slightly cold/desaturated |

An earlier fixed-exposure attempt was too dark to play. The accepted state preserves visible architecture and paths while remaining clearly dimmer than the future awakened state.

## Restore Assets

Backups captured before the sleeping lighting pass:

- `/Game/TempleHubFlow/Maps/L_Temple_Hub_BeforeSleepingLighting`
- `/Game/TempleHubFlow/Maps/Level_Temple_BeforeSleepingLighting`

`/Game/TempleHubFlow/Maps/Level_Lighting_BeforeSleepingLighting` was produced while locating actor ownership and is not needed for restoring the accepted Hub lighting work.

## Scripts

Apply accepted sleeping lighting:

- `Scripts/apply_temple_hub_sleeping_lighting.py`
- `Scripts/apply_temple_hub_sleeping_temple_layer.py`

Restore pre-lighting values:

- `Scripts/restore_temple_hub_original_lighting.py`
- `Scripts/restore_temple_hub_original_temple_layer.py`

Inspection utilities:

- `Scripts/inspect_temple_hub_lighting.py`
- `Scripts/inspect_temple_hub_postprocess.py`
- `Scripts/inspect_temple_hub_actor_ownership.py`

## Next Work

Create the completed/awakened lighting state and a Hub atmosphere controller driven by the existing temple progress state:

- Read `BP_TempleGameInstance` completion count on Hub load.
- Use current lighting as state `0/3`.
- Add bright awakened state for `3/3`.
- Optionally create intermediate rune and local-light feedback for `1/3` and `2/3`.

