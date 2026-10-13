# Unreal NPC Simulation

An NPC scheduling and interaction system implemented in C++ for Unreal Engine 5.7, with Blueprint assets for configuration and demonstration.

This repository focuses on the system itself. It is a technical prototype, **not a complete game built around the system**. The included level is used to demonstrate and test NPC behavior.

## Features

- World clock with configurable simulation speed.
- Daily schedules defined through Data Assets, hours, minutes, and Gameplay Tags.
- Selection of interaction objects based on the current scheduled activity.
- Object reservations with a limited number of NPC slots.
- Behavior Tree nodes for selecting objects and executing tasks in order.
- Movement and animation tasks that reset for repeated execution.
- Editor markers for manually positioning movement destinations.
- A simple widget displaying world time.

## How it works

Each NPC has a daily schedule. The current world time determines its activity, and the activity's Gameplay Tag determines which interaction object it should use.

The NPC reserves an available object and runs its interaction tasks in order. Tasks repeat while that interaction remains relevant. When selecting a new object, the NPC releases its previous reservation so another NPC can use the slot.

Each NPC uses separate runtime task instances. These instances are reset and reused between task cycles.

## Getting started

Requires **Unreal Engine 5.7** and a compatible C++ toolchain. The project has been built on Windows with Visual Studio 2022.

1. Clone or download the repository.
2. Generate project files for [`SimulationWord.uproject`](SimulationWord.uproject), or open it with Rider.
3. Build the `SimulationWordEditor` target using `Win64 / Development`.
4. Open the project in Unreal Editor.
5. Open `Content/ThirdPerson/Lvl_ThirdPerson` and press **Play** to run the demonstration.

## Basic setup

**World time:** Adjust `Time Scale` under **Project Settings > Simulation World Settings**. A value of `60` advances the clock by one game minute per real second.

**Schedules:** Create a `UScheduleTemplateDataAsset`, add activities sorted by start time, and assign it to the NPC's `NPC Schedule` property. The project includes `DA_WorkSchedule` as an example.

**Interaction objects:** Add a `SmartInteractionComponent` to an actor. Set its `Smart Tag`, `Max Workers`, and ordered `Interaction Tasks`. Schedule tags must match object tags exactly.

**Movement:** Set the task's `Destination` in world coordinates, or select its marker in the viewport and move it with the translation gizmo. The destination must be reachable through the level's navigation mesh.

**Animation:** Assign a montage and ensure the NPC's Animation Blueprint contains the matching montage slot.

## Source code

- [`Source/SimulationWord`](Source/SimulationWord) contains the runtime system: NPCs, schedules, subsystems, interaction tasks, and Behavior Tree nodes.
- [`Source/SimulationWordEditor`](Source/SimulationWordEditor) contains the movement destination visualizer.
- [`Content/Blueprints`](Content/Blueprints) contains the demonstration's NPC, AI, schedule, and UI assets.

## Technical documentation

See [docs](docs/README.md) for architecture, runtime execution, configuration, and extension guidance.

The system is under active development.
