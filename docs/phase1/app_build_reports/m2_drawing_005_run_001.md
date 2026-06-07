# M2-Drawing-005 Run 001

## Summary

```text
todoId = TODO-036
phase = M2-Drawing-005
decision = complex-skeleton-l2-not-run
```

本轮把 TODO-035 生成的复杂字段骨架推进到 AutoCAD L2 验证准备和环境探测：

- 生成 TODO-036 专用三图纸 Detail 包。
- 离线检查复杂字段容器和 `General-Info` 扩展字段。
- 记录旧 `FDrawingObj.dbx / FDrawing.arx` 文件和旧样例包 hash。
- 探测 AutoCAD 可执行文件和注册表。
- 本机未发现 AutoCAD 运行环境，所以 L2 自动导入未运行。

## Evidence

```text
E-DEV-055 = AutoCAD L2 package probe
E-DETAIL-003 = old Detail01.stl complex field structure
E-DEV-057 = DetailWriter complex skeleton P0
E-DEV-058 = TODO-036 complex skeleton L2 probe package and environment record
```

## Changed Files

```text
app/tools/detail_l2_fixture_probe.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

## Generated Artifacts

```text
docs/phase1/app_build_reports/m2_drawing_005_detail_package/Detail.xml
docs/phase1/app_build_reports/m2_drawing_005_detail_package/Detail01.stl
docs/phase1/app_build_reports/m2_drawing_005_detail_package/Detail02.stl
docs/phase1/app_build_reports/m2_drawing_005_detail_package/Detail03.stl
docs/phase1/app_build_reports/m2_drawing_005_detail_probe.json
```

## Probe Result

```text
schemaVersion = detail-l2-fixture-probe/v1
runId = DW-L2-TODO036-001
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
autocadL2 = not_run
complexSkeleton.passed = true
complexSkeleton.scope = complex containers + General-Info only; pointStb L2 is deferred
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
```

## Detail Package Hashes

```text
Detail.xml
  cbf196dbf3e68fb41486d3cd365f27fdadb394cff6a114ebe4dd020138b56e4d

Detail01.stl
  664ef81b821316a89808dfee5b9fee1c704552739314eecdf062b8908c851580

Detail02.stl
  5383f724471a82d56d90bbe27230e930f3f9d6cb9fd72434fc08dad3d3365657

Detail03.stl
  e58a3e2492cb6cf3f94c95b2b3c3542e9aeeba3ea030cd05722fc24702ba23f8
```

## Validation

Build:

```text
detail_l2_fixture_probe
detail_writer_tests
  -> pass
```

Probe:

```text
detail_l2_fixture_probe
  -> pass
```

Default CTest:

```text
17 / 17 pass
```

Readiness gate:

```text
M1-Formal-Ready
84 / 84 pass
0 error
0 warning
```

Protected-layer leak scan:

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
  -> no matches
```

Diff check:

```text
git diff --check
  -> pass
```

xhigh read-only review:

```text
first verdict = needs_fix
Important = pointStb L2 variable pollution
main-flow fix = removed pointStb from TODO-036 L2 package
final verdict = allow_commit
Critical = none
Important = none
Route alignment = pass
AutoCAD L2 false-claim check = ok
Todo/46 consistency = pass
```

## Boundary

```text
AutoCAD L2 import = not_run
FDrawing acceptance = unknown
pointStb L2 = deferred
FaceEdge generation = not_implemented
section / hidden / hatch / joint algorithm = not_implemented
ZValue semantics = gap
CompanyName old default = not_confirmed
golden = not_collected
```

## Next

```text
TODO-037 / M2-Drawing-006
  -> DetailWriter pointStb / FaceEdge 兼容字段骨架 P0
```
