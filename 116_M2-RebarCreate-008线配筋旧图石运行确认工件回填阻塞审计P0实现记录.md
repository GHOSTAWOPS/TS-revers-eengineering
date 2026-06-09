# M2-RebarCreate-008 线配筋旧图石运行确认工件回填阻塞审计 P0 实现记录

todoId = TODO-078
phase = M2-RebarCreate-008
evidenceIds = E-DEV-100
decision = blocked_runtime_artifacts_missing

## 目标

本轮只处理 `TODO-078 / 线配筋旧图石运行确认工件回填 P0`。

目标是核对 `TODO-077` 采样模板目录中是否已经存在用户现场旧图石运行工件：

```text
无选择点击结果
选错对象点击结果
有效对象点击结果
旧主参数窗口截图
Dialog #383 运行截图和字段
状态栏 pane 截图
模型树或输出前后状态
hashes.txt
填写完整的 capture_notes.md
```

## 审计结果

检查目录：

```text
docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture/
```

当前只存在模板文件：

```text
README.md
capture_notes.md
collect_hashes.ps1
```

当前缺失：

```text
hashes.txt
case_no_selection_*.png
case_wrong_object_*.png
case_valid_*.png
line_group_*_listing.txt
line_group_*_hashes.txt
填写后的旧图石版本 / SFL hash / 操作步骤 / 状态栏 pane / 参数窗口字段
```

`capture_notes.md` 仍是空模板，未填写旧图石版本、SFL 文件名/hash、命令入口、三条点击路径、参数窗口字段、状态栏 pane 或操作步骤。

## 工程结论

本轮不能把 `TODO-078` 标记为完成。

原因很简单：

```text
TODO-078 的验收对象是真实旧图石运行工件。
当前目录只有采样模板，没有真实截图、hash、listing 或填写后的 notes。
```

因此：

```text
TODO-078 = blocked
GAP-UI-REB-001 = 继续 open
GAP-IDA-002 = 继续 open
REQ-REB-C-002 = RuntimeCaptureBackfillBlocked
```

## 下一步策略

为了不在缺用户运行工件时空转，下一步新增静态补证节点：

```text
TODO-079 / M2-RebarCreate-009
线配筋旧 UI / Dialog 静态资源补证 P0
```

`TODO-079` 只允许：

```text
IDA MCP / 静态资源 / 文档补证
不启动旧图石
不安装 HASP
不修改 RebarGroupCreator
不修改 LineGroupParameterDialog 文案
不实现完整线配筋算法
不采 golden
```

它不能替代 `TODO-078` 的现场运行确认，只是在等待真实运行工件期间继续收窄旧 UI / Dialog / 状态栏证据。

## 边界

本轮没有做：

```text
没有启动旧图石。
没有安装 HASP。
没有修改系统目录。
没有改 app 业务代码。
没有改 RebarGroupCreator 算法。
没有改 LineGroupParameterDialog 字段或文案。
没有迁入父目录 rebar 业务。
没有让 domain/rebar 依赖 OCCT/AIS。
没有实现面配筋、弧筋、接头、Excel、Detail 或 golden。
```

## 验证

```text
runtimeCaptureAudit = blocked_runtime_artifacts_missing
readinessGateUnit = pass
readinessGateStrict = pass
defaultCTest = pass
domainRebarCommandOCCLeak = pass
todoSingleNext = pass
gitDiffCheck = pass
xhighReview = not_required_docs_only
```

说明：

```text
本轮只改文档、run report 和 todo 状态。
未改代码、测试或构建脚本，所以 xhigh 只读 review 不强制。
```

验证命令：

```text
py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> Ran 40 tests, OK

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_078_blocked
  -> decision=M1-Formal-Ready, errorCount=0, warningCount=0, passCount=84/84

ctest --test-dir .\app\build --output-on-failure
  -> 100% tests passed, 0 tests failed out of 21, total=208.54 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches

Import-Csv .\todo.csv | Where-Object { $_.status -eq 'next' }
  -> TODO-079 only

git diff --check
  -> exit 0; CRLF normalization warnings only for 11_需求证据追溯矩阵.md and todo.csv
```

## 下一步

建议下一步进入：

```text
TODO-079 / M2-RebarCreate-009
线配筋旧 UI / Dialog 静态资源补证 P0
```

完成后再等待用户补齐 `TODO-078` 真实旧图石运行工件。
