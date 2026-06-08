# M2-Drawing-034 Run 001

todoId = TODO-065
phase = M2-Drawing-034
evidenceId = E-DEV-087
decision = generate-package-and-schedule-runtime-capture-prep

## Summary

本轮完成 `TODO-065 / 生成工程图与下料表旧图石运行确认清单与工件门禁准备 P0`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实工程图算法。

核心结论：

```text
runtimeCaptureTemplate:
  docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/
    README.md
    capture_notes.md

generatePackageChecklist:
  output directory listing
  Detail.xml / DetailNN.stl file names
  SHA256
  LastWriteTime
  rerun overwrite behavior
  optional UnCutSteel.TXT witness

scheduleChecklist:
  output path
  file name
  SHA256
  result screenshot or file
  extra runtime dialog screenshot if any

rejectRules:
  UnCutSteel.TXT alone is not success proof
  menu screenshot alone is not runtime evidence
  blank workbook / unrelated prompt images are rejected
  hashes without file names / paths / steps are rejected
  overwrite claims without before/after evidence are rejected
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
todoCsvParse = pass; TODO-065=done; next=TODO-066
gitDiffCheck = pass; CRLF normalization warnings only
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
ctest = pass; 18/18 pass
domainRebarOCCLeak = pass; no matches in app/src/domain/rebar; domain_rebar_boundary also passed in CTest
relationshipTrace = pass; REQ-DRAW-003/004 include E-DEV-087 and next switched to TODO-066
```

同名 JSON 已与以上验证结果同步。

## Next

```text
nextTodo = TODO-066
task = 生成工程图与下料表旧图石真实运行工件回填 P0
```

说明：

```text
TODO-066 不再继续补模板。

下一步转到真实工件回填：
  生成工程图输出目录 / 文件 / hash / 覆盖行为
  下料表输出结果 / 额外运行时弹窗
```
