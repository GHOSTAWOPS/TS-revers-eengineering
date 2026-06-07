# M2-Drawing-010 DetailWriter 线容器 AutoCAD L2 运行确认准备 P0 实现记录

## 结论

本轮完成 `TODO-041 / M2-Drawing-010`：

- 复用正式 `DetailWriter` 和 `detail_l2_fixture_probe --fixture line-containers` 生成 `TODO-041` 专用三图纸 Detail 包。
- 生成 `line-containers` AutoCAD L2 手工运行确认清单。
- 记录 `FDrawingObj.dbx / FDrawing.arx` 文件存在和 hash。
- 记录本机 `acad.exe / accoreconsole.exe / AutoCAD registry` 探测结果。
- 当前本机仍未发现 AutoCAD 可执行文件或注册表键，所以 `autocadL2 = not_run`。

本轮只把 `TODO-040` 的 line-containers 独立包转成 L2 运行确认材料，不声明 AutoCAD L2 通过、旧插件接受新包、连续线 / 隐藏线 / 中心线 / 填充线生成算法、完整工程图或 golden 已完成。

## 证据 ID

新增证据：

```text
E-DEV-063
  -> TODO-041 / M2-Drawing-010 DetailWriter line-containers AutoCAD L2 运行确认准备 P0。
```

依赖证据：

```text
E-DEV-062
  -> TODO-040 line-containers LineN 字段骨架。

E-DETAIL-003
  -> 旧 Detail01.stl 样例确认 continue-line / hidden-line /
     central-line / hatch-line 容器。

GAP-DRAW-001
  -> AutoCAD 动态导入仍未运行。

GAP-DRAW-002
  -> DetailNN.stl 字段完整性和旧插件容忍度仍未闭合。
```

## 实现范围

修改文件：

```text
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

新增文档 / 报告：

```text
77_M2-Drawing-010DetailWriter线容器AutoCADL2运行确认准备P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_010_run_001.md
docs/phase1/app_build_reports/m2_drawing_010_run_001.json
docs/phase1/app_build_reports/m2_drawing_010_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_010_line_containers_l2_checklist.md
docs/phase1/app_build_reports/m2_drawing_010_detail_package/
```

## TDD / Gate 更新

本轮先补 readiness gate 单测：

```text
test_route_guardrail_requires_todo041_done_report
```

RED：

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> FAIL
  -> TODO-041 done 时 gate 未要求 77 实现记录和 m2_drawing_010_run_001.md
```

GREEN：

```text
known_reports 新增 TODO-041:
  77_M2-Drawing-010DetailWriter线容器AutoCADL2运行确认准备P0实现记录.md
  docs/phase1/app_build_reports/m2_drawing_010_run_001.md
```

## 新 Detail 包

输出目录：

```text
docs/phase1/app_build_reports/m2_drawing_010_detail_package
```

文件：

```text
Detail.xml
Detail01.stl
Detail02.stl
Detail03.stl
```

probe：

```text
docs/phase1/app_build_reports/m2_drawing_010_detail_probe.json
```

关键结果：

```text
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

包 hash：

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

## FDrawing / AutoCAD 环境

旧插件目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020
```

旧插件文件：

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
AutoCAD L2 import = not_run
```

## IDA MCP 状态

本轮没有新增 IDA 证据。

原因：

- `continue-line / hidden-line / central-line / hatch-line` 容器字段证据已经由旧 `Detail01.stl` 样例和 `E-DEV-062` 覆盖。
- 本轮目标是 L2 运行确认准备和环境阻塞记录。
- 当前阻塞点是 AutoCAD 运行环境不可用，不是缺少一个 VisualTS 函数判断。

## 手工 L2 确认清单

独立清单已落地：

```text
docs/phase1/app_build_reports/m2_drawing_010_line_containers_l2_checklist.md
```

判定边界：

```text
旧样例包和 TODO-041 新包都能导入
  -> 可把本样本记为 line-containers AutoCAD L2 pass。

旧样例包可导入，TODO-041 新包失败
  -> 不能写 pass，优先补 Detail 字段完整性 / 旧插件容忍度。

旧样例包也失败
  -> 不能判断新包问题，优先处理 AutoCAD / FDrawing 加载环境。
```

## 验证

本轮执行：

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> RED before gate mapping
  -> GREEN after TODO-041 known_reports mapping

detail_l2_fixture_probe --fixture line-containers --run-id DW-L2-TODO041-001
  -> pass

py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_010_detail_probe.json
  -> pass
```

最终验证、readiness gate、OCCT/AIS 泄漏扫描和 xhigh 只读 review 结果见：

```text
docs/phase1/app_build_reports/m2_drawing_010_run_001.md
```

## 不声明

本轮不声明：

```text
不得声明 AutoCAD L2 import 已通过。
不得声明旧 FDrawing 插件接受 TODO-041 新包。
不得声明旧插件接受 continue-line / hidden-line / central-line / hatch-line LineN 字段组合。
不得声明连续线 / 隐藏线 / 中心线 / 填充线生成规则已闭合。
不得声明 ZValue 业务语义已闭合。
不得声明真实连续线算法已实现。
不得声明真实隐藏线算法已实现。
不得声明真实中心线算法已实现。
不得声明真实填充线算法已实现。
不得声明真实剖切线算法已实现。
不得声明真实接头线算法已实现。
不得声明完整工程图已完成。
不得声明 golden 已采集。
```

## 下一步

下一节点需按 `todo.csv` 和 `46` 的最新 `status=next` 执行，不在本轮自动进入。

建议候选：

```text
继续工程图字段专项，或先补 AutoCAD / 旧图石运行确认证据。
```

原因：

- `TODO-041` 只是把 line-containers 包转成 L2 运行确认准备项。
- 旧插件是否接受当前字段组合仍要等 AutoCAD 环境或用户手工运行确认。
- 真实连续线 / 隐藏线 / 中心线 / 填充线生成算法仍是后续缺口。
