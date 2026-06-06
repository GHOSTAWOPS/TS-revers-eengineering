# Tushi Rebar 123 STP Witness

## Summary

```text
sampleId = tushi_rebar_123_stp
decision = witness-pass
```

This report fixes `123.stp` as the first old Tushi rebar geometry witness.

## Source Relation

Source SFL:

```text
path = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\消力池下游侧带齿槽底板结构图石钢筋模型.sfl
size = 563,051 bytes
sha256 = a5645635ff77346e1dacccbac57b7f594d1278d74bf70f4be762dd93ced90515
```

Exported STP:

```text
path = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\123.stp
size = 87,746,388 bytes
sha256 = 6a3c4f2044c2cc1f1123f0f58c61b2c869fbad3110a3585fdab49e69daf52a2a
lastWriteTime = 2026-06-05 10:02:17 +08:00
```

Known old-run evidence:

```text
old UI path = 开始 / 文件 / 输出钢筋
run log = 12_运行确认日志.md / RUN-20260605-001
```

## STEP Header

```text
FILE_DESCRIPTION(('STEP AP214'),'1')
FILE_NAME('123.stp','2026-06-05T02:02:12',...,'Spatial InterOp 3D',...)
FILE_SCHEMA(('AUTOMOTIVE_DESIGN { 1 0 10303 214 1 1 1 1 }'))
```

## Current Import Probe

Command:

```powershell
app\build\step_import_probe.exe "..\123.stp"
```

Result:

```text
ok=true
read_ok=true
transfer_ok=true
roots=1
free_shapes=1
solids=754
faces=3016
edges=9048
vertices=18096
length_unit=METRE
```

## Existing Selection Gate

Report:

```text
docs/phase1/step_selection/tushi_rebar_123_stp/step_selection_diff.json
docs/phase1/step_selection/tushi_rebar_123_stp/step_selection_diff.md
```

Result:

```text
decision = pass
probeRunCount = 5
sourceSha256 = sha256:6a3c4f2044c2cc1f1123f0f58c61b2c869fbad3110a3585fdab49e69daf52a2a
```

Stable refs:

| Kind | Baseline count | Min match rate | Duplicate count | Mismatched runs |
|---|---:|---:|---:|---|
| faces | 3016 | 1.000000 | 0 | none |
| edges | 4524 | 1.000000 | 0 | none |
| vertices | 3016 | 1.000000 | 0 | none |

## Boundary

This witness proves only geometry import and stable selection reference behavior for this sample.

It does not prove old rebar creation rules, old command parameters, schedule formulas, Detail output correctness, or that the new system can already generate the same geometry.
