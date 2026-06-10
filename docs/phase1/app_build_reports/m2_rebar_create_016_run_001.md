# M2-RebarCreate-016 Run 001

todoId = TODO-086
phase = M2-RebarCreate-016
evidenceIds = E-IDA-049, E-DEV-107, E-DEV-108
decision = done_line_group_dirty_save_clear_entry_p0

## Summary

本轮完成 `TODO-086 / 线配筋 dirty 状态与保存清除入口准备 P0`。

核心结果：

```text
Project.Save P0
  -> MainWindow 调用 TsRebarProjectRuntime.saveSnapshot
  -> 成功保存后清 ProjectDirty / RebarDirty / DrawingDirty
  -> 保存失败保持 dirty
  -> 记录 lastSaveResultForInspection

LineGroup save snapshot
  -> 从当前 SteelData 构造 TsRebarProjectSnapshot
  -> 在 app 保存边界把 binding geometryPath 重写为 runtime 可验证 JSON path
  -> 不污染 domain/rebar
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
usbDogInheritedByNewSystem = false
oldSaveUiImplemented = false
oldUndoSaveParityImplemented = false
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
targetedSaveClearTests = pass
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
$bat = 'D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat'
cmd /c "call ""%bat%"" >nul && cmake --build .\app\build --target line_group_display_smoke_tests --config Debug"
  -> line_group_display_smoke_tests build pass

cmd /c "call ""%bat%"" >nul && ctest --test-dir .\app\build -C Debug -R line_group_display_smoke_123 --output-on-failure"
  -> line_group_display_smoke_123 pass, 78.58 sec

cmd /c "call ""%bat%"" >nul && cmake --build .\app\build --target command_registry_tests line_group_display_smoke_tests --config Debug"
  -> command_registry_tests / line_group_display_smoke_tests build pass

cmd /c "call ""%bat%"" >nul && ctest --test-dir .\app\build -C Debug -R command_registry_tests --output-on-failure && ctest --test-dir .\app\build -C Debug -R line_group_display_smoke_123 --output-on-failure"
  -> command_registry_tests pass
  -> line_group_display_smoke_123 pass, 77.03 sec

cmd /c "call ""%bat%"" >nul && cmake --build .\app\build --config Debug"
  -> full build pass; final rerun after xhigh minor fix rebuilt MainWindow.cpp and linked tsrebar_app / line_group_display_smoke_tests

cmd /c "call ""%bat%"" >nul && ctest --test-dir .\app\build -C Debug --output-on-failure"
  -> 21/21 tests passed, final rerun total 257.91 sec

py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> 60/60 tests passed

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_086_final
  -> M1-Formal-Ready, 84/84 pass

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no hits, exit code 1

git diff --check
  -> pass, exit code 0
```

Final strict gate was rerun after xhigh readonly review and report closeout.

## Xhigh Review

```text
agent = 019eaf3c-9af9-7f52-b559-318f98a1ba78 / Einstein
mode = readonly
verdict = allow_commit
critical = none
important = none
minor = removed unused <set> include from app/src/app/MainWindow.cpp
agentClosed = true
```

## Residual Gaps

```text
GAP-REB-C-002 = old dirty / undo / save parity still open
GAP-DEV-001 = full app Save/Open UI and binding repair workflow still extend beyond this P0
GAP-UI-REB-001 = old LineGroup dialog / status prompt runtime confirmation still open
```

## Next

```text
nextTodo = TODO-087
action = line group saved package runtime open-back verification P0
```

`TODO-087` 不应自动开始；本轮完成后停下复盘。
