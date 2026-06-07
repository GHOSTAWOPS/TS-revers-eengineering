# M2-Drawing-008 Run 001

## Summary

```text
todoId = TODO-039
phase = M2-Drawing-008
decision = section-line-autocad-l2-ready-not-run
```

本轮把 `TODO-038` 的 `section-line` 独立包转成 AutoCAD L2 运行确认准备项：

- 生成 `TODO-039` 专用三图纸 Detail 包。
- 生成 `section-line` 手工 L2 确认清单。
- 记录 FDrawing 插件文件 hash。
- 记录 AutoCAD 可执行文件和注册表探测。
- 本机未发现 AutoCAD 运行环境，所以 AutoCAD L2 自动导入未运行。

## Evidence

```text
E-DETAIL-003 = old Detail01.stl section-line Line/Arc/ZValue static fields
E-DEV-056 = complex field static evidence
E-DEV-057 = DetailWriter complex skeleton P0
E-DEV-059 = pointStb / FaceEdge independent field skeleton P0
E-DEV-060 = TODO-038 section-line Line/Arc/ZValue field skeleton P0
E-DEV-061 = TODO-039 section-line AutoCAD L2 confirmation preparation P0
```

## Changed Files

```text
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## Generated Artifacts

```text
75_M2-Drawing-008DetailWriterSectionLineAutoCADL2运行确认准备P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_008_detail_package/Detail.xml
docs/phase1/app_build_reports/m2_drawing_008_detail_package/Detail01.stl
docs/phase1/app_build_reports/m2_drawing_008_detail_package/Detail02.stl
docs/phase1/app_build_reports/m2_drawing_008_detail_package/Detail03.stl
docs/phase1/app_build_reports/m2_drawing_008_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_008_section_line_l2_checklist.md
```

## Probe Result

```text
schemaVersion = detail-l2-fixture-probe/v1
runId = DW-L2-TODO039-001
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

## AutoCAD / FDrawing

```text
FDrawingObj.dbx = present
FDrawing.arx = present
old sample Detail.xml = present
old sample Detail01.stl = present
acad.exe = not_found
accoreconsole.exe = not_found
AutoCAD registry keys = not_found
automaticL2Possible = false
```

## Validation

TDD gate test:

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> RED before gate mapping
  -> GREEN after TODO-039 known_reports mapping
  -> 11 tests OK
```

Probe:

```text
detail_l2_fixture_probe --fixture section-line --run-id DW-L2-TODO039-001
  -> pass
```

JSON validation:

```text
py -3 -m json.tool m2_drawing_008_detail_probe.json
  -> pass
```

Final verification:

```text
cmake --build app/build --config Debug
  -> ninja: no work to do

ctest --test-dir app/build --output-on-failure
  -> 17/17 tests passed
  -> 0 failures

py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass, 0 error, 0 warning

rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
  -> no matches

git diff --check
  -> no whitespace errors
```

xhigh read-only review:

```text
first review
  -> Verdict: block
  -> Critical: None
  -> Important: 46 still contained an old TODO-033 short-term goal block.

fix
  -> 46 short-term goal, current next, current execution suggestion and CSE
     control contract now point to TODO-040.

rerun review
  -> Verdict: allow_commit
  -> Critical: None
  -> Important: None
  -> Minor: None
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
