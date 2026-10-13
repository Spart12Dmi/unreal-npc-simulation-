# Configuration and setup

[Documentation index](README.md)

## Build and launch

The project targets Unreal Engine **5.7**. The demonstrated build environment is Windows with Visual Studio 2022 and Unreal's required C++ toolchain. Rider can be used as the IDE with the toolchain installed.

1. Generate project files from `SimulationWord.uproject`, or load the project through Rider's Unreal integration.
2. Build `SimulationWordEditor` for `Win64` with the `Development` configuration.
3. Open the project in Unreal Editor.
4. Open `Content/ThirdPerson/Lvl_ThirdPerson`, the configured editor startup and game default map.
5. Press Play to run the demonstration.

The editor target includes both the runtime and editor modules. The editor visualizer registers at `PostEngineInit`. Restart the editor after changing native visualizer code to load the new implementation.

## Simulation time

`UWorldTimeSubsystem` starts at 08:00 and advances its `FTimespan` with:

```text
simulation delta seconds = world tick DeltaTime * TimeScale
```

`TimeScale` is read from `USimulationWorldSettings` when the subsystem initializes. Editing the setting during an existing session does not automatically update the subsystem's cached scale; restart the play session to apply it.

| Value | Effect |
| :--- | :--- |
| `0` | Stops the custom simulation clock. |
| `1` | One simulation second per world tick second. |
| `60` | One simulation minute per world tick second. |
| `1000` | The value currently saved in project config for fast demonstration. |

The C++ settings default is `20.0`; project config overrides it. The setting's editor metadata clamps its lower bound to zero.

Configuration location:

```ini
[/Script/SimulationWord.SimulationWorldSettings]
TimeScale=1000.000000
```

This lives in [DefaultGame.ini](../Config/DefaultGame.ini) and is exposed as **Simulation World Settings** in Project Settings.

The custom time scale advances schedule time. It does not directly change navigation speed, montage play rate, or Unreal timer durations. The clock accumulates elapsed days; daily schedule evaluation uses only its time-of-day components.

## NPC and controller

Use an NPC derived from `ABaseNPC` and a controller derived from `ABaseAiController`.

Configure the NPC's AI controller class and automatic possession for the way it is placed or spawned. Assign its `NPCSchedule` asset. On the controller, set the `BehaviorTree` property.

`OnPossess()` stores the pawn as `NPCOwner`. `BeginPlay()` attempts to start the configured tree. Ensure possession has established the expected NPC before executing the project-specific BT tasks.

Existing demonstration assets:

| Asset path under `Content` | Purpose |
| :--- | :--- |
| `Blueprints/NPCs/BP_BaseNPC` | NPC Blueprint. |
| `Blueprints/NPCs/BP_BaseNPCAIController` | Controller Blueprint. |
| `Blueprints/NPCs/AI/BB_BaseNPC` | Blackboard asset. |
| `Blueprints/NPCs/AI/BTT_BaseNPC_Main` | Main AI tree asset. |
| `Blueprints/NPCs/AI/BTT_SmartObjectExecution` | Execution tree asset. |
| `Blueprints/NPCs/Schedules/DA_WorkSchedule` | Example schedule. |

Verify the Blackboard has the [native schema](runtime-execution.md): `SelectedInteraction`, `ExecutionTasks`, and `CurrentTaskIndex`. Follow the runtime document when wiring tree repetition and the reservation decorator.

## Schedules and Gameplay Tags

Create a Data Asset using `UScheduleTemplateDataAsset`. Each `FScheduleActivityRow` contains:

| Property | Meaning | Editor range |
| :--- | :--- | :--- |
| `StartHour` | Activity start hour | `0` to `23` |
| `StartMinute` | Activity start minute | `0` to `59` |
| `ScheduleTag` | Requested interaction category | Registered Gameplay Tag |

Rows must be sorted by ascending start time. Activities remain active until the next entry; before the first entry, the final row from the previous day remains active.

Example configuration, not a claim about the contents of `DA_WorkSchedule`:

| Start | Tag |
| :--- | :--- |
| 08:00 | `Interaction.Work.Factory` |
| 12:00 | `Interaction.Food` |
| 13:00 | `Interaction.Work.Factory` |
| 18:00 | `Interaction.Rest` |
| 22:00 | `Interaction.Sleep` |

Registered project tags include `Interaction.Food`, `Interaction.Rest`, `Interaction.Shop`, `Interaction.Sleep`, `Interaction.Work`, and `Interaction.Work.Factory`. Add or maintain tags in [DefaultGameplayTags.ini](../Config/DefaultGameplayTags.ini) or the Gameplay Tags settings editor.

Assign the same exact tag to the corresponding interaction component. Parent-tag matching is not implemented by registry lookup or the schedule decorator.

## Interaction actors

1. Add `USmartInteractionComponent` to an actor.
2. On the placed actor, configure `SmartTag` and `MaxWorkers`. These properties are instance-editable.
3. Add task objects to `InteractionTasks` in execution order. This array is editable and instanced.
4. Configure the movement or animation properties on each task.

`MaxWorkers` defaults to one. A nonpositive value makes the component unavailable under the current capacity check.

The component registers its tag during initialization. Changing tags on already initialized components does not reindex them automatically. Configure tags before starting the simulation.

An empty task list can be extracted successfully, but execution and index advancement fail when there is no task to run. Supply at least one task for an executable interaction.

### Movement

`UMoveToInteractionTask` exposes:

| Property | C++ default | Meaning |
| :--- | :--- | :--- |
| `Destination` | `(0, 0, 0)` | World-space target location. |
| `AcceptanceRadius` | `50.0` cm | Distance accepted as reaching the destination. |

Provide navigation coverage, typically with a Nav Mesh Bounds Volume, and keep the destination reachable. The request disallows partial paths, so the end of a partial route is not accepted as the destination.

The destination is an absolute world position. Moving the interaction actor does not automatically translate its task destinations.

### Animation

`UPlayAnimationInteractionTask` exposes:

| Property | C++ default | Meaning |
| :--- | :--- | :--- |
| `Montage` | Null | Montage to play on the NPC's AnimInstance. |
| `PlayRate` | `1.0` | Positive playback-rate multiplier. |
| `CancelBlendOutTime` | `0.15` seconds | Nonnegative blend-out duration on unsuccessful termination. |

The NPC must be a character with a skeletal mesh and AnimInstance. Configure the Animation Blueprint with a slot matching the montage. A root-motion montage reports Movement in addition to Animation through its resource flags.

## Destination visualizer

The editor module registers a visualizer for `USmartInteractionComponent`. It draws a cyan diamond and point for each valid movement task's destination, plus a line to the actor. The selected destination is yellow.

To edit a destination:

1. Select the interaction actor in the level viewport.
2. Click the destination marker.
3. Activate translation with **W** and drag the gizmo.
4. Save the level to retain the actor instance's changes.

The visualizer writes directly to `UMoveToInteractionTask::Destination` and records a scoped transaction for undo. It keeps the actor selected because selecting the non-scene interaction component alone hides Unreal's transform widget.

Only movement-task destinations are visualized. Other vector properties are unaffected. A destination left at its default appears at the world origin, even if the interaction actor is elsewhere.

## Clock widget

Create or use a Widget Blueprint derived from `UWorldTimeWidget`. Its widget hierarchy must include a TextBlock named **`WorldTimeText`**, matching the required `BindWidget` member.

Assign that Widget Blueprint to the simulation game mode's `WidgetClass`. The game mode creates it for the first player controller and adds it to the viewport during `BeginPlay()`.

The widget caches the subsystem and the last displayed string. It checks for a change each UI tick and updates the TextBlock only when the string changes.

## Source references

- [Project descriptor](../SimulationWord.uproject)
- [Settings declaration](../Source/SimulationWord/Public/Core/Settings/SimulationWorldSettings.h)
- [NPC declaration](../Source/SimulationWord/Public/NPCS/BaseNPC.h)
- [Controller lifecycle](../Source/SimulationWord/Private/NPCS/BaseAiController.cpp)
- [Task configuration](../Source/SimulationWord/Public/NPCS/Tasks)
- [Editor visualizer](../Source/SimulationWordEditor/Private/SimulationWordEditor.cpp)
- [Widget binding](../Source/SimulationWord/Public/Widgets/WorldTimeWidget.h)
