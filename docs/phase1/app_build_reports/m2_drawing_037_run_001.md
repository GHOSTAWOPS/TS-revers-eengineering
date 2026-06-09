# M2-Drawing-037 Run 001

todoId = TODO-068
phase = M2-Drawing-037
evidenceId = E-DEV-090
decision = detail-writer-runtime-package-format-delta-complete

## Summary

本轮完成 `TODO-068 / DetailWriter 真实字段差异 P0：空 Detail.xml 与主图表格策略`。

本轮没有启动旧图石，没有安装 HASP，没有运行 AutoCAD L2，没有实现 Excel writer，没有实现真实工程图算法，也没有修改钢筋创建业务。

## Changed Code

```text
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
```

## Implemented

```text
Detail.xml:
  output = <StyleRoot/>\r\n
  evidence = RUN-20260609-001 real old package

DetailNN table distribution:
  Detail01.stl = StbTable + MaterialTable
  Detail02.stl+ = empty StbTables container

StbTable attributes:
  count
  HeightValue0
  HeightValueCount
  Volume1225
  NumCombineGoJians
  SteelNetArea
  GJTAOTNumber
  GJTAOTVolue
  LinkTop
  LinkDown
  DCGQSJ
  HYLJJ
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
excelWriterImplemented = false
autocadL2 = not_run
golden = not_started
rebarBusinessCodeModified = false
parentRebarMigrated = false
```

## Verification

```text
detailWriterTdd = pass; new test failed before implementation and passed after implementation
detailWriterTargetBuild = pass; cmake --build app/build --target detail_writer_tests
detailWriterTests = pass; ctest -R detail_writer_tests
fullBuildDebug = pass; cmake --build app/build --config Debug
ctest = pass; 18/18 pass; total 132.26 sec
domainRebarOCCLeak = pass; rg TopoDS_/AIS_/BRep/TopAbs_ app/src/domain/rebar returned no matches
gitDiffCheck = pass; warnings only for existing CRLF normalization notice in 11_需求证据追溯矩阵.md and todo.csv
readinessGateUnit = pass; py tools/phase1_readiness_gate/test_phase1_readiness_gate.py; 26 tests OK
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
xhighReview = needs_fix_resolved; no Critical; Important stale 99 TODO-068 route fixed by main flow; agent closed
```

## Residual Gaps

```text
GAP-DRAW-006 = old plugin existence/acceptance requirement still needs AutoCAD L2
GAP-DRAW-007 = StbRow extra attrs, Std# multi-container, table formulas and lineStb field narrowing remain open
GAP-DRAW-008 = real Excel workbook writer remains open
AutoCADL2 = not_run
golden = not_started
```

## Next

```text
nextTodo = TODO-069
action = DetailWriter real-field delta P1: StbRow extended attribute skeleton
scope = small DetailWriter/test slice; no Excel writer; no hidden-line/hatch/pointStb/FaceEdge algorithm; no AutoCAD L2
```
