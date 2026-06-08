# M2-Drawing-033 Run 001

todoId = TODO-064
phase = M2-Drawing-033
evidenceId = E-DEV-086
idaEvidenceId = E-IDA-044
decision = generate-package-caption-resource-closure-and-modal-candidate-narrowing

## Summary

本轮完成 `TODO-064 / 生成工程图与下料表中文caption资源绑定及真正生成工程图弹窗候选补证 P1`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实工程图算法。

核心结论：

```text
captionClosure:
  36124 = 生成工程图
  35057 = 下料表

resourcePattern:
  描述/help \n caption

supportingExample:
  36050 -> 输出钢筋几何 \n 输出钢筋

modalCandidateNarrowing:
  sub_140600AA0
    -> sub_140601600
    -> sub_1404FE300
    -> CDialog::DoModal
    -> output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT

dialog427:
  OptionFactory
  not direct GenerateBarSchedule dialog truth
```

## IDA Evidence

```text
database = visualts_todo062_generate_drawing
module = VisualTS.exe
hexraysReady = true
coveredFunctions:
  sub_140600AA0
  sub_140601600
  sub_1404FE300
  sub_1406AC240
coveredResources:
  36124
  35057
  36050
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
todoCsvParse = pass; TODO-064=done; next=TODO-065
gitDiffCheck = pass; CRLF normalization warnings only
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
ctest = pass; 18/18 pass
domainRebarOCCLeak = pass; no matches in app/src/domain/rebar; domain_rebar_boundary also passed in CTest
relationshipTrace = pass; REQ-DRAW-003/004 include E-IDA-044/E-DEV-086 and next switched to TODO-065
```

同名 JSON 已与以上验证结果同步。

## Next

```text
nextTodo = TODO-065
task = 生成工程图与下料表旧图石运行确认清单与工件门禁准备 P0
```

说明：

```text
TODO-065 不再继续追静态 caption。

下一步转到运行确认准备：
  生成工程图输出目录 / 文件列表 / hash / 覆盖策略模板
  下料表真实输出结果和额外运行时弹窗采样门禁
```
