# M2-Drawing-007 Run 001

## Summary

```text
todoId = TODO-038
phase = M2-Drawing-007
decision = section-line-field-skeleton-l2-not-run
```

本轮把旧 `Detail01.stl` 样例中已确认的 `section-line` 变量做成独立字段骨架专项：

- `DetailWriter` 新增 `section-line / lines / LineN` 字段骨架。
- `DetailWriter` 新增 `section-line / Arcs / ArcN` 字段骨架。
- `LineN / ArcN` 均保留 `ZValue` 字符串字段。
- `detail_l2_fixture_probe` 新增 `--fixture section-line`。
- 生成 TODO-038 独立三图纸 Detail 包。
- 本机仍未发现 AutoCAD 运行环境，所以 AutoCAD L2 自动导入未运行。

## Evidence

```text
E-DETAIL-003 = old Detail01.stl section-line Line/Arc/ZValue static fields
E-DEV-056 = complex field static evidence
E-DEV-057 = DetailWriter complex skeleton P0
E-DEV-059 = pointStb / FaceEdge independent field skeleton P0
E-DEV-060 = TODO-038 section-line Line/Arc/ZValue field skeleton P0
```

## Changed Files

```text
app/src/drawing/detail/DetailWriter.h
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
app/tools/detail_l2_fixture_probe.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

## Generated Artifacts

```text
docs/phase1/app_build_reports/m2_drawing_007_detail_package/Detail.xml
docs/phase1/app_build_reports/m2_drawing_007_detail_package/Detail01.stl
docs/phase1/app_build_reports/m2_drawing_007_detail_package/Detail02.stl
docs/phase1/app_build_reports/m2_drawing_007_detail_package/Detail03.stl
docs/phase1/app_build_reports/m2_drawing_007_detail_probe.json
```

## Probe Result

```text
schemaVersion = detail-l2-fixture-probe/v1
runId = DW-L2-TODO038-001
fixture = section-line
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
autocadL2 = not_run
sectionLine.passed = true
lineCount = 1
arcCount = 1
lineFieldsPassed = true
arcFieldsPassed = true
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
```

## Detail Package Hashes

```text
Detail.xml
  cbf196dbf3e68fb41486d3cd365f27fdadb394cff6a114ebe4dd020138b56e4d

Detail01.stl
  e56dfe1df05924bbaf96f25ca5a94f2af42f301c2cfbaa2900e105649e8720c8

Detail02.stl
  1af17e716f21796f2e4bc9ad132edb15dea93e89e9584b65b712581d4bbc7bed

Detail03.stl
  453356eb35160647479bc4dded9c80a9f6d531dc5818f67d8f5963d1da766fec
```

## Validation

Build:

```text
detail_writer_tests
detail_l2_fixture_probe
  -> pass / no work to do
```

Unit test:

```text
detail_writer_tests
  -> pass
```

Probe:

```text
detail_l2_fixture_probe --fixture section-line
  -> pass
```

Default CTest:

```text
ctest --test-dir app\build --output-on-failure
  -> 17/17 pass
```

Readiness gate:

```text
py -3 tools\phase1_readiness_gate\check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass, 0 error, 0 warning
```

Protected-layer leak scan:

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app\src\domain\rebar app\src\drawing app\src\project
  -> pass / no matches
```

JSON validation:

```text
py -3 -m json.tool m2_drawing_007_run_001.json
py -3 -m json.tool m2_drawing_007_detail_probe.json
  -> pass
```

xhigh review:

```text
xhigh review
  -> allow_commit
  -> Critical: none
  -> Important: none
  -> Minor: sectionLineProbe 当前按本轮 fixture 至少 1 个合法 Line/Arc 判断；
     后续多元素全量检查可在 L2/算法专项收紧。
```

## Boundary

```text
AutoCAD L2 import = not_run
FDrawing acceptance = unknown
section-line fields = explicit fixture skeleton only
section-line generation rule = gap
ZValue semantics = gap
section / hidden / hatch / joint algorithm = not_implemented
golden = not_collected
```
