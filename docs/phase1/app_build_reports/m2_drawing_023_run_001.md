# M2-Drawing-023 Run 001

todoId = TODO-054
phase = M2-Drawing-023
evidenceId = E-DEV-076
idaEvidenceId = E-IDA-036
decision = joint-handler-object-filter-static-classification

## Summary

本轮完成 `TODO-054 / 接头 handler 业务对象筛选链静态分类 P0`。

结论：

```text
barjoint* 使用 selection item +13 业务对象，predicate = sub_1405E0E70 / dword_140993E74。
groupjoint* 使用 selection item +13 业务对象，predicate = sub_1405F17C0 / dword_140993EC8。
segjoint* 使用 selection item +13 业务对象，predicate = sub_1405DA020 / dword_140993E1C。
featjoint* 使用 selection item +13 业务对象，predicate = sub_14045A7F0 / dword_1409931D0。
goujianjoint* 直接筛 selection item +80 == 4，再遍历 item+120 链取 node+104 对象。
```

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实接头线 / Others 几何算法。

## IDA Evidence

```text
database = visualts_i64_todo051
module = VisualTS.exe
autoAnalysisReady = true
hexraysReady = true
stringsCacheSize = 16320
```

Handlers:

```text
barjointnew       -> sub_1405DFAA0
barjointclear     -> sub_1405DF710
barjointmove      -> sub_1405DFEF0
barjointrev       -> sub_1405E02D0
groupjointnew     -> sub_1405F0060
groupjointclear   -> sub_1405EFCC0
groupjointrev     -> sub_1405F0850
groupjointmove    -> sub_1405F0430
featjointnew      -> sub_1405EF140
featjointclear    -> sub_1405EEDA0
segjointnew       -> sub_1405D94C0
segjointclear     -> sub_1405D9450
goujianjointnew   -> sub_1405EF8D0
goujianjointclear -> sub_1405EF510
```

Key field evidence:

```text
barjoint:
  obj+108 = JointRuler / period-like field written by sub_1405E1D50
  obj+112 = normalized joint position written by sub_1405E1CC0
  obj+116 = reverse flag written by sub_1405E1D20
  obj+88  = old geometry/display chain traversed by sub_1405DB6C0 / sub_1405DBE20

groupjoint:
  groupjointmove reads *(obj+80), then sub_1405DC6E0(*(obj+80)) / 1000.0 as first distance value

goujianjoint:
  selection item +80 == 4
  item+120 -> linked nodes
  node+104 -> action object
  node+128 -> next node
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
todoCsvParse = pass; next=TODO-055; TODO-054=done
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
nextTodo = TODO-055
task = 接头 handler 动作函数字段语义深追 P0
```

说明：

```text
TODO-055 仍是 IDA / 静态证据节点。
不启动旧图石，不安装 HASP，不实现接头线 / Others 算法。
TODO-050 继续 blocked，等待用户现场 USB 狗 + HASP + 旧图石运行样例。
```
