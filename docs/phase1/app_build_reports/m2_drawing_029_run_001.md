# M2-Drawing-029 Run 001

todoId = TODO-060
phase = M2-Drawing-029
evidenceId = E-DEV-082
decision = generated-node112-runtime-checklist-blocked-facts-triage

## Summary

本轮完成 `TODO-060 / generated node+112 旧图石运行确认与 Excel/状态栏对照 P0` 的 docs-only 闭环。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实接头线 / `Others` 几何算法。

核心结论：

```text
candidateArtifacts:
  1.xls
    -> rows=1 cols=1 空白工作簿
    -> 不是旧下料表样例

  screenshot.png
    -> JDK 提示窗口
    -> 不是 pane3 截图

  visualts_prompt_capture.png
    -> 旧启动阻塞提示
    -> 不是 pane3 / Excel 证据

manualChecklist:
  -> 已固定真实 pane3 截图
  -> 已固定真实 Excel E/F 列导出
  -> 已固定输出路径 / hash 要求
```

## Artifact Triage

```text
1.xls
  SHA256 = DBCF9E3226EF111DD0FFE712EC247A0568B28191FAD0ECB55D5759C83E958049
  workbook = Sheet1
  usedRows = 1
  usedCols = 1

screenshot.png
  SHA256 = 1FFF0DB032A81DA95D73DDEC2C6ED5D431D4B81AD7DE4F1F9EC4FF9E4699480C
  meaning = JDK prompt only

visualts_prompt_capture.png
  SHA256 = 56C44CEF1FB7211131A0239264AD4BE931C7BB487E9676D59488C3A346B3E101
  meaning = VisualTS startup-block prompt only
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
todoCsvParse = pass; next=TODO-061; TODO-060=done
gitDiffCheck = pass; warnings only from CRLF/LF normalization in local worktree
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
drawingBoundary = pass
projectBoundary = pass
ctest = pass; 18/18 pass
phase1GateUnitTests = pass; 22 tests
relationshipTrace = pass; REQ-DRAW-003/REQ-DRAW-004 include E-DEV-082
```

同名 JSON 已与以上验证结果同步。

## Next

```text
nextTodo = TODO-061
task = generated node+112 旧图石真实运行截图与 Excel 导出回填 P0
```

说明：

```text
TODO-061 只接收真实旧图石截图和真实 Excel 导出。
不再把当前目录里的伪候选工件当成运行证据。
```
