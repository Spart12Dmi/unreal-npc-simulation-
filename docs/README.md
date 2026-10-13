# Technical documentation

This documentation describes the NPC scheduling and interaction system implemented in this repository. The project is a technical prototype with a demonstration level, not a complete game built around the system.

The documents describe current C++ behavior and configuration requirements. Behavior Tree asset wiring is an integration concern; reference layouts shown here do not certify the contents of binary `.uasset` files.

## Contents

| Document | Coverage |
| :--- | :--- |
| [Architecture](architecture.md) | Modules, responsibilities, data flow, object ownership, and reservation rules. |
| [Runtime execution](runtime-execution.md) | Behavior Tree nodes, Blackboard schema, task lifecycle, completion, cancellation, and reset. |
| [Configuration](configuration.md) | Build requirements, NPC setup, schedules, tags, interaction objects, navigation, animation, and editor tooling. |
| [Extending the system](extending-the-system.md) | Implementing new tasks, cleanup contracts, runtime state, and integration checks. |

## Terminology

| Term | Meaning in this project |
| :--- | :--- |
| Activity | The Gameplay Tag selected from an NPC's schedule at the current world time. |
| Interaction object | An actor exposing a `USmartInteractionComponent` with a tag, capacity, and task list. |
| Reservation | Membership of the NPC in an interaction component's `Workers` array. |
| Task definition | An editable task object stored in the component's instanced task array. |
| Runtime task | A task object duplicated during extraction for a particular NPC's execution. |
| Task cycle | Repeated execution of runtime tasks in array order, with the index wrapping to zero. |

The project's smart interactions are implemented by its own component and subsystem. The inspected runtime module does not depend on Unreal's Smart Objects plugin.

## Source organization

- [Runtime module](../Source/SimulationWord): subsystems, settings, schedules, NPCs, interaction components, tasks, and UI.
- [Editor module](../Source/SimulationWordEditor): the movement destination visualizer.
- [Blueprint assets](../Content/Blueprints): demonstration configuration and AI assets.
- [Project configuration](../Config): world-time settings, Gameplay Tags, and engine configuration.

[Back to the project README](../README.md)
