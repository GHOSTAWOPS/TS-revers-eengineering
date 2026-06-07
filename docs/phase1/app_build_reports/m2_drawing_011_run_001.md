# M2-Drawing-011 Run 001

## Summary

```text
todoId = TODO-042
phase = M2-Drawing-011
decision = point-face-edge-autocad-l2-ready-not-run
```

本轮把 `TODO-037` 的 `pointStb / FaceEdge` 独立包转成 AutoCAD L2 运行确认准备项：

- 生成 `TODO-042` 专用三图纸 Detail 包。
- 生成 `pointStb / FaceEdge` 手工 L2 确认清单。
- 记录 FDrawing 插件文件 hash。
- 记录 AutoCAD 可执行文件和注册表探测。
- 本机未发现 AutoCAD 运行环境，所以 AutoCAD L2 自动导入未运行。

## Evidence

```text
E-DETAIL-003 = old Detail01.stl pointStb / FaceEdge static fields
E-DEV-059 = TODO-037 pointStb / FaceEdge field skeleton P0
E-DEV-063 = TODO-041 line-containers AutoCAD L2 confirmation preparation P0
E-DEV-064 = TODO-042 pointStb / FaceEdge AutoCAD L2 confirmation preparation P0
```

## Changed Files

```text
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## Generated Artifacts

```text
78_M2-Drawing-011DetailWriter点筋FaceEdgeAutoCADL2运行确认准备P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_011_detail_package/Detail.xml
docs/phase1/app_build_reports/m2_drawing_011_detail_package/Detail01.stl
docs/phase1/app_build_reports/m2_drawing_011_detail_package/Detail02.stl
docs/phase1/app_build_reports/m2_drawing_011_detail_package/Detail03.stl
docs/phase1/app_build_reports/m2_drawing_011_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_011_point_face_edge_l2_checklist.md
```

## Probe Result

```text
schemaVersion = detail-l2-fixture-probe/v1
runId = DW-L2-TODO042-001
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
  2bd56433cf2858aaf8d88e32518f808d997f9839f142f130b2fab3fa90cc24c4

Detail02.stl
  e4dc455d01721f814ee0ad38f58fe0a46129350caa723e3e5d01140e7b711568

Detail03.stl
  4a685909e798fb850743e0e6a734b29f453bde0d9d060dc889e8ca1169cf39d3
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
  -> RED before TODO-042 known_reports mapping
  -> GREEN after TODO-042 known_reports mapping
  -> 14 tests OK
```

Probe:

```text
detail_l2_fixture_probe --fixture point-face-edge --run-id DW-L2-TODO042-001
  -> pass
```

JSON validation:

```text
py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_011_detail_probe.json
py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_011_run_001.json
  -> pass
```

Final verification:

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> 14 tests OK

ctest --test-dir app/build --output-on-failure
  -> 17/17 tests passed

py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
  -> decision = M1-Formal-Ready
  -> passCount = 84 / checkCount = 84
  -> errorCount = 0
  -> warningCount = 0

py -3 app/tools/check_domain_rebar_boundary.py app/src/domain/rebar
py -3 app/tools/check_domain_rebar_boundary.py app/src/drawing
py -3 app/tools/check_domain_rebar_boundary.py app/src/project
  -> boundary ok for all three protected roots

git diff --check
  -> pass
```

xhigh read-only review:

```text
verdict = needs_fix
critical = none
important = report / JSON / 46 pending verification state fixed by main flow
minor = m2_drawing_011_detail_probe.json evidence remains generator-side E-DEV-059; accepted as non-blocking because run report carries E-DEV-064
route assessment = no OCCT direct rebar rewrite, no protected-layer OCCT/AIS leak, no AutoCAD L2 pass misclaim, TODO-042 done / TODO-043 next is consistent after fix
agent = Anscombe / 019ea199-5c49-7b53-a863-4aa49676ee49
agent closed after review = true
```

## Boundary

```text
AutoCAD L2 import = not_run
FDrawing acceptance = unknown
pointStb / FaceEdge L2 checklist = preparation only
pointStb generation rule = gap
FaceEdge generation rule = gap
section / hidden / hatch / joint drawing algorithm = not_implemented
golden = not_collected
```
