# M0-Pivot-002 Markdown Route Audit Run 001

## Scope

Docs-only route audit after the 2026-06-11 pivot.

Primary setpoint:

```text
All current-entry Markdown docs must point to:
STEP-only + RebarSmart evidence + Tushi Detail compatibility.
```

Current next must remain:

```text
TODO-090 / M1-Detail-001 DetailPackage 数据模型 P0
```

## Changed Files

- `00_总览.md`
- `06_技术路线与替代方案.md`
- `08_开发命令契约.md`
- `09_钢筋领域模型草案.md`
- `18_新设计文件格式替代SFL策略.md`
- `23_父目录源码参考边界与路线纠偏.md`
- `35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`
- `36_正式Qt6_OCCT工程架构与首个实现切片.md`
- `125_M2-RebarCreate-017线配筋保存包Runtime回读验证P0实现记录.md`
- `docs/phase1/app_build_reports/m2_rebar_create_017_run_001.md`
- `127_M0-Pivot-002全量Markdown路线审计记录.md`
- `docs/phase1/app_build_reports/m0_pivot_002_md_route_audit_run_001.md`

## Verification

```powershell
rg -n "<stale-route-patterns>" --glob "*.md"
```

Result:

```text
no matches
```

```powershell
Import-Csv todo.csv | Where-Object {$_.status -eq 'next'} | Select-Object id,phase,task,status
```

Result:

```text
TODO-090 / M1-Detail-001 / DetailPackage 数据模型 P0 / next
```

```powershell
git diff --check
```

Result:

```text
pass
```

## Review

A read-only subagent audit independently flagged the same remaining risks:

- `35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`
- `36_正式Qt6_OCCT工程架构与首个实现切片.md`
- `125_M2-RebarCreate-017...`
- `docs/phase1/app_build_reports/m2_rebar_create_017_run_001.md`
- `00_总览.md` entry for `98_M2-Control-001`

Those issues were corrected in this run.

The completed subagent was closed after review.

## Boundary

No app code, tests, build scripts, RebarSmart binaries, customer samples, license files,
private reverse-engineering listings, HASP setup, or old VisualTS runtime actions were touched.
