# M2-Drawing-024 Run 001

todoId = TODO-055
phase = M2-Drawing-024
evidenceId = E-DEV-077
idaEvidenceId = E-IDA-037
decision = joint-handler-action-field-semantics-static-trace

## Summary

本轮完成 `TODO-055 / 接头 handler 动作函数字段语义深追 P0`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实接头线 / Others 几何算法。

核心结论：

```text
groupjoint object:
  +80 = child joint object chain head

child joint object:
  +72  = next child in groupjoint chain
  +88  = generated joint geometry/display chain head
  +96  = odd/even JointDistbet selector
  +108 = JointRuler / period, stored as integer mm
  +112 = normalized joint phase / position
  +116 = reverse flag
  +128 = lower-level generated ACIS/HOOPS entity chain

write helpers:
  sub_1405E1D50 -> backup + write +108
  sub_1405E1CC0 -> normalize + backup + write +112
  sub_1405E1D20 -> backup + write +116
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
sub_1405E9640  groupjointnew action
sub_1405E7960  groupjointclear / shared clear
sub_1405ED6C0  groupjointrev action
sub_1405EBA30  groupjointmove action
sub_140446AE0  feat/goujian clear adapter
sub_1405CEB60  segjointclear action
sub_14045D580  segjointnew Dialog #428 init
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
todoCsvParse = pass; next=TODO-056; TODO-055=done
relationshipTrace = pass; REQ-DRAW-003/REQ-PROJ-001/REQ-TECH-002 include E-IDA-037/E-DEV-077 and status M2Drawing024JointActionFieldsTraced
gitDiffCheck = pass after EOF blank-line cleanup
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
drawingBoundary = pass
projectBoundary = pass
ctest = pass; 18/18 pass
```

最终验证结果会回填同名 JSON。

## Next

```text
nextTodo = TODO-056
task = 接头重建几何核心 sub_1405DB6C0 静态深追 P0
```

说明：

```text
TODO-056 仍建议是 IDA / 静态证据节点。
不启动旧图石，不安装 HASP，不实现接头线 / Others 算法。
TODO-050 继续 blocked，等待用户现场 USB 狗 + HASP + 旧图石运行样例。
```
