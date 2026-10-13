# Unreal NPC Simulation

An NPC scheduling and interaction system for **Unreal Engine 5.7**. Runtime logic is implemented in C++, with Blueprint assets for configuring NPCs, schedules, and interactions.

The project focuses on the NPC system rather than a complete game built around it. The included level provides a demonstration of scheduled activities and object interactions.

## Features

- **World time:** a configurable simulation clock with an on-screen time display.
- **Daily schedules:** activities defined in Data Assets using start times and Gameplay Tags.
- **Interaction selection:** NPCs find objects that match their current scheduled activity.
- **Reservations:** each object has a configurable number of available NPC slots.
- **Task execution:** Behavior Trees coordinate selection and sequential movement or animation tasks.
- **Editor tools:** movement destinations can be positioned directly in the viewport.

## How it works

Each NPC follows a schedule that maps times of day to activity tags. At the current world time, the NPC selects an available interaction object with the matching tag and reserves a slot.

The object's tasks define the actions to perform, such as moving to a destination and playing an animation. Tasks run in order and reset for repeated execution. When the NPC selects another object, it releases its previous reservation so the slot becomes available again.

## Getting started

Requires Unreal Engine 5.7 and a compatible C++ toolchain. The project has been built on Windows with Visual Studio 2022; Rider can also be used as the IDE.

1. Clone or download the repository.
2. Generate project files for [`SimulationWord.uproject`](SimulationWord.uproject), or open it with Rider.
3. Build the `SimulationWordEditor` target with the **Win64 / Development** configuration.
4. Open the project in Unreal Editor.
5. Open `Content/ThirdPerson/Lvl_ThirdPerson` and press **Play** to run the demonstration.

## Basic setup

**World time:** Set `Time Scale` under **Project Settings > Simulation World Settings**. A value of `60` runs the simulation clock at 60 times the normal rate.

**Schedules:** Create a `UScheduleTemplateDataAsset`, add activities in ascending start-time order, and assign it to the NPC's `NPC Schedule`. The project includes `DA_WorkSchedule` as an example.

**Interaction objects:** Add a `SmartInteractionComponent` to an actor. Set its `Smart Tag`, `Max Workers`, and ordered `Interaction Tasks`. Schedule tags must match object tags exactly.

**Movement:** Set `Destination` in world coordinates or move its viewport marker with the translation gizmo. Provide navigation coverage so the NPC can reach the target.

**Animation:** Assign a montage and ensure the NPC's Animation Blueprint contains the matching montage slot.

## Project structure

- [`Source/SimulationWord`](Source/SimulationWord) — runtime subsystems, NPCs, schedules, interaction tasks, and Behavior Tree nodes.
- [`Source/SimulationWordEditor`](Source/SimulationWordEditor) — movement destination visualizer.
- [`Content/Blueprints`](Content/Blueprints) — demonstration NPCs, AI assets, schedules, and UI.

## Technical documentation

The [documentation](docs/README.md) covers architecture, runtime execution, configuration, and extending the system.

The system is under active development.
