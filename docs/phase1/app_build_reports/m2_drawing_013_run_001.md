# M2-Drawing-013 Run 001

todoId = TODO-044
phase = M2-Drawing-013
decision = others-steeljoint-autocad-l2-ready-not-run
autocadL2 = not_run

## 结论

本轮把 `TODO-043` 的 `others-steeljoint` 独立包转成 AutoCAD L2 运行确认准备项：

- 生成 `TODO-044` 专用三图纸 Detail 包。
- 生成 `Others / steeljoint-line` 手工 L2 确认清单。
- 记录 FDrawing 插件 hash、旧样例包 hash 和 TODO-044 新包 hash。
- 记录本机 `acad.exe / accoreconsole.exe / AutoCAD registry` 探测结果。
- 本机未发现 AutoCAD 运行环境，所以 AutoCAD L2 自动导入未运行。

本轮不声明旧插件接受新包，不声明 AutoCAD L2 通过，不实现真实接头线算法、Others 几何算法、完整工程图或 golden。

## 证据

```text
E-DEV-065 = TODO-043 Others / steeljoint-line 字段骨架 P0
E-DEV-066 = TODO-044 Others / steeljoint-line AutoCAD L2 运行确认准备 P0
GAP-DRAW-001 = AutoCAD 动态导入仍未运行
GAP-DRAW-002 = DetailNN.stl 字段完整性和旧插件容忍度仍未闭合
```

## 工件

```text
80_M2-Drawing-013DetailWriterOthersSteeljointAutoCADL2运行确认准备P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_013_run_001.md
docs/phase1/app_build_reports/m2_drawing_013_run_001.json
docs/phase1/app_build_reports/m2_drawing_013_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_013_others_steeljoint_l2_checklist.md
docs/phase1/app_build_reports/m2_drawing_013_detail_package/
```

## Probe 摘要

```text
runId = DW-L2-TODO044-001
fixture = others-steeljoint
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
autocadL2 = not_run
othersSteeljoint.passed = true
Others present = true
Others children = 0
steeljoint-line present = true
steeljoint-line / joints present = true
algorithmImplemented = false
autocadL2Claimed = false
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
```

## 文件 Hash

新包：

```text
Detail.xml
  cbf196dbf3e68fb41486d3cd365f27fdadb394cff6a114ebe4dd020138b56e4d

Detail01.stl
  7e25983036b3150021f42ba47c608340b4ba868a42e88931140105bd061f484a

Detail02.stl
  d54fbdce40da3487fe524bd028a1c149cb121631b09e37f488a10cf2768a116d

Detail03.stl
  f2964b47a639f1234158917ef9b29109f99bb9bc997196f8aa6192b6dc4af993
```

旧插件：

```text
FDrawingObj.dbx
  a7c8493507de802f4881c0b60db3dbbe4c5982cdf9f9c42a9c805ec2feec97b0

FDrawing.arx
  30db8152da1d42de0f2550c6a4242bcc4781f8d5b649428c6ddcbf647a29d428
```

旧样例包：

```text
old-sample/Detail.xml
  b02abeeb7b9e0ab802566fa617e0627d9883010172efd9cd50dacf940b5afeb5

old-sample/Detail01.stl
  e3bb220e39f5657a38bfc6793e246208760816913520daf9c97c3544d0421c3e
```

## AutoCAD 环境

```text
acad.exe = not_found
accoreconsole.exe = not_found
HKLM/HKCU AutoCAD registry root keys = exists; version child groups = 0
automaticL2Possible = false
```

因此：

```text
AutoCAD L2 import = not_run
```

## TDD / Gate

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> RED before TODO-044 known_reports mapping
  -> GREEN after TODO-044 known_reports mapping
```

## 验证

```text
app/build/detail_l2_fixture_probe.exe --fixture others-steeljoint --views 3 --run-id DW-L2-TODO044-001
  -> pass; detail_probe JSON regenerated

py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_013_detail_probe.json
py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_013_run_001.json
  -> pass

cmake --build app/build
  -> pass; ninja: no work to do

ctest --test-dir app/build --output-on-failure
  -> 18/18 passed; total 125.83 sec

py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> 16/16 OK

py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
  -> M1-Formal-Ready; 84/84 pass; 0 errors; 0 warnings

py -3 app/tools/check_domain_rebar_boundary.py app/src/domain/rebar
py -3 app/tools/check_domain_rebar_boundary.py app/src/drawing
py -3 app/tools/check_domain_rebar_boundary.py app/src/project
  -> pass; boundary ok for all three protected layers

git diff --check
  -> pass; exit 0

py -3 app/tests/integration/test_detail_l2_fixture_probe_environment.py \
  ../__tdd_red_todo044_probe/app/build/detail_l2_fixture_probe.exe
  -> RED confirmed on old HEAD probe:
     HKEY_LOCAL_MACHINE\SOFTWARE\Autodesk\AutoCAD exists mismatch
     HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Autodesk\AutoCAD exists mismatch
     HKEY_CURRENT_USER\SOFTWARE\Autodesk\AutoCAD exists mismatch

py -3 app/tests/integration/test_detail_l2_fixture_probe_environment.py \
  app/build/detail_l2_fixture_probe.exe
  -> GREEN; exit 0
```

## xhigh 只读 Review

```text
verdict = allow_commit
Critical = none
Important = none
Fixed = no code/doc fix required after review
Minor = none
Route drift = no
L2 claim audit = no misclaim
Algorithm claim audit = no misclaim
Protected OCCT leak = no leak
agent = todo044_report_xhigh_reviewer / Faraday / 019ea247-5a97-7310-ac0e-77d3e1e3b054
agent closed after review = true
```

## 明确不声明

```text
AutoCAD L2 import 已通过。
旧 FDrawing 插件接受 TODO-044 新包。
旧插件接受 Others / steeljoint-line 字段组合。
真实接头线算法已实现。
Others 几何算法已实现。
真实连续线 / 隐藏线 / 中心线 / 填充线算法已实现。
真实剖切线算法已实现。
点筋真实算法已实现。
FaceEdge 生成规则已闭合。
完整工程图已完成。
golden 已采集。
```

## 下一步

```text
TODO-045 / M2-Drawing-014
  -> 真实接头线 / Others 几何算法证据补齐
  -> 优先用 IDA MCP 或旧图石运行确认查旧生成规则
  -> 没有证据前不实现算法
```
