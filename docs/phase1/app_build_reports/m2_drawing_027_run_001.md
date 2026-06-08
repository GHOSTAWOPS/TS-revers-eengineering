# M2-Drawing-027 Run 001

todoId = TODO-058
phase = M2-Drawing-027
evidenceId = E-DEV-080
idaEvidenceId = E-IDA-040
decision = joint-dialog-message-map-child-node112-static-trace

## Summary

本轮完成 `TODO-058 / JoingSegDlg message map 与 child/node+112 字段收口 P1`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实接头线 / Others 几何算法。

核心结论：

```text
messageMap:
  sub_14045D700
    -> off_14075B110
    -> 0x14075B090 entries

radioHandlers:
  1380 -> sub_14045DA00 -> dialog+304 = 0
  1384 -> sub_14045DA10 -> dialog+304 = 1
  1381 -> sub_14045DA20 -> dialog+304 = 2

applyStopPoint:
  no standalone Apply entry in JoingSegDlg static message map
  dialog still uses default OK/Cancel framework statically

child+112:
  phase / position / start offset
  unit = integer mm
  write path = sub_1405E1CC0
  read path  = sub_1405DC6E0(child) = child+112 % child+108

generated node+112:
  child+88 generated-chain int mm contribution field
  summed by sub_1405DC6C0
  used by formatting / status display / rebuild precondition callers

open stop points:
  generated node+112 final business debug name
  old Chinese caption / runtime non-static Apply shape
  runtime non-empty steeljoint-line/Others sample
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
sub_14045D6F0
sub_14045D700
sub_14045D580
sub_14045D650
sub_14045D720
sub_14045DFF0
sub_14045DA00
sub_14045DA10
sub_14045DA20
sub_1405DC6C0
sub_1405DC6E0
sub_1405DBC20
sub_1405DFEF0
sub_1405E02D0
sub_1405EAEC0
sub_1406F72A0
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
appBusinessCodeModified = false
algorithmImplemented = false
autocadL2 = not_run
xhighReview = completed; important_fixed; rereview_clean
```

## Verification

```text
todoCsvParse = pass; next=TODO-059; TODO-058=done
relationshipTrace = pass; REQ-DRAW-003/REQ-PROJ-001/REQ-TECH-002 include E-IDA-040/E-DEV-080 and status M2Drawing027JointDialogMessageMapTraced
gitDiffCheck = pass; warnings only from CRLF/LF normalization in local worktree, no content errors
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
drawingBoundary = pass
projectBoundary = pass
ctest = pass; 18/18 pass
phase1GateUnitTests = pass; 22 tests
```

同名 JSON 已与以上验证结果同步；第二轮 xhigh 只读 review 已完成，结论为 clean after fix。

## Next

```text
nextTodo = TODO-059
task = generated node+112 展示/状态栏字符串链静态深追 P1
```

说明：

```text
TODO-059 仍建议是 IDA / 静态证据节点。
不启动旧图石，不安装 HASP，不实现接头线 / Others 算法。
```
