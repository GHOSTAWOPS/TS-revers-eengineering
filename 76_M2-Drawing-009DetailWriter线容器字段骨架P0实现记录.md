# M2-Drawing-009 DetailWriter 线容器字段骨架 P0 实现记录

## 结论

本轮完成 `TODO-040 / M2-Drawing-009` 的 P0 字段骨架：

- `DetailDrawingViewOptions` 新增视图级 `continueLines / hiddenLines / centralLines / hatchLines` 字段载体。
- `DetailWriter` 可在 `PartDetailDrawing / continue-line / lines` 下输出 `LineN`。
- `DetailWriter` 可在 `PartDetailDrawing / hidden-line / lines` 下输出 `LineN`。
- `DetailWriter` 可在 `PartDetailDrawing / central-line / lines` 下输出 `LineN`。
- `DetailWriter` 可在 `PartDetailDrawing / hatch-line / lines` 下输出 `LineN`。
- `LineN` 覆盖旧样例确认的 `start_x / start_y / end_x / end_y / ZValue` 字段。
- `detail_l2_fixture_probe --fixture line-containers` 可生成独立三图纸 Detail 包，并离线确认 `lineContainers.passed=true`。

本轮只做旧样例字段骨架，不声明真实连续线、隐藏线、中心线、填充线、剖切线、接头线算法、旧插件接受度、AutoCAD L2 或 golden 已完成。

## 证据 ID

新增证据：

```text
E-DEV-062
  -> TODO-040 / M2-Drawing-009 DetailWriter continue-line / hidden-line /
     central-line / hatch-line 线容器 LineN 字段骨架 P0。
```

依赖证据：

```text
E-DETAIL-003
  -> 旧 Detail01.stl 样例确认 PartDetailDrawing 下存在
     continue-line / hidden-line / central-line / hatch-line 容器。

E-DEV-056
  -> Detail 复杂字段静态证据。

E-DEV-061
  -> TODO-039 section-line AutoCAD L2 运行确认准备和环境阻塞记录。

GAP-DRAW-001
  -> AutoCAD 动态导入仍未运行。

GAP-DRAW-002
  -> DetailNN.stl 字段完整性和旧插件容忍度仍未闭合。
```

## 实现范围

修改文件：

```text
app/src/drawing/detail/DetailWriter.h
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
app/tools/detail_l2_fixture_probe.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

新增文档 / 报告：

```text
76_M2-Drawing-009DetailWriter线容器字段骨架P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_009_run_001.md
docs/phase1/app_build_reports/m2_drawing_009_run_001.json
docs/phase1/app_build_reports/m2_drawing_009_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_009_detail_package/
```

## DTO

新增视图级 DTO：

```text
DetailLegacyLineGeometry
  startX / startY
  endX / endY
  zValue
```

兼容别名：

```text
DetailSectionLineGeometry = DetailLegacyLineGeometry
```

边界：

- DTO 位于 `drawing/detail` 输出选项层，不进入 `domain/rebar`。
- DTO 只是显式字段载体，不负责从 OCCT、钢筋领域对象或视图算法生成隐藏线 / 填充线。
- `ZValue` 只按旧样例字符串透传，业务语义继续保留为缺口。

## DetailWriter 输出

输出字段：

```text
continue-line / lines / LineN
  start_x
  start_y
  end_x
  end_y
  ZValue

hidden-line / lines / LineN
  start_x
  start_y
  end_x
  end_y
  ZValue

central-line / lines / LineN
  start_x
  start_y
  end_x
  end_y
  ZValue

hatch-line / lines / LineN
  start_x
  start_y
  end_x
  end_y
  ZValue
```

## TDD / Gate 更新

本轮先补单元测试和 readiness gate 单测：

```text
testDetailWriterWritesLineContainerFieldSkeleton
test_route_guardrail_requires_todo040_done_report
```

RED：

```text
detail_writer_tests 编译失败：
  -> DetailDrawingViewOptions 尚无 continueLines / hiddenLines /
     centralLines / hatchLines 字段。

py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> TODO-040 done 时 gate 尚未要求 76 实现记录和
     m2_drawing_009_run_001.md。
```

GREEN：

```text
DetailDrawingViewOptions 新增四类线容器字段。
DetailWriter 写出四类 LineN 字段骨架。
detail_l2_fixture_probe 新增 --fixture line-containers。
known_reports 新增 TODO-040:
  76_M2-Drawing-009DetailWriter线容器字段骨架P0实现记录.md
  docs/phase1/app_build_reports/m2_drawing_009_run_001.md
```

## 独立 Probe

`detail_l2_fixture_probe` 新增：

```text
--fixture line-containers
```

输出：

```text
runId = DW-L2-TODO040-001
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

- `continue-line / hidden-line / central-line / hatch-line` 容器证据已经由旧 `Detail01.stl` 样例和 `E-DEV-056` 覆盖。
- 本轮目标是把旧样例确认的线容器 `LineN` 字段做成 writer/probe 可验证骨架。
- 当前缺口是线生成算法、旧插件容忍度和 AutoCAD 运行确认，不是缺少一个可直接决定 P0 字段骨架的 VisualTS 函数判断。

## 验证

本轮执行：

```text
cmd /c "call D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat >nul && cmake --build app\build --config Debug --target detail_writer_tests && app\build\detail_writer_tests.exe"
  -> pass

detail_l2_fixture_probe --fixture line-containers --run-id DW-L2-TODO040-001
  -> pass

py -3 -m json.tool docs/phase1/app_build_reports/m2_drawing_009_detail_probe.json
  -> pass

app 默认 CTest
  -> 17/17 pass

readiness gate strict
  -> M1-Formal-Ready, 84/84 pass, 0 error, 0 warning

保护层 OCCT/AIS 泄漏扫描
  -> pass, no matches
```

最终 `xhigh` 只读 review 结果已写入 `m2_drawing_009_run_001` 报告：

```text
Round1:
  Critical = None
  Important = 初审 block，因为 46、run report 和 run JSON 的 review 状态仍为 pending；
              主流程 agent 已修复这些状态字段。
  Minor = None

Round2:
  Critical = None
  Important = None
  Minor = None
  Verdict = allow_commit
```

## 不声明

本轮不声明：

```text
不得声明 AutoCAD L2 import 已通过。
不得声明旧 FDrawing 插件接受 TODO-040 新包。
不得声明旧插件接受 continue-line / hidden-line / central-line / hatch-line LineN 字段组合。
不得声明连续线 / 隐藏线 / 中心线 / 填充线生成规则已闭合。
不得声明 ZValue 业务语义已闭合。
不得声明真实隐藏线算法已实现。
不得声明真实填充线算法已实现。
不得声明真实剖切线算法已实现。
不得声明真实接头线算法已实现。
不得声明完整工程图已完成。
不得声明 golden 已采集。
```

## 下一步

下一节点建议：

```text
TODO-041 / M2-Drawing-010
  -> DetailWriter line-containers AutoCAD L2 运行确认准备 P0
```

原因：

- `TODO-040` 已经把四类线容器字段骨架做成独立干净变量包。
- 下一步应像 `TODO-039` 对 `section-line` 做的事情一样，把 `line-containers` 独立包转成 AutoCAD L2 手工或自动运行确认准备项。
- 不应直接跳到真实隐藏线、填充线、中心线或连续线算法。
