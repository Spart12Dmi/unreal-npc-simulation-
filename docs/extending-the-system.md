# Extending the system

[Documentation index](README.md)

## Add an interaction task

Create a C++ subclass of `UBasedInteractionTask`. The base class is abstract, Blueprint-visible, inline-editable, and default-to-instanced, allowing subclasses to be configured in the component's task array.

| Override | Responsibility |
| :--- | :--- |
| `ExecuteTask()` | Validate action-specific dependencies, start the action, and report completion. |
| `CleanupTask()` | Remove callbacks and release or cancel resources owned by this action. |
| `GetRequiredResources()` | Describe movement/animation resource requirements when applicable. |

Use `Finish(EInteractionTaskState::Succeeded)` or `Finish(EInteractionTaskState::Failed)` for normal outcomes. The base cancellation method reports `Cancelled`.

`Start()`, `Finish()`, and `Reset()` manage the execution state. Subclasses should not implement a second independent state machine for the same lifecycle. Report exactly one terminal outcome for each accepted run; repeated `Finish()` calls after termination have no effect.

## Cleanup contract

`CleanupTask()` is invoked before completion is broadcast. It can also run during UObject destruction without a gameplay completion broadcast.

Remove listeners before stopping an operation that can trigger those listeners. Track request or instance identifiers when the engine resource may be replaced by another operation. The supplied movement and montage tasks demonstrate this pattern.

Clear subclass runtime handles and weak references in cleanup. Base `Reset()` clears only the pawn and lifecycle state; it does not reset custom subclass fields, clear the entire completion delegate, or repeat resource cleanup.

Keep editable configuration separate from runtime data. Use reflected transient/duplicate-transient fields for runtime UObject references when appropriate, and ensure every new execution initializes non-reflected runtime members and cleanup clears them. Do not store an executing pawn or handle in a shared definition as persistent configuration.

## Runtime objects and completion listeners

The extraction node duplicates each configured task with the Blackboard component as Outer. The runtime task array is then stored in the native Struct key. Do not replace duplication with a simple array copy when tasks contain mutable per-NPC execution state.

Bind `OnFinished` before calling `Start()`. Completion can occur inside `Start()`, so callers must support both immediate and delayed outcomes.

Use the delegate's `EInteractionTaskState` parameter to interpret the outcome. The BT runner resets the task during completion handling, so another listener may observe `NotStarted` through `GetState()` after that reset.

Remove only the listener you own through its delegate handle. Clearing the entire multicast delegate would discard other listeners.

## Add a Behavior Tree node

Use `UBTTaskNode` or `UBTTask_BlackboardBase` for actions, and `UBTDecorator` or `UBTDecorator_BlackboardBase` for conditions according to the node's Blackboard needs.

Keep decorator condition calculation free of reservation changes. The reservation task performs the implemented release/clear/select transition when its branch executes.

If a node stores per-NPC runtime members, use `bCreateNodeInstance = true` or implement an appropriate node-memory layout. Shared tree-node members must not hold one NPC's active action state. The existing execute-next node uses instancing.

A new asynchronous runner must detach callbacks on abort, node finish, and instance destruction, and only finish the tree node that still owns the operation.

## Extend selection or schedule behavior

New activity categories can be introduced through registered Gameplay Tags and schedule rows. Components must register the exact tag requested by the schedule.

For hierarchical matching, nearest-object selection, or NPC-specific filtering, update the registry selection policy and the schedule decorator consistently. Current lookup is exact-tag, first-available selection, and its reservation method relies on the lookup capacity check.

For a clock feature such as starting-time configuration or live time-scale changes, update the subsystem explicitly. Current starting time is fixed in the subsystem, and time scale is read only at initialization.

## Integration verification

Before considering a new action complete, verify the behaviors relevant to it:

- Normal completion advances the BT index and leaves the task ready for the next cycle.
- Immediate completion returns a correct BT result without a duplicate latent completion.
- Cancellation removes callbacks and stops only the action's owned operation.
- Two NPCs using the same component have separate task execution state.
- Reselecting an object releases the old NPC slot and resets task data and index.
- Failed selection leaves the NPC without a stale selected object or task list.

These are manual verification criteria. This documentation does not claim that an automated suite or all listed scenarios have been executed.

## Source references

- [Base task API](../Source/SimulationWord/Public/NPCS/Tasks/BasedInteractionTask.h)
- [Movement cleanup example](../Source/SimulationWord/Private/NPCS/Tasks/MoveToInteractionTask.cpp)
- [Animation cleanup example](../Source/SimulationWord/Private/NPCS/Tasks/PlayAnimationInteractionTask.cpp)
- [Task extraction](../Source/SimulationWord/Private/NPCS/BehaviorTree_Tasks/BTTask_ExtractExecutionTasks.cpp)
- [Execute-next runner](../Source/SimulationWord/Private/NPCS/BehaviorTree_Tasks/ImplementTaskTree/BTTask_ExecuteNextTask.cpp)
