# M2-Drawing-008 DetailWriter section-line AutoCAD L2 运行确认准备 P0 实现记录

## 结论

本轮完成 `TODO-039 / M2-Drawing-008`：

- 复用正式 `DetailWriter` 和 `detail_l2_fixture_probe --fixture section-line` 生成 `TODO-039` 专用三图纸 Detail 包。
- 生成 `section-line` AutoCAD L2 手工运行确认清单。
- 记录 `FDrawingObj.dbx / FDrawing.arx` 文件存在和 hash。
- 记录本机 `acad.exe / accoreconsole.exe / AutoCAD registry` 探测结果。
- 当前本机仍未发现 AutoCAD 可执行文件或注册表键，所以 `autocadL2 = not_run`。

本轮只把 `TODO-038` 的 section-line 独立包转成 L2 运行确认材料，不声明 AutoCAD L2 通过、旧插件接受新包、剖切线生成算法、隐藏线 / 填充线 / 接头线算法、`ZValue` 语义或 golden 已完成。

## 证据 ID

新增证据：

```text
E-DEV-061
  -> TODO-039 / M2-Drawing-008 DetailWriter section-line AutoCAD L2 运行确认准备 P0。
```

依赖证据：

```text
E-DEV-060
  -> TODO-038 section-line Line/Arc/ZValue 字段骨架。

E-DETAIL-003
  -> 旧 Detail01.stl 样例确认 section-line LineN / ArcN / ZValue 字段。

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
75_M2-Drawing-008DetailWriterSectionLineAutoCADL2运行确认准备P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_008_run_001.md
docs/phase1/app_build_reports/m2_drawing_008_run_001.json
docs/phase1/app_build_reports/m2_drawing_008_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_008_section_line_l2_checklist.md
docs/phase1/app_build_reports/m2_drawing_008_detail_package/
```

## TDD / Gate 更新

本轮先补 readiness gate 单测：

```text
test_route_guardrail_requires_todo039_done_report
```

RED：

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> FAIL
  -> TODO-039 done 时 gate 未要求 75 实现记录和 m2_drawing_008_run_001.md
```

GREEN：

```text
known_reports 新增 TODO-039:
  75_M2-Drawing-008DetailWriterSectionLineAutoCADL2运行确认准备P0实现记录.md
  docs/phase1/app_build_reports/m2_drawing_008_run_001.md

py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> 11 tests OK
```

## 新 Detail 包

输出目录：

```text
docs/phase1/app_build_reports/m2_drawing_008_detail_package
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
docs/phase1/app_build_reports/m2_drawing_008_detail_probe.json
```

关键结果：

```text
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
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
```

包 hash：

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

- `section-line / LineN / ArcN / ZValue` 字段证据已经由旧 `Detail01.stl` 样例和 `E-DEV-060` 覆盖。
- 本轮目标是 L2 运行确认准备和环境阻塞记录。
- 当前阻塞点是 AutoCAD 运行环境不可用，不是缺少一个 VisualTS 函数判断。

## 手工 L2 确认清单

独立清单已落地：

```text
docs/phase1/app_build_reports/m2_drawing_008_section_line_l2_checklist.md
```

判定边界：

```text
旧样例包和 TODO-039 新包都能导入
  -> 可把本样本记为 section-line AutoCAD L2 pass。

旧样例包可导入，TODO-039 新包失败
  -> 不能写 pass，优先补 Detail 字段完整性 / 旧插件容忍度。

旧样例包也失败
  -> 不能判断新包问题，优先处理 AutoCAD / FDrawing 加载环境。
```

## 验证

本轮执行：

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> 11 tests OK

detail_l2_fixture_probe --fixture section-line --run-id DW-L2-TODO039-001
  -> pass

py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_008_detail_probe.json
  -> pass

app 默认 CTest
  -> 17/17 pass

readiness gate strict
  -> M1-Formal-Ready, 84/84 pass, 0 error, 0 warning

保护层 OCCT/AIS 泄漏扫描
  -> pass, no matches

git diff --check
  -> no whitespace errors

xhigh 只读 review
  -> first review block: Important 指出 46 仍有旧 TODO-033 短期 Goal 块
  -> 已修复 46，使短期 Goal / 当前下一步 / 当前执行建议 / CSE 控制合同指向 TODO-040
  -> rerun allow_commit; Critical 0; Important 0; Minor 0
```

## 不声明

本轮不声明：

```text
AutoCAD L2 import 已通过。
旧 FDrawing 插件接受 TODO-039 新包。
旧插件接受 section-line Line/Arc/ZValue 字段组合。
section-line 生成规则已闭合。
ZValue 三段值业务语义已闭合。
真实剖切线算法已实现。
隐藏线 / 填充线 / 接头线算法已实现。
完整工程图已完成。
golden 已采集。
```

## 下一步

下一节点建议：

```text
TODO-040 / M2-Drawing-009
  -> DetailWriter continue-line / hidden-line / central-line / hatch-line 线容器字段骨架 P0
```

原因：

- `section-line` 已经完成字段骨架和 L2 准备。
- 旧样例还确认 `continue-line / hidden-line / central-line / hatch-line` 容器存在。
- 下一步只应做线容器字段骨架和干净变量 probe，不实现隐藏线、填充线或真实工程图算法。
