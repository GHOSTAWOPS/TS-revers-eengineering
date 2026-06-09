# M2-Drawing-036 Run 001

todoId = TODO-067
phase = M2-Drawing-036
evidenceId = E-DETAIL-004, E-DEV-089
decision = real-detail-and-schedule-field-gap-report-complete

## Summary

本轮完成 `TODO-067 / 旧图石真实 Detail 与下料表字段对照 P0`。

本轮没有启动旧图石，没有安装 HASP，没有修改 app 业务代码，没有实现真实工程图算法，也没有运行 AutoCAD L2。

输入证据：

```text
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
  Detail.xml
  Detail01.stl
  Detail02.stl
  Detail03.stl
  Detail04.stl
  下料表.xls
```

对照对象：

```text
app/src/drawing/detail/DetailWriter.cpp
app/src/drawing/detail/DetailWriter.h
app/src/domain/rebar/RebarScheduleService.cpp
app/src/domain/rebar/RebarScheduleService.h
```

## Field Gap Findings

```text
Detail.xml:
  oldRuntime = <StyleRoot/>
  currentWriter = non-empty StyleRoot / Styles / Style1
  gap = GAP-DRAW-006

DetailNN table distribution:
  oldRuntime = Detail01 has StbTable + MaterialTable; Detail02..04 have no StbTable/MaterialTable
  currentWriter = every DetailNN writes StbTable + MaterialTable
  gap = GAP-DRAW-007

StbTable attributes:
  oldRuntime = count + HeightValue0 + HeightValueCount + Volume1225 +
               NumCombineGoJians + SteelNetArea + GJTAOTNumber +
               GJTAOTVolue + LinkTop + LinkDown + DCGQSJ + HYLJJ
  currentWriter = count only
  gap = GAP-DRAW-007

StbRow attributes:
  oldRuntime extra = smallTable + mirrorType + mirrorSEFlag
  currentWriter = missing these 3 fields

StbGroup/Std:
  oldRuntime = multiple Std# per StbGroup can appear
  currentWriter = Std1 only

lineStb StbGeo:
  oldRuntime = line StbGeo has start/end/offset fields
  currentWriter = non-point StbGeo also writes middle/start_r/end_r

General-Info:
  field skeleton = mostly present
  value gaps = CompanyName, DrawingUnit, DrawingScale, GeneralScale,
               PartDetailDrawing.num, DimensionChicun defaults,
               BasePoint/Range/CutPlaneDir/TopDir/CutPlanePos

Excel workbook:
  oldRuntime = 3 sheets: 钢筋表 / 钢筋汇总表 / 钢筋下料单
  currentApp = no Excel writer; RebarScheduleService only produces in-memory schedule
  gap = GAP-DRAW-008
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
appBusinessCodeModified = false
algorithmImplemented = false
autocadL2 = not_run
golden = not_started
parentDetailXmlMixedIntoSameRun = false
xhighReview = required_after_gate_mapping_change
```

## Verification

```text
fieldExtraction = pass; Detail01..04 normalized XML schema inspected
excelExtraction = pass; Excel COM read-only open; 3 sheet headers inspected
detailWriterComparison = pass; DetailWriter.cpp/h inspected
scheduleServiceComparison = pass; RebarScheduleService.cpp/h inspected
ctest = pass; 18/18 tests passed; total real time 133.26s
readinessGateStrict = pass; M1-Formal-Ready; 84/84 checks passed
domainRebarOCCLeak = pass; rg TopoDS_/AIS_/BRep/TopAbs_ app/src/domain/rebar returned no matches
gitDiffCheck = pass; git diff --check returned clean
```

## Residual Gaps

```text
GAP-DRAW-006 = Detail.xml empty StyleRoot strategy and old plugin existence requirement
GAP-DRAW-007 = main-sheet-only StbTable/MaterialTable strategy and StbTable extended attributes
GAP-DRAW-008 = real Excel workbook writer, unit conversion, display format and joint/welding columns
AutoCADL2 = not_run
golden = not_started
```

## Next

```text
nextTodo = TODO-068
action = DetailWriter real-field delta P0: empty Detail.xml and main-sheet table strategy
scope = implementation slice; no hidden-line/hatch/pointStb/FaceEdge algorithm; no Excel writer; no AutoCAD L2
```
