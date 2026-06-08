# M2-Drawing-025 Run 001

todoId = TODO-056
phase = M2-Drawing-025
evidenceId = E-DEV-078
idaEvidenceId = E-IDA-038
decision = joint-rebuild-db6c0-static-trace

## Summary

本轮完成 `TODO-056 / 接头重建几何核心 sub_1405DB6C0 静态深追 P0`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实接头线 / Others 几何算法。

核心结论：

```text
sub_1405DB6C0(child) = 接头生成链几何重建核心

child fields:
  +88  = generated joint geometry/display chain head
  +108 = JointRuler / period, integer mm
  +112 = normalized phase / position, integer mm modulo period
  +116 = reverse flag

node fields:
  +72  = EDGE*
  +80  = owner/head for predecessor lookup
  +88  = next generated-chain node
  +104 = multi-edge connection helper participant
  +112 = summed by sub_1405DC6C0, business name open
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
sub_1405DB6C0
sub_1405DBE20
sub_1405DC6E0
sub_1405DC840
sub_1405E06F0
sub_1405E05A0
sub_1405E1E70
sub_1405D36D0
sub_14054C2C0
sub_14054C820
sub_1405BBEF0
sub_1405DC870
sub_1405DC6C0
```

## DB6C0 Reconstruction Semantics

```text
1. clear old generated chain via sub_1405DBE20(child)
2. periodM = *(int *)(child+108) / 1000.0
3. phaseM = sub_1405DC6E0(child) / 1000.0
4. if phaseM < 0.02 then phaseM = periodM
5. if child+116 reverse then start from sub_1405DC840(child)
6. iterate node+72 EDGE along node+88 or predecessor chain
7. accumulate EDGE::length(edge, 1)
8. get bounded curve and evaluate point by bounded_curve vfunc(+64)
9. call end_indexed_polygon(node, SPAposition*, int_mm_index)
10. after first hit use periodM for subsequent repeated points
```

Important disassembly fact:

```text
0x1405dbb1b  mov r8d, ebx   -> int mm index
0x1405dbb1e  mov rcx, rsi   -> generated-chain node
0x1405dbb21  mov rdx, rax   -> SPAposition*
0x1405dbb24  call direct_render_mesh_manager::end_indexed_polygon
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
appBusinessCodeModified = false
algorithmImplemented = false
autocadL2 = not_run
xhighReview = completed; critical_fixed; important_fixed
```

## Verification

```text
todoCsvParse = pass; next=TODO-057; TODO-056=done
relationshipTrace = pass; REQ-DRAW-003/REQ-PROJ-001/REQ-TECH-002 include E-IDA-038/E-DEV-078 and status M2Drawing025JointRebuildCoreTraced
gitDiffCheck = pass; CRLF/LF warnings only, no content errors
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
drawingBoundary = pass
projectBoundary = pass
ctest = pass; 18/18 pass
```

最终验证结果会回填同名 JSON。

## Next

```text
nextTodo = TODO-057
task = 接头 DB6C0 owning 结构与 Dialog #428 确定链补证 P0
```

说明：

```text
TODO-057 仍建议是 IDA / 静态证据节点。
不启动旧图石，不安装 HASP，不实现接头线 / Others 算法。
TODO-050 继续 blocked，等待用户现场 USB 狗 + HASP + 旧图石运行样例。
```
