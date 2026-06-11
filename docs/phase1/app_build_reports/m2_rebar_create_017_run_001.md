# M2-RebarCreate-017 Run 001

todoId = TODO-087
phase = M2-RebarCreate-017
evidenceIds = E-DEV-046, E-DEV-108, E-DEV-109
decision = done_line_group_save_package_runtime_openback_p0

## Summary

本轮完成 `TODO-087 / 线配筋保存包 runtime 回读验证 P0`。

核心结果：

```text
Project.Save 产出的 .tsrebar package
  -> 可被 TsRebarProjectRuntime.open(...) 回读
  -> finalState = OpenedWarning
  -> bindingDecision != blocked
  -> 恢复 1 个 SteelBarGroup / SteelBar / SteelBarSegment
  -> 恢复 group binding geometryPath
  -> 恢复 evidence_index.json 中的 evidence / gap id
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
usbDogInheritedByNewSystem = false
oldSaveUiImplemented = false
oldOpenUiImplemented = false
oldUndoSaveParityImplemented = false
sflCompatibilityImplemented = false
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
targetedRuntimeOpenBackTests = pass
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

cmd /c "call ""%bat%"" >nul && cmake --build .\app\build --target project_runtime_tests --config Debug"
  -> project_runtime_tests build pass

cmd /c "call ""%bat%"" >nul && ctest --test-dir .\app\build -C Debug -R project_runtime_tests --output-on-failure"
  -> project_runtime_tests pass

cmd /c "call ""%bat%"" >nul && cmake --build .\app\build --target line_group_display_smoke_tests --config Debug"
  -> line_group_display_smoke_tests build pass

cmd /c "call ""%bat%"" >nul && ctest --test-dir .\app\build -C Debug -R line_group_display_smoke_123 --output-on-failure"
  -> line_group_display_smoke_123 pass

cmd /c "call ""%bat%"" >nul && cmake --build .\app\build --config Debug"
  -> full build pass

cmd /c "call ""%bat%"" >nul && ctest --test-dir .\app\build -C Debug --output-on-failure"
  -> default CTest pass

py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> readiness gate unit pass

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_087_final
  -> strict readiness gate pass

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no hits, exit code 1

git diff --check
  -> pass, exit code 0
```

## Xhigh Review

```text
mode = readonly
verdict = allow_commit
critical = none
important = none
agentClosed = true
```

## Residual Gaps

```text
GAP-DEV-001 = full app Save/Open UI and binding repair workflow still open
GAP-REB-C-002 = old dirty / undo / save parity still open
GAP-UI-REB-001 = old LineGroup dialog / status prompt runtime confirmation still open
```

## Next

```text
historicalNextBeforePivot = TODO-088
historicalAction = Project.Open runtime snapshot 恢复到 app 内存 / 显示入口 P0
currentNextAfterPivot = TODO-090 / M1-Detail-001 DetailPackage 数据模型 P0
```

`TODO-088` 是 2026-06-11 路线切换前的历史 next。
路线切换后不应自动开始；当前 next 以 `todo.csv` 和 `46` 为准。
