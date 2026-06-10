# M2-RebarCreate-015 Run 001

todoId = TODO-085
phase = M2-RebarCreate-015
evidenceIds = E-IDA-049, E-DEV-106, E-DEV-107
decision = done_line_group_dirty_transaction_p0

## Summary

本轮完成 `TODO-085 / 线配筋命令成功 dirty/transaction 状态 P0`。

核心结果：

```text
CommandResult
  -> 新增 CommandDirtyFlags
  -> 新增 CommandTransactionState

Rebar.Create.LineGroup success
  -> ProjectDirty + RebarDirty + DrawingDirty
  -> transaction.commandKey = Rebar.Create.LineGroup
  -> legacyDirtyEvidenceId = E-IDA-049
  -> unresolvedDirtyParityGap = GAP-REB-C-002

MainWindow
  -> 成功创建后累计 app dirty transaction
  -> no selection / wrong type / cancel / failure 不 dirty
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
uiTextChanged = false
oldDirtyUndoSaveParityImplemented = false
fullLineGroupAlgorithmImplemented = false
faceRebarImplemented = false
arcRebarExpanded = false
jointImplemented = false
excelImplemented = false
detailChanged = false
golden = not_started
occtDirectRebarRewrite = false
parentRebarMigrated = false
domainRebarOcctLeak = pass
```

## Verification

```text
targetedDirtyTransactionTests = pass
targetedSmoke = pass
defaultCTest = pass
readinessGateUnit = pass
readinessGateStrict = pass
domainRebarCommandOCCLeak = pass
todoSingleNext = pass
gitDiffCheck = pass
xhighReview = allow_commit
agentClosed = true
```

Fresh verification commands:

```text
cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build --target rebar_line_group_command_handler_tests line_group_display_smoke_tests && ctest --test-dir .\app\build -R rebar_line_group_command_handler_tests --output-on-failure"
  -> rebar_line_group_command_handler_tests pass

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && ctest --test-dir .\app\build -R line_group_display_smoke_123 --output-on-failure"
  -> line_group_display_smoke_123 pass, 77.47 sec

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build --target command_registry_tests tsrebar_app && ctest --test-dir .\app\build -R command_registry_tests --output-on-failure"
  -> command_registry_tests pass

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && ctest --test-dir .\app\build -R app_smoke --output-on-failure"
  -> app_smoke pass

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build && ctest --test-dir .\app\build --output-on-failure"
  -> 21/21 tests passed, total 208.21 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no hits, exit code 1

Import-Csv .\todo.csv | Where-Object { $_.status -eq 'next' }
  -> TODO-086 only

git diff --check
  -> pass, exit code 0; CRLF warning for todo.csv only
```

Final readiness gate and xhigh review are updated after full verification and readonly review complete.

xhigh readonly review:

```text
reviewer = Halley
verdict = allow_commit
critical = none
important = none
minor = none
requiredMainFlowFixes = none
agentClosed = true
```

Reviewer note:

```text
applyDirtyStateFromCommandResult before displayCreatedLineGroup is acceptable for this P0:
handler Completed already means SteelData was appended; AIS display failure is presentation failure,
not a model rollback condition.
```

## Residual Gaps

```text
GAP-REB-C-002 = full line rebar creation, undo/dirty/runtime golden still open
GAP-DEV-004 = old dirty/save prompt runtime confirmation still open
GAP-UI-REB-001 = old LineGroup dialog fields/defaults/status prompts still open
```

## Next

```text
nextTodo = TODO-086
action = line group dirty state save-clear entry preparation P0
```

`TODO-086` 不应自动开始；本轮完成后停下复盘。
