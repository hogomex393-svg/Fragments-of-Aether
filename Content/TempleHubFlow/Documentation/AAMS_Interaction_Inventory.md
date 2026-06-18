# AAMS Interaction Inventory

Scope: scanned project assets whose paths contain Interact, Interaction, Popup, Widget, Teleporter, Door, Lever, or Button.

## Best candidates to reference

| Asset | Usefulness | Notes |
| --- | --- | --- |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Components/AC_Interact_AAMS` | High | AAMS interaction component. Good reference for how the main character expects interactable objects to behave. Not modified. |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Interfaces/ActorComponentInterfaces/BPI_AAMS_Interaction` | High | Main interaction interface candidate. Useful if the dialogue system later needs to fully integrate with AAMS interaction prompts. |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Interfaces/AAMS_ObjectInterfaces/BPI_AAMS_InteractObject` | High | Object-side interaction interface candidate. Useful reference for custom interactables. |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Actors/UtilityActors/BP_InteractPopUp` | Medium | Existing popup behavior reference. The temple dialogue system currently uses its own lightweight C++ widget to avoid modifying AAMS. |
| `/Game/AAMovementSystem/Demo/UI/W_Interact` | Medium | Existing interaction prompt widget. Good visual reference for "Press E" style prompts. |
| `/Game/AAMovementSystem/Demo/UI/W_JumpInteract` | Low | Specialized prompt variant. Reference only. |
| `/Game/AAMovementSystem/Demo/UI/W_TutorialPopup` | Low | Tutorial popup reference, not needed for core dialogue. |
| `/Game/AAMovementSystem/Demo/UI/WB_PopupNotification` | Medium | Existing notification widget reference. |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Actors/Interactables/BP_Teleporter` | High | Candidate for later portal unlock flow. Not modified by the current implementation. |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Actors/Interactables/BP_TeleporterLevel` | High | Candidate for level-changing portal flow. Not modified by the current implementation. |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Actors/Interactables/BP_Door` | Medium | Good reference for post-dialogue gate logic. |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Actors/Interactables/Triggers/BP_TriggerButton` | Medium | Good reference for trigger-style interaction behavior. |
| `/Game/AAMovementSystem/AAMovementSystem/Blueprints/Actors/Interactables/Triggers/BP_TriggerLever` | Medium | Good reference for trigger-style interaction behavior. |

## Current implementation decision

The former C++ temple opening dialogue trigger has been removed. The character-select flow, opening dialogue, and level-select handoff are now expected to be handled in Blueprint.

## Later integration path

If the temple dialogue should match the AAMS interaction stack exactly, the next step is to implement the AAMS interaction interface on a TempleHubFlow blueprint or C++ actor and reuse the existing AAMS prompt widgets. That should be done after confirming the relevant AAMS interface signatures in the Blueprint editor.
