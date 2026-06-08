# M2-Drawing-031 Run 001

todoId = TODO-062
phase = M2-Drawing-031
evidenceId = E-DEV-084
idaEvidenceId = E-IDA-042
decision = generate-package-ribbon-static-stop-point

## Correction Note

本记录保留为 `TODO-062` 的历史 run report。

但它内部把 `36124 / 35057` 与 `psallc / psexcel` 的对应关系写反了。
当前应以后续 `TODO-063` 的纠偏结果为准：

```text
36124 / 0x8D1C -> psallc -> sub_140600AA0
35057 / 0x88F1 -> psexcel -> sub_140605B20
0x8CD2 -> ysteelout
Dialog #274 -> Set2Dpage
```

不要再把本文件正文里的 `36124 -> psexcel` 或
`35057 -> dispatch -> psallc` 当成当前真值；
当前真值见 `101_M2-Drawing-032...` 与
`m2_drawing_032_run_001.*`。

## Summary

本轮完成 `TODO-062 / 生成工程图命令入口 / Ribbon 绑定静态补证 P0`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实工程图算法。

核心结论：

```text
topRibbon:
  sub_1406F37B0
    -> 工程图 / 输出 -> {36124, 35057}

knownScheduleChain:
  36124
    -> psexcel / 0x8D1C / sub_140605B20

internalWriter:
  psallc / 0x8CD2 / sub_140600AA0
    -> Detail.xml + DetailNN.stl 主写出链

meaningBoundary:
  35057
    -> 高置信“生成工程图”候选入口
    -> dispatch 和 Dialog #274 binding 仍未完全闭合
```

## IDA Evidence

```text
database = visualts_todo062_generate_drawing
module = VisualTS.exe
hexraysReady = true
coveredFunctions:
  sub_1406F37B0
  sub_140600AA0
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
todoCsvParse = pass; TODO-062=done; next=TODO-063
gitDiffCheck = pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
ctest = pass; 18/18 pass
relationshipTrace = pass; REQ-DRAW-003 includes E-IDA-042/E-DEV-084 and status M2Drawing031GeneratePackageStaticStopPointReady
```

同名 JSON 已与以上验证结果同步。

## Next

```text
nextTodo = TODO-063
task = 生成工程图 35057 到内部导出 dispatch / Dialog #274 静态补证 P1
```

说明：

```text
TODO-063 继续补中间那一跳：
  35057 -> dispatch -> psallc / 0x8CD2
  Dialog #274 -> 最终 binding
```
