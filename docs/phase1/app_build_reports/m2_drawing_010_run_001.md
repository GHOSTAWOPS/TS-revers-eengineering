# M2-Drawing-010 Run 001

## Summary

```text
todoId = TODO-041
phase = M2-Drawing-010
decision = line-containers-autocad-l2-ready-not-run
```

本轮把 `TODO-040` 的 `line-containers` 独立包转成 AutoCAD L2 运行确认准备项：

- 生成 `TODO-041` 专用三图纸 Detail 包。
- 生成 `line-containers` 手工 L2 确认清单。
- 记录 FDrawing 插件文件 hash。
- 记录 AutoCAD 可执行文件和注册表探测。
- 本机未发现 AutoCAD 运行环境，所以 AutoCAD L2 自动导入未运行。

## Evidence

```text
E-DETAIL-003 = old Detail01.stl line container static fields
E-DEV-056 = complex field static evidence
E-DEV-061 = section-line L2 confirmation preparation P0
E-DEV-062 = TODO-040 line-containers LineN field skeleton P0
E-DEV-063 = TODO-041 line-containers AutoCAD L2 confirmation preparation P0
```

## Changed Files

```text
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## Generated Artifacts

```text
77_M2-Drawing-010DetailWriter线容器AutoCADL2运行确认准备P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_010_detail_package/Detail.xml
docs/phase1/app_build_reports/m2_drawing_010_detail_package/Detail01.stl
docs/phase1/app_build_reports/m2_drawing_010_detail_package/Detail02.stl
docs/phase1/app_build_reports/m2_drawing_010_detail_package/Detail03.stl
docs/phase1/app_build_reports/m2_drawing_010_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_010_line_containers_l2_checklist.md
```

## Probe Result

```text
schemaVersion = detail-l2-fixture-probe/v1
runId = DW-L2-TODO041-001
fixture = line-containers
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
autocadL2 = not_run
lineContainers.passed = true
containerCount = 4
continue-line lineCount = 1
hidden-line lineCount = 1
central-line lineCount = 1
hatch-line lineCount = 1
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
```

## Detail Package Hashes

```text
Detail.xml
  cbf196dbf3e68fb41486d3cd365f27fdadb394cff6a114ebe4dd020138b56e4d

Detail01.stl
  7a570d129125c78638f842cb9e81e1d358dccf54cc43f09ed5eebcaccda0e539

Detail02.stl
  c887e9d74386bbca8330e964d06fed215e9a6380fc1ec4929206999544c214f2

Detail03.stl
  5a398917d79513847646eab4a6b3ba9dc3ddb6a8e2491233a443c21708bee81a
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
  -> RED before TODO-041 known_reports mapping
  -> GREEN after TODO-041 known_reports mapping
  -> 13 tests OK
```

Probe:

```text
detail_l2_fixture_probe --fixture line-containers --run-id DW-L2-TODO041-001
  -> pass
```

JSON validation:

```text
py -3 -m json.tool m2_drawing_010_detail_probe.json
  -> pass
```

Final verification:

```text
py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_010_detail_probe.json
py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_010_run_001.json
  -> pass

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
verdict = allow_commit
Critical = None
Important = None
Minor = m2_drawing_010_run_001.md/json and 46 pending fields needed final回填

review notes:
  -> 未发现路线偏移。
  -> 未发现 AutoCAD L2 被误声明通过。
  -> 未发现把 line-containers 字段骨架或 L2 准备误写成真实连续线、隐藏线、中心线、填充线算法完成。
  -> todo.csv 当前只有 TODO-042 为 next，TODO-041 为 done。
  -> readiness gate 新增 TODO-041 done-report 映射和对应单测合理。
```

## Boundary

```text
AutoCAD L2 import = not_run
FDrawing acceptance = unknown
line-containers L2 checklist = preparation only
line-container generation rule = gap
ZValue semantics = gap
continue / hidden / central / hatch / section / joint drawing algorithm = not_implemented
golden = not_collected
```
