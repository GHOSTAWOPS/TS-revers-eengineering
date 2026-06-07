# M2-Drawing-003 Run 001

## Summary

```text
todoId = TODO-034
phase = M2-Drawing-003
decision = static-detail-complex-fields-ready
```

本轮整理旧 Detail 复杂字段静态证据，形成后续 DetailWriter 复杂字段骨架开发输入。

## Evidence

```text
E-DETAIL-003 = old Detail01.stl complex field structure
E-IDA-027 = FDrawing.arx object / method static symbols
E-DEV-056 = TODO-034 static evidence package
```

## XML Field Findings

旧样例 `autocad2020/Detail01.stl` 确认：

```text
PartDetailDrawing
  General-Info
  continue-line
  hidden-line
  central-line
  section-line
  hatch-line
  Others
  steeljoint-line
```

复杂容器：

```text
continue-line -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines
hidden-line -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines
central-line -> lines
section-line -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines
hatch-line -> lines
steeljoint-line -> joints
```

`section-line` 样例包含：

```text
LineN: start_x / start_y / end_x / end_y / ZValue
ArcN: center_x / center_y / center_z / radius / start_angle / end_angle / ZValue
```

点筋样例包含：

```text
StbGeo shapeType=C:
  point_x / point_y / point_z
  offset_x / offset_y / offset_z
  offset_x2 / offset_y2 / offset_z2

FaceEdge shapeType=L:
  start_x / start_y / end_x / end_y

FaceEdge shapeType=A:
  m_ArcDotReverse
  start_x / start_y / middle_x / middle_y / end_x / end_y
```

## IDA / FDrawing Findings

IDA MCP 新 session：

```text
fdrawing_arx_todo034
```

确认 FDrawing 插件侧符号：

```text
CViewInfo
CWSNLineDim
CWSNPointDim
CWSNSteelBarTable
CWSNMaterialTable
CWSNSectionTitle
CWSNOptionSectionSymbolDlg
CWSNArchInfo::GetHiddenLayer
CWSNArchInfo::GetSectionSymbolTextStyle
```

未在 FDrawing 字符串缓存中命中：

```text
DrawingRoot / PartDetailDrawing / General-Info
continue-line / hidden-line / section-line / hatch-line / steeljoint-line
FaceEdge / StbGroup / StbGeo / ZValue
```

因此本轮把 XML 字段名来源标为旧样例包证据，不把 FDrawing 静态符号写成 XML 字段名完全确认。

## Boundary

```text
AutoCAD L2 import = not_run
DetailWriter algorithm change = not_done
section / hidden / hatch algorithm = not_done
ZValue semantics = gap
FaceEdge generation rule = gap
golden = not_collected
```

## Verification

```text
IDA MCP idb_list
  -> fdrawing_arx_todo034 active

IDA MCP find_regex
  -> CViewInfo / CWSNLineDim / CWSNPointDim 等 FDrawing 符号可见

git diff --check
  -> pass

py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass, 0 error, 0 warning

rg TopoDS_|AIS_|BRep|TopAbs_ app/src/domain/rebar app/src/drawing app/src/project
  -> no matches

app 默认 CTest
  -> 17/17 pass
```

## Next

```text
TODO-035 / M2-Drawing-004
  -> DetailWriter 复杂字段骨架 P0
```
