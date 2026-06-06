# TODO-020 IDA Line / Arc Chain Run 001

## Decision

```text
decision = evidence-ready
ida_mcp = available
session = visualts_i64_todo020
```

## Scope

This run inspected the old VisualTS line / arc rebar group chain:

```text
sub_1404DE720
sub_1404DE110
sub_1404D10C0
sub_140451730
sub_1405D5670
sub_14058F160
sub_140580950
sub_14059B980
sub_1405BD0C0
sub_1405C7260
sub_1405E49D0
```

## Result

- IDA MCP opened `VisualTS.exe.i64` successfully.
- `sub_1404DE720` confirms the line-group entry gathers selected object entities and endpoint distances.
- `sub_1404DE110` confirms the arc-group entry uses `api_entity_entity_distance`, default distance `0.1`, and calls the common creation transaction.
- `sub_1404D10C0` confirms the command transaction boundary and the `0.002` minimum distance gate.
- `sub_1405D5670` confirms split / spline / trim / min-distance cleanup for `seg_steelbargroup.cpp`.
- `sub_1405BD0C0` confirms `ENTITY::backup()` before writing the replacement edge to `entity + 72`.
- `sub_1405E49D0` confirms old object-tree dirty display behavior using `*` prefix.

## Remaining Gaps

- The fourth double parameter observed in `sub_1405D5670` is still not fully sourced.
- Business names for `objA / objB / createdPayload` are still unresolved.
- Old UI mapping for `sgroupbararc` to `扇形筋` / `同心圆` still needs runtime confirmation.

