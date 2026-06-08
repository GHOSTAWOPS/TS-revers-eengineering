# M2-Drawing-028 Run 001

todoId = TODO-059
phase = M2-Drawing-028
evidenceId = E-DEV-081
idaEvidenceId = E-IDA-041
decision = generated-node112-display-statusbar-excel-static-trace

## Summary

本轮完成 `TODO-059 / generated node+112 展示/状态栏字符串链静态深追 P1`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实接头线 / Others 几何算法。

核心结论：

```text
sub_1405DC6C0
  = sum(generated node+112) over child+88 generated chain

excelExport:
  title = 钢筋下料表
  E2 = 焊头(个)      = sub_1405DC6C0(obj)
  F2 = 单下料长(mm) = sub_1405DBC20(obj, buffer)

statusBar:
  if sum(generated node+112) > 0:
    pane3 = 焊接 / 绑扎 / 套筒连接

meaningBoundary:
  generated node+112
    -> 接头 / 焊头计数展示链字段
    -> final debug symbol name still open
```

## IDA Evidence

```text
database = visualts_i64_todo051
module = VisualTS.exe
hexraysReady = true
stringsCacheSize = 16320
```

Covered functions:

```text
sub_1405DBC20
sub_1406F72A0
sub_1405EAEC0
sub_1405DFEF0
sub_1405E02D0
sub_140601D80
sub_1405DBE50
sub_1405DC6A0
sub_1405EAF40
sub_1405E0660
sub_1405DC6C0
sub_1405DC870
sub_1404554B0
sub_1406B4670
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
appBusinessCodeModified = false
algorithmImplemented = false
autocadL2 = not_run
xhighReview = not_required_docs_only
```

## Verification

```text
todoCsvParse = pass; next=TODO-060; TODO-059=done
gitDiffCheck = pass; warnings only from CRLF/LF normalization in local worktree
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
drawingBoundary = pass
projectBoundary = pass
ctest = pass; 18/18 pass
phase1GateUnitTests = pass; 22 tests
relationshipTrace = pass; REQ-DRAW-003/REQ-PROJ-001/REQ-TECH-002 include E-IDA-041/E-DEV-081 and status M2Drawing028GeneratedNodeDisplayChainTraced
```

同名 JSON 已与以上验证结果同步。

## Next

```text
nextTodo = TODO-060
task = generated node+112 旧图石运行确认与 Excel/状态栏对照 P0
```

说明：

```text
TODO-060 进入最小运行确认阶段。
如果现场条件仍不满足，只记录 blocked 事实，不把运行确认写成完成。
```
