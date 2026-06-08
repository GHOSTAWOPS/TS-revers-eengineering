# M2-Drawing-026 Run 001

todoId = TODO-057
phase = M2-Drawing-026
evidenceId = E-DEV-079
idaEvidenceId = E-IDA-039
decision = joint-db6c0-owning-dialog-static-trace

## Summary

本轮完成 `TODO-057 / 接头 DB6C0 owning 结构与 Dialog #428 确定链补证 P0`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实接头线 / Others 几何算法。

核心结论：

```text
type ids:
  dword_140993E74  -> steelbar
  dword_140993EC8 -> steelbargroup
  dword_140993E1C -> seg_steelbargroup

Dialog #428:
  openChain:
    sub_1405D94C0
      -> sub_14045D580
      -> vtable+728(dialog, 428, 0)
      -> ShowWindow(5)

  okChain:
    sub_14045D720(OnOK)
      -> sub_1405CB160(segObj, mode, firstOffset, period)
      -> sub_1405B7350(per-node)

child+112:
  best current name = phase / position / 起始偏移
  unit = integer mm
  write path = sub_1405E1CC0
  read path  = sub_1405DC6E0(child) = child+112 % child+108

open stop points:
  generated node+112 business name
  Dialog #428 standalone Apply handler
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
sub_1405E0EA0
sub_1405F1820
sub_1405DA0E0
sub_1405D94C0
sub_14045D580
sub_14045D650
sub_14045D720
sub_14045DFF0
sub_14045DA00
sub_14045DA10
sub_14045DA20
sub_1405CB160
sub_1405B7350
sub_1405DFAA0
sub_1405E9640
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
appBusinessCodeModified = false
algorithmImplemented = false
autocadL2 = not_run
xhighReview = completed; important_fixed; minor_refined
```

## Verification

```text
todoCsvParse = pass; next=TODO-058; TODO-057=done
relationshipTrace = pass; REQ-DRAW-003/REQ-PROJ-001/REQ-TECH-002 include E-IDA-039/E-DEV-079 and status M2Drawing026JointOwningDialogTraced
gitDiffCheck = pass; warnings only from unrelated parent worktree files, no content errors
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
drawingBoundary = pass
projectBoundary = pass
ctest = pass; 18/18 pass
phase1GateUnitTests = pass; 18 tests
```

最终验证结果会回填同名 JSON。

## Next

```text
nextTodo = TODO-058
task = JoingSegDlg message map 与 child/node+112 字段收口 P1
```

说明：

```text
TODO-058 仍建议是 IDA / 静态证据节点。
不启动旧图石，不安装 HASP，不实现接头线 / Others 算法。
```
