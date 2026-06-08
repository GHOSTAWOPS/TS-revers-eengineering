# M2-Drawing-022 Run 001

todoId = TODO-053
phase = M2-Drawing-022
evidenceId = E-DEV-075
idaEvidenceId = E-IDA-035
decision = context-menu-command-dispatch-static-stop-point

## Summary

本轮完成 `TODO-053 / 接头 context menu / command dispatch 静态深追 P0`。

结论：

```text
动态 context menu 机制已确认。
sub_1405C2EF0 / sub_1406BA690 是关键 popup 构造路径。
LoadMenuW 资源 0xAD / 0xF8 已检查。
VisualTS.exe 全量 MENU 资源不含已知接头 command id。
已知接头 command id 仍没有普通 immediate 命中。
旧 UI caption / 右键菜单项 / command dispatch 绑定仍未闭合。
```

## IDA / Static Evidence

```text
database = visualts_i64_todo051
hexraysReady = true

CreatePopupMenu xrefs:
  sub_1405C2EF0
  sub_1406BA690

LoadMenuW xrefs:
  sub_1406B1A00 -> menu resource 0xAD
  sub_1407072E0 -> menu resource 0xF8

TrackPopupMenu xrefs:
  sub_1405C2EF0
  sub_1406B1A00
  sub_1406BA690
  sub_1407072E0
```

Known joint IDs checked:

```text
36046, 36047, 36048, 36049,
36055, 36056,
36057, 36058,
36061, 36062, 36063, 36064,
36241, 36242

immediateMatches = 0
```

Resource scan:

```text
menuResourceCount = 15
menuResourceIds = 0x7e,0x7f,0x95,0x96,0x97,0xa6,0xa8,0xa9,0xad,0xae,0xbc,0xbd,0xf8,0x1ae,0x3f7
menu 0xAD jointIds = []
menu 0xF8 jointIds = []
allMenuJointHits = 0
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
driverInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
algorithmImplemented = false
autocadL2 = not_run
xhighReview = not_required_docs_only
```

## Verification

```text
todoCsvParse = pass; next=TODO-054; TODO-053=done
gitDiffCheck = pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84
domainRebarBoundary = pass
drawingBoundary = pass
projectBoundary = pass
ctest = pass; 18/18
```

最终验证结果已回填同名 JSON。

## Next

```text
nextTodo = TODO-054
task = 接头 handler 业务对象筛选链静态分类 P0
```

说明：

```text
TODO-050 继续 blocked，等待用户现场 USB 狗 + HASP + 旧图石运行样例。
TODO-054 仍是只读 IDA / 文档节点，不启动旧图石，不实现真实算法。
```
