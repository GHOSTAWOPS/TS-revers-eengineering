# M2-RebarCreate-007 线配筋旧图石运行确认清单与工件门禁 P0 实现记录

todoId = TODO-077
phase = M2-RebarCreate-007
evidenceIds = E-DEV-099

## 目标

本轮只完成 `TODO-077 / 线配筋旧图石运行确认清单与工件门禁 P0`。

目标是把 TODO-076 静态 stop point 中还没闭合的内容，
转成用户可执行的旧图石运行确认清单：

```text
旧线配筋失败提示
旧状态栏 pane 文案
旧参数窗口字段 / 默认值 / 单位 / 灰显状态
公共 Input_float / Dialog #383 的真实运行标题 / 标签 / 默认值
无选择 / 选错对象 / 有效对象三条点击路径
```

本轮是运行确认准备，不实现算法。

## 本轮新增工件

```text
docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture/
  README.md
  capture_notes.md
  collect_hashes.ps1
```

## 采样清单

后续现场运行旧图石时，至少要覆盖：

```text
1. 无选择点击线配筋。
2. 选错对象点击线配筋。
3. 有效对象点击线配筋。
4. 有效对象参数窗口 OK / Cancel / Apply 行为。
5. 若出现 Dialog #383，记录标题、标签、默认值、单位和按钮行为。
6. 状态栏点击前 / 点击后 pane 文本。
7. 模型树或输出工件点击前 / 点击后状态。
```

## 拒收规则

本轮明确拒收：

```text
1. 只有菜单截图，没有点击结果。
2. 截图没有状态栏或弹窗上下文，却声称关闭状态栏 / 提示缺口。
3. Dialog #383 截图缺标题、标签、默认值或按钮区域。
4. 新 Qt6 app 的 P0 参数窗口截图冒充旧 VisualTS 证据。
5. 没有 SFL 文件名、hash 和操作步骤。
6. 只有启动许可提示或 HASP 阻塞提示。
7. 只有 hash，没有文件名、路径和操作步骤。
8. 没有点击前后对照，却声称模型树或输出发生变化。
9. 父目录旧工程截图或其他项目 Detail / STP 混入本轮证据。
10. 没有说明 USB 狗 / HASP 状态，却把失败归因写死。
```

## 工程结论

当前能写入：

```text
旧运行确认清单和拒收伪工件规则已准备好。
TODO-076 的旧提示 / 状态栏 / 参数窗口缺口没有被静态证据关闭。
后续真实运行回填应优先关闭 GAP-UI-REB-001 和 GAP-IDA-002。
```

当前不能写入：

```text
旧线配筋失败提示已确认。
旧线配筋状态栏 pane 文案已确认。
旧线配筋主参数窗口字段已确认。
Dialog #383 已确认出现在真实线配筋流程。
当前 LineGroupParameterDialog 已经等于旧图石窗口。
完整线配筋算法 / golden 已完成。
```

## 边界

本轮没有做：

```text
没有启动旧图石。
没有安装 HASP。
没有修改系统目录。
没有改 RebarGroupCreator 算法。
没有改 LineGroupParameterDialog 字段。
没有改 MainWindow UI 文案。
没有迁入父目录 rebar 业务。
没有让 domain/rebar 依赖 OCCT/AIS。
没有实现面配筋、弧筋、接头、Excel、Detail 或 golden。
```

## 验证

```text
readinessGateUnit = pass
readinessGateStrict = pass
defaultCTest = pass
domainRebarCommandOCCLeak = pass
gitDiffCheck = pass
xhighReview = needs_fix_important_fixed
```

验证命令：

```text
py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> Ran 40 tests, OK

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_077
  -> decision=M1-Formal-Ready, errorCount=0, warningCount=0, passCount=84/84

ctest --test-dir .\app\build --output-on-failure
  -> 100% tests passed, 0 tests failed out of 21, total=207.14 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches

git diff --check
  -> exit 0; CRLF normalization warnings only for 11_需求证据追溯矩阵.md and todo.csv
```

xhigh 只读 review：

```text
Assessment = needs_fix
Critical = none
Important fixed:
  1. 回填 TODO-077 done report 的真实验证结果，移除 pending 状态。
  2. 将“最终验证前占位”加入 done-report pending marker，
     并补 TODO-077 负例测试，防止占位报告被误放行。
Minor deferred:
  - TODO-077 accept test 目前只验证文件存在和无 pending marker；
    后续可继续增强 todoId / phase 断言，但不阻塞本轮 P0。
Reviewer agent = closed
```

## 下一步

建议下一步进入：

```text
TODO-078 / M2-RebarCreate-008
线配筋旧图石运行确认工件回填 P0
```

下一步只接收和核对真实旧图石运行工件，
不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 或 golden。
