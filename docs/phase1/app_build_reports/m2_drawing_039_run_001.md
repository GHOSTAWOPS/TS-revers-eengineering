# M2-Drawing-039 Run 001

todoId = TODO-070
phase = M2-Drawing-039
evidenceId = E-DEV-092
decision = detail-writer-linestb-stbgeo-field-set-narrowed

## Summary

本轮完成 `TODO-070 / DetailWriter 真实字段差异 P2：lineStb StbGeo 字段条件化骨架`。

本轮没有启动旧图石，没有安装 HASP，没有运行 AutoCAD L2，没有实现 Excel writer，没有实现真实工程图算法，也没有修改钢筋创建业务。

## Changed Code

```text
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
```

## Implemented

```text
lineStb + shapeType=L StbGeo#:
  keep segID / stbSeqNum / shapeType
  keep start_x/y/z
  keep end_x/y/z
  keep offset_x/y/z
  omit middle_x/y/z
  omit start_r / end_r
  omit length

Preserved:
  pointStb point field skeleton
  FaceEdge field skeleton
  non-line / arc segment existing fields
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
domainRebarOccLeak = false
```

## Verification

```text
detailWriterTdd = pass; new test failed before implementation with lineStb line StbGeo must not emit middle_x in TODO-070 field set and passed after implementation
detailWriterTargetBuild = pass; cmake --build app/build --target detail_writer_tests
detailWriterTests = pass; ctest -R detail_writer_tests
fullBuildDebug = pass; cmake --build app/build --config Debug
ctest = pass; 18/18 pass; total 172.89 sec
domainRebarOCCLeak = pass; rg TopoDS_/AIS_/BRep/TopAbs_ app/src/domain/rebar returned no matches
gitDiffCheck = pass
readinessGateUnit = pass; py tools/phase1_readiness_gate/test_phase1_readiness_gate.py; 29 tests OK
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
xhighReview = needs_fix_important_fixed; Critical none; Important doc/report xhigh status placeholders fixed by main flow
```

## Residual Gaps

```text
GAP-DRAW-002 = lineStb line fields narrowed, but StbGroup multi-Std, arc/complex field conditions, old plugin import and real drawing algorithms remain open
GAP-DRAW-007 = StbRow formulas and mirror semantics remain open
GAP-DRAW-008 = real Excel workbook writer remains open
AutoCADL2 = not_run
golden = not_started
```

## Next

```text
nextTodo = TODO-071
action = Rebar.Create.LineGroup old-logic evidence and P0 slice preparation
scope = use IDA / old VisualTS evidence first; no OCCT-direct rebar rewrite; no parent rebar migration
```
