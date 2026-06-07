# M2-Drawing-006 Run 001

## Summary

```text
todoId = TODO-037
phase = M2-Drawing-006
decision = point-face-edge-field-skeleton-l2-not-run
```

本轮把 `TODO-036` 拆出的点筋变量做成独立干净变量专项：

- `DetailWriter` 保留 `pointStb StbGeo shapeType=C` 字段骨架。
- 新增显式 `FaceEdge shapeType=L/A` 字段骨架输出。
- `detail_l2_fixture_probe` 新增 `--fixture point-face-edge`。
- 生成 TODO-037 独立三图纸 Detail 包。
- 离线检查 `pointStb / FaceEdge L/A` 字段。
- 本机仍未发现 AutoCAD 运行环境，所以 AutoCAD L2 自动导入未运行。

## Evidence

```text
E-DETAIL-003 = old Detail01.stl pointStb / FaceEdge static fields
E-DEV-056 = complex field static evidence
E-DEV-057 = DetailWriter complex skeleton and pointStb P0
E-DEV-058 = TODO-036 clean-variable correction
E-DEV-059 = TODO-037 pointStb / FaceEdge field skeleton P0
```

## Changed Files

```text
app/src/domain/rebar/RebarDomainTypes.h
app/src/domain/rebar/SteelBarGroup.h
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
app/tools/detail_l2_fixture_probe.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

## Generated Artifacts

```text
docs/phase1/app_build_reports/m2_drawing_006_detail_package/Detail.xml
docs/phase1/app_build_reports/m2_drawing_006_detail_package/Detail01.stl
docs/phase1/app_build_reports/m2_drawing_006_detail_package/Detail02.stl
docs/phase1/app_build_reports/m2_drawing_006_detail_package/Detail03.stl
docs/phase1/app_build_reports/m2_drawing_006_detail_probe.json
```

## Probe Result

```text
schemaVersion = detail-l2-fixture-probe/v1
runId = DW-L2-TODO037-001
fixture = point-face-edge
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
autocadL2 = not_run
pointFaceEdge.passed = true
pointGroupCount = 2
pointGeoCount = 2
faceEdgeCount = 2
lineFaceEdgePassed = true
arcFaceEdgePassed = true
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
```

## Detail Package Hashes

```text
Detail.xml
  f4927ef3f83e91c858eeead3832fbcf86f8956b2837a7df5b74de3273d46be8c

Detail01.stl
  89f3bd590e028a3858c045882294d3c257f2e119f1561cd9bd709cce8996ede8

Detail02.stl
  84782c687d049ceff11953c0743c1258f0884deab08048ad430ee5ce8fac5c90

Detail03.stl
  e670adc2fbfd486cbb119a6fadd81ccd04263b7f33349457f1587aafe15e90f2
```

## Validation

Build:

```text
detail_writer_tests
detail_l2_fixture_probe
  -> pass
```

Probe:

```text
detail_l2_fixture_probe --fixture point-face-edge
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

Xhigh read-only review:

```text
Initial Verdict = needs_fix
Critical = none
Important = fixed:
  - run_001.json xhighReview 从 pending 收口；
  - 11_需求证据追溯矩阵.md 的 REQ-PROJ-001 / REQ-TECH-002
    汇总行纳入 E-DEV-059。
Minor = pointFaceEdgeProbe uses global counts; per-StbGroup FaceEdge ownership
        assertion can be strengthened later.
Final Verdict = allow_commit
```

## Boundary

```text
AutoCAD L2 import = not_run
FDrawing acceptance = unknown
pointStb L2 acceptance = unknown
FaceEdge fields = explicit fixture skeleton only
FaceEdge generation rule = gap
point-bar generation algorithm = not_implemented
section / hidden / hatch / joint algorithm = not_implemented
golden = not_collected
```
