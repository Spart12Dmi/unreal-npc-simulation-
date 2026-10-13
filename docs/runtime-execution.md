# Runtime execution

[Documentation index](README.md)

## Blackboard schema

All three keys are declared by `UNPCBlackboardData` with instance synchronization disabled.

| Key | Key type | Default | Meaning |
| :--- | :--- | :--- | :--- |
| `SelectedInteraction` | Object, restricted to `USmartInteractionComponent` | Null | The NPC's currently selected interaction component. |
| `ExecutionTasks` | Struct containing `FExecutionTasksData` | Empty task array | Runtime task objects extracted for this NPC. |
| `CurrentTaskIndex` | Int | `0` | Index of the next task to execute. |

Use the constants on `UNPCBlackboardData` rather than duplicating key-name strings.

### Reading and writing task data

Relevant includes:

```cpp
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Struct.h"
#include "NPCS/Blackboard/ExecutionTasksData.h"
#include "NPCS/Blackboard/NPCBlackboardData.h"
#include "StructUtils/StructView.h"
```

Read the native Struct key through its key type:

```cpp
const FConstStructView Value =
    Blackboard->GetValue<UBlackboardKeyType_Struct>(
        UNPCBlackboardData::ExecutionTasksKey);

const FExecutionTasksData* Data = Value.GetPtr<const FExecutionTasksData>();
if (!Data || Data->Tasks.IsEmpty())
{
    return EBTNodeResult::Failed;
}
```

The view refers to the Blackboard's current storage. Treat it as a short-lived read and reacquire it after replacing the value.

Reset the task list explicitly:

```cpp
const FExecutionTasksData EmptyTasks;
Blackboard->SetValue<UBlackboardKeyType_Struct>(
    UNPCBlackboardData::ExecutionTasksKey,
    FConstStructView::Make(EmptyTasks));
Blackboard->SetValueAsInt(UNPCBlackboardData::CurrentTaskIndexKey, 0);
```

Writing an empty struct resets the task list; setting the index to zero prepares it for the next interaction.

## Behavior Tree node contracts

| Node | Inputs | Successful effect | Failure conditions |
| :--- | :--- | :--- | :--- |
| `UBTTask_ReserveSmartInteraction` | NPC schedule, world time, registry, Blackboard | Releases the old reservation, reserves a matching available object, clears old tasks, resets the index | Missing dependencies or no available matching object. |
| `UBTTask_ExtractExecutionTasks` | `SelectedInteraction`, component task definitions | Duplicates definitions and stores the runtime task array | Missing interaction, invalid definition, or failed Blackboard write. |
| `UBTTask_ExecuteNextTask` | `ExecutionTasks`, `CurrentTaskIndex`, NPC | Starts one task and completes with its result | Invalid index or task, task not ready, rejected start, or unsuccessful completion. |
| `UBTTask_IncrementTaskIndex` | Nonempty task array and valid Int index | Writes `(CurrentIndex + 1) % Tasks.Num()` | Invalid key types, missing data, empty array, or invalid index. |

`ExecuteNextTask` executes one action. It neither increments the index nor starts the following task. Array wraparound does not restart a BT node by itself; the tree must revisit the execution branch.

Extraction validates and duplicates definitions into a local array before writing the new array to Blackboard. If the selected component is missing, it explicitly stores an empty array and fails. Other early failures do not commit the partial local array.

## Schedule decorator semantics

Despite its name, `UBTDecorator_IsInteractionMatchSchedule` currently implements a **needs selection** condition:

| Situation | Raw condition result |
| :--- | :--- |
| Required controller, NPC, schedule, clock, or Blackboard is missing | `false` |
| No valid interaction is selected | `true` |
| Selected interaction tag differs from scheduled tag | `true` |
| Selected interaction tag equals scheduled tag | `false` |

This predicate is suitable for a reservation branch. Attaching it directly as a positive matching condition on an execution branch would invert the intended behavior. The asset's inverse-condition setting can invert the raw result again.

For entry-only checking on a reservation Sequence, use **Observer Aborts = None** in that decorator's asset settings. A false condition denies entry. It does not cancel an already running Sequence.

The decorator does not independently poll world time. Its inherited Blackboard observer watches the configured selected key, not clock progression. The tree must revisit the schedule decision to respond to time changes. An indefinitely looping execution subtree that never returns to that decision will not gain periodic schedule checks from `None`.

## Reference tree integration

One integration pattern is a Selector with a preparation branch followed by an execution branch:

```text
Selector
  Sequence: prepare interaction
    Decorator: needs selection, Observer Aborts = None
    Reserve Smart Interaction
    Extract Smart Interaction Tasks
  Sequence: execute one action
    Guard: a valid interaction/task list is available
    Execute Next Task
    Increment Task Index
```

Arrange the parent tree's repetition so it returns to the selection decision between actions. If using a separate execution subtree, arrange its loop boundaries deliberately. A Selector reaching the execution branch after a failed preparation branch still needs guards against empty data.

This is a reference pattern, not an export of the demonstration's binary tree assets.

## Task lifecycle

`UBasedInteractionTask` supports these transitions:

```text
NotStarted --Start--> Running --Finish--> Succeeded / Failed / Cancelled
                            --Cancel--> Cancelled
Succeeded / Failed / Cancelled --Reset--> NotStarted
```

| Method | Contract |
| :--- | :--- |
| `Start(APawn*)` | Game-thread call. Rejects an instance not in `NotStarted` and rejects the class default object. Sets the pawn and state, then invokes `ExecuteTask()`. |
| `Finish(Result)` | Acts only while running. Stores a terminal result, invokes `CleanupTask()`, then broadcasts `OnFinished(this, Result)`. |
| `Cancel()` | Game-thread call. Finishes a running task as `Cancelled`; has no effect on a non-running task. |
| `Reset()` | Game-thread call. Clears the pawn and returns a non-running task to `NotStarted`; does not reset a running task. |
| `BeginDestroy()` | Cleans up a running task without broadcasting gameplay completion during destruction. |

`Start()` returning `true` means the start was accepted, not that the action succeeded. An invalid pawn can produce an accepted start followed by immediate failure. Success and failure are reported through state and the completion event.

### Immediate and asynchronous completion

The BT node binds `OnFinished` before `Start()`. It sets `bStartingTask` while calling `Start()` so an immediate callback does not try to finish a node that has not yet returned `InProgress`.

After `Start()` returns:

- A running accepted task produces `InProgress`.
- An immediately successful task produces `Succeeded`.
- Rejection or unsuccessful immediate completion produces `Failed`.

For later completion, the callback saves the owning tree, clears the active task, and calls `FinishLatentTask()` if the node is still active. It uses the delegate's result parameter because clearing the task resets its stored state.

### Cleanup ordering

`ClearActiveTask()`:

1. Removes the BT node's completion delegate from the task.
2. Clears the delegate handle, active-task reference, and tree reference.
3. Cancels a running task if cancellation was requested.
4. Resets the same task object for reuse.

Removing the listener before cancellation prevents cancellation callbacks from completing the BT node again. The same cleanup is used on abort, node finish, and instance destruction.

The node sets `bCreateNodeInstance = true` because its active task, tree pointer, and delegate handle belong to one NPC. Other nodes that keep per-NPC mutable members must also use node instancing or proper node memory.

Aborting execution cancels the active task. It does not itself release the component reservation; the reservation task owns the implemented release-on-reselection path.

## Built-in actions

**Movement:** Uses the pawn's AI controller and path-following component. Partial paths are disabled; agent and goal radii are excluded from the reach test. Already being at the goal is an immediate success. Completion is filtered by move request ID. Cleanup removes the listener and aborts only the still-current owned request on unsuccessful termination.

**Animation:** Requires a character mesh, AnimInstance, valid montage, positive play rate, and nonnegative cancellation blend time. A normal montage end succeeds; an interrupted end fails. Cleanup removes its listener and stops the matching active montage instance on unsuccessful termination.

`GetRequiredResources()` exposes movement/animation flags. A root-motion montage also reports Movement. The current sequential runner does not implement resource arbitration or parallel execution.

## Source references

- [Blackboard schema](../Source/SimulationWord/Private/NPCS/Blackboard/NPCBlackboardData.cpp)
- [Runtime task data](../Source/SimulationWord/Public/NPCS/Blackboard/ExecutionTasksData.h)
- [Base task lifecycle](../Source/SimulationWord/Private/NPCS/Tasks/BasedInteractionTask.cpp)
- [Execute-next node](../Source/SimulationWord/Private/NPCS/BehaviorTree_Tasks/ImplementTaskTree/BTTask_ExecuteNextTask.cpp)
- [Index advancement](../Source/SimulationWord/Private/NPCS/BehaviorTree_Tasks/ImplementTaskTree/BTTask_IncrementTaskIndex.cpp)
- [Schedule decorator](../Source/SimulationWord/Private/NPCS/BehaviorTreeDecorators/BTDecorator_IsInteractionMatchSchedule.cpp)
