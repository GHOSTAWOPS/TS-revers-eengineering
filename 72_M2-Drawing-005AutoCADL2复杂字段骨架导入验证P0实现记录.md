# M2-Drawing-005 AutoCAD L2复杂字段骨架导入验证 P0 实现记录

## 结论

本轮完成 `TODO-036 / M2-Drawing-005` 的 P0 验证闭环：

- `detail_l2_fixture_probe` 已生成 TODO-036 专用复杂字段骨架 Detail 包。
- 新包包含 `Detail.xml + Detail01.stl + Detail02.stl + Detail03.stl`。
- probe 已校验 `PartDetailDrawing num="8"`、复杂容器骨架和 `General-Info` 首批扩展字段。
- 已记录 `FDrawingObj.dbx / FDrawing.arx` 和旧样例包 hash。
- 本机未发现 `acad.exe / accoreconsole.exe` 或 AutoCAD 注册表键。
- 因此 AutoCAD L2 自动导入未运行。

本轮不声明旧 AutoCAD 2020 + FDrawing 插件已经接受当前复杂字段骨架。

## 证据 ID

新增证据：

```text
E-DEV-058
  -> TODO-036 / M2-Drawing-005 AutoCAD L2复杂字段骨架导入验证 P0。
```

依赖证据：

```text
E-DEV-055
  -> AutoCAD L2 验证包生成和环境阻塞记录。

E-DETAIL-003
  -> 旧 Detail01.stl 复杂字段静态结构。

E-DEV-057
  -> DetailWriter 复杂字段骨架 P0。
```

## 实现范围

修改文件：

```text
app/tools/detail_l2_fixture_probe.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

新增文档 / 报告：

```text
72_M2-Drawing-005AutoCADL2复杂字段骨架导入验证P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_005_run_001.md
docs/phase1/app_build_reports/m2_drawing_005_run_001.json
docs/phase1/app_build_reports/m2_drawing_005_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_005_detail_package/
```

## Probe 能力

`detail_l2_fixture_probe` 本轮新增 / 强化：

```text
runId = DW-L2-TODO036-001
schemaVersion = detail-l2-fixture-probe/v1
```

探测内容：

- 生成 3 张 `DetailNN.stl`。
- 检查 `PartDetailDrawing` 直接子节点。
- 检查 `continue-line / hidden-line / section-line` 等复杂容器子节点。
- 检查 `General-Info` 扩展属性存在。
- 探测 `acad.exe / accoreconsole.exe`。
- 探测 AutoCAD 注册表键。
- 探测 `FDrawingObj.dbx / FDrawing.arx` 和旧样例包。

复杂骨架检查范围：

```text
complex containers + General-Info only; pointStb L2 is deferred
```

这意味着本轮只验证复杂工程图字段骨架是否能被稳定生成和离线检查，不把点筋 / FaceEdge 变量混入旧插件接受度判断。

## 新 Detail 包

输出目录：

```text
docs/phase1/app_build_reports/m2_drawing_005_detail_package
```

文件：

```text
Detail.xml     186 bytes
Detail01.stl  4624 bytes
Detail02.stl  4624 bytes
Detail03.stl  4624 bytes
```

SHA256：

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

## Complex Skeleton

`Detail01.stl` 当前检查结果：

```text
PartDetailDrawing.num = 8
complexSkeleton.passed = true
General-Info.passed = true
```

`PartDetailDrawing` 已包含：

```text
General-Info
continue-line
hidden-line
central-line
section-line
hatch-line
Others
steeljoint-line
```

复杂容器已包含：

```text
continue-line:
  lines / circles / Arcs / Ellipses / EllipseArcs / Splines

hidden-line:
  lines / circles / Arcs / Ellipses / EllipseArcs / Splines

central-line:
  lines

section-line:
  lines / circles / Arcs / Ellipses / EllipseArcs / Splines

hatch-line:
  lines

steeljoint-line:
  joints
```

`General-Info` 首批检查属性全部存在，包括：

```text
CompanyName
ExportYesNo
ExpSteelYesNo
ExpSteelMark
DimensionChicunB
DimensionPointBarB
DimensionLineBarB
DimensionLLineBarB
DimensionBDist
DispCuttedSymb
HalfViewH
HalfViewW
Range_XMLMin_X
Range_XMLMax_Y
CutPlaneDirX0
CutPlaneDirY0
CutPlaneDirZ0
TopDirZ
DrawingType
LevelDrawing
DrawTaoTong
```

注意：

- `CompanyName = design-company` 仍是中性占位。
- 当前没有确认这是旧图石默认值。

## FDrawing / AutoCAD 状态

FDrawing 插件目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020
```

插件文件：

```text
FDrawingObj.dbx
  sha256 = a7c8493507de802f4881c0b60db3dbbe4c5982cdf9f9c42a9c805ec2feec97b0

FDrawing.arx
  sha256 = 30db8152da1d42de0f2550c6a4242bcc4781f8d5b649428c6ddcbf647a29d428
```

旧样例包：

```text
old-sample/Detail.xml
  sha256 = b02abeeb7b9e0ab802566fa617e0627d9883010172efd9cd50dacf940b5afeb5

old-sample/Detail01.stl
  sha256 = e3bb220e39f5657a38bfc6793e246208760816913520daf9c97c3544d0421c3e
```

AutoCAD 环境探测：

```text
acad.exe = not_found
accoreconsole.exe = not_found
HKLM/HKCU AutoCAD registry keys = not_found
automaticL2Possible = false
```

因此：

```text
autocadL2 = not_run
```

## IDA MCP 状态

本轮 `idb_list` 结果：

```text
sessions = []
count = 0
```

本轮没有新增 IDA 证据。原因是 `TODO-036` 的核心验收是旧 AutoCAD / FDrawing 运行接受度；当前阻塞点是运行环境不可用，而不是缺一个旧 VisualTS 函数结论。

已使用的旧字段证据仍来自：

```text
E-DETAIL-003
E-IDA-027
E-DEV-056
E-DEV-057
```

## xhigh Review

本轮代码修改后执行了 xhigh 只读 review。

第一轮结论：

```text
verdict = needs_fix
Critical = none
Important = pointStb L2 变量污染
```

主流程修复：

- 从 TODO-036 L2 package 中移除 `pointStb` fixture。
- 本轮 scope 固定为 `complex containers + General-Info only`。
- `pointStb / FaceEdge` 后续另开干净变量专项。

修复后主流程验证通过。

最终只读复审：

```text
verdict = allow_commit
Critical = none
Important = none
Route alignment = pass
AutoCAD L2 false-claim check = ok
Todo/46 consistency = pass
```

复审结论：

- 未发现 OCCT 直接写钢筋业务或父目录 rebar 业务迁入。
- 文档和 JSON 均明确 `autocadL2 = not_run`。
- `todo.csv` 和 `46` 均已切到 `TODO-037 / pointStb + FaceEdge` 独立干净变量专项。

## 验证

已执行：

```text
cmake --build app\build --target detail_l2_fixture_probe detail_writer_tests --config Debug
  -> pass

detail_l2_fixture_probe
  -> pass

app 默认 CTest
  -> 17/17 pass

py -3 tools\phase1_readiness_gate\check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass

rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
  -> no matches

git diff --check
  -> pass
```

## 不声明

未确认事项：

```text
AutoCAD L2 import 未通过。
旧插件接受当前复杂字段骨架未确认。
旧插件接受空容器或默认字段未确认。
CompanyName 旧默认值未确认。
pointStb L2 未通过。
FaceEdge 未实现。
ZValue 语义未闭合。
剖切线 / 隐藏线 / 填充线 / 接头线算法未实现。
完整工程图未完成。
golden 未采集。
```

## 下一步

推荐下一节点：

```text
TODO-037 / M2-Drawing-006
  -> DetailWriter pointStb / FaceEdge 兼容字段骨架 P0
```

原因：

- `TODO-036` 复审已经证明点筋变量混进复杂骨架 L2 会污染结论。
- `pointStb / FaceEdge` 应该作为独立干净变量专项推进。
- 这仍然不是 golden，也不是完整工程图算法。
